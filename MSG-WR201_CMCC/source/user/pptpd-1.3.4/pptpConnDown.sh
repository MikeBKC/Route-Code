#!/bin/sh
###########################
# pptp���ӶϿ�ʱ�����õĽű�
# Ŀǰʵ��ɾ��lantolan·�ɹ���
###########################
# $1 = ifname;
# $2 = peer_authname;
# $3 = devname; /var/run/$devname.ppp
#�����û������ʵ����
INST=`uttcli inst pptpSrvAccount` 
for i in $INST
do
    USERNAME=`uttcli getbynm pptpSrvAccount $i userName`
    if [ "$USERNAME" == "$2" ];then
	INSTNAME=$i
	break
    fi
done

#ɾ��Դ��ַ·�� mantis 
pppinfo=`cat /var/run/$3.ppp`
count=1
for i in $pppinfo
do
if [ "$count" == "5" ];then
peerip=$i
fi
count=`expr $count + 1`
done
ip route del table 101 ${peerip}/32 dev $1

#��ȡ�û�����
#USERTYPE="lantolan"
#USERTYPE=`pptpd-prof $2 peerType`
USERTYPE=`uttcli getbynm pptpSrvAccount $INSTNAME peerType`

#��lantolan�����·��
if [ "$USERTYPE" != "lantolan" ]; then
    exit 0
fi

#��ȡ�Զ�����ip��ַ����������
#REMOTELANIP="192.168.16.0"
#REMOTELANIP=`pptpd-prof $2 remoteLanIp`
REMOTELANIP=`uttcli getbynm pptpSrvAccount $INSTNAME remoteLanIp`
#��ȡ�Զ���������
#REMOTELANMASK="255.255.255.0"
#REMOTELANMASK=`pptpd-prof $2 remoteLanNetmask`
REMOTELANMASK=`uttcli getbynm pptpSrvAccount $INSTNAME remoteLanNetmask`

#���·��
route del -net $REMOTELANIP netmask $REMOTELANMASK dev $1

#lan to lanɾ��table 101·��
mask=`mathCal -m $REMOTELANMASK`
ip route del table 101 ${REMOTELANIP}/${mask} dev $1

