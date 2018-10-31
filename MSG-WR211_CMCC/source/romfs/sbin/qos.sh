#!/bin/sh
#iptables -t mangle -I OUTPUT -j MARK --set-xmark 10
#iptables -t mangle -I PREROUTING -p tcp --tcp-flags SYN,RST SYN -j MARK --set-xmark 10
#iptables -t mangle -I PREROUTING -j CONNMARK --restore-mark
#iptables -t mangle -I PREROUTING -p tcp -m layer7 --l7proto web --sport 80 -j CONNMARK --set-xmark 10
#iptables -t mangle -I PREROUTING -i eth0 -p tcp --dport 80 -m layer7 --l7proto webget-post -j MARK --set-xmark 10
#iptables -t mangle -I PREROUTING -i eth0 -p tcp --tcp-flags SYN -j CONNMARK --set-xmark 33 
#iptables -t mangle -I POSTROUTING -p tcp --tcp-flags ACK -m mart --mark 10 -j MARK --set-xmark 10
