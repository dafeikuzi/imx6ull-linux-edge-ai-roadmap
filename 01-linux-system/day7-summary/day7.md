# Day 7 工作记录：阶段 1 复盘与面试化整理

日期：2026-06-10

阶段：`01-linux-system` / Day 7 复盘与面试化整理

## 1. 阶段 1 总结

```text
开发板: 100ask IMX6ULL_Pro
Ubuntu 版本: Ubuntu 22.04.5 LTS
串口设备: /dev/ttyACM0
串口参数: 115200 8N1
U-Boot 版本: U-Boot 2017.03 (Jun 03 2020 - 13:12:42 +0800)
Kernel 版本: Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
RootFS 来源: 100ask Buildroot 2020.02-g65177d4
交叉工具链: gcc-linaro-7.5.0-2019.12 arm-linux-gnueabihf-
SD/eMMC 启动: root=/dev/mmcblk1p2 rootwait rw
NFS 启动: root=/dev/nfs nfsroot=192.168.77.132:/nfs/imx6ull-rootfs,v3,tcp
板端 IP: 192.168.77.200/24
Ubuntu NFS IP: 192.168.77.132/24
Windows Realtek IP: 192.168.77.1/24
```

阶段 1 已完成的主线：

```text
串口连接
-> U-Boot 进入命令行并分析 bootcmd/bootargs
-> Kernel 和 DTB 启动证据确认
-> SD/eMMC RootFS 验证
-> Ubuntu NFS RootFS 启动
-> arm-linux-gnueabihf-gcc 交叉编译 hello
-> 板端运行 hello
```

## 2. 已保存证据

```text
01-linux-system/day1-summary/day1.md
01-linux-system/day2-summary/day2.md
01-linux-system/day3-summary/day3.md
01-linux-system/day4-summary/day4.md
01-linux-system/day5-summary/day5.md
01-linux-system/day6-summary/day6.md
01-linux-system/boot_logs/day1-first-boot.log
01-linux-system/boot_logs/kernel.log
01-linux-system/uboot_notes/day2-uboot-env.md
01-linux-system/kernel_notes/day3-kernel-dtb.md
01-linux-system/rootfs_notes/day4-sd-rootfs.md
01-linux-system/rootfs_notes/nfs-rootfs.md
01-linux-system/hello_cross_compile/hello.c
01-linux-system/hello_cross_compile/README.md
```

## 3. 每日成果

### Day 1 串口与首次启动

```text
Ubuntu 识别串口为 /dev/ttyACM0。
sudo picocom -b 115200 /dev/ttyACM0 成功进入 100ask Buildroot 登录界面。
```

### Day 2 U-Boot

```text
成功进入 U-Boot `=>`。
记录 version、printenv、bootcmd、mmcargs、mmcroot、bdinfo。
确认 bootargs 不是常驻变量，而是由 mmcargs 在启动时临时生成。
确认当前 bootcmd 从 mmc1:2 加载 /boot/zImage 和 /boot/100ask_imx6ull-14x14.dtb。
```

### Day 3 Kernel 与设备树

```text
Kernel: 4.9.88
cmdline: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
DTB model: Freescale i.MX6 ULL 14x14 EVK Board
DTB compatible: fsl,imx6ull-14x14-evk; fsl,imx6ull
RootFS: mmcblk1p2 ext4
eth0: Link is Up - 100Mbps/Full
```

### Day 4 SD/eMMC RootFS

```text
RootFS 以 /dev/root ext4 rw 方式挂载。
Buildroot 版本为 2020.02-g65177d4。
根分区 1.5G，总可用约 691M。
eth0 静态 IP 192.168.77.200/24 已持久化。
板端能 ping 通 Windows 192.168.77.1 和 Ubuntu 192.168.77.132。
```

### Day 5 NFS RootFS

```text
Ubuntu NFS 导出 /nfs/imx6ull-rootfs 到 192.168.77.0/24。
板端通过 root=/dev/nfs 启动。
mount 显示 / 来自 192.168.77.132:/nfs/imx6ull-rootfs。
板端 touch /hello-from-board 后 Ubuntu 侧可见同一文件。
```

### Day 6 交叉编译 hello

```text
Ubuntu 使用 arm-linux-gnueabihf-gcc 编译 hello.c。
file hello 显示 ELF 32-bit LSB executable, ARM, EABI5。
hello 复制到 /nfs/imx6ull-rootfs/root/hello。
板端从 /root/hello 运行。
输出 hello from IMX6ULL cross compile，退出码为 0。
```

