#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#if (MAC_FILTER == FYES)

static void macFilterInit(void);
static void editMacFilterJumpRule(char action, char *ifName);
static void editOneMacFilterRule(const MacFilterProfile *prof, char action);
static void macFilterDelAll(void);
extern void wirelessMacFilterGlobal(unsigned int index);
static void  WireMacFilterInit(void);
#if (WIRELESS == FYES)
extern void wirelessMacFilterDelAll(void);
#if (WIRELESS_5G == FYES)
extern void wireless5gMacFilterDelAll(void);
#endif
#endif
/*���ݾ�̬�ֲ������жϵ�ǰ���ڹ�������,
 * �޹��������Ҫɾ��INPUT����MAC�������Ĺ���
 * �й���ʱ��Ҫ��֤��ת�������������*/
static void editMacFilterJumpRule(char action, char *ifName)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *prof = NULL;
    static int macFilterCount = 0;
    static int buildFlag = 0;/*��������ӱ�־ 1--����� 0--δ���*/

    if ((action == Iptables_ADD) || (action == Iptables_INS))/*��ӹ���*/
    {
	macFilterCount += 1;
    }
    else if (action == Iptables_DEL)/*ɾ������*/
    {
	macFilterCount -= 1;
    }
    else if (action == Iptables_FLUSH) /*��չ���*/
    {
	macFilterCount = 0;
    }/*����һ��Iptables_SPECIAL�����ı����*/

    prof = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == PROFOK) /*����Ƿ���Ч*/
    {
	if (buildFlag == 0)
	{
	    /*���޹����й��򣬻��߲�����������*/
	    if ((macFilterCount > 0) || ((prof->head.active == TRUE) && (prof->action == MAC_ALLOW)))
	    {
		doSystem("arptables -N %s", MAC_FILTER_CHAIN);/*�½�MAC��ַ������*/
		doSystem("arptables -I INPUT -j %s -i %s+", MAC_FILTER_CHAIN, ifName);/*���IMPUT������������ת����*/
		buildFlag = 1;
	    }
	}
	else/*��ת��������ӹ�*/
	{
	    if ((macFilterCount <= 0) && ((prof->head.active != TRUE) || (prof->action != MAC_ALLOW))) /*�й����޹����Ҳ��ǿ���ֻ����״̬*/
	    {
		doSystem("arptables -D INPUT -j %s -i %s+ 1>/dev/null 2>&1", MAC_FILTER_CHAIN, ifName);/*ɾ����ת����*/
		doSystem("arptables -X %s 1>/dev/null 2>&1", MAC_FILTER_CHAIN);/*��ȫɾ���ù�����*/
		buildFlag = 0;
	    }
	}
	if ((prof->head.active == TRUE) && (prof->action == MAC_ALLOW))
	{
	    doSystem("arptables -D %s -j DROP 1>/dev/null 2>&1", MAC_FILTER_CHAIN);
	    doSystem("arptables -A %s -j DROP", MAC_FILTER_CHAIN);
	}
    }
}

/*MAC��ַ����ȫ������profilechange ��������*/
void macFilterGlobalProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const MacFilterGlobalProfile *oldProf, const MacFilterGlobalProfile *newProf)
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    break;
	case PROF_CHANGE_EDIT:
	    /*ÿ��ȫ�����øı�����³�ʼ��һ��*/
	    macFilterInit();
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_DEL:
	    break;
	case PROF_CHANGE_DELALL:
	    break;
	default:/*null*/
	    break;
    }
}


/*mac��ַ�����˺�profilechange��������*/
void macFilterProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const MacFilterProfile* oldProfileDataPtr, const MacFilterProfile* newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    editOneMacFilterRule(newProfileDataPtr, Iptables_INS);/*���*/
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_ADD_ALL:
	    macFilterInit();/*�������ʱ���³�ʼ�����й���*/
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_EDIT:
	    editOneMacFilterRule(oldProfileDataPtr, Iptables_DEL);/*ɾ��*/
	    editOneMacFilterRule(newProfileDataPtr, Iptables_INS);
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_DEL:
	    editOneMacFilterRule(oldProfileDataPtr, Iptables_DEL);
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	case PROF_CHANGE_DELALL:
	    macFilterDelAll();
#if(MAC_FILTER_JION_WIFI == FYES)
	    WireMacFilterInit();
#endif
	    break;
	default:/*null*/
	    break;
    }
}


/*ɾ�������һ��arptables����*/
static void editOneMacFilterRule(const MacFilterProfile *prof, char action)
{
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    char *tmpmac = NULL;

    profG = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((ProfInstIsFree(profG) == PROFOK) && (profG->head.active == TRUE)) /*����Ƿ���Ч*/
    {
	editMacFilterJumpRule(action, getLanIfName());
	tmpmac = converMac6To12Byte(prof->mac);/*ת��mac��ַ*/
	Conversion2StdMac(tmpmac);
	if (profG->action == MAC_ALLOW)/*����ʱ���������ķ��أ����ඪ��*/
	{
	    doSystem("arptables -%c %s --source-mac %s -j RETURN", action, MAC_FILTER_CHAIN, tmpmac);
	}
	else/*��ֹʱ���������Ķ��������෵��*/
	{
	    doSystem("arptables -%c %s --source-mac %s -j DROP", action, MAC_FILTER_CHAIN, tmpmac);
	}
	doSystem("ip neigh flush all");/*���arp���ù���������Ч*/
    }
}


