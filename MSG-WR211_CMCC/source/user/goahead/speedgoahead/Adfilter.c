#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

#if (ADFILTER == FYES)
#define DEBUG_ADFILTER 0
#if DEBUG_ADFILTER
#define DEBUG_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define DEBUG_PRINTF(fmt, args...)
#endif


unsigned int oldCheckCycle;
unsigned int policyCounter;
unsigned int timeCheckUpdatePoint;
extern void funInitAdfilter(void);
extern int policyUpdateCheck(AdfilterProfile *adfilterProfile,unsigned char flag);
extern int doSystem(char *fmt, ...);
static int isWanConnected(void);
static void initAdfilterChain(int flag);
static void setAdfilterRule(const AdfilterProfile *adfilterProfile);
static void adfilterProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const AdfilterProfile *oldProfileDataPtr, const AdfilterProfile *newProfileDataPtr );
static void initAdfilterChain(int flag)
{
    /*总是先清空，然后根据是否开启再重建*/
    doSystem("iptables -t filter -F adfilter_chain >/dev/null 2>&1;iptables -t filter -D INPUT -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t filter -D FORWARD -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t filter -X adfilter_chain >/dev/null 2>&1;iptables -t nat -D PREROUTING -m uttdev --is-lan-in -p tcp -d %s --dport 80 -j REDIRECT --to-ports %d >/dev/null 2>&1;killall adfilter >/dev/null 2>&1",DNS_REDIRECT_IP,PROXY_PORT);
    if (flag) {
	doSystem("iptables -t filter -N adfilter_chain >/dev/null 2>&1;iptables -t filter -A INPUT -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t filter -A FORWARD -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t nat -A PREROUTING -m uttdev --is-lan-in -p tcp -d %s --dport 80 -j REDIRECT --to-ports %d >/dev/null 2>&1;adfilter & >/dev/null 2>&1",DNS_REDIRECT_IP,PROXY_PORT);
    }
}
/********************************************************************
* 函数： setAdfilterRule
* 功能： 广告过滤生效函数
* 创建： 2014-06-11
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： 赵冲
********************************************************************/
static void setAdfilterRule(const AdfilterProfile *adfilterProfile)
{
    int i;

    char cmdstr[512];
    memset(cmdstr, 0 ,sizeof(cmdstr));

    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
    for (i = 0; i < *adfilterProfile->originalHost[i] != '\0'; i++) {
	sprintf(cmdstr,"iptables -A adfilter_chain -m domain --name %s -j ",adfilterProfile->originalHost[i]);
	if (strcmp(adfilterProfile->host[i],"drop") == 0) {
	    strcat(cmdstr, "DROP");
	} else {
	    sprintf (cmdstr + strlen(cmdstr), "REJECT --reject-with dns-redirect:%s",DNS_REDIRECT_IP);
	}
	strcat(cmdstr, ">/dev/null 2>&1");
	doSystem(cmdstr);
    }
}
/********************************************************************
* 函数： adfilterProfileChange
* 功能： 广告过滤配置变化 生效函数
* 创建： 2014-06-11
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： 赵冲
********************************************************************/
static void adfilterProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const AdfilterProfile *oldProfileDataPtr, const AdfilterProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
	    if (newProfileDataPtr->head.active == TRUE) {
#if (APP_POLICY_REMOTE_UPDATE == FYES)
		if (newProfileDataPtr->updateType == 0) {/*计算检测升级的时间*/
		    oldCheckCycle = (SECONDS_OF_DAY * newProfileDataPtr->checkCycle);
		    policyCounter = 0;
		    srand(time(NULL));
		    timeCheckUpdatePoint = rand() % oldCheckCycle;
		    DEBUG_PRINTF("%s-%d,oldCheckCycle=%u\n",__func__, __LINE__,oldCheckCycle);
		}
#endif
		initAdfilterChain(TRUE);
		setAdfilterRule(newProfileDataPtr);
	    } else {
		initAdfilterChain(FALSE);
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
}
/********************************************************************
* 函数： funInitAdfilter
* 功能： 广告过滤 初始化入口函数
* 创建： 2014-06-11
* 参数： 略
* 返回： 略
* 输出： 略
* 作者： 赵冲 
********************************************************************/
void funInitAdfilter(void)
{
    AdfilterProfile *adfilterProfile;
    MibProfileType mibType = MIB_PROF_ADFILTER;

    registerForProfileUpdates(MIB_PROF_ADFILTER, (Pointer)NULL, (ProfileUpdateFnc)adfilterProfileChange);/*触发函数*/

    adfilterProfile = (AdfilterProfile*)ProfGetInstPointByIndex(mibType, 0);
    
    if (adfilterProfile != NULL && adfilterProfile->head.active == TRUE) {
	initAdfilterChain(TRUE);
	setAdfilterRule(adfilterProfile);
	if (adfilterProfile->updateType == 0) {/*计算检测升级的时间*/
	    srand(time(NULL));
	    oldCheckCycle = (SECONDS_OF_DAY * adfilterProfile->checkCycle);
	    timeCheckUpdatePoint = rand() % oldCheckCycle;
	    DEBUG_PRINTF("%s-%d,oldCheckCycle=%u,timeCheckUpdatePoint=%u\n",__func__, __LINE__,oldCheckCycle,timeCheckUpdatePoint);
	}
    }
    DEBUG_PRINTF("%s-%d\n",__func__, __LINE__);
}

