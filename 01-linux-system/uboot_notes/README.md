# U-Boot Notes

## 必须掌握

- U-Boot 是 Kernel 启动前的引导程序，负责基础硬件初始化、加载 Kernel/DTB、传递 `bootargs`。
- `bootcmd` 是自动启动命令。
- `bootargs` 是传给 Kernel 的命令行参数。
- `saveenv` 会把环境变量写入持久存储，错误参数也会被保存，修改前要先记录原值。

## 常用命令

```text
version
bdinfo
printenv
printenv bootcmd
printenv bootargs
setenv name value
saveenv
ping <server_ip>
boot
```

## 笔记模板

```text
U-Boot source:
Defconfig:
Build command:
Output image:
Flash/write method:

bootcmd:
bootargs:
ipaddr:
serverip:
ethaddr:

What I changed:
Rollback command:
```
