# Final Demo Script

## 目标

演示 IMX6ULL Edge AI Gateway 的完整闭环：

```text
采集 -> 上云 -> 展示 -> AI/LLM 分析 -> 控制回传 -> 外设动作
```

## 演示步骤

1. 启动 IMX6ULL，确认串口登录正常。
2. 启动 `sensor_collector`，观察 JSON 输出。
3. 启动 MQTT/ThingsBoard，确认 Dashboard 有实时数据。
4. 修改温度阈值或模拟异常数据。
5. 观察 ThingsBoard 告警或 PC 服务日志。
6. 调用 Ollama/LocalAI 分析最近数据。
7. PC 服务发送控制命令。
8. IMX6ULL 收到命令并控制 LED/蜂鸣器。

## 需要截图或记录

- 串口启动日志。
- `sensor_collector` JSON 输出。
- ThingsBoard Dashboard。
- AI/LLM 分析结果。
- 控制命令回传日志。
- 板端 LED/蜂鸣器动作视频。

