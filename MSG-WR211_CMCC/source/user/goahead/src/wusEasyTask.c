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
 * ���ܣ��޸Ĺ���Աadmin������
 */
static void formUserEasyTask(webs_t wp, char_t *path, char_t *query)
{
    char_t *wir = NULL;
    wir = websGetVar(wp, T("wir"), T(""));/*�����������������*/
#if (ADMIN_CONF == FYES)
    setSysAdm(wp, path, query);/*�������룬�޷���ֵ*/
#endif
    /*����ԭҳ����������������ѡ����תҳ��������������*/
    if (strcmp(wir, T("0")) == 0)
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=0"));/*ҳ����ת*/
    }
    else
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=1"));/*ҳ����ת*/
    }/*end -- (strcmp(wir, T("0")) == 0)*/
}

/*
 * ���ܣ�ֹͣWAN�����ܼ��
 */
static void formStopWanCheck(webs_t wp, char_t *path, char_t *query)
{
    doSystem(T("killall uttLinkCheck >/dev/null 2>&1"));/*kill������*/
    wanConnStatsChange(CHECK_INTERRUPT);/*��WAN��״̬����Ϊ����ж�*/
}

/*
 * ���ܣ��رճ������ñ�־,������Ӧ״̬
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
	/*��isDefaultConf�ɳ�����Ϊ�ǳ���*/
	profSys->isDefaultConf = 1;
    }
}

/*
 * ���ܣ���Ӧҳ���ϵ�"������������������"
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
 * ���ܣ����WAN������״̬
 * ����ֵ��0--�ɹ� 1--ʧ��
 */
static int outPutWanStat(webs_t wp, SystemProfile *profSys)
{
    int i = 0;
    int retVal = 0;
    for (i=0;i<profSys->wanIfCount;i++)
    {
	/*����WAN�������ӻ�Ͽ�*/
	if (get_phylink_status(i + 1) == 1)
	{
	    websWrite(wp, T("wanStat[%d] = \"UP\";"), i);/*������*/
	}
	else/*δ����*/
	{
	    websWrite(wp, T("wanStat[%d] = \"DOWN\";"), i);
	}
	websWrite(wp, T("wanPort[%d] = \"WAN%d\";"), i, i + 1);
    }/*end -- (i=0;i<profSys->wanIfCount;i++)*/
    return retVal;
}

#if (WLAN_WORK_TIME == FYES)
/*
 * ��ɫ�������ܱ���
 */
static void editSafeToNetState(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    char *safeToNet = NULL;/*��ɫ����*/

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
/*�·��UI ���WAN��״̬*/
static void formStatForWan_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
     
    /*ajaxͷ*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));


    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);

    if (ProfInstIsFree(prof) == 0)
    {
	if (get_phylink_status(1) == 0)
	{
	    websWrite(wp, T("connStatus = %d;"), DISCONNECT);/*��ǰWAN��״̬*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*���ӷ�ʽ*/
	}
	else if ((prof->wanLinkStates & (1U << 1)) > 0U)
	{
	    websWrite(wp, T("connStatus = %d;"), CONNECTED);/*��ǰWAN��״̬*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*���ӷ�ʽ*/
	}
	else
	{
	    websWrite(wp, T("connStatus = %d;"), prof->ConnStatus);/*��ǰWAN��״̬*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(prof->wanMode, ethConnModeEnum));
	}
    }
    
    websDone(wp, 200);/*ajaxβ*/
}
#endif


