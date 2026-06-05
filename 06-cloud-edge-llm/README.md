# 06 Cloud Edge LLM

目标：用 Zenoh 或 MQTT 把 IMX6ULL 数据传到 PC 服务，写入数据库，并调用本地 LLM 生成故障解释和控制建议。

## 技术栈

- Zenoh
- Python
- SQLite
- Ollama 或 LocalAI
- JSON

## 数据流

```text
IMX6ULL sensor JSON
-> Zenoh key demo/imx6ull/sensor
-> PC data_service
-> SQLite
-> Ollama/LocalAI analysis
-> Zenoh key demo/imx6ull/control/led
-> IMX6ULL control
```

## 快速原型

安装依赖：

```bash
pip install eclipse-zenoh requests
```

启动订阅服务：

```bash
python data_service/edge_data_service.py
```

发布测试数据：

```bash
python zenoh_client/zenoh_pub_sensor.py
```

