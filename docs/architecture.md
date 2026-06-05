# 系统架构

## 总体架构

```text
+-------------------+        MQTT / Zenoh        +----------------------+
| IMX6ULL_Pro       | -------------------------> | PC / Edge Server     |
|-------------------|                            |----------------------|
| Linux + RootFS    |                            | ThingsBoard / EMQX   |
| Device Drivers    |                            | Zenoh Router         |
| sensor_collector  |                            | SQLite / Qdrant      |
| led_control       | <------------------------- | Ollama / LocalAI     |
+-------------------+      RPC / control cmd     +----------------------+
```

## 数据流

```text
sensor/key/gpio
-> driver or sysfs/dev node
-> sensor_collector
-> JSON
-> MQTT telemetry or Zenoh key
-> dashboard/database/AI service
-> alarm/control command
-> led_control or device driver
```

## 推荐 Topic / Key

MQTT for ThingsBoard:

```text
v1/devices/me/telemetry
v1/devices/me/attributes
v1/devices/me/rpc/request/+
```

Zenoh:

```text
demo/imx6ull/sensor
demo/imx6ull/ai/result
demo/imx6ull/control/led
```

## IMX6ULL 与 PC 分工

IMX6ULL：

- Linux 系统、驱动、采集、控制
- 轻量模型推理
- MQTT/Zenoh 通信

PC/服务器：

- Dashboard 和 MQTT broker
- Zenoh router
- 数据库
- 大模型和较重 AI 推理