/*
 * ���ܣ���ҳ�������Ҫʹ��ajax���µ���Ϣ
 *	ÿ����ִ��һ��
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
     
    /*ajaxͷ*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibTypeSys, 0);
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 1);

    if (ProfInstIsFree(prof) == 0)
    {
	if (get_phylink_status(1) == 0)
	{
	    websWrite(wp, T("connStatus = %d;"), DISCONNECT);/*��ǰWAN��״̬*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*���ӷ�ʽ*/
	}
	else if ((prof->wanLinkStates & (1U << 1)) > 0U)
	{
	    websWrite(wp, T("connStatus = %d;"), CONNECTED);/*��ǰWAN��״̬*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*���ӷ�ʽ*/
	}
	else
	{
	    websWrite(wp, T("connStatus = %d;"), prof->ConnStatus);/*��ǰWAN��״̬*/
	    websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(prof->wanMode, ethConnModeEnum));
	}
    }/*end -- (ProfInstIsFree(prof) == 0)*/
    if (ProfInstIsFree(profSys) == 0)
    {
	websWrite(wp, T("n_totalrecs = %d;"), profSys->wanIfCount);/*WAN������*/
	websWrite(wp, T("totalrecs = %d;"), profSys->lanPortCount);/*LAN������*/
	/*��ʾÿ��LAN��״̬*/
	for (i=0;i<profSys->lanPortCount;i++)
	{
	    if (get_lan_status(i + 1) == 1)
	    {
		websWrite(wp, T("lanStat[%d] = \"UP\";"), i);/*������*/
	    }
	    else
	    {
		websWrite(wp, T("lanStat[%d] = \"DOWN\";"), i);/*δ����*/
	    }/*end -- (get_lan_status(i + 1) == 1)*/
	    websWrite(wp, T("lanPort[%d] = \"LAN%d\";"), i, i + 1);/*LAN������*/
	}/*end -- (i=0;i<profSys->lanPortCount;i++)*/
	outPutWanStat(wp, profSys);/*��ʾWAN��״̬*/
    }/*end -- (ProfInstIsFree(profSys) == 0)*/
    
    websDone(wp, 200);/*ajaxβ*/
}

/*
 * ���ܣ��׹�������ҳ�汣��
 */
static void formEasyTaskConfig(webs_t wp, char_t *path, char_t *query)
{ 
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
#if (WIRELESS_BASE_CONFIG == FYES)
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeWl = MIB_PROF_WIRELESS;
    WirelessProfile *profWl= NULL;
    char_t *ssid = NULL;/*������������*/
#endif
    int wanIndex = 1;
    char_t *ctype = NULL, *wir = NULL, *wireDisplay = NULL;
    
    ctype = websGetVar(wp, T("connectionType"), T(""));/*��������*/
    wir = websGetVar(wp, T("wir"), T(""));/*�����������ʾ*/
    wireDisplay = websGetVar(wp, T("wireDisplay"), T(""));
#if (WIRELESS_BASE_CONFIG == FYES)
    ssid = websGetVar(wp, T("ssid"), T(""));
#endif
         
    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIf, wanIndex);

    /*����WAN��*/
    if(setWanx(wp, wanIndex, T(""), ctype, profIf) == 0) 
    {
	initWan(wanIndex);
    }/*end -- (setWanx(wp, wanIndex, T(""), ctype, profIf) == 0)*/

    editEasyTaskState(wp, path, query);

    /*�ɶ���ҳ����ת��ҳ��Ϊ"0", �г���ҳ����ת��Ϊ"1"*/
    if (strcmp(wir, T("1")) == 0)
    {
#if (WIRELESS == FYES)
	editSafeToNetState(wp, path, query);
#endif
	/*�޷���ʾ�����ְ�ȫ����Ϊ"0"������������һ��Ϊ"0"����ҳ�治����ʾ���������*/
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
	websRedirect(wp, T("EasyManageConfig.asp?wir=1"));/*ҳ����ת*/
    }
    else
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=0"));/*ҳ����ת*/
    }/*end -- (strcmp(wir, T("1")) == 0)*/
#if (WIRELESS_BASE_CONFIG == FYES)
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
#endif
    nvramWriteCommit();/*����*/
    return;
}

/*
 * ���ܣ���ʾ�˿�����
 */
static void formGetPortCount(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *prof = NULL;

    prof = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0); 
    /*�׹��������Ҫ��ʾ�ӿ�״̬*/
    websWrite(wp, T("var wanIfCount=%d;"), prof->wanIfCount);/*wan����*/
    websWrite(wp, T("var numPorts = %u;\n"), prof->lanPortCount);/*lan����*/
}


#if (UTT_SMART_UI == FYES)
/*�·��UI ���¼��WAN������״̬*/
static void formWanReCheck_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    prof = (EasyTaskProfile *)ProfGetInstPointByIndex(mibType, 0);
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    if (ProfInstIsFree(prof) == PROFOK)
    {
	doSystem(T("killall uttLinkCheck >/dev/null 2>&1"));/*kill������*/
	doSystem("uttLinkCheck %s &", getWanIfName(1));
    }
    websDone(wp, 200); 
}
#endif

