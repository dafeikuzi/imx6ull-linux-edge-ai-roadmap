# NFS RootFS Notes

目标：让 IMX6ULL 使用 Ubuntu 虚拟机中的 `/nfs/imx6ull-rootfs` 作为根文件系统启动，方便快速调试用户态程序和配置文件。

## 网络约定

建议虚拟机 Ubuntu 使用桥接网络，并给 Ubuntu 和开发板分配同一网段的固定 IP。

```text
Ubuntu IP: 192.168.77.132
Board IP: 192.168.77.200
Gateway: 192.168.77.1
Netmask: 255.255.255.0
Ubuntu interface: ens33
NFS export: /nfs/imx6ull-rootfs
```

检查 Ubuntu 网络：

```bash
ip addr
ip route
ping <BOARD_IP>
```

检查 U-Boot 网络：

```text
printenv ipaddr
printenv serverip
ping <Ubuntu_IP>
```

## PC 侧

```bash
sudo apt install nfs-kernel-server
sudo mkdir -p /nfs/imx6ull-rootfs
sudo chown -R root:root /nfs/imx6ull-rootfs
sudo mkdir -p /etc/exports.d
echo "/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)" | sudo tee /etc/exports.d/imx6ull-rootfs.exports
sudo exportfs -ra
sudo systemctl restart nfs-kernel-server
```

确认导出结果：

```bash
sudo exportfs -v
showmount -e 127.0.0.1
```

把 BSP rootfs 解压到 NFS 目录时，注意保留权限和设备节点：

```bash
sudo tar -xpf <rootfs.tar.*> -C /nfs/imx6ull-rootfs
sync
```

## U-Boot bootargs 模板

```text
console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.77.132:/nfs/imx6ull-rootfs,v3,tcp ip=192.168.77.200:192.168.77.132:192.168.77.1:255.255.255.0::eth0:off rw
```

把 `<PC_IP>`、`<BOARD_IP>`、`<GATEWAY_IP>` 替换成真实网络环境。

常用 U-Boot 设置示例：

```text
setenv ipaddr 192.168.77.200
setenv serverip 192.168.77.132
setenv gatewayip 192.168.77.1
setenv netmask 255.255.255.0
setenv bootargs 'console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.77.132:/nfs/imx6ull-rootfs,v3,tcp ip=192.168.77.200:192.168.77.132:192.168.77.1:255.255.255.0::eth0:off rw'
run bootcmd
```

注意：第一次测试不要执行 `saveenv`。确认 NFS 启动成功后，再决定是否固化环境变量。

## 板端验证

启动进入 shell 后执行：

```bash
cat /proc/cmdline
mount
ip addr
touch /hello-from-board
```

Ubuntu 侧确认：

```bash
ls -l /nfs/imx6ull-rootfs/hello-from-board
```

如果能看到板端创建的文件，说明 NFS rootfs 已经真实挂载成功。

## 日志记录模板

```text
Date:
Ubuntu version:
Kernel version:
RootFS source:
Toolchain:
Ubuntu IP: 192.168.77.132
Board IP: 192.168.77.200
Gateway: 192.168.77.1
Boot media:
U-Boot bootargs:

Key boot log:

Board verification commands:

Problems and fixes:
```

## 常见问题

| 现象 | 优先检查 |
|---|---|
| U-Boot ping 不通 Ubuntu | 虚拟机是否桥接、Windows/Ubuntu 防火墙、网线、同网段 IP |
| Kernel 提示 unable to mount root fs | `root=/dev/nfs`、`nfsroot=` 路径、Kernel 是否启用 NFS root |
| NFS permission denied | `/etc/exports`、`no_root_squash`、`exportfs -ra`、目录权限 |
| 板端启动后修改不可见 | 可能实际仍从 SD rootfs 启动，检查 `/proc/cmdline` 和 `mount` |
