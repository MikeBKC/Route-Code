#!/bin/sh

CONF_DIR=/etc/ppp
L2TP_FILE=/etc/options.l2tp

if [ ! -n "$4" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <remotename> <user> <passwd> <mtu>"
  exit 0
fi

L2TP_REMOTE="$1"
L2TP_USER_NAME="$2"
L2TP_PASSWORD="$3"
L2TP_MTU="$4"
#L2TP_SERVER_IP="$4"
L2TP_AUTHTYPE="$5"
#L2TP_OPMODE="$6"
#ENCRYPTMODE="$7"
#L2TP_OPTIME="$8"

if [ ! -d $CONF_DIR ] ; then mkdir -p $CONF_DIR; fi

echo "refuse-eap" # $L2TP_FILE
echo "remotename $L2TP_REMOTE"
echo "user \"$L2TP_USER_NAME\""  # $L2TP_FILE
echo "password \"$L2TP_PASSWORD\"" # $L2TP_FILE
echo "noauth" # $L2TP_FILE  
echo "unit 0"
echo "lock" # $L2TP_FILE
echo "connect true" # $L2TP_FILE
#echo "pty '/bin/pptp $L2TP_SERVER_IP --nolaunchpppd'" # $L2TP_FILE
echo "mtu $L2TP_MTU" # $L2TP_FILE
echo "maxfail 0" # $L2TP_FILE
echo "usepeerdns" # $L2TP_FILE
echo "persist" # $L2TP_FILE
#if [ $L2TP_OPMODE == "KeepAlive" ]; then 
#    echo "persist" # $L2TP_FILE
#    echo "holdoff $L2TP_OPTIME" # $L2TP_FILE
#elif [ $L2TP_OPMODE == "OnDemand" ]; then
#    L2TP_OPTIME=`expr $L2TP_OPTIME \* 60`
#    echo "demand" # $L2TP_FILE
#    echo "idle $L2TP_OPTIME" # $L2TP_FILE
#fi

echo "-mschap"
if [ "$L2TP_AUTHTYPE" == "MS-CHAPV2" ]; then
    echo "refuse-chap"
    echo "refuse-pap"
    if [ "$ENCRYPTMODE" == "MPPE" ];then
	echo "require-mppe-128"
    fi
elif [ "$L2TP_AUTHTYPE" == "PAP" ]; then
    echo "-mschap-v2"
    echo "refuse-chap"
elif [ "$L2TP_AUTHTYPE" == "CHAP" ]; then
    echo "-mschap-v2"
    echo "refuse-pap"
fi
echo "nodefaultroute" # $L2TP_FILE
echo "ipcp-accept-remote"
echo "ipcp-accept-local"
echo "noipdefault" # $L2TP_FILE
#echo "ktune" # $L2TP_FILE
echo "default-asyncmap nopcomp noaccomp" # $L2TP_FILE
echo "novj nobsdcomp nodeflate" # $L2TP_FILE
echo "lcp-echo-interval 30" # $L2TP_FILE
echo "lcp-echo-failure 6" # $L2TP_FILE
