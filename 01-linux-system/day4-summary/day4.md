# Day 4 工作记录：RootFS 与 SD/eMMC 启动

日期：2026-06-10

阶段：`01-linux-system` / Day 4 RootFS 与 SD/eMMC 启动

## 1. Day 3 收口总结

Day 3 已完成 Kernel 与设备树运行态验证。

已确认：

```text
Kernel image: /boot/zImage
DTB: /boot/100ask_imx6ull-14x14.dtb
Kernel: Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
Kernel command line: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
Device tree model: Freescale i.MX6 ULL 14x14 EVK Board
Device tree compatible: fsl,imx6ull-14x14-evk; fsl,imx6ull
RootFS mount: /dev/mmcblk1p2 mounted as ext4
Ethernet: eth0 link up at 100Mbps/full duplex
User space: [root@100ask:~]# shell is available
```

Day 3 结论：

```text
U-Boot 能加载 Kernel 和 DTB。
Kernel 能识别 MMC/eMMC、FEC 网卡、串口和设备树。
Kernel 能从 /dev/mmcblk1p2 挂载 ext4 RootFS 并进入 Buildroot 用户态。
```

Day 3 遗留：

```text
Kernel/DTB 编译验证尚未执行，作为可选增强项，不阻塞 Day 4。
```

## 2. 今日目标

Day 4 从 Kernel 继续向用户态推进，目标是确认当前 SD/eMMC RootFS 本身是否稳定、可用、可解释：

- 确认当前 RootFS 来源和挂载方式。
- 理解 RootFS 目录结构和 init/服务启动结果。
- 检查磁盘空间、挂载点、网络配置和常用工具。
- 判断后续 Day 5 NFS 和 Day 6 交叉编译 hello 需要补哪些工具。

## 3. 当前已知 RootFS 信息

来自 Day 3：

```text
Boot media: eMMC/MMC block device
Root partition: /dev/mmcblk1p2
Filesystem: ext4
Kernel bootargs: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
RootFS type: 100ask Buildroot system
Login user: root
Password: empty
```

Kernel 挂载证据：

```text
[    4.061494] EXT4-fs (mmcblk1p2): mounted filesystem with ordered data mode. Opts: (null)
[    4.069915] VFS: Mounted root (ext4 filesystem) on device 179:2.
[    4.323888] EXT4-fs (mmcblk1p2): re-mounted. Opts: data=ordered
```

## 4. 板端执行命令

以下命令在 IMX6ULL 串口 shell 中执行：

```text
[root@100ask:~]#
```

### 4.1 基础系统信息

```bash
uname -a
cat /proc/cmdline
cat /etc/os-release 2>/dev/null || true
busybox | head -3
```

### 4.2 RootFS 挂载与空间

```bash
mount
df -h
cat /proc/mounts
ls -al /
ls -al /bin /sbin /etc /lib /dev /proc /sys /tmp /root
```

重点看：

```text
/ 是否来自 /dev/root 或 /dev/mmcblk1p2
/ 是否是 rw
/proc、/sys、/dev、/tmp 是否正常挂载
剩余空间是否足够后续放 hello、日志和临时文件
```

### 4.3 init 和服务

```bash
ps
ls -al /etc/init.d
for s in /etc/init.d/S*; do echo "== $s =="; done
```

重点看是否有：

```text
S40network
sshd 或 dropbear
telnetd
mosquitto
```

### 4.4 网络状态

```bash
ifconfig -a
route -n
cat /etc/network/interfaces
ping -c 4 192.168.77.1
ping -c 4 192.168.77.132
```

当前期望：

```text
eth0 = 192.168.77.200/24
Windows Realtek = 192.168.77.1/24
Ubuntu ens33 = 192.168.77.132/24
eth0 ping Windows 和 Ubuntu 成功
eth1 不参与当前直连网络
```

### 4.5 后续调试工具检查

```bash
which sh
which ifconfig
which ip
which ssh
which scp
which dropbear
which gdbserver
which mosquitto_pub
which mosquitto_sub
which sqlite3
which vi
which tar
which wget
which tftp
```

如果 `which` 不存在，使用：

```bash
command -v <tool>
```

这些工具不要求今天全部具备。Day 4 只记录现状：

```text
有网络工具 -> Day 5 NFS 更顺滑。
有 scp/ssh/dropbear -> 后续传文件更方便。
有 gdbserver -> 后续调试 C 程序更方便。
有 mosquitto_pub/sub -> 后续 MQTT 阶段更方便。
```

## 5. 今日证据保存

把串口输出保存或粘贴到：

```text
01-linux-system/rootfs_notes/day4-sd-rootfs.md
```

建议截图：

```text
01-linux-system/images/day4-rootfs-check.png
```

截图内容优先包含：

```text
cat /proc/cmdline
mount | head
df -h
ifconfig eth0
ping -c 4 192.168.77.132
```

## 6. 当前完成标准

- [x] 填写 `rootfs_notes/day4-sd-rootfs.md`。
- [x] 记录 `cat /proc/cmdline`。
- [x] 记录 `/` 的挂载来源、文件系统类型和读写模式。
- [x] 记录 `df -h`。
- [x] 记录 `/etc/network/interfaces`。
- [x] 复测 eth0 ping Ubuntu `192.168.77.132`。
- [x] 记录当前已存在/缺失的完整调试工具清单。`dropbear` 缺失，`ssh/scp/gdbserver/mosquitto_pub/mosquitto_sub/sqlite3/vi/tar/wget/tftp` 可用。

## 7. Day 4 结束后应能回答

```text
1. 当前 RootFS 放在哪里？
2. Kernel 是通过哪个 bootargs 找到 RootFS 的？
3. RootFS 是否以 rw 方式挂载？
4. Buildroot RootFS 里有哪些关键目录？
5. 当前网络配置是否持久？
6. 后续 NFS/hello/MQTT 还缺哪些用户态工具？
```

## 8. 下一步预告

Day 4 完成后进入 Day 5：

```text
准备 Ubuntu NFS 服务。
把 RootFS 解压到 /nfs/imx6ull-rootfs。
临时设置 U-Boot bootargs，让 Kernel 从 root=/dev/nfs 启动。
验证板端创建文件后 Ubuntu NFS 目录可见。
```

## 9. 当前 Day 4 证据摘要

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

判断：

```text
当前 SD/eMMC RootFS 稳定可用，且网络配置已经持久化。
Day 5 NFS 的网络前置条件已经满足。
```
