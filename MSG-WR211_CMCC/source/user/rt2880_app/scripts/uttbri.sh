
#!/bin/sh

if [ "$1" = "-r" ]; then
    uttcli set sysConf  sysConf brideg_mode_flag 0
elif [ "$1" = "-b" ]; then
    uttcli set  sysConf  sysConf brideg_mode_flag 1
else
    echo "unkonw argument"
fi

