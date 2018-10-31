#!/bin/sh
# $Id: config-dns.sh,v 1.10.26.2 2015/09/23 10:45:53 cao.yongxiang Exp $
# modifed by bchen 
# add "$IspDns" != "0.0.0.0"
. /sbin/config.sh
if [ "$CONFIG_IPV6" = "y" ];then
    tmpname="/tmp/ipv4resolv.conf"
fi
fname="/etc/resolv.conf"
fbak="/etc/resolv_conf.bak"

if [ $# -lt 1 ]; then
    echo "config-dns.sh <wanNo> [<dns1>] [<dns2>]";
    exit;
fi

echo -n  > $fname

if [ "$CONFIG_IPV6" = "y" ];then
    cat /tmp/ipv6resolv.conf >> $fname
fi
dnsp=`uttcli get dhcpServer dnsProxyEn`
if [ "$dnsp" = "Enable" ]; then
    IspDns=`uttcli get dhcpServer dnsPpDns`
    if [ -n $IspDns -a "$IspDns" != "0.0.0.0" ]; then
	if [ "$CONFIG_IPV6" = "y" ];then
	    echo nameserver $IspDns > $tmpname
	    cat $tmpname > $fname
	    cat /tmp/ipv6resolv.conf >> $fname
	else
	    echo nameserver $IspDns > $fname
        fi
    fi
    IspDns2=`uttcli get dhcpServer dnsPsDns`
    if [ -n $IspDns2 -a "$IspDns2" != "0.0.0.0" ]; then
	if [ "$CONFIG_IPV6" = "y" ];then
	    echo nameserver $IspDns2 >> $tmpname
	    cat $tmpname > $fname
	    cat /tmp/ipv6resolv.conf >> $fname
	else
	    echo nameserver $IspDns2 >> $fname
        fi

    fi
fi

wanCount=`uttcli get sysConf wanIfCount`
wanindex=1
while [ $wanindex -le $wanCount ]; do
    linkStatusF=/var/run/Wan${wanindex}link.status
    if [ $wanindex == $1 ]; then
        if [ "$2" != "" ]; then
	    if [ "$CONFIG_IPV6" = "y" ];then
		echo nameserver $2 > $tmpname
		cat $tmpname > $fname
		cat /tmp/ipv6resolv.conf >> $fname
	    else
		echo nameserver $2 > $fname
	    fi

        fi
        if [ "$3" != "" ]; then
	    if [ "$CONFIG_IPV6" = "y" ];then
		echo nameserver $3 >> $tmpname
		cat $tmpname > $fname
		cat /tmp/ipv6resolv.conf >> $fname
	    else
		echo nameserver $3 >> $fname
	    fi

        fi
    else
        if [ -s $linkStatusF ]; then
            linkstatus=`cat /var/run/Wan${wanindex}link.status`
            if [ $linkstatus == "1" ]; then
		if [ "$CONFIG_IPV6" = "y" ];then
		    cat /var/run/wan$wanindex.dns > $tmpname
		    cat $tmpname > $fname
		    cat /tmp/ipv6resolv.conf >> $fname
		else
		    cat /var/run/wan$wanindex.dns > $fname
		fi
            fi
        fi
    fi
    wanindex=`expr $wanindex + 1`
done
