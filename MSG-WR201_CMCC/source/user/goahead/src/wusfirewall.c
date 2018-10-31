/*
 *	firewall.c -- Firewall Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wusfirewall.c,v 1.22 2014/09/19 13:43:38 xu.jian Exp $
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



#if (FIREWALL == FYES)
#ifdef FW_DBG
#define FW_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define FW_PRINTF(fmt, args...)
#endif
#if (IP_GRP== FYES)
extern int getIpGrpList(int eid, webs_t wp, int argc, char_t **argv);
#endif
extern int getFwGrpList(int eid, webs_t wp, int argc, char_t **argv);
extern void  _ProfRestore(MibProfileType proftype,int instIndex, Pointer prof);
extern int inst_move(char *sID, char * tID, MibProfileType proftype, int flag);
extern int moveInstToFirst(char *sID, char * tID, MibProfileType proftype);
extern unsigned int ip2int(char *ip);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern char* converMac6To17Byte(MacAddr macAddr);
extern int moveToLast(char* name);
#if (PORT_GRP== FYES)
/*
 *检查服务组是否被防火墙引用
 */
extern bool checkFwGrpInFireWall(char* groupName)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
    bool flag = FALSE;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
        if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == 1))
        {
            if ((prof->fwTypeEn == FW_GRP) && (strcmp(prof->fwGrpName,groupName) == 0))
            {
                flag = TRUE;
                break;
            }
        }
    }
    return flag;
}
#endif
/***********************************************************************
 * 函 数 名：formMove_Config
 * 功能描述：  插入函数
 * 输入参数：null
 * 输出参数：null
 * 创建日期：11-11-01
 * 修改日期：
 * 作者：   yqyang
 * 附加说明：
 **********************************************************************/

void formMove_Config(webs_t wp, char *path, char *query)
{
    char *sID,*locationID;
    MibProfileType proftype;

    sID = websGetVar(wp, T("sID"), NULL);
    locationID= websGetVar(wp, T("tID"),NULL);				/* 是什么位置之前*/
    proftype= MIB_PROF_FIRE_WALL;

    if(strcmp(sID,locationID) == 0)
    {
	goto out;
    }
    if(moveInstToFirst(sID, locationID, proftype)) {
	nvramWriteCommit();
    }
out:
	websRedirect(wp, "FireWall.asp");
}
#if 0
/***********************************************************************
 * 功能描述：将要移动的实例号移动到目的实例号之前或之后
 * 输入参数：
 *	    sID:	要移动的实例号。
 *	    tID:	目的实例号。
 *	    proftype:	实例类型。
 *	    flag:	1   移动到目的实例号之前
 *			2   移动到目的实例号之后
 * 输出参数：	bool
 * 创建日期：	11-11-4
 * 作者：	yqyang
 * 附加说明：
 **********************************************************************/
extern bool inst_move(char *sID, char * tID, MibProfileType proftype)
{
    int  instindex,index,low,high,i;
    struProfAlloc *proflist  = NULL;
    struProfAlloc *proflist2  = NULL;

    index= ProfGetInstIndexByName(proftype,tID);
    instindex = ProfGetInstIndexByName(proftype,sID);
    ProfInstLowHigh(proftype, &high, &low);

    if(index != (instindex+1)){                                           /*需要调整位置*/
	for(i=low;i<high;i++){                                     /*  从头开始备份  */
	    /* 假如找到目标位置， 就将源实例备份插    入到目标实例位置   随后也需要备份 目标实例，整条练就完成的连接起来了 */  
	    if(i==index){   
		ProfBackupByIndex(proftype,PROF_CHANGE_EDIT,instindex, &proflist);  
	    }
	    if(i==instindex) continue;                                  /*源实例位置 在index位置已经插入，现在不需要插入了*/
	    ProfBackupByIndex(proftype,100,i, &proflist);  /*100代表备份，但是不触发profilechange*/
	}
    }
    else
    {
	return FALSE;
    }
    index=0;                                                        /*恢复位置从头开始*/
    proflist2 = proflist;
    while(proflist!= NULL) {                                            /*恢复  从开始位置恢复表*/
	_ProfRestore(proflist->proftype, index++, proflist->oldProf);
	proflist = proflist->next;
    }

    ProfUpdate(proflist2);

    ProfFreeAllocList(proflist);
    ProfFreeAllocList(proflist2);

    return TRUE;
}
/*function moveInstToFirst move to internet.c*/
/***********************************************************************
 * 功能描述：将要移动的实例号移动到目的实例号之前
 * 输入参数：
 *	    sID:	要移动的实例号。
 *	    tID:	目的实例号。
 *	    proftype:	实例类型。
 * 输出参数：	0 移动失败（已在前面）
 *              1  移动成功
 * 创建日期：	2012-08-09
 * 作者：	Jianqing.Sun
 **********************************************************************/
