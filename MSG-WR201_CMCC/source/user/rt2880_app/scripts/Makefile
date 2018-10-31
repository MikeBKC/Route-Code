SCRIPTS = $(notdir $(wildcard *.sh))

all:

clean:

romfs:
	sed -e '/^#/d' ${ROOTDIR}/${CONFIG_LINUXDIR}/.config > .config
	chmod 755 .config
	$(ROMFSINST) .config /sbin/config.sh
	for i in $(SCRIPTS) ; do \
		$(ROMFSINST) $$i /sbin/$$i ; \
	done
	cp -f $(shell pwd)/ppp-on $(shell pwd)/ppp-off $(ROMFSDIR)/sbin/

ifeq ("$(CONFIG_USER_STORAGE)", "")	
	rm $(ROMFSDIR)/sbin/storage.sh
endif

ifeq ("$(CONFIG_USB)", "")	
	rm $(ROMFSDIR)/sbin/automount.sh
endif

ifeq ("$(CONFIG_USER_3G)", "")	
	rm $(ROMFSDIR)/sbin/3g.sh
	rm $(ROMFSDIR)/sbin/config-3g-ppp.sh
endif

ifeq ("$(CONFIG_USER_INADYN)", "")	
	rm $(ROMFSDIR)/sbin/ddns.sh
endif

ifeq ("$(CONFIG_USER_PPPD)", "")	
	rm $(ROMFSDIR)/sbin/config-pppoe.sh
endif

ifeq ("$(CONFIG_USER_RP_L2TP)", "")	
	rm $(ROMFSDIR)/sbin/config-l2tp.sh
endif

ifeq ("$(CONFIG_USER_PPTP_CLIENT)", "")	
	rm $(ROMFSDIR)/sbin/config-pptp.sh
endif


ifneq ("$(CONFIG_UTT_NETFILTER)", "")	
	rm $(ROMFSDIR)/sbin/GroupScript.sh
endif

ifeq ("$(CONFIG_USER_WIFIDOG)", "")
	rm $(ROMFSDIR)/sbin/usb_modeswitch.sh 
endif

ifeq ("$(CONFIG_WIRELESS)$(CONFIG_WIRELESS_EXT)$(CONFIG_NET_WIRELESS)", "")	
	rm $(ROMFSDIR)/sbin/wlan.sh
	rm $(ROMFSDIR)/sbin/wlan_5g.sh
	rm $(ROMFSDIR)/sbin/wifi_unload.sh
	rm $(ROMFSDIR)/sbin/fit_ap_wlan_5g.sh
	rm $(ROMFSDIR)/sbin/fit_ap_wlan.sh
endif
