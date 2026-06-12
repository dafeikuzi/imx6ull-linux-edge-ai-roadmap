# Day 4 SD/eMMC RootFS 记录

日期：2026-06-10

日志来源：

```text
用户粘贴的 Day 4 板端串口输出。
```

## 1. Day 3 已确认前置条件

```text
Kernel: Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
Kernel command line: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
RootFS partition: /dev/mmcblk1p2
Filesystem: ext4
Device tree model: Freescale i.MX6 ULL 14x14 EVK Board
eth0 link: 100Mbps/full duplex
```

## 2. RootFS 基本信息

待填写：

```text
RootFS source: 100ask Buildroot system
Boot media: eMMC/MMC
Root partition: /dev/mmcblk1p2
Filesystem: ext4
Mount mode: rw
Init system: Buildroot init scripts under /etc/init.d, linuxrc exists at /
BusyBox version: `busybox` command is not in PATH or not installed as standalone command
Login user: root
```

命令：

```bash
uname -a
cat /proc/cmdline
cat /etc/os-release 2>/dev/null || true
busybox | head -3
```

输出：

```text
[root@100ask:~]# uname -a
Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux

[root@100ask:~]# cat /proc/cmdline
console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw

[root@100ask:~]# cat /etc/os-release 2>/dev/null || true
NAME=Buildroot
VERSION=2020.02-g65177d4
ID=buildroot
VERSION_ID=2020.02
PRETTY_NAME="Buildroot 2020.02"

[root@100ask:~]# busybox | head -3
-bash: busybox: command not found
```

## 3. 挂载点与空间

命令：

```bash
mount
df -h
cat /proc/mounts
```

需要摘录：

```text
/ mount source: /dev/root
/ filesystem: ext4
/ mount mode: rw
/ free space: 691M available out of 1.5G, 50% used
/proc mounted: yes, proc on /proc type proc (rw,relatime)
/sys mounted: yes, sysfs on /sys type sysfs (rw,relatime)
/dev mounted: yes, devtmpfs on /dev type devtmpfs (rw,relatime,size=86120k,nr_inodes=21530,mode=755)
/tmp mounted: yes, tmpfs on /tmp type tmpfs (rw,relatime)
```

输出：

```text
[root@100ask:~]# mount
/dev/root on / type ext4 (rw,relatime,data=ordered)
devtmpfs on /dev type devtmpfs (rw,relatime,size=86120k,nr_inodes=21530,mode=755)
sysfs on /sys type sysfs (rw,relatime)
proc on /proc type proc (rw,relatime)
devpts on /dev/pts type devpts (rw,relatime,gid=5,mode=620,ptmxmode=666)
tmpfs on /dev/shm type tmpfs (rw,relatime,mode=777)
tmpfs on /tmp type tmpfs (rw,relatime)
tmpfs on /run type tmpfs (rw,nosuid,nodev,relatime,mode=755)
debugfs on /sys/kernel/debug type debugfs (rw,relatime)
none on /sys/kernel/config type configfs (rw,relatime)
adb on /dev/usb-ffs/adb type functionfs (rw,relatime)

[root@100ask:~]# df -h
Filesystem      Size  Used Avail Use% Mounted on
/dev/root       1.5G  691M  691M  50% /
devtmpfs         85M     0   85M   0% /dev
tmpfs           245M     0  245M   0% /dev/shm
tmpfs           245M   68K  245M   1% /tmp
tmpfs           245M  496K  245M   1% /run

[root@100ask:~]# cat /proc/mounts
/dev/root / ext4 rw,relatime,data=ordered 0 0
devtmpfs /dev devtmpfs rw,relatime,size=86120k,nr_inodes=21530,mode=755 0 0
sysfs /sys sysfs rw,relatime 0 0
proc /proc proc rw,relatime 0 0
devpts /dev/pts devpts rw,relatime,gid=5,mode=620,ptmxmode=666 0 0
tmpfs /dev/shm tmpfs rw,relatime,mode=777 0 0
tmpfs /tmp tmpfs rw,relatime 0 0
tmpfs /run tmpfs rw,nosuid,nodev,relatime,mode=755 0 0
debugfs /sys/kernel/debug debugfs rw,relatime 0 0
none /sys/kernel/config configfs rw,relatime 0 0
adb /dev/usb-ffs/adb functionfs rw,relatime 0 0
```

## 4. RootFS 目录结构

命令：

```bash
ls -al /
ls -al /bin /sbin /etc /lib /dev /proc /sys /tmp /root
```

摘录：

```text
/bin: exists
/sbin: exists
/etc: exists
/lib: exists
/dev: mounted devtmpfs
/proc: mounted procfs
/sys: mounted sysfs
/tmp: mounted tmpfs, world-writable sticky directory
/root: exists, root private directory
```

Root directory excerpt:

