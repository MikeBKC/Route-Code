#!/bin/sh

if [ $# != 2 ];then
    echo "Usage: pptp-up.sh <ppp name> <channel name>"
    exit;
fi

pppname=$1
chnl_name=$2

echo 1 >/var/run/${chnl_name}link.status
cat /proc/uptime > /var/run/${chnl_name}.uptime
chnl_name=`echo $chnl_name |sed "s/^pptp_//g"`
# vpn free session minus 1
vpnHandle minus pptpclient $chnl_name 
lanIp=`uttcli getbynm pptpClient $chnl_name remoteLanIp`
lanNm=`uttcli getbynm pptpClient $chnl_name remoteLanNetmask`
#pptp_client=`nvram_get 2860 PptpClients |sed "s/;/\n/g" |grep "^${chnl_name},"`
echo pptp_client  >/dev/ttyS0
system_cmd="route add "
#count=0
#for i in `echo $pptp_client |sed "s/,/\n/g"`
#do
#    count=`expr $count + 1`
#    if [ $count == "6" ];then
#        system_cmd="${system_cmd}-net $i "
#    elif [ $count == "7" ];then
#        system_cmd="${system_cmd}netmask $i "
#    elif [ $count == "8" ];then
#        system_cmd="${system_cmd}gw $i "
#    fi
#done
system_cmd="${system_cmd}-net $lanIp netmask $lanNm "
system_cmd="${system_cmd}dev $pppname "
echo "$system_cmd"  >/dev/ttyS0
$system_cmd
echo 1 >/proc/sys/net/ipv4/route/flush
Natenable=`uttcli getbynm pptpClient $chnl_name Natenable`
if [ "$Natenable" = "Yes" ];then
iptables -t nat -A pptp_default_nat -o $1 -j MASQUERADE
fi

#根据用户名获得静态路由配置信息
INST=`uttcli inst staticRoute` 
for i in $INST
do
    INTERFACE=`uttcli getbynm staticRoute $i interface`
    if [ "$INTERFACE" == "(PPTPC)$chnl_name" ];then
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
    if [ "$INTERFACE" == "PC_$chnl_name" ];then
	snd2speed ipsecUp $INTERFACE
    fi
done

#SOHO开启VPN，添加DNS和路由
if [ $VPN_PROXY == $FYES ]; then
    vpnMode=`uttcli getbynm pptpClient $chnl_name vpnMode`
    vpndns1=$(sed -n '1p' /var/run/vpn.dns)
    vpndns2=$(sed -n '2p' /var/run/vpn.dns)
    setname=sys_vpn
    if [ $vpnMode == 0 ]||[ $vpnMode == 1 ];then
	dnsmasqConf.sh addGlobal $vpndns1 $vpndns2 $pppname $setname
    elif [ $vpnMode == 2 ];then
	dnsmasqConf.sh add $vpndns1 $vpndns2 $pppname $setname
	iptables -t mangle -D PREROUTING -m conntrack --ctstate NEW -m set --set $setname dst -j MARK --set-mark 0x10/0xfe;
	iptables -t mangle -D PREROUTING -m conntrack --ctstate NEW -m set --set $setname dst -j CONNMARK --set-mark 0x10/0xfe;
	iptables -t mangle -A PREROUTING -m conntrack --ctstate NEW -m set --set $setname dst -j MARK --set-mark 0x10/0xfe;
	iptables -t mangle -A PREROUTING -m conntrack --ctstate NEW -m set --set $setname dst -j CONNMARK --set-mark 0x10/0xfe;
    fi
fi
