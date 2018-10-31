#!/bin/sh

#
# This script is used to generate secondary lines for putting into the
# nic machine's named.conf. It uses the file "zones.txt" as input.
#
# This zone should contain lines like:
#   zonename.foo.bar.             {beet,carrot}
#
#

echo "// These lines generated with the gen-secondaries.sh script"

cat zones.txt | sed -e '/^#/d' -e '/^$/d' | while read zone master rest
do
    case $master in
    beet)   ip=192.1.2.129;;
    carrot) ip=192.1.2.130;;
    *) echo Unknown master: $master >&2; continue;;
    esac

    echo 'zone "'$zone'" { type slave;  masters { '$ip';}; file "/etc/bind/sec/db.'$zone'.signed.bak"; };'
done

































#
# $Log: gen-secondaries.sh,v $
# Revision 1.1.1.1  2011/08/17 11:10:48  brwang
# 3520G start
#
# Revision 1.1.1.1  2011/03/15 05:24:32  lei.minghuan
# import ralink
#
# Revision 1.1.1.1  2009/12/14 12:44:50  brwang
# ralink wlan
#
# Revision 1.1.1.1  2007-10-08 08:01:05  steven
#
#
# Revision 1.1  2002/10/24 05:35:46  mcr
# 	file/script to generate secondary list.
#
#
