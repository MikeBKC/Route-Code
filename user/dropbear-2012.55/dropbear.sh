#!/bin/sh
dropbear_path=/usr/local/sbin
killall dropbear
$dropbear_path/dropbear -p $1
