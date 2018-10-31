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
 ** 函数： fhotelChange                                            **
 ** 功能： 根据判断做出响应                                        **
 ** 创建： 2012-10-30                                              ** 
 ** 参数： 略                                                      **
 ** 返回： 略                                                      **
 ** 输出： 略                                                      **
 ** 作者： zheng.xiao                                              ** 
 ********************************************************************/ 
static void hotelChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    if (oldProfileDataPtr->hotelPnpEn != newProfileDataPtr->hotelPnpEn) 
    {/*不一样才触发*/
	hotelPnpSet();
#if (UTT_SMART_UI == FYES)
	/*mantis:13634
	 * 开机后WAN口up之前需要为即插即用添加网关，之前由易管理
	 * 开机时添加，现在开机不启动即插即用，改为配置改变时添加*/
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
 * 判断酒店即擦即用 是否开启
 * return ; 1 开启
 *          0 未开启
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
 * 酒店即插即用 生效相关
 */
static void hotelPnpSet(void)
{
#if (ARP_BINDS == FYES)
    /*主要为是否允许非绑定用户上网*/
    arpGlobalInit();
#endif
    /*dns 代理*/
    init_dnsmasq(1);
#if (NET_SECURITY == FYES)
    /*DdoS攻击防御*/
    netSecuritySet();
#endif
    /*其他相关功能生效*/
    hotelPnpInit();
    editDNSRedirectRule();
    return;
}

/**
 *  酒店功能 初始化
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
    /*arp代理*/
    doSystem("echo %d > /proc/sys/net/ipv4/conf/%s/proxy_arp", arpProxy, lanIf);
    #if 0
/*移至内核ipt_pre_routing_hook函数实现*/
    /*更改原来多线路mark匹配的地方*/
    doSystem("iptables -t mangle -%c PREROUTING ! -i %s -j MARK --and-mark 0x%x", iptAct2, lanIf, (~IPRULE_CONNMARK_MSK));
#endif
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
    /*lan口新起来的连接打标签（非lan网段的酒店用户）*/
    doSystem("iptables -t mangle -%c PREROUTING -i %s -m set ! --set %s src -m conntrack --ctstate NEW -j CONNMARK --set-mark %u/0x%x 1>/dev/null 2>&1",iptAct1, lanIf, SYS_LAN_RANGE_GROUP, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK);
#if 0
/*移至内核ipt_pre_routing_hook函数实现*/
    /*lan口进来的包。只有当源ip为酒店用户，且标签为酒店打上的，才不需ip rule匹配该标签。防止从lan进来的酒店用户包
     * 直接从lan返回*/    
    doSystem("iptables -t mangle -%c PREROUTING -i %s -m set ! --set %s src -m connmark --mark %u/0x%x  -j MARK --and-mark 0x%x 1>/dev/null 2>&1 ", iptAct1, lanIf, SYS_LAN_RANGE_GROUP, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK, (~HOTEL_CONNNARK_MSK));
#else
    /*酒店即擦即用接入用户 打上的mark 不应该 被ip rule 匹配
     * 防止酒店 即插即用用户 访问外网的包 直接从lan 回去
     * 同时 为保证 酒店用户能 正常访问内网服务器
     * 故 服务器回复 酒店用户的包中的mark 应该被ip rule 匹配*/
    doSystem("iptables -t mangle -%c PREROUTING -i %s -m set ! --set %s src  -j MARK --and-mark 0x%x 1>/dev/null 2>&1 ", iptAct1, lanIf, SYS_LAN_RANGE_GROUP, (~HOTEL_CONNNARK_MSK));
#endif
#else
#error ipset has been moved
#endif
#else
#error MULTI_LAN hash been moved
#endif

#if 0
/*移至内核ipt_pre_routing_hook函数实现*/
    /*wan口进来的。非多线路打上的mark才匹配 相关 ip rule.防止wan进来的包直接从wan出去*/
    doSystem("iptables -t mangle -%c PREROUTING  ! -i %s -m connmark ! --mark %u/0x%x -j MARK --and-mark 0x%x  1>/dev/null 2>&1",iptAct1, lanIf, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK, (~POLICYROUTE_CONNMARK_MSK));
#endif

     /*路由相关*/
    doSystem("ip route %s table %d dev %s; ip rule %s fwmark 0x%x/0x%x lookup %d prio %d; ip route flush cache 1>/dev/null 2>&1", iprAct, HOTEL_PNP_ROUTE_TABLE, lanIf, iprAct, HOTEL_PNP_CONNMARK, HOTEL_CONNNARK_MSK, HOTEL_PNP_ROUTE_TABLE, HOTEL_PNP_ROUTE_RULE_PRIO);
    return;
}


/********************************************************************
 ** 函数： fhotelChange                                            **
 ** 功能： 根据判断做出响应                                        **
 ** 创建： 2012-10-30                                              ** 
 ** 参数： 略                                                      **
 ** 返回： 略                                                      **
 ** 输出： 略                                                      **
 ** 作者： zheng.xiao                                              ** 
 ********************************************************************/ 
void funInitHotel(void)
{
     hotelPnpInit();
     editDNSRedirectRule();
     registerForProfileUpdates(MIB_PROF_SYSTEM,(Pointer)NULL, (ProfileUpdateFnc)hotelChange);
     return;    
}
#endif


