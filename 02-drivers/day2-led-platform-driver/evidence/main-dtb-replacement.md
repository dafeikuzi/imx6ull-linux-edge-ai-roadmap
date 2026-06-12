# Day 2 主 DTB 替换验证

日期：2026-06-11

## 1. 替换前提

在替换主 DTB 前，已经先完成：

```text
1. 从原始 DTB 反编译得到 DTS。
2. 只追加 imx6ull_led 节点。
3. 编译 minimal DTB。
4. 通过 U-Boot 临时加载 minimal DTB 启动成功。
5. SSH、设备树节点、驱动 probe、/dev/imx6ull_led、led_test 均验证通过。
```

因此本次替换使用的是已验证的 minimal DTB，而不是此前有 1023 行差异的整板重编译 DTB。

## 2. 备份与替换

替换前主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38686 bytes
sha256: 28b27760efbad840072095f3ffd39295ce8a8452b36578e88fe81000a11afb18
```

minimal DTB：

```text
/root/stage2/day2-led/100ask_imx6ull-14x14-day2-minimal.dtb
size: 38828 bytes
sha256: be49fb54cd774bd1187608ea26946d975ef0d9495ce4f9f6af2f317bc6a6ecf8
```

备份文件：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

替换命令：

```bash
cd /boot
cp -f 100ask_imx6ull-14x14.dtb 100ask_imx6ull-14x14.dtb.before-day2-minimal
cp -f /root/stage2/day2-led/100ask_imx6ull-14x14-day2-minimal.dtb 100ask_imx6ull-14x14.dtb
sync
reboot
```

## 3. 普通重启验证

普通重启后，不再手工 U-Boot 加载测试 DTB。

验证结果：

```text
SSH_OK
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
```

主 DTB 已经是 minimal DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38828 bytes
sha256: be49fb54cd774bd1187608ea26946d975ef0d9495ce4f9f6af2f317bc6a6ecf8
```

网络：

```text
eth0 UP 192.168.77.200/24
```

## 4. 驱动与 LED 测试

测试命令：

```bash
cd /root/stage2/day2-led
insmod ./imx6ull_led.ko
ls -l /dev/imx6ull_led
./led_test /dev/imx6ull_led 1
./led_test /dev/imx6ull_led 0
./led_test /dev/imx6ull_led 1
dmesg | grep -E 'imx6ull_led|imx6ull-led|led on|led off'
```

结果：

```text
crw------- 1 root root 10, 56 /dev/imx6ull_led
imx6ull-led imx6ull_led: imx6ull led driver probed
imx6ull-led imx6ull_led: led on
imx6ull-led imx6ull_led: led off
imx6ull-led imx6ull_led: led on
```

日志：

```text
logs/day2-main-dtb-replacement-test.log
```

## 5. 回滚方法

如果后续启动异常，可通过串口/U-Boot 或能进入 shell 时执行：

```bash
mount -o remount,rw /
cp -f /boot/100ask_imx6ull-14x14.dtb.before-day2-minimal /boot/100ask_imx6ull-14x14.dtb
sync
reboot -f
```

也可以使用此前备份：

```bash
cp -f /boot/100ask_imx6ull-14x14.dtb.bak- /boot/100ask_imx6ull-14x14.dtb
sync
reboot -f
```

## 6. 结论

主 DTB 替换完成并验证通过：

```text
普通重启成功。
SSH 正常。
eth0 网络正常。
imx6ull_led 设备树节点自动生效。
imx6ull_led.ko probe 成功。
/dev/imx6ull_led 创建成功。
led_test 完成 on/off/on 控制。
```
