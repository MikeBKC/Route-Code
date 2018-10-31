#!/bin/sh
. /sbin/global.sh

getWanIfName $1
export dhcpPid=/var/run/udhcpc_wan$1.pid
export PPP_OPT_FILE=/etc/options_wan$1.pppoe

ROUTE_FILE="/var/run/dhcp_$wan_if.route"
if [ -r $dhcpPid ]; then
    kill `cat $dhcpPid` > null 2>&1
    if [ -r $ROUTE_FILE ]; then
	rm $ROUTE_FILE
    fi
fi
ifconfig $wan_if 0.0.0.0
snd2speed down $1
