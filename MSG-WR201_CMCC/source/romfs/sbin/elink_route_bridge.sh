#!/bin/sh
switch_restore()
{
    switch reg w 14 5555
    switch reg w 40 1001
    switch reg w 44 1001
    switch reg w 48 1001
    switch reg w 4c 1
    switch reg w 50 2001
    switch reg w 70 ffffffff
    switch reg w 98 7f7f
    switch reg w e4 7f
    switch clear
}
mac=`uttcli get interface 1 ethernet mac`
    brctl delif br0 eth2.1
    ifconfig br0 down
    brctl delif br1 eth2.2
    vconfig rem eth2.1    
    vconfig rem eth2.2    
    brctl addif br1 eth2  
    ifconfig eth2 down  
    ifconfig eth2 hw ether $mac up
    brctl delif br0 ra0
    brctl addif br1 ra0
    switch_restore
    killall udhcpd
    uttcli2 set sysConf  sysConf Elink_flag 1
