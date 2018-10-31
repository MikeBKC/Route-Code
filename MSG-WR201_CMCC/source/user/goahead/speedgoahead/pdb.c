
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "uttfunction.h"
#include "StaticRoute.h"
#include "pdb.h"
#include "groupbase.h"
#if (UTTPDB == FYES)
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
#endif
//#define PDB_DEBUG
#ifdef PDB_DEBUG
#define PDB_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define PDB_PRINTF(fmt, args...)
#endif

#define STATS_FIXED  1 /*�û�״̬����һֱ����*/


extern int doSystem(char_t *fmt, ...);
extern int _getWanLinkState(int wanNo);

extern void createPdbFile(PdbProfile *profPdb);
#if (GAME_BOOST == FYES) || (WIFIDOG_AUTH == FYES) /*wifidog΢�Ų���*/
void createPdbFiles(PdbProfile *profPdb);
extern void createL7File(char* fname,char* content);
#endif
extern void PDBUpdataInit(void);
#ifdef CONFIG_UTT_NETFILTER
/* 
 * update pdb identify rule, but not update dns rule.
 * dns rule only update by init or pdb db update
 */
static void updatePdbIndRule(void);

/*
 * update dns rule by init or pdb db update
 */
static void updateDnsKernelRule(void);

extern int ProfIsDefault(MibProfileType mibType, int instIndex, Pointer prof, int size);
static void UserStatusProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
extern int createIndRuleByCount();
#endif

void PDBUpdataProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const PdbProfile* oldProfileDataPtr, const PdbProfile* newProfileDataPtr )
{
#if (STATIC_ROUTE == FYES)
    Uint32 wanCount = 0;
    Uint32 slotIndex;
    char pdbname[UTT_INST_NAME_LEN + 1];
    InterfaceProfile *profIF = NULL;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    PdbProfile *newProf = (PdbProfile *)newProfileDataPtr;
#endif
    int isRoutePdb = 0;

    switch(changeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
#if (STATIC_ROUTE == FYES)
#if 1
	    /* �ӿ���Ŀ */
	    wanCount = getWanSlotCount();

	    /* ����wan�� */
	    for (slotIndex = 1U; slotIndex <= wanCount; slotIndex++) {

		profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, slotIndex);
		if ((profIF == NULL) || (profIF->head.active == FALSE)) { /* ��·��ɾ�� */
		    continue;
		}

		memset(pdbname, 0, UTT_INST_NAME_LEN+1);
		/* �����·���õ���Ӫ��pdb���� */
		snprintf(pdbname, UTT_INST_NAME_LEN, "%d", profIF->isp);
		if ((strcmp(pdbname, newProf->head.name) == 0) && (_getWanLinkState(slotIndex) == 1)) {
		    ISPRouteTableFlush(slotIndex); 
		    ISPRouteTableAdd(slotIndex);
		    /*
		     * ��·��pdb
		     */
		    isRoutePdb = 1;
		}
	    }
#endif
#endif
	    /*
	     * ����·��pdb������ʼ������
	     */
	    if (isRoutePdb == 0) {
		PDBUpdataInit();
#ifdef CONFIG_UTT_NETFILTER
extern void reNewPdbChainJumpRule(void);
		reNewPdbChainJumpRule();

#if 1
		/* update dns kernle rule, pdb db not support dns now */
		updateDnsKernelRule();
#endif

#endif
#if (GROUP_BASE == FYES)
		extern  void groupbaseInit(void);
		groupbaseInit();
		extern void actPolicyDelAll(void);
		actPolicyDelAll();/*ɾ���ں˶�Ӧ�����в��ԡ����ں��ٴ�����*/
#endif
#if (GAME_BOOST == FYES)
		extern void updateGameBoostRule(int index);
		updateGameBoostRule(index);
#endif
		extern void initPdbPolicyRule(void) ;
		initPdbPolicyRule();//������Ϊ�������¸���ʹ֮������Ч
	    }
	    break;
	default:
	    break;
    };
    return;
}


