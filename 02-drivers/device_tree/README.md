# Legacy Device Tree Notes

这个目录保留为早期设备树占位入口。阶段 2 的正式设备树记录放在：

```text
02-drivers/day3-device-tree-notes/
```

LED 和 KEY 的学习节点模板分别位于：

```text
02-drivers/day2-led-platform-driver/dt/imx6ull-led-node.dts
02-drivers/day4-key-irq-driver/dt/imx6ull-key-node.dts
02-drivers/day5-key-poll-driver/dt/imx6ull-key-node.dts
```

规则：每个小阶段的设备树片段必须放在自己的 `dt/` 目录里，避免后续无法判断某个节点属于哪个实验。

