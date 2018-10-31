
SWID =  $(shell date +%y%m%d)
COMP = lzma

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

#ifeq ($(CONFIG_SQUASHFS), y)
ROOT_IMG :=$(shell pwd)/target.squashfs
#else
#ROOT_IMG := target.cramfs
#endif
TARGETDIR := $(ROOTDIR)/romfs
#TARGETDIR := $(ROOTDIR)/target
romfs: 
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROMFSINST) /etc_ro/rcS
	$(ROMFSINST) /etc_ro/inittab
	$(ROMFSINST) /etc_ro/motd
	$(ROMFSINST) /etc/fstab

image: 
#	$(MAKE)  -C $(ROOTDIR)/bcm57xx/router/
#$(MAKE) install -C $(ROOTDIR)/bcm57xx/router/

	$(MAKE) CONFIG_SQUASHFS=$(CONFIG_SQUASHFS) -C $(ROOTDIR)/bcm57xx/router/compressed
	cp $(ROOTDIR)/bcm57xx/router/compressed/vmlinuz ./ 
	$(MAKE) -C $(ROOTDIR)/bcm57xx/router/squashfs mksquashfs
	$(ROOTDIR)/bcm57xx/router/squashfs/mksquashfs $(TARGETDIR) $(ROOT_IMG) -noappend -all-root
#cp $(ROOTDIR)/bcm57xx/router/compressed/vmlinuz ./ 
#	$(COMP) -9 -f -S .$(COMP) vmlinux
	trx -o linux.trx vmlinuz $(ROOT_IMG)

#	trx -o linux.trx vmlinuz 
	if grep -q "CONFIG_SQUASHFS=y" $(ROOTDIR)/$(LINUXDIR)/.config ; then \
		cp $(ROOTDIR)/bcm57xx/router/compressed/vmlinuz-lzma ./ ; \
		trx -o linux-lzma.trx vmlinuz-lzma $(ROOT_IMG) ; \
	fi
	mv linux.trx  $(UTT_MACHINE)/bin/$(UTTARCH)$(UTTVERSION)-$(SWID).bin 


