# 学习路线

## 阶段 1：Linux 系统构建

重点技能：

- U-Boot 启动参数
- Linux Kernel 编译
- RootFS 制作
- SD 卡启动、NFS 启动、串口登录、SSH 登录
- 交叉编译工具链

达成效果：

- IMX6ULL 可以启动自制系统。
- PC 可以通过串口和网络调试板子。
- 一个交叉编译的 `hello` 程序可以在板子运行。

参考资料：

- 韦东山 IMX6ULL Pro 开发板资料
- Buildroot 官方仓库：<https://github.com/buildroot/buildroot>
- NXP i.MX Linux BSP 资料

## 阶段 2：驱动和设备树

重点技能：

- 设备树节点、pinctrl、GPIO 编号
- 字符设备驱动
- `read/write/ioctl`
- 按键中断
- I2C/UART 传感器访问
- `dmesg` 调试

达成效果：

- LED 可以被用户态程序控制。
- 按键触发中断。
- I2C 或 UART 传感器可以稳定读取数据。

## 阶段 3：用户态边缘应用

重点技能：

- C/C++ Linux 系统编程
- 文件、串口、线程、信号处理
- JSON 封装
- 日志、配置、后台运行
- systemd 或 init 脚本自启动

达成效果：

- 板端定时采集数据并输出 JSON。
- 程序支持配置文件和日志。
- 程序可作为后台服务运行。

## 阶段 4：MQTT 上云

重点技能：

- MQTT topic、QoS、断线重连
- ThingsBoard 设备 Token
- Telemetry、Attributes、RPC
- 云端 Dashboard 和告警

达成效果：

- IMX6ULL 数据进入 ThingsBoard。
- Dashboard 显示实时曲线。
- 云端 RPC 控制板端 LED。

## 阶段 5：轻量 AI

重点技能：

- INT8 量化模型
- Edge Impulse Linux SDK
- ncnn 模型转换和 C++ 推理
- ARMv7 交叉编译
- 推理耗时和内存统计

达成效果：

- 传感器异常或简单图像分类可以推理。
- 推理结果上传到云端。

## 阶段 6：云边协同和大模型

重点技能：

- Zenoh Pub/Sub 和 Query
- SQLite/GreptimeDB 数据记录
- Ollama/LocalAI 本地大模型服务
- 服务端 Python 数据处理
- 控制指令回传

达成效果：

- PC 服务能订阅板端数据。
- 数据写入数据库。
- 大模型根据数据生成故障解释。
- 控制建议返回 IMX6ULL。

