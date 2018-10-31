
. /sbin/config.sh
. /sbin/global.sh

if [ "$#" == 0 ]; then
offset1=`expr 1 + 0`
else
offset1=`expr $1 + 0`
fi
offset2=`expr $offset1 + 1`
offset3=`expr $offset1 + 2`
offset4=`expr $offset1 + 3`
#radio_off=`nvram_get 2860 RadioOff`
num1=`uttcli getbynm wireless wireless$offset1 active`
num2=`uttcli getbynm wireless wireless$offset2 active`
num3=`uttcli getbynm wireless wireless$offset3 active`
num4=`uttcli getbynm wireless wireless$offset4 active`
if [ "$num1" == "Yes" ]; then
bssidnum=`expr 0 + 1`
fi
if [ "$num2" == "Yes" ]; then
bssidnum=`expr $bssidnum + 1`
fi
if [ "$num3" == "Yes" ]; then
bssidnum=`expr $bssidnum + 1`
fi
if [ "$num4" == "Yes" ]; then
bssidnum=`expr $bssidnum + 1`
fi

radio_on=`uttcli getbynm wireless wireless0 active`
if [ "$radio_on" == "" ]; then
	radio_on=`uttcli getbynm wireless wireless active`
fi
m_radio=1
if [ "$radio_on" == "No" ]; then
m_radio=0
fi
#m_radio=0
#if [ "$bssidnum" -gt 0 ]; then
#m_radio=1
#fi
#echo "$bssidnum"
#echo "$m_radio"

ifRaxWdsxDown()
{
    ifconfig ra0 down 1>/dev/null 2>&1
    ifconfig ra1 down 1>/dev/null 2>&1
    ifconfig ra2 down 1>/dev/null 2>&1
    ifconfig ra3 down 1>/dev/null 2>&1
    ifconfig ra4 down 1>/dev/null 2>&1
    ifconfig ra5 down 1>/dev/null 2>&1
    ifconfig ra6 down 1>/dev/null 2>&1
    ifconfig ra7 down 1>/dev/null 2>&1

    ifconfig wds0 down 1>/dev/null 2>&1
    ifconfig wds1 down 1>/dev/null 2>&1
    ifconfig wds2 down 1>/dev/null 2>&1
    ifconfig wds3 down 1>/dev/null 2>&1

    ifconfig apcli0 down 1>/dev/null 2>&1
}

brctl delif br0 ra0 1>/dev/null 2>&1
brctl delif br0 ra1 1>/dev/null 2>&1
brctl delif br0 ra2 1>/dev/null 2>&1
brctl delif br0 ra3 1>/dev/null 2>&1
brctl delif br0 ra4 1>/dev/null 2>&1
brctl delif br0 ra5 1>/dev/null 2>&1
brctl delif br0 ra6 1>/dev/null 2>&1
brctl delif br0 ra7 1>/dev/null 2>&1

if [ "$NV1800N" != "" ];then
    brctl delif br0 apcli0 1>/dev/null 2>&1
fi

if [ "$m_radio" -gt 0 ]; then
ifRaxWdsxDown
sleep 1
rmmod rt2860v2_ap 1>/dev/null 2>&1
    sleep 1
#    ralink_init make_wireless_config rt2860
    uttcli make_ap_config wireless
#    insmod -q rt2860v2_ap
if [ "$CONFIG_RALINK_RT2880" != "" ];then
    insmod /lib/modules/2.4.30/kernel/drivers/net/wireless/rt2860v2_ap/rt2860v2_ap.o
elif [ "$CONFIG_RALINK_MT7620" != "" ];then
    if [ "$CONFIG_RT2860V2_AP" == "m" ];then
        insmod /lib/modules/2.6.36/kernel/drivers/net/wireless/rt2860v2_ap/rt2860v2_ap.ko
    else
	ifconfig ra0 up
    fi
else
    insmod /lib/modules/2.6.21/kernel/drivers/net/wireless/rt2860v2_ap/rt2860v2_ap.ko
