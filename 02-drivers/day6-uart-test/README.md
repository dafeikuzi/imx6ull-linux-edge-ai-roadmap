# Day 6 UART Test

目标：使用用户态程序访问 UART。阶段 2 不重写 IMX6ULL UART 内核驱动，而是理解串口设备节点和 `termios` 配置方式。

## 今日产物目录

```text
src/       # uart_test.c, Makefile
dt/        # 串口设备树摘录
logs/      # 串口列表、程序输出
evidence/  # 收发截图和结论
```

## 检查串口

```bash
ls -l /dev/ttymxc*
dmesg | grep -Ei "ttymxc|uart|serial" | tail -80
```

注意：`/dev/ttymxc0` 通常是控制台串口，不建议拿来做普通收发实验。优先使用板子上空闲的串口。

## 编译

```bash
cd 02-drivers/day6-uart-test/src
make CROSS_COMPILE=arm-linux-gnueabihf-
```

## 板端运行

```bash
./uart_test /dev/ttymxc1 115200
```

程序会把标准输入写入串口，并尝试读取串口返回的数据。实际测试需要 USB-TTL 或另一个串口设备配合。

