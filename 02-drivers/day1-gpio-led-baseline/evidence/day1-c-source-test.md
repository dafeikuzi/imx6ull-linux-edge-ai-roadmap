# Day 1 C 源码补充与测试

日期：2026-06-11

## 1. 补充原因

Day1 原本只有 shell 采集脚本和日志。根据阶段 2 的新约束：

```text
每一个小阶段的 src/ 目录必须保留至少一个与本阶段相关的 .c 源码文件。
如果当天不写内核驱动，也要提供用户态验证工具、采集工具或最小示例 C 程序。
Makefile 必须和源码放在同一个 src/ 目录。
```

因此 Day1 补充用户态 GPIO sysfs 测试工具。

## 2. 新增文件

```text
src/gpio_sysfs_toggle.c
src/Makefile
```

用途：

```text
通过 /sys/class/gpio/export 导出 GPIO。
设置 direction = out。
写入 value0。
延时。
写入 value1。
读回 value 验证。
```

## 3. 交叉编译结果

Ubuntu 编译命令：

```bash
cd /home/liuzhiwei/stage2-day1-gpio-src
export PATH=/opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin:$PATH
make clean
make CC=arm-linux-gnueabihf-gcc
file gpio_sysfs_toggle
```

结果：

```text
gpio_sysfs_toggle: ELF 32-bit LSB executable, ARM, EABI5
```

## 4. 板端运行测试

部署路径：

```text
/root/stage2/day1-gpio_sysfs_toggle
```

测试命令：

```bash
/root/stage2/day1-gpio_sysfs_toggle 133 0 1 200
echo $?
cat /sys/class/gpio/gpio133/value
```

测试结果：

```text
gpio_sysfs_toggle: gpio=133 value0=0 value1=1 delay_ms=200
after value0: 0
after value1: 1
EXIT:0
1
```

日志：

```text
logs/gpio-sysfs-toggle-c-test.log
```

## 5. 说明

本次使用 `gpio133` 做 C 工具验证，因为：

```text
gpio133 = GPIO5_IO05
Day1 曾确认该 GPIO 可以通过 sysfs 写入。
```

没有用 `gpio131` 做 sysfs 复测，因为当前 Day2 已经让 `imx6ull_led.ko` 绑定 `GPIO5_IO03/gpio131`。
再用 sysfs 抢同一个 GPIO 可能出现资源占用冲突，这正是驱动资源管理要避免的情况。

## 6. 结论

Day1 现在包含完整源码产物：

```text
C 源码: src/gpio_sysfs_toggle.c
Makefile: src/Makefile
ARM 二进制: src/gpio_sysfs_toggle
板端测试日志: logs/gpio-sysfs-toggle-c-test.log
```

Day1 可以满足“有中文文档、有源码、有日志、有证据”的阶段产物要求。
