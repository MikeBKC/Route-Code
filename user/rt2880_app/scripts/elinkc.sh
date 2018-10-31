#!/bin/sh
#NUM=`ps aux| grep elinkc | grep -v grep |wc -l`
NUM=`ps aux| grep lua | grep -v grep |wc -l`
elinkc_flag=`uttcli get sysConf elinkc_flag`

#echo ${elinkc_flag}
#echo ${NUM}
echo "elinkc.sh start"
if [ "$elinkc_flag" == "" ]; then
    #killall elinkc
    killall -9 lua
elif [ "$elinkc_flag" == "No" ]; then
    #killall elinkc
    killall -9 lua
elif [ "$elinkc_flag" == "0" ];then
    #killall elinkc
    killall -9 lua
elif [ "$elinkc_flag" == "yes" ]; then
    if [ "$NUM" -lt "3" ]; then
        #elinkc &
        lua /1elink.lua
    fi
elif [ "$elinkc_flag" == "1" ]; then
    if [ "$NUM" -lt "3" ]; then
        #elinkc &
        lua /1elink.lua
    fi
fi

