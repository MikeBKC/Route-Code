action=$1
task=$2
notPerform=$3

mkdir /var/spool 2>/dev/null
mkdir /var/spool/cron/ 2>/dev/null
mkdir /var/spool/cron/crontabs 2>/dev/null
crondFile=/var/spool/cron/crontabs/admin

task=`echo "$task" | sed "s/;;/\n/g"`
#trim and merge spaces
task=`echo "$task" | sed "s/ \{2,\}/ /g;s/^ *//g;s/ *$//g"`

if [ "$action" == "add" ]; then
    echo "$task" >> $crondFile
elif [ "$action" == "del" ]; then
    if [ -f $crondFile ]; then
	task=`grep -v -F "$task" $crondFile`
	echo "$task" >$crondFile
    else
        exit 0
    fi
else
    exit 0
fi
 
if [ "$notPerform" != "1" ]; then
    killall crond 
    crond &
fi
