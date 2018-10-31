#include    <stdio.h>
#include    "uttMachine.h"
#if(EASY_TASK == FYES)
#include    "mib.h"
#include    "profacce.h"

#include    <stdlib.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <net/route.h>
#include    <string.h>
#include    <dirent.h>
#include    "internet.h"
#include    "webs.h"
#include    "utils.h"
#include    "firewall.h"
#include    "management.h"
#include    "station.h"
#include    "wireless.h"
#include    "um.h"
#include    "uttfunction.h"
#include    "comExeFun.h"
#include    "ifmisc.h"

#ifdef EASYTASK_DEBUG
#define EASYTASK_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define EASYTASK_PRINTF(fmt, args...)
#endif

extern int get_lan_status(int lanIndex);
extern int get_phylink_status(int wanIndex);
static void formUserEasyTask(webs_t wp, char_t *path, char_t *query);
static void formStopWanCheck(webs_t wp, char_t *path, char_t *query);
static void formStatForEasyTask(webs_t wp, char_t *path, char_t *query);
extern void closeDefConf();
static void editEasyTaskState(webs_t wp, char_t *path, char_t *query);
static void formCloseDefConf(webs_t wp, char_t *path, char_t *query);
static void formEasyTaskConfig(webs_t wp, char_t *path, char_t *query);
static void formGetPortCount(webs_t wp, char_t *path, char_t *query);
static void formEasyTaskReCheck(webs_t wp, char_t *path, char_t *query);
static int outPutWanStat(webs_t wp, SystemProfile *profSys);
static int aspoutEasyTaskNotice(int eid, webs_t wp, int argc, char_t **argv);
static int aspoutEasyTaskConfig(int eid, webs_t wp, int argc, char_t **argv);
static int aspoutEasyTask(int eid, webs_t wp, int argc, char_t **argv);
extern void editWlanTimeConfig(webs_t wp, char_t *path, char_t *query, int lock);
static void formEasyTask(webs_t wp, char_t *path, char_t *query);
static void formEasyTaskNoticeEn(webs_t wp, char_t *path, char_t *query);
extern void formDefineEasyTask(void);
extern int setWanx(webs_t wp, int wanIndex, char_t *wanNo, char_t *ctype, InterfaceProfile *profIf);
extern int initWan(int wanIndex);
#if (WIRELESS_BASE_CONFIG == FYES)
extern int getAPCliSlotIndex(void);
#endif
#if (WIRELESS == FYES)
static void setAPSecurity(webs_t wp, char_t *path, char_t *query);
extern void clearRadiusSet(WirelessProfile *prof);
#endif
#if (ADMIN_CONF == FYES)
extern void setSysAdm(webs_t wp, char_t *path, char_t *query);
#endif
#if (SMART_QOS_FUN == FYES)
extern void editSmartQosConfig(webs_t wp, char_t *path, char_t *query);
#endif

/*
 * 功能：修改管理员admin的密码
 */
static void formUserEasyTask(webs_t wp, char_t *path, char_t *query)
{
    char_t *wir = NULL;
    wir = websGetVar(wp, T("wir"), T(""));/*无线设置输入框隐藏*/
#if (ADMIN_CONF == FYES)
    setSysAdm(wp, path, query);/*设置密码，无返回值*/
#endif
    /*根据原页面有无无线设置来选择跳转页面有无无线设置*/
    if (strcmp(wir, T("0")) == 0)
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=0"));/*页面跳转*/
    }
    else
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=1"));/*页面跳转*/
    }/*end -- (strcmp(wir, T("0")) == 0)*/
}

/*
 * 功能：停止WAN口智能检测
 */
static void formStopWanCheck(webs_t wp, char_t *path, char_t *query)
{
    doSystem(T("killall uttLinkCheck >/dev/null 2>&1"));/*kill检测进程*/
    wanConnStatsChange(CHECK_INTERRUPT);/*将WAN口状态设置为检测中断*/
}

/*
 * 功能：关闭出厂配置标志,设置相应状态
 */
extern void closeDefConf()
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);
    if (ProfInstIsFree(profIf) == 0)
    {
	wanConnStatsChange(CONNECTING);
    }
    if (ProfInstIsFree(profSys) == 0)
    {
	/*将isDefaultConf由出厂变为非出厂*/
	profSys->isDefaultConf = 1;
    }
}

