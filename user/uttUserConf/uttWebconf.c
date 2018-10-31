#include "stdio.h"
#include "uttMachine.h"
#include "stdlib.h"
#include <config/autoconf.h>
#define CNT(a) sizeof(a)/sizeof(*a)

#if (UTT_SMART_UI == FYES)
#define PAGE_NAME_TYPE "Sui" 
#else
#define PAGE_NAME_TYPE ""
#endif

/*通过在此数组中添加页面来用uttMachine.h里面的宏对页面进行控制*/
char *RomWebs[] = {
#if (FEATURE_AC == FYES) && (DIAGNOSE_DETECT == FYES)
	"js/jquery.timers-1.2.js",
#endif
#if (UTT_SMART_UI == FYES)

#if (FEATURE_AC == FYES)
	"images_Sui/ApState.png",
	"images_Sui/reset.png",
	"images_Sui/restart.png",
	"images_Sui/update.png",
	"ApWirelessSeting_Sui.asp",
	"ApState.asp",
#endif

#if (UTT_KNOWIFI == FYES)
	"learnConfig.asp",
	"images_Sui/learnConfig.png",
	"lang/zhcn/learnConfig",
	"transition.asp",
	"images_Sui/configure.png",
#endif

#if (NET_SECURITY == FYES)
	"AccessControl_Sui.asp",
	"images_Sui/accessCtrl_Sui.png",
#endif

#if (UTT_LOGIN_LIMIT == FYES)
	"forbidlogin.asp",
#endif

#if ((NET_SECURITY == FYES) || (UTT_LOGIN_LIMIT == FYES))
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"lang/en/AccessControl",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"lang/zhcn/AccessControl",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"lang/zhtw/AccessControl",
#endif
#endif

#if (VPN_PROXY == FYES)
	"VPNSui.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"lang/en/VPNSui",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"lang/zhcn/VPNSui",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"lang/zhtw/VPNSui",
#endif
#endif
#if (MAKE_MONEY == FYES)
	"makeMoney.asp",
	"savePhone.asp",
	"income.asp",
//	"getParam.asp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"lang/en/MakeMoney",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"lang/zhcn/MakeMoney",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"lang/zhtw/MakeMoney",
#endif
	"index_login.asp",
	"appprogram.asp",
	"home.asp",
	"runstatus.asp",
	"sysinfo.asp",
	"SecuritySetting.asp",
	"Userstatistics.asp",
	"dhcpServer.asp",
	"lanConfig.asp",
	"DMZConfig.asp",
	"VirtualServer.asp",
	"DDNS_Sui.asp",
	"FirmwareUpgrade_Sui.asp",
	"upgrade_Sui.asp",
	"reload_Sui.asp",
	"BackupRecovery.asp",
	"UPnP_Sui.asp",
	"RemoteManage.asp",
	"SysClockManage.asp",
	"logoutSuccess_Sui.asp",
	"IPMac_Sui.asp",
	"IPMacConfigSui.asp",
	"progress.asp",
#if (ADMIN_CONF == FYES)
	"User_editSui.asp",
#endif
	"InterfaceFlow_Sui.asp",
	"AutoCheck_Sui.asp",
	"ConfWizard_Sui.asp",
	"WireWizard_Sui.asp",
	"WirelessSeting_Sui.asp",
#if (WIRELESS_5G == FYES)
	"WireWizard_Sui_5g.asp",
	"WirelessSeting5g_Sui.asp",
#endif
#if (HARDWARE_NAT == FYES)
    "hardwarenat.asp",
#endif
#if (GROUP_BASE == FYES)
	"Group_Sui.asp",
#endif
#if (HOTEL_PNP == FYES)
	"hotel_Sui.asp",
#endif
#if (STATIC_ROUTE == FYES)
	"RouteSui.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"lang/en/Route",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"lang/zhcn/Route",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"lang/zhtw/Route",
#endif
#endif
#if (MAC_FILTER == FYES)
	"MacFilterSui.asp",
#endif
#if (PARENTS_CONTROL == FYES)
	"ParentsControlSui.asp",
#endif
#if(EASY_TASK == FYES)
	"EasyManage_Sui.asp",
	"EasyManageWelcome_Sui.asp",
	"EasyManageSuccess_Sui.asp",
	"NoticEasyConfig_Sui.asp",
	"images/link1.png",
	"images_Sui/easymanage.png",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"lang/en/EasyTask",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"lang/zhcn/EasyTask",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"lang/zhtw/EasyTask",
#endif
#endif
#if ((UTT_NV_A307W == FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A309W == FYES)|| (UTT_NV_A310W == FYES) || (UTT_NV_A310Wv2 == FYES) || (UTT_NV_A650W == FYES)||(UTT_NV_A755W == FYES ) || (UTT_NV_D904W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
#if (CONNECT_LIMIT == FYES)
	"ConnectionLimit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/ConnectLimit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/ConnectLimit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/ConnectLimit",
#endif
#endif
#endif
#if (FTP_SERVER == FYES)
	"NetShare_Sui.asp",
	"images_Sui/netshare.png",
#endif
	"TimedWireless.asp",
	"redirect.asp",
#if (ADFILTER == FYES)
	"advideo_Sui.asp",
#endif
    "SmartQos_Sui.asp",
    "modelchang.asp",
#if (MIDDLE_SOFT == FYES)
    "Timeplan.asp",
#else
    "bind.asp",
#endif
#if(APP_ELINKC == FYES)
    "elinkc.asp",
#endif
    "css/default.css",
	"css/jquery-ui-1.8.12.custom.css",
	"css/style.css",
        "css/style_new.css",
	"css/uttwlan.css",
	"css/style_port.css",
	"script/curvycorners.js",
	/*"script/jquery-1.8.1.js",*/
	"script/jquery-1.8.1.min.js",
	"script/frame.js",
	"script/contextMenu.js",
	"script/iframeTools.js",
	"script/jquery.corner.js",
	"script/menu.js",
	"script/jquery.artDialog.js",
	"script/jquery.timers-1.2.js",
	"script/jquery-ui-1.8.12.custom.min.js",
	"script/jquery.selectbox.js",
	"js/common.js",
	"js/sms_send.js",
	"js/config.js",
	"js/uttwlan.js",
	"js/bwm-common.js",
	"js/dyn_config.js",
	"images/qt.gif",
	"images_Sui/IP_MAC.png",
	"images_Sui/10.png",
	"images_Sui/alert-icon.png",
	"images_Sui/backup.png",
	"images_Sui/broadband_set.png",
	"images_Sui/broadband_set.png",
	"images_Sui/clockm.png",
	"images_Sui/cloud.png",
	"images_Sui/custom.png",
	"images_Sui/dd_arrow.gif",
	"images_Sui/dhcp.png",
	"images_Sui/diagnostic_test.png",
	"images_Sui/disconnectlan.png",
	"images_Sui/dmz.png",
	"images_Sui/dns.png",
	"images_Sui/footer_line.png",
	"images_Sui/homebg_13.png",
	"images_Sui/input_txt_bg.png",
	"images_Sui/interfaceflow.png",
	"images_Sui/internet_set.png",
	"images_Sui/lan.png",
	"images_Sui/lightning.png",
	"images_Sui/loading.gif",
	"images_Sui/login_btn.png",
	"images_Sui/login_logo.png",
	"images_Sui/macfilter.png",
	"images_Sui/nav_pipe.png",
	"images_Sui/parentcontrol.png",
	"images_Sui/password_set.png",
	"images_Sui/piep_line.png",
	"images_Sui/reboot.png",
	"images_Sui/remote_magage.png",
	"images_Sui/round_buttons.png",
	"images_Sui/runstatus.png",
	"images_Sui/secondbg.png",
	"images_Sui/security.png",
	"images_Sui/softwareup.png",
	"images_Sui/sprite.png",
	"images_Sui/static_router.png",
	"images_Sui/system_info.png",
	"images_Sui/upnp.png",
	"images_Sui/userslist.png",
	"images_Sui/virtual_server.png",
	"images_Sui/wanimg.png",
	"images_Sui/wds.png",
	"images_Sui/wireless_set.png",
	"images_Sui/wireless_timing.png",
	"images_Sui/stream.gif",
	"images_Sui/logo.png",
        "images_Sui/logo1.png",
        "images_Sui/logo1.gif",
	"images_Sui/on-off.png",
	"images_Sui/wired.png",
	"images_Sui/wireless.png",
	"images_Sui/edit.png",
	"images_Sui/del.png",
	"images_Sui/signal0.png",
	"images_Sui/signal1.png",
	"images_Sui/signal2.png",
	"images_Sui/signal3.png",
	"images_Sui/signal4.png",
	"images_Sui/ADfilter.png",
	"images_Sui/broadband.png",
	"images_Sui/addDomain.png",
        "images_Sui/addDomain1.png",
	"images_Sui/hotel.png",
	"images_Sui/arrow.gif",
	"images_Sui/blur.htc",
	"images_Sui/sysclock.png",
	"images_Sui/upgrade_icon.png",
	"images_Sui/upgradeIcon1.png",
#if (APP_ELINKC == FYES)
    "images_Sui/elinkc.png",
#endif
	"images_Sui/link.png",	
#if (GALAXYWIND == FYES)
	"images_Sui/galaxywindlogo.png",
#endif
#if (MIDDLE_SOFT == FYES)
    "images_Sui/chinatelecom.png",
    "images_Sui/cmcc_logo.png",
#endif
#if (SKYWORTH == FYES)
    "images_Sui/sky.png",
#endif
#if (MAKE_MONEY == FYES)
	"images_Sui/makeMoney.png",
#endif
	"images/errorimg.png",
#if (VPN_PROXY == FYES)
	"images_Sui/vpnProxy.png",
#endif

#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/HomePage",
    "lang/en/Menu",
    "lang/en/common",
    "lang/en/SoftwareUpdate",
    "lang/en/PdbUpdata",
    "lang/en/InterfaceFlow",
    "lang/en/JS",
    "lang/en/left",
    "lang/en/SmartQos",
    "lang/en/SmartQosSui",
    "lang/en/NetShareSui",
    "lang/en/WANConfig",
    "lang/en/WANConfigSui",
    "lang/en/WirelessSui",
    "lang/en/DHCP",
    "lang/en/DDNS",
    "lang/en/SysInfo",
    "lang/en/WlanSecurity",
    "lang/en/WlanBase",
    "lang/en/LANConfig",
    "lang/en/UPnP",
    "lang/en/commonWords",
    "lang/en/Group",
    "lang/en/GroupSui",
    "lang/en/ParentsControlSui",
    "lang/en/DnsFilter",
    "lang/en/WlanMacFilter",
    "lang/en/serviceZone",
    "lang/en/NatStatic",
    "lang/en/DMZ",
    "lang/en/UserSui",
    "lang/en/RunStatus",
    "lang/en/RunStatus_Sui",
    "lang/en/MacFilterSui",
    "lang/en/TimeWireless",
    "lang/en/SysClock",
    "lang/en/Remote",
    "lang/en/User_edit",
    "lang/en/DeviceConfig",
    "lang/en/DeviceConfigSui",
    "lang/en/Diag",
    "lang/en/advideo",
    "lang/en/advideoSui",
    "lang/en/hotel",
    "lang/en/FtpServer",
    "lang/en/IPMac",
    "lang/en/Link",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/HomePage",
    "lang/zhcn/Menu",
    "lang/zhcn/common",
    "lang/zhcn/SoftwareUpdate",
    "lang/zhcn/PdbUpdata",
    "lang/zhcn/InterfaceFlow",
    "lang/zhcn/JS",
    "lang/zhcn/left",
    "lang/zhcn/SmartQos",
    "lang/zhcn/SmartQosSui",
    "lang/zhcn/NetShareSui",
    "lang/zhcn/WANConfig",
    "lang/zhcn/WANConfigSui",
    "lang/zhcn/WirelessSui",
    "lang/zhcn/DHCP",
    "lang/zhcn/DDNS",
    "lang/zhcn/SysInfo",
    "lang/zhcn/WlanSecurity",
    "lang/zhcn/WlanBase",
    "lang/zhcn/LANConfig",
    "lang/zhcn/UPnP",
    "lang/zhcn/commonWords",
    "lang/zhcn/Group",
    "lang/zhcn/GroupSui",
    "lang/zhcn/ParentsControlSui",
    "lang/zhcn/DnsFilter",
    "lang/zhcn/WlanMacFilter",
    "lang/zhcn/serviceZone",
    "lang/zhcn/NatStatic",
    "lang/zhcn/DMZ",
    "lang/zhcn/UserSui",
    "lang/zhcn/RunStatus",
    "lang/zhcn/RunStatus_Sui",
    "lang/zhcn/MacFilterSui",
    "lang/zhcn/TimeWireless",
    "lang/zhcn/SysClock",
    "lang/zhcn/Remote",
    "lang/zhcn/User_edit",
    "lang/zhcn/DeviceConfig",
    "lang/zhcn/DeviceConfigSui",
    "lang/zhcn/Diag",
    "lang/zhcn/advideo",
    "lang/zhcn/advideoSui",
    "lang/zhcn/hotel",
    "lang/zhcn/FtpServer",
    "lang/zhcn/IPMac",
    "lang/zhcn/Link",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/HomePage",
    "lang/zhtw/Menu",
    "lang/zhtw/common",
    "lang/zhtw/SoftwareUpdate",
    "lang/zhtw/PdbUpdata",
    "lang/zhtw/InterfaceFlow",
    "lang/zhtw/JS",
    "lang/zhtw/left",
    "lang/zhtw/SmartQos",
    "lang/zhtw/SmartQosSui",
    "lang/zhtw/NetShareSui",
    "lang/zhtw/WANConfig",
    "lang/zhtw/WANConfigSui",
    "lang/zhtw/WirelessSui",
    "lang/zhtw/DHCP",
    "lang/zhtw/DDNS",
    "lang/zhtw/SysInfo",
    "lang/zhtw/WlanSecurity",
    "lang/zhtw/WlanBase",
    "lang/zhtw/LANConfig",
    "lang/zhtw/UPnP",
    "lang/zhtw/commonWords",
    "lang/zhtw/Group",
    "lang/zhtw/GroupSui",
    "lang/zhtw/ParentsControlSui",
    "lang/zhtw/DnsFilter",
    "lang/zhtw/WlanMacFilter",
    "lang/zhtw/NatStatic",
    "lang/zhtw/serviceZone",
    "lang/zhtw/DMZ",
    "lang/zhtw/UserSui",
    "lang/zhtw/RunStatus",
    "lang/zhtw/RunStatus_Sui",
    "lang/zhtw/MacFilterSui",
    "lang/zhtw/TimeWireless",
    "lang/zhtw/SysClock",
    "lang/zhtw/Remote",
    "lang/zhtw/User_edit",
    "lang/zhtw/DeviceConfig",
    "lang/zhtw/DeviceConfigSui",
    "lang/zhtw/Diag",
    "lang/zhtw/advideo",
   "lang/zhtw/advideoSui",
    "lang/zhtw/hotel",
    "lang/zhtw/IPMac",
    "lang/zhtw/Link",
#endif
    "other/bwm-graph.svg",

#else
/*else of UTT_SMART_UI*/

	"_list1.htm",
	"_listhead1.htm",
	"assist.asp",
	"top.asp",
	"bottom.asp",
	"update.asp",
	"framePage.asp",
	"ReBoot.asp",
	"reload.asp",
	"index.asp",
    "FastConfig.asp",
	"FastConfig_Net.asp",
	"FastConfig_Net2.asp",
    "RunStatus.asp",
    "InterfaceFlow.asp",
    "WANConfig.asp",
	"LANConfig.asp",
#if (IPV6_INTERFACE == FYES)
        "ipv6LANConfig.asp",
        "ipv6WANConfig.asp",
#endif

#if (IPV6_RADVD == FYES)
        "ipv6PrefixList.asp",
        "ipv6PrefixList_edit.asp",
        "radvd.asp",
#endif
#if (IPV6_DHCPV6 == FYES)
        "DHCPv6.asp",
#endif
#if (IPV6_ROUTE == FYES)
        "ipv6Route.asp",
        "ipv6Route_edit.asp",
#endif
#if (IPV6_TUNNEL == FYES)
        "tunnel.asp",
#endif
 #if (IPV6_ISATAP == FYES)
        "isatap.asp",
#endif
	"SaleService.asp",
	"DoubleRoad.asp",
	"DoubleRoadInfo.asp",
	"DoubleRoadInfo_edit.asp",
	"secondTop.asp",
	"SoftwareUpdate.asp",
	"SysClock.asp",
	"SysInfo.asp",
	"left.asp",
	"langChange.asp",
	"redirect.asp",
	"css/style.css",
	"css/uttwlan.css",
	"images/body_main.png",
	"images/bottom.png",
	"images/close.gif",
	"images/delete.gif",
	"images/dot.gif",
	"images/edit_1.gif",
	"images/edit.gif",
	"images/err.gif",
	"images/label_l.gif",
	"images/label_r.gif",
	"images/left.gif",
	"images/line.gif",
	"images/lock.gif",
	"images/logo.png",
	"images/qt.gif",
	"images/remark_1.gif",
	"images/remark_2.gif",
	"images/serline.gif",
	"images/space.gif",
	"images/signal.gif",
	"images/tablehead.gif",
	"images/table_main.png",
	"images/tag1.gif",
	"images/tag.gif",
	"images/three_l.gif",
	"images/three_r.gif",
	"images/topbg.gif",
	"images/top_l.gif",
	"images/top_r.gif",
	"other/bwm-graph.svg",
	"other/pieAct.svg",
	"other/pieFlow.svg",
	"js/bwm-common.js",
	"js/common.js",
	"js/sms_send.js",
	"js/commonSkipAuth.js",
	"js/uttwlan.js",
	"js/pie.js",
	"js/dyn_config.js",
	"js/config.js",
    "lang/b28n.js",
	"advideo.asp",
#if (SNMP == FYES)
    "Snmp.asp",
#endif
#if (FEATURE_AC == FYES)
    "js/jquery.min.js",
    "js/highcharts.js",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"enrhelpinfo.htm",
	"images/en_Forum.png",
	"images/en_guanggao.png",
	"images/en_Knowledge.png",
	"images/en_Reservation.png",
	"images/en_seruttcare.png",
	"images/en_start.png",
    "lang/en/common",
    "lang/en/top",
    "lang/en/bottom",
    "lang/en/Link",
    "lang/en/ReBoot",
    "lang/en/SoftwareUpdate",
    "lang/en/SaleService",
    "lang/en/DoubleRoad",
    "lang/en/DoubleRoadInfo",
    "lang/en/commonWords",
    "lang/en/JS",
    "lang/en/PdbUpdata",
    "lang/en/FastConfig",
    "lang/en/FastConfig_Net",
    "lang/en/RunStatus",
    "lang/en/InterfaceFlow",
    "lang/en/WANConfig",
    "lang/en/LANConfig",
	"lang/en/langChange",
	"lang/en/SysClock",
	"lang/en/SysInfo",
	"lang/en/left",
    "lang/en/list",
    "lang/en/SNMP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"rhelpinfo.htm",
	"images/guanggao.png",
	"images/serbbs.png",
	"images/serbook.png",
	"images/serlib.png",
	"images/seruttcare.png",
	"images/start.gif",
    "lang/zhcn/common",
    "lang/zhcn/top",
    "lang/zhcn/bottom",
    "lang/zhcn/Link",
    "lang/zhcn/ReBoot",
    "lang/zhcn/SoftwareUpdate",
    "lang/zhcn/SaleService",
    "lang/zhcn/DoubleRoad",
    "lang/zhcn/DoubleRoadInfo",
    "lang/zhcn/commonWords",
    "lang/zhcn/JS",
    "lang/zhcn/PdbUpdata",
    "lang/zhcn/FastConfig",
    "lang/zhcn/FastConfig_Net",
    "lang/zhcn/RunStatus",
    "lang/zhcn/InterfaceFlow",
    "lang/zhcn/WANConfig",
    "lang/zhcn/LANConfig",
#if (IPV6_INTERFACE == FYES)
    "lang/zhcn/ipv6LANConfig",
    "lang/zhcn/ipv6WANConfig",
    "lang/zhcn/radvd",
    "lang/zhcn/DHCPv6",
    "lang/zhcn/ipv6Route",
    "lang/zhcn/tunnel",
    "lang/zhcn/isatap",
#endif
	"lang/zhcn/langChange",
	"lang/zhcn/SysClock",
	"lang/zhcn/SysInfo",
	"lang/zhcn/left",
    "lang/zhcn/list",
    "lang/zhcn/advideo",
    "lang/zhcn/SNMP",
#if (FEATURE_AC == FYES)
    "lang/zhcn/ap_sys_info",
#endif
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"zhtwrhelpinfo.htm",
	"images/zhtwguanggao.png",
	"images/zhtwserbbs.png",
	"images/zhtwserbook.png",
	"images/zhtwserlib.png",
	"images/zhtwseruttcare.png",
	"images/zhtwstart.png",
    "lang/zhtw/common",
    "lang/zhtw/top",
    "lang/zhtw/bottom",
    "lang/zhtw/Link",
    "lang/zhtw/ReBoot",
    "lang/zhtw/SoftwareUpdate",
    "lang/zhtw/SaleService",
    "lang/zhtw/DoubleRoad",
    "lang/zhtw/DoubleRoadInfo",
    "lang/zhtw/commonWords",
    "lang/zhtw/JS",
    "lang/zhtw/PdbUpdata",
    "lang/zhtw/FastConfig",
    "lang/zhtw/FastConfig_Net",
    "lang/zhtw/RunStatus",
    "lang/zhtw/InterfaceFlow",
    "lang/zhtw/WANConfig",
    "lang/zhtw/LANConfig",
	"lang/zhtw/langChange",
	"lang/zhtw/SysClock",
	"lang/zhtw/SysInfo",
	"lang/zhtw/left",
    "lang/zhtw/list",
    "lang/zhtw/SNMP",
#endif

#if (HTTP_CACHE == FYES)
	"cachePage.asp",
	"other/cloudFlowBar.svg",
	"other/dirUtilizationPie.svg",
	"js/cloudFlowBar.js",
	"js/dirUtilizationPie.js",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/cache",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/cache",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/cache",
#endif
#endif

#if(NET_MODE_SWITCH == FYES)
#if 0
	"netModeSwitch.asp",
	"netModeReload.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/netMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/netMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/netMode",
#endif
#endif
#endif

#if (BRIDGE == FYES)
//	"bridge.asp",
	"netConfig.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
//    "lang/en/bridge",
    "lang/en/netConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
//    "lang/zhcn/bridge",
    "lang/zhcn/netConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
//    "lang/zhtw/bridge",
    "lang/zhtw/netConfig",
#endif
#endif

#if(CACHE_SERVER == FYES)
	"CacheServerConfig.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/CacheConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/CacheConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/CacheConfig",
#endif
#endif

#if (SETTINGS_CONF == FYES)
    "DeviceConfig.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DeviceConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DeviceConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DeviceConfig",
#endif
#endif

#if (ANTI_SNIFFER == FYES)
    "AntiNetSnifer.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/AntiNetSnifer",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/AntiNetSnifer",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/AntiNetSnifer",
#endif
#endif

#if (NET_SECURITY == FYES)
	"FwBase.asp",
	"FwBaseOut.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FwBase",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FwBase",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FwBase",
#endif
#endif

#if ((SMART_QOS_FUN == FYES) || (GAME_BOOST == FYES))
    "SmartQos.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/SmartQos",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/SmartQos",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/SmartQos",
#endif
#endif


#if (CWMP == FYES)
    "rac.asp",
#endif

#if (WEB_AUTH == FYES)
    "WebAuthGlobal.asp",
    "WebAuthServer.asp",
    "WebAuth_auth.asp",
    "WebAuth_message.asp",
    "WebAuth_edit.asp",
    "WebAuthStatus.asp",
#if (WEBAUTH_AUTO == FYES)
    "images/success.png",
    "images/account.png",
    "images/phone.png",
    "images/weixin.png",
    "WebAuth_auth_auto.asp",
    "WebAuth_message_auto.asp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/WebAuth",
    "lang/en/WebAuth_auth",
    "lang/en/WebAuth_message",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/WebAuth",
    "lang/zhcn/WebAuth_auth",
    "lang/zhcn/WebAuth_message",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/WebAuth",
    "lang/zhtw/WebAuth_auth",
    "lang/zhtw/WebAuth_message",
#endif
#endif



#if (WEBAUTH_SELF_HELP == FYES)
	"waUsers.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/waUsers",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/waUsers",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/waUsers",
#endif
#endif

#if (WIFIDOG_AUTH == FYES)
#if (WIFIDOG_NOTICE == FYES)
    "NoticeWifi.htm",
#endif
#endif


#if (POLICY_ROUTE == FYES)
    "PolicyRoute.asp",
    "PolicyRoute_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/PolicyRoute",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/PolicyRoute",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/PolicyRoute",
#endif
#endif


#if (FAT_FIT_SWITCH == FYES)
    "fatFitSwitch.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/fatFitSwitch",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/fatFitSwitch",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/fatFitSwitch",
#endif
#endif

#if (WIRELESS_BASE_CONFIG == FYES)
    "WlanBase.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/WlanBase",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/WlanBase",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/WlanBase",
#endif
#endif

#if(UPNP_GLOBAL_ENABLE == FYES)
    "UPnP.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/UPnP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/UPnP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/UPnP",
#endif
#endif

#if (IP_GRP == FYES)
    "UserGroup.asp",
    "UserGroup_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/UserGroup",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/UserGroup",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/UserGroup",
#endif
#endif

#if (PORT_GRP == FYES)
    "FwService.asp",
    "FwService_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FwService",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FwService",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FwService",
#endif
#endif

#if (TIME_GRP == FYES)
    "TimeRange.asp",
    "TimeRange_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/TimeRange",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/TimeRange",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/TimeRange",
#endif
#endif


#if (IP_SEC == FYES)
    "IPSec.asp",
    "IPSec_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/IPSec",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/IPSec",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/IPSec",
#endif
#endif


#if (ARP_BINDS == FYES)
    "IPMac.asp",
    "IPMacConfig.asp",
    "IPMac_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/IPMac",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/IPMac",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/IPMac",
#endif
#endif


#if (DHCP_SERVER == FYES)
    "DhcpServer.asp",
    "DhcpFix_edit.asp",
    "DhcpFix.asp",
#if	(DHCP_AUTO_BIND == FYES)
    "DhcpAutoBind.asp",
#endif
	"DhcpClient.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DHCP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DHCP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DHCP",
#endif
#endif


#if (DHCP_POOL == FYES)
    "DhcpServerForAc.asp",
    "DhcpPoolForAc.asp",
    "DhcpFixForAc.asp",
    "DhcpFix_editForAc.asp",
    "DhcpClientForAc.asp",
	"vlanConfig.asp",
	"vlanEdit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DhcpClient",
    "lang/en/DhcpFix",
    "lang/en/DhcpFix_edit",
    "lang/en/DhcpPool",
    "lang/en/DhcpServer",
    "lang/en/vlanConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DhcpClient",
    "lang/zhcn/DhcpFix",
    "lang/zhcn/DhcpFix_edit",
    "lang/zhcn/DhcpPool",
    "lang/zhcn/DhcpServer",
    "lang/zhcn/vlanConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DhcpClient",
    "lang/zhtw/DhcpFix",
    "lang/zhtw/DhcpFix_edit",
    "lang/zhtw/DhcpPool",
    "lang/zhtw/DhcpServer",
    "lang/zhtw/vlanConfig",
#endif
#endif


#if (UTTPDB == FYES)
	"PdbUpdata.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/PdbUpdata",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/PdbUpdata",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/PdbUpdata",
#endif
#endif

#if (LOG_MANAGE == FYES)
	"LogManage.asp",
	"NetActionAudit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/LogManage",
    "lang/en/NetActionAudit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/LogManage",
    "lang/zhcn/NetActionAudit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/LogManage",
    "lang/zhtw/NetActionAudit",
#endif
#endif

#if (USER_STATUS == FYES)
	"UserLinkRate.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/UserLinkRate",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/UserLinkRate",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/UserLinkRate",
#endif
#endif

#if (GROUP_BASE == FYES)
	"Group_edit.asp",
	"GlobalBase_edit.asp",
	"Group.asp",
	"GlobalBase.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/Group",
    "lang/en/GlobalBase_edit",
    "lang/en/GlobalBase",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/Group",
    "lang/zhcn/GlobalBase_edit",
    "lang/zhcn/GlobalBase",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/Group",
    "lang/zhtw/GlobalBase_edit",
    "lang/zhtw/GlobalBase",
#endif
#endif


#if (EXCEPT_QQ == FYES)
	"QQFilter.asp",
	"QQFilterImport.asp",
	"QQFilter_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/QQFilter",
    "lang/en/QQFilterImport",
    "lang/en/PPPoEServer",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/QQFilter",
    "lang/zhcn/QQFilterImport",
    "lang/zhcn/PPPoEServer",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/QQFilter",
    "lang/zhtw/QQFilterImport",
    "lang/zhtw/PPPoEServer",
#endif
#endif


#if (EXCEPT_MSN == FYES)
	"MSNFilter.asp",
	"MSNFilter_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/MSNFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/MSNFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/MSNFilter",
#endif
#endif

#if (EXCEPT_ALI == FYES)
	"AlitalkFilter.asp",
	"AlitalkFilter_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/AlitalkFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/AlitalkFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/AlitalkFilter",
#endif
#endif

#if (NET_SNIPER == FYES)
	"NetSniper.asp",
	"NetSniperConf.asp",
	"NetSniperList.asp",
	"NetSniperNotice.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	"lang/en/NetSniper",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
	"lang/zhcn/NetSniper",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
	"lang/zhtw/NetSniper",
#endif
#endif

#if 1
#if (L2TP_SERVER == FYES || L2TP_CLIENT == FYES)
	"L2TPSrvAccount.asp",
	"L2TPSrvGlobal.asp",
	"L2TPList.asp",
	"L2TPClient.asp",
#endif
#endif

#if (STATIC_NAT == FYES)
	"NatStatic_edit.asp",
	"NatStatic.asp",
	"Remote.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/NatStatic",
    "lang/en/Remote",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/NatStatic",
    "lang/zhcn/Remote",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/NatStatic",
    "lang/zhtw/Remote",
#endif
#endif

#if (NAT_RULE == FYES)
	"NatRule.asp",
	"NatRule_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/NatRule",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/NatRule",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/NatRule",
#endif
#endif


#if (DMZ == FYES)
	"DMZ.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DMZ",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DMZ",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DMZ",
#endif
#endif

#if (FIREWALL == FYES)
	"FireWall.asp",
	"FireWall_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FireWall",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FireWall",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FireWall",
#endif
#endif


#if	((PPTP_SERVER == FYES) && (PPTP_CLIENT == FYES))
	"PPTPSrvAccount.asp",
	"PPTPClientList.asp",
	"PPTPClient.asp",
	"PPTPSrvGlobal.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/PPTP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/PPTP",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/PPTP",
#endif
#endif


#if (STATIC_ROUTE == FYES)
	"Route.asp",
	"Route_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/Route",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/Route",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/Route",
#endif
#endif


#if (HOTEL_PNP == FYES)
	"hotel.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/hotel",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/hotel",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/hotel",
#endif
#endif


#if (DDNS == FYES)
#if (DDNS_ADVANCE == FYES)
	"DDNSAdvance.asp",
	"DDNS_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DDNSAdvance",
    "lang/en/DDNS_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DDNSAdvance",
    "lang/zhcn/DDNS_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DDNSAdvance",
    "lang/zhtw/DDNS_edit",
#endif
#else
	"DDNS.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DDNS",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DDNS",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DDNS",
#endif
#endif
#endif


#if (PPPOE_SERVER == FYES)
	"PPPoEConSta.asp",
	"PPPoEGlobal.asp",
	"PPPoE_edit.asp",
	"PPPoEImport.asp",
	"PPPoEExport.asp",
	"PPPoEServer.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/PPPoEConSta",
    "lang/en/PPPoEImport",
    "lang/en/PPPoEGlobal",
    "lang/en/PPPoEServer",
    "lang/en/PPPoE_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/PPPoEConSta",
    "lang/zhcn/PPPoEImport",
    "lang/zhcn/PPPoEGlobal",
    "lang/zhcn/PPPoEServer",
    "lang/zhcn/PPPoE_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/PPPoEConSta",
    "lang/zhtw/PPPoEImport",
    "lang/zhtw/PPPoEGlobal",
    "lang/zhtw/PPPoEServer",
#endif
#endif


#if (PPPOE_SELF_HELP == FYES)
	"poeUsers.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/poeUsers",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/poeUsers",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/poeUsers",
#endif
#endif


#if (ADMIN_CONF == FYES)
	"User.asp",
	"User_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/User",
    "lang/en/User_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/User",
    "lang/zhcn/User_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/User",
    "lang/zhtw/User_edit",
#endif
#endif


#if (DNS_FILTER == FYES)
	"DnsFilter.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DnsFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DnsFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DnsFilter",
#endif
    "DnsNotify.htm",
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    "uttCli.asp",
#endif

#if (PORT_MIRROR_6530G == FYES)
	"PortMirror6530G.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/PortMirror",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/PortMirror",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/PortMirror",
#endif
#endif

#if (PORT_MIRROR == FYES)
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8367RB)
	"PortMirror8367M.asp",
