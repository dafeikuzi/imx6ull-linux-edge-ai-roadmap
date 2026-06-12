# Day 1 工作总结：串口连接与首次启动

日期：2026-06-10

阶段：`01-linux-system` / Day 1 环境与启动链路

## 1. 今日目标

Day 1 的目标是先建立最基础的板端调试通道，并确认 IMX6ULL_Pro 能通过串口进入 Linux 系统。

本日不追求修改 U-Boot、Kernel 或 RootFS，只验证这些基础事实：

- Ubuntu 虚拟机能识别 USB 转串口设备。
- `picocom` 能打开串口。
- IMX6ULL_Pro 上电后能输出启动日志。
- 板端能进入 Buildroot Linux 登录界面。
- Ubuntu 侧 ARM 交叉工具链已经可用。

## 2. 当前环境

主机与板卡关系：

```text
Windows 主机
-> VMware / Ubuntu 22.04.5 LTS 虚拟机
-> USB 转串口 /dev/ttyACM0
-> IMX6ULL_Pro 开发板
-> Buildroot Linux 系统
```

Ubuntu 虚拟机信息：

```text
Ubuntu version: Ubuntu 22.04.5 LTS
Network interface: ens33
Ubuntu IP: 192.168.88.132/24
Gateway: 192.168.88.2
```

串口信息：

```text
Serial device: /dev/ttyACM0
Serial tool: picocom v3.1
Baudrate: 115200
Format: 8N1
Command: sudo picocom -b 115200 /dev/ttyACM0
```

交叉工具链：

```text
Toolchain path:
/opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

Target:
arm-linux-gnueabihf

Version:
gcc version 7.5.0 (Linaro GCC 7.5-2019.12)
```

## 3. 今日操作过程

### 3.1 Ubuntu 识别 USB 转串口

最开始尝试查看 Kernel 日志：

```bash
dmesg | tail -30
```

普通用户返回：

```text
dmesg: read kernel buffer failed: Operation not permitted
```

这不是串口失败，而是 Ubuntu 权限限制。需要使用：

```bash
sudo dmesg | tail -30
```

随后检查串口设备：

```bash
ls -l /dev/ttyUSB*
ls -l /dev/ttyACM0
ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

结果显示：

```text
crw-rw---- 1 root dialout 166, 0 ... /dev/ttyACM0
```

结论：

- Ubuntu 已识别 USB 转串口设备。
- 当前串口设备名不是 `/dev/ttyUSB0`，而是 `/dev/ttyACM0`。
- 设备属于 `root:dialout`，普通用户需要加入 `dialout` 组后才能免 `sudo` 访问。

可选权限修复：

```bash
sudo usermod -aG dialout $USER
```

执行后需要注销 Ubuntu 再登录，或者重启 Ubuntu。

### 3.2 使用 picocom 打开串口

执行：

```bash
sudo picocom -b 115200 /dev/ttyACM0
```

关键输出：

```text
picocom v3.1
port is        : /dev/ttyACM0
flowcontrol    : none
baudrate is    : 115200
parity is      : none
databits are   : 8
stopbits are   : 1
Terminal ready
```

解释：

- `Terminal ready` 表示 Ubuntu 已经成功打开串口。
- `115200 8N1` 是当前串口调试参数。
- 后续只要开发板上电或复位，串口终端就能接收板端输出。

退出 `picocom`：

```text
Ctrl + A
Ctrl + X
```

### 3.3 开发板启动到 Buildroot Linux

串口中出现：

```text
Welcome to imx6ull buildroot system !

