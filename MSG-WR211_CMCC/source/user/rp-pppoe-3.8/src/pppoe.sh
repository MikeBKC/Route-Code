#!/bin/sh


if [ ! -n "$6" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <wannum> <optionfile> <user> <password> <eth_name> <opmode>"
  exit 0
fi

PPPOE_USER_NAME="$3"
PPPOE_PASSWORD="$4"
PPPOE_IF="$5"
PPPOE_OPMODE="$6"
PPPOE_IDLETIME="$9"
PPPOE_FILE=$2
PPPOE_MTU=$8

if [ "$7" == "NONE" ]; then
    PPPOE_USER_NAME=""
    PPPOE_PASSWORD=""
fi
if [ "$8" == "" ]; then
    PPPOE_MTU=1492
fi

echo "noauth" > $PPPOE_FILE
echo "user '$PPPOE_USER_NAME'" >> $PPPOE_FILE
echo "password '$PPPOE_PASSWORD'" >> $PPPOE_FILE
echo "nomppe" >> $PPPOE_FILE
echo "hide-password" >> $PPPOE_FILE
echo "noipdefault" >> $PPPOE_FILE
#echo "defaultroute" >> $PPPOE_FILE
echo "nodetach" >> $PPPOE_FILE
echo "usepeerdns" >> $PPPOE_FILE
echo "mtu $PPPOE_MTU" >> $PPPOE_FILE
if [ "$7" == "PAP" ]; then
    echo "refuse-chap" >> $PPPOE_FILE
elif [ "$7" == "CHAP" ]; then
    echo "refuse-pap" >> $PPPOE_FILE
fi
echo "-mschap" >> $PPPOE_FILE
echo "-mschap-v2" >> $PPPOE_FILE
if [ $PPPOE_OPMODE != "DEMAND" ]; then
	echo "persist" >> $PPPOE_FILE
elif [ $PPPOE_OPMODE == "DEMAND" ]; then
	PPPOE_IDLETIME=`expr $PPPOE_IDLETIME \* 60`
	echo "demand" >> $PPPOE_FILE
	echo "idle $PPPOE_IDLETIME" >> $PPPOE_FILE
fi
echo "ipcp-accept-remote" >> $PPPOE_FILE 
echo "ipcp-accept-local" >> $PPPOE_FILE 
echo "lcp-echo-failure 10" >> $PPPOE_FILE
echo "lcp-echo-interval 5" >> $PPPOE_FILE
echo "linkname wan$1" >> $PPPOE_FILE
#echo "ktune" >> $PPPOE_FILE
#echo "default-asyncmap nopcomp noaccomp" >> $PPPOE_FILE
#echo "novj nobsdcomp nodeflate" >> $PPPOE_FILE
echo "plugin /etc_ro/ppp/plugins/rp-pppoe.so $PPPOE_IF" >> $PPPOE_FILE

