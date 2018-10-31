#include    <stdio.h>
#include    <string.h>
#include	<mib.h>
#include	"typedef.h"
#include    "um.h"
#include    "webs.h"
#include    "utils.h"
#include    "webmsg.h"
#include	"uttfunction.h"
#include	"profacce.h"
#include	"uttMachine.h"
#include	"comExeFun.h"
#include	"translate.h"
#include	"mibSnmp.h"
#if (FEATURE_SW_SNMP == FYES)
static void formSysSnmpConf(webs_t wp, char_t *path, char_t *query);
static int aspOutPutGetSysSnmpConf(int eid, webs_t wp, int argc, char **argv);

/******************************************************************
 * ������formDefineSysSnmp
 * ���ܣ�ҳ���뽻��ת������
 * ��������
 * ���أ���
 * �������
 * ���ߣ�jfgu
 * ���ڣ�2011-1-28
 ********************************************************************/
extern void websDefineSnmp(void)
{
    websFormDefine(T("formSysSnmp"), formSysSnmpConf);
    websAspDefine(T("aspOutPutGetSysSnmpConf"), aspOutPutGetSysSnmpConf);
}

/********************************************************************
 * ������aspOutPutGetSysSnmpConf
 * ���ܣ���ʾ����
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-1-28
 *******************************************************************/
static int aspOutPutGetSysSnmpConf(int eid, webs_t wp, int argc, char **argv)
{
#if 0
    printf("enable %d, snmpTrap %s, readCommunity %s, writeCommunity %s, trapCommunity %s\n",
	    cfg_read_snmp_enable(),
	    cfg_read_snmp_trap(),
	    cfg_read_snmp_read_community(),
	    cfg_read_snmp_write_community(),
	    cfg_read_snmp_trap_community());
#endif
	MibProfileType mibType = MIB_PROF_SNMP;
	SnmpProfile *prof = NULL;
	prof=(SnmpProfile *)ProfGetInstPointByIndex(mibType,0);

    websWrite(wp, "var onSnmp = %d;\n", prof->snmp_enable);
    websWrite(wp, "var snmpTrap = \"%s\";\n", prof->trap_ip);
    websWrite(wp, "var readCommunity = \"%s\";\n", prof->r_community);
    websWrite(wp, "var writeCommunity = \"%s\";\n", prof->w_community);
    websWrite(wp, "var trapCommunity = \"%s\";\n", prof->trap_community);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}

/********************************************************************
 * ������formSysSnmpConf
 * ���ܣ�����snmp����
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-1-28
 ********************************************************************/
static void formSysSnmpConf(webs_t wp, char_t *path, char_t *query)
{
    char *snmpEnable = NULL;
    char *trapIp = NULL;
    char *readCom = NULL;
    char *writeCom = NULL;
    char *trapCom = NULL;
	MibProfileType mibType = MIB_PROF_SNMP;
	SnmpProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

    /*ȡ����*/ 
    snmpEnable = websGetVar(wp, T("snmp"), T(""));
    trapIp = websGetVar(wp, T("snmpTrap"), T(""));
    readCom = websGetVar(wp, T("readCommunity"), T(""));
    writeCom = websGetVar(wp, T("writeCommunity"), T(""));
    trapCom = websGetVar(wp, T("trapCommunity"), T(""));
#if 0
    printf("web: snmp enable %s, trapIp %s, read %s, write, %s, trap %s\n",
	    snmpEnable, trapIp, readCom, writeCom, trapCom);
#endif
    /*���ô���*/
    if ((snmpEnable == NULL) || (trapIp == NULL) || (readCom == NULL)
	    || (writeCom == NULL) || (trapCom == NULL))
    {
	setTheErrorMsg(T("Incorrect parameter."));
	websRedirect(wp, "snmpConf.asp");
	return;
    }

	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	prof = (SnmpProfile *)ProfGetInstPointByIndex(mibType,0);
    
	if(prof != NULL) 
	{
	/*����snmp*/
    if (strcmp(snmpEnable, "1") == 0)
    {
		prof->snmp_enable = 1u;
    }
    else
    {
	/*�ر�snmp*/
		prof->snmp_enable = 0u;
    }
	strncpy(prof->trap_ip, trapIp, MAX_SNMP_COMMUNITY_LEN+4);
	strncpy(prof->r_community, readCom, MAX_SNMP_COMMUNITY_LEN);
	strncpy(prof->w_community, writeCom, MAX_SNMP_COMMUNITY_LEN);
	strncpy(prof->trap_community, trapCom, MAX_SNMP_COMMUNITY_LEN);

		prof->head.active = TRUE;
   }


	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

    websRedirect(wp, "snmpConf.asp");

    return; 
}
#endif
