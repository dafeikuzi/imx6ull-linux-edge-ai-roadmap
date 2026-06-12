# Day 5 工作记录：NFS RootFS

日期：2026-06-10

阶段：`01-linux-system` / Day 5 NFS RootFS

## 1. Day 4 完成总结

Day 4 已完成 SD/eMMC RootFS 验证。

核心证据：

```text
OS: Buildroot 2020.02-g65177d4
Kernel: Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
/ mount: /dev/root on / type ext4 (rw,relatime,data=ordered)
Disk: /dev/root 1.5G total, 691M used, 691M available, 50% used
Network: eth0 192.168.77.200/24, default gateway 192.168.77.1
Windows ping: 192.168.77.1 4/4 success
Ubuntu ping: 192.168.77.132 4/4 success
Tools: ssh/scp/gdbserver/mosquitto_pub/mosquitto_sub/sqlite3/vi/tar/wget/tftp available; dropbear missing
```

Day 4 结论：

```text
当前 SD/eMMC RootFS 稳定可用。
RootFS 以 rw 方式挂载，空间足够后续实验。
板端 eth0 静态网络已经持久化。
Linux 阶段板端到 Ubuntu 的网络已经满足 Day 5 NFS 前置条件。
```

## 2. 今日目标

Day 5 的目标是让 IMX6ULL 使用 Ubuntu 虚拟机中的目录作为 NFS RootFS。

完成后应能证明：

```text
Kernel command line 包含 root=/dev/nfs。
Kernel 从 192.168.77.132:/nfs/imx6ull-rootfs 挂载根文件系统。
板端创建的 /hello-from-board 能在 Ubuntu 的 /nfs/imx6ull-rootfs/hello-from-board 看到。
```

## 3. 当前网络拓扑

```text
IMX6ULL eth0: 192.168.77.200/24
Windows Realtek: 192.168.77.1/24
Ubuntu VM ens33: 192.168.77.132/24
VMware VMnet0: bridged to Realtek PCIe GbE Family Controller
NFS export: /nfs/imx6ull-rootfs
```

注意：

```text
Day 5 依赖 Linux Kernel 阶段 eth0 网络。
U-Boot 阶段 ping 未作为必须项解决，不阻塞 Kernel 挂载 NFS RootFS。
```

## 4. Ubuntu 侧准备 NFS 服务

以下命令在 Ubuntu 虚拟机执行，不是在板端执行。

确认 Ubuntu IP：

```bash
ip -br addr show ens33
```

期望：

```text
ens33 UP 192.168.77.132/24
```

安装 NFS 服务：

```bash
sudo apt update
sudo apt install -y nfs-kernel-server
```

准备导出目录：

```bash
sudo mkdir -p /nfs/imx6ull-rootfs
sudo chown -R root:root /nfs/imx6ull-rootfs
```

配置 `/etc/exports`：

```bash
echo "/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)" | sudo tee /etc/exports.d/imx6ull-rootfs.exports
sudo exportfs -ra
sudo systemctl restart nfs-kernel-server
```

检查导出：

```bash
sudo exportfs -v
showmount -e 127.0.0.1
```

## 5. 准备 NFS RootFS 内容

推荐来源：

```text
优先使用韦东山 BSP 提供的 rootfs 压缩包。
如果暂时找不到压缩包，可以先从当前 SD/eMMC RootFS 拷贝，但要注意设备节点、权限和伪文件系统。
```

当前在 Windows 资料目录中已找到可用 rootfs 压缩包：

```text
推荐使用:
E:\韦东山Linux嵌入式\开发板资料\02_100ask_imx6ull_pro_2022.08\03_开发板系统固件\Buildroot_image\rootfs.tar.bz2
Size: 140921733 bytes

暂不优先:
E:\韦东山Linux嵌入式\开发板资料\02_100ask_imx6ull_pro_2022.08\03_开发板系统固件\Buildroot_OTA(支持升级的EMMC系统镜像)\rootfs.tar.bz2
E:\韦东山Linux嵌入式\开发板资料\02_100ask_imx6ull_pro_2022.08\03_开发板系统固件\Buildroot_OTA(支持升级的EMMC系统镜像)\rootfs.cpio.gz
E:\韦东山Linux嵌入式\开发板资料\02_100ask_imx6ull_pro_2022.08\03_开发板系统固件\第1个项目_电子产品量产测试与烧写\sdcard_rootfs.tar.bz2
```

