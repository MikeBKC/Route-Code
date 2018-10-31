
. /sbin/config.sh
. /sbin/global.sh

if [ "$#" == 0 ]; then
offset1=`expr 1 + 2`
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
radio_on=`uttcli getbynm wireless wireless5g active`
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
    iwpriv rai0 set RadioOn=1
    iwpriv rai1 set RadioOn=1
    iwpriv rai2 set RadioOn=1
    iwpriv rai3 set RadioOn=1
    iwpriv rai4 set RadioOn=1
    iwpriv rai5 set RadioOn=1
    iwpriv rai6 set RadioOn=1
    iwpriv rai7 set RadioOn=1

    sleep 1

    ifconfig rai0 down 1>/dev/null 2>&1
    ifconfig rai1 down 1>/dev/null 2>&1
    ifconfig rai2 down 1>/dev/null 2>&1
    ifconfig rai3 down 1>/dev/null 2>&1
    ifconfig rai4 down 1>/dev/null 2>&1
    ifconfig rai5 down 1>/dev/null 2>&1
    ifconfig rai6 down 1>/dev/null 2>&1
    ifconfig rai7 down 1>/dev/null 2>&1

    ifconfig wds0 down 1>/dev/null 2>&1
    ifconfig wds1 down 1>/dev/null 2>&1
    ifconfig wds2 down 1>/dev/null 2>&1
    ifconfig wds3 down 1>/dev/null 2>&1

    ifconfig apcli0 down 1>/dev/null 2>&1
}

brctl delif br0 rai0 1>/dev/null 2>&1
brctl delif br0 rai1 1>/dev/null 2>&1
brctl delif br0 rai2 1>/dev/null 2>&1
brctl delif br0 rai3 1>/dev/null 2>&1
brctl delif br0 rai4 1>/dev/null 2>&1
brctl delif br0 rai5 1>/dev/null 2>&1
brctl delif br0 rai6 1>/dev/null 2>&1
brctl delif br0 rai7 1>/dev/null 2>&1

if [ "$NV1800N" != "" ];then
    brctl delif br0 apcli0 1>/dev/null 2>&1
fi

if [ "$m_radio" -gt 0 ]; then
ifRaxWdsxDown
sleep 1
rmmod MT7610_ap
sleep 1
#    ralink_init make_wireless_config rt2860
    uttcli make_ap_config_5g wireless
    insmod /lib/modules/2.6.36/kernel/drivers/net/wireless/MT7610_ap/MT7610_ap.ko
    ifconfig rai0 0.0.0.0
    brctl addif br0 rai0
	if [ "$bssidnum" = "2" ]; then
		ifconfig rai1 0.0.0.0
	elif [ "$bssidnum" = "3" ]; then
		ifconfig rai1 0.0.0.0
		ifconfig rai2 0.0.0.0
	elif [ "$bssidnum" = "4" ]; then
		ifconfig rai1 0.0.0.0
		ifconfig rai2 0.0.0.0
		ifconfig rai3 0.0.0.0
	elif [ "$bssidnum" = "5" ]; then
		ifconfig rai1 0.0.0.0
		ifconfig rai2 0.0.0.0
		ifconfig rai3 0.0.0.0
		ifconfig rai4 0.0.0.0
	elif [ "$bssidnum" = "6" ]; then
		ifconfig rai1 0.0.0.0
		ifconfig rai2 0.0.0.0
		ifconfig rai3 0.0.0.0
		ifconfig rai4 0.0.0.0
		ifconfig rai5 0.0.0.0
	elif [ "$bssidnum" = "7" ]; then
		ifconfig rai1 0.0.0.0
		ifconfig rai2 0.0.0.0
		ifconfig rai3 0.0.0.0
		ifconfig rai4 0.0.0.0
		ifconfig rai5 0.0.0.0
		ifconfig rai6 0.0.0.0
	elif [ "$bssidnum" = "8" ]; then
		ifconfig rai1 0.0.0.0
		ifconfig rai2 0.0.0.0
		ifconfig rai3 0.0.0.0
		ifconfig rai4 0.0.0.0
		ifconfig rai5 0.0.0.0
		ifconfig rai6 0.0.0.0
		ifconfig rai7 0.0.0.0
	fi
    if [ "$bssidnum" = "2" ]; then
	    brctl addif br0 rai1
    elif [ "$bssidnum" = "3" ]; then
	    brctl addif br0 rai1
	    brctl addif br0 rai2
    elif [ "$bssidnum" = "4" ]; then
	    brctl addif br0 rai1
	    brctl addif br0 rai2
	    brctl addif br0 rai3
    elif [ "$bssidnum" = "5" ]; then
	    brctl addif br0 rai1
	    brctl addif br0 rai2
	    brctl addif br0 rai3
	    brctl addif br0 rai4
    elif [ "$bssidnum" = "6" ]; then
	    brctl addif br0 rai1
	    brctl addif br0 rai2
	    brctl addif br0 rai3
	    brctl addif br0 rai4
	    brctl addif br0 rai5
    elif [ "$bssidnum" = "7" ]; then
	    brctl addif br0 rai1
	    brctl addif br0 rai2
	    brctl addif br0 rai3
	    brctl addif br0 rai4
	    brctl addif br0 rai5
	    brctl addif br0 rai6
    elif [ "$bssidnum" = "8" ]; then
	    brctl addif br0 rai1
	    brctl addif br0 rai2
	    brctl addif br0 rai3
	    brctl addif br0 rai4
	    brctl addif br0 rai5
	    brctl addif br0 rai6
	    brctl addif br0 rai7
    fi

