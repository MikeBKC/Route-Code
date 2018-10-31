#!/bin/sh
switch_restore()
{
    switch reg w 14 5555
    switch reg w 40 1001
    switch reg w 44 1001
    switch reg w 48 1001
    switch reg w 4c 1
    switch reg w 50 2001
    switch reg w 70 ffffffff
    switch reg w 98 7f7f
    switch reg w e4 7f
    switch clear
}
mac=`uttcli get interface 1 ethernet mac`
br1_ip=`uttcli get interface 0 ethernet static ip`
judge_br1ip=`ifconfig br1 |grep 'inet addr' |awk '{print $2}' |awk  -F  ":"  '{print $2}'`
    brctl delif br0 eth2.1
    ifconfig br0 down
    if [ $judge_br1ip == ""];then
        ifconfig br1 down
        ifconfig br1 $br1_ip
    fi
    brctl delif br1 eth2.2
    vconfig rem eth2.1    
    vconfig rem eth2.2    
    brctl addif br1 eth2  
    ifconfig eth2 down  
    ifconfig eth2 hw ether $mac up
    wlan.sh
    killall udhcpd
    switch_restore
    iptables -t mangle -F
    bridge_vif.sh add
    #uttcli2 set dhcpServer dhcpServer DhcpSEn Disable
    #dhcp.sh
