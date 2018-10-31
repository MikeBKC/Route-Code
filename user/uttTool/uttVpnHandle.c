#include	<string.h>
#include	<stdarg.h>
#include	<fcntl.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"
#include	"utt_pppoeClient.h"

#if 0
#define DBGPRINT(fmt, args...)	    printf(fmt,##args)
#else
#define DBGPRINT(fmt, args...) 
#endif

#if (VPN_CONTROL == FYES)
static void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*应该命令主要针对ipset故不会太大*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*生成命令字符串*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

    DBGPRINT(cmdstr);
    /*运行命令*/
    system(cmdstr);
    return;
}

#if (IP_SEC == FYES)
/* searchFreeindex
 * 查找未使用的索引号
 * 输入：null
 * 输出：null
 * 返回：未使用的索引号
 * */
static int searchFreeindex()
{
    int i=0;
    int index = -1;
#if (VPN_ITEM_CONTROL == FYES)
    for(i = 0;i<MAX_VPN_ITEMS_NUM;i++)
#else
    for(i = 0;i<MAX_IPSEC_PROFILES;i++)
#endif
    {
	if(strlen(nvramProfile->saState[i].name) == 0U)
	{
	    nvramProfile->saState[i].sa_state = 0U;
	    index = i;
	    break;
	}
    }
    return index;
}
/* searchIndexByname
 * 根据名字查找索引号
 * 输入：ipsec名字
 * 输出：null
 * 返回：索引号
 * */
static int searchIndexByname(char *name)
{
    int i=0;
    int index = -1;
#if (VPN_ITEM_CONTROL == FYES)
    for(i = 0;i<MAX_VPN_ITEMS_NUM;i++)
#else
    for(i = 0;i<MAX_IPSEC_PROFILES;i++)
#endif
    {
	if(strncmp(nvramProfile->saState[i].name, name, UTT_INST_NAME_LEN) == 0U)
	{
	    index = i;
	    break;
	}
    }
    return index;
}
#endif
#if (L2TP_SERVER == FYES)
static void killOneL2tpClient(char *user) 
{
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    if(user != NULL) {
    fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDONLY);

    if(fd != -1) {
        while (read(fd, &info, st_size) > 0) {
	    DBGPRINT("%s: user=%s,pid=%d,STime=%d,LTime=%d,ConTime=%d,rx=%lld,tx=%lld\n",__FUNCTION__,info.user,info.pid,info.STime,info.LTime,info.ConTime,info.rxByte,info.txByte);
	    if (info.ConTime == 0) {
            if(strncmp(user, info.user, UTT_PPP_CLIENT_NAME_LEN) == 0) {
                doSystem("kill -INT %d", info.pid);
		break;
            }
	    }
        }
	close(fd);
    }
    }
    return;
}
#endif /*L2TP_SERVER*/
#endif /*VPN_CONTROL*/
/* usage()
 * 命令使用帮助
 * */
static void usage()
{
    printf("usage: vpnHandle  print \n");
    printf("			print vpn free session num.\n");
    return;
}
/* main()
 * vpn 并发条目控制函数
 * vpn条目加减控制
 * pptp client,server和IPSec调用
 **/
