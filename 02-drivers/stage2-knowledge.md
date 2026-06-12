# 02 Drivers Knowledge for Beginners

这份文档解释阶段 2 要用到的驱动知识。目标不是让你一开始就背内核源码，而是把“设备树、platform 驱动、字符设备、GPIO、中断、UART、I2C”串成一条可以在 IMX6ULL 上实操的路线。

## 0. 阶段 2 的地图

阶段 1 已经证明开发板能启动 Linux、能用 NFS RootFS、能运行交叉编译程序。阶段 2 要进入 Kernel 和硬件之间的区域：

```text
用户态程序
  |
  | open/read/write/poll/ioctl
  v
/dev/imx6ull_led, /dev/imx6ull_key, /dev/ttymxc*, /dev/i2c-*
  |
  v
Linux 驱动
  |
  v
GPIO / IRQ / UART / I2C 控制器
  |
  v
LED / 按键 / 串口模块 / I2C 传感器
```

你要掌握的不只是“代码能跑”，还要能解释：

- 设备树怎样描述硬件。
- 驱动怎样匹配设备树。
- 用户程序怎样访问驱动。
- `dmesg` 怎样定位问题。

## 1. 驱动是什么

驱动是 Linux Kernel 里负责控制具体硬件的软件。用户态程序通常不直接操作寄存器，而是通过系统调用访问设备文件：

```text
应用程序 write(fd, "1", 1)
  -> VFS
  -> file_operations.write
  -> LED 驱动
  -> GPIO 控制器
  -> LED 引脚电平变化
```

阶段 2 先写简单驱动，不追求一次性覆盖所有内核子系统。

## 2. 为什么先做 LED 和按键

LED 是最小输出设备，按键是最小输入设备。它们组合起来能覆盖驱动开发最核心的概念：

| 实验 | 学到什么 |
|---|---|
| LED | GPIO 输出、字符设备、`write()` |
| KEY | GPIO 输入、中断、等待队列、阻塞 `read()` |
| KEY poll | `poll/select` 事件通知 |
| Device Tree | `compatible`、`gpios`、`pinctrl` |
| UART | 用户态串口、`termios` |
| I2C | 总线设备、设备地址、用户态访问、client driver 思路 |

## 3. 设备树是什么

设备树是 Kernel 的硬件说明书。它告诉 Kernel：板子上有什么设备、设备用哪些引脚、GPIO 极性是什么、应该匹配哪个驱动。

一个学习用 LED 节点可以写成：

```dts
imx6ull_led {
    compatible = "demo,imx6ull-led";
    led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>;
    status = "okay";
};
```

关键字段：

| 字段 | 含义 |
|---|---|
| `compatible` | 匹配驱动的字符串 |
| `led-gpios` | LED 使用的 GPIO 资源 |
| `status = "okay"` | 启用这个节点 |
| `pinctrl-0` | 引脚复用和电气属性配置 |

## 4. platform_driver 是什么

很多片上外设或板级小设备没有自动枚举能力，需要设备树告诉 Kernel 它存在。Linux 常用 `platform_driver` 来管理这类设备。

典型结构：

```c
static const struct of_device_id imx6ull_led_of_match[] = {
    { .compatible = "demo,imx6ull-led" },
    { }
};

static struct platform_driver imx6ull_led_driver = {
    .probe = imx6ull_led_probe,
    .remove = imx6ull_led_remove,
    .driver = {
        .name = "imx6ull-led",
        .of_match_table = imx6ull_led_of_match,
    },
};
```

流程：

```text
Kernel 展开设备树
-> 发现 compatible = "demo,imx6ull-led"
-> 找到 of_match_table 中相同字符串的驱动
-> 调用 probe()
-> 驱动申请 GPIO、注册 /dev 节点
```

## 5. gpiod descriptor API

老写法常使用 GPIO 编号：

```c
gpio_request(3, "led");
gpio_direction_output(3, 1);
gpio_set_value(3, 0);
```

这种方式依赖全局 GPIO 编号，不够通用。阶段 2 主线使用 descriptor 风格：

```c
led->gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
gpiod_set_value(led->gpiod, 1);
```

它的好处：

- GPIO 来自设备树，不硬编码编号。
- `GPIO_ACTIVE_LOW` 这类极性能被框架处理。
- `devm_*` 自动释放资源，减少出错。

注意：Linux 4.9 已支持 descriptor API，但具体函数或头文件可能与新内核略有差异。若 BSP 头文件缺少某个新接口，按当天 README 的兼容提示改写。

## 6. 字符设备是什么

