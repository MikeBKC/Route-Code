#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"
#include "translate.h"

#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"

#define BEHAVIOR_DBG

#ifdef BEHAVIOR_DBG 
#define BEHAVIOR_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define BEHAVIOR_PRINTF(fmt, args...)
#endif

#if (GROUP_BASE == FYES)
#define	WEB_COEFFICIENT	    11

static bool groupbase_instmove(char *sID, char * tID, MibProfileType proftype);
extern void  _ProfRestore(MibProfileType proftype,int instIndex, Pointer prof);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern int moveLastToFirst(char* name);
static GroupBaseType getWebGroupType(webs_t wp);
static char* getRedWebByGbt(GroupBaseType groupType);
#ifdef CONFIG_UTT_NETFILTER /* ReOS SE v1.1������Ϊ�����޸�  33��->256�� */
#define UTT_MAX_PDB_POLICY_CNT ((UTT_MAX_PDB_POLICY) * 8)
static Boolean pdb_policy_match(Uint8 *policy, int index);
static int SetPdbPolicy(Uint8* pdbPolicy, char_t *str);
static void pdb_policy_set(Uint8 *policy, int index);
#endif
#ifdef CONFIG_UTT_NETFILTER
static void formUttNfGlobalConfig(webs_t wp, char_t *path, char_t *query)
{

    char *uttNfEnable = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
    struProfAlloc *profhead = NULL;

    /* get web value */
    uttNfEnable = websGetVar(wp, "GlobalEnable", T(""));

    /* get SystemProfile */
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);

    if(strcmp(uttNfEnable, "on") == 0) {
	enable = FUN_ENABLE;
#if 0
	/* ������Ϊ�����������ʶ��ͳ��û��������ôͬ������*/
	if (profSys->UttNfIndentEn == FUN_DISABLE) { 
	    profSys->UttNfIndentEn = FUN_ENABLE;
	}
#endif
    }
    profSys->UttNfRuleEn = enable;

    /* update systemProfile
     * and write to flash
     */
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    websRedirect(wp, "GlobalBase.asp");/*ҳ����ת*/
    return;

}
#endif
/***
 * ��ȡ��ǰҳ���Ӧ�������ͣ�������Ϊ�����ϸ������
 * bhou
 * 2011-11-07
 */
static GroupBaseType getWebGroupType(webs_t wp)
{
    char_t *page;
    GroupBaseType retType = GBT_FINE_LIMIT;

    if(strstr(wp->url, "goform")==NULL)/*��ǰҳ�治Ҳ��goformҳ��*/
    {
	if((strstr(wp->url, "Base") != NULL))/*url����base��Ϊ��Ϊ����*/
	{
	    retType = GBT_ACT_CONTROL;	
	}
    }
    else
    {/*��ǰҳ��Ϊgoform��ɶ�ȡҳ���ϵ����ؿؼ�*/
	page = websGetVar(wp, T("page"), T(""));
	if(strcmp(page, "limit")!=0)
	{/*��Ϊ����ҳ��*/
	    retType = GBT_ACT_CONTROL;	
	}
    }

    return retType;
}
/**
 * ���ݵ�ǰ������������ȷ����������ת����ҳ��
 *
 */
static char* getRedWebByGbt(GroupBaseType groupType)
{
    static char retWeb[32];
    memset(retWeb, 0, sizeof(retWeb));

    if(groupType == GBT_ACT_CONTROL)
    {/*������Ϊ������תҳ��*/
	strcpy(retWeb ,"GlobalBase.asp");
    }
    else
    {/*��ϸ��������תҳ��*/
	strcpy(retWeb ,"Group.asp");
    }
    return retWeb;
}

#ifdef CONFIG_UTT_NETFILTER
/*
 * ��ҳ��������õ�pdbʵ����
 */
static void websWritePdbName(webs_t wp, Uint8 *pdbPolicy) {
    int i, min, max;
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0) {
	    if ((prof->type != RoutePdb) && (prof->index < UTT_MAX_PDB_POLICY_CNT) && pdb_policy_match(pdbPolicy, prof->index)) {
		websWrite(wp, "%s= \"on\";", prof->head.name);
	    }
	}
    }

    return;
}

/*
 *����ʵ���Ĳ�����ȡ����
 */
static int getOneGroupBaseConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name = NULL;
    const char *errMsg=NULL;
    GroupBaseProfile *prof;
    UINT32 d_temp;
    char_t data[16];
#if 0
    char_t *tmp = NULL;
    char_t pdbPolicys[256];
#endif
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;/*Ĭ�ϴ�ӡ��ϸ������ҳ��*/ 

    if(ejArgs(argc,argv,T("%s"), &edit_name) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    if(edit_name == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_GROUP_NAME_NULL);
	setTheErrorMsg(errMsg);
	goto out;
    }
    webGroupType = getWebGroupType(wp);/*��ȡҳ��Ҫ��ʾ��������*/
    prof = (GroupBaseProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
	setTheErrorMsg(errMsg);
    } else {
	websWrite(wp, "var GroupNames=\"%s\";", prof->head.name);
	websWrite(wp, "var group_timestarts=\"%s\";", prof->timeStart);
	websWrite(wp, "var group_timestops=\"%s\";", prof->timeStop);
	d_temp = prof->day;
	Byte2Days(d_temp,data);
	websWrite(wp, "var group_days=\"%s\";", data);
	if(prof->addrTypeEn == ADDR_IPRANGE)
	{
	    websWrite(wp,"var FromIPs = \"%N\";",prof->ipStart);
	    websWrite(wp,"var EndIPs = \"%N\";",prof->ipEnd);
	    websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPRANGE);
	}
#if (IP_GRP == FYES)
	else
	{
	    websWrite(wp,"var ipGrpNames = \"%s\";",prof->ipGrpName);
	    websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPGRP);
	}
#endif
	if(prof->dst_addrTypeEn == ADDR_IPRANGE)
	{
	    websWrite(wp,"var dst_FromIPs = \"%N\";",prof->dst_ipStart);
	    websWrite(wp,"var dst_EndIPs = \"%N\";",prof->dst_ipEnd);
	    websWrite(wp,"var dst_sourceIPs = \"%d\";",ADDR_IPRANGE);
	}
#if (IP_GRP == FYES)
	else
	{
	    websWrite(wp,"var dst_ipGrpNames = \"%s\";",prof->dst_ipGrpName);
	    websWrite(wp,"var dst_sourceIPs = \"%d\";",ADDR_IPGRP); 
	}
#endif


	if(webGroupType == GBT_FINE_LIMIT)
	{/*��ϸ������*/
	    websWrite(wp, "var FromIPs=\"%N\";", prof->ipStart);
	    websWrite(wp, "var EndIPs=\"%N\";", prof->ipEnd);
	    websWrite(wp, "var dst_FromIPs=\"%N\";", prof->dst_ipStart);
	    websWrite(wp, "var dst_EndIPs=\"%N\";", prof->dst_ipEnd);
	    websWrite(wp, "var policy=\"%d\";", prof->limitPolicy);/*���ٲ���*/
	    websWrite(wp, "var uRate=\"%d\";", prof->uRate);/*�ϴ�����*/
	    websWrite(wp, "var dRate=\"%d\";", prof->dRate);/*��������*/
	}
	else/*������Ϊ����*/
	{

	    if(prof->addrTypeEn == ADDR_IPRANGE)
	    {
		websWrite(wp,"var FromIPs = \"%N\";",prof->ipStart);
		websWrite(wp,"var EndIPs = \"%N\";",prof->ipEnd);
		websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPRANGE);
	    }
#if (IP_GRP == FYES)
	    else
	    {
		websWrite(wp,"var ipGrpNames = \"%s\";",prof->ipGrpName);
		websWrite(wp,"var sourceIPs = \"%d\";",ADDR_IPGRP);
	    }
#endif
#if 0
	    strcpy(pdbPolicys, prof->pdbPolicy);
	    tmp = strtok(pdbPolicys, UTT_SPLIT_SIGN_RECORDS);
	    while(tmp != NULL){
		websWrite(wp, "%s= \"on\";", tmp);
		tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
	    }
#endif
	    websWritePdbName(wp, prof->pdbPolicy1);
	}
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#else

/*
 *����ʵ���Ĳ�����ȡ����
 */