/*
 * 功能：对应页面上的"上网正常，无需配置"
 */
static void formCloseDefConf(webs_t wp, char_t *path, char_t *query)
{
    closeDefConf();
    nvramWriteCommit();
#if (UTT_SMART_UI == FYES)
    //setSuccessTips();
    websRedirect(wp, T("EasyManageSuccess_Sui.asp"));
#else
    websRedirect(wp, T("EasyManageSuccess.asp"));
#endif
    return;
}

/*
 * 功能：输出WAN口连接状态
 * 返回值：0--成功 1--失败
 */
static int outPutWanStat(webs_t wp, SystemProfile *profSys)
{
    int i = 0;
    int retVal = 0;
    for (i=0;i<profSys->wanIfCount;i++)
    {
	/*设置WAN口已连接或断开*/
	if (get_phylink_status(i + 1) == 1)
	{
	    websWrite(wp, T("wanStat[%d] = \"UP\";"), i);/*已连接*/
	}
	else/*未连接*/
	{
	    websWrite(wp, T("wanStat[%d] = \"DOWN\";"), i);
	}
	websWrite(wp, T("wanPort[%d] = \"WAN%d\";"), i, i + 1);
    }/*end -- (i=0;i<profSys->wanIfCount;i++)*/
    return retVal;
}

#if (WLAN_WORK_TIME == FYES)
/*
 * 绿色上网功能保存
 */
static void editSafeToNetState(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    char *safeToNet = NULL;/*绿色上网*/

    safeToNet = websGetVar(wp, T("safeToNet"), T(""));
    
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == PROFOK)
    {
	if (strcmp(safeToNet, T("on")) == 0)
	{
	    prof->safeToNet = FUN_ENABLE;
	    editWlanTimeConfig(wp, path, query, 1);
	}
	else
	{
	    prof->safeToNet = FUN_DISABLE;
	}
    }
}
#endif


#if (UTT_SMART_UI == FYES)
/*新风格UI 输出WAN口状态*/
static void formStatForWan_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
     
    /*ajax头*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));


    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);

    if (ProfInstIsFree(prof) == 0)
    {
	if (get_phylink_status(1) == 0)
	{
	    websWrite(wp, T("connStatus = %d;"), DISCONNECT);/*当前WAN口状态*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*连接方式*/
	}
	else if ((prof->wanLinkStates & (1U << 1)) > 0U)
	{
	    websWrite(wp, T("connStatus = %d;"), CONNECTED);/*当前WAN口状态*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*连接方式*/
	}
	else
	{
	    websWrite(wp, T("connStatus = %d;"), prof->ConnStatus);/*当前WAN口状态*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(prof->wanMode, ethConnModeEnum));
	}
    }
    
    websDone(wp, 200);/*ajax尾*/
}
#endif


/*
 * 功能：向页面输出需要使用ajax更新的信息
 *	每两秒执行一次
 */
static void formStatForEasyTask(webs_t wp, char_t *path, char_t *query)
{
    int i = 0;
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
     
    /*ajax头*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);

    if (ProfInstIsFree(prof) == 0)
    {
	if (get_phylink_status(1) == 0)
	{
	    websWrite(wp, T("connStatus = %d;"), DISCONNECT);/*当前WAN口状态*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*连接方式*/
	}
	else if ((prof->wanLinkStates & (1U << 1)) > 0U)
	{
	    websWrite(wp, T("connStatus = %d;"), CONNECTED);/*当前WAN口状态*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*连接方式*/
	}
	else
	{
	    websWrite(wp, T("connStatus = %d;"), prof->ConnStatus);/*当前WAN口状态*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(prof->wanMode, ethConnModeEnum));
	}
    }/*end -- (ProfInstIsFree(prof) == 0)*/
    if (ProfInstIsFree(profSys) == 0)
    {
	websWrite(wp, T("n_totalrecs = %d;"), profSys->wanIfCount);/*WAN口数量*/
	websWrite(wp, T("totalrecs = %d;"), profSys->lanPortCount);/*LAN口数量*/
	/*显示每个LAN口状态*/
	for (i=0;i<profSys->lanPortCount;i++)
	{
	    if (get_lan_status(i + 1) == 1)
	    {
		websWrite(wp, T("lanStat[%d] = \"UP\";"), i);/*已连接*/
	    }
	    else
	    {
		websWrite(wp, T("lanStat[%d] = \"DOWN\";"), i);/*未连接*/
	    }/*end -- (get_lan_status(i + 1) == 1)*/
	    websWrite(wp, T("lanPort[%d] = \"LAN%d\";"), i, i + 1);/*LAN口名称*/
	}/*end -- (i=0;i<profSys->lanPortCount;i++)*/
	outPutWanStat(wp, profSys);/*显示WAN口状态*/
    }/*end -- (ProfInstIsFree(profSys) == 0)*/
    
    websDone(wp, 200);/*ajax尾*/
}

