#!/bin/sh
# public functions and variables

# Wan interface name -> $wan_if

NV1800N=`uname -v|grep WA1800N`
getWanIfName()
{
    if [ $# -lt 1 ]; then
        echo "Usage: `basename $0` <wanIndex>";
        return;
    fi

    wan_if="`uttif gnbi $1`"
}

# Wan PPP interface name -> $wan_ppp_if
getPppIfName()
{
    if [ $# -lt 1 ]; then
        echo "Usage: `basename $0` <wanIndex>";
        return;
    fi
    getWanIfName ${1}
    ConnMode=`uttcli get interface $wanIndex ethernet connMode`
    if [ "$ConnMode" = "PPPOE" -o "$ConnMode" = "3G" ]; then
        wan_if=`cat /var/run/$wan_if.ppp|sed "s/ /\n/g" |grep ppp`
    fi
}

getWanNo()
{
    echo "`uttif gibn $1`"
}
# LAN interface name -> $lan_if
getLanIfName()
{
    lan_if="`uttif gnbi 0`"
}

getSysName()
{
    sys_name="UTT.AP"
}