字符设备适合按字节读写，常见接口是 `/dev/xxx`。驱动提供 `file_operations`，用户态通过系统调用触发这些回调。

```c
static const struct file_operations imx6ull_led_fops = {
    .owner = THIS_MODULE,
    .write = imx6ull_led_write,
};
```

用户态：

```bash
echo 1 > /dev/imx6ull_led
echo 0 > /dev/imx6ull_led
```

驱动态：

```text
write()
-> copy_from_user()
-> 判断输入字符
-> gpiod_set_value()
```

## 7. miscdevice 为什么适合入门

标准字符设备需要 `alloc_chrdev_region`、`cdev_init`、`class_create`、`device_create`。这很完整，但入门时样板较多。

`miscdevice` 是一种简化字符设备注册方式：

```c
static struct miscdevice led_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "imx6ull_led",
    .fops = &imx6ull_led_fops,
};
```

注册后会自动生成：

```text
/dev/imx6ull_led
```

阶段 2 推荐先用 `miscdevice`，后续再学习完整 `cdev` 流程。

## 8. 按键中断

按键不能靠用户程序不停轮询 GPIO，这会浪费 CPU。更合理的方式是中断：

```text
按键电平变化
-> GPIO 控制器产生 IRQ
-> Kernel 调用驱动 irq handler
-> 驱动记录事件
-> 唤醒等待队列
-> read() 返回事件给用户态
```

中断处理函数里不能做耗时操作，通常只做：

- 读取或记录状态。
- 设置标志位。
- 唤醒等待队列。

## 9. 阻塞 read 和 poll

阻塞 `read()`：

```text
用户程序 read()
-> 没有按键事件就睡眠
-> 中断来了唤醒
-> read() 返回事件
```

`poll/select`：

```text
用户程序 poll()
-> 驱动告诉内核自己等待哪个队列
-> 有事件时 poll 返回可读
-> 用户程序再 read()
```

这为后续用户态采集程序打基础，因为真实项目往往需要同时监听多个 fd。

## 10. UART 测试

IMX6ULL 的串口通常已经有内核驱动。阶段 2 不重写 UART 驱动，先学会用户态访问：

```text
/dev/ttymxc0  # 通常是控制台串口
/dev/ttymxc1  # 可能是外接串口，具体看设备树和板子资料
```

用户态通过 `termios` 配置：

- 波特率。
- 数据位。
- 停止位。
- 校验位。
- 阻塞/非阻塞读写。

## 11. I2C 测试

I2C 是总线模型。一个控制器下面可以挂多个设备，每个设备有地址。

先用用户态工具验证：

```bash
i2cdetect -y 0
i2cdump -y 0 0x50
```

再理解内核驱动：

```text
i2c_driver
  -> of_match_table / id_table
  -> probe()
  -> i2c_smbus_read_byte_data()
```

阶段 2 第一版只要求能检测/读取设备，不强制写完整 I2C client driver。

## 12. dmesg 调试

驱动问题优先看 `dmesg`：

```bash
dmesg | tail -50
dmesg -c
insmod imx6ull_led.ko
dmesg | tail -50
```

常见问题：

| 现象 | 优先检查 |
|---|---|
| `probe()` 没执行 | `compatible` 是否匹配，DTB 是否真的替换 |
| `/dev/xxx` 不存在 | `misc_register` 是否成功，udev/mdev 是否运行 |
| `insmod` 报 unknown symbol | Kernel 版本、模块编译路径、配置不匹配 |
| GPIO 申请失败 | 设备树属性名、pinctrl、GPIO 是否被占用 |
| 按键无中断 | IRQ 触发边沿、GPIO 输入方向、硬件接线 |

## 13. 阶段 2 必会问答

1. 设备树和驱动怎么匹配？
   - 设备树节点的 `compatible` 和驱动的 `of_match_table` 匹配后，Kernel 调用驱动的 `probe()`。
2. `probe()` 里通常做什么？
   - 获取设备树资源，申请 GPIO/IRQ，初始化私有数据，注册字符设备。
3. 为什么推荐 `devm_*`？
   - 设备卸载或 probe 失败时资源自动释放，减少清理路径错误。
4. LED 驱动为什么适合用 `write()`？
   - LED 是输出设备，用户态写入目标状态即可控制硬件。
5. 按键为什么需要中断？
   - 按键是异步事件，中断能避免用户态持续轮询浪费 CPU。
6. `poll()` 的意义是什么？
   - 让用户程序能同时等待多个文件描述符事件，是后续采集程序常用模型。
