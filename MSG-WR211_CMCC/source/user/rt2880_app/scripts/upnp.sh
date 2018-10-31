. /sbin/global.sh
killall upnpd
getLanIfName
upnp1=`uttcli get interface 1 UpnpActive`
ip=`uttcli get interface 0 ethernet static ip`
wanif=`uttif gnbi 1`
route del -net 239.0.0.0 netmask 255.0.0.0 dev $lan_if
if [ -r "/tmp/upnp" ]; then
    rm /tmp/upnp
fi
if [ $upnp1 = 1 ]; then
    route add -net 239.0.0.0 netmask 255.0.0.0 dev $lan_if
    upnp_xml.sh $ip
    upnpd -f $wanif $lan_if &
fi
