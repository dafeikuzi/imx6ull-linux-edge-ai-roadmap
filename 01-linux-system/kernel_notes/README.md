# Kernel Notes

## 必须掌握

- Kernel 镜像负责操作系统核心逻辑，设备树描述板级硬件。
- IMX6ULL 的串口、MMC、网卡、GPIO 等硬件信息主要通过 `.dtb` 传给 Kernel。
- Kernel command line 通常来自 U-Boot `bootargs`，可在板端用 `cat /proc/cmdline` 查看。

## 常用编译命令

具体 defconfig 和 dtb 名称以韦东山 BSP 文档为准。

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- <board_defconfig>
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage dtbs -j$(nproc)
```

## 启动后检查

```bash
uname -a
cat /proc/cmdline
dmesg | grep -Ei "mmc|eth|fec|root|nfs|tty"
ls /proc/device-tree
```

## 笔记模板

```text
Kernel source:
Defconfig:
DTB:
Build command:
zImage path:
dtb path:
Boot method:

Kernel command line:
RootFS mount log:
Network driver log:
MMC driver log:

Problems and fixes:
```
