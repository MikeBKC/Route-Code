#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/utsname.h>
#include "../../autoconf.h"
#include "translate.h"
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
    #include  <linux/types.h>
    #include  <linux/socket.h>
    #include  <linux/if.h>
#endif
#include <linux/wireless.h>

#include <user_oid.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "management.h"
#include "wireless.h"
#include "linux/autoconf.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#if (FEATURE_AP_BASIC_INFO == FYES)

#if 0
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    "webmsg.h"
#endif


/********************************************************************
* 函数： aspOutPutApEffectConfUrcp
* 功能： 页面与交互转换函数
* 创建： 2012-06-20
* 参数： 略
* 输出： 无
* 返回： 无
* 作者： xhhu 
*******************************************************************/
#if 0
static int aspOutPutApEffectConfUrcp(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof = NULL;
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
	//websWrite(wp,"var config = %d;", prof->effect_conf);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

/********************************************************************
* 函数： getStrInfo
* 功能： transmit the parameters in the page of AP system info
* 创建： 2012-07-19
* 参数： webs_t wp
* 输出： void
* 返回： void
* 作者： ck
*******************************************************************/
static void getStrInfo(webs_t wp, char *var, char *name, const char *buf)
{
	char *p = NULL;
	char tmp[20];

	memset(tmp, '\0', sizeof(tmp));

	p = strstr(buf, name);
	if (p != NULL) {
		p = p + strlen(name);
		while ((*p == '=') || (*p == ':') || (*p == ' ')) {
			p++;
		}

		sscanf(p, "%[^ ]", tmp);
		if (*tmp != '\0') {
			if (!strcmp(name, "Bit Rate")) {
				websWrite(wp, T("var %s=\"%s Mb/s\";\n"), var, tmp);
			} else {
				websWrite(wp, T("var %s=\"%s\";\n"), var, tmp);
			}
		}
	}

	return;
}

/********************************************************************
* 函数： getWirelessInfo
* 功能： transmit the parameters of wireless
*		 in the page of AP system info
* 创建： 2012-07-19
* 参数： webs_t wp
* 输出： void
* 返回： void
* 作者： ck
*******************************************************************/
static void getWirelessInfo(webs_t wp)
{
	int fd;
	int ret = 0;
	unsigned int channel;
	char *p = NULL;
	char buf[2048];
	const char *message=NULL;
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	websWrite(wp, T("var macForWlan = \"\";\n"));
	websWrite(wp, T("var channel = \"\";\n"));
	websWrite(wp, T("var rate = \"\";\n"));
	websWrite(wp, T("var wlanMode = \"\";\n"));

	memset(buf, '\0', sizeof(buf));
	doSystem("iwconfig %s > /tmp/apInfo", getApIfName());
	fd = open("/tmp/apInfo", O_RDONLY);
	if (fd != -1) {
		ret = read(fd, buf, sizeof(buf));
		if (ret != -1) {
			getStrInfo(wp, "macForWlan", "Access Point", buf);
			getStrInfo(wp, "channel", "Channel", buf);
//			getStrInfo(wp, "rate", "Bit Rate", buf);
		}
	}

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
	if ((NULL != prof) && (TRUE == prof->head.active)) {
		if (4 == prof->mBaseCf.WirelessMode) {
			websWrite(wp, T("var wlanMode=\"11g\";\n"));
		} else if (6 == prof->mBaseCf.WirelessMode) {
			websWrite(wp, T("var wlanMode=\"11n\";\n"));
		} else {
			websWrite(wp, T("var wlanMode=\"11b/g/n\";\n"));
		}

        if (0 == prof->fitAp.rate) {
			getStrInfo(wp, "rate", "Bit Rate", buf);
		} else if (1 == prof->fitAp.rate) {
			websWrite(wp, T("var rate=\"11 Mb/s\";\n"));
		} else if (2 == prof->fitAp.rate) {
			websWrite(wp, T("var rate=\"54 Mb/s\";\n"));
		} else if (3 == prof->fitAp.rate) {
			websWrite(wp, T("var rate=\"150 Mb/s\";\n"));
		} else if (4 == prof->fitAp.rate) {
			websWrite(wp, T("var rate=\"300 Mb/s\";\n"));
		}


		channel = prof->mBaseCf.Channel;
		if (prof->mBaseCf.AutoChannelSelect != 0) {
		message=getTransDataForC(C_LANG_INDEX_APBASICINFO_AUTO);
		
			websWrite(wp, T("var channel = \"%S\";\n"),message);
		} else {
			websWrite(wp, T("var channel = \"%d\";\n"), channel);
		}
	}

	close(fd);
	return;
}

/********************************************************************
* 函数： getApStatusInfo
* 功能： transmit the parameters of Ap status
*		 in the page of AP system info
* 创建： 2012-07-19
* 参数： webs_t wp
* 输出： void
* 返回： void
* 作者： ck
*******************************************************************/
static void getApstatusInfo(webs_t wp)
{
	int s;
	struct iwreq iwr;
	unsigned int host_no = 0u;
	RT_802_11_MAC_TABLE table= {0};
	int min, max, index;
	char szBuf[128];
	Uint32 mode = 0u, flag = 0;
	FatFitSwitchProfile *profFFS = NULL;
	FitApSzProfile *profFAS = NULL;
	WirelessProfile *profWl = NULL;
	MibProfileType mibTypeFFS = MIB_PROF_FAT_FIT_SWITCH;
	MibProfileType mibTypeFAS = MIB_PROF_FIT_AP_SZ;
	MibProfileType mibTypeWl = MIB_PROF_WIRELESS;
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
	FitAp5gSzProfile *profFAS_5g = NULL;
	MibProfileType mibTypeFAS_5g = MIB_PROF_FIT_AP_SZ_5G;
#endif
	websWrite(wp, T("var workingMode = \"\";\n"));
	websWrite(wp, T("var SZ=\"\";\n"));
	websWrite(wp, T("var ssid=\"\";\n"));
	websWrite(wp, T("var cltsCount = 0;\n"));

	// get the ap mode. for example: fat ap, fit ap
    profFFS = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibTypeFFS, 0);
    if (profFFS != NULL)
    {
		mode = profFFS->fatEnable;
    }
	websWrite(wp, T("var workingMode = \"%d\";\n"), mode);

	// get the service name
	memset(szBuf, '\0', sizeof(szBuf));
	ProfInstLowHigh(mibTypeFAS, &max, &min);

	for (index = min; index < max; index++) 
	{
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
		profFAS_5g = (FitAp5gSzProfile *)ProfGetInstPointByIndex(mibTypeFAS_5g, index);
		if ((profFAS_5g != NULL) && (TRUE == profFAS_5g->head.active) && ((ProfInstIsFree((Pointer)profFAS_5g)) == 0))
		{
			if (1 == flag) 
			{
				strcat(szBuf, ",");
			}
			strcat(szBuf, profFAS_5g->head.name);
			flag = 1;
		}
#endif
		profFAS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeFAS, index);
		if ((profFAS != NULL) && (TRUE == profFAS->head.active) && ((ProfInstIsFree((Pointer)profFAS)) == 0)){
			if (1 == flag) {
				strcat(szBuf, ",");
			}
			strcat(szBuf, profFAS->head.name);
			flag = 1;
		}
	}
	if (1 == flag) {
		websWrite(wp, T("var SZ=\"%s\";\n"), szBuf);
		flag = 0;
	} else {
		websWrite(wp, T("var SZ=\"\";\n"));
	}

	//get the ssid name
	memset(szBuf, '\0', sizeof(szBuf));
	ProfInstLowHigh(mibTypeWl, &max, &min);
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
	for (index = min + 2; index < max; index++) 
