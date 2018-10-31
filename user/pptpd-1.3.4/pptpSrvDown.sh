#!/bin/sh

############################
# pptp全局配置关闭调用脚本
#
############################
process=`cat /var/run/pptpd.pid`
kill -9 $process

# 获取账号拨号pptpd pid
otherpid=`ps | grep " pptpd \[" | sed "s/^ *//g"|sed "s/ .*$//g"`
for i in $otherpid
do
    kill -9 $i >/dev/null 2>&1
    vpnHandle plus
done
rm /tmp/uttPptpSrvMsg
