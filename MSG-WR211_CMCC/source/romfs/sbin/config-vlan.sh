#!/bin/sh

# $Id: config-vlan.sh,v 1.1.2.1 2017/12/26 17:36:00 wu.xiaoyu Exp $
. /sbin/config.sh

vconfig rem eth2.1 2>/dev/null
vconfig rem eth2.2 2>/dev/null
vconfig rem eth2.3 2>/dev/null
vconfig rem eth2.4 2>/dev/null
vconfig rem eth2.5 2>/dev/null
vconfig add eth2 1
#set untag
switch reg w 100 403 #vlan0&vlan1 untag
#set PVID
if [ "$1" = "bridge" ];then
switch reg w 40 1001 #port0&port1 
switch reg w 44 1001
switch reg w 48 1 #port4&port5 
#VLAN member port 
switch vlan set 0 1 1111111 
else
vconfig add eth2 2
switch reg w 40 1001 #port0&port1VLAN member port             
switch reg w 44 2001 #port2&port3
switch reg w 48 1 #port4&port5   
#VLAN member port                
switch vlan set 0 1 1110111      
switch vlan set 1 2 0001011  
fi
#clear mac table if vlan configuration changed
switch clear

