#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: config all romfs clean prune

#
# Include architecture specific build rules.
#
ifndef ROOTDIR
ROOTDIR=..
endif

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(BUSYBOX_CONFIG)
-include $(ARCH_CONFIG)
-include $(MODULES_CONFIG)

VEND=$(ROOTDIR)/vendors

#
# must run the vendor build first
#
dir_v = $(VEND)/$(CONFIG_VENDOR)/$(CONFIG_PRODUCT)/.
dir_p = $(ROOTDIR)/prop

dir_y = 

# must be builded earlier than other
# 比如uttUserConf必须被编译好，并生成配置文件后，才能编译goahead
dirPri_y =

dir_n =
dir_  =
dir_$(CONFIG_EBTABLES)                      += ebtables-v2.0.10-4
dir_y					    += rp-pppoe-3.8
dirPri_y				    += uttUserConf
dir_y				            += uttUserConf
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7620),y)
dir_$(CONFIG_USER_802_1X)		    += 802.1x_new
else
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7621),y)
dir_$(CONFIG_USER_802_1X)		    += 802.1x_new
else
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7628),y)
dir_$(CONFIG_USER_802_1X)		    += 802.1x_new
else
ifeq ($(CONFIG_DEFAULTS_RALINK_RT2880),y)
dir_$(CONFIG_USER_802_1X)                   += 802.1x_old
else
dir_$(CONFIG_USER_802_1X)		    += 802.1x
endif
endif
endif
endif
dir_$(CONFIG_USER_ALINK) 				+= alink
dir_$(CONFIG_USER_RT2561_802_1X)	    += rt61apd
dir_$(CONFIG_USER_BIGPOND) 		    += bigpond
dir_$(CONFIG_USER_BONNIE)		    += bonnie++
dir_$(CONFIG_USER_BRCTL_BRCTL)              += bridge-utils
dir_$(CONFIG_USER_BT_UTILS)                 += bt-utils
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox
dir_$(CONFIG_USER_CTORRENT)                 += ctorrent-dnh3.2
dir_$(CONFIG_USER_DHCP6)                    += dhcp6
dir_$(CONFIG_DHRYSTONE)		    	    += dhrystone
#dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq-2.40
dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq-2.76
dir_$(CONFIG_USER_DISKTYPE)		    += disktype-9
dir_$(CONFIG_USER_ETHTOOL)		    += ethtool
dir_$(CONFIG_USER_GDBSERVER_GDBREPLAY)      += gdbserver
dir_$(CONFIG_USER_GDBSERVER_GDBSERVER)      += gdbserver
dirPri_$(CONFIG_USER_MATRIXSSL)             += matrixssl-1.8.3
dir_$(CONFIG_USER_MATRIXSSL)                += matrixssl-1.8.3
dir_$(CONFIG_USER_GOAHEAD_HTTPD)            += goahead
dir_$(CONFIG_USER_IPLINK_DDNS)            += iplink
dir_$(CONFIG_USER_UTTCARE_DDNS)           += uttcare
dir_$(CONFIG_USER_PROFTPD)                  += proftpd-1.3.1.0
dir_$(CONFIG_USER_SAMBA)            		+= samba-3.0.37
dir_y					    += phddns-2.0.5.19225
dir_y            += uttTool
dir_$(CONFIG_USER_WIFIDOG) 		    += wifidog
ifeq ($(CONFIG_DEFAULTS_ATHEROS_AR7100),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.4
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.4
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.4
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.4
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT3052),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.0rc1
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.0rc1
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7620),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
dir_$(CONFIG_USER_EBTABLES)                 += ebtables-v2.0.10-4
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7621),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7628),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT5350),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.0rc1
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.0rc1
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT3883),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.0rc1
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.0rc1
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010),y)
dir_$(CONFIG_USER_IPTABLES_IPTABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)	    += iptables-1.4.10
dir_$(CONFIG_USER_IPTABLES_IPSET)           += ipset-4.5   
endif
dir_$(CONFIG_USER_IXIA_ENDPOINT)	    += ixia_endpoint
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_IGMP_PROXY)  		    += igmpproxy
dir_$(CONFIG_USER_IPROUTE2)  		    += iproute2-2.6.24-rc7
dir_$(CONFIG_USER_IPROUTEISATAP)            += iproute2-2.6.38
dir_$(CONFIG_USER_LMBENCH3)		    += lmbench3
dir_$(CONFIG_USER_LLTD)			    += lldt
dir_$(CONFIG_USER_LSUSB)		    += lsusb
dir_$(CONFIG_USER_MINIUPNPD)		    += miniupnpd-1.1
dir_$(CONFIG_USER_MTDUTILS)                 += mtd-utils
dir_$(CONFIG_USER_MTD_WRITE)         	    += mtd_write
dir_$(CONFIG_USER_MKDOSFS)                  += mkdosfs-2.11
dir_$(CONFIG_USER_NETCAT)		    += netcat
dir_$(CONFIG_USER_NETSTAT_NAT)		    += netstat-nat
dir_$(CONFIG_USER_NTPCLIENT_NTPCLIENT)      += ntpclient
dir_$(CONFIG_USER_NTPCLIENT_ADJTIMEX)       += ntpclient
dir_$(CONFIG_IP_NF_ARPTABLES)		    += arptables-v0.0.3-3
dir_$(CONFIG_USER_NTFS3G)		    += ntfs-3g
dir_$(CONFIG_USER_NTFSPROGS)                += ntfsprogs
dir_$(CONFIG_USER_OPENSWAN)                 += openswan-2.4.9
dir_$(CONFIG_USER_OPENSSL)	   	    += openssl-0.9.8e
dir_$(CONFIG_USER_NETCWMP) 			+= netcwmp-master
dir_$(CONFIG_USER_PORTMAP)                  += portmap
dir_$(CONFIG_USER_PPPOECD)                  += ppp-2.4.1/pppoecd
dir_y                     += ppp-2.4.4
#dir_$(CONFIG_USER_PPPD)                     += ppp-2.4.2
#dir_$(CONFIG_USER_PPPD_PPPSTATS)            += ppp-2.4.2
#dir_$(CONFIG_USER_PPPD_PPPDUMP)             += ppp-2.4.2
dir_$(CONFIG_USER_PPTP_CLIENT)              += pptp-client
dir_$(CONFIG_USER_PCIUTIL_LSPCI)            += pciutils-3.0.0
dir_$(CONFIG_USER_PCIUTIL_SETPCI)           += pciutils-3.0.0
dir_$(CONFIG_USER_P910ND)                   += p910nd-0.91
dir_$(CONFIG_USER_RADVD)                    += radvd-1.0
dir_$(CONFIG_RALINKAPP)		    	    += rt2880_app
dir_$(CONFIG_USER_RP_L2TP)		    += rp-l2tp-0.4
#dir_$(CONFIG_USER_RPPPPOE_RELAY)  	    += rp-pppoe-3.8
#dir_$(CONFIG_USER_RPPPPOE_SNIFF)  	    += rp-pppoe-3.8
dir_$(CONFIG_USER_STRACE_STRACE)            += strace
dir_$(CONFIG_USER_STUPID_FTPD)              += stupid-ftpd
#dir_$(CONFIG_USER_SAMBA)		    += samba-3.0.2
dir_$(CONFIG_USER_TOTD)                     += totd-1.5
dir_$(CONFIG_USER_TCPDUMP)                  += tcpdump
dir_$(CONFIG_USER_UVC_STREAM)		    += uvc_stream
dir_$(CONFIG_USER_UPNP_IGD)                 += linux-igd
dir_$(CONFIG_USER_WIRELESS_TOOLS)           += wireless_tools
dir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant-0.5.7
dir_$(CONFIG_USER_WSC)			    += wsc_upnp
dir_$(CONFIG_USER_ETHTOOL)		    += ethtool
dir_$(CONFIG_USER_ZEBRA)		    += zebra-0.95a_ripd
dir_$(CONFIG_USER_USBMODESWITCH_095)	    += usb_modeswitch-0.9.5
dir_$(CONFIG_USER_USBMODESWITCH_107)	    += usb_modeswitch-1.0.7
dir_$(CONFIG_USER_USBMODESWITCH_097)	    += usb_modeswitch-0.9.7
dir_$(CONFIG_USER_COMGT)		    += comgt-0.32
dir_$(CONFIG_USER_SDPARM) 		    += sdparm-1.02
dir_$(CONFIG_USER_3G)	 		    += hso-1.6
dir_$(CONFIG_USER_L2TP_SERVER)		    += xl2tpd-1.3.0
dir_$(CONFIG_USER_PPTP_SERVER)		    += pptpd-1.3.4
dir_$(CONFIG_DROPBEAR_SSH_SERVER)	    += dropbear-2012.55
ifeq ($(CONFIG_USER_LUA_ELINK),y)
dir_y       += lua-elink
dir_y	   	    += openssl-0.9.8x
dir_y	    += lua-5.3.4
dir_y	    += lua-cjson-2.1.0
dir_y	    += luasocket-master
dir_y += so
endif
#dir_$(CONFIG_USER_LUA)	    += lua-5.3.4
#dir_$(CONFIG_USER_LUA_CJSON)	    += lua-cjson-2.1.0
#dir_$(CONFIG_USER_LUA_SOCKET)	    += luasocket-master
dir_$(CONFIG_CISCO_KLISH)                   += klish-1.5.6
dir_y                       += netkeeper
dir_$(CONFIG_MKE2FS)                       += e2fsprogs-1.42.6
dir_$(CONFIG_USER_OPENSWAN_2_6_30)	    += openswan-2.6.30
dir_$(CONFIG_USER_PERL)			    += perl-5.10.0
dir_$(CONFIG_USER_SNMP)				+=net-snmp-5.3.4

