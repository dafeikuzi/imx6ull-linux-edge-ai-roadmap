# RootFS Notes

## 必须掌握

- RootFS 是 Linux 用户态运行环境，包含命令、动态库、配置、设备节点和 init 脚本。
- 阶段 1 优先使用韦东山 BSP rootfs 跑通闭环。
- Buildroot 可作为增强任务，用来理解 rootfs 和工具链的自动化构建。

## SD RootFS 检查

板端执行：

```bash
uname -a
cat /proc/cmdline
mount
ip addr
df -h
ls -l /
```

## RootFS 信息模板

```text
RootFS source:
Create method:
Boot media:
Partition:
Init system:
BusyBox version:
SSH/dropbear:
gdbserver:
Network tools:

Verification output:

Problems and fixes:
```
