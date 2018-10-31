#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"

#if (DNS_FILTER == FYES)
#ifdef DNS_FILTER_DEBUG
#define DNS_FILTER_PRINTF(fmt, args...)          printf(fmt, ## args)
#else
#define DNS_FILTER_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
static void domainAddInit(void);
#endif
#if (NOTICE == FYES)
static void DnsNoticeDisChain(char action, char *chain, char *name, char *ip);
static void DnsNoticeEnChain(char action, char *chain, char *ip);
static void DnsDisNotice(int action);
static void DnsEnNotice(int action);
#endif
static void DnsConfigChain(char action, char *chain, char *name, char *target);

static void dnsFilterDrop(char *chain);
static void dnsFilterType(DnsFilterProfile *prof, SystemProfile *profSys, char *chain);
static void dnsFilterTimeIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain, char *chain);
#if (TIME_GRP == FYES)
extern void dnsFilterTime(TimeGrpProfile *timeGrpProf, char action, char *fir_chain, char *pre_chain, char *chain);
#else
extern void dnsFilterTime(char action, char *fir_chain, char *pre_chain, char *chain);
#endif
static void dnsFilterIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain);
static void dnsFilterProfInit()
{
    int min,max,index;
    char tmp[10];
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL; 
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	sprintf(tmp, "%d", index);
	if((strcmp(prof->domainName,"")!=0) && (strcmp(prof->head.name,tmp)!=0))//���ṹ���д�������ʱ�������к��Ƿ��indexһ��
	{
	    strcpy(prof->head.name,tmp);
	}
    }
}
/***********************************************************************
 * �� �� ����   dnsFilterInit
 * �������ڣ�	2011-11-02
 * �޸����ڣ�
 * ���ߣ�       
 * ����˵����	��ʼ������
***********************************************************************/
extern void dnsFilterInit(SystemProfile *oldProf)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof;
    SystemProfile *profSys = NULL;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
    char if_addr[16];

    memset(if_addr, 0, sizeof(if_addr));
    getIfIp(getLanIfName(), if_addr);
#endif

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    
    doSystem("iptables -F " DNS_PRE_INPUT_CHAIN "");
    doSystem("iptables -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -F " DNS_PRE_FORWARD_CHAIN "");
    doSystem("iptables -F " DNS_FORWARD_CHAIN "");

    if(oldProf != NULL)
    {
	DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
	dnsFilterTimeIp(oldProf, Iptables_DEL, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
	dnsFilterTimeIp(oldProf, Iptables_DEL, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);
    }
    else
    {
	DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
	dnsFilterTimeIp(profSys, Iptables_DEL, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
	dnsFilterTimeIp(profSys, Iptables_DEL, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);
    }
    dnsFilterTimeIp(profSys, Iptables_ADD, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
    dnsFilterTimeIp(profSys, Iptables_ADD, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if (!strcmp(prof->domainName, ""))
	{
	    continue;
	}
	if (profSys->dnsFilterEn == FUN_ENABLE)
	{
	    if (prof != NULL)
	    {
		dnsFilterType(prof, profSys, DNS_INPUT_CHAIN);
		dnsFilterType(prof, profSys, DNS_FORWARD_CHAIN);
	    }
	}
	else
	{
	    doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
	    doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");
	}
    }
    if((profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) && (profSys->dnsFilterEn == FUN_ENABLE))
    {/*ֻ����profile�е������������Ľ�ֹ*/
#if (NOTICE == FYES)
	profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
	    if (profNotice->head.active == 1) {
		DnsNoticeEnChain(Iptables_ADD, DNS_INPUT_CHAIN, if_addr);
		DnsNoticeEnChain(Iptables_ADD, DNS_FORWARD_CHAIN, if_addr);
	    } else {
#endif
		dnsFilterDrop(DNS_INPUT_CHAIN);
		dnsFilterDrop(DNS_FORWARD_CHAIN);
#if (NOTICE == FYES)
	    }
	} else {
	    dnsFilterDrop(DNS_INPUT_CHAIN);
	    dnsFilterDrop(DNS_FORWARD_CHAIN);
	}
#endif
    } 
    return;
}
static void dnsFilterDrop(char *chain)
{
    doSystem("iptables -A %s -j DROP", chain); /*��ֹ����dns��*/
    DNS_FILTER_PRINTF("iptables -A %s -j DROP\n", chain); /*��ֹ����dns��*/
    return;
}
/********************************************************************
* ������ dnsFilterTimeIp
* ���ܣ� ���ʱ���鼰ip�����
* ������ 2012-05-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void dnsFilterTimeIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain, char *chain)
{
#if (TIME_GRP == FYES)
    TimeGrpProfile *timeGrpProf = NULL;

    if(strlen(profSys->dnsFilterControl.timeGrpName) != 0U)
    {
	timeGrpProf = (TimeGrpProfile *)ProfGetInstPointByName(MIB_PROF_TIMEGRP, profSys->dnsFilterControl.timeGrpName);
    }
    else
    {
	timeGrpProf = NULL;
    }
#endif

    dnsFilterIp(profSys, action, fir_chain, pre_chain);
#if (TIME_GRP == FYES)
    dnsFilterTime(timeGrpProf, action, fir_chain, pre_chain, chain);
#else
    dnsFilterTime(action, fir_chain, pre_chain, chain);
#endif
    return;
}
/********************************************************************
* ������ dnsFilterTime
* ���ܣ� ���ʱ�������
* ������ 2012-05-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
#if (TIME_GRP == FYES)
extern void dnsFilterTime(TimeGrpProfile *timeGrpProf, char action, char *fir_chain, char *pre_chain, char *chain)
#else
extern void dnsFilterTime(char action, char *fir_chain, char *pre_chain, char *chain)
#endif
{
#if (TIME_GRP == FYES)

    int i = 0;
    char *daystr = NULL;
    char *timestr = NULL;
    char *datestr = NULL;

    if((timeGrpProf != NULL)&&(strcmp(timeGrpProf->datestart,"Forever")!=0))
    {
	datestr = getDateForIptables(timeGrpProf->datestart, timeGrpProf->datestop);/*iptables �����ֶ�*/
    }
