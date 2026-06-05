# Ollama Service

PC 侧安装 Ollama 后，拉取一个小模型：

```bash
ollama pull qwen2.5:1.5b
ollama serve
```

测试：

```bash
curl http://127.0.0.1:11434/api/generate \
  -d '{"model":"qwen2.5:1.5b","prompt":"分析IMX6ULL温度38度是否异常","stream":false}'
```

如果 PC 性能较弱，可以换更小模型，或者先只使用 `edge_data_service.py` 中的规则判断。

