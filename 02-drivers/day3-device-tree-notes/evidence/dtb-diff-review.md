# Day 3 DTB Diff 复盘：bad diff vs good diff

日期：2026-06-11

## 1. 为什么复盘

Day2 第一次替换主 DTB 后，板子能进 Kernel，但用户态卡在 udev 相关流程，网络没有恢复。

根因不是“LED 节点一定有错”，而是：

```text
用于编译 DTB 的源码 DTS 基线
和
板子当前 /boot 里的出厂 DTB 基线
不一致。
```

结果是一个看似只想改 LED 的操作，实际改变了很多板级设备。

## 2. bad diff

文件：

```text
../day2-led-platform-driver/evidence/dtb-diff/diff-original-vs-day2.patch
```

规模：

```text
1023 lines
```

bad DTB 大小：

```text
original-restored.dtb  38686 bytes
day2-test.dtb          37825 bytes
```

关键危险变化：

```text
gpio_keys_100ask 节点丢失/被替换
gpio-keys 从 disabled 变成默认启用
ecspi@02008000 从 okay 变成 disabled
adc@02198000 从 okay 变成 disabled
uart6 从 okay 变成 disabled
backlight PWM/亮度参数变化
多个 pinctrl/GPIO 配置差异
```

这些变化会影响：

```text
input 设备
触摸屏/按键事件
SPI 设备
ADC 设备
串口设备
背光
udev 设备枚举
用户态服务启动
```

所以它不是一个安全的 LED-only 修改。

## 3. good diff

文件：

```text
logs/diff-original-vs-current-main.patch
```

规模：

```text
17 lines
```

当前主 DTB：

```text
original-restored.dtb  38686 bytes
current-main.dtb       38828 bytes
```

唯一目标变化：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <0x39>;
    led-gpios = <0x15 0x3 0x1>;
    status = "okay";
};
```

`status` 变化检查：

```text
+ status = "okay";
```

该 `status` 只属于新增的 `imx6ull_led` 节点，不影响其他外设。

## 4. 判断 diff 是否安全

以后每次修改 DTB，都用这个检查规则：

### 4.1 行数

```text
几十行以内通常可人工审查。
几百/上千行必须高度警惕。
```

行数不是唯一标准，但它是第一道风险提示。

### 4.2 status

重点 grep：

```bash
grep -n '^[-+].*status' diff.patch
```

危险信号：

```text
无关节点 okay -> disabled
无关节点 disabled -> okay
原本 disabled 的 gpio-keys 被启用
```

### 4.3 compatible

重点 grep：

```bash
grep -n '^[-+].*compatible' diff.patch
```

危险信号：

```text
原有节点 compatible 被替换
新增了会被系统已有驱动绑定的 generic compatible
删除了厂商定制 compatible
```

### 4.4 gpios / interrupts / pinctrl

重点 grep：

```bash
grep -n '^[-+].*gpios' diff.patch
grep -n '^[-+].*interrupt' diff.patch
grep -n '^[-+].*pinctrl' diff.patch
```

危险信号：

```text
无关 GPIO 变化
无关 interrupt 变化
无关 pinctrl 变化
```

这些变化很容易导致资源冲突或设备 probe 失败。

### 4.5 关键板级节点

必须重点观察：

```text
ethernet / fec
usdhc / mmc
uart / serial
i2c
spi
gpio-keys
touchscreen
backlight
regulator
pinctrl
```

这些节点被无意改变，后果可能比 LED 节点本身严重得多。

## 5. 为什么从原始 DTB 反编译更安全

直接从源码 DTS 重新编译，前提是：

```text
源码 DTS 必须和当前板子正在使用的 DTB 完全同源。
```

但实际常见情况：

```text
BSP 源码版本不同
厂家出厂 DTB 有额外补丁
课程资料和板载系统不是同一批
zip 解压导致符号链接/头文件异常
```

所以阶段实验更稳的路径是：

```text
从当前板子可启动 DTB 反编译
只做最小补丁
重新编译测试 DTB
diff 检查
U-Boot 临时加载
普通重启验证
最后替换主 DTB
```

## 6. Day3 当前结论

当前主 DTB 已经通过 good diff 验证：

```text
当前主 DTB 相对原始 DTB 只有 17 行 diff。
只新增 imx6ull_led 节点。
SSH 和 eth0 正常。
驱动 probe 和 app 测试正常。
```

这说明当前 Day3 可以把 Day2 minimal DTB 当作新的工作基线。
