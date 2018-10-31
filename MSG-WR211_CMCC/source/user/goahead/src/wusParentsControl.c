/*
 *	wusParentsControl.c -- Parents Control Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "translate.h"
#include "ifmisc.h"
#include <net/if_arp.h>



#if (PARENTS_CONTROL == FYES)

#define DBGPRINT_PARENTS(fmt, args...)

/***********************************************************************
 * 函 数 名：compareParentsOrChildMac
 * 功能描述：插入函数
 * 输入参数：macAddr : 需要比较的MAC即页面输入MAC
 *	     flag :  0 与家长MAC地址比较
 *		     1 与所有小孩MAC地址比较
 * 输出参数： ret: 0  与家长或小孩MAC不同，可以输入
 *		   1  与家长或消耗MAC相同，不可输入
 * 创建日期：2013-05-13
 * 修改日期：
 * 作者：   
 * 附加说明：
 **********************************************************************/
static int compareParentsOrChildMac(MacAddr *macAddr, int flag) 
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    int ret = 0;
    int min, max, i;
    const char *ErrMsg = NULL;

    if (flag == 0) {
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0U);
    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0U)) {
	if(macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac1)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac2)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac3)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac4)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac5)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac6)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac7)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac8)) == 0) {
	    ret = 1;
	} else if (macAddrCmp(macAddr, &(profSys->parentControlEn.parentMac9)) == 0) {
	    ret = 1;
	}
	if(ret == 1)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_PARENTS_CHILD_MAC_NOT_SAME); /*maybe need change*/
	    setTheErrorMsg(ErrMsg);
	}

    }
    } else if(flag == 1) {
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(macAddrCmp(macAddr, &(prof->mac)) == 0) {
		ret = 1;
		ErrMsg = getTransDataForC(C_LANG_INDEX_PARENTS_CHILD_MAC_NOT_SAME); /*maybe need change*/
		setTheErrorMsg(ErrMsg);
		break;
	    }
	}
    }
    }
    DBGPRINT_PARENTS("%s-%d: ret =%d\n", __func__, __LINE__,ret);
    return ret;
}
/***********************************************************************
 * 函 数 名：formParentsControlGlobalConfig
 * 功能描述：插入函数
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：   
 * 附加说明：
 **********************************************************************/
static void formParentsControlGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;
    MacAddr macAddr;
    char *Enable = NULL;
    char *macStr1=NULL,*macStr2=NULL,*macStr3=NULL,*macStr4=NULL,*macStr5=NULL,*macStr6=NULL,*macStr7=NULL,*macStr8=NULL,*macStr9=NULL;

    /* get web value */
    Enable = websGetVar(wp, "AllowControlEnable", T(""));
    macStr1 = websGetVar(wp, "parentpc1", T(""));
    macStr2 = websGetVar(wp, "parentpc2", T(""));
    macStr3 = websGetVar(wp, "parentpc3", T(""));
    macStr4 = websGetVar(wp, "parentpc4", T(""));
    macStr5 = websGetVar(wp, "parentpc5", T(""));
    macStr6 = websGetVar(wp, "parentpc6", T(""));
    macStr7 = websGetVar(wp, "parentpc7", T(""));
    macStr8 = websGetVar(wp, "parentpc8", T(""));
    macStr9 = websGetVar(wp, "parentpc9", T(""));
    DBGPRINT_PARENTS("%s-%d: Enable =%s,mac1 =%s,mac2 =%s,mac3 =%s,mac4 =%s\n", __func__, __LINE__,Enable,macStr1,macStr2,macStr3,macStr4);
    
    /* get SystemProfile */
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    
    if(strcmp(Enable, "on") == 0) {
        enable = FUN_ENABLE;
    }
    profSys->parentControlEn.parentsEn = enable;
    DBGPRINT_PARENTS("%s-%d: profSys->parentControlEn.parentsEn =%d\n", __func__, __LINE__,profSys->parentControlEn.parentsEn);
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr1, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac1 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr2, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac2 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr3, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac3 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr4, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac4 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr5, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac5 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr6, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac6 = macAddr;
    }memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr7, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac7 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr8, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac8 = macAddr;
    }
    memset(&macAddr, 0, sizeof(macAddr));
    converMac12To6Byte(macStr9, &macAddr);
    if (compareParentsOrChildMac(&macAddr, 1) == 0) {
	profSys->parentControlEn.parentMac9 = macAddr;
    }
    DBGPRINT_PARENTS("%s-%d: after convert mac1 =%s,mac2 =%s,mac3 =%s,mac4 =%s\n", __func__, __LINE__,
	    profSys->parentControlEn.parentMac1.addr,profSys->parentControlEn.parentMac2.addr,
	    profSys->parentControlEn.parentMac3.addr,profSys->parentControlEn.parentMac4.addr);
    
    /* update systemProfile
     * and write to flash
     */
