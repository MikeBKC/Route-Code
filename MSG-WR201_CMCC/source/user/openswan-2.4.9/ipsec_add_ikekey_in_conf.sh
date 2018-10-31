#!/bin/sh
IPSEC_CONF_FILE=/etc/ipsec.conf

if [ ! -n "$6" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <auth_type:rsa/psk> <encrypt:3des/aes128/null>"
	echo "          <auth:md5/sha1> <key_grp:modp768/modp1024/modp1536>"
	echo "          <ike_lefetime> <pfs:yes/no>"
	exit 0
fi

TYPE="$1" #RSA/PSK
ENCRYPT="$2" # Encryption: 3des, aes128, null
AUTH="$3" # Auth: md5, sha1
KEY_GRP="$4" # Key Group: modp768, modp1024, modp1536
IKE_LIFE_TIME="$5" # 3600s
PFS="$6" # Perfect Forward Secrecy: yes, no

echo "  auth=esp" >> $IPSEC_CONF_FILE
echo "  esp=$ENCRYPT-$AUTH" >> $IPSEC_CONF_FILE

if [ $TYPE = "rsa" ]; then
echo "  authby=rsasig" >> $IPSEC_CONF_FILE
else  
echo "  authby=secret" >> $IPSEC_CONF_FILE
fi

echo "  ike=$ENCRYPT-$AUTH-$KEY_GRP" >> $IPSEC_CONF_FILE
echo "  ikelifetime=$IKE_LIFE_TIME" >> $IPSEC_CONF_FILE
echo "  pfs=$PFS" >> $IPSEC_CONF_FILE

