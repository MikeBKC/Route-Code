#!/bin/sh

PPPOE_FILE=/etc/options.pppoe

if [ ! -n "$4" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <eth_name> <opmode>"
  exit 0
fi

PPPOE_USER_NAME="$1"
PPPOE_PASSWORD="$2"
PPPOE_IF="$3"
PPPOE_OPMODE="$4"
PPPOE_IDLETIME="$5"

if [ $6 == "NONE" ]; then
    PPPOE_USER_NAME=""
    PPPOE_PASSWORD=""
fi

echo "noauth" > $PPPOE_FILE
echo "user '$PPPOE_USER_NAME'" >> $PPPOE_FILE
echo "password '$PPPOE_PASSWORD'" >> $PPPOE_FILE
echo "nomppe" >> $PPPOE_FILE
echo "hide-password" >> $PPPOE_FILE
echo "noipdefault" >> $PPPOE_FILE
echo "defaultroute" >> $PPPOE_FILE
echo "nodetach" >> $PPPOE_FILE
echo "usepeerdns" >> $PPPOE_FILE
if [ $6 == "PAP" ]; then
    echo "refuse-chap" >> $PPPOE_FILE
elif [ $6 == "CHAP" ]; then
    echo "refuse-pap" >> $PPPOE_FILE
fi
if [ $PPPOE_OPMODE == "KeepAlive" ]; then
	echo "persist" >> $PPPOE_FILE
elif [ $PPPOE_OPMODE == "OnDemand" ]; then
	PPPOE_IDLETIME=`expr $PPPOE_IDLETIME \* 60`
	echo "demand" >> $PPPOE_FILE
	echo "idle $PPPOE_IDLETIME" >> $PPPOE_FILE
fi
echo "ipcp-accept-remote" >> $PPPOE_FILE 
echo "ipcp-accept-local" >> $PPPOE_FILE 
echo "lcp-echo-failure 3" >> $PPPOE_FILE
echo "lcp-echo-interval 60" >> $PPPOE_FILE
#echo "ktune" >> $PPPOE_FILE
#echo "default-asyncmap nopcomp noaccomp" >> $PPPOE_FILE
#echo "novj nobsdcomp nodeflate" >> $PPPOE_FILE
echo "plugin /etc_ro/ppp/plugins/rp-pppoe.so $PPPOE_IF" >> $PPPOE_FILE