#if 1
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#endif
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "ParentsControlSui.asp");/*页面跳转*/
#else
    websRedirect(wp, "ParentsControl.asp");/*页面跳转*/
#endif
    return;

}

/***********************************************************************
 * 函 数 名：isTimeGrpReredByParentsControl
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
extern int isTimeGrpReredByParentsControl(char *timeGrpName)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    int ret = 0;
    int max, min, i;
    
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0) {
	    if(prof->timeGrp !='\0')
	    {
		if(strcmp(prof->timeGrp, timeGrpName) == 0)
		{/*被引用了*/
		    ret = 1;
		    break;
		}
	    }
	}
    }
    return ret;
}
/***********************************************************************
 * 函 数 名：formParentsControlDelAll
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static void formParentsControlDelAll(webs_t wp, char_t *path, char_t *query)
{ 
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    struProfAlloc *profList  = NULL;

    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back = 1;
	    }
	    ProfDelInstByIndex(mibType, i);
	}
    }
#if 1
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#endif
#if (UTT_SMART_UI == FYES)
   setSuccessTips();
    websRedirect(wp, "ParentsControlSui.asp");
#else
    websRedirect(wp, "ParentsControl.asp");
#endif
}

/***********************************************************************
 * 函 数 名：formParentsControlDel
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static void formParentsControlDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    struProfAlloc *profList  = NULL;
    int back = 0;
    char_t *userName=NULL,*tmp,*ptr;
	const char* ErrMsg = NULL;

    a_assert(wp);
    userName = websGetVar(wp,T("delstr"),T("")); 
    if(!strcmp(userName,T(""))) {   
		//ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME);
		setTheErrorMsg(T(""));
		goto out;
    }
    DBGPRINT_PARENTS("%s-%d: del username =%s\n", __func__, __LINE__,userName);
    tmp=strtok_r(userName,UTT_SPLIT_SIGN_STR,&ptr);
    while(tmp!=NULL) {
	if(back == 0){
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	    back = 1;
	}
	ProfDelInstByName(mibType, tmp); 
	tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }
#if 1
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#endif
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp,"ParentsControlSui.asp");
#else
    websRedirect(wp,"ParentsControl.asp");
#endif
}
/***********************************************************************
 * 函 数 名：getParentsControlGlobalInfo()
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static int getParentsControlGlobalInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    /* get SystemProfile */
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if(profSys != NULL) {
	if(profSys->parentControlEn.parentsEn == FUN_ENABLE) {
	    websWrite(wp,"var AllowControlEnables = \"%s\";",T("on"));
	} else {
	    websWrite(wp,"var AllowControlEnables = \"%s\";",T("off"));
	}
	websWrite(wp,"var parentsMac1 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac1));
	websWrite(wp,"var parentsMac2 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac2));
	websWrite(wp,"var parentsMac3 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac3));
	websWrite(wp,"var parentsMac4 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac4));
	websWrite(wp,"var parentsMac5 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac5));
	websWrite(wp,"var parentsMac6 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac6));
	websWrite(wp,"var parentsMac7 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac7));
	websWrite(wp,"var parentsMac8 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac8));
	websWrite(wp,"var parentsMac9 = \"%s\";",converMac6To12Byte(profSys->parentControlEn.parentMac9));
	websWrite(wp,"\r\n");
    }
    return 0;
}
/***********************************************************************
 * 函 数 名： getOneParentsControlInfo
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static int getOneParentsControlInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    char_t *edit_name; 
    const char *ErrMsg = NULL;
    int i = 0, num = 0;

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR); /*need change*/
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if(edit_name == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_CANT_NULL); /*need change*/
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    prof = (ParentsControlProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF); /*need change*/
		setTheErrorMsg(ErrMsg);
    } else {
	websWrite(wp,"var userName = \"%s\";",prof->head.name);
	websWrite(wp,"var Enable = \"%d\";",prof->head.active);
	websWrite(wp,"var userMac = \"%s\";",converMac6To12Byte(prof->mac));
#if (TIME_GRP == FYES)
	websWrite(wp,"var timeGrp = \"%s\";",prof->timeGrp);
#endif
	websWrite(wp,"var userUrl = new Array;");
	for(i=0;i< MAX_ALLOW_URL_ITEM;i++)
	{
	    if(strlen(prof->allowURL[i].url) != 0U) {
		websWrite(wp,"userUrl[%d] = \"%s\";",i,prof->allowURL[i].url);
		num++;
	    }
	}
	websWrite(wp,"var confurlCount = %d;",num);
	websWrite(wp,"var urlCount = %d;",MAX_ALLOW_URL_ITEM);
    }
out:
    websWrite(wp,"var max_totalrecs =%d\n;",MAX_PARENTS_CONTROL_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#if (UTT_SMART_UI == FYES)
/*新风格UI  家长控制列表*/
static int getParentsControlConf_Sui(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    char_t data[MAX_ALLOW_URL_ITEM*MAX_URL_NAME_LEN+1U]={0};
    int min = 0, max = 0, i = 0, j = 0,num = 0;
    UINT32 d_temp;
    char_t daydata[28] = {0};
    //const char* ErrMsg = NULL;

    websWrite(wp,"var UserNames=new Array();");
    websWrite(wp,"var arp_Macs=new Array();");
    websWrite(wp,"var fw_days = new Array();");
    websWrite(wp,"var fw_timestarts=new Array();");
    websWrite(wp,"var fw_timestops=new Array();");
    websWrite(wp,"var userUrls = new Array();");
    websWrite(wp,"\r\n");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && (ProfInstIsFree(prof) == 0)) {

	websWrite(wp,"UserNames[%d] = \"%s\";",num,prof->head.name);
	websWrite(wp,"arp_Macs[%d] = \"%s\";",num,converMac6To17Byte(prof->mac));
	d_temp = prof->day;
	Byte2Days(d_temp,daydata);
	websWrite(wp,"fw_days[%d] = \"%s\";",num,daydata);
	websWrite(wp,"fw_timestarts[%d] = \"%s\";",num,prof->timeStart);
	websWrite(wp,"fw_timestops[%d] = \"%s\";",num,prof->timeStop);

	memset(data, 0U, sizeof(data));
	for(j=0;j<MAX_ALLOW_URL_ITEM;j++)
	{
	    if(strlen(prof->allowURL[j].url)!=0U) {
		if(j!=0)
		{
		    strcat(data, ",");
		}
		strcat(data, prof->allowURL[j].url);
	    }
	}
	websWrite(wp,"userUrls[%d] = \"%s\";",num,data);
	num++;
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs =%d;",MAX_PARENTS_CONTROL_PROFILES);
    #if (UTT_SMART_UI == FYES)
      getTheRightMsg(eid,wp,argc,argv);
  #endif
    return 0;
}
#endif

/***********************************************************************
 * 函 数 名：getParentsControlConf
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static int getParentsControlConf(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    char_t data[MAX_ALLOW_URL_ITEM*MAX_URL_NAME_LEN+1U]={0};
    int min = 0, max = 0, i = 0, j = 0,num = 0;
    //const char* ErrMsg = NULL;

    websWrite(wp,"var UserNames=new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var arp_Macs=new Array();");
    websWrite(wp,"var timeGrps = new Array();");
    websWrite(wp,"var userUrls = new Array();");
    websWrite(wp,"\r\n");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && (ProfInstIsFree(prof) == 0)) {

	websWrite(wp,"UserNames[%d] = \"%s\";",num,prof->head.name);
	if(prof->head.active == FUN_ENABLE) {
	    websWrite(wp,"Allows[%d] = \"%s\";",num,T("yes"));
	} else {
	    websWrite(wp,"Allows[%d] = \"%s\";",num,T("no"));
	}
	websWrite(wp,"arp_Macs[%d] = \"%s\";",num,converMac6To17Byte(prof->mac));
#if (TIME_GRP == FYES)
	websWrite(wp,"timeGrps[%d] = \"%s\";",num,prof->timeGrp);
#endif
	memset(data, 0U, sizeof(data));
	for(j=0;j<MAX_ALLOW_URL_ITEM;j++)
	{
	    if(strlen(prof->allowURL[j].url)!=0U) {
		if(j!=0)
		{
		    strcat(data, ",");
		}
		strcat(data, prof->allowURL[j].url);
	    }
	}
	websWrite(wp,"userUrls[%d] = \"%s\";",num,data);
	num++;
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs =%d;",MAX_PARENTS_CONTROL_PROFILES);
    return 0;
}
/***********************************************************************
 * 函 数 名：formConfigParentsControlAllow
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static void  formConfigParentsControlAllow(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibtype = MIB_PROF_PARENTS_CONTROL;
    ParentsControlProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    FunEnableEnum allowEn = FUN_ENABLE;
    int instIndex = -1;

    char *userName = NULL;
    char *allow = NULL;
    const char *errmsg = NULL;

    a_assert(wp);
    userName=websGetVar(wp,T("AllowID"), T(""));/*用户名*/
    allow=websGetVar(wp,T("Allow"), T(""));/*允许*/
    instIndex = ProfGetInstIndexByName(mibtype, userName);
    if(instIndex == PROFFAIL) 
    {/*欲修改的配置不存在*/
	errmsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND); /*maybe need change*/
	setTheErrorMsgOnFree(errmsg);
    }
    else
    {	/*要操作的配置存在*/
	DBGPRINT_PARENTS("%s-%d: instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
	prof = (ParentsControlProfile *)ProfGetInstPointByIndex(mibtype, instIndex);
	if((prof!=NULL) && (ProfInstIsFree(prof) == 0))
	{
	    if (0 == strcmp("no",allow))/*禁止该绑定用户*/
	    {
		allowEn = FUN_DISABLE;
	    }
	    DBGPRINT_PARENTS("%s-%d: allow=%s,allowEn=%d\n",__FUNCTION__,__LINE__,allow,allowEn);
	    /* modify  */
	    profAction = PROF_CHANGE_EDIT;
	    if(prof->head.active != allowEn)
	    {
		ProfBackupByIndex(mibtype, profAction, instIndex, &profhead);
		prof->head.active = allowEn;
		DBGPRINT_PARENTS("%s-%d: active=%d\n",__FUNCTION__,__LINE__,prof->head.active);
#if 1
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();		/*save*/
#endif
	    }
	}
    }

#if 1
#endif
    websRedirect(wp, "ParentsControl.asp");
    return;
}

#if (UTT_SMART_UI == FYES)
/*新风格UI  家长控制孩子PC设置保存*/
static void formParentsControlConf_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    ParentsControlProfile *prof= NULL;
    MacAddr macAddr;

    char_t *userName = NULL,*userNameOld = NULL, *macStr =NULL, *urlStr = NULL;
    char_t *action = NULL, *tmp = NULL, *ptr = NULL;
    char *day = NULL, *timestart = NULL, *timestop = NULL;
    int instIndex, i=0, j=0;
    const char *ErrMsg = NULL;
    a_assert(wp);

    action = websGetVar(wp, T("Action"), T("")); 
    userName = websGetVar(wp,T("username"), T(""));
    userNameOld = websGetVar(wp, T("usernameOld"), T("")); 
    macStr = websGetVar(wp,T("selMacAddress"), T("")); 
    macStr = websGetVar(wp,T("writeMacAddress"), T("")); 
    day=websGetVar(wp,T("day"),T(""));
    timestart=websGetVar(wp,T("timestart"), T(""));
    timestop=websGetVar(wp,T("timestop"), T(""));
    urlStr = websGetVar(wp,T("SiteVals"), T(""));
    DBGPRINT_PARENTS("%s-%d: Action=%s\nuserName=%s\nuserNameOld=%s\nmacStr=%s\nday=%s\ntimestart=%s\ntimestop=%s\nurlStr =%s\n", __func__, __LINE__,action, userName, userNameOld, macStr, day, timestart, timestop, urlStr);

    memset(&macAddr, 0, sizeof(macAddr));
    if (strlen(macStr) != 0U) {
	converMac12To6Byte(macStr, &macAddr);
    }
    if (compareParentsOrChildMac(&macAddr, 0) == 0) {
	if(strcmp(action,"add") == 0) {
	    instIndex = ProfGetInstIndexByName(mibType, userName);
	    if(instIndex != PROFFAIL)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_REPEATED); /*maybe need change*/
		setTheErrorMsg(ErrMsg);
		goto out;
	    }
	    instIndex = ProfNewInst(mibType, userName, FALSE);
	    if(instIndex == PROFFAIL) 
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED); /*maybe need change*/
		setTheErrorMsg(ErrMsg);
		goto out;
	    }
	    profAction = PROF_CHANGE_ADD;

	} else {//modify
	    instIndex = ProfGetInstIndexByName(mibType, userNameOld);
	    if(instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME); /*maybe need change*/
		setTheErrorMsg(ErrMsg);
		goto out;
	    }
	    if (strcmp(userName ,userNameOld) != 0){
		int newIndex;
		newIndex = ProfGetInstIndexByName(mibType, userName);
		if(newIndex != PROFFAIL)
		{
		    ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_EXISTS);
		    setTheErrorMsg(ErrMsg);
		    goto out;
		}
	    }
	    profAction = PROF_CHANGE_EDIT;
	}
	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
	prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	ProfSetNameByPoint(prof, userName);
	prof->head.active = FUN_ENABLE;

	prof->mac = macAddr;
	DBGPRINT_PARENTS("%s-%d: prof->mac =%s\n", __func__, __LINE__,prof->mac.addr);
	for(j=0;j<MAX_ALLOW_URL_ITEM;j++) 
	{
	    memset(prof->allowURL[j].url,0,MAX_URL_NAME_LEN+1);
	}
	tmp=strtok_r(urlStr,UTT_SPLIT_SIGN_STR,&ptr);
	while(tmp!=NULL) 
	{
	    strncpy(prof->allowURL[i++].url,tmp,MAX_URL_NAME_LEN);
	    DBGPRINT_PARENTS("%s-%d: tmp =%s, prof->url[%d] =%s\n", __func__, __LINE__,tmp,i-1,prof->allowURL[i-1].url);
	    tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
	}
	prof->day = days2Byte(day); 
	strcpy((char*)prof->timeStart, timestart); 
	strcpy((char*)prof->timeStop, timestop); 

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }
out:
    #if(UTT_SMART_UI==FYES)
    setSuccessTips();
    #endif
    websRedirect(wp, "ParentsControlSui.asp");
}
#endif