/*
 * 功能：易管理配置页面保存
 */
static void formEasyTaskConfig(webs_t wp, char_t *path, char_t *query)
{ 
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
#if (WIRELESS_BASE_CONFIG == FYES)
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeWl = MIB_PROF_WIRELESS;
    WirelessProfile *profWl= NULL;
    char_t *ssid = NULL;/*无线网络名称*/
#endif
    int wanIndex = 1;
    char_t *ctype = NULL, *wir = NULL, *wireDisplay = NULL;
    
    ctype = websGetVar(wp, T("connectionType"), T(""));/*连接类型*/
    wir = websGetVar(wp, T("wir"), T(""));/*无线输入框显示*/
    wireDisplay = websGetVar(wp, T("wireDisplay"), T(""));
#if (WIRELESS_BASE_CONFIG == FYES)
    ssid = websGetVar(wp, T("ssid"), T(""));
#endif
         
    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIf, wanIndex);

    /*设置WAN口*/
    if(setWanx(wp, wanIndex, T(""), ctype, profIf) == 0) 
    {
	initWan(wanIndex);
    }/*end -- (setWanx(wp, wanIndex, T(""), ctype, profIf) == 0)*/

    editEasyTaskState(wp, path, query);

    /*由断线页面跳转的页面为"0", 有出厂页面跳转的为"1"*/
    if (strcmp(wir, T("1")) == 0)
    {
#if (WIRELESS == FYES)
	editSafeToNetState(wp, path, query);
#endif
	/*无法显示的两种安全类型为"0"，以上两个任一个为"0"，则页面不会显示无线输入框*/
	if (strcmp(wireDisplay, T("1")) == 0)
	{
#if (WIRELESS_BASE_CONFIG == FYES)
	    profWl = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeWl, 0);
	    if ((ProfInstIsFree(profWl) == 0) && (strcmp(ssid, T("")) != 0))
	    {
		ProfBackupByIndex(mibTypeWl, PROF_CHANGE_EDIT, 0, &profhead);
		strcpy(profWl->mBaseCf.SSID1, ssid);
	    }
#endif
#if (WIRELESS == FYES)
	    setAPSecurity(wp, path, query);
#endif
	}/*end -- (strcmp(wireDisplay, T("1")) == 0)*/
	websRedirect(wp, T("EasyManageConfig.asp?wir=1"));/*页面跳转*/
    }
    else
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=0"));/*页面跳转*/
    }/*end -- (strcmp(wir, T("1")) == 0)*/
#if (WIRELESS_BASE_CONFIG == FYES)
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#endif
    nvramWriteCommit();/*保存*/
    return;
}

/*
 * 功能：显示端口数量
 */
static void formGetPortCount(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *prof = NULL;

    prof = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0); 
    /*易管理界面需要显示接口状态*/
    websWrite(wp, T("var wanIfCount=%d;"), prof->wanIfCount);/*wan口数*/
    websWrite(wp, T("var numPorts = %u;\n"), prof->lanPortCount);/*lan口数*/
}


#if (UTT_SMART_UI == FYES)
/*新风格UI 重新检测WAN口连接状态*/
static void formWanReCheck_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    prof = (EasyTaskProfile *)ProfGetInstPointByIndex(mibType, 0);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    if (ProfInstIsFree(prof) == PROFOK)
    {
	doSystem(T("killall uttLinkCheck >/dev/null 2>&1"));/*kill检测进程*/
	doSystem("uttLinkCheck %s &", getWanIfName(1));
    }
    websDone(wp, 200); 
}
#endif

