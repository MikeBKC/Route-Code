#!/bin/sh

# $Id: config-vlan.sh,v 1.5.164.3 2016/07/19 10:35:51 chen.kaiyu Exp $
. /sbin/config.sh

vconfig rem eth2.1 2>/dev/null
vconfig rem eth2.2 2>/dev/null
vconfig rem eth2.3 2>/dev/null
vconfig rem eth2.4 2>/dev/null
vconfig rem eth2.5 2>/dev/null
vconfig add eth2 1
vconfig add eth2 2
#set untag
switch reg w 100 403 #vlan0&vlan1 untag
#set PVID
switch reg w 40 1001 #port0&port1
switch reg w 44 1001 #port2&port3
switch reg w 48 2 #port4&port5
#VLAN member port
switch vlan set 0 1 1111011
switch vlan set 1 2 0000111
#clear mac table if vlan configuration changed
switch clear

