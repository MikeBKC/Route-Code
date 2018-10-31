#!/bin/sh

############################
# l2tp全局配置关闭调用脚本
#
############################

# 等待pppd进程都被杀掉
sleep 1
process=`cat /var/run/xl2tpd.pid`
kill -9 $process
