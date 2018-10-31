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
#if (FEATURE_PORT_VLAN == FYES)
#include    <switch.h>
#include    <sw_ioctl.h>
#include    <linux/switch/swioc_common.h>
#include    "translate.h"


static void formPortVlan(webs_t wp, char_t *path, char_t *query);
static int getPortVlan(int eid, webs_t wp, int argc, char **argv);
static void formDelPortVlan(webs_t wp, char_t *path, char_t *query);
/********************************************************************
 * ������ formDefineFilter
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
 *******************************************************************/
extern void formDefinePortVlan(void)
{
    websAspDefine(T("aspOutPutPortVlan"), getPortVlan);
    websFormDefine(T("formPortVlan"), formPortVlan);
    websFormDefine(T("formdelPortVlan"), formDelPortVlan);
}

/**
 *�����豸��ǰvlan ������ת����Ӧ��ҳ��
 *
 * **/
extern void webRedirectByVlanType(webs_t wp)
{
    unsigned int vlan_flag = 0u;
   const char *ErrMsg =NULL; 
    vlan_flag = sw_vlan_type_get();
    switch(vlan_flag)/**�ж�����*/
    {
	case VLAN_TYPE_8021Q:/*tag vlan*/
	    websRedirect(wp, "switch/tag_vlan.asp");
	    break;
	case VLAN_TYPE_PORT:/*port vlan*/
	    websRedirect(wp, "switch/port_vlan.asp");
	    break;
	case VLAN_TYPE_NONE:/*������vlan*/
	    websRedirect(wp, "switch/vlan.asp");
	    break;
	default:/*err*/
	 ErrMsg = getTransDataForC(C_LANG_INDEX_PORT_VLAN_TYPE_ERR);  
	      setTheErrorMsg((char *)ErrMsg);
	   // setTheErrorMsg(T("vlan ���ʹ���!"));
	    websRedirect(wp, "switch/vlan.asp");
	    break;
    }
    return;
}
/*******************************************************************
 *������ formDelPortVlan
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� KeHuatao
 *������ 2010-11-22
 ******************************************************************/
static void formDelPortVlan(webs_t wp, char_t *path, char_t *query)
{
    char* str_value, *tmp, *ptr, pvlan_name[UTT_INST_NAME_LEN+1];
    int changed=0;
    struProfAlloc *profList  = NULL;
    MibProfileType mibType = MIB_PROF_PORT_VLAN;
	const char *ErrMsg =NULL;
    if(sw_vlan_type_get() == VLAN_TYPE_PORT)
    {
	/*���*/
	str_value = websGetVar(wp, "delstr", T(""));
	tmp = strtok_r(str_value, UTT_SPLIT_SIGN_STR, &ptr);/*����ɾ������ӳ��*/
	while(tmp!=NULL)
	{
	    changed = 1;
	    sprintf(pvlan_name, "pvlan%s", tmp);
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
    }
    else
    {
	 ErrMsg = getTransDataForC(C_LANG_INDEX_PORT_VLAN_FAIL_NOT_VLAN);  
	      setTheErrorMsg((char *)ErrMsg);
//	setTheErrorMsg(T("����ʧ�ܣ���ǰ�豸vlan���ͣ��Ƕ˿�vlan!"));
    }
    webRedirectByVlanType(wp);

    return;
}
/*******************************************************************
 *������ formVlan
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� KeHuatao
 *������ 2010-11-22
 ******************************************************************/
static void formPortVlan(webs_t wp, char_t *path, char_t *query)
{
    int instIndex, web_value, min, max;
    unsigned int pm;
    char* str_value, pvlan_name[UTT_INST_NAME_LEN+1];
	const char *ErrMsg =NULL;
    PortVlanProfile *prof;
    ProfChangeType profAction;
    struProfAlloc *profhead= NULL;
    MibProfileType mibType = MIB_PROF_PORT_VLAN;

    if(sw_vlan_type_get() == VLAN_TYPE_PORT)
    {
	str_value = websGetVar(wp, "vlanNo", T(""));
	/*���*/
	web_value = strtoul(str_value, NULL, 10);

        ProfInstLowHigh(mibType, &max, &min);
	instIndex = web_value - 1; 
	if((instIndex < min) || (instIndex >= max)) /*id����*/
	{
	    setTheErrorMsg("vlan id error!");
	    websRedirect(wp, "port_vlan.asp");
	    return;
	}

	/*backup*/
        profAction = PROF_CHANGE_ADD;
        ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

	/*�����õĴ洢λ�ù̶����������vlan id��instIndex*/
	prof = (PortVlanProfile*)ProfGetInstPointByIndex(mibType, instIndex);
        prof->head.active=TRUE;
	sprintf(pvlan_name, "pvlan%s", str_value);
	/*pvlan��name*/
	ProfSetNameByPoint(prof, pvlan_name);

	/*��Ա*/
	str_value = websGetVar(wp, "chkstr", T(""));
	pm = (unsigned int)strtoul(str_value, NULL, 10);
	int_to_bit_array(prof->members+START_INDEX, PORT_COUNT, pm);

	/*����*/
	str_value = websGetVar(wp, "vlanName", T(""));
	web_value = sizeof(prof->description);
	memset(prof->description, 0, web_value);
	strncpy(prof->description, str_value, web_value-1);

        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);   
        nvramWriteCommit();
    }
    else
    {
	 ErrMsg = getTransDataForC(C_LANG_INDEX_PORT_VLAN_FAIL_NOT_VLAN);  
	      setTheErrorMsg((char *)ErrMsg);

	//setTheErrorMsg(T("����ʧ�ܣ���ǰ�豸vlan���ͣ��Ƕ˿�vlan!"));
    }
    webRedirectByVlanType(wp);
    return;
}