/*
 * 功能：易管理页面智能检测
 */
static void formEasyTaskReCheck(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    char_t *wir = NULL;
    wir = websGetVar(wp, T("wir"), T(""));
    prof = (EasyTaskProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == 0)
    {
	doSystem(T("killall uttLinkCheck >/dev/null 2>&1"));/*kill检测进程*/
	doSystem("uttLinkCheck %s &", getWanIfName(1));
    }
    if (strcmp(wir, T("0")) == 0)
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=0"));/*页面跳转*/
    }
    else
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=1"));/*页面跳转*/
    }
}

#if (WIRELESS == FYES)
/*
 * 功能：保存无线设置
 */
static void setAPSecurity(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;
    char *wirelessPwd = NULL;
    
    wirelessPwd = websGetVar(wp, T("wirelessPwd"), T(""));/*无线密码*/
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if (ProfInstIsFree(prof) == 0)
    {
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibType, profAction, 0, &profhead);
	clearRadiusSet(prof);
	/*无密码则无安全机制*/
	if (strcmp(wirelessPwd, T("")) == 0)
	{
	    strcpy(prof->mSafeCf.SelAuthMode, T("OPEN"));
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType, T("NONE"));
	    prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	}
	else/*输入了密码默认第四种安全机制*/
	{
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType, T("TKIPAES"));
	    prof->mSafeCf.ap.AuthMode.RekeyInterval = 3600U;
	    prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	    strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, wirelessPwd);/*除了密码其余为默认*/
	    strcpy(prof->mSafeCf.SelAuthMode, T("WPAPSKWPA2PSK"));
	}/*end -- (strcmp(wirelessPwd, T("")) == 0)*/
    }/*end -- (ProfInstIsFree(prof) == 0)*/
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
}
#endif

#if (UTT_SMART_UI == FYES)
/*新风格UI 不再提示按钮提交 改成直接提交不使用ajax*/
static void formEasyTaskNoticeEn_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *easyTaskNoticeEn = NULL;

    easyTaskNoticeEn = websGetVar(wp, T("enable"), T(""));/*不再提示复选框*/
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    EASYTASK_PRINTF("%s--%d:easyTaskWeb=%s\n", __func__, __LINE__, easyTaskNoticeEn);
    if (ProfInstIsFree(prof) == 0)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(easyTaskNoticeEn, T("on")) == 0)
	{
	    prof->easyTaskNoticeEn = FUN_ENABLE;/*开启提示*/
	}
	else
	{
	    prof->easyTaskNoticeEn = FUN_DISABLE;/*关闭提示*/
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
    websRedirect(wp, T("NoticEasyConfig_Sui.asp"));
    return;
}
#endif
/*
 * 不再提示开关提交函数
 */
static void formEasyTaskNoticeEn(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *easyTaskNoticeEn = NULL;

    easyTaskNoticeEn = websGetVar(wp, T("easyTaskNoticeEn"), T(""));/*不再提示复选框*/
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == 0)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(easyTaskNoticeEn, T("on")) == 0)
	{
	    prof->easyTaskNoticeEn = FUN_ENABLE;/*开启提示*/
	}
	else
	{
	    prof->easyTaskNoticeEn = FUN_DISABLE;/*关闭提示*/
	}/*end -- (strcmp(easyTaskNoticeEn, T("on")) == 0)*/

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }/*end -- (ProfInstIsFree(prof) == 0)*/
    return;
}

/*
 * 功能：易管理功能开启或关闭
 */
