#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.106.14.2 2015/08/06 14:06:41 xue.ruigang Exp $
#

NAME	= goahead

# User Management switch
UMSW	= -DUSER_MANAGEMENT_SUPPORT

# Digest Access switch
ifeq ("$(LOGIN_PAGE_DIY)","Sui")
DASW	= 
else
DASW	= -DDIGEST_ACCESS_SUPPORT
endif

CFLAGS += -I./ -I../include
CFLAGS += -I$(ROOTDIR)/lib/zlib
CFLAGS += -I$(ROOTDIR)/include
CFLAGS += -I$(ROOTDIR)/config
# SSL switches
ifeq ("$(CONFIG_USER_GOAHEAD_SSL)", "y")
SSLPATCHFILE = matrix_ssl.o sslSocket.o
MATRIXDIR = $(ROOTDIR)/user/matrixssl-1.8.3
SSLINC = $(MATRIXDIR)
SSLLIB = $(MATRIXDIR)/src/libmatrixsslstatic.a
SSLSW = -DWEBS_SSL_SUPPORT -DMATRIX_SSL -I$(SSLINC)
endif

# If-Modified-Support switches (requires math library, libm.a)
# IFMODSW = -DWEBS_IF_MODIFIED_SUPPORT
# IFMODLIB = /usr/lib/libm.a

# Dependencies
DEPEND_FILES	= asp.o balloc.o base64.o cgi.o default.o  \
				  ejlex.o ejparse.o form.o urcp_msg.o\
				  h.o handler.o mime.o misc.o page.o web_api.o\
				  ringq.o rom.o \
				  sock.o sockGen.o $(SSLPATCHFILE) \
				  security.o sym.o uemf.o url.o value.o \
				  md5c.o um.o umui.o websda.o emfdb.o \
				  webrom.o webs.o websuemf.o \
				  internet.o des.o utils.o \
				  wusDhcpServer.o wusDnsFilter.o firewall.o uttfunction.o wusArpbind.o \
				  wusAdmin$(PAGE_NAME_TYPE).o wusstaticnat.o wusnatrule.o wusupnp.o wusDMZ.o management.o $(NAME).o wusSoftwareUpload.o crc32.o \
				  wusMultiWan.o wusPptpClient.o wusPppoeServer.o wusTaskScheduler.o \
				  wusPortVlan.o wusSwPortConf.o  wusAssist.o wusMirror.o wusL2tpServer.o wusL2tpClient.o wusAdvideo.o \
				  wusAggregation.o wusPptpServer.o wusAntiNetSniffer.o wusSmartQos.o wusgroupbase.o \
				  wusPortStatics.o wusUrcpDiscovery.o webmsg.o wusTopology.o wusRemoteManage.o wuspdb.o \
				  wusConfigUrcp.o wusSysConfUrcp.o wusAggrUrcp.o wusPortConfigUrcp.o wusRatelimitUrcp.o \
				  wusVlanUrcp.o wusMirrorUrcp.o wusStaticRoute.o wusSettingsConf.o wusInternet.o \
				  wusLanConfig.o wusDdns.o wusfirewall.o wuScript.o wusFwBase.o wusTopPage.o wusUserStats.o \
				  wusWirelessMacFilter.o wusWirelessHostList.o wusWlanBase.o wusWlanSecurity.o wusWlanAdvanced.o \
				  wusFatFitSwitch.o wusApPermission.o wusApConfTemp.o wusServiceZone.o wusDhcpPool.o wusDnsAgency.o \
				  wusApManage.o  wusApSoftwareManage.o wusApBasicConfUrcp.o \
				  wusApBasicInfo.o wusApConfManage.o wusApEffectConf.o wusPushSoftware.o wusApWlanCltList.o \
				  wusPushConfTemp.o wusApJoinSZ.o wusFitApLocalConfTemp.o wusNotice.o wusPortMirror.o \
				  wusWebAuth.o wusExceptQQ.o wusExceptMSN.o wusExceptAli.o wusIpGroup.o wusFwService.o wusTimeRange.o wusPortMirror6530G.o \
				  wusLogManage.o wusConnLimit.o translate.o wusRddPortVlan.o wusIPSec.o wusPortSpeed.o wusSyslog.o \
				  wusDeviceLoginControl.o wusHotel.o wusLicense.o wusIdentifyBind.o wusHardwareNAT.o wusPolicyRoute.o \
				  wusP2PLimit.o wusAcWorkMode.o wusWevoMfg.o \
				  wusSysLogInfo.o wusParentsControl.o wusEasyTask.o dyn_Config.o wusMacFilter.o \
			      wusApLog.o wusApLoadBalance.o wusSnmp.o wusSNMP.o wusTagVlan.o wusNetSniper.o wusDiagnose.o wusNetworkStatus.o  wusAppManage.o \
				  wusNetShareManage.o wusFtpServer.o wusNetShareUser.o  sha1.o linklist.o wusWebAuthCaptcha.o wusAdfilter.o wusAppType.o wusApMacFilter.o wusVlanConfig.o wusApInfo.o wusApPositionDraw.o \
				 wusIpv6Internet.o wusRadvd.o wusIpv6RouteConfig.o wusDhcp6.o wusIpv6PrefixList.o wusTunnel.o wusIsatap.o wusCwmp.o wusCliForEngineerOnly.o wusMakeMoney.o