```text
drwxr-xr-x   2 root root  4096 Jan  1 00:01 bin
drwxr-xr-x   2 root root  4096 Jan  1  1970 boot
drwxr-xr-x  13 root root  3360 Jan  1 00:19 dev
drwxr-xr-x  25 root root  4096 Jan  1 00:01 etc
drwxrwxr-x   3 root root  4096 Jan  1  1970 home
drwxr-xr-x  11 root root  4096 Jan  1 00:01 lib
lrwxrwxrwx   1 root root     3 Jul 21  2024 lib32 -> lib
-rwxr-xr-x   1 root root  5408 Jul 21  2024 linuxrc
drwxr-xr-x   2 root root  4096 Oct 20  2022 mnt
drwxr-xr-x   2 root root  4096 Oct 20  2022 opt
dr-xr-xr-x 114 root root     0 Jan  1 00:00 proc
drwx------   3 root root  4096 Jan  1 00:30 root
drwxr-xr-x   6 root root   300 Jan  1 00:19 run
drwxr-xr-x   2 root root  4096 Jul 21  2024 sbin
dr-xr-xr-x  13 root root     0 Jan  1 00:51 sys
drwxrwxrwt   8 root root   260 Jan  1 00:19 tmp
drwxr-xr-x  10 root root  4096 Jul 21  2024 usr
drwxr-xr-x   6 root root  4096 Jul 21  2024 var
```

## 5. init 和服务

命令：

```bash
ps
ls -al /etc/init.d
```

摘录：

```text
init process: not checked in this output; /linuxrc exists
network service: /etc/network/interfaces exists and configures eth0 static
ssh/dropbear: not checked in this output
telnet: not checked in this output
mosquitto: not checked in this output
other important services: not checked in this output
```

## 6. 网络状态

命令：

```bash
ifconfig -a
route -n
cat /etc/network/interfaces
ping -c 4 192.168.77.1
ping -c 4 192.168.77.132
```

期望：

```text
eth0 = 192.168.77.200/24
gateway = 192.168.77.1
Ubuntu VM = 192.168.77.132
ping Ubuntu success
```

输出：

```text
eth0:
  MAC: 00:01:3F:2D:3E:4D
  IPv4: 192.168.77.200/24
  State: UP BROADCAST RUNNING MULTICAST
  RX/TX: packets increasing, no errors

eth1:
  MAC: 00:01:1F:2D:3E:4D
  No IPv4 address
  RX/TX: 0 packets
  Configured as manual in /etc/network/interfaces

route -n:
  default via 192.168.77.1 dev eth0
  192.168.77.0/24 dev eth0

/etc/network/interfaces:
  auto lo
  iface lo inet loopback

  auto eth0
  iface eth0 inet static
      address 192.168.77.200
      netmask 255.255.255.0
      gateway 192.168.77.1

  iface eth1 inet manual

ping Windows Realtek 192.168.77.1:
  4 packets transmitted, 4 packets received, 0% packet loss
  round-trip min/avg/max = 1.750/1.828/1.968 ms

ping Ubuntu VM 192.168.77.132:
  4 packets transmitted, 4 packets received, 0% packet loss
  round-trip min/avg/max = 1.924/2.451/3.701 ms
```

## 7. 调试工具检查

命令：

```bash
for t in sh ifconfig ip ssh scp dropbear gdbserver mosquitto_pub mosquitto_sub sqlite3 vi tar wget tftp; do
  printf "%-16s " "$t"
  command -v "$t" || echo "missing"
done
```

输出：

```text
sh               /bin/sh
ifconfig         /sbin/ifconfig
ip               /sbin/ip
ssh              /usr/bin/ssh
scp              /usr/bin/scp
dropbear         missing
gdbserver        /usr/bin/gdbserver
mosquitto_pub    /usr/bin/mosquitto_pub
mosquitto_sub    /usr/bin/mosquitto_sub
sqlite3          /usr/bin/sqlite3
vi               /bin/vi
tar              /bin/tar
wget             /usr/bin/wget
tftp             /usr/bin/tftp
```

解释：

```text
这段输出来自 `command -v <tool>`。
如果输出路径，说明该工具存在并且当前 shell 可以找到。
如果输出 missing，说明该工具不存在或不在 PATH 中。

当前缺失:
  dropbear

当前可用:
  shell: sh
  network/file transfer: ifconfig, ip, ssh, scp, wget, tftp
  debug: gdbserver
  MQTT: mosquitto_pub, mosquitto_sub
  database: sqlite3
  editor/archive: vi, tar
```

## 8. Day 4 结论

待完成后填写：

```text
RootFS 是否从 /dev/mmcblk1p2 启动: 是。bootargs 指向 /dev/mmcblk1p2，Kernel 日志确认 mmcblk1p2 ext4 挂载，运行时 / 显示为 /dev/root。
RootFS 是否 rw 挂载: 是。mount 和 /proc/mounts 均显示 / 为 ext4 rw。
剩余空间是否充足: 当前 / 分区 1.5G，已用 691M，可用 691M，50% 使用率，足够 Day 6 hello 和日志实验。
网络配置是否持久: 是。/etc/network/interfaces 已写入 eth0 静态 IP 192.168.77.200/24 和 gateway 192.168.77.1。
eth0 是否能 ping 通 Ubuntu: 是。ping 192.168.77.132 4/4 成功，0% packet loss。
后续 Day 5 NFS 前置是否满足: 基本满足。Linux 阶段网络、默认路由和 Ubuntu 连通性都已确认。
后续 Day 6 hello 部署方式: 可通过网络或后续 NFS 放入板端；当前 / 分区空间足够。
缺失工具: dropbear 缺失；busybox 作为独立命令不可用。ssh/scp/gdbserver/mosquitto_pub/mosquitto_sub/sqlite3/vi/tar/wget/tftp 均可用。
```
