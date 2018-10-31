#!/bin/sh

SAMBA_FILE=/etc/smb.conf


ANONYMOUS="$1"
BIND_INTERFACE="$2"
SHARE_DIR="$3"

echo "[global]
use sendfile = yes
netbios name = UTT_SAMBA
server string = Samba Server
workgroup = admin
security = user
guest account = guest 
log file = /var/log.samba
socket options = TCP_NODELAY SO_RCVBUF=48000 SO_SNDBUF=40000
encrypt passwords = yes
use spnego = yes
client use spnego = yes
disable spoolss = yes
smb passwd file = /etc/smbpasswd
host msdfs = no
strict allocate = No
os level = 20
log level = 0
max log size = 100
null passwords = yes
mangling method = hash
dos charset = CP936
unix charset = UTF8
display charset = UTF8" > $SAMBA_FILE

if [ "$BIND_INTERFACE" == "no" ]; then
echo "bind interfaces only = yes" >> $SAMBA_FILE
fi

echo "interfaces = lo br0" >> $SAMBA_FILE

if [ "$ANONYMOUS" == "novalid" ]; then
echo "map to guest = bad user" >> $SAMBA_FILE
fi

if [ "$SHARE_DIR" != "" ]; then
echo "$SHARE_DIR">> $SAMBA_FILE
fi