ifneq ("$(CONFIG_USER_GOAHEAD_LANG_EN)", "")
DEPEND_FILES += transData_en.o 
endif

ifneq ("$(CONFIG_USER_GOAHEAD_LANG_ZHTW)", "")
DEPEND_FILES += transData_zhtw.o
endif

ifneq ("$(CONFIG_USER_GOAHEAD_LANG_ZHCN)", "")
DEPEND_FILES += transData_zhcn.o
endif
ifneq ("$(CONFIG_RT2860V2_WIFI)", "")
DEPEND_FILES += wireless.o 
DEPEND_FILES += station.o
DEPEND_FILES += wps.o
endif
ifneq ("$(CONFIG_INIC_MII)", "") 
DEPEND_FILES += inic.o
CFLAGS += -DINICv2_SUPPORT
else
ifneq ("$(CONFIG_INIC_PCI)", "")
DEPEND_FILES += inic.o
CFLAGS += -DINICv2_SUPPORT
else
ifneq ("$(CONFIG_INIC_USB)", "")
DEPEND_FILES += inic.o
CFLAGS += -DINICv2_SUPPORT
endif
endif
endif
ifneq ("$(CONFIG_RT2561_AP)", "")
DEPEND_FILES += legacy.o
endif
ifeq ("$(CONFIG_USER_GOAHEAD_IPV6)", "y")
CFLAGS += -DWF_USE_IPV6
endif
ifeq ("$(CONFIG_USER_GOAHEAD_HOSTNAME)", "y")
CFLAGS += -DGA_HOSTNAME_SUPPORT
endif
ifneq ("$(CONFIG_USER_USB_DISK)", "")
##DEPEND_FILES += usb.o
##usb.c ÓÃsw_usb.cÌæ´ú
DEPEND_FILES += sw_usb.o
CFLAGS  += -DSYS_USB
endif
ifneq ("$(CONFIG_LIB_LIBUSB_USER_FORCE)", "")
CFLAGS  += -I$(ROOTDIR)/lib/libusb-user
LDLIBS	+= -lusb-user
endif
ifeq ("$(CONFIG_RALINKAPP_MPLAYER)", "y")
DEPEND_FILES += media.o
endif
ifeq ("$(CONFIG_RALINKAPP_SWQOS)", "y")
DEPEND_FILES += qos.o
endif

ifeq ($(CONFIG_USER_CACHE),y)
DEPEND_FILES += wusCache.o
endif

ifeq ($(CONFIG_USER_NET_MODE_SWITCH),y)
DEPEND_FILES += wusNetModeSwitch.o
endif

ifeq ($(CONFIG_USER_BRIDGE),y)
#DEPEND_FILES += wusBridgeConfig.o
DEPEND_FILES += wusCacheModeConfig.o
endif
ifeq ($(CONFIG_USER_CACHE_SERVER),y)
DEPEND_FILES += wusCacheServer.o
endif