#else
	"PortMirror.asp",
#endif
#if ((UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES))
	"PortMirror840E.asp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/PortMirror",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/PortMirror",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/PortMirror",
#endif
#if (CONFIG_RALINK_MT7620 == FYES || CONFIG_RALINK_MT7621 == FYES)
            "PortMirror7620e.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
	        "lang/en/PortMirror",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
		    "lang/zhcn/PortMirror",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
		        "lang/zhtw/PortMirror",
#endif
#endif
#endif

/* CONFIG_SWITCH_EXT*/
#if defined (CONFIG_SWITCH_EXT)
	"cgi-bin/exportLinkageConf.cgi",
	"switch/js/topo.js",
	"switch/js/common_switch.js",
	"switch/_list1.htm ",
	"switch/_listhead2.htm",
	"switch/other/tu.svg",
	"switch/images/device_none.png",
	"switch/images/jack_copper_disabled_bottom.png",
	"switch/images/jack_copper_disabled_top.png",
	"switch/images/jack_copper_down_bottom.png",
	"switch/images/jack_copper_down_top.png",
	"switch/images/jack_copper_link_bottom.png",
	"switch/images/jack_copper_link_top.png",
	"switch/images/jack_sfp_disabled_top.png",
	"switch/images/jack_sfp_down_top.png",
	"switch/images/jack_sfp_link_top.png",
	"switch/images/router_normal.png",
	"switch/images/switch_normal.png",
	"switch/images/switch_warning.png",
	"switch/discover.asp",
	"switch/linkageConf.asp",
	"switch/linkageConfSend.asp",
	"switch/remote_manage.asp",
	"switch/remote_manage_group.asp",
	"switch/stat_detailed.asp",
	"switch/sysConf.asp",
	"switch/sysInfo.asp",
	"switch/topoy.asp",
	"switch/wait_remote_auth.asp",
	"switch/wait_remote_conf.asp",
	"switch/wait_rrcp.asp",
	"switch/wait_topoy.asp",
