#!/bin/sh

. /sbin/config.sh

if [ "$CONFIG_RALINK_MT7620" != "" -o "$CONFIG_RALINK_MT7621" != "" -o "$CONFIG_RALINK_MT7628" != ""  ]; then
KERNEL_VER=2.6.36
else
KERNEL_VER=2.6.21
fi

#ȡ��/proc/bus/usb/devices�е�Vendor��ProdID
vend_id=`utt3gcat v`
prod_id=`utt3gcat p`
echo "vend_id is "$vend_id
echo "prod_id is "$prod_id

if [ "$vend_id" == "" -o "$prod_id" == "" ]; then
	exit
fi
#��Vendor��ProdID��ֵ�����»��������������õ�usbmodeswitchʱ�õ������ļ���
conf_file=$vend_id\_$prod_id
echo "conf_file is "$conf_file

#ȡ���ӿں�
index3g=`uttif gibn ttyUSB0`
echo "index3g is "$index3g

#����û�������
user=`uttcli get interface $index3g ethernet wan3g pppname`
pass=`uttcli get interface $index3g ethernet wan3g ppppasswd`
if [ "$user" == "" ]; then
	user=user
fi
if [ "$pass" == "" ]; then
	pass=pass
fi

echo "user is "$user
echo "pass is "$pass
#�ص�3g��pppd����
ppp-off $index3g

#ж��ģ��
rmmod fat
sleep 1
rmmod vfat
sleep 1
rmmod isofs
sleep 1
rmmod usbserial
sleep 1
rmmod usb-storage
sleep 1


#����ģʽ
connMode=`uttcli get interface $index3g ethernet connMode`
if [ "$connMode" != "PPPOE" ]; then
    echo "connMode != PPPOE,exit!!!"
    exit;
fi

#����ģ��
if [ "$vend_id" = "12d1" -a "$prod_id" = "1446" ] || [ "$vend_id" = "12d1" -a "$prod_id" = "1001" ] || [ "$vend_id" = "19d2" -a "$prod_id" = "2000" ]; then
#	insmod /lib/modules/2.6.21/kernel/fs/fat/fat.ko
#	insmod /lib/modules/2.6.21/kernel/fs/vfat/vfat.ko
#	insmod /lib/modules/2.6.21/kernel/fs/isofs/isofs.ko
	insmod /lib/modules/${KERNEL_VER}/kernel/fs/fat/fat.ko
    if [ "$CONFIG_RALINK_MT7620" != "" -o "$CONFIG_RALINK_MT7621" != "" -o "$CONFIG_RALINK_MT7628" != ""  ]; then
	insmod /lib/modules/${KERNEL_VER}/kernel/fs/fat/vfat.ko
    else
	insmod /lib/modules/${KERNEL_VER}/kernel/fs/vfat/vfat.ko
    fi
	insmod /lib/modules/${KERNEL_VER}/kernel/fs/isofs/isofs.ko
fi

if [ ! "$vend_id" = "12d1" -o ! "$prod_id" = "1001" ]; then
	insmod /lib/modules/${KERNEL_VER}/kernel/drivers/usb/storage/usb-storage.ko
	sleep 5
fi

#�ж�Ҫ��Ҫ��usb_modeswitch
#if [ "$vend_id" = "19d2" -a "$prod_id" = "2000" ]; then
#	echo "###ZTE-MF637U###"
#	usb_modeswitch_flag=n
#else
#        echo "###unknow###"
	usb_modeswitch_flag=y
	#exit
#fi


#״̬Ϊy������usb_modeswitch
if [ "$usb_modeswitch_flag" = "y" ]; then
	usb_modeswitch -v $vend_id -p $prod_id -c /etc_ro/usb/$conf_file
fi

sleep 5

if [ "$vend_id" = "19d2" -a "$prod_id" = "2000" ] || [ "$vend_id" = "19d2" -a "$prod_id" = "0031" ]; then
	target_vend_id=19d2
	target_prod_id=0031
else
#ת���󣬶���ת��֮���Vendor��ProdID���ر�Ŀ��ر���
	target_vend_id=`utt3gcat v`
	target_prod_id=`utt3gcat p`
fi

if [ ! "$target_vend_id" = "12d1" -o ! "$target_prod_id" = "1d09" ]; then
	if [ ! -e /dev/sr0 ]; then
	    rmmod usb-storage
	    sleep 1
	fi
	rmmod usbserial
	sleep 1
#����usbserial.ko ������ת��֮���Vendor��ProdID
	insmod /lib/modules/${KERNEL_VER}/kernel/drivers/usb/serial/usbserial.ko vendor=0x$target_vend_id product=0x$target_prod_id	
fi

#ʹ��ת�����ID�ж���Ϊ�˷�ttyUSB0���豸û�γ�ֻ�Ĳ���������£���ΪIDǰһ���Ѿ������ߵ�else��ѡ����ttyUSB0

#ȷ��tty�ӿ�
if [ "$target_vend_id" = "12d1" -a "$target_prod_id" = "1001" ]; then
	modem_f=ttyUSB0	
elif [ "$target_vend_id" = "12d1" -a "$target_prod_id" = "140c" ]; then
	modem_f=ttyUSB0
elif [ "$target_vend_id" = "12d1" -a "$target_prod_id" = "1d09" ]; then
	modem_f=ttyACM0
elif [ "$target_vend_id" = "1e0e" -a "$target_prod_id" = "ce1e" ]; then
	modem_f=ttyUSB2
elif [ "$target_vend_id" = "19d2" -a "$target_prod_id" = "0031" ]; then
if [ "$CONFIG_RALINK_MT7620" != "" -o "$CONFIG_RALINK_MT7621" != ""  -o "$CONFIG_RALINK_MT7628" != "" ]; then
modem_f=ttyUSB2
else
modem_f=ttyUSB3
fi
	if [ -e /dev/sr0 ]; then
		eject /dev/sr0
		rmmod usb-storage
		sleep 2
	fi
else
	modem_f=ttyUSB0
fi

#֮ǰ�����Ϻŵ����⣬���ַ���ת����ASCII��ʽ���ַ���
user=`netkeeper NORMAL $user`

echo "user is "$user
echo "pass is "$pass
echo "modem_f is "$modem_f

if [ "$vend_id" = "12d1" -a "$prod_id" = "1da1" ]; then
	insmod /lib/modules/${KERNEL_VER}/kernel/drivers/usb/class/cdc-acm.ko
	sleep 2
fi

#���ɲ�����Ҫ�������ļ�
config-3g-ppp.sh -p ${pass} -u ${user} -b 115200 -m $modem_f -c Generic_conn.scr -d Generic_disconn.scr
#���ţ�����ʹ��/etc/3g
pppd file /etc/3g & 