int main(int argc, char **argv) 
{
#if (VPN_CONTROL == FYES)
#if (IP_SEC == FYES)
    MibProfileType mibipsec = MIB_PROF_IPSEC;
    ipsecConfProfile *ipsecProf = NULL;
    int saIndex = -1;
#endif

    if((argc < 2) || ((argc == 4)&&(strcmp(argv[2], "pptpclient")!=0)&&(strcmp(argv[2], "ipsec")!=0)&&(strcmp(argv[2],"l2tpclient")!=0)&&(strcmp(argv[2],"l2tpserver")!=0))) {
	usage();
	return -1;
    }
    /*共享内存初始化*/
    if(ProfInit() == PROFFAIL) 
    {
	return -1;
    }

    printf("[VPN]: vpn session num print \n");
    printf("[VPN]: argv =%s \n",argv[1]);
    if(strcmp(argv[1], "plus")==0) {
	uttSemP(SEM_VPN_NO, 0);
	if((argc==4)&&(strcmp(argv[2], "ipsec")==0)){
#if (IP_SEC == FYES)
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByName(mibipsec, argv[3]);
	    if((ipsecProf!=NULL)) {
		if(ipsecProf->sa_state == 1) {
		    nvramProfile->vpnProfile.count++;
		}
		ipsecProf->sa_state = 0U;
		/*先删除配置，然后sa down的情况*/
		if((saIndex = searchIndexByname(argv[3])) >= 0) {
		    memset(nvramProfile->saState[saIndex].name,0U,UTT_INST_NAME_LEN+1);
		    nvramProfile->saState[saIndex].sa_state = 0U;
		}
	    } else {
		/*先删除配置，然后sa down的情况*/
		if((saIndex = searchIndexByname(argv[3])) >= 0) {
		    nvramProfile->vpnProfile.count++;
		    memset(nvramProfile->saState[saIndex].name,0U,UTT_INST_NAME_LEN+1);
		    nvramProfile->saState[saIndex].sa_state = 0U;
		}
	    }
#endif
	} else {
	    nvramProfile->vpnProfile.count++;
	}
	if(nvramProfile->vpnProfile.count > MAX_VPN_SESSION_NUM) {
	    nvramProfile->vpnProfile.count = MAX_VPN_SESSION_NUM;
	}
	uttSemV(SEM_VPN_NO, 0);
    } else if(strcmp(argv[1], "minus")==0) {
	uttSemP(SEM_VPN_NO, 0);
	if(((argc==4) && strcmp(argv[2], "pptpclient")==0)) {
#if (PPTP_CLIENT == FYES)
	    printf("%s-%d: vpn minus, pptpclient =%s\n",__func__,__LINE__,argv[3]);
	    if(nvramProfile->vpnProfile.count<=0) {
		/*没有空想链接时，直接杀掉此进程*/
		nvramProfile->vpnProfile.count--;
		/* 杀掉之前进程 */
		doSystem("pptp-down.sh pptp_%s &", argv[3]);
	    } else {
		nvramProfile->vpnProfile.count--;
	    }
#endif
	} else if((argc==4)&&(strcmp(argv[2], "ipsec")==0)){
#if (IP_SEC == FYES)
	    ipsecProf = (ipsecConfProfile *)ProfGetInstPointByName(mibipsec, argv[3]);
	    if(nvramProfile->vpnProfile.count<=0) {
		if(ipsecProf->sa_state != 1U) {
		/*没有空想链接时，直接杀掉此进程*/
		nvramProfile->vpnProfile.count--;
		ipsecProf->sa_state = 1U;
		/*先删除配置，然后sa down的情况*/
		if((saIndex = searchFreeindex()) >=0) {
		    strncpy(nvramProfile->saState[saIndex].name,argv[3],UTT_INST_NAME_LEN);
		    nvramProfile->saState[saIndex].sa_state = 1U;
		}
		/* 杀掉之前进程 */
		doSystem("ipsec auto --delete \"%s\" &", argv[3]);
		}
		//ipsecProf->sa_state = 0U;
	    } else {
		if((ipsecProf!=NULL)) {
		    if(ipsecProf->sa_state!=1U) {
			nvramProfile->vpnProfile.count--;
			ipsecProf->sa_state = 1U;
			/*先删除配置，然后sa down的情况*/
			if((saIndex = searchFreeindex()) >=0) {
			    strncpy(nvramProfile->saState[saIndex].name,argv[3],UTT_INST_NAME_LEN);
			    nvramProfile->saState[saIndex].sa_state = 1U;
			}
		    }
		}

	    }
#endif
	} else if(((argc==4) && strcmp(argv[2], "l2tpclient")==0)) {
#if (L2TP_CLIENT == FYES)
	    printf("%s-%d: vpn minus, l2tpclient =%s\n",__func__,__LINE__,argv[3]);
	    if(nvramProfile->vpnProfile.count<=0) {
		/*没有空想链接时，直接杀掉此进程*/
		nvramProfile->vpnProfile.count--;
		/* 杀掉之前进程 */
		doSystem("l2tp-down.sh l2tp_%s &", argv[3]);
	    } else {
		nvramProfile->vpnProfile.count--;
	    }
#endif
	} else if(((argc==4) && strcmp(argv[2], "l2tpserver")==0)) {
#if (L2TP_SERVER == FYES)
	    printf("%s-%d: vpn minus, l2tpserver =%s\n",__func__,__LINE__,argv[3]);
	    if(nvramProfile->vpnProfile.count<=0) {
		killOneL2tpClient(argv[3]);
	    } else {
		nvramProfile->vpnProfile.count--;
	    }
#endif
	} else {
	    nvramProfile->vpnProfile.count--;
	}
	uttSemV(SEM_VPN_NO, 0);
    } else if(strcmp(argv[1], "print") == 0){
	printf("[VPN]: vpn session num =%d \n",nvramProfile->vpnProfile.count);
    } else {
	printf("command parameter error.\n");
	usage();
    }

    /*共享内存分离*/
    ProfDetach();
#endif
    return 0;
}
