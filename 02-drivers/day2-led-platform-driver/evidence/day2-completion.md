# Day 2 完成记录：LED platform driver 最小 DTB 闭环

日期：2026-06-11

## 1. 安全策略

本次没有再次覆盖主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb 保持原始文件，38686 bytes
```

采用的安全流程：

```text
1. 从板子原始 DTB 反编译得到 DTS。
2. 只追加 imx6ull_led 节点。
3. 编译为独立测试 DTB。
4. 测试 DTB 只放到 /root/stage2/day2-led/。
5. 通过 U-Boot 临时 ext4load 测试 DTB 启动。
6. 不覆盖 /boot 主 DTB。
```

## 2. 最小 DTB 结果

文件：

```text
evidence/dtb-diff/original-plus-imx6ull-led-minimal.dts
evidence/dtb-diff/original-plus-imx6ull-led-minimal.dtb
evidence/dtb-diff/diff-original-vs-minimal.patch
```

大小和差异：

```text
original-restored.dtb                    38686 bytes
original-plus-imx6ull-led-minimal.dtb    38828 bytes
diff-original-vs-minimal.patch           17 lines
```

新增节点：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <0x39>;
    led-gpios = <0x15 0x3 0x1>;
    status = "okay";
};
```

说明：

```text
0x15 = 原始 DTB 中 GPIO5 控制器 phandle
0x3  = GPIO5_IO03
0x1  = GPIO_ACTIVE_LOW
0x39 = 原始 DTB 中 ledgrp pinctrl
```

## 3. U-Boot 临时启动证据

串口日志：

```text
logs/serial-boot-minimal-dtb.log
```

关键命令：

```text
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /root/stage2/day2-led/100ask_imx6ull-14x14-day2-minimal.dtb
bootz 0x80800000 - 0x83000000
```

启动后 SSH 正常恢复。

板端确认：

```text
compatible: demo,imx6ull-led
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

## 4. 驱动加载与字符设备

板端执行：

```bash
cd /root/stage2/day2-led
insmod ./imx6ull_led.ko
ls -l /dev/imx6ull_led
./led_test /dev/imx6ull_led 1
./led_test /dev/imx6ull_led 0
./led_test /dev/imx6ull_led 1
dmesg | tail -80
```

结果：

```text
crw------- 1 root root 10, 56 /dev/imx6ull_led
imx6ull-led imx6ull_led: imx6ull led driver probed
imx6ull-led imx6ull_led: led on
imx6ull-led imx6ull_led: led off
imx6ull-led imx6ull_led: led on
```

说明：

```text
/dev/imx6ull_led 已创建。
platform_driver 已通过 compatible 匹配 probe。
用户态 led_test 能通过字符设备控制 GPIO5_IO03 LED。
```

## 5. 注意事项

加载模块时出现：

```text
imx6ull_led: no symbol version for module_layout
imx6ull_led: loading out-of-tree module taints kernel.
```

当前模块仍然加载成功，驱动 probe 和 LED 控制均正常。

这属于外部模块常见警告，应在后续整理 Kernel build 目录时继续观察，
但不阻塞 Day 2 的 LED platform driver 闭环。

## 6. Day 2 结论

Day 2 完成：

```text
最小 DTB 临时启动成功。
主 DTB 未覆盖。
imx6ull_led platform driver probe 成功。
/dev/imx6ull_led 字符设备创建成功。
led_test 完成 on/off/on 控制。
SSH、eth0、用户态启动流程正常。
```

## 7. 主 DTB 替换验证

在 minimal DTB 临时启动验证通过后，已按用户要求替换主 DTB 并测试。

当前主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38828 bytes
sha256: be49fb54cd774bd1187608ea26946d975ef0d9495ce4f9f6af2f317bc6a6ecf8
```

原始 DTB 备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

普通重启后验证通过：

```text
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
eth0 = 192.168.77.200/24
/dev/imx6ull_led 已创建
led_test on/off/on 正常
```

详细记录：

```text
evidence/main-dtb-replacement.md
logs/day2-main-dtb-replacement-test.log
```

## 8. DTB 安全调试 SOP

已整理后续细改主 DTB 的完整 SOP：

```text
evidence/dtb-safe-debug-sop.md
```

该文档包括：

```text
原始 DTB 备份
反编译 DTS
最小补丁
diff 检查
U-Boot 临时加载
普通重启验收
主 DTB 替换
回滚方法
Windows / Ubuntu / IMX6ULL 控制链路
```
