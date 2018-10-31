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
 * ͨ�����ñ仯 ��Ч����
 */
static void noticeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const NoticeProfile *oldProfileDataPtr, const NoticeProfile *newProfileDataPtr )
{
    NoticeProfile *oldProf = (NoticeProfile *)oldProfileDataPtr;
    NoticeProfile *newProf = (NoticeProfile *)newProfileDataPtr;
    char ip[16] = {0};
    /* ���lan��IP */
    getIfIp((char *)getLanIfName(), ip);

    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    editCapRule(index, oldProf, Iptables_DEL);/*��ɾ��֮ǰ��*/
	    editCapRule(index, newProf, Iptables_ADD);/*������µĹ���*/
	    if (oldProf->head.active != newProf->head.active) {
		/* ��֤˳�� */
	        renewNTManglePREROUTINGJumpRule();

		if (newProf->head.active == TRUE) { /* ���� */
		    if (index == 0) { /* �ճ�ͨ�� */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -A POSTROUTING -p tcp -m set --set %s src -m set ! --set %s dst -j %s",SYS_HTTP_PORT, NOTICE_OK_GRP, NOTICE_CHAIN_R);
#else
			doSystem("iptables -t mangle -A POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s", NOTICE_OK_GRP, NOTICE_CHAIN_R);
#endif
		    } 
#if (PPPOE_ACCOUNT == FYES)
		    else if (index == 1) { /* ����ͨ�� */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -I POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -m set ! --set %s dst -j %s",SYS_HTTP_PORT, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -I POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -j %s",SYS_HTTP_PORT, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#else
			doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -I POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#endif
		    }
#endif
		    else if (index == 2) { /*��������ͨ��*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
			sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
			DnsNoticeChange(1);
#endif
		    }
		} else { /* �ر� */
		    if (index == 0) { /* �ճ�ͨ�� */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set ! --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, NOTICE_OK_GRP, NOTICE_CHAIN_R);
#else
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", NOTICE_OK_GRP, NOTICE_CHAIN_R);
#endif
		    } 
#if (PPPOE_ACCOUNT == FYES)
		    else if (index == 1) { /* ����ͨ�� */
#if (CACHE_SERVER == FYES)
			doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -D POSTROUTING -p tcp -m set --set %s src -m set --set %s dst -j %s 1>/dev/null 2>&1",SYS_HTTP_PORT, POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#else
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -m set ! --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRING_ACC, POE_NT_OK, POE_NT_R_CHAIN);
			doSystem("iptables -t mangle -D POSTROUTING -p tcp --sport 80 -m set --set %s dst -j %s 1>/dev/null 2>&1", POE_EXPIRED_ACC, POE_NT_DROP_R_CHAIN);
#endif
		    }
#endif
		    else if (index == 2) { /*��������ͨ��*/
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
 * �ؽ�mangle PREROUTING��ͨ����ع����ճ���Poeͨ��
 * Poeͨ�����ȼ������ճ�ͨ�棬��˳���ϵ
 */
extern void renewNTManglePREROUTINGJumpRule(void) {
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
    char ip[16] = {0};
    /* ���lan��IP */
    getIfIp((char *)getLanIfName(), ip);

#if (PPPOE_ACCOUNT == FYES)
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 1);
    if ((prof) && (prof->head.active == TRUE)) { /* poe����ͨ�� */
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
	/*�ճ�����ͨ���ʼ��*/
#if (IP_GRP == FYES)
	doSystem("iptables -t mangle -D PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s 1>/dev/null 2>&1", SYS_LAN_GROUP, PRE_NOTICE);
	doSystem("iptables -t mangle -A PREROUTING -p tcp --dport 80 -m set ! --set %s dst -j %s", SYS_LAN_GROUP, PRE_NOTICE);
#else
#error ipset has been removed
#endif
#else
	/*�ճ�����ͨ���ʼ��*/
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

/*ͨ���ʼ������
 * ���ܣ���������й��� ���� �� ���������
 * bhou*/
static void noticeInit(void)
{
    MibProfileType mibTypeNT = MIB_PROF_NOTICE;
    NoticeProfile *profNT = NULL;

#if 0
    /*�������*/
    doSystem("iptables -t mangle -D PREROUTING -i %s -d ! %s -j %s 1>/dev/null 2>&1", getLanIfName(), ip, NOTICE_CHAIN);/*lan�ڽ����Ĳ�ͨ��*/
    /*�����������*/
    /*ɾ����*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,NOTICE_CHAIN);
#endif

    /*�����ipset��*/
    /*�˲��ֱ�����iptables�������������½��С��������õ�ipset���޷�ɾ��*/
    ipsetDelGrp(NOTICE_OK_GRP, TRUE);/*ɾ����ͨ���û���*/
    noticeOkGrpAdd(NOTICE_OK_GRP);/*���������*/
#if 0 /* NOTICE_WEB_GRP����Ҫ */
    ipsetDelGrp(NOTICE_WEB_GRP, TRUE);/*ɾ����ͨ���û���*/
    noticeWebGrpAdd(NOTICE_WEB_GRP);/*�����ͨ���û���*/
#endif


    /*����ͨ�������*/
    doSystem("iptables -t mangle -N %s" ,NOTICE_CHAIN);
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",NOTICE_CHAIN);

    /*����PPPOE����������һ������������*/
    doSystem("iptables -t mangle -N  %s", PRE_NOTICE);
    doSystem("iptables -t mangle -F  %s >/dev/null 2>&1", PRE_NOTICE);
#if 0
    /*���е�Ŀ��ip��ַ����lan��IP��ַ�Ķ���������*/
    doSystem("iptables -t mangle -A PREROUTING -d ! %s -j %s", ip, PRE_NOTICE);
#endif
#if (PPPOE_ACCOUNT == FYES)
    /*������ͨ�����ȼ������ճ�ͨ��*/
#if (CACHE_SERVER==FYES)
    doSystem("iptables -t mangle -I %s -m connmark --mark %u/0x%x -j RETURN", PRE_NOTICE, POE_NT_R,NOTICE_POE_CONNMARK,NOTICE_CONNMARK_MSK);
#else
    doSystem("iptables -t mangle -I %s -m set --set %s src,src,dst -j RETURN", PRE_NOTICE, POE_NT_R);
#endif
#endif
    /*lan�ڽ������Ҳ���ok���еļ�������*/
    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -m set ! --set %s src -j %s", PRE_NOTICE, NOTICE_OK_GRP, NOTICE_CHAIN);

    doSystem("cron-init.sh del \"ipset -F %s\"", NOTICE_OK_GRP);/*��ն�ʱ��ok�������*/

#if 1
#if 0
    /*�������*/
    doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", NOTICE_CHAIN_R);
    /*ɾ����*/
    doSystem("iptables -t mangle -X %s  1>/dev/null 2>&1" ,NOTICE_CHAIN_R);
#endif

    /* ����http redirect���� */
    doSystem("iptables -t mangle -N %s" ,NOTICE_CHAIN_R);
    /*�����������*/
    doSystem("iptables -t mangle -F %s 1>/dev/null 2>&1",NOTICE_CHAIN_R);

#if 0
    doSystem("iptables -t mangle -A POSTROUTING -j %s", NOTICE_CHAIN_R);
#endif
#if 0
    doSystem("cron-init.sh del \"ipset -F %s\"", NOTICE_R_GRP);/*��ն�ʱ��ok�������*/
#endif
#endif
    /*��ʼ����ͨ��capture����*/
    noticeCapRuleInit();
    /* 
     * �ؽ�mangle PREROUTING��ͨ�������Ҫ��֤˳�� 
     * mangle PREROUTING��poe_nt_cap_chain�ڴ˳�ʼ��,�½���initInternet��
     */
    renewNTManglePREROUTINGJumpRule();

    profNT = (NoticeProfile*)ProfGetInstPointByIndex(mibTypeNT, 0);
    if ((profNT) && (profNT->head.active == TRUE)) {
	/*�������*/
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
 * ��ʱ���ͨ��ok�顣����ÿ��ͨ�����ʼʱ�����ok��
 */
static void flushOkGrpTask(NoticeProfile *prof, char action)
{
    char crondAct[8];
    memset(crondAct, 0, sizeof(crondAct));

    if(action == Iptables_ADD)
    {/*��Ӽƻ�����*/
	strcpy(crondAct, "add");
    }
    else if(action == Iptables_DEL)
    {/*ɾ���ƻ�����*/
	strcpy(crondAct, "del");
    }
    else
    {/*ʲô������*/
	strcpy(crondAct, "exit");
    }

    /*��Ӽƻ�����ʱ���ipset ok��*/
    doSystem("cron-init.sh %s \"%s * * %s ipset -F %s\"",
	    crondAct,
	    getTimeForCrond((Uchar*)prof->timestart),
	    getWeekForCrond(prof->day),
	    NOTICE_OK_GRP
	    );
#if 0
    /* add by dhyu */
    /* ������http get��¼������ */
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
 * ͨ��capture����任
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

    if(prof->head.active == TRUE)/*��Ч*/
    {
	ipstr = getIpRangeForIptables(prof->ipstart, prof->ipend, 1);/*iptables ip range�ִ�*/
	timestr = getTimeForIptables(prof->timestart, prof->timestop);/*iptables ʱ���ִ�*/
	daystr = getDayForIptables(prof->day);/*iptables���ִ�*/
	datestr = getDateForIptables(prof->datestart, prof->datestop);/*iptables �����ֶ�*/


#if 0
	/*��Ҫͨ����û���http����ָ�� �豸webserver ������*/

	/*�������ڹ�����ʵ�ֵļ�¼���ݵ�ipset�飬��Ҫͨ������ʵ�֣���������ں���ת����Ͽ�*/
	doSystem("iptables -t mangle -%c %s %s -p tcp --dport 80 %s %s -j SET --add-set %s src",action, NOTICE_CHAIN, ipstr, timestr, daystr , NOTICE_WEB_GRP);/*���Ϲ��򡣼�¼ip��ַ����ͨ���û���*/
	doSystem("iptables -t mangle -%c %s %s -p tcp --dport 80 %s %s -j REDIRECT --to-ports 80",action, NOTICE_CHAIN, ipstr, timestr, daystr);/*���ϸ���http��ָ�򱾵�web������*/
#else
	if (action == Iptables_ADD) {
	    /* �½�http redirect ��¼�� */
	    addr.s_addr = prof->ipstart;
	    strcpy(ipFrom, inet_ntoa(addr));
	    addr.s_addr = prof->ipend;
	    strcpy(ipTo, inet_ntoa(addr));
	    noticeRGrpAdd(NOTICE_R_GRP, ipFrom, ipTo);
	}
	/*��Ҫͨ����û���http����ָ�� �豸webserver ������*/

#if 0 /* ����ҪNOTICE_WEB_GRP������¼�� */
	/*�������ڹ�����ʵ�ֵļ�¼���ݵ�ipset�飬��Ҫͨ������ʵ�֣���������ں���ת����Ͽ�*/
	doSystem("iptables -t mangle -%c %s %s -p tcp --dport 80 %s %s -m layer7 --l7proto httpGet -j SET --add-set %s src",action, NOTICE_CHAIN, ipstr, timestr, daystr , NOTICE_WEB_GRP);/*���Ϲ��򡣼�¼ip��ַ����ͨ���û���*/
#endif
#if (CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -%c %s %s %s %s %s -m layer7 --l7proto httpGet -j CONNMARK --set-mark %u/0x%x",action, NOTICE_CHAIN, ipstr, timestr, daystr, datestr,NOTICE_GRP_CONNMARK,NOTICE_CONNMARK_MSK);/*����http get��������ӣ���¼��notice_web_r���� */
#else
	doSystem("iptables -t mangle -%c %s %s %s %s %s -m layer7 --l7proto httpGet -j SET --add-set %s src,src,dst",action, NOTICE_CHAIN, ipstr, timestr, daystr, datestr, NOTICE_R_GRP);/*����http get��������ӣ���¼��notice_web_r���� */
#endif


	/* notice_chain_r�������ӹ��� */
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
	    /* ɾ���� */
	    ipsetDelGrp(NOTICE_R_GRP, TRUE);
	}
#endif

	/*���һ����Ӷ���������������Ż��¡�ֻ�����һ���������crond*/
	flushOkGrpTask(prof, action);/*��ʱ���ok��*/
    }

    return;
}
/**
 * ͨ��capture����任
 * action : Iptables_ADD for add , Iptables_DEL for delete
 * bhou
 * */
static void editCapRule(Uint32 profIndex, NoticeProfile *prof, char action)
{
    if(profIndex == 0u)
    {/*�ճ�ͨ��*/
	editNtCapRule(prof, action);
    }
#if (PPPOE_ACCOUNT == FYES)
    else
    {
	if(profIndex == 1u)
	{/*pppoeͨ��*/
	    editPoeCapRule(prof, action);
	}
    }
#endif
    return;
}

#if (PPPOE_ACCOUNT == FYES)
/*�����ڹ���
 * */
static void editPoeExpiringCap(NoticeProfile *prof, char action)
{

    /*��ȥ��http����¼*/
#if (CACHE_SERVER==FYES)
    doSystem("iptables -t mangle -%c %s -m layer7 --l7proto httpGet -j CONNMARK --set-mark %u/0x%x",action, POE_NT_CAP_CHAIN,NOTICE_POE_CONNMARK,NOTICE_CONNMARK_MSK);/*����http get��������ӣ���¼������ */
#else
    doSystem("iptables -t mangle -%c %s -m layer7 --l7proto httpGet -j SET --add-set %s src,src,dst",action, POE_NT_CAP_CHAIN, POE_NT_R);/*����http get��������ӣ���¼������ */
#endif
    /*�޸�http�ظ���ָ��webserver*/
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

/*���ڹ���
 * */
static void editPoeExpiredCap(NoticeProfile *prof, char action)
{
    /*�޸�http�ظ���ָ��webserver*/
#if 1
    doSystem("iptables -t mangle -%c %s -j HTTPREDIRECT --redirect-dir %s --return d", action, POE_NT_DROP_R_CHAIN, NOTICE_HTML_2_NAME);
#else
    doSystem("iptables -t mangle -%c %s -p tcp ! --tcp-flags SYN SYN -j HTTPREDIRECT --redirect-dir %s", action, POE_NT_DROP_R_CHAIN, NOTICE_HTML_2_NAME);
#endif
    if(action == Iptables_ADD)
    {
	action = Iptables_INS;/*���������ڶ����������ǰ��*/
    }

    /* ��http�ҷ�dns�İ�����ֱ�Ӷ���
     * 80�˿ڵ����а���ָ�� web server
     * dns��������Ϊ������������Խ���http����
     * */
    /*����3�������˳���ܸı�*/

    /*���¹�������http�������ȼ����*/
    doSystem("iptables -t mangle -%c %s -p tcp --dport 80 -j RETURN", action, POE_DROP_CHAIN);	
    /*�����������ȼ���������http��
     * Ϊ��ֹ��get���͵�http�����ù���ֻƥ�京��html���ݵ�http��
     * ֻ����get��Ϊ�˷�ֹ ���ַǷ�http��������goahead æ���������
     * ��bug 9270*/

    /*������get�����˹�����get������֮�£���l7ֻƥ��һ�Ρ�������ᶪ����get���������ᶪ����������*/
    doSystem("iptables -t mangle -%c %s -p tcp --dport 80 -m layer7 --l7proto nonZeroLen -j DROP", action, POE_DROP_CHAIN);
   /*����get��*/
    doSystem("iptables -t mangle -%c %s -p tcp --dport 80 -m layer7 --l7proto httpGet -j RETURN", action, POE_DROP_CHAIN);

    /*�����������ȼ���ߡ�Ϊ����dns*/
    doSystem("iptables -t mangle -%c %s -p udp --dport 53 -j RETURN", action, POE_DROP_CHAIN);	
    /*��nat������DROP���򣬹�����drop���� PRE_DROP_CHAIN*/
    return;
}
/**
 * pppoe�Ʒ�ͨ�����
 * action : Iptables_ADD for add , Iptables_DEL for delete
 * bhou
 * */
static void editPoeCapRule(NoticeProfile *prof, char action)
{
    if(prof->head.active == TRUE)/*��Ч*/
    {
	editPoeExpiredCap(prof, action);/*���ڹ���*/
	if(prof->remainDays > 0)
	{
	    editPoeExpiringCap(prof, action);/*ֻ�е�������ʣ����������0������н�����ͨ��*/
	}
    }

    return;
}
#endif
/**
 * ��ʼ��ͨ�� capture���򡣷��ϸù�����û���ͨ��
 * bhou 
 */
static void noticeCapRuleInit(void)
{
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
#if 1 
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);

    /*������ӹ���*/
    for(i = min; i < max; i++) {
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0) {
	    /*���һ������*/
	    editCapRule(i, prof,Iptables_ADD);

	}
    }
#else
    /* by dhyu, ����PPPoE�Ʒ�ͨ�� */
    /* ��ȡ�ճ�����ͨ�� */
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL && ProfInstIsFree(prof) == 0 && prof->head.active == TRUE) {
        /*���һ������*/
        editCapRule(prof,Iptables_ADD);
    }
#endif
    return;
}
#if 0
#if (CACHE_SERVER == FYES)
/*
 *
 *����������÷����仯ʱ��Ҫ����ͨ������е�sport 
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
 * ͨ�湦�� ��ʼ����ں���
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
    registerForProfileUpdates(MIB_PROF_NOTICE, (Pointer)NULL, (ProfileUpdateFnc)noticeProfileChange);/*��������*/

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
    noticeInit();/*��ʼ��*/
    return;
}
#endif

