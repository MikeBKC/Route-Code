SWID =  $(shell date +%y%m%d)
ROMFS_DIR = $(ROOTDIR)/romfs
RAMDISK  = $(IMAGEDIR)/ramdisk
COMP = lzma

ROMFS_DIRS = \
	tmp \
	bin \
	dev dev/pts \
	home \
	etc \
	etc_ro etc_ro/web etc_ro/ppp etc_ro/ppp/peers etc_ro/ppp/plugins etc_ro/ppp/3g \
	etc_ro/wlan etc_ro/Wireless etc_ro/xml etc_ro/usb \
	etc_ro/linuxigd etc_ro/Wireless/RT2860AP etc_ro/Wireless/RT61AP etc_ro/Wireless/iNIC \
	lib lib/modules lib/ipsec \
	mnt \
	proc \
	sbin \
	sys \
	media \
	usr usr/bin usr/sbin usr/codepages \
	var 

clean:
	rm -f linux.bin.gz

romfs: 
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	#generate device node
	@if [ \! -d $(ROMFSDIR)/dev ];then mkdir -p $(ROMFSDIR)/dev;fi;
	$(ROMFSINST) /etc_ro/rcS
	$(ROMFSINST) /etc_ro/inittab
	$(ROMFSINST) /etc_ro/motd
	$(ROMFSINST) /etc/fstab

romfs.post:: 
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	#Squash File System
	rm -f $(RAMDISK)  
	#linux2.6.x support squashfs with gzip

image: 
	$(OBJCOPY) -O binary -R .note -R .comment -S $(ROOTDIR)/$(LINUXDIR)/vmlinux linux.bin
	rm -f linux.bin.gz
	gzip -9 linux.bin
#	uboot-mkimage -A ppc -O linux -T kernel -C gzip -a 0 -e 0 -n "Mentor Graphics System Builder/2.6.35/p1010rdb" -d linux.bin.gz uImage
#	uboot-mkimage -A ppc -O linux -T kernel -C gzip -a 0 -e 0 -n "Linux Kernel Image" -d linux.bin.gz uImage
	uboot-mkimage -A ppc -O linux -T kernel -C gzip -a 0 -e 0 -n "$(UTTARCH)" -d linux.bin.gz uImage
	mv uImage $(UTT_MACHINE)/bin/$(UTTARCH)$(UTTVERSION)-$(SWID).bin