#if (FEATURE_SW_PORT_CONF == FYES)
	"switch/ports.asp",
#endif

#if (FEATURE_PORT_VLAN == FYES)
	"switch/port_vlan.asp",
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
	"switch/mirror.asp",
#endif

#if (FEATURE_SW_PORT_TRUNK == FYES)
#if defined (CONFIG_UTT_NV1540)
	"switch/aggregation_100M.asp",
#else
	"switch/aggregation.asp",
#endif
#endif

#if(FEATURE_PORT_STATISTICS == FYES)
	"switch/stat_detailed.asp",
#endif
#endif

#if (TASK_SCHEDULER == FYES)
	"TaskScheduler_edit.asp",
	"TaskScheduler.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/TaskScheduler",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/TaskScheduler",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/TaskScheduler",
#endif
#endif


#if (NOTICE == FYES)
	"Notice.asp",
	"PPPoENotice.asp",
	"DnsFilter_Notice.asp",
	"Notify.asp",
	"Notify.htm",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/Notice",
	"lang/en/DnsNotice",
	"lang/en/DnsFilter",
	"lang/en/PPPoENotice",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/Notice",
	"lang/zhcn/DnsNotice",
	"lang/zhcn/DnsFilter",
	"lang/zhcn/PPPoENotice",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/Notice",
	"lang/zhtw/DnsNotice",
	"lang/zhtw/DnsFilter",
	"lang/zhtw/PPPoENotice",
