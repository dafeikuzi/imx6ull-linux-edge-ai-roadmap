# Day 4 启动记录：KEY IRQ driver

日期：2026-06-11

## 1. Day3 交接

Day3 已确认当前主 DTB 是 Day2 minimal DTB：

```text
/boot/100ask_imx6ull-14x14.dtb = 38828 bytes
/proc/device-tree/imx6ull_led/compatible = demo,imx6ull-led
eth0 = 192.168.77.200/24
```

Day4 目标：

```text
新增 imx6ull_key 设备树节点。
编写 KEY IRQ platform driver。
通过 key-gpios 获取 GPIO。
gpiod_to_irq() 转换中断号。
read() 在无按键事件时阻塞。
按键中断后唤醒 read() 并返回 pressed/released。
```

## 2. KEY 资源预检查

当前 DTB 中已有：

```text
gpio_keys_100ask: enabled
gpio-keys: disabled
```

运行时 `/dev/input` 当前主要是：

```text
event0 = 20cc000.snvs:snvs-powerkey
event1 = goodix-ts
```

没有看到 `gpio_keys_100ask` 暴露为标准 input event 设备。

debugfs GPIO 当前显示：

```text
gpio-131 | led   | out hi
gpio-133 | sysfs | out hi
```

未看到 `gpio129` 被占用。

## 3. Day4 选用 GPIO

Day4 先选用：

```text
KEY GPIO: GPIO5_IO01
global gpio: gpio129
active: GPIO_ACTIVE_LOW
pinctrl: key1_100ask
phandle: 0x3a
```

当前反编译 DTS：

```dts
key1_100ask {
    fsl,pins = <0xc 0x50 0x0 0x5 0x0 0x110a0>;
    linux,phandle = <0x3a>;
    phandle = <0x3a>;
};
```

最小补丁计划：

```dts
imx6ull_key {
    compatible = "demo,imx6ull-key";
    pinctrl-names = "default";
    pinctrl-0 = <0x3a>;
    key-gpios = <0x15 0x1 0x1>;
    status = "okay";
};
```

其中：

```text
0x15 = GPIO5 controller phandle
0x1  = GPIO5_IO01
0x1  = GPIO_ACTIVE_LOW
```

## 4. 安全规则

Day4 仍按 Day2/Day3 DTB SOP：

```text
当前主 DTB -> 反编译 DTS -> 最小补丁 -> 编译测试 DTB -> diff 检查 -> U-Boot 临时启动 -> 验证 -> 再考虑替换主 DTB
```

不要直接从 Kernel 源码 DTS 整板重编译主 DTB。
