#!/bin/sh
###########################
# l2tp���ӽ���ʱ�����õĽű�
# Ŀǰʵ�����lantolan·�ɹ���
###########################
# $1=ifname;
# $2=peer_authname;

#�����û������ʵ����
INST=`uttcli inst l2tpSrvAccount` 
for i in $INST
do
    USERNAME=`uttcli getbynm l2tpSrvAccount $i userName`
    if [ "$USERNAME" == "$2" ];then
	INSTNAME=$i
	break
    fi
done

#��ȡ�û�����
#USERTYPE="lantolan"
#USERTYPE=`xl2tpd-prof $2 peerType`
USERTYPE=`uttcli getbynm l2tpSrvAccount $INSTNAME peerType`

#��lantolan�����·��
if [ "$USERTYPE" != "lantolan" ]; then
    exit 0
fi

#��ȡ�Զ�����ip��ַ����������
#REMOTELANIP="192.168.16.0"
#REMOTELANIP=`xl2tpd-prof $2 remoteLanIp`
REMOTELANIP=`uttcli getbynm l2tpSrvAccount $INSTNAME remoteLanIp`

#��ȡ�Զ���������
#REMOTELANMASK="255.255.255.0"
#REMOTELANMASK=`xl2tpd-prof $2 remoteLanNetmask`
REMOTELANMASK=`uttcli getbynm l2tpSrvAccount $INSTNAME remoteLanNetmask`

#���·��
route add -net $REMOTELANIP netmask $REMOTELANMASK dev $1
