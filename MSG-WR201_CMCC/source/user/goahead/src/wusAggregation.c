
#include <stdio.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"

#if (FEATURE_SW_PORT_TRUNK == FYES)
static char *chk_port[PORT_COUNT]={"chkPort0", "chkPort1", "chkPort2", "chkPort3", "chkPort4", "chkPort5", "chkPort6", "chkPort7", "chkPort8", "chkPort9", "chkPort10", "chkPort11", "chkPort12", "chkPort13", "chkPort14", "chkPort15", "chkPort16", "chkPort17", 
	  "chkPort18","chkPort19", "chkPort20", "chkPort21", "chkPort22", "chkPort23"};
#define DELSEPARATE T(",")

#ifdef CONFIG_IP1725
#define  AGGR_WEB_NAME  "switch/aggregation_100M.asp"
#else
#define  AGGR_WEB_NAME  "switch/aggregation.asp"
#endif
void  webOutPutRstpEnable(webs_t wp);
void webOutPutLacpEnable(webs_t wp);
extern void webAspOutVlan(webs_t wp);

static int getAggrGroupList(int eid, webs_t wp, int argc, char **argv);
static void formAggrGroupDel(webs_t wp, char_t *path, char_t *query); 
static void formAggrGroup(webs_t wp, char_t *path, char_t *query);
/********************************************************************
 * 函数： fromDefineAggr
 * 功能： 页面与交互转换函数
 * 创建： 2010-10-08
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： jfgu
 * 维护： jfgu
 *******************************************************************/
extern void formDefineAggr(void)
{
	websAspDefine(T("aspOutPutAggrGroupList"), getAggrGroupList);   
	websFormDefine(T("formAggrGroup"), formAggrGroup);
	websFormDefine(T("formAggrGroupDel"), formAggrGroupDel);
}


/*******************************************************************
 *函数： formIpMacPortDel 
 *功能： 页面提交函数
 *参数： 略
 *返回： 无
 *作者： jfgu
 *日期： 2010-10-08
 ******************************************************************/
static void formAggrGroup(webs_t wp, char_t *path, char_t *query)
{
    char *action = NULL, *member = NULL;
    char *remark = NULL,*group = NULL;
    unsigned char members[PORT_ARRAY_SIZE];
    char userName[UTT_INST_NAME_LEN + 1];
    int group_id, pIndex;
    unsigned int port_no;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_TRUNK;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    TrunkProfile *prof = NULL;

    /* get value from web page */
    action = websGetVar(wp, "actions", T(""));
    remark = websGetVar(wp, "aggName", T(""));
    group = websGetVar(wp, "aggNo", T(""));
    group_id = (int)strtoul(group, NULL, 10);

    memset(members, 0, sizeof(members));
    memset(userName, '\0', sizeof(userName));
    userName[0] = 't';
    
    if(strlen(remark) > UTT_INST_NAME_LEN - 1)
    {
	setTheErrorMsg(T("汇聚组名称不能超过11字节"));
    }
    else
    {
	/* get aggreration members from web page. */
	for (port_no = 0u; port_no < PORT_COUNT; port_no++)
	{
	    member = websGetVar(wp, chk_port[port_no], T(""));
	    /* Join the port in aggr members when checkbox value is "on" */
	    if (strcmp(member, "on") == 0) 
	    {
		members[port_no+START_INDEX] = 1u;
	    }
	}

	/* judge the action and according it to exec different operation */
	if (strncmp(action, "add", 3) == 0)
	{
	    /* Add aggr group members */
	    profAction = PROF_CHANGE_ADD;
	}
	else
	{
	    /* Modify aggr group members */
	    profAction = PROF_CHANGE_EDIT;
	}

	pIndex = group_id - 1; /* profile index */
	strcat(userName, group); /* profile head name */

	/* get pIndex profile */
	prof=(TrunkProfile*)ProfGetInstPointByIndex(mibType, pIndex);
	if(prof != NULL)
	{
	    /* Backup the profile */
	    ProfBackupByIndex(mibType, profAction, pIndex, &profhead);

	    /* save the aggr group information to the profile */
	    ProfSetNameByPoint(prof, userName);
	    prof->head.active = TRUE;
	    prof->tMode = TRUNK_MODE;
	    prof->tGroup = group_id;
	    strcpy(prof->tName, remark);
	    memcpy(prof->tMembers, members, sizeof(members));

	    /* update profile and exec profile change */
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }

    websRedirect(wp, AGGR_WEB_NAME);

    return ;
}

/*******************************************************************
 *函数： formIpMacPortDel 
 *功能： 页面提交函数
 *参数： 略
 *返回： 无
 *作者： jfgu
 *日期： 2010-10-08
 ******************************************************************/
static void formAggrGroupDel(webs_t wp, char_t *path, char_t *query)
{
    char *id, *idnext, *del_str = NULL;
    unsigned short index;
    int i, min, max, change = 0;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_TRUNK;
    ProfChangeType profAction = PROF_CHANGE_DEL;
    TrunkProfile *prof = NULL;

    del_str = websGetVar(wp, T("delstr"), T(""));
    /*get delstr from web*/
    if ((strcmp(del_str, "") == 0) || del_str == NULL)
    {
	setTheErrorMsg(T("找不到删除的组"));
	websRedirect(wp, AGGR_WEB_NAME);
	return;
    }

    /*delstr is "group 1, group 2, ....."*/
    idnext = strtok(del_str, DELSEPARATE);

    /*分割*/
    while(idnext != NULL)
    {
	id = idnext;
	idnext = strtok(NULL, DELSEPARATE);
	index = (unsigned short)strtoul(id, NULL, 10);

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++)
	{
	    prof = (TrunkProfile*)ProfGetInstPointByIndex(mibType, i);

	    if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	    {
		ProfBackupByIndex(mibType, profAction, i, &profhead);

		if(prof->tGroup == index)
		{
		    /*Delect Trunk Profile i */
		    ProfDelInstByIndex(mibType, i);
		    change = 1;
		}
	    }
	}
    }
    if(change == 1)
    {
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();	
    }
    websRedirect(wp, AGGR_WEB_NAME);
    return;    
}

