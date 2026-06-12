# Day 3 Kernel 与设备树记录

日期：2026-06-10

日志来源：

```text
01-linux-system/boot_logs/kernel.log
```

当前状态：

```text
Day 3 核心证据已填充。
当前 kernel.log 来自用户粘贴的串口/dmesg 片段，能证明 MMC、RootFS、FEC 网卡和设备树信息。
该片段从 [0.535604] 开始，缺少更早的 `Linux version ...` 和 `Kernel command line: ...` 原始行。
```

## 1. 已知启动文件

来自 U-Boot `printenv` 和启动流程分析：

```text
Kernel image expected by U-Boot: /boot/zImage
DTB expected by U-Boot: /boot/100ask_imx6ull-14x14.dtb
U-Boot loadaddr: 0x80800000
U-Boot fdt_addr: 0x83000000
Boot command: bootz
RootFS: /dev/mmcblk1p2 rootwait rw
```

## 2. Kernel 编译记录

本次 Day 3 先确认当前板端实际启动链路，尚未替换或重编译 Kernel。

```text
Kernel source: not checked in this repo; use external 100ask BSP Kernel source when compiling
Defconfig: pending BSP documentation/source confirmation
Toolchain: arm-linux-gnueabihf-
Build command: pending
Build result: not run in this turn
zImage output path: expected arch/arm/boot/zImage after build
dtb output path: expected arch/arm/boot/dts/<board>.dtb after build
```

推荐命令：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- <board_defconfig>
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage dtbs -j$(nproc)
```

## 3. Kernel 启动信息

从 `boot_logs/kernel.log` 和 Day 2 U-Boot 证据摘录：

```text
Kernel version: Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
Kernel command line: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
Machine model: Freescale i.MX6 ULL 14x14 EVK Board
Device-tree compatible: fsl,imx6ull-14x14-evk; fsl,imx6ull
```

已知当前 bootargs 生成方式：

```text
mmcargs=setenv bootargs console=${console},${baudrate} root=${mmcroot}
mmcroot=/dev/mmcblk1p2 rootwait rw
```

因此预期 Kernel command line 是：

```text
console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

板端实测：

```bash
uname -a
```

输出：

```text
Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
```

板端实测：

```bash
cat /proc/cmdline
```

输出：

```text
console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

这与 Day 2 从 U-Boot `mmcargs` 推导出的结果一致。

## 4. RootFS 挂载证据

摘录：

```text
Root device: /dev/mmcblk1p2
Filesystem type: ext4
VFS/EXT4 mount log:
  [    4.030517] EXT4-fs (mmcblk1p2): couldn't mount as ext3 due to feature incompatibilities
  [    4.061494] EXT4-fs (mmcblk1p2): mounted filesystem with ordered data mode. Opts: (null)
  [    4.069915] VFS: Mounted root (ext4 filesystem) on device 179:2.
  [    4.323888] EXT4-fs (mmcblk1p2): re-mounted. Opts: data=ordered
Init/login evidence:
  [root@100ask:~]# shell prompt is available, proving user space is running.
```

当前预期：

```text
RootFS 来自 /dev/mmcblk1p2，不是 NFS。
能出现 Buildroot 登录界面，说明 Kernel 已经成功挂载 RootFS 并进入用户态。
```

## 5. MMC/SD/eMMC 证据

从 Kernel 日志摘录：

```text
[    3.244810] mmc0: SDHCI controller on 2190000.usdhc [2190000.usdhc] using ADMA
[    3.324379] mmc1: SDHCI controller on 2194000.usdhc [2194000.usdhc] using ADMA
[    3.447746] mmc1: new DDR MMC card at address 0001
[    3.464438] mmcblk1: mmc1:0001 8GTF4R 7.28 GiB
[    3.529620]  mmcblk1: p1 p2 p3
```

记录模板：

```text
MMC controller log: mmc0 and mmc1 initialized by SDHCI/usdhc drivers
Detected block device: mmcblk1, 8GTF4R, 7.28 GiB
Root partition: mmcblk1p2
```

## 6. 网卡证据

Day 2 已确认 Linux 阶段当前可用有线链路是：

```text
eth0 = 192.168.77.200/24
Ubuntu ens33 = 192.168.77.132/24
Windows Realtek = 192.168.77.1/24
eth1 = no carrier / disabled
```

Day 3 Kernel 日志确认 FEC/PHY 初始化：

```text
FEC driver log:
  [    2.421184] fec 20b4000.ethernet eth0: registered PHC device 0
  [    2.493526] fec 2188000.ethernet eth1: registered PHC device 1
