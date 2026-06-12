# Day 5 KEY Poll Driver

目标：在 Day 4 按键中断驱动基础上增加 `.poll`，让用户态程序可以用 `poll()` 或 `select()` 等待按键事件。

## 当前状态

Day5 已完成：

```text
驱动: imx6ull_key_poll.ko
设备节点: /dev/imx6ull_key
IRQ: 208 / imx6ull-key-poll
app: key_poll_test
结果: poll() 捕获按键事件成功
```

总结：

```text
evidence/day5-summary.md
```

## 今日产物目录

```text
src/       # imx6ull_key_poll.c, key_poll_test.c, Makefile
dt/        # 复用 KEY 设备树节点
logs/      # poll 程序输出和 dmesg
evidence/  # poll 验收截图和结论
```

## 为什么要 poll

阻塞 `read()` 只能等待一个 fd。真实采集程序可能同时等待：

```text
按键 fd
串口 fd
MQTT socket
定时器 fd
```

`poll()` 可以统一等待多个事件，是后续用户态边缘应用的基础。

## 板端验收

```bash
cd /root/stage2/day5-key-poll
insmod imx6ull_key_poll.ko
./key_poll_test /dev/imx6ull_key
./key_poll_test /dev/imx6ull_key 2 60000
dmesg | tail -80
rmmod imx6ull_key_poll
```

## 代码解释

- `.poll` 中调用 `poll_wait()`，把当前进程挂到等待队列。
- 如果已有事件，返回 `POLLIN | POLLRDNORM`。
- 用户态 `poll()` 返回后再调用 `read()` 取走事件。
