.EXPORT_ALL_VARIABLES:

MKchange=y
#DIRS = src speedgoahead
DIRS = cgi-src src speedgoahead
ROOT_DIRECTORY = /etc_ro/web
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_EN)", "y")
ROOT_DIRECTORY_LANG_EN = /etc_ro/web/lang/en
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_ZHCN)", "y")
ROOT_DIRECTORY_LANG_CN = /etc_ro/web/lang/zhcn
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_ZHTW)", "y")
ROOT_DIRECTORY_LANG_TW = /etc_ro/web/lang/zhtw
endif
ROM_WEBS = \
AntiNetSnifer.asp         FireWall.asp         _list1.htm           PPPoENotice.asp         TimeRange_edit.asp\
assist.asp                FireWall_edit.asp    _listhead1.htm       PPPoEServer.asp         top.asp\
bottom.asp      	  DnsNotify.htm        LogManage.asp        PPTPClient.asp          update.asp\
Syslog.asp                framePage.asp        IdentifyBind.asp     PPTPClientList.asp      UPnP.asp\
QQFilterImport.asp	  SysLogInfo.asp       MSNFilter.asp        PPTPSrvAccount.asp      User.asp\
			  SysLogInfo_edit.asp  MSNFilter_edit.asp   PPTPSrvGlobal.asp       User_edit.asp\
                          FwBase.asp           NatRule.asp          QQFilter.asp            UserGroup.asp\
css/                      FwBaseOut.asp        NatRule_edit.asp     QQFilter_edit.asp       UserGroup_edit.asp\
DDNS.asp                  GlobalBase.asp       NatStatic.asp        ReBoot.asp              UserLinkRate.asp\
DeviceConfig.asp          GlobalBase_edit.asp  NatStatic_edit.asp   reload.asp              \
DeviceLoginControl.asp    Group.asp            NetActionAudit.asp   Remote.asp              WANConfig.asp\
DhcpClient.asp            Group_edit.asp       Notice.asp           WANportnumber.asp\
DhcpFix.asp               hotel.asp            Notify.asp           Route.asp               waUsers.asp\
DhcpFix_edit.asp          images/              Notify.htm           Route_edit.asp   \
DhcpServer.asp            index.asp            DhcpAutoBind.asp     RunStatus.asp           WebAuth_auth.asp\
DMZ.asp                   InterfaceFlow.asp    other/                                       WebAuth_edit.asp\
DnsFilter.asp             IPMac.asp            DnsFilter_Notice.asp SaleService.asp         WebAuth_message.asp\
DoubleRoad.asp            IPMacConfig.asp      PdbUpdata.asp  \
DoubleRoadInfo.asp        IPMac_edit.asp                    \
DoubleRoadInfo_edit.asp   js/                  poeUsers.asp         secondTop.asp          \
					                            SmartQos.asp            \
FastConfig.asp				                            SoftwareUpdate.asp      \
					       PPPoEConSta.asp      SysClock.asp            \
FastConfig_Net.asp        LANConfig.asp        PPPoE_edit.asp       SysInfo.asp             \
                          lang/                                     TaskScheduler.asp\
                          langChange.asp       PPPoEGlobal.asp      TaskScheduler_edit.asp\
                          left.asp             PPPoEImport.asp      TimeRange.asp ConnectionLimit.asp IPSec.asp IPSec_edit.asp License.asp\
PolicyRoute.asp PolicyRoute_edit.asp  P2PLimit.asp	WebAuthGlobal.asp	WebAuthServer.asp ParentsControl.asp ParentsControl_edit.asp\
EasyManageConfig.asp	  EasyManage.asp       NoticEasyConfig.asp  EasyManageWelcome.asp	PPPoEExport.asp\
EasyManageSuccess.asp	  MacFilter.asp	       MacFilter_edit.asp   MacFilterConfig.asp	   Snmp.asp vlanConfig.asp vlanEdit.asp 


ifeq ("$(CONFIG_USER_CACHE)","y")
ROM_WEBS += cachePage.asp
endif
ifeq ("$(CONFIG_USER_CACHE_SERVER)","y")
ROM_WEBS += CacheServerConfig.asp
endif
ifeq ("$(CONFIG_USER_NET_MODE_SWITCH)","y")
#ROM_WEBS += netModeSwitch.asp
#ROM_WEBS += netModeReload.asp
endif
ifeq ("$(CONFIG_USER_BRIDGE)","y")
#ROM_WEBS += bridge.asp
ROM_WEBS += netConfig.asp
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_EN)", "y")
ROM_WEBS += enrhelpinfo.htm
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_ZHTW)", "y")
ROM_WEBS += zhtwrhelpinfo.htm
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_ZHCN)", "y")
ROM_WEBS += rhelpinfo.htm
ifneq ("$(CONFIG_UTT_NV1800N)", "") 
ROM_WEBS += FitApHelpInfo.htm
endif
endif

