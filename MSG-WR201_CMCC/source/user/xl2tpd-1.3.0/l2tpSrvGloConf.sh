#!/bin/sh
##############################
# l2tp server ȫ�������޸Ľű�
##############################

# ����Ŀ¼���ļ�
PPP_CONF_DIR=/etc/ppp
PPP_CONF_L2TP=/etc/ppp/options.xl2tpd
L2TP_CONF_DIR=/etc/xl2tpd
L2TP_CONF=/etc/xl2tpd/xl2tpd.conf

# ����
REMOTEIP=$1	# ��ַ��
LOCALIP=$2	# ��������ip
AUTHTYPE=$3	# ��֤��ʽ
LISTENIP=$4	# ����ip
PREDNS=$5
SECDNS=$6

if [ ! -d $PPP_CONF_DIR ]; then 
    mkdir -p $PPP_CONF_DIR
fi

# /etc/options.xl2tpd����
echo "ipcp-accept-local" > $PPP_CONF_L2TP
echo "ipcp-accept-remote" >> $PPP_CONF_L2TP
echo "ms-dns $PREDNS" >> $PPP_CONF_L2TP
echo "ms-dns $SECDNS" >> $PPP_CONF_L2TP
echo "noccp" >> $PPP_CONF_L2TP
echo "auth" >> $PPP_CONF_L2TP
echo "crtscts" >> $PPP_CONF_L2TP
echo "idle 1800" >> $PPP_CONF_L2TP
echo "mtu 1410" >> $PPP_CONF_L2TP
echo "mru 1410" >> $PPP_CONF_L2TP
echo "nodefaultroute" >> $PPP_CONF_L2TP
echo "lock" >> $PPP_CONF_L2TP
echo "proxyarp" >> $PPP_CONF_L2TP
echo "connect-delay 5000" >> $PPP_CONF_L2TP

# /etc/xl2tpd/xl2tpd.conf
if [ ! -d $L2TP_CONF_DIR ]; then 
    mkdir -p $L2TP_CONF_DIR
fi

#���Ӽ���ip��ַ����
echo "[global]" > $L2TP_CONF
echo "listen-addr = $LISTENIP" >> $L2TP_CONF
echo "[lns default]" >> $L2TP_CONF
echo "ip range = $REMOTEIP" >> $L2TP_CONF
echo "local ip = $LOCALIP" >> $L2TP_CONF 
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
echo "name = UttVPNserver" >> $L2TP_CONF
echo "pppoptfile = /etc/ppp/options.xl2tpd" >> $L2TP_CONF
echo "length bit = yes" >> $L2TP_CONF

