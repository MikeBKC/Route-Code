#!/bin/sh
#
# $Id: dhcp.sh,v 1.6.232.1 2017/12/26 15:31:41 wu.xiaoyu Exp $
#
# usage: dhcp.sh
#add by cxhu

. /sbin/global.sh
getLanIfName
config-udhcpd.sh -k
#echo "" > /var/udhcpd.leases
echo "" > /etc/udhcpd.conf
bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
# dhcp server
dhcp=`uttcli get dhcpServer DhcpSEn`
dnsp=`uttcli get dhcpServer dnsProxyEn`
if [ "$dhcp" = "Enable" ]; then
        start=`uttcli get dhcpServer ipStart`
        end=`uttcli get dhcpServer ipEnd`
        mask=`uttcli get dhcpServer nm`
        gw=`uttcli get dhcpServer gw`
        pd=`uttcli get dhcpServer pDns`
        sd=`uttcli get dhcpServer sDns`
        lease=`uttcli get dhcpServer lease`
        if [ "$dnsp" = "Enable" ]; then
            pd=$gw
        fi
        config-udhcpd.sh -s $start
        config-udhcpd.sh -e $end
        if [ $bridge_mode_flag == "1" ];then    
            config-udhcpd.sh -i br1
        else
            config-udhcpd.sh -i $lan_if
        fi
        config-udhcpd.sh -m $mask
        if [ "$pd" != "" -o "$sd" != "" ]; then
                config-udhcpd.sh -d $pd $sd
        fi
        if [ "$gw" != "" ]; then
                config-udhcpd.sh -g $gw
        fi
        if [ "$lease" != "" ]; then
                config-udhcpd.sh -t $lease
        fi
        config-udhcpd.sh -r 1
fi

config-dns.sh 0 0.0.0.0 0.0.0.0