static int moveInstToFirst(char *sID, char * tID, MibProfileType proftype)
{
    int tmp = 0, min = 0, max = 0, i = 0, tagIndex = 0, srcIndex = 0;
    struProfAlloc *proflist  = NULL;
    struProfAlloc *proflist2  = NULL;

    tagIndex = ProfGetInstIndexByName(proftype,tID);
    srcIndex = ProfGetInstIndexByName(proftype,sID);
    ProfInstLowHigh(proftype, &max, &min);

    FW_PRINTF("%s------%d,srcIndex = %d, tagIndex = %d\n", __func__, __LINE__, srcIndex, tagIndex);
    if(tagIndex != (srcIndex + 1))
    {                                           /*需要调整位置*/
	for(i = min; i < max; i++)
	{                                     /*  从头开始备份  */
	    /* 假如找到目标位置， 就将源实例备份插    入到目标实例位置   随后也需要备份 目标实例，整条练就完成的连接起来了 */  
	    if(i==tagIndex)
	    {   
		ProfBackupByIndex(proftype,PROF_CHANGE_EDIT,srcIndex, &proflist);  
	    }
	    if(i==srcIndex) continue;                                  /*源实例位置 在tagIndex位置已经插入，现在不需要插入了*/
	    ProfBackupByIndex(proftype,100,i, &proflist);  /*100代表备份，但是不触发profilechange*/
	}
	tmp = 1;
    }
    tagIndex=0;                                                        /*恢复位置从头开始*/
    proflist2 = proflist;
    while(proflist!= NULL) 
    {                                            /*恢复  从开始位置恢复表*/
	_ProfRestore(proflist->proftype, tagIndex++, proflist->oldProf);
	proflist = proflist->next;
    }

    ProfUpdate(proflist2);

    ProfFreeAllocList(proflist);
    ProfFreeAllocList(proflist2);
    return tmp;
}
#endif

/***********************************************************************
 * 函 数 名：   formFilterList
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formFilterList(webs_t wp, char_t *path, char_t *query)
{
    char_t *PolicyName,*PolicyNameOld,*Enable,
	   *srcFromIp,*srcEndIp,*GroupName,*sourceIP,*srcFromPort = 0,*srcEndPort = 0,/**Priority,*/*Status;
    /*目的地址修改为目的地址组*/
	char_t *destFromIp,*destEndIp,*destGroupName,*destIP;

    char_t *FilterType,*Protocol = 0,*dstFromPort = 0,*dstEndPort = 0,
	   *dstFromIp,*dstEndIp,*FilterKey = 0,*fw_day,*fw_timestart,*fw_timestop;
    char_t *action;
#if (PORT_GRP== FYES)
    char_t *fwGrpName = NULL; 
#endif
    char_t *fwType    = NULL;
    struct in_addr addr_SrcFromIp,addr_SrcEndIp,addr_DstFromIp,addr_DstEndIp;
    unsigned int nSrcFromIp,nSrcEndIp,nDstFromIp = 0,nDstEndIp = 0;
    int instIndex;
	const char *ErrMsg = NULL;
    a_assert(wp);

    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    FireWallProfile *prof= NULL;