/*ɾ������MAC���˹���*/
static void macFilterDelAll(void)
{
    doSystem("arptables -F %s", MAC_FILTER_CHAIN);/*ɾ�����й��˹���*/
    editMacFilterJumpRule(Iptables_FLUSH, getLanIfName());/*ɾ��INPUT��ת����͹�����*/
    doSystem("ip neigh flush all");/*���arp������������Ч*/
}
#if(MAC_FILTER_JION_WIFI == FYES)
#if(WIRELESS == FYES)
//����ȫ�ֱ�����mac��ַ��copy�ͳ�ʼ������
static void  WireMacFilterInit(void)
{
    int index = 0, min = 0, max = 0;
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    char *tmpmac;
    WirelessMacFilterProfile * wire_prof=NULL;
#if (WIRELESS_5G == FYES)
    Wireless5gMacFilterProfile *wire_5g_prof=NULL;
#endif
    WirelessMacFilterGlobalProfile *profGlobal;
#if (WIRELESS_5G == FYES)
    WirelessMacFilterGlobalProfile *profGlobal_5g;
#endif
    profG = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, index);
    profGlobal=(WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL, 0);//2.4G
#if (WIRELESS_5G == FYES)
    profGlobal_5g=(WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL, 1);//5G
#endif
    //ȫ�ֱ���copy
    if (profG->head.active == TRUE)
    {
	profGlobal->filterEn= FUN_ENABLE;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->filterEn= FUN_ENABLE;
#endif
    }
    else
    {
	profGlobal->filterEn = FUN_DISABLE;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->filterEn = FUN_DISABLE;
#endif
    }

    if(profG->action==MAC_ALLOW)
    {
	profGlobal->action=AP_MAC_ALLOW;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->action=AP_MAC_ALLOW;
#endif
    }
    else
    {
	profGlobal->action=AP_MAC_DENY;
#if (WIRELESS_5G == FYES)
	profGlobal_5g->action=AP_MAC_DENY;
#endif
    }
    ProfInstLowHigh(mibType, &max, &min);
    //mac��ַcopy
    for (index = min; index < max; index++)
    {
	prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	tmpmac = converMac6To12Byte(prof->mac);
	if(ProfInstIsFree(prof) == PROFOK)
	{
	    wire_prof=(WirelessMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_MAC_FILTER, index);
	    wire_prof->mac=prof->mac;
	    strcpy(wire_prof->head.name,tmpmac);
#if (WIRELESS_5G == FYES)
	    wire_5g_prof=(Wireless5gMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_WIRELESS_5G_MAC_FILTER, index);
	    wire_5g_prof->mac=prof->mac;
	    strcpy(wire_5g_prof->head.name,tmpmac);
#endif
	}
	else
	{
	    ProfDelInstByIndex(MIB_PROF_WIRELESS_MAC_FILTER, index);
#if (WIRELESS_5G == FYES)
	    ProfDelInstByIndex(MIB_PROF_WIRELESS_5G_MAC_FILTER, index);
#endif
	}
    }
	    wirelessMacFilterGlobal(0);
#if (WIRELESS_5G == FYES)
	    wirelessMacFilterGlobal(1);
#endif
}
#endif
#endif
/* ��ʼ����������������ÿ���޸�ȫ�����ö�����һ��*/
static void macFilterInit(void)
{
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
   
    int index = 0, min = 0, max = 0;
   
    macFilterDelAll();/*ɾ������MAC���˹���*/
    
    profG = (MacFilterGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, index);
    if (ProfInstIsFree(profG) == PROFOK) /*����Ƿ���Ч*/
    {
	if (profG->head.active == TRUE)/*���ܲ������򲻽�������*/
	{
	    /*���ֻ������ӵ�mac���ʵ�û���mac��ַ��profile��������⴦�����һ���������й���*/
	    editMacFilterJumpRule(Iptables_SPECIAL, getLanIfName());
	    ProfInstLowHigh(mibType, &max, &min);
	    for (index = min; index < max; index++)
	    {
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if(ProfInstIsFree(prof) == PROFOK)
		{
		    editOneMacFilterRule(prof, Iptables_INS);/*����ÿ��prof��ӹ���*/
		}
	    }
	}
    }
    return;
}


/*
 * ��ʼ��
 */
void funInitMacFilter(void)
{
    registerForProfileUpdates(MIB_PROF_MAC_FILTER_GLOBAL, NULL, (ProfileUpdateFnc)macFilterGlobalProfileChange);/*MAC��ַ����ȫ������*/
    registerForProfileUpdates(MIB_PROF_MAC_FILTER, NULL, (ProfileUpdateFnc)macFilterProfileChange);/*MAC��ַ������Ŀ����*/
    macFilterInit();/*������ʼ��*/
#if(MAC_FILTER_JION_WIFI == FYES)
    WireMacFilterInit();
#endif
}
#endif
