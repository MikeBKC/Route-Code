#!/bin/sh
###########################
# l2tp连接建立时，调用的脚本
# 目前实现添加lantolan路由功能
###########################
# $1=ifname;
# $2=peer_authname;

#根据用户名获得实例名
INST=`uttcli inst l2tpSrvAccount` 
for i in $INST
do
    USERNAME=`uttcli getbynm l2tpSrvAccount $i userName`
    if [ "$USERNAME" == "$2" ];then
	INSTNAME=$i
	break
    fi
done

#获取用户类型
#USERTYPE="lantolan"
#USERTYPE=`xl2tpd-prof $2 peerType`
USERTYPE=`uttcli getbynm l2tpSrvAccount $INSTNAME peerType`

#非lantolan不添加路由
if [ "$USERTYPE" != "lantolan" ]; then
    exit 0
fi

#获取对端内网ip地址，子网掩码
#REMOTELANIP="192.168.16.0"
#REMOTELANIP=`xl2tpd-prof $2 remoteLanIp`
REMOTELANIP=`uttcli getbynm l2tpSrvAccount $INSTNAME remoteLanIp`

#获取对端内网掩码
#REMOTELANMASK="255.255.255.0"
#REMOTELANMASK=`xl2tpd-prof $2 remoteLanNetmask`
REMOTELANMASK=`uttcli getbynm l2tpSrvAccount $INSTNAME remoteLanNetmask`

#添加路由
route add -net $REMOTELANIP netmask $REMOTELANMASK dev $1