#if 0
    char macStr[18];
    MacAddr macAddr;
    memset(macStr, 0, sizeof(macStr));
    memset(&macAddr, 0, sizeof(macAddr));
#endif

    PolicyName=websGetVar(wp,T("PolicyName"), T(""));
    Enable=websGetVar(wp,T("PolicyEnable"), T(""));
    srcFromIp=websGetVar(wp,T("FromIP"), T(""));
    srcEndIp=websGetVar(wp,T("EndIP"), T(""));
    GroupName = websGetVar(wp,T("GroupName"), T("")); 
    sourceIP = websGetVar(wp,T("sourceIP"), T(""));
    /*目的地址修改为目的地址组*/
    destFromIp=websGetVar(wp,T("destFromIP"), T(""));
    destEndIp=websGetVar(wp,T("destEndIP"), T(""));
    destGroupName = websGetVar(wp,T("destGroupName"), T("")); 
    destIP = websGetVar(wp,T("destIP"), T(""));
    FW_PRINTF("%s-----%d, GroupName = %s\n", __func__, __LINE__, GroupName);
#if 0
    Priority=websGetVar(wp,T("PRI"), T(""));
#endif
    Status =websGetVar(wp,T("Status"), T(""));
    FilterType=websGetVar(wp,T("FilterType"), T(""));

    nSrcFromIp=ip2int(srcFromIp);
    nSrcEndIp=ip2int(srcEndIp);

    if((strcmp(sourceIP, "ipRange") == 0) &&((inet_aton(srcFromIp, &addr_SrcFromIp) == 0) || (inet_aton(srcEndIp, &addr_SrcEndIp) == 0)))
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    /*目的地址修改为目的地址组*/
    nDstFromIp=ip2int(destFromIp);
    nDstEndIp=ip2int(destEndIp);
    if((strcmp(destIP, "destIpRange") == 0) &&((inet_aton(destFromIp, &addr_DstFromIp) == 0) || (inet_aton(destEndIp, &addr_DstEndIp) == 0)))
    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

	fwType    = websGetVar(wp,T("fwType"), T(""));
    FW_PRINTF("%s-----%d, fwtype = %s\n", __func__, __LINE__, fwType);

    if(strcmp(fwType,"fwdiy") == 0){
    if(strtoul(FilterType,0,10)  == TYPE_FILTER_IP) {
    /*
	 * IP过滤
	 */

	Protocol=websGetVar(wp,T("Protocol"), T(""));
	dstFromPort=websGetVar(wp,T("DesFromPort"), T(""));
	dstEndPort=websGetVar(wp,T("DesEndPort"), T(""));
#if 0
	dstFromIp=websGetVar(wp,T("DesFromIP"), T(""));
	dstEndIp=websGetVar(wp,T("DesEndIP"), T(""));
#endif
	srcFromPort=websGetVar(wp,T("SouFromPort") , T(""));
	srcEndPort=websGetVar(wp,T("SouEndPort"), T(""));
#if 0
	nDstFromIp=ip2int(dstFromIp);
	nDstEndIp=ip2int(dstEndIp);
	if((inet_aton(dstFromIp, &addr_DstFromIp) == 0) || (inet_aton(dstEndIp, &addr_DstEndIp) == 0) ){  
		ErrMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(ErrMsg);
	    goto out;
	}
#endif
    } else {
	/*
	 * URL 和 关键字过滤
	 */
	FilterKey=websGetVar(wp,T("FilterKey"), T(""));
    }
    }else if(strcmp(fwType,"fwgrp") == 0){
#if (PORT_GRP == FYES)
        fwGrpName = websGetVar(wp,T("fwGrpName"), T(""));
#endif
    }

    fw_day=websGetVar(wp,T("day"),T(""));
    fw_timestart=websGetVar(wp,T("timestart"), T(""));
    fw_timestop=websGetVar(wp,T("timestop"), T(""));
    action = websGetVar(wp, T("Action"), T("")); 

    PolicyNameOld = websGetVar(wp, T("PolicyNameold"), T("")); 
    instIndex = ProfGetInstIndexByName(mibType, PolicyNameOld);
    if(strcmp(action,"add") == 0) {
	if(instIndex != PROFFAIL)
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_REPEATED);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, PolicyName, FALSE);
	if(instIndex == PROFFAIL) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	profAction = PROF_CHANGE_ADD;

    } else {//modify

	if(instIndex == PROFFAIL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME);
	    setTheErrorMsg(ErrMsg);
	    goto out;
	}
	if (strcmp(PolicyName ,PolicyNameOld) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, PolicyName);
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
    prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    ProfSetNameByPoint(prof, PolicyName);
    prof->head.active = strtoul(Enable,0,10); 
    if(strcmp(sourceIP, "ipRange") == 0)
    {
	prof->addrTypeEn = ADDR_IPRANGE; 
	prof->srcFromIp = addr_SrcFromIp.s_addr; 
	prof->srcEndIp = addr_SrcEndIp.s_addr; 
#if 0
	printf("%u\n",prof->srcFromIp);
#endif
    }
    else
    {
#if (IP_GRP == FYES)
	prof->addrTypeEn = ADDR_IPGRP; 
	strcpy(prof->ipGrpName, GroupName);
#endif
    }
    /*目的地址修改为目的地址组*/
    if(strcmp(destIP, "destIpRange") == 0)
    {
	prof->destAddrTypeEn = ADDR_IPRANGE; 
	prof->IpFilter.dstFromIp = addr_DstFromIp.s_addr; 
	prof->IpFilter.dstEndIp = addr_DstEndIp.s_addr; 
    }
    else
    {
#if (IP_GRP == FYES)
	prof->destAddrTypeEn = ADDR_IPGRP; 
	strcpy(prof->destIpGrpName, destGroupName);
#endif
    }

