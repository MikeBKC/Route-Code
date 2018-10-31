KERNELZ = $(IMAGEDIR)/zImage
SWID =  $(shell date +%y%m%d)
ROMFS_DIR = $(ROOTDIR)/romfs
RAMDISK  = $(IMAGEDIR)/ramdisk
MKIMAGE_DIR = $(ROOTDIR)/user/mkimage
CUR_DIR = $(shell pwd)
#COMP = gzip
ifeq ($(UTTFUNCVER),)
UTTFUNCVER = 0
endif

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
#ToMAXÈí¼þÃû³ÆÐÞ¸Ä
ifeq ($(CONFIG_UTT_TOMAX), y)
UTTBIN=$(UTTARCH)-$(UTTHWMODEL)-$(UTTVERSION)-$(SWID).bin
else
UTTBIN=$(UTTARCH)$(UTTVERSION)-$(SWID).bin
endif

clean:
	rm -f mkimage

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

mkimage: $(MKIMAGE_DIR)/crc32.c $(MKIMAGE_DIR)/mkimage.c 
	$(HOSTCC) $(HOST_CFLAGS) -DUSE_HOSTCC -o $@ -I$(MKIMAGE_DIR)/include $^ 

image: mkimage
	$(OBJCOPY) -O binary -R .note -R .comment -S $(ROOTDIR)/$(LINUXDIR)/vmlinux $(KERNELZ)
	cd $(IMAGEDIR); rm -f $(KERNELZ).*; gzip -9 $(KERNELZ)
#	uboot-mkimage -A ppc -O linux -T kernel -C gzip -a 0 -e 0 -n "Mentor Graphics System Builder/2.6.35/p1010rdb" -d linux.bin.gz uImage
#	uboot-mkimage -A ppc -O linux -T kernel -C gzip -a 0 -e 0 -n "Linux Kernel Image" -d linux.bin.gz uImage

#	cd $(IMAGEDIR) ; $(CUR_DIR)/mkimage -A ppc -O linux -T kernel -C gzip -a 88000000 -e $(shell readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}') -n $(UTTARCH)  -d zImage.gz -V $(UTTVERSION) -H $(UTTHWMODEL) -F $(UTTFUNCMODE) $(UTTARCH)$(UTTVERSION)-$(SWID).bin 
	cd $(IMAGEDIR) ; $(CUR_DIR)/mkimage -A ppc -O linux -T kernel -C gzip -a 0 -e 0 -n $(UTTARCH)  -d zImage.gz -V $(UTTVERSION) -H $(UTTHWMODEL) -F $(UTTFUNCMODE) -U $(UTTFUNCVER) $(UTTBIN) 

	@cp $(IMAGEDIR)/$(UTTBIN) $(UTT_MACHINE)/bin
	@rm $(IMAGEDIR)/$(UTTBIN) -f
