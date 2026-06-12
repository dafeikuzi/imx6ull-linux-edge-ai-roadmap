# Day 3 工作记录：Kernel 与设备树

日期：2026-06-10

阶段：`01-linux-system` / Day 3 Kernel 与设备树

## 1. 今日目标

Day 3 的目标是把 U-Boot 已经确认的启动链路继续向下追到 Kernel 和设备树：

- 理解 Kernel 镜像 `zImage`、设备树 `.dtb`、RootFS 之间的关系。
- 确认当前 U-Boot 从哪里加载 Kernel 和 DTB。
- 保存一次完整 Kernel 启动串口日志到 `boot_logs/kernel.log`。
- 从日志里摘出 Kernel 版本、Kernel command line、MMC、网卡、RootFS 挂载和 init 启动证据。
- 如果 BSP Kernel 源码已准备好，尝试编译 `zImage dtbs`，但今天的最低验收是“能解释当前板子实际启动的 Kernel 和 DTB”。

## 2. 当前已知启动链路

来自 Day 2 的 U-Boot 证据：

```text
U-Boot version: 2017.03 (Jun 03 2020 - 13:12:42 +0800)
bootcmd: MMC first, fallback netboot
mmcdev: 1
mmcpart: 2
Kernel image: /boot/zImage
DTB: /boot/100ask_imx6ull-14x14.dtb
loadaddr: 0x80800000
fdt_addr: 0x83000000
RootFS: /dev/mmcblk1p2 rootwait rw
Kernel bootargs: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

关键理解：

```text
U-Boot 负责把 /boot/zImage 加载到内存，把 /boot/100ask_imx6ull-14x14.dtb 加载到内存，
然后执行 bootz，把控制权交给 Linux Kernel。
Kernel 收到 bootargs 后，从 /dev/mmcblk1p2 挂载 Buildroot RootFS。
```

## 3. Ubuntu 侧保存 Kernel 串口日志

在 Ubuntu 虚拟机中执行，提示符应类似：

```text
liuzhiwei@liuzhiwei-virtual-machine:~$
```

创建日志目录并打开串口：

```bash
mkdir -p ~/imx6ull-day3-logs
sudo picocom -b 115200 --logfile ~/imx6ull-day3-logs/kernel.log /dev/ttyACM0
```

操作步骤：

1. 先启动上面的 `picocom`。
2. 复位或重新上电 IMX6ULL。
3. 这次不要打断 U-Boot，让它自动启动 Linux。
4. 等到出现 `100ask login:`。
5. 登录 `root`，密码为空。
6. 在板端执行第 4 节命令。
7. 退出 `picocom`：`Ctrl + A`，再按 `Ctrl + X`。
8. 把日志复制到仓库：

```bash
cp ~/imx6ull-day3-logs/kernel.log <repo-path>/01-linux-system/boot_logs/kernel.log
```

如果仓库在 Windows 中，就先把 Ubuntu 里的日志复制到 Windows，再放入：

```text
01-linux-system/boot_logs/kernel.log
```

## 4. 板端登录后执行的命令

以下命令在 IMX6ULL 串口 shell 中执行，提示符通常类似：

```text
[root@100ask:~]#
```

不要在 Ubuntu 虚拟机里执行这些命令，否则会记录成主机信息。

```bash
uname -a
cat /proc/cmdline
mount
df -h
ifconfig -a
route -n
dmesg | grep -Ei "Linux version|Kernel command line|mmc|fec|eth|phy|VFS|EXT4|root|init|ttymxc"
ls /proc/device-tree
tr -d '\0' < /proc/device-tree/model; echo
tr -d '\0' < /proc/device-tree/compatible; echo
```

如果 `tr` 命令不可用，可退一步执行：

```bash
cat /proc/device-tree/model
cat /proc/device-tree/compatible
```

## 5. 如果要尝试编译 Kernel 和 DTB

在 Ubuntu 虚拟机中进入韦东山 BSP 的 Kernel 源码目录。

先确认工具链：

```bash
which arm-linux-gnueabihf-gcc
arm-linux-gnueabihf-gcc -v
```

查找可用 defconfig 和目标 dtb：

```bash
find arch/arm/configs -iname '*imx*defconfig' | sort
find arch/arm/boot/dts -iname '*6ull*100ask*.dts' -o -iname '*imx6ull*.dts' | sort
```

加载板卡默认配置，具体 defconfig 以 BSP 文档为准：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- <board_defconfig>
```

编译 Kernel 和设备树：

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage dtbs -j$(nproc)
```

常见产物位置：

```text
arch/arm/boot/zImage
arch/arm/boot/dts/<board>.dtb
```

当前板端 U-Boot 期望启动分区中存在：

```text
/boot/zImage
/boot/100ask_imx6ull-14x14.dtb
```

注意：

```text
今天不要急着覆盖 SD/eMMC 里的 zImage 和 dtb。
先完成日志采集和编译验证。替换启动镜像前，需要备份原始 /boot/zImage 和 /boot/*.dtb。
```

## 6. 日志中要找的证据

在 `boot_logs/kernel.log` 中标注这些关键行：

```text
Linux version ...
Kernel command line: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
mmc...
fec...
eth0...
VFS: Mounted root ...
EXT4-fs ...
Freeing unused kernel memory ...
Run /sbin/init ...
Welcome to imx6ull buildroot system !
```

如果日志中没有 `Run /sbin/init`，也可以用进入 `100ask login:` 或 shell 作为用户态启动成功的证据。

## 7. 当前完成标准

- [x] 保存 Kernel 启动日志摘录到 `01-linux-system/boot_logs/kernel.log`。
- [x] 在 `kernel_notes/day3-kernel-dtb.md` 中填写 Kernel 版本：`Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux`。
- [x] 在 `kernel_notes/day3-kernel-dtb.md` 中填写 Kernel command line：`console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw`。
- [x] 在 `kernel_notes/day3-kernel-dtb.md` 中填写 DTB 文件名和设备树 model/compatible。
- [x] 标注 MMC、网卡、RootFS 挂载和 init/登录界面证据。
- [ ] 可选：完成 BSP Kernel `zImage dtbs` 编译，并记录 defconfig、产物路径和编译结果。

## 8. 给下一步的判断

Day 3 完成后，应该能回答：

```text
1. 当前板子启动的是哪个 Kernel？
2. U-Boot 加载的是哪个 zImage 和 dtb？
3. Kernel 实际收到的 bootargs 是什么？
4. RootFS 是从 SD/eMMC 还是 NFS 挂载？
5. 当前有线网卡在 Kernel/Linux 里对应 eth0 还是 eth1？
```

这些答案会直接服务 Day 4 SD RootFS 和 Day 5 NFS RootFS。

## 9. 当前小错误

板端曾输入：

```bash
name -a
```

返回：

```text
-bash: name: command not found
```

这是命令拼写错误。正确命令是：

```bash
uname -a
```

目前 `cat /proc/cmdline` 已成功补齐：

```text
console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
```

随后 `uname -a` 已成功补齐：

```text
Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
```