#endif
#endif


#if (CONNECT_LIMIT == FYES)
	"ConnectionLimit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/ConnectLimit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/ConnectLimit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/ConnectLimit",
#endif
#endif


#if (PORT_CONF_REDUCED == FYES)
	"portSpeed.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/portSpeed",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/portSpeed",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/portSpeed",
#endif
#endif


#if (PVLAN_REDUCED == FYES)/*精简版port vlan*/
	"port_vlan_rdd.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/port_vlan_rdd",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/port_vlan_rdd",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/port_vlan_rdd",
#endif
#endif


#if (SYSLOG == FYES)
	"Syslog.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/Syslog",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/Syslog",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/Syslog",
#endif
#endif

#if ((SSH_CLI == FYES)||(HTTPS == FYES))
	"DeviceLoginControl.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DeviceLoginControl",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DeviceLoginControl",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DeviceLoginControl",
#endif
#endif

#if(UTT_LICENSE == FYES)
	"License.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/License",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/License",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/License",
#endif
#endif

#if (USER_DEF_WAN_COUNT == FYES)
	"WANportnumber.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/WANportnumber",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/WANportnumber",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/WANportnumber",
#endif
#endif

#if(IDENTIFY_BIND == FYES)
	"IdentifyBind.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/IdentifyBind",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/IdentifyBind",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/IdentifyBind",
