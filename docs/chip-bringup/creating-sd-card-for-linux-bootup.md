# Creating SD Card for Linux Bootup

Recommend using this SanDisk 32GB 32G Ultra Micro SD HC Class 10 TF Flash SDHC Memory Card - SDSQUNB-032G-GN3MN micro SD card:

{% embed url="https://www.amazon.com/gp/product/B010NE3QHQ" %}

Insert the SD card to a Ubuntu host machine. Run the following command to list all disks

```bash
sudo fdisk -l
```

Find the target SD card and note down the drive path. Here, it's `/dev/sda/` for this 32GB SD card.

<figure><img src="../.gitbook/assets/image (1).png" alt=""><figcaption></figcaption></figure>



## Removing exiting partition

Remove all existing partitions on this card by using `sudo gdisk <path_to_the_card>`.

(`d` to delete existing sector, `w` to apply setting)

```bash
> sudo gdisk /dev/sda
GPT fdisk (gdisk) version 1.0.8

Partition table scan:
  MBR: protective
  BSD: not present
  APM: not present
  GPT: present

Command (? for help): d
Partition number (1-2): 1

Command (? for help): d
Using 2

Command (? for help): w

Final checks complete. About to write GPT data. THIS WILL OVERWRITE EXISTING PARTITIONS!!

Do you want to proceed? (Y/N): Y
OK; writing new GUID partition table (GPT) to /dev/sda.
Warning: The kernel is still using the old partition table.
The new table will be used at the next reboot or after you
run partprobe(8) or kpartx(8)
The operation has completed successfully.

```



## Wiping existing partition table

( `x` to enter expert mode, `z` to destroy GPT data structures and exit)

```bash
> sudo gdisk /dev/sda
GPT fdisk (gdisk) version 1.0.8

Partition table scan:
  MBR: protective
  BSD: not present
  APM: not present
  GPT: present

Found valid GPT with protective MBR; using GPT.

Command (? for help): x

Expert command (? for help): z
About to wipe out GPT on /dev/sda. Proceed? (Y/N): Y
GPT data structures destroyed! You may now partition the disk using fdisk or
other utilities.
Blank out MBR? (Y/N): Y
```



## Creating new partition table

(`d` to delete existing sector, `w` to apply setting)

```bash
> sudo gdisk /dev/sda
GPT fdisk (gdisk) version 1.0.8

Partition table scan:
  MBR: not present
  BSD: not present
  APM: not present
  GPT: not present

Creating new GPT entries in memory.

Command (? for help): o
This option deletes all partitions and creates a new protective MBR.
Proceed? (Y/N): Y

Command (? for help): w

Final checks complete. About to write GPT data. THIS WILL OVERWRITE EXISTING
PARTITIONS!!

Do you want to proceed? (Y/N): Y
OK; writing new GUID partition table (GPT) to /dev/sda.
The operation has completed successfully.
```



## Creating new partition

we need to enable expert mode to be able to create partition strictly at sector 34

For the ending sector, it depends on how large the Linux image is.

(`x` to enter expert mode. `l` to change alignment. `1` to set it to 1 sector. `m` to return to the main menu.)

```bash
> sudo gdisk /dev/sda
GPT fdisk (gdisk) version 1.0.8

Partition table scan:
  MBR: protective
  BSD: not present
  APM: not present
  GPT: present

Found valid GPT with protective MBR; using GPT.

Command (? for help): x

Expert command (? for help): l
Enter the sector alignment value (1-65536, default = 2048): 1

Expert command (? for help): m

Command (? for help): n
Partition number (1-128, default 1): 1
First sector (34-11721045134, default = 2048) or {+-}size{KMGTP}: 34
Last sector (2048-11721045134, default = 11721045134) or {+-}size{KMGTP}: 65537
Current type is 8300 (Linux filesystem)
Hex code or GUID (L to show codes, Enter = 8300): 700
Changed type of partition to 'Microsoft basic data'

Command (? for help): w

Final checks complete. About to write GPT data. THIS WILL OVERWRITE EXISTING PARTITIONS!!

Do you want to proceed? (Y/N): Y
OK; writing new GUID partition table (GPT) to /dev/sda.
The operation has completed successfully.

```

The first section then can be used to load the Linux binary image.

Use `dd` to transfer the binary content directly.

```bash
sudo dd if=~/Desktop/eaglex_bringup/eaglex_bbl.bin of=/dev/sda1

32768+0 records in
32768+0 records out
16777216 bytes (17 MB, 16 MiB) copied, 3.21026 s, 5.2 MB/s

```

## Additional Info

Here's a list of properties of Windows file system

1 is the raw Microsoft basic data sector we created above

2 is FAT

3 is FAT32

4 is NTFS

<figure><img src="../.gitbook/assets/image.png" alt=""><figcaption></figcaption></figure>



