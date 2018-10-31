#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "uttMachine.h" 

#if(FIT_AP_LOCAL_CONF_TEMP == FYES)

#define DEBUG_TEMP 0
#if DEBUG_TEMP
#define TEMP_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define TEMP_DEBUG(fmt, argx...)
#endif

static void formFitApLocalConfTemp(webs_t wp, char_t *path, char_t *query);
static int aspOutPutFitApLocalConfTemp(int eid, webs_t wp, int argc, char_t **argv);

/***********************************************************************
 * 函 数 名：   formFitApLocalConfTemp
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void formFitApLocalConfTemp(webs_t wp, char_t *path, char_t *query)
{
    TEMP_DEBUG("\n---> formConfigApConfTemp() begin <---\n");
    char  *wireless, *ssid, *sgi, *preamble, *wmm, *rate, *bw, *channel, *power, *manual, *mode, *isolate;
    WirelessProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeConf = MIB_PROF_AP_EFFECT_CONF;
    ApEffectConfProfile *profConf = NULL;

    wireless = websGetVar(wp, T("en_wireless"), T("")); 
    ssid = websGetVar(wp, T("ssid"), T("")); 
    sgi = websGetVar(wp, T("SGI"), T("")); 
    isolate = websGetVar(wp, T("isolate"), T("")); 
    preamble = websGetVar(wp, T("preamble"), T("")); 
    wmm = websGetVar(wp, T("wmm"), T("")); 
    rate = websGetVar(wp, T("rate"), T("")); 
    bw = websGetVar(wp, T("BW"), T("")); 
    channel = websGetVar(wp, T("channel"), T("")); 
    power = websGetVar(wp, T("power"), T("")); 
    manual = websGetVar(wp, T("manual"), T("")); 
    mode = websGetVar(wp, T("mode"), T("")); 
    TEMP_DEBUG("\n---> wireless:%d ssid:%s sgi:%d preamble:%d wmm:%d rate:%ld bw:%ld channel:%ld power:%ld manual:%ld mode:%ld <---\n", atoi(wireless), ssid, atoi(sgi), atoi(preamble), atoi(wmm), strtoul(rate, 0, 10), strtoul(bw, 0, 10), strtoul(channel, 0 , 10), strtoul(power, 0, 10), strtoul(manual, 0, 10), strtoul(mode, 0, 10));
    prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);
    profConf = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibTypeConf, 0);
    ProfBackupByIndex(mibType, profAction, 0, &profhead);
    if (prof != NULL)
    {
	prof->head.active = atoi(wireless);
	strncpy(prof->mBaseCf.SSID1, ssid, sizeof(prof->mBaseCf.SSID1));
	prof->mBaseCf.HT_GI = atoi(sgi);
	prof->mBaseCf.NoForwarding = atoi(isolate);
	prof->advance.preamble = atoi(preamble);
	prof->advance.wmm = atoi(wmm);
	if (atoi(rate) == 0)
	{
	    prof->fitAp.rate = AUTO_RATE;
	}
	else if (atoi(rate) == 11)
	{
	    prof->fitAp.rate = M11;
	}
	else if (atoi(rate) == 54)
	{
	    prof->fitAp.rate = M54;
	}
	else if (atoi(rate) == 150)
	{
	    prof->fitAp.rate = M150;
	}
	else
	{
	    prof->fitAp.rate = M300;
	}
	prof->mBaseCf.ChanWidth = strtoul(bw, 0, 10);
	prof->mBaseCf.Channel = strtoul(channel, 0, 10);

	if (prof->mBaseCf.Channel == 0) {
		prof->mBaseCf.AutoChannelSelect = 2;
	} else {
		prof->mBaseCf.AutoChannelSelect = 0;
	}

	prof->fitAp.autoPower = strtoul(power, 0, 10);
	if (atoi(manual) == 1)
	{
	    prof->fitAp.manPower = LOW;
	}
	else if (atoi(manual) == 2)
	{
	    prof->fitAp.manPower = MIDDLE;
	}
	else
	{
	    prof->fitAp.manPower = HIGH;
	}
	prof->mBaseCf.WirelessMode = strtoul(mode, 0, 10);
	ProfSetNameByPoint(prof, "wireless0");
    }
    TEMP_DEBUG("\n---> PROF:  wireless:%d ssid:%s sgi:%d preamble:%d wmm:%d rate:%d bw:%d channel:%d autopower:%d manual:%d mode:%d <---\n", prof->head.active, prof->mBaseCf.SSID1, prof->mBaseCf.HT_GI, prof->advance.preamble, prof->advance.wmm, prof->fitAp.rate, prof->mBaseCf.ChanWidth, prof->mBaseCf.Channel, prof->fitAp.autoPower, prof->fitAp.manPower, prof->mBaseCf.WirelessMode);
    if ((profConf != NULL) && (profConf->config == PROF_ENABLE))
    {
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	sleep(5u);
    }
    nvramWriteCommit();

    websRedirect(wp, "wlanPara.asp");
    TEMP_DEBUG("\n---> formConfigApConfTemp() end <---\n");
}

/***********************************************************************
 * 函 数 名：   aspOutPutFitApLocalConfTemp
 * 功能描述：	读一个条目送页面
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int aspOutPutFitApLocalConfTemp(int eid, webs_t wp, int argc, char_t **argv)
{
    TEMP_DEBUG("\n---> aspOutPutFitApLocalConfTemp() begin <---\n");
    MibProfileType mibType = MIB_PROF_WIRELESS;
    ApConfProfile *profConf = NULL;
    WirelessProfile *prof = NULL;

    prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
	websWrite(wp,"var wireless = %s;\n", (prof->head.active == 1 ? "true" : "false"));
	websWrite(wp,"var ssid = \"%s\";\n", prof->mBaseCf.SSID1);
	websWrite(wp,"var sgi = %s;\n", (prof->mBaseCf.HT_GI == 1 ? "true" : "false"));
	websWrite(wp,"var cltIsolate = %s;\n", (prof->mBaseCf.NoForwarding == 1 ? "true" : "false"));
	websWrite(wp,"var preamble = %s;\n", (prof->advance.preamble == 1 ? "true" : "false"));
	websWrite(wp,"var wmm = %s;\n", (prof->advance.wmm == 1 ? "true" : "false"));
	if (prof->fitAp.rate == AUTO_RATE)
	{
	    websWrite(wp,"var rate = %d;\n", 0);
	}
	else if (prof->fitAp.rate == M11)
	{
	    websWrite(wp,"var rate = %d;\n", 11);
	}
	else if (prof->fitAp.rate == M54)
	{
	    websWrite(wp,"var rate = %d;\n", 54);
	}
	else if (prof->fitAp.rate == M150)
	{
	    websWrite(wp,"var rate = %d;\n", 150);
	}
	else
	{
	    websWrite(wp,"var rate = %d;\n", 300);
	}
	websWrite(wp , "var maxRate = %d;\n", MAX_WIFI_RATE);
	websWrite(wp,"var bw = %d;\n", prof->mBaseCf.ChanWidth);
	websWrite(wp,"var channel = %d;\n", prof->mBaseCf.Channel);
	websWrite(wp,"var power = %d;\n", prof->fitAp.autoPower);
	if (prof->fitAp.manPower == 1)
	{
	    websWrite(wp,"var manPower = %d;\n", 1);
	}
	else if (prof->fitAp.manPower == 2)
	{
	    websWrite(wp,"var manPower = %d;\n", 2);
	}
	else
	{
	    websWrite(wp,"var manPower = %d;\n", 3);
	}
	websWrite(wp,"var mode = %d;\n", prof->mBaseCf.WirelessMode);
    }

    profConf = (ApConfProfile *)ProfGetInstPointByName(MIB_PROF_AP_CONF, "default");
    if ((NULL != profConf ) && (0 == ProfInstIsFree((Pointer)profConf)) && (TRUE == profConf->head.active)) {
	    websWrite(wp,"var apType = \"%s\";\n", (profConf->types));
    }

    getTheErrorMsg(eid, wp, argc, argv);
    TEMP_DEBUG("\n---> aspOutPutFitApLocalConfTemp() end <---\n");
    return 0;
}

/***********************************************************************
 * 函 数 名：   formDefineFitApLocalConfTemp
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineFitApLocalConfTemp(void)
{
    websAspDefine(T("aspOutPutFitApLocalConfTemp"), aspOutPutFitApLocalConfTemp);
    websFormDefine(T("formFitApLocalConfTemp"), formFitApLocalConfTemp);
}
#endif
