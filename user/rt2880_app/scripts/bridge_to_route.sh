#!/bin/sh
switch_divide()
{
    switch reg w 40 1001 #port0&port1
    switch reg w 44 1001 #port2&port3
    switch reg w 48 2 #port4&port5
    #VLAN member port
    switch vlan set 0 1 1111011 
    switch vlan set 1 2 0000111 
    switch clear
}
mac=`uttcli getbynm sysConf  sysConf eth2_mac`
br0_ip=`uttcli get interface 0 ethernet static ip`
br0_mac=`uttcli get interface 0 ethernet mac`
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
ifconfig br1 0.0.0.0
ifconfig eth2.1 down
ifconfig eth2.2 down
ifconfig eth2.1 hw ether $eth_1_mac up
ifconfig eth2.2 hw ether $eth_2_mac up
brctl addif br0 eth2.1
brctl addif br1 eth2.2
brctl delif br1 eth2
wlan.sh
wlan_5g.sh
dhcp.sh
