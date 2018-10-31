#!/bin/sh

#pptpClients=`nvram_get 2860 PptpClients |sed "s/;/ /g"`
pptpClients=`uttcli inst pptpClient`
for chnl_name in $pptpClients
do
#    count=0
    sys_cmd1="pptp.sh "
    sys_cmd2="pppd file /etc/options.pptp_"
#    for i in `echo $pptpClient |sed "s/,/ /g"`
#    do
#        count=`expr $count + 1`
#        if [ $count == "1" ];then
    sys_cmd2="${sys_cmd2}$chnl_name "
#            sys_cmd2="${sys_cmd2}$i "
#            chnl_name=$i
    echo 0 >/var/run/pptp_${chnl_name}link.status 
    cat /proc/uptime  >/var/run/pptp_${chnl_name}.uptime 

    enable=`uttcli getbynm pptpClient $chnl_name active`
    user=`uttcli getbynm pptpClient $chnl_name userName`
    passwd=`uttcli getbynm pptpClient $chnl_name passwd`
    authType=`uttcli getbynm pptpClient $chnl_name authMode`
    serverIp=`uttcli getbynm pptpClient $chnl_name serverHost`
    EncryptionMode=`uttcli getbynm pptpClient $chnl_name EncryptionMode`
    mtu=`uttcli getbynm pptpClient $chnl_name mtu`
    sys_cmd1="${sys_cmd1}$user $passwd $mtu $serverIp $authType KeepAlive $EncryptionMode"

#        elif [ $count == "2" ];then
#            flag=$i
#        elif [ $count == "3" ];then
#            sys_cmd1="${sys_cmd1}$i "
#        elif [ $count == "4" ];then
#            sys_cmd1="${sys_cmd1}$i "
#        elif [ $count == "5" ];then
#            sys_cmd1="${sys_cmd1}$i "
#        elif [ $count == "8" ];then
#            sys_cmd1="${sys_cmd1}$i KeepAlive "
#gateway=$i  
#        fi
#    done
    if [ "$enable" == "Yes" ]; then
        `$sys_cmd1 >/etc/options.pptp_${chnl_name}`
#oldpid=`ps |grep "$sys_cmd2" |sed "s/^ *//g"|sed "s/ .*$//g"`
#echo "kill pid[$oldpid]"
#kill -9 $oldpid
#sleep 1
#ip route del $gateway/32 
        `echo $sys_cmd2 `&
    fi
done