#endif
#endif

#if (P2P_LIMIT == FYES)
	"P2PLimit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/P2PLimit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/P2PLimit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/P2PLimit",
#endif
#endif

#if(BRIDGE == FYES)
	"bridge.asp",
#endif

#if (NET_MODE_SWITCH== FYES)
	"netModeSwitch.asp",
	"netModeReload.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/netMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/netMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/netMode",
#endif
#endif

#if(WIRELESS_BASE_CONFIG == FYES)
	"WlanBase.asp",
	"WlanBase_old.asp",
	"WlanBase_list_wait_apScan.asp",
	"WlanBase_list_apScan.asp",
	"FastConfig_Net_WLAN.asp",
	"FastConfig_Wlan.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/WlanBase",
    "lang/en/FastConfig_Wlan",
	"lang/en/FastConfig_Net_WLAN",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/WlanBase",
    "lang/zhcn/FastConfig_Wlan",
	"lang/zhcn/FastConfig_Net_WLAN",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/WlanBase",
    "lang/zhtw/FastConfig_Wlan",
	"lang/zhtw/FastConfig_Net_WLAN",
#endif
#endif

#if(WIRELESS_5G == FYES)
	"WlanBase_5g.asp",
	"WlanBase_list.asp",
	"WlanBase_list_wait.asp",
	"WlanAdvanced_5g.asp",
	"WlanHostInfo_5g.asp",
	"WlanMacFilter_5g.asp",
	"WlanMacFilter_edit_5g.asp",
	"WlanSecurity_5g.asp",
	"FastConfig_Wlan_5g.asp",
#endif

#if(WIRELESS == FYES)
	"WlanAdvanced.asp",
	"WlanHostInfo.asp",
	"WlanMacFilter.asp",
	"WlanMacFilter_edit.asp",
	"WlanSecurity.asp",

#if (defined (CONFIG_UTT_NV520W) || defined (CONFIG_UTT_U1000N)|| defined (CONFIG_UTT_NV512W)|| defined (CONFIG_UTT_NV512WV2)|| defined (CONFIG_UTT_NV518W)|| defined (CONFIG_UTT_NV518WVPN)|| defined (CONFIG_UTT_NV842W) || defined (CONFIG_UTT_NV520WV3) || defined (CONFIG_UTT_NV842WV3) || defined (CONFIG_UTT_NV512WV3) || defined (CONFIG_UTT_NV518WV3) || defined (CONFIG_UTT_NV518WV2) || defined (CONFIG_UTT_NV515W) || defined(CONFIG_UTT_NVD908W))
	"FastConfig_OpMode_Ap.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FastConfig_OpMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FastConfig_OpMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FastConfig_OpMode",
#endif
#else
	"FastConfig_OpMode.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FastConfig_OpMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FastConfig_OpMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FastConfig_OpMode",
#endif
#endif

#if (WAN_3G_SWITCH  == FYES )
	"FastConfig_Net_3G.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FastConfig_Net_3G",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FastConfig_Net_3G",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FastConfig_Net_3G",
#endif
#endif

#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/WlanAdvanced",
    "lang/en/WlanHostInfo",
    "lang/en/WlanMacFilter",
    "lang/en/WlanSecurity",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/WlanAdvanced",
    "lang/zhcn/WlanHostInfo",
    "lang/zhcn/WlanMacFilter",
    "lang/zhcn/WlanSecurity",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/WlanAdvanced",
    "lang/zhtw/WlanHostInfo",
    "lang/zhtw/WlanMacFilter",
    "lang/zhtw/WlanSecurity",
