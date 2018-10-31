#!/bin/sh
newip=`uttcli get interface 0 ethernet static ip`
newnm=`uttcli get interface 0 ethernet static nm`
bridgeip_mask=`uttIpAndNm $newip $newnm`
ifconfig br1:2 down
ifconfig br1:2 $newip netmask $newnm
ip route del $bridgeip_mask dev br1
ip route flush table 1
ip route add table 1 $bridgeip_mask dev br1
killall telnetd
telnetd -b $newip
