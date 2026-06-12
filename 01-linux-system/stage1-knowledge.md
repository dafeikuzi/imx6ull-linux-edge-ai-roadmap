# 01 Linux System Knowledge for Beginners

这份文档是给 Linux 新手看的阶段 1 知识讲义。它不急着让你背命令，而是先把专业名词讲清楚：它是什么、为什么存在、在 IMX6ULL 启动过程中扮演什么角色、你能在哪里看到它。

配套执行计划是 `one-week-sprint.md`。建议先读本讲义，再按计划实操。

默认场景：

- 你的电脑：Windows。
- 编译环境：虚拟机 Ubuntu。
- 开发板：IMX6ULL_Pro。
- 第一阶段目标：让开发板能稳定启动 Linux，能通过串口/网络调试，能运行你交叉编译出来的 `hello` 程序。

## 0. 先建立一张地图

你可以把嵌入式 Linux 学习想成三台“机器”之间的配合：

```text
Windows 主机
  |
  | 运行虚拟机、整理资料、管理 Git
  v
Ubuntu 虚拟机
  |
  | 编译 U-Boot / Kernel / RootFS / hello，提供 NFS/TFTP
  v
IMX6ULL 开发板
  |
  | 真正运行 ARM Linux 系统
  v
传感器、LED、按键、网口、串口等硬件
```

新手最容易混乱的是：你写代码的电脑和运行代码的板子不是同一种 CPU。

- 你的 Windows/Ubuntu 通常是 x86_64。
- IMX6ULL 是 ARM Cortex-A7。
- x86 程序不能直接拿到 ARM 板子上运行。
- 所以你需要“交叉编译”。

阶段 1 的完整闭环是：

```text
Ubuntu 编译系统组件
-> 写入 SD 卡或通过网络提供给板子
-> IMX6ULL 上电
-> U-Boot 启动
-> Kernel 启动
-> 挂载 RootFS
-> 进入 shell
-> 运行 hello
```

## 1. Linux 新手先懂这些基础词

### 1.1 操作系统是什么

操作系统是夹在“硬件”和“应用程序”之间的一层管理者。

没有操作系统时，程序要自己处理很多麻烦事：

- 怎么访问内存。
- 怎么读写磁盘。
- 怎么收发网络包。
- 怎么控制串口、网卡、GPIO。
- 多个程序同时运行时谁先谁后。

Linux Kernel 就是 Linux 操作系统的核心。它负责管理硬件和系统资源。

你可以这样理解：

```text
应用程序：sensor_collector、hello、shell
    |
系统调用：open/read/write/ioctl/socket
    |
Linux Kernel：进程、内存、文件系统、网络、驱动
    |
硬件：CPU、DDR、SD 卡、网卡、串口、GPIO
```

### 1.2 Kernel 是什么

Kernel 中文常叫“内核”。它不是一个普通应用程序，而是操作系统最核心的部分。

Kernel 的作用：

- 管理 CPU：让多个程序看起来同时运行。
- 管理内存：给程序分配内存，防止互相乱改。
- 管理文件系统：让程序能用文件路径访问数据。
- 管理设备驱动：让程序能访问串口、网卡、LED、按键。
- 管理网络：提供 TCP/IP、UDP、socket 等能力。

在阶段 1，你会编译出一个 Kernel 镜像，例如：

```text
zImage
Image
uImage
```

这些都是“内核本体”的不同打包形式。

### 1.3 Shell 是什么

Shell 是你登录 Linux 后看到的命令行环境。

你输入：

```bash
ls
cd /root
cat /proc/cmdline
```

这些命令不是 Kernel 直接理解的，而是 shell 接收你的输入，然后启动对应程序。

嵌入式板子里常见 shell 来自 BusyBox，例如：

```text
/bin/sh
```

你能进入 shell，说明：

- Kernel 已经启动。
- RootFS 已经挂载。
- init 或登录程序已经工作。

### 1.4 文件系统是什么

文件系统是一套组织文件的方法。你平时看到的：

```text
/bin
/etc
/lib
/dev
/proc
/sys
/root
```

