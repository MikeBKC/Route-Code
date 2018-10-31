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
#include "internet.h"
#include "spdComFun.h"
#include "ldra.h"
#include    <string.h>
#include    <dirent.h>
#include "pdb.h"
#include "groupbase.h"

#if (GROUP_BASE == FYES)
#ifdef  GROUP_SE_DBG 
#define GROUP_SE_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define GROUP_SE_PRINTF(fmt, args...)
#endif
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
extern void actPolicyDelAll(void);
extern int uttNfNlPolicyDelAll(void);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
#endif
#endif

extern void groupbaseInit(void);
extern void groupiptablesInit(void);
static void iptablesClearAllPdbChain(const GroupBaseProfile *prof);
extern void EditOneInstGroup(GroupBaseProfile *prof, char action);
static void EditOneActGroup(GroupBaseProfile *prof, char action);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
static void EditOneFineLimitGroup(const GroupBaseProfile *prof, char action);
#endif
extern void initPdbPolicyRule(void) ;
static char cmdstr[512];
#ifdef CONFIG_UTT_NETFILTER
static int changeCountByAllPdbPolicy(void);
static int changeCountByOnePdbPolicy(Uint8 *policy);
static int pdb_policy_match(Uint8 *policy, int index);
extern void createPdbL7IndRule(PdbProfile *prof);
extern void createPdbPortIndRule(PdbProfile *prof);
extern void createDnsIndRule(PdbProfile *prof);
extern int createIndRuleByCount(void);
extern void reNewPdbChainJumpRule(void);
static int count[MAX_PDB_PROFILES];/*��¼ÿ����Ϊ�������Ϳ����Ĵ���,����QQһ�����������Σ���Ӧ������Ϊ2*/
#endif

extern void utt_init_GROUP_LIMIT_CHAIN(LimitType type, char action)
{
    static char groupLimitCount = 0;/*��ϸ�����ٹ�������*/
    char groupLimitCountBak = groupLimitCount;
#if (PPPOE_LIMIT == FYES)
    static char pppAccLimitCount = 0;/*PPPoE���ٹ�������*/
    char pppAccLimitCountBak = pppAccLimitCount;
#endif

    if (type == FIXED_RATE_LIMIT)
    {
	if (action == Iptables_ADD)
	{
	    groupLimitCount += 1;
	}
	else if (action == Iptables_DEL)
	{
	    groupLimitCount -= 1;
	}
	else if (action == Iptables_FLUSH)
	{
	    groupLimitCount = 0;
	}
    }
#if (PPPOE_LIMIT == FYES)
    else if (type == PPPOE_ACC_LIMIT)
    {
	if (action == Iptables_ADD)
	{
	    pppAccLimitCount += 1;
	}
	else if (action == Iptables_DEL)
	{
	    pppAccLimitCount -= 1;
	}
	else if (action == Iptables_FLUSH)
	{
	    pppAccLimitCount = 0;
	}
    }
#endif
    if (groupLimitCountBak == 0)
    {
	if (groupLimitCount != 0)
	{
	    doSystem("iptables -t mangle -I FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
#if (WIFI_RATE_LIMIT == FYES)
	    doSystem("iptables -t mangle -D FORWARD -j "WIFI_RATE_LIMIT_CHAIN" 1>/dev/null 2>&1");
	    doSystem("iptables -t mangle -I FORWARD -j "WIFI_RATE_LIMIT_CHAIN" 1>/dev/null 2>&1");
#endif
	}
    }
    else
    {
	if (groupLimitCount == 0)
	{
	    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
	}
    }
#if (PPPOE_LIMIT == FYES)
    if ((groupLimitCountBak == 0) || (pppAccLimitCountBak == 0))
    {
	if ((groupLimitCount != 0) && (pppAccLimitCount != 0))
	{
	    doSystem("iptables -t mangle -I %s -m uttdev --lan-to-wan  -m set --set %s src -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	    doSystem("iptables -t mangle -I %s -m uttdev --wan-to-lan  -m set --set %s dst -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	}
    }
    else
    {
	if ((groupLimitCount == 0) || (pppAccLimitCount == 0))
	{
	    doSystem("iptables -t mangle -D %s -m uttdev --lan-to-wan  -m set --set %s src -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	    doSystem("iptables -t mangle -D %s -m uttdev --wan-to-lan  -m set --set %s dst -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	}
    }
#endif
    return;
}

void GroupBaseProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const GroupBaseProfile* oldProfileDataPtr, const GroupBaseProfile* newProfileDataPtr )
{
#ifdef CONFIG_UTT_NETFILTER
    GroupBaseProfile* prof = NULL;
#endif
    switch(changeType) {
	case PROF_CHANGE_ADD:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(newProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    EditOneInstGroup((GroupBaseProfile *)newProfileDataPtr,Iptables_ADD);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)newProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_EDIT:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    EditOneInstGroup((GroupBaseProfile *)oldProfileDataPtr,Iptables_DEL);
	    EditOneInstGroup((GroupBaseProfile *)newProfileDataPtr,Iptables_ADD);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)oldProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_DEL:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    EditOneInstGroup((GroupBaseProfile *)oldProfileDataPtr,Iptables_DEL);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)oldProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_DELALL:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    iptablesClearAllPdbChain((GroupBaseProfile *)oldProfileDataPtr);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)oldProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_SPECIAL:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType != GBT_FINE_LIMIT) {
#endif
#ifndef CONFIG_UTT_NETFILTER
	    /* ���������Ϊ����PDB���ڵ��� */
	    doSystem("iptables -t mangle -F %s" ,L7PROTO_PDB_CHAIN);
	    doSystem("iptables -t filter -F %s" ,PORT_DNS_PDB_CHAIN);
	    doSystem("iptables -t filter -F %s" ,DNS_PDB_CHAIN_LOOP);
#endif
	    doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_CHAIN);
	    utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, Iptables_FLUSH);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
		doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_PRE_CHAIN);
		doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_POST_CHAIN);
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    } else {/*�޸Ĺ�������ȫ��ɾ����Ȼ����������*/
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),PROF_CHANGE_DELALL);
	    }
