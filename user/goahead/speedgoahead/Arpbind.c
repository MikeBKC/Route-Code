/*arp�����
 * bhou
 * 2011-10-25*/
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


#if (ARP_BINDS == FYES)
static void arpAllowNotBindEn(FunEnableEnum allowEn, char* ifName);
#if (HOTEL_PNP == FYES)
extern int isHotelPnpEn(void);
void  arpGlobalInit(void);
#else
static void  arpGlobalInit(void);
#endif

static void addArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName);
static void delArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName);
static void delAllArpBind(void);
static void arpBindInit(void);
static void editArpBindJumpRule(char action, char *ifName);

static void editArpBindJumpRule(char action, char *ifName)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    static int arpBindCount = 0;
    static int buildFlag = 0;/*��������ӱ�־ 1--����� 0--δ���*/
    int allowOther = 1;


    if (action == Iptables_ADD)/*���*/
    {
	arpBindCount += 1;
    }
    else if (action == Iptables_DEL)
    {
	arpBindCount -= 1;
    }
    else if (action == Iptables_FLUSH)/*���*/
    {
	arpBindCount = 0;
    }
    
    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*��ȡlan��*/
    if (ProfInstIsFree(prof) == PROFOK) /*��������*/
    {
	if(prof->arpAllowOtherEn == FUN_DISABLE)
	{
	    allowOther = 0;
	}
#if (HOTEL_PNP == FYES)
	/*�����Ƶ깦��������ǰ�����*/
	if(isHotelPnpEn() == 1 )   
	{
	    allowOther = 1;
	}
#endif
	if (buildFlag == 0)
	{
	    if ((arpBindCount > 0) || (allowOther != 1))/*��Ŀ����Ϊ0���߲�����ǰ��û���½����Ҫ�Զ�����*/
	    {
		doSystem("arptables -D INPUT -j %s -i %s+ 1>/dev/null 2>&1", ARP_BIND_CHAIN, ifName);/*��ɾ�������*/
		doSystem("arptables -X %s 1>/dev/null 2>&1", ARP_BIND_CHAIN);
		doSystem("arptables -N %s 1>/dev/null 2>&1", ARP_BIND_CHAIN);
		doSystem("arptables -I INPUT -j %s -i %s+ 1>/dev/null 2>&1", ARP_BIND_CHAIN, ifName);
		buildFlag = 1;
	    }
	}
	else/*���������*/
	{
	    if ((arpBindCount <= 0) && (allowOther == 1))
	    {
		doSystem("arptables -D INPUT -j %s -i %s+ 1>/dev/null 2>&1", ARP_BIND_CHAIN, ifName);
		doSystem("arptables -X %s 1>/dev/null 2>&1", ARP_BIND_CHAIN);
		buildFlag = 0;
	    }
	}
    }
}

/*arpȫ������profilechange ��������*/
void arpGlobalProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const InterfaceProfile *oldProf, const InterfaceProfile *newProf )
{
    if(changeType == PROF_CHANGE_EDIT)
    {
	if(index == 0) {
	    /*lan�����*/
	    /*ֻ�е�arpAllowOtherEn�仯�����ú���ʱ��������������*/
	    if(oldProf->arpAllowOtherEn != newProf->arpAllowOtherEn)
	    {
		arpAllowNotBindEn(newProf->arpAllowOtherEn, getLanIfName());/*�����arp���û����ӵ��豸*/
	    }
	    /*end*/
	}

    }
    return;
}
#if (DHCP_POOL == FYES)
static char* getLanIfNameForAc(char* poolName){

    DhcpPoolProfile *prof= NULL;
    MibProfileType mibType= MIB_PROF_DHCP_POOL;
    int min = 0, max = 0, i;
    static char interface[16];

    memset(interface,'\0',sizeof(interface));
    if ('\0' == *poolName){
        return  getLanIfName();
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
            if ( !strcmp(prof->head.name,poolName) ){
                if (i == 0) {
                    return getLanIfName();
                 } else {
                    sprintf(interface,"%s.%d",getLanIfName(),prof->vid);
//					printf("interface=%s,prof->interface=%s,prof->vid=%d\n",interface,prof->interface,prof->vid);
                    return interface;
//                  return prof->interface;
                }
            }
        }
    }

    return  getLanIfName();
}
#endif


void arpBindProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const UserProfile* oldProfileDataPtr, const UserProfile* newProfileDataPtr )
{
#if 0
    printf("arp prof change type:%d\n", changeType);
#endif
    switch(changeType) {
	case PROF_CHANGE_ADD:
#if (DHCP_POOL == FYES)
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfNameForAc(newProfileDataPtr->dhcpPoolName));/*���*/
#else
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfName());/*���*/
#endif
	    break;
	case PROF_CHANGE_ADD_ALL:
	    arpBindInit();/*���³�ʼ������Ϣ*/
	    break;
	case PROF_CHANGE_EDIT:
#if (DHCP_POOL == FYES)
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfNameForAc(oldProfileDataPtr->dhcpPoolName));/*��ɾ���ϵ�*/
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfNameForAc(newProfileDataPtr->dhcpPoolName));/*����µ�*/
#else
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfName());/*��ɾ���ϵ�*/
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfName());/*����µ�*/
#endif
	    break;
	case PROF_CHANGE_DEL:
#if (DHCP_POOL == FYES)
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfNameForAc(oldProfileDataPtr->dhcpPoolName));/*ɾ���ϵ�*/
#else
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfName());/*ɾ���ϵ�*/
#endif
	    break;
	case PROF_CHANGE_DELALL:
	    delAllArpBind();/*ɾ������*/
	    break;
	default:
	    break;
    };
}
/**
 * arp ����ǰ��û����ӵ��豸
 * bhou
 * 2011-10-25
 */
static void arpAllowNotBindEn(FunEnableEnum allowEn, char* ifName)
{
#if 0
#if (DHCP_POOL == FYES)
    int min = 0, max = 0, index;
    UserProfile *prof;
	ServiceZoneProfile *profSz = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    char tmpName[16];
    memset(tmpName,0,sizeof(tmpName));
#endif
#endif

    doSystem("arptables -D %s -j DROP 2>/dev/null", ARP_BIND_CHAIN);/*��ɾ��֮ǰ��ӵ�*/

    editArpBindJumpRule(Iptables_SPECIAL, ifName);
#if (HOTEL_PNP == FYES)
    /*�����Ƶ깦��������ǰ�����*/
    if(isHotelPnpEn() == 0 )   
    {
#endif
#if 0
#if (DHCP_POOL == FYES)
	ProfInstLowHigh(MIB_PROF_SERVICE_ZONE, &max, &min);

	for (index = (min + 1); index < max; index++) {
		profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(MIB_PROF_SERVICE_ZONE, index);
		if ((profSz != NULL) && (profSz->head.active == 1)) {
            if ( profSz->serviceEn == PROF_ENABLE ){
                sprintf(tmpName,"%s.%d",getLanIfName(),profSz->vid);
                doSystem("arptables -D %s -j DROP 2>/dev/null", ARP_BIND_CHAIN);/*��ɾ��֮ǰ��ӵ�*/
                if(allowEn == FUN_DISABLE)
                {
                    doSystem("arptables -A %s -j DROP", ARP_BIND_CHAIN);
                    doSystem("ip neigh flush dev %s 2>/dev/null",tmpName);/*���arp������������Ч*/
                }
            }
	    }
	}

#endif
#endif

	if(allowEn == FUN_DISABLE)
	{
	    /*Ĭ��Ϊ����������������ӹ���*/
	    doSystem("arptables -A %s -j DROP", ARP_BIND_CHAIN);
#if 0
	    doSystem("ip neigh flush dev %s 2>/dev/null",ifName);/*���arp������������Ч*/
#else
	    doSystem("ip neigh flush all");/*���arp������������Ч*/
#endif
	}

#if (HOTEL_PNP == FYES)
    }
#endif

    return;
}
/*arpȫ�����ó�ʼ��
 * bhou
 * 2011-10-25
 * */
