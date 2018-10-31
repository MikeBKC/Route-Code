#include	<string.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"
#include	"argcmd.h"
#include	<sys/sysinfo.h>
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
#endif

#define SWORD_DEBUG 0
#if SWORD_DEBUG 
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_UTT_NETFILTER
int uttNfNlSock;
#endif

/*
 * 此函数向内核传递索引值和开启或关闭调试动作
 */
static void debug_kernel(UttDebugType appFlag, unsigned int action)
{
#ifdef CONFIG_UTT_NETFILTER
    UttNlNfDebugConf nlMsg;
    memset(&nlMsg, 0, sizeof(nlMsg)); 

    uttNfNlSock = uttNfNlSkBind(getpid(), 0);/*单播到本进程*/
    if(uttNfNlSock < 0) {
	SWORD_PRINTF("%s-%d, bind error", __func__, __LINE__);
	return;
    }
    nlMsg.msgHead.nlType = UTT_NLNF_DEBUG;
    if (action == 0u) {
	nlMsg.confType = UTT_NFCONF_DEL;
    } else {
	nlMsg.confType = UTT_NFCONF_ADD;
    }
    nlMsg.entryVal.debugFlag = appFlag;
    uttNfNlSend(uttNfNlSock, (char*)&nlMsg, sizeof(nlMsg), 0 ,0);/*发送消息给内核*/
    SWORD_PRINTF("nlMsg.confType = %u, nlMsg.entryVal.debugFlag = %u\n", nlMsg.confType, nlMsg.entryVal.debugFlag);
#endif
    return;
}

/*
 * 改变用户层开关
 */
static unsigned int debug_user(DebugPara *para)
{
    unsigned int ret = 0u;
    FILE *fp = NULL;
    char buffer[MAX_TTY_LEN + 1];
    int i = 0;

    memset(buffer, 0, MAX_TTY_LEN + 1);
    ret = (para->printEn == 0u) ? 1u : 0u;
    if ((fp = popen("tty", "r")) != NULL) {
	if(fgets(buffer, MAX_TTY_LEN, fp) != NULL) {
	    for (i = 0; buffer[i] != '\0'; i++) {
		if (buffer[i] == '\n') {/*去掉最后的回车符*/
		    buffer[i] = '\0';
		    break;
		}
	    }
	    strncpy(nvramProfile->uttDebug.tty_str, buffer, MAX_TTY_LEN);
	    para->printEn = ret;
	}
	pclose(fp);
    }
    SWORD_PRINTF("%s-%d, para->printEn = %u, nvramProfile->uttDebug.tty_str = 2%s2\n", __func__, __LINE__, para->printEn, nvramProfile->uttDebug.tty_str);
    return ret;
}

static void debug_help(void)
{
    printf("Usage:\n");
#if (DEBUG_DNS_NOTICE == FYES)
    printf(" uttDebug dns_notice     -   Domain filter notice\n");
#endif
#if (DEBUG_WEBAUTH_ACC == 1)
    printf(" uttDebug webauth_acc    -   Web auth account\n");
    printf(" uttDebug webauth_setusedtime username hour minute second	-   setUsedtime\n");
#endif
#if (DEBUG_POLICY_ROUTE == 1)
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    printf(" uttDebug policy_route   -   policy route config\n");
#endif
#endif
#if (DEBUG_GOAHEAD == FYES)
    printf(" uttDebug goahead	     -   goahead debug\n");
#endif
#if (DEBUG_DHCP_COLLISION == 1)
    printf(" uttDebug dhcp_collision -   dhcp collision detection debug\n");
#endif
    return;
}
int main(int argc, char **argv) 
{
    unsigned int appFlag = 0u;
    unsigned int kerEn = 0u;
    DebugPara *curPara = NULL;
    unsigned int action = 0u;

#if (DEBUG_WEBAUTH_ACC == 1)
    int min = 0;
    int max = 0;
    int i = 0;
    int hour = 0, minute = 0, second = 0;
#endif

    ProfInit();
    if (argc == 2) {
#if (DEBUG_DNS_NOTICE == FYES)
	if (strcmp(argv[1], "dns_notice") == 0) {
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTDNSREDIRECT
	    appFlag = UTT_DEBUG_DNS_NOTICE;
#endif
	    curPara  = &(nvramProfile->uttDebug.dnsDebug);
	    kerEn = 1u;
	} else 
#endif
#if (DEBUG_WEBAUTH_ACC == 1)
	if (strcmp(argv[1], "webauth_acc") == 0) {
	    curPara  = &(nvramProfile->uttDebug.webAuthAccDebug);
	} else 
#endif
#if (DEBUG_POLICY_ROUTE == 1)
	if (strcmp(argv[1], "policy_route") == 0) {
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICYROUTE
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
	    appFlag = UTT_DEBUG_POLICY_ROUTE;
#endif
#endif
	    curPara  = &(nvramProfile->uttDebug.policyrouteDebug);
	    kerEn = 1u;
	} else
#endif
#if (DEBUG_GOAHEAD == 1)
	if (strcmp(argv[1], "goahead") == 0) {
	    curPara  = &(nvramProfile->uttDebug.goaheadDebug);
	} else 
#endif
#if (DEBUG_DHCP_COLLISION == 1)
	if (strcmp(argv[1], "dhcp_collision") == 0) {
#ifdef CONFIG_UTT_DHCP_COLLISION
	    appFlag = UTT_DEBUG_DHCP_COLLISION;
#endif
	    curPara  = &(nvramProfile->uttDebug.dhcpCollisionDebug);
	    kerEn = 1u;
	} else 
#endif
	{
	    debug_help();
	}
	action = debug_user(curPara);
	if (action == 1u) {
	    printf("%s %s open\n", argv[0], argv[1]);
	} else {
	    printf("%s %s close\n", argv[0], argv[1]);
	}
	if (kerEn == 1u) {/*判断此功能是否涉及到内核*/
	    debug_kernel(appFlag, action);
	}
    }
#if (DEBUG_WEBAUTH_ACC == 1)
    else if (argc == 6) {
	MibProfileType mibType = MIB_PROF_WEB_AUTH;
	WebAuthProfile *prof = NULL;

	hour = strtol(argv[3], NULL, 10);
	minute = strtol(argv[4], NULL, 10);
	second = strtol(argv[5], NULL, 10);

	ProfInstLowHigh(mibType, &max, &min);
	for(i=min;i<max;i++)
	{
	    prof = (WebAuthProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	    {
		if(strncmp(prof->user,argv[2],UTT_USER_LEN) == 0)/*查看用户名是否已存在*/
		{
		    uttSemP(SEM_WEBAUTH_NO, 0);
		    prof->timeUsed = hour * 3600U + minute * 60U + second;
		    uttSemV(SEM_WEBAUTH_NO, 0);
		    ProfDetach();
		    return 0;
		}
	    }
	}
	printf("username don't exist\n");
    }
#endif
    else {
	debug_help();
    }

    ProfDetach();
    return 0;
}

#endif