ifeq ($(CONFIG_SWITCH_EXT),y)
dir_$(CONFIG_SWITCH_EXT) 	            += switch
else
dir_y					    += switch
endif

ifeq ($(CONFIG_USER_CACHE),y)
dir_y+=openssl-0.9.8x1
dir_y+=expat-2.0.1
dir_y+=libpcap-1.0.0
dir_y+=tcl8.5.8
dir_y+=pcre-8.11
dir_y+=zlib-1.2.3
dir_y+=trafficserver-3.3.1
dir_y+=cacheurl
endif
ifneq ($(CONFIG_USER_CAPWAP_AC)$(CONFIG_USER_CAPWAP_WTP),)
dir_y                  +=capwap-0.93.3
endif
dir_$(CONFIG_USER_LOGMONITOR) += log-monitor
#TEST
dir_$(CONFIG_USER_HELLO_WORLD)		    += hello_world
#dir_y = goahead
#--


all: config
	#$(MAKE) -j$(HOST_NCPU) $(sort $(dir_y) $(dir_v) $(dir_p)) || exit $$?
	#排序sort被去掉，不然matrixssl与goahead的编译顺序有问题
	$(MAKE) -j1 $(dirPri_y) || exit $$?
	$(MAKE) -j$(HOST_NCPU) $(dir_y) $(dir_v) $(dir_p) || exit $$?