static void editEasyTaskState(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *easyTaskEn = NULL;

#if (UTT_SMART_UI == FYES)
    char *easyTaskNoticeEn = NULL;
    easyTaskEn = websGetVar(wp, T("easyManageEn"), T(""));
    easyTaskNoticeEn = websGetVar(wp, T("noticeEnable"), T(""));
#else
    easyTaskEn = websGetVar(wp, T("allowEasyManage"), T(""));/*启用易管理复选框*/
#endif
    
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == 0)
    {
#if (UTT_SMART_UI == FYES)
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(easyTaskNoticeEn, "on") == 0)
	{
	    prof->easyTaskNoticeEn = FUN_ENABLE;
	}
	else
	{
	    prof->easyTaskNoticeEn = FUN_DISABLE;
	}
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
#else
	/*开启或关闭易管理*/
	if (strcmp(easyTaskEn, T("on")) == 0)
	{
	    prof->head.active = TRUE;
	}
	else
	{
	    prof->head.active = FALSE;
	}
#endif
    }
}

/*
 * 易管理开启关闭,对应页面上的启用易管理
 */
static void formEasyTask(webs_t wp, char_t *path, char_t *query)
{
#if ((WLAN_WORK_TIME == FYES) && (UTT_SMART_UI != FYES))
    editWlanTimeConfig(wp, path, query, 1);/*设置无线时间*/
#endif
    editEasyTaskState(wp, path, query);/*设置易管理开关*/
#if ((SMART_QOS_FUN == FYES) && (UTT_SMART_UI != FYES))
    editSmartQosConfig(wp, path, query); /*设置流量控制*/
#endif
    nvramWriteCommit();
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, T("EasyManage_Sui.asp"));
#else
    websRedirect(wp, T("EasyManage.asp"));
#endif
    return;
}

/*
 * 易管理配置页面输出信息
 */
static int aspoutEasyTaskConfig(int eid, webs_t wp, int argc, char_t **argv)
{
#if (DHCP_SERVER == FYES)
#if 0
    int nodhcp = 1;/*连接方式为DHCP则不需要弹出提示*/
    FILE *fp;
    struct dhcpOfferedAddr {
        unsigned char hostname[16];
        unsigned char mac[16];
        unsigned long ip; 
        unsigned long expires;
    } lease;
    struct in_addr addr;
    char *dhcpip = NULL;/*保存装换后ip*/
#endif
#endif    
#if ((WIRELESS_BASE_CONFIG == FYES) || (WIRELESS == FYES))
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
#endif
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibTypeEt = MIB_PROF_EASY_TASK;
    EasyTaskProfile *profEt = NULL;
#if (WLAN_WORK_TIME == FYES)
    UINT32 daysTemp = 0U;
    char_t data[16];
#endif

#if ((WIRELESS_BASE_CONFIG == FYES) || (WIRELESS == FYES))
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if (ProfInstIsFree(prof) == PROFOK)
    {
#if (WIRELESS_BASE_CONFIG == FYES)
	websWrite(wp, T("wirelessSSID=\"%s\";"), prof->mBaseCf.SSID1);/*无线网络名称*/
#if (WLAN_WORK_TIME == FYES)
	daysTemp = prof->mBaseCf.day;
	Byte2Days(daysTemp, data);
	websWrite(wp, "var days = \"%s\";", data);/*日期*/
	websWrite(wp, "var weekdaytimestart = \"%s\";", prof->mBaseCf.weekdayTimeStart);/*工作日开始时间*/
	websWrite(wp, "var weekdaytimestop = \"%s\";", prof->mBaseCf.weekdayTimeStop);/*工作日结束时间*/
	websWrite(wp, "var weekendtimestart = \"%s\";", prof->mBaseCf.weekendTimeStart);/*周末开始时间*/
	websWrite(wp, "var weekendtimestop = \"%s\";", prof->mBaseCf.weekendTimeStop);/*周末结束时间*/
#endif
#endif
#if (WIRELESS == FYES)
	/*是否为无安全机制*/
	if ((strcmp(prof->mSafeCf.SelAuthMode, T("OPEN")) == 0) && (strcmp(prof->mSafeCf.ap.AuthMode.EncrypType, T("NONE")) == 0))
	{
	    websWrite(wp, T("wireLessPwd = \"\";"));
	    websWrite(wp, T("wireDisplay = 1;"));
	}
	else/*不是无安全机制*/
	{
	    /*是否为默认的安全机制，即第四种*/
	    if (strcmp(prof->mSafeCf.SelAuthMode, T("WPAPSKWPA2PSK")) == 0)
	    {
		websWrite(wp, T("wireLessPwd = \"%s\";"), prof->mSafeCf.ap.AuthMode.pskPsswd);
		websWrite(wp, T("wireDisplay = 1;"));/*wireDisplay为0页面不显示无线输入框*/
	    }
	}
#endif
    }/*end -- (ProfInstIsFree(prof) == 0)*/
#endif
#if (DHCP_SERVER == FYES)
#if 0
    /*判断当前连接ip是否由DHCP分配*/
    doSystem(T("killall -q -USR1 udhcpd"));
    fp = fopen(T("/var/udhcpd.leases"), T("r"));
    if (NULL != fp)
    {
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease))
	{
	    addr.s_addr = lease.ip;
	    EASYTASK_PRINTF("%s--%d:addr=%s,ipaddr=%s\n", __func__, __LINE__, inet_ntoa(addr), wp->ipaddr);
	    dhcpip = inet_ntoa(addr);
	    if ((dhcpip != NULL) && (strcmp(wp->ipaddr, dhcpip) == 0))
	    {
		nodhcp = 0;
		break;
	    }
	}
	fclose(fp);
    }/*end -- (NULL != fp)*/
    websWrite(wp, T("nodhcp = %d;"), nodhcp);
