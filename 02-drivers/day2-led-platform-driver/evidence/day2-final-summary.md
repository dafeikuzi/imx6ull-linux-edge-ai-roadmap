# Day 2 最终收尾总结：LED platform driver

日期：2026-06-11

## 1. Day 2 目标

Day 2 的目标是完成一个“真实驱动闭环”：

```text
Device Tree 节点
  -> platform_driver compatible 匹配
  -> probe() 获取 GPIO
  -> 注册 /dev/imx6ull_led
  -> 用户态 led_test 控制 LED
  -> dmesg 记录 probe/on/off
```

## 2. 最终硬件与设备树结论

LED 引脚：

```text
GPIO: GPIO5_IO03
sysfs: gpio131
电平: active-low，低电平点亮
设备树 GPIO 描述: GPIO5 phandle + pin 3 + GPIO_ACTIVE_LOW
```

当前主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38828 bytes
状态: 已替换为 Day2 minimal DTB
```

原始 DTB 备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

当前运行时节点：

```text
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
```

## 3. 已完成工作

### 3.1 模块和测试程序

已完成：

```text
imx6ull_led.ko 编译成功
led_test 交叉编译成功
板端部署路径: /root/stage2/day2-led/
```

模块 vermagic：

```text
4.9.88 SMP preempt mod_unload modversions ARMv7 p2v8
```

### 3.2 DTB 调试

先踩坑，再修正：

```text
错误路径: 从源码 DTS 重新编译整板 DTB，导致 1023 行 diff，板子用户态异常。
正确路径: 从板子原始 DTB 反编译，只追加 imx6ull_led 最小节点。
```

最小 DTB 结果：

```text
original-restored.dtb                    38686 bytes
original-plus-imx6ull-led-minimal.dtb    38828 bytes
diff-original-vs-minimal.patch           17 lines
```

### 3.3 临时启动验证

已通过 U-Boot 临时加载测试 DTB：

```text
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /root/stage2/day2-led/100ask_imx6ull-14x14-day2-minimal.dtb
bootz 0x80800000 - 0x83000000
```

验证结果：

```text
SSH 正常返回。
eth0 正常。
imx6ull_led 节点出现。
驱动 probe 成功。
```

### 3.4 主 DTB 替换验证

在临时启动确认安全后，替换主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb = Day2 minimal DTB
```

普通重启后验证通过：

```text
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
eth0: 192.168.77.200/24
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
```

### 3.5 驱动最终验收

板端结果：

```text
insmod imx6ull_led.ko 成功
/dev/imx6ull_led 创建成功
led_test /dev/imx6ull_led 1 -> led on
led_test /dev/imx6ull_led 0 -> led off
led_test /dev/imx6ull_led 1 -> led on
```

dmesg：

```text
imx6ull-led imx6ull_led: imx6ull led driver probed
imx6ull-led imx6ull_led: led on
imx6ull-led imx6ull_led: led off
imx6ull-led imx6ull_led: led on
```

## 4. 当前板端状态

```text
Board IP: 192.168.77.200
RootFS: /dev/mmcblk1p2
主 DTB: Day2 minimal DTB
imx6ull_led.ko: 已加载
/dev/imx6ull_led: 存在
```

如果需要卸载：

```bash
rmmod imx6ull_led
```

如果需要恢复原始 DTB：

```bash
mount -o remount,rw /
cp -f /boot/100ask_imx6ull-14x14.dtb.before-day2-minimal /boot/100ask_imx6ull-14x14.dtb
sync
reboot -f
```

## 5. 已保存关键证据

```text
evidence/day2-completion.md
evidence/main-dtb-replacement.md
evidence/dtb-safe-debug-sop.md
evidence/dtb-diff/README.md
evidence/dtb-diff/diff-original-vs-day2.patch
evidence/dtb-diff/diff-original-vs-minimal.patch
logs/module-build-success.log
logs/serial-boot-minimal-dtb.log
logs/day2-led-minimal-dtb-test.log
logs/day2-main-dtb-replacement-test.log
```

## 6. 遗留注意项

模块加载时有警告：

```text
imx6ull_led: no symbol version for module_layout
imx6ull_led: loading out-of-tree module taints kernel.
```

当前不阻塞 Day 2：

```text
模块已加载。
probe 成功。
字符设备创建成功。
LED 控制正常。
```

后续可在 Kernel 编译环境整理时继续追踪 `Module.symvers` / `CONFIG_MODVERSIONS` 匹配问题。

## 7. 源码更新

Day2 驱动和 app 已更新：

```text
驱动: src/imx6ull_led.c 支持 write + read
app: src/led_test.c 支持 on/off/read/status/toggle
Makefile: 支持 all/app/clean
```

新版源码已重新交叉编译并部署到板端验证：

```text
led state: 0
led on
led state: 1
led off
led state: 0
led on
led off
led on
led off
led state: 0
```

详细记录：

```text
evidence/source-update-driver-app.md
logs/source-update-build.log
logs/source-update-board-test.log
```

## 8. Day 2 结论

Day 2 已完成，可以进入 Day 3。

完成状态：

```text
LED platform driver: 完成
Device Tree minimal patch: 完成
主 DTB 替换: 完成
回滚路径: 完成
用户态控制: 完成
证据归档: 完成
```
