UTTVERSIONS = v1.8.13 v1.8.13 v1.8.13 v1.8.13 v1.8.13
UTTHWMODELS = V1.0 V2.0 V1.0 V1.0 W.00
UTTFUNCMODE = 0xD800
UTTARCHS = MSG-WR301 nvA651Wv2 WA700Nset 651W+ MSG-WR201
UTTMODELS = MSG-WR301 A651Wv2 WA700Nset 651W+ MSG-WR201
#UTTMODEL_PREV = ½øÈ¡
UTT_MACHINE = $(shell pwd)
UTT_NEEDCHANGEFILE = $(UTT_MACHINE)/needChangeFile
ifeq ($(mode),dev)
UTT_ROOTDIR = $(UTT_MACHINE)/../
else
UTT_ROOTDIR = $(UTT_MACHINE)/source
endif
    
UTT_CONFDIR = $(UTT_ROOTDIR)/config
include $(UTT_NEEDCHANGEFILE)/root-config
export UTTVERSION UTTARCH UTTMODEL UTT_MACHINE UTTMODEL_PREV UTTHWMODEL UTTFUNCMODE
source_dir = config lib $(CONFIG_LINUXDIR) Makefile uttLinuxconf uClibc++ vendors user tools uttShareHead
build:
	$(MAKE) -C $(UTT_ROOTDIR) dep
	mkdir $(UTT_ROOTDIR)/include/linux -p
	mkdir $(UTT_ROOTDIR)/include/config -p
	cp $(UTT_ROOTDIR)/$(CONFIG_LINUXDIR)/include/linux/autoconf.h $(UTT_ROOTDIR)/include/linux -f
	cp $(UTT_CONFDIR)/autoconf.h $(UTT_ROOTDIR)/include/config -f
	$(MAKE) -C $(UTT_ROOTDIR) $(STAGE)
rel_prev:
	rm ./source -fr
	mkdir source
#	for i in $(source_dir) ; do cp -fr ../$$i source ; done
	cd ../; rm -f utttmp.tar;tar cvf utttmp.tar $(source_dir); mv utttmp.tar $(UTT_MACHINE)/source 
	cd source;tar xvf utttmp.tar; rm -f utttmp.tar
	cp $(UTT_NEEDCHANGEFILE)/config.arch $(UTT_ROOTDIR) -f
	cp $(UTT_NEEDCHANGEFILE)/config-autoconf.h $(UTT_CONFDIR)/autoconf.h -f
	cp $(UTT_NEEDCHANGEFILE)/config-config $(UTT_CONFDIR)/.config -f
	cp $(UTT_NEEDCHANGEFILE)/root-config $(UTT_ROOTDIR)/.config -f
	cp $(UTT_NEEDCHANGEFILE)/busybox-config $(UTT_ROOTDIR)/user/busybox/.config -f
	cp $(UTT_NEEDCHANGEFILE)/linux-config $(UTT_ROOTDIR)/$(CONFIG_LINUXDIR)/.config -f
	cp $(UTT_NEEDCHANGEFILE)/internet.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts -f
	cp $(UTT_NEEDCHANGEFILE)/config-vlan.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts -f
	cp $(UTT_NEEDCHANGEFILE)/web/config.js $(UTT_ROOTDIR)/user/goahead/web/js/config.js -f
	cp $(UTT_NEEDCHANGEFILE)/config-autoconf.h $(UTT_ROOTDIR)/uttShareHead/config/autoconf.h -f
	cp $(UTT_NEEDCHANGEFILE)/uttMachine.h $(UTT_ROOTDIR)/uttShareHead/config -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/disconnectlan.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/disconnectlan.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/homebg_13.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/homebg_13.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/lightning.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/lightning.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/stream.gif $(UTT_ROOTDIR)/user/goahead/web/images_Sui/stream.gif -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/wanimg.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/wanimg.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/round_buttons.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/round_buttons.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/css/style_port.css $(UTT_ROOTDIR)/user/goahead/web/css/style_port.css -f
	cp $(UTT_NEEDCHANGEFILE)/web/css/style_new.css $(UTT_ROOTDIR)/user/goahead/web/css/style_new.css -f
	make -C $(UTT_ROOTDIR) clean
	$(MAKE) -C $(UTT_ROOTDIR)/uttLinuxconf

