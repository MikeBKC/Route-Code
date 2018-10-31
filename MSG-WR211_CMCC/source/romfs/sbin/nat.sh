#!/bin/sh
#
# $Id: nat.sh,v 1.10 2014/12/29 11:28:21 xiao.yunyang Exp $
#
# usage: nat.sh <wanNo> <A(dd)|D(el)>
#

. /sbin/global.sh

wanNo=$1
action=$2
lan_ip=`uttcli get interface 0 ethernet static ip`
wan_ip=`uttcli get interface ${wanNo} ethernet static ip`
nat_en=`uttcli get interface ${wanNo} NatActive`
wanmode=`uttcli get interface ${wanNo} ethernet connMode`
IdentifyBindEn=`uttcli get sysConf IdentifyBindEn`
natsess=`uttmachines NATSESSIONS`
getPppIfName $wanNo
echo 1 > /proc/sys/net/ipv4/ip_forward

	echo 30 > /proc/sys/net/ipv4/netfilter/ip_conntrack_icmp_timeout
	echo 10 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout
	echo 1200 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established
	echo $natsess > /proc/sys/net/nf_conntrack_max
	echo $natsess > /proc/sys/net/ipv4/netfilter/ip_conntrack_max
        if [ "$wanmode" = "PPPOE" ]; then
                pppoeip=`cat /var/run/$wan_if.ppp|sed "s/ /\n/g"|sed -n "4p"`
        fi
	if [ "$wanmode" = "PPPOE" -o "$wanmode" = "L2TP" -o "$wanmode" = "PPTP" -o "$wanmode" = "3G" ]; then
		wan_if=`cat /var/run/$wan_if.ppp|sed "s/ /\n/g" |grep ppp`
        fi

        if [ "${action}" = "D" ]; then
            iptables -t mangle -F connmark_wan${wanNo}
	    iptables -t nat -F wan${wanNo}_default_nat
        elif [ "${action}" = "A" ]; then
            iptables -t mangle -F connmark_wan${wanNo}
	    iptables -t nat -F wan${wanNo}_default_nat
if [ "$nat_en" = "1" ]; then
	    if [ "$wanmode" = "STATIC" ]; then
		iptables -t nat -A wan${wanNo}_default_nat -o $wan_if -j SNAT --to $wan_ip
	    elif [ "$wanmode" = "PPPOE" ]; then
                iptables -t nat -A wan${wanNo}_default_nat -o $wan_if -j SNAT --to $pppoeip
	    else
		iptables -t nat -A wan${wanNo}_default_nat -o $wan_if -j MASQUERADE
	    fi
fi
            iptables -t mangle -A connmark_wan${wanNo} -i $wan_if -m conntrack  --ctstate NEW  -j CONNMARK --set-mark 0x${wanNo}0/0xff
	    if [ "$IdentifyBindEn" = "Enable" ]; then
		iptables -t mangle -A connmark_wan${wanNo} -o $wan_if -p tcp --dport 443 -m conntrack  --ctstate NEW  -j CONNMARK --set-mark 0x${wanNo}0/0xff
		iptables -t mangle -A connmark_wan${wanNo} -o $wan_if -p udp -m multiport --dport 4000,8000,8001 -m conntrack  --ctstate NEW  -j CONNMARK --set-mark 0x${wanNo}0/0xff
	    fi
        fi