#endif
	    groupbaseInit();
	    break;
	default:
	    break;
    }
#ifdef CONFIG_UTT_NETFILTER
    if((prof != NULL) && 
	    (prof->groupType ==  GBT_ACT_CONTROL))
    {
	/*ɾ���ں����в���
	 * ���ں��ٴ�����*/
	actPolicyDelAll();
    }
    
    /*GroupBaseProfile�иı�ʱ�ı�����*/
    changeCountByAllPdbPolicy();
    /*���½���ʶ��������û����������Ϊ�����أ�����;�˳��������ɹ���*/
    createIndRuleByCount();

#endif
    return;
}

#ifndef CONFIG_UTT_NETFILTER
#if (UTTPDB == FYES)
/*
 *
 *��iptales domian ���͵�pdbname���д���
 *
 */
static void _pdbProfAddDomainInit(char *str[],int lineNum,char grouptype,char *ipstr,char *timestr, char *daystr,FILE *fp)
{

    char temp[MAX_PDB_LINE_LENGTH],*temp1;
    char *ptr;
    char slipchar[]=",";
    int i;
    char domianstr[256];
    char cmdstr[256];
    for(i=0;i<lineNum;i++){
	if (strlen(str[i]) == 0) {
	    continue;
	}
	strcpy(temp,str[i]);
	temp1 = strtok_r(temp,slipchar,&ptr);
	while(temp1){
	    if(strlen(temp1) > 4 ){
		/*
		 * ��ÿһ����������iptables domain����
		 */
		memset(domianstr, 0x00, sizeof(domianstr));
		memset(cmdstr, 0x00, sizeof(cmdstr));
		sprintf(domianstr," -m domain --name \"%s\" ",temp1);
		fprintf(fp,"iptables -t filter -%c %s %s%s%s%s -j DROP\n",grouptype,PORT_DNS_PDB_CHAIN,ipstr,domianstr,timestr,daystr);
		fprintf(fp,"iptables -t filter -%c %s %s%s%s%s -j DROP\n",grouptype,DNS_PDB_CHAIN_LOOP,ipstr,domianstr,timestr,daystr);

	    }
	    temp1=strtok_r(NULL,slipchar,&ptr);
	}
    }
}
#endif
#endif