PHY driver log:
  [    2.315591] libphy: Fixed MDIO Bus: probed
  [    2.402274] libphy: fec_enet_mii_bus: probed
  [    7.808337] Generic PHY 20b4000.ethernet-1:01: attached PHY driver [Generic PHY] (mii_bus:phy_addr=20b4000.ethernet-1:01, irq=-1)
  [    9.418720] Generic PHY 20b4000.ethernet-1:00: attached PHY driver [Generic PHY] (mii_bus:phy_addr=20b4000.ethernet-1:00, irq=-1)
eth0 link log:
  [    7.820801] IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
  [    9.894816] fec 20b4000.ethernet eth0: Link is Up - 100Mbps/Full - flow control rx/tx
  [    9.902719] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
eth1 link log:
  [    9.407513] IPv6: ADDRCONF(NETDEV_UP): eth1: link is not ready
  [    9.431233] IPv6: ADDRCONF(NETDEV_UP): eth1: link is not ready
```

板端验证命令：

```bash
ifconfig -a
route -n
ping -c 4 192.168.77.132
dmesg | grep -Ei "fec|eth|phy|link"
```

## 7. 设备树观察

板端命令：

```bash
ls /proc/device-tree
tr -d '\0' < /proc/device-tree/model; echo
tr -d '\0' < /proc/device-tree/compatible; echo
```

待填写：

```text
/proc/device-tree/model:
Freescale i.MX6 ULL 14x14 EVK Board

/proc/device-tree/compatible:
fsl,imx6ull-14x14-evk
fsl,imx6ull
```

解释：

```text
DTB 是 U-Boot 传给 Kernel 的硬件描述。
Kernel 根据 DTB 里的 compatible、reg、interrupts、pinctrl 等信息匹配和初始化驱动。
```

## 8. Day 3 结论

```text
是否保存 kernel.log: 已保存用户粘贴的 Day 3 摘录到 boot_logs/kernel.log；建议后续再补完整冷启动串口日志。
Kernel 是否成功启动: 是。串口 shell 已进入 [root@100ask:~]#。
DTB 是否匹配当前板卡: 基本匹配。设备树 model 为 Freescale i.MX6 ULL 14x14 EVK Board，compatible 包含 fsl,imx6ull-14x14-evk 和 fsl,imx6ull。
RootFS 是否从 /dev/mmcblk1p2 挂载: 是。EXT4 和 VFS 日志确认 mmcblk1p2 已作为 ext4 rootfs 挂载。
eth0 是否仍能 ping 通 Ubuntu: 本次粘贴未包含 ping 输出；但 Kernel 日志确认 eth0 link up，Day 2 已确认 eth0 能 ping 通 Ubuntu 192.168.77.132。
是否完成 Kernel 编译: 否。本次先做启动证据确认，未在 Ubuntu BSP Kernel 源码中执行 zImage dtbs 编译。
遗留问题: 可选继续做 Kernel/DTB 编译验证。
```

## 9. 操作失误记录

本次板端先输入了错误命令：

```bash
name -a
```

系统返回：

```text
-bash: name: command not found
```

原因：

```text
正确命令是 `uname -a`，不是 `name -a`。
```