fi
#if [ "$CONFIG_RALINK_RT5350" != "" ] || [ "$CONFIG_RALINK_RT2880" != "" ];then
#     uttcli set_ra0_macaddr
#fi
	ifconfig ra0 0.0.0.0
	brctl addif br0 ra0
	if [ "$bssidnum" = "2" ]; then
		ifconfig ra1 0.0.0.0
	elif [ "$bssidnum" = "3" ]; then
		ifconfig ra1 0.0.0.0
		ifconfig ra2 0.0.0.0
	elif [ "$bssidnum" = "4" ]; then
		ifconfig ra1 0.0.0.0
		ifconfig ra2 0.0.0.0
		ifconfig ra3 0.0.0.0
	elif [ "$bssidnum" = "5" ]; then
		ifconfig ra1 0.0.0.0
		ifconfig ra2 0.0.0.0
		ifconfig ra3 0.0.0.0
		ifconfig ra4 0.0.0.0
	elif [ "$bssidnum" = "6" ]; then
		ifconfig ra1 0.0.0.0
		ifconfig ra2 0.0.0.0
		ifconfig ra3 0.0.0.0
		ifconfig ra4 0.0.0.0
		ifconfig ra5 0.0.0.0
	elif [ "$bssidnum" = "7" ]; then
		ifconfig ra1 0.0.0.0
		ifconfig ra2 0.0.0.0
		ifconfig ra3 0.0.0.0
		ifconfig ra4 0.0.0.0
		ifconfig ra5 0.0.0.0
		ifconfig ra6 0.0.0.0
	elif [ "$bssidnum" = "8" ]; then
		ifconfig ra1 0.0.0.0
		ifconfig ra2 0.0.0.0
		ifconfig ra3 0.0.0.0
		ifconfig ra4 0.0.0.0
		ifconfig ra5 0.0.0.0
		ifconfig ra6 0.0.0.0
		ifconfig ra7 0.0.0.0
	fi
    if [ "$bssidnum" = "2" ]; then
	    brctl addif br0 ra1
    elif [ "$bssidnum" = "3" ]; then
	    brctl addif br0 ra1
	    brctl addif br0 ra2
    elif [ "$bssidnum" = "4" ]; then
	    brctl addif br0 ra1
	    brctl addif br0 ra2
	    brctl addif br0 ra3
    elif [ "$bssidnum" = "5" ]; then
	    brctl addif br0 ra1
	    brctl addif br0 ra2
	    brctl addif br0 ra3
	    brctl addif br0 ra4
    elif [ "$bssidnum" = "6" ]; then
	    brctl addif br0 ra1
	    brctl addif br0 ra2
	    brctl addif br0 ra3
	    brctl addif br0 ra4
	    brctl addif br0 ra5
    elif [ "$bssidnum" = "7" ]; then
	    brctl addif br0 ra1
	    brctl addif br0 ra2
	    brctl addif br0 ra3
	    brctl addif br0 ra4
	    brctl addif br0 ra5
	    brctl addif br0 ra6
    elif [ "$bssidnum" = "8" ]; then
	    brctl addif br0 ra1
	    brctl addif br0 ra2
	    brctl addif br0 ra3
	    brctl addif br0 ra4
	    brctl addif br0 ra5
	    brctl addif br0 ra6
	    brctl addif br0 ra7
    fi

    #设置br0 mac地址
#    mac=`uttcli get interface 0 ethernet mac`
#   if [ "$mac" != "00:00:00:00:00:00" ]; then
#	ifconfig br0 down
#	ifconfig br0 hw ether $mac up
#  fi

#设置br0 mac地址
if [ "$CONFIG_WLAN_LAN_ISOLATION" != "" ];then
#mac=`ifconfig eth2.1 | grep HWaddr  | sed 's/eth2.1      Link encap:Ethernet  HWaddr//'`
mac=`uttcli get interface 0 ethernet mac`
else
mac=`ifconfig eth2 | grep HWaddr  | sed 's/eth2      Link encap:Ethernet  HWaddr//'`
fi
if [ "$mac" != "00:00:00:00:00:00" ]; then
#    ifconfig br0 down
    ifconfig br0 hw ether $mac up
fi

    # in order to use broadcast IP address in L2 management daemon
    iwpriv ra0 set IgmpSnEnable=1 1>/dev/null 2>&1
    if [ "$bssidnum" = "2" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "3" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra2 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "4" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra3 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "5" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra4 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "6" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra4 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra5 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "7" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra4 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra5 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra6 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "8" ]; then
	iwpriv ra1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra4 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra5 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra6 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv ra7 set IgmpSnEnable=1 1>/dev/null 2>&1
    fi
else
    iwpriv ra0 set RadioOn=0
    if [ "$bssidnum" = "2" ]; then
	iwpriv ra1 set RadioOn=0
    elif [ "$bssidnum" = "3" ]; then
	iwpriv ra1 set RadioOn=0
	iwpriv ra2 set RadioOn=0
    elif [ "$bssidnum" = "4" ]; then
	iwpriv ra1 set RadioOn=0
	iwpriv ra2 set RadioOn=0
	iwpriv ra3 set RadioOn=0
    elif [ "$bssidnum" = "5" ]; then
	iwpriv ra1 set RadioOn=0
	iwpriv ra2 set RadioOn=0
	iwpriv ra3 set RadioOn=0
	iwpriv ra4 set RadioOn=0
    elif [ "$bssidnum" = "6" ]; then
	iwpriv ra1 set RadioOn=0
	iwpriv ra2 set RadioOn=0
	iwpriv ra3 set RadioOn=0
	iwpriv ra4 set RadioOn=0
	iwpriv ra5 set RadioOn=0
    elif [ "$bssidnum" = "7" ]; then
	iwpriv ra1 set RadioOn=0
	iwpriv ra2 set RadioOn=0
	iwpriv ra3 set RadioOn=0
	iwpriv ra4 set RadioOn=0
	iwpriv ra5 set RadioOn=0
	iwpriv ra6 set RadioOn=0
    elif [ "$bssidnum" = "8" ]; then
	iwpriv ra1 set RadioOn=0
	iwpriv ra2 set RadioOn=0
	iwpriv ra3 set RadioOn=0
	iwpriv ra4 set RadioOn=0
	iwpriv ra5 set RadioOn=0
	iwpriv ra6 set RadioOn=0
	iwpriv ra7 set RadioOn=0
    fi
fi
