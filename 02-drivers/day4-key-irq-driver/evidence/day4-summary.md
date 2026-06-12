# Day 4 总结：KEY IRQ driver

日期：2026-06-11

## 1. Day4 目标

Day4 目标是完成按键中断驱动闭环：

```text
Device Tree imx6ull_key 节点
  -> platform_driver compatible 匹配
  -> devm_gpiod_get(dev, "key", ...)
  -> gpiod_to_irq()
  -> request_threaded_irq()
  -> 中断中记录 pressed/released
  -> wake_up_interruptible()
  -> 用户态 read() 阻塞读取事件
```

## 2. KEY GPIO 选择

Day4 使用：

```text
KEY GPIO: GPIO5_IO01
global gpio: gpio129
active: GPIO_ACTIVE_LOW
pinctrl: key1_100ask
phandle: 0x3a
```

设备树节点：

```dts
imx6ull_key {
    compatible = "demo,imx6ull-key";
    pinctrl-names = "default";
    pinctrl-0 = <0x3a>;
    key-gpios = <0x15 0x1 0x1>;
    status = "okay";
};
```

## 3. DTB 修改结果

Day4 仍然按 Day2/Day3 SOP 走最小补丁：

```text
当前主 DTB -> 反编译 DTS -> 追加 imx6ull_key -> 编译测试 DTB -> diff 检查 -> U-Boot 临时启动 -> 验证 -> 替换主 DTB
```

DTB diff：

```text
diff-current-vs-day4-key-minimal.patch = 17 lines
```

说明：

```text
只新增 imx6ull_key 节点。
没有修改 gpio_keys_100ask。
没有启用 generic gpio-keys。
没有改 SPI/ADC/UART/backlight/ethernet/mmc 等无关节点。
```

当前主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38970 bytes
```

备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day4-key  # Day2 minimal DTB
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

## 4. 编译结果

构建日志：

```text
logs/day4-key-build.log
```

结果：

```text
imx6ull_key.ko: ELF 32-bit LSB relocatable, ARM, EABI5
key_read_test:  ELF 32-bit LSB executable, ARM, EABI5
vermagic:       4.9.88 SMP preempt mod_unload modversions ARMv7 p2v8
```

## 5. 临时 DTB 启动验证

U-Boot 临时加载：

```text
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /root/stage2/day4-key/100ask_imx6ull-14x14-day4-key-minimal.dtb
bootz 0x80800000 - 0x83000000
```

启动后确认：

```text
/proc/device-tree/imx6ull_key/compatible = demo,imx6ull-key
key-gpios = <0x15 0x1 0x1>
pinctrl-0 = <0x3a>
```

## 6. 驱动 probe 验证

板端：

```bash
cd /root/stage2/day4-key
insmod ./imx6ull_key.ko
ls -l /dev/imx6ull_key
cat /proc/interrupts | grep imx6ull-key
cat /sys/kernel/debug/gpio | grep gpio-129
```

结果：

```text
/dev/imx6ull_key created
irq=208
gpio-129 | key | in hi IRQ
```

dmesg：

```text
imx6ull-key imx6ull_key: imx6ull key irq driver probed, irq=208 value=0 dev=/dev/imx6ull_key
```

## 7. 阻塞 read 事件验证

临时 DTB 模式下已捕获按键事件：

```text
waiting for key events on /dev/imx6ull_key...
event[1]: pressed value=0
event[2]: released value=1
EXIT:0
```

IRQ 计数：

```text
208: 20 gpio-mxc 1 Edge imx6ull-key
```

日志：

```text
logs/day4-key-read-test.log
```

## 8. 主 DTB 替换验证

临时 DTB 验证成功后，已替换主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb = Day4 KEY minimal DTB
```

普通重启后确认：

```text
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
/proc/device-tree/imx6ull_key/compatible = demo,imx6ull-key
eth0 = 192.168.77.200/24
```

主 DTB 模式下已确认：

```text
insmod imx6ull_key.ko 成功
/dev/imx6ull_key 创建成功
irq=208 注册成功
gpio129 被 key 驱动占用
```

主 DTB 模式下第二次 `key_read_test` 因未捕获到新的物理按键事件超时：

```text
waiting for key events on /dev/imx6ull_key...
EXIT:124
```

这不影响 Day4 核心结论，因为临时 DTB 模式已经捕获到真实按下/松开事件；
主 DTB 模式已验证节点自动生效和驱动 probe。后续若需要，可再按一次按键补主 DTB 模式事件日志。

## 9. 代码说明

驱动：

```text
src/imx6ull_key.c
```

关键点：

```text
devm_gpiod_get(dev, "key", GPIOD_IN) 读取 key-gpios。
gpiod_to_irq() 将 GPIO 转成 IRQ。
devm_request_threaded_irq() 注册上升沿/下降沿中断。
wait_event_interruptible() 实现阻塞 read。
wake_up_interruptible() 在中断中唤醒用户态。
poll() 已预留，为 Day5 poll/select 做准备。
```

app：

```text
src/key_read_test.c
```

用法：

```bash
./key_read_test /dev/imx6ull_key
./key_read_test /dev/imx6ull_key 2
```

其中 `2` 表示读取两个事件后退出。

## 10. Day4 结论

Day4 核心闭环完成：

```text
KEY 设备树最小补丁完成。
U-Boot 临时 DTB 启动成功。
主 DTB 替换成功。
KEY IRQ driver probe 成功。
/dev/imx6ull_key 创建成功。
阻塞 read 捕获 pressed/released 事件成功。
```

可以进入 Day5 KEY poll driver。
