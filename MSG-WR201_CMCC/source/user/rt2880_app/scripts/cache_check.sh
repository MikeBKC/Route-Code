#!/bin/sh
#
# $Id: cache_check.sh,v 1.2 2013/06/03 16:02:09 chen.bin Exp $
#
# usage: nat.sh <wanNo> <A(dd)|D(el)>
#

. /sbin/global.sh

host=$1
sleep 1
killall cachecheck
sleep 1
cachecheck $host&