#endif

#if (TIME_GRP == FYES)

    if((timeGrpProf != NULL)&&(strcmp(timeGrpProf->datestart,"Forever")!=0))
    {
	/*��ʼ���������ڵ�dns����ƥ��*/
	doSystem("iptables -%c %s -p udp --dport 53 -m time %s -j %s", action, fir_chain, datestr, pre_chain);
	DNS_FILTER_PRINTF("iptables -%c %s -p udp --dport 53 -m time %s -j %s\n", action, fir_chain, datestr, pre_chain);
    }
    else
    {/*ʱ������Ϊ�մ�������ʱ��*/
	/*dns����ƥ��*/
	doSystem("iptables -%c %s -p udp --dport 53 -j %s", action, fir_chain, pre_chain);
	DNS_FILTER_PRINTF("iptables -%c %s -p udp --dport 53 -j %s\n", action, fir_chain, pre_chain);
    }
#else
    /*dns����ƥ��*/
    doSystem("iptables -%c %s -p udp --dport 53 -j %s", action, fir_chain, pre_chain);
    DNS_FILTER_PRINTF("iptables -%c %s -p udp --dport 53 -j %s\n", action, fir_chain, pre_chain);
#endif

#if (TIME_GRP == FYES)
    if((timeGrpProf != NULL)&&(strcmp(timeGrpProf->datestart,"Forever")!=0))
    {
	for(i=0;i < TIMEGRP_ENTRYS; i++)
	{ /*��һ��ʱ��η��ϵ��� ��ת����������*/
	    daystr = getDayForIptables(timeGrpProf->timeEntry[i].day);
	    timestr = getTimeForIptables(timeGrpProf->timeEntry[i].timestart,timeGrpProf->timeEntry[i].timestop);
	    if(timeGrpProf->timeEntry[i].enable == FUN_ENABLE)
	    {
		doSystem("iptables -%c %s %s %s -j %s", action, pre_chain, timestr, daystr, chain);
		DNS_FILTER_PRINTF("iptables -%c %s %s %s -j %s\n", action, pre_chain, timestr, daystr, chain);
	    }
	}
    }
    else
    {/*ʱ������Ϊ�մ�������ʱ��*/
	doSystem("iptables -%c %s -j %s", action, pre_chain, chain);
	DNS_FILTER_PRINTF("iptables -%c %s -j %s\n", action, pre_chain, chain);
    }