#endif
#endif

#if (HARDWARE_NAT == FYES)
	"HardwareNAT.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/HardwareNAT",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/HardwareNAT",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/HardwareNAT",
#endif
#endif

#if (FEATURE_AP == FYES)
#if (FEATURE_AP_BASIC_INFO == FYES)
	"fitApSysInfo.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/fitAPSysInfo",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/fitAPSysInfo",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/fitAPSysInfo",
#endif
#endif
#if 0
#if (FEATURE_AP_EFFECT_CONF == FYES)
	"apParaSwitch.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apParaSwitch",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apParaSwitch",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apParaSwitch",
#endif
#endif

#if(FIT_AP_LOCAL_CONF_TEMP == FYES)
	"wlanPara.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/wlanPara",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/wlanPara",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/wlanPara",
#endif
#endif
#endif
#if (FAT_FIT_SWITCH == FYES)
	"fatFitSwitch.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/fatFitSwitch",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/fatFitSwitch",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/fatFitSwitch",
#endif
#endif

	"apSwitching.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apSwitching",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apSwitching",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apSwitching",
#endif

	"fitApClts.asp",
	"fitApIfConf.asp",
	"fitApReload.asp",
	"FitApHelpInfo.htm",
	"js/fit_config.js",

#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/fitApClts",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/fitApClts",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/fitApClts",
#endif
#endif

#if (DNS_AGENCY == FYES)
	"DnsProxy.asp",
	"DnsProxyForAc.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/DnsProxy",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/DnsProxy",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/DnsProxy",
#endif
#endif

#if (FEATURE_AC == FYES)
	"ap_wait_rrcp.asp",
	"apMngRecord.asp",
	"apManage.asp",
	"apDiscover.asp",
	"wlanCltList.asp",
    "WlanApClientsStatistics.asp",
	"apSwitching.asp",
	"ap_wait_remote_conf.asp",
	"pushSoft.asp",
    "switch/js/common_switch.js",
	"FastConfig_acWorkMode.asp",
	"FastConfig_Net_WAN1.asp",
	"FastConfig_edit_Vlan.asp",
	"FastConfig_edit_DhcpPool.asp",
	"FastConfig_edit_SZ.asp",
	"images/2.png",
	"images/3.png",
	"images/4.png",
	"images/5.png",
	"images/setting.png",
	"images/reboot.png",
	"images/position.png",
	"WlanSSIDClientsStatistics.asp",
        "WlanHistoryClientsStatistics.asp",
	"firstPage.asp",
	"firstWlanSSIDClts.asp",
	"firstWlanHistoryClts.asp",
	"firstSysInfo.asp",
	"firstWlanCltList.asp",
	"firstApLog.asp",
	"system_summary.asp",
	"ap_rf.asp",
	"ap_rf_5G.asp",
	"ap_information.asp",
	"historyFlowStatistics.asp",
	"wlanCltMacFilter.asp",
	"sta_history.asp",
	"images/apico.png",
	"images/banner.png",
	"images/freshico.png",
	"images/userico.png",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apManage",
    "lang/en/apDiscover",
    "lang/en/wlanCltList",
    "lang/en/apSwitching",
    "lang/en/pushSoft",
    "lang/en/ap_wait_remote_conf",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apManage",
    "lang/zhcn/apDiscover",
    "lang/zhcn/wlanCltList",
    "lang/zhcn/apSwitching",
    "lang/zhcn/pushSoft",
    "lang/zhcn/ap_wait_remote_conf",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apManage",
    "lang/zhtw/apDiscover",
    "lang/zhtw/wlanCltList",
    "lang/zhtw/apSwitching",
    "lang/zhtw/pushSoft",
    "lang/zhtw/ap_wait_remote_conf",
#endif
#if (FEATURE_AP_MAIL == FYES)
	"apMailConfig.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apMailConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apMailConfig",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apMailConfig",
#endif
#endif

#endif

#if (AC_WORK_MODE == FYES)
	"acMode.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/acMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/acMode",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/acMode",
#endif
#endif

#if (AP_LOAD_BALANCE == FYES)
	"apLoadBalance_edit.asp",
	"apLoadBalanceList.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apLoadBalance",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apLoadBalance",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apLoadBalance",
#endif
#endif

#if (AP_LOG_MANAGE == FYES)
	"apLog.asp",
	"apLogManage.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/NetActionAudit",
    "lang/en/apLogManage",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/NetActionAudit",
    "lang/zhcn/apLogManage",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/NetActionAudit",
    "lang/zhtw/apLogManage",
#endif
#endif


#if (AP_PERMISSION == FYES)
	"apPermission.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apPermission",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apPermission",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apPermission",
#endif
#endif

#if (AP_DRAWING == FYES)
	"DrawingPreview.asp",
	"ImportDrawing.asp",
	"images/unknow.png",
	"images/normal.png",
	"images/offline.png",
	"images/DPpush.png",
	"images/DPpush2.png",
	"images/DPmanage.png",
	"images/DPstatus.png",
	"images/DPdelete.png",
	"images/DPscroll.png",
	"images/DPscrollbt.png",
	"js/contextMenu.js",
	"default.jpg",
#endif

#if (SERVICE_ZONE == FYES)
	"serviceZone.asp",
	"edit_SZ.asp",
#if SZONE_AUTO_UPDATA
    "SzoneAutoUpdata.asp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/serviceZone",
	"lang/en/edit_SZ",
#if SZONE_AUTO_UPDATA
	"lang/en/SzoneAutoUpdata",
#endif
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/serviceZone",
	"lang/zhcn/edit_SZ",
#if SZONE_AUTO_UPDATA
	"lang/zhcn/SzoneAutoUpdata",
#endif
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/serviceZone",
	"lang/zhtw/edit_SZ",
#if SZONE_AUTO_UPDATA
	"lang/zhtw/SzoneAutoUpdata",
#endif
#endif
#endif

#if(FEATURE_AP_CONF_TEMP == FYES)
	"apTmp.asp",
	"editTemp.asp",
	"pushTmp.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apTmp",
    "lang/en/pushTmp",
	"lang/en/editTemp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apTmp",
    "lang/zhcn/pushTmp",
	"lang/zhcn/editTemp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apTmp",
    "lang/zhtw/pushTmp",
	"lang/zhtw/editTemp",
#endif
#endif

#if (FEATURE_AP_SW_MNG == FYES)
	"wlanUpdateUpload.asp",
	"apSoftMng.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/wlanUpdateUpload",
	"lang/en/apSoftMng",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/wlanUpdateUpload",
	"lang/zhcn/apSoftMng",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/wlanUpdateUpload",
	"lang/zhtw/apSoftMng",
#endif
#endif

#if (defined (SW_FEATURE_URCP) || (FEATURE_AC == FYES))
	"apUpdateConf.asp",
	"apUploadConf.asp",
	"pushConf.asp",
	"ap_wait_remote_auth.asp",
	"ap_remote_manage_group.asp",
	"ap_remote_manage.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/getApConf",
    "lang/en/pushConf",
    "lang/en/ap_wait_remote_auth",
    "lang/en/ap_remote_manage",
    "lang/en/ap_remote_manage_group",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/getApConf",
    "lang/zhcn/pushConf",
    "lang/zhcn/ap_wait_remote_auth",
    "lang/zhcn/ap_remote_manage",
    "lang/zhcn/ap_remote_manage_group",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/getApConf",
    "lang/zhtw/pushConf",
    "lang/zhtw/ap_wait_remote_auth",
    "lang/zhtw/ap_remote_manage",
    "lang/zhtw/ap_remote_manage_group",
#endif
#endif

#if (FEATURE_AP_BASIC_CONF == FYES)
	"apBasicConf.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/apBasicConf",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/apBasicConf",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/apBasicConf",
#endif
#endif

#if (SYS_LOG_INFO == FYES)
	"SysLogInfo.asp",
	"SysLogInfo_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/SysLogInfo",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/SysLogInfo",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/SysLogInfo",
#endif
#endif

#if (PARENTS_CONTROL == FYES)
	"ParentsControl.asp",
	"ParentsControl_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/ParentsControl",
    "lang/en/DnsFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/ParentsControl",
    "lang/zhcn/DnsFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/ParentsControl",
    "lang/zhtw/DnsFilter",
#endif
#endif

