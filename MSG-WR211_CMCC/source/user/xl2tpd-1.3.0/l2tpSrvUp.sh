#!/bin/sh

############################
# l2tpȫ�����ÿ������ýű�
#
############################
#�ȴ����̱�ɱ����֮��������
isL2tpSrvUp=$(ps | grep -v 'grep' | grep -c xl2tpd)
if [ ${isL2tpSrvUp} == '0' ];then
    sleep 2
    xl2tpd -c /etc/xl2tpd/xl2tpd.conf
fi