/*
 * ���ܣ��׹���ҳ�����ܼ��
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
	doSystem(T("killall uttLinkCheck >/dev/null 2>&1"));/*kill������*/
	doSystem("uttLinkCheck %s &", getWanIfName(1));
    }
    if (strcmp(wir, T("0")) == 0)
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=0"));/*ҳ����ת*/
    }
    else
    {
	websRedirect(wp, T("EasyManageConfig.asp?wir=1"));/*ҳ����ת*/
    }
}

#if (WIRELESS == FYES)
/*
 * ���ܣ�������������
 */
static void setAPSecurity(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    WirelessProfile *prof= NULL;
    char *wirelessPwd = NULL;
    
    wirelessPwd = websGetVar(wp, T("wirelessPwd"), T(""));/*��������*/
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if (ProfInstIsFree(prof) == 0)
    {
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibType, profAction, 0, &profhead);
	clearRadiusSet(prof);
	/*���������ް�ȫ����*/
	if (strcmp(wirelessPwd, T("")) == 0)
	{
	    strcpy(prof->mSafeCf.SelAuthMode, T("OPEN"));
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType, T("NONE"));
	    prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	}
	else/*����������Ĭ�ϵ����ְ�ȫ����*/
	{
	    strcpy(prof->mSafeCf.ap.AuthMode.EncrypType, T("TKIPAES"));
	    prof->mSafeCf.ap.AuthMode.RekeyInterval = 3600U;
	    prof->mSafeCf.ap.AuthMode.IEEE8021X = 0;
	    strcpy(prof->mSafeCf.ap.AuthMode.pskPsswd, wirelessPwd);/*������������ΪĬ��*/
	    strcpy(prof->mSafeCf.SelAuthMode, T("WPAPSKWPA2PSK"));
	}/*end -- (strcmp(wirelessPwd, T("")) == 0)*/
    }/*end -- (ProfInstIsFree(prof) == 0)*/
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
}
#endif

#if (UTT_SMART_UI == FYES)
/*�·��UI ������ʾ��ť�ύ �ĳ�ֱ���ύ��ʹ��ajax*/
static void formEasyTaskNoticeEn_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *easyTaskNoticeEn = NULL;

    easyTaskNoticeEn = websGetVar(wp, T("enable"), T(""));/*������ʾ��ѡ��*/
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    EASYTASK_PRINTF("%s--%d:easyTaskWeb=%s\n", __func__, __LINE__, easyTaskNoticeEn);
    if (ProfInstIsFree(prof) == 0)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(easyTaskNoticeEn, T("on")) == 0)
	{
	    prof->easyTaskNoticeEn = FUN_ENABLE;/*������ʾ*/
	}
	else
	{
	    prof->easyTaskNoticeEn = FUN_DISABLE;/*�ر���ʾ*/
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
 * ������ʾ�����ύ����
 */
static void formEasyTaskNoticeEn(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    char *easyTaskNoticeEn = NULL;

    easyTaskNoticeEn = websGetVar(wp, T("easyTaskNoticeEn"), T(""));/*������ʾ��ѡ��*/
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == 0)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	if (strcmp(easyTaskNoticeEn, T("on")) == 0)
	{
	    prof->easyTaskNoticeEn = FUN_ENABLE;/*������ʾ*/
	}
	else
	{
	    prof->easyTaskNoticeEn = FUN_DISABLE;/*�ر���ʾ*/
	}/*end -- (strcmp(easyTaskNoticeEn, T("on")) == 0)*/

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }/*end -- (ProfInstIsFree(prof) == 0)*/
    return;
}

/*
 * ���ܣ��׹����ܿ�����ر�
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
    easyTaskEn = websGetVar(wp, T("allowEasyManage"), T(""));/*�����׹���ѡ��*/
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
	/*������ر��׹���*/
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
 * �׹������ر�,��Ӧҳ���ϵ������׹���
 */