/***********************************************************************
 * 函 数 名：formParentsControlConf
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
static void formParentsControlConf(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    ParentsControlProfile *prof= NULL;
    MacAddr macAddr;

    char_t *userName = NULL,*userNameOld = NULL,*Enable = NULL, *macStr =NULL, *urlStr = NULL;
    char_t *action = NULL, *tmp = NULL, *ptr = NULL;
#if (TIME_GRP == FYES)
    char_t  *timeGrpStr = NULL;
#endif
    int instIndex, i=0, j=0;
	const char *ErrMsg = NULL;
    a_assert(wp);

    action = websGetVar(wp, T("Action"), T("")); 
    userName = websGetVar(wp,T("username"), T(""));
    userNameOld = websGetVar(wp, T("usernameOld"), T("")); 
    Enable = websGetVar(wp,T("AllowControlEnable"), T(""));
    DBGPRINT_PARENTS("%s-%d: username =%s,usernameold =%s, enable =%s\n", __func__, __LINE__,userName,userNameOld,Enable);
    macStr = websGetVar(wp,T("selMacAddress"), T("")); 
    DBGPRINT_PARENTS("%s-%d: macStr =%s\n", __func__, __LINE__,macStr);
    macStr = websGetVar(wp,T("writeMacAddress"), T("")); 
    DBGPRINT_PARENTS("%s-%d: macStr =%s\n", __func__, __LINE__,macStr);
#if (TIME_GRP == FYES)
    timeGrpStr = websGetVar(wp,T("timeRangeName"), T("")); 
    DBGPRINT_PARENTS("%s-%d: timeGrpStr =%s\n", __func__, __LINE__,timeGrpStr);
#endif
    urlStr = websGetVar(wp,T("SiteVals"), T(""));
    DBGPRINT_PARENTS("%s-%d: urlStr =%s\n", __func__, __LINE__,urlStr);

    memset(&macAddr, 0, sizeof(macAddr));
    if (strlen(macStr) != 0U) {
	converMac12To6Byte(macStr, &macAddr);
    }
    if (compareParentsOrChildMac(&macAddr, 0) == 0) {
    instIndex = ProfGetInstIndexByName(mibType, userNameOld);
    if(strcmp(action,"add") == 0) {
	if(instIndex != PROFFAIL)
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_REPEATED); /*maybe need change*/
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, userName, FALSE);
	if(instIndex == PROFFAIL) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED); /*maybe need change*/
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	profAction = PROF_CHANGE_ADD;

    } else {//modify

	if(instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME); /*maybe need change*/
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (strcmp(userName ,userNameOld) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, userName);
	    if(newIndex != PROFFAIL)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_EXISTS);
			setTheErrorMsg(ErrMsg);
			goto out;
	    }
	}
	profAction = PROF_CHANGE_EDIT;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    ProfSetNameByPoint(prof, userName);
    if(strcmp(Enable, T("on")) == 0U) {
	prof->head.active = FUN_ENABLE;
    } else {
	prof->head.active = FUN_DISABLE;
    }

    prof->mac = macAddr;
    DBGPRINT_PARENTS("%s-%d: prof->mac =%s\n", __func__, __LINE__,prof->mac.addr);
