# Day 2 LED Platform Driver

目标：编写一个现代结构的 LED platform 驱动，通过设备树匹配 GPIO，并向用户态提供 `/dev/imx6ull_led`。

## 当前状态

Day 2 已完成。

最终结论：

```text
LED: GPIO5_IO03 / gpio131 / active-low
DTB: 主 DTB 已替换为基于原始 DTB 的 minimal DTB
节点: /proc/device-tree/imx6ull_led
compatible: demo,imx6ull-led
驱动: imx6ull_led.ko 可 probe
用户态设备: /dev/imx6ull_led
测试: led_test on/off/on 通过
```

收尾总结：

```text
evidence/day2-final-summary.md
```

## 今日产物目录

```text
src/       # imx6ull_led.c, led_test.c, Makefile
dt/        # LED 设备树节点
logs/      # 编译、insmod、dmesg、rmmod 输出
evidence/  # LED 亮灭截图、最终验收记录
```

## 驱动结构

```text
设备树 compatible = "demo,imx6ull-led"
  -> platform_driver 匹配
  -> probe() 获取 led-gpios
  -> misc_register() 创建 /dev/imx6ull_led
  -> write("1") / write("0") 控制 GPIO
```

## 编译

在 Ubuntu BSP Kernel 源码环境中：

```bash
cd 02-drivers/day2-led-platform-driver/src
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export KDIR=/path/to/linux-4.9.88
make
```

部署到 NFS RootFS：

```bash
sudo mkdir -p /nfs/imx6ull-rootfs/root/stage2/day2-led
sudo cp imx6ull_led.ko led_test /nfs/imx6ull-rootfs/root/stage2/day2-led/
sync
```

## 板端验收

```bash
cd /root/stage2/day2-led
insmod imx6ull_led.ko
ls -l /dev/imx6ull_led
echo 1 > /dev/imx6ull_led
echo 0 > /dev/imx6ull_led
./led_test /dev/imx6ull_led 1
./led_test /dev/imx6ull_led 0
./led_test /dev/imx6ull_led read
./led_test /dev/imx6ull_led toggle 4 150
dmesg | tail -50
rmmod imx6ull_led
```

## 代码解释

- `of_device_id`：声明驱动能匹配 `demo,imx6ull-led`。
- `probe()`：设备树匹配成功后执行，负责申请 GPIO 和注册设备节点。
- `devm_gpiod_get()`：从设备树读取 `led-gpios`，由内核自动管理释放。
- `misc_register()`：创建 `/dev/imx6ull_led`，比完整 `cdev` 更适合入门。
- `write()`：支持 `1/on` 点亮，`0/off` 熄灭。
- `read()`：返回当前 LED 逻辑状态，`0\n` 或 `1\n`。
- `led_test.c`：用户态 app，支持 `on/off/read/status/toggle`。

## 源码更新记录

最新版驱动和 app 说明：

```text
evidence/source-update-driver-app.md
logs/source-update-build.log
logs/source-update-board-test.log
```

## 兼容提示

如果 Linux 4.9 BSP 中 `devm_gpiod_get()` 编译失败，检查头文件是否包含：

```c
#include <linux/gpio/consumer.h>
```

如果 `GPIOD_OUT_LOW` 不存在，可临时改成旧 GPIO API，但文档中仍保留 descriptor API 作为主线。