static void formEasyTask(webs_t wp, char_t *path, char_t *query)
{
#if ((WLAN_WORK_TIME == FYES) && (UTT_SMART_UI != FYES))
    editWlanTimeConfig(wp, path, query, 1);/*��������ʱ��*/
#endif
    editEasyTaskState(wp, path, query);/*�����׹�����*/
#if ((SMART_QOS_FUN == FYES) && (UTT_SMART_UI != FYES))
    editSmartQosConfig(wp, path, query); /*������������*/
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
 * �׹�������ҳ�������Ϣ
 */
static int aspoutEasyTaskConfig(int eid, webs_t wp, int argc, char_t **argv)
{
#if (DHCP_SERVER == FYES)
#if 0
    int nodhcp = 1;/*���ӷ�ʽΪDHCP����Ҫ������ʾ*/
    FILE *fp;
    struct dhcpOfferedAddr {
        unsigned char hostname[16];
        unsigned char mac[16];
        unsigned long ip; 
        unsigned long expires;
    } lease;
    struct in_addr addr;
    char *dhcpip = NULL;/*����װ����ip*/
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
	websWrite(wp, T("wirelessSSID=\"%s\";"), prof->mBaseCf.SSID1);/*������������*/
#if (WLAN_WORK_TIME == FYES)
	daysTemp = prof->mBaseCf.day;
	Byte2Days(daysTemp, data);
	websWrite(wp, "var days = \"%s\";", data);/*����*/
	websWrite(wp, "var weekdaytimestart = \"%s\";", prof->mBaseCf.weekdayTimeStart);/*�����տ�ʼʱ��*/
	websWrite(wp, "var weekdaytimestop = \"%s\";", prof->mBaseCf.weekdayTimeStop);/*�����ս���ʱ��*/
	websWrite(wp, "var weekendtimestart = \"%s\";", prof->mBaseCf.weekendTimeStart);/*��ĩ��ʼʱ��*/
	websWrite(wp, "var weekendtimestop = \"%s\";", prof->mBaseCf.weekendTimeStop);/*��ĩ����ʱ��*/
#endif
#endif
#if (WIRELESS == FYES)
	/*�Ƿ�Ϊ�ް�ȫ����*/
	if ((strcmp(prof->mSafeCf.SelAuthMode, T("OPEN")) == 0) && (strcmp(prof->mSafeCf.ap.AuthMode.EncrypType, T("NONE")) == 0))
	{
	    websWrite(wp, T("wireLessPwd = \"\";"));
	    websWrite(wp, T("wireDisplay = 1;"));
	}
	else/*�����ް�ȫ����*/
	{
	    /*�Ƿ�ΪĬ�ϵİ�ȫ���ƣ���������*/
	    if (strcmp(prof->mSafeCf.SelAuthMode, T("WPAPSKWPA2PSK")) == 0)
	    {
		websWrite(wp, T("wireLessPwd = \"%s\";"), prof->mSafeCf.ap.AuthMode.pskPsswd);
		websWrite(wp, T("wireDisplay = 1;"));/*wireDisplayΪ0ҳ�治��ʾ���������*/
	    }
	}
#endif
    }/*end -- (ProfInstIsFree(prof) == 0)*/
#endif
#if (DHCP_SERVER == FYES)
#if 0
    /*�жϵ�ǰ����ip�Ƿ���DHCP����*/
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
	websWrite(wp, T("connType = \"%s\";"), MibGetStrByEnum(profIf->ether.connMode, ethConnModeEnum));/*���ӷ�ʽ*/
	websWrite(wp, T("mask = \"%N\";"), profIf->ether.sta.nm);/*��������*/
	websWrite(wp, T("ipAddress = \"%N\";"), profIf->ether.sta.ip);/*ip��ַ*/
	websWrite(wp, T("gateWay = \"%N\";"), profIf->ether.sta.gw);/*���ص�ַ*/
	websWrite(wp, T("priDns = \"%N\";"), profIf->ether.sta.pd);/*DNS������*/
	websWrite(wp, T("secDns = \"%N\";"), profIf->ether.sta.sd);
	websWrite(wp, T("pppoeUsername = \"%s\";"), profIf->ether.pppoe.user);/*PPPoE�û���*/
	websWrite(wp, T("pppoePwd = \"%s\";"), profIf->ether.pppoe.passwd);/*PPPoE����*/
    }
    if (ProfInstIsFree(profSys) == PROFOK)
    {
	websWrite(wp, T("stopChanConnType = %d;"), profSys->isDefaultConf);/*�ǳ������ò���Ҫ���ݼ��ṹ�ı����ӷ�ʽ*/
    }
    if (ProfInstIsFree(profEt) == PROFOK)
    {
	/*�ж��׹����Ƿ���*/
	if (profEt->head.active == TRUE)
	{
	    websWrite(wp, T("var easyTaskEn = \"on\";"));/*�����׹����ؿ���*/
	}
	else
	{
	    websWrite(wp, T("var easyTaskEn = \"off\";"));/*�����׹����عر�*/
	}
#if (WLAN_WORK_TIME == FYES)
	websWrite(wp, "var safeToNet = %d;", profEt->safeToNet);/*��ɫ����*/
#endif
	getTheErrorMsg(eid, wp, argc, argv);
    }
    return 0;
}


