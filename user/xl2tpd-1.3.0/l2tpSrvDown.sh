#!/bin/sh

############################
# l2tpȫ�����ùرյ��ýű�
#
############################

# �ȴ�pppd���̶���ɱ��
sleep 1
process=`cat /var/run/xl2tpd.pid`
kill -9 $process
