# Hello Cross Compile

这个小实验用于验证阶段 1 的交叉编译闭环：在 Ubuntu 虚拟机上生成 ARM 程序，并在 IMX6ULL 板端运行。

## 编译

动态链接版本：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 hello.c -o hello
file hello
```

静态链接版本，可选：

```bash
arm-linux-gnueabihf-gcc -Wall -O2 -static hello.c -o hello-static
file hello-static
```

## 部署到板端

任选一种方式：

- NFS rootfs：把 `hello` 复制到 `/nfs/imx6ull-rootfs/root/`。
- SCP：`scp hello root@<BOARD_IP>:/root/`。
- SD 卡：复制到 rootfs 分区。

板端运行：

```bash
chmod +x /root/hello
/root/hello
```

## 验收记录

```text
Ubuntu compile command:

file hello output:

Board run output:

One-sentence explanation:
Cross compile means the compiler runs on the Ubuntu x86 host, but the generated ELF binary targets the ARM CPU on IMX6ULL.
```