* SourceCode:     https://gitee.com/weidongshan
* Support:        https://support@100ask.net
* Shop:           https://100ask.taobao.com/
* Login:          name: root  Passwd:
100ask login: root
```

结论：

- 开发板已经从启动介质启动。
- U-Boot 已经自动加载 Kernel。
- Kernel 已经挂载 RootFS。
- 当前 RootFS 是 Buildroot 系统。
- 串口登录用户为 `root`，密码为空。

### 3.4 登录后命令验证

登录后执行：

```bash
uname -a
cat /proc/cmdline
```

截图中能看到：

```text
Linux liuzhiwei-virtual-machine 6.8.0-124-generic ...
BOOT_IMAGE=/boot/vmlinuz-6.8.0-124-generic root=UUID=... ro find_preseed=/preseed.cfg ...
```

注意：这张截图显示的是 Ubuntu 虚拟机里的命令输出，而不是 IMX6ULL 板端输出，因为提示符是：

```text
liuzhiwei@liuzhiwei-virtual-machine:~/Desktop$
```

它可以证明 Ubuntu 开发环境正常，但不能证明板端 root shell 已经执行命令。

为了补齐更强的板端证据，建议在 `picocom` 的 IMX6ULL 登录 shell 中执行：

```bash
uname -a
cat /proc/cmdline
mount
ip addr
df -h
```

并把截图保存为：

```text
01-linux-system/images/day1-board-root-shell-commands.png
```

这张截图应该包含板端提示符，例如：

```text
#
```

或类似：

```text
root@100ask:~#
```

## 4. 已保存证据

启动日志：

```text
01-linux-system/boot_logs/day1-first-boot.log
```

这份日志已经包含：

- `picocom` 串口参数。
- IMX6ULL Buildroot 启动信息。
- DHCP 获取 IP 失败信息。
- `sshd`、`telnetd`、`mosquitto` 等服务启动信息。
- Ubuntu 版本、Ubuntu IP、交叉工具链版本。
- Day 1 总结与遗留问题。

已有截图：

```text
01-linux-system/images/day1-serial-login.png
```

这张截图证明：

- 串口终端显示 `Welcome to imx6ull buildroot system !`。
- 板端进入 `100ask login:` 登录界面。
- 已输入 `root` 用户名。

建议补充截图：

```text
01-linux-system/images/day1-board-root-shell-commands.png
```

这张截图用于证明：

- 已真正进入 IMX6ULL 板端 shell。
- 能在板端执行 `uname -a`、`cat /proc/cmdline` 等命令。
- 能区分 Ubuntu 主机命令输出和 IMX6ULL 板端命令输出。

## 5. 今日关键结论

Day 1 已通过：

- Ubuntu 虚拟机识别到串口设备 `/dev/ttyACM0`。
- `picocom` 能以 `115200 8N1` 打开串口。
- IMX6ULL_Pro 能启动到 Buildroot Linux 登录界面。
- 串口登录路径已经打通。
- 交叉工具链 `arm-linux-gnueabihf-gcc` 已安装并可用。

Day 1 暂未完全解决：

- 板端 `eth0` / `eth1` 网络链路未就绪。
- DHCP 获取 IP 失败。
- 当前还没有抓到 U-Boot 阶段 banner 和 `printenv` 输出。

## 6. 遗留问题与下一步

### 6.1 网络问题

日志中出现：

```text
udhcpc: sending discover
udhcpc: no lease, failing
Failed to bring up eth0.
IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
IPv6: ADDRCONF(NETDEV_UP): eth1: link is not ready
```

说明开发板启动时尝试通过 DHCP 获取 IP，但网口没有拿到地址。

后续 Day 5 做 NFS rootfs 前必须解决：

- 确认网线插到正确网口。
- 确认网口连接到路由器/交换机。
- 确认 Ubuntu 虚拟机和开发板处于同一网段。
- 必要时给开发板配置静态 IP，例如 `192.168.88.200`。

### 6.2 U-Boot 阶段证据

Day 1 日志中没有看到完整 U-Boot banner，原因可能是：

- U-Boot 倒计时太短。
- 打开串口时 U-Boot 阶段已经过去。
- 当前日志从 Kernel/用户态服务阶段开始记录。

Day 2 需要：

- 在开发板复位或上电前提前打开 `picocom`。
- 上电后立即按键打断 U-Boot 自动启动。
- 执行：

```text
version
printenv
printenv bootcmd
printenv bootargs
bdinfo
```

并保存为：

```text
01-linux-system/boot_logs/uboot.log
```

## 7. 给新窗口恢复上下文的方法

如果新开一个 Codex/ChatGPT 窗口，建议先发下面这段话：

```text
请先阅读以下文件并恢复项目上下文：
README.md
docs/session_summary_2026-06-05.md
progress.md
task_plan.md
findings.md
01-linux-system/README.md
01-linux-system/one-week-sprint.md
01-linux-system/stage1-knowledge.md
01-linux-system/day1-summary/day1.md
01-linux-system/boot_logs/day1-first-boot.log

当前进度：
Day 1 已完成串口连接和首次启动验证。Ubuntu 22.04.5 虚拟机识别串口为 /dev/ttyACM0，使用 sudo picocom -b 115200 /dev/ttyACM0 成功进入 IMX6ULL Buildroot 登录界面。已有日志 day1-first-boot.log 和截图 day1-serial-login.png。交叉工具链 arm-linux-gnueabihf-gcc 已安装在 /opt/100ask/...。待处理问题是开发板网口 DHCP 失败，以及 Day 2 需要进入 U-Boot 命令行保存 printenv/bootargs/bootcmd。
```

新窗口恢复后，优先从 Day 2 开始：

```text
目标：进入 U-Boot 命令行，保存 U-Boot banner、version、printenv、bootcmd、bootargs，并理解 U-Boot 如何加载 Kernel 和 dtb。
```