#else
    doSystem("iptables -%c %s -j %s", action, pre_chain, chain);
    DNS_FILTER_PRINTF("iptables -%c %s -j %s\n", action, pre_chain, chain);
#endif
    return;
}

/********************************************************************
* ������ dnsFilterIp
* ���ܣ� ip�����
* ������ 2012-05-21
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void dnsFilterIp(SystemProfile *profSys, char action, char *fir_chain, char *pre_chain)
{
    char *ipstr = NULL;
    ipstr = getIpRangeForIptables_opposite(profSys->dnsFilterControl.ipstart, profSys->dnsFilterControl.ipend, 1);
    if(profSys->dnsFilterControl.addrTypeEn == ADDR_IPRANGE)
    {/*�ǿ������εĲ�ƥ��*/
	doSystem("iptables -%c %s %s -j RETURN", action, pre_chain, ipstr); 
	DNS_FILTER_PRINTF("iptables -%c %s %s -j RETURN\n", action, pre_chain, ipstr); 
    }
#if (IP_GRP == FYES)
    else
    {/*�ǿ�����Ĳ�ƥ��*/
	if(profSys->dnsFilterControl.addrTypeEn == ADDR_IPGRP)
	{
	    if(strlen(profSys->dnsFilterControl.ipGrpName) != 0U)
	    {
		doSystem("iptables -%c %s -m set ! --set %s src -j RETURN", action, pre_chain, profSys->dnsFilterControl.ipGrpName);
		DNS_FILTER_PRINTF("iptables -%c %s -m set ! --set %s src -j RETURN\n", action, pre_chain, profSys->dnsFilterControl.ipGrpName);
	    }
	}
    }
#endif
    return;
}
/********************************************************************
* ������ dnsFilterType
* ���ܣ� �����������
* ������ 2012-05-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void dnsFilterType(DnsFilterProfile *prof, SystemProfile *profSys, char *chain)
{
    char target[60];
    memset(target, 0, sizeof(target));
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
    char if_addr[16];
#endif

    DNS_FILTER_PRINTF("%s------%d,profSys->dnsFilterControl.filterType = %d\n", __func__, __LINE__, profSys->dnsFilterControl.filterType);
    if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_DIS)
    {/*ֻ��ֹprofile�е�����������������*/
#if (NOTICE == FYES)
	profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
	    if (profNotice->head.active == 1) {
		memset(if_addr, 0, sizeof(if_addr));
		getIfIp(getLanIfName(), if_addr);
		sprintf(target, "REJECT --reject-with dns-redirect:%s", if_addr);
	    } else {
#endif
		strcpy(target,"DROP");
#if (NOTICE == FYES)
	    }
	} else {
	    strcpy(target,"DROP");
	}
