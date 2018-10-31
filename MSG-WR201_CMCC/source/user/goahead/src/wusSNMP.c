#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "um.h"
#include "webs.h"
#include "utils.h"
#include "webmsg.h"
#include "uttfunction.h"
#include "profacce.h"
#include "uttMachine.h"
#include "comExeFun.h"
#include "translate.h"
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#if (SNMP == FYES)
static int aspOutPutGetSNMP(int eid, webs_t wp, int argc, char **argv);
static void formSNMP(webs_t wp, char_t *path, char_t *query);

extern void websDefineSNMP(void)
{
    websFormDefine(T("SNMPConfig"), formSNMP);
    websAspDefine(T("aspOutPutGetSNMP"), aspOutPutGetSNMP);
}

static int aspOutPutGetSNMP(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_SNMP_INFO;
    SnmpInfoProfile *prof = NULL;
    prof=(SnmpInfoProfile *)ProfGetInstPointByIndex(mibType,0);

    websWrite(wp, "var snmp_enable =\"%d\";\n", prof->snmp_enable);
    websWrite(wp, "var community = \"%s\";\n", prof->community);
    websWrite(wp, "var dev_name = \"%s\";\n", prof->dev_name);
    websWrite(wp, "var contactor = \"%s\";\n", prof->contactor);
    websWrite(wp, "var position = \"%s\";\n", prof->position);
    websWrite(wp, "var host_manage_enable = \"%d\";\n", prof->host_manage_enable);

    websWrite(wp, "var host1 = \"%s\";\n", prof->host1);
    websWrite(wp, "var host2 = \"%s\";\n", prof->host2);
    websWrite(wp, "var host3 = \"%s\";\n", prof->host3);
    SWORD_PRINTF("%s-%d, snmp_enable = %d, community = %s, dev_name = %s, contactor = %s, position = %s, host_manage_enable = %d,host1 = %s,host2 = %s,host3 = %s\n", __func__, __LINE__, prof->snmp_enable,  prof->community, prof->dev_name, prof->contactor,prof->position,prof->host_manage_enable,prof->host1,prof->host2,prof->host3);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
static void formSNMP(webs_t wp, char_t *path, char_t *query)
{
    char *snmpEnable = NULL;
    char *community = NULL;
    char *dev_name = NULL;
    char *contactor = NULL;
    char *position = NULL;
    char *host_manage_enable = NULL;
    char *host1 = NULL;
    char *host2 = NULL;
    char *host3 = NULL;
    MibProfileType mibType = MIB_PROF_SNMP_INFO;
    SnmpInfoProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
#if (STATIC_NAT == FYES)
    InterfaceProfile *profIf = NULL;
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
    ProfChangeType profAction;
    int instIndex;
    Uint32 lanIp = htonl(0xc0a80101);
    const char *ErrMsg = NULL;
#endif
    /*取配置*/ 
    snmpEnable = websGetVar(wp, T("SNMPEnable"), T(""));
    community = websGetVar(wp, T("RoComm"), T(""));
    dev_name = websGetVar(wp, T("RoHost"), T(""));
    contactor = websGetVar(wp, T("RoContact"), T(""));
    position = websGetVar(wp, T("RoLocation"), T(""));
    host_manage_enable = websGetVar(wp, T("RsHost"), T(""));
    host1 = websGetVar(wp, T("server1"), T(""));
    host2 = websGetVar(wp, T("server2"), T(""));
    host3 = websGetVar(wp, T("server3"), T(""));
    SWORD_PRINTF("%s-%d,snmpEnable = %s\n", __func__, __LINE__,snmpEnable);
    /*配置错误*/
    if ((snmpEnable == NULL))
    {
	setTheErrorMsg(T("配置错误"));
	websRedirect(wp, "Snmp.asp");
	return;
    }
#if (STATIC_NAT == FYES)

    instIndex = ProfGetInstIndexByName(mibTypeNat, SNMP_NAT_NAME);
    if (strcmp(snmpEnable, "on") == 0) {
	/* 不存在则新建 */
	if (instIndex == PROFFAIL) {
	    instIndex = ProfNewInst(mibTypeNat, SNMP_NAT_NAME, FALSE);
	    if (instIndex == PROFFAIL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_STATIC_ERR);
			setTheErrorMsg(ErrMsg);
	    } else {
		/*获取LAN口IP*/
		profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
		if(profIf!=NULL) {
		lanIp = profIf->ether.sta.ip;
		}

	    profAction = PROF_CHANGE_ADD;
	    ProfBackupByIndex(mibTypeNat, profAction, instIndex, &profhead);
	    profNat = (StaticNatProfile *)ProfGetInstPointByIndex(mibTypeNat, instIndex);
	    ProfSetNameByPoint(profNat, SNMP_NAT_NAME);
	    profNat->head.active = 1;
	    profNat->protocol = STATIC_NAT_UDP;
	    profNat->OutPort = SNMP_PORT1;
	    profNat->IP = lanIp;
	    profNat->InnerPort = SNMP_PORT1;
	    profNat->PortNum = 1;
	    strcpy(profNat->NatBind, "WAN1");
	    SWORD_PRINTF("%s: name=%s,active=%d,pro=%d,outport=%d,ip=0x%x,innerport=%d,portnum=%d,bind=%s\n", 
		    __FUNCTION__, profNat->head.name, profNat->head.active, profNat->protocol, profNat->OutPort, 
		    profNat->IP, profNat->InnerPort, profNat->PortNum, profNat->NatBind);
	    }
	}
    } else {
	/* 存在则删除 */
	if (instIndex != PROFFAIL) {
	    profAction = PROF_CHANGE_DEL;
	    ProfBackupByIndex(mibTypeNat, profAction, instIndex, &profhead);
	    ProfDelInstByIndex(mibTypeNat, instIndex);
	}
    }
#endif
    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    prof = (SnmpInfoProfile *)ProfGetInstPointByIndex(mibType,0);
    
    if(prof != NULL) 
    {
	/*开启snmp*/
	if (strcmp(snmpEnable, "on") == 0)
	 {
	    prof->snmp_enable = 1u;
	}
	else
	{
		/*关闭snmp*/
	    prof->snmp_enable = 0u;
	}
	if (strcmp(host_manage_enable, "on") == 0)
	 {
	    prof->host_manage_enable = 1u;
	}
	else
	{
		/*关闭snmp*/
	    prof->host_manage_enable = 0u;
	}

	strncpy(prof->community, community, MAX_SNMP_COMMUNITY_LEN);
	strncpy(prof->dev_name, dev_name, MAX_SNMP_COMMUNITY_LEN);
	strncpy(prof->contactor, contactor, MAX_SNMP_COMMUNITY_LEN);
	strncpy(prof->position, position, MAX_SNMP_COMMUNITY_LEN);
	strncpy(prof->host1, host1, 16);
	strncpy(prof->host2, host2, 16);
	strncpy(prof->host3, host3, 16);
	prof->head.active = TRUE;
   }

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    SWORD_PRINTF("%s-%d,snmpEnable = %s,snmp_enable = %d, community = %s, dev_name = %s, contactor = %s, position = %s, host_manage_enable = %d,host1 = %s,host2 = %s,host3 = %s\n", __func__, __LINE__,snmpEnable, prof->snmp_enable,  prof->community, prof->dev_name, prof->contactor,prof->position,prof->host_manage_enable,prof->host1,prof->host2,prof->host3);
        websRedirect(wp, "Snmp.asp");

    return; 
}
#endif