#endif
#endif
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    profEt = (EasyTaskProfile*)ProfGetInstPointByIndex(mibTypeEt, 0);
    if (ProfInstIsFree(profIf) == PROFOK)
    {
	websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*连接方式*/
	websWrite(wp, T("mask = \"%N\";"), profIf->ether.sta.nm);/*子网掩码*/
	websWrite(wp, T("ipAddress = \"%N\";"), profIf->ether.sta.ip);/*ip地址*/
	websWrite(wp, T("gateWay = \"%N\";"), profIf->ether.sta.gw);/*网关地址*/
	websWrite(wp, T("priDns = \"%N\";"), profIf->ether.sta.pd);/*DNS服务器*/
	websWrite(wp, T("secDns = \"%N\";"), profIf->ether.sta.sd);
	websWrite(wp, T("pppoeUsername = \"%s\";"), profIf->ether.pppoe.user);/*PPPoE用户名*/
	websWrite(wp, T("pppoePwd = \"%s\";"), profIf->ether.pppoe.passwd);/*PPPoE密码*/
    }
    if (ProfInstIsFree(profSys) == PROFOK)
    {
	websWrite(wp, T("stopChanConnType = %d;"), profSys->isDefaultConf);/*非出厂设置不需要根据检测结构改变连接方式*/
    }
    if (ProfInstIsFree(profEt) == PROFOK)
    {
	/*判断易管理是否开启*/
	if (profEt->head.active == TRUE)
	{
	    websWrite(wp, T("var easyTaskEn = \"on\";"));/*启用易管理开关开启*/
	}
	else
	{
	    websWrite(wp, T("var easyTaskEn = \"off\";"));/*启用易管理开关关闭*/
	}
#if (WLAN_WORK_TIME == FYES)
	websWrite(wp, "var safeToNet = %d;", profEt->safeToNet);/*绿色上网*/
#endif
	getTheErrorMsg(eid, wp, argc, argv);
    }
    return 0;
}


/*
 * 功能：断线通告页面输出所需参数
 * 返回值：0 -- 成功 1 -- 失败
 */
static int aspoutEasyTaskNotice(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == 0)
    {
	websWrite(wp, T("easyTaskOn = %d;"), prof->head.active);/*易管理开关*/
	websWrite(wp, T("enabled = %d;"), prof->easyTaskNoticeEn);/*不再提示按钮状态*/
	if (get_phylink_status(1) == 0)
	{
	    websWrite(wp, T("ConnStatus = %d;"), DISCONNECT);/*当前WAN口状态*/
	}
	else
	{
	    websWrite(wp, T("ConnStatus = %d;"), prof->ConnStatus);/*WAN口连接状态*/
	}
    }
    return 0;
}

/*
 * 易管理页面输出启用易管理开关状态
 */
