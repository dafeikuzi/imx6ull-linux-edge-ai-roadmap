# Final Demo

这个目录用于最终整合项目：`IMX6ULL Edge AI Gateway`。

## 最小闭环

```text
sensor_collector
-> tb_mqtt_bridge.py
-> ThingsBoard Dashboard
-> edge_data_service.py
-> Ollama/LocalAI advice
-> control command
-> led_control
```

## 推荐交付物

- `imx6ull_edge_ai_gateway/`：最终整合代码。
- `logs/`：串口、应用、服务端日志。
- `screenshots/`：Dashboard、终端、数据库查询截图。
- `demo_script.md`：演示步骤。

## 第一版 MVP 标准

- 板端能输出 JSON。
- PC 能接收并转发 MQTT。
- Dashboard 能显示实时温湿度。
- 温度超过阈值时，PC 服务生成控制建议。
- 板端收到命令后控制 LED 或打印控制日志。

