#!/bin/sh
##############################
# pptp server 全局配置修改脚本
##############################

# 配置目录，文件
PPP_CONF_DIR=/etc/ppp
PPP_CONF_PPTP=/etc/ppp/options.pptpd
PPTP_CONF_DIR=/etc/
PPTP_CONF=/etc/pptpd.conf

# 变量
MTU=$1
REMOTEIP=$2	# 地址池
LOCALIP=$3	# 本地虚拟ip
PREDNS=$4
SECDNS=$5
AUTHTYPE=$6	# 验证方式
LISTENIP=$7	# 监听ip
ENCRYPTMODE=$8  # 加密方式

if [ ! -d $PPP_CONF_DIR ]; then 
    mkdir -p $PPP_CONF_DIR
fi

# /etc/options.pptpd配置
echo "name pptpd" > $PPP_CONF_PPTP
echo "mtu $MTU" >> $PPP_CONF_PPTP
if [ "$AUTHTYPE" = "MS-CHAPV2" ]; then
    echo "require-mschap-v2" >> $PPP_CONF_PPTP
    if [ "$ENCRYPTMODE" == "MPPE" ];then
	echo "require-mppe-128" >> $PPP_CONF_PPTP
    fi
elif [ "$AUTHTYPE" = "CHAP" ]; then
    echo "require-chap" >> $PPP_CONF_PPTP
    echo "refuse-pap" >> $PPP_CONF_PPTP
elif [ "$AUTHTYPE" = "PAP" ]; then
    echo "require-pap" >> $PPP_CONF_PPTP
    echo "refuse-chap" >> $PPP_CONF_PPTP
elif [ "$AUTHTYPE" = "THRIN" ]; then
    echo "require-pap" >> $PPP_CONF_PPTP
    echo "require-chap" >> $PPP_CONF_PPTP
    echo "require-mschap-v2" >> $PPP_CONF_PPTP
elif [ "$AUTHTYPE" = "NONE" ]; then
    echo "refuse-pap" >> $PPP_CONF_PPTP
    echo "refuse-chap" >> $PPP_CONF_PPTP
fi
echo "ms-dns $PREDNS" >> $PPP_CONF_PPTP
echo "ms-dns $SECDNS" >> $PPP_CONF_PPTP
echo "proxyarp" >> $PPP_CONF_PPTP
echo "debug" >> $PPP_CONF_PPTP
echo "lock" >> $PPP_CONF_PPTP
echo "nobsdcomp" >> $PPP_CONF_PPTP
echo "novj" >> $PPP_CONF_PPTP
echo "novjccomp" >> $PPP_CONF_PPTP
echo "nologfd" >> $PPP_CONF_PPTP
echo "lcp-echo-interval 30" >> $PPP_CONF_PPTP
echo "lcp-echo-failure 6" >> $PPP_CONF_PPTP

# /etc/pptpd.conf
if [ ! -d $PPTP_CONF_DIR ]; then 
    mkdir -p $PPTP_CONF_DIR
fi

echo "ppp /bin/pppd" > $PPTP_CONF
echo "listen $LISTENIP" >> $PPTP_CONF
echo "option /etc/ppp/options.pptpd" >> $PPTP_CONF
echo "debug" >> $PPTP_CONF
echo "stimeout 10" >> $PPTP_CONF
echo "logwtmp" >> $PPTP_CONF
echo "localip $LOCALIP" >> $PPTP_CONF 
echo "remoteip $REMOTEIP" >> $PPTP_CONF 

