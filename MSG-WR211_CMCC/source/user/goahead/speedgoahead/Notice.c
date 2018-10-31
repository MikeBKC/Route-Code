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

#if (NOTICE == FYES)
#include    <linux/netfilter/nf_conntrack_common.h>
#include "utt_conf_define.h"
#if (PPPOE_SERVER == FYES)
extern void getPoePoolFT(PPPoESrvGloProfile *prof, char** ipFrom, char** ipTo);
#endif
extern void renewNTManglePREROUTINGJumpRule(void);
static void noticeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const NoticeProfile *oldProfileDataPtr, const NoticeProfile *newProfileDataPtr );
static void noticeInit(void);
static void editNtCapRule(NoticeProfile *prof, char action);
static void editCapRule(Uint32 profIndex, NoticeProfile *prof, char action);
static void noticeCapRuleInit(void);
static void flushOkGrpTask(NoticeProfile *prof, char action);
extern void DnsNoticeChange(int action);

#if (PPPOE_ACCOUNT == FYES)
static void editPoeCapRule(NoticeProfile *prof, char action);
static void editPoeExpiredCap(NoticeProfile *prof, char action);
static void editPoeExpiringCap(NoticeProfile *prof, char action);
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
#endif
/**
 * 通告配置变化 生效函数
 */