/*������Ϊ��������*/
static void EditOneActGroup(GroupBaseProfile *prof, char action)
{
#ifdef CONFIG_UTT_NETFILTER
    if(action ==  Iptables_ADD)
    {/*�ӹ����ʱ�����ת��*/
	uttSemP(SEM_UTTNF_NO,0);/*���� ��ֹ��ͻ*/
	if(strcmp(prof->timeStart, "All") == 0)
	{
	    prof->sTimeStart = 0u;/*�����뿪ʼ*/
	    prof->sTimeStop = converTimeToSec("24:00");/*���һ�����*/
	}
	else
	{
	    /*ת��Ϊ��*/
	    prof->sTimeStart = converTimeToSec(prof->timeStart);
	    prof->sTimeStop = converTimeToSec(prof->timeStop);
	}
	uttSemV(SEM_UTTNF_NO,0);/*�ͷ�����ֹ��ͻ*/
    }
#else
    /* FomeIp,EndIp used for lan Users, while FromIp2,EndIp2 used for PPPoE clients */
    unsigned int Allow;

    FILE *fp;
    /*pdb name strtok by PdbPolicy */
    char pdbname[20], pdbPolicys[256]; 
    char porttmp[512],portstr[512];

    /* initiate  local vairables */
    memset(pdbname, 0x00, sizeof(pdbname));
    memset(pdbPolicys, 0x00, sizeof(pdbPolicys));
    memset(cmdstr, 0x00, sizeof(cmdstr));
    memset(porttmp, 0x00, sizeof(porttmp));

    //doSystem("cat /dev/null>/sbin/GroupScript.sh");
    if((fp = fopen("/sbin/GroupScript.sh","w+")) <= 0)
    {
	DBGPRINT("open GroupScript.sh is error!\n");
    }
    Allow = prof->head.active;
    strcpy(pdbPolicys, prof->pdbPolicy);

    char* ipstr, *timestr, *daystr;
    char typestr[100];
    memset(typestr, 0x00, sizeof(typestr));


    ipstr = getIpRangeForIptables(prof->ipStart, prof->ipEnd, 1);
    timestr = getTimeForIptables(prof->timeStart, prof->timeStop);
    daystr = getDayForIptables(prof->day);
    /*
     * get PDB profile
     */
#if (UTTPDB == FYES)
    MibProfileType mibTypePdb = MIB_PROF_PDB_GLOBAL;
    PdbProfile *profPdb = NULL;
    char *tmp;
    /*
     *	ÿһ��������Ϊ�����ʵ�� pdbPolicys ��ָ����ͬ��pdbname,  ÿһ��pdbname����һ��ʵ�������洢��pdbname��Ӧ����ص���Ϣ
     */
    tmp = strtok(pdbPolicys, UTT_SPLIT_SIGN_RECORDS);
    while(tmp != NULL){
	strcpy(pdbname, tmp);
	profPdb = (PdbProfile*)ProfGetInstPointByName(mibTypePdb, pdbname);
	if(profPdb != NULL && ProfInstIsFree(profPdb) == 0 && profPdb->head.active == TRUE) {
	    /*
	     *  ��ͬ��PDB���ͽ��в�ͬ��ʵ�ַ�ʽ  L7PROTO_PDB_CHAIN
	     */
	    if(profPdb->type == L7Pdb) {
		/* L7 ͨ��iptables l7 ʵ�� */
		sprintf(typestr," -m layer7 --l7proto %s ",pdbname);
		fprintf(fp,"iptables -t mangle -%c %s %s%s%s%s -j DROP 1>/dev/null 2>&1\n",action,L7PROTO_PDB_CHAIN,ipstr,typestr,timestr,daystr);
	    }
	    else if(profPdb->type == PortPdb) {
		/* PORT ͨ������iptables port ���� �����ж˿ڱ�����porttmp��*/
		if(profPdb->line[0] != NULL)
		{
		    strcat(porttmp,profPdb->line[0]);
		    strcat(porttmp,",");
		}
		else
		{;}
	    }
	    else if(profPdb->type == DnsPdb) {


		/* DNS ͨ������iptables domainʵ��*/
		if(strcmp(pdbname,defaultPdbName4) == 0) {
		    /*webQQ */
		    /*  �������������ǽ�� ��ֹw.qq.com ��ƥ����www.qq.com ���Ծ���Ҫ�ȷŵ� */
		    fprintf(fp,"iptables -t filter -%c %s %s -m domain --name www.qq.com %s%s -j ACCEPT\n",action,PORT_DNS_PDB_CHAIN,ipstr,timestr,daystr);
		    fprintf(fp,"iptables -t filter -%c %s %s -m domain --name www.qq.com %s%s -j ACCEPT\n",action,DNS_PDB_CHAIN_LOOP,ipstr,timestr,daystr);
		    _pdbProfAddDomainInit(pdb_webQQ, MAX_PDB_WEBQQ_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName23) == 0) {
		    /* ��Ϸ��ַ*/
		    _pdbProfAddDomainInit(pdb_GAMEURL, MAX_PDB_GAMEURL_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName24) == 0) {
		    /* ֤ȯ��վ  */
		    _pdbProfAddDomainInit(pdb_STOCKURL, MAX_PDB_STOCKURL_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName25) == 0) {
		    /* ������վ  */
		    _pdbProfAddDomainInit(pdb_SHOPURL, MAX_PDB_SHOPURL_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName26) == 0) {
		    /* �罻��վ  */
		    _pdbProfAddDomainInit(pdb_SOCIALURL, MAX_PDB_SOCIALURL_LINES,action,ipstr,timestr,daystr,fp);
		}else
		{;}
	    }
	}
	else
	{;}
	tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
    /*
     *������PORT���͵Ķ˿��ռ���������ϳ�һ������
     *   ��linux���ж� -m multiport --dport �Ķ˿����� ��ǰ��15���˿ڣ��Ѿ��޸�Ϊ50��
     */
    /*ȥ�������ģ���*/
    porttmp[strlen(porttmp)-1] = '\0';
    sprintf(portstr,"  -p tcp -m multiport --dport %s ",porttmp);
    fprintf(fp,"iptables -t filter -%c %s %s%s%s%s -j DROP 1>/dev/null 2>&1\n",action,DNS_PDB_CHAIN_LOOP,ipstr,portstr,timestr,daystr);
    fclose(fp);
    doSystem("GroupScript.sh");
    GROUP_SE_PRINTF("Run GroupScript.sh,please wait.....\n");
#endif
#endif
}

/**
 * Ϊ��ϸ�����ٻ�ȡip��Χ
 * ipstrRet:�洢�����Ϣ
 */
static void getIpRangeForFineLimit(char* ipstrRet, const GroupBaseProfile *prof, bool isSrc)
{
    char* ipstr = NULL;
    char *dst_ipstr = NULL;
    if(prof->addrTypeEn == ADDR_IPRANGE)
    {/*�����ν�������*/
        ipstr = getIpRangeForIptables(prof->ipStart, prof->ipEnd, isSrc);/*ip��Χ*/
        strcpy(ipstrRet, ipstr);
    }
#if (IP_GRP == FYES)
    else
    {/*�����������*/
        if(prof->ipGrpName[0] == '\0')/*�����û�*/ {
            ipstr = getIpRangeForIptables(0u, 0u, isSrc);/*ip��Χ*/
            strcpy(ipstrRet, ipstr);
        } else {/*ipset��*/
            sprintf(ipstrRet, " -m set --set %s ", prof->ipGrpName);
            if(isSrc) {
                strcat(ipstrRet, "src ");
            } else {
                strcat(ipstrRet,"dst ");
            }
        }
    }
#endif
    if(prof->dst_addrTypeEn == ADDR_IPRANGE )
    {
        dst_ipstr = getDstIpRangeForIptables(prof->dst_ipStart, prof->dst_ipEnd, !isSrc);
        if(prof->addrTypeEn == ADDR_IPGRP)
        {
            strcat(ipstrRet, "-m iprange ");
        }
        strcat(ipstrRet, dst_ipstr);
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstrRet);
    }
#if(IP_GRP == FYES)
    else
    {
        if(prof->dst_ipGrpName[0] == '\0'){
            dst_ipstr = getDstIpRangeForIptables(0u, 0u, !isSrc);
            strcat(ipstrRet, dst_ipstr);
        }else{
            //sprintf(ipstrRet, " -m set --set %s ", prof->dst_ipGrpName);
            strcat(ipstrRet, " -m set --set ");
            strcat(ipstrRet, prof->dst_ipGrpName);
            if(!isSrc){
                strcat(ipstrRet, " src");
            }else{
                strcat(ipstrRet, " dst");
            }
        }
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstrRet);
     }
