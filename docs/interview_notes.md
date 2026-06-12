# 面试笔记

## 简历项目 1：IMX6ULL Linux 系统构建

简历写法：

> 基于 IMX6ULL 完成 U-Boot、Linux Kernel、RootFS 构建，支持 SD/NFS 启动、SSH 调试和用户程序交叉编译部署。

面试要能讲清楚：

- U-Boot 到 Kernel 的启动流程。
- Kernel command line 里 `root=`、`console=` 的作用。
- NFS rootfs 为什么方便调试。
- 交叉编译工具链和目标架构的关系。

学习与证据入口：

- `01-linux-system/stage1-knowledge.md`
- `01-linux-system/one-week-sprint.md`
- `01-linux-system/day7-summary/day7.md`
- `01-linux-system/boot_logs/uboot.log`
- `01-linux-system/boot_logs/kernel.log`
- `01-linux-system/rootfs_notes/nfs-rootfs.md`
- `01-linux-system/hello_cross_compile/`

当前实测证据摘要：

```text
U-Boot: 2017.03
Kernel: Linux 100ask 4.9.88
SD/eMMC RootFS: /dev/mmcblk1p2 ext4
NFS RootFS: 192.168.77.132:/nfs/imx6ull-rootfs
Board IP: 192.168.77.200
Ubuntu IP: 192.168.77.132
Toolchain: gcc-linaro-7.5.0 arm-linux-gnueabihf-
hello: ELF 32-bit LSB executable, ARM, EABI5
```

阶段 1 必会问答：

1. U-Boot 到 Kernel 的启动流程是什么？
   - BootROM 从启动介质加载 SPL/U-Boot，U-Boot 初始化基础硬件，根据 `bootcmd` 加载 Kernel 镜像和 dtb，并把 `bootargs` 传给 Kernel。Kernel 初始化驱动和文件系统后挂载 rootfs，最后启动 init 进入用户态。
2. `bootargs` 里的 `console`、`root`、`ip` 分别做什么？
   - `console` 指定 Kernel 日志和登录串口，`root` 指定根文件系统位置，`ip` 在 NFS rootfs 场景下指定板端网络参数。
3. 为什么 NFS rootfs 适合调试？
   - 板端直接挂载 Ubuntu 上的目录作为 rootfs，修改用户态程序和配置后不用反复烧写 SD 卡，迭代更快。
4. Kernel 和设备树分别负责什么？
   - Kernel 提供操作系统核心和驱动框架，设备树描述板级硬件资源，Kernel 根据设备树匹配并初始化驱动。
5. 交叉编译工具链为什么必须匹配目标架构？
   - IMX6ULL 是 ARM 目标平台，x86 Ubuntu 本机编译产物不能在 ARM CPU 上运行，必须用 ARM 工具链生成目标架构 ELF。

## 简历项目 2：设备驱动与设备树

简历写法：

> 基于 100ask IMX6ULL_Pro 完成核心 Linux 驱动实验，使用设备树和 platform_driver 匹配 GPIO 设备，实现 LED 字符设备控制、按键 GPIO 中断、阻塞 read 与 poll 事件通知，并完成 UART termios 和 I2C 用户态访问验证，形成可复现的 dmesg 调试和 NFS 部署流程。

面试要能讲清楚：

- 设备树 `compatible` 如何匹配 `platform_driver`。
- `probe()` 中如何获取 GPIO/IRQ 并注册 `/dev` 节点。
- 字符设备 `read/write/poll` 和用户态系统调用的关系。
- 按键中断为什么要唤醒等待队列，为什么不能在中断里做耗时操作。
- UART 为什么第一版先用用户态 `termios`，I2C 为什么先用 `i2c-tools` 和 `/dev/i2c-*` 验证。

学习与证据入口：

- `02-drivers/stage2-knowledge.md`
- `02-drivers/stage2-sprint.md`
- `02-drivers/day2-led-platform-driver/`
- `02-drivers/day4-key-irq-driver/`
- `02-drivers/day5-key-poll-driver/`
- `02-drivers/day6-uart-test/`
- `02-drivers/day7-i2c-test/`

## 简历项目 3：边缘采集与 MQTT 上云

简历写法：

> 使用 C/C++ 实现 IMX6ULL 用户态采集程序，支持配置化采样、JSON 封装、日志记录、异常重试，并通过 MQTT 接入 ThingsBoard，实现数据可视化和远程控制。

面试要能讲清楚：

- MQTT 与 HTTP 的区别。
- 断线重连如何做。
- JSON 数据格式如何设计。
- 云端 RPC 控制板端外设的完整链路。

## 简历项目 4：边缘 AI 与云边协同

简历写法：

> 在 ARMv7 Linux 平台部署 INT8 轻量模型，并构建 IMX6ULL 云边协同系统，板端采集数据上传，服务端写入数据库并调用本地大模型生成异常分析与控制建议。

面试要能讲清楚：

- 为什么 IMX6ULL 不适合直接跑大模型。
- INT8 量化的收益。
- 板端小模型和云端大模型的分工。
- Zenoh/MQTT 在边缘系统里的作用。
