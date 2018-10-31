#!/bin/sh
#
# $Id: syslog.sh,v 1.6 2014/11/07 10:26:10 guo.deyuan Exp $
#
# usage: syslog.sh
#

ServerEn=`uttcli get syslog active`
ServerIp=`uttcli get syslog SyslogIp`
ServerPort=`uttcli get syslog SyslogPort`
touch /etc/enable
ServerInterval=`uttcli get syslog SyslogInterval`
killall -q klogd
killall -q syslogd
killall beatheart
klogd 
if [ "$ServerEn" == "Yes" ]; then
    if [ "ServerIp" != "" ]; then
	syslogd -R $ServerIp:$ServerPort -L -C8
    beatheart $ServerIp $ServerPort $ServerInterval &
    else
	syslogd -C8
    fi
else
    syslogd -C8
fi

