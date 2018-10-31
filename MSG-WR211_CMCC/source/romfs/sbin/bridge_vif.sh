#!/bin/sh

cmd=$1
cmd_2=$2
bridgeip=`uttcli get interface 0 ethernet static ip`
bridgenm=`uttcli get interface 0 ethernet static nm`
bridgeip_mask=`netmask_tool $bridgeip $bridgenm`
wan_state=`cat /var/run/Wan1link.status`
wanmode=`uttcli get interface 1 ethernet connMode`
dhcp_br1=`cat /var/run/dhcp_br1.route`
wan_br1_ip=`ifconfig br1 |grep 'inet addr' |awk '{print $2}' |awk  -F  ":"  '{print $2}'`
wan_br1_netmask=`ifconfig br1 |grep 'Mask' |awk '{print $4}' |awk  -F  ":"  '{print $2}'`
wan_br1_net=`netmask_tool $wan_br1_ip $wan_br1_netmask`
iptables -D INPUT -p udp -m uttdev ! --is-lan-in -m multiport ! --dports 161,500,1701,4500,34951 -m conntrack --ctstate NEW -j DROP
if [ "$cmd" = "add" ];then
	if [ "$wan_state" = "1" -o "$wanmode" = "STATIC" ] || [ "$dhcp_br1" != "" ];then
		killall udhcpd 
		ifconfig br1:2 down
		if [ "$wanmode" != "PPPOE" ];then
			ifconfig br1:2 $bridgeip netmask $bridgenm
		fi
		if [ "$wanmode" == "PPPOE" ];then
			ifconfig br1 $bridgeip netmask $bridgenm
		fi
		ip route del $bridgeip_mask dev br1
		ip route flush table 1
		ip route add table 1 $bridgeip_mask dev br1
		iptables -A INPUT -p udp -m uttdev ! --is-lan-in -m multiport ! --dports 161,500,1701,4500,34951 -m conntrack --ctstate NEW -j DROP
        if [ "$wan_br1_ip" != "" ];then
            ipset -F sys_lan_group
            ipset -F sys_lan_range_group
            sleep 1
            ipset -A sys_lan_group $bridgeip
            ipset -A sys_lan_range_group $bridgeip_mask 
            ipset -A sys_lan_group $wan_br1_ip
            ipset -A sys_lan_range_group $wan_br1_net
        fi
    elif [ "$cmd_2" == "dhcp" ];then
		killall udhcpd 
		ifconfig br1:2 down
		ifconfig br1 $bridgeip netmask $bridgenm
		ip route del $bridgeip_mask dev br1
		ip route flush table 1
		ip route add table 1 $bridgeip_mask dev br1
        dhcp.sh
    else
        killall udhcpd 
		ifconfig br1:2 down
		ifconfig br1 $bridgeip netmask $bridgenm
		ip route del $bridgeip_mask dev br1
		ip route flush table 1
		ip route add table 1 $bridgeip_mask dev br1
        if [ "$dhcp_br1" == "" ];then
            dhcp.sh
        fi
    fi
    telnetd_exec=`ps -ef|grep telnetd|grep -v grep`
    if [ "$telnetd_exec" == "" ]; then
        new_ip=`uttcli get interface 0 ethernet static ip`
        telnetd -b $new_ip                          
    fi                                                  
else
	ifconfig br1:2 down
	ip route flush table 1
	iptables -A INPUT -p udp -m uttdev ! --is-lan-in -m multiport ! --dports 161,500,1701,4500,34951 -m conntrack --ctstate NEW -j DROP
    if [ "$wan_br1_ip" != "" ];then
        ipset -D sys_lan_group $wan_br1_ip
        ipset -D sys_lan_range_group $wan_br1_net
    fi
fi
