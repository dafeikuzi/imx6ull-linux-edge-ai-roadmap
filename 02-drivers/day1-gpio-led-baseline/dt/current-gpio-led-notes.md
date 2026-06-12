# 当前 GPIO/LED 记录

日期：2026-06-11

## 1. 运行环境

```text
开发板: 100ask IMX6ULL_Pro
Kernel: Linux 4.9.88
访问方式: ssh root@192.168.77.200
当前启动参数: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
Day 1 当前启动方式: SD/eMMC RootFS，不是 NFS RootFS
```

## 2. LED class 观察

当前系统只暴露了 MMC 触发相关 LED class：

```text
/sys/class/leds/mmc0::
/sys/class/leds/mmc1::
```

这两个节点绑定的是 USDHC/MMC 活动触发器，不是清晰命名的用户 LED 节点。因此 Day 2 不使用它们作为自定义 LED 字符设备驱动目标。

## 3. GPIO debugfs 基线

开发板当前暴露的 GPIO chip 范围如下：

```text
gpiochip0: GPIOs 0-31    -> GPIO1
gpiochip1: GPIOs 32-63   -> GPIO2
gpiochip2: GPIOs 64-95   -> GPIO3
gpiochip3: GPIOs 96-127  -> GPIO4
gpiochip4: GPIOs 128-159 -> GPIO5
gpiochip5: GPIOs 504-511 -> 74hc595 扩展 GPIO
```

Day 1 初始发现已通过 sysfs 导出的 GPIO：

```text
gpio133: direction=out, value=1, active_low=0, edge=none
```

全局 GPIO 133 的映射关系：

```text
gpiochip4 base 128 + offset 5 = GPIO5_IO05
```

后续检查本地 100ask LED 驱动示例和板级 DTS 后确认：`GPIO5_IO05 / gpio133` 不是本阶段要使用的 LED 引脚。

## 4. Day 2 LED 引脚锁定

本地 100ask LED 驱动资料一致指向：

```text
LED 引脚: GPIO5_IO03
全局 GPIO: gpiochip4 base 128 + offset 3 = gpio131
设备树 pinctrl: MX6ULL_PAD_SNVS_TAMPER3__GPIO5_IO03
电平极性: 低电平点亮，100ask 示例驱动中输出 0 表示 on
```

本地证据：

```text
drv/02_led/02_led_drv_imx6ull/board_100ask_imx6ull.c:
  .pin = GROUP_PIN(5, 3), /* GPIO5_IO03 */

drv/02_led/06_led_drv_final/100ask_led.dts:
  pin = <GROUP_PIN(5, 3)>; /* GPIO5_IO03 */

drv/02_led/06_led_drv_final/chip_demo_gpio.c:
  val &= ~GPIO5_IO03_BIT; /* Active-low LED: output 0 turns it on. */

drv/05_gpio_irq/01_simple/device_tree/原始100ask_imx6ull-14x14.dts:
  pinctrl_leds: ledgrp {
      MX6ULL_PAD_SNVS_TAMPER3__GPIO5_IO03 0x000110A0
  };
```

最终结论：

```text
Day 2 LED 驱动使用 GPIO5_IO03 / gpio131。
不要使用 GPIO5_IO05 / gpio133 作为 LED 引脚，除非后续原理图证明当前板卡版本另有差异。
```

