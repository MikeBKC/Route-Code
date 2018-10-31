#!/bin/sh
#
# $Id: internet.sh,v 1.1.2.1 2017/12/26 17:36:00 wu.xiaoyu Exp $
#
# usage: internet.sh
#

. /sbin/config.sh
. /sbin/global.sh

genSysFiles()
{
	pass=`uttcli getbynm adminConf admin password`
	if [ "$pass" != "" ]; then
	echo "admin::0:0:Adminstrator:/:/bin/sh" > /etc/passwd
	echo "admin:x:0:admin" > /etc/group
		chpasswd.sh admin $pass
	fi
}

bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
genSysFiles
usbmod_exist=`mount | grep dwc_otg | wc -l`
if [ $usbmod_exist == 0 ]; then
#insmod -q lm
#insmod -q dwc_otg
insmod /lib/modules/2.6.21/kernel/arch/mips/rt2880/lm.ko
insmod /lib/modules/2.6.21/kernel/drivers/usb/dwc_otg/dwc_otg.ko
fi
#get eth2_mac
pro_eth2_mac=`uttcli getbynm sysConf  sysConf eth2_mac`
if [ "$pro_eth2_mac" = "" ];then
eth2_mac=`ifconfig eth2 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'` 
uttcli set sysConf  sysConf  eth2_mac $eth2_mac
fi

#set vlan
ifconfig eth2 0.0.0.0
brctl addbr br1
if [ $bridge_mode_flag == "1" ];then
brctl addif br1 eth2
ifconfig br1 0.0.0.0
else
brctl addbr br0
config-vlan.sh
ifconfig eth2.1 0.0.0.0
ifconfig eth2.2 0.0.0.0
brctl addif br1 eth2.2
ifconfig br1 0.0.0.0
brctl addif br0 eth2.1
ifconfig br0 0.0.0.0
fi
echo "the number of wan is " $WanCount


ifconfig lo 127.0.0.1

#set bridge


#arp study control
echo 1 > /proc/sys/net/ipv4/conf/all/arp_ignore
#forward enable
echo 1 > /proc/sys/net/ipv4/ip_forward

#clear iptables chain
iptables --flush
iptables --flush -t nat
iptables --flush -t mangle
#link up 
#$mii_mgr -s -p 0 -r 0 -v 3100
#$mii_mgr -s -p 1 -r 0 -v 3100
#$mii_mgr -s -p 2 -r 0 -v 3100
#$mii_mgr -s -p 3 -r 0 -v 3100
#$mii_mgr -s -p 4 -r 0 -v 3100
