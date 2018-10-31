#!/bin/sh
###########################
# pptp server���һ���˺�
# �˺�������м���4���ո�
###########################
PPP_CHAP_OPT_FILE=/etc/ppp/chap-secrets
PPP_PAP_OPT_FILE=/etc/ppp/pap-secrets

NAME=$1
SRV_NAME=$2
PASSWD=$3
ASS_IP=$4

# ��pap-secrets�ļ�
if [ ! -f $PPP_PAP_OPT_FILE ]; then
    touch $PPP_PAP_OPT_FILE
fi

echo -e "$NAME    $SRV_NAME    $PASSWD    $ASS_IP" >> $PPP_PAP_OPT_FILE

# ��chap-secrets�ļ�
if [ ! -f $PPP_CHAP_OPT_FILE ]; then
    touch $PPP_CHAP_OPT_FILE
fi

echo -e "$NAME    $SRV_NAME    $PASSWD    $ASS_IP" >> $PPP_CHAP_OPT_FILE
