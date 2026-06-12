# Findings

## Workspace

- Current workspace: `E:\韦东山Linux嵌入式`.
- Existing materials include 韦东山 IMX6ULL Pro course PDFs/DOCX, board resources, learning logs, and test code.
- Root workspace is not a git repository, so the roadmap is created as an independent directory.

## Project Direction

- Best internship-oriented route is embedded Linux first: system build, drivers, C/C++ user-space, networking.
- Edge AI and LLM integration should be positioned as add-ons: board-side light inference or data collection, PC/server-side model service.
- The final portfolio should demonstrate a closed loop: collect data, upload, visualize, infer/analyze, return command, control hardware.

## 01 Linux System Sprint

- User prefers a theory-first route, but wants a one-week sprint that still produces daily hands-on evidence.
- Ubuntu virtual machine is acceptable as the main BSP/Kernel/RootFS build environment; bridge networking should be the default for NFS/TFTP and board access.
- The first bring-up path should prioritize 韦东山 IMX6ULL_Pro BSP to get a complete board boot loop before using Buildroot as an enhancement.
- Stage 1 evidence should focus on U-Boot logs, Kernel logs, RootFS/NFS notes, screenshots, and a cross-compiled hello program rather than committing full BSP source or large images.
- Public references for Stage 1 knowledge should prefer official/project documentation: U-Boot docs, Linux Kernel docs, Buildroot manual, Ubuntu NFS docs, Bootlin training materials, and NXP i.MX 6ULL materials.
- The Stage 1 knowledge guide needs to teach concepts before commands because the target reader is a Linux beginner.
- The selected public fallback toolchain is Linaro `gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz`, which matches the `arm-linux-gnueabihf-` prefix used throughout the Stage 1 docs. Official size is 109676168 bytes.

## Day 1 Evidence

- Ubuntu VM serial device is `/dev/ttyACM0`, not `/dev/ttyUSB0`.
- `picocom` with `115200 8N1` can open the serial connection.
- IMX6ULL reaches the 100ask Buildroot login screen over serial.
- Board networking is not ready yet: DHCP fails on `eth0`, and both `eth0`/`eth1` report link not ready.
- A screenshot with prompt `liuzhiwei@liuzhiwei-virtual-machine` is host Ubuntu evidence, not board evidence; board-side command screenshots should be captured inside the serial login shell.

## Day 2 U-Boot Plan

- Day 2 should first capture U-Boot evidence, not permanently modify the board environment.
- Use `sudo picocom -b 115200 --logfile ~/imx6ull-day2-logs/uboot.log /dev/ttyACM0` from Ubuntu to preserve the full serial session.
- Required U-Boot commands are `version`, `printenv`, `printenv bootcmd`, `printenv bootargs`, and `bdinfo`.
- Because Day 1 DHCP failed, U-Boot network checks should use temporary `setenv` values first: board IP `192.168.88.200`, server IP `192.168.88.132`, gateway `192.168.88.2`, netmask `255.255.255.0`.
- Avoid `saveenv` until the original environment has been saved and a rollback path is known.
- The previous global `*.log` ignore rule would hide evidence logs; `01-linux-system/boot_logs/*.log` is now explicitly unignored.
- First Day 2 serial capture did not stop at the `=>` prompt; it automatically booted into Buildroot. Still, it confirmed U-Boot `2017.03 (Jun 03 2020)`, default environment warning `bad CRC`, mmc1 boot, Kernel `4.9.88`, and rootfs from `root=/dev/mmcblk1p2`.
- Second Day 2 capture successfully reached the U-Boot prompt.
- Current boot path is MMC-first: `bootcmd` runs `findfdt`, attempts undefined `findtee`, selects `mmcdev=1`, loads `/boot/zImage` and `/boot/100ask_imx6ull-14x14.dtb` from `mmcpart=2`, then `mmcboot` runs `bootz`.
- `printenv bootargs` returns not defined because `bootargs` is generated dynamically by `mmcargs` during `mmcboot`, not stored as a persistent environment variable.
- Key persistent variables include `bootdelay=3`, `mmcdev=1`, `mmcpart=2`, `mmcroot=/dev/mmcblk1p2 rootwait rw`, `fdt_file=100ask_imx6ull-14x14.dtb`, `ethaddr=00:01:1f:2d:3e:4d`, and `eth1addr=00:01:3f:2d:3e:4d`.
- U-Boot networking currently fails before IP-level communication: `ethernet@020b4000 Waiting for PHY auto negotiation ... TIMEOUT`, followed by ARP retry failure. This aligns with Linux boot logs showing `eth0`/`eth1` link not ready, so physical link or selected Ethernet port should be checked first.
- Switching U-Boot to `ethernet@02188000` avoids the PHY auto-negotiation timeout, but ARP still fails. After moving the direct-link network to `192.168.77.0/24`, tcpdump sees Realtek self-probe ARP for `192.168.77.1`, but not ARP from the board IP `192.168.77.200`, so the remaining issue is below IP reply handling: board ARP is not visible on the host/VM capture path or the selected physical port still does not match the U-Boot device.
- Temporarily disabling Windows Defender Firewall did not change the U-Boot ping result. tcpdump shows Windows/Realtek ARP requests for `192.168.77.200`, but not U-Boot ARP requests from `192.168.77.200` to `192.168.77.1`, so firewall is not the primary issue.
- Linux-stage testing shows the active physical link is `eth0`, not `eth1`: `eth0` is `LOWER_UP` and reports `Link is Up - 100Mbps/Full`, while `eth1` is `NO-CARRIER`. Because `eth1` was assigned `192.168.77.200` before `eth0` was assigned `192.168.77.201`, same-subnet routing may be confused; continue with `eth1` down and only `eth0` configured.
- After disabling `eth1` and configuring only `eth0=192.168.77.200/24`, the IMX6ULL Linux system successfully pinged Windows Realtek `192.168.77.1` with 4/4 replies. Direct cable networking is confirmed at the Linux stage on `eth0`.
- Final direct-link topology works: IMX6ULL `eth0=192.168.77.200/24`, Windows Realtek `192.168.77.1/24`, VMware VMnet0 bridged to Realtek, and Ubuntu VM `ens33=192.168.77.132/24`. The board successfully pings Ubuntu with 4/4 replies, so Day 5 NFS can use this direct-link network.