## 4. 必会问答

### 4.1 U-Boot 到 Kernel 的启动流程是什么？

```text
BootROM 从启动介质加载 SPL/U-Boot。
U-Boot 初始化基础硬件，执行 bootcmd。
当前 bootcmd 从 mmc1:2 加载 /boot/zImage 和 /boot/100ask_imx6ull-14x14.dtb。
U-Boot 设置 bootargs，并通过 bootz 跳转到 Kernel。
Kernel 根据 DTB 初始化硬件，根据 bootargs 挂载 RootFS，最后启动 init 进入用户态。
```

### 4.2 bootargs 里的 console、root、ip 分别做什么？

```text
console=ttymxc0,115200:
  指定 Kernel 日志和控制台使用 IMX 串口 ttymxc0，波特率 115200。

root=/dev/mmcblk1p2:
  指定 SD/eMMC 第 2 分区作为根文件系统。

root=/dev/nfs:
  指定通过网络 NFS 挂载根文件系统。

ip=192.168.77.200:192.168.77.132:192.168.77.1:255.255.255.0::eth0:off:
  指定 NFS 启动时板端 IP、服务器 IP、网关、掩码、网口和关闭 DHCP。
```

### 4.3 为什么 NFS RootFS 适合调试？

```text
NFS RootFS 让板端把 Ubuntu 的目录挂载成自己的 /。
修改 Ubuntu /nfs/imx6ull-rootfs 里的文件后，板端立即可见。
这样调试用户态程序时不用反复烧写 SD 卡，特别适合 Day 6 hello 和后续 C/C++ 应用开发。
```

### 4.4 Kernel 和设备树分别负责什么？

```text
Kernel 是操作系统核心，负责进程、内存、文件系统、网络和驱动框架。
设备树描述板级硬件资源，比如串口、网卡、MMC、GPIO、pinctrl 和 compatible。
Kernel 根据设备树匹配并初始化对应驱动。
```

### 4.5 交叉编译是什么？

```text
交叉编译是编译器运行的平台和程序运行的平台不同。
本阶段是在 x86_64 Ubuntu 上运行 arm-linux-gnueabihf-gcc，
生成 ARM 32-bit ELF，最终在 IMX6ULL ARM Cortex-A7 上运行。
```

## 5. 简历写法

```text
基于 100ask IMX6ULL_Pro 完成嵌入式 Linux 系统 bring-up，打通串口调试、U-Boot 环境变量分析、Kernel/DTB 启动验证、SD/eMMC RootFS 与 NFS RootFS 启动，并使用 Linaro arm-linux-gnueabihf 工具链完成 ARM 用户态程序交叉编译和 NFS 部署运行。
```

可拆成项目要点：

```text
- 分析 U-Boot bootcmd/bootargs，确认 zImage、DTB 和 RootFS 加载路径。
- 采集 Kernel 启动日志，验证 MMC、FEC 网卡、设备树和 ext4 RootFS 挂载。
- 配置 PC-VM-开发板直连网络，固定 Ubuntu 与 IMX6ULL IP，跑通 NFS RootFS。
- 使用 arm-linux-gnueabihf-gcc 交叉编译 hello，并通过 NFS 部署到 IMX6ULL 运行。
```

## 6. 进入阶段 2 前检查

- [x] 串口稳定可用。
- [x] 能进入 U-Boot 命令行。
- [x] 能解释 bootcmd 和 bootargs。
- [x] Kernel 和 DTB 启动证据已保存。
- [x] SD/eMMC RootFS 可启动。
- [x] NFS RootFS 可启动。
- [x] 交叉编译 hello 可在板端运行。
- [x] 板端 eth0 与 Ubuntu 网络稳定。
- [x] Day 6 板端 hello 实际输出已补齐。

## 7. 下一阶段准备

阶段 2 是 `02-drivers/`，建议从最小 LED/GPIO 开始：

```text
1. 识别板端 LED/按键对应的 GPIO 和设备树节点。
2. 从 sysfs 或已有驱动开始控制 LED。
3. 编写最小字符设备驱动。
4. 写用户态程序 open/write/ioctl 控制设备。
5. 记录 dmesg、设备节点、用户态运行输出。
```
