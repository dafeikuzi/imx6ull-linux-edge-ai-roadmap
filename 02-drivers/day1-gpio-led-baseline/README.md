# Day 1 GPIO/LED Baseline

目标：不急着写驱动，先确认当前系统中 LED/GPIO 的真实状态。后续所有驱动实验都要建立在“知道硬件资源在哪里”的基础上。

## 今日产物目录

```text
src/       # gpio_sysfs_toggle.c、Makefile；Day1 用户态 GPIO 验证源码
dt/        # 保存现有设备树节点摘录
logs/      # 保存 sysfs、dmesg、gpio 查询输出
evidence/  # 保存截图和最终结论
```

不要把日志直接放到 `02-drivers/` 根目录。

## 板端检查命令

```bash
uname -a
cat /proc/cmdline
ls /sys/class/leds 2>/dev/null || true
ls /sys/class/gpio 2>/dev/null || true
dmesg | grep -Ei "gpio|led|pinctrl" | tail -80
```

如果系统有 LED class：

```bash
for led in /sys/class/leds/*; do
    echo "$led"
    cat "$led/trigger" 2>/dev/null || true
    cat "$led/brightness" 2>/dev/null || true
done
```

如果系统开放 sysfs GPIO，先确认 GPIO 编号再测试，不要盲目 export：

```bash
cat /sys/kernel/debug/gpio 2>/dev/null || true
```

## Day1 C 源码工具

Day1 也必须保留 `.c` 源码产物。本日源码是用户态 GPIO sysfs 测试工具：

```text
src/gpio_sysfs_toggle.c
src/Makefile
```

交叉编译：

```bash
cd src
make CC=arm-linux-gnueabihf-gcc
file gpio_sysfs_toggle
```

板端示例：

```bash
./gpio_sysfs_toggle 133 0 1 500
./gpio_sysfs_toggle 131 0 1 500
```

说明：

```text
gpio133 = GPIO5_IO05，Day1 曾验证 sysfs 可写。
gpio131 = GPIO5_IO03，最终锁定为 Day2 LED 引脚。
如果 gpio131 已被 imx6ull_led 驱动占用，sysfs export 可能失败，这是正常资源占用现象。
```

## 记录模板

```text
Kernel:
DTB:
已有 LED class:
已有 GPIO debugfs:
候选 LED GPIO:
候选 KEY GPIO:
是否能用现有接口控制 LED: 是/否
问题记录:
结论:
```

## 今日结论要求

最后在 `evidence/day1-result.md` 中写清楚：

- 当前系统是否已有 LED class。
- 哪个 LED/GPIO 最适合 Day 2 驱动实验。
- 是否需要修改设备树。
- Day1 的 C 工具是否能交叉编译并在板端运行。
