#!/bin/sh
#
# $Id: lan.sh,v 1.16.138.3.4.3 2017/12/26 14:00:41 wu.xiaoyu Exp $
#
# usage: wan.sh
#

. /sbin/global.sh

bridge_mode_init()
{
    bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
    if [ $bridge_mode_flag == "1" ];then
        bridge_vif.sh add
        new_ip=`uttcli get interface 0 ethernet static ip`
        killall telnetd
        telnetd -b $new_ip
    fi
}

if [ "$1" == "bridge" ]; then
    bridge_mode_init
    exit
fi
killall pppoe-server
sleep 3

getWanIfName 1
getLanIfName
# stop all
#killall -q udhcpd
killall -q upnpd > null 2>&1
#killall -q dnsmasq

# ip address
ip=`uttcli get interface 0 ethernet static ip`
nm=`uttcli get interface 0 ethernet static nm`
ip2=`uttcli get interface 0 ethernet static ip2`
nm2=`uttcli get interface 0 ethernet static nm2`
ip3=`uttcli get interface 0 ethernet static ip3`
nm3=`uttcli get interface 0 ethernet static nm3`
ip4=`uttcli get interface 0 ethernet static ip4`
nm4=`uttcli get interface 0 ethernet static nm4`
mac=`uttcli get interface 0 ethernet mac`
hostn=`uttcli get_hostname`
lan_if2="${lan_if}:2";
lan_if3="${lan_if}:3";
lan_if4="${lan_if}:4";
ifconfig $lan_if $ip netmask $nm
if [ "$ip2" != "0.0.0.0" ];then
ifconfig $lan_if2 $ip2 netmask $nm2
elif [ "$ip2" == "0.0.0.0" ];then
lan2_exsit=`ifconfig|grep $lan_if2`
if [ "$lan2_exsit" != "" ];then
ifconfig $lan_if2 down
fi
fi
if [ "$ip3" != "0.0.0.0" ];then
ifconfig $lan_if3 $ip3 netmask $nm3
elif [ "$ip3" == "0.0.0.0" ];then
lan3_exsit=`ifconfig|grep $lan_if3`
if [ "$lan3_exsit" != "" ];then
ifconfig $lan_if3 down
fi
fi
if [ "$ip4" != "0.0.0.0" ];then
ifconfig $lan_if4 $ip4 netmask $nm4
elif [ "$ip4" == "0.0.0.0" ];then
lan4_exsit=`ifconfig|grep $lan_if4`
if [ "$lan4_exsit" != "" ];then
ifconfig $lan_if4 down
fi
fi
bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
if [ $bridge_mode_flag == "0" ];then
if [ "$mac" != "00:00:00:00:00:00" ]; then
    if [ "$lan_if" == "br0" ]; then #ÎÞÏß
	ifconfig eth2.1 down 
	ifconfig eth2.1 hw ether $mac up
    fi
    ifconfig $lan_if down 
    ifconfig $lan_if hw ether $mac up
fi
fi
    snd2speed up 0 &
killall goahead
sleep 1
goahead &

# hostname
hostname $hostn

killall telnetd
telnetd -b $ip

echo "127.0.0.1 localhost.localdomain localhost" > /etc/hosts

# connlimit lan ip
lan_ip=$1
if [ "$lan_ip" != "" ];then
    echo $lan_ip > /proc/sys/net/netfilter/utt_local_address
fi
lan_ifname=`uttif gnbi 0`
echo $lan_ifname > /proc/sys/net/netfilter/utt_local_ifname


route add -host 255.255.255.255 dev $lan_if