#ÉèÖÃbr0 macµØÖ·
#if [ "$CONFIG_WLAN_LAN_ISOLATION" != "" ];then
#mac=`uttcli get interface 0 ethernet mac`
#else
#mac=`ifconfig eth2 | grep HWaddr  | sed 's/eth2      Link encap:Ethernet  HWaddr//'`
#fi
#if [ "$mac" != "00:00:00:00:00:00" ]; then
#    ifconfig br0 hw ether $mac up
#fi

    # in order to use broadcast IP address in L2 management daemon
    iwpriv rai0 set IgmpSnEnable=1 1>/dev/null 2>&1
    if [ "$bssidnum" = "2" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "3" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai2 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "4" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai3 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "5" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai4 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "6" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai4 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai5 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "7" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai4 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai5 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai6 set IgmpSnEnable=1 1>/dev/null 2>&1
    elif [ "$bssidnum" = "8" ]; then
	iwpriv rai1 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai2 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai3 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai4 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai5 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai6 set IgmpSnEnable=1 1>/dev/null 2>&1
	iwpriv rai7 set IgmpSnEnable=1 1>/dev/null 2>&1
    fi
    
else
    iwpriv rai0 set RadioOn=0
    if [ "$bssidnum" = "2" ]; then
	iwpriv rai1 set RadioOn=0
    elif [ "$bssidnum" = "3" ]; then
	iwpriv rai1 set RadioOn=0
	iwpriv rai2 set RadioOn=0
    elif [ "$bssidnum" = "4" ]; then
	iwpriv rai1 set RadioOn=0
	iwpriv rai2 set RadioOn=0
	iwpriv rai3 set RadioOn=0
    elif [ "$bssidnum" = "5" ]; then
	iwpriv rai1 set RadioOn=0
	iwpriv rai2 set RadioOn=0
	iwpriv rai3 set RadioOn=0
	iwpriv rai4 set RadioOn=0
    elif [ "$bssidnum" = "6" ]; then
	iwpriv rai1 set RadioOn=0
	iwpriv rai2 set RadioOn=0
	iwpriv rai3 set RadioOn=0
	iwpriv rai4 set RadioOn=0
	iwpriv rai5 set RadioOn=0
    elif [ "$bssidnum" = "7" ]; then
	iwpriv rai1 set RadioOn=0
	iwpriv rai2 set RadioOn=0
	iwpriv rai3 set RadioOn=0
	iwpriv rai4 set RadioOn=0
	iwpriv rai5 set RadioOn=0
	iwpriv rai6 set RadioOn=0
    elif [ "$bssidnum" = "8" ]; then
	iwpriv rai1 set RadioOn=0
	iwpriv rai2 set RadioOn=0
	iwpriv rai3 set RadioOn=0
	iwpriv rai4 set RadioOn=0
	iwpriv rai5 set RadioOn=0
	iwpriv rai6 set RadioOn=0
	iwpriv rai7 set RadioOn=0
    fi
fi
