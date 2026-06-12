#!/bin/sh

echo "## 1. 系统信息"
uname -a
cat /proc/cmdline

echo "## 2. LED class"
ls -ld /sys/class/leds/* 2>/dev/null || true
for led in /sys/class/leds/*; do
    [ -e "$led" ] || continue
    echo "LED=$led"
    printf "trigger="
    cat "$led/trigger" 2>/dev/null || true
    printf "brightness="
    cat "$led/brightness" 2>/dev/null || true
done

echo "## 3. GPIO class"
ls /sys/class/gpio 2>/dev/null || true

echo "## 4. gpio133 初始观察"
if [ -d /sys/class/gpio/gpio133 ]; then
    for f in direction value active_low edge; do
        printf "%s=" "$f"
        cat /sys/class/gpio/gpio133/$f 2>/dev/null || true
    done
fi

echo "## 5. gpio131 LED 候选观察"
if [ ! -d /sys/class/gpio/gpio131 ]; then
    echo 131 > /sys/class/gpio/export 2>/dev/null || true
fi

if [ -d /sys/class/gpio/gpio131 ]; then
    for f in direction value active_low edge; do
        printf "%s=" "$f"
        cat /sys/class/gpio/gpio131/$f 2>/dev/null || true
    done
fi

echo "## 6. dmesg 中 GPIO/LED/pinctrl 相关日志"
dmesg | grep -Ei "gpio|led|pinctrl" | tail -80

echo "## 7. debugfs GPIO 状态"
cat /sys/kernel/debug/gpio 2>/dev/null || true

