# Day4 到 Day5 交接总结

日期：2026-06-11

## Day4 已完成

```text
KEY GPIO: GPIO5_IO01 / gpio129 / active-low
DTB 节点: imx6ull_key
compatible: demo,imx6ull-key
IRQ: 208
设备节点: /dev/imx6ull_key
阻塞 read: pressed/released 已验证
```

主 DTB 当前已经包含：

```text
imx6ull_led
imx6ull_key
```

当前主 DTB：

```text
/boot/100ask_imx6ull-14x14.dtb
size: 38970 bytes
```

回滚备份：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day4-key
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

## Day5 起点

Day5 不需要再新增设备树节点。

Day5 重点：

```text
在 Day4 KEY IRQ 基础上验证 poll/select 模型。
用户态使用 poll() 等待 /dev/imx6ull_key。
poll 返回后再 read() 取走 pressed/released 事件。
```

注意：

```text
Day4 的 imx6ull_key.ko 当前可能仍加载。
Day5 测试独立 imx6ull_key_poll.ko 前，需要先 rmmod imx6ull_key。
两个模块使用同一个 compatible 和 /dev/imx6ull_key，不能同时加载。
```
