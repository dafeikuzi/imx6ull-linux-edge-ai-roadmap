# 02 Drivers

目标：复现并理解 GPIO、Key、UART、I2C 相关驱动和设备树调试。

## 技术栈

- Linux Kernel Module
- Device Tree
- GPIO / IRQ / UART / I2C
- 字符设备
- `dmesg`、`insmod`、`rmmod`

## 实现任务

- `led_driver/`：LED 字符设备驱动，可通过用户态控制亮灭。
- `key_irq_driver/`：按键中断驱动，支持阻塞读或 poll。
- `uart_test/`：串口收发测试。
- `i2c_sensor/`：I2C 传感器读取，优先复现 AT24C02/DHT/温湿度模块。
- `device_tree/`：保存设备树片段和修改记录。
- `debug_logs/`：保存 `dmesg`、加载卸载日志。

## 验收证据

```bash
insmod led_driver.ko
./led_test /dev/imx6ull_led 1
./led_test /dev/imx6ull_led 0
dmesg | tail -50
```

## 面试重点

- 字符设备注册流程。
- `open/read/write/ioctl` 的作用。
- 设备树 compatible 如何匹配驱动。
- 按键中断为什么不能在中断里做耗时操作。