#endif


    return;
}

#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
#define IF_DIR_LEN  32
/**
 * ��ϸ����������
 * TODO:
 * ����lan-to-wan��wan-to-lan֮���VPN��·�������ˣ���Ҫ��proc/sys/net/ipv4/conf�ж�Ӧ��if_index����ppp��ӿڶ�Ӧ��ʵ�ӿ�
 */
static void EditOneFineLimitGroup(const GroupBaseProfile *prof, char action)
{
    char ipstr[106], *timestr, *daystr;
    char ratestr[32];
    char limittype[32];
    int i = 0;

    Uint32 rateTmp = 0u;
    utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, action);
	/* mantis_0011328 */

#if 1
    char if_dir[IF_DIR_LEN + 1] = {0};
#endif

    /*����ϴ��Լ���������*/
    for(i = 0 ;i < 2; i++)
    {
        memset(ipstr, 0, sizeof(ipstr));
#if 1
        memset(if_dir, 0, IF_DIR_LEN + 1);
#endif
        if( i == 0)
        {/*�ϴ�����*/
            getIpRangeForFineLimit(ipstr, prof, 1);/*Դip��Χ*/
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstr);
            strcpy(limittype, "src");/*���Դip��������*/
            rateTmp = prof->uRate;
            //strcpy(dst_limittype, "src"); /*���Ŀ�ĵ�ַ��Դip��������*/
#if 1
            strncpy(if_dir, " -m uttdev --lan-to-wan ", IF_DIR_LEN);
#endif
        }
        else
        {/*��������*/
            getIpRangeForFineLimit(ipstr, prof, 0);/* Ŀ��ip��Χ*/
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstr);
            strcpy(limittype, "dst");/*���Ŀ��ip��������*/
            rateTmp = prof->dRate;
            //strcpy(dst_limittype, "dst");/*���Ŀ�ĵ�ַ��Ŀ��ip��������*/

