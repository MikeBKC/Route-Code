#added by brwang

#CC = mipsel-linux-uclibc-gcc
#STRIP = mipsel-linux-uclibc-strip
EXEC = speedweb
EXEC_BAK = speedweb_bak
DATA_COMP = $(shell date "+%y%m%d")
EXEC2 = snd2speed
EXEC3 = cachecheck
UTTCLI = uttcli
UTTCLI2 = uttcli2

OBJS = dosystem.o internet.o balloc.o pppoeServer.o DhcpServer.o main.o \
       port_vlan.o SwPortConf.o mirror.o  sw_acl.o L2tpServer.o L2tpClient.o aggregation.o PptpServer.o groupbase.o \
       nat.o SmartQos.o AntiSniffer.o pdb.o interface.o StaticRoute.o initServer.o ntp.o ddns.o \
       TaskScheduler.o DnsFilter.o firewall.o PptpClient.o Arpbind.o spdComFun.o FwBase.o WirelessMacFilter.o \
       Wireless.o Notice.o pppAccStat.o Mirror821.o WebAuth.o ExceptQQ.o ExceptMSN.o ExceptAli.o IpGroup.o FwService.o TimeGroup.o \
       Mirror6530G.o uttNfNl.o LogManage.o ConnLimit.o rdd_port_vlan.o IPSec.o Syslog.o SSH.o Hotel.o license.o \
       ApPermission.o ServiceZone.o DnsAgency.o DhcpPool.o apConf.o FatFitSwitch.o ApEffectConf.o \
       IdentifyBind.o Https.o HardwareNAT.o PolicyRoute.o SysLogInfo.o ParentsControl.o WanLinkCheck.o EasyTask.o\
       MacFilter.o snmp.o SNMP.o apLoadBalance.o TagVlan.o GameBoost.o NetSniper.o NetShareManage.o FtpServer.o NetShareUser.o apMail.o acWorkMode.o Mirror7620e.o Adfilter.o vlanConfig.o webmsg.o CapwapConfig.o \
IpConflictAutoChange.o Ipv6Internet.o Ipv6Route.o Radvd.o Dhcp6.o Tunnel.o Isatap.o cwmp.o CliForEngineerOnly.o MakeMoney.o



ifeq ($(CONFIG_USER_CACHE),y)
OBJS += cache.o
endif
ifeq ($(CONFIG_USER_NET_MODE_SWITCH),y)
#OBJS += NetModeSwitch.o
endif
ifeq ($(CONFIG_USER_BRIDGE),y)
#OBJS += Bridge.o
OBJS += CacheMode.o
endif
ifeq ($(CONFIG_USER_CACHE_SERVER),y)
OBJS += CacheServer.o
endif
OBJS2 = snd2speed.o
OBJS3 = cacheCheck.o
UTTCLIOBJS = uttcli.o
UTTCLIOBJS2 = uttcli2.o
#ROOTDIR = /data3/wang.bingrong/brwang/wlancvs/ralink-305x/source
CFLAGS  += -Wall -fno-strict-aliasing
CFLAGS  += -DUEMF -DWEBS -DUTT_HWMODEL=\"$(UTTHWMODEL)\"
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS 	+= -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS 	+= -I$(ROOTDIR)/config
CFLAGS 	+= -I$(ROOTDIR)/user/goahead/src
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli -I$(ROOTDIR)/lib/mxml-2.6
CFLAGS  += -I$(ROOTDIR)/user/ppp-2.4.4/pppd
ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010), y)
CFLAGS  += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/
endif
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include/

ifeq ($(CONFIG_LIB_LIBSWITCH_FORCE),y)
CFLAGS  += -I$(ROOTDIR)/lib/libswitch
endif

LDFLAGS += -L$(ROOTDIR)/lib/lib
LDLIBS  += -lpthread -lconfig

ifeq ($(CONFIG_LIB_LIBSWITCH_FORCE),y)
LDLIBS  += -lswitch
endif

ifneq ("$(CONFIG_LIB_LIBUSB_USER_FORCE)", "")
CFLAGS  += -I$(ROOTDIR)/lib/libusb-user
LDLIBS	+= -lusb-user
endif

ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
LDLIBS	+= -liconv
endif

all: $(EXEC) $(EXEC2) $(EXEC3) $(UTTCLI) $(UTTCLI2)


$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
	cp -fpR $(EXEC) $(EXEC_BAK)_$(DATA_COMP)

$(EXEC2):$(OBJS2)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS2) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
$(EXEC3):$(OBJS3)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS3) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
$(UTTCLI):$(UTTCLIOBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(UTTCLIOBJS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
$(UTTCLI2):$(UTTCLIOBJS2)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(UTTCLIOBJS2) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/$(EXEC2)
	$(ROMFSINST) /bin/$(EXEC3)
	$(ROMFSINST) /bin/$(UTTCLI)
	$(ROMFSINST) /bin/$(UTTCLI2)

clean:
	rm -f $(EXEC) $(EXEC2) $(EXEC3)  $(UTTCLI) $(UTTCLI2)*.o 