dev_prev:    
	cp $(UTT_NEEDCHANGEFILE)/config.arch $(UTT_ROOTDIR) -f
	cp $(UTT_NEEDCHANGEFILE)/config-autoconf.h $(UTT_CONFDIR)/autoconf.h -f
	cp $(UTT_NEEDCHANGEFILE)/config-config $(UTT_CONFDIR)/.config -f
	cp $(UTT_NEEDCHANGEFILE)/root-config $(UTT_ROOTDIR)/.config -f
	cp $(UTT_NEEDCHANGEFILE)/busybox-config $(UTT_ROOTDIR)/user/busybox/.config -f
	cp $(UTT_NEEDCHANGEFILE)/linux-config $(UTT_ROOTDIR)/$(CONFIG_LINUXDIR)/.config -f
	cp $(UTT_NEEDCHANGEFILE)/internet.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts -f
	cp $(UTT_NEEDCHANGEFILE)/config-vlan.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts -f
	cp $(UTT_NEEDCHANGEFILE)/web/config.js $(UTT_ROOTDIR)/user/goahead/web/js/config.js -f
	cp $(UTT_NEEDCHANGEFILE)/config-autoconf.h $(UTT_ROOTDIR)/uttShareHead/config/autoconf.h -f
	cp $(UTT_NEEDCHANGEFILE)/uttMachine.h $(UTT_ROOTDIR)/uttShareHead/config -f
	$(MAKE) -C $(UTT_ROOTDIR)/uttLinuxconf
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/disconnectlan.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/disconnectlan.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/homebg_13.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/homebg_13.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/lightning.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/lightning.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/stream.gif $(UTT_ROOTDIR)/user/goahead/web/images_Sui/stream.gif -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/wanimg.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/wanimg.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/images_Sui/round_buttons.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/round_buttons.png -f
	cp $(UTT_NEEDCHANGEFILE)/web/css/style_port.css $(UTT_ROOTDIR)/user/goahead/web/css/style_port.css -f
	cp $(UTT_NEEDCHANGEFILE)/web/css/style_new.css $(UTT_ROOTDIR)/user/goahead/web/css/style_new.css -f
	cp $(UTT_NEEDCHANGEFILE)/web/home.asp $(UTT_ROOTDIR)/user/goahead/web/ -f
	cp $(UTT_NEEDCHANGEFILE)/script/*.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts/ -f
	cp $(UTT_NEEDCHANGEFILE)/rcS.utt $(UTT_ROOTDIR)/vendors/Ralink/MT7628/
dev_prev_debug:
	@UTTVERSION_ARR=($(UTTVERSIONS)); \
		UTTHWMODEL_ARR=($(UTTHWMODELS)); \
		UTTARCH_ARR=($(UTTARCHS)); \
		UTTMODEL_ARR=($(UTTMODELS)); \
		len="$${#UTTVERSION_ARR[*]}"; \
		i=0; \
		while(( $$i < $$len )); \
		do \
		UTTVERSION=$${UTTVERSION_ARR[i]} UTTHWMODEL=$${UTTHWMODEL_ARR[i]} UTTARCH=$${UTTARCH_ARR[i]} UTTMODEL=$${UTTMODEL_ARR[i]}; \
		if [ "$(model)" = "$${UTTARCH}" ]; \
		then \
		$(MAKE) dev_prev;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/uttMachine.h $(UTT_ROOTDIR)/uttShareHead/config/ -f; \
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/linux-config $(UTT_ROOTDIR)/$(CONFIG_LINUXDIR)/.config ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/config-vlan.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts/ ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/stream.gif $(UTT_ROOTDIR)/user/goahead/web/images_Sui ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/home.asp $(UTT_ROOTDIR)/user/goahead/web/ ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/homebg_13.png $(UTT_ROOTDIR)/user/goahead/web/images_Sui/ ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/style_new.css $(UTT_ROOTDIR)/user/goahead/web/css/ ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/style_port.css $(UTT_ROOTDIR)/user/goahead/web/css/ ;\
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/set.lua $(UTT_ROOTDIR)/user/lua-elink/ap/uttdata/ -f ; \
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/get.lua $(UTT_ROOTDIR)/user/lua-elink/ap/uttdata/ -f ; \
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/1elink.lua $(UTT_ROOTDIR)/user/lua-elink/ap/ -f ; \
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/menu.js $(UTT_ROOTDIR)/user/goahead/web/script/ -f ; \
		cp $(UTT_NEEDCHANGEFILE)/$${UTTARCH}/*.sh $(UTT_ROOTDIR)/user/rt2880_app/scripts/ -f ; \
		fi; \
		i=$$((i+1)); \
		done;
build_debug:
	@UTTVERSION_ARR=($(UTTVERSIONS)); \
		UTTHWMODEL_ARR=($(UTTHWMODELS)); \
		UTTARCH_ARR=($(UTTARCHS)); \
		UTTMODEL_ARR=($(UTTMODELS)); \
		len="$${#UTTVERSION_ARR[*]}"; \
		i=0; \
		while(( $$i < $$len )); \
		do \
		UTTVERSION=$${UTTVERSION_ARR[i]} UTTHWMODEL=$${UTTHWMODEL_ARR[i]} UTTARCH=$${UTTARCH_ARR[i]} UTTMODEL=$${UTTMODEL_ARR[i]}; \
		if [ "$(model)" = "$${UTTARCH}" ]; \
		then \
		$(MAKE) -C $(UTT_ROOTDIR) dep; \
		mkdir $(UTT_ROOTDIR)/include/linux -p; \
		mkdir $(UTT_ROOTDIR)/include/config -p; \
		cp $(UTT_ROOTDIR)/$(CONFIG_LINUXDIR)/include/linux/autoconf.h $(UTT_ROOTDIR)/include/linux -f; \
		cp $(UTT_CONFDIR)/autoconf.h $(UTT_ROOTDIR)/include/config -f; \
		$(MAKE) -C $(UTT_ROOTDIR) $(STAGE); \
		fi; \
		i=$$((i+1)); \
		done;
clean:
	make -C $(UTT_ROOTDIR) clean

up:
	for i in $(source_dir) ; do cvs up -d $(UTT_ROOTDIR)/$$i ; done