ifneq ("$(CONFIG_SWITCH_EXT)$(CONFIG_UTT_AC)", "") 
    ROM_WEBS += switch/
    ROM_WEBS += cgi-bin/
ifneq ("$(CONFIG_UTT_NV1540)", "") 
    ROM_WEBS_DEL += /switch/aggregation.asp
else
    ROM_WEBS_DEL += /switch/aggregation_100M.asp
endif
endif

ifeq ("$(CONFIG_UTT_NV1800N)", "")
ROM_WEBS += port_vlan_rdd.asp
endif


ifneq ("$(CONFIG_UTT_NV840G)$(CONFIG_UTT_NV3320G)$(CONFIG_UTT_NV840V2)$(CONFIG_UTT_MV841)$(CONFIG_UTT_WX100)$(CONFIG_UTT_NV512W)$(CONFIG_UTT_NV512WV3)", "") 
    ROM_WEBS_DEL += PPTPClient.asp PPTPClientList.asp PPTPSrvAccount.asp PPTPSrvGlobal.asp
endif

ifneq ("$(CONFIG_UTT_NV510W)$(CONFIG_UTT_NV520W)$(CONFIG_UTT_U1000N)$(CONFIG_UTT_NV512W)$(CONFIG_UTT_NV518W)$(CONFIG_UTT_NV842W)$(CONFIG_UTT_NV520WV3)$(CONFIG_UTT_NV842WV3)$(CONFIG_UTT_NV512WV3)$(CONFIG_UTT_NV518WV3)", "") 
    ROM_WEBS += WlanBase.asp WlanAdvanced.asp WlanHostInfo.asp WlanMacFilter.asp WlanMacFilter_edit.asp \
		WlanSecurity.asp FastConfig_Net_3G.asp FastConfig_Net_WLAN.asp FastConfig_OpMode.asp \
		FastConfig_Wlan.asp    
#   ROM_WEBS += WlanMultiSsid.asp WlanMultiSsid_edit.asp
endif
ifneq ("$(CONFIG_UTT_NV520W)$(CONFIG_UTT_U1000N)$(CONFIG_UTT_NV512W)$(CONFIG_UTT_NV518W)$(CONFIG_UTT_NV842W)$(CONFIG_UTT_NV520WV3)$(CONFIG_UTT_NV842WV3)$(CONFIG_UTT_NV512WV3)$(CONFIG_UTT_NV518WV3)", "") 
    ROM_WEBS_DEL += FastConfig_OpMode.asp License.asp FastConfig_Net_3G.asp
    ROM_WEBS += FastConfig_OpMode_Ap.asp
endif
ifneq ("$(CONFIG_UTT_nv520W)$(CONFIG_UTT_NV512W)$(CONFIG_UTT_NV520WV3)$(CONFIG_UTT_NV842WV3)$(CONFIG_UTT_NV512WV3)", "") 
    ROM_WEBS_DEL += IPSec.asp IPSec_edit.asp
endif
ifneq ("$(CONFIG_UTT_NV1800N)", "") 
    ROM_WEBS += WlanBase.asp WlanAdvanced.asp WlanHostInfo.asp WlanMacFilter.asp WlanMacFilter_edit.asp \
		WlanSecurity.asp 
#   ROM_WEBS += WlanMultiSsid.asp WlanMultiSsid_edit.asp
endif
ifneq ("$(CONFIG_UTT_LV4000)$(CONFIG_UTT_LV6530G)$(CONFIG_UTT_LV4330G)$(CONFIG_UTT_LVN8000)$(CONFIG_UTT_LV5830G)$(CONFIG_UTT_LVNE6000)$(CONFIG_UTT_LV2200)$(CONFIG_UTT_LV6000)$(CONFIG_UTT_LV6830G)$(CONFIG_UTT_WX2000)", "") 
    ROM_WEBS += PortMirror6530G.asp
endif
ifneq ("$(CONFIG_UTT_MV821)$(CONFIG_UTT_MV811)$(CONFIG_UTT_MV841)", "") 
    ROM_WEBS += PortMirror.asp
endif
ifneq ("$(CONFIG_UTT_NV840E)$(CONFIG_UTT_NV840D)$(CONFIG_UTT_NV800)", "")
    ROM_WEBS += PortMirror840E.asp
