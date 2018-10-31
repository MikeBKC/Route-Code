#!/bin/sh

PPP_3G_FILE=/etc_ro/ppp/peers/3g

usage () {
  echo "usage: config-3g-ppp.sh [option]..."
  echo "options:"
  echo "  -h              : print this help"
  echo "  -p password     : set password"
  echo "  -u username     : set username"
  echo "  -b baud         : Set baudrate"
  echo "  -m dev 	  : set modem device"
  echo "  -c conn         : set connect AT script"
  echo "  -d disconn	  : set disconnect AT script"
  exit
}


for arg in $*
  do
    if [ "$1" != "" ] 
    then
      case "$1" in
        "-p")
          PASSWORD="password $2" 
    	  shift ;;
        "-u")
          USERNAME="user $2" 
    	  shift ;;
        "-b")
          BAUD="$2"
	  shift ;;
        "-m")
          MODEM="/dev/$2" 
	  shift ;;
        "-c")
          CONN="$2" 
	  shift ;;
        "-d")
          DISCONN="$2" 
	  shift ;;
        "-h")
	  usage ;;
        *) 
	  echo "illegal option -- $2" 
	  usage ;;
      esac
      shift
  fi
  done

index3g=`uttif gibn ttyUSB0`
#connname=`nvram_get 2860 wan_3g_connname`
#dialnum=`nvram_get 2860 wan_3g_dialnum`
connname=`uttcli get interface $index3g ethernet wan3g connname`
dialnum=`uttcli get interface $index3g ethernet wan3g dialnum`

cat /etc_ro/ppp/3g/Generic_conn.scr1 > /etc_ro/ppp/3g/$CONN
if [ "$connname" = "" ]; then
	echo if % = 0 goto next3 >>/etc_ro/ppp/3g/$CONN
else
	echo if % = 0 goto next2 >>/etc_ro/ppp/3g/$CONN
fi
cat /etc_ro/ppp/3g/Generic_conn.scr2 >> /etc_ro/ppp/3g/$CONN
echo send \"AT+CGDCONT=1,\\\"IP\\\",\\\"$connname\\\",,0,0^m\">>/etc_ro/ppp/3g/$CONN
cat /etc_ro/ppp/3g/Generic_conn.scr3 >> /etc_ro/ppp/3g/$CONN
echo send \"ATDT${dialnum}^m\">>/etc_ro/ppp/3g/$CONN
cat /etc_ro/ppp/3g/Generic_conn.scr4 >>/etc_ro/ppp/3g/$CONN

echo $MODEM > $PPP_3G_FILE
echo $BAUD >> $PPP_3G_FILE
echo $USERNAME >> $PPP_3G_FILE
echo $PASSWORD >> $PPP_3G_FILE
echo "modem" >> $PPP_3G_FILE
echo "crtscts" >> $PPP_3G_FILE
echo "noauth" >> $PPP_3G_FILE
echo "nodetach" >> $PPP_3G_FILE
#echo "defaultroute" >> $PPP_3G_FILE
echo "noipdefault" >> $PPP_3G_FILE
echo "nopcomp" >> $PPP_3G_FILE
echo "noaccomp" >> $PPP_3G_FILE
echo "novj" >> $PPP_3G_FILE
echo "nobsdcomp" >> $PPP_3G_FILE
echo "usepeerdns" >> $PPP_3G_FILE
echo "ipcp-accept-local" >> $PPP_3G_FILE
echo "ipcp-accept-remote" >> $PPP_3G_FILE
#if [ $PPPOE_OPMODE == "KeepAlive" ]; then
	echo "persist" >> $PPP_3G_FILE
#	echo "holdoff $PPPOE_OPTIME" >> $PPP_3G_FILE
#elif [ $PPPOE_OPMODE == "OnDemand" ]; then
#	PPPOE_OPTIME=`expr $PPPOE_OPTIME \* 60`
#	echo "demand" >> $PPP_3G_FILE
#	echo "idle $PPPOE_OPTIME" >> $PPP_3G_FILE
#fi
echo "nodeflate" >> $PPP_3G_FILE 
echo "connect \"/bin/comgt -d $MODEM -s /etc_ro/ppp/3g/$CONN\"" >> $PPP_3G_FILE 
echo "disconnect \"/bin/comgt -d $MODEM -s /etc_ro/ppp/3g/$DISCONN\"" >> $PPP_3G_FILE 