选择理由：

```text
`Buildroot_image/rootfs.tar.bz2` 是普通 Buildroot rootfs 包，最适合先做 Day 5 NFS RootFS 验证。
OTA 目录里的 rootfs 包和 cpio 包更偏 EMMC OTA/升级系统镜像。
量产测试目录里的 sdcard_rootfs.tar.bz2 更偏烧写/测试项目，不作为 Day 5 首选。
```

如果有 rootfs tar 包：

```bash
sudo tar -xpf <rootfs.tar.*> -C /nfs/imx6ull-rootfs
sync
```

检查关键目录：

```bash
sudo ls -al /nfs/imx6ull-rootfs
sudo test -x /nfs/imx6ull-rootfs/linuxrc && echo linuxrc-ok
sudo test -d /nfs/imx6ull-rootfs/bin && echo bin-ok
sudo test -d /nfs/imx6ull-rootfs/etc && echo etc-ok
```

## 6. U-Boot 临时 NFS bootargs

进入 U-Boot `=>` 后，先只做临时设置，不执行 `saveenv`。

```text
setenv serverip 192.168.77.132
setenv ipaddr 192.168.77.200
setenv gatewayip 192.168.77.1
setenv netmask 255.255.255.0
setenv bootargs 'console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.77.132:/nfs/imx6ull-rootfs,v3,tcp ip=192.168.77.200:192.168.77.132:192.168.77.1:255.255.255.0::eth0:off rw'
run bootcmd
```

说明：

```text
不执行 saveenv，失败后断电重启仍回到 SD/eMMC RootFS。
这里仍复用当前 bootcmd 加载 /boot/zImage 和 /boot/100ask_imx6ull-14x14.dtb，只改变 Kernel 的 rootfs 来源。
```

如果 `run bootcmd` 重新生成了 SD bootargs 并覆盖手动设置，则改用手动加载方式，具体以当前 U-Boot 环境为准：

```text
mmc dev 1
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /boot/100ask_imx6ull-14x14.dtb
bootz 0x80800000 - 0x83000000
```

## 7. 板端 NFS 启动后验证

如果成功进入 shell，执行：

```bash
cat /proc/cmdline
mount
df -h
ifconfig eth0
touch /hello-from-board
ls -l /hello-from-board
```

Ubuntu 侧验证：

```bash
ls -l /nfs/imx6ull-rootfs/hello-from-board
```

成功标准：

```text
板端 /proc/cmdline 包含 root=/dev/nfs。
mount 中 / 的来源是 192.168.77.132:/nfs/imx6ull-rootfs 或类似 NFS 记录。
Ubuntu 能看到板端创建的 hello-from-board。
```

## 8. 当前完成标准

- [ ] Ubuntu `ens33` 确认为 `192.168.77.132/24`。
- [x] NFS 服务安装并运行。
- [x] `/etc/exports.d/imx6ull-rootfs.exports` 配置完成。
- [x] `exportfs -v` 和 `showmount -e 127.0.0.1` 正常。
- [x] `/nfs/imx6ull-rootfs` 已放入可启动 RootFS 内容。
- [x] U-Boot 临时 NFS bootargs 已尝试。
- [x] 板端从 NFS RootFS 进入 shell。
- [x] 板端 `touch /hello-from-board` 后 Ubuntu 侧可见。

## 9. 常见失败与判断

```text
卡在 VFS unable to mount root fs:
  检查 nfsroot 路径、exports、RootFS 内容是否完整。

NFS permission denied:
  检查 /etc/exports.d/imx6ull-rootfs.exports 是否包含 no_root_squash，执行 sudo exportfs -ra。

Kernel 找不到网络:
  检查 bootargs 里的 ip=...::eth0:off，确认 eth0 是当前有线链路。

启动后仍是 /dev/root ext4:
  说明实际仍从 SD/eMMC RootFS 启动，检查 /proc/cmdline 和 U-Boot bootargs 是否被 bootcmd 覆盖。
```

## 10. 当前故障记录

Ubuntu 执行导出配置时出现：

```text
tee: /etc/exports.d/imx6ull-rootfs.exports: No such file or directory
/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)
Export list for 127.0.0.1:
```

判断：

