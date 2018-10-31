#include "uttMachine.h"
#if (PPTP_CLIENT == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"

static void pptp_default_nat_init(void);
#ifdef PPTP_DBG
#define PPTP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define PPTP_PRINTF(fmt, args...)
#endif
/*
 * pptp client账号profile change函数
 */
static void pptpCliProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    PPTPCliProfile *newProf = (PPTPCliProfile *)newProfileDataPtr;
    PPTPCliProfile *oldProf = (PPTPCliProfile *)oldProfileDataPtr;
    char cmdstr[128] = {0};

    switch(changeType) {
        case PROF_CHANGE_ADD:
	    /* 启用进程 */
#if (VPN_PROXY == FYES)
	    newProf->connectStatus = 0;
#endif
	    if (newProf->head.active == 1) {
#if (VPN_PROXY == FYES)
		newProf->connectStatus = 1;
#endif
#if (PPTP_ENCRYPT_MODE == FYES)
		doSystem("pptp.sh %s %s %u %s %s KeepAlive %s >/etc/options.pptp_%s", 
			newProf->userName, newProf->passwd, newProf->mtu, newProf->serverHost, 
			getEnumString(pppAuthEnum, newProf->pppAuthMode), newProf->EncryptionMode, newProf->head.name);
#else
		doSystem("pptp.sh %s %s %u %s %s KeepAlive >/etc/options.pptp_%s", 
			newProf->userName, newProf->passwd, newProf->mtu, newProf->serverHost, 
			getEnumString(pppAuthEnum, newProf->pppAuthMode), newProf->head.name);
#endif
		sprintf(cmdstr, "`ps |grep \"pppd file /etc/options.pptp_%s $\"|sed \"s/^ *//g\"|sed \"s/ .*$//g\"`", newProf->head.name);
		doSystem("kill -9 %s >/dev/null 2>&1", cmdstr);
		doSystem("ip route del %s scope link", newProf->serverHost);
		doSystem("pppd file /etc/options.pptp_%s &", newProf->head.name);
		pptp_default_nat_init();
	    }
            break;
        case PROF_CHANGE_EDIT:
	    /* 杀掉之前进程 */
	    doSystem("pptp-down.sh pptp_%s ", oldProf->head.name);
	    /* 启用进程 */
#if (VPN_PROXY == FYES)
	    newProf->connectStatus = 0;
#endif
	    if (newProf->head.active == 1) {
#if (VPN_PROXY == FYES)
		newProf->connectStatus = 1;
#endif
		newProf->remoteLanIp = newProf->remoteLanIpDis & newProf->remoteLanNetmask;
#if (PPTP_ENCRYPT_MODE == FYES)
		doSystem("pptp.sh %s %s %u %s %s KeepAlive %s >/etc/options.pptp_%s", 
			newProf->userName, newProf->passwd, newProf->mtu, newProf->serverHost, 
			getEnumString(pppAuthEnum, newProf->pppAuthMode), newProf->EncryptionMode, newProf->head.name);
#else
		doSystem("pptp.sh %s %s %u %s %s KeepAlive >/etc/options.pptp_%s", 
			newProf->userName, newProf->passwd, newProf->mtu, newProf->serverHost, 
			getEnumString(pppAuthEnum, newProf->pppAuthMode), newProf->head.name);
#endif
		sprintf(cmdstr, "`ps |grep \"pppd file /etc/options.pptp_%s $\"|sed \"s/^ *//g\"|sed \"s/ .*$//g\"`", newProf->head.name);
		doSystem("kill -9 %s >/dev/null 2>&1", cmdstr);
		doSystem("ip route del %s scope link", newProf->serverHost);
		doSystem("pppd file /etc/options.pptp_%s &", newProf->head.name);
		pptp_default_nat_init();
	    }
            break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
	    /*
	     * 删除实例，调用杀进程脚本
	     */
	    doSystem("pptp-down.sh pptp_%s", oldProf->head.name);
	    /* 
	     * 删除文件 etc/options.pptp_@ , /var/run/@link.status, 
	     * /var/run/@.uptime, /var/run/pptp_@.ppp 
	     */ 
	    doSystem("rm -f /etc/options.pptp_%s &", oldProf->head.name);
	    doSystem("rm -f /var/run/pptp_%slink.status &", oldProf->head.name);
	    doSystem("rm -f /var/run/pptp_%s.uptime &", oldProf->head.name);
	    doSystem("rm -f /var/run/pptp_%s.ppp &", oldProf->head.name);
	    pptp_default_nat_init();
            break;
        default:
            break;
    }
    return;
}
/********************************************************************
* 函数： pptp_default_nat_init
* 功能： 判断是否有PPTP客户端配置然后决定是否加链
* 创建： 2012-12-28
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
static void pptp_default_nat_init(void)
{
    MibProfileType mibType = MIB_PROF_PPTP_CLI;
    PPTPCliProfile *prof = NULL;
    int i, min = 0, max = 0;
    int flag = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (PPTPCliProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
#if (VPN_PROXY == FYES)
	    prof->connectStatus = 0;
	    if (prof->head.active == 1)
	    {
		prof->connectStatus = 1;
	    }
#endif
	    flag = 1;
	    break;
	}
    }
    if (flag == 1) {/*有客户端配置存在*/
	doSystem("iptables -t nat -D POSTROUTING -j pptp_default_nat;iptables -t nat -N pptp_default_nat;iptables -t nat -A POSTROUTING -j pptp_default_nat");
    } else {
	doSystem("iptables -t nat -D POSTROUTING -j pptp_default_nat");
    }

    return;
}
/*
 * 客户端初始化
 * TODO
 */
static void pptpCliInit(void) {
    pptp_default_nat_init();
    return;
}

/*
 * 初始化函数
 */
extern void funInitPptpCli(void) {
    registerForProfileUpdates(MIB_PROF_PPTP_CLI, NULL, pptpCliProfileChange);
    pptpCliInit();

    return;
}
#endif