#endif
    } 
    else 
    {
	if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN)
	{/*ֻ����profile�е������������Ľ�ֹ*/
	    strcpy(target,"RETURN");
	} 
    }
    doSystem("iptables -A %s -m domain --name \"%s\" -j %s", chain, prof->domainName, target);
    DNS_FILTER_PRINTF("iptables -A %s -m domain --name \"%s\" -j %s\n", chain, prof->domainName, target);
    return;
}
#if (NOTICE == FYES)
static void DnsNoticeDisChain(char action, char *chain, char *name, char *ip)
{
    doSystem("iptables -%c %s -m domain --name \"%s\" -j REJECT --reject-with dns-redirect:%s", action, chain, name, ip);
    return;
}
static void DnsNoticeEnChain(char action, char *chain, char *ip)
{
    doSystem("iptables -%c %s -j REJECT --reject-with dns-redirect:%s", action, chain, ip);
    return;
}
static void DnsDisNotice(int action)
{
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    int min, max, index;
    InterfaceProfile *profIF = NULL;
    char if_addr[16];

    memset(if_addr, 0, sizeof(if_addr));
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
    if ((profIF != NULL) && (profIF->head.active == 1)) {/*�ж�lan��profile�Ƿ�����*/
	/*�˴�������getIfIp��ȡLan�ڣ���ΪgetIfIpȡ������ip���ܻ��Ǹ�Lan��֮ǰ��ip*/
	int2ip(htonl(profIF->ether.sta.ip), if_addr);
    }

    ProfInstLowHigh(mibType, &max, &min);

    doSystem("iptables -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -F " DNS_FORWARD_CHAIN "");
    for (index = min; index < max; index++) {
	prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0)){
	    if (strcmp(prof->domainName, "") != 0) {
		if (action == 0) {
		    DnsConfigChain(Iptables_ADD, DNS_INPUT_CHAIN, prof->domainName, DROP_CHAIN);
		    DnsConfigChain(Iptables_ADD, DNS_FORWARD_CHAIN, prof->domainName, DROP_CHAIN);
		} else {
		    DnsNoticeDisChain(Iptables_ADD, DNS_INPUT_CHAIN, prof->domainName, if_addr);
		    DnsNoticeDisChain(Iptables_ADD, DNS_FORWARD_CHAIN, prof->domainName, if_addr);
		}
	    }
	}
    }
    return;
}
static void DnsEnNotice(int action)
{
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    DnsFilterProfile *prof = NULL;
    int min, max, index;
    char if_addr[16];

    memset(if_addr, 0, sizeof(if_addr));
    getIfIp(getLanIfName(), if_addr);

    if (action == 0) {
	DnsNoticeEnChain(Iptables_DEL, DNS_INPUT_CHAIN, if_addr);
	DnsNoticeEnChain(Iptables_DEL, DNS_FORWARD_CHAIN, if_addr);
	dnsFilterDrop(DNS_INPUT_CHAIN);
	dnsFilterDrop(DNS_FORWARD_CHAIN);
    } else {
	doSystem("iptables -F " DNS_INPUT_CHAIN "");
	doSystem("iptables -F " DNS_FORWARD_CHAIN "");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
	    prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)){
		if (strcmp(prof->domainName, "") != 0) {
		    DnsConfigChain(Iptables_ADD, DNS_INPUT_CHAIN, prof->domainName, RETURN_CHAIN);
		    DnsConfigChain(Iptables_ADD, DNS_FORWARD_CHAIN, prof->domainName, RETURN_CHAIN);
		}
	    }
	}
	DnsNoticeEnChain(Iptables_ADD, DNS_INPUT_CHAIN, if_addr);
	DnsNoticeEnChain(Iptables_ADD, DNS_FORWARD_CHAIN, if_addr);
    }
    return;
}
extern void DnsNoticeChange(int action)
{
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if (profSys->dnsFilterEn == FUN_ENABLE)  {
	if (profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_DIS) {
	    /*ֻ��ֹprofile�е�����������������*/
	    DnsDisNotice(action);
	} 
	else if (profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) {
	    /*ֻ����profile�е������������Ľ�ֹ*/
	    DnsEnNotice(action);
	} else {
	    /*none*/
	}
    }
    return;
}
#endif
static void DnsConfigChain(char action, char *chain, char *name, char *target)
{
    doSystem("iptables -%c %s -m domain --name \"%s\" -j %s", action, chain, name, target);
    return;
}
static void dnsFilterChangeChain(DnsFilterProfile *prof, int change)
{
    SystemProfile *profSys = NULL;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
    char if_addr[16];

#endif
    char action;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if ((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) {
	if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_DIS) {
	    /*ֻ��ֹprofile�е�����������������*/
	    if (change == 1) {
		action = Iptables_ADD;
	    } else {
		action = Iptables_DEL;
	    }
#if (NOTICE == FYES)
	    profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
	    if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
		if (profNotice->head.active == 1) {
		    memset(if_addr, 0, sizeof(if_addr));
		    getIfIp(getLanIfName(), if_addr);
		    DnsNoticeDisChain(action, DNS_INPUT_CHAIN, prof->domainName, if_addr);
		    DnsNoticeDisChain(action, DNS_FORWARD_CHAIN, prof->domainName, if_addr);
		} else {
#endif
		    DnsConfigChain(action, DNS_INPUT_CHAIN, prof->domainName, DROP_CHAIN);
		    DnsConfigChain(action, DNS_FORWARD_CHAIN, prof->domainName, DROP_CHAIN);
#if (NOTICE == FYES)
		}
	    }
#endif
	} else if(profSys->dnsFilterControl.filterType == DNSFILTER_DOMAIN_EN) {
	    if (change == 1) {
		action = Iptables_INS;
	    } else {
		action = Iptables_DEL;
	    }
	    DnsConfigChain(action, DNS_INPUT_CHAIN, prof->domainName, RETURN_CHAIN);
	    DnsConfigChain(action, DNS_FORWARD_CHAIN, prof->domainName, RETURN_CHAIN);
	} else {
	    /*none*/
	}
    }
    return;
}
/***********************************************************************
 * �� �� ����   dnsFilterAdd 
 * �������ڣ�	2011-11-02
 * �޸����ڣ�
 * ���ߣ�       
 * ����˵����	��Ӻ���
***********************************************************************/
void dnsFilterAdd(Uint32 index)
{
    DnsFilterProfile *prof;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;

    prof = (DnsFilterProfile *)ProfGetInstPointByIndex(mibType, index);
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) { 
	if(strcmp(prof->domainName, "") != 0) {
	    dnsFilterChangeChain(prof, 1);
	}
    }
    return;
}