void PDBUpdataInit(void)
{
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
#ifdef CONFIG_UTT_NETFILTER
	    if((prof->type == L7Pdb) && (ProfIsDefault(mibType, i, prof, sizeof(PdbProfile)) != 0)) {
		createPdbFile(prof);
#if (GAME_BOOST == FYES)
	    }else if(prof->type == SpeedupPdb){
			createPdbFiles(prof);
#endif
#if (WIFIDOG_AUTH == FYES)
	    /*wifidog΢�Ų���*/
	    }else if(prof->type == wifidogUp){
		    PDB_PRINTF("%s-%d: index %d, name %s\n",__func__,__LINE__,i,prof->head.name);
			createPdbFiles(prof);
#endif
		
	}
#else
	    if(prof->type == L7Pdb && strlen(prof->line[0]) > 3 ){
		createPdbFile(prof);
#if (GAME_BOOST == FYES)
	    }else if(prof->type == SpeedupPdb){
			createPdbFiles(prof);
#endif
#if (WIFIDOG_AUTH == FYES)
	    /*wifidog΢�Ų���*/
	    }else if(prof->type == wifidogUp){
			createPdbFiles(prof);
#endif
		
	}
#endif
	}
    }
}


#if (GAME_BOOST == FYES) || (WIFIDOG_AUTH == FYES) /*wifidog΢�Ų���*/

extern char* parsePDB(char* pdb,const char* key);
void createPdbFiles(PdbProfile *profPdb){
	int i=0;
	char pdb[256],*fname,*content;	

	for(i=0;i<MAX_PDB_LINE_COUNT;i++)
	{
		if(strlen(profPdb->line[i])!=0)
		{
			strcpy(pdb,profPdb->line[i]);
			fname = parsePDB(pdb,"name");
			strcpy(pdb,profPdb->line[i]);
			content = parsePDB(pdb,"l7filter");
			
			if(fname!=NULL&&content!=NULL)
			createL7File(fname,content);
		
		    //printf("%s-%d: i=%d,line[%d]=%s\n",__FUNCTION__,__LINE__,i,i,profPdb->line[i]);
		}
	}
}
extern void createL7File(char* fname,char* content)
{
	char file_name[50]={0};
	FILE *fp;
	char name[UTT_INST_NAME_LEN+1];
	int i=0;
	strcpy(name, fname);
	sprintf(file_name,"/etc_ro/l7-protocols/%s.pat",name);
	if(!(fp=fopen(file_name,"w+"))) {
	    return;
	} else {
	    strcat(name,"\n");
	    fputs(name,fp);
	    name[strlen(name)-1]='\0';
		fputs(content,fp);
	    fclose(fp);
	}
}

#endif

/*
 * ��ȡPDB���ã�д��l7-protocols\name.pat
 * yqyang
 */

extern void createPdbFile(PdbProfile *profPdb)
{
	char file_name[50]={0};
	FILE *fp;
	char name[UTT_INST_NAME_LEN+1];
	int i=0;
	strcpy(name, profPdb->head.name);
	sprintf(file_name,"/etc_ro/l7-protocols/%s.pat",name);
	if(!(fp=fopen(file_name,"w+"))) {
	    return;
	} else {
	    strcat(name,"\n");
	    fputs(name,fp);
	    name[strlen(name)-1]='\0';
	    for(i=0;i<MAX_PDB_LINE_COUNT;i++)
	    {
		if(strlen(profPdb->line[i])!=0)
		{
		    fputs(profPdb->line[i],fp);
		    //printf("%s-%d: i=%d,line[%d]=%s\n",__FUNCTION__,__LINE__,i,i,profPdb->line[i]);
		}
	    }
	    fclose(fp);
	}
}

#if (USER_STATUS == FYES)
/*
 * ��ʼ��crond��ÿ��00:10:00���ͳ����Ϣ
 */
static void initCrondTimer(void) {
    doSystem("cron-init.sh del uttStatsClear 1 >/dev/nul 2>&1;cron-init.sh add \"%s uttStatsClear\" >/dev/null 2>&1 &", UTT_STATS_CLEAR_TIME);
}
#endif


/* ��pdb�� */
extern void clearPdbChain(char *name) {
    doSystem("iptables -t %s -F %s 1>/dev/null 2>&1", PDB_NF_TABLE, name);
    return;
}
/*
 * �½�DNSʶ�����
 */
