#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#ifndef CONFIG_RALINK_RT2880
#include    <linux/netfilter/nf_conntrack_common.h>
#endif
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"

#if (HOTEL_PNP == FYES)

#include "utt_conf_define.h"
#if (ARP_BINDS == FYES)
extern void  arpGlobalInit(void);
#endif
extern void init_dnsmasq(int wanNo);
#if (NET_SECURITY == FYES)
extern void netSecuritySet(void);
#endif
#if (UTT_SMART_UI == FYES)
extern void addGwForETask(void);
extern void delRtForETask(void);
#endif

static void hotelPnpSet(void);
static void hotelPnpInit(void);
int isHotelPnpEn(void);

/********************************************************************
 ** ������ fhotelChange                                            **
 ** ���ܣ� �����ж�������Ӧ                                        **
 ** ������ 2012-10-30                                              ** 
 ** ������ ��                                                      **
 ** ���أ� ��                                                      **
 ** ����� ��                                                      **
 ** ���ߣ� zheng.xiao                                              ** 
 ********************************************************************/ 
static void hotelChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    if (oldProfileDataPtr->hotelPnpEn != newProfileDataPtr->hotelPnpEn) 
    {/*��һ���Ŵ���*/
	hotelPnpSet();
#if (UTT_SMART_UI == FYES)
	/*mantis:13634
	 * ������WAN��up֮ǰ��ҪΪ���弴��������أ�֮ǰ���׹���
	 * ����ʱ��ӣ����ڿ������������弴�ã���Ϊ���øı�ʱ���*/
	if (newProfileDataPtr->hotelPnpEn == FUN_ENABLE)
	{
	    addGwForETask();
	}
	else
	{
	    delRtForETask();
	}
#endif
    }
    return;
}
/**
 * �жϾƵ꼴������ �Ƿ���
 * return ; 1 ����
 *          0 δ����
 */
int isHotelPnpEn(void)
{
    SystemProfile *profSys = NULL;
    int ret = 0;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if(profSys->hotelPnpEn == FUN_ENABLE )   
    {
	ret = 1;
    }
    return ret;
}

/**
 * �Ƶ꼴�弴�� ��Ч���
 */
static void hotelPnpSet(void)
{
#if (ARP_BINDS == FYES)
    /*��ҪΪ�Ƿ�����ǰ��û�����*/
    arpGlobalInit();
#endif
    /*dns ����*/
    init_dnsmasq(1);
#if (NET_SECURITY == FYES)
    /*DdoS��������*/
    netSecuritySet();
#endif
    /*������ع�����Ч*/
    hotelPnpInit();
    editDNSRedirectRule();
    return;
}

/**
 *  �Ƶ깦�� ��ʼ��
 */
static void hotelPnpInit(void)
{
    char *lanIf = getLanIfName();
    char iptAct1 = Iptables_ADD;
    char iptAct2 = Iptables_DEL;
    char iprAct[4] = "add";
    int arpProxy = 1;

    if(isHotelPnpEn() == 0)
    {/*disable*/
	iptAct1 = Iptables_DEL;
	iptAct2 = Iptables_ADD;
        strcpy(iprAct, "del");
	arpProxy = 0;
    }
    /*arp����*/
    doSystem("echo %d > /proc/sys/net/ipv4/conf/%s/proxy_arp", arpProxy, lanIf);
    #if 0
/*�����ں�ipt_pre_routing_hook����ʵ��*/
    /*����ԭ������·markƥ��ĵط�*/
    doSystem("iptables -t mangle -%c PREROUTING ! -i %s -j MARK --and-mark 0x%x", iptAct2, lanIf, (~IPRULE_CONNMARK_MSK));
#endif
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
    /*lan�������������Ӵ��ǩ����lan���εľƵ��û���*/
    doSystem("iptables -t mangle -%c PREROUTING -i %s -m set ! --set %s src -m conntrack --ctstate NEW -j CONNMARK --set-mark %u/0x%x 1>/dev/null 2>&1",iptAct1, lanIf, SYS_LAN_RANGE_GROUP, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK);
#if 0
/*�����ں�ipt_pre_routing_hook����ʵ��*/
    /*lan�ڽ����İ���ֻ�е�ԴipΪ�Ƶ��û����ұ�ǩΪ�Ƶ���ϵģ��Ų���ip ruleƥ��ñ�ǩ����ֹ��lan�����ľƵ��û���
     * ֱ�Ӵ�lan����*/    
    doSystem("iptables -t mangle -%c PREROUTING -i %s -m set ! --set %s src -m connmark --mark %u/0x%x  -j MARK --and-mark 0x%x 1>/dev/null 2>&1 ", iptAct1, lanIf, SYS_LAN_RANGE_GROUP, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK, (~HOTEL_CONNNARK_MSK));
#else
    /*�Ƶ꼴�����ý����û� ���ϵ�mark ��Ӧ�� ��ip rule ƥ��
     * ��ֹ�Ƶ� ���弴���û� ���������İ� ֱ�Ӵ�lan ��ȥ
     * ͬʱ Ϊ��֤ �Ƶ��û��� ������������������
     * �� �������ظ� �Ƶ��û��İ��е�mark Ӧ�ñ�ip rule ƥ��*/
    doSystem("iptables -t mangle -%c PREROUTING -i %s -m set ! --set %s src  -j MARK --and-mark 0x%x 1>/dev/null 2>&1 ", iptAct1, lanIf, SYS_LAN_RANGE_GROUP, (~HOTEL_CONNNARK_MSK));
#endif
#else
#error ipset has been moved
#endif
#else
#error MULTI_LAN hash been moved
#endif

#if 0
/*�����ں�ipt_pre_routing_hook����ʵ��*/
    /*wan�ڽ����ġ��Ƕ���·���ϵ�mark��ƥ�� ��� ip rule.��ֹwan�����İ�ֱ�Ӵ�wan��ȥ*/
    doSystem("iptables -t mangle -%c PREROUTING  ! -i %s -m connmark ! --mark %u/0x%x -j MARK --and-mark 0x%x  1>/dev/null 2>&1",iptAct1, lanIf, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK, (~POLICYROUTE_CONNMARK_MSK));
#endif

     /*·�����*/
    doSystem("ip route %s table %d dev %s; ip rule %s fwmark 0x%x/0x%x lookup %d prio %d; ip route flush cache 1>/dev/null 2>&1", iprAct, HOTEL_PNP_ROUTE_TABLE, lanIf, iprAct, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK, HOTEL_PNP_ROUTE_TABLE, HOTEL_PNP_ROUTE_RULE_PRIO);
    return;
}


/********************************************************************
 ** ������ fhotelChange                                            **
 ** ���ܣ� �����ж�������Ӧ                                        **
 ** ������ 2012-10-30                                              ** 
 ** ������ ��                                                      **
 ** ���أ� ��                                                      **
 ** ����� ��                                                      **
 ** ���ߣ� zheng.xiao                                              ** 
 ********************************************************************/ 
void funInitHotel(void)
{
     hotelPnpInit();
     editDNSRedirectRule();
     registerForProfileUpdates(MIB_PROF_SYSTEM,(Pointer)NULL, (ProfileUpdateFnc)hotelChange);
     return;    
}
#endif


