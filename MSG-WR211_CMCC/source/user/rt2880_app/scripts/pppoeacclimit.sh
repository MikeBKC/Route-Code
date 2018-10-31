#!/bin/sh
#���ӻ���ɾ��PPPOE�˺ŵ����ٹ���
#$1 action ɾ���������
#$2 pppoeaccname �˺�����
#$3 PPPOE�˺ŵ�IP��ַ
action=$1
pppoeaccname=$2
pppoeaccip=$3
nurate=$4  
ndrate=$5

urate=`expr $nurate \* 1024 / 8`
drate=`expr $ndrate \* 1024 / 8`

if [ $nurate -ne 0 ]   
then  
    iptables -t mangle -$action pppoeacc_limit -s $pppoeaccip -m time --timestart 00:00 --timestop 23:59 --days Mon,Tue,Wed,Thu,Fri,Sat,Sun -m hashlimit --hashlimit-name $2src --hashlimit $urate/sec --hashlimit-burst $urate --hashlimit-mode srcip -j ACCEPT
    iptables -t mangle -$action pppoeacc_limit -s $pppoeaccip -m time --timestart 00:00 --timestop 23:59 --days Mon,Tue,Wed,Thu,Fri,Sat,Sun -j DROP
fi

if [ $ndrate -ne 0 ] 
then
    iptables -t mangle -$action pppoeacc_limit -d $pppoeaccip -m time --timestart 00:00 --timestop 23:59 --days Mon,Tue,Wed,Thu,Fri,Sat,Sun -m hashlimit --hashlimit-name $2dst --hashlimit $drate/sec --hashlimit-burst $drate --hashlimit-mode dstip -j ACCEPT
    iptables -t mangle -$action pppoeacc_limit -d $pppoeaccip -m time --timestart 00:00 --timestop 23:59 --days Mon,Tue,Wed,Thu,Fri,Sat,Sun -j DROP
fi