/*
 * ���ܣ�����ͨ��ҳ������������
 * ����ֵ��0 -- �ɹ� 1 -- ʧ��
 */
static int aspoutEasyTaskNotice(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;

    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (ProfInstIsFree(prof) == 0)
    {
	websWrite(wp, T("easyTaskOn = %d;"), prof->head.active);/*�׹�����*/
	websWrite(wp, T("enabled = %d;"), prof->easyTaskNoticeEn);/*������ʾ��ť״̬*/
	if (get_phylink_status(1) == 0)
	{
	    websWrite(wp, T("ConnStatus = %d;"), DISCONNECT);/*��ǰWAN��״̬*/
	}
	else
	{
	    websWrite(wp, T("ConnStatus = %d;"), prof->ConnStatus);/*WAN������״̬*/
	}
    }
    return 0;
}

/*
 * �׹���ҳ����������׹�����״̬
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
	websWrite(wp, "var days = \"%s\";", data);/*����*/
	websWrite(wp, "var weekdaytimestarts=\"%s\";", profWl->mBaseCf.weekdayTimeStart);
	websWrite(wp, "var weekdaytimestops=\"%s\";", profWl->mBaseCf.weekdayTimeStop);
	websWrite(wp, "var weekendtimestarts=\"%s\";", profWl->mBaseCf.weekendTimeStart);
	websWrite(wp, "var weekendtimestops=\"%s\";", profWl->mBaseCf.weekendTimeStop);
#endif
    }
#endif
    if (ProfInstIsFree(prof) == 0)
    {
	/*�ж��׹����Ƿ���*/
	if (prof->head.active == TRUE)
	{
	    websWrite(wp, T("var easyTaskEn = \"on\";"));/*�����׹����ؿ���*/
	}
	else
	{
	    websWrite(wp, T("var easyTaskEn = \"off\";"));/*�����׹����عر�*/
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
 * ���ܣ�����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 */
void formDefineEasyTask(void) 
{
	/*�޸Ĺ���Աadmin������*/
	websFormDefine(T("formUserEasyTask"), formUserEasyTask);
	/*�׹������ر�*/
	websFormDefine(T("formEasyTask"), formEasyTask);
	/*������ʾ�����ύ����*/
	websFormDefine(T("formEasyTaskNoticeEn"), formEasyTaskNoticeEn);
	/*����ͨ��ҳ������������*/
	websAspDefine(T("aspoutEasyTaskNotice"), aspoutEasyTaskNotice);
	/*��ҳ�������Ҫʹ��ajax���µ���Ϣ*/
	websFormDefine(T("formStatForEasyTask"), formStatForEasyTask);
#if (UTT_SMART_UI == FYES)
	/*�·��UI ���WAN��״̬*/
	websFormDefine(T("formStatForWan_Sui"), formStatForWan_Sui);
	/*�·��UI ���¼��WAN������״̬*/
	websFormDefine(T("formWanReCheck_Sui"), formWanReCheck_Sui);
	/*������ʾ�����ύ����,�·��UI*/
	websFormDefine(T("formEasyTaskNoticeEn_Sui"), formEasyTaskNoticeEn_Sui);
#endif
	/*����ҳ����������׹�����״̬*/
	websAspDefine(T("aspoutEasyTask"), aspoutEasyTask);
	/*�׹�������ҳ�������Ϣ*/
	websAspDefine(T("aspoutEasyTaskConfig"), aspoutEasyTaskConfig);
	/*ֹͣWAN�����ܼ��*/
	websFormDefine(T("formStopWanCheck"), formStopWanCheck);
	/*�رճ������ñ�־,������Ӧ״̬*/
	websFormDefine(T("formCloseDefConf"), formCloseDefConf);
	/*�׹�������ҳ�汣��*/
	websFormDefine(T("formEasyTaskConfig"), formEasyTaskConfig);
	/*�׹���ҳ�����ܼ��*/
	websFormDefine(T("formEasyTaskReCheck"), formEasyTaskReCheck);
	/*��ʾ�˿�����*/
	websFormDefine(T("formGetPortCount"), formGetPortCount);
}
#endif
