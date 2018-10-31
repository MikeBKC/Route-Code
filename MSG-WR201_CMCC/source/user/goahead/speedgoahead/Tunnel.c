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
#include "Tunnel.h"
#if (IPV6_TUNNEL == FYES)


#define SWORD_DEBUG 1
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Ipv6TunnelChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const TunnelProfile *oldProfileDataPtr, const TunnelProfile *newProfileDataPtr );

extern void Tunnelinit(void);
static void TunnelRuleAdd(TunnelProfile *oldProfileDataPtr,TunnelProfile *newProfileDataPtr);



/********************************************************************
 * * ������ TunnelRuleAdd
 * * ���ܣ� �������������Ӻ���
 * * ������ 2014-10-15
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void TunnelRuleAdd(TunnelProfile *oldProfileDataPtr,TunnelProfile *newProfileDataPtr)
{
    
    doSystem("ip tunnel del he-ipv6");
   if(newProfileDataPtr->tunnelEnabled == 1){
	doSystem("ip tunnel add he-ipv6 mode sit remote %N local %N ttl 255",newProfileDataPtr->remote_ip4_addr,newProfileDataPtr->local_ip4_addr);
	doSystem("ip link set he-ipv6 up");
	doSystem("ip addr add %s/64 dev he-ipv6",newProfileDataPtr->remote_ip6_addr);
	doSystem("ip route add ::/0 dev he-ipv6 metric 1");
//	doSystem("ip -f inet6 addr");
    }
}

/********************************************************************
 * * ������ Tunnelinit
 * * ���ܣ� Tunnel��ʼ������
 * * ������ 2014-10-15
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
extern void Tunnelinit()
{
    MibProfileType mibType=MIB_PROF_TUNNEL;
    TunnelProfile   *prof=NULL;
    prof=(TunnelProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (prof->tunnelEnabled == 1)){ 
	doSystem("ip tunnel add he-ipv6 mode sit remote %N local %N ttl 255",prof->remote_ip4_addr,prof->local_ip4_addr);
	doSystem("ip link set he-ipv6 up");
	doSystem("ip addr add %s/64 dev he-ipv6",prof->remote_ip6_addr);
	doSystem("ip route add ::/0 dev he-ipv6 metric 1");
//        doSystem("ip -f inet6 addr");
    }    
}



/********************************************************************
 * * ������ Ipv6TunnelChange
 * * ���ܣ� Tunnel Change����
 * * ������ 2014-10-15
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void Ipv6TunnelChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const TunnelProfile *oldProfileDataPtr, const TunnelProfile *newProfileDataPtr )
{

    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    TunnelRuleAdd((TunnelProfile *)oldProfileDataPtr,(TunnelProfile *)newProfileDataPtr);
            break;

	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;
}



/********************************************************************
 * * ������ funInitTunnel
 * * ���ܣ� Tunnel ��ʼ����ں���
 * * ������ 2014-10-15
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang
 * ********************************************************************/
void funInitTunnel(void)
{
    
    registerForProfileUpdates(MIB_PROF_TUNNEL, (Pointer)NULL, (ProfileUpdateFnc)Ipv6TunnelChange);/*TUNNEL��������*/
    Tunnelinit();/*��ʼ��*/
    printf("--%s--%d\n",__func__,__LINE__);
    return;
}

#endif
