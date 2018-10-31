#!/bin/sh
########################
# l2tp server删除一个账号
# 账号名称等中间空4格
########################

PPP_CHAP_OPT_FILE=/etc/ppp/chap-secrets
PPP_PAP_OPT_FILE=/etc/ppp/pap-secrets

NAME=$1
SRV_NAME=$2
#pap-secrets
if [ -f $PPP_PAP_OPT_FILE ]; then
    sed -i "/$NAME    $SRV_NAME/d" $PPP_PAP_OPT_FILE
fi

#chap-secrets
if [ -f $PPP_CHAP_OPT_FILE ]; then
    sed -i "/$NAME    $SRV_NAME/d" $PPP_CHAP_OPT_FILE
fi
