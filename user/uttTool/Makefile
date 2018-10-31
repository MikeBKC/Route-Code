#added by brwang
CLI_STCONFIG = cliStConf
EXEC = uttif
EXEC_SN = uttsn
EXEC_CRC = uttCrcCheckTool 
EXEC_SN_SOFT = uttsn_soft

EXEC_FC = uttfc
EXEC_BOOT = uttboot
EXEC_MACHINES = uttmachines
EXEC_STATS = uttStatsClear
EXEC_SELFHELP = uttclearOptCnt
EXEC_UPDATEDOMAIN = uttUpDomain
EXEC_UPDATEDOMAINWIFI = uttUpDomainwifi
EXEC_LICENSE = uttlicense
EXEC_DEBUG = uttDebug
EXEC_REBOOTS = rebootS
EXEC_FLASHWRITE = flashWrite
EXEC_PRDEBUG = prdebug
EXEC_3G_SWITCH = utt3gswitch
EXEC_3G_CAT = utt3gcat
EXEC_READERRMSG = readerrmsg
EXEC_ERASEERRMSG = eraseerrmsg
EXEC_LINK_CHECK = uttLinkCheck
EXEC_LINE_DETECTION = uttLineDetec
EXEC_MII_8367M = mii_8367m
EXEC_8337N = 8337_mgr 
EXEC_PORTSET = uttportset
EXEC_WEBAUTHSES = webAuthSes
EXEC_VPNHANDLE = vpnHandle
EXEC_UDPSERVER = udpserver
EXEC_WEBUDPSERVER = webudpserver
EXEC_SMSCLIENT = smsClient
EXEC_SMSSEND = smssend
EXEC_USBSWITCH = uttusbswitch
EXEC_UTTMATHCAL = mathCal
EXEC_ADFILTER = adfilter
EXEC_BEATHEART = beatheart
EXEC_APPCHECKTOOL= uttAppCheckTool
EXEC_APPUPDATETOOL= uttAppUpdateTool
EXEC_CHECKTOOL= checkreboot
EXEC_GETIPANDNM= netmask_tool 

EXEC_UTTCFGCLI = uttCfgCli
EXEC_UTTCFGSER = uttCfgSer

OBJS_UTTCFGSER = uttCfgSer.o
OBJS_UTTCFGCLI = uttCfgCli.o

OBJS = uttifname.o
OBJS_SN = uttsn.o
OBJS_CRC = uttCrcCheckTool.o
OBJS_SN_SOFT =uttsn_soft.o
OBJS_FC = uttfc.o
OBJS_BOOT = uttupgradeboot.o
OBJS_MACHINES = uttmachines.o
OBJS_STATS = uttStatsClear.o
OBJS_SELFHELP = uttclearOptCnt.o
OBJS_UPDATEDOMAIN = uttUpDomain.o
OBJS_LICENSE = uttlicense.o
OBJS_DEBUG = uttDebug.o
OBJS_REBOOTS = rebootS.o
OBJS_FLASHWRITE = flashWrite.o
OBJS_PRDEBUG = uttprDebug.o
OBJS_3G_SWITCH = utt3gswitch.o
OBJS_3G_CAT = utt3gcat.o
OBJS_READERRMSG = readerrmsg.o
OBJS_ERASEERRMSG = eraseerrmsg.o
OBJS_LINK_CHECK = uttLinkCheck.o
OBJS_LINE_DETECTION = uttLineDetection.o uttPing.o
OBJS_MII_8367M = mii_8367m.o
OBJS_8337N = 8337_mgr.o
OBJS_PORTSET = uttportset.o
OBJS_WEBAUTHSES = uttWebAuthSession.o
OBJS_VPNHANDLE = uttVpnHandle.o
OBJS_UDPSERVER = udpserver.o
OBJS_WEBUDPSERVER = webudpserver.o
OBJS_SMSCLIENT = smsClient.o
OBJS_SMSCOM = smssend.o
OBJS_USBSWITCH = uttusbswitch.o
OBJS_UTTMATHCAL = mathCal.o
OBJS_ADFILTER = adfilter.o
OBJS_BEATHEART = uttbeatheart.o
OBJS_APPCHECKTOOL= uttAppCheckTool.o
OBJS_APPUPDATETOOL= uttAppUpdateTool.o md5.o
OBJS_CHECKREBOOT= checkreboot.o
OBJS_GETSTATINFO= uttgetstatinfo.o 
OBJS_GETIPANDNM= uttIpAndNm.o 

CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include -I$(ROOTDIR)/lib/libconfig/cliConfig
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli
LDFLAGS += -L$(ROOTDIR)/lib/lib -lconfig
ifneq ("$(CONFIG_DEFAULTS_FREESCALE_P1010)", "")
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net
endif
#ifneq ("$(CONFIG_USER_L2TP_SERVER)", "")
CFLAGS += -I$(ROOTDIR)/user/ppp-2.4.4/pppd
#endif

EXE_SETS = $(EXEC) $(EXEC_SN) $(EXEC_CRC) $(EXEC_SN_SOFT) $(EXEC_FC) $(EXEC_BOOT) $(EXEC_MACHINES) $(EXEC_SELFHELP) $(EXEC_UPDATEDOMAIN) $(EXEC_UPDATEDOMAINWIFI) $(EXEC_LICENSE) $(EXEC_REBOOTS) $(EXEC_FLASHWRITE) $(EXEC_3G_SWITCH) $(EXEC_3G_CAT) $(EXEC_LINK_CHECK) $(EXEC_LINE_DETECTION) $(EXEC_PORTSET) $(EXEC_WEBAUTHSES) $(EXEC_VPNHANDLE) $(EXEC_UDPSERVER) $(EXEC_WEBUDPSERVER) $(EXEC_SMSCLIENT) $(EXEC_UTTMATHCAL)  $(EXEC_BEATHEART)

ifneq ("$(CONFIG_BOARD_P1010_B)", "")
    EXE_SETS += $(EXEC_MII_8367M)
endif

ifneq ("$(CONFIG_BOARD_P1010_C)", "")
ifeq ("$(CONFIG_AR8337N)", "y")
    EXE_SETS += $(EXEC_MII_8367M)
    EXE_SETS += $(EXEC_8337N)
endif
endif

ifeq ("$(CONFIG_CISCO_KLISH)","y")
    EXE_SETS += $(CLI_STCONFIG)
endif

ifeq ("$(CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA)", "y")
    EXE_SETS += $(EXEC_STATS)
endif

ifeq ("$(CONFIG_UTT_DEBUG)","y")
    EXE_SETS += $(EXEC_DEBUG)
    EXE_SETS += $(EXEC_PRDEBUG)
endif

ifeq ("$(CONFIG_UTT_PANIC_DEBUG)","y") -a ("$(CONFIG_MIPS)","y")
    EXE_SETS += $(EXEC_READERRMSG)
    EXE_SETS += $(EXEC_ERASEERRMSG)
endif

ifeq ("$(CONFIG_USER_WIFIDOG)", "y")
    EXE_SETS += $(EXEC_SMSSEND)
    EXE_SETS += $(EXEC_USBSWITCH)
endif

ifneq ("$(CONFIG_UTT_ADFILTER)", "")
    EXE_SETS += $(EXEC_ADFILTER)
endif

ifneq ("$(CONFIG_UTT_APPUPDATETOOL)", "")
    CFLAGS += -I$(ROOTDIR)/lib/libcJSON
    LDFLAGS += -lcJSON
    EXE_SETS += $(EXEC_APPCHECKTOOL)
    EXE_SETS += $(EXEC_APPUPDATETOOL)
endif
ifeq ("$(CONFIG_USER_CFGCLI_LAN)","y")
    EXE_SETS += $(EXEC_UTTCFGCLI)
endif
ifeq ("$(CONFIG_USER_CFGSER_LAN)","y")
	EXE_SETS += $(EXEC_UTTCFGSER)
endif
    EXE_SETS += $(EXEC_CHECKTOOL)
    EXE_SETS += $(EXEC_GETIPANDNM)

all:$(EXE_SETS)

$(CLI_STCONFIG):uttCliStConf.o
	$(CC) $(LDFLAGS) -lcliconf -o $@ uttCliStConf.o
	$(STRIP) --remove-section=.note --remove-section=.comment $@
