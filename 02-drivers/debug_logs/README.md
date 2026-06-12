# Legacy Debug Logs Notes

阶段 2 不再使用集中式 `debug_logs/` 目录保存所有日志。

硬性规则：每个小阶段的日志必须放在自己的 `logs/` 目录中，例如：

```text
02-drivers/day2-led-platform-driver/logs/
02-drivers/day4-key-irq-driver/logs/
02-drivers/day7-i2c-test/logs/
```

这样可以避免后期无法判断某份 `dmesg` 属于哪个实验。

