# IMX6ULL 主 DTB 安全细改调试 SOP

日期：2026-06-11

## 1. 核心原则

主 DTB 是整板硬件描述，不能把它当成某个驱动的局部配置文件。

Kernel 会根据 DTB 决定：

```text
哪些 platform_device / i2c_client / spi_device 被创建
哪些 GPIO、IRQ、clock、regulator、pinctrl 被分配
哪些外设启用或禁用
/dev、/sys、/proc/device-tree 暴露什么设备
udev、input、libinput、网络、背光、触摸屏等用户态流程如何被触发
```

所以以后细改主 DTB 的原则是：

```text
只从板子当前可启动的原始 DTB 出发。
只做最小差异。
先临时启动测试。
再普通重启验证。
最后才允许替换主 DTB。
永远保留可恢复备份。
```

“万无一失”在真实硬件上不能靠信心，只能靠可验证、可回滚、差异最小化。

## 2. 不推荐路径

不要直接这样做：

```text
从 Kernel 源码包里的 DTS 重新编译整板 DTB
直接覆盖 /boot/100ask_imx6ull-14x14.dtb
reboot 后再祈祷
```

本次踩坑原因就是：

```text
源码包 DTS 基线 != 板子当前出厂 DTB 基线
```

旧 Day2 测试 DTB 和原始 DTB 的 diff 有 1023 行，真实变化包括：

```text
gpio_keys_100ask 节点丢失/替换
gpio-keys 从 disabled 变成默认启用
ecspi、adc、uart6 状态变化
backlight 参数变化
多个 pinctrl/GPIO 差异
```

这类变化足以导致用户态 udev、input、网络或其他外设流程异常。

## 3. 推荐安全流程

### Step 1：备份当前可启动 DTB

板端：

```bash
cd /boot
cp -f 100ask_imx6ull-14x14.dtb 100ask_imx6ull-14x14.dtb.before-change
sync
sha256sum 100ask_imx6ull-14x14.dtb 100ask_imx6ull-14x14.dtb.before-change
ls -l 100ask_imx6ull-14x14.dtb*
```

建议每次专题使用明确名字：

```text
100ask_imx6ull-14x14.dtb.before-day2-minimal
100ask_imx6ull-14x14.dtb.before-day3-xxx
```

不要只依赖开发板时间戳，因为板子时间可能是 1970。

### Step 2：从板子当前 DTB 反编译 DTS

把板子当前 DTB 拉到工作目录：

```bash
scp root@192.168.77.200:/boot/100ask_imx6ull-14x14.dtb original.dtb
```

在 Ubuntu 或内核源码目录使用 dtc：

```bash
dtc -I dtb -O dts -o original.dts original.dtb
```

如果系统没有 `dtc`，可以使用内核源码里的：

```bash
/path/to/kernel/scripts/dtc/dtc -I dtb -O dts -o original.dts original.dtb
```

### Step 3：只做最小补丁