/***********************************************************************
 * �� �� ����   dnsFilterDel 
 * �������ڣ�	2011-11-02
 * �޸����ڣ�
 * ���ߣ�       
 * ����˵����	ɾ������
***********************************************************************/
void dnsFilterDel(DnsFilterProfile *prof)
{
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	if(strcmp(prof->domainName, "") != 0) {
	    dnsFilterChangeChain(prof, 0);
	}
    }
    return;
}

/***********************************************************************
 * �� �� ����   dnsFilterDelAll
 * �������ڣ�	2011-11-02
 * �޸����ڣ�
 * ���ߣ�       
 * ����˵����	ɾ�����к���
***********************************************************************/
void dnsFilterDelAll(void)
{
    doSystem("iptables -t filter -F " DNS_INPUT_CHAIN "");
    doSystem("iptables -t filter -F " DNS_FORWARD_CHAIN "");
}

/***********************************************************************
 * �� �� ����   dnsFilterProfileChange
 * �������ڣ�	2011-11-02
 * �޸����ڣ�
 * ���ߣ�       
 * ����˵����	��Ч����
***********************************************************************/
void dnsFilterProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const DnsFilterProfile *oldProfileDataPtr, const DnsFilterProfile *newProfileDataPtr )
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    dnsFilterAdd(index);
	    break;
        case PROF_CHANGE_EDIT:
	    DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
	    dnsFilterInit(NULL);
	    break;
        case PROF_CHANGE_DEL:
	    dnsFilterDel((DnsFilterProfile *)oldProfileDataPtr);
	    break;
        case PROF_CHANGE_DELALL:
	    dnsFilterDelAll();
	    break;
        default:
	    break;
    };
#endif
}

#if (FIREWALL == FYES)
extern void fw_rule_init(void);
#endif
extern void dnsFilter_rule_init()
{
    SystemProfile *profSys = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profSys->dnsFilterEn == FUN_ENABLE )
    {
	/* ����ֻ����һ��������������ɾ��ȷ��������������� */
	doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
	doSystem("iptables -I INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -I FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
    }     
    return ;
}

static void dnsFilterGlobalChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    /**
     * ���������ɹرյ�����
     */
    if ((oldProfileDataPtr->dnsFilterEn == FALSE) && (newProfileDataPtr->dnsFilterEn == TRUE)) {
	domainAddInit();/*��������ں�hash��*/
    } else {
	if ((oldProfileDataPtr->dnsFilterEn == TRUE) && (newProfileDataPtr->dnsFilterEn == FALSE)) {/*�ɿ������ر�*/
	    /*ɾ�������ں�hash����*/
	    sendMsgToKernel(profChangeNfNlSock, MIB_PROF_DNS_FILTER,0, (Pointer)NULL,0,PROF_CHANGE_DELALL);
	}
    }
    sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;
#if (NOTICE == FYES)
    NoticeProfile *profNotice = NULL;
