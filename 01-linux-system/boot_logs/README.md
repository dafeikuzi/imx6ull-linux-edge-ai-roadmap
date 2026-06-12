# Boot Logs

这里保存阶段 1 的关键串口日志。

## 建议文件

- `uboot.log`：U-Boot banner、版本、`printenv`、`bootcmd`、`bootargs`。
- `kernel.log`：Kernel 从 `Starting kernel ...` 到进入 shell 的完整日志。
- `nfs-boot.log`：可选，NFS rootfs 启动日志片段。

## 每份日志开头建议补充

```text
Date:
Board:
Ubuntu version:
BSP version:
U-Boot version:
Kernel version:
RootFS source:
Toolchain:
Boot media:
Ubuntu IP:
Board IP:
Serial setting: 115200 8N1
```

## 重点标注

- U-Boot：`bootcmd`、`bootargs`、`ipaddr`、`serverip`。
- Kernel：`Kernel command line`、MMC、Ethernet、rootfs mount、init。
- NFS：IP 配置、NFS mount、rootfs 路径、错误码。
