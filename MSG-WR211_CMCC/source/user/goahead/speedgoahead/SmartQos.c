
#include <stdlib.h>
#include <stdio.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"
#if (SMART_QOS_FUN == FYES)
#include    <linux/netfilter/nf_conntrack_common.h>

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <linux/version.h>
#endif
#define PROC_DEV_CONF "/proc/sys/net/ipv4/conf/"
void wanClearSmartQosSniff(int wanIndex)
{
    char *wanName = NULL;
    wanName = getWanIfNamePPP(wanIndex);
    if(wanName != NULL) {
        doSystem("tc qdisc del dev %s root >null 2>&1", wanName);
        doSystem("echo 0 > "PROC_DEV_CONF"%s/bandwith_enable", wanName);
    }
    doSystem("iptables -t mangle -F wan%d_Qos", wanIndex);
}


void webRateLimitClear(int wanIndex){
	doSystem("iptables -t mangle -F limit%d_Qos",wanIndex);
	doSystem("iptables -t mangle -D FORWARD -j limit%d_Qos >/dev/null 2>&1", wanIndex);
	doSystem("iptables -t mangle -X limit%d_Qos 1>/dev/null 2>&1", wanIndex);
}
void webRateLimit(InterfaceProfile *profIf,int  wanIndex,char action){
    char upRate[32],downRate[32];
    char *wanName = NULL;
    wanName = getWanIfNamePPP(wanIndex);
	doSystem("iptables -t mangle -N limit%d_Qos 1>/dev/null 2>&1", wanIndex);
	doSystem("iptables -t mangle -I FORWARD 1 -j limit%d_Qos >/dev/null 2>&1", wanIndex);
    if(profIf->txRate>0){
		//sprintf(upRate,"%.0u",profIf->txRate*1024*70/(8*32*100));防止当profIf->rxRate很大时产生越位改用下面语句
		sprintf(upRate,"%.0u",profIf->txRate*28/10); //
		/*up*/
		doSystem("iptables -t mangle -%c limit%d_Qos -p tcp --dport 80 -o %s -m connmark --mark %u/0x%x -m uttdev --lan-to-wan -m iprange --src-range 0.0.0.0-255.255.255.255 -m limit --limit %s/s --limit-burst %s -j RETURN",action,wanIndex,wanName,QOS_WEB_CONNMARK, QOS_CONNMARK_MSK, upRate,upRate);
		doSystem("iptables -t mangle -%c limit%d_Qos -p tcp --dport 80 -o %s -m connmark --mark %u/0x%x -m uttdev --lan-to-wan -m iprange --src-range 0.0.0.0-255.255.255.255 -j DROP",action,wanIndex,wanName,QOS_WEB_CONNMARK, QOS_CONNMARK_MSK);
	}
	/*down*/
	if(profIf->rxRate>0){
		//sprintf(downRate,"%.0u",profIf->rxRate*1024*70/(8*32*100));
		sprintf(downRate,"%.0u",profIf->rxRate*28/10);//防止当profIf->rxRate很大时产生越位
		doSystem("iptables -t mangle -%c limit%d_Qos -p tcp --sport 80 -i %s -m connmark --mark %u/0x%x -m uttdev --wan-to-lan -m iprange --dst-range 0.0.0.0-255.255.255.255 -m limit --limit %s/s --limit-burst %s -j RETURN",action,wanIndex,wanName,QOS_WEB_CONNMARK, QOS_CONNMARK_MSK,downRate,downRate);
		doSystem("iptables -t mangle -%c limit%d_Qos -p tcp --sport 80 -i %s -m connmark --mark %u/0x%x -m uttdev --wan-to-lan -m iprange --dst-range 0.0.0.0-255.255.255.255 -j DROP",action,wanIndex,wanName,QOS_WEB_CONNMARK, QOS_CONNMARK_MSK);
	}
}

extern void webRateLimitInit(int wanIndex){

	MibProfileType mibType = MIB_PROF_INTERFACE;
	InterfaceProfile *prof = NULL;
	prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
	if(prof!=NULL&&prof->smartQosEn==FUN_ENABLE){
		webRateLimitClear(wanIndex);
		webRateLimit(prof,wanIndex,Iptables_ADD);
	}
}

void wanInitSmartQosSniff(int wanIndex)
{
    char *wanName = NULL;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);

        if(profIf->smartQosEn == FUN_ENABLE && profIf->txRate != 0
            && profIf->rxRate != 0) {
	    wanName = getWanIfNamePPP(wanIndex);
	    doSystem("tc qdisc add dev %s root handle 1: prio", wanName);
	    doSystem("tc filter add dev %s parent 1: protocol ip handle %u fw classid 1:1", wanName, QOS_WEB_CONNMARK);

        doSystem("iptables -t mangle -F wan%d_Qos", wanIndex);
#if 0
        if(profIf->rxRate < 30000)
            doSystem("iptables -t mangle -A wan%d_Qos -o %s -p tcp -j TCPWINDOWS", wanIndex, wanName);
#endif
#if 0
        doSystem("tc qdisc add dev %s root handle 1: prio", wanName);
#endif
        doSystem("tc qdisc add dev %s parent 1:1 handle 10: pfifo", wanName);
        doSystem("echo 10 > "PROC_DEV_CONF"%s/bandwith_queueL", wanName);
        doSystem("echo 3 > "PROC_DEV_CONF"%s/bandwith_credit", wanName);
        doSystem("echo 50 > "PROC_DEV_CONF"%s/bandwith_interval", wanName);
        doSystem("echo %d > "PROC_DEV_CONF"%s/bandwith_down", (profIf->rxRate)*128, wanName);
        doSystem("echo %d > "PROC_DEV_CONF"%s/bandwith_up", (profIf->txRate)*128, wanName);
        doSystem("echo 80 > "PROC_DEV_CONF"%s/bandwith_down_ratio", wanName);
        doSystem("echo 80 > "PROC_DEV_CONF"%s/bandwith_up_ratio", wanName);
        doSystem("echo 1 > "PROC_DEV_CONF"%s/bandwith_enable", wanName);
        doSystem("echo 0 > "PROC_DEV_CONF"%s/bandwith_delay", wanName);
        doSystem("echo 20000 > "PROC_DEV_CONF"%s/bandwith_sdown", wanName);
        doSystem("echo 8000 > "PROC_DEV_CONF"%s/bandwith_sup", wanName);
        doSystem("tc qdisc add dev %s parent 1:2 handle 20: smartQos", wanName);
    }
}
#if (GAME_BOOST == FYES)
extern void gameBoostRuleclear(void);
extern void gameBoostRuleInit(void);
#endif

void smartQosProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const InterfaceProfile *oldProf, const InterfaceProfile *newProf)
{
    char *wanName = NULL;
	wanName = getWanIfNamePPP(index);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    if (index == 1) {/*only once*/
	if ((oldProf->smartQosEn == FUN_DISABLE) && (newProf->smartQosEn == FUN_ENABLE)) {/*由关闭到开启*/
#if LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 36)
	    doSystem("insmod /lib/modules/2.6.36/kernel/net/sched/sch_prio.ko;\
			insmod /lib/modules/2.6.36/kernel/net/sched/sch_utt.ko");
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 21)
	    doSystem("insmod /lib/modules/2.6.21/kernel/net/sched/sch_prio.ko;\
			insmod /lib/modules/2.6.21/kernel/net/sched/sch_utt.ko");
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 30)
	    doSystem("insmod /lib/modules/2.6.30.8/kernel/net/sched/sch_prio.ko;\
			insmod /lib/modules/2.6.30.8/kernel/net/sched/sch_utt.ko");
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 35)
	    doSystem("insmod /lib/modules/2.6.35/kernel/net/sched/sch_prio.ko;\
			insmod /lib/modules/2.6.35/kernel/net/sched/sch_utt.ko");
#else
#error
#endif
	}
    }
#endif
	if((oldProf->smartQosEn == FUN_ENABLE)&&(newProf->smartQosEn == FUN_DISABLE)){
		webRateLimitClear(index);
#if (GAME_BOOST == FYES)
		if(index==1){/*only once*/
			gameBoostRuleclear();
		}
#endif
	}else if(newProf->smartQosEn == FUN_ENABLE){
		 webRateLimitClear(index);
		 webRateLimit(newProf,index,Iptables_ADD);
#if (GAME_BOOST == FYES)
		if(index==1){
			gameBoostRuleclear();
			gameBoostRuleInit();
		}
#endif
	}	
    if(oldProf->smartQosEn != newProf->smartQosEn 
            || oldProf->txRate != newProf->txRate
            || oldProf->rxRate != newProf->rxRate) {
        wanClearSmartQosSniff(index);
        if(newProf->smartQosEn == FUN_ENABLE && newProf->head.active == 1
                && _getWanLinkState(index) == 1) {
            wanInitSmartQosSniff(index);
        } else {
            wanName = getWanIfNamePPP(index);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
            doSystem("tc qdisc add dev %s root handle 1: prio", wanName);
#endif
        }
    }
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    if (index == 1) {/*only once*/
	if((oldProf->smartQosEn == FUN_ENABLE) && (newProf->smartQosEn == FUN_DISABLE)){/*由开启到关闭*/
	    doSystem("rmmod sch_utt;rmmod sch_prio");
	}
    }
#endif
}

void funInitSmartQos()
{
    int i = 0;
    Uint32 wanCount = 0;
    wanCount = getWanSlotCount();

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    InterfaceProfile *prof;

    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    if((prof->smartQosEn == FUN_ENABLE)){/*由开启到关闭*/
#if LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 36)
	doSystem("insmod /lib/modules/2.6.36/kernel/net/sched/sch_prio.ko;\
		    insmod /lib/modules/2.6.36/kernel/net/sched/sch_utt.ko");
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 21)
	doSystem("insmod /lib/modules/2.6.21/kernel/net/sched/sch_prio.ko;\
		    insmod /lib/modules/2.6.21/kernel/net/sched/sch_utt.ko");
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 30)
	doSystem("insmod /lib/modules/2.6.30.8/kernel/net/sched/sch_prio.ko;\
		    insmod /lib/modules/2.6.30.8/kernel/net/sched/sch_utt.ko");
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 35)
	doSystem("insmod /lib/modules/2.6.35/kernel/net/sched/sch_prio.ko;\
		    insmod /lib/modules/2.6.35/kernel/net/sched/sch_utt.ko");
#else
#error
#endif
    }
#endif
#if 0
    /*移到内核 ipt_post_routing_hook实现*/
    /*tc 在postrouting之后。但无法匹配掩码。故这里需把其他位置零。
     * 注意：qos的connmark标签，在内核实现*/
    doSystem("iptables -t mangle -A POSTROUTING -j MARK --and-mark 0x%x", QOS_CONNMARK_MSK);
#endif
    for(i = 1; i <= wanCount; i++) {
        doSystem("iptables -t mangle -N wan%d_Qos 1>/dev/null 2>&1", i);
#if 0
	/* wan_Qos链目前没有用 暂时把此规则注释掉 */
        doSystem("iptables -t mangle -I FORWARD 1 -j wan%d_Qos >/dev/null 2>&1", i);
#endif
    }
    doSystem("qos.sh");
    registerForProfileUpdates(MIB_PROF_INTERFACE, NULL, (ProfileUpdateFnc)smartQosProfileChange);
}
#endif