## Day 3 Kernel and Device Tree Plan

- Current U-Boot evidence already identifies the Kernel and DTB files loaded from the boot partition: `/boot/zImage` and `/boot/100ask_imx6ull-14x14.dtb`.
- The expected Kernel command line is `console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw`, generated by `mmcargs` from `console`, `baudrate`, and `mmcroot`.
- Day 3 should collect a fresh full serial boot log as `01-linux-system/boot_logs/kernel.log`; the existing Day 1 log is explanatory and partial, not a complete raw Kernel evidence log.
- The minimum Day 3 closure does not require replacing the board's Kernel. It requires confirming the actual booted Kernel version, command line, DTB model/compatible, MMC/rootfs mount, FEC Ethernet initialization, and Buildroot login evidence.
- If BSP Kernel source is available, compile `zImage dtbs` in Ubuntu with `ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-`, but do not overwrite `/boot/zImage` or the board DTB until the original boot files are backed up.
- The pasted Day 3 Kernel excerpt confirms `ttymxc0` console enablement, `mmcblk1` detection, `mmcblk1p2` ext4 rootfs mount, FEC Ethernet registration, `eth0` link-up at 100Mbps/full duplex, and a usable root shell.
- Device tree runtime inspection reports model `Freescale i.MX6 ULL 14x14 EVK Board` and compatible strings `fsl,imx6ull-14x14-evk` plus `fsl,imx6ull`, which matches the expected IMX6ULL 14x14 EVK style DTB used by the current boot path.
- Board-side `uname -a` confirms the running Kernel is `Linux 100ask 4.9.88 #1 SMP PREEMPT Wed Apr 8 14:08:53 CST 2026 armv7l GNU/Linux`.
- Board-side `cat /proc/cmdline` confirms the actual Kernel command line is `console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw`.
- During evidence collection, `name -a` returned `-bash: name: command not found`; this is only a command typo, and the correct Kernel-version command is `uname -a`.

## Day 4 SD/eMMC RootFS Plan

