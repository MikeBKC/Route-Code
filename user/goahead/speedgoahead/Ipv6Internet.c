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
#include "Radvd.h"
#include "Ipv6Internet.h"
//#include "ifmisc.h"
#if (IPV6_INTERFACE == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Ipv6InterfaceProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Ipv6InterfaceProfile *oldProfileDataPtr, const Ipv6InterfaceProfile *newProfileDataPtr );
static void Ipv6LanInit(void);
extern void Ipv6WanInit(void);

static void Ipv6LanConfig(Ipv6InterfaceProfile *profold,Ipv6InterfaceProfile *profnew);
static void Ipv6WanConfigAdd(Ipv6InterfaceProfile *prof);
static void Ipv6WanConfigDel(Ipv6InterfaceProfile *prof);

/********************************************************************
 * * ������ Ipv6LanConfigDel
 * * ���ܣ� Ipv6Lan��ɾ�����ú���
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/

static void Ipv6LanConfigDel(Ipv6InterfaceProfile *profold)
{
    char *port = getLanIfName();
    if(strcmp(profold->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,profold->ipv6LAN_addr,profold->LANprefixLength);
    }
//    getIpv6addr(name,addr);
//    printf("***%s----%d--addr=%s\n",__func__,__LINE__,addr);
//    if(strcmp(profnew->ipv6LAN_addr,addr) != 0){
	Radvdinit();
//    }
}

/********************************************************************
 * * ������ Ipv6LanConfig
 * * ���ܣ� Ipv6Lan�����ú���
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/

static void Ipv6LanConfig(Ipv6InterfaceProfile *profold,Ipv6InterfaceProfile *profnew)
{
    char *port = getLanIfName();
    if(strcmp(profold->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,profold->ipv6LAN_addr,profold->LANprefixLength);
    }
    if(strcmp(profnew->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s add %s/%d",port,profnew->ipv6LAN_addr,profnew->LANprefixLength);
	Radvdinit();
    }
}
/********************************************************************
 * * ������ Ipv6WanConfigAdd
 * * ���ܣ� Ipv6Wan��������Ӻ���
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/

static void Ipv6WanConfigAdd(Ipv6InterfaceProfile *prof)
{   
    int flag;
    char *port = getWanIfName(prof->port);
    if(strcmp(prof->ipv6WAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("ifconfig %s add %s/%d",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("ip -6 route add %s dev %s via %s metric 2",prof->ipv6Dns1,port,prof->ipv6Gateway); 
	doSystem("ip -6 route add %s dev %s via %s metric 2",prof->ipv6Dns2,port,prof->ipv6Gateway); 

	printf("ifconfig %s add %s/%d\n",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("route -A inet6 add default gw %s dev %s metric 2",prof->ipv6Gateway,port);
	doSystem("echo 'nameserver %s' > /tmp/ipv6resolv.conf",prof->ipv6Dns1);
	doSystem("echo 'nameserver %s' >> /tmp/ipv6resolv.conf",prof->ipv6Dns2);
	doSystem("cat /tmp/ipv4resolv.conf > /etc/resolv.conf");
	doSystem("cat /tmp/ipv6resolv.conf >> /etc/resolv.conf");
    }

}
/********************************************************************
 * * ������ Ipv6WanConfigDel
 * * ���ܣ� Ipv6Wan������ɾ������
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/

static void Ipv6WanConfigDel(Ipv6InterfaceProfile *prof)
{   
    char *port = getWanIfName(prof->port);
    if(strcmp(prof->ipv6WAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("ip -6 route del %s dev %s via %s metric 2",prof->ipv6Dns1,port,prof->ipv6Gateway); 
	doSystem("ip -6 route del %s dev %s via %s metric 2",prof->ipv6Dns2,port,prof->ipv6Gateway); 
	doSystem("route -A inet6 del default gw %s dev %s metric 2",prof->ipv6Gateway,port);
	doSystem("echo '' > /tmp/ipv6resolv.conf");
	doSystem("cat /tmp/ipv4resolv.conf > /etc/resolv.conf");
	doSystem("cat /tmp/ipv6resolv.conf >> /etc/resolv.conf");

    }
}

/********************************************************************
 * * ������ Ipv6LanInit
 * * ���ܣ� Ipv6Lan�ڳ�ʼ������
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/

static void Ipv6LanInit(void)
{
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;
    prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 0);
    Ipv6LanConfig((Ipv6InterfaceProfile *)prof,(Ipv6InterfaceProfile *)prof);
}
/********************************************************************
 * * ������ Ipv6WanInit
 * * ���ܣ� Ipv6Wan�ڳ�ʼ������
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/

extern void Ipv6WanInit(void)
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
   
    for(index=min;index<max-1;index++){
	prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , index+1);

//        printf("*****************************\n");
	if((prof != NULL) &&  (ProfInstIsFree(prof) == 0) && (prof->head.active == 1)){
             
	    Ipv6WanConfigAdd((Ipv6InterfaceProfile *)prof);
        printf("addr=%s,-%s --%d\n",prof->ipv6WAN_addr,__func__,__LINE__);
	}
    }
}

/********************************************************************
 * * ������ Ipv6Interface ProfileChange
 * * ���ܣ� IPv6�ӿ����ñ仯 ����
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void Ipv6InterfaceProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const Ipv6InterfaceProfile *oldProfileDataPtr, const Ipv6InterfaceProfile *newProfileDataPtr )
{
        switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    if((strcmp(oldProfileDataPtr->ipv6LAN_addr,newProfileDataPtr->ipv6LAN_addr) != 0)|| (oldProfileDataPtr->LANprefixLength != newProfileDataPtr->LANprefixLength)||(oldProfileDataPtr->LanAutoEnabled != newProfileDataPtr->LanAutoEnabled)){                   //LAN�����÷����ı�
		Ipv6LanConfig((Ipv6InterfaceProfile *)oldProfileDataPtr,(Ipv6InterfaceProfile *)newProfileDataPtr);
	    }
            if((strcmp(oldProfileDataPtr->ipv6WAN_addr,newProfileDataPtr->ipv6WAN_addr) != 0) || (oldProfileDataPtr->WANprefixLength != newProfileDataPtr->WANprefixLength) || (strcmp(oldProfileDataPtr->ipv6Dns1, newProfileDataPtr->ipv6Dns1) != 0)|| (strcmp(oldProfileDataPtr->ipv6Dns2,newProfileDataPtr->ipv6Dns2) != 0) || (strcmp(oldProfileDataPtr->ipv6Gateway, newProfileDataPtr->ipv6Gateway) != 0)){
              //WAN�����÷����ı� 
		Ipv6WanConfigDel((Ipv6InterfaceProfile *)oldProfileDataPtr);    //ɾ��������
                Ipv6WanConfigAdd((Ipv6InterfaceProfile *)newProfileDataPtr);    //���������
	    }
	    break;
	case PROF_CHANGE_DEL:
                
	    if((strcmp(oldProfileDataPtr->ipv6LAN_addr,newProfileDataPtr->ipv6LAN_addr) != 0)|| (oldProfileDataPtr->LANprefixLength != newProfileDataPtr->LANprefixLength)||(oldProfileDataPtr->LanAutoEnabled != newProfileDataPtr->LanAutoEnabled)){                   //LAN�����÷����ı�
		Ipv6LanConfigDel((Ipv6InterfaceProfile *)oldProfileDataPtr);
	    }

	    if((strcmp(oldProfileDataPtr->ipv6WAN_addr,newProfileDataPtr->ipv6WAN_addr) != 0) || (oldProfileDataPtr->WANprefixLength != newProfileDataPtr->WANprefixLength) || (strcmp(oldProfileDataPtr->ipv6Dns1, newProfileDataPtr->ipv6Dns1) != 0)|| (strcmp(oldProfileDataPtr->ipv6Dns2,newProfileDataPtr->ipv6Dns2) != 0) || (strcmp(oldProfileDataPtr->ipv6Gateway, newProfileDataPtr->ipv6Gateway) != 0)){
		Ipv6WanConfigDel((Ipv6InterfaceProfile *)oldProfileDataPtr);
	    }
                break;
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;



}


/********************************************************************
 * * ������ funInitIpv6Interface
 * * ���ܣ� IPv6�ӿ� ��ʼ����ں���
 * * ������ 2014-09-19
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao��yunyang
 * ********************************************************************/
void funInitIpv6Interface(void)
{
    registerForProfileUpdates(MIB_PROF_IPV6_INTERFACE, (Pointer)NULL, (ProfileUpdateFnc)Ipv6InterfaceProfileChange);/*��������*/

//    Ipv6LanInit();/*LAN��ʼ��*/
//    Ipv6WanInit();/*WAN��ʼ��*/
    return;
}

#endif