#if (IP_GRP == FYES)
    FW_PRINTF("%s-----%d,prof->ipGrpName = %s, GroupName = %s\n", __func__, __LINE__, prof->ipGrpName, GroupName);
#endif
#if 0
    prof->PRI = strtoul(Priority,0,10); 
#endif
    prof->Status = strtoul(Status,0,10);
    prof->FilterType = strtoul(FilterType,0,10); 


    if(strtoul(FilterType,0,10)  == TYPE_FILTER_IP) {
    if(strcmp(fwType,"fwdiy") == 0){
        prof->fwTypeEn = FW_DIY;
	prof->IpFilter.Protocol = strtoul(Protocol,0,10); 
	prof->IpFilter.dstFromPort = strtoul(dstFromPort,0,10);
	prof->IpFilter.dstEndPort = strtoul(dstEndPort,0,10);
#if 0
    prof->IpFilter.dstFromIp = addr_DstFromIp.s_addr;
    prof->IpFilter.dstEndIp =  addr_DstEndIp.s_addr;
#endif

	prof->IpFilter.srcFromPort =  strtoul(srcFromPort,0,10);
	prof->IpFilter.srcEndPort = strtoul(srcEndPort,0,10);
    }else if(strcmp(fwType,"fwgrp") == 0){
#if (PORT_GRP == FYES)
        prof->fwTypeEn = FW_GRP;
        memcpy(prof->fwGrpName,fwGrpName,sizeof(prof->fwGrpName));
#endif
    }

#if 0
    }else if (strtoul(FilterType,0,10)  == TYPE_FILTER_MAC) {
	strcpy(macStr, FilterKey);
	strRemoveChar(macStr, ':');
	strRemoveChar(macStr, '-');
	converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
	strncpy((char*)prof->WordFilter.FilterKey, converMac6To17Byte(macAddr), 17U);
#endif
    }else {
    if(strcmp(fwType,"fwdiy") == 0){
        prof->fwTypeEn = FW_DIY;
	strcpy((char*)prof->WordFilter.FilterKey, FilterKey);
    }
#if (PORT_GRP == FYES)
    else if(strcmp(fwType,"fwgrp") == 0){
        prof->fwTypeEn = FW_GRP;
        memcpy(prof->fwGrpName,fwGrpName,sizeof(prof->fwGrpName));
    }
#endif
    }

    prof->fw_day = days2Byte(fw_day); 
    strcpy((char*)prof->fw_timestart, fw_timestart); 
    strcpy((char*)prof->fw_timestop, fw_timestop); 
    if((profAction == PROF_CHANGE_ADD) && (moveToLast(prof->head.name) == 1))
    {/*添加*/
	FW_PRINTF("%s-----%d, profGroupBase->head.name = %s\n", __func__, __LINE__, prof->head.name);
    }
    else
    {
	ProfUpdate(profhead);
    }
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "FireWall.asp");
}
/***********************************************************************
 * 函 数 名：   formFilterListDel
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formFilterListDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *PolicyName=NULL,*tmp,*ptr;
    a_assert(wp);
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    struProfAlloc *profList  = NULL;
    int back = 0;
	const char* ErrMsg = NULL;

    PolicyName = websGetVar(wp,T("delstr"),T("")); 
    if(!strcmp(PolicyName,T(""))) {   
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_STRA_NAME_GN);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    tmp=strtok_r(PolicyName,UTT_SPLIT_SIGN_STR,&ptr);
    while(tmp!=NULL) {
	if(back == 0){
	    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	    back = 1;
	}
	ProfDelInstByName(mibType, tmp); 
	tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
    websRedirect(wp,"FireWall.asp");
}
/***********************************************************************
 * 函 数 名：   formFilterListDelAll
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formFilterListDelAll(webs_t wp, char_t *path, char_t *query)
{ 
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    Pointer prof;
    struProfAlloc *profList  = NULL;

    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
	prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back = 1;
	    }
	    ProfDelInstByIndex(mibType, i);
	}
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "FireWall.asp");
}

/***********************************************************************
 * 函 数 名：   getOneFilterInfo
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static int getOneFilterInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name; 
    FireWallProfile *prof;
    UINT32 d_temp;
    char_t data[16];
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
	const char *ErrMsg = NULL;

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR);
		setTheErrorMsg(ErrMsg);
		goto out;
    }
    if(edit_name == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_STRATEGY_NAME_CANT_NULL);
		setTheErrorMsg(ErrMsg);
		goto out;
    }

    prof = (FireWallProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
		setTheErrorMsg(ErrMsg);
    } else {
		websWrite(wp,"var PolicyNames=\"%s\";",prof->head.name);
		websWrite(wp,"var PolicyEnables =\"%d\";",prof->head.active);
	if(prof->addrTypeEn == ADDR_IPRANGE)
	{
	    websWrite(wp,"var FromIPs = \"%N\";",prof->srcFromIp);
	    websWrite(wp,"var EndIPs = \"%N\";",prof->srcEndIp);
	    websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPRANGE);
	}
	else
	{
#if (IP_GRP == FYES)
	    websWrite(wp,"var ipGrpNames = \"%s\";",prof->ipGrpName);
	    websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPGRP);
#endif
	}
	websWrite(wp,"var Statuss = \"%d\";",prof->Status);
#if 0
	websWrite(wp,"var PRIs= \"%d\";",prof->PRI);
#endif
	websWrite(wp,"var FilterTypes = \"%d\";",prof->FilterType);
	d_temp = prof->fw_day;
	Byte2Days(d_temp,data);
	websWrite(wp,"var fw_days = \"%s\";",data);
	websWrite(wp,"var fw_timestarts = \"%s\";",prof->fw_timestart);
	websWrite(wp,"var fw_timestops = \"%s\";",prof->fw_timestop);

    websWrite(wp,"fwTypeEn = %d;",prof->fwTypeEn);
    if(prof->fwTypeEn == FW_DIY)
    {
	if(prof->FilterType == TYPE_FILTER_IP) {
	    websWrite(wp,"Protocols = \"%d\";",prof->IpFilter.Protocol);
	    websWrite(wp,"DesFromPorts = \"%d\";",prof->IpFilter.dstFromPort);
	    websWrite(wp,"DesEndPorts = \"%d\"; ",prof->IpFilter.dstEndPort);
#if 0
	    websWrite(wp,"DesFromIPs = \"%N\"; ",prof->IpFilter.dstFromIp);
	    websWrite(wp,"DesEndIPs = \"%N\"; ", prof->IpFilter.dstEndIp);
#endif
	    websWrite(wp,"SouFromPorts = \"%d\";",prof->IpFilter.srcFromPort);
	    websWrite(wp,"SouEndPorts = \"%d\";",prof->IpFilter.srcEndPort);
	}else {
	    websWrite(wp,"FilterKeys = \"%s\";",prof->WordFilter.FilterKey);
	}
    }
#if (PORT_GRP == FYES)
    else if(prof->fwTypeEn == FW_GRP)
    {
	    websWrite(wp,"fwGrpName = \"%s\";",prof->fwGrpName);
    }
#endif

    /*目的地址修改为目的地址组*/
	if(prof->destAddrTypeEn == ADDR_IPRANGE)
	{
	    websWrite(wp,"var destFromIPs = \"%N\";",prof->IpFilter.dstFromIp);
	    websWrite(wp,"var destEndIPs = \"%N\";",prof->IpFilter.dstEndIp);
	    websWrite(wp,"var destIPs = \"%d\";",ADDR_IPRANGE);
	}
	else
	{
#if (IP_GRP == FYES)
	    websWrite(wp,"var destIpGrpNames = \"%s\";",prof->destIpGrpName);
	    websWrite(wp,"var destIPs = \"%d\";",ADDR_IPGRP);
#endif
	}
    }