/*
 * Function: getAggrMember(webs_t wp)
 *
 * Descroption: output port aggreration infor to web page.
 *
 * History: 2011.09.16  created by Chen.Shiyu
 */
int getAggrMember(webs_t wp)
{
    int min, max, flag, index, g_No = 0;
    unsigned int port_no;
    MibProfileType mibType = MIB_PROF_TRUNK;
    TrunkProfile *prof = NULL;

    /* websWrite array define */
    websWrite(wp, "var aggGrpNos = new Array();\n");
    websWrite(wp, "var aggLists = new Array();\n");

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	flag = 0;
	/* get trunk configure from profile */
	prof = (TrunkProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	{
	    /* output trunk groups informaton to WEB page. */
	    websWrite(wp, "aggGrpNos[%d] = \"%d\";", g_No, prof->tGroup);
	    /* Each aggr group comprise members. */
	    websWrite(wp, "aggLists[%d] = \"", g_No);

	    /* webswrite this format (1 2 3 4 ....) */		
	    for(port_no = START_INDEX; port_no < PORT_ARRAY_SIZE+START_INDEX-1; port_no++)
	    {
		if(prof->tMembers[port_no] == 1u)
		{
		    if(flag == 0)
		    {
			websWrite(wp, "%d", port_no);
			flag = 1;
		    }
		    else
		    {
			websWrite(wp, " %d", port_no);
		    }
		}
	    }
	    websWrite(wp, "\";\n");
	    g_No++;
	}
    }
    return 0;
}

/********************************************************************
 * 函数： getMacTableList 
 * 功能： 页面显示函数
 * 创建： 2010-08-24
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Chen.Shiyu
 * 日期 2011.09.09 
 ********************************************************************/ 
static int getAggrGroupList(int eid, webs_t wp, int argc, char **argv)
{
    int min, max, flag, index, g_no = 0;
    unsigned int port = 0u, port_no;
    MibProfileType mibType = MIB_PROF_TRUNK;
    TrunkProfile *prof = NULL;
    MibProfileType mibType_m = MIB_PROF_MIRROR;
    MirrorProfile *prof_m = NULL;

#ifdef CONFIG_IP1725	
    websWrite(wp, "numPorts = 4;\n");
#else
#ifdef CONFIG_VSC7390
    websWrite(wp, "numPorts = %d;\n", PORT_COUNT);
#endif
#endif
    /**websWrite array define*/
    websWrite(wp, "var aggGrpNos = new Array();\n");
    websWrite(wp, "var aggLists = new Array();\n");
    websWrite(wp, "var aggGrpNames = new Array();\n");

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	flag = 0;
	/* get trunk configure from profile */
	prof = (TrunkProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0)&& (prof->head.active == TRUE))
	{
	    /* output trunk groups informaton to WEB page. */
	    websWrite(wp, "aggGrpNos[%d] = \"%d\";", g_no, prof->tGroup);
	    /* Each aggr group comprise members. */
	    websWrite(wp, "aggLists[%d] = \"", g_no);

	    /* webswrite this format (1 2 3 4 ....) */		
	    for(port_no = START_INDEX; port_no < PORT_ARRAY_SIZE+START_INDEX-1; port_no++)
	    {
		if(prof->tMembers[port_no] == 1u)
		{
		    if(flag == 0)
		    {
			websWrite(wp, "%d", port_no);
			flag = 1;
		    }
		    else
		    {
			websWrite(wp, " %d", port_no);
		    }
		}
	    }
	    websWrite(wp, "\";");
	    /* Get group name from the Trunk Profile. */
	    websWrite(wp, "aggGrpNames[%d] = \"%s\";", g_no, prof->tName);
	    websWrite(wp, "\n");
	    g_no++;
	}
    }

    /*镜像信息*/
    prof_m=(MirrorProfile*)ProfGetInstPointByIndex(mibType_m, 0);
    if((prof_m != NULL) && (ProfInstIsFree(((Pointer)prof_m)) == 0))
    {
	for(port_no = START_INDEX; port_no < PORT_ARRAY_SIZE+START_INDEX-1; port_no++)
	{
	    if(prof_m->monitoredPort[port_no] == SW_MIRROR_PORT_ENABLE)/*启用*/
	    {
		port = prof_m->mirrorPort;
		break;
	    }
	}
    }
    websWrite(wp, "var mirrorPort = %d;\n", port);

#if 0
    /*lacp enable*/
    webOutPutLacpEnable(wp);
    /*rstp状态*/
    webOutPutRstpEnable(wp);
#else
    websWrite(wp, "var lacp = new Array();"); /*如果添加lacp需添加相关打印*/
    websWrite(wp, "var rstp = new Array();"); /*如果添加rstp需添加相关打印*/
#endif

    /*vlan信息*/
    webAspOutVlan(wp);

    if(strstr(wp->url, "aggregation"))
    {
	getTheErrorMsg(eid, wp, argc, argv);
    }

    return 0;
}

#endif
