# IMX6ULL Linux Edge AI Roadmap

这是一个面向嵌入式 Linux 实习的长期复现路线仓库。目标不是一次性复制某个大项目，而是把 IMX6ULL_Pro 的学习拆成一组能逐步跑通、能展示、能写进简历的项目。

最终闭环：

```text
IMX6ULL 采集传感器/按键/GPIO
-> C/C++ 用户态程序封装 JSON
-> MQTT 或 Zenoh 上传到 PC/云端
-> ThingsBoard 展示和告警
-> Edge AI 判断异常
-> 数据库保存历史
-> Ollama/LocalAI 生成分析建议
-> 返回控制指令
-> IMX6ULL 控制 LED/蜂鸣器/外设
```

## 推荐学习顺序

| 阶段 | 目录 | 目标 |
|---|---|---|
| 1 | `01-linux-system/` | 构建 U-Boot、Kernel、RootFS，完成串口/SSH/NFS 调试 |
| 2 | `02-drivers/` | 调试 LED、Key、UART、I2C、设备树和内核模块 |
| 3 | `03-user-app/` | 编写 C/C++ 采集程序、控制程序、日志和配置 |
| 4 | `04-mqtt-cloud/` | MQTT 接入 ThingsBoard，实现数据上云和远程控制 |
| 5 | `05-edge-ai/` | 复现 Edge Impulse/ncnn，部署 INT8 小模型 |
| 6 | `06-cloud-edge-llm/` | Zenoh + 数据库 + Ollama/LocalAI，完成云边协同 |
| 7 | `final-demo/` | 整合成 IMX6ULL Edge AI Gateway |

## 怎么使用这个仓库

1. 先读 `docs/learning_route.md`，按阶段推进。
2. 每完成一个阶段，把命令、日志、截图放到对应目录。
3. 每个阶段都保留 `README.md` 中的验收证据。
4. 最后用 `docs/interview_notes.md` 整理简历项目和面试回答。

## 推荐外部项目

- Buildroot: <https://github.com/buildroot/buildroot>
- ThingsBoard: <https://github.com/thingsboard/thingsboard>
- EMQX: <https://github.com/emqx/emqx>
- Edge Impulse standalone Linux: <https://github.com/edgeimpulse/example-standalone-inferencing-linux>
- ncnn: <https://github.com/Tencent/ncnn>
- Zenoh: <https://github.com/eclipse-zenoh/zenoh>
- Qdrant: <https://github.com/qdrant/qdrant>
- Ollama: <https://github.com/ollama/ollama>
- LocalAI: <https://github.com/mudler/LocalAI>

