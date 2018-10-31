#!/bin/sh
judge_wifi=`uttcli get sysConf  wifisleep`
if [ "$judge_wifi" == "0" ];then
    mmap w 0x10000600 0x0
    mmap w 0x10000064 0x501  
fi
