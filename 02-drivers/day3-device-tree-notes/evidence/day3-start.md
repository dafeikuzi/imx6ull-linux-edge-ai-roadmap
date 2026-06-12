# Day 3 启动记录：Device Tree 专题

日期：2026-06-11

## 1. Day2 交接状态

Day2 已完成 LED platform driver 闭环：

```text
主 DTB 已替换为 Day2 minimal DTB。
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led。
imx6ull_led.ko 可 probe。
/dev/imx6ull_led 已创建。
led_test on/off/on 已通过。
```

当前板子状态：

```text
Board IP: 192.168.77.200
RootFS: /dev/mmcblk1p2
Kernel: Linux 4.9.88
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
eth0: 192.168.77.200/24
```

回滚备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

## 2. Day3 目标

Day3 聚焦设备树本身：

```text
compatible 匹配机制
gpios 属性与 gpiod_get 命名关系
pinctrl 与 pinmux
phandle 数字含义
DTB 反编译与 diff
主 DTB 替换和回滚 SOP
KEY 节点预研
```

## 3. Day3 不做什么

Day3 暂不写 KEY 驱动，KEY 驱动放到 Day4。

Day3 不再盲目从 Kernel 源码 DTS 直接整板编译主 DTB。

Day3 不把实验产物放在 `02-drivers/` 根目录。

## 4. 今日产物

计划产物：

```text
evidence/current-dtb-state.md
evidence/device-tree-knowledge-card.md
evidence/key-node-precheck.md
evidence/day3-summary.md
logs/current-dtb-runtime.log
logs/current-main-dtb-diff.log
dt/current-main.dts
dt/day3-key-node-notes.dts
```