static int isWanConnected(void)
{
    InterfaceProfile *profIf = NULL;
    Uint32 state = 0U;
    int flag = 0;
    Uint32 connStatus = 0U;/*默认值0表示wan口断开,如果存在多wan口，有一个连接上则置1*/
    Uint32 connType = 0U;/*默认为0表示未知接入，1表示DHCP接入，2表示固定接入，3表示PPPoE*/
    Uint32 phylinkStatus = 0U;/*物理链路连接状态，默认为0表示网线未连接*/
    char *wanName;
    MibProfileType mibType = MIB_PROF_EASY_TASK; 
    char if_addr[16] = {0};
#if (EASY_TASK == FYES)
    EasyTaskProfile *prof = NULL; 
#endif

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    phylinkStatus = (Uint32)get_phylink_status(1);/*获取wan1口状态*/

#if (EASY_TASK == FYES)
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
#endif

    switch(profIf->ether.connMode) {
	case ETH_CONN_DHCP:
	    connType = 1U;/*DHCP模式接入*/
	    wanName = getWanIfName(1);
	    if((flag = getIfIp(wanName,if_addr)) == 0) {/*如果DHCP获取到了IP,则置1*/
		state = 1U;/*如果获取到了IP，但仍不能上网，连接状态仍然是1*/
	    }
	    state &= phylinkStatus;/*如果网线断开或者DHCP获取不到IP地址，都会为0*/ 	    
	    break;
	case ETH_CONN_STATIC:
	    state = phylinkStatus;/*如果物理网络正常，但配置的固定IP错误不能上网，仍然为1*/
	    connType = 2U;
	    break;
	case ETH_CONN_PPPOE:
	default:
#if (EASY_TASK == FYES)
	    if (ProfInstIsFree(prof) == 0) {
		state = prof->ConnStatus;
	    }
#else
	    state = getpppoeStatus(1);/*获取PPPoE的连接状态*/
	    state &= phylinkStatus;
#endif
	    connType = 3U;
	    break;
    }
    if (state == 1U) {
	connStatus = 1U;
#if (EASY_TASK == FYES)
    } else if (state != 0U) {
	connStatus = state;
#endif
    }

    return connStatus;
}
/*该函数每分钟执行一次*/
int policyUpdateCheck(AdfilterProfile *adfilterProfile,unsigned char flag)
{
    SystemProfile *profSys = NULL;
    int status;

    if (!isWanConnected()) {/*如果wan口不通或者检测周期为0，则直接返回*/
	return 1;
    }
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

    DEBUG_PRINTF("---func:%s-line:%d----status=%d--\n",__func__,__LINE__,status);
    if (flag == 0U) {/*执行检测进程*/
	doSystem("uttAppCheckTool %d &",ADFILTER_UDTYPE);
    } else {
	status = profSys->appUpdateStatus[ADFILTER_UDTYPE];
	if (status == APP_WAITING_UPDATE) {/*有版本更新*/
	    doSystem("uttAppUpdateTool %d &",ADFILTER_UDTYPE);
	} else {
	    if (flag == 10U) {/*避免由于网络问题，造成进程阻塞*/
		doSystem("killall uttAppUpdateTool &; killall wget &");
	    }
	}
    }
    DEBUG_PRINTF("---func:%s-line:%d--timeCheckUpdatePoint=%d,policyCounter=%d----\n",__func__, __LINE__,timeCheckUpdatePoint,(policyCounter % (SECONDS_OF_DAY * adfilterProfile->checkCycle)));

    return 0;
}
#endif
