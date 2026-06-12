# Day 6 工作记录：交叉编译 hello

日期：2026-06-10

阶段：`01-linux-system` / Day 6 交叉编译 hello

## 1. Day 5 完成总结

Day 5 已完成 NFS RootFS 启动闭环。

核心证据：

```text
Board prompt: [root@imx6ull:~]#
Kernel bootargs: root=/dev/nfs nfsroot=192.168.77.132:/nfs/imx6ull-rootfs,v3,tcp
RootFS mount: 192.168.77.132:/nfs/imx6ull-rootfs on / type nfs
NFS version: v3 over TCP
Board-created file: /hello-from-board
Ubuntu-visible file: /nfs/imx6ull-rootfs/hello-from-board
```

Day 5 结论：

```text
IMX6ULL 已能通过 Ubuntu NFS 目录启动 RootFS。
后续用户态程序只要复制到 /nfs/imx6ull-rootfs，就能被板端直接看到。
```

## 2. 今日目标

Day 6 的目标是完成最小交叉编译闭环：

```text
Ubuntu x86_64 上编译 C 程序
-> 生成 ARM 32-bit ELF
-> 放入 NFS RootFS
-> IMX6ULL 板端运行并输出 hello
```

要证明的事情：

```text
1. 编译器运行在 Ubuntu 主机。
2. 生成的程序目标架构是 ARM。
3. 程序能在 IMX6ULL 板端运行。
4. x86 主机程序和 ARM 板端程序不能混用。
```

## 3. 源码

仓库中已有：

```text
01-linux-system/hello_cross_compile/hello.c
```

内容：

```c
#include <stdio.h>

int main(void)
{
    puts("hello from IMX6ULL cross compile");
    return 0;
}
```

## 4. Ubuntu 侧编译

以下命令在 Ubuntu 虚拟机执行，不是在板端执行。

进入源码目录。如果仓库已同步到 Ubuntu，进入仓库中的目录；如果没有，也可以先在 Ubuntu 任意目录创建 `hello.c`。

确认工具链：

```bash
which arm-linux-gnueabihf-gcc
arm-linux-gnueabihf-gcc -v
```

动态链接编译：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 hello.c -o hello
file hello
```

可选静态链接编译：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 -static hello.c -o hello-static
file hello-static
```

期望 `file` 输出包含：

```text
ELF 32-bit LSB executable, ARM, EABI5
```

## 5. 通过 NFS 部署到板端

当前板端已经从 NFS RootFS 启动，因此推荐直接复制到 Ubuntu 的 NFS 目录：

```bash
sudo cp hello /nfs/imx6ull-rootfs/root/hello
sudo chmod +x /nfs/imx6ull-rootfs/root/hello
sync
ls -l /nfs/imx6ull-rootfs/root/hello
file /nfs/imx6ull-rootfs/root/hello
```

如果也编译了静态版本：

```bash
sudo cp hello-static /nfs/imx6ull-rootfs/root/hello-static
sudo chmod +x /nfs/imx6ull-rootfs/root/hello-static
sync
```

## 6. 板端运行验证

以下命令在 IMX6ULL 板端执行：

```bash
ls -l /root/hello
file /root/hello 2>/dev/null || true
/root/hello
echo $?
```

期望输出：

```text
hello from IMX6ULL cross compile
0
```

如果动态链接版本无法运行，再测试静态链接版本：

```bash
ls -l /root/hello-static
/root/hello-static
echo $?
```

## 7. 证据记录模板

待填写：

```text
Ubuntu compiler path: /opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
Compiler version: gcc version 7.5.0 (Linaro GCC 7.5-2019.12)
Compile command: arm-linux-gnueabihf-gcc -Wall -O2 hello.c -o hello
file hello output: hello: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, for GNU/Linux 3.2.0, BuildID[sha1]=0a71b9dd616b56bec75c78f5ca04a27c9b9bd49e, with debug_info, not stripped
NFS deploy path:
NFS deploy path: /nfs/imx6ull-rootfs/root/hello, visible on board as /root/hello
Board run command: /root/hello
Board run output: hello from IMX6ULL cross compile
Exit code: 0
```

一句话解释：

```text
交叉编译就是编译器运行在 x86_64 Ubuntu 上，但输出的是 ARM IMX6ULL 能执行的 ELF 程序。
```

## 8. 当前完成标准

- [x] Ubuntu 能找到 `arm-linux-gnueabihf-gcc`。
- [x] `hello.c` 编译成功。
- [x] `file hello` 确认是 ARM 32-bit ELF。
- [x] `hello` 已复制到 `/nfs/imx6ull-rootfs/root/hello`。
- [x] 板端 `/root/hello` 能运行。
- [x] 板端输出 `hello from IMX6ULL cross compile`，退出码为 `0`。

## 10. Day 6 完成总结

Day 6 已完成交叉编译 hello 闭环。

完成链路：

```text
Ubuntu x86_64
-> arm-linux-gnueabihf-gcc 编译 hello.c
-> 生成 ARM 32-bit ELF
-> 复制到 /nfs/imx6ull-rootfs/root/hello
-> IMX6ULL 通过 NFS RootFS 看到 /root/hello
-> 板端运行 /root/hello
```

当前证据：

```text
Compiler: gcc version 7.5.0 (Linaro GCC 7.5-2019.12)
Binary: ELF 32-bit LSB executable, ARM, EABI5
Deploy path: /nfs/imx6ull-rootfs/root/hello
Board command:
  ls -l /root/hello
  /root/hello
  echo $?
Board output:
  hello from IMX6ULL cross compile
  0
Status: complete
```

最终板端证据：

```text
[root@imx6ull:~]# /root/hello
hello from IMX6ULL cross compile
[root@imx6ull:~]# echo $?
0
```

## 9. 常见问题

```text
arm-linux-gnueabihf-gcc: command not found
  工具链 PATH 未设置。检查 Day 1 工具链路径 /opt/100ask/.../bin。

/root/hello: No such file or directory
  可能没有复制到 NFS RootFS，或当前板端不是从 NFS 启动。

Permission denied
  执行 chmod +x /nfs/imx6ull-rootfs/root/hello。

not found 但文件存在
  动态链接器或 libc 不匹配。改用 -static 编译 hello-static 测试。

Exec format error
  编译成了 x86 程序，不是 ARM 程序。用 file hello 检查。
```
