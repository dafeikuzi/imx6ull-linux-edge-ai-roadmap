# 04 MQTT Cloud

目标：把 IMX6ULL 用户态采集数据通过 MQTT 上传到 ThingsBoard，并实现 RPC 控制回传。

## 技术栈

- MQTT
- ThingsBoard
- EMQX 或 Mosquitto
- Python 原型 / C 客户端实装
- JSON

## ThingsBoard Topic

```text
v1/devices/me/telemetry
v1/devices/me/attributes
v1/devices/me/rpc/request/+
```

## 实现任务

- PC 或云端启动 ThingsBoard。
- 在 ThingsBoard 创建设备并获取 Access Token。
- 运行 `mqtt_client/tb_mqtt_bridge.py`，读取 stdin JSON 并发布 telemetry。
- 后续把 Python 原型替换成板端 C MQTT 客户端。
- 实现 RPC：云端按钮控制 LED。

## 验收

```bash
python mqtt_client/tb_mqtt_bridge.py --host 127.0.0.1 --token YOUR_TOKEN
```

然后粘贴：

```json
{"device":"imx6ull-pro","timestamp":1710000000,"temperature":31.2,"humidity":60.0,"key_state":0,"led_state":1}
```

ThingsBoard Dashboard 应显示温湿度数据。

