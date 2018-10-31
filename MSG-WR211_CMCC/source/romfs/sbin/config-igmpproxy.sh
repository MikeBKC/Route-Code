#!/bin/sh
#
# $Id: config-igmpproxy.sh,v 1.1.1.1 2011/08/17 11:10:39 brwang Exp $
#
# usage: config-igmpproxy.sh <wan_if_name> <lan_if_name>
#

. /sbin/global.sh

igmpproxy.sh $wan_if $lan_if ppp0
killall -q igmpproxy
igmpproxy