都是 Linux 文件系统树的一部分。

在 Linux 里，“一切皆文件”是很重要的思想：

- 普通文本是文件。
- 程序是文件。
- 设备节点也是文件，例如 `/dev/ttyUSB0`。
- 内核信息也能以文件形式看见，例如 `/proc/cmdline`。

### 1.5 RootFS 是什么

RootFS 是 Root File System，中文叫“根文件系统”。

“根”指的是 `/`。Linux 启动后看到的一切目录，都是从 `/` 开始的。

RootFS 里通常有：

| 目录 | 给新手的解释 |
|---|---|
| `/bin` | 常用命令，比如 `ls`、`cp` |
| `/sbin` | 系统管理命令，比如 `init`、`ifconfig` |
| `/etc` | 配置文件 |
| `/lib` | 动态库 |
| `/dev` | 设备节点 |
| `/proc` | Kernel 暴露出来的运行信息 |
| `/sys` | 设备和驱动相关信息 |
| `/root` | root 用户目录 |

Kernel 本身只是一颗“操作系统核心”。它启动后必须找到 RootFS，才能进入用户态世界。

如果 Kernel 启动了，但是 RootFS 错了，常见结果是：

- 找不到根文件系统。
- 找不到 `/sbin/init`。
- 进不了 shell。

所以阶段 1 的关键之一就是：Kernel 必须知道 RootFS 在哪里。

### 1.6 init 是什么

init 是 Linux 用户态的第一个进程，进程号通常是 1。

Kernel 挂载 RootFS 后，会尝试启动 init。init 再负责启动后续服务，比如：

- 登录终端。
- 网络服务。
- SSH。
- 你的业务程序。

嵌入式系统里 init 可能来自 BusyBox。

你可以简单记：

```text
Kernel 负责把系统带到用户态门口。
init 负责把用户态世界真正跑起来。
```

### 1.7 驱动是什么

驱动是 Kernel 中负责管理具体硬件的代码。

应用程序不应该直接操作寄存器，因为那样危险、复杂、不可移植。应用程序通常通过文件或系统调用访问设备：

```text
应用程序
  |
open/read/write/ioctl
  |
设备文件 /dev/xxx 或 sysfs
  |
Kernel 驱动
  |
硬件寄存器
```

阶段 1 不深入写驱动，但你要知道 Kernel 日志里出现的串口、网卡、MMC、NFS，都依赖对应驱动。

## 2. 嵌入式 Linux 和 PC Linux 有什么不同

你在 Ubuntu 上开机，通常不用关心 bootloader、Kernel、RootFS，因为电脑厂商和发行版已经帮你做好了。

但嵌入式开发板不同：

- 板子没有标准 BIOS/UEFI 环境。
- 硬件型号差异很大。
- 启动介质可能是 SD、NAND、eMMC、SPI Flash。
- RootFS 可能来自 SD，也可能来自网络 NFS。
- Kernel 需要知道这块板子具体有哪些硬件。

所以你要自己处理这些问题：

```text
谁来引导 Kernel？        -> U-Boot
Kernel 怎么知道硬件？    -> Device Tree
Kernel 去哪里找 RootFS？ -> bootargs
用户态命令从哪里来？     -> RootFS / BusyBox / Buildroot
程序怎么给 ARM 板运行？  -> 交叉编译
```

## 3. 上电后到底发生了什么

IMX6ULL 从上电到进入 shell，可以分成 5 个台阶。

```text
1. BootROM
2. SPL / U-Boot
3. Linux Kernel
4. RootFS
5. init / shell
```

### 3.1 BootROM

BootROM 是芯片内部固化的一小段程序。它在芯片生产时就已经写好了，普通开发者不能修改。

作用：

- 上电后最先运行。
- 根据启动拨码判断从哪里启动。
- 从 SD、NAND、eMMC 等介质读取下一阶段程序。

你通常看不到 BootROM 的详细日志。你能控制的是：

- 启动拨码。
- 启动介质里写入的 U-Boot 是否正确。

### 3.2 SPL