static int getOneGroupBaseConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name = NULL;
    const char *errMsg=NULL;
    GroupBaseProfile *prof;
    UINT32 d_temp;
    char_t data[16];
    char_t *tmp = NULL;
    char_t pdbPolicys[256];
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;/*Ĭ�ϴ�ӡ��ϸ������ҳ��*/ 

    if(ejArgs(argc,argv,T("%s"), &edit_name) < 1) {
	errMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }

    if(edit_name == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_GROUP_NAME_NULL);
	setTheErrorMsg(errMsg);
	goto out;
    }

    webGroupType = getWebGroupType(wp);/*��ȡҳ��Ҫ��ʾ��������*/
    prof = (GroupBaseProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
	setTheErrorMsg(errMsg);
    } else {
	websWrite(wp, "var GroupNames=\"%s\";", prof->head.name);
	websWrite(wp, "var FromIPs=\"%N\";", prof->ipStart);
	websWrite(wp, "var EndIPs=\"%N\";", prof->ipEnd);
	websWrite(wp, "var group_timestarts=\"%s\";", prof->timeStart);
	websWrite(wp, "var group_timestops=\"%s\";", prof->timeStop);
	d_temp = prof->day;
	Byte2Days(d_temp,data);
	websWrite(wp, "var group_days=\"%s\";", data);
	if(webGroupType == GBT_FINE_LIMIT)
	{/*��ϸ������*/
	    websWrite(wp, "var policy=\"%d\";", prof->limitPolicy);/*���ٲ���*/
	    websWrite(wp, "var uRate=\"%d\";", prof->uRate);/*�ϴ�����*/
	    websWrite(wp, "var dRate=\"%d\";", prof->dRate);/*��������*/
	}
	else/*������Ϊ����*/
	{
	    strcpy(pdbPolicys, prof->pdbPolicy);
	    tmp = strtok(pdbPolicys, UTT_SPLIT_SIGN_RECORDS);
	    while(tmp != NULL){
		websWrite(wp, "%s= \"on\";", tmp);
		tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
	    }
	}
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

static int coefficient(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var coefficient = %d;",WEB_COEFFICIENT);
#if ((UTT_NV_A308W == FYES)  || (UTT_NV_A310Wv2 == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A309W == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES) || (UTT_NV_A750W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_A755W == FYES)|| (UTT_NV_A650W_INT == FYES) || (UTT_NV_U1000NV2 == FYES) || (UTT_NV_A655W == FYES)||(UTT_NV_A658W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0)) 
    websWrite(wp, "homeuser = 1;");
#endif

#if (YAHOO_EN == FYES)
    websWrite(wp, "var yahoo_mark= 1;");
#endif
#if 0
    websWrite(wp,"var ip_num = %d;",UTT_MAX_GROUP_IP);
#endif
    return 0;
}

#ifdef CONFIG_UTT_NETFILTER
#define LIST_PDB_STR_LEN  40
#define LIST_PDB_ELLIPSIS "..."

/*
 * ����pdbPolicy bitλ��ȡ�б�����ʾ�Ľ�ֹӦ����(QQ;MSN;...)
 */
static char *getDenyPdbString(Uint8 *pdbPolicy) {
    int i, min, max;
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    static char buf[LIST_PDB_STR_LEN + 1] = {0};
    int len = sizeof(buf) - strlen(LIST_PDB_ELLIPSIS);
    int len1 = 0;
    int needSplit = 0;

    memset(buf, 0, sizeof(buf));
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) { /* ����pdbʵ�����ҳ���Ч��pdb�� */
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0) {
	    if ((prof->index < UTT_MAX_PDB_POLICY_CNT) && pdb_policy_match(pdbPolicy, prof->index)) {
		len1 = strlen(prof->head.name); /* ��ȡʵ�������� */
		if ((len1 + 1) < len) { /* buf��������ʵ��������, ������ */
		    strcat(buf, prof->head.name);
		    strcat(buf, UTT_SPLIT_SIGN_RECORDS);
		    len -= len1;
		    len -= 1;
		} else {
		    needSplit = 1;
		    break;
		}
	    }
	}
    }

    if (needSplit == 1) {
	strcat(buf, LIST_PDB_ELLIPSIS);
    }

    return buf;
}

#if (UTT_SMART_UI == FYES)
/* �·��UI��������Ϊ��������ҳ��Ĳ����������*/
static int getGroupConfigList_Sui(int eid, webs_t wp, int argc, char_t **argv)
{
    int min = 0, max = 0;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;/*Ĭ�ϴ�ӡ��ϸ������ҳ��*/ 
    int i = 0;
    int totalrecs = 0;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;

#if(UTT_SMART_UI == FYES)
    getTheRightMsg(eid, wp, argc, argv);
#endif

    char_t *GroupName=T("GroupNames[%d] = \"%s\";");
    char_t *FromIP =T("FromIPs[%d] = \"%N\";");
    char_t *EndIP = T("EndIPs[%d] = \"%N\"; ");
    char_t *macAddrFmt = T("mac_addr[%d] = \"%d\"; ");
    char_t *Allow= T("Allows[%d] = \"%d\"; ");
    char_t *uRateFmt = T("uRate[%d] = \"%d\"; ");
    char_t *dRateFmt = T("dRate[%d] = \"%d\"; ");
    char_t *policyFmt = T("policy[%d] = \"%d\"; ");

    webGroupType = getWebGroupType(wp);/*��ȡҳ��Ҫ��ʾ��������*/
    websWrite(wp,"var GroupNames=new Array();");
    websWrite(wp,"var sourceIPs =new Array();");
    websWrite(wp,"var FromIPs = new Array();");
    websWrite(wp,"var EndIPs=new Array();");
    websWrite(wp,"var mac_addr =new Array();");
    websWrite(wp,"var ipGrpNames =new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var policy=new Array();");
    websWrite(wp,"var dRate=new Array();");
    websWrite(wp,"var uRate=new Array();");
    websWrite(wp,"var max_totalrecs=%d;", MAX_FINE_LIMIT_PROFILES);

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(prof->groupType == webGroupType)/*ֻ���뵱ǰҳ����Ϣƥ��������ʾ*/
	    {
#if (MACADDR_ADD_LIMIT == FYES)
		if(prof->limitPolicy == 3)
		{

		    websWrite(wp, GroupName, totalrecs, prof->head.name);
		    websWrite(wp, Allow, totalrecs, prof->head.active);
		 
		    websWrite(wp,"macadds[%d] = \"%s\";",totalrecs,converMac6To17Byte(prof->mac_addr));
		    websWrite(wp,"sourceIPs[%d] = \"%d\";",totalrecs,2);
		    websWrite(wp, macAddrFmt, totalrecs, prof->mac_addr); 
		    websWrite(wp, policyFmt, totalrecs, prof->limitPolicy);/*���ٲ���*/
		    websWrite(wp, uRateFmt, totalrecs, prof->uRate);/*�ϴ�����*/
		    websWrite(wp, dRateFmt, totalrecs, prof->dRate);/*��������*/
		    totalrecs++;
		}else
#endif
		{
		    websWrite(wp, GroupName, totalrecs, prof->head.name);
		    websWrite(wp, Allow, totalrecs, prof->head.active);
		    if(prof->addrTypeEn == ADDR_IPRANGE)
		    {
			websWrite(wp,"FromIPs[%d] = \"%N\";",totalrecs,prof->ipStart);
			websWrite(wp,"EndIPs[%d] = \"%N\";",totalrecs,prof->ipEnd);
			websWrite(wp,"sourceIPs[%d] = \"%d\";",totalrecs,ADDR_IPRANGE);
		    }
#if (IP_GRP == FYES)
		    else
		    {
			const char * ipGrpMsg = NULL;
			if(strlen(prof->ipGrpName) == 0)
			{
			    ipGrpMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
			    websWrite(wp,"ipGrpNames[%d] = \"%s\";",totalrecs, ipGrpMsg);
			}
			else
			{
			    websWrite(wp,"ipGrpNames[%d] = \"%s\";",totalrecs,prof->ipGrpName);
			}
			websWrite(wp,"sourceIPs[%d] = \"%d\";",totalrecs,ADDR_IPGRP);
		    }
#endif
		    websWrite(wp, FromIP, totalrecs, prof->ipStart);
		    websWrite(wp, EndIP, totalrecs, prof->ipEnd);
		    websWrite(wp, policyFmt, totalrecs, prof->limitPolicy);/*���ٲ���*/
		    websWrite(wp, uRateFmt, totalrecs, prof->uRate);/*�ϴ�����*/
		    websWrite(wp, dRateFmt, totalrecs, prof->dRate);/*��������*/
		    totalrecs++;
		}
	    }
	}
    }

    websWrite(wp,"var totalrecs=%d;",totalrecs);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif


/*
 * ������Ϊ��������ҳ��Ĳ����������
 *
 */
static int getGroupConfigList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t data[28]={0};
    int min = 0, max = 0;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;/*Ĭ�ϴ�ӡ��ϸ������ҳ��*/ 
    int i = 0;
    int totalrecs = 0;
    UINT32 d_temp;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
#ifdef CONFIG_UTT_NETFILTER
    SystemProfile *profSys = NULL;
