# 01 Linux System

目标：构建 IMX6ULL 可启动 Linux 系统，并建立串口、SSH、NFS、交叉编译调试流程。

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
- 串口登录截图放入 `images/`
- SSH 登录截图放入 `images/`

## 常用命令模板

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
```