#else
	for (index = min + 1; index < max; index++) 
#endif
	{
	    profWl = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeWl, index);
		if ((profWl != NULL) && (TRUE == profWl->head.active) && ((ProfInstIsFree((Pointer)profWl)) == 0)){
			if (1 == flag) {
				strcat(szBuf, ",");
			}
			strcat(szBuf, profWl->mBaseCf.SSID1);
			flag = 1;
		}
	}
	if (1 != flag) {
	    profWl = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeWl, 0);
		if ((profWl != NULL) && (TRUE == profWl->head.active) && ((ProfInstIsFree((Pointer)profWl)) == 0)){
			strcat(szBuf, profWl->mBaseCf.SSID1);
			flag = 1;
		}
	}

	if (1 == flag) {
		websWrite(wp, T("var ssid=\"%s\";\n"), szBuf);
	} else {
		websWrite(wp, T("var ssid=\"\";\n"));
	}

	/* 连接客户数初始设为0，如果下面ioctl读取失败，页面显示为0 */
    websWrite(wp, T("var cltsCount = \"0\";\n"));

	// get the clients number
	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s >= 0) {
#ifdef CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
		if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) {
#else
		if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
#endif
			close(s);
		} else {
			host_no = table.Num;
			websWrite(wp, T("var cltsCount = \"%d\";\n"), host_no);

		}
	}
	return;
}

/********************************************************************
* 函数： getBasicInfo
* 功能： transmit the parameters of Ap basic info 
*		 in the page of AP system info
* 创建： 2012-07-19
* 参数： webs_t wp
* 输出： void
* 返回： void
* 作者： ck
*******************************************************************/
static void getBasicInfo(webs_t wp)
{
    char_t *tmp = NULL;
    struct utsname utsbuf;
	char *tmscb = "&#8482";
	extern unsigned int productSn;
	MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof = NULL;

	websWrite(wp,"var deviceName = \"\";\n");
	websWrite(wp, T("var softVer=\"\";\n"));
	websWrite(wp, T("var firmVer=\"\";\n"));
	websWrite(wp, T("var serialNo = \"\";\n"));

    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");
    tmp = strtok(NULL, " ");
	websWrite(wp, T("var softVer=\"%s%s\";\n"), utsbuf.version, tmp);
	websWrite(wp, T("var firmVer=\"%s\";\n"), UTT_HWMODEL);
	websWrite(wp, T("var serialNo = \"%d\";\n"), productSn);

    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
		websWrite(wp,"var deviceName = \"%s\";\n", prof->dev_name);
    }

	return;
}


/********************************************************************
* 函数： getInterfaceInfo
* 功能： transmit the parameters of Ap Interface info 
*		 in the page of AP system info
* 创建： 2012-07-19
* 参数： webs_t wp
* 输出： void
* 返回： void
* 作者： ck
*******************************************************************/
static void	getInterfaceInfo(webs_t wp)
{
	MibProfileType mibType = MIB_PROF_INTERFACE;
	InterfaceProfile *profIf = NULL;
    char *name = NULL;
    unsigned int ip;
    unsigned int mask;

	websWrite(wp, T("var ipAddr=\"\";\n"));
	websWrite(wp, T("var macForInterface =\"\";\n"));
	websWrite(wp, T("var subMask = \"\";\n"));
	websWrite(wp, T("var gateway = \"\";\n"));

    name = getLanIfName();
    get_ip(name, &ip);
	websWrite(wp, T("var ipAddr=\"%N\";\n"), ip);
    get_mask(name, &mask);
	websWrite(wp, T("var subMask = \"%N\";\n"), mask);

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
	if (NULL != profIf) {
//	    websWrite(wp, T("var ipAddr=\"%N\";\n"),  profIf->ether.sta.ip);
		websWrite(wp, T("var macForInterface =\"%s\";\n"), converMac6To17Byte(profIf->ether.mac));
//		websWrite(wp, T("var subMask = \"%N\";\n"), profIf->ether.sta.nm);
		websWrite(wp, T("var gateway = \"%N\";\n"), profIf->ether.sta.gw);
	}

	return;
}

/********************************************************************
* 函数： aspOutPutApBasicInfoUrcp
* 功能： 页面与交互转换函数
* 创建： 2012-06-20
* 参数： 略
* 输出： 无
* 返回： 无
* 作者： xhhu 
*******************************************************************/
static int aspOutPutApBasicInfoUrcp(int eid, webs_t wp, int argc, char **argv)
{
	getBasicInfo(wp);
	getWirelessInfo(wp);
	getInterfaceInfo(wp);
	getApstatusInfo(wp);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/********************************************************************
 * 函数： formDefineApBasicConfUrcp
 * 功能： 页面与交互转换函数
 * 创建： 2012-06-20
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： xhhu 
 *******************************************************************/
extern void formDefineApBasicInfo(void)
{
//    websAspDefine(T("aspOutPutApEffectConfUrcp"), aspOutPutApEffectConfUrcp);
    websAspDefine(T("aspOutPutApBasicInfoUrcp"), aspOutPutApBasicInfoUrcp);
}
#endif