out:
    websWrite(wp,"var max_totalrecs =%d;",MAX_FIREWALL_FILTER_PROFILES);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/***********************************************************************
 * 函 数 名：   getFilterList
 * 创建日期：	2011-11-03
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static int getFilterList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t data[28]={0};
    int min = 0, max = 0;
    int i = 0;
    int num = 0;
    UINT32 d_temp;
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
	const char* ErrMsg = NULL;

    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profSys->firewallEn == FUN_ENABLE)
    {
	websWrite(wp, "GlobalEnable=\"%s\";", T("on"));
    }
    else
    {
	websWrite(wp, "GlobalEnable=\"%s\";", T("off"));
    }

    websWrite(wp,"var PolicyNames=new Array();");
    websWrite(wp,"var PolicyEnables = new Array();");
    websWrite(wp,"var FromIPs = new Array();");
    websWrite(wp,"var EndIPs=new Array();");
#if (IP_GRP == FYES)
    websWrite(wp,"var ipGrpNames=new Array();");
#endif
    websWrite(wp,"var sourceIPs=new Array();");

    /*目的地址修改为目的地址组*/
    websWrite(wp,"var destFromIPs = new Array();");
    websWrite(wp,"var destEndIPs=new Array();");
#if (IP_GRP == FYES)
    websWrite(wp,"var destIpGrpNames=new Array();");