endif
ifneq ("$(CONFIG_UTT_NV840G)$(CONFIG_UTT_NV521G)$(CONFIG_UTT_NV1220G)$(CONFIG_UTT_NV3320G)$(CONFIG_UTT_WX100)", "") 
    ROM_WEBS += PortMirror8367M.asp HardwareNAT.asp
endif
ifeq ("$(CONFIG_UTT_AP)", "y")
	ROM_WEBS += fitApSysInfo.asp fitApClts.asp apParaSwitch.asp wlanPara.asp fatFitSwitch.asp fitApIfConf.asp apSwitching.asp \
		fitApReload.asp
endif
ifeq ("$(CONFIG_UTT_AC)", "y")
	ROM_WEBS += DhcpPoolForAc.asp DhcpServerForAc.asp DhcpFixForAc.asp DhcpClientForAc.asp DnsProxyForAc.asp DnsProxy.asp \
		DhcpFix_editForAc.asp apManage.asp apPermission.asp apDisCover.asp serviceZone.asp edit_SZ.asp apDiscover.asp\
		apTmp.asp editTemp.asp wlanUpdateUpload.asp getApConf.asp apUpdateConf.asp apUploadConf.asp wlanCltList.asp apMngRecord.asp apSwitching.asp\
		ap_wait_remote_auth.asp ap_remote_manage_group.asp ap_remote_manage.asp apBasicConf.asp ap_wait_rrcp.asp\
		ap_wait_remote_conf.asp apSoftMng.asp getApConf.asp pushConf.asp pushTmp.asp pushSoft.asp acMode.asp cgi-bin/
endif

#the pages not used 
#ROM_WEBS += CallTrace.asp ConnectionLimit.asp ConnectionLimit_edit.asp error.asp FastConfig_Net2.asp\
            FluxStat.asp  FwAddress.asp FwAddress_edit.asp L2TPClient.asp L2TPList.asp L2TPSrvAccount.asp\
	    L2TPSrvGlobal.asp MACClone.asp OpMode.asp pdbupgroup.asp pdbup.asp portnumber.asp PPPoEFlowNotice.asp\
	    RunStatus_Wire.asp ScanAP.asp ScanAPfrmae.asp UserStats.asp  

UTT_WEBS_ASP = FastConfig RunStatus InterfaceFlow ReBoot WANConfig DoubleRoad portnumber LANConfig DhcpServer DDNS UPnP WlanBase WlanSecurity WlanMacFilter WlanAdvanced WlanMultiSsid WlanHostInfo NatStatic IPMac Route PPPoEGlobal AntiNetSnifer ports mirror port_vlan aggregation_100M GlobalBase PdbUpdata Group SmartQos Notice FwBase FireWall DnsFilter ConnectionLimit PPTPClientList L2TPList discover linkageConf topoy User langChange SysClock DeviceConfig SoftwareUpdate Remote TaskScheduler RunStatus UserStats FluxStat SysInfo SaleService
#ifeq ("$(CONFIG_IP1725)", "y")
#WEB_DIR = web_100M
#else
#WEB_DIR = web
#endif

EN_IMGS = $(ROMFSDIR)$(ROOT_DIRECTORY)/images/en_Forum.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/en_Knowledge.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/en_Reservation.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/en_guanggao.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/en_seruttcare.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/en_start.gif 

ZHCN_IMGS = $(ROMFSDIR)$(ROOT_DIRECTORY)/images/serbook.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/serlib.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/serbbs.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/guanggao.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/seruttcare.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/start.gif 

ZHTW_IMGS = $(ROMFSDIR)$(ROOT_DIRECTORY)/images/zhtwserbook.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/zhtwserlib.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/zhtwserbbs.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/zhtwguanggao.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/zhtwseruttcare.gif $(ROMFSDIR)$(ROOT_DIRECTORY)/images/zhtwstart.gif 
#CFLAGS += -I./include
all:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

#needfile:
#ifeq ($(CONFIG_DEFAULTS_ATHEROS_AR7100),y)
#	cp $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h ./include/linux -f
#	cp $(ROOTDIR)/config/autoconf.h ./include/config -f
#endif


romfs:
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY) 
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)
ifeq ("$(MKchange)","y")
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/js
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/css
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/other
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/images
ifneq ("$(CONFIG_SWITCH_EXT)$(CONFIG_UTT_AC)", "") 
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/cgi-bin
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/switch
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/switch/js
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/switch/other
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY)/switch/images
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_EN)", "y")
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY_LANG_EN)
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_ZHCN)", "y")
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY_LANG_CN)
endif
ifeq ("$(CONFIG_USER_GOAHEAD_LANG_ZHTW)", "y")
	mkdir -p $(ROMFSDIR)$(ROOT_DIRECTORY_LANG_TW)
