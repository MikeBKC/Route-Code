#!/bin/sh
. /sbin/config.sh
genDevNode()
{
	mounted=`mount | grep mdev | wc -l`
	if [ $mounted -eq 0 ]; then
	mount -t ramfs mdev /dev
	mkdir /dev/pts
	mount -t devpts devpts /dev/pts

	if [ "$CONFIG_RALINK_RT2880" != "" ];then
	mknod   /dev/console     c       5 1
	mknod   /dev/mem         c       1 1
	mknod   /dev/null        c       1 3
	mknod   /dev/ttyp0        c	3   0	
	mknod	/dev/ttyp1        c	3   1
	mknod	/dev/tty0         c	4   0	
	mknod	/dev/tty1         c	4   1

	mknod   /dev/ptyp0   c   2   0
	mknod   /dev/ptyp1   c   2   1
	mknod   /dev/ptmx    c   5   2   

        fi
        mknod   /dev/video0      c       81      0
        mknod   /dev/spiS0       c       217     0
        mknod   /dev/i2cM0       c       218     0
	if [ "$CONFIG_RALINK_MT7620" = "y" -o "$CONFIG_RALINK_MT7621" = "y" -o "$CONFIG_RALINK_MT7628" = "y" ]; then
        mknod   /dev/rdm0        c       253     0
	else
        mknod   /dev/rdm0        c       254     0
        fi
        mknod   /dev/flash0      c       200     0
        mknod   /dev/swnat0      c       210     0
        mknod   /dev/hwnat0      c       220     0
        mknod   /dev/acl0        c       230     0
        mknod   /dev/ac0         c       240     0
        mknod   /dev/mtr0        c       250     0
        mknod   /dev/gpio        c       252     0
	mknod   /dev/PCM         c       233     0
	mknod   /dev/I2S         c       234     0

	if [ "$CONFIG_RALINK_RT2880" != "" ];then
	mknod   /dev/mtdblock0 b  31 0 
	mknod   /dev/mtdblock1 b  31 1 
	mknod   /dev/mtdblock2 b  31 2 
	mknod   /dev/mtdblock3 b  31 3 
	mknod   /dev/mtdblock4 b  31 4 
	mknod   /dev/mtd0      c  90 0 
	mknod   /dev/mtd0ro    c  90 1 
	mknod   /dev/mtd1      c  90 2 
	mknod   /dev/mtd1ro    c  90 3 
	mknod   /dev/mtd2      c  90 4 
	mknod   /dev/mtd2ro    c  90 5 
	mknod   /dev/mtd3      c  90 6 
	mknod   /dev/mtd3ro    c  90 7 
	mknod   /dev/mtd4      c  90 8 
	mknod   /dev/mtd4ro    c  90 9 
	fi
	fi

	echo "# <device regex> <uid>:<gid> <octal permissions> [<@|$|*> <command>]" > /etc/mdev.conf
        echo "# The special characters have the meaning:" >> /etc/mdev.conf
        echo "# @ Run after creating the device." >> /etc/mdev.conf
        echo "# $ Run before removing the device." >> /etc/mdev.conf
        echo "# * Run both after creating and before removing the device." >> /etc/mdev.conf
        echo "sd[a-z].* 0:0 0660 */sbin/automount_boot.sh \$MDEV" >> /etc/mdev.conf
        echo "mmcblk[0-9]p[1-9] 0:0 0660 */sbin/automount_boot.sh \$MDEV" >> /etc/mdev.conf

        #enable usb hot-plug feature
        echo "/sbin/mdev" > /proc/sys/kernel/hotplug
}

setMDEV()
{
#Linux2.6 uses udev instead of devfs, we have to create static dev node by myself.
	echo "# <device regex> <uid>:<gid> <octal permissions> [<@|$|*> <command>]" > /etc/mdev.conf
        echo "# The special characters have the meaning:" >> /etc/mdev.conf
        echo "# @ Run after creating the device." >> /etc/mdev.conf
        echo "# $ Run before removing the device." >> /etc/mdev.conf
        echo "# * Run both after creating and before removing the device." >> /etc/mdev.conf
        echo "sd[a-z].* 0:0 0660 */sbin/automount.sh \$MDEV" >> /etc/mdev.conf
	echo "mmcblk[0-9]p[0-9] 0:0 0660 */sbin/automount.sh \$MDEV" >> /etc/mdev.conf

        #enable usb hot-plug feature
        echo "/sbin/mdev" > /proc/sys/kernel/hotplug
}

genDevNode
mdev -s
setMDEV


if [ "$CONFIG_BOARD_SYS_LED_GPIO" != "" -a "$CONFIG_BOARD_SYS_LED_GPIO" != "256" ]; then
    gpio l $CONFIG_BOARD_SYS_LED_GPIO 4000 0 0 0 0&
else

if [ "$CONFIG_IP1725" = "y" -o "$CONFIG_RALINK_RT3883" = "y" -o "$CONFIG_UTT_WX100" = "y" -o "$CONFIG_UTT_NV510V4" = "y" ]; then
if [ "$CONFIG_UTT_NV840E" = "y" -o "$CONFIG_UTT_NV840D" = "y" -o "$CONFIG_UTT_NV800" = "y" ]; then
gpio l 13 2 2 4000 0 1&
else
gpio l 9 2 2 4000 0 1&
fi
else
if [ "$CONFIG_RALINK_RT2880" = "y" ]; then
gpio l 12 2 2 4000 0 1&
else
if [ "$CONFIG_RALINK_MT7620" = "y" ]; then

if [ "$CONFIG_BOARD_MTK7620_E" = "y" ]; then
gpio l 8 2 2 4000 0 1&
elif [ "$CONFIG_BOARD_MTK7620_EN" = "y" ]; then
gpio l 8 2 2 4000 0 1&
elif [ "$CONFIG_BOARD_MTK7620_H" = "y" ]; then
gpio l 8 2 2 4000 0 1&
elif [ "$CONFIG_BOARD_MTK7620_K" = "y" ]; then
gpio l 8 2 2 4000 0 1&

elif [ "$CONFIG_BOARD_MTK7620_F" = "y" ]; then
gpio l 39 2 2 4000 0 1&
elif [ "$CONFIG_BOARD_MTK7620_G" = "y" ]; then
gpio l 39 2 2 4000 0 1&
elif [ "$CONFIG_BOARD_MTK7620_I" = "y" ]; then
gpio l 0 2 2 4000 0 1&
else
gpio l 8 2 2 4000 0 1&
fi

else
if [ "$CONFIG_PPC_85xx" != "y" ]; then
gpio l 13 2 2 4000 0 1&
fi
if [ "$CONFIG_RALINK_MT7621" = "y" ];then
gpio l 24 2 2 4000 0 1&
fi
if [ "$CONFIG_BOARD_MTK7628_AN" = "y" ];then
gpio l 2 2 2 4000 0 1&
fi
fi
fi
fi

fi


