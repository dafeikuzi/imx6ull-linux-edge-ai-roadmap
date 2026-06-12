# ARM Toolchain Package

本目录用于临时存放阶段 1 使用的 ARM Linux 交叉工具链安装包。

推荐工具链：

```text
gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz
```

公开下载地址：

```text
https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/arm-linux-gnueabihf/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz
```

官方返回大小：

```text
109676168 bytes
```

这个包适合放到 Ubuntu x86_64 虚拟机里使用，解压后提供的编译器前缀是：

```text
arm-linux-gnueabihf-
```

也就是可以使用：

```bash
arm-linux-gnueabihf-gcc
arm-linux-gnueabihf-ld
arm-linux-gnueabihf-objcopy
```

## 当前下载状态

当前工作区曾尝试从 Linaro 官方站下载该文件，但网络较慢，下载未完成。未完成文件会保存为：

```text
gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz.partial
```

如果你看到 `.partial` 文件，说明它不是完整安装包，不能直接解压使用。

## 在 Windows 继续下载

在 PowerShell 中执行：

```powershell
cd E:\imx6ull-linux-edge-ai-roadmap
curl.exe -L -C - --retry 5 --retry-delay 5 `
  -o 01-linux-system\toolchains\gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz `
  "https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/arm-linux-gnueabihf/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz"
```

下载完成后确认大小：

```powershell
(Get-Item 01-linux-system\toolchains\gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz).Length
```

结果应为：

```text
109676168
```

## 在 Ubuntu 中安装

把完整 `.tar.xz` 文件复制到 Ubuntu 虚拟机，例如放到 `~/Downloads`。

解压到 `/opt/100ask`：

```bash
sudo mkdir -p /opt/100ask
sudo tar -xf ~/Downloads/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz -C /opt/100ask
```

确认 bin 目录：

```bash
find /opt/100ask -type f -name arm-linux-gnueabihf-gcc
```

假设输出是：

```text
/opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
```

临时加入 PATH：

```bash
export PATH=/opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin:$PATH
```

永久加入 PATH：

```bash
echo 'export PATH=/opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

验证：

```bash
which arm-linux-gnueabihf-gcc
arm-linux-gnueabihf-gcc -v
```

能看到 `/opt/100ask/.../bin/arm-linux-gnueabihf-gcc` 和 GCC 版本信息，就说明工具链配置成功。
