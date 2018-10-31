#!/bin/sh
#
# $Id: config-pppoe.sh,v 1.1.1.1 2011/08/17 11:10:39 brwang Exp $
#
# usage: config-pppoe.sh <user> <password> <wan_if_name>
#

usage()
{
	echo "Usage:"
	echo "  $0 <user> <password> <wan_if_name>"
	exit 1
}

if [ "$5" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

export PPP_OPT_FILE=/etc/options_wan$1.pppoe
#syslogd -m 0
pppoe.sh $1 $PPP_OPT_FILE $2 $3 $4 $5 $6 $7 $8
pppd file $PPP_OPT_FILE &
#iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu


