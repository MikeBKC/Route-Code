#include	"uttMachine.h"
#include	"typedef.h"
#include    "wsIntrn.h"
#include    "um.h"
#include	<mib.h>
#include    "utils.h"
#include    "uttfunction.h"
#include    "profacce.h"
#include	"comExeFun.h"
#include    "argcmd.h"
#include    "wuScript.h"
#include    "mib.h"


#if (APP_POLICY_REMOTE_UPDATE == FYES)
extern void getAllAppType(int eid, webs_t wp, int argc, char **argv);
#endif
void aspoutDynConfig(int eid, webs_t wp, int argc, char **argv)
{
#if(DST == FYES)
    websWrite(wp, "var dst = 1;");
#else
    websWrite(wp, "var dst = 0;");
#endif
#if (MACADDR_ADD_LIMIT == FYES)
	websWrite(wp, "var mac_limit_menu = 1;");
#else
	websWrite(wp, "var mac_limit_menu = 0;");
#endif

#if (CONFIG_USER_GOAHEAD_LANG_EN == FYES)
	websWrite(wp, "var lang_en = 1;");
#else
	websWrite(wp, "var lang_en = 0;");
#endif

#if (WEBAUTH_ADVANCE == FYES)
	websWrite(wp, "var WEBAUTH_ADVANCE = 1;");
#else
	websWrite(wp, "var WEBAUTH_ADVANCE = 0;");
#endif
#if (WEBAUTH_ACCOUNT == FYES)
	websWrite(wp, "var WEBAUTH_ACCOUNT = 1;");
#else
	websWrite(wp, "var WEBAUTH_ACCOUNT = 0;");
#endif
#if (FEATURE_SW_SNMP == FYES)
	websWrite(wp, "var snmpConf = 1;");
#else
	websWrite(wp, "var snmpConf = 0;");
#endif

#if (AP_LOG_MANAGE == FYES)
	websWrite(wp, "var apLog = 1;");
#else
	websWrite(wp, "var apLog = 0;");
#endif

#if (AP_LOAD_BALANCE == FYES)
	websWrite(wp, "var apLoadBalance = 1;");
#else
	websWrite(wp, "var apLoadBalance = 0;");
#endif

#if (SYS_LOG_INFO == FYES)
	websWrite(wp, "var SysLogInfo = 1;\n");
#else
	websWrite(wp, "var SysLogInfo = 0;\n");
#endif

#if (MAC_FILTER == FYES)
	websWrite(wp, "var MacFilter = 1;\n");
#else
	websWrite(wp, "var MacFilter = 0;\n");
#endif

#if (MAKE_MONEY == FYES)
	websWrite(wp, "var IW_MMoney = 1;\n");
#else
	websWrite(wp, "var IW_MMoney = 0;\n");
#endif
#if(BRAND_LEVEL_ONE == FYES)
	websWrite(wp, "var levelone_ctrl = 1;\n");
	websWrite(wp, "var LEVELONE_LOGO = 1;\n");
#else
	websWrite(wp, "var levelone_ctrl = 0;\n");
	websWrite(wp, "var LEVELONE_LOGO = 0;\n");
#endif
#if(GALAXYWIND == FYES)
	websWrite(wp, "var galaxywind_ctrl = 1;\n");
#else
	websWrite(wp, "var galaxywind_ctrl = 0;\n");
#endif

#if (UTT_SMART_UI == FYES)
	websWrite(wp, "var uttSmartUi = 1;");
#else
	websWrite(wp, "var uttSmartUi = 0;");
#endif

#if (GAME_BOOST == FYES)
	websWrite(wp, "var Game_Boost =1;");
#else
	websWrite(wp, "var Game_Boost =0;");
#endif

#if (WIRELESS_5G == FYES)
	websWrite(wp, "var WlsConf_5g = 1;");
	websWrite(wp, "var WlanBase_5g = 1;");
	websWrite(wp, "var WlanSecurity_5g = 1;");
	websWrite(wp, "var WlanMacFilter_5g = 1;");
	websWrite(wp, "var WlanAdvanced_5g = 1;");
	websWrite(wp, "var WlanMultiSsid_5g = 1;");
	websWrite(wp, "var WlanHostInfo_5g = 1;");
#if (UTT_SMART_UI == FYES)
	websWrite(wp, "var wlanSet_5g = 1;");
#endif
#else
	websWrite(wp, "var WlsConf_5g = 0;");
	websWrite(wp, "var WlanBase_5g = 0;");
	websWrite(wp, "var WlanSecurity_5g = 0;");
	websWrite(wp, "var WlanMacFilter_5g = 0;");
	websWrite(wp, "var WlanAdvanced_5g = 0;");
	websWrite(wp, "var WlanMultiSsid_5g = 0;");
	websWrite(wp, "var WlanHostInfo_5g = 0;");
#if (UTT_SMART_UI == FYES)
	websWrite(wp, "var wlanSet_5g = 0;");
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
	websWrite(wp, "var NetShareManage = 1;");
	websWrite(wp, "var FtpServer = 1;");
	websWrite(wp, "var ShareAccountList = 1;");
#else
	websWrite(wp, "var NetShareManage = 0;");
	websWrite(wp, "var FtpServer = 0;");
	websWrite(wp, "var ShareAccountList = 0;");
#endif

#if (SMART_WLAN_REPEATER == FYES)
	websWrite(wp, "var smartWlanRepeater = 1;");
#else
	websWrite(wp, "var smartWlanRepeater = 0;");
#endif
#if (SMART_CLOUD_SHARE == FYES)
	websWrite(wp, "var smartCloudShare = 1;");
#else
	websWrite(wp, "var smartCloudShare = 0;");
#endif
#if (SMART_SECURITY_CONF == FYES)
	websWrite(wp, "var smartSecutityConf = 1;");
#else
	websWrite(wp, "var smartSecutityConf = 0;");
#endif
#if (DIAGNOSE_DETECT == FYES)
	websWrite(wp, "var diagTest = 1;");
#else
	websWrite(wp, "var diagTest = 0;");
#endif
#if (HOTEL_PNP == FYES)
	websWrite(wp, "var hotel = 1;");
#else
	websWrite(wp, "var hotel = 0;");
#endif

#if (ADFILTER == FYES)
	websWrite(wp, "var ADfilter = 1;");
#else
	websWrite(wp, "var ADfilter = 0;");
#endif

#if (APP_POLICY_REMOTE_UPDATE == FYES)
    getAllAppType(eid,wp,argc,argv);
#else
	websWrite(wp, "var SOFT_UDTYPE = 0;");
	websWrite(wp, "var ADFILTER_UDTYPE = 0;");
#endif
#if(AUTO_UPDATE == FYES)
	websWrite(wp, "var auto_update = 1;\n");
#else
	websWrite(wp, "var auto_update = 0;\n");
#endif

#if (FEATURE_AP_MAC_FILTER == FYES)
	websWrite(wp, "var fitApMacFilter = 1;\n");
#else
	websWrite(wp, "var fitApMacFilter = 0;\n");
#endif
#if (VLAN_CONFIG == FYES)
	websWrite(wp, "var vlanConfig = 1;\n");
#else
	websWrite(wp, "var vlanConfig = 0;\n");
#endif
#if (VPN_PROXY == FYES)
        websWrite(wp, "var vpnProxy = 1;\n");
#else
        websWrite(wp, "var vpnProxy = 0;\n");
#endif
#if (NET_SECURITY == FYES)
	websWrite(wp, "var accessCtrl = 1;\n");
#else
	websWrite(wp, "var accessCtrl = 0;\n");
#endif
#if (WECHAT_CWMP == FYES)
	websWrite(wp, "var weChatManage = 1;\n");
#else
	websWrite(wp, "var weChatManage = 0;\n");
#endif
#if (UTT_SSID_ISOLATION == FYES)
    websWrite(wp,"var ssidSeparateCtr=1;\n");
#else
    websWrite(wp,"var ssidSeparateCtr=0;\n");
#endif
#if (UTT_KNOWIFI == FYES)
    websWrite(wp,"var uttKnowifi=1;\n");
#else
    websWrite(wp,"var uttKnowifi=0;\n");
#endif
#if (UTT_5G_CHANNEL_149_TO_165 == FYES)
	websWrite(wp,"var utt_5g_channel_149_to_165=1;\n");
#else 
	websWrite(wp,"var utt_5g_channel_149_to_165=0;\n");
#endif
#if (UTT_WLAN_WAPI == FYES)
    websWrite(wp,"var utt_wapi=1;\n");
#else
    websWrite(wp,"var utt_wapi=0;\n");
#endif
#if (UTT_SMART_UI == FYES)
#if (FEATURE_AC == FYES)
    websWrite(wp,"var ApwlanSet=1;\n");
    websWrite(wp,"var ApSta=1;\n");
#else
    websWrite(wp,"var ApwlanSet=0;\n");
    websWrite(wp,"var ApSta=0;\n");
#endif
#endif
#if (MIDDLE_SOFT == FYES)
    websWrite(wp,"var middlesoft = 1;\n");
    websWrite(wp,"var Ctrlchannel = 1;\n");
    websWrite(wp,"var uttKnowifi = 0;\n");
    websWrite(wp,"var taskplan = 1;\n");
#else
    websWrite(wp,"var middlesoft = 0;\n");
    websWrite(wp,"var Ctrlchannel = 0;\n");
    websWrite(wp,"var taskplan = 0;\n");
#endif

#if (AUTO_CHANNEL == FYES)
    websWrite(wp,"var ctrllan = 1;\n");
#else
    websWrite(wp,"var ctrllan = 0;\n");
#endif
#if (WMM_ENABLE == FYES)
    websWrite(wp,"var Wmmenable = 1;\n");
#else
    websWrite(wp,"var Wmmenable = 0;\n");
#endif
#if (APP_ELINKC == FYES)
    websWrite(wp, "var elinkc_flag = 1;\n");
#else
    websWrite(wp, "var elinkc_flag = 0;\n");
#endif
#if (CTRL_SERIALNUM == FYES)
    websWrite(wp,"var ctrlserialnum = 1;\n");
#else
    websWrite(wp,"var ctrlserialnum = 0;\n");
#endif
#if (RAISECOM_A30W == FYES)
    websWrite(wp,"var ctrlwds = 0;\n");
    websWrite(wp,"var changvar  = 0;\n");
    websWrite(wp,"var raisecom_a30w = 1;\n");
#else 
    websWrite(wp,"var ctrlwds = 1;\n");
    websWrite(wp,"var changvar  = 1;\n");
    websWrite(wp,"var raisecom_a30w = 0;\n");
#endif
#if (SKYWORTH == FYES)
    websWrite(wp,"var skyworth = 1;\n");
#else
    websWrite(wp,"var skyworth = 0;\n");
#endif
}
extern void websDefineDynConfig(void)
{
 websAspDefine(T("aspoutDynConfig"), aspoutDynConfig);
}
