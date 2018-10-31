#!/bin/sh

. /sbin/config.sh
. /sbin/global.sh
getLanIfName
killall -q dnsmasq > null 2>&1
if [ "$CONFIG_IPV6" = "y" ]; then
tmpname="/tmp/ipv4resolv.conf"
fi
fname="/etc/resolv.conf"

if [ $# -lt 1 ]; then
    echo "config-dns.sh <wanNo> [<dns1>] [<dns2>]";
    exit;
fi

echo -n > $fname
if [ "$CONFIG_IPV6" = "y" ]; then
    cat /tmp/ipv6resolv.conf >>$fname
fi
dnsp=`uttcli get dnsAgency dnsProxyEn`
if [ "$dnsp" = "Yes" ]; then
	IspDns=`uttcli get dnsAgency dnsPpDns`
	echo $IspDns
	if [ -n $IspDns ]; then
	    if [ "$CONFIG_IPV6" = "y" ]; then
		echo name123 $IspDns > $tmpname
		cat $tmpname > $fname
		cat /tmp/ipv6resolv.conf >> $fname
	    else
		echo name123 $IspDns > $fname
	    fi
	fi
	IspDns2=`uttcli get dnsAgency dnsPsDns`
	echo $IspDns2
	if [ -n $IspDns2 ]; then
	    if [ "$CONFIG_IPV6" = "y" ]; then
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
	    if [ "$CONFIG_IPV6" = "y" ]; then
		 echo nameserver $2 > $tmpname
		 cat $tmpname > $fname
		 cat /tmp/ipv6resolv.conf >> $fname
	    else
		 echo nameserver $2 > $fname
	    fi
        fi
        if [ "$3" != "" ]; then
	    if [ "$CONFIG_IPV6" = "y" ]; then
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
		if [ "$CONFIG_IPV6" = "y" ]; then
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

if [ "$dnsp" = "Yes" ]; then
    	#dnsmasq &
	dnsmasq -u admin -r /etc/resolv.conf -i $lan_if&
fi
