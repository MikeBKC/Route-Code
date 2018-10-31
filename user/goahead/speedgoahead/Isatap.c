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
#include "Isatap.h"
#if (IPV6_ISATAP == FYES)


#define SWORD_DEBUG 1
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Ipv6IsatapChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const IsatapProfile *oldProfileDataPtr, const IsatapProfile *newProfileDataPtr );

extern void Isatapinit(void);
static void IsatapRuleAdd(IsatapProfile *oldProfileDataPtr,IsatapProfile *newProfileDataPtr);



/********************************************************************
 * * ������ IsatapRuleAdd
 * * ���ܣ� ISATAP����������������Ӻ���
 * * ������ 2014-10-24
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void IsatapRuleAdd(IsatapProfile *oldProfileDataPtr,IsatapProfile *newProfileDataPtr)
{
    
    doSystem("/bin/ipv6/ip tunnel del name is0");    //ʹ������һ��ip���̣���·��ִ������
   if(newProfileDataPtr->ISATAPEnabled == 1){
	doSystem("/bin/ipv6/ip tunnel add name is0 mode isatap local %N",newProfileDataPtr->ISATAP_IPv4Gat);
	doSystem("/bin/ipv6/ip link set is0 up");
	doSystem("ip -6 addr add %s/64 dev is0",newProfileDataPtr->ISATAP_IPv6Pre);
    }
    Radvdinit();     //��дradvd�����ļ�
}

/********************************************************************
 * * ������ Isatapinit
 * * ���ܣ� Isatap��ʼ������
 * * ������ 2014-10-24
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
extern void Isatapinit()
{
    MibProfileType mibType=MIB_PROF_ISATAP;
    IsatapProfile   *prof=NULL;
    prof=(IsatapProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (prof->ISATAPEnabled == 1)){ 
	doSystem("/bin/ipv6/ip tunnel add name is0 mode isatap local %N",prof->ISATAP_IPv4Gat);
	doSystem("/bin/ipv6/ip link set is0 up");
	doSystem("ip -6 addr add %s/64 dev is0",prof->ISATAP_IPv6Pre);
    }    
}



/********************************************************************
 * * ������ Ipv6IsatapChange
 * * ���ܣ� Isatap Change����
 * * ������ 2014-10-24
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void Ipv6IsatapChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const IsatapProfile *oldProfileDataPtr, const IsatapProfile *newProfileDataPtr )
{

    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
            printf("***%s---%d\n",__func__,__LINE__);
	    IsatapRuleAdd((IsatapProfile *)oldProfileDataPtr,(IsatapProfile *)newProfileDataPtr);
            break;

	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;
}



/********************************************************************
 * * ������ funInitIsatap
 * * ���ܣ� Isatap ��ʼ����ں���
 * * ������ 2014-10-24
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang
 * ********************************************************************/
void funInitIsatap(void)
{
    
    registerForProfileUpdates(MIB_PROF_ISATAP, (Pointer)NULL, (ProfileUpdateFnc)Ipv6IsatapChange);/*ISATAP��������*/
    doSystem("cd bin;mkdir ipv6;cd ..;mv ip /bin/ipv6/");
    Isatapinit();/*��ʼ��*/
    printf("--%s--%d\n",__func__,__LINE__);
    return;
}

#endif
