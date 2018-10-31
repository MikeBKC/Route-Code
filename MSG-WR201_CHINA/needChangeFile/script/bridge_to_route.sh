#!/bin/sh
switch_divide()
{
    switch reg w 40 1001 #port0&port1VLAN member port             
    switch reg w 44 2001 #port2&port3
    switch reg w 48 1 #port4&port5   
    #VLAN member port                
    switch vlan set 0 1 1110111      
    switch vlan set 1 2 0001011  
}
mac=`uttcli getbynm sysConf  sysConf eth2_mac`
br0_ip=`uttcli get interface 0 ethernet static ip`
br0_mac=`uttcli get interface 0 ethernet mac`
judge_br1ip=`ifconfig br1 |grep 'inet addr' |awk '{print $2}' |awk  -F  ":"  '{print $2}'`
if [ $judge_br1ip == $br0_ip ];then
    ifconfig br1 down
    ifconfig br1 0.0.0.0
fi
brctl addbr br0
ifconfig br0 $br0_ip
ifconfig br0 hw ether $br0_mac up
ifconfig eth2 down
ifconfig eth2 hw ether $mac up
eth_1_mac=`ifconfig br0 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'`
eth_2_mac=`ifconfig br1 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'`
vconfig add eth2 1
vconfig add eth2 2
switch_divide
ifconfig br1 down
bridge_vif.sh del
ifconfig br1 0.0.0.0

ifconfig eth2.1 down
ifconfig eth2.2 down
sleep 1
ifconfig eth2.1 hw ether $eth_1_mac up
ifconfig eth2.2 hw ether $eth_2_mac up
brctl addif br0 eth2.1
brctl addif br1 eth2.2
brctl delif br1 eth2
wlan.sh
wan.sh 1
killall udhcpd
iptables -t mangle -A INPUT -p tcp -m tcp --dport 80 -m set ! --match-set sys_lan_group dst -j DROP
dhcp.sh
