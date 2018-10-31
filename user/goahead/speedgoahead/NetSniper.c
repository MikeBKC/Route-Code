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
#include "utt_conf_define.h"

#if (NET_SNIPER == FYES)
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter/nf_conntrack_common.h>
extern int  spdNfNlSock;
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static int uttNfNlNetSniperInit(const NetSniperProfile *prof){
	UttNlNfNetSniperConf nlConf;
	NetSniperEntryVal entryVal;
	memset(&nlConf,0,sizeof(nlConf));
	memset(&entryVal,0,sizeof(entryVal));
	entryVal.ctlTime = prof->ctlTime;
	entryVal.linearIntvl = prof->linearIntvl;
	entryVal.chkIntvl = prof->chkIntvl;
	entryVal.chkCnt = prof->chkCnt;
	nlConf.msgHead.nlType = UTT_NLNF_NET_SNIPER;
	nlConf.confType = UTT_NFCONF_INIT;
	nlConf.netSniperEntryVal = entryVal;
	uttNfNlSend(spdNfNlSock,(char*)&nlConf,sizeof(nlConf),0U,0U);
}
static int uttNfNlNetSniperFini(void){
	UttNlNfNetSniperConf nlConf;
	memset(&nlConf,0,sizeof(nlConf));
	nlConf.msgHead.nlType = UTT_NLNF_NET_SNIPER;
	nlConf.confType = UTT_NFCONF_FINI;
	uttNfNlSend(spdNfNlSock,(char*)&nlConf,sizeof(nlConf),0U,0U);
}

static void netSniperRuleUpdate(const NetSniperProfile *prof,char action){
    if(action == Iptables_DEL){
		doSystem("iptables -t mangle -F NET_SNIPER_CHAIN");
	}else{
		if(strlen(prof->ipGrpName)>0){
			doSystem("iptables -t mangle -%c NET_SNIPER_CHAIN -m uttdev --is-lan-in -m set ! --set %s src -j UTTNETSNIPER",action,prof->ipGrpName);
		}else{
			doSystem("iptables -t mangle -%c NET_SNIPER_CHAIN -m uttdev --is-lan-in  -j UTTNETSNIPER",action);
		}
	}
	doSystem("iptables -t mangle -%c PREROUTING -j NET_SNIPER_CHAIN",action);
	doSystem("iptables -t mangle -%c PREROUTING -p tcp --dport 80 -m set ! --set sys_lan_group dst  -j sniper_notice 1>/dev/null 2>&1",action);
	doSystem("iptables -t mangle -%c sniper_notice -m uttdev --is-lan-in -m uttsniper --is-need-notice  -j sniper_notice_chain",action);
	doSystem("iptables -t mangle -%c sniper_notice_chain  -m layer7 --l7proto httpGet -j  CONNMARK --set-mark %u/0x%x",action,NOTICE_SNIPER_CONNMARK,NOTICE_CONNMARK_MSK);
	doSystem("iptables -t mangle -%c POSTROUTING -p tcp --sport 80  -j sniper_notice_chain_r",action);
	doSystem("iptables -t mangle -%c sniper_notice_chain_r -m connmark --mark %u/0x%x -j HTTPREDIRECT --redirect-dir Notice4.asp --return c",action,NOTICE_SNIPER_CONNMARK,NOTICE_CONNMARK_MSK);
		
}

/*************************************************************
 * 函数: netSniperProfileChange
 * 功能:
 * 创建: 2013-10-15
 * 参数: void
 * 返回: void
 * 作者: chen.bin
 *
 ************************************************************/
static void netSniperProfileChange(Pointer tag,MibProfileType profileType,
		ProfChangeType changeType,Uint32 index,
		const NetSniperProfile *oldProfile,
		const NetSniperProfile *newProfile)
{

	switch(changeType)
	{
		case PROF_CHANGE_ADD:
		case PROF_CHANGE_EDIT:
			if((oldProfile->head.active == FUN_DISABLE)&&
					(newProfile->head.active == FUN_ENABLE)){
				uttNfNlNetSniperInit(newProfile);
				netSniperRuleUpdate(newProfile,Iptables_ADD);
			}else if((oldProfile->head.active == FUN_ENABLE)&&
					(newProfile->head.active == FUN_DISABLE)){
				netSniperRuleUpdate(oldProfile,Iptables_DEL);
				uttNfNlNetSniperFini();
			}else if((oldProfile->head.active == FUN_ENABLE)&&
					(newProfile->head.active == FUN_ENABLE)&&
					(strcmp(oldProfile->ipGrpName,newProfile->ipGrpName)!=0)
					){
				netSniperRuleUpdate(oldProfile,Iptables_DEL);
				netSniperRuleUpdate(newProfile,Iptables_ADD);
			}else if((oldProfile->head.active == FUN_ENABLE)&&
					(newProfile->head.active == FUN_ENABLE)&&
					(oldProfile->ctlTime != newProfile->ctlTime)
					){
				uttNfNlNetSniperFini();
				uttNfNlNetSniperInit(newProfile);
			}

		case PROF_CHANGE_DEL:
		case PROF_CHANGE_DELALL:
		default:break;
	}
	return;
}

/*************************************************************
 * 函数:initiNetSniper
 * 功能: 内网网络尖兵初始化函数
 * 创建: 2013-10-15
 * 参数: void
 * 返回: void
 * 作者: chen.bin
 *
 ************************************************************/
void initNetSniper(void)
{
	NetSniperProfile *prof = NULL;
	prof = (NetSniperProfile *)ProfGetInstPointByIndex(MIB_PROF_NET_SNIPER,0);
	doSystem("iptables -t mangle -N NET_SNIPER_CHAIN");
	doSystem("iptables -t mangle -N sniper_notice");
	doSystem("iptables -t mangle -N sniper_notice_chain");
	doSystem("iptables -t mangle -N sniper_notice_chain_r");
	if((prof !=NULL) && (prof->head.active == TRUE)){
		uttNfNlNetSniperInit(prof);
		netSniperRuleUpdate(prof,Iptables_ADD);
	}

}



/*************************************************************
 * 函数:funInitiNetSniper
 * 功能: 内网网络尖兵初始化函数
 * 创建: 2013-10-15
 * 参数: void
 * 返回: void
 * 作者: chen.bin
 *
 ************************************************************/
void funInitNetSniper(void)
{
	registerForProfileUpdates(MIB_PROF_NET_SNIPER,(Pointer)NULL,
			(ProfileUpdateFnc)netSniperProfileChange);
	initNetSniper();
}


#endif
#endif
