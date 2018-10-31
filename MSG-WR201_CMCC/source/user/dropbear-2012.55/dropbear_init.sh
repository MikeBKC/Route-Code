#!/bin/sh
dropbear_file_path=/etc/dropbear
mkdir -p $dropbear_file_path
/usr/local/bin/dropbearkey -t rsa -f $dropbear_file_path/dropbear_rsa_host_key
/usr/local/bin/dropbearkey -t dss -f $dropbear_file_path/dropbear_dss_host_key
if [ "$1" = "1" ];then
dropbear.sh $2
fi
