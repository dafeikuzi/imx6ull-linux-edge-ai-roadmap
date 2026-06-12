# Day 3 设备树知识卡

日期：2026-06-11

## 1. 设备树是什么

设备树 DTB 是 Kernel 启动时读取的“整板硬件描述”。

它告诉 Kernel：

```text
板子上有哪些设备
设备挂在哪条总线
设备使用哪些寄存器、GPIO、IRQ、clock、regulator、pinctrl
哪些设备启用，哪些设备禁用
哪个驱动应该绑定哪个设备
```

在本项目里，U-Boot 加载：

```text
/boot/zImage
/boot/100ask_imx6ull-14x14.dtb
```

Kernel 启动后可以通过运行时文件系统查看设备树：

```bash
find /proc/device-tree -maxdepth 2
```

## 2. Day2 LED 节点

当前主 DTB 中的 LED 节点：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    pinctrl-names = "default";
    pinctrl-0 = <0x39>;
    led-gpios = <0x15 0x3 0x1>;
    status = "okay";
};
```

该节点是 Day2 自定义 LED platform driver 的硬件入口。

## 3. compatible

设备树：

```dts
compatible = "demo,imx6ull-led";
```

驱动：

```c
static const struct of_device_id imx6ull_led_of_match[] = {
    { .compatible = "demo,imx6ull-led" },
    { }
};
```

含义：

```text
Kernel 扫描设备树节点。
发现 status 可用的节点。
用 compatible 字符串匹配 platform_driver 的 of_match_table。
匹配成功后创建/绑定 platform_device，并调用驱动 probe()。
```

面试表达：

```text
compatible 是设备树节点和驱动匹配的关键字符串。
platform_driver 通过 of_match_table 声明自己支持哪些 compatible。
```

## 4. status

当前节点：

```dts
status = "okay";
```

含义：

```text
okay 表示设备启用。
disabled 表示设备禁用。
没有 status 时，很多设备树解析路径会按可用处理，但工程上建议显式写清楚。
```

Day2 曾遇到的问题：

```text
错误 DTB 让 gpio-keys 从 disabled 变成默认启用。
这类无意变化可能引入 input 设备、udev 事件和 GPIO 抢占。
```

## 5. led-gpios

当前节点：

```dts
led-gpios = <0x15 0x3 0x1>;
```

解释：

```text
0x15 = GPIO5 控制器 phandle
0x03 = GPIO5_IO03
0x01 = GPIO_ACTIVE_LOW
```

驱动读取方式：

```c
led->gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
```

为什么属性叫 `led-gpios`，代码写 `"led"`：

```text
gpiod_get(dev, "led", ...) 会优先查找 led-gpios。
命名规则是 con_id + "-gpios"。
因此 "led" 对应设备树属性 led-gpios。
```

## 6. GPIO_ACTIVE_LOW

当前 LED 是低电平点亮：

```text
GPIO_ACTIVE_LOW
```

使用 GPIO descriptor API 后，驱动通常写逻辑值：

```c
gpiod_set_value(desc, 1);  // 逻辑 on
gpiod_set_value(desc, 0);  // 逻辑 off
```

descriptor 层会根据 active-low 自动处理物理电平。

也就是说：

```text
逻辑 1 = LED 亮
物理电平可能是低
```

这比手写寄存器或裸 GPIO 编号更不容易出错。

## 7. pinctrl

当前节点：

```dts
pinctrl-names = "default";
pinctrl-0 = <0x39>;
```

对应 pinctrl：

```dts
ledgrp {
    fsl,pins = <0x14 0x58 0x0 0x5 0x0 0x110a0>;
    linux,phandle = <0x39>;
    phandle = <0x39>;
};
```

含义：

```text
pinctrl 负责把 SoC 引脚复用成 GPIO 功能，并设置 pad 电气属性。
GPIO 控制器只能控制“已经复用成 GPIO 的引脚”。
如果 pinctrl 错了，GPIO API 可能能申请成功，但实际引脚不按预期工作。
```

Day2 LED 使用：

```text
MX6ULL_PAD_SNVS_TAMPER3__GPIO5_IO03
```

这就是 GPIO5_IO03 对应的 pinmux。

## 8. phandle

源码 DTS 常见写法：

```dts
led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>;
pinctrl-0 = <&pinctrl_leds>;
```

反编译 DTS 常见写法：

```dts
led-gpios = <0x15 0x3 0x1>;
pinctrl-0 = <0x39>;
```

原因：

```text
DTB 编译后，&gpio5 / &pinctrl_leds 这种标签引用会变成 phandle 数字。
反编译出来通常只能看到数字，不一定能恢复原标签名。
```

所以做反编译补丁时要先定位数字含义：

```text
找 phandle = <0x15> 的节点，确认它是 GPIO5。
找 phandle = <0x39> 的节点，确认它是 ledgrp。
```

## 9. platform_device 和 platform_driver

设备树节点启用后，Kernel 会为它创建 platform device。

驱动注册：

```c
static struct platform_driver imx6ull_led_driver = {
    .probe = imx6ull_led_probe,
    .remove = imx6ull_led_remove,
    .driver = {
        .name = "imx6ull-led",
        .of_match_table = imx6ull_led_of_match,
    },
};
module_platform_driver(imx6ull_led_driver);
```

匹配成功后调用：

```c
static int imx6ull_led_probe(struct platform_device *pdev)
```

在 probe 中做：

```text
申请私有结构体
获取 GPIO descriptor
注册 miscdevice
保存 drvdata
打印 probe 成功日志
```

## 10. 为什么要 diff DTB

DTB 是整板配置，不能只凭肉眼看某一段。

必须 diff：

```bash
dtc -I dtb -O dts -o original.dts original.dtb
dtc -I dtb -O dts -o test.dts test.dtb
diff -u original.dts test.dts > diff.patch || true
```

安全 diff 应该满足：

```text
只出现目标节点变化。
不出现无关 status 变化。
不出现无关 pinctrl 变化。
不出现 key、spi、adc、uart、backlight、ethernet、mmc 等无关变化。
```

本次 Day2 的结论：

```text
bad diff:  1023 lines，不安全
good diff: 17 lines，安全
```

## 11. 后续写 KEY 驱动时的启发

Day4 KEY 不能直接照抄 LED。

需要先确认：

```text
KEY GPIO 是否已被 100ask 原有驱动占用。
是否已有 gpio_keys_100ask 节点启用。
generic gpio-keys 是否 disabled。
中断触发边沿怎么描述。
驱动中是使用 gpiod_to_irq() 还是直接读取 interrupts 属性。
```

也就是说，设备树不是“随便加节点”，而是要先理解当前板子的资源占用。