extern void createDnsIndRule(PdbProfile *prof) {
    char temp[MAX_PDB_LINE_LENGTH + 1],*temp1;
    char *ptr;
    char slipchar[]=",";
    int i;
    UttNlNfDnsConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_DNS;
    /*����*/
    nlConf.confType = UTT_NFCONF_ADD;
    nlConf.entryVal.priType = prof->category;/*��һ����*/
    nlConf.entryVal.secType = prof->index;/*�ڶ�����*/


    for(i=0;i<MAX_PDB_LINE_COUNT;i++){
	if (strlen(prof->line[i]) > 0) {
	    strcpy(temp,prof->line[i]);
	    temp1 = strtok_r(temp,slipchar,&ptr);
	    while(temp1){
		if(strlen(temp1) > 4 ){
		    strncpy(nlConf.entryVal.domainName, temp1, MAX_DOMAIN_NAME_LENGTH);/*����*/
		    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
		}
		temp1=strtok_r(NULL,slipchar,&ptr);
	    }
	}
    }

    return;
}
/*
 * �½�l7ʶ�����
 */
extern void createPdbL7IndRule(PdbProfile *prof) {
    if (strcmp(prof->head.name, "AliIM") == 0) {
	doSystem("iptables -t %s -A %s -p tcp -m multiport --dport 80,443,16000 -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    } else if (strcmp(prof->head.name, "Fetion") == 0) {
	doSystem("iptables -t %s -A %s -p tcp -m multiport --dport 443,8080 -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    } else if (strcmp(prof->head.name, "QQLive") == 0) {
	doSystem("iptables -t %s -A %s -p tcp -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
	doSystem("iptables -t %s -A %s -p udp -m layer7 --l7proto %s2 -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    } else if (strcmp(prof->head.name, "PPTV") == 0) {
	doSystem("iptables -t %s -A %s -p tcp -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
	doSystem("iptables -t %s -A %s -p udp -m layer7 --l7proto %s2 -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    } else if (strcmp(prof->head.name, "wanmei") == 0) {
	doSystem("iptables -t %s -A %s -p tcp --sport 29001:29010 -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    } else if (strcmp(prof->head.name, "TL3") == 0) {
	doSystem("iptables -t %s -A %s -p tcp --dport 3733 -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    } else {
	doSystem("iptables -t %s -A %s -m layer7 --l7proto %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN, prof->head.name, prof->category, prof->index);
    }
    return;
}

/*
 * �½�Portʶ�����
 */
extern void createPdbPortIndRule(PdbProfile *prof) {
    if (strlen(prof->line[0]) != 0) {
	if (strstr(prof->line[0], ",")) { /* ��˿� */
	    doSystem("iptables -t %s -A %s -p tcp -m multiport --dport %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1",PDB_NF_TABLE, PDB_IND_CHAIN, prof->line[0], prof->category, prof->index);
	} else { /* ��һ�˿� */
	    doSystem("iptables -t %s -A %s -p tcp --dport %s -j UTTINDENTIFY --set %d,%d >/dev/null 2>&1",PDB_NF_TABLE, PDB_IND_CHAIN, prof->line[0], prof->category, prof->index);
	}
    }
    return;
}

/*
 * ����pdb ʶ�����
 */
static void updatePdbIndRule(void) {
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    PdbProfile *prof= NULL;
    SystemProfile *profSys = NULL;
    int max, min, i;

    /* ��� */
    clearPdbChain(PDB_IND_CHAIN);
    doSystem("iptables -t %s -D PREROUTING -j UTTWEBIND >/dev/null 2>&1", PDB_NF_TABLE);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    /* ʶ��û�п�����ֻ������򲻸��¹��� */
    if (profSys->UttNfIndentEn != FUN_ENABLE) {
	return;
    }

    ProfInstLowHigh(mibType, &max, &min);
    /* l7, portʶ�� */
    for (i = min; i < max; i++) {
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 1) {
	    continue;
	}

	if (prof->type == L7Pdb) {
	    createPdbL7IndRule(prof);
	} else if(prof->type == PortPdb) {
	    createPdbPortIndRule(prof);
	}
#if 0
	else if((updateDnsF == 1) && (prof->type == DnsPdb))
	{
	    createDnsIndRule(prof);
	}
#endif
    }

    /* dnsʶ��,��ɾ�ټ� */
    doSystem("iptables -t %s -A PREROUTING -j UTTWEBIND >/dev/null 2>&1", PDB_NF_TABLE);

    /* ������ܣ�����unknown���� ���жϻỰ��ʶ���ʶ */
    doSystem("iptables -t %s -I %s -m layer7 --l7proto unknown --l7classified -j RETURN >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN);

    return;
}

/*
 * ����dns pdb
 */
static void updateDnsKernelRule(void) {
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    int max, min, i;

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 1) {
	    continue;
	}

	if (prof->type == DnsPdb) {
#if (UTT_SMART_UI ==FYES)
	    /*��UIֻʹ���˹����˵�PDB��������ֻ��Ҫ���������˶�Ӧ��DNS���򼴿�*/
	    if ((strcmp("Advertise",prof->head.name) == 0)) {
		createDnsIndRule(prof);
	    }
#else
	    createDnsIndRule(prof);
#endif
	}
    }

    return;
}



#if (USER_STATUS == FYES)
/*
 * ����pdbͳ�ƹ���
 */
static void updatePdbStatsRule(void) {
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

#if 0
    clearPdbChain(PDB_STATS_CHAIN);
#endif

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
#ifndef STATS_FIXED
    /* ʶ��û�п�����ֻ������򲻸��¹��� */
    if (profSys->UttNfIndentEn != FUN_ENABLE) {
	return;
    }
#endif
#if 0
    doSystem("iptables -t %s -A %s -j UTTHOSTSTA >/dev/null 2>&1", PDB_NF_TABLE, PDB_STATS_CHAIN);
#endif
    return;
}
#endif
/*
 * ����pdb ʶ��ͳ�ƣ����Թ���
 */
static void updatePdbRule(void) {
    updatePdbIndRule();

#if (USER_STATUS == FYES)
    updatePdbStatsRule();
#endif
   
}

/*
 * �½�ʶ��ͳ�ƣ�������
 * utt_indentify_chain - ʶ����
 * utt_stats_chain	-   ͳ����
 * utt_policy_chain	-   ������
 */
static void createPdbChain(void) {
    /* ɾ��, ��ֹspeedweb���� */
#if 0
    clearPdbChain(PDB_IND_CHAIN);
    clearPdbChain(PDB_STATS_CHAIN);
    clearPdbChain(PDB_POLICY_CHAIN);
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_IND_CHAIN);
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_STATS_CHAIN);
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_POLICY_CHAIN);
#endif


    /* �½� */
    doSystem("iptables -t %s -N %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN);

#if (EXCEPT_QQ == FYES) || (EXCEPT_MSN == FYES) || (EXCEPT_ALI == FYES)
    doSystem("iptables -t %s -N %s 1>/dev/null 2>&1", PDB_NF_TABLE, IM_EXCEPT_CHAIN);
#endif

#if (USER_STATUS == FYES)
#if 0
    doSystem("iptables -t %s -N %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_STATS_CHAIN);
#endif
#endif

#if (GROUP_BASE == FYES)
    doSystem("iptables -t %s -N %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_POLICY_CHAIN);
#endif

#if 0
    /*
     * ������˳���ܴ�
     */
    doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_IND_CHAIN);
    doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, IM_EXCEPT_CHAIN);
    doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_STATS_CHAIN);
    doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_POLICY_CHAIN);
#endif
}

/*
 * IMʶ���Ƿ���
 * ĿǰָQQ������,����������������MSN�������Ƿ���
 * ����ֵ��
 *  1	-   QQ����������MSN���߰�������������֮һ����
 *  0	-   QQ������,����������������MSN��������û�п���
 */
static int isAnyIMOn(void) {
    int rst = 0;
#if ((EXCEPT_MSN == FYES) || (EXCEPT_QQ == FYES) || (EXCEPT_ALI == FYES) )
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if (!profSys) {
	return rst;
    }
#endif

#if (EXCEPT_QQ == FYES)
    if ((!rst) && ((profSys->exceptQQEn == FUN_ENABLE) || (profSys->exceptEnterpriseQQEn == FUN_ENABLE))) {
	rst = 1;
    }
#endif

#if (EXCEPT_MSN == FYES)
    if ((!rst) && (profSys->exceptMSNEn == FUN_ENABLE)) {
	rst = 1;
    }
#endif

#if (EXCEPT_ALI == FYES)
    if ((!rst) && (profSys->exceptAliEn == FUN_ENABLE)) {
 	rst = 1;
     }
#endif
    return rst;
}

/*
 * �½�mangle FORWARD��pdb����ת����
 * ����utt_indentify_chain,im_except_chain,utt_stats_chain,utt_policy_chain
 * ��ɾ�������½�
 */
extern void reNewPdbChainJumpRule(void) {
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (!profSys) {
	return;
    }

    /* ɾ�� */
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_IND_CHAIN);

#if (USER_STATUS == FYES)
#if 0
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_STATS_CHAIN);
#else
    doSystem("iptables -t %s -D %s -j UTTHOSTSTA 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN);
#endif
#endif

#if (GROUP_BASE == FYES)
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_POLICY_CHAIN);
#endif


#if (EXCEPT_QQ == FYES) || (EXCEPT_MSN == FYES) || (EXCEPT_ALI == FYES)
    doSystem("iptables -t %s -D %s -j %s 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, IM_EXCEPT_CHAIN);
#endif

    /* �½� */
    if ((profSys->UttNfIndentEn == FUN_ENABLE) 
#if (GROUP_BASE == FYES)
	|| (profSys->UttNfRuleEn == FUN_ENABLE)
#endif
	) 
    { 
	doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_IND_CHAIN);

#if (EXCEPT_QQ == FYES) || (EXCEPT_MSN == FYES) || (EXCEPT_ALI == FYES)
    if (isAnyIMOn()) { /* IM on */
	doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, IM_EXCEPT_CHAIN);
    }
#endif
    }

#if 0
#if (USER_STATUS == FYES)
#ifndef STATS_FIXED
    if (profSys->UserStatusEn == FUN_ENABLE)
#endif
    { /* ����ͳ�� */
#if 0
	doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_STATS_CHAIN);
#else
	doSystem("iptables -t %s -A %s -j UTTHOSTSTA 1>/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN);
#endif
    }
#endif
#endif

#if (GROUP_BASE == FYES)
    if (profSys->UttNfRuleEn == FUN_ENABLE) { /* ����������Ϊ���� */
	doSystem("iptables -t %s -A %s -j %s >/dev/null 2>&1", PDB_NF_TABLE, PDB_NF_CHAIN, PDB_POLICY_CHAIN);
    }
#endif

    return;
}

