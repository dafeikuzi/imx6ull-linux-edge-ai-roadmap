# Legacy Test App Notes

阶段 2 不再使用集中式 `test_app/` 目录保存所有用户态测试程序。

硬性规则：每个实验的测试程序必须放在自己的 `src/` 目录中，例如：

```text
02-drivers/day2-led-platform-driver/src/led_test.c
02-drivers/day4-key-irq-driver/src/key_read_test.c
02-drivers/day5-key-poll-driver/src/key_poll_test.c
```

