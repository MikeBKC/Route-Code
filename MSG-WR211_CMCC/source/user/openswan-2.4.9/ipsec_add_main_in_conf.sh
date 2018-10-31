#!/bin/sh
IPSEC_CONF_FILE=/etc/ipsec.conf
IPSEC_SECRET_FILE=/etc/ipsec.secrets

if [ ! -n "${10}" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <tunnel_name> <auth_type:psk/rsa> <left_id> <left> <left_subnet> "
	echo "          <right_id> <right> <right_subnet> <nat-t:yes/no> <pskey/rsakey> "
	echo "key:"
	echo "		-psk=pre shared key"
	echo "		-rsa=right rsa key"
	echo "right_subnet:"
	echo "		-Single: x.x.x.x/32"
	echo "		-Subnet: x.x.x.x/[0~32]"
	echo "		-Any: %any"
	echo "		-NAT-T: %any"
	exit 0
fi

TUNNEL_NAME="$1"
AUTH_TYPE="$2" #PSK/RSA
LEFTID="$3"
LEFT="$4" # x.x.x.x
LEFT_SUBNET="$5" # x.x.x.x/x
RIGHTID="$6"
RIGHT="$7" # x.x.x.x
RIGHT_SUBNET="$8" 
NAT_TRAVERSAL="$9" #yes/no
KEY="${10}"

echo "Generate connection profile to /etc/ipsec.conf"

if [ ! -f $IPSEC_CONF_FILE ]; then
echo "version 2.0" >>  $IPSEC_CONF_FILE
echo "" >>  $IPSEC_CONF_FILE
echo "config setup" >> $IPSEC_CONF_FILE
echo "  interfaces=%defaultroute" >> $IPSEC_CONF_FILE
echo "  klipsdebug=none" >> $IPSEC_CONF_FILE
echo "  plutodebug=none" >> $IPSEC_CONF_FILE
echo "  nat_traversal=$NAT_TRAVERSAL" >> $IPSEC_CONF_FILE
echo "" >> $IPSEC_CONF_FILE
fi

echo "conn $TUNNEL_NAME" >> $IPSEC_CONF_FILE
echo "  left=$LEFT" >> $IPSEC_CONF_FILE
echo "  leftsubnet=$LEFT_SUBNET" >> $IPSEC_CONF_FILE
echo "  leftid=$LEFTID" >> $IPSEC_CONF_FILE
echo "  leftnexthop=%defaultroute" >> $IPSEC_CONF_FILE

if [ $AUTH_TYPE = "rsa" ]; then
ipsec showhostkey --left >> $IPSEC_CONF_FILE
fi

echo "  right=$RIGHT" >> $IPSEC_CONF_FILE
echo "  rightsubnet=$RIGHT_SUBNET" >> $IPSEC_CONF_FILE
echo "  rightid=$RIGHTID" >> $IPSEC_CONF_FILE
if [ $AUTH_TYPE = "rsa" ]; then
echo "  rightrsasigkey=$KEY" >> $IPSEC_CONF_FILE
fi

echo "  auto=add" >> $IPSEC_CONF_FILE

echo " Add PSA Key to /etc/ipsec.secrets"
echo "$LEFT $RIGHT : PSK \"0x$KEY\" " >> $IPSEC_SECRET_FILE

