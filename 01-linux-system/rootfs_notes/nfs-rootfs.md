# NFS RootFS Notes

## PC 侧

```bash
sudo apt install nfs-kernel-server
sudo mkdir -p /nfs/imx6ull-rootfs
sudo chown -R $USER:$USER /nfs/imx6ull-rootfs
echo "/nfs/imx6ull-rootfs *(rw,sync,no_root_squash,no_subtree_check)" | sudo tee -a /etc/exports
sudo exportfs -ra
sudo systemctl restart nfs-kernel-server
```

## U-Boot bootargs 模板

```text
console=ttymxc0,115200 root=/dev/nfs nfsroot=<PC_IP>:/nfs/imx6ull-rootfs,v3,tcp ip=<BOARD_IP>:<PC_IP>:<GATEWAY_IP>:255.255.255.0::eth0:off
```

把 `<PC_IP>`、`<BOARD_IP>`、`<GATEWAY_IP>` 替换成真实网络环境。

