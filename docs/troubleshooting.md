# Troubleshooting

## 串口无输出

- 检查串口线 TX/RX/GND。
- 检查波特率，常见为 `115200 8N1`。
- 检查拨码开关和启动介质。
- 检查 U-Boot 是否写入正确位置。

## Kernel 启动后找不到 rootfs

- 检查 `bootargs` 中 `root=` 是否正确。
- SD 卡 rootfs 检查分区号，例如 `/dev/mmcblk1p2`。
- NFS rootfs 检查 PC IP、板子 IP、NFS 导出目录。
- 确认内核启用了对应文件系统和网卡驱动。

## 驱动加载失败

- `dmesg` 查看错误。
- `modinfo` 检查模块信息。
- 确认内核版本与模块编译版本一致。
- 设备树 compatible 和驱动匹配字符串必须一致。

## MQTT 无法连接

- 检查 broker IP 和端口，默认 MQTT 端口为 `1883`。
- 检查 ThingsBoard Access Token。
- 在 PC 上先用 `mosquitto_pub` 测试。
- 检查板子能否 `ping` 到 PC/服务器。

## AI 推理过慢

- 优先使用 INT8 小模型。
- 避免在 IMX6ULL 上跑大检测模型和 LLM。
- 使用 ncnn/Edge Impulse 的 ARMv7 目标构建。
- 记录推理耗时、CPU 占用和内存占用，作为项目证据。

