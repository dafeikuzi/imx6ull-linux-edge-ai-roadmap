# Day 4 KEY IRQ Driver

目标：编写按键中断驱动。用户态读取 `/dev/imx6ull_key` 时，如果没有按键事件就阻塞；按键触发中断后，驱动唤醒 `read()` 并返回事件。

## 当前状态

Day4 核心闭环已完成：

```text
KEY GPIO: GPIO5_IO01 / gpio129 / active-low
DTB 节点: /proc/device-tree/imx6ull_key
compatible: demo,imx6ull-key
IRQ: 208
用户态设备: /dev/imx6ull_key
事件: pressed value=0 / released value=1
```

总结：

```text
evidence/day4-summary.md
```

## 今日产物目录

```text
src/       # imx6ull_key.c, key_read_test.c, Makefile
dt/        # KEY 设备树节点
logs/      # 编译、insmod、dmesg、按键输出
evidence/  # 按键触发截图和最终结论
```

## 驱动结构

```text
key-gpios
  -> gpiod_to_irq()
  -> request threaded irq
  -> irq handler 记录 pressed/released
  -> wake_up_interruptible()
  -> read() 返回事件字符串
```

## 编译和部署

```bash
cd 02-drivers/day4-key-irq-driver/src
export KDIR=/path/to/linux-4.9.88
make
sudo mkdir -p /nfs/imx6ull-rootfs/root/stage2/day4-key
sudo cp imx6ull_key.ko key_read_test /nfs/imx6ull-rootfs/root/stage2/day4-key/
sync
```

## 板端验收

```bash
cd /root/stage2/day4-key
insmod imx6ull_key.ko
ls -l /dev/imx6ull_key
./key_read_test /dev/imx6ull_key
./key_read_test /dev/imx6ull_key 2
dmesg | tail -80
rmmod imx6ull_key
```

## 代码解释

- `wait_queue_head_t`：让 `read()` 在无事件时睡眠。
- `atomic_t event_pending`：标记是否有按键事件。
- `request_threaded_irq()`：注册中断处理。
- `IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING`：同时监听按下和松开。
- `copy_to_user()`：把事件字符串返回给用户态。