#if 1
            strncpy(if_dir, " -m uttdev --wan-to-lan ", IF_DIR_LEN);
#endif
        }
        timestr = getTimeForIptables((char*)prof->timeStart, (char*)prof->timeStop);/*iptablesʱ�����*/
        daystr = getDayForIptables(prof->day);/*iptables�����*/

        if(rateTmp == 0u)
        {
            /* * �ϴ�������Ϊ��ʱ�����٣�ֱ�ӷ��� -j RETURN **/
            sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -j RETURN", action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr);
            GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s%s -j RETURN\n", action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr);
            doSystem(cmdstr);
        }
        else
        {
            if(prof->limitPolicy == EXCLUSIVE_RATE)
            {/*��������*/
                sprintf(ratestr , "%.0u", rateTmp*1024 /8);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
                if(i==0){
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-in  %s%s%s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN", action ,GROUP_LIMIT_PRE_CHAIN,  ipstr, timestr,daystr ,prof->head.name, limittype, ratestr ,ratestr, limittype);
                }else{
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-out  %s%s%s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN", action ,GROUP_LIMIT_POST_CHAIN,  ipstr, timestr,daystr ,prof->head.name, limittype, ratestr ,ratestr, limittype);
                }
#else
#if 0
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN",
                        action ,GROUP_LIMIT_CHAIN, ipstr, timestr,daystr ,prof->head.name, limittype, ratestr ,ratestr, limittype);
#else
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -m hashlimit --hashlimit-name %d%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr,  timestr,daystr ,prof->ProfIndex, limittype, ratestr ,ratestr, limittype);
                GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s%s -m hashlimit --hashlimit-name %d%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN\n",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr ,prof->ProfIndex, limittype, ratestr ,ratestr, limittype);
#endif
#endif
                doSystem(cmdstr);
            } 
            else 
            {/*�������*/
                sprintf(ratestr , "%.0u", rateTmp*1024 /(8*32));
                /* share rate */
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
                if(i==0){
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-in %s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN",
                            action ,GROUP_LIMIT_PRE_CHAIN,  ipstr,timestr,daystr ,ratestr ,ratestr);
                }else{
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-out %s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN",
                            action ,GROUP_LIMIT_POST_CHAIN,  ipstr,timestr,daystr ,ratestr ,ratestr);
                }
#else
#if 0
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN", action ,GROUP_LIMIT_CHAIN,ipstr,timestr,daystr ,ratestr ,ratestr);
#else
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr ,ratestr ,ratestr);
                GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN\n",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr ,ratestr ,ratestr);
#endif
#endif
                /* make is in use */
                doSystem(cmdstr);
            }
            /* drop packages while rate is overflow */
#if 0
            sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s -j DROP", action ,GROUP_LIMIT_CHAIN ,ipstr,timestr,daystr);
