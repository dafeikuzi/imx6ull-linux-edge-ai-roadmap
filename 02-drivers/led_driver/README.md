# LED Driver

这里放 LED 字符设备驱动。建议先复现韦东山课程代码，再逐步改成你自己的结构。

## 推荐接口

```text
/dev/imx6ull_led
write "1" -> turn on
write "0" -> turn off
```

## 文件建议

```text
led_driver.c
led_test.c
Makefile
```

## 验收

- `insmod` 成功。
- `/dev/imx6ull_led` 出现。
- 用户态程序可以控制 LED。
- `dmesg` 有 probe/open/write 日志。