#endif

    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
	    if ((newProf->dnsFilterEn != oldProf->dnsFilterEn) || 
		    (memcmp(&newProf->dnsFilterControl, &oldProf->dnsFilterControl, sizeof(DnsFilterControl))!=0))
	    {
		dnsFilterInit(oldProf);
#if (NOTICE == FYES)
		profNotice = (NoticeProfile*)ProfGetInstPointByIndex(MIB_PROF_NOTICE, 2);
		if((profNotice != NULL) && (ProfInstIsFree(profNotice) == 0)) {
		    if (profNotice->head.active == 1) {
			DnsNoticeChange(1);
		    }
		}
#endif
	    }

	    if ((oldProf->dnsFilterEn == FUN_ENABLE) 
		    && (newProf->dnsFilterEn == FUN_DISABLE)) { /* ��->�� */

		doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
		doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
	    } else if ((oldProf->dnsFilterEn == FUN_DISABLE) 
		    && (newProf->dnsFilterEn == FUN_ENABLE)) { /* ��->�� */
		/* ����ֻ����һ��������������ɾ��ȷ��������������� */
		doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
		doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
		doSystem("iptables -I INPUT -j %s", PRE_DNS_INPUT_CHAIN);
		doSystem("iptables -I FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
#if 0
#if (FIREWALL == FYES)
		/* ȷ�������������ȼ����ڷ���ǽ���ȼ� */
		fw_rule_init();
#endif
#endif
	    }
	    break;
	default:
	    break;
    }
#endif
}

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
static void domainAddInit(void)
{
    int min = 0, max = 0,i;
    DnsFilterProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_DNS_FILTER;
    
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (DnsFilterProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    DNS_FILTER_PRINTF("#######func:%s,line:%d,prof->head.name=%s###########\n",__func__,__LINE__,prof->head.name);
	    sendMsgToKernel(profChangeNfNlSock, mibType,i, (Pointer)NULL,0,PROF_CHANGE_ADD);
	}
    }
}
#endif
/***********************************************************************
 * �� �� ����   funInitDnsFilter
 * �������ڣ�	2011-11-02
 * �޸����ڣ�
 * ���ߣ�       
 * ����˵����	ע�ắ��
***********************************************************************/
void funInitDnsFilter(void)
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    int min = 0, max = 0,i;
    SystemProfile *profSys = NULL;
    MibProfileType mibType;;
    
    mibType = MIB_PROF_SYSTEM;
    ProfInstLowHigh(mibType, &max, &min);
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0) && (profSys->head.active == TRUE)) {
	DNS_FILTER_PRINTF("#######func:%s,line:%d,profSys->head.name=%s###########\n",__func__,__LINE__,profSys->head.name);
	if (profSys->dnsFilterEn == TRUE) {
	    sendMsgToKernel(profChangeNfNlSock, mibType,0, (Pointer)NULL,0,PROF_CHANGE_ADD);
	    domainAddInit();
	}
    }
    registerForProfileUpdates(MIB_PROF_DNS_FILTER, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterProfileChange);
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)dnsFilterGlobalChange);
#else
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 

    registerForProfileUpdates(MIB_PROF_DNS_FILTER, (Pointer)NULL, (ProfileUpdateFnc)dnsFilterProfileChange);
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)dnsFilterGlobalChange);

    /*Ϊ�˽��ɾ���м������������ᵼ��������������󸲸�ԭ������������*/
    dnsFilterProfInit();

    DNS_FILTER_PRINTF("%s------%d\n", __func__, __LINE__);
    doSystem("iptables -N " PRE_DNS_INPUT_CHAIN "");
    doSystem("iptables -N " PRE_DNS_FORWARD_CHAIN "");

    doSystem("iptables -N " DNS_PRE_INPUT_CHAIN "");
    doSystem("iptables -N " DNS_INPUT_CHAIN "");
    doSystem("iptables -N " DNS_PRE_FORWARD_CHAIN "");
    doSystem("iptables -N " DNS_FORWARD_CHAIN "");

    if (profSys->dnsFilterEn == FUN_ENABLE) {
	doSystem("iptables -D INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -D FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
	doSystem("iptables -A INPUT -j %s", PRE_DNS_INPUT_CHAIN);
	doSystem("iptables -A FORWARD -j %s", PRE_DNS_FORWARD_CHAIN);
    }
    dnsFilterInit(NULL);
#endif
}
#endif
