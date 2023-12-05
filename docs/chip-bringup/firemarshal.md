# FireMarshal



First, set up Chipyard following this tutorial:

{% content-ref url="../chipyard/setting-up-chipyard/setting-up-chipyard-on-ubuntu.md" %}
[setting-up-chipyard-on-ubuntu.md](../chipyard/setting-up-chipyard/setting-up-chipyard-on-ubuntu.md)
{% endcontent-ref %}



```bash
sudo chmod +r /boot/vmlinuz-*
```

[https://askubuntu.com/questions/1046828/how-to-run-libguestfs-tools-tools-such-as-virt-make-fs-without-sudo](https://askubuntu.com/questions/1046828/how-to-run-libguestfs-tools-tools-such-as-virt-make-fs-without-sudo)







```bash
cd $chipyard/
source ./env.sh
cd $chipyard/software/firemarshal/
```



```bash
./init-submodules.sh
```



build the thing

```bash
./marshal build br-base.json
```



launch in QEMU

```bash
./marshal launch br-base.json
```





build for vcu118

edit `$chipyard/software/firemarshal/marshal-config.yaml` to have the following content

```yaml
firesim-dir: "../../sims/firesim/"
board-dir: "boards/prototype/"
```



build

```bash
./marshal -v -d build br-base.json # here the -d indicates --nodisk or initramfs
```



generate binary

```bash
./marshal -v -d install -t prototype br-base.json
```





