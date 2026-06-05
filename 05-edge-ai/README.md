# 05 Edge AI

目标：在 ARMv7 Linux 或 PC 边缘端部署轻量 AI 模型，把推理结果接入 MQTT/ThingsBoard。

## 技术栈

- Edge Impulse standalone inferencing Linux
- ncnn
- TFLite / INT8
- C/C++
- ARMv7 交叉编译

## 推荐顺序

1. 在 PC 上跑通 Edge Impulse 示例。
2. 使用 ARMv7 参数交叉编译。
3. 在 IMX6ULL 上运行小模型。
4. 记录推理耗时和内存。
5. 把结果转成 JSON 上传云端。
6. 再复现 ncnn MobileNet/SqueezeNet 分类。

## 推荐模型

- 传感器异常分类：`normal/warning/alarm`
- 简单图像分类：`empty/object`
- 音频关键词识别：可选

## 验收

输出类似：

```json
{"device":"imx6ull-pro","ai_label":"warning","ai_score":0.87,"latency_ms":42}
```

并能上传 ThingsBoard。

