# 01 Linux System One-Week Sprint

目标：在虚拟机 Ubuntu + IMX6ULL_Pro 实板上完成阶段 1 闭环，能解释启动链路，能编译 BSP/U-Boot/Kernel/RootFS，能记录启动日志，能跑通 SD/NFS rootfs，能交叉编译 `hello` 并在板端运行。

学习方式：原理优先，但每天必须落到实操证据。每一天结束时，把命令、日志、截图或问题记录提交到本阶段目录。

## Day 1 环境与启动链路

### 学习目标

- 理解 BootROM -> SPL/U-Boot -> Kernel -> RootFS -> init。
- 明确 Ubuntu 虚拟机、IMX6ULL、串口、SD 卡、网线之间的关系。
- 准备后续编译、烧写、串口调试、NFS/TFTP 的工具。

### 原理重点

- BootROM 固化在芯片内部，负责从启动介质加载第一级启动程序。
- U-Boot 负责初始化基础硬件、读取环境变量、加载 Kernel 和设备树。
- Kernel 通过 command line 找到 rootfs，并启动第一个用户态进程。
- RootFS 提供 `/bin`、`/sbin`、`/etc`、`/lib`、`/dev` 等运行环境。

### 实操步骤

1. 虚拟机 Ubuntu 使用桥接网络，记录 Ubuntu IP、网关和网卡名。
2. 安装基础工具：

```bash
sudo apt update
sudo apt install -y build-essential git make gcc g++ bc bison flex \
  libssl-dev libncurses-dev u-boot-tools device-tree-compiler \
  minicom picocom nfs-kernel-server tftpd-hpa net-tools file tree
```

3. 安装或解压 ARM 交叉工具链。若手头没有韦东山资料中的工具链，可使用本仓库 `toolchains/README.md` 记录的公开 Linaro 工具链包。
4. 检查工具链：

```bash
arm-linux-gnueabihf-gcc -v
which arm-linux-gnueabihf-gcc
```

5. 连接 USB 转串口，Ubuntu 内确认串口设备：

```bash
ls -l /dev/ttyUSB*
sudo dmesg | tail -30
```

6. 打开串口终端：

```bash
sudo picocom -b 115200 /dev/ttyUSB0
```

### 当日证据

- Ubuntu 版本：`lsb_release -a`
- 工具链路径和版本：`which arm-linux-gnueabihf-gcc`、`arm-linux-gnueabihf-gcc -v`
- 串口连接截图：放入 `images/`
- 网络信息：Ubuntu IP、网关、板卡计划 IP

## Day 2 U-Boot 原理与编译

### 学习目标

- 理解 U-Boot 的作用、环境变量、`bootcmd`、`bootargs`。
- 能按 BSP 文档编译 U-Boot。
- 能在串口中进入 U-Boot 命令行并记录环境变量。

### 原理重点

- `bootcmd` 是自动启动命令。
- `bootargs` 会传给 Kernel，决定串口、rootfs、网络启动等行为。
- `console=ttymxc0,115200` 指定 Kernel 日志输出串口。
- `root=/dev/mmcblkXpY` 或 `root=/dev/nfs` 指定根文件系统来源。

### 实操步骤

1. 进入 BSP 的 U-Boot 源码目录。
2. 加载板卡默认配置，具体 defconfig 以韦东山文档为准：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- <board_defconfig>
```

3. 编译 U-Boot：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
```

4. 按 BSP 文档烧写或替换启动介质中的 U-Boot。
5. 上电后在倒计时阶段按键进入 U-Boot。
6. 记录命令输出：

```text
printenv
version
bdinfo
```

### 当日证据

- 保存串口日志到 `boot_logs/uboot.log`。
- 记录 U-Boot 版本、编译命令、defconfig 名称。
- 单独摘出 `bootcmd`、`bootargs`、`ethaddr`、`ipaddr`、`serverip`。

## Day 3 Kernel 与设备树

### 学习目标

- 理解 Kernel 镜像、设备树 `.dtb`、驱动与设备树的关系。
- 能编译 Kernel 和 IMX6ULL 对应设备树。
- 能用 BSP 默认 rootfs 启动一次系统并保存完整 Kernel 日志。

### 原理重点

- Kernel 镜像负责操作系统核心逻辑。
- 设备树描述板级硬件资源，Kernel 根据设备树匹配驱动。
- Kernel command line 来自 U-Boot `bootargs`。

### 实操步骤

1. 进入 BSP 的 Kernel 源码目录。
2. 加载板卡默认配置：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- <board_defconfig>
```

3. 编译 Kernel 和设备树：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage dtbs -j$(nproc)
```

4. 按 BSP 文档把 `zImage` 和 `.dtb` 放到启动分区或 TFTP 目录。
5. 启动开发板，保存完整串口日志。

### 当日证据

- 保存 Kernel 启动日志到 `boot_logs/kernel.log`。
- 标注日志中的 Kernel 版本、command line、MMC、网卡、rootfs 挂载。
- 记录 defconfig、dtb 文件名、Kernel 镜像路径。

## Day 4 RootFS 与 SD 卡启动

### 学习目标

- 理解 RootFS 目录结构和 init 流程。
- 能从 SD 卡 rootfs 启动并登录 shell。
- 能检查系统版本、挂载点、网络和磁盘空间。

### 原理重点

- RootFS 是用户态程序和配置文件集合。
- BusyBox 提供常用 Linux 命令。
- `/etc/inittab` 或 init 脚本决定系统启动后的用户态行为。
- 动态链接程序依赖 rootfs 中的动态库。