static int aspoutEasyTask(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
#if ((WIRELESS == FYES) && (UTT_SMART_UI != FYES))
    MibProfileType mibTypeWl = MIB_PROF_WIRELESS;
    WirelessProfile *profWl = NULL;
    UINT32 daysTemp = 0U;
    char_t data[16];
#endif
    
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
#if ((WIRELESS == FYES) && (UTT_SMART_UI != FYES))
    profWl = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeWl, 0);
    if (ProfInstIsFree(profWl) == PROFOK)
    {
	websWrite(wp,"WrlessEnables=%d;", profWl->head.active);
#if (WLAN_WORK_TIME == FYES)
	daysTemp = profWl->mBaseCf.day;
	Byte2Days(daysTemp, data);
	websWrite(wp, "var days = \"%s\";", data);/*日期*/
	websWrite(wp, "var weekdaytimestarts=\"%s\";", profWl->mBaseCf.weekdayTimeStart);
	websWrite(wp, "var weekdaytimestops=\"%s\";", profWl->mBaseCf.weekdayTimeStop);
	websWrite(wp, "var weekendtimestarts=\"%s\";", profWl->mBaseCf.weekendTimeStart);
	websWrite(wp, "var weekendtimestops=\"%s\";", profWl->mBaseCf.weekendTimeStop);
#endif
    }
#endif
    if (ProfInstIsFree(prof) == 0)
    {
	/*判断易管理是否开启*/
	if (prof->head.active == TRUE)
	{
	    websWrite(wp, T("var easyTaskEn = \"on\";"));/*启用易管理开关开启*/
	}
	else
	{
	    websWrite(wp, T("var easyTaskEn = \"off\";"));/*启用易管理开关关闭*/
	}
	if (prof->easyTaskNoticeEn == FUN_ENABLE)
	{
	    websWrite(wp, T("easyTaskNoticeEn = \"on\";"));
	}
	else
	{
	    websWrite(wp, T("easyTaskNoticeEn = \"off\";"));
	}
    }
 #if (UTT_SMART_UI == FYES)
      getTheRightMsg(eid,wp,argc,argv);
 #endif
    return 0;
}
   
/*
 * 功能：把网页上调用的asp函数转换成该文件中定义的函数
 */
void formDefineEasyTask(void) 
{
	/*修改管理员admin的密码*/
	websFormDefine(T("formUserEasyTask"), formUserEasyTask);
	/*易管理开启关闭*/
	websFormDefine(T("formEasyTask"), formEasyTask);
	/*不再提示开关提交函数*/
	websFormDefine(T("formEasyTaskNoticeEn"), formEasyTaskNoticeEn);
	/*断线通告页面输出所需参数*/
	websAspDefine(T("aspoutEasyTaskNotice"), aspoutEasyTaskNotice);
	/*向页面输出需要使用ajax更新的信息*/
	websFormDefine(T("formStatForEasyTask"), formStatForEasyTask);
#if (UTT_SMART_UI == FYES)
	/*新风格UI 输出WAN口状态*/
	websFormDefine(T("formStatForWan_Sui"), formStatForWan_Sui);
	/*新风格UI 重新检测WAN口连接状态*/
	websFormDefine(T("formWanReCheck_Sui"), formWanReCheck_Sui);
	/*不再提示开关提交函数,新风格UI*/
	websFormDefine(T("formEasyTaskNoticeEn_Sui"), formEasyTaskNoticeEn_Sui);
#endif
	/*管理页面输出启用易管理开关状态*/
	websAspDefine(T("aspoutEasyTask"), aspoutEasyTask);
	/*易管理配置页面输出信息*/
	websAspDefine(T("aspoutEasyTaskConfig"), aspoutEasyTaskConfig);
	/*停止WAN口智能检测*/
	websFormDefine(T("formStopWanCheck"), formStopWanCheck);
	/*关闭出厂配置标志,设置相应状态*/
	websFormDefine(T("formCloseDefConf"), formCloseDefConf);
	/*易管理配置页面保存*/
	websFormDefine(T("formEasyTaskConfig"), formEasyTaskConfig);
	/*易管理页面智能检测*/
	websFormDefine(T("formEasyTaskReCheck"), formEasyTaskReCheck);
	/*显示端口数量*/
	websFormDefine(T("formGetPortCount"), formGetPortCount);
}
#endif
