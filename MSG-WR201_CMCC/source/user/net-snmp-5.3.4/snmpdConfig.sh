#!/bin/sh

READ_COMMUNITY=`uttcli get SNMP comunity`
SYSDEVICE=`uttcli get SNMP dev_name`
SYSCONTACT=`uttcli get SNMP contactor`
SYSLOCATION=`uttcli get SNMP position`
HOSTEN=`uttcli get SNMP host_manage_enable`
HOSTNAME1=`uttcli get SNMP host1`
HOSTNAME2=`uttcli get SNMP host2`
HOSTNAME3=`uttcli get SNMP host3`

CONFIGFILE='/etc/snmpd.conf'
#CONFIGFILE='snmpd.conf'

# /etc/snmpd.conf
#com2sec notConfigUser default public
#com2sec notConfigUser 192.168.101.132 public
#group notConfigGroup v1 notConfigUser
#group notConfigGroup v2c notConfigUser
#view all included .1 80
#access notConfigGroup "" any noauth exact all none none

#only host can read device info
if [ "$READ_COMMUNITY" == "" ]
then
    READ_COMMUNITY=public
fi

echo "# write snmpd.conf for snmpd " > $CONFIGFILE
if [ "$HOSTEN" == "1" ]
then

    if [ "$HOSTNAME1" != "" ]
    then
	if [ "$HOSTNAME1" != "0.0.0.0" ]
	then
	echo "com2sec notConfigUser $HOSTNAME1 $READ_COMMUNITY " >> $CONFIGFILE
	fi
    fi
    if [ "$HOSTNAME2" != "" ]
    then
	if [ "$HOSTNAME2" != "0.0.0.0" ]
	then
	echo "com2sec notConfigUser $HOSTNAME2 $READ_COMMUNITY " >> $CONFIGFILE
	fi
    fi
    if [ "$HOSTNAME3" != "" ]
    then
	if [ "$HOSTNAME3" != "0.0.0.0" ]
	then
	echo "com2sec notConfigUser $HOSTNAME3 $READ_COMMUNITY " >> $CONFIGFILE
	fi
    fi

else
echo "com2sec notConfigUser default $READ_COMMUNITY " > $CONFIGFILE
fi
echo "group notConfigGroup v2c notConfigUser " >> $CONFIGFILE
echo "view all included .1 80 " >> $CONFIGFILE
echo "access notConfigGroup \"\" any noauth exact all none none " >> $CONFIGFILE

if [ "$SYSLOCATION" != "" ]
then
echo "sysLocation $SYSLOCATION " >> $CONFIGFILE
fi
if [ "$SYSCONTACT" != "" ]
then
echo "sysContact $SYSCONTACT " >> $CONFIGFILE
fi
if [ "$SYSDEVICE" != "" ]
then
echo "sysName $SYSDEVICE " >> $CONFIGFILE
fi

