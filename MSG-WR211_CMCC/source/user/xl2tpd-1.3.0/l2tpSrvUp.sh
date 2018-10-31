#!/bin/sh

############################
# l2tp全局配置开启调用脚本
#
############################
#等待进程被杀死，之后再启用
isL2tpSrvUp=$(ps | grep -v 'grep' | grep -c xl2tpd)
if [ ${isL2tpSrvUp} == '0' ];then
    sleep 2
    xl2tpd -c /etc/xl2tpd/xl2tpd.conf
fi
