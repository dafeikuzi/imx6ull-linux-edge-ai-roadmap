# Day 3 总结：Device Tree 专题

日期：2026-06-11

## 1. Day3 目标

Day3 的目标不是继续盲目新增外设，而是把 Day2 中真实发生过的 DTB 问题整理成可复用的方法：

```text
怎么看设备树
怎么解释节点属性
怎么判断 DTB diff 是否安全
怎么临时启动测试 DTB
怎么替换主 DTB
怎么回滚
Day4 KEY 驱动前要检查哪些资源
```

## 2. 当前主 DTB 状态

当前板端主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38828 bytes
```

启动状态：

```text
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
model: Freescale i.MX6 ULL 14x14 EVK Board
compatible: fsl,imx6ull-14x14-evk, fsl,imx6ull
eth0: 192.168.77.200/24
```

Day2 LED 节点：

```text
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
```

备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

## 3. 已完成产物

当前 DTB 运行态：

```text
logs/current-dtb-runtime.log
logs/current-main-dtb-decompile.log
logs/current-main-dtb-diff.log
dt/current-main.dtb
dt/current-main.dts
dt/original-before-day2.dts
```

知识与复盘：

```text
evidence/current-dtb-state.md
evidence/device-tree-knowledge-card.md
evidence/dtb-diff-review.md
evidence/key-node-precheck.md
```

启动与计划：

```text
evidence/day3-start.md
evidence/day3-checklist.md
evidence/day3-progress.md
```

## 4. 设备树机制总结

Day2 LED 节点：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <0x39>;
    led-gpios = <0x15 0x3 0x1>;
    status = "okay";
};
```

关键解释：

```text
compatible: 匹配 platform_driver.of_match_table。
status: okay 表示启用。
led-gpios: 被 devm_gpiod_get(dev, "led", ...) 读取。
pinctrl-0: 指向 ledgrp，完成 GPIO5_IO03 的 pinmux。
0x15: GPIO5 控制器 phandle。
0x3: GPIO5_IO03。
0x1: GPIO_ACTIVE_LOW。
0x39: ledgrp pinctrl phandle。
```

## 5. DTB diff 复盘

bad diff：

```text
diff-original-vs-day2.patch = 1023 lines
```

危险原因：

```text
不只是新增 LED 节点。
还改变/删除了 key、spi、adc、uart6、backlight、pinctrl 等板级配置。
```

good diff：

```text
diff-original-vs-current-main.patch = 17 lines
```

安全原因：

```text
只新增 imx6ull_led 节点。
唯一 status 新增属于 imx6ull_led。
没有无关外设状态变化。
```

## 6. KEY 预研结论

当前 DTB 中有：

```text
gpio_keys_100ask: enabled
gpio-keys: disabled
```

运行时 `/dev/input` 当前主要是：

```text
event0 = snvs-powerkey
event1 = goodix-ts
```

debugfs GPIO 看到：

```text
gpio131 = Day2 LED 驱动占用
gpio133 = Day1 sysfs 测试导出
```

Day4 做 KEY IRQ 前必须先确认：

```text
GPIO5_IO01/gpio129 是否被占用
另一个 key GPIO 是否被占用
是否需要禁用 gpio_keys_100ask
是否选择未占用 GPIO 做自定义 key IRQ
```

## 7. 后续 DTB 修改规则

以后修改主 DTB 必须按这个顺序：

```text
1. 备份当前可启动 DTB。
2. 从当前可启动 DTB 反编译 DTS。
3. 只做一个功能点的最小补丁。
4. 编译独立测试 DTB。
5. diff 检查，确认没有无关变化。
6. U-Boot 临时加载测试 DTB。
7. 验证 SSH、eth0、/proc/device-tree、dmesg。
8. 全部通过后才替换主 DTB。
9. 普通重启后再次完整验证。
10. 保存日志和回滚命令。
```

## 8. Day3 结论

Day3 已完成设备树专题的核心闭环：

```text
当前主 DTB 状态记录完成。
imx6ull_led 节点解释完成。
compatible/gpios/pinctrl/phandle/status 解释完成。
bad diff vs good diff 复盘完成。
KEY 节点预研完成。
后续主 DTB 修改 SOP 明确。
```

可以进入 Day4 KEY IRQ driver。