#endif

    char_t *GroupName=T("GroupNames[%d] = \"%s\";");
    char_t *FromIP =T("FromIPs[%d] = \"%N\";");
    char_t *EndIP = T("EndIPs[%d] = \"%N\"; ");
    char_t *Allow= T("Allows[%d] = \"%d\"; ");
    char_t *group_day = T("group_days[%d] = \"%s\"; ");
    char_t *group_timestart = T("group_timestarts[%d] = \"%s\"; ");
    char_t *group_timestop = T("group_timestops[%d] = \"%s\"; ");
    char_t *PdbPolicy= T("pdbPolicys[%d] = \"%s\"; ");

    char_t *uRateFmt = T("uRate[%d] = \"%d kbit/s\"; ");
    char_t *dRateFmt = T("dRate[%d] = \"%d kbit/s\"; ");
    char_t *policyFmt = T("policy[%d] = \"%d\"; ");

    webGroupType = getWebGroupType(wp);/*��ȡҳ��Ҫ��ʾ��������*/
    websWrite(wp,"var GroupNames=new Array();");
    websWrite(wp,"var sourceIPs =new Array();");
    websWrite(wp,"var FromIPs = new Array();");
    websWrite(wp,"var EndIPs=new Array();");
    websWrite(wp,"var ipGrpNames =new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var group_days=new Array();");
    websWrite(wp,"var group_timestarts=new Array();");
    websWrite(wp,"var group_timestops = new Array();");


    char_t *dst_FromIP =T("dst_FromIPs[%d] = \"%N\";");
    char_t *dst_EndIP = T("dst_EndIPs[%d] = \"%N\"; ");

    websWrite(wp,"var dst_sourceIPs =new Array();");
    websWrite(wp,"var dst_FromIPs = new Array();");
    websWrite(wp,"var dst_EndIPs=new Array();");
    websWrite(wp,"var dst_ipGrpNames =new Array();");

    if(webGroupType == GBT_ACT_CONTROL)/*ҳ��Ϊ ��Ϊ����ҳ��*/
    {
#ifdef CONFIG_UTT_NETFILTER
	profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	if(profSys->UttNfRuleEn == FUN_ENABLE) {
	    websWrite(wp, "uttNfEnable=\"%s\";", T("on"));
	} else {
	    websWrite(wp, "uttNfEnable=\"%s\";",T("off"));
	}
#endif
	websWrite(wp,"var pdbPolicys= new Array();");
	websWrite(wp,"var pdbNames=new Array();");
	websWrite(wp,"var max_totalrecs=%d;", MAX_ACT_CONTROL_PROFILES);
    }
    else/*ҳ��Ϊ��ϸ������ҳ��*/
    {
	websWrite(wp,"var policy=new Array();");
	websWrite(wp,"var dRate=new Array();");
	websWrite(wp,"var uRate=new Array();");
	websWrite(wp,"var max_totalrecs=%d;", MAX_FINE_LIMIT_PROFILES);
    }
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) 
	{
	    if(prof->groupType == webGroupType)/*ֻ���뵱ǰҳ����Ϣƥ��������ʾ*/
	    {
		websWrite(wp, GroupName, totalrecs, prof->head.name);
		websWrite(wp, Allow, totalrecs, prof->head.active);
		d_temp = prof->day;
		Byte2Days(d_temp,data);
		websWrite(wp, group_day, totalrecs,data);
		websWrite(wp, group_timestart, totalrecs, prof->timeStart);
		websWrite(wp, group_timestop, totalrecs, prof->timeStop);
		if(prof->addrTypeEn == ADDR_IPRANGE) 
		{
		    websWrite(wp,"FromIPs[%d] = \"%N\";",totalrecs,prof->ipStart);
		    websWrite(wp,"EndIPs[%d] = \"%N\";",totalrecs,prof->ipEnd);
		    websWrite(wp,"sourceIPs[%d] = \"%d\";",totalrecs,ADDR_IPRANGE);
		}
#if (IP_GRP == FYES)
		else
		{
		    const char * ipGrpMsg = NULL;
		    if(strlen(prof->ipGrpName) == 0) {
			ipGrpMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
			websWrite(wp,"ipGrpNames[%d] = \"%s\";",totalrecs, ipGrpMsg);
		    } else {
			websWrite(wp,"ipGrpNames[%d] = \"%s\";",totalrecs,prof->ipGrpName);
		    }
		    websWrite(wp,"sourceIPs[%d] = \"%d\";",totalrecs,ADDR_IPGRP);
		}
#endif

		if(prof->dst_addrTypeEn == ADDR_IPRANGE)
		{
		    websWrite(wp,"dst_FromIPs[%d] = \"%N\";",totalrecs,prof->dst_ipStart);
		    websWrite(wp,"dst_EndIPs[%d] = \"%N\";",totalrecs,prof->dst_ipEnd);
		    websWrite(wp,"dst_sourceIPs[%d] = \"%d\";",totalrecs,ADDR_IPRANGE);

		}
#if(IP_GRP == FYES)
		else
		{
		    const char * dst_ipGrpMsg = NULL;
		    if(strlen(prof->dst_ipGrpName) == 0){
			dst_ipGrpMsg = getTransDataForC(C_LANG_INDEX_ALL_USERS);
			websWrite(wp,"dst_ipGrpNames[%d] = \"%s\";",totalrecs, dst_ipGrpMsg);         
		    }else{                                                                  
			websWrite(wp,"dst_ipGrpNames[%d] = \"%s\";",totalrecs,prof->dst_ipGrpName);   
		    }                                                                         
		    websWrite(wp,"dst_sourceIPs[%d] = \"%d\";",totalrecs,ADDR_IPGRP);   

		}
#endif
		if(webGroupType == GBT_FINE_LIMIT)
		{/*��ϸ������*/
		    websWrite(wp, FromIP, totalrecs, prof->ipStart);
		    websWrite(wp, EndIP, totalrecs, prof->ipEnd);
		    websWrite(wp, dst_FromIP, totalrecs, prof->dst_ipStart);
		    websWrite(wp, dst_EndIP, totalrecs, prof->dst_ipEnd); 
		    websWrite(wp, policyFmt, totalrecs, prof->limitPolicy);/*���ٲ���*/
		    websWrite(wp, uRateFmt, totalrecs, prof->uRate);/*�ϴ�����*/
		    websWrite(wp, dRateFmt, totalrecs, prof->dRate);/*��������*/
		}
		else/*������Ϊ����*/
		{
#if 0
		    websWrite(wp, PdbPolicy, totalrecs, prof->pdbPolicy);
#endif
		    websWrite(wp, PdbPolicy, totalrecs, getDenyPdbString((Uint8 *)(prof->pdbPolicy1)));
		}
#if 0
		printf("totalrecs = %s GroupName = %s FromIP = %s EndIP = %s group_day = %s group_timestart = %s group_timestop = %s\n",totalrecs, prof->head.name, prof->ipStart, prof->ipEnd,data,prof->timeStart,prof->timeStop);
#endif
		totalrecs++;
	    }
	}
    }

    websWrite(wp,"var totalrecs=%d;",totalrecs);
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid, wp, argc, argv);
    return 0;
}
#else

/*
 * ������Ϊ��������ҳ��Ĳ����������
 *
 */
static int getGroupConfigList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t data[28]={0};
    int min = 0, max = 0;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;/*Ĭ�ϴ�ӡ��ϸ������ҳ��*/ 
    int i = 0;
    int totalrecs = 0;
    UINT32 d_temp;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;

    char_t *GroupName=T("GroupNames[%d] = \"%s\";");
    char_t *FromIP =T("FromIPs[%d] = \"%N\";");
    char_t *EndIP = T("EndIPs[%d] = \"%N\"; ");
    char_t *Allow= T("Allows[%d] = \"%d\"; ");
    char_t *group_day = T("group_days[%d] = \"%s\"; ");
    char_t *group_timestart = T("group_timestarts[%d] = \"%s\"; ");
    char_t *group_timestop = T("group_timestops[%d] = \"%s\"; ");
    char_t *PdbPolicy= T("pdbPolicys[%d] = \"%s\"; ");

    char_t *uRateFmt = T("uRate[%d] = \"%d kbit/s\"; ");
    char_t *dRateFmt = T("dRate[%d] = \"%d kbit/s\"; ");
    char_t *policyFmt = T("policy[%d] = \"%d\"; ");

    webGroupType = getWebGroupType(wp);/*��ȡҳ��Ҫ��ʾ��������*/
    websWrite(wp,"var GroupNames=new Array();");
    websWrite(wp,"var FromIPs = new Array();");
    websWrite(wp,"var EndIPs=new Array();");
    websWrite(wp,"var Allows=new Array();");
    websWrite(wp,"var group_days=new Array();");
    websWrite(wp,"var group_timestarts=new Array();");
    websWrite(wp,"var group_timestops = new Array();");

    if(webGroupType == GBT_ACT_CONTROL)/*ҳ��Ϊ ��Ϊ����ҳ��*/
    {
	websWrite(wp,"var pdbPolicys= new Array();");
	websWrite(wp,"var pdbNames=new Array();");
	websWrite(wp,"var max_totalrecs=%d;", MAX_ACT_CONTROL_PROFILES);
    }
    else/*ҳ��Ϊ��ϸ������ҳ��*/
    {
	websWrite(wp,"var policy=new Array();");
	websWrite(wp,"var dRate=new Array();");
	websWrite(wp,"var uRate=new Array();");
	websWrite(wp,"var max_totalrecs=%d;", MAX_FINE_LIMIT_PROFILES);
    }



    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(prof->groupType == webGroupType)/*ֻ���뵱ǰҳ����Ϣƥ��������ʾ*/
	    {
		websWrite(wp, GroupName, totalrecs, prof->head.name);
		websWrite(wp, Allow, totalrecs, prof->head.active);
		websWrite(wp, FromIP, totalrecs, prof->ipStart);
		websWrite(wp, EndIP, totalrecs, prof->ipEnd);
		d_temp = prof->day;
		Byte2Days(d_temp,data);
		websWrite(wp, group_day, totalrecs,data);
		websWrite(wp, group_timestart, totalrecs, prof->timeStart);
		websWrite(wp, group_timestop, totalrecs, prof->timeStop);
		if(webGroupType == GBT_FINE_LIMIT)
		{/*��ϸ������*/
		    websWrite(wp, policyFmt, totalrecs, prof->limitPolicy);/*���ٲ���*/
		    websWrite(wp, uRateFmt, totalrecs, prof->uRate);/*�ϴ�����*/
		    websWrite(wp, dRateFmt, totalrecs, prof->dRate);/*��������*/
		}
		else/*������Ϊ����*/
		{
		    websWrite(wp, PdbPolicy, totalrecs, prof->pdbPolicy);
		}
#if 0
		printf("totalrecs = %s GroupName = %s FromIP = %s EndIP = %s group_day = %s group_timestart = %s group_timestop = %s\n",totalrecs, prof->head.name, prof->ipStart, prof->ipEnd,data,prof->timeStart,prof->timeStop);
#endif
		totalrecs++;
	    }
	}
    }

    websWrite(wp,"var totalrecs=%d;",totalrecs);
    getTheErrorMsg(eid, wp, argc, argv);
    getTheRightMsg(eid, wp, argc, argv);
    return 0;
}
#endif

