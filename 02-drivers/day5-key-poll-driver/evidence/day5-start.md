# Day5 启动记录：KEY poll driver

日期：2026-06-11

## Day4 交接

Day4 已完成 KEY IRQ 阻塞 read：

```text
KEY GPIO: GPIO5_IO01 / gpio129 / active-low
DTB 节点: imx6ull_key
compatible: demo,imx6ull-key
IRQ: 208
设备节点: /dev/imx6ull_key
```

当前主 DTB 已包含：

```text
imx6ull_led
imx6ull_key
```

Day5 不需要新增 DTB 节点，直接复用 Day4 的 `imx6ull_key`。

## Day5 目标

```text
验证 .poll 文件操作。
用户态使用 poll() 等待按键事件。
poll 返回后调用 read() 读取 pressed/released。
为后续多 fd 用户态采集程序打基础。
```

## 注意事项

Day4 的 `imx6ull_key.ko` 和 Day5 的 `imx6ull_key_poll.ko` 使用同一个：

```text
compatible = demo,imx6ull-key
/dev/imx6ull_key
```

因此二者不能同时作为设备绑定驱动。

Day5 测试前需要：

```bash
rmmod imx6ull_key
insmod imx6ull_key_poll.ko
```

如果 `rmmod` busy，则普通重启后再加载 Day5 poll 驱动。