#else
            /* *   ����Ѿ����٣����ٶȴﵽ����ֵ�����ݰ����� -j DROP  **/
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
            if(i==0){
                sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-in %s%s%s -j DROP", action , GROUP_LIMIT_PRE_CHAIN , ipstr,timestr,daystr);
            }else{
                sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-out %s%s%s -j DROP", action ,GROUP_LIMIT_POST_CHAIN , ipstr,timestr,daystr);
            }
#else
            sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -j DROP", action ,GROUP_LIMIT_CHAIN ,if_dir, ipstr, timestr,daystr);
            GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s%s -j DROP\n", action ,GROUP_LIMIT_CHAIN ,if_dir, ipstr, timestr,daystr);
#endif
            doSystem(cmdstr);
        }
#endif
    }
    return;
}
#endif

void EditOneInstGroup(GroupBaseProfile *prof, char action)
{
    if(action ==  Iptables_ADD && prof->head.active == FALSE)
    {
	/* ����������Ϊ�����ʵ������������������������*/
        return;
    }

    /* ���������Ϊ����PDB���ڵ��� */
    if(prof->groupType == GBT_ACT_CONTROL)
    {/*������Ϊ����*/
        EditOneActGroup(prof, action);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
    }
    else
    {/*��ϸ������*/
        if(prof->groupType == GBT_FINE_LIMIT)
	{
	    EditOneFineLimitGroup(prof, action);
	}
    /* ���������Ϊ����PDB���ڵ��� */
#endif
    }
    return;
}

void groupiptablesInit(void)
{
    /*��ϸ�����ٳ�ʼ��*/
    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -D PREROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_POST_CHAIN);
#endif
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, GROUP_LIMIT_CHAIN);
    utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, Iptables_FLUSH);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
	 doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", GROUP_LIMIT_PRE_CHAIN, GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", GROUP_LIMIT_POST_CHAIN, GROUP_LIMIT_POST_CHAIN);
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I PREROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_PRE_CHAIN, GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I POSTROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_POST_CHAIN, GROUP_LIMIT_POST_CHAIN);
#endif
#if 0    
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, GROUP_LIMIT_CHAIN);
#else
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
#endif
#ifndef CONFIG_UTT_NETFILTER
    /*
     * init L7PROTO
     */
    doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", L7PROTO_PDB_CHAIN);
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", L7PROTO_PDB_CHAIN, L7PROTO_PDB_CHAIN);
    doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I POSTROUTING 1 -j %s 1>/dev/null 2>&1", L7PROTO_PDB_CHAIN, L7PROTO_PDB_CHAIN);
    /*
     *init portpdb  and dns forword chain
     */
    doSystem("iptables -t filter -D FORWARD -j %s 1>/dev/null 2>&1", PORT_DNS_PDB_CHAIN);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", PORT_DNS_PDB_CHAIN, PORT_DNS_PDB_CHAIN);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1; iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", PORT_DNS_PDB_CHAIN, PORT_DNS_PDB_CHAIN);
/*
 * init dns loop chain with input chain
 */
    doSystem("iptables -t filter -D INPUT -j %s 1>/dev/null 2>&1", DNS_PDB_CHAIN_LOOP);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", DNS_PDB_CHAIN_LOOP, DNS_PDB_CHAIN_LOOP);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1; iptables -t filter -A INPUT -j %s 1>/dev/null 2>&1", DNS_PDB_CHAIN_LOOP, DNS_PDB_CHAIN_LOOP);
#endif
    groupbaseInit();
}

static void iptablesClearAllPdbChain(const GroupBaseProfile *prof)
{
#ifndef CONFIG_UTT_NETFILTER
    if(prof->groupType == GBT_ACT_CONTROL)
    {
	/* ���������Ϊ����PDB���ڵ��� */
	doSystem("iptables -t mangle -F %s" ,L7PROTO_PDB_CHAIN);
	doSystem("iptables -t filter -F %s" ,PORT_DNS_PDB_CHAIN);
	doSystem("iptables -t filter -F %s" ,DNS_PDB_CHAIN_LOOP);
    }
#endif
    if(prof->groupType == GBT_FINE_LIMIT)
    {
	doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_CHAIN);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_POST_CHAIN);
#endif
	utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, Iptables_FLUSH);
    }
}

void groupbaseInit(void)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
#if 0
    iptablesClearAllPdbChain();
