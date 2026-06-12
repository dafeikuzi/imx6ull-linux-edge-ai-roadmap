# Day 1 结果：GPIO/LED 基线确认

日期：2026-06-11

## 1. 执行结果摘要

```text
开发板网络连通性: 成功，ping 192.168.77.200 可达
SSH 登录: 成功，root@192.168.77.200 可非交互执行命令
Kernel: Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux
启动参数: console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw
Day 1 当前 RootFS: SD/eMMC /dev/mmcblk1p2，本次不是 NFS RootFS
```

## 2. 当前系统已有 LED class

```text
/sys/class/leds/mmc0::
/sys/class/leds/mmc1::
```

判断：这两个是 MMC/SD 卡活动触发 LED，不是明确的用户态普通 LED，因此不适合作为 Day 2 自定义 LED 字符设备驱动的目标。

## 3. 初始 GPIO 观察

```text
初始发现已导出的 GPIO: gpio133
方向: out
初始值: 1
sysfs 切换测试: 通过 /sys/class/gpio/gpio133/value 完成 1 -> 0 -> 1
GPIO 映射: gpio133 = GPIO5_IO05
```

结论：`gpio133 / GPIO5_IO05` 能被 sysfs 控制，但本地 100ask LED 示例并没有把它作为 LED 引脚。

## 4. 已锁定的 Day 2 LED GPIO

```text
Day 2 LED GPIO: GPIO5_IO03
全局 sysfs GPIO: gpio131
设备树 GPIO 写法: <&gpio5 3 GPIO_ACTIVE_LOW>
Pinmux 名称: MX6ULL_PAD_SNVS_TAMPER3__GPIO5_IO03
锁定依据: 本地 100ask LED 驱动示例和板级 DTS 片段都使用 GROUP_PIN(5, 3) / GPIO5_IO03 作为 LED。
```

`gpio131` 已通过 sysfs 导出并切换：

```text
gpio131_high=1
gpio131_low=0
gpio131_restore=1
```

## 5. Day 1 结论

- 当前系统有 LED class，但只有 `mmc0::` 和 `mmc1::`，属于 MMC 活动触发 LED，不作为 Day 2 目标。
- `gpio133 / GPIO5_IO05` 可以控制，但不是本阶段锁定的 LED 引脚。
- Day 2 LED 驱动应使用 `GPIO5_IO03 / gpio131`。
- 现代驱动不应硬编码全局 GPIO 编号，应通过设备树 `led-gpios` 获取 GPIO。
- 该 LED 是低电平有效，设备树中应写 `GPIO_ACTIVE_LOW`。

## 6. Day 2 建议设备树节点

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>;
    status = "okay";
};
```

