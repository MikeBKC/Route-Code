#!/bin/sh
. /sbin/config.sh

if [ $# != 1 ]; then
    echo "Usage `basename $0` <DDNS ServerIP>"
fi
ServerIP=$1
wanIndex=$2
table=`echo ${wanIndex} |sed "s/WAN/10/g"`
#ip route flush table 100
route=`ip route show table ${table} |grep ^default |sed "s/^default/${ServerIP}\/32/g"`
ip route replace table 100 $route


