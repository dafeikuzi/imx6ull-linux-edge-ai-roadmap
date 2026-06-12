# Day 3 KEY 节点预研

日期：2026-06-11

## 1. 当前 key 相关节点

当前主 DTB 中有两个 key 相关节点：

```dts
gpio_keys_100ask {
    compatible = "100ask,gpio_key";
    gpios = <0x15 0x1 0x1 0x9 0xe 0x1>;
    pinctrl-names = "default";
    pinctrl-0 = <0x3a 0x3b>;
};
```

```dts
gpio-keys {
    compatible = "gpio-keys";
    pinctrl-names = "default";
    status = "disabled";

    user1 {
        label = "User1 Button";
        gpios = <0x15 0x1 0x1>;
        gpio-key,wakeup;
        linux,code = <0x2>;
    };

    user2 {
        label = "User2 Button";
        gpios = <0x9 0xe 0x1>;
        gpio-key,wakeup;
        linux,code = <0x3>;
    };
};
```

## 2. GPIO 初步解释

`gpio_keys_100ask`：

```text
<0x15 0x1 0x1> = GPIO5_IO01, active-low
<0x9  0xe 0x1> = 另一个 GPIO 控制器 phandle 下的 IO14, active-low
```

`gpio-keys`：

```text
user1 = GPIO5_IO01, active-low
user2 = 另一个 GPIO 控制器 phandle 下的 IO14, active-low
status = disabled
```

## 3. Day4 风险

Day4 要做自定义 KEY IRQ driver，需要注意：

```text
gpio_keys_100ask 节点当前是启用状态。
generic gpio-keys 节点当前是 disabled。
如果自定义驱动直接抢同一个 GPIO，可能和 100ask 原有 key 驱动冲突。
```

Day4 需要先确认：

```bash
ls -l /dev | grep -i key
cat /proc/interrupts | grep -Ei 'gpio|key'
dmesg | grep -Ei 'gpio_key|gpio-key|key|input' | tail -80
cat /sys/kernel/debug/gpio | grep -E 'gpio-129|gpio-.*key|GPIO5'
```

其中：

```text
GPIO5_IO01 = global gpio129
```

## 4. Day4 建议路线

建议不要一开始直接改主 DTB 去启用 `gpio-keys`。

更稳路线：

```text
1. 先确认 100ask 原有 key 驱动是否已经占用 GPIO。
2. 如果占用，Day4 自定义 KEY 驱动应选择未占用 GPIO，或先做临时 DTB 禁用原节点。
3. 若需要修改 DTB，必须沿用 Day2/Day3 SOP：从当前主 DTB 反编译，最小补丁，U-Boot 临时启动，确认后再替换。
```

## 5. 当前结论

KEY 设备树需要谨慎处理。

Day4 的重点不是“随便写一个中断驱动”，而是：

```text
先查资源占用。
再决定使用哪个 GPIO。
最后通过设备树最小补丁绑定自定义 key IRQ driver。
```

## 6. 运行时补充检查

日志：

```text
logs/key-runtime-precheck.log
```

当前 `/dev/input`：

```text
event0 = 20cc000.snvs:snvs-powerkey
event1 = goodix-ts
```

当前没有看到 `gpio_keys_100ask` 暴露为标准 input event 设备。

`/proc/interrupts` 相关项：

```text
40: GPC 4 Level 20cc000.snvs:snvs-powerkey
80: gpio-mxc 1 Edge inv_mpu
84: gpio-mxc 5 Edge gt9xx
98: gpio-mxc 19 Edge 2190000.usdhc cd
```

debugfs GPIO 片段：

```text
gpio-131 | led   | out hi
gpio-133 | sysfs | out hi
```

说明：

```text
gpio131 当前已被 Day2 LED 驱动占用。
gpio133 是 Day1 sysfs 测试遗留导出 GPIO。
日志中未直接显示 gpio129/gpio78 已被 key/input 驱动占用。
```

## 7. Day4 初步建议

Day4 KEY IRQ driver 可以优先围绕 `GPIO5_IO01/gpio129` 继续确认。

但在真正写驱动和改 DTB 前，需要做两步：

```bash
cat /sys/kernel/debug/gpio | grep -E 'gpio-129|gpio-78|gpio-131|gpio-133'
cat /proc/interrupts | grep -Ei 'gpio|key'
```

如果 `gpio129` 未被占用：

```text
可基于当前主 DTB 做最小补丁，新增 imx6ull_key 节点。
```

如果 `gpio129` 已被 100ask 原驱动占用：

```text
先不要硬抢。
应通过临时 DTB 禁用 gpio_keys_100ask，或选择另一个未占用 GPIO。
```

Day4 仍然必须走 Day2/Day3 的 DTB SOP：

```text
当前主 DTB -> 反编译 -> 最小补丁 -> diff -> U-Boot 临时启动 -> 验证 -> 再替换主 DTB
```
