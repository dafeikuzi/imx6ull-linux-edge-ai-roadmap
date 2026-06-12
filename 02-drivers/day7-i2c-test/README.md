# Day 7 I2C Test

目标：先用用户态方式验证 I2C 总线和设备地址，再理解 I2C client driver 的基本结构。阶段 2 第一版不强制写完整 I2C 内核驱动。

## 今日产物目录

```text
src/       # i2c_read.c, Makefile
dt/        # I2C 设备树摘录或设备地址记录
logs/      # i2cdetect、程序输出
evidence/  # 传感器/EEPROM 读取结论
```

## 板端检查

```bash
ls -l /dev/i2c-*
i2cdetect -l 2>/dev/null || true
dmesg | grep -Ei "i2c" | tail -80
```

扫描示例：

```bash
i2cdetect -y 0
i2cdetect -y 1
```

注意：不要对未知设备随意执行写操作。第一步只做扫描和只读寄存器测试。

## 编译

```bash
cd 02-drivers/day7-i2c-test/src
make CROSS_COMPILE=arm-linux-gnueabihf-
```

## 板端运行

读取示例：

```bash
./i2c_read /dev/i2c-0 0x50 0x00
```

参数含义：

```text
/dev/i2c-0  I2C 控制器设备节点
0x50        7-bit I2C 设备地址，常见 EEPROM 示例地址
0x00        要读取的寄存器地址
```

## I2C 内核驱动结构

完整 I2C client driver 通常长这样：

```text
of_device_id / i2c_device_id
  -> i2c_driver
  -> probe(client)
  -> i2c_smbus_read_byte_data()
  -> 注册字符设备或 input/iio 子系统
```

阶段 2 先把用户态读写和总线概念跑通，后续传感器项目再补完整 client driver。

