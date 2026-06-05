# LED RPC Handler

ThingsBoard RPC 请求示例：

```json
{
  "method": "setLed",
  "params": 1
}
```

板端处理逻辑：

```text
subscribe v1/devices/me/rpc/request/+
-> parse method=setLed
-> write 1/0 to /dev/imx6ull_led or sysfs LED path
-> publish response to v1/devices/me/rpc/response/<request_id>
```

后续任务：把 `tb_mqtt_bridge.py` 的 RPC 打印逻辑改成调用 `03-user-app/led_control/led_control`。

