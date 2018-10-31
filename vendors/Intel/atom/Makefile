
MKIMAGE_DIR = $(ROOTDIR)/user/mkimage
CUR_DIR = $(shell pwd)
SWID =  $(shell date +%y%m%d)

ROMFS_DIRS = \
	tmp \
	bin \
	dev dev/pts \
	home \
	etc \
	etc_ro etc_ro/web etc_ro/ppp etc_ro/ppp/peers etc_ro/ppp/plugins \
	etc_ro/xml etc_ro/usb \
	etc_ro/linuxigd \
	lib lib/modules \
	mnt \
	proc \
	sbin \
	sys \
	media \
	usr usr/bin usr/sbin usr/codepages \
	var etc_ro/init.d


romfs: 
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROMFSINST) /etc_ro/rc.local
	$(ROMFSINST) /etc_ro/inittab
	$(ROMFSINST) -d /etc_ro/init.d

makesoft: $(MKIMAGE_DIR)/crc32.c $(CUR_DIR)/creatSoft.c 
	$(HOSTCC) $(HOST_CFLAGS) -DUSE_HOSTCC -o $@  $^ 

rm_makesoft:
	rm $(CUR_DIR)/makesoft -f

image: rm_makesoft makesoft
	cp $(ROOTDIR)/$(LINUXDIR)/arch/x86/boot/bzImage $(CUR_DIR)/root.grub/boot/vmlinux -f
	./gen_image_x86.sh $(CUR_DIR)/ramfs.img 8 $(CUR_DIR)/root.grub 20 root.ext2
	$(CUR_DIR)/makesoft $(UTTARCH) $(IMAGEDIR)/$(UTTARCH)$(UTTVERSION)-$(SWID).bin $(shell readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}') 0x80060000 linux ramfs.img $(UTTFUNCVER) $(UTTVERSION) $(UTTHWMODEL)
	@cp $(IMAGEDIR)/$(UTTARCH)$(UTTVERSION)-$(SWID).bin $(UTT_MACHINE)/bin
	@rm $(IMAGEDIR)/$(UTTARCH)$(UTTVERSION)-$(SWID).bin -f
