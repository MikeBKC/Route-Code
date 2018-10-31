#!/usr/bin/env bash
# Copyright (C) 2006-2010 OpenWrt.org
set -x 
PATH=/data3/wang.bingrong/brwang/x86/backfire/staging_dir/host/bin:$PATH
[ $# == 5 ] || {
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image>"
    exit 1
}

#genext2fs -U -b 2000 -N 6000 -d ./x86root/ root.ext2
OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"

rm -f "$OUTPUT"

head=16
sect=63
cyl=$(( ($KERNELSIZE + $ROOTFSSIZE) * 1024 * 1024 / ($head * $sect * 512)))

# create partition table
#set `ptgen -o "$OUTPUT" -h $head -s $sect -p ${KERNELSIZE}m -p ${ROOTFSSIZE}m`
set `ptgen -o "$OUTPUT" -h $head -s $sect -p 29050k -p 50m  -p 50m -p 150m`

KERNELOFFSET="$(($1 / 512))"
KERNELSIZE="$(($2 / 512))"
#ROOTFSOFFSET="$(($3 / 512))"
#ROOTFSSIZE="$(($4 / 512))"

FROOTFSOFFSET="$(($5 / 512))"
FROOTFSSIZE="$(($6 / 512))"

BLOCKS="$((($KERNELSIZE / 2) - 1))"

[ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
#dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc
#
dd if="" of="$OUTPUT" bs=512 seek="$FROOTFSOFFSET" conv=notrunc
#dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$FROOTFSOFFSET" conv=notrunc

[ -n "$NOGRUB" ] && exit 0

#genext2fs -d "$KERNELDIR" -b "$BLOCKS" "$OUTPUT.kernel"
genext2fs -d "$KERNELDIR" -b 10240 "$OUTPUT.kernel"
dd if="$OUTPUT.kernel" of="$OUTPUT" bs=512 seek="$KERNELOFFSET" conv=notrunc

#rm -f "$OUTPUT.kernel"

which chpax >/dev/null && chpax -zp $(which grub)
#geometry (hd0) $cyl $head $sect
grub --batch --no-curses --no-floppy --device-map=/dev/null <<EOF
device (hd0) $OUTPUT
geometry (hd0)
root (hd0,0)
setup (hd0)
quit
EOF

#qemu-img convert -f raw -O vmdk $OUTPUT $OUTPUT.vmdk
qemu-img convert -f raw -O vmdk $OUTPUT linux.vmdk
