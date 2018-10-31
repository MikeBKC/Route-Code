#!/bin/sh
########################
# pptp server删除一个账号
# 账号名称等中间空4格
########################

PPP_CHAP_OPT_FILE=/etc/ppp/chap-secrets
PPP_PAP_OPT_FILE=/etc/ppp/pap-secrets

NAME=$1
SRV_NAME=$2

#删pap-secrets文件
if [ -f $PPP_PAP_OPT_FILE ]; then
    sed -i "/$NAME    $SRV_NAME/d" $PPP_PAP_OPT_FILE
fi

#删chap-secrets文件
if [ -f $PPP_CHAP_OPT_FILE ]; then
    sed -i "/$NAME    $SRV_NAME/d" $PPP_CHAP_OPT_FILE
fi
