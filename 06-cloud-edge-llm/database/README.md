# Database

`edge_data_service.py` 默认使用 SQLite：

```text
06-cloud-edge-llm/database/imx6ull_edge.db
```

查询最近记录：

```bash
sqlite3 imx6ull_edge.db "select ts, device, temperature, humidity from sensor_records order by id desc limit 10;"
```

后续扩展：

- GreptimeDB：用于时序数据和监控指标。
- Qdrant：用于设备日志、知识库、故障文档的向量检索。