### 实操步骤

1. 使用 BSP 默认 rootfs 或 BSP 提供的 rootfs 制作 SD 卡 rootfs 分区。
2. 检查 U-Boot `bootargs` 中 SD rootfs 参数，例如：

```text
root=/dev/mmcblk1p2 rootwait rw
```

3. 板端登录后执行：

```bash
uname -a
mount
ip addr
df -h
cat /proc/cmdline
```

4. 检查是否具备后续调试工具：`ifconfig` 或 `ip`、`scp` 或 `dropbear/sshd`、`gdbserver`。

### 当日证据

- 串口登录截图放入 `images/`。
- 记录 rootfs 来源、制作方式、分区信息。
- 记录 `/proc/cmdline` 和 `mount` 输出。

## Day 5 NFS RootFS

### 学习目标

- 理解 NFS rootfs 为什么适合嵌入式调试。
- 能让 IMX6ULL 通过 Ubuntu 虚拟机导出的 NFS 目录启动。
- 能通过日志定位 NFS 启动失败原因。

### 原理重点

- NFS rootfs 让板端直接使用 PC/Ubuntu 上的目录作为根文件系统。
- 修改 Ubuntu 目录中的文件后，板端立即可见，适合快速迭代用户态程序。
- NFS 启动依赖 U-Boot 网络参数、Kernel 网卡驱动、NFS 服务和导出权限。

### 实操步骤

1. Ubuntu 虚拟机使用桥接网络，并配置固定 IP。
2. 准备 NFS 目录：

```bash
sudo mkdir -p /nfs/imx6ull-rootfs
sudo chown -R $USER:$USER /nfs/imx6ull-rootfs
```

3. 把 BSP rootfs 解压到 `/nfs/imx6ull-rootfs`。
4. 配置 `/etc/exports`，参考 `rootfs_notes/nfs-rootfs.md`。
5. 设置 U-Boot `bootargs`，模板：

```text
console=ttymxc0,115200 root=/dev/nfs nfsroot=<Ubuntu_IP>:/nfs/imx6ull-rootfs,v3,tcp ip=<BOARD_IP>:<Ubuntu_IP>:<GATEWAY>:255.255.255.0::eth0:off
```

6. 板端启动后验证：

```bash
cat /proc/cmdline
mount
touch /hello-from-board
```

7. Ubuntu 侧确认 `/nfs/imx6ull-rootfs/hello-from-board` 是否出现。

### 当日证据

- 完善 `rootfs_notes/nfs-rootfs.md`。
- 保存 NFS 启动日志片段。
- 记录 Ubuntu IP、板卡 IP、网关、NFS 导出目录。

## Day 6 交叉编译 hello

### 学习目标

- 理解交叉编译、目标架构、动态/静态链接。
- 能在 Ubuntu 上编译 ARM 版本 `hello`，并在板端运行。
- 能说明为什么 Windows 或 x86 Ubuntu 编译出的程序不能直接在 IMX6ULL 运行。

### 原理重点

- 编译器运行在 Ubuntu x86 上，输出 ARM 机器码，这就是交叉编译。
- `file` 可以检查 ELF 的目标架构。
- 动态链接程序需要板端 rootfs 中存在匹配的动态库。
- 静态链接更易部署，但文件更大。

### 实操步骤

1. 使用 `hello_cross_compile/hello.c`。
2. 动态链接编译：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 hello.c -o hello
file hello
```

3. 可选：静态链接编译：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 -static hello.c -o hello-static
file hello-static
```

4. 通过 NFS、SCP 或 SD 卡把程序放到板端：

```bash
chmod +x hello
./hello
```

### 当日证据

- 记录编译命令和 `file hello` 输出。
- 记录板端运行输出。
- 整理“本机编译 vs 交叉编译”的一句话解释。

## Day 7 复盘与面试化整理

### 学习目标

- 把阶段 1 的实操证据整理成可以长期复用的材料。
- 能用自己的话解释 U-Boot、Kernel、RootFS、NFS、交叉编译。
- 准备进入 `02-drivers/`。

### 复盘问题

1. U-Boot 到 Kernel 的启动流程是什么？
2. `bootargs` 里的 `console`、`root`、`ip` 分别做什么？
3. 为什么 NFS rootfs 适合嵌入式调试？
4. Kernel 和设备树分别负责什么？
5. 交叉编译工具链为什么必须匹配目标架构？

### 阶段总结模板

```markdown
## 阶段 1 总结

- 开发板：
- Ubuntu 版本：
- BSP 版本：
- U-Boot 版本：
- Kernel 版本：
- RootFS 来源：
- 交叉工具链：
- 启动方式：SD / NFS
- 已保存证据：
- 最大问题：
- 解决方法：
- 下一阶段准备：
```

### 进入下一阶段前检查

- 串口稳定可用。
- SD 启动稳定可用。
- NFS rootfs 至少成功启动一次。
- 交叉编译 hello 可在板端运行。
- 能保存并解释启动日志。

## 常见失败定位

| 现象 | 优先检查 |
|---|---|
| 串口无输出 | TX/RX/GND、波特率、启动拨码、U-Boot 烧写位置 |
| 卡在 Starting kernel | dtb 是否匹配、Kernel 镜像路径、bootargs |
| 找不到 rootfs | `root=`、分区号、NFS 路径、rootfs 权限 |
| NFS mount 失败 | 桥接网络、IP、`/etc/exports`、Kernel 网卡驱动、NFS v3/v4 |
| hello 无法运行 | ELF 架构、动态库、可执行权限、rootfs ABI |