- Day 4 should treat the current 100ask Buildroot system as a known-good SD/eMMC RootFS and inspect it from inside the board shell.
- The key proof is not only `root=/dev/mmcblk1p2`, but also the actual `/` mount source, filesystem type, read/write mode, available disk space, init scripts, and persistent network configuration.
- Because Day 2 already fixed `eth0=192.168.77.200/24`, Day 4 should re-check `/etc/network/interfaces`, `route -n`, and `ping -c 4 192.168.77.132` to confirm the RootFS carries the network configuration across reboots.
- Tool availability should be recorded before Day 5/Day 6: `ssh/scp/dropbear`, `gdbserver`, `mosquitto_pub/sub`, `sqlite3`, `tar`, `wget`, and `tftp` may or may not exist in the current Buildroot image.
- Day 4 board output confirms the current RootFS is Buildroot `2020.02-g65177d4`; `/` is `/dev/root` ext4 mounted `rw`, backed by the `root=/dev/mmcblk1p2` boot argument and Day 3 Kernel mount evidence.
- The root filesystem has enough room for Stage 1 experiments: `1.5G` total, `691M` available, `50%` used.
- RootFS network persistence is confirmed: `/etc/network/interfaces` contains the static `eth0=192.168.77.200/24`, default gateway `192.168.77.1`, and `eth1` manual configuration.
- Board-to-host connectivity is ready for Day 5 NFS: ping to Windows `192.168.77.1` and Ubuntu `192.168.77.132` both show 4/4 replies and 0% packet loss.
- `busybox` is not available as a standalone command in the current shell, despite the system being Buildroot-based; do not rely on `busybox` command output for evidence. Use concrete commands such as `mount`, `df`, `ifconfig`, `route`, and `ping`.
- Day 4 tool check shows useful user-space tools are already present: `ssh`, `scp`, `gdbserver`, `mosquitto_pub`, `mosquitto_sub`, `sqlite3`, `vi`, `tar`, `wget`, and `tftp`. `dropbear` is missing, but OpenSSH client tools are present.

## Day 5 NFS RootFS Plan

- Day 4 is complete. The board is ready for Day 5 because Linux-stage networking is stable and persistent: board `192.168.77.200` can ping Ubuntu `192.168.77.132`.
- The NFS export should be scoped to the direct-link subnet instead of world-writable wildcard export: `/nfs/imx6ull-rootfs 192.168.77.0/24(rw,sync,no_root_squash,no_subtree_check)`.
- First NFS boot attempts should use temporary U-Boot variables only; do not run `saveenv` until NFS rootfs has booted successfully and rollback is understood.
- Because current `bootcmd` may run `mmcargs` and overwrite `bootargs`, be ready to bypass `run bootcmd` by manually loading `/boot/zImage` and `/boot/100ask_imx6ull-14x14.dtb`, then running `bootz`.
- Ubuntu may not have `/etc/exports.d/` by default. If `tee /etc/exports.d/imx6ull-rootfs.exports` fails with `No such file or directory`, create it with `sudo mkdir -p /etc/exports.d` or append the export line directly to `/etc/exports`.
- Ubuntu NFS export is now confirmed active: `/nfs/imx6ull-rootfs` is exported to `192.168.77.0/24` with `rw`, `sync`, `no_subtree_check`, and `no_root_squash`.
- Preferred Day 5 RootFS archive found in the board materials: `E:\韦东山Linux嵌入式\开发板资料\02_100ask_imx6ull_pro_2022.08\03_开发板系统固件\Buildroot_image\rootfs.tar.bz2`. Use this before OTA or production-test rootfs archives.
- NFS RootFS content is present and structurally valid: `/nfs/imx6ull-rootfs/linuxrc` is executable, and `/bin` plus `/etc` exist.
- First U-Boot NFS boot attempt failed before Kernel load because `ext4load` was typed as `/boo` instead of `/boot/zImage`; this is a command typo, not a missing file or NFS issue.
- Day 5 NFS RootFS succeeded. The board booted with `root=/dev/nfs`, mounted `192.168.77.132:/nfs/imx6ull-rootfs` as `/`, and a file created on the board at `/hello-from-board` was visible on Ubuntu under `/nfs/imx6ull-rootfs/hello-from-board`.
- The observed Bluetooth firmware warning and `udevd` unknown `kvm` group warning are non-blocking for Stage 1 NFS validation.

## Day 6 Cross Compile Plan

- Since Day 5 NFS RootFS is working, Day 6 should deploy `hello` by copying the Ubuntu-built binary into `/nfs/imx6ull-rootfs/root/hello`; the board will see it as `/root/hello`.
- Use `arm-linux-gnueabihf-gcc` on Ubuntu and verify the binary with `file hello`; the expected architecture evidence is `ELF 32-bit ... ARM`.
- If the dynamically linked `hello` fails on the board with a misleading `No such file or directory`, build `hello-static` with `-static` to bypass runtime dynamic-linker/libc mismatches.
- Day 6 is complete: the cross-compiled ARM `hello` was deployed through the NFS RootFS and run on the board as `/root/hello`, printing `hello from IMX6ULL cross compile` with exit code `0`.

## Day 7 Stage 1 Review

- Stage 1 is functionally complete: serial, U-Boot inspection, Kernel/DTB evidence, SD/eMMC RootFS, NFS RootFS, and cross-compiled hello have all been exercised.
- The strongest portfolio narrative is not "built a whole distro from scratch", but "completed embedded Linux bring-up and debug workflow on real IMX6ULL hardware, with reproducible logs and network/NFS/交叉编译闭环".
- The final `/root/hello` output has been captured, so Stage 1 evidence is complete enough to move into `02-drivers/`.