例如 Day2 只追加一个节点：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <0x39>;
    led-gpios = <0x15 0x3 0x1>;
    status = "okay";
};
```

不要顺手整理格式、重命名节点、删除旧节点、启用无关节点。

修改前必须确认 phandle 含义：

```text
0x15 = 原始 DTB 中 GPIO5 控制器 phandle
0x3  = GPIO5_IO03
0x1  = GPIO_ACTIVE_LOW
0x39 = 原始 DTB 中 ledgrp pinctrl
```

### Step 4：重新编译成独立测试 DTB

```bash
dtc -I dts -O dtb -o test-minimal.dtb original-plus-change.dts
dtc -I dtb -O dts -o test-minimal.roundtrip.dts test-minimal.dtb
diff -u original.dts test-minimal.roundtrip.dts > diff-original-vs-minimal.patch || true
wc -l diff-original-vs-minimal.patch
```

验收标准：

```text
diff 行数应该很小。
只允许出现本次目标节点/属性变化。
不能出现无关 status 变化。
不能出现 key、SPI、ADC、UART、backlight、ethernet、mmc 等无关节点变化。
```

本次 Day2 合格结果：

```text
original-restored.dtb                  38686 bytes
original-plus-imx6ull-led-minimal.dtb  38828 bytes
diff-original-vs-minimal.patch         17 lines
```

### Step 5：部署到独立测试路径

不要先放到 `/boot/100ask_imx6ull-14x14.dtb`。

先放到独立文件名：

```bash
scp test-minimal.dtb root@192.168.77.200:/root/stage2/dayX/test-minimal.dtb
```

确认主 DTB 未变：

```bash
ls -l /boot/100ask_imx6ull-14x14.dtb /root/stage2/dayX/test-minimal.dtb
sha256sum /boot/100ask_imx6ull-14x14.dtb /root/stage2/dayX/test-minimal.dtb
```

### Step 6：U-Boot 临时加载测试 DTB

中断 U-Boot 后，临时执行：

```text
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /root/stage2/dayX/test-minimal.dtb
bootz 0x80800000 - 0x83000000
```

注意：

```text
这里不 saveenv。
这里不覆盖 /boot 主 DTB。
失败后断电/复位会自动回到原始主 DTB。
```

### Step 7：临时启动后验收

板端：

```bash
cat /proc/cmdline
find /proc/device-tree -maxdepth 2 -name '*目标节点*'
tr -d '\0' </proc/device-tree/imx6ull_led/compatible
dmesg | tail -100
ip -br addr show eth0
ls -l /dev/目标设备
```

Day2 验证项：

```text
SSH 能回来。
eth0 仍是 192.168.77.200/24。
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led。
insmod imx6ull_led.ko 能 probe。
/dev/imx6ull_led 创建。
led_test on/off/on 正常。
```

### Step 8：替换主 DTB

只有 Step 6/7 全部通过后，才替换主 DTB：

```bash
cd /boot
cp -f 100ask_imx6ull-14x14.dtb 100ask_imx6ull-14x14.dtb.before-dayX-change
cp -f /root/stage2/dayX/test-minimal.dtb 100ask_imx6ull-14x14.dtb
sync
reboot
```

### Step 9：普通重启后再次验收

普通重启后必须重新验证：

```bash
cat /proc/cmdline
tr -d '\0' </proc/device-tree/imx6ull_led/compatible
ls -l /boot/100ask_imx6ull-14x14.dtb*
sha256sum /boot/100ask_imx6ull-14x14.dtb /root/stage2/dayX/test-minimal.dtb
ip -br addr show eth0
insmod /root/stage2/dayX/imx6ull_led.ko
ls -l /dev/imx6ull_led
/root/stage2/dayX/led_test /dev/imx6ull_led 1
/root/stage2/dayX/led_test /dev/imx6ull_led 0
dmesg | grep -E '目标驱动|probe|error|fail'
```

### Step 10：记录证据

每次必须保存：

```text
原始 DTB
修改后的 DTS
测试 DTB
diff patch
串口临时启动日志
主 DTB 替换日志
dmesg 验证日志
回滚文件名
```

对应 Day2 已保存：

```text
evidence/dtb-diff/original-restored.dtb
evidence/dtb-diff/original-plus-imx6ull-led-minimal.dts
evidence/dtb-diff/original-plus-imx6ull-led-minimal.dtb
evidence/dtb-diff/diff-original-vs-minimal.patch
logs/serial-boot-minimal-dtb.log
logs/day2-main-dtb-replacement-test.log
evidence/main-dtb-replacement.md
```

## 4. 回滚方法

如果替换主 DTB 后系统异常，优先用串口进 U-Boot，用备份 DTB 临时启动：

```text
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw init=/bin/sh'
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /boot/100ask_imx6ull-14x14.dtb.before-dayX-change
bootz 0x80800000 - 0x83000000
```

进入最小 shell 后：

```sh
mount -o remount,rw /
cp -f /boot/100ask_imx6ull-14x14.dtb.before-dayX-change /boot/100ask_imx6ull-14x14.dtb
sync
reboot -f
```

Day2 当前可用回滚文件：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

## 5. 我是怎么控制 IMX6ULL 板子的

### Windows 工作区

当前项目在 Windows：

```text
E:\imx6ull-linux-edge-ai-roadmap
```

Windows 侧负责：

```text
保存项目文档和证据。
通过 ssh/scp 访问板子。
通过 Python paramiko 访问 Ubuntu VM。
把日志拉回项目目录。
```

### Ubuntu VM

Ubuntu VM：

```text
IP: 192.168.77.132
user: liuzhiwei
password: 082800
serial: /dev/ttyACM0
```

Ubuntu 侧负责：

```text
运行交叉编译工具链。
编译内核模块和 DTB。
通过 /dev/ttyACM0 抓串口。
在需要时中断 U-Boot。
```

使用的典型方式：

```text
Windows -> paramiko SSH -> Ubuntu
Ubuntu -> /dev/ttyACM0 -> IMX6ULL 串口/U-Boot/Linux console
```

### IMX6ULL 板子

板子：

```text
IP: 192.168.77.200
user: root
serial console: ttymxc0,115200
```

板子正常启动后，通过 SSH 控制：

```bash
ssh root@192.168.77.200 "uname -a; cat /proc/cmdline"
scp file root@192.168.77.200:/root/stage2/dayX/
```

需要进 U-Boot 或救援时，通过 Ubuntu 串口控制：

```text
打开 /dev/ttyACM0
重启/复位板子
发送按键中断 U-Boot
在 => 提示符输入 ext4load / bootz / setenv 等命令
```

### 本次 Day2 实际控制链路

正常部署：

```text
Windows ssh/scp -> IMX6ULL root@192.168.77.200
```

编译和 DTB 处理：

```text
Windows paramiko -> Ubuntu 192.168.77.132
Ubuntu 内核源码 dtc / make -> 生成模块和 DTB
```

串口临时启动：

```text
Windows paramiko -> Ubuntu
Ubuntu Python/termios -> /dev/ttyACM0
串口中断 U-Boot
发送 ext4load/bootz 临时启动命令
```

强制重启抢 U-Boot：

```bash
ssh root@192.168.77.200 "echo 1 > /proc/sys/kernel/sysrq; echo b > /proc/sysrq-trigger"
```

这个命令会立即重启板子，有助于串口脚本提前守住 U-Boot 窗口。

## 6. 后续细改主 DTB 的检查清单

每次修改主 DTB 前问自己：

```text
是否从当前板子原始 DTB 反编译？
是否有明确备份文件？
diff 是否只有目标变化？
是否临时启动过？
SSH 是否回来？
eth0 是否正常？
dmesg 是否无新增严重错误？
/proc/device-tree 是否出现目标节点？
/dev 或 /sys 是否出现预期设备？
是否记录了回滚命令？
```

只有这些都满足，才允许替换主 DTB。
