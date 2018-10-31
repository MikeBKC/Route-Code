#!/bin/sh
#
# $Id: config-mdhcp-init.sh,v 1.2 2014/01/17 11:50:16 chen.kun Exp $
#
# usage: see function usage()
#

. /sbin/config.sh
. /sbin/global.sh

fname="/etc/udhcpd.conf"
fbak="/etc/udhcpd.conf_bak"
pidfile="/var/run/udhcpd.pid"
leases="/var/udhcpd.leases"

usage () {
  echo "usage: config-mdhcp.sh [option]..."
  echo "options:"
  echo "  -h              : print this help"
  echo "  -v vid          : set vlan id"
  echo "  -s ipaddr       : set ipaddr as start of the IP lease block"
  echo "  -e ipaddr       : set ipaddr as end of the IP lease block"
  echo "  -i ifc          : set ifc as the interface that udhcpd will use"
  echo "  -d dns1 [dns2]  : set dns1 and dns2 as DNS"
  echo "  -m mask         : set mask as subnet netmask"
  echo "  -g gateway      : set gateway as router's IP address"
  echo "  -t time         : set time seconds as the IP life time"
  echo "  -r [sleep_time] : run dhcp server"
  echo "  -p              : pidfile /var/run/pidfile.pid"
  echo "  -k              : kill the running dhcp server"
  echo "  -S [mac ipaddr] : statically assign IP to given MAC address"
  exit
}

config () {
  case "$1" in
    "-v")
#      sed -e '/vid/d' $fname > $fbak
	  cat $fname > $fbak
      echo "vid $2" >> $fbak ;;
    "-s")
#      sed -e '/start/d' $fname > $fbak
	  cat $fname > $fbak
      echo "start $2" >> $fbak ;;
    "-e")
#     sed -e '/end/d' $fname > $fbak
	  cat $fname > $fbak
      echo "end $2" >> $fbak ;;
    "-i")
	  sed -e '/interface/d' $fname > $fbak
      echo "interface $2" >> $fbak ;;
    "-d")
#      sed -e '/option *dns/d' $fname > $fbak
	  cat $fname > $fbak
      echo "option dns $2 $3" >> $fbak ;;
    "-m")
#      sed -e '/option *subnet/d' $fname > $fbak
	  cat $fname > $fbak
      echo "option subnet $2" >> $fbak ;;
    "-g")
#      sed -e '/option *router/d' $fname > $fbak
	  cat $fname > $fbak
      echo "option router $2" >> $fbak ;;
    "-t")
#      sed -e '/option *lease/d' $fname > $fbak
	  cat $fname > $fbak
      echo "option lease $2" >> $fbak ;;
    "-p")
	  sed -e '/pidfile/d' $fname > $fbak
	  echo "pidfile $pidfile" >> $fbak;;
    "-r")
      if [ -e "/etc/udhcpd.$2.tmp" ]; then
        cat /etc/udhcpd.$2.tmp >> $fname
      fi
	  touch $leases > null 2>&1
      sed -e '/lease_file/d' $fname > $fbak
      echo "lease_file $leases" >> $fbak;;
    "-S")
      if [ "$2" = "" ]; then
        sed -e '/static_lease/d' $fname > $fbak
      elif [ "$3" = "" ]; then
	echo "insufficient arguments.."
	usage
      else
        echo "static_lease $2 $3" >> $fname
	return
      fi
      ;;
    *) return;;
  esac
  cat $fbak > $fname
  rm -f $fbak
  return
}


# argv 1 is empty
if [ "$1" = "" ]; then
  usage
fi

touch $fname
case "$1" in
  "-h") usage;;
  "-v") config "$1" "$3";;
  "-s") config "$1" "$3";;
  "-e") config "$1" "$3";;
  "-i") config "$1" "$3";;
  "-d") config "$1" "$3" "$4";;
  "-m") config "$1" "$3";;
  "-g") config "$1" "$3";;
  "-t") config "$1" "$3";;
  "-S") config "$1" "$3" "$4";;
  "-p") config "$1";;
  "-r") config "$1" "$3";;
  "-k")
    if [ -e ${pidfile} ]; then
      kill `cat $pidfile`
    fi
    rm -f $pidfile ;;
  *) usage;;
esac
