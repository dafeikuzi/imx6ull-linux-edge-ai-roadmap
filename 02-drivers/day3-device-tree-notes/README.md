# Day 3 Device Tree 专题

目标：把 Day2 中真实踩过的 DTB 问题系统化，形成后续 LED/KEY/UART/I2C 驱动都能复用的设备树操作能力。

Day3 不追求大量新增外设，而是把“设备树怎么看、怎么改、怎么验证、怎么回滚”讲透。

## 当前起点

来自 Day2 的当前板端状态：

```text
Board IP: 192.168.77.200
RootFS: /dev/mmcblk1p2
主 DTB: /boot/100ask_imx6ull-14x14.dtb
当前主 DTB 已是 Day2 minimal DTB
已新增节点: /proc/device-tree/imx6ull_led
compatible: demo,imx6ull-led
原始 DTB 备份: /boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
```

Day3 必须继承 Day2 的安全规则：

```text
每次只改一个功能点。
先反编译当前可启动 DTB。
先 diff 检查。
先 U-Boot 临时启动。
再考虑替换主 DTB。
保留明确回滚文件。
```

## 今日产物目录

```text
src/       # 通常不写新驱动源码，可放辅助脚本
dt/        # DTS/DTB 片段、反编译文件说明、KEY 节点预研
logs/      # dtc、diff、/proc/device-tree、dmesg 输出
evidence/  # Day3 总结、设备树知识卡、回滚演练记录
```

## Day3 必做内容

1. 记录当前主 DTB 的运行状态。
2. 反编译当前主 DTB，确认 `imx6ull_led` 节点真实存在。
3. 解释 Day2 minimal 节点中的每个字段：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <...>;
    led-gpios = <...>;
    status = "okay";
};
```

4. 解释 `compatible` 如何匹配 `platform_driver.of_match_table`。
5. 解释 `led-gpios` 如何被 `devm_gpiod_get(dev, "led", ...)` 读取。
6. 解释 `pinctrl-0` 为什么必须复用正确 pinmux。
7. 解释 `phandle` 为什么反编译后是数字，源码 DTS 中通常是 `&gpio5` / `&pinctrl_leds`。
8. 对比两个 diff：

```text
bad diff:  diff-original-vs-day2.patch      1023 lines
good diff: diff-original-vs-minimal.patch   17 lines
```

9. 整理 KEY 设备树节点预研，为 Day4 按键中断驱动做准备。
10. 写出后续修改主 DTB 的检查清单。

## Day3 板端检查命令

```bash
cat /proc/cmdline
tr -d '\0' </proc/device-tree/model; echo
tr -d '\0' </proc/device-tree/compatible; echo
find /proc/device-tree -maxdepth 2 -iname '*led*' -o -iname '*key*'
tr -d '\0' </proc/device-tree/imx6ull_led/compatible; echo
hexdump -Cv /proc/device-tree/imx6ull_led/led-gpios
hexdump -Cv /proc/device-tree/imx6ull_led/pinctrl-0
dmesg | grep -Ei 'imx6ull|led|key|gpio|pinctrl|of:' | tail -120
```

## Day3 Ubuntu/DTB 检查命令

```bash
dtc -I dtb -O dts -o current-main.dts current-main.dtb
grep -n -A12 -B6 'imx6ull_led' current-main.dts
grep -n -A8 -B4 'gpio_keys_100ask' current-main.dts
grep -n -A20 -B6 'gpio-keys' current-main.dts
diff -u original.dts current-main.dts > diff-original-vs-current-main.patch || true
wc -l diff-original-vs-current-main.patch
```

## KEY 节点预研方向

Day4 会做按键中断驱动。Day3 只预研，不急着写驱动。

重点确认：

```text
当前板子已有哪些 key 节点？
gpio_keys_100ask 使用了哪些 GPIO？
generic gpio-keys 当前是否 disabled？
哪个按键适合 Day4 自定义驱动？
是否需要先避开已有 input 驱动？
```

候选检查：

```bash
find /proc/device-tree -maxdepth 2 -iname '*key*'
tr -d '\0' </proc/device-tree/gpio_keys_100ask/compatible 2>/dev/null; echo
hexdump -Cv /proc/device-tree/gpio_keys_100ask/gpios 2>/dev/null
tr -d '\0' </proc/device-tree/gpio-keys/status 2>/dev/null; echo
```

## 回滚原则

当前 Day2 替换主 DTB 后，原始备份仍在：

```text
/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
/boot/100ask_imx6ull-14x14.dtb.bak-
```

如需恢复原始 DTB：

```bash
mount -o remount,rw /
cp -f /boot/100ask_imx6ull-14x14.dtb.before-day2-minimal /boot/100ask_imx6ull-14x14.dtb
sync
reboot -f
```

如果无法进 Linux shell，用 U-Boot 临时启动备份 DTB：

```text
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw init=/bin/sh'
ext4load mmc 1:2 0x80800000 /boot/zImage
ext4load mmc 1:2 0x83000000 /boot/100ask_imx6ull-14x14.dtb.before-day2-minimal
bootz 0x80800000 - 0x83000000
```

## Day3 验收标准

```text
当前主 DTB 运行状态记录完成。
imx6ull_led 节点解释完成。
bad diff / good diff 对比解释完成。
compatible/gpios/pinctrl/phandle 机制解释完成。
KEY 节点预研完成。
后续 DTB 修改检查清单完成。
所有产物放在 day3-device-tree-notes/ 对应子目录。
```