$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_SN):$(OBJS_SN)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_SN)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_CRC):$(OBJS_CRC)
	    $(CC) $(LDFLAGS) -o $@ $(OBJS_CRC)
		    $(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_SN_SOFT):$(OBJS_SN_SOFT)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_SN_SOFT)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_FC):$(OBJS_FC)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_FC)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_BOOT):$(OBJS_BOOT)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_BOOT)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_MACHINES):$(OBJS_MACHINES)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_MACHINES)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_STATS):$(OBJS_STATS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_STATS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_SELFHELP):$(OBJS_SELFHELP)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_SELFHELP)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_UPDATEDOMAIN):$(OBJS_UPDATEDOMAIN)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_UPDATEDOMAIN)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_UPDATEDOMAINWIFI):$(OBJS_UPDATEDOMAIN)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_UPDATEDOMAIN)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_LICENSE):$(OBJS_LICENSE)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_LICENSE)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_DEBUG):$(OBJS_DEBUG)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_DEBUG)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_REBOOTS):$(OBJS_REBOOTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_REBOOTS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_FLASHWRITE):$(OBJS_FLASHWRITE)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_FLASHWRITE)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_PRDEBUG):$(OBJS_PRDEBUG)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_PRDEBUG)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_3G_SWITCH):$(OBJS_3G_SWITCH)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_3G_SWITCH)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_3G_CAT):$(OBJS_3G_CAT)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_3G_CAT)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_READERRMSG):$(OBJS_READERRMSG)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_READERRMSG)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_ERASEERRMSG):$(OBJS_ERASEERRMSG)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_ERASEERRMSG)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_LINK_CHECK):$(OBJS_LINK_CHECK)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_LINK_CHECK)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_LINE_DETECTION):$(OBJS_LINE_DETECTION)
	$(CC) -lpthread -o $@ $(OBJS_LINE_DETECTION)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_MII_8367M):$(OBJS_MII_8367M)
	$(CC) -o $@ $(OBJS_MII_8367M)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_8337N):$(OBJS_8337N)
	$(CC) -o $@ $(OBJS_8337N)

$(EXEC_PORTSET):$(OBJS_PORTSET)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_PORTSET)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_WEBAUTHSES):$(OBJS_WEBAUTHSES)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_WEBAUTHSES)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_VPNHANDLE):$(OBJS_VPNHANDLE)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_VPNHANDLE)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_UDPSERVER):$(OBJS_UDPSERVER)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_UDPSERVER)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_WEBUDPSERVER):$(OBJS_WEBUDPSERVER)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_WEBUDPSERVER)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_SMSCLIENT):$(OBJS_SMSCLIENT)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_SMSCLIENT)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_SMSSEND):$(OBJS_SMSCOM)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_SMSCOM)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_USBSWITCH):$(OBJS_USBSWITCH)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_USBSWITCH)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_UTTMATHCAL):$(OBJS_UTTMATHCAL)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_UTTMATHCAL)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_ADFILTER):$(OBJS_ADFILTER)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_ADFILTER)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_BEATHEART):$(OBJS_BEATHEART)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_BEATHEART)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_APPCHECKTOOL):$(OBJS_APPCHECKTOOL)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_APPCHECKTOOL)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_CHECKTOOL):$(OBJS_CHECKREBOOT)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_CHECKREBOOT)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

$(EXEC_GETIPANDNM):$(OBJS_GETIPANDNM)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_GETIPANDNM) 
	$(STRIP) --remove-section=.note --remove-section=.comment $@
	
$(EXEC_APPUPDATETOOL):$(OBJS_APPUPDATETOOL)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_APPUPDATETOOL)
	$(STRIP) --remove-section=.note --remove-section=.comment $@






ifeq ("$(CONFIG_USER_CFGCLI_LAN)","y")
$(EXEC_UTTCFGCLI):$(OBJS_UTTCFGCLI)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_UTTCFGCLI)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
endif
ifeq ("$(CONFIG_USER_CFGSER_LAN)","y")
$(EXEC_UTTCFGSER):$(OBJS_UTTCFGSER)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_UTTCFGSER)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
endif
romfs:

ifneq ("$(CONFIG_UTT_AP)", "y")
ifeq ("$(CONFIG_CISCO_KLISH)","y")
	$(ROMFSINST) /bin/$(CLI_STCONFIG)
endif
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/$(EXEC_MACHINES)
ifeq ("$(CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA)", "y")
	$(ROMFSINST) /bin/$(EXEC_STATS)
endif
ifeq ("$(CONFIG_UTT_DEBUG)","y")
	$(ROMFSINST) /bin/$(EXEC_DEBUG)
	$(ROMFSINST) /bin/$(EXEC_PRDEBUG)
endif
	$(ROMFSINST) /bin/$(EXEC_SELFHELP)
	$(ROMFSINST) /bin/$(EXEC_UPDATEDOMAIN)
	$(ROMFSINST) /bin/$(EXEC_LICENSE)
	$(ROMFSINST) /bin/$(EXEC_REBOOTS)
	$(ROMFSINST) /bin/$(EXEC_FLASHWRITE)
	$(ROMFSINST) /bin/$(EXEC_CRC)	
ifeq ("$(CONFIG_USER_3G)", "y")	
	$(ROMFSINST) /bin/$(EXEC_3G_SWITCH)
	$(ROMFSINST) /bin/$(EXEC_3G_CAT)