#if (TIME_GRP == FYES)
    strncpy(prof->timeGrp,timeGrpStr,UTT_INST_NAME_LEN);
    DBGPRINT_PARENTS("%s-%d: prof->timeGrp =%s\n", __func__, __LINE__,prof->timeGrp);
#endif
    for(j=0;j<MAX_ALLOW_URL_ITEM;j++) {
	memset(prof->allowURL[j].url,0,MAX_URL_NAME_LEN+1);
    }
    tmp=strtok_r(urlStr,UTT_SPLIT_SIGN_STR,&ptr);
    while(tmp!=NULL) {
	strncpy(prof->allowURL[i++].url,tmp,MAX_URL_NAME_LEN);
	DBGPRINT_PARENTS("%s-%d: tmp =%s, prof->url[%d] =%s\n", __func__, __LINE__,tmp,i-1,prof->allowURL[i-1].url);
	tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }
#if 1
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#endif
    }
out:
    websRedirect(wp, "ParentsControl.asp");
}


#if 0
static void addGlobal(void)
{
    MacAddr macAddr;
    char macStr[13];
    SystemProfile *profSys = NULL;
    
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if (profSys != NULL) {
    profSys->parentControlEn.parentsEn = FUN_ENABLE;
    DBGPRINT_PARENTS("%s-%d: profSys->parentControlEn.parentsEn =%d\n", __func__, __LINE__,profSys->parentControlEn.parentsEn);
    memset(&macAddr, 0U, sizeof(macAddr));
    memset(macStr, 0U, sizeof(macStr));
    strcpy(macStr, T("50e549b432d1"));
    converMac12To6Byte(macStr, &macAddr);
    profSys->parentControlEn.parentMac1 = macAddr;
    }
}
static void addTestInst(int count)
{
    ParentsControlProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_PARENTS_CONTROL;
    MacAddr macAddr;
    char macStr[13];
    char userName[11];
    int instIndex = 0, i = 0;
    
    for (i=0;i<count;i++) {
    memset(userName, 0U, sizeof(userName));
    snprintf(userName, 10U, "%s%d", T("Parent"),i);
    DBGPRINT_PARENTS("%s-%d: userName =%s\n", __func__, __LINE__,userName);
    instIndex = ProfGetInstIndexByName(mibType, userName);
    if(instIndex != PROFFAIL)
    {
	DBGPRINT_PARENTS("%s-%d: error!\n", __func__, __LINE__);
	return;
    }
    instIndex = ProfNewInst(mibType, userName, FALSE);
    if(instIndex == PROFFAIL) 
    {
	DBGPRINT_PARENTS("%s-%d: new inst error!\n", __func__, __LINE__);
	return;
    }

    //ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    prof = (ParentsControlProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if (prof != NULL) {
    ProfSetNameByPoint(prof, userName);
    prof->head.active = 1;

    memset(&macAddr, 0, sizeof(macAddr));
    strcpy(macStr, T("112233445566"));
    converMac12To6Byte(macStr, &macAddr);
    prof->mac = macAddr;
    strcpy(prof->allowURL[0].url, T("www.163.com"));
    strcpy(prof->timeGrp, T("allTime"));
    }else {
	DBGPRINT_PARENTS("%s-%d: get prof error!\n", __func__, __LINE__);
    }
    }
}
#endif
/***********************************************************************
 * 函 数 名：formDefineParentsControlConf
 * 功能描述：
 * 输入参数：null
 * 输出参数：null
 * 创建日期：2013-04-27
 * 修改日期：
 * 作者：       
 * 附加说明：无
 ***********************************************************************/
void formDefineParentsControlConf(void)
{
#if (UTT_SMART_UI == FYES)
    websFormDefine(T("formParentsControlConf_Sui"),formParentsControlConf_Sui);
    websAspDefine(T("getParentsControlConf_Sui"),getParentsControlConf_Sui);
#endif
    websFormDefine(T("formParentsControlConf"),formParentsControlConf);
    websFormDefine(T("formConfigParentsControlAllow"),formConfigParentsControlAllow);
    websFormDefine(T("formParentsControlDel"),formParentsControlDel);
    websFormDefine(T("formParentsControlDelAll"),formParentsControlDelAll);
    websFormDefine(T("formParentsControlGlobalConfig"),formParentsControlGlobalConfig);
    websAspDefine(T("getOneParentsControlInfo"),getOneParentsControlInfo);
    websAspDefine(T("getParentsControlConf"),getParentsControlConf);
    websAspDefine(T("getParentsControlGlobalInfo"), getParentsControlGlobalInfo);
#if 0
    addGlobal();
    addTestInst(1);
#endif
}
#endif
