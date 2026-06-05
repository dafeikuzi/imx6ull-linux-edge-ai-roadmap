# AI Benchmark

| Model | Runtime | Target | Precision | Latency ms | RSS MB | Notes |
|---|---|---|---|---:|---:|---|
| sensor-anomaly | Edge Impulse | IMX6ULL ARMv7 | INT8 | TBD | TBD | First target |
| mobilenet-small | ncnn | PC / IMX6ULL | FP32/INT8 | TBD | TBD | Optional |

## 记录命令

```bash
/usr/bin/time -v ./your_inference_binary
```

## 结论模板

- 模型是否适合板端实时运行：
- 推荐采样频率：
- 是否需要放到 PC 边缘端：

