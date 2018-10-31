#!/bin/sh
. /sbin/global.sh
# udhcpc script edited by Tim Riker <Tim@Rikers.org>

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
ROUTE_FILE="/var/run/dhcp_$interface.route"
RESOLV_CONF="/etc/resolv.conf"
wanNo=`getWanNo $interface`
wanchkip=`uttcli get interface $wanNo detectIp`
pridns=1
[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"
case "$1" in
    deconfig)
if [ $bridge_mode_flag != "1" ];then
        /sbin/ifconfig $interface 0.0.0.0
fi
	if [ -r $ROUTE_FILE ]; then
	    rm $ROUTE_FILE
	fi
if [ $bridge_mode_flag != "1" ];then
	snd2speed down $wanNo
fi
if [ "$bridge_mode_flag" == "1" ];then
    bridge_vif.sh add dhcp
fi
        ;;
    bound)
        /sbin/ifconfig $interface $ip $BROADCAST $NETMASK

        [ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
        echo $dns > /tmp/res
        for i in $dns
        do
            if [ $pridns == 1 ]; then
                echo nameserver $i > /var/run/wan${wanNo}.dns
                pridns=0
            else
                echo nameserver $i >> /var/run/wan${wanNo}.dns
            fi
        done

        if [ -n "$router" ] ; then
            while route del default gw 0.0.0.0 dev $interface ; do
                :
            done

            metric=0
            for i in $router ; do
                metric=`expr $metric + 1`
                if [ "${wanchkip}" != "" ]; then
			if [ "${wanchkip}" == "0.0.0.0" ]; then
				ip route replace table `expr 90 + $wanNo` $i/32 via $i dev $interface
			else
				ip route replace table `expr 90 + $wanNo` ${wanchkip}/32 via $i dev $interface
			fi
                fi
		echo $i > $ROUTE_FILE

                snd2speed up $wanNo&
                if [ -e /var/run/linkcheck${wanNo}.pid ]; then
                    kill -9 `cat /var/run/linkcheck${wanNo}.pid` > null 2>&1
                fi
            done
        fi
        
        cat /proc/uptime > /var/run/wan${wanNo}.uptime
        ;;
esac

exit 0

