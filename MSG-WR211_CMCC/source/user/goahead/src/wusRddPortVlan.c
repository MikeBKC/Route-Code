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
 * ������ formDefineFilter
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2012-09-11
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� Jianqing.Sun
 *******************************************************************/
extern void formDefinePortVlan(void)
{
    websAspDefine(T("aspOutPutPortVlan"), getPortVlan);
    websFormDefine(T("formPortVlan"), formPortVlan);
    websFormDefine(T("formdelPortVlan"), formDelPortVlan);
}

/*******************************************************************
 * ������ formDelPortVlan
 * ���ܣ� ҳ���ύ����
 * ������ 2012-09-11
 * ������ 
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
 ******************************************************************/
static void formDelPortVlan(webs_t wp, char_t *path, char_t *query)
{
    char* str_value, *tmp, *ptr, pvlan_name[UTT_INST_NAME_LEN+1];
    int changed=0;
    struProfAlloc *profList  = NULL;
    MibProfileType mibType = MIB_PROF_RDD_PORT_VLAN;

    /*���*/
    str_value = websGetVar(wp, "delstr", T(""));
    SWORD_PRINTF("%s------%d, str_value = %s\n", __func__, __LINE__, str_value);
    tmp = strtok_r(str_value, UTT_SPLIT_SIGN_STR, &ptr);/*����ɾ������ӳ��*/
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
 * ������ int_to_bit_array
 * ���ܣ� ��λ��װmember
 * ������ 2012-09-11
 * ������ len ����  value ��ֵ 
 * ����:  char����member
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
	    member[i + 1] = 1u;/*�˿ںŴ�1��ʼ�����Ǵ�0��ʼ*/
	}
	else
	{
	    member[i + 1] = 0u;/*�˿ںŴ�1��ʼ�����Ǵ�0��ʼ*/
	}
    }

    return count;
}
/*********************************************************************
 * ������ formVlan
 * ���ܣ� ҳ���ύ����
 * ������ 2012-09-11
 * ������ ҳ��������� 
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
    /*���*/
    web_value = strtoul(str_value, NULL, 10);

    SWORD_PRINTF("%s------%d, str_value = %s, web_value = %d\n", __func__, __LINE__, str_value, web_value);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    ProfInstLowHigh(mibType, &max, &min);
    instIndex = web_value - 1; 
    if((instIndex < min) || (instIndex >= max)) /*id����*/
    {
	setTheErrorMsg("vlan id error!");
	websRedirect(wp, "port_vlan_rdd.asp");
    }
    else
    {

	/*backup*/
	profAction = PROF_CHANGE_ADD;
	ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

	/*�����õĴ洢λ�ù̶����������vlan id��instIndex*/
	prof = (RddPortVlanProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	prof->head.active = TRUE;
	sprintf(pvlan_name, "pvlan%s", str_value);
	/*pvlan��name*/
	ProfSetNameByPoint(prof, pvlan_name);

	/*��Ա*/
	str_value = websGetVar(wp, "chkstr", T(""));
	pm = (unsigned int)strtoul(str_value, NULL, 10);
	int_to_bit_array(prof->members, profSys->lanPortCount, pm);/*�˿ںŴ�1��ʼ�����Ǵ�0��ʼ*/

	/*����*/
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
 * ������ getPortVlan
 * ���ܣ� ҳ����ʾ����
 * ������ 2012-09-11
 * ������ ҳ��������� 
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
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
    /*�˿���*/
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
	    sprintf(&vlanGrpNos[num], "\"%d\",", (i+1));/*pvlan ���*/

	    num = strlen(vlanGrpNames);
	    sprintf(&vlanGrpNames[num], "\"%s\",",  prof->description);/*pvlan����*/

            web_print_port_list(web_buf, prof->members, profSys->lanPortCount);
	    if(vlan_count>0u)
	    {
                websWrite(wp, ",");/*vlan��Ա*/
	    }
            websWrite(wp, "%s", web_buf);/*vlan��Ա*/
            vlan_count = vlan_count+1u;/*����*/
	}
    }

    /*��Ϊ��*/
    if(vlan_count > 0u)
    {
	num = strlen(vlanGrpNos)-1u;
	vlanGrpNos[num] = '\0';
	num = strlen(vlanGrpNames)-1u;
	vlanGrpNames[num] = '\0';
    }

    websWrite(wp, ");\n");/*vlan��Ա*/
    websWrite(wp, "var vlanGrpNos = new Array(%s);\n", vlanGrpNos);/*vlan��Ա*/
    websWrite(wp, "var vlanGrpNames = new Array(%s);\n", vlanGrpNames);/*vlan��Ա*/


    getTheErrorMsg(eid, wp, argc, argv);
    websWrite(wp,"var totalrecs=%d;",vlan_count);
    websWrite(wp,"var max_totalrecs =%d;",profSys->lanPortCount);

    return 0;
}
#endif
