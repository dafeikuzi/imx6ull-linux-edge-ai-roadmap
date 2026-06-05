# 2026-06-05 工作总结

## 今日目标

把 IMX6ULL_Pro 嵌入式 Linux 实习复现路线整理成一个可长期维护的 Git 项目，并推送到 GitHub，方便后续持续补充代码、日志、截图、复现记录和面试材料。

## 今日完成任务

1. 明确项目定位

- 主攻方向：嵌入式 Linux 实习。
- 项目主线：Linux 系统构建、驱动调试、用户态应用、MQTT/Zenoh 通信、轻量 AI、云边协同、大模型服务。
- 资源分工：IMX6ULL 负责采集、控制、轻量推理和通信；PC/服务器负责 ThingsBoard、数据库、Zenoh router、Ollama/LocalAI 和较重 AI。

2. 创建项目骨架

- 创建阶段目录：
  - `01-linux-system/`
  - `02-drivers/`
  - `03-user-app/`
  - `04-mqtt-cloud/`
  - `05-edge-ai/`
  - `06-cloud-edge-llm/`
  - `final-demo/`
  - `docs/`
- 每个阶段都有 README 或说明文件，便于后续逐项复现。

3. 添加核心文档

- `README.md`：项目总说明和推荐学习顺序。
- `docs/learning_route.md`：分阶段学习路线。
- `docs/architecture.md`：系统架构和数据流。
- `docs/interview_notes.md`：简历表达和面试要点。
- `docs/troubleshooting.md`：常见问题排查。
- `docs/evidence_template.md`：复现证据记录模板。

4. 添加 starter code

- `03-user-app/sensor_collector/sensor_collector.c`
  - 用户态 C 采集程序。
  - 读取配置文件。
  - 输出统一 JSON。
  - 支持日志追加。
- `03-user-app/led_control/led_control.c`
  - 用户态 LED 控制程序。
  - 可写 sysfs 或设备节点。
- `04-mqtt-cloud/mqtt_client/tb_mqtt_bridge.py`
  - 从 stdin 读取 JSON。
  - 发布到 ThingsBoard MQTT telemetry topic。
  - 订阅 RPC topic。
- `06-cloud-edge-llm/zenoh_client/zenoh_pub_sensor.py`
  - Zenoh 传感器数据发布原型。
- `06-cloud-edge-llm/data_service/edge_data_service.py`
  - PC 侧订阅 Zenoh 数据。
  - 写入 SQLite。
  - 调用 Ollama。
  - 生成 LED 控制建议并回传。

5. 做基础验证

- Python 脚本使用 `py -3 -m py_compile` 通过语法检查。
- C 程序使用 `gcc -fsyntax-only` 通过源码语法检查。
- 清理了验证生成的 `__pycache__` 文件。

6. 包装成 Git 项目

- 在 `E:\imx6ull-linux-edge-ai-roadmap` 初始化 Git 仓库。
- 设置当前分支为 `main`。
- 添加 `.gitignore`，忽略编译产物、镜像、模型、数据库、密钥等。
- 添加 `.gitattributes`，固定 Linux 友好的换行规则。
- 完成初始提交：
  - `56fe703 init imx6ull linux edge ai roadmap`
  - `42e954d chore: configure repository line endings`

7. 推送到 GitHub

- GitHub 仓库：
  - https://github.com/dafeikuzi/imx6ull-linux-edge-ai-roadmap
- 本地 `main` 已跟踪远程 `origin/main`。

## 今日遇到的问题

1. 工作区最初不是 Git 仓库

- 现象：`git status` 报错 `fatal: not a git repository`。
- 处理：在迁移后的项目目录 `E:\imx6ull-linux-edge-ai-roadmap` 中重新初始化 Git。

2. Python 命令不可用

- 现象：`python` 指向 Windows Store 占位符，返回 exit code `9009`。
- 处理：改用 `py -3 -m py_compile` 做 Python 语法检查。

3. Windows/MSYS2 GCC 完整编译失败

- 现象：`gcc -o xxx.exe` 在当前 Windows 环境完整编译阶段返回失败。
- 判断：更像当前 shell/MSYS2 后端环境问题，不是源码语法问题。
- 处理：使用 `gcc -fsyntax-only` 验证 C 源码语法；后续实际目标编译建议放到 Linux、WSL 或 ARM 交叉编译环境。

4. 第一次推送 GitHub 网络失败

- 现象：`git push` 出现 `Recv failure: Connection was reset`。
- 处理：确认远程仓库可访问后重试，第二次推送成功。

5. GitHub CLI 不存在

- 现象：`gh` 命令不可用。
- 处理：不使用 GitHub CLI，改为手动创建 GitHub 仓库，然后用 HTTPS remote 推送。

## 后续解决思路

1. 配置真实 Git 身份

当前仓库本地 Git 身份是临时值：

```text
user.name = IMX6ULL Learner
user.email = imx6ull@example.local
```

建议后续改成自己的 GitHub 信息：

```powershell
git config user.name "你的名字"
git config user.email "你的邮箱"
```

2. 建议使用 WSL/Linux 做编译

后续涉及 Buildroot、Kernel、驱动、交叉编译，建议使用 Ubuntu/WSL：

```text
Windows 负责资料整理和 Git 管理
WSL/Ubuntu 负责编译 Buildroot、Kernel、驱动、C 程序
IMX6ULL 负责运行和验证
```

3. 下一阶段优先做 `01-linux-system`

建议下一步从系统构建开始：

- 整理韦东山 BSP 编译步骤。
- 记录 U-Boot 启动日志。
- 记录 Kernel 启动日志。
- 跑通 NFS rootfs。
- 交叉编译 hello 程序。
- 把证据填入 `docs/evidence_template.md`。

4. 每完成一个小闭环就提交

推荐提交节奏：

```powershell
git status
git add .
git commit -m "docs: add nfs rootfs bringup notes"
git push
```

5. 后续新开对话的上下文读取顺序

新对话可以优先读取：

```text
README.md
docs/session_summary_2026-06-05.md
progress.md
task_plan.md
docs/learning_route.md
```

这样可以快速恢复项目定位、当前进度、遇到的问题和下一步计划。