SPL 是 Secondary Program Loader，二级程序加载器。

为什么需要 SPL？

因为芯片刚上电时，DDR 内存可能还没初始化，大程序不能直接运行。SPL 比较小，先完成最基础的初始化，尤其是 DDR，然后再加载完整 U-Boot。

不是每个 BSP 都要求你单独关注 SPL。有些资料会把 SPL 和 U-Boot 的烧写流程封装好。

新手阶段只要记住：

```text
BootROM 通常先加载一个很小的启动程序。
这个小程序初始化 DDR 后，再加载完整 U-Boot。
```

### 3.3 U-Boot

U-Boot 是 bootloader。bootloader 的意思是“启动加载器”。

它不是 Linux。它是 Linux 启动前的一段程序。

U-Boot 做什么：

- 初始化串口，让你能看到启动日志。
- 初始化 DDR、MMC、网卡等基础硬件。
- 从 SD 卡、eMMC、NAND 或网络加载 Kernel。
- 加载设备树 dtb。
- 把启动参数 bootargs 传给 Kernel。
- 跳转到 Kernel。

你在串口里看到类似：

```text
U-Boot 20xx.xx
Hit any key to stop autoboot:
```

说明 U-Boot 已经运行起来了。

### 3.4 Linux Kernel

U-Boot 加载 Kernel 后，会把控制权交给 Kernel。

Kernel 开始运行后，你会看到类似：

```text
Starting kernel ...
Linux version ...
Kernel command line: ...
```

Kernel 会初始化各种子系统和驱动，然后根据 `root=` 参数寻找 RootFS。

### 3.5 RootFS 和 shell

Kernel 找到 RootFS 后，会启动 init。init 再启动登录终端或 shell。

你最终看到：

```text
login:
```

或者：

```text
#
```

说明你已经进入用户态，可以执行 Linux 命令了。

## 4. U-Boot 从零理解

### 4.1 U-Boot 为什么存在

Kernel 很强大，但它不能凭空启动自己。

上电时硬件状态很原始：

- DDR 可能还没准备好。
- 串口还没初始化。
- SD 卡控制器还没初始化。
- 网卡还不能用。

所以需要一个比 Kernel 更早运行的程序，把系统带到“Kernel 可以接手”的状态。这就是 U-Boot 的价值。

### 4.2 U-Boot 和 Kernel 的关系

可以这样想：

```text
U-Boot 是点火的人。
Kernel 是发动机。
RootFS 是驾驶舱和工具箱。
```

U-Boot 点火后，Kernel 开始真正管理系统。U-Boot 不会一直管理 Linux。

### 4.3 U-Boot 环境变量是什么

环境变量就是 U-Boot 保存的一组配置。

你可以把它理解成 U-Boot 的“启动配置表”。

常见变量：

| 变量 | 新手解释 |
|---|---|
| `bootcmd` | 倒计时结束后自动执行什么命令 |
| `bootargs` | 交给 Kernel 的启动说明书 |
| `ipaddr` | 板子自己的 IP |
| `serverip` | Ubuntu 服务器 IP |
| `gatewayip` | 网关 IP |
| `netmask` | 子网掩码 |
| `ethaddr` | 板子网卡 MAC 地址 |

查看：

```text
printenv
printenv bootargs
```

修改：

```text
setenv name value
```

保存：

```text
saveenv
```

注意：`saveenv` 会把配置写入持久存储。新手改之前一定先记录原值。

### 4.4 bootcmd 是什么

`bootcmd` 是 U-Boot 自动启动时执行的命令。

比如它可能做这些事：

```text
从 SD 卡读取 zImage
从 SD 卡读取 dtb
执行 bootz 启动 Kernel
```

你可以理解为：

```text
bootcmd = U-Boot 自动开机脚本
```

### 4.5 bootargs 是什么

`bootargs` 是 U-Boot 传给 Kernel 的参数字符串。

Kernel 启动时需要知道很多事情：

- 日志输出到哪个串口？
- RootFS 在哪里？
- RootFS 是 SD 还是 NFS？
- 网络 IP 怎么配置？

