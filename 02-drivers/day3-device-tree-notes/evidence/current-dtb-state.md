# Day 3 当前主 DTB 状态

日期：2026-06-11

## 1. 当前启动状态

```text
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
model: Freescale i.MX6 ULL 14x14 EVK Board
compatible: fsl,imx6ull-14x14-evk, fsl,imx6ull
eth0: 192.168.77.200/24
```

当前主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38828 bytes
```

备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

## 2. 当前 LED 节点

运行时：

```text
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
```

`led-gpios` 十六进制：

```text
00 00 00 15 00 00 00 03 00 00 00 01
```

解释：

```text
0x15 = GPIO5 控制器 phandle
0x03 = GPIO5_IO03
0x01 = GPIO_ACTIVE_LOW
```

`pinctrl-0`：

```text
00 00 00 39
```

解释：

```text
0x39 = ledgrp pinctrl phandle
```

反编译 DTS 中的节点：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <0x39>;
    led-gpios = <0x15 0x3 0x1>;
    status = "okay";
};
```

对应 pinctrl：

```dts
ledgrp {
    fsl,pins = <0x14 0x58 0x0 0x5 0x0 0x110a0>;
    linux,phandle = <0x39>;
    phandle = <0x39>;
};
```

## 3. 当前驱动状态

Day2 驱动仍处于加载状态：

```text
imx6ull_led 2612 0
/dev/imx6ull_led 存在
```

dmesg 已确认：

```text
imx6ull-led imx6ull_led: imx6ull led driver probed
imx6ull-led imx6ull_led: led on
imx6ull-led imx6ull_led: led off
```

## 4. 已保存文件

```text
dt/current-main.dtb
dt/current-main.dts
logs/current-dtb-runtime.log
logs/current-main-dtb-decompile.log
```

## 5. 结论

当前主 DTB 已经稳定包含 Day2 的 `imx6ull_led` 最小节点。

Day3 后续可以基于这个运行状态讲解：

```text
compatible -> platform_driver 匹配
led-gpios -> devm_gpiod_get(dev, "led", ...)
pinctrl-0 -> 引脚复用配置
phandle -> 反编译后的引用编号
```
