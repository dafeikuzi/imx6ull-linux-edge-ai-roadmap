# 02 Drivers Core Sprint

目标：在 IMX6ULL_Pro 上完成核心驱动闭环。阶段 2 聚焦 GPIO/LED、按键中断、设备树、UART、I2C 和调试证据，不把所有外设驱动塞进一个阶段。

## 0. 阶段产物目录规范

这是阶段 2 的硬性规则：

```text
每一个小阶段必须有自己的目录。
源码、Makefile、设备树片段、日志、截图、测试输出必须放在对应目录。
禁止把实验产物直接放在 02-drivers/ 根目录。
```

每个实验目录结构：

```text
README.md
src/
dt/
logs/
evidence/
```

源码约束：

```text
每一个小阶段的 src/ 目录必须保留至少一个与本阶段相关的 .c 源码文件。
如果当天不写内核驱动，也要提供用户态验证工具、采集工具或最小示例 C 程序。
Makefile 必须和源码放在同一个 src/ 目录。
不要只保留 shell 脚本、日志或说明文档。
```

## 1. 前置环境

来自阶段 1 的已知条件：

```text
Board: 100ask IMX6ULL_Pro
Kernel: Linux 4.9.88
DTB: /boot/100ask_imx6ull-14x14.dtb
NFS RootFS: 192.168.77.132:/nfs/imx6ull-rootfs
Board IP: 192.168.77.200
Ubuntu IP: 192.168.77.132
Toolchain: arm-linux-gnueabihf-
```

Ubuntu 侧准备：

```bash
sudo apt install -y build-essential make gcc file device-tree-compiler
which arm-linux-gnueabihf-gcc
```

Kernel 模块编译需要 BSP Kernel 源码，并且源码配置要匹配当前运行内核。模板命令：

```bash
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export KDIR=/path/to/100ask/linux-4.9.88
make -C "$KDIR" M="$PWD" modules
```

## Day 1: GPIO/LED baseline

目录：`day1-gpio-led-baseline/`

目标：

- 找到板载 LED 或可用 GPIO。
- 先用系统已有接口验证 LED/GPIO 能被控制。
- 记录现有设备树和 sysfs 状态。

命令：

```bash
ls /sys/class/leds 2>/dev/null || true
find /sys/class/gpio -maxdepth 2 -type f 2>/dev/null | head
dmesg | grep -Ei "gpio|led|pinctrl" | tail -50
```

证据：

- GPIO/LED 发现过程写入 `day1-gpio-led-baseline/README.md`。
- 命令输出保存到 `logs/`。
- 截图或最终结论保存到 `evidence/`。
- 用户态 GPIO 验证源码保存到 `src/gpio_sysfs_toggle.c`，并提供 `src/Makefile`。

## Day 2: LED platform driver

目录：`day2-led-platform-driver/`

目标：

- 编写 `platform_driver`。
- 通过设备树 `compatible = "demo,imx6ull-led"` 匹配。
- 使用 `devm_gpiod_get()` 获取 LED GPIO。
- 注册 `/dev/imx6ull_led`。

用户态接口：

```text
echo 1 > /dev/imx6ull_led
echo 0 > /dev/imx6ull_led
```

验收：

```bash
insmod imx6ull_led.ko
ls -l /dev/imx6ull_led
echo 1 > /dev/imx6ull_led
echo 0 > /dev/imx6ull_led
dmesg | tail -50
rmmod imx6ull_led
```

## Day 3: Device Tree notes

目录：`day3-device-tree-notes/`

目标：

- 整理 LED/KEY 设备树节点。
- 记录 pinctrl、GPIO 属性和 DTB 替换流程。
- 学会回滚 DTB，避免板子起不来。

备份命令：

```bash
cp /boot/100ask_imx6ull-14x14.dtb /boot/100ask_imx6ull-14x14.dtb.bak-stage2
sync
```

替换前必须确认：

```bash
ls -l /boot/100ask_imx6ull-14x14.dtb*
cat /proc/cmdline
```

回滚原则：

```text
如果替换 DTB 后启动异常，用原 SD/eMMC RootFS 或读卡器恢复 .bak-stage2。
第一次测试不要同时修改 U-Boot 环境变量和 DTB。
```

## Day 4: KEY IRQ driver

目录：`day4-key-irq-driver/`

目标：

- 用设备树描述按键 GPIO。
- 驱动中获取 GPIO 并转换为 IRQ。
- 中断中记录事件并唤醒等待队列。
- 用户态 `read()` 阻塞等待事件。

验收：

```bash
insmod imx6ull_key.ko
cat /dev/imx6ull_key
# 按下/松开按键后应输出 pressed 或 released
dmesg | tail -50
rmmod imx6ull_key
```

## Day 5: KEY poll driver

目录：`day5-key-poll-driver/`

目标：

- 在 Day 4 基础上增加 `.poll`。
- 用户态程序用 `poll()` 等待按键事件。
- 为后续多 fd 采集程序打基础。

验收：

```bash
insmod imx6ull_key_poll.ko
./key_poll_test /dev/imx6ull_key
dmesg | tail -50
rmmod imx6ull_key_poll
```

## Day 6: UART test

目录：`day6-uart-test/`

目标：

- 不重写 UART 内核驱动。
- 使用用户态 `termios` 打开 `/dev/ttymxc*`。
- 完成波特率、数据位、停止位、校验位配置。

验收：

```bash
ls -l /dev/ttymxc*
./uart_test /dev/ttymxc1 115200
```

注意：`/dev/ttymxc0` 通常是控制台串口，不建议直接用于普通收发实验。

## Day 7: I2C test

目录：`day7-i2c-test/`

目标：

- 使用 `i2c-tools` 检测 I2C bus 和设备地址。
- 使用用户态程序读取寄存器。
- 理解 I2C client driver 的结构，但不强制实现完整内核驱动。

验收：

```bash
ls -l /dev/i2c-*
i2cdetect -y 0
./i2c_read /dev/i2c-0 0x50 0x00
```

设备地址 `0x50` 只是 EEPROM 示例，实际地址以模块为准。

## Day 8: Review and interview

目录：`day8-review-interview/`

目标：

- 整理阶段 2 证据。
- 写出简历项目描述。
- 准备设备树、字符设备、中断、poll、UART、I2C 的面试问答。

阶段 2 完成标准：

- [ ] LED 用户态可控。
- [ ] KEY 阻塞读可用。
- [ ] KEY poll/select 可用。
- [ ] 设备树修改和回滚流程记录清楚。
- [ ] UART 用户态收发测试完成。
- [ ] I2C 设备检测或读取完成。
- [ ] 每个实验目录都有 README、logs、evidence。
