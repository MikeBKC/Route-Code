#!/bin/sh
########################
# pptp serverɾ�������˺�
# �˺����Ƶ��м��4��
########################

PPP_CHAP_OPT_FILE=/etc/ppp/chap-secrets
PPP_PAP_OPT_FILE=/etc/ppp/pap-secrets

SRV_NAME=$1

#ɾpap-secrets
if [ -f $PPP_PAP_OPT_FILE ]; then
    sed -i "/$SRV_NAME/d" $PPP_PAP_OPT_FILE
fi

#ɾchap-secrets
if [ -f $PPP_CHAP_OPT_FILE ]; then
    sed -i "/$SRV_NAME/d" $PPP_CHAP_OPT_FILE
fi
