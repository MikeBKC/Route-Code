#!/bin/sh

. /sbin/global.sh

usage () {
	echo "usage: mdhcp.sh [option]..."
	echo "options:"
	echo "  -h						   : print this help"
	echo "  add interface dhcpPool     : start using dhcpPool in the interface"
	echo "  del interface              : stop using dhcpPool in the interface"
}

add () {
	file="/etc/udhcpd_vlan.conf"

	if [ -e "$file" ]; then
		echo -n >> $file
	else
		touch $file
	fi

	dhcp=$2
	if [ "$dhcp" != "" ]; then
		vid=`uttcli getbynm dhcpPool $dhcp vid`
		start=`uttcli getbynm dhcpPool $dhcp ipStart`
		end=`uttcli getbynm dhcpPool $dhcp ipEnd`
		mask=`uttcli getbynm dhcpPool $dhcp nm`
		gw=`uttcli getbynm dhcpPool $dhcp gw`
		pd=`uttcli getbynm dhcpPool $dhcp pDns`
		sd=`uttcli getbynm dhcpPool $dhcp sDns`
		lease=`uttcli getbynm dhcpPool $dhcp lease`
	fi

	tmp=0
	if [ $tmp = 1 ]; then
		echo "------------------"
		echo $start
		echo $end
		echo $mask
		echo $gw
		echo $pd
		echo $sd
		echo $lease
		echo "------------------"
	fi

	config-mdhcp.sh -v $1 $vid
	config-mdhcp.sh -s $1 $start
	config-mdhcp.sh -e $1 $end
	config-mdhcp.sh -m $1 $mask
	if [ "$pd" != "" -o "$sd" != "" ]; then
		config-mdhcp.sh -d $1 $pd $sd
	fi
	if [ "$gw" != "" ]; then
		config-mdhcp.sh -g $1 $gw
	fi
	if [ "$lease" != "" ]; then
		config-mdhcp.sh -t $1 $lease
	fi

	config-mdhcp.sh -r $1 $2
#	config-mdhcp.sh -i $1 $1
#	config-mdhcp.sh -p $1
}

del () {
	file="/var/run/udhcpd.pid"

	if [ -e $file ]; then
		kill `cat $file`
		rm -f $file
	fi

	rm -rf "/etc/udhcpd_vlan.conf"
}

case "$1" in
	"add") add "$2" "$3";;
	"del") del "$2";;
	"*")   usage;;
esac