#endif

    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0 && prof->head.active == TRUE) {
	    prof->ProfIndex = i + 1;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(prof->groupType == GBT_FINE_LIMIT) {/*��ϸ������*/
		sendMsgToKernel(profChangeNfNlSock, mibType,i, (Pointer)NULL,0,PROF_CHANGE_ADD);
	    } else {
#endif
	    /*
	     *edit one Inst Group 
	     */
	    EditOneInstGroup(prof,Iptables_ADD);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif

	    /*
	     *end edit one Inst Group
	     */
	}
    }

}
#ifdef CONFIG_UTT_NETFILTER
extern void reNewPdbChainJumpRule(void);

static void uttNfGlobalProfileChange(Pointer tag, MibProfileType profileType, ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    if(oldProfileDataPtr->UttNfRuleEn !=  newProfileDataPtr->UttNfRuleEn)
	    {
            reNewPdbChainJumpRule(); /* ��ʼ����utt_polich_chain�й����Ĺ��� */
            initPdbPolicyRule();
            /*���ռ�¼������������ܿ���������count���鿪����Ӧʶ��*/
            createIndRuleByCount();
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
#endif
void funInitGroupBase()
{
    registerForProfileUpdates(MIB_PROF_GROUP_BASE, NULL, (ProfileUpdateFnc)GroupBaseProfileChange);
    groupiptablesInit();
#ifdef CONFIG_UTT_NETFILTER
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)uttNfGlobalProfileChange);/*��������*/
    initPdbPolicyRule() ;
#endif

}

#ifdef CONFIG_UTT_NETFILTER
/** 
 * ɾ���ں˶�Ӧ�� ��Ϊ�������
 */
void actPolicyDelAll(void)
{
    /**
     * Ϊ�˱�̼� ����ֻҪ��Ϊ�������øı�
     * ����netlink��Ϣ�����ں��������
     * Ϊ�˷�ֹ�������̻��߳���ý��̵ĳ�ͻ����������
     * ��speedweb��netlink�����߳� ���������ʱ �Ѿ�������Ϣ��� ��δ���͡���������������ں˻������������һ��
     * �����
     *
     * ��� ��һ����� ��Ӧgoahead�������������ͷ���
     */
    uttSemP(SEM_UTTNF_NO,0);
    uttNfNlPolicyDelAll();
    uttSemV(SEM_UTTNF_NO,0);
    return;
}
/*ɾ���ں˶�Ӧ�����в���*/
extern int uttNfNlPolicyDelAll(void)
{
    int ret = 0;
    UttNlNfPolicy  nlConf;
    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_POLICY;
    /*����*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;
    ret = uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/
    return ret;
}


/*
 * ���²��Թ���
 * ��ע��������PDB_NF_TABLE�е�PDB_POLICY_CHAIN jump����ĳ�ʼ���������ʼ����pdb.c�н���
 */
void initPdbPolicyRule(void) {
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    clearPdbChain(PDB_POLICY_CHAIN);
#if (DHCP_SERVER == FYES)
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *prof = NULL;
    prof = (DhcpServerProfile *)ProfGetInstPointByIndex(mibType, 0);
    doSystem("iptables -t %s -D INPUT -p udp --dport 53 -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE);
#endif
#if (DHCP_POOL == FYES)
#if (DNS_AGENCY == FYES)
    MibProfileType mibType = MIB_PROF_DNS_AGENCY;
    DnsAgencyProfile *prof = NULL;
    prof = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibType, 0);
    doSystem("iptables -t %s -D INPUT -p udp --dport 53 -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE);
#endif
#endif
    actPolicyDelAll();
    if (profSys->UttNfRuleEn == FUN_ENABLE)
    {/*����������Ϊ����*/
	doSystem("iptables -t %s -A %s -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE, PDB_POLICY_CHAIN);
#if ((DHCP_SERVER == FYES) || (DHCP_POOL == FYES))
#if (DNS_AGENCY == FYES)
	if (prof->DnsProxyEn == FUN_ENABLE)
	{
	    doSystem("iptables -t %s -A INPUT -p udp --dport 53 -j UTTPOLICY", PDB_NF_TABLE);
	}
#endif
#endif
    }

    /*����GroupBaseProfile�е�PdbPolicy1��ʼ�����飬�����Ǽ�¼��Щ������Ϊ��������*/
    changeCountByAllPdbPolicy();
    /*���ݼ�¼������齨����Ӧʶ�����*/
    createIndRuleByCount(); 
    
    return;
}



/*
 * ����˵��������һ��GroupBaseProfile�е�pdbPolicy1�޸�����
 * ����ʱ�䣺2012-11-28
 */
static int changeCountByOnePdbPolicy(Uint8 *policy)
{
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof = NULL;
    int max = 0, min = 0, i = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
        if (ProfInstIsFree(prof) == 1)
        {
            continue;
        }
        if(pdb_policy_match(policy, prof->index) == 1)
        {
            /*ĳ����ֹ�ʹ�����Ӧλ��1������ʹ�ô�����1��ֵΪ0��ʾû�б�ʹ��*/
            count[i]+=1;
        }
    }
    return 0;
}


/*
 * ����˵������������GroupBaseProfile�е�pdbPolicy1�޸����飬�����¼���Ǹ����ܱ���������
 * ����ʱ�䣺2012-11-28
 * ����ֵ��0--����ִ����� 1--��;�˳�
 */
static int changeCountByAllPdbPolicy(void)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;    
    int max = 0, min = 0, i = 0;
    

    memset(count, 0, sizeof(count));
    ProfInstLowHigh(mibType, &max, &min);
    for( i = min; i < max; i++)
    {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(prof->groupType == GBT_ACT_CONTROL)
	    {
		changeCountByOnePdbPolicy(prof->pdbPolicy1);
	    }
	}
    }      

    return 0;
}