/********************************************************************
 * ������ formGroupActionAllow 
 * ���ܣ� �б��е�����
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� yqyang    
 * ������ 2011-09-13
 *******************************************************************/
static void formGroupActionAllow(webs_t wp, char *path, char *query)
{
    UINT32 Allow;
    char_t *AllowID;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL;
    int instIndex;
    const char *errMsg=NULL;

    Allow = (UINT32)strtoul(websGetVar(wp, T("Allow"), NULL),NULL,0);
    AllowID = websGetVar(wp, T("AllowID"), NULL);

    if (strlen(AllowID) == 0)
    {
	/* check variable */
	errMsg = getTransDataForC(C_LANG_INDEX_SEL_USER);
	setTheErrorMsg(errMsg);
	goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    instIndex = ProfGetInstIndexByName(mibType, AllowID);
    if(instIndex == PROFFAIL)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_INST_NOT_FIND);
	setTheErrorMsg(errMsg);
	goto out;
    }

    uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    profGroupBase->head.active = Allow;
    uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
    // printf("AllowID = %s instIndex = %d Allow = %d active = %d\n", AllowID,instIndex,Allow,profGroupBase->head.active);
out:
    websRedirect(wp, "GlobalBase.asp");/*ҳ����ת*/
}


/********************************************************************
 * ������ checkConfigOver
 * ���ܣ� ��������Ƿ����ǰ�Ĺ����ص�
 * ������ ��
 * ����� ��
 * ���أ� return 1 ��ʾ�ص� return 0 ��ʾ���ص�  
 * ���ߣ� yqyang    
 * ������ 2011-12-05
 *******************************************************************/
int checkConfigOver(GroupBaseType webGroupType,IPADDR startIp, IPADDR endIp, char *day,char *timestart , char *timestop,char *oldeditname)
{
    int max = 0, min = 0, ret = 0,i; 
    GroupBaseProfile *prof = NULL;

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(oldeditname != NULL && (strcmp(oldeditname, prof->head.name) == 0))
	    {

	    }else{
		if(
			webGroupType == prof->groupType
			&& (prof->ipStart == startIp) 
			&& (prof->ipEnd == endIp) 
			&& prof->day == days2Byte(day) 
			&& (strcmp(prof->timeStart,timestart) == 0) 
			&& (strcmp(prof->timeStop, timestop) == 0)
		  )
		{
		    ret = 1;
		}
	    }
	}
    }
    return ret;
}

#ifdef CONFIG_UTT_NETFILTER
#if (UTT_SMART_UI == FYES)

