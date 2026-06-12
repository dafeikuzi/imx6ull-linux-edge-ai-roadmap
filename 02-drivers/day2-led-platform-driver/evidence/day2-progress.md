# Day 2 进展：LED platform driver

日期：2026-06-11

## 1. Day 1 结论接入

Day 1 已锁定 LED 引脚：

```text
GPIO: GPIO5_IO03
sysfs: gpio131
设备树: led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>
电平: 低电平点亮
```

Day 2 的设备树模板已使用该引脚：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>;
    status = "okay";
};
```

## 2. 已完成

- 已将 Day 2 源码上传到 Ubuntu：

```text
/home/liuzhiwei/stage2-day2-led/src/
```

- 已用 100ask Linaro 工具链编译用户态测试程序：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 led_test.c -o led_test
```

- `file led_test` 已确认是 ARM 32-bit ELF。
- 已将 `led_test` 部署到板端：

```text
/root/stage2/day2-led/led_test
```

- 板端执行 `led_test /dev/imx6ull_led 1` 返回：

```text
open: No such file or directory
```

该结果符合预期，因为内核模块还没有编译和加载，`/dev/imx6ull_led` 尚不存在。

## 3. DTB 风险与恢复结果

`imx6ull_led.ko` 已编译成功。首次测试时曾替换主 DTB：

已完成：

```text
Kernel 源码: D:\user\browser\imx-linux4.9.88-master.zip
Ubuntu 解压路径: /home/liuzhiwei/kernel-src/imx-linux4.9.88-master
defconfig: 100ask_imx6ull_defconfig
模块: imx6ull_led.ko
vermagic: 4.9.88 SMP preempt mod_unload modversions ARMv7 p2v8
```

已编译测试 DTB：

```text
100ask_imx6ull-14x14-day2.dtb
```

DTB 修改：

```text
禁用原 gpio-leds 节点。
新增 imx6ull_led 节点，compatible = "demo,imx6ull-led"。
led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>。
```

替换后现象：

```text
已把新 DTB 替换为 /boot/100ask_imx6ull-14x14.dtb。
原 DTB 备份为 /boot/100ask_imx6ull-14x14.dtb.bak-。
执行 reboot 后，板端网络 192.168.77.200 未恢复。
Ubuntu /dev/ttyACM0 存在，已读到 U-Boot 和 Kernel 启动片段。
系统进入用户态后卡在 udev 相关流程，未出现 login/shell，网络也未恢复。
```

已通过 U-Boot + `init=/bin/sh` 恢复：

```text
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw init=/bin/sh'
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /boot/100ask_imx6ull-14x14.dtb.bak-
bootz 0x80800000 - 0x83000000
```

进入最小 shell 后执行：

```bash
mount -o remount,rw /
cp /boot/100ask_imx6ull-14x14.dtb.bak- /boot/100ask_imx6ull-14x14.dtb
sync
reboot -f
```

恢复结果：

```text
/boot/100ask_imx6ull-14x14.dtb 恢复为 38686 bytes。
板子正常启动。
SSH 和 eth0=192.168.77.200/24 已恢复。
```

## 4. 下一步

不再直接覆盖主 DTB。先在正常系统下确认资源占用：

```bash
ls -l /sys/class/leds
find /proc/device-tree -maxdepth 3 -iname '*led*'
mount -t debugfs debugfs /sys/kernel/debug 2>/dev/null || true
cat /sys/kernel/debug/gpio | grep -E 'gpio-131|GPIO5|led'
```

已确认当前正常启动下：

```text
/sys/class/leds 只有 mmc0:: 和 mmc1::。
未看到明显的用户 LED 设备树节点暴露。
debugfs 暂未显示 gpio131 被已知 LED 驱动占用。
```

后续 Day 2 建议走更保守路径：

```text
1. 保留 platform driver + Device Tree 作为标准写法。
2. 先增加一个临时 fallback/self-test 版本，用 gpio131 验证字符设备和 LED 控制闭环。
3. 等闭环验证完成后，再单独进入 Day 3 设备树专题处理 DTB 替换、回滚和差异定位。
```

如果之后确认新 DTB 能安全启动，再执行完整 platform driver 验证：

```bash
cd /root/stage2/day2-led
insmod imx6ull_led.ko
ls -l /dev/imx6ull_led
./led_test /dev/imx6ull_led 1
./led_test /dev/imx6ull_led 0
dmesg | tail -50
```

## 5. 已保存日志

```text
logs/ubuntu-env-check.log
logs/led-test-build.log
logs/kernel-module-build-blocker.log
logs/module-build-success.log
logs/dtb-build-and-deploy.log
logs/serial-recovery-attempt.log
logs/dtb-recovery-success.log
logs/board-led-test-without-driver.log
```

## 6. DTB 差异检查

已将原始 DTB 和 Day2 测试 DTB 反编译并保存到：

```text
evidence/dtb-diff/
```

关键结论：

```text
original-restored.dtb  38686 bytes
day2-test.dtb          37825 bytes
diff patch             1023 lines
```

Day2 测试 DTB 和原始 DTB 不是只差一个 `imx6ull_led` 节点。
实际差异还包括：

```text
gpio_keys_100ask 节点丢失/被替换。
gpio-keys 从 disabled 变成默认启用。
ecspi@02008000 从 okay 变成 disabled。
adc@02198000 从 okay 变成 disabled。
uart6 从 okay 变成 disabled。
backlight PWM/亮度参数变化。
若干 pinctrl/GPIO 配置差异。
```

因此本次问题更像是“DTS 基线不一致导致整板 DTB 被整体改变”，
不是单纯 LED 节点导致系统异常。

详细记录见：

```text
evidence/dtb-diff/README.md
evidence/dtb-diff/diff-original-vs-day2.patch
```

## 7. 按安全流程重新推进并完成

已按以下原则重新推进：

```text
不要再直接覆盖主 DTB。
先从板子原始 DTB 反编译出 DTS。
只在原始 DTS 上做最小 LED 补丁。
先用 U-Boot 临时加载测试 DTB。
确认启动、SSH、dmesg 都正常后，才考虑替换 /boot 主 DTB。
```

执行结果：

```text
original-restored.dtb                    38686 bytes
original-plus-imx6ull-led-minimal.dtb    38828 bytes
diff-original-vs-minimal.patch           17 lines
```

U-Boot 临时加载：

```text
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /root/stage2/day2-led/100ask_imx6ull-14x14-day2-minimal.dtb
bootz 0x80800000 - 0x83000000
```

板端验证：

```text
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
/dev/imx6ull_led 已创建
led_test /dev/imx6ull_led 1 -> led on
led_test /dev/imx6ull_led 0 -> led off
led_test /dev/imx6ull_led 1 -> led on
```

主 DTB 状态：

```text
/boot/100ask_imx6ull-14x14.dtb 保持原始 38686 bytes，未覆盖。
```

完成记录见：

```text
evidence/day2-completion.md
logs/serial-boot-minimal-dtb.log
logs/day2-led-minimal-dtb-test.log
```
