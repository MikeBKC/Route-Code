#!/bin/sh
###########################
# pptp连接断开时，调用的脚本
# 目前实现删除lantolan路由功能
###########################
# $1 = ifname;
# $2 = peer_authname;
# $3 = devname; /var/run/$devname.ppp
#根据用户名获得实例名
INST=`uttcli inst pptpSrvAccount` 
for i in $INST
do
    USERNAME=`uttcli getbynm pptpSrvAccount $i userName`
    if [ "$USERNAME" == "$2" ];then
	INSTNAME=$i
	break
    fi
done

#删除源地址路由 mantis 
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

#获取用户类型
#USERTYPE="lantolan"
#USERTYPE=`pptpd-prof $2 peerType`
USERTYPE=`uttcli getbynm pptpSrvAccount $INSTNAME peerType`

#非lantolan不添加路由
if [ "$USERTYPE" != "lantolan" ]; then
    exit 0
fi

#获取对端内网ip地址，子网掩码
#REMOTELANIP="192.168.16.0"
#REMOTELANIP=`pptpd-prof $2 remoteLanIp`
REMOTELANIP=`uttcli getbynm pptpSrvAccount $INSTNAME remoteLanIp`
#获取对端内网掩码
#REMOTELANMASK="255.255.255.0"
#REMOTELANMASK=`pptpd-prof $2 remoteLanNetmask`
REMOTELANMASK=`uttcli getbynm pptpSrvAccount $INSTNAME remoteLanNetmask`

#添加路由
route del -net $REMOTELANIP netmask $REMOTELANMASK dev $1

#lan to lan删除table 101路由
mask=`mathCal -m $REMOTELANMASK`
ip route del table 101 ${REMOTELANIP}/${mask} dev $1

