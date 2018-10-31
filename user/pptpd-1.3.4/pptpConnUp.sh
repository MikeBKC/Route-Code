#!/bin/sh
###########################
# pptp连接建立时，调用的脚本
# 目前实现添加lantolan路由功能
###########################
# $1=ifname;
# $2=peer_authname;
# $3=devname  /var/run/$devname.ppp

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
#获取用户类型
#USERTYPE="lantolan"
#USERTYPE=`pptpd-prof $2 peerType`
USERTYPE=`uttcli getbynm pptpSrvAccount $INSTNAME peerType`

#源地址路由表中增加路由 mantis 14899
#pptp目前绑定在wan1口, table 101
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
route add -net $REMOTELANIP netmask $REMOTELANMASK dev $1

#lan to lan添加table 101路由
mask=`mathCal -m $REMOTELANMASK`
ip route add table 101 ${REMOTELANIP}/${mask} dev $1


#根据用户名获得静态路由配置信息
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
#获取连接的pptp 隧道名，然后启动绑定此隧道的ipsec配置
INST=`uttcli inst ipsecConf`
for i in $INST
do
    INTERFACE=`uttcli getbynm ipsecConf $i profile`
    if [ "$INTERFACE" == "PS_$INSTNAME" ];then
	snd2speed ipsecUp $INTERFACE
    fi
done
