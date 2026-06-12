# Day 8 Review and Interview

目标：把阶段 2 的实验结果整理成长期可复用的证据和面试表达。不要只保存代码，要保存“我怎么定位、怎么验证、怎么解释”的材料。

## 今日产物目录

```text
src/       # 本日通常不需要源码
dt/        # 汇总最终使用过的设备树节点
logs/      # 汇总关键 dmesg 摘录
evidence/  # 阶段 2 总结、简历描述、面试问答
```

## 阶段 2 总结模板

```text
Board:
Kernel:
DTB:
Toolchain:
NFS RootFS:

LED result:
KEY IRQ result:
KEY poll result:
UART result:
I2C result:

Most important bug:
How it was fixed:
Next stage preparation:
```

## 简历写法

```text
基于 100ask IMX6ULL_Pro 完成核心 Linux 驱动实验，使用设备树和 platform_driver 匹配 GPIO 设备，实现 LED 字符设备控制、按键 GPIO 中断、阻塞 read 与 poll 事件通知，并完成 UART termios 和 I2C 用户态访问验证，形成可复现的 dmesg 调试和 NFS 部署流程。
```

## 面试必会问答

### 1. 设备树 compatible 如何匹配驱动？

设备树节点里写 `compatible = "demo,imx6ull-led"`，驱动的 `of_match_table` 里也写同样字符串。Kernel 展开设备树后会匹配二者，匹配成功就调用驱动的 `probe()`。

### 2. probe() 里一般做什么？

`probe()` 负责初始化设备：读取设备树资源，申请 GPIO/IRQ，初始化私有结构体，注册 `/dev` 节点，并保存 driver data。

### 3. 为什么推荐 devm_*？

`devm_*` 申请的资源绑定到 device 生命周期。`probe()` 中途失败或驱动卸载时，内核能自动释放资源，减少手写错误清理路径。

### 4. 字符设备 read/write 和 ioctl 有什么区别？

`read()` 适合从设备取数据，`write()` 适合向设备写入简单命令或数据，`ioctl()` 适合表达更复杂、不适合字节流的控制命令。

### 5. 按键中断为什么不能做耗时操作？

中断上下文会影响系统响应，不能睡眠，也不适合做大量工作。按键中断里只记录事件、设置标志、唤醒等待队列，复杂处理留给进程上下文。

### 6. poll 比阻塞 read 多解决了什么问题？

阻塞 `read()` 只能等待当前设备。`poll()` 可以同时等待多个 fd 的事件，适合后续边缘采集程序同时处理按键、串口、网络和定时器。

### 7. 为什么 UART 和 I2C 第一版先用用户态？

IMX6ULL 的 UART/I2C 控制器驱动通常已经由 BSP 提供。阶段 2 先验证设备节点、总线和数据读写，比重写控制器驱动更贴近后续业务开发。

## 进入阶段 3 前检查

- [ ] 能解释设备树和驱动匹配流程。
- [ ] 能解释 `probe/remove` 做什么。
- [ ] 能解释 `open/read/write/poll` 和用户态系统调用关系。
- [ ] LED 控制证据已保存。
- [ ] KEY 中断和 poll 证据已保存。
- [ ] UART/I2C 测试证据已保存。
- [ ] 每个实验产物都在独立目录中。

