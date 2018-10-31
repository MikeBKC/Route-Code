#!/bin/sh

CONF_DIR=/etc/ppp
PPTP_FILE=/etc/options.pptp

if [ ! -n "$3" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <server_ip> KeepAlive"
  exit 0
fi

PPTP_USER_NAME="$1"
PPTP_PASSWORD="$2"
PPTP_MTU="$3"
PPTP_SERVER_IP="$4"
PPTP_AUTHTYPE="$5"
PPTP_OPMODE="$6"
ENCRYPTMODE="$7"
PPTP_OPTIME="$8"

if [ ! -d $CONF_DIR ] ; then mkdir -p $CONF_DIR; fi

echo "noauth" # $PPTP_FILE  
echo "refuse-eap" # $PPTP_FILE
echo "user \"$PPTP_USER_NAME\""  # $PPTP_FILE
echo "password \"$PPTP_PASSWORD\"" # $PPTP_FILE
echo "connect true" # $PPTP_FILE
echo "pty '/bin/pptp $PPTP_SERVER_IP --nolaunchpppd'" # $PPTP_FILE
echo "lock" # $PPTP_FILE
echo "mtu $PPTP_MTU" # $PPTP_FILE
echo "maxfail 0" # $PPTP_FILE
echo "usepeerdns" # $PPTP_FILE
if [ $PPTP_OPMODE == "KeepAlive" ]; then 
    echo "persist" # $PPTP_FILE
#    echo "holdoff $PPTP_OPTIME" # $PPTP_FILE
elif [ $PPTP_OPMODE == "OnDemand" ]; then
    PPTP_OPTIME=`expr $PPTP_OPTIME \* 60`
    echo "demand" # $PPTP_FILE
    echo "idle $PPTP_OPTIME" # $PPTP_FILE
fi

echo "-mschap"
if [ "$PPTP_AUTHTYPE" == "MS-CHAPV2" ]; then
    echo "refuse-chap"
    echo "refuse-pap"
    if [ "$ENCRYPTMODE" == "MPPE" ];then
	echo "require-mppe-128"
    fi
elif [ "$PPTP_AUTHTYPE" == "PAP" ]; then
    echo "-mschap-v2"
    echo "refuse-chap"
elif [ "$PPTP_AUTHTYPE" == "CHAP" ]; then
    echo "-mschap-v2"
    echo "refuse-pap"
fi
#echo "defaultroute" # $PPTP_FILE
echo "ipcp-accept-remote ipcp-accept-local noipdefault" # $PPTP_FILE
echo "ktune" # $PPTP_FILE
echo "default-asyncmap nopcomp noaccomp" # $PPTP_FILE
echo "novj nobsdcomp nodeflate" # $PPTP_FILE
echo "lcp-echo-interval 30" # $PPTP_FILE
echo "lcp-echo-failure 6" # $PPTP_FILE
echo "unit 0" # $PPTP_FILE