#if (HOTEL_PNP == FYES)
void  arpGlobalInit(void)
#else
static void  arpGlobalInit(void)
#endif
{
    InterfaceProfile *prof = NULL;

    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*��ȡlan��*/

    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*��������*/
    {
	arpAllowNotBindEn(prof->arpAllowOtherEn, getLanIfName());/*�����arp���û����ӵ��豸*/
    }
    return;

}
/***********************************************************************
 * �� �� ����   addArpBind
 * ����������   ���ARP������ʱϵͳ���һ��arp -s����
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	modify by bhou at 2011-10-31
***********************************************************************/
static void addArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName)
{
    struct in_addr addrS;
    char* ipStr, *macStr;
    char arpAction[32]="RETURN";/*������û�����*/

    memset(&addrS, 0, sizeof(addrS));
    addrS.s_addr = addrIp;
    ipStr = inet_ntoa(addrS);/*ת��Ϊ�ַ�����ʽ*/
    macStr = converMac6To17Byte(macAddr);/*ת��Ϊð�ŷָ���ʽ*/

    editArpBindJumpRule(Iptables_ADD, ifName);

    if(allow == FUN_DISABLE)
    {
	strcpy(arpAction,"DROP");/*��������û�����*/
    }

    doSystem("arptables -I %s -s %s --source-mac %s -j %s", ARP_BIND_CHAIN, ipStr, macStr, arpAction);
    doSystem("arptables -A %s -s %s  -j DROP", ARP_BIND_CHAIN, ipStr);
    doSystem("arptables -A %s --source-mac %s  -j DROP", ARP_BIND_CHAIN, macStr);

    doSystem("ip neigh flush dev %s 2>/dev/null",ifName);/*���arp������������Ч*/
}

/***********************************************************************
 * �� �� ����   delArpBind
 * ����������   ɾ�����޸�ARP������ʱϵͳ����������
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	modify by bhou at 2011-10-31
 ***********************************************************************/
static void delArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName)
{
    struct in_addr addrS;
    char* ipStr, *macStr;
    char arpAction[32]="RETURN";/*������û�����*/

    memset(&addrS, 0, sizeof(addrS));
    addrS.s_addr = addrIp;
    ipStr = inet_ntoa(addrS);/*ת��Ϊ�ַ�����ʽ*/
    macStr = converMac6To17Byte(macAddr);/*ת��Ϊð�ŷָ���ʽ*/

    if(allow == FUN_DISABLE)
    {
	strcpy(arpAction,"DROP");/*��������û�����*/
    }

    doSystem("arptables -D %s -s %s --source-mac %s -j %s", ARP_BIND_CHAIN, ipStr, macStr, arpAction);
    doSystem("arptables -D %s -s %s -j DROP", ARP_BIND_CHAIN, ipStr);
    doSystem("arptables -D %s --source-mac %s -j DROP", ARP_BIND_CHAIN, macStr);
    editArpBindJumpRule(Iptables_DEL, ifName);
}

/***********************************************************************
 * �� �� ����   delAllArpBind
 * ����������   �б�ɾ���������ý��в���ʱ��Ч����
 * �������ڣ�	2011-11-04
 * �޸����ڣ�
 * ���ߣ�       bhou
 * ����˵����	��
***********************************************************************/
static void delAllArpBind(void)
{
    doSystem("arptables -F %s", ARP_BIND_CHAIN);/*��ɾ������*/
    editArpBindJumpRule(Iptables_FLUSH, getLanIfName());
    arpGlobalInit();/*ȫ�����ó�ʼ��*/
    return;
}

/***********************************************************************
 * ����������   arp bind ��ʼ������
 * �������ڣ�	2011-10-31
 * ���ߣ�       bhou
 * ����˵����	��
 ***********************************************************************/
static void arpBindInit(void)
{
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int i = 0,max = 0,min = 0;

    doSystem("arptables -F %s", ARP_BIND_CHAIN);/*ɾ������*/
    editArpBindJumpRule(Iptables_FLUSH, getLanIfName());

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) /*���������������*/
    {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) /*��������*/
	{
#if (DHCP_POOL == FYES)
	    addArpBind(prof->ip, prof->mac, prof->bindEn, getLanIfNameForAc(prof->dhcpPoolName));/*����µ�*/
#else
	    addArpBind(prof->ip, prof->mac, prof->bindEn, getLanIfName());/*����µ�*/
#endif
	}
    }
    arpGlobalInit();/*ȫ�����ó�ʼ��*/
    return;
}


/*arp�󶨹������
 * bhou
 * 2011-10-25
 * */
void funInitArpBind(void)
{
    registerForProfileUpdates(MIB_PROF_INTERFACE, NULL, (ProfileUpdateFnc)arpGlobalProfileChange);/*arpȫ������*/
    registerForProfileUpdates(MIB_PROF_USER, NULL, (ProfileUpdateFnc)arpBindProfileChange);/*arp��*/

    arpBindInit();/*�󶨳�ʼ��*/
    return;
}
#endif
