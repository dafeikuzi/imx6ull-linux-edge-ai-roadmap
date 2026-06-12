# Day 2 DTB 差异检查

日期：2026-06-11

## 文件

```text
original-restored.dtb  原始/已恢复主 DTB，来自 /boot/100ask_imx6ull-14x14.dtb
day2-test.dtb         Day2 测试 DTB，来自 /root/stage2/day2-led/100ask_imx6ull-14x14-day2.dtb
original-restored.dts 原始 DTB 反编译结果
day2-test.dts         Day2 DTB 反编译结果
diff-original-vs-day2.patch  两者反编译后的 diff
```

## 大小对比

```text
original-restored.dtb  38686 bytes
day2-test.dtb          37825 bytes

original-restored.dts  49418 bytes
day2-test.dts          48172 bytes
diff patch             1023 lines
```

结论：Day2 测试 DTB 和板子原始 DTB 不是“只差一个 LED 节点”。

## 重要差异

### 1. 新增了 Day2 LED 节点

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <...>;
    led-gpios = <... GPIO5_IO03 ... GPIO_ACTIVE_LOW>;
    status = "okay";
};
```

这部分是 Day2 预期修改。

### 2. 原始 100ask key 节点被替换/丢失

原始 DTB 中有：

```dts
gpio_keys_100ask {
    compatible = "100ask,gpio_key";
    gpios = <...>;
    pinctrl-names = "default";
    pinctrl-0 = <...>;
};
```

Day2 DTB 中该节点不再存在，对应位置变成了 `imx6ull_led`。

### 3. generic gpio-keys 状态发生变化

原始 DTB：

```dts
gpio-keys {
    compatible = "gpio-keys";
    pinctrl-names = "default";
    status = "disabled";
    user1 { ... };
    user2 { ... };
};
```

Day2 DTB：

```dts
gpio-keys {
    compatible = "gpio-keys";
    pinctrl-names = "default";
    user { ... };
};
```

Day2 DTB 没有 `status = "disabled"`，设备树默认等价于启用。

### 4. 多个外设状态/配置变化

真实语义变化包括：

```text
ecspi@02008000: status okay -> disabled
adc@02198000: status okay -> disabled
serial@021fc000 / uart6: status okay -> disabled
backlight: PWM 周期、亮度表、默认亮度变化
spi4: GPIO 引用和 reset 相关 phandle 变化
```

其中大量 `phandle = <...>` 数字变化只是编译器重新编号，不一定代表硬件语义变化；
但 `status`、节点是否存在、GPIO/pinctrl 内容变化属于真实风险。

## 为什么主 DTB 修改影响这么大

DTB 是 Kernel 启动时的“整板硬件描述”，不是某一个驱动的局部配置。

Kernel 会根据 DTB：

```text
1. 创建 platform_device / i2c_client / spi_device 等设备。
2. 给驱动分配 GPIO、IRQ、clock、regulator、pinctrl。
3. 决定哪些外设启用，哪些外设禁用。
4. 影响 /sys、/dev/input、/dev/spidev、网卡、触摸屏、背光等用户态设备事件。
5. 触发 udev/mdev/libinput 等用户态规则处理。
```

所以主 DTB 一旦不是原始 DTB 的最小补丁版本，而是从另一个 DTS 基线重新编译出来，
就可能同时改变多个设备。Day2 的现象中，串口日志卡在 `udevd` 相关流程，
和 DTB 中 input/touch/key/SPI/网卡等设备事件变化是吻合的。

## 根因判断

本次问题的核心不是 `imx6ull_led` 这个节点本身一定有错，
而是 Day2 使用的 Kernel 源码 DTS 基线与板子当前 `/boot` 里的出厂 DTB 不完全一致。

也就是说：

```text
错误路径：
从源码包里的 DTS 重新编译整板 DTB -> 覆盖原始主 DTB

风险：
把 LED 以外的很多板级差异也一起带上了
```

更安全路径：

```text
从板子当前原始 DTB 反编译得到 DTS
只在这个 DTS 上追加最小 LED 节点
重新编译成测试 DTB
先用 U-Boot 临时加载测试 DTB
确认启动正常后再考虑替换 /boot 主 DTB
```

## 后续策略

Day2 不再直接覆盖主 DTB。

建议：

```text
1. 先用 gpio131 fallback/self-test 方式验证 LED 字符设备闭环。
2. Day3 设备树专题中，从 original-restored.dtb 反编译出来做最小补丁。
3. 测试 DTB 只通过 U-Boot 临时 ext4load 加载，不覆盖 /boot 主 DTB。
4. 启动成功、SSH 成功、dmesg 无异常后，才允许进入替换主 DTB 步骤。
```
