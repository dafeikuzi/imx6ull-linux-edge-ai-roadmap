# 02 Drivers

目标：在已经跑通 `01-linux-system` 的基础上，完成 IMX6ULL 核心驱动闭环。阶段 2 不追求覆盖所有外设驱动，而是优先掌握后续用户态采集、MQTT、Edge AI 网关最需要的能力：GPIO/LED、按键中断、设备树、字符设备、UART、I2C 和 `dmesg` 调试。

## 硬性目录规则

每一个小阶段的产物必须放在独立目录中，禁止把源码、`.ko`、测试程序、日志、截图、设备树片段混放在 `02-drivers/` 根目录。

每个实验目录固定使用这个结构：

```text
README.md     # 当天目标、操作、验收、问题记录
src/          # 驱动源码、用户态测试源码、Makefile
dt/           # 设备树片段、pinctrl/GPIO 修改记录
logs/         # dmesg、insmod、rmmod、程序运行输出
evidence/     # 截图、关键输出摘录、最终结论
```

如果某一天暂时没有源码或设备树修改，也保留对应目录，方便后续补证据。

## 执行入口

- `stage2-knowledge.md`：阶段 2 知识讲义。
- `stage2-sprint.md`：阶段 2 操作文档和验收清单。

## 核心实验路线

| Day | 目录 | 目标 |
|---|---|---|
| 1 | `day1-gpio-led-baseline/` | 识别 GPIO/LED 资源，先用系统已有接口验证 LED 可控 |
| 2 | `day2-led-platform-driver/` | 编写 LED platform 驱动，通过 `/dev/imx6ull_led` 控制亮灭 |
| 3 | `day3-device-tree-notes/` | 整理设备树节点、pinctrl、DTB 替换和回滚流程 |
| 4 | `day4-key-irq-driver/` | 编写按键中断驱动，支持阻塞 `read()` |
| 5 | `day5-key-poll-driver/` | 给按键驱动增加 `poll/select` 事件通知 |
| 6 | `day6-uart-test/` | 用用户态 `termios` 完成串口收发测试 |
| 7 | `day7-i2c-test/` | 用 `i2c-tools` 和用户态程序验证 I2C 设备访问 |
| 8 | `day8-review-interview/` | 复盘阶段 2，整理简历和面试问答 |

## 默认环境

```text
Board: 100ask IMX6ULL_Pro
Kernel: Linux 4.9.88
DTB: /boot/100ask_imx6ull-14x14.dtb
Serial: /dev/ttyACM0, 115200 8N1
Ubuntu VM: 192.168.77.132
Board eth0: 192.168.77.200
NFS RootFS: 192.168.77.132:/nfs/imx6ull-rootfs
Toolchain: arm-linux-gnueabihf-
```

## 驱动结构选择

阶段 2 的主线采用通用、现代、可面试解释的驱动结构：

```text
Device Tree node
  -> platform_driver of_match_table
  -> devm_* resource management
  -> gpiod descriptor API
  -> miscdevice / character device
  -> user program open/read/write/poll
```

当前板端是 Linux 4.9.88，文档会优先使用 4.9 可用的写法。如果某些新接口不可用，按文档中的兼容说明改用旧接口。

## 扩展驱动地图

这些驱动暂不纳入阶段 2 必做主线，只作为后续扩展方向：

| 类型 | 用途 | 建议 |
|---|---|---|
| SPI | 外接 Flash、屏幕、传感器 | 做完 I2C 后扩展 |
| PWM | 蜂鸣器、背光、舵机 | 适合做控制类 Demo |
| ADC | 模拟量采集 | 后续传感器采集可补 |
| LCD/Framebuffer/DRM | 显示屏 | 工作量较大，单独专题 |
| Touchscreen/Input | 触摸屏、输入子系统 | 配合 LCD 学习 |
| USB | Host/Device、U 盘、ADB | 偏系统集成 |
| RTC | 实时时钟 | 简单外设，可后补 |
| Watchdog | 系统异常复位 | 适合可靠性专题 |
| Audio/ALSA | 音频输入输出 | 复杂，后续专题 |
| Camera/V4L2 | 摄像头采集 | 适合 Edge AI 图像方向 |
| Ethernet/FEC | 网络驱动 | 当前先会调试，不重写 |
| MMC/SD | 块设备、存储 | 当前先会看日志，不重写 |

## 验收标准

- LED 能通过 `/dev/imx6ull_led` 被用户态控制。
- KEY 能通过 `/dev/imx6ull_key` 阻塞读取按键事件，并支持 `poll/select`。
- UART 能通过用户态程序配置波特率并完成收发测试。
- I2C 能通过 `i2cdetect` 或用户态程序检测/读取目标设备。
- 每个实验的命令、日志、截图、问题和结论都保存在自己的目录中。