CFLAGS	+= -DWEBS -DUEMF -DOS="LINUX" -DLINUX $(UMSW) $(DASW) $(SSLSW) $(IFMODSW) -DUTT_SWVERSION=\"$(UTTVERSION)\" -DUTT_HWMODEL=\"$(UTTHWMODEL)\" -DUTT_FUNCVER=\"$(UTTFUNCVER)\"
CFLAGS  += -Wall -fno-strict-aliasing
CFLAGS	+= -I$(ROOTDIR)/$(LINUXDIR)/drivers/char

ifeq ("$(CONFIG_DEFAULTS_RALINK_RT3052)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif
ifeq ("$(CONFIG_DEFAULTS_RALINK_RT3883)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif

ifeq ("$(CONFIG_DEFAULTS_RALINK_RT2880)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif

ifeq ("$(CONFIG_DEFAULTS_RALINK_RT5350)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif
ifeq ("$(CONFIG_DEFAULTS_RALINK_MT7620)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif
ifeq ("$(CONFIG_DEFAULTS_RALINK_MT7621)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif
ifeq ("$(CONFIG_DEFAULTS_RALINK_MT7628)", "y")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS  += -I$(ROOTDIR)/user/ppp-2.4.4/pppd
CFLAGS  += -I$(ROOTDIR)/$(LINUXDIR)/drivers/flash 

ifeq ($(CONFIG_LIB_LIBSWITCH_FORCE),y)
CFLAGS  += -I$(ROOTDIR)/lib/libswitch
endif
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli
OTHERS	= -DB_STATS -DB_FILL -DDEBUG
#CFLAGS  += -DDEBUG
LDFLAGS	+= $(SSLLIB) $(IFMODLIB)
LDLIBS	+= -lconfig -lpthread 

ifeq ($(CONFIG_LIB_LIBSWITCH_FORCE),y)
LDLIBS	+= -lswitch
endif

ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
LDLIBS	+= -liconv
endif

ifeq ($(CONFIG_LIB_LIBCJSON_FORCE),y)
CFLAGS += -I$(ROOTDIR)/lib/libcJSON
LDLIBS  += -lcJSON
endif
CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
UCONF_H	= $(ROOTDIR)/config/autoconf.h

all:	$(NAME)

#
#	Primary link
#
$(NAME): clean_inet $(DEPEND_FILES)
	$(CC) -o $@ $(DEPEND_FILES) $(LDFLAGS) $(EXTRALIBS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(NAME)
ifeq ("$(CONFIG_WEVO_MFG)", "y")
	$(ROMFSINST) /bin/wevo_mfg
endif
ifeq ("$(CONFIG_USER_GOAHEAD_SSL)", "y")
	$(ROMFSINST) /etc_ro/serverkey.pem
	$(ROMFSINST) /etc_ro/servercert.pem
endif

clean:
	rm -f $(NAME) *.o

clean_inet:
	rm -f internet.o

#
#	Dependencies
#
asp.o:  webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

balloc.o: balloc.c uemf.h

base64.o:  base64.c webs.h wsIntrn.h  ej.h ejIntrn.h uemf.h

cgi.o:  webs.h wsIntrn.h uemf.h

default.o:  default.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

ejlex.o:  ejlex.c ej.h ejIntrn.h uemf.h

ejparse.o:  ejparse.c ej.h ejIntrn.h uemf.h

emfdb.o:  emfdb.h wsIntrn.h uemf.h

firewall.o: firewall.c webs.h firewall.h

wusfirewall.o: wusfirewall.c 

wusDnsFilter.o: wusDnsFilter.c webs.h
uttfunction.o: uttfunction.c uttfunction.h

arpbind.o:arpbind.c

#global.o:global.c

#group.o:group.c

wusAdmin$(PAGE_NAME_TYPE).o:wusAdmin$(PAGE_NAME_TYPE).c

wusstaticnat.o:wusstaticnat.c

wusnatrule.o:wusnatrule.c

form.o:  form.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

goahead.o: goahead.c uemf.h wsIntrn.h internet.h utils.h wireless.h firewall.h management.h qos.h $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

h.o:  h.c uemf.h

handler.o:  handler.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

inic.o: inic.c inic.h internet.h utils.h webs.h 

legacy.o: legacy.c legacy.h internet.h utils.h webs.h 

internet.o: internet.c internet.h utils.h webs.h  $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

management.o: management.c management.h webs.h $(CONF_H) $(UCONF_H) 

matrix_ssl.o: matrix_ssl.c wsIntrn.h webs.h websSSL.h sslSocket.h

md5c.o:  md5.h wsIntrn.h uemf.h

media.o: media.c media.h webs.h

mime.o:  mime.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

misc.o:  misc.c uemf.h

page.o:  page.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

ringq.o:  ringq.c uemf.h

rom.o:  rom.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

security.o:  security.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

sock.o:  sock.c uemf.h

sockGen.o:  sockGen.c uemf.h $(CONF_H)

sslSocket.o: sslSocket.c sslSocket.h

station.o: station.c station.h oid.h stapriv.h webs.h

usb.o: usb.c usb.h webs.h $(UCONF_H)

sym.o:  sym.c uemf.h

uemf.o:  uemf.c uemf.h

um.o:  webs.h wsIntrn.h um.h uemf.h

umui.o:  webs.h wsIntrn.h um.h uemf.h

url.o:  url.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

des.o: des.h

utils.o: utils.c utils.h webs.h $(CONF_H) $(UCONF_H)

value.o:  value.c uemf.h

webrom.o:  webrom.c webs.h wsIntrn.h uemf.h

webs.o:  webs.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

websda.o:  webs.h wsIntrn.h websda.h uemf.h

websuemf.o:  websuemf.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

websSSL.o:  websSSL.c websSSL.h wsIntrn.h ej.h ejIntrn.h uemf.h

wireless.o: wireless.c wireless.h internet.h utils.h webs.h $(CONF_H) $(UCONF_H)

wps.o: wps.c wps.h utils.h webs.h internet.h wireless.h station.h oid.h $(CONF_H)

qos.o: qos.h utils.h $(CONF_H)

wusgroupbase.o:wusgroupbase.c

wuspdb.o:wuspdb.c

wusDMZ.o:wusDMZ.c

wusupnp.o:wusupnp.c

wusWlanBase.o:wusWlanBase.c

wusWlanAdvanced.o:wusWlanAdvanced.c

wusWlanSecurity.o:wusWlanSecurity.c

wusNotice.o:wusNotice.c

wusWebAuth.o:wusWebAuth.c

wusPortMirror.o:wusPortMirror.c

wusExceptQQ.o:wusExceptQQ.c

wusExceptMSN.o:wusExceptMSN.c

wusExceptAli.o:wusExceptAli.c

wusIpGroup.o:wusIpGroup.c

wusFwService.o:wusFwService.c

wusTimeRange.o:wusTimeRange.c

wusLogManage.o:wusLogManage.c

wusPortMirror6530G.o:wusPortMirror6530G.c

wusConnLimit.o:wusConnLimit.c

wusPortSpeed.o:wusPortSpeed.c

wusIPSec.o:wusIPSec.c

wusLicense.o:wusLicense.c

wusHardwareNAT.o:wusHardwareNAT.c

wusP2PLimit.o:wusP2PLimit.c

wusEasyTask.o:wusEasyTask.c

wusMacFilter.o:wusMacFilter.c

dyn_Config.o:dyn_Config.c

wusSnmp.o:wusSnmp.c

wusSNMP.o:wusSNMP.c

wusTagVlan.o:wusTagVlan.c

wusDiagnose.o:wusDiagnose.c

wusNetworkStatus.o:wusNetworkStatus.c 

wusAppManage.o:wusAppManage.c

sha1.o:sha1.c

linklist.o:linklist.c

wusWebAuthCaptcha.o:wusWebAuthCaptcha.c

wusIpv6Internet.o:wusIpv6Internet.c

wusRadvd.o:wusRadvd.c

wusDhcp6.o:wusDhcp6.c

wusIpv6RouteConfig.o:wusIpv6RouteConfig.c

wusIpv6PrefixList.o:wusIpv6PrefixList.c

wusTunnel.o:wusTunnel.c

wusIsatap.o:wusIsatap.c
