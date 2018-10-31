#!/bin/sh
##############################
# l2tp server 全局配置修改脚本
##############################

# 配置目录，文件
PPP_CONF_DIR=/etc/ppp
L2TP_CONF_DIR=/etc/xl2tpd
L2TP_CONF=/etc/xl2tpd/xl2tpd.conf

# 变量
L2TP_ENT=$1	# lac 连接名
L2TP_USER=$2	# 客户端账号
L2TP_LNS=$3	# 远端服务器
AUTHTYPE=$4	# 验证方式

if [ ! -d $PPP_CONF_DIR ]; then 
    mkdir -p $PPP_CONF_DIR
fi

# /etc/xl2tpd/xl2tpd.conf
if [ ! -d $L2TP_CONF_DIR ]; then 
    mkdir -p $L2TP_CONF_DIR
fi

#增加监听ip地址配置
echo "[lac $L2TP_ENT]" >> $L2TP_CONF
echo "name = $L2TP_USER" >> $L2TP_CONF
echo "lns = $L2TP_LNS" >> $L2TP_CONF 
if [ "$AUTHTYPE" = "CHAP" ]; then
    echo "require chap = yes" >> $L2TP_CONF
    echo "refuse pap = yes" >> $L2TP_CONF
elif [ "$AUTHTYPE" = "PAP" ]; then
    echo "require pap = yes" >> $L2TP_CONF
    echo "refuse chap = yes" >> $L2TP_CONF
elif [ "$AUTHTYPE" = "EITHER" ]; then
    echo "require pap = yes" >> $L2TP_CONF
    echo "require chap = yes" >> $L2TP_CONF
elif [ "$AUTHTYPE" = "NONE" ]; then
    echo "refuse pap = yes" >> $L2TP_CONF
    echo "refuse chap = yes" >> $L2TP_CONF
fi
echo "require authentication = yes" >> $L2TP_CONF
echo "pppoptfile = /etc/options.l2tp_$L2TP_ENT" >> $L2TP_CONF
echo "length bit = yes" >> $L2TP_CONF
echo "autodial = yes" >> $L2TP_CONF
echo "" >> $L2TP_CONF