#endif
    websWrite(wp,"var destIPs=new Array();");

#if 0
    websWrite(wp,"var PRIs=new Array();");
#endif
    websWrite(wp,"var Statuss=new Array();");
    websWrite(wp,"var FilterTypes=new Array();");
    websWrite(wp,"var Protocols = new Array();");
    websWrite(wp,"var DesFromPorts=new Array();");
    websWrite(wp,"var DesEndPorts=new Array();");
    websWrite(wp,"var DesFromIPs = new Array();");
    websWrite(wp,"var DesEndIPs = new Array();");
    websWrite(wp,"var SouFromPorts=new Array();");
    websWrite(wp,"var SouEndPorts=new Array();");
    websWrite(wp,"var FilterKeys = new Array();");
    websWrite(wp,"var fwGrpType = new Array();");
#if (PORT_GRP == FYES)
    websWrite(wp,"var fwGrpName = new Array();");
#endif
    websWrite(wp,"var fw_days = new Array();");
    websWrite(wp,"var fw_timestarts=new Array();");
    websWrite(wp,"var fw_timestops=new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && (ProfInstIsFree(prof) == 0)) {

    if(prof->fwTypeEn == FW_DIY)
    {
	    websWrite(wp,"fwGrpType[%d] = \"fwdiy\";",num);
	    if(prof->FilterType == TYPE_FILTER_IP) {
		websWrite(wp,"Protocols[%d] = \"%d\";",num,prof->IpFilter.Protocol);
		websWrite(wp,"DesFromPorts[%d] = \"%d\";",num,prof->IpFilter.dstFromPort);
		websWrite(wp,"DesEndPorts[%d] = \"%d\"; ",num,prof->IpFilter.dstEndPort);
#if 0
		websWrite(wp,"DesFromIPs[%d] = \"%N\"; ",num,prof->IpFilter.dstFromIp);
		websWrite(wp,"DesEndIPs[%d] = \"%N\"; ",num, prof->IpFilter.dstEndIp);
#endif
		websWrite(wp,"SouFromPorts[%d] = \"%d\";",num,prof->IpFilter.srcFromPort);
		websWrite(wp,"SouEndPorts[%d] = \"%d\";",num,prof->IpFilter.srcEndPort);
	    } else {

		websWrite(wp,"FilterKeys[%d] = \"%s\";",num,prof->WordFilter.FilterKey);
	    }
    }
#if (PORT_GRP == FYES)
    else if(prof->fwTypeEn == FW_GRP)
    {
	    websWrite(wp,"fwGrpType[%d] = \"fwgrp\";",num);
	    websWrite(wp,"fwGrpName[%d] = \"%s\";",num,prof->fwGrpName);
    }
#endif

	    websWrite(wp,"PolicyNames[%d] = \"%s\";",num,prof->head.name);
	    websWrite(wp,"PolicyEnables[%d] = \"%d\";",num,prof->head.active);
	if(prof->addrTypeEn == ADDR_IPRANGE)
	{
	    websWrite(wp,"FromIPs[%d] = \"%N\";",num,prof->srcFromIp);
	    websWrite(wp,"EndIPs[%d] = \"%N\";",num,prof->srcEndIp);
	    websWrite(wp,"sourceIPs[%d] = \"%d\";",num,ADDR_IPRANGE);
	}
	else
	{
#if (IP_GRP == FYES)
	    if(strlen(prof->ipGrpName) == 0)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
		websWrite(wp,"ipGrpNames[%d] = \"%s\";",num, ErrMsg);
	    }
	    else
	    {
		websWrite(wp,"ipGrpNames[%d] = \"%s\";",num,prof->ipGrpName);
	    }
	    websWrite(wp,"sourceIPs[%d] = \"%d\";",num,ADDR_IPGRP);

#endif
	}
    /*目的地址修改为目的地址组*/
	if(prof->destAddrTypeEn == ADDR_IPRANGE)
	{
	    websWrite(wp,"destFromIPs[%d] = \"%N\";",num,prof->IpFilter.dstFromIp);
	    websWrite(wp,"destEndIPs[%d] = \"%N\";",num,prof->IpFilter.dstEndIp);
	    websWrite(wp,"destIPs[%d] = \"%d\";",num,ADDR_IPRANGE);
	}
	else
	{
#if (IP_GRP == FYES)
	    if(strlen(prof->destIpGrpName) == 0)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
		websWrite(wp,"destIpGrpNames[%d] = \"%s\";",num, ErrMsg);
	    }
	    else
	    {
		websWrite(wp,"destIpGrpNames[%d] = \"%s\";",num,prof->destIpGrpName);
	    }
	    websWrite(wp,"destIPs[%d] = \"%d\";",num,ADDR_IPGRP);