#if (MACADDR_ADD_LIMIT == FYES)
/*xrg����mac��������ip����ʹ�ã��������else*/
static void formConfigGroupConfig_Sui(webs_t wp, char_t *path, char_t *query)
{
    GroupBaseType webGroupType = GBT_FINE_LIMIT; 

    char_t *GroupName = NULL,*GroupNameOld = NULL,*FromIp = NULL,*EndIp = NULL,*sourceIP =NULL ,*macaddress = NULL ;

    MacAddr macAddr;

    char_t *policyStr = NULL, *uRateStr = NULL, *dRateStr = NULL;
    char_t *action;
    const char *errMsg=NULL;

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL,*prof_limit = NULL;
    struct in_addr addr_fromip, addr_endip;
    int instIndex;
    int semFlag = 0;
    int max = 0, min = 0;
    Uint32 maxProf = 0u;
    a_assert(wp);

    policyStr = websGetVar(wp,T("policy"), T(""));/*���ٲ���*/

    if(strncmp(policyStr,"3",1)==0)/*mac����*/
    {
	GroupName=websGetVar(wp,T("GroupName"), "");
	sourceIP = websGetVar(wp,T("sourceIP"), T(""));
	macaddress=websGetVar(wp,T("writeMacAddress"),T(""));

	maxProf = MAX_FINE_LIMIT_PROFILES;/*��ϸ���������ʵ����*/
	dRateStr = websGetVar(wp,T("dRate"), NULL);/*��������*/
	uRateStr = websGetVar(wp,T("uRate"), NULL);/*�ϴ�����*/

	action = websGetVar(wp, T("Action"), T("")); 

	GroupNameOld= websGetVar(wp, T("GroupNameOld"), T("")); 

	ProfInstLowHigh(mibType, &max, &min);
	uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
	semFlag = 1;
	if(strcmp(action,"add") == 0)
	{
	    instIndex = ProfGetInstIndexByName(mibType, GroupName);
	    if(instIndex != PROFFAIL) 
	    {
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
		errMsg = "The group name already exists in the bandwidth setting in the list";
#else
		errMsg = "�������Ѵ����ڴ��������б���!";
#endif
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    /*����mac�͵ײ���ж�*/
	    int i;
	    if (strlen(macaddress) != 0U) 
	    {
		converMac12To6Byte(macaddress, &macAddr);
	    }
	    for(i = min; i < max; i++)
	    {
		prof_limit = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
		if(prof_limit != NULL && ProfInstIsFree(prof_limit) == 0)
		{
		    if(memcmp(&(prof_limit->mac_addr),&macAddr,6)==0)
		    {
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
			errMsg = "The MAC already exists";
#else
			errMsg = "��mac�Ѿ�����!";
#endif
			setTheErrorMsg(errMsg);
			goto out;
		    }
		}
	    }


	    /*ͳ��ҳ���Ӧ�������Ƿ�ʵ���Ѿ���ӵ����*/
	    if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    instIndex = ProfNewInst(mibType, GroupName, FALSE);
	    profAction = PROF_CHANGE_ADD;
	}
	else /*modify*/
	{
	    instIndex = ProfGetInstIndexByName(mibType, GroupNameOld);
	    if(instIndex == PROFFAIL) {
		errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    if (strcmp(GroupNameOld ,GroupName) != 0){
		int newIndex;
		newIndex = ProfGetInstIndexByName(mibType, GroupName);
		if(newIndex != PROFFAIL)
		{
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
		errMsg = "The group name already exists in the bandwidth setting in the list";
#else
		errMsg = "�������Ѵ����ڴ��������б���!";
#endif
		    setTheErrorMsg(errMsg);
		    goto out;
		}
	    }
	    profAction = PROF_CHANGE_SPECIAL;
	}
	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
	profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if(profAction == PROF_CHANGE_ADD)
	{
	    profGroupBase->groupType = webGroupType;/*��¼���ͷ��㲻ͬ��ҳ��ʹ��*/
	    /*mantis11137*/
	    profGroupBase->ProfIndex = instIndex + 1;
	}
	/*�����жϷ�ֹżȻ������ֵ� ҳ���޸Ĳ���ʱ ҳ��������������Ͳ�ƥ��*/
	ProfSetNameByPoint(profGroupBase, GroupName);
	profGroupBase->day= 127; 
	strcpy(profGroupBase->timeStart, "All"); 
	strcpy(profGroupBase->timeStop, "0");
	/*����mac��ַ�������ݽṹ*/
	  memset(&macAddr, 0, sizeof(macAddr));
	  if (strlen(macaddress) != 0U) 
	  {
	    printf("xrg   macaddr = %s\n",macaddress);
	    converMac12To6Byte(macaddress, &macAddr);
	    profGroupBase->mac_addr = macAddr;
	  }
	  else
	  {
		printf("error mac\n");	
	  }
	profGroupBase->limitPolicy = strtoul(policyStr, NULL, 0);/*���ٲ���*/
	profGroupBase->uRate = strtoul(uRateStr, NULL, 0);/*�ϴ�����*/
	profGroupBase->dRate = strtoul(dRateStr, NULL, 0);/*��������*/

	semFlag = 0;
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    }
    else
    { 
	GroupName=websGetVar(wp,T("GroupName"), "");
	FromIp=websGetVar(wp,T("FromIP"), NULL);
	EndIp=websGetVar(wp,T("EndIP"), NULL);

	sourceIP = websGetVar(wp,T("sourceIP"), T(""));
	if(strcmp(sourceIP, "iprange") == 0)
	{
	    if(inet_aton(FromIp, &addr_fromip) == 0)
	    {  
		errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    if(inet_aton(EndIp, &addr_endip) == 0){  
		errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
	maxProf = MAX_FINE_LIMIT_PROFILES;/*��ϸ���������ʵ����*/
	dRateStr = websGetVar(wp,T("dRate"), NULL);/*��������*/
	uRateStr = websGetVar(wp,T("uRate"), NULL);/*�ϴ�����*/
	BEHAVIOR_PRINTF("%s-----%d,FromIP = %s, EndIP = %s, dRateStr = %s, uRateStr = %s, policyStr = %s\n", __func__, __LINE__, FromIp, EndIp, dRateStr, uRateStr, policyStr);
	action = websGetVar(wp, T("Action"), T("")); 
	GroupNameOld= websGetVar(wp, T("GroupNameOld"), T("")); 

	ProfInstLowHigh(mibType, &max, &min);

	uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
	semFlag = 1;
	if(strcmp(action,"add") == 0)
	{
	    instIndex = ProfGetInstIndexByName(mibType, GroupName);
	    if(instIndex != PROFFAIL) 
	    {
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
		errMsg = "The group name already exists in the bandwidth setting in the list";
#else
		errMsg = "�������Ѵ����ڴ��������б���!";
#endif
		setTheErrorMsg(errMsg);
		goto out;
	    }

	    /*ͳ��ҳ���Ӧ�������Ƿ�ʵ���Ѿ���ӵ����*/
	    if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    instIndex = ProfNewInst(mibType, GroupName, FALSE);
	    profAction = PROF_CHANGE_ADD;
	}
	else /*modify*/
	{
	    instIndex = ProfGetInstIndexByName(mibType, GroupNameOld);
	    if(instIndex == PROFFAIL) {
		errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	    if (strcmp(GroupNameOld ,GroupName) != 0){
		int newIndex;
		newIndex = ProfGetInstIndexByName(mibType, GroupName);
		if(newIndex != PROFFAIL)
		{
		    errMsg = "�������Ѵ����ڴ��������б���!";
		    setTheErrorMsg(errMsg);
		    goto out;
		}
	    }
	    profAction = PROF_CHANGE_SPECIAL;
	}
	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
	profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if(profAction == PROF_CHANGE_ADD)
	{
	    profGroupBase->groupType = webGroupType;/*��¼���ͷ��㲻ͬ��ҳ��ʹ��*/
	    /*mantis11137*/
	    profGroupBase->ProfIndex = instIndex + 1;
	}
	/*�����жϷ�ֹżȻ������ֵ� ҳ���޸Ĳ���ʱ ҳ��������������Ͳ�ƥ��*/
	ProfSetNameByPoint(profGroupBase, GroupName);
	profGroupBase->day= 127; 
	strcpy(profGroupBase->timeStart, "All"); 
	strcpy(profGroupBase->timeStop, "0");
	if(strcmp(sourceIP, "iprange") == 0)
	{
	    profGroupBase->addrTypeEn = ADDR_IPRANGE; 
	    profGroupBase->ipStart = addr_fromip.s_addr; 
	    profGroupBase->ipEnd = addr_endip.s_addr; 
	}
	profGroupBase->limitPolicy = strtoul(policyStr, NULL, 0);/*���ٲ���*/
	profGroupBase->uRate = strtoul(uRateStr, NULL, 0);/*�ϴ�����*/
	profGroupBase->dRate = strtoul(dRateStr, NULL, 0);/*��������*/

	semFlag = 0;
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    }
    if((profAction == PROF_CHANGE_ADD)
	    && (moveLastToFirst(profGroupBase->head.name) == 1))
    {/*��ϸ������*/
	BEHAVIOR_PRINTF("%s-----%d, profGroupBase->head.name = %s\n", __func__, __LINE__, profGroupBase->head.name);
    }
    else
    {
	ProfUpdate(profhead);
    }
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    if(semFlag == 1)
    {
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
#endif
    websRedirect(wp, "Group_Sui.asp");/*ҳ����ת*/
}

#else

/*�·��UI �������ñ���*/
static void formConfigGroupConfig_Sui(webs_t wp, char_t *path, char_t *query)
{
    GroupBaseType webGroupType = GBT_FINE_LIMIT; 
    char_t *GroupName = NULL,*GroupNameOld = NULL,*FromIp = NULL,*EndIp = NULL,*sourceIP =NULL;
    char_t *policyStr = NULL, *uRateStr = NULL, *dRateStr = NULL;
    char_t *action;
    const char *errMsg=NULL;

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL;
    struct in_addr addr_fromip, addr_endip;
    int instIndex;
    int semFlag = 0;
    int max = 0, min = 0 ;
    Uint32 maxProf = 0u;
    a_assert(wp);

    GroupName=websGetVar(wp,T("GroupName"), "");
    FromIp=websGetVar(wp,T("FromIP"), NULL);
    EndIp=websGetVar(wp,T("EndIP"), NULL);

    sourceIP = websGetVar(wp,T("sourceIP"), T(""));
    if(strcmp(sourceIP, "iprange") == 0)
    {
	if(inet_aton(FromIp, &addr_fromip) == 0)
	{  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if(inet_aton(EndIp, &addr_endip) == 0){  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
    }
    maxProf = MAX_FINE_LIMIT_PROFILES;/*��ϸ���������ʵ����*/
    dRateStr = websGetVar(wp,T("dRate"), NULL);/*��������*/
    uRateStr = websGetVar(wp,T("uRate"), NULL);/*�ϴ�����*/
    policyStr = websGetVar(wp,T("policy"), T(""));/*���ٲ���*/
    BEHAVIOR_PRINTF("%s-----%d,FromIP = %s, EndIP = %s, dRateStr = %s, uRateStr = %s, policyStr = %s\n", __func__, __LINE__, FromIp, EndIp, dRateStr, uRateStr, policyStr);
    action = websGetVar(wp, T("Action"), T("")); 
    GroupNameOld= websGetVar(wp, T("GroupNameOld"), T("")); 

    ProfInstLowHigh(mibType, &max, &min);

    uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
    semFlag = 1;
    if(strcmp(action,"add") == 0)
    {
	instIndex = ProfGetInstIndexByName(mibType, GroupName);
	if(instIndex != PROFFAIL) 
	{
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
		errMsg = "The group name already exists in the bandwidth setting in the list";
#else
		errMsg = "�������Ѵ����ڴ��������б���!";
#endif
	    setTheErrorMsg(errMsg);
	    goto out;
	}

	/*ͳ��ҳ���Ӧ�������Ƿ�ʵ���Ѿ���ӵ����*/
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, GroupName, FALSE);
	profAction = PROF_CHANGE_ADD;
    }
    else /*modify*/
    {
	instIndex = ProfGetInstIndexByName(mibType, GroupNameOld);
	if(instIndex == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if (strcmp(GroupNameOld ,GroupName) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, GroupName);
	    if(newIndex != PROFFAIL)
	    {
#if(CONFIG_USER_GOAHEAD_LANG_EN == FYES)
		errMsg = "The group name already exists in the bandwidth setting in the list";
#else
		errMsg = "�������Ѵ����ڴ��������б���!";
#endif
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
	profAction = PROF_CHANGE_SPECIAL;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if(profAction == PROF_CHANGE_ADD)
    {
	profGroupBase->groupType = webGroupType;/*��¼���ͷ��㲻ͬ��ҳ��ʹ��*/
	/*mantis11137*/
	profGroupBase->ProfIndex = instIndex + 1;
    }
    /*�����жϷ�ֹżȻ������ֵ� ҳ���޸Ĳ���ʱ ҳ��������������Ͳ�ƥ��*/
    ProfSetNameByPoint(profGroupBase, GroupName);
    profGroupBase->day= 127; 
    strcpy(profGroupBase->timeStart, "All"); 
    strcpy(profGroupBase->timeStop, "0");
    if(strcmp(sourceIP, "iprange") == 0)
    {
	profGroupBase->addrTypeEn = ADDR_IPRANGE; 
	profGroupBase->ipStart = addr_fromip.s_addr; 
	profGroupBase->ipEnd = addr_endip.s_addr; 
    }
    profGroupBase->limitPolicy = strtoul(policyStr, NULL, 0);/*���ٲ���*/
    profGroupBase->uRate = strtoul(uRateStr, NULL, 0);/*�ϴ�����*/
    profGroupBase->dRate = strtoul(dRateStr, NULL, 0);/*��������*/

    semFlag = 0;
    uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    if((profAction == PROF_CHANGE_ADD)
	    && (moveLastToFirst(profGroupBase->head.name) == 1))
    {/*��ϸ������*/
	BEHAVIOR_PRINTF("%s-----%d, profGroupBase->head.name = %s\n", __func__, __LINE__, profGroupBase->head.name);
    }
    else
    {
	ProfUpdate(profhead);
    }
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    if(semFlag == 1)
    {
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
#endif
    websRedirect(wp, "Group_Sui.asp");/*ҳ����ת*/
}
#endif
#endif


/********************************************************************
 * ������ formConfigGroupConfig
 * ���ܣ� ҳ���ύ����
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� yqyang    
 * ������ 2011-09-05
 *******************************************************************/
static void formConfigGroupConfig(webs_t wp, char_t *path, char_t *query)
{

    GroupBaseType webGroupType = GBT_FINE_LIMIT; 
    char_t *GroupName = NULL,*GroupNameOld = NULL,*FromIp = NULL,*EndIp = NULL,*GroupNameType = NULL,*sourceIP =NULL,*PdbPolicy = NULL;
    char_t *dst_FromIp = NULL,*dst_EndIp = NULL,*dst_GroupNameType = NULL,*dst_sourceIP =NULL;
    struct in_addr dst_addr_fromip, dst_addr_endip;
    char_t *policyStr = NULL, *uRateStr = NULL, *dRateStr = NULL;
    char_t *group_day,*group_timestart,*group_timestop;
    char_t *action;
    char PdbPolicy1[UTT_MAX_PDB_POLICY_CNT * 4] = {0};
    const char *errMsg=NULL;

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL;
    struct in_addr addr_fromip, addr_endip;
    int instIndex;
    int semFlag = 0;
    int max = 0, min = 0;
    Uint32 maxProf = 0u;
    a_assert(wp);

    GroupName=websGetVar(wp,T("GroupName"), NULL);
    FromIp=websGetVar(wp,T("FromIP"), NULL);
    EndIp=websGetVar(wp,T("EndIP"), NULL);
    group_day=websGetVar(wp,T("day"), NULL);
    group_timestart=websGetVar(wp,T("timestart"), NULL);
    group_timestop=websGetVar(wp,T("timestop"), NULL);

    webGroupType = getWebGroupType(wp);/*��ȡ��ǰ������������*/
    GroupNameType = websGetVar(wp,T("GroupNameType"), T("")); 
    sourceIP = websGetVar(wp,T("sourceIP"), T(""));

    BEHAVIOR_PRINTF("%s-----%d,FromIP = %s, EndIP = %s, GroupNameType = %s, sourceIP = %s\n", __func__, __LINE__, FromIp, EndIp, GroupNameType, sourceIP);
    dst_FromIp=websGetVar(wp,T("dst_FromIP"), NULL);
    dst_EndIp=websGetVar(wp,T("dst_EndIP"), NULL);
    dst_GroupNameType = websGetVar(wp,T("dst_GroupNameType"), T("")); 
    dst_sourceIP = websGetVar(wp,T("dst_sourceIP"), T(""));

    if(strcmp(sourceIP, "ipRange") == 0)
    {
	BEHAVIOR_PRINTF("%s-----%d---Runs OK---\n", __func__, __LINE__);
	if(inet_aton(FromIp, &addr_fromip) == 0)
	{  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if(inet_aton(EndIp, &addr_endip) == 0){  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
    }
    if(strcmp(dst_sourceIP, "dst_ipRange") == 0)
    {
	BEHAVIOR_PRINTF("%s-----%d---Runs OK---\n", __func__, __LINE__);
	if(inet_aton(dst_FromIp, &dst_addr_fromip) == 0)
	{  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if(inet_aton(dst_EndIp, &dst_addr_endip) == 0){  
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
    }
    if(webGroupType == GBT_ACT_CONTROL)/*��Ϊ����ҳ��*/
    {
	maxProf = MAX_ACT_CONTROL_PROFILES;/*��Ϊ�������ʵ����*/
	PdbPolicy = websGetVar(wp,T("pdbPolicy"), NULL);
    }
    else/*ҳ��Ϊ��ϸ������ҳ��*/
    {
	maxProf = MAX_FINE_LIMIT_PROFILES;/*��ϸ���������ʵ����*/
	dRateStr = websGetVar(wp,T("dRate"), NULL);/*��������*/
	uRateStr = websGetVar(wp,T("uRate"), NULL);/*�ϴ�����*/
	policyStr = websGetVar(wp,T("policy"), T(""));/*���ٲ���*/
	BEHAVIOR_PRINTF("%s-----%d,FromIP = %s, EndIP = %s, dRateStr = %s, uRateStr = %s, policyStr = %s\n", __func__, __LINE__, FromIp, EndIp, dRateStr, uRateStr, policyStr);
	BEHAVIOR_PRINTF("%s-----%d,dst_FromIP = %s, dst_EndIP = %s\n", __func__, __LINE__, dst_FromIp, dst_EndIp);
    }
#if 0
    unsigned int nFromIp , nEndIp;
    nFromIp=ip2int(FromIp);
    nEndIp=ip2int(EndIp);
    printf("nFromIp = %x nEndIp = %x\n",nFromIp,nEndIp);
#endif
    action = websGetVar(wp, T("Action"), T("")); 
    GroupNameOld= websGetVar(wp, T("GroupNameold"), T("")); 


    ProfInstLowHigh(mibType, &max, &min);

    uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
    semFlag = 1;
    profGroupBase  = (GroupBaseProfile*)ProfGetInstPointByIndex(MIB_PROF_GROUP_BASE, 0);
#if 0
    if(addr_endip.s_addr - addr_fromip.s_addr + 1 > UTT_MAX_GROUP_IP)
	if(nEndIp - nFromIp + 1 > UTT_MAX_GROUP_IP)
	{
	    sprintf(error_msg ,"��ip�������ܴ���%d��",UTT_MAX_GROUP_IP);
	    setTheErrorMsg(error_msg);
	    goto out;
	}
#endif
    if(strcmp(action,"add") == 0)
    {
#if 0
	if(checkConfigOver(webGroupType,addr_fromip.s_addr,addr_endip.s_addr,group_day,group_timestart,group_timestop,NULL) == 1)
	{
	    setTheErrorMsg(T("������IP��ַ�κ���Чʱ������Ѿ�����!"));
	    goto out;
	}
#endif
	instIndex = ProfGetInstIndexByName(mibType, GroupName);
	if(instIndex != PROFFAIL) 
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_GROUP_EXIST_IN_LIST);
	    setTheErrorMsg(errMsg);
	    goto out;
	}

	/*ͳ��ҳ���Ӧ�������Ƿ�ʵ���Ѿ���ӵ����*/
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, GroupName, FALSE);
#if 0
	if(instIndex == PROFFAIL) 
	{
	    setTheErrorMsg(T("�Ѵ������Ŀ��"));
	    goto out;
	}
#endif
#if 0
	if(webGroupType == GBT_FINE_LIMIT)
	{
	    profAction = PROF_CHANGE_SPECIAL;
	}
	else
	{
#endif
	    profAction = PROF_CHANGE_ADD;
#if 0
	}
#endif
    }
    else /*modify*/
    {
	instIndex = ProfGetInstIndexByName(mibType, GroupNameOld);
	if(instIndex == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if (strcmp(GroupNameOld ,GroupName) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, GroupName);
	    if(newIndex != PROFFAIL) {
		errMsg = getTransDataForC(C_LANG_INDEX_GROUP_EXIST_IN_LIST);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
#if 0
	if(checkConfigOver(webGroupType,addr_fromip.s_addr,addr_endip.s_addr,group_day,group_timestart,group_timestop,GroupNameOld) == 1)
	{
	    setTheErrorMsg(T("������IP��ַ�κ���Чʱ������Ѿ�����!"));
	    goto out;
	}
#endif
	if(webGroupType == GBT_FINE_LIMIT) {
	    profAction = PROF_CHANGE_SPECIAL;
	} else {
	    profAction = PROF_CHANGE_EDIT;
	}
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if(profAction == PROF_CHANGE_ADD)
    {
	profGroupBase->groupType = webGroupType;/*��¼���ͷ��㲻ͬ��ҳ��ʹ��*/
	/*mantis11137*/
	if(webGroupType == GBT_FINE_LIMIT)
	{/*��ϸ������*/
	    profGroupBase->ProfIndex = instIndex + 1;
	}
    }
    /*�����жϷ�ֹżȻ������ֵ� ҳ���޸Ĳ���ʱ ҳ��������������Ͳ�ƥ��*/
    ProfSetNameByPoint(profGroupBase, GroupName);
    profGroupBase->day= days2Byte(group_day); 
    strcpy(profGroupBase->timeStart, group_timestart); 
    strcpy(profGroupBase->timeStop, group_timestop);
    if(strcmp(sourceIP, "ipRange") == 0) {
	profGroupBase->addrTypeEn = ADDR_IPRANGE; 
	profGroupBase->ipStart = addr_fromip.s_addr; 
	profGroupBase->ipEnd = addr_endip.s_addr; 
    }
#if (IP_GRP == FYES)
    else
    {
	profGroupBase->addrTypeEn = ADDR_IPGRP; 
	strcpy(profGroupBase->ipGrpName, GroupNameType);
    }
#endif

    if(strcmp(dst_sourceIP, "dst_ipRange") == 0) { 
	profGroupBase->dst_addrTypeEn = ADDR_IPRANGE; 
	profGroupBase->dst_ipStart =dst_addr_fromip.s_addr; 
	profGroupBase->dst_ipEnd = dst_addr_endip.s_addr; 
    }
#if (IP_GRP == FYES)
    else {
	profGroupBase->dst_addrTypeEn = ADDR_IPGRP;
	strcpy(profGroupBase->dst_ipGrpName, dst_GroupNameType);
    }
#endif
    if(webGroupType == GBT_FINE_LIMIT)
    {/*��ϸ������*/
	profGroupBase->limitPolicy = strtoul(policyStr, NULL, 0);/*���ٲ���*/
	profGroupBase->uRate = strtoul(uRateStr, NULL, 0);/*�ϴ�����*/
	profGroupBase->dRate = strtoul(dRateStr, NULL, 0);/*��������*/
    }
    else/*������Ϊ����*/
    {
	strncpy(PdbPolicy1, PdbPolicy, UTT_MAX_PDB_POLICY_CNT * 4);
	SetPdbPolicy((Uint8 *)profGroupBase->pdbPolicy1, PdbPolicy1);
    }

    semFlag = 0;
    uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    if((webGroupType == GBT_FINE_LIMIT) && (profAction == PROF_CHANGE_ADD)
	    && (moveLastToFirst(profGroupBase->head.name) == 1))
    {/*��ϸ������*/
	BEHAVIOR_PRINTF("%s-----%d, profGroupBase->head.name = %s\n", __func__, __LINE__, profGroupBase->head.name);
    } else {
	ProfUpdate(profhead);
    }
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    if(semFlag == 1)
    {
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    }
    websRedirect(wp, getRedWebByGbt(webGroupType));/*ҳ����ת*/
}
#else
/********************************************************************
 * ������ formConfigGroupConfig
 * ���ܣ� ҳ���ύ����
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� yqyang    
 * ������ 2011-09-05
 *******************************************************************/
static void formConfigGroupConfig(webs_t wp, char_t *path, char_t *query)
{

    GroupBaseType webGroupType = GBT_FINE_LIMIT; 
    char_t *GroupName,*GroupNameOld,*FromIp,*EndIp,*PdbPolicy;
    char_t *policyStr, *uRateStr, *dRateStr;
    char_t *group_day,*group_timestart,*group_timestop;
    char_t *action;
    const char *errMsg=NULL;

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    GroupBaseProfile *profGroupBase = NULL;
    struct in_addr addr_fromip, addr_endip;
    int instIndex;
    int max = 0, min = 0;
    Uint32 maxProf = 0u;
    a_assert(wp);

    GroupName=websGetVar(wp,T("GroupName"), NULL);
    FromIp=websGetVar(wp,T("FromIP"), NULL);
    EndIp=websGetVar(wp,T("EndIP"), NULL);
    group_day=websGetVar(wp,T("day"), NULL);
    group_timestart=websGetVar(wp,T("timestart"), NULL);
    group_timestop=websGetVar(wp,T("timestop"), NULL);

    webGroupType = getWebGroupType(wp);/*��ȡ��ǰ������������*/
    if(webGroupType == GBT_ACT_CONTROL)/*��Ϊ����ҳ��*/
    {
	maxProf = MAX_ACT_CONTROL_PROFILES;/*��Ϊ�������ʵ����*/
	PdbPolicy = websGetVar(wp,T("pdbPolicy"), NULL);
    }
    else/*ҳ��Ϊ��ϸ������ҳ��*/
    {
	maxProf = MAX_FINE_LIMIT_PROFILES;/*��ϸ���������ʵ����*/
	dRateStr = websGetVar(wp,T("dRate"), NULL);/*��������*/
	uRateStr = websGetVar(wp,T("uRate"), NULL);/*�ϴ�����*/
	policyStr = websGetVar(wp,T("policy"), T(""));/*���ٲ���*/
    }
#if 0
    unsigned int nFromIp , nEndIp;
    nFromIp=ip2int(FromIp);
    nEndIp=ip2int(EndIp);
    printf("nFromIp = %x nEndIp = %x\n",nFromIp,nEndIp);
#endif
    action = websGetVar(wp, T("Action"), T("")); 
    GroupNameOld= websGetVar(wp, T("GroupNameold"), T("")); 

    if(inet_aton(FromIp, &addr_fromip) == 0){  
	errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    if(inet_aton(EndIp, &addr_endip) == 0){  
	errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }

    ProfInstLowHigh(mibType, &max, &min);
    profGroupBase  = (GroupBaseProfile*)ProfGetInstPointByIndex(MIB_PROF_GROUP_BASE, 0);
#if 0
    if(addr_endip.s_addr - addr_fromip.s_addr + 1 > UTT_MAX_GROUP_IP)
	if(nEndIp - nFromIp + 1 > UTT_MAX_GROUP_IP)
	{
	    sprintf(error_msg ,"��ip�������ܴ���%d��",UTT_MAX_GROUP_IP);
	    setTheErrorMsg(error_msg);
	    goto out;
	}
#endif
    if(strcmp(action,"add") == 0)
    {
#if 0
	if(checkConfigOver(webGroupType,addr_fromip.s_addr,addr_endip.s_addr,group_day,group_timestart,group_timestop,NULL) == 1)
	{
	    setTheErrorMsg(T("������IP��ַ�κ���Чʱ������Ѿ�����!"));
	    goto out;
	}
#endif
	instIndex = ProfGetInstIndexByName(mibType, GroupName);
	if(instIndex != PROFFAIL) 
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_GROUP_EXIST_IN_LIST);
	    setTheErrorMsg(errMsg);
	    goto out;
	}

	/*ͳ��ҳ���Ӧ�������Ƿ�ʵ���Ѿ���ӵ����*/
	if(ProfCountInstByValue(mibType, OFFANDSIZE(GroupBaseProfile, groupType), 0u, (Pointer)&webGroupType) >= maxProf)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	instIndex = ProfNewInst(mibType, GroupName, FALSE);
#if 0
	if(instIndex == PROFFAIL) 
	{
	    setTheErrorMsg(T("�Ѵ������Ŀ��"));
	    goto out;
	}
#endif
	profAction = PROF_CHANGE_ADD;
    }
    else /*modify*/
    {
	instIndex = ProfGetInstIndexByName(mibType, GroupNameOld);
	if(instIndex == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if (strcmp(GroupNameOld ,GroupName) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, GroupName);
	    if(newIndex != PROFFAIL)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_GROUP_EXIST_IN_LIST);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
#if 0
	if(checkConfigOver(webGroupType,addr_fromip.s_addr,addr_endip.s_addr,group_day,group_timestart,group_timestop,GroupNameOld) == 1)
	{
	    setTheErrorMsg(T("������IP��ַ�κ���Чʱ������Ѿ�����!"));
	    goto out;
	}
#endif
	profAction = PROF_CHANGE_EDIT;
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    profGroupBase = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if(profAction == PROF_CHANGE_ADD)
    {
	profGroupBase->groupType = webGroupType;/*��¼���ͷ��㲻ͬ��ҳ��ʹ��*/
    }
    /*�����жϷ�ֹżȻ������ֵ� ҳ���޸Ĳ���ʱ ҳ��������������Ͳ�ƥ��*/
    ProfSetNameByPoint(profGroupBase, GroupName);
    profGroupBase->ipStart = addr_fromip.s_addr; 
    profGroupBase->ipEnd = addr_endip.s_addr; 
    profGroupBase->day= days2Byte(group_day); 
    strcpy(profGroupBase->timeStart, group_timestart); 
    strcpy(profGroupBase->timeStop, group_timestop); 
    if(webGroupType == GBT_FINE_LIMIT)
    {/*��ϸ������*/
	profGroupBase->limitPolicy = strtoul(policyStr, NULL, 0);/*���ٲ���*/
	profGroupBase->uRate = strtoul(uRateStr, NULL, 0);/*�ϴ�����*/
	profGroupBase->dRate = strtoul(dRateStr, NULL, 0);/*��������*/
    }
    else/*������Ϊ����*/
    {
	strcpy(profGroupBase->pdbPolicy, PdbPolicy); 
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, getRedWebByGbt(webGroupType));/*ҳ����ת*/
}
#endif


#ifdef CONFIG_UTT_NETFILTER
/* �ж�pdb bitλ�Ƿ����� */
static Boolean pdb_policy_match(Uint8 *policy, int index)
{
    if (policy[index >> 3] & (1 << (index & 7))) {
	return TRUE;
    }
    return FALSE;
}

/* ����pdb bitλ */
static void pdb_policy_set(Uint8 *policy, int index)
{
    policy[index >> 3] |= (1 << (index & 7));
}

/*
 * ��str�ָ���ö�Ӧbitλ��pdbPolicy
 */
static int SetPdbPolicy(Uint8 *pdbPolicy, char_t *str)
{
    char_t *tmp = NULL;
    int index = 0;
    memset(pdbPolicy, 0, UTT_MAX_PDB_POLICY);
    tmp = strtok(str, UTT_SPLIT_SIGN_RECORDS);
    while(tmp != NULL)
    {
	index = strtol(tmp, NULL, 10);
	if ((index != 0) && (index < UTT_MAX_PDB_POLICY_CNT)) {
	    pdb_policy_set(pdbPolicy, index);
	}

	tmp=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
    return 0;
}
#endif

/*
 *������Ϊ�����б�ɾ������
 *
 */
static void formGroupConfigDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *GroupName=NULL, *tmp = NULL;
    const char *errMsg=NULL;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    struProfAlloc *profList  = NULL;
    GroupBaseType webGroupType;
    a_assert(wp);
#if (UTT_SMART_UI == FYES)
    webGroupType = GBT_FINE_LIMIT;/*�·��UIֻ�о�ϸ������*/
#else
    webGroupType = getWebGroupType(wp);/*��ȡ��ǰ������������*/
#endif

    GroupName = websGetVar(wp,T("delstr"),NULL); 
    if(GroupName == NULL) {   
	errMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
	setTheErrorMsg(errMsg);
	goto out;
    }

    tmp=strtok(GroupName, UTT_SPLIT_SIGN_STR);

    uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
    while(tmp != NULL) {

	ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	ProfDelInstByName(mibType, tmp); 
	tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
    }
    uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips(); 
    websRedirect(wp, "Group_Sui.asp");/*ҳ����ת*/
#else
    websRedirect(wp, getRedWebByGbt(webGroupType));/*ҳ����ת*/
#endif
}
static void formGroupConfigDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile* prof;
    struProfAlloc *profList  = NULL;
    GroupBaseType webGroupType;
    a_assert(wp);

#if (UTT_SMART_UI == FYES)
    webGroupType = GBT_FINE_LIMIT;/*�·��UIֻ�о�ϸ������*/
#else
    webGroupType = getWebGroupType(wp);/*��ȡ��ǰ������������*/
#endif
    ProfInstLowHigh(mibType, &max, &min);

    uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(prof->groupType == webGroupType)/*ֻɾ����Ӧ����*/
	    {
		if(back == 0) {
		    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		    back = 1;
		}
		ProfDelInstByIndex(mibType, i);
	    }
	}
    }
    uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/

    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "Group_Sui.asp");/*ҳ����ת*/
#else
    websRedirect(wp, getRedWebByGbt(webGroupType));/*ҳ����ת*/
#endif

}
/********************************************************************
 * ������ formGroupConfigMoveConfig
 * ���ܣ� ҳ���ύ����,��ϸ������ҳ����ƶ�����
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ�  
 * ������ 2012-05-21
 *******************************************************************/
static void formGroupConfigMoveConfig(webs_t wp, char_t *path, char_t *query)
{
    char *sID,*locationID;
    MibProfileType proftype;

    GroupBaseType webGroupType;
    /*��ȡ��ǰ������������*/
    webGroupType = getWebGroupType(wp);
    /*get source inst and des inst*/
    sID = websGetVar(wp, T("sID"), NULL);
    /* ��ʲôλ��֮ǰ*/
    locationID = websGetVar(wp, T("tID"),NULL);                          
    proftype = MIB_PROF_GROUP_BASE;

    if(strcmp(sID,locationID) != 0)
    {
	if(groupbase_instmove(sID, locationID, proftype)) {
	    nvramWriteCommit();
	}
    }
    /*ҳ����ת*/
    websRedirect(wp, getRedWebByGbt(webGroupType));
    return;

}

static bool groupbase_instmove(char *sID, char * tID, MibProfileType proftype)
{
    int  instindex = 0,index = 0,low = 0,high = 0,i = 0;
    struProfAlloc *proflist  = NULL;
    struProfAlloc *proflist2  = NULL;

    index= ProfGetInstIndexByName(proftype,tID);
    instindex = ProfGetInstIndexByName(proftype,sID);
    ProfInstLowHigh(proftype, &high, &low);


    BEHAVIOR_PRINTF("%s-----%d,tID = %s, sID = %s\n", __func__, __LINE__, tID, sID);
    uttSemP(SEM_UTTNF_NO,0);/*������ֹ��ͻ*/
    BEHAVIOR_PRINTF("%s-----%d, index = %d, instindex = %d\n", __func__, __LINE__, index, instindex);
    /*��Ҫ����λ��*/
    if(index != (instindex+1)){ 
	/*  ��ͷ��ʼ����  */
	for(i=low;i<high;i++){                                     
	    /* �����ҵ�Ŀ��λ�ã� �ͽ�Դʵ�����ݲ�    �뵽Ŀ��ʵ��λ��   ���Ҳ��Ҫ���� Ŀ��ʵ��������������ɵ����������� */  
	    if(i==index){   
		ProfBackupByIndex(proftype,PROF_CHANGE_SPECIAL,instindex, &proflist);  
	    }
	    if(i==instindex) continue;			    /*Դʵ��λ�� ��indexλ���Ѿ����룬���ڲ���Ҫ������*/
	    ProfBackupByIndex(proftype,100,i, &proflist);   /*100�����ݣ����ǲ�����profilechange*/
	}
    }
    else
    {
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
	return FALSE;
    }
    index=0;			/*�ָ�λ�ô�ͷ��ʼ*/
    proflist2 = proflist;
    while(proflist!= NULL) {	/*�ָ�  �ӿ�ʼλ�ûָ���*/
	_ProfRestore(proflist->proftype, index++, proflist->oldProf);
	proflist = proflist->next;
    }

    uttSemV(SEM_UTTNF_NO,0);/*�ͷ���*/
    ProfUpdate(proflist2);

    ProfFreeAllocList(proflist);
    ProfFreeAllocList(proflist2);

    return TRUE;
}

/********************************************************************
 * ������ websDefineWebAuth
 * ���ܣ� �Ѿ�ϸ�������������ƶ�����ǰ
 * ������ 2012-08-08
 * ������ name �����������
 * ���أ� 1 - �������ƶ�
 *	  0 - δ�������ƶ�
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
extern int moveLastToFirst(char* name)
{
    int i = 0, min = 0, max = 0;
    int rst = 0;
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile* prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) 
	{/*�ҳ�������С����,���������ƶ�����֮ǰ*/
	    if(strcmp(prof->head.name, name) != 0)/*ȷ����������С(������ӵ�һ��)*/
	    {
		groupbase_instmove(name, prof->head.name, mibType); 
		BEHAVIOR_PRINTF("%s-----%d, prof->head.name = %s\n", __func__, __LINE__, prof->head.name);
		rst = 1;
	    } else {
		rst = 0;
	    }
	    break;
	}
    }
    return rst;
}
void formDefineGroupConfig(void)
{
    websFormDefine(T("formGroupConfigDelAll"), formGroupConfigDelAll);
    websFormDefine(T("formGroupConfigDel"), formGroupConfigDel);
    websFormDefine(T("formGroupConfig"),formConfigGroupConfig );
    websFormDefine(T("formGroupAllow"),formGroupActionAllow);
    websFormDefine(T("formGroupConfigMoveConfig"),formGroupConfigMoveConfig);
    websAspDefine(T("aspOutOneGroupBaseConfig"), getOneGroupBaseConfig);
    websAspDefine(T("aspOutGroupConfigList"), getGroupConfigList);
    websAspDefine(T("coefficient"), coefficient);

#if (UTT_SMART_UI == FYES)
    websFormDefine(T("formGroupConfig_Sui"), formConfigGroupConfig_Sui);
    websAspDefine(T("aspOutGroupConfigList_Sui"), getGroupConfigList_Sui);
#endif
#ifdef CONFIG_UTT_NETFILTER
    websFormDefine(T("formUttNfGlobalConfig"), formUttNfGlobalConfig);
#endif
}
#endif
