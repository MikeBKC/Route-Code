#! /bin/sh
if [ "$1" == "" ]; then
echo "parameter is none" 
exit 1
fi

#1 : if a disk have a invalid partition table ,then  must try mount this disk
#2 : if a disk have a valid partition table ,then just need to mount all partitions
#    and must wait it ,if not a "device busy" error may be coming.
if [ $DEVTYPE = "disk" ]; then
sleep 2
fi
mounted=`mount | grep $1 | wc -l`

# mounted, assume we umount
if [ $mounted -ge 1 ]; then
echo "R/dev/$1" 
echo "R/dev/$1" 
if ! umount "/dev/$1"; then
exit 1
fi

if ! rm -r "/media/$1"; then
exit 1
fi
# not mounted, lets mount under /media
else
if ! mkdir -p "/media/$1"; then
exit 1
fi

if ! mount "/dev/$1" "/media/$1"; then
# failed to mount, clean up mountpoint
if ! ntfs-3g "/dev/$1" "/media/$1" -o force; then
rm -r "/media/$1"
exit 1
fi
fi
mount -o remount,rw /media/$1
echo "A/media/$1" 
echo "A/media/$1" 
fi

snd2speed mdev $MDEV $ACTION
# Goahead need to know the event happened.
killall -SIGTTIN goahead
#killall -SIGTTIN speedweb
#kill -SIGTTOU $1
exit 0