/*******************************************************************
 *������ webAspOutPortVlan
 *���ܣ� ��ӡport vlan�����Ϣ��ҳ��
 *������ 
 *���أ� ��
 *���ߣ� bhou
 *������ 2011-01-17
 ******************************************************************/
extern void webAspOutPortVlan(webs_t wp)
{
    int i, min, max, vlan_count=0u;
    char web_buf[72];
    PortVlanProfile *prof;
    MibProfileType mibType = MIB_PROF_PORT_VLAN;

    /*�˿���*/
    websWrite(wp, "var vlanLists = new Array(");
    web_buf[0] = '\0';

    ProfInstLowHigh(MIB_PROF_PORT_VLAN, &max, &min);

    for(i=min;i<max;i++)
    {
        prof = (PortVlanProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
            web_print_port_list(web_buf, prof->members, PORT_ARRAY_SIZE);
	    if(vlan_count>0)
	    {
                websWrite(wp, ",");/*vlan��Ա*/
	    }
            websWrite(wp, "%s", web_buf);/*vlan��Ա*/
            vlan_count++;/*����*/
	}
    }
    websWrite(wp, ");\n");/*vlan��Ա*/

    return;
}
/*******************************************************************
 *������ webAspOutVlan
 *���ܣ� ��ӡvlan�����Ϣ��ҳ��
 *������ 
 *���أ� ��
 *���ߣ� bhou
 *������ 2011-01-17
 ******************************************************************/
extern void webAspOutVlan(webs_t wp)
{
    int num = sw_vlan_type_get();
    /*�˿���*/
    websWrite(wp, "var vlanFlag = %d;\n", num);
    if(num == VLAN_TYPE_PORT)
    {
	webAspOutPortVlan(wp);
    }
#if 0
    if(num == VLAN_TYPE_8021Q)
    {
	webAspOutTagVlan(wp);
    }
#endif
    return;
}
/********************************************************************
 * ������ getVlan
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2010-11-22
 ********************************************************************/ 
static int getPortVlan(int eid, webs_t wp, int argc, char **argv)
{
    unsigned int vlan_count, num;
    int i, min = 0, max = 0;
    char vlanGrpNos[128], vlanGrpNames[480], web_buf[72];
    MibProfileType mibType = MIB_PROF_PORT_VLAN;
    PortVlanProfile *prof;

    i = sw_vlan_type_get();
    websWrite(wp, "var vlanFlag = %d;\n", i);
    /*�˿���*/
    num = PORT_COUNT;
    websWrite(wp, "var numPorts = %d;\n", num);
    websWrite(wp, "var vlanLists = new Array(");

    vlan_count = 0u;
    vlanGrpNos[0] = '\0';
    vlanGrpNames[0] = '\0';
    web_buf[0] = '\0';

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++)
    {
        prof = (PortVlanProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
	{
	    num = strlen(vlanGrpNos);
	    sprintf(&vlanGrpNos[num], "\"%d\",", (i+1u));/*pvlan ���*/

	    num = strlen(vlanGrpNames);
	    sprintf(&vlanGrpNames[num], "\"%s\",",  prof->description);/*pvlan����*/

            web_print_port_list(web_buf, prof->members, PORT_ARRAY_SIZE);
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

    getAggrMember(wp);

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#endif
