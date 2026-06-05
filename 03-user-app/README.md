# 03 User App

目标：实现板端用户态采集和控制程序，让数据采集从“手动测试”变成“可运行服务”。

## 技术栈

- C/C++
- Makefile
- Linux file/sysfs/dev node API
- JSON
- 日志和配置文件
- init 脚本或 systemd

## 主要程序

- `sensor_collector/`：读取传感器/GPIO 状态，输出 JSON。
- `led_control/`：控制 LED 设备节点或 sysfs。
- `config/collector.conf`：采样周期、设备路径、输出模式。
- `scripts/run_collector.sh`：板端启动脚本。

## 验收

```bash
cd sensor_collector
make
./sensor_collector ../config/collector.conf
```

期望输出：

```json
{"device":"imx6ull-pro","temperature":26.50,"humidity":58.20,"key_state":0,"led_state":1}
```