## Candidate Open-Source Projects

- Buildroot: rootfs/toolchain/build-system reference.
- ThingsBoard and EMQX: MQTT cloud dashboard and broker.
- Edge Impulse standalone Linux and ncnn: light ARMv7 AI inference path.
- Zenoh: modern edge communication.
- Qdrant, Ollama, LocalAI: vector database and local LLM service on PC/server.

## Stage 2 Driver Plan

- Stage 2 should stay as a core driver loop, not a full peripheral encyclopedia.
- Required core topics are GPIO LED, KEY IRQ, KEY poll/select, Device Tree, UART user-space termios, I2C user-space access, and dmesg-driven debugging.
- The chosen modern structure is Device Tree node -> `platform_driver` -> `of_match_table` -> `devm_*` resource acquisition -> GPIO descriptor API -> `miscdevice` or character-device user interface.
- Current board Kernel is Linux 4.9.88, so templates avoid newer helpers such as `dev_err_probe()` and should stay compatible with older BSP headers where possible.
- Every Stage 2 sub-stage must use its own directory with `README.md`, `src/`, `dt/`, `logs/`, and `evidence/`. Empty directories are kept with `.gitkeep`.
- SPI, PWM, ADC, LCD, touchscreen, USB, RTC, watchdog, audio, camera, Ethernet, and MMC are documented only as extension routes for now.
- The current Windows GCC cannot validate Linux/POSIX-specific user-space programs that include `poll.h`, `termios.h`, or `linux/i2c-dev.h`; validate those on Ubuntu or with `arm-linux-gnueabihf-gcc`.

## Stage 2 Day 1 GPIO/LED Baseline

- Board `192.168.77.200` is reachable from the current environment and SSH as `root` works in non-interactive mode.
- Day 1 was executed while the board was booted from SD/eMMC (`root=/dev/mmcblk1p2`), not NFS RootFS. This is acceptable for read-only GPIO/LED discovery, but Day 2 module deployment should preferably use NFS again.
- Current LED class exposes `mmc0::` and `mmc1::`, which are MMC trigger entries, not a clearly named user LED.
- GPIO debugfs shows `gpio133` exported as `sysfs`, output high. Since `gpiochip4` covers global GPIOs 128-159, `gpio133` maps to `GPIO5_IO05`.
- Sysfs toggling of `gpio133` succeeded (`1 -> 0 -> 1`), but local 100ask LED examples do not use GPIO5_IO05 as the LED pin.
- Local 100ask LED source and DTS examples consistently lock the LED to `GPIO5_IO03` / global `gpio131`: `GROUP_PIN(5, 3)`, `MX6ULL_PAD_SNVS_TAMPER3__GPIO5_IO03`, active-low control.
- Day 2 LED node should use `led-gpios = <&gpio5 3 GPIO_ACTIVE_LOW>;`, not GPIO5_IO05/gpio133.

## Stage 2 Day 2 LED Driver

- Ubuntu VM login works with user `liuzhiwei`; toolchain exists at `/opt/100ask/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin`, but this path is not exported by default.
- `led_test.c` cross-compiles successfully with `arm-linux-gnueabihf-gcc` and runs as an ARM 32-bit ELF on the board.
- The board-side `open: No such file or directory` from `led_test /dev/imx6ull_led 1` is expected until `imx6ull_led.ko` is built and loaded.
- The current blocker for `imx6ull_led.ko` is the missing matching Kernel build directory for board Kernel `4.9.88`.
- Board module symlink expects `/home/book/100ask_imx6ull-sdk/Linux-4.9.88`, but this source path is not available in the current Ubuntu VM or board rootfs.
- Local `linux-4.9.tar.xz` is generic `4.9.0`, not `4.9.88`; using it would risk `invalid module format` or ABI mismatch.
- User-provided `D:\user\browser\imx-linux4.9.88-master.zip` is the correct source baseline: it reports `VERSION=4`, `PATCHLEVEL=9`, `SUBLEVEL=88`, and includes `100ask_imx6ull_defconfig`.
- Old Kernel 4.9.88 source required two host-side compatibility fixes on Ubuntu 22.04:
  - Remove duplicate DTC lexer `YYLTYPE yylloc` definition.
  - Restore broken zip-extracted `include/dt-bindings/input/linux-event-codes.h` symlink by copying the actual `include/uapi/linux/input-event-codes.h`.
