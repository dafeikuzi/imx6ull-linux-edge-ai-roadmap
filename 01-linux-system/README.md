# 01 Linux System

目标：构建 IMX6ULL 可启动 Linux 系统，并建立串口、SSH、NFS、交叉编译调试流程。

## 推荐执行方式

本阶段按 `one-week-sprint.md` 执行：

1. 先理解启动链路：BootROM -> SPL/U-Boot -> Kernel -> RootFS -> init。
2. 再按韦东山 IMX6ULL_Pro BSP 跑通实板闭环。
3. 每天至少沉淀一个证据：命令、日志、截图、问题记录或面试解释。

配套学习内容：

- `stage1-knowledge.md`：完整知识讲义，解释启动链路、U-Boot、Kernel、设备树、RootFS、NFS、交叉编译和排障。
- `one-week-sprint.md`：一周执行计划，按天安排学习和验收。

默认环境：

- 编译环境：虚拟机 Ubuntu，网络建议使用桥接模式。
- 开发板：IMX6ULL_Pro 实板。
- 路线偏好：原理优先，但一周内必须跑通 U-Boot、Kernel、RootFS、NFS 和交叉编译 hello。

## 技术栈

- U-Boot
- Linux Kernel
- RootFS
- Buildroot 或韦东山 BSP
- NFS、TFTP、串口终端、SSH

## 实现任务

- 编译 U-Boot 并记录启动日志。
- 编译 Kernel 和设备树。
- 制作 RootFS，加入 busybox、dropbear/openssh、gdbserver、mosquitto-clients、sqlite。
- 支持 SD 卡启动和 NFS rootfs 启动。
- 交叉编译 `hello` 程序并在板端运行。

## 验收证据

- `boot_logs/uboot.log`
- `boot_logs/kernel.log`
- `rootfs_notes/nfs-rootfs.md`
- `hello_cross_compile/hello.c`
- `hello_cross_compile/README.md`
- 串口登录截图放入 `images/`
- SSH 登录截图放入 `images/`

## 常用命令模板

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
```

## 不提交到仓库的内容

- 完整 BSP 源码。
- Kernel/U-Boot 编译产物。
- SD 卡镜像、rootfs 压缩包、大型日志包。
- 密钥、Token、私有网络配置。

仓库只保存可复现资料：命令、配置片段、关键日志、截图、问题记录和总结。