这些信息很多都来自 `bootargs`。

例如：

```text
console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

拆开看：

| 片段 | 含义 |
|---|---|
| `console=ttymxc0,115200` | Kernel 日志输出到 IMX 串口，波特率 115200 |
| `root=/dev/mmcblk1p2` | 根文件系统在 MMC 设备的第 2 个分区 |
| `rootwait` | 等待 MMC 设备准备好 |
| `rw` | RootFS 以可读写方式挂载 |

NFS rootfs 的例子：

```text
console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.1.10:/nfs/imx6ull-rootfs,v3,tcp ip=192.168.1.20:192.168.1.10:192.168.1.1:255.255.255.0::eth0:off
```

这句话比较长，但本质是：

```text
Kernel 日志走串口。
RootFS 不在 SD 卡，而在网络 NFS。
Ubuntu 服务器 IP 是 192.168.1.10。
板子 IP 是 192.168.1.20。
```

### 4.6 U-Boot 阶段你要会看什么

先不要急着看源码。阶段 1 先学会观察：

1. 串口有没有 U-Boot banner。
2. 能不能按键进入 U-Boot 命令行。
3. `printenv bootcmd` 是什么。
4. `printenv bootargs` 是什么。
5. `ipaddr` 和 `serverip` 是否在同一网段。
6. `ping <Ubuntu_IP>` 能不能通。

## 5. Kernel 从零理解

### 5.1 Kernel 为什么不能直接通吃所有硬件

Linux Kernel 可以支持很多硬件，但它不可能在启动时自动猜出你的板子长什么样。

比如同样是 IMX6ULL：

- 有的板子 LED 接在这个 GPIO。
- 有的板子 LED 接在另一个 GPIO。
- 有的板子有 Wi-Fi，有的没有。
- 有的屏幕接口不同。

Kernel 需要一份“硬件说明书”。这就是设备树。

### 5.2 设备树是什么

设备树英文是 Device Tree。

它用一种树状结构描述硬件，例如：

```text
CPU
内存
串口
网卡
I2C
SPI
GPIO
LED
按键
```

设备树不是驱动。它只是告诉 Kernel：

```text
这里有一个设备。
它的寄存器地址是这里。
它的中断号是这个。
它的引脚这样复用。
它应该匹配哪个驱动。
```

常见文件：

| 文件 | 新手解释 |
|---|---|
| `.dts` | 人能读的设备树源文件 |
| `.dtsi` | 被多个 dts 共用的片段 |
| `.dtb` | 编译后给 Kernel 用的二进制设备树 |

### 5.3 compatible 是什么

设备树里常见：

```dts
compatible = "fsl,imx6ull";
```

`compatible` 可以理解成“匹配暗号”。

Kernel 里的驱动也会声明自己支持哪些 compatible。二者匹配上，Kernel 才知道这个硬件该由哪个驱动处理。

阶段 2 写驱动时会深入这个机制。阶段 1 先记住：

```text
设备树描述硬件。
驱动控制硬件。
compatible 负责把二者匹配起来。
```

### 5.4 Kernel command line 是什么

Kernel command line 就是 Kernel 启动参数。它通常来自 U-Boot 的 `bootargs`。

板端进入 Linux 后看：

```bash
cat /proc/cmdline
```

你看到的内容，就是 Kernel 实际收到的启动参数。

这一步很重要，因为新手经常以为自己改了 U-Boot 参数，但 Kernel 实际收到的不是那一份。

### 5.5 Kernel 日志怎么读

Kernel 启动日志很多，新手先抓主线：

```text
Linux version ...
Kernel command line: ...
mmc...
fec...
VFS: Mounted root ...
Run /sbin/init as init process
```

逐个解释：

| 日志 | 说明 |
|---|---|
| `Linux version` | 当前启动的 Kernel 版本 |
| `Kernel command line` | Kernel 收到的 bootargs |
| `mmc` | SD/eMMC 相关驱动 |
| `fec` | i.MX 系列网卡驱动常见名字 |
| `VFS: Mounted root` | RootFS 挂载成功 |
| `Run /sbin/init` | Kernel 开始进入用户态 |

如果你看到：

```text
Kernel panic - not syncing: VFS: Unable to mount root fs
```

大概率是 RootFS 没找到、分区错、NFS 错、文件系统支持缺失。

## 6. RootFS 从零理解

### 6.1 为什么 Kernel 还需要 RootFS

Kernel 启动后只能说明操作系统核心活了，但用户还没法正常使用系统。

因为常用命令不在 Kernel 里：

- `ls` 不在 Kernel 里。
- `sh` 不在 Kernel 里。
- `ifconfig` 不在 Kernel 里。
- 配置文件也不在 Kernel 里。

这些都在 RootFS 里。

可以这样理解：

```text
Kernel = 管理硬件和资源的核心
RootFS = 用户态生活环境
```

### 6.2 BusyBox 是什么

BusyBox 是嵌入式 Linux 常用工具集合。

普通 Linux 里，`ls`、`cp`、`mount` 可能分别是不同程序。BusyBox 把很多命令集合在一个小程序里，节省空间。

所以你可能看到：

```text
/bin/ls -> busybox
/bin/cp -> busybox
/bin/sh -> busybox
```

这不是异常，而是嵌入式系统常见设计。

### 6.3 动态库是什么

程序运行时可能依赖一些公共代码，这些公共代码被做成“动态库”。

例如 C 程序常依赖 libc。

动态链接程序运行时需要 RootFS 里有对应动态库。如果缺少动态库，程序可能无法运行。

有时你执行一个文件，报：

```text
No such file or directory
```

但文件明明存在。这可能是动态链接器不存在，而不是你的程序文件不存在。

排查：

```bash
file hello
readelf -l hello | grep interpreter
```

新手阶段可以先用静态链接减少依赖：

```bash
arm-linux-gnueabihf-gcc -static hello.c -o hello-static
```

### 6.4 SD RootFS 和 NFS RootFS 的区别

| 类型 | RootFS 放在哪里 | 优点 | 缺点 |
|---|---|---|---|
| SD RootFS | SD 卡分区 | 独立运行，不依赖网络 | 修改文件常要拔卡或复制 |
| NFS RootFS | Ubuntu 目录 | 修改方便，调试快 | 依赖网络和 NFS 服务 |

阶段 1 两个都要懂：

- SD 启动证明板子可以独立启动。
- NFS 启动提高开发调试效率。

## 7. NFS 从零理解

### 7.1 NFS 是什么

NFS 是 Network File System，网络文件系统。

它允许一台机器通过网络访问另一台机器上的目录。

在我们的场景中：

```text
Ubuntu 把 /nfs/imx6ull-rootfs 共享出来。
IMX6ULL 通过网线把这个目录挂载成自己的 /。
```

这就是 NFS rootfs。

### 7.2 为什么 NFS rootfs 对新手很有用

如果你用 SD RootFS，每次改一个程序可能要：

```text
编译 -> 复制到 SD 卡 -> 插回板子 -> 启动 -> 测试
```

如果你用 NFS RootFS：

```text
编译 -> 复制到 Ubuntu 的 /nfs/imx6ull-rootfs -> 板子直接看到 -> 测试
```

这个效率差异非常大。

### 7.3 NFS rootfs 为什么要求桥接网络

虚拟机 NAT 模式下，板子可能访问不到 Ubuntu 虚拟机。

桥接模式下：

```text
Windows、Ubuntu 虚拟机、IMX6ULL 都像接在同一个路由器/交换机上。
```

这样板子才能直接访问 Ubuntu 的 NFS 服务。

### 7.4 NFS 启动参数慢慢拆

完整参数：

```text
console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.1.10:/nfs/imx6ull-rootfs,v3,tcp ip=192.168.1.20:192.168.1.10:192.168.1.1:255.255.255.0::eth0:off
```

拆成几块：

```text
console=ttymxc0,115200
```

Kernel 日志从串口输出。

```text
root=/dev/nfs
```

告诉 Kernel：根文件系统不是本地 SD 卡，而是 NFS。

```text
nfsroot=192.168.1.10:/nfs/imx6ull-rootfs,v3,tcp
```

告诉 Kernel：NFS 服务器是 `192.168.1.10`，目录是 `/nfs/imx6ull-rootfs`，用 NFS v3 和 TCP。

```text
ip=192.168.1.20:192.168.1.10:192.168.1.1:255.255.255.0::eth0:off
```

告诉 Kernel：板子 IP、服务器 IP、网关、掩码、网口名、不要自动 DHCP。

## 8. 交叉编译从零理解

### 8.1 编译是什么

C 代码是人能读的文本：

```c
printf("hello\n");
```

CPU 不能直接执行 C 代码。编译器会把 C 代码变成机器码。

```text
C 源码 -> 编译器 -> 可执行文件
```

### 8.2 为什么叫交叉编译

如果你在 Ubuntu x86_64 上编译给 Ubuntu x86_64 自己运行，叫本机编译。

```text
x86_64 Ubuntu 编译 -> x86_64 Ubuntu 运行
```

如果你在 Ubuntu x86_64 上编译给 ARM IMX6ULL 运行，叫交叉编译。

```text
x86_64 Ubuntu 编译 -> ARM IMX6ULL 运行
```

“交叉”的意思就是：编译发生的平台和运行程序的平台不同。

### 8.3 工具链是什么

工具链是一组配合使用的编译工具，不只是 gcc。

常见工具：

| 工具 | 作用 |
|---|---|
| `gcc` | 编译 C 代码 |
| `ld` | 链接程序 |
| `as` | 汇编 |
| `objcopy` | 转换目标文件格式 |
| `strip` | 去掉调试信息，减小体积 |
| `readelf` | 查看 ELF 信息 |

`arm-linux-gnueabihf-gcc` 这个名字可以拆开：

| 部分 | 含义 |
|---|---|
| `arm` | 目标架构是 ARM |
| `linux` | 目标系统是 Linux |
| `gnu` | GNU 工具链 |
| `eabihf` | ARM EABI，硬浮点 |
| `gcc` | C 编译器 |

如果你没有韦东山资料中的工具链包，可以先使用本仓库 `01-linux-system/toolchains/README.md` 记录的公开 Linaro 工具链：

```text
gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz
```

它解压后提供的编译器前缀就是阶段 1 文档中使用的：

```text
arm-linux-gnueabihf-
```

注意：这个 `.tar.xz` 包是给 Ubuntu x86_64 虚拟机使用的，不是复制到 IMX6ULL 板子上运行的。它的作用是在 Ubuntu 上生成 ARM 板子能运行的程序。

### 8.4 怎么确认程序是不是 ARM 版本

用：

```bash
file hello
```

如果是给 IMX6ULL 的程序，你希望看到类似：

```text
ELF 32-bit LSB executable, ARM, EABI5
```

如果看到：

```text
x86-64
```

那它就是给 Ubuntu 主机运行的，不是给板子运行的。

## 9. BSP 和 Buildroot 从零理解

### 9.1 BSP 是什么

BSP 是 Board Support Package，板级支持包。

你可以把 BSP 理解成开发板厂家为这块板准备的一整套资料：

- U-Boot 源码和配置。
- Kernel 源码和配置。
- 设备树。
- RootFS。
- 烧写工具。
- 编译脚本。
- 文档教程。

为什么新手优先用 BSP？

因为 BSP 是离你的具体开发板最近的一套资料。它更可能已经适配好 DDR、网卡、屏幕、SD 卡等硬件。

### 9.2 Buildroot 是什么

Buildroot 是一个自动构建嵌入式 Linux 系统的工具。

它可以帮你生成：

- 交叉工具链。
- RootFS。
- Kernel。
- U-Boot。
- BusyBox。
- 各种用户态软件包。

但是对新手来说，Buildroot 一上来信息量比较大。所以阶段 1 的顺序建议是：

```text
先用 BSP 跑通板子
-> 再用 Buildroot 理解系统如何自动构建
```

## 10. 串口、SSH、TFTP、NFS 分别是什么

### 10.1 串口

串口是最基础的调试通道。

它的价值是：即使网络没起来、RootFS 没起来，U-Boot 和 Kernel 早期日志也能通过串口输出。

阶段 1 串口必须先搞定。

### 10.2 SSH

SSH 是网络登录工具。

板子网络和 SSH 服务正常后，你可以从 Ubuntu 登录板子：

```bash
ssh root@<BOARD_IP>
```

但 SSH 依赖更多条件：

- Kernel 启动成功。
- RootFS 正常。
- 网络正常。
- SSH/dropbear 服务存在并启动。

所以 SSH 不是最底层调试手段，串口才是。

### 10.3 TFTP

TFTP 是简单文件传输协议。

嵌入式里常用于 U-Boot 阶段从 Ubuntu 下载 Kernel 或 dtb。

比如：

```text
tftp 80800000 zImage
tftp 83000000 board.dtb
```

阶段 1 不一定必须用 TFTP，但你要知道它常和 U-Boot 网络启动配合。

### 10.4 NFS

NFS 是网络文件系统。

TFTP 常用于“下载一个文件”，NFS 常用于“把整个目录当文件系统挂载”。

简单区别：

| 工具 | 用途 |
|---|---|
| TFTP | U-Boot 下载 Kernel/dtb |
| NFS | Kernel 挂载 RootFS |
| SSH/SCP | Linux 启动后登录/复制文件 |
| 串口 | 最底层日志和命令行 |

## 11. 阶段 1 操作为什么这样安排

### 11.1 为什么先串口

因为没有串口，你看不到 U-Boot 和 Kernel 早期发生了什么。排障会变成猜。

### 11.2 为什么先 BSP

因为 BSP 最贴近你的具体 IMX6ULL_Pro 硬件。先跑通，比一开始研究通用源码更有效。

### 11.3 为什么要记录 U-Boot 日志

U-Boot 日志证明：

- 启动介质和拨码正确。
- U-Boot 已经运行。
- 能进入命令行。
- 启动参数可检查、可修改。

### 11.4 为什么要记录 Kernel 日志

Kernel 日志证明：

- Kernel 镜像能运行。
- dtb 大概率匹配。
- 串口、MMC、网卡等驱动有初始化迹象。
- RootFS 是否挂载成功。

### 11.5 为什么要跑 hello

hello 是最小闭环：

```text
你写 C 代码
-> Ubuntu 交叉编译
-> 放到板子
-> 板子运行
```

这证明你已经具备后续写用户态采集程序的基础。

## 12. 常见错误背后的原因

### 12.1 串口无输出

可能不是 Linux 问题，因为 Linux 还没开始。

优先考虑：

- USB 转串口没有接到虚拟机。
- TX/RX 接反。
- GND 没接。
- 波特率错。
- 启动拨码错。
- U-Boot 没写对位置。

### 12.2 卡在 Starting kernel

说明 U-Boot 已经把控制权交给 Kernel 了，但 Kernel 后续没有正常输出。

可能原因：

- dtb 不匹配。
- Kernel 配置不对。
- `console=` 串口参数错。
- Kernel 崩溃太早。

### 12.3 找不到 RootFS

说明 Kernel 已经启动了一部分，但没找到用户态根目录。

可能原因：

- `root=` 写错。
- SD 分区号错。
- NFS IP 或路径错。
- Kernel 没启用对应文件系统。
- RootFS 内容不完整。

### 12.4 hello 运行不了

可能原因：

- 编译成了 x86 程序，不是 ARM。
- 没有执行权限。
- 动态库缺失。
- 动态链接器缺失。
- ABI 和 RootFS 不匹配。

## 13. 新手应该怎样学习这章

建议按三轮学。

### 第一轮：只理解主线

先记住：

```text
U-Boot 负责启动 Kernel。
Kernel 负责管理硬件和挂载 RootFS。
Device Tree 告诉 Kernel 板子硬件长什么样。
RootFS 提供用户态命令和库。
NFS 让 RootFS 可以放在 Ubuntu 上。
交叉编译让 x86 Ubuntu 编译 ARM 程序。
```

这一轮不追求记住所有命令。

### 第二轮：对照串口日志

拿真实启动日志，对照找：

- U-Boot banner。
- `bootargs`。
- `Starting kernel`。
- `Kernel command line`。
- MMC/网卡日志。
- `VFS: Mounted root`。
- login 或 shell。

看到日志能定位阶段，就已经进步很大。

### 第三轮：动手改一个最小参数

比如：

- 改 `bootargs` 从 SD rootfs 到 NFS rootfs。
- 编译 hello 并放到 NFS rootfs。
- 修改 Ubuntu 里的文件，看板子是否立即看到。

这时候你会真正明白这些概念为什么有用。

## 14. 阶段 1 必会问答

### 14.1 U-Boot 是什么

U-Boot 是 Linux 启动前运行的 bootloader。它负责初始化基础硬件，加载 Kernel 和设备树，并把 `bootargs` 传给 Kernel。

### 14.2 Kernel 是什么

Kernel 是 Linux 操作系统核心，负责管理 CPU、内存、文件系统、网络和设备驱动。

### 14.3 设备树是什么

设备树是硬件描述文件。它告诉 Kernel 板子上有哪些硬件、地址在哪里、中断号是什么、应该匹配哪个驱动。

### 14.4 RootFS 是什么

RootFS 是根文件系统，是 Linux 用户态环境。里面有命令、动态库、配置文件、设备节点和 init。

### 14.5 bootargs 是什么

`bootargs` 是 U-Boot 传给 Kernel 的启动参数。它告诉 Kernel 日志输出到哪里、RootFS 在哪里、网络参数是什么。

### 14.6 NFS rootfs 是什么

NFS rootfs 是让板子通过网络把 Ubuntu 上的一个目录挂载成自己的根文件系统。它适合调试，因为修改 Ubuntu 目录里的文件，板子马上能看到。

### 14.7 交叉编译是什么

交叉编译是在一种 CPU 平台上编译另一种 CPU 平台运行的程序。这里是在 x86 Ubuntu 上编译 ARM IMX6ULL 能运行的程序。

## 15. 最小操作索引

理解完概念后，再看这些命令会更有意义。

### 15.1 查看 U-Boot 环境

```text
printenv
printenv bootcmd
printenv bootargs
```

### 15.2 查看 Kernel 实际启动参数

```bash
cat /proc/cmdline
```

### 15.3 查看 RootFS 挂载情况

```bash
mount
df -h
```

### 15.4 查看网络

```bash
ip addr
ip route
```

### 15.5 交叉编译 hello

```bash
arm-linux-gnueabihf-gcc -Wall -O2 hello.c -o hello
file hello
```

### 15.6 判断是否 NFS rootfs

板端：

```bash
mount
touch /hello-from-board
```

Ubuntu：

```bash
ls -l /nfs/imx6ull-rootfs/hello-from-board
```

## 16. 公开参考资料

这些资料适合作为后续深入阅读，不建议新手一开始逐页硬啃。先用本讲义建立地图，再查官方文档。

- U-Boot Environment Variables: <https://docs.u-boot.org/en/v2024.10/usage/environment.html>
- U-Boot bootm command: <https://docs.u-boot.org/en/latest/usage/cmd/bootm.html>
- Linux Kernel command-line parameters: <https://www.kernel.org/doc/html/latest/admin-guide/kernel-parameters.html>
- Linux Kernel NFS root documentation: <https://docs.kernel.org/admin-guide/nfs/nfsroot.html>
- Linux and the Devicetree: <https://www.kernel.org/doc/html/latest/devicetree/usage-model.html>
- Buildroot user manual: <https://buildroot.org/downloads/manual/manual.html>
- Ubuntu NFS server documentation: <https://documentation.ubuntu.com/server/how-to/networking/install-nfs/>
- Bootlin embedded Linux training materials: <https://bootlin.com/docs/>
- NXP i.MX 6ULL product page and manuals: <https://www.nxp.com/products/i.MX6ULL>
