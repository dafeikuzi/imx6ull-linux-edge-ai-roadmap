# Day 2 源码更新：驱动与 app

日期：2026-06-11

## 1. 更新目标

根据阶段 2 的源码产物要求，Day2 源码不只保留最小可运行版本，还要便于后续复用和面试讲解。

本次重点更新：

```text
内核驱动: src/imx6ull_led.c
用户态 app: src/led_test.c
构建文件: src/Makefile
```

## 2. 驱动更新点

原版本只支持：

```text
write("1") -> led on
write("0") -> led off
```

新版支持：

```text
write("1") / write("on")   -> led on
write("0") / write("off")  -> led off
read()                     -> 返回 "0\n" 或 "1\n"
```

驱动接口：

```c
static ssize_t imx6ull_led_write(struct file *file,
                                 const char __user *buf,
                                 size_t count,
                                 loff_t *ppos);

static ssize_t imx6ull_led_read(struct file *file,
                                char __user *buf,
                                size_t count,
                                loff_t *ppos);
```

关键机制：

```text
miscdevice 创建 /dev/imx6ull_led。
file->private_data 指向 miscdevice。
container_of() 找回 struct imx6ull_led。
devm_gpiod_get(dev, "led", ...) 对应设备树 led-gpios。
gpiod_set_value() 使用逻辑电平，设备树 active-low 会由 GPIO descriptor 层处理。
gpiod_get_value() 读取逻辑状态。
```

## 3. app 更新点

原版本只支持：

```bash
./led_test /dev/imx6ull_led 0
./led_test /dev/imx6ull_led 1
```

新版支持：

```bash
./led_test /dev/imx6ull_led on
./led_test /dev/imx6ull_led off
./led_test /dev/imx6ull_led read
./led_test /dev/imx6ull_led status
./led_test /dev/imx6ull_led toggle 4 150
```

其中：

```text
read/status: 调用驱动 read()，打印 led state。
toggle count delay_ms: 循环 on/off，适合肉眼观察 LED。
```

## 4. 兼容性处理

本次曾尝试使用：

```c
gpiod_set_consumer_name()
```

但当前 Linux 4.9.88 BSP 不支持该接口，编译报错：

```text
implicit declaration of function 'gpiod_set_consumer_name'
```

处理方式：

```text
删除该调用。
保持驱动兼容 Linux 4.9.88。
```

## 5. 重新编译结果

Ubuntu 构建日志：

```text
logs/source-update-build.log
```

结果：

```text
imx6ull_led.ko: ELF 32-bit LSB relocatable, ARM, EABI5
led_test:       ELF 32-bit LSB executable, ARM, EABI5
vermagic:       4.9.88 SMP preempt mod_unload modversions ARMv7 p2v8
```

## 6. 板端测试结果

板端部署路径：

```text
/root/stage2/day2-led/imx6ull_led.ko
/root/stage2/day2-led/led_test
```

测试命令：

```bash
cd /root/stage2/day2-led
insmod ./imx6ull_led.ko
./led_test /dev/imx6ull_led read
./led_test /dev/imx6ull_led on
./led_test /dev/imx6ull_led status
./led_test /dev/imx6ull_led off
./led_test /dev/imx6ull_led read
./led_test /dev/imx6ull_led toggle 4 150
./led_test /dev/imx6ull_led status
```

结果：

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

dmesg：

```text
imx6ull-led imx6ull_led: imx6ull led driver probed, dev=/dev/imx6ull_led
imx6ull-led imx6ull_led: led on
imx6ull-led imx6ull_led: led off
```

日志：

```text
logs/source-update-board-test.log
```

## 7. 注意事项

第一次测试时旧模块仍在内核中，`rmmod` 返回：

```text
Device or resource busy
```

导致新模块没有真正加载，`read/status` 仍然打到旧驱动并返回：

```text
read: Invalid argument
```

解决：

```text
普通重启清掉旧模块。
重新 insmod 新版 imx6ull_led.ko。
再测试 read/status/on/off/toggle。
```

## 8. 结论

Day2 源码已经升级完成：

```text
驱动支持 write + read。
app 支持 on/off/read/status/toggle。
Makefile 支持 all/app/clean。
新版源码已交叉编译。
新版模块和 app 已在板端验证通过。
```
