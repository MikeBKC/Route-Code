. /sbin/global.sh $1

if [ $# != 3 ]; then
    echo "Usage: edit_scope_route.sh <wanNo> <action> <linkstatus>"
    echo "       <action>:1 up; 2 down}";
    exit;
fi

wanNo=$1
action=$2
linkstatus=$3
getWanIfName $wanNo
#wanmode=`nvram_get 2860 Wan${wanNo}ConnMode`
wanmode=`uttcli get interface ${wanNo} ethernet connMode`
if [ "$wanmode" = "PPPOE" ]; then
    wan_if=`cat /var/run/$wan_if.ppp|sed "s/ /\n/g" |grep ppp`
fi

scope_route=`ip route show dev $wan_if proto kernel  scope link`
if [ -z "$scope_route" ]; then 
    scope_route=`ip route show table default dev $wan_if proto kernel  scope link`
    if [ -z "$scope_route" ]; then
        exit
    fi
    ip route del table default dev $wan_if proto kernel  scope link $scope_route 
else
    ip route del dev $wan_if proto kernel  scope link $scope_route
fi
new_route="`echo $scope_route|sed "s/metric.*$//g"` metric  ${action}${wanNo}"
if [ $action = "2" ]; then
#wanStatus=`nvram_get 2860 Wan${wanNo}Status`
    if [ $linkstatus = "0" ]; then
        TABLE="table default"
    fi
fi
ip route replace $TABLE dev $wan_if proto kernel  scope link $new_route