#endif
	}

#if 0
	    websWrite(wp,"PRIs[%d] = \"%d\";",num,prof->PRI);
#endif
	    websWrite(wp,"Statuss[%d] = \"%d\";",num,prof->Status);
	    websWrite(wp,"FilterTypes[%d] = \"%d\";",num,prof->FilterType);
	    d_temp = prof->fw_day;
	    Byte2Days(d_temp,data);
	    websWrite(wp,"fw_days[%d] = \"%s\";",num,data);
	    websWrite(wp,"fw_timestarts[%d] = \"%s\";",num,prof->fw_timestart);
	    websWrite(wp,"fw_timestops[%d] = \"%s\";",num,prof->fw_timestop);
	    num++;
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs =%d;",MAX_FIREWALL_FILTER_PROFILES);
    return 0;
}
#if 0
/*move to file wusIpGroup.c*/
#if (IP_GRP == FYES)
/********************************************************************
 * 函数： getIpGrpList
 * 功能： 向页面输出用户组
 * 创建： 2012-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int getIpGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int num = 0;

    websWrite(wp,"var addGroupNames = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
	    websWrite(wp,"addGroupNames[%d] = \"%s\";",num,prof->head.name);
	    num++;
	}
    }
    return 0;
}
#endif
#endif


/***
 * 2013-2-5
 *zhengxiao
 *防火墙访问控制策略页面，状态单选框动作
 */
