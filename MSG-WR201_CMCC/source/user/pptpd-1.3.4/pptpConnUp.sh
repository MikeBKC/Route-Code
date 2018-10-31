#!/bin/sh
###########################
# pptp���ӽ���ʱ�����õĽű�
# Ŀǰʵ�����lantolan·�ɹ���
###########################
# $1=ifname;
# $2=peer_authname;
# $3=devname  /var/run/$devname.ppp

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
#��ȡ�û�����
#USERTYPE="lantolan"
#USERTYPE=`pptpd-prof $2 peerType`
USERTYPE=`uttcli getbynm pptpSrvAccount $INSTNAME peerType`

#Դ��ַ·�ɱ�������·�� mantis 14899
#pptpĿǰ����wan1��, table 101
pppinfo=`cat /var/run/$3.ppp`
count=1
for i in $pppinfo
do
    if [ "$count" == "5" ];then
	peerip=$i
    fi
    count=`expr $count + 1`
done
ip route add table 101 ${peerip}/32 dev $1


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
route add -net $REMOTELANIP netmask $REMOTELANMASK dev $1

#lan to lan���table 101·��
mask=`mathCal -m $REMOTELANMASK`
ip route add table 101 ${REMOTELANIP}/${mask} dev $1


#�����û�����þ�̬·��������Ϣ
INST=`uttcli inst staticRoute` 
for i in $INST
do
    INTERFACE=`uttcli getbynm staticRoute $i interface`
    if [ "$INTERFACE" == "(PPTPS)$INSTNAME" ];then
	dstip=`uttcli getbynm staticRoute $i dstIp`
	maskInt=`uttcli getbynm staticRoute $i netmask`
	metric=`uttcli getbynm staticRoute $i metric`
	ip route replace table 120 $dstip/$maskInt dev $1 metric $metric
    fi
done
#��ȡ���ӵ�pptp �������Ȼ�������󶨴������ipsec����
INST=`uttcli inst ipsecConf`
for i in $INST
do
    INTERFACE=`uttcli getbynm ipsecConf $i profile`
    if [ "$INTERFACE" == "PS_$INSTNAME" ];then
	snd2speed ipsecUp $INTERFACE
    fi
done