#if(EASY_TASK == FYES)
	"EasyManage.asp",
	"EasyManageConfig.asp",
	"EasyManageWelcome.asp",
	"EasyManageSuccess.asp",
	"NoticEasyConfig.asp",
	"images/portdown.gif",
	"images/portup.gif",
	"images/link1.png",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/EasyTask",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/EasyTask",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/EasyTask",
#endif
#endif
#if (SYS_LOG_INFO == FYES)
    "SysLogInfo.asp",
    "SysLogInfo_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/SysLogInfo",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/SysLogInfo",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/SysLogInfo",
#endif
#endif
#if (MAC_FILTER == FYES)
    "MacFilter.asp",
    "MacFilter_edit.asp",
    "MacFilterConfig.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/MacFilter",
#if (WIRELESS != FYES)
    "lang/en/WlanMacFilter",
#endif
#if (ARP_BINDS != FYES)
    "lang/en/IPMac",
#endif
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/MacFilter",
#if (WIRELESS != FYES)
    "lang/zhcn/WlanMacFilter",
#endif
#if (ARP_BINDS != FYES)
    "lang/zhcn/IPMac",
#endif
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/MacFilter",
#if (WIRELESS != FYES)
    "lang/zhtw/WlanMacFilter",
#endif
#if (ARP_BINDS != FYES)
    "lang/zhtw/IPMac",
#endif
#endif
#endif

#if (FEATURE_AP_MAC_FILTER == FYES)
	"FitApMacFilter.asp",
	"FitApMacFilter_edit.asp",
	"pushApMacFilter.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FitApMacFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FitApMacFilter",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/FitApMacFilter",
#endif
#endif

#if (FEATURE_SW_SNMP == FYES)
	"snmpConf.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/snmpConf",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/snmpConf",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/snmpConf",
#endif

#endif

#if (TAG_VLAN == FYES)
    "TagVlanList.asp",
    "TagVlan_edit.asp",
    "vid.asp",
    "vlan_cc.asp",
#if (WAN_TAG_VLAN == FYES)
    "WanTagVlan.asp",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/TagVlan",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/TagVlan",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/TagVlan",
#endif
#endif

#endif/*end of UTT_SMART_UI*/

#if (LOGIN_PAGE_DIY == FYES)
	"login.asp",
#endif

#if (DIAGNOSE_DETECT == FYES)
#if (FEATURE_AC == FYES)
	"diagnose.html",
#else
	"diagnose.asp",
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
    "NetShareManage.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/NetShareManage",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/NetShareManage",
#endif
#endif
#if (FTP_SERVER == FYES)
    "FtpServer.asp",
    "FtpServer_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/FtpServer",
    "lang/en/FtpServer_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/FtpServer",
    "lang/zhcn/FtpServer_edit",
#endif
#endif
#if (NET_SHARE_USER == FYES)
    "ShareAccountList.asp",
    "ShareAccount_edit.asp",
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/ShareAccountList",
    "lang/en/ShareAccount_edit",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/ShareAccountList",
    "lang/zhcn/ShareAccount_edit",
#endif
#endif

//WLAN_MULTI_SSID
#if(WLAN_MULTI_SSID == FYES)
#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/WirelessSui",
#endif 
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/WirelessSui",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/WirelessSui",
#endif

#else
 #if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1)
    "lang/zhcn/_WirelessSui",
#endif 
#if (CONFIG_USER_GOAHEAD_LANG_EN == 1)
    "lang/en/_WirelessSui",
#endif
#if (CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    "lang/zhtw/_WirelessSui",
#endif
   
#endif
    "null"
};


/* ToMAX新页面在web/ToMAX中 */
#if (UTT_TOMAX == FYES)
char *RomWebsToMAX[] = {
    "GlobalBase.asp",
    "TimeRange.asp",
    "FireWall.asp",
    "IPSec.asp",
    "IPSec_edit.asp",
    "WlanHostInfo_5g.asp",
    "SysLogInfo.asp",
    "PPTPSrvGlobal.asp",
    "WlanBase.asp",
    "WlanMultiSsid.asp",
    "SysLogInfo_edit.asp",
    "InterfaceFlow.asp",
    "MSNFilter.asp",
    "Syslog.asp",
    "TimeRange_edit.asp",
    "MacFilterConfig.asp",
    "images/logo.jpg",
    "images/tablehead.gif",
    "images/three_r.gif",
    "images/remark_1.gif",
    "images/edit_1.gif",
    "images/remark_2.gif",
    "images/edit.gif",
    "images/three_l.gif",
    "SysInfo.asp",
    "Notice.asp",
    "AlitalkFilter_edit.asp",
    "PPPoEConSta.asp",
    "Route_edit.asp",
    "IPMacConfig.asp",
    "Route.asp",
    "NatStatic.asp",
    "FastConfig_Wlan.asp",
    "DnsFilter_Notice.asp",
    "WlanMacFilter_5g.asp",
    "WebAuthServer.asp",
    "NatStatic_edit.asp",
    "UserLinkRate.asp",
    "FastConfig.asp",
    "DhcpClient.asp",
    "WebAuthGlobal.asp",
    "hotel.asp",
    "PPPoEServer.asp",
    "FastConfig_Wlan_5g.asp",
    "WlanSecurity_5g.asp",
    "DoubleRoadInfo.asp",
    "frame.asp",
    "DoubleRoad.asp",
    "secondMenu.asp",
    "IdentifyBind.asp",
    "UserGroup_edit.asp",
    "FireWall_edit.asp",
    "NetActionAudit.asp",
    "PPPoE_edit.asp",
    "LANConfig.asp",
    "MSNFilter_edit.asp",
    "SmartQos.asp",
    "PPTPClient.asp",
    "PPTPSrvAccount.asp",
    "PPTPClientList.asp",
    "MacFilter_edit.asp",
    "UPnP.asp",
    "DMZ.asp",
    "LogManage.asp",
    "PPPoEImport.asp",
    "DDNS.asp",
    "DhcpServer.asp",
    "WANConfig.asp",
    "NatRule_edit.asp",
    "DnsFilter.asp",
    "AntiNetSnifer.asp",
    "css/ie7/ie7.js",
    "css/ie7/ie7.css",
    "css/uttwlan.css",
    "css/style.css",
    "css/menu.css",
    "css/fonts/icomoon.woff",
    "css/fonts/icomoon.ttf",
    "css/fonts/icomoon.svg",
    "css/fonts/icomoon.eot",
    "header.asp",
    "UserGroup.asp",
    "AlitalkFilter.asp",
    "menu.asp",
    "ConnectionLimit.asp",
    "PdbUpdata.asp",
    "TaskScheduler.asp",
    "WebAuthStatus.asp",
    "IPMac.asp",
    "FastConfig_acWorkMode.asp",
    "User.asp",
    "NatRule.asp",
    "User_edit.asp",
    "WlanMacFilter.asp",
    "js/jquery.min.js",
    "js/uttwlan.js",
    "js/common.js",
    "js/highcharts.js",
    "js/bwm-common.js",
    "SysClock.asp",
    "WlanHostInfo.asp",
    "DeviceConfig.asp",
    "QQFilter_edit.asp",
    "PolicyRoute_edit.asp",
    "Group_edit.asp",
    "Remote.asp",
    "WlanBase_5g.asp",
    "PPPoEGlobal.asp",
    "L2TPList.asp",
    "QQFilterImport.asp",
    "Group.asp",
    "QQFilter.asp",
    "IPMac_edit.asp",
    "PPPoENotice.asp",
    "SoftwareUpdate.asp",
    "FastConfig_Net.asp",
    "WlanAdvanced.asp",
    "TaskScheduler_edit.asp",
    "PPPoEExport.asp",
    "MacFilter.asp",
    "GlobalBase_edit.asp",
    "WlanAdvanced_5g.asp",
    "DoubleRoadInfo_edit.asp",
    "ReBoot.asp",
    "DhcpAutoBind.asp",
    "WlanSecurity.asp",
    "PolicyRoute.asp",
    "DhcpFix.asp",
    "DhcpFix_edit.asp",
    "GlobalBase.asp",
    "TimeRange.asp",
    "FireWall.asp",
    "IPSec.asp",
    "IPSec_edit.asp",
    "WlanHostInfo_5g.asp",
    "SysLogInfo.asp",
    "PPTPSrvGlobal.asp",
    "WlanBase.asp",
    "WlanMultiSsid.asp",
    "SysLogInfo_edit.asp",
    "InterfaceFlow.asp",
    "MSNFilter.asp",
    "Syslog.asp",
    "TimeRange_edit.asp",
    "MacFilterConfig.asp",
    "images/logo.jpg",
    "images/tablehead.gif",
    "images/three_r.gif",
    "images/remark_1.gif",
    "images/edit_1.gif",
    "images/remark_2.gif",
    "images/edit.gif",
    "images/three_l.gif",
    "SysInfo.asp",
    "Notice.asp",
    "AlitalkFilter_edit.asp",
    "PPPoEConSta.asp",
    "Route_edit.asp",
    "IPMacConfig.asp",
    "Route.asp",
    "NatStatic.asp",
    "FastConfig_Wlan.asp",
    "DnsFilter_Notice.asp",
    "WlanMacFilter_5g.asp",
    "WebAuthServer.asp",
    "NatStatic_edit.asp",
    "UserLinkRate.asp",
    "FastConfig.asp",
    "DhcpClient.asp",
    "WebAuthGlobal.asp",
    "hotel.asp",
    "PPPoEServer.asp",
    "FastConfig_Wlan_5g.asp",
    "WlanSecurity_5g.asp",
    "DoubleRoadInfo.asp",
    "frame.asp",
    "DoubleRoad.asp",
    "secondMenu.asp",
    "IdentifyBind.asp",
    "UserGroup_edit.asp",
    "FireWall_edit.asp",
    "NetActionAudit.asp",
    "PPPoE_edit.asp",
    "LANConfig.asp",
    "MSNFilter_edit.asp",
    "SmartQos.asp",
    "PPTPClient.asp",
    "PPTPSrvAccount.asp",
    "PPTPClientList.asp",
    "MacFilter_edit.asp",
    "UPnP.asp",
    "DMZ.asp",
    "LogManage.asp",
    "PPPoEImport.asp",
    "DDNS.asp",
    "DhcpServer.asp",
    "WANConfig.asp",
    "NatRule_edit.asp",
    "DnsFilter.asp",
    "AntiNetSnifer.asp",
    "css/ie7/ie7.js",
    "css/ie7/ie7.css",
    "css/uttwlan.css",
    "css/style.css",
    "css/menu.css",
    "css/fonts/icomoon.woff",
    "css/fonts/icomoon.ttf",
    "css/fonts/icomoon.svg",
    "css/fonts/icomoon.eot",
    "header.asp",
    "UserGroup.asp",
    "AlitalkFilter.asp",
    "menu.asp",
    "ConnectionLimit.asp",
    "PdbUpdata.asp",
    "TaskScheduler.asp",
    "WebAuthStatus.asp",
    "IPMac.asp",
    "FastConfig_acWorkMode.asp",
    "User.asp",
    "NatRule.asp",
    "User_edit.asp",
    "WlanMacFilter.asp",
    "js/jquery.min.js",
    "js/uttwlan.js",
    "js/common.js",
    "js/highcharts.js",
    "js/bwm-common.js",
    "SysClock.asp",
    "WlanHostInfo.asp",
    "DeviceConfig.asp",
    "QQFilter_edit.asp",
    "PolicyRoute_edit.asp",
    "Group_edit.asp",
    "Remote.asp",
    "WlanBase_5g.asp",
    "PPPoEGlobal.asp",
    "L2TPList.asp",
    "QQFilterImport.asp",
    "Group.asp",
    "QQFilter.asp",
    "IPMac_edit.asp",
    "PPPoENotice.asp",
    "SoftwareUpdate.asp",
    "FastConfig_Net.asp",
    "WlanAdvanced.asp",
    "TaskScheduler_edit.asp",
    "PPPoEExport.asp",
    "MacFilter.asp",
    "GlobalBase_edit.asp",
    "WlanAdvanced_5g.asp",
    "DoubleRoadInfo_edit.asp",
    "ReBoot.asp",
    "DhcpAutoBind.asp",
    "WlanSecurity.asp",
    "PolicyRoute.asp",
    "DhcpFix.asp",
    "portnumber.asp",
    "PortMirror7620e.asp",
    "vlanConfig.asp",
#if (PORT_GRP == FYES)
    "FwService.asp",
    "FwService_edit.asp",
#endif
#if (L2TP_SERVER == FYES || L2TP_CLIENT == FYES)
    "L2TPSrvAccount.asp",
    "L2TPSrvGlobal.asp",
    "L2TPList.asp",
    "L2TPClient.asp",
#endif
#if (NET_SHARE_MANAGE == FYES)
    "NetShareManage.asp",
#endif
#if (FTP_SERVER == FYES)
    "FtpServer.asp",
    "FtpServer_edit.asp",
#endif
#if (NET_SHARE_USER == FYES)
    "ShareAccountList.asp",
    "ShareAccount_edit.asp",
#endif
#if (PORT_MIRROR == FYES)
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8367RB)
    "PortMirror8367M.asp",