static void formFilterAllow(webs_t wp, char *path, char *query)
{
    UINT32 Allow;
    char_t *AllowID;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    ProfChangeType profAction;
    FireWallProfile *prof= NULL;
    GroupBaseProfile *profGroupBase = NULL;
    int instIndex;
    const char *ErrMsg=NULL;

    char_t *PolicyName=NULL,*Enable=NULL;

    PolicyName=websGetVar(wp,T("AllowID"), T(""));
    Enable=websGetVar(wp,T("Allow"), T(""));       
    
    if((PolicyName == NULL) || (Enable == NULL)){
    	ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
	setTheErrorMsg(ErrMsg);
    } else {
    instIndex = ProfGetInstIndexByName(mibType, PolicyName);
    if(instIndex != PROFFAIL)
    {
    	profAction = PROF_CHANGE_EDIT;
    	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

    	prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if(prof != NULL)
	{   
    	    prof->head.active = strtoul(Enable,0,10);
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }
    }
    out:
    websRedirect(wp, "FireWall.asp");/*页面跳转*/
}
     

/****
 *2013-2-5
 *zheng.xiao
 *防火墙访问控制页面，全局控制按钮
 */
static void formFilterGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *Enable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;

    Enable = websGetVar(wp, "GlobalEnable", T(""));
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if(profSys != NULL)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    
        if(strcmp(Enable, "on") == 0) {
            enable = FUN_ENABLE;
        }
        profSys->firewallEn = enable;
    
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
        nvramWriteCommit();
    }
    websRedirect(wp, "FireWall.asp");/*页面跳转*/
    return;

}


/***********************************************************************
 * 函 数 名：   formDefineNatRuleConfig
 * 创建日期：	2011-11-02
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
void formDefinefirewallConfig(void)
{
    websFormDefine(T("Move_Config"),formMove_Config);
    websFormDefine(T("formFireWall"),formFilterList);
    websFormDefine(T("formFireWallDelAll"),formFilterListDelAll);
    websFormDefine(T("formFireWallDel"),formFilterListDel);
    websAspDefine(T("aspOutOneFireWallRuleInfo"),getOneFilterInfo);
    websAspDefine(T("aspOutFireWall"),getFilterList);
    websFormDefine(T("formFireWallAllow"),formFilterAllow);
   websFormDefine(T("formFireWallGlobalConfig"), formFilterGlobalConfig);

#if (IP_GRP == FYES)
    websAspDefine(T("aspOutIpGrpList"),getIpGrpList);
#endif
    websAspDefine(T("aspOutFwGrpList"),getFwGrpList);
}
#endif