static void noticeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const NoticeProfile *oldProfileDataPtr, const NoticeProfile *newProfileDataPtr )
{
    NoticeProfile *oldProf = (NoticeProfile *)oldProfileDataPtr;
    NoticeProfile *newProf = (NoticeProfile *)newProfileDataPtr;
    char ip[16] = {0};
    /* 获得lan口IP */
    getIfIp((char *)getLanIfName(), ip);

    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    editCapRule(index, oldProf, Iptables_DEL);/*先删除之前的*/
	    editCapRule(index, newProf, Iptables_ADD);/*再添加新的规则*/
	    if (oldProf->head.active != newProf->head.active) {
		/* 保证顺序 */
	        renewNTManglePREROUTINGJumpRule();

		if (newProf->head.active == TRUE) { /* 开启 */
		    if (index == 0) { /* 日常通告 */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -A POSTROUTING -p tcp -m set --set %s src -m set ! --set %s dst -j %s",SYS_HTTP_PORT, NOTICE_OK_GRP, NOTICE_CHAIN_R);
#else
			doSystem("iptables -t mangle -A POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s", NOTICE_OK_GRP, NOTICE_CHAIN_R);
#endif
		    } 
#if (PPPOE_ACCOUNT == FYES)
		    else if (index == 1) { /* 过期通告 */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -I POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -m set ! --set %s dst -j %s",SYS_HTTP_PORT, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -I POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -j %s",SYS_HTTP_PORT, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#else
			doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#endif
		    }
#endif
		    else if (index == 2) { /*域名过滤通告*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
			sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
			DnsNoticeChange(1);
#endif
		    }
		} else { /* 关闭 */
		    if (index == 0) { /* 日常通告 */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, NOTICE_OK_GRP, NOTICE_CHAIN_R);
#else
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", NOTICE_OK_GRP, NOTICE_CHAIN_R);
#endif
		    } 
#if (PPPOE_ACCOUNT == FYES)
		    else if (index == 1) { /* 过期通告 */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#else
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#endif
		    }
#endif
		    else if (index == 2) { /*域名过滤通告*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
			sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
			DnsNoticeChange(0);
#endif
		    }
		}
	    }
	    break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:
            break;
    };
    return;
}

#if (PPPOE_ACCOUNT == FYES)
#if (MULTI_LAN == FYES)
extern void poeNtCapChainAOrD(char action);
#else
extern void poeNtCapChainAOrD(char* ipstr, char action);
#endif
#endif
/*
 * 重建mangle PREROUTING中通告相关规则，日常和Poe通告
 * Poe通告优先级高于日常通告，有顺序关系
 */
extern void renewNTManglePREROUTINGJumpRule(void) {
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
    char ip[16] = {0};
    /* 获得lan口IP */
    getIfIp((char *)getLanIfName(), ip);

#if (PPPOE_ACCOUNT == FYES)
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 1);
    if ((prof) && (prof->head.active == TRUE)) { /* poe过期通告 */
#if (MULTI_LAN == FYES)
	poeNtCapChainAOrD(Iptables_DEL);
	poeNtCapChainAOrD(Iptables_ADD);
#else
	poeNtCapChainAOrD(ip, Iptables_DEL);
	poeNtCapChainAOrD(ip, Iptables_ADD);
#endif
    } else {
#if (MULTI_LAN == FYES)
	poeNtCapChainAOrD(Iptables_DEL);
#else
	poeNtCapChainAOrD(ip, Iptables_DEL);
#endif
    }
#endif
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 0);
    if ((prof) && (prof->head.active == TRUE)) {
#if (MULTI_LAN == FYES)
	/*日常事务通告初始化*/
#if (IP_GRP == FYES)
	doSystem("iptables -t mangle -D PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", SYS_LAN_GROUP, PRE_NOTICE);
	doSystem("iptables -t mangle -A PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s", SYS_LAN_GROUP, PRE_NOTICE);
#else
#error ipset has been removed
#endif
#else
	/*日常事务通告初始化*/
	doSystem("iptables -t mangle -D PREROUTING -p tcp --dport 80 ! -d %s -j %s 1>/dev/null 2>&1", ip, PRE_NOTICE);
	doSystem("iptables -t mangle -A PREROUTING -p tcp --dport 80 ! -d %s -j %s", ip, PRE_NOTICE);
#endif
    } else {
#if (MULTI_LAN == FYES)
#if (IP_GRP == FYES)
	doSystem("iptables -t mangle -D PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", SYS_LAN_GROUP, PRE_NOTICE);
#else
#error ipset has been removed
#endif
#else
	doSystem("iptables -t mangle -D PREROUTING -p tcp --dport 80 ! -d %s -j %s 1>/dev/null 2>&1", ip, PRE_NOTICE);
#endif
    }
    return;
}

/*通告初始化函数
 * 功能：先清空现有规则 链表 ， 再重新添加
 * bhou*/
static void noticeInit(void)
{
    MibProfileType mibTypeNT = MIB_PROF_NOTICE;
    NoticeProfile *profNT = NULL;

#if 0
    /*清除规则*/
    doSystem("iptables -t mangle -D PREROUTING -i %s -d ! %s -j %s 1>/dev/null 2>&1", getLanIfName(), ip, NOTICE_CHAIN);/*lan口进来的才通告*/
    /*清空链表内容*/
    /*删除链*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,NOTICE_CHAIN);
#endif

    /*再添加ipset组*/
    /*此部分必须在iptables规则清除的情况下进行。否则被引用的ipset组无法删除*/
    ipsetDelGrp(NOTICE_OK_GRP, TRUE);/*删除已通告用户组*/
    noticeOkGrpAdd(NOTICE_OK_GRP);/*再添加新组*/
#if 0 /* NOTICE_WEB_GRP不需要 */
    ipsetDelGrp(NOTICE_WEB_GRP, TRUE);/*删除需通告用户组*/
    noticeWebGrpAdd(NOTICE_WEB_GRP);/*添加需通告用户组*/
#endif


    /*增加通告规则链*/
    doSystem("iptables -t mangle -N %s" ,NOTICE_CHAIN);
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",NOTICE_CHAIN);

    /*增加PPPOE拨号上网和一般上网规则链*/
    doSystem("iptables -t mangle -N  %s", PRE_NOTICE);
    doSystem("iptables -t mangle -F  %s >/dev/null 2>&1", PRE_NOTICE);
#if 0
    /*所有的目的ip地址不是lan口IP地址的都加入链中*/
    doSystem("iptables -t mangle -A PREROUTING -d ! %s -j %s", ip, PRE_NOTICE);
#endif
#if (PPPOE_ACCOUNT == FYES)
    /*将过期通告优先级高于日常通告*/
#if (CACHE_SERVER==FYES)
    doSystem("iptables -t mangle -I %s -m connmark --mark %u/0x%x -j RETURN", PRE_NOTICE, POE_NT_R,NOTICE_POE_CONNMARK,NOTICE_CONNMARK_MSK);
#else
    doSystem("iptables -t mangle -I %s -m set --set %s src,src,dst -j RETURN", PRE_NOTICE, POE_NT_R);
#endif
#endif
    /*lan口进来的且不在ok组中的加入链中*/
    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -m set ! --set %s src -j %s", PRE_NOTICE, NOTICE_OK_GRP, NOTICE_CHAIN);

    doSystem("cron-init.sh del \"ipset -F %s\"", NOTICE_OK_GRP);/*清空定时请ok组的任务*/

#if 1
#if 0
    /*清除规则*/
    doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", NOTICE_CHAIN_R);
    /*删除链*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,NOTICE_CHAIN_R);
#endif

    /* 增加http redirect规则 */
    doSystem("iptables -t mangle -N %s" ,NOTICE_CHAIN_R);
    /*清空链表内容*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",NOTICE_CHAIN_R);

#if 0
    doSystem("iptables -t mangle -A POSTROUTING -j %s", NOTICE_CHAIN_R);
#endif
#if 0
    doSystem("cron-init.sh del \"ipset -F %s\"", NOTICE_R_GRP);/*清空定时请ok组的任务*/
#endif
#endif
    /*初始化需通告capture规则*/
    noticeCapRuleInit();
    /* 
     * 重建mangle PREROUTING中通告规则，需要保证顺序 
     * mangle PREROUTING中poe_nt_cap_chain在此初始化,新建在initInternet中
     */
    renewNTManglePREROUTINGJumpRule();

    profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibTypeNT, 0);
    if ((profNT) && (profNT->head.active == TRUE)) {
	/*清除规则*/
#if (CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, NOTICE_OK_GRP, NOTICE_CHAIN_R);
	doSystem("iptables -t mangle -A POSTROUTING -p tcp -m set --set %s src -m set ! --set %s dst -j %s",SYS_HTTP_PORT, NOTICE_OK_GRP, NOTICE_CHAIN_R);
#else
	doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", NOTICE_OK_GRP, NOTICE_CHAIN_R);
	doSystem("iptables -t mangle -A POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s", NOTICE_OK_GRP, NOTICE_CHAIN_R);
#endif
    }

    return;
}

/**
 * 定时清空通告ok组。即在每次通告的起始时间清空ok组
 */
static void flushOkGrpTask(NoticeProfile *prof, char action)
{
    char crondAct[8];
    memset(crondAct, 0, sizeof(crondAct));

    if(action == Iptables_ADD)
    {/*添加计划任务*/
	strcpy(crondAct, "add");
    }
    else if(action == Iptables_DEL)
    {/*删除计划任务*/
	strcpy(crondAct, "del");
    }
    else
    {/*什么都不做*/
	strcpy(crondAct, "exit");
    }

    /*添加计划任务定时清空ipset ok组*/
    doSystem("cron-init.sh %s \"%s * * %s ipset -F %s\"",
	    crondAct,
	    getTimeForCrond((Uchar*)prof->timestart),
	    getWeekForCrond(prof->day),
	    NOTICE_OK_GRP
	    );
#if 0
    /* add by dhyu */
    /* 添加清除http get记录组任务 */
    doSystem("cron-init.sh %s \"%s * * %s ipset -F %s\"",
	    crondAct,
	    getTimeForCrond(prof->timestart),
	    getWeekForCrond(prof->day),
	    NOTICE_R_GRP
	    );
#endif

    return;
}
/**
 * 通告capture规则变换
 * action : Iptables_ADD for add , Iptables_DEL for delete
 * bhou
 * */
static void editNtCapRule(NoticeProfile *prof, char action)
{
    char *ipstr = NULL;
    char *timestr = NULL;
    char *daystr = NULL;
    char *datestr = NULL;
#if 1
    char ipFrom[16] = {0};
    char ipTo[16] = {0};
    struct in_addr addr;
#endif

    if(prof->head.active == TRUE)/*生效*/
    {
	ipstr = getIpRangeForIptables(prof->ipstart, prof->ipend, 1);/*iptables ip range字串*/
	timestr = getTimeForIptables(prof->timestart, prof->timestop);/*iptables 时间字串*/
	daystr = getDayForIptables(prof->day);/*iptables天字串*/
	datestr = getDateForIptables(prof->datestart, prof->datestop);/*iptables 日期字段*/


#if 0
	/*需要通告的用户。http请求指向 设备webserver 服务器*/

	/*这里能在规则中实现的记录内容到ipset组，不要通过命令实现，因规则在内核运转。会较快*/
	doSystem("iptables -t mangle -%c %s %s -p tcp --dport 80 %s %s -j SET --add-set %s src",action, NOTICE_CHAIN, ipstr, timestr, daystr , NOTICE_WEB_GRP);/*符合规则。记录ip地址到需通告用户组*/
	doSystem("iptables -t mangle -%c %s %s -p tcp --dport 80 %s %s -j REDIRECT --to-ports 80",action, NOTICE_CHAIN, ipstr, timestr, daystr);/*符合负责http包指向本地web服务器*/
#else
	if (action == Iptables_ADD) {
	    /* 新建http redirect 记录组 */
	    addr.s_addr = prof->ipstart;
	    strcpy(ipFrom, inet_ntoa(addr));
	    addr.s_addr = prof->ipend;
	    strcpy(ipTo, inet_ntoa(addr));
	    noticeRGrpAdd(NOTICE_R_GRP, ipFrom, ipTo);
	}
	/*需要通告的用户。http请求指向 设备webserver 服务器*/

#if 0 /* 不需要NOTICE_WEB_GRP组来记录了 */
	/*这里能在规则中实现的记录内容到ipset组，不要通过命令实现，因规则在内核运转。会较快*/
	doSystem("iptables -t mangle -%c %s %s -p tcp --dport 80 %s %s -m layer7 --l7proto httpGet -j SET --add-set %s src",action, NOTICE_CHAIN, ipstr, timestr, daystr , NOTICE_WEB_GRP);/*符合规则。记录ip地址到需通告用户组*/
#endif
#if (CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -%c %s %s %s %s %s -m layer7 --l7proto httpGet -j CONNMARK --set-mark %u/0x%x",action, NOTICE_CHAIN, ipstr, timestr, daystr, datestr,NOTICE_GRP_CONNMARK,NOTICE_CONNMARK_MSK);/*符合http get规则的连接，记录到notice_web_r组中 */
#else
	doSystem("iptables -t mangle -%c %s %s %s %s %s -m layer7 --l7proto httpGet -j SET --add-set %s src,src,dst",action, NOTICE_CHAIN, ipstr, timestr, daystr, datestr, NOTICE_R_GRP);/*符合http get规则的连接，记录到notice_web_r组中 */
#endif


	/* notice_chain_r链中增加规则 */
#if (CACHE_SERVER==FYES)
#if 1
	doSystem("iptables -t mangle -%c %s -m connmark --mark %u/0x%x -j HTTPREDIRECT --redirect-dir %s --return c", action, NOTICE_CHAIN_R,NOTICE_GRP_CONNMARK,NOTICE_CONNMARK_MSK,NOTICE_HTML_1_NAME);
#else
	doSystem("iptables -t mangle -%c %s -m set --set %s dst,dst,src -j HTTPREDIRECT --redirect-dir %s", action, NOTICE_CHAIN_R, NOTICE_R_GRP, NOTICE_HTML_1_NAME);
#endif
#else
#if 1
	doSystem("iptables -t mangle -%c %s -m set --set %s dst,dst,src -j HTTPREDIRECT --redirect-dir %s --return c", action, NOTICE_CHAIN_R, NOTICE_R_GRP, NOTICE_HTML_1_NAME);
#else
	doSystem("iptables -t mangle -%c %s -m set --set %s dst,dst,src -j HTTPREDIRECT --redirect-dir %s", action, NOTICE_CHAIN_R, NOTICE_R_GRP, NOTICE_HTML_1_NAME);
#endif
#endif

	if (action == Iptables_DEL) {
	    /* 删除组 */
	    ipsetDelGrp(NOTICE_R_GRP, TRUE);
	}
#endif

	/*如果一次添加多条任务。这里可以优化下。只让最后一此添加重启crond*/
	flushOkGrpTask(prof, action);/*定时清空ok组*/
    }

    return;
}
/**
 * 通告capture规则变换
 * action : Iptables_ADD for add , Iptables_DEL for delete
 * bhou
 * */
static void editCapRule(Uint32 profIndex, NoticeProfile *prof, char action)
{
    if(profIndex == 0u)
    {/*日常通告*/
	editNtCapRule(prof, action);
    }
#if (PPPOE_ACCOUNT == FYES)
    else
    {
	if(profIndex == 1u)
	{/*pppoe通告*/
	    editPoeCapRule(prof, action);
	}
    }
#endif
    return;
}

#if (PPPOE_ACCOUNT == FYES)
/*将过期规则
 * */
static void editPoeExpiringCap(NoticeProfile *prof, char action)
{

    /*出去的http包记录*/
#if (CACHE_SERVER==FYES)
    doSystem("iptables -t mangle -%c %s -m layer7 --l7proto httpGet -j CONNMARK --set-mark %u/0x%x",action, POE_NT_CAP_CHAIN,NOTICE_POE_CONNMARK,NOTICE_CONNMARK_MSK);/*符合http get规则的连接，记录到组中 */
#else
    doSystem("iptables -t mangle -%c %s -m layer7 --l7proto httpGet -j SET --add-set %s src,src,dst",action, POE_NT_CAP_CHAIN, POE_NT_R);/*符合http get规则的连接，记录到组中 */
#endif
    /*修改http回复包指向webserver*/
#if (CACHE_SERVER==FYES)
#if 1
    doSystem("iptables -t mangle -%c %s -m connmark --mark %u/0x%x -j HTTPREDIRECT --redirect-dir %s --return c", action, POE_NT_R_CHAIN, NOTICE_POE_CONNMARK,NOTICE_CONNMARK_MSK, NOTICE_HTML_2_NAME);
#else
    doSystem("iptables -t mangle -%c %s -m set --set %s dst,dst,src -j HTTPREDIRECT --redirect-dir %s", action, POE_NT_R_CHAIN, POE_NT_R, NOTICE_HTML_2_NAME);
#endif
#else
#if 1
    doSystem("iptables -t mangle -%c %s -m set --set %s dst,dst,src -j HTTPREDIRECT --redirect-dir %s --return c", action, POE_NT_R_CHAIN, POE_NT_R, NOTICE_HTML_2_NAME);
#else
    doSystem("iptables -t mangle -%c %s -m set --set %s dst,dst,src -j HTTPREDIRECT --redirect-dir %s", action, POE_NT_R_CHAIN, POE_NT_R, NOTICE_HTML_2_NAME);
#endif
#endif

    return;
}

/*过期规则
 * */
static void editPoeExpiredCap(NoticeProfile *prof, char action)
{
    /*修改http回复包指向webserver*/
#if 1
    doSystem("iptables -t mangle -%c %s -j HTTPREDIRECT --redirect-dir %s --return d", action, POE_NT_DROP_R_CHAIN, NOTICE_HTML_2_NAME);
#else
    doSystem("iptables -t mangle -%c %s -p tcp ! --tcp-flags SYN SYN -j HTTPREDIRECT --redirect-dir %s", action, POE_NT_DROP_R_CHAIN, NOTICE_HTML_2_NAME);
#endif
    if(action == Iptables_ADD)
    {
	action = Iptables_INS;/*允许规则放在丢弃规则的最前面*/
    }

    /* 非http且非dns的包，被直接丢弃
     * 80端口的所有包被指向 web server
     * dns包允许，是为了让浏览器可以进行http请求
     * */
    /*以下3条规则的顺序不能改变*/

    /*以下规则允许http请求。优先级最低*/
    doSystem("iptables -t mangle -%c %s -p tcp --dport 80 -j RETURN", action, POE_DROP_CHAIN);	
    /*以下这条优先级高于允许http包
     * 为禁止非get类型的http包。该规则只匹配含有html内容的http包
     * 只允许get是为了防止 部分非法http包。导致goahead 忙的情况出现
     * 如bug 9270*/

    /*丢弃非get包。此规则在get包接受之下，但l7只匹配一次。故这里会丢弃非get包。但不会丢弃三次握手*/
    doSystem("iptables -t mangle -%c %s -p tcp --dport 80 -m layer7 --l7proto nonZeroLen -j DROP", action, POE_DROP_CHAIN);
   /*接受get包*/
    doSystem("iptables -t mangle -%c %s -p tcp --dport 80 -m layer7 --l7proto httpGet -j RETURN", action, POE_DROP_CHAIN);

    /*以下这条优先级最高。为允许dns*/
    doSystem("iptables -t mangle -%c %s -p udp --dport 53 -j RETURN", action, POE_DROP_CHAIN);	
    /*因nat表中无DROP规则，故以上drop放于 PRE_DROP_CHAIN*/
    return;
}
/**
 * pppoe计费通告规则
 * action : Iptables_ADD for add , Iptables_DEL for delete
 * bhou
 * */
static void editPoeCapRule(NoticeProfile *prof, char action)
{
    if(prof->head.active == TRUE)/*生效*/
    {
	editPoeExpiredCap(prof, action);/*过期规则*/
	if(prof->remainDays > 0)
	{
	    editPoeExpiringCap(prof, action);/*只有当将过期剩余天数大于0，则会有将过期通告*/
	}
    }

    return;
}
#endif
/**
 * 初始化通告 capture规则。符合该规则的用户被通告
 * bhou 
 */
static void noticeCapRuleInit(void)
{
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
#if 1 
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);

    /*遍历添加规则*/
    for(i = min; i < max; i++) {
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0) {
	    /*添加一个规则*/
	    editCapRule(i, prof,Iptables_ADD);

	}
    }