endif
ifeq ("$(CONFIG_UTT_PANIC_DEBUG)","y") -a ("$(CONFIG_MIPS)","y")
	$(ROMFSINST) /bin/$(EXEC_READERRMSG)
	$(ROMFSINST) /bin/$(EXEC_ERASEERRMSG)
endif
	$(ROMFSINST) /bin/$(EXEC_LINK_CHECK)
	$(ROMFSINST) /bin/$(EXEC_LINE_DETECTION)
ifneq ("$(CONFIG_BOARD_P1010_B)", "")
	$(ROMFSINST) /bin/$(EXEC_MII_8367M)
endif
ifneq ("$(CONFIG_BOARD_P1010_C)", "")
ifeq ("$(CONFIG_AR8337N)", "y")
	$(ROMFSINST) /bin/$(EXEC_MII_8367M)
	$(ROMFSINST) /bin/$(EXEC_8337N)
endif
endif
	$(ROMFSINST) /bin/$(EXEC_PORTSET)
	$(ROMFSINST) /bin/$(EXEC_WEBAUTHSES)
	$(ROMFSINST) /bin/$(EXEC_VPNHANDLE)
	$(ROMFSINST) /bin/$(EXEC_UDPSERVER)
	$(ROMFSINST) /bin/$(EXEC_WEBUDPSERVER)
	$(ROMFSINST) /bin/$(EXEC_SMSCLIENT)
ifeq ("$(CONFIG_USER_WIFIDOG)", "y")
	$(ROMFSINST) /bin/$(EXEC_UPDATEDOMAINWIFI)
	$(ROMFSINST) /bin/$(EXEC_SMSSEND)
	$(ROMFSINST) /bin/$(EXEC_USBSWITCH)
	$(ROMFSINST) /bin/$(EXEC_3G_CAT)
endif
	$(ROMFSINST) /bin/$(EXEC_UTTMATHCAL)
ifneq ("$(CONFIG_UTT_ADFILTER)", "")
	$(ROMFSINST) /bin/$(EXEC_ADFILTER)
endif
ifneq ("$(CONFIG_UTT_APPUPDATETOOL)", "")
	$(ROMFSINST) /bin/$(EXEC_APPCHECKTOOL)
	$(ROMFSINST) /bin/$(EXEC_APPUPDATETOOL)
endif
	$(ROMFSINST) /bin/$(EXEC_BEATHEART)
ifeq ("$(CONFIG_USER_CFGCLI_LAN)","y")
	$(ROMFSINST) /bin/$(EXEC_UTTCFGCLI)
endif
ifeq ("$(CONFIG_USER_CFGSER_LAN)","y")
	$(ROMFSINST) /bin/$(EXEC_UTTCFGSER)
endif
else
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/$(EXEC_REBOOTS)
endif

	$(ROMFSINST) /bin/$(EXEC_CHECKTOOL)
	
	$(ROMFSINST) /bin/$(EXEC_GETIPANDNM)
clean:
	rm -f  $(CLI_STCONFIG) $(EXEC) $(EXEC_SN) $(OBJS_CRC) $(OBJS_CHECKREBOOT) $(OBJS_GETSTATINFO) $(EXEC_SN_SOFT) $(EXEC_FC) $(EXEC_BOOT) $(EXEC_MACHINES) $(EXEC_STATS) $(EXEC_SELFHELP) $(EXEC_UPDATEDOMAIN) $(EXEC_UPDATEDOMAINWIFI) $(EXEC_LICENSE) $(EXEC_REBOOTS) $(EXEC_FLASHWRITE) $(EXEC_PRDEBUG) $(EXEC_3G_SWITCH) $(EXEC_3G_CAT) $(EXEC_READERRMSG) $(EXEC_ERASEERRMSG) $(EXEC_LINK_CHECK) $(EXEC_LINE_DETECTION) $(EXEC_MII_8367M) $(EXEC_PORTSET) $(EXEC_WEBAUTHSES) $(EXEC_VPNHANDLE) $(EXEC_UDPSERVER) $(EXEC_WEBUDPSERVER) $(EXEC_SMSCLIENT) $(EXEC_UTTMATHCAL) $(EXEC_ADFILTER) $(EXEC_BEATHEART) $(EXEC_APPCHECKTOOL) $(EXEC_CHECKTOOL) $(EXEC_APPUPDATETOOL) $(EXEC_UTTCFGCLI) $(EXEC_UTTCFGSER) $(EXEC_GETIPANDNM) *.o
