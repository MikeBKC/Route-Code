#!/bin/sh

index3g=`uttif gibn ttyUSB0`

killall utt3gswitch
killall usb_modeswitch
killall 3g_switch.sh
ppp-off $index3g
active=`uttcli get interface $index3g active`
if [ "$active" = "No" ]; then
    exit 0
fi
sleep 3
3g_switch.sh
#�ȴ�����Ŵ�ӡ��
sleep 6
killall utt3gswitch
utt3gswitch &
