
. /sbin/config.sh
. /sbin/global.sh

#radio_off=`nvram_get 2860 RadioOff`
radio_on=`uttcli getbynm wireless wireless0 active`
moneywifi_on=`uttcli getbynm makemoney makemoney MakeMoneyEn`
moneytime_active=`uttcli getbynm makemoney makemoney wifitime_active`
if [ "$radio_on" == "" ]; then
	radio_on=`uttcli getbynm wireless wireless active`
fi
time_active=`uttcli getbynm wireless wireless0 time_active`
if [ "$time_active" == "" ]; then
	time_active=`uttcli getbynm wireless wireless time_active`
fi
if [ "$time_active" == "" ]; then
    time_active="Yes"
fi
ssid2=`uttcli getbynm wireless wireless0 mbasecfg ssid2`
if [ "$ssid2" == "" ]; then
    ssid2=`uttcli getbynm wireless wireless mbasecfg ssid2`
fi
ssid3=`uttcli getbynm wireless wireless0 mbasecfg ssid3`
if [ "$ssid3" == "" ]; then
    ssid3=`uttcli getbynm wireless wireless mbasecfg ssid3`
fi
ssid4=`uttcli getbynm wireless wireless0 mbasecfg ssid4`
if [ "$ssid4" == "" ]; then
    ssid4=`uttcli getbynm wireless wireless mbasecfg ssid4`
fi
#set bridge_or_route
bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
if [ $bridge_mode_flag == "1" ];then
    pre_mode=br0
    now_mode=br1
else
    pre_mode=br1
    now_mode=br0
fi
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

addWds2Br0()
{
#    wds_en=`nvram_get 2860 WdsEnable`
    wds_en=`uttcli getbynm wireless wireless0 mbasecfg apWds wdsmode`

    if [ "$wds_en" == "" ]; then
	wds_en=`uttcli getbynm wireless wireless mbasecfg apWds wdsmode`
    fi
    if [ "$wds_en" == "repeaterMode" ] || [ "$wds_en" == "lazyMode" ] || [ "$wds_en" == "bridgeMode" ]; then
        ifconfig wds0 up
        ifconfig wds1 up
        ifconfig wds2 up
        ifconfig wds3 up
        #brctl addif br0 wds0
        #brctl addif br0 wds1
        #brctl addif br0 wds2
	    #brctl addif br0 wds3
        brctl delif $pre_mode wds0
        brctl delif $pre_mode wds1
        brctl delif $pre_mode wds2
        brctl delif $pre_mode wds3
        brctl addif $now_mode wds0
        brctl addif $now_mode wds1
        brctl addif $now_mode wds2
        brctl addif $now_mode wds3
    fi
    if [ "$NV1800N" != "" ] && [ "$wds_en" == "apClientMode" ];then
	ifconfig apcli0 up
	#brctl addif br0 apcli0
    brctl delif $pre_mode apcli0
    brctl addif $now_mode apcli0
    fi
    wdsEnable=`uttcli getbynm wireless wireless mbasecfg apClient wdsSwitch`
    if [ "$wdsEnable" == "on" ]; then
	ifconfig apcli0 up
	#brctl addif br0 apcli0
    brctl delif $pre_mode apcli0
    brctl addif $now_mode apcli0
    else
	#brctl delif br0 apcli0 1>/dev/null 2>&1
	brctl delif $now_mode apcli0 1>/dev/null 2>&1
	ifconfig apcli0 down 1>/dev/null 2>&1
    fi
}

#brctl delif br0 ra0 1>/dev/null 2>&1
#brctl delif br0 ra1 1>/dev/null 2>&1
#brctl delif br0 ra2 1>/dev/null 2>&1
#brctl delif br0 ra3 1>/dev/null 2>&1
#brctl delif br0 ra4 1>/dev/null 2>&1
#brctl delif br0 ra5 1>/dev/null 2>&1
#brctl delif br0 ra6 1>/dev/null 2>&1
#brctl delif br0 ra7 1>/dev/null 2>&1

brctl delif $now_mode ra0 1>/dev/null 2>&1
brctl delif $now_mode ra1 1>/dev/null 2>&1
brctl delif $now_mode ra2 1>/dev/null 2>&1
brctl delif $now_mode ra3 1>/dev/null 2>&1
brctl delif $now_mode ra4 1>/dev/null 2>&1

#echo "127"
if [ "$NV1800N" != "" ];then
    brctl delif br0 apcli0 1>/dev/null 2>&1
fi

ifRaxWdsxDown
sleep 2
if [ "$CONFIG_RT2860V2_AP" == "m" ];then
    rmmod rt2860v2_ap