/* ����ʶ��ͳ��profile change */
/********************************************************************
* ������ UserStatusProfileChange
* ���ܣ� UserStatus���ر仯 ��Ч����
* ������ 2012-06-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void UserStatusProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
#if 0
	    if((oldProfileDataPtr->UttNfIndentEn == FUN_ENABLE) && (newProfileDataPtr->UttNfIndentEn == FUN_DISABLE))
	    {
		UserStatusChainDel();
	    }
	    else if((oldProfileDataPtr->UttNfIndentEn == FUN_DISABLE) && (newProfileDataPtr->UttNfIndentEn == FUN_ENABLE))
	    {
		UserStatusChainAdd();
	    }
	    else
	    {
		/*test*/
	    }
#endif
	    if (oldProfileDataPtr->UttNfIndentEn != newProfileDataPtr->UttNfIndentEn) {
		/*
		 * �ؽ�mangle FORWARD�����������
		 */
		reNewPdbChainJumpRule();

		updatePdbIndRule();

#if (USER_STATUS == FYES)
#ifndef STATS_FIXED
		updatePdbStatsRule();
#endif
#endif

#ifdef CONFIG_UTT_NETFILTER
		createIndRuleByCount();		
#endif
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:/*default*/
            break;
    }
    return;
}
/* ����ʶ��ͳ��profile change end */

void funInitPDB(void)
{
    registerForProfileUpdates(MIB_PROF_PDB_GLOBAL, NULL, (ProfileUpdateFnc)PDBUpdataProfileChange);
    PDBUpdataInit();
#ifdef CONFIG_UTT_NETFILTER
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)UserStatusProfileChange);
    createPdbChain();
    reNewPdbChainJumpRule();
    updatePdbRule();
    updateDnsKernelRule();
#if (USER_STATUS == FYES)
    initCrondTimer();
#endif
#if (GAME_BOOST == FYES)
	extern void gameboostInit(void);
	gameboostInit();
#endif
#endif
}
#endif