#else
    /* by dhyu, 跳过PPPoE计费通告 */
    /* 获取日常事务通告 */
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL && ProfInstIsFree(prof) == 0 && prof->head.active == TRUE) {
        /*添加一个规则*/
        editCapRule(prof,Iptables_ADD);
    }
#endif
    return;
}
#if 0
#if (CACHE_SERVER == FYES)
/*
 *
 *缓存服务配置发生变化时需要调整通告规则中的sport 
*/
void noticeUpdateSport(const CacheServerProfile *oldProfile){
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *profNT = NULL,*profPOE = NULL;
	profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibType,0);
	profPOE = (NoticeProfile*)ProfGetInstPointByIndex(mibType,1);
	Uint32 sport = 80;
    CacheServerProfile *profCacheServer = NULL;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(MIB_PROF_CACHE_SERVER, 0);
	if(profCacheServer->cacheServerEnable == FUN_ENABLE){
		sport = profCacheServer->cacheServerPort;
	}
		
	if((profNT!=NULL)&&(profNT->head.active==1U)){
		doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1 ",NOTICE_OK_GRP, NOTICE_CHAIN_R);
		doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport %u -m set ! --set %s dst -j %s 1>/dev/null 2>&1 ",oldProfile->cacheServerPort,NOTICE_OK_GRP, NOTICE_CHAIN_R);
		doSystem("iptables -t mangle -A POSTROUTING -p tcp --sport %u -m set ! --set %s dst -j %s",sport,NOTICE_OK_GRP, NOTICE_CHAIN_R);
	}