fi
# 2.4G 5G使用同一个模块不能重启
#if [ "$CONFIG_RLT_WIFI" == "m" ];then
#    rmmod rlt_wifi
#fi
if [ "$CONFIG_BOARD_MTK7621_F" != "" ];then
    if [ "$CONFIG_RLT_WIFI" == "m" ];then
    #每次都加载rlt_wifi.ko, 需要保证wlan.sh 比wlan_5g.sh提前调用
    #重复加载没有影响
	insmod /lib/modules/2.6.36/kernel/drivers/net/wireless/rlt_wifi_ap/rlt_wifi.ko
    fi
fi
    sleep 2
    uttcli make_ap_config wireless

if [ "$radio_on" == "Yes" ] && [ "$time_active" == "Yes" ]; then
#    ralink_init make_wireless_config rt2860
#    uttcli make_ap_config wireless
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
if [ "$CONFIG_BOARD_MTK7621_F" == "" ];then
    if [ "$CONFIG_RT2860V2_AP" == "m" ];then
	insmod /lib/modules/2.6.21/kernel/drivers/net/wireless/rt2860v2_ap/rt2860v2_ap.ko
    fi
fi
fi
#if [ "$CONFIG_RALINK_RT5350" != "" ] || [ "$CONFIG_RALINK_RT2880" != "" ];then
#    uttcli set_ra0_macaddr
#fi
    ifconfig ra0 0.0.0.0
    #brctl addif br0 ra0
    brctl delif $pre_mode ra0
    brctl addif $now_mode ra0
    if [ "$ssid2" != "" ]; then
	if [ "$CONFIG_RT2860V2_AP" == "y" ];then
	    ifconfig ra1 up
	fi
	ifconfig ra1 0.0.0.0
	#brctl addif br0 ra1
    brctl delif $pre_mode ra1
    brctl addif $now_mode ra1
    fi
# add ssid 3,4
    if [ "$ssid3" != "" ]; then
	if [ "$CONFIG_RT2860V2_AP" == "y" ];then
	    ifconfig ra2 up
	fi
	ifconfig ra2 0.0.0.0
	#brctl addif br0 ra2
	brctl delif $pre_mode ra2
	brctl addif $now_mode ra2
    fi
    
    if [ "$ssid4" != "" ]; then
	if [ "$CONFIG_RT2860V2_AP" == "y" ];then
	    ifconfig ra3 up
	fi
	ifconfig ra3 0.0.0.0
	#brctl addif br0 ra3
	brctl delif $pre_mode ra3
	brctl addif $now_mode ra3
    fi
#add ssid 3,4
    if [ "$moneywifi_on" == "Enable" ] && [ "$moneytime_active" == "Yes" ]; then
    ifconfig ra2 up
    ifconfig ra2 0.0.0.0
    #brctl addif br0 ra2
	brctl delif $pre_mode ra2
	brctl addif $now_mode ra2

    ifconfig ra3 up
    ifconfig ra3 0.0.0.0
    #brctl addif br0 ra3
	brctl delif $pre_mode ra3
	brctl addif $now_mode ra3
    fi 

    addWds2Br0

    # in order to use broadcast IP address in L2 management daemon
    iwpriv ra0 set IgmpSnEnable=1
    if [ "$ssid2" != "" ]; then
	iwpriv ra1 set IgmpSnEnable=1
    fi
#add 3,4
    if [ "$ssid3" != "" ]; then
    iwpriv ra2 set IgmpSnEnable=1
    fi
    if [ "$ssid4" != "" ]; then
	iwpriv ra3 set IgmpSnEnable=1
    fi
#add 3,4
    if [ "$moneywifi_on" == "Enable" ] && [ "$moneytime_active" == "Yes" ]; then
        iwpriv ra2 set IgmpSnEnable=1
        iwpriv ra3 set IgmpSnEnable=1
    fi
    gpio l 2 2 2 4000 0 1 
else
    iwpriv ra0 set RadioOn=0
    if [ "$ssid2" != "" ]; then
	iwpriv ra1 set RadioOn=0
    fi
#add ssid3,4    
    if [ "$ssid3" != "" ]; then
	iwpriv ra2 set RadioOn=0
    fi
    
    if [ "$ssid4" != "" ]; then
	iwpriv ra3 set RadioOn=0
    fi
#add ssid3,4
    if [ "$moneywifi_on" == "Enable" ] && [ "$moneytime_active" == "Yes" ]; then
        iwpriv ra2 set RadioOn=0
        iwpriv ra3 set RadioOn=0
    fi
    gpio l 2 0 4000 0 0 0
fi
