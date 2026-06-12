# Day 1 工作总结：GPIO/LED 基线

日期：2026-06-11

## 已完成工作

- 通过网络确认开发板 `192.168.77.200` 可达，并可使用 SSH 执行板端命令。
- 读取了板端 Kernel 和启动参数：

```text
Kernel: Linux 100ask 4.9.88
bootargs: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

- 检查了 `/sys/class/leds`，发现当前只有：

```text
mmc0::
mmc1::
```

这两个是 MMC 活动触发 LED，不作为 Day 2 自定义 LED 驱动目标。

- 检查了 `/sys/kernel/debug/gpio`，确认 GPIO5 的全局编号范围：

```text
gpiochip4: GPIOs 128-159 -> GPIO5
```

- 初始发现 `gpio133 / GPIO5_IO05` 已导出并可切换，但后续通过 100ask 本地 LED 示例源码确认它不是本阶段锁定的 LED 引脚。
- 锁定 Day 2 使用的 LED 引脚：

```text
LED 引脚: GPIO5_IO03
全局 GPIO: gpio131
设备树写法: <&gpio5 3 GPIO_ACTIVE_LOW>
Pinmux: MX6ULL_PAD_SNVS_TAMPER3__GPIO5_IO03
电平: 低电平点亮
```

## 最终结论

Day 2 LED platform driver 应使用 `GPIO5_IO03 / gpio131`，设备树属性写为：

```dts
led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>;
```

不要使用 `GPIO5_IO05 / gpio133` 作为 Day 2 LED 引脚。

## 保存的证据

```text
logs/board-gpio-led-baseline.log
logs/gpio133-toggle.log
logs/gpio131-lock-check.log
logs/gpio-sysfs-toggle-c-test.log
dt/current-gpio-led-notes.md
evidence/day1-result.md
evidence/day1-c-source-test.md
src/collect_day1_gpio_led.sh
src/gpio_sysfs_toggle.c
src/Makefile
```

## C 源码补充

Day1 已补充用户态 C 验证工具：

```text
src/gpio_sysfs_toggle.c
src/Makefile
```

该工具已交叉编译为 ARM 32-bit ELF，并在板端使用 `gpio133` 完成写入读回测试：

```text
after value0: 0
after value1: 1
EXIT:0
```
