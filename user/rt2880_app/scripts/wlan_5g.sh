
. /sbin/config.sh
. /sbin/global.sh

radio_on=`uttcli getbynm wireless wireless5g active`
time_active=`uttcli getbynm wireless wireless5g time_active`
moneywifi_on=`uttcli getbynm makemoney makemoney MakeMoneyEn`
moneytime_active=`uttcli getbynm makemoney makemoney wifitime_active`
if [ "$time_active" == "" ]; then
	time_active="Yes"
fi
#echo time_active="$time_active"
ssid2=`uttcli getbynm wireless wireless5g mbasecfg ssid2`
wifi_mod_path="/lib/modules/2.6.36/kernel/drivers/net/wireless/"

if [ "$CONFIG_RLT_WIFI" != "" ]; then
    w5g_mod_path="rlt_wifi_ap/"
    w5g_mod_name="rlt_wifi"
else
    w5g_mod_path="MT7610_ap/"
    w5g_mod_name="MT7610_ap"
fi
w5g_mod="${wifi_mod_path}${w5g_mod_path}${w5g_mod_name}.ko"
ifRaxWdsxDown()
{
    ifconfig rai0 down 1>/dev/null 2>&1
    ifconfig rai1 down 1>/dev/null 2>&1
    ifconfig rai2 down 1>/dev/null 2>&1
    ifconfig rai3 down 1>/dev/null 2>&1
    ifconfig rai4 down 1>/dev/null 2>&1
    ifconfig rai5 down 1>/dev/null 2>&1
    ifconfig rai6 down 1>/dev/null 2>&1
    ifconfig rai7 down 1>/dev/null 2>&1

    ifconfig wdsi0 down 1>/dev/null 2>&1
    ifconfig wdsi1 down 1>/dev/null 2>&1
    ifconfig wdsi2 down 1>/dev/null 2>&1
    ifconfig wdsi3 down 1>/dev/null 2>&1

    ifconfig apclii0 down 1>/dev/null 2>&1
}
#set bridge_or_route
bridge_mode_flag=`uttcli get sysConf brideg_mode_flag`
if [ $bridge_mode_flag == "1" ];then
    pre_mode=br0
    now_mode=br1
else
    pre_mode=br1
    now_mode=br0
fi
addWds2Br0()
{
    wds_en=`uttcli getbynm wireless wireless5g mbasecfg apWds wdsmode`
    if [ "$wds_en" == "" ]; then
	wds_en=`uttcli getbynm wireless wireless5g mbasecfg apWds wdsmode`
    fi
    if [ "$wds_en" == "repeaterMode" ]; then
        ifconfig wdsi0 up
        ifconfig wdsi1 up
        ifconfig wdsi2 up
        ifconfig wdsi3 up
        #brctl addif br0 wdsi0
        #brctl addif br0 wdsi1
        #brctl addif br0 wdsi2
    	#brctl addif br0 wdsi3
        brctl delif $pre_mode wdsi0
        brctl delif $pre_mode wdsi1
        brctl delif $pre_mode wdsi2
        brctl delif $pre_mode wdsi3
        brctl addif $now_mode wdsi0
        brctl addif $now_mode wdsi1
        brctl addif $now_mode wdsi2
        brctl addif $now_mode wdsi3
    fi
    wdsEnable=`uttcli getbynm wireless wireless5g mbasecfg apClient wdsSwitch`
    if [ "$wdsEnable" == "on" ]; then
	ifconfig apclii0 up
	#brctl addif br0 apclii0
    brctl delif $pre_mode apclii0
    brctl addif $now_mode apclii0
    fi
}

#brctl delif br0 rai0 1>/dev/null 2>&1
#brctl delif br0 rai1 1>/dev/null 2>&1
#brctl delif br0 rai2 1>/dev/null 2>&1
#brctl delif br0 rai3 1>/dev/null 2>&1
#brctl delif br0 rai4 1>/dev/null 2>&1
#brctl delif br0 rai5 1>/dev/null 2>&1
#brctl delif br0 rai6 1>/dev/null 2>&1
#brctl delif br0 rai7 1>/dev/null 2>&1

brctl delif $pre_mode rai0 1>/dev/null 2>&1
brctl delif $pre_mode rai1 1>/dev/null 2>&1
brctl delif $pre_mode rai2 1>/dev/null 2>&1
brctl delif $pre_mode rai3 1>/dev/null 2>&1
if [ "$NV1800N" != "" ];then
    brctl delif br0 apcli0 1>/dev/null 2>&1
fi

ifRaxWdsxDown
sleep 2
if [ "$CONFIG_BOARD_MTK7621_F" == "" ];then
rmmod ${w5g_mod_name}
fi
sleep 1
    uttcli make_ap_config_5g wireless
if [ "$CONFIG_BOARD_MTK7621_F" == "" ];then
    insmod ${w5g_mod}
fi
if [ "$radio_on" == "Yes" ] && [ "$time_active" == "Yes" ]; then
    sleep 1
#    uttcli make_ap_config_5g wireless

#if [ "$CONFIG_BOARD_MTK7621_F" == "" ];then
#    insmod ${w5g_mod}
#fi

    ifconfig rai0 0.0.0.0
    #brctl addif br0 rai0
    brctl delif $pre_mode rai0
    brctl addif $now_mode rai0
    if [ "$ssid2" != "" ]; then
	ifconfig rai1 0.0.0.0
	#brctl addif br0 rai1
	brctl delif $pre_mode rai1
	brctl addif $now_mode rai1
    fi
    if [ "$moneywifi_on" == "Enable" ] && [ "$moneytime_active" == "Yes" ]; then
        ifconfig rai2 0.0.0.0
        #brctl addif br0 rai2
        brctl delif $pre_mode rai2
        brctl addif $now_mode rai2
        ifconfig rai3 0.0.0.0
        #brctl addif br0 rai3
        brctl delif $pre_mode rai3
        brctl addif $now_mode rai3
    fi
    addWds2Br0
else
    iwpriv rai0 set RadioOn=0
    if [ "$ssid2" != "" ]; then
	iwpriv rai1 set RadioOn=0
    fi
    if [ "$moneywifi_on" == "Enable" ] && [" $moneytime_active" == "Yes" ]; then
        iwpriv rai2 set RadioOn=0
        iwpriv rai3 set RadioOn=0
    fi
fi
