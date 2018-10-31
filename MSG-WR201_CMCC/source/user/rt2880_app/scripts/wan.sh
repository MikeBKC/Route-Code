#!/bin/sh
#
# $Id: wan.sh,v 1.21.34.7 2017/12/26 13:53:35 wu.xiaoyu Exp $
#
# usage: wan.sh <wanIndex>
#

. /sbin/global.sh
. /sbin/config.sh

switch_restore()
{
    switch reg w 14 5555
    switch reg w 40 1001
    switch reg w 44 1001
    switch reg w 48 1001
    switch reg w 4c 1
    switch reg w 50 2001
    switch reg w 70 ffffffff
    switch reg w 98 7f7f
    switch reg w e4 7f
    switch clear
}

if [ $# -ne 1 ]; then
    echo "Usage: wan.sh <wanIndex>"
    exit;
fi

# parameter controls
wanIndex=$1
if [ $wanIndex -gt 6 ] || [ $wanIndex -lt 1 ]; then
    echo "parameter error.WanNo:$wanIndex"
    exit;
fi

getWanIfName $wanIndex

bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`

# init status files 
if [ -r "/var/run/linkcheck${wanIndex}.pid" ]; then
    kill -9 `cat /var/run/linkcheck${wanIndex}.pid` > null 2>&1
fi
echo 0 >/var/run/Wan${wanIndex}link.status
touch /var/run/wan${wanIndex}.dns

ppp-off $wanIndex
index3g=`uttif gibn ttyUSB0`
if [ "$wanIndex" = "$index3g" ]; then
    3g.sh &
    exit 0
fi

udhcpc-down.sh $wanIndex
echo $wanIndex > /proc/sys/net/ipv4/conf/$wan_if/if_index
tc qdisc del dev $wan_if root
if [ "$NV1800N" == "" ]; then
    sleep 4
fi

active=`uttcli get interface $wanIndex active`
if [ "$active" = "No" ]; then
    exit 0
fi

wanmode=`uttcli get interface $wanIndex ethernet connMode`
if [ "$wanmode" = "" ]; then
    exit 0
fi

# down and up, Route lists on this port would be lost now.

ifconfig $wan_if 0.0.0.0
mac=`uttcli get interface $wanIndex ethernet mac | sed -e 's/[-:]//g'`
if [ "$mac" != "" ]; then
curMac=`ifconfig $wan_if | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'`
curMac=`echo $curMac | sed -e 's/[-:]//g'`
if [ "`echo $curMac | grep -i $mac`" == "" ]; then
# mac change
    ifconfig $wan_if down
    ifconfig $wan_if hw ether $mac
    ifconfig eth2.2 down
    ifconfig eth2.2 hw ether $mac up
# ar8316 wan port speed set, such as model 6530G
    uttportset $wanIndex
    ifconfig $wan_if up
fi
fi

if [ $bridge_mode_flag == "1" ];then
    newip=`uttcli get interface 0 ethernet static ip`
    ifconfig eth2 down 
    ifconfig eth2 hw ether $mac up
    switch_restore
    bridge_vif.sh add
    iptables -t mangle -F
fi

if [ "$wanmode" = "STATIC" ]; then
    ip=`uttcli get interface $wanIndex ethernet static ip`
    nm=`uttcli get interface $wanIndex ethernet static nm`
    gw=`uttcli get interface $wanIndex ethernet static gw`
    pd=`uttcli get interface $wanIndex ethernet static pdns`
    sd=`uttcli get interface $wanIndex ethernet static sdns`
    
    ifconfig $wan_if $ip netmask $nm
    wanchkip=`uttcli get interface $wanIndex detectIp`
    wanchkrate=`uttcli get interface $wanIndex txBand`
    if [ "$wanchkrate" != "0" ]  && [ "${wanchkip}" == "0.0.0.0" ]; then
	wanchkip=$gw
    fi
    if [ "${wanchkip}" != "0.0.0.0" ]; then
        ip route replace table `expr 90 + $wanIndex` ${wanchkip}/32 via $gw dev $wan_if
    fi

    echo nameserver $pd > /var/run/wan$wanIndex.dns
    echo nameserver $sd >>/var/run/wan$wanIndex.dns

    snd2speed up $wanIndex
    if [ -r "/var/run/linkcheck${wanIndex}.pid" ]; then
        kill -9 `cat /var/run/linkcheck${wanIndex}.pid`
    fi

    cat /proc/uptime > /var/run/wan${wanIndex}.uptime
elif [ "$wanmode" = "DHCP" ]; then
    export dhcpPid=/var/run/udhcpc_wan$wanIndex.pid
    udhcpc -i $wan_if -s /sbin/udhcpc.sh -p $dhcpPid -R -f&
elif [ "$wanmode" = "PPPOE" ]; then
    u=`uttcli get interface $wanIndex ethernet pppoe user`
    dailMode=`uttcli get interface $wanIndex ethernet pppoe spDialMode`
    u=`netkeeper $dailMode $u`

    pw=`uttcli get interface $wanIndex ethernet pppoe passwd`
    pppoe_opmode=`uttcli get interface $wanIndex ethernet pppoe pppConnMode`
    authmode=`uttcli get interface $wanIndex ethernet pppoe authType`
    mtu=`uttcli get interface $wanIndex ethernet pppoe mtu`

    if [ -r "/etc/wan$wanIndex.lock" ];then
        echo "conflict wan.sh, exit."
        exit;        
    fi                   
    touch /etc/wan$wanIndex.lock

    if [ "$pppoe_opmode" = "" ]; then
    	echo "pppoecd $wan_if -u $u -p $pw"
    	pppoecd $wan_if -u "$u" -p "$pw"
    else
        pppoe_optime=`uttcli get interface $wanIndex ethernet pppoe idleTime`
    	config-pppoe.sh $wanIndex $u $pw $wan_if $pppoe_opmode $authmode $mtu $pppoe_optime
    fi
    rm /etc/wan$wanIndex.lock 
else
	echo "wan.sh: unknown wan connection type: $wanmode"
	exit 1
fi