#
# add directory dependancies here
#
.PHONY: $(sort $(dir_y) $(dir_p))

$(sort $(dir_y) $(dir_p)):  config
	[ ! -d "$@" ] || ( touch $@/.sgbuilt_user && $(MAKE) -j1 -C $@ ) || exit $$?

%_only:
	touch $(@:_only=)/.sgbuilt_user && $(MAKE) -j1 -C $(@:_only=)

%_clean:
	$(MAKE) -j1 -C $(@:_clean=) clean; rm -f $(@:_clean=)/.sgbuilt_user; true

romfs:
	for i in $(sort $(dir_y)) $(dir_p) ; do \
		[ ! -d $$i ] || $(MAKE) -C $$i romfs || exit $$? ; \
	done

clean:
	-for i in $(dir_v) $(sort $(dir_y) $(dir_n) $(dir_)) $(dir_p) ; do \
		if [ -f $$i/.sgbuilt_user ]; then \
			$(MAKE) -C $$i clean ; \
			rm -f $$i/.sgbuilt_user; \
		fi; \
	done

prune:
	-for i in $(sort $(dir_n) $(dir_)) ; do \
		found=0; \
		for j in $(sort $(dir_y)) ; do \
			if [ $$i == $$j ]; then \
				found=1; \
			fi; \
		done; \
		if [ $$found == 0 ]; then \
			[ "$$i" = "openswan" ] && make prune_openswan; \
			rm -fr $$i; \
		fi; \
	done

prune_openswan:
	@mkdir $(ROOTDIR)/$@
	@mv $(ROOTDIR)/openswan/linux/net/ipsec/{Config.in*,Kconfig} $(ROOTDIR)/$@/
	@rm -fr $(ROOTDIR)/openswan
	@mkdir -p $(ROOTDIR)/openswan/linux/net/ipsec
	@mv $(ROOTDIR)/$@/* $(ROOTDIR)/openswan/linux/net/ipsec/
	@rm -fr $(ROOTDIR)/$@
