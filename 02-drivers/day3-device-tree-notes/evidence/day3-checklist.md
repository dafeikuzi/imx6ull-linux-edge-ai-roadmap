# Day 3 Checklist

## 当前 DTB 状态

- [x] 记录 `/proc/cmdline`
- [x] 记录 `/proc/device-tree/model`
- [x] 记录 `/proc/device-tree/compatible`
- [x] 记录 `/proc/device-tree/imx6ull_led`
- [x] 记录 `/boot` 下 DTB 和备份文件

## 设备树知识

- [x] 解释 `compatible`
- [x] 解释 `status = "okay"`
- [x] 解释 `led-gpios`
- [x] 解释 `pinctrl-names`
- [x] 解释 `pinctrl-0`
- [x] 解释 `phandle`
- [x] 解释 `GPIO_ACTIVE_LOW`

## Diff 复盘

- [x] 解释 1023 行 bad diff 为什么危险
- [x] 解释 17 行 minimal diff 为什么安全
- [x] 写出以后判断 diff 是否安全的规则

## KEY 预研

- [x] 找到当前 key 相关节点
- [x] 记录 `gpio_keys_100ask`
- [x] 记录 `gpio-keys`
- [x] 判断 Day4 是否需要先禁用已有节点或另选 GPIO

## 收尾

- [x] 形成 Day3 summary
- [x] 更新 progress.md
- [x] 更新 findings.md
