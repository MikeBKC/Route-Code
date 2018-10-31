#include    <stdio.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>

#include    "webs.h"
#include    "utils.h"
#include    "uttfunction.h"

#include    <mib.h>
#include    <profacce.h>
#if (PVLAN_REDUCED == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif


static void formPortVlan(webs_t wp, char_t *path, char_t *query);
static int getPortVlan(int eid, webs_t wp, int argc, char **argv);
static void formDelPortVlan(webs_t wp, char_t *path, char_t *query);
/********************************************************************
 * 函数： formDefineFilter
 * 功能： 页面与交互转换函数
 * 创建： 2012-09-11
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： Jianqing.Sun
 *******************************************************************/
extern void formDefinePortVlan(void)
{
    websAspDefine(T("aspOutPutPortVlan"), getPortVlan);
    websFormDefine(T("formPortVlan"), formPortVlan);
    websFormDefine(T("formdelPortVlan"), formDelPortVlan);
}

/*******************************************************************
 * 函数： formDelPortVlan
 * 功能： 页面提交函数
 * 创建： 2012-09-11
 * 参数： 
 * 返回： 无
 * 输出： 略
 * 作者： Jianqing.Sun
 ******************************************************************/
static void formDelPortVlan(webs_t wp, char_t *path, char_t *query)
{
    char* str_value, *tmp, *ptr, pvlan_name[UTT_INST_NAME_LEN+1];
    int changed=0;
    struProfAlloc *profList  = NULL;
    MibProfileType mibType = MIB_PROF_RDD_PORT_VLAN;

    /*组号*/
    str_value = websGetVar(wp, "delstr", T(""));
    SWORD_PRINTF("%s------%d, str_value = %s\n", __func__, __LINE__, str_value);
    tmp = strtok_r(str_value, UTT_SPLIT_SIGN_STR, &ptr);/*可能删除多条映射*/
    while(tmp!=NULL)
    {
	changed = 1;
	sprintf(pvlan_name, "pvlan%s", tmp);
	SWORD_PRINTF("%s------%d, pvlan_name = %s\n", __func__, __LINE__, pvlan_name);
	ProfBackupByName(mibType, PROF_CHANGE_DEL, pvlan_name, &profList);
	ProfDelInstByName(mibType, pvlan_name);
	tmp = strtok_r((char*)NULL, UTT_SPLIT_SIGN_STR, &ptr);
    }
    if(changed==1)
    {
	ProfUpdate(profList);
	ProfFreeAllocList(profList);   
	nvramWriteCommit();
    }
    websRedirect(wp, "port_vlan_rdd.asp");

    return;
}
/*********************************************************************
 * 函数： int_to_bit_array
 * 功能： 按位组装member
 * 创建： 2012-09-11
 * 参数： len 长度  value 数值 
 * 返回:  char数组member
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
extern int int_to_bit_array(unsigned char member[], Uint32 len, Uint32 value)
{
    Uint32 i;
    int count = 0;

    for (i = 0u; i < len; i++)
    {
	if ((value & (0x00000001u << i)) > 0u)
	{
	    count++;
	    member[i + 1] = 1u;/*端口号从1开始而不是从0开始*/
	}
	else
	{
	    member[i + 1] = 0u;/*端口号从1开始而不是从0开始*/
	}
    }

    return count;
}
/*********************************************************************
 * 函数： formVlan
 * 功能： 页面提交函数
 * 创建： 2012-09-11
 * 参数： 页面基本参数 
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
static void formPortVlan(webs_t wp, char_t *path, char_t *query)
{
    int instIndex, web_value, min, max;
    unsigned int pm;
    char* str_value, pvlan_name[UTT_INST_NAME_LEN+1];
    RddPortVlanProfile *prof;
    ProfChangeType profAction;
    struProfAlloc *profhead= NULL;
    MibProfileType mibType = MIB_PROF_RDD_PORT_VLAN;
    SystemProfile	*profSys = NULL;

    str_value = websGetVar(wp, "vlanNo", T(""));
    /*组号*/
    web_value = strtoul(str_value, NULL, 10);

    SWORD_PRINTF("%s------%d, str_value = %s, web_value = %d\n", __func__, __LINE__, str_value, web_value);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    ProfInstLowHigh(mibType, &max, &min);
    instIndex = web_value - 1; 
    if((instIndex < min) || (instIndex >= max)) /*id错误*/
    {
	setTheErrorMsg("vlan id error!");
	websRedirect(wp, "port_vlan_rdd.asp");
    }
    else
    {

	/*backup*/
	profAction = PROF_CHANGE_ADD;
	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

	/*各配置的存储位置固定，则可以用vlan id来instIndex*/
	prof = (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	prof->head.active = TRUE;
	sprintf(pvlan_name, "pvlan%s", str_value);
	/*pvlan做name*/
	ProfSetNameByPoint(prof, pvlan_name);

	/*成员*/
	str_value = websGetVar(wp, "chkstr", T(""));
	pm = (unsigned int)strtoul(str_value, NULL, 10);
	int_to_bit_array(prof->members, profSys->lanPortCount, pm);/*端口号从1开始而不是从0开始*/

	/*名称*/
	str_value = websGetVar(wp, "vlanName", T(""));
	web_value = sizeof(prof->description);
	memset(prof->description, 0, web_value);
	strncpy(prof->description, str_value, web_value-1);
	SWORD_PRINTF("%s------%d, prof->description = %s\n", __func__, __LINE__, prof->description);

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);   
	nvramWriteCommit();
	websRedirect(wp, "port_vlan_rdd.asp");
    }
    return;
}
/*********************************************************************
 * 函数： getPortVlan
 * 功能： 页面显示函数
 * 创建： 2012-09-11
 * 参数： 页面基本参数 
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
static int getPortVlan(int eid, webs_t wp, int argc, char **argv)
{
    unsigned int vlan_count, num;
    int i = 0, min = 0, max = 0;
    char vlanGrpNos[128], vlanGrpNames[480], web_buf[72];
    MibProfileType mibType = MIB_PROF_RDD_PORT_VLAN;
    RddPortVlanProfile *prof = NULL;
    SystemProfile	*profSys = NULL;

    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    /*端口数*/
    num = profSys->lanPortCount;
    websWrite(wp, "var numPorts = %d;\n", num);
    websWrite(wp, "var vlanLists = new Array(");

    vlan_count = 0u;
    vlanGrpNos[0] = '\0';
    vlanGrpNames[0] = '\0';
    web_buf[0] = '\0';

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++)
    {
        prof = (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
	{
	    num = strlen(vlanGrpNos);
	    sprintf(&vlanGrpNos[num], "\"%d\",", (i+1));/*pvlan 组号*/

	    num = strlen(vlanGrpNames);
	    sprintf(&vlanGrpNames[num], "\"%s\",",  prof->description);/*pvlan名称*/

            web_print_port_list(web_buf, prof->members, profSys->lanPortCount);
	    if(vlan_count>0u)
	    {
                websWrite(wp, ",");/*vlan成员*/
	    }
            websWrite(wp, "%s", web_buf);/*vlan成员*/
            vlan_count = vlan_count+1u;/*计数*/
	}
    }

    /*不为空*/
    if(vlan_count > 0u)
    {
	num = strlen(vlanGrpNos)-1u;
	vlanGrpNos[num] = '\0';
	num = strlen(vlanGrpNames)-1u;
	vlanGrpNames[num] = '\0';
    }

    websWrite(wp, ");\n");/*vlan成员*/
    websWrite(wp, "var vlanGrpNos = new Array(%s);\n", vlanGrpNos);/*vlan成员*/
    websWrite(wp, "var vlanGrpNames = new Array(%s);\n", vlanGrpNames);/*vlan成员*/


    getTheErrorMsg(eid, wp, argc, argv);
    websWrite(wp,"var totalrecs=%d;",vlan_count);
    websWrite(wp,"var max_totalrecs =%d;",profSys->lanPortCount);

    return 0;
}
#endif