#else
    "PortMirror.asp",
#endif
#endif
#if (PVLAN_REDUCED == FYES)/*精简版port vlan*/
    "port_vlan_rdd.asp",
#endif

#if (FEATURE_AP == FYES)
    "js/old_config.js",
    "js/fit_config.js",
    "FitApHelpInfo.htm",
#endif
#if (FEATURE_AC == FYES)
    "firstPage.asp",
    "firstWlanSSIDClts.asp",
    "firstWlanHistoryClts.asp",
    "firstWlanCltList.asp",
    "firstApLog.asp",
    "ap_remote_manage.asp",
    "wlanCltList.asp",
    "DrawingPreview.asp",
    "apSoftMng.asp",
//    "acMode.asp",
    "historyFlowStatistics.asp",
    "ap_remote_manage_group.asp",
    "DhcpPoolForAc.asp",
    "images/reboot.png",
    "images/setting.png",
    "images/position.png",
    "lang/zhcn/ap_sys_info",
    "lang/zhcn/commonWords",
    "lang/zhcn/common",
    "lang/zhcn/FtpServer",
    "lang/zhcn/JS",
    "lang/zhcn/NetShareManage",
    "lang/zhcn/ShareAccount_edit",
    "lang/zhcn/ShareAccountList",
    "js/old_config.js",
#endif
#if ((FEATURE_AP == FYES) || (FEATURE_AC == FYES))
    "lang/zhcn/ap_sys_info",
    "lang/zhcn/commonWords",
    "lang/zhcn/common",
    "lang/zhcn/FtpServer",
    "lang/zhcn/JS",
    "lang/zhcn/NetShareManage",
    "lang/zhcn/ShareAccount_edit",
    "lang/zhcn/ShareAccountList",
    "lang/zhcn/WlanBase",
    "js/config.js",
#endif
    "waUsers.asp", 
    "Notify.asp",
    "Notify.htm",
    "DnsNotify.htm",
    "rhelpinfo.htm",
    "poeUsers.asp",
    "lang/zhcn/common",
    "null"
};
#endif
/********************************************************************
 * 函数： main
 * 功能： 生成页面控制
 * 创建： 2013-03-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int main(int argc, char **argv) 
{
    FILE *fp = NULL;

    fp = fopen("web-config", "w");/*打开文件，不存在则创建*/
    if (fp == NULL) {
	return 0;
    }
    int i = 0;
    //#if (CONFIG_USER_GOAHEAD_LANG_ZHCN == 1||CONFIG_USER_GOAHEAD_LANG_ZHTW == 1)
    //system("cp  ../goahead/web/css/style_new_zh.css  ../goahead/web/css/style_new.css ");
    //#else
   // system("cp  ../goahead/web/css/style_new_en.css  ../goahead/web/css/style_new.css ");
   // #endif 
    fputs("PAGE_NAME_TYPE = "PAGE_NAME_TYPE"\n",fp );
#if (VPN_PROXY == FYES)
    fputs("CONFIG_UTT_VPN_PROXY=y\n", fp);
#else
    fputs("CONFIG_UTT_VPN_PROXY=n\n", fp);
#endif
    fputs("ROMFS_WEBS = ", fp);
    /*将页面写入文件中，以空格隔开*/
    for (i = 0; i < (CNT(RomWebs) - 1); i++) {
	fputs(RomWebs[i], fp);
	fputs(" ", fp);
    }

    /* ToMAX 产品页面控制 */
#if (UTT_TOMAX == FYES)
    fputs("\nROMFS_WEBS_ToMAX = ", fp);
    for (i = 0; i < (CNT(RomWebsToMAX) - 1); i++) {
	fputs(RomWebsToMAX[i], fp);
	fputs(" ", fp);
    }
#endif
    fclose(fp);/*关闭文件*/

    return 0;
}