#if (PPPOE_ACCOUNT == FYES)
	if((profPOE!=NULL)&&(profPOE->head.active==1U)){
		doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
		doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
		doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport %u -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1",oldProfile->cacheServerPort, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
		doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport %u -m set --set %s dst -j %s 1>/dev/null 2>&1",oldProfile->cacheServerPort, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
		doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport %u -m set --set %s dst -m set ! --set %s dst -j %s",sport , POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
		doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport %u -m set --set %s dst -j %s",sport, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
	}
#endif
}
#endif
#endif
/*
 * 通告功能 初始化入口函数
 */
void funInitNotice(void)
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    int min = 0, max = 0,i;
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    sendMsgToKernel(profChangeNfNlSock, mibType,i, (Pointer)NULL,0,PROF_CHANGE_ADD);
	}
    }
#endif
    registerForProfileUpdates(MIB_PROF_NOTICE, (Pointer)NULL, (ProfileUpdateFnc)noticeProfileChange);/*触发函数*/

	ipsetDelGrp(SYS_HTTP_PORT,TRUE);
	httpPortGrpAdd(SYS_HTTP_PORT);
	Uint32 sport = 80;
#if (CACHE_SERVER == FYES)
    CacheServerProfile *profCacheServer = NULL;
    MibProfileType mibType = MIB_PROF_CACHE_SERVER;
    profCacheServer = (CacheServerProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(profCacheServer->cacheServerEnable == FUN_ENABLE){
		sport = profCacheServer->cacheServerPort;
	}
#endif
	httpPortAdd(SYS_HTTP_PORT,sport);
    noticeInit();/*初始化*/
    return;
}
#endif

