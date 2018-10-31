#!/bin/sh

ROPSF=/etc_ro/ppp/pppoe-server-options
#ROPPPDF=/etc_ro/ppp/options
PSF=/etc/ppp/pppoe-server-options
PCHAPF=/etc/ppp/chap-secrets
PPAPF=/etc/ppp/pap-secrets
#PPPDF=/etc/ppp/options
killall -q -9 pppoe-server
# mantis 8923: 每次初始化pppoeserver时，都删除uttPppoeClientInfo文件
rm /var/run/uttPppoeClientInfo 2>/dev/null

cp $ROPSF $PSF
#cp $ROPPPDF $PPPDF
#auth=`nvram_get 2860 PoeSerAuth`
auth=`uttcli get pppoeSrvGlo authMode`
if [ "$auth" = "CHAP" ]; then
    echo require-chap >> $PSF
    echo -pap >> $PSF
#    echo require-chap >> $PPPDF
elif [ "$auth" = "PAP" ]; then
    echo require-pap >> $PSF
    echo -chap >> $PSF
#    echo require-pap >> $PPPDF
elif [ "$auth" = "EITHER" ]; then
    echo require-chap >> $PSF
    echo require-pap >> $PSF
fi
echo -mschap >> $PSF
echo -mschap-v2 >> $PSF

pdns=`uttcli get pppoeSrvGlo priDns`
sdns=`uttcli get pppoeSrvGlo secDns`
if [ "$pdns" != "0.0.0.0" ]; then
    echo ms-dns $pdns >> $PSF
fi
#mantis 8163
#if [ "$sdns" != "0.0.0.0" ]; then
echo ms-dns $sdns >> $PSF
#fi

#ips=`nvram_get 2860 PoeSerStart`
#ipc=`nvram_get 2860 PoeSerCliCnt`
#iponlinec=`nvram_get 2860 PoeSerOnlCnt`
