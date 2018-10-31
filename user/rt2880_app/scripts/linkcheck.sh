#!/bin/sh
source /sbin/global.sh
if [ $# -ne 1 ]
then
    echo "Usage: linkcheck.sh <wanNo>"
    exit
fi

wanNo=$1
getWanIfName $wanNo
wanName=Wan$wanNo
wanmode=`uttcli get interface $wanNo ethernet connMode`
if [ "$wanmode" = "PPPOE" -o "$wanmode" = "PPTP" ]; then
    wan_if=`cat /var/run/$wan_if.ppp|sed "s/ /\n/g" |grep ppp`
fi
ipaddr=`uttcli get interface $wanNo detectIp`
count=`uttcli get interface $wanNo detectCount`
intval=`uttcli get interface $wanNo detectInterval`
wanchkrate=`uttcli get interface $wanNo txBand`
gw=`uttcli get interface $wanNo ethernet static gw`
if [ -z $ipaddr ] || [ -z $count ] || [ -z $intval ]
then
    exit
fi

if [ $intval -eq 0 ] || [ $count -eq 0 ]
then
    echo "check interval or count is zero, exit."
    exit;
fi

if [ "$wanchkrate" != "0" ]  && [ "${ipaddr}" == "0.0.0.0" ]; then
if [ "$wanmode" = "STATIC" ]; then
    ipaddr=$gw
else
    ipaddr=`cat /var/run/dhcp_${wan_if}.route`
fi
fi
timeout=`expr ${count} \* ${intval} `
#echo $timeout
#echo 1 >/var/run/${wanName}link.status
echo $$ >/var/run/linkcheck${wanNo}.pid

while [ 1 ]
do
    FailRate=`ping -q -I ${wan_if} -i ${intval} -c ${count} -w ${timeout} ${ipaddr} |sed "s/, /\n/g" |grep "% packet loss" |sed "s/% packet loss.*$//g"`
    if [ -z ${FailRate} ]; then
        FailRate=100;
    fi

    LinkStatus=`cat /var/run/Wan${wanNo}link.status`
    echo `date`"> Current ${wan_if} Status:${LinkStatus}. Fail Rate:${FailRate}"
 
    if [ ${FailRate} -eq 100 ] && [ ${LinkStatus} -eq 1 ]
    then
        echo "change link ${wan_if} to sleep now."
#echo 0 >/var/run/Wan${wanNo}link.status
        snd2speed down ${wanNo}
    fi
    
    if [ ${FailRate} -le 50 ] && [ ${LinkStatus} -eq 0 ]
    then
        echo "change link ${wan_if} to alive now."
#echo 1 >/var/run/Wan${wanNo}link.status
        snd2speed up ${wanNo}
    fi

    if [ "`ifconfig ${wan_if}`" == "" ]
    then
       exit
    fi
done
