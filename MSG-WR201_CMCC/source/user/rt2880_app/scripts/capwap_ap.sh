#!/bin/sh
#
# $Id: capwap_ap.sh,v 1.0 2014/12/04
#
# usage: capwap_ap.sh
#
#
. /sbin/global.sh
. /sbin/config.sh
if [ "$#" == 0 ]; then
mode=0
else
mode=$1
fi
# set the mac of AP

mac=`uttcli get interface 0 ethernet mac | sed -e 's/[-:]//g'`
if [ "$CONFIG_WLAN_LAN_ISOLATION" != "" ];then
    ifconfig eth2.1 down
    ifconfig eth2.1 hw ether $mac
    ifconfig eth2.1 up
    ifconfig eth2.2 down
    ifconfig eth2.2 hw ether $mac
    ifconfig eth2.2 up
    ifconfig eth2.3 down
    ifconfig eth2.3 hw ether $mac
    ifconfig eth2.3 up
else
    ifconfig eth2 down
    ifconfig eth2 hw ether $mac
    ifconfig eth2 up
fi
ifconfig br0 hw ether $mac up
# set the mac of AP end
if [ $mode -eq 1 ]; then
    sleep 5
    killall -9 apd
    apd &
elif [ $mode -eq 2 ]; then
    sleep 5
    killall -9 apd
    apd &
elif [ $mode -eq 3 ]; then
    sleep 5
    killall -9 acd
    acd &
else
    sleep 3
    killall -9 apd
    apd &
fi