endif
	for i in $(ROMFS_WEBS) ; do $(ROMFSINST) -d web/$$i $(ROOT_DIRECTORY)/$$i; done
ifeq ("$(CONFIG_UTT_TOMAX)", "y")
	## ToMAX页面控制, ToMAX新页面在web/ToMAX目录，一些老js还在web中，复用老js和css等
	for i in $(ROMFS_WEBS_ToMAX) ; do $(ROMFSINST) -d web/ToMAX/$$i $(ROOT_DIRECTORY)/$$i; done
endif
else
	for i in $(ROM_WEBS) ; do $(ROMFSINST) web/$$i $(ROOT_DIRECTORY)/$$i; done
	for i in $(ROM_WEBS_DEL) ; do rm $(ROMFSDIR)$(ROOT_DIRECTORY)/$$i; done
endif
#	$(ROMFSINST) $(ROOT_DIRECTORY)

#	for i in $(UTT_WEBS_ASP) ; do make -C $$i $@ || exit $?; done
#ifeq ("$(CONFIG_USER_PPPOECD)", "y")
#	mv $(ROMFSDIR)$(ROOT_DIRECTORY)/internet/wan_simple.asp $(ROMFSDIR)$(ROOT_DIRECTORY)/internet/wan.asp
#endif
#ifeq ("$(CONFIG_RT2860V2_AP_MESH)", "")
#ifeq ("$(CONFIG_RT2860V2_STA_MESH)", "")
#	rm $(ROMFSDIR)$(ROOT_DIRECTORY)/wireless/mesh.asp
#endif
#endif
#ifeq ("$(CONFIG_RT2860V2_AP_WDS)", "")
#	rm $(ROMFSDIR)$(ROOT_DIRECTORY)/wireless/wds.asp
#endif
#ifeq ("$(CONFIG_RT2860V2_STA)", "")
#	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/station
#	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/wps/wps_sta.asp
#endif

ifeq ("$(CONFIG_USB)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb
else
ifeq ("$(CONFIG_USER_STORAGE)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb/STORAGE*
endif
ifeq ("$(CONFIG_USER_USHARE)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb/USHARE*
endif
ifeq ("$(CONFIG_USER_UVC_STREAM)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb/UVC*
endif
ifeq ("$(CONFIG_USER_P910ND)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb/P910ND*
endif
endif

ifeq ("$(CONFIG_RALINKAPP_MPLAYER)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/media
endif

ifneq ("$(CONFIG_INIC_MII)", "") 
	$(ROMFSINST) inic $(ROOT_DIRECTORY)
else
ifneq ("$(CONFIG_INIC_PCI)", "") 
	$(ROMFSINST) inic $(ROOT_DIRECTORY)
else
ifneq ("$(CONFIG_INIC_USB)", "") 
	$(ROMFSINST) inic $(ROOT_DIRECTORY)
endif
endif
endif

ifneq ("$(CONFIG_RT2561_AP)", "")
	$(ROMFSINST) legacy $(ROOT_DIRECTORY)
endif

ifneq ("$(MKchange)","y")
ifneq ("$(CONFIG_USER_GOAHEAD_LANG_EN)", "y")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/lang/en
	rm -rf $(EN_IMGS)
endif
ifneq ("$(CONFIG_USER_GOAHEAD_LANG_ZHTW)", "y")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/lang/zhtw
	rm -rf $(ZHTW_IMGS)
endif
ifneq ("$(CONFIG_USER_GOAHEAD_LANG_ZHCN)", "y")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/lang/zhcn
	rm -rf $(ZHCN_IMGS)
endif
endif
	for i in $(DIRS) ; do make -C $$i $@ ; done
#ifeq ("$(CONFIG_LOGREAD)", "")
#	rm $(ROMFSDIR)$(ROOT_DIRECTORY)/adm/syslog.asp
#else
#ifeq ("$(CONFIG_KLOGD)", "")
#	rm $(ROMFSDIR)$(ROOT_DIRECTORY)/adm/syslog.asp
#else
#endif
#endif

clean:
ifeq ($(CONFIG_DEFAULTS_ATHEROS_AR7100),y)
	rm  ./include/linux/autoconf.h -f
	rm  ./include/config/autoconf.h -f
endif
	for i in $(DIRS) ; do make -C $$i clean ; done

include ../uttUserConf/web-config