```text
/etc/exports.d/ 目录不存在，导致 tee 无法创建 imx6ull-rootfs.exports。
showmount 只有表头、没有导出条目，说明 NFS export 尚未生效。
```

修复命令二选一。

方案 A：创建 `/etc/exports.d` 目录后继续使用单独配置文件：

```bash
sudo mkdir -p /etc/exports.d
echo "/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)" | sudo tee /etc/exports.d/imx6ull-rootfs.exports
sudo exportfs -ra
sudo systemctl restart nfs-kernel-server
sudo exportfs -v
showmount -e 127.0.0.1
```

方案 B：直接写入 `/etc/exports`：

```bash
echo "/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)" | sudo tee -a /etc/exports
sudo exportfs -ra
sudo systemctl restart nfs-kernel-server
sudo exportfs -v
showmount -e 127.0.0.1
```

期望 `showmount` 输出包含：

```text
Export list for 127.0.0.1:
/nfs/imx6ull-rootfs 192.168.77.0/24
```

## 11. NFS RootFS 内容检查

Ubuntu 侧已解压推荐的 `Buildroot_image/rootfs.tar.bz2` 到：

```text
/nfs/imx6ull-rootfs
```

关键检查结果：

```text
linuxrc-ok
bin-ok
etc-ok
```

判断：

```text
NFS RootFS 基本目录结构有效，可以进入 U-Boot 做临时 NFS bootargs 启动测试。
```

## 12. U-Boot 手动加载错误记录

第一次手动加载 Kernel 时输入了：

```text
ext4load mmc 1:2 0x80800000 /boo
```

U-Boot 返回：

```text
** File not found /boo **
```

判断：

```text
这不是 NFS 问题，也不是 Kernel 文件缺失。
原因是路径输入被截断，正确 Kernel 路径应为 `/boot/zImage`。
```

继续执行完整命令即可：

```text
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /boot/100ask_imx6ull-14x14.dtb
bootz 0x80800000 - 0x83000000
```

## 13. NFS RootFS 启动成功证据

板端登录提示符：

```text
[root@imx6ull:~]#
```

板端 `/proc/cmdline`：

```text
console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.77.132:/nfs/imx6ull-rootfs,v3,tcp ip=192.168.77.200:192.168.77.132:192.168.77.1:255.255.255.0::eth0:off rw
```

板端 `mount` 证明 `/` 来自 NFS：

```text
192.168.77.132:/nfs/imx6ull-rootfs on / type nfs (rw,relatime,vers=3,rsize=4096,wsize=4096,namlen=255,hard,nolock,proto=tcp,timeo=600,retrans=2,sec=sys,mountaddr=192.168.77.132,mountvers=3,mountproto=tcp,local_lock=all,addr=192.168.77.132)
```

板端 `df -h`：

```text
Filesystem                Size      Used Available Use% Mounted on
192.168.77.132:/nfs/imx6ull-rootfs
                         77.7G     16.7G     57.0G  23% /
```

板端创建文件：

```text
touch /hello-from-board
ls -l /hello-from-board
-rw-r--r--    1 root     root             0 Jun 10  2026 /hello-from-board
```

Ubuntu 侧验证文件可见：

```text
ls -l /nfs/imx6ull-rootfs/hello-from-board
-rw-r--r-- 1 root root 0  6月 10 21:55 /nfs/imx6ull-rootfs/hello-from-board
```

说明：

```text
Day 5 NFS RootFS 已成功。
当前启动仍使用临时 U-Boot bootargs，没有执行 saveenv。
```

非阻塞日志：

```text
Bluetooth: hci0: Failed to load rtl_bt/rtl8723b_config.bin
udevd[196]: specified group 'kvm' unknown
```

判断：

```text
这些是 RootFS 用户态/固件配置警告，不影响 NFS RootFS 启动验证。
```

当前状态：

```text
用户已执行 `sudo mkdir -p /etc/exports.d` 并重新写入 `/etc/exports.d/imx6ull-rootfs.exports`。
该故障已修复。
`sudo exportfs -v` 和 `showmount -e 127.0.0.1` 已确认导出条目生效。
```

实际输出：

```text
/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)
/nfs/imx6ull-rootfs
        192.168.77.0/24(sync,wdelay,hide,no_subtree_check,sec=sys,rw,secure,no_root_squash,no_all_squash)
Export list for 127.0.0.1:
/nfs/imx6ull-rootfs 192.168.77.0/24
```
