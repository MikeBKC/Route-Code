#!/bin/sh

############################
# pptpȫ�����ùرյ��ýű�
#
############################
process=`cat /var/run/pptpd.pid`
kill -9 $process

# ��ȡ�˺Ų���pptpd pid
otherpid=`ps | grep " pptpd \[" | sed "s/^ *//g"|sed "s/ .*$//g"`
for i in $otherpid
do
    kill -9 $i >/dev/null 2>&1
    vpnHandle plus
done
rm /tmp/uttPptpSrvMsg
