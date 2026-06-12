# Day 2 工作记录：进入 U-Boot 与保存环境变量

日期：2026-06-10

阶段：`01-linux-system` / Day 2 U-Boot 原理与环境变量

## 1. 今日目标

Day 2 的核心目标不是先修改系统，而是先进入 U-Boot 命令行，保存可以解释启动过程的第一手证据：

- U-Boot banner 和版本。
- `version` 输出。
- `printenv` 完整输出。
- `bootcmd`、`bootargs`、`ipaddr`、`serverip`、`ethaddr` 等关键变量。
- `bdinfo` 板级信息。
- 可选：U-Boot 阶段临时网络测试结果。

今日原则：

- 先观察，后修改。
- 先记录原值，再尝试 `setenv`。
- 暂时不要执行 `saveenv`，除非已经确认修改无误并且知道回滚方法。

## 2. Ubuntu 串口日志保存命令

在 Ubuntu 虚拟机中执行：

```bash
mkdir -p ~/imx6ull-day2-logs
sudo picocom -b 115200 --logfile ~/imx6ull-day2-logs/uboot.log /dev/ttyACM0
```

说明：

- 串口设备沿用 Day 1 已验证的 `/dev/ttyACM0`。
- `--logfile` 会把串口内容保存到 Ubuntu 的 `~/imx6ull-day2-logs/uboot.log`。
- 退出 `picocom`：先按 `Ctrl + A`，再按 `Ctrl + X`。

进入 U-Boot 后，最后把 Ubuntu 中的日志复制到仓库：

```bash
cp ~/imx6ull-day2-logs/uboot.log <repo-path>/01-linux-system/boot_logs/uboot.log
```

如果仓库不在 Ubuntu 内，可以先把日志文件复制到 Windows，再放入：

```text
01-linux-system/boot_logs/uboot.log
```

## 3. 进入 U-Boot 的操作步骤

1. 先启动 `picocom`，确认出现 `Terminal ready`。
2. 保持串口窗口激活。
3. 给开发板重新上电或按复位键。
4. 上电后立即连续按键盘回车或空格，尝试打断自动启动。
5. 如果成功，会看到类似提示：

```text
Hit any key to stop autoboot:
=>
```

`=>` 就是 U-Boot 命令行提示符。

如果没有进入 U-Boot，而是直接进入 Linux 登录界面，说明打断太晚或 `bootdelay` 太短。重新复位，提前开始连续按回车。

## 4. U-Boot 中执行的命令

先执行只读命令：

```text
version
printenv
printenv bootcmd
printenv bootargs
printenv ipaddr
printenv serverip
printenv gatewayip
printenv netmask
printenv ethaddr
printenv eth1addr
bdinfo
```

如果某些变量不存在，U-Boot 可能输出：

```text
## Error: "xxx" not defined
```

这不是失败，只说明当前环境没有设置这个变量。

## 5. 可选：U-Boot 临时网络测试

Day 1 中 Linux 阶段 DHCP 失败，所以 Day 2 可以先在 U-Boot 中做最小网络测试。

已知 Ubuntu 网络信息：

```text
Ubuntu IP: 192.168.88.132
Gateway: 192.168.88.2
Netmask: 255.255.255.0
Suggested board IP: 192.168.88.200
```

在 U-Boot 中临时设置变量：

```text
setenv ipaddr 192.168.88.200
setenv serverip 192.168.88.132
setenv gatewayip 192.168.88.2
setenv netmask 255.255.255.0
ping ${serverip}
```

注意：

- 这里不要执行 `saveenv`。
- 不执行 `saveenv` 时，复位后这些临时变量可能恢复原状。
- 如果 `ping` 失败，优先检查网线、网口、路由器/交换机、Ubuntu 桥接网络。

## 6. 需要摘录到笔记里的变量

把实际输出填入：

```text
U-Boot version:

bootcmd:

bootargs:

ipaddr:

serverip:

gatewayip:

netmask:

ethaddr:

eth1addr:

bootdelay:

```

## 7. 初步理解模板

`bootcmd` 的作用：

```text
U-Boot 倒计时结束后自动执行的启动命令，通常负责从 SD/eMMC/NAND/TFTP 加载 Kernel 和 dtb，然后执行 bootz/bootm。
```

`bootargs` 的作用：

```text
U-Boot 传给 Linux Kernel 的启动参数，里面通常包含 console、rootfs 位置、rootwait/rw、网络/NFS 参数等。
```

判断当前 rootfs 来源：

```text
如果 bootargs 中有 root=/dev/mmcblk...，通常是 SD/eMMC rootfs。
如果 bootargs 中有 root=/dev/nfs 和 nfsroot=...，就是 NFS rootfs。
```

## 8. 今日完成标准

- [ ] 成功进入 U-Boot 命令行。
- [ ] 保存 `01-linux-system/boot_logs/uboot.log`。
- [ ] 记录 `version`、`printenv`、`bootcmd`、`bootargs`、`bdinfo`。
- [ ] 明确当前系统从哪里加载 Kernel、dtb 和 rootfs。
- [ ] 只做临时网络测试，不误执行 `saveenv`。
- [ ] 把关键变量整理到 `01-linux-system/uboot_notes/day2-uboot-env.md`。

## 9. 当前待补充

已从实板串口输出确认：

- U-Boot 版本：`U-Boot 2017.03 (Jun 03 2020 - 13:12:42 +0800)`。
- 已成功进入 U-Boot `=>` 命令行。
- `bootcmd`、`mmcargs`、`mmcroot`、`fdt_file`、`ethaddr`、`eth1addr`、`bdinfo` 已记录到 `uboot_notes/day2-uboot-env.md`。
- `bootargs` 不是常驻变量，而是在 `run mmcboot` 时由 `run mmcargs` 临时生成。

待继续补充：

- U-Boot 阶段 `ping ${serverip}` 已测试失败：`ethernet@020b4000` 的 PHY auto negotiation 超时，随后 ARP 重试失败。
- 将 Ubuntu 上的 `~/imx6ull-day2-logs/uboot.log` 复制到 `01-linux-system/boot_logs/uboot.log`。

## 10. 当前网络问题判断

U-Boot 网络测试输出：

```text
ethernet@020b4000 Waiting for PHY auto negotiation to complete......... TIMEOUT !
Could not initialize PHY ethernet@020b4000
Using ethernet@020b4000 device

ARP Retry count exceeded; starting again
ping failed; host 192.168.88.132 is not alive
```

初步结论：

- IP 参数已经临时设置到同一网段：板卡 `192.168.88.200`，Ubuntu `192.168.88.132`。
- 失败点更靠前：U-Boot 没有完成 PHY 自协商。
- 这和 Day 1 Linux 阶段 `eth0/eth1 link is not ready` 互相印证。
- 下一步优先排查网线、网口、交换机/路由器端口、网口灯，以及 U-Boot 当前选择的是哪个网口。
