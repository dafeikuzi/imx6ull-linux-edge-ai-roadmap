# Day5 总结：KEY poll driver

日期：2026-06-11

## 1. Day5 目标

Day5 在 Day4 KEY IRQ 基础上验证 `poll()` 模型：

```text
按键中断发生
  -> 驱动记录事件
  -> wake_up_interruptible()
  -> .poll 返回 POLLIN | POLLRDNORM
  -> 用户态 poll() 返回
  -> 用户态 read() 读取事件
```

## 2. 设备树状态

Day5 复用 Day4 主 DTB，不新增设备树节点：

```text
/proc/device-tree/imx6ull_key/compatible = demo,imx6ull-key
key-gpios = GPIO5_IO01 / gpio129 / active-low
```

## 3. 源码

驱动：

```text
src/imx6ull_key_poll.c
```

app：

```text
src/key_poll_test.c
```

Makefile：

```text
src/Makefile
```

## 4. 代码更新点

驱动：

```text
支持 read()
支持 poll()
中断中记录 pressed/released value
使用 wait queue 唤醒 poll/read
```

app：

```bash
./key_poll_test /dev/imx6ull_key [count] [timeout_ms]
```

示例：

```bash
./key_poll_test /dev/imx6ull_key 2 60000
```

表示最多等待 60 秒，捕获 2 个事件后退出。

## 5. 编译结果

日志：

```text
logs/day5-key-poll-build.log
```

结果：

```text
imx6ull_key_poll.ko: ELF 32-bit LSB relocatable, ARM, EABI5
key_poll_test:       ELF 32-bit LSB executable, ARM, EABI5
vermagic:            4.9.88 SMP preempt mod_unload modversions ARMv7 p2v8
```

## 6. 板端加载

首次尝试时 Day4 旧模块 `imx6ull_key.ko` 仍在，`rmmod` busy，导致 Day5 poll 驱动不能独占绑定设备。

处理：

```text
普通重启清空旧模块。
只加载 imx6ull_key_poll.ko。
```

加载后：

```text
imx6ull_key_poll 3699 0
/dev/imx6ull_key created
IRQ 208: imx6ull-key-poll
gpio-129 | key | in hi IRQ
```

日志：

```text
logs/day5-key-poll-insmod.log
```

## 7. poll 测试结果

测试命令：

```bash
./key_poll_test /dev/imx6ull_key 2 60000
```

结果：

```text
poll waiting on /dev/imx6ull_key, count=2, timeout_ms=60000
poll event[1]: released value=1
poll event[2]: pressed value=0
EXIT:0
```

IRQ：

```text
208: 4 gpio-mxc 1 Edge imx6ull-key-poll
```

日志：

```text
logs/day5-key-poll-test.log
```

## 8. Day5 结论

Day5 完成：

```text
.poll 驱动接口验证成功。
用户态 poll() 能等待 /dev/imx6ull_key。
poll 返回后 read() 能读取按键事件。
按键事件和 IRQ 计数匹配。
```

可以进入 Day6 UART 用户态测试。