- `imx6ull_led.ko` now builds successfully with matching `4.9.88` vermagic.
- Test DTB deployment is risky: after replacing `/boot/100ask_imx6ull-14x14.dtb`, the board booted into Kernel/user-space but did not return to network and appeared stuck around udev timeout handling.
- Recovery succeeded by interrupting U-Boot, booting `/boot/zImage` with backup DTB `/boot/100ask_imx6ull-14x14.dtb.bak-`, adding `init=/bin/sh`, and copying the backup DTB back over `/boot/100ask_imx6ull-14x14.dtb`.
- After recovery, SSH and `eth0=192.168.77.200/24` are back. The board is usable again.
- Normal runtime after recovery exposes only `mmc0::` and `mmc1::` under `/sys/class/leds`; no clear user LED node is visible from sysfs.
- Day 2 should not directly overwrite the main DTB again. Use a conservative gpio131 validation path first, then move full DTB replacement/debugging into the Day 3 device-tree workflow with explicit rollback.
- DTB diff confirms the Day2 test DTB is not a minimal LED-only change: `diff-original-vs-day2.patch` has 1023 lines, original DTB is 38686 bytes, test DTB is 37825 bytes.
- Real semantic DTB differences include missing/replaced `gpio_keys_100ask`, generic `gpio-keys` changing from disabled to enabled, `ecspi@02008000` disabled, `adc@02198000` disabled, `uart6` disabled, and backlight PWM/brightness changes.
- Root cause is likely DTS baseline mismatch between the user-provided Kernel source package and the board's shipped `/boot/100ask_imx6ull-14x14.dtb`; the LED node itself is not proven to be the direct cause.
- The safe Day2 path succeeded after decompiling the restored original DTB, adding only `imx6ull_led`, recompiling a minimal test DTB, and booting it temporarily from U-Boot.
- The minimal test DTB differs from the original by only 17 diff lines and adds `demo,imx6ull-led` without changing the existing key/SPI/ADC/UART/backlight nodes.
- `imx6ull_led.ko` probes successfully against the minimal DTB and creates `/dev/imx6ull_led`; `led_test` controls the LED and dmesg records `led on/off/on`.
- `imx6ull_led: no symbol version for module_layout` appears during module insertion, but the module still loads and operates. Keep this as a follow-up Kernel build hygiene item rather than a Day2 blocker.
- After the minimal DTB was validated through temporary U-Boot boot, replacing the main `/boot/100ask_imx6ull-14x14.dtb` with that minimal DTB succeeded. Normal reboot, SSH, `eth0`, device-tree node discovery, module probe, and LED on/off tests all passed.
- The original main DTB is backed up as `/boot/100ask_imx6ull-14x14.dtb.before-day2-minimal` and `/boot/100ask_imx6ull-14x14.dtb.bak-`, providing a rollback path for later Day3 experiments.
- Day 3 should treat the current Day2 minimal DTB as the running baseline and focus on explaining and validating device-tree mechanics before adding KEY changes.
- Day 4 KEY work must account for existing board key nodes (`gpio_keys_100ask` and disabled `gpio-keys`) so the custom key IRQ driver does not fight an already-bound input/key driver.
- Day3 confirms the current main DTB is safe as the new baseline: compared with the original DTB, the diff is 17 lines and only adds `imx6ull_led`.
- Runtime KEY precheck shows `/dev/input` currently exposes `snvs-powerkey` and `goodix-ts`, while `gpio_keys_100ask` is present in DTB but does not appear as a standard input event device in the captured runtime state.
- Debugfs currently shows `gpio131` occupied by the Day2 LED driver and `gpio133` exported by sysfs; Day4 should check `gpio129`/other key GPIOs before binding a custom IRQ driver.
- Day4 KEY IRQ succeeded using `GPIO5_IO01/gpio129` with existing `key1_100ask` pinctrl. The custom node `compatible = "demo,imx6ull-key"` probes and maps to IRQ 208.
- Blocking read on `/dev/imx6ull_key` captured real key events: `pressed value=0` and `released value=1`.
- The main DTB now includes both Day2 LED and Day4 KEY nodes. The Day2-only main DTB is backed up as `/boot/100ask_imx6ull-14x14.dtb.before-day4-key`.
- Day5 KEY poll reuses the Day4 DT node and does not require another DTB change.
- Day4 `imx6ull_key.ko` and Day5 `imx6ull_key_poll.ko` cannot both bind the same `demo,imx6ull-key` platform device; rebooting to clear the old module was the cleanest test path.
- `key_poll_test /dev/imx6ull_key 2 60000` successfully captured poll events: `released value=1` and `pressed value=0`; IRQ 208 count increased under `imx6ull-key-poll`.