/*
 * �������ܣ��ж�pdb bitλ�Ƿ�����
 * ����ʱ�䣺2012-11-28
 * ����ֵ��1--������ 0--û�б�����
 */
static int pdb_policy_match(Uint8 *policy, int index)
{
    if (policy[index >> 3] & (1 << (index & 7))) {
	return 1;
    }
    return 0;
}


/*
 * �������ܣ��������鿪����Ӧʶ���ܣ������е����ִ���������
 * ����ʱ�䣺2012-11-28
 * ����ֵ��1--����Ҫ������������ 0--��������������˳�
 */
extern int createIndRuleByCount()
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof = NULL;
    int max = 0, min = 0, i = 0;
    /*��¼�Ƿ���DNS�౻�����ı�־*/
    int flagDNS = 0;
    /*��¼�Ƿ����κι��򱻿����ı�־*/
    int flagUnknown = 0;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

    /*���ʶ��ͳ���ǿ����ģ�����ʶ�������pdb.c�п���������Ҫ��������*/
    if (profSys->UttNfIndentEn == FUN_ENABLE)
    {
	return 1;
    }
     

    /*���utt_indentify_chain��,�������ж�ʶ��ͳ��֮���������Ϊ֮ǰ�������˳���ܸı�*/
    clearPdbChain(PDB_IND_CHAIN);
    doSystem("iptables -t %s -D PREROUTING -j UTTWEBIND >/dev/null 2>&1", PDB_NF_TABLE);

    /*������Ϊ����û�п���������Ҫ��������*/
    if (profSys->UttNfRuleEn == FUN_DISABLE)
    {
	return 1;
    }

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
        if (ProfInstIsFree(prof) == 1)
        {
            continue;
        }
	/*�����ǰcount[i]�����ô�������0��������Ӧʶ����*/
	if (count[i] > 0)
	{
	    /*���ݲ�ͬ���ʹ�����ͬ����*/
	    if (prof->type == L7Pdb) 
	    {
		createPdbL7IndRule(prof);
	    } 
	    else if(prof->type == PortPdb) 
	    {
		createPdbPortIndRule(prof);
	    }
	    else if(prof->type == DnsPdb )
	    {
#if 0
		createDnsIndRule(prof);
#endif
		flagDNS = 1;/*���κ�dns���͵Ľ�ֹ���򴴽�dns����*/
	    }
	    flagUnknown = 1;/*���κ���Ϊ����ֹ�򴴽�unknown����*/
	}
    }

    if (flagDNS == 1)
    {
	/* dnsʶ��,��ɾ�ټ� */
	doSystem("iptables -t %s -A PREROUTING -j UTTWEBIND >/dev/null 2>&1", PDB_NF_TABLE);
    }
    if (flagUnknown == 1)
    {
	/* ������ܣ�����unknown���� ���жϻỰ��ʶ���ʶ */
	doSystem("iptables -t %s -I %s -m layer7 --l7proto unknown --l7classified -j RETURN >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN);
    }
    return 0;
}
#endif
#endif
