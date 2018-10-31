#!/bin/sh
#
# $Id: config-l2tp.sh,v 1.1.1.1 2011/08/17 11:10:39 brwang Exp $
#
# usage: config-l2tp.sh <mode> <mode_params> <server> <user> <password>
#

. /sbin/config.sh
. /sbin/global.sh

usage()
{
	echo "Usage:"
	echo "  $0 <mode> <mode_params> <server> <user> <password>"
	echo "Modes:"
	echo "  static - <mode_params> = <wan_if_name> <wan_ip> <wan_netmask> <gateway>"
	echo "  dhcp - <mode_params> = <wan_if_name>"
	echo "Example:"
	echo "  $0 static eth2.2 10.10.10.254 255.255.255.0 10.10.10.253 192.168.1.1 user pass"
	echo "  $0 dhcp eth2.2 192.168.1.1 user pass"
	exit 1
}

if [ "$5" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

if [ "$1" = "static" ]; then
	if [ "$7" = "" ]; then
		echo "$0: insufficient arguments"
		usage $0
	fi
	ifconfig $2 $3 netmask $4
	route del default
	if [ "$5" != "0.0.0.0" ]; then
		route add default gw $5
	fi
	l2tp_srv=$6
	l2tp_u=$7
	l2tp_pw=$8
	l2tp_opmode=$9
	l2tp_optime=${10}
elif [ "$1" = "dhcp" ]; then
	killall -q udhcpc
	udhcpc -i $2 -s /sbin/udhcpc.sh -p /var/run/udhcpd.pid &
	l2tp_srv=$3
	l2tp_u=$4
	l2tp_pw=$5
	l2tp_opmode=$6
	l2tp_optime=$7
else
	echo "$0: unknown connection mode: $1"
	usage $0
fi

if [ "$CONFIG_PPPOL2TP" == "y" ]; then
rm -f /var/run/openl2tpd.pid
openl2tp.sh $l2tp_u $l2tp_pw $l2tp_srv $l2tp_opmode $l2tp_optime
openl2tpd
else
l2tp.sh $l2tp_u $l2tp_pw $l2tp_srv $l2tp_opmode $l2tp_optime
l2tpd
sleep 1
l2tp-control "start-session $l2tp_srv"
fi


