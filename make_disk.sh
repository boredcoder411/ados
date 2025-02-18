#!/bin/bash
set -e

# Ensure required files exist
if [[ ! -f bootsector.bin ]]; then
    echo "Error: bootsector.bin not found."
    exit 1
fi

if [[ ! -f t.wad ]]; then
    echo "Error: t.wad not found."
    exit 1
fi

# Making the disk image
IMAGE="image.img"
dd if=/dev/zero of=$IMAGE bs=1M count=10

# Write bootsector
dd if=bootsector.bin of=$IMAGE conv=notrunc

# Making partitions with sfdisk
sfdisk $IMAGE < partition_script

# Writing the boot code which is overwritten by sfdisk
dd if=bootsector.bin of=$IMAGE bs=446 count=1 conv=notrunc

# Setting up loop device
LOOP_DEV=$(sudo losetup --find --show $IMAGE)

# Forcing the kernel to update the partition list
sudo partprobe $LOOP_DEV

# Putting the filesystem on the partition
sudo dd if=t.wad of=${LOOP_DEV}p2

# Detaching the loop device
sudo losetup -d $LOOP_DEV

echo "Disk image created! Use xxd $IMAGE | less to inspect"
