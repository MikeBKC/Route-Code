#include <stdlib.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ifmisc.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"
#include "uttfunction.h"
#include "comExeFun.h"
#include <pthread.h>
#include <net/if_arp.h>
#include <unistd.h>
#include "StaticRoute.h"

/**
 * 物理线路检测 但应遵循一个原则
 * 即 物理线路检测down掉了 线路 ，则必须 能在适当时机up线路
 * 若 其他功能down了线路（如用户配置的线路检测,且检测地址为外网dns），则物理线路检测不应该up线路
 */

extern int getWanDhcpDefaultRoute(int wannum, char* ipaddr);

#define WAN_LINK_CHECK_INTERVAL 10u
typedef struct __PhyStateCount{
    int workState;/*当前工作状态。如若为up则 wan口路由存在*/
    int diffCount;/*实际状态中与工作状态不同的次数计数*/
}PhyStateCount;

static PhyStateCount phyStaCounter[MAX_WAN_IF_COUNT + 1] ;
static pthread_t wan_link_check_task_thread;
static int WanLinkCheckTaskStatus;
static void wanLinkCheckTask(void);
extern int get_phylink_status(int wanIndex);
static int wanLinkPoll(unsigned int pollCount, PhyStateCount* phystate);
extern void InterfaceStatusChange(Uint32 wanIndex, Uint32 status);
extern int doSystem(char *fmt, ...);
static int wanStaChaFlag;



#if (defined(CONFIG_USER_CFGSER_LAN) && (FEATURE_SOHO_REPEATER != FYES))
extern int get_lan_status(int lanIndex);
static int isOnCfgLanStatus = 0 ;
static int offCfgLanStatusCount = 0;
static int onCfgLanStausCount = 0;
#define WAIT_TIME_INTERVAL 5/*添加宏来控制网线断几秒到接几秒来确认*/ 
static int isOnCfgLanStatus_flag = 0;
static int onStateCount = 0;
#endif




#ifdef EASY_TASK_DEBUG
#define EASY_TASK_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define EASY_TASK_PRINTF(fmt, args...)
#endif




#if (defined(CONFIG_USER_CFGSER_LAN) && (FEATURE_SOHO_REPEATER != FYES))
/*检测lan口的状态，1,2,3,4*/
static void LanLinkPollLanCfg()
{
    unsigned int i,ret=0,stateCount = 0;
    for( i = 1 ; i < 5 ; i++ )
    {
	if(ret = get_lan_status(i)==1)
	{
	    stateCount++;
	}
    }
    if(isOnCfgLanStatus == 0)
    {
	if(stateCount == 0)
	{
	    onStateCount = stateCount;
	    printf("=====tateCount = 0========line = %d...func = %s \n",__LINE__,__func__);  
	    isOnCfgLanStatus = 1;
	}
	else
	{
	    if(++onCfgLanStausCount>=WAIT_TIME_INTERVAL)
	    {
		printf("=====start uttCfgSer=========line = %d...func = %s \n",__LINE__,__func__);  
		onCfgLanStausCount = 0;
		isOnCfgLanStatus = 1;
		onStateCount = stateCount;
		system("uttCfgSer&");
	    } 
	}
    }
    else 
    {
	if(stateCount == onStateCount)
	{
	    if(++offCfgLanStatusCount >=WAIT_TIME_INTERVAL)
	    {
		isOnCfgLanStatus_flag = 1;	
		offCfgLanStatusCount = 0 ;
	    }
	}
	else if(stateCount < onStateCount)	
	{
	    onStateCount = stateCount;
	}
	if((isOnCfgLanStatus_flag==1 )&&(stateCount > onStateCount))
	{
	    if(++onCfgLanStausCount>=WAIT_TIME_INTERVAL)
	    {
		onStateCount = stateCount;
		printf("=====start uttCfgSer=========line = %d...func = %s \n",__LINE__,__func__);  
		isOnCfgLanStatus_flag = 0;
		onCfgLanStausCount = 0;
		system("uttCfgSer&");
	    }      
	}
    }
}
#endif

/**
 * 设置 物理线路检测 计数器的值
 * wanIndex: wan口索引 对应 物理线路状态计数器
 * workState：当前工作状态
 */
extern phyStaCounterSet(int wanIndex, int workState)
{
    phyStaCounter[wanIndex].workState = workState;/*更新工作状态*/
    phyStaCounter[wanIndex].diffCount = 0;/*统计清零*/
    return;
}
/**
 * 物理检测计数器初始化
 * */
static phyStaCounterInit(void)
{
    int wanCount = 0, wanIndex = 0;
    wanCount = getWanSlotCount();
    for(wanIndex = 1; wanIndex <= wanCount; wanIndex++)
    {
	/*工作状态默认为1 
	 * 因为设备启动 会自动初始化 所有接口*/
	phyStaCounter[wanIndex].workState = 1;
	phyStaCounter[wanIndex].diffCount = 0;
    }
    return;

}
/*
 * getOldLinkStatus - 获取原来的线路状态
 *
 * 通过WAN口索引获取此线路原来的状态
 * 
 * 参数：
 *	num	-	WAN口索引
 *
 * 返回值：
 *	ret	-	线路原来的状态
 *	0	-	线路原来断开
 *	1	-	线路原来是连接
 *
 */
static int getOldLinkStatus(unsigned int num)
{
	int ret = 0;
	FILE *fp = NULL;
	char command[30];
	char buffer[10];

	/*数组初始化*/
	memset(command, 0, 30);
	memset(buffer, 0, 10);
	sprintf(command, "cat /var/run/Wan%dlink.status", num);

	/*打开通道*/
	if ((fp = popen(command, "r")) != NULL) {
		if (fgets(buffer, sizeof(buffer), fp) != NULL) 
			ret = (int)strtol(buffer, 0, 10);
		pclose(fp);/*关闭通道*/
	}
	return ret;
}

/*
 * changeWanRoute	-	改变WAN口路由
 *
 * 当WAN口的物理连接状态改变时添加或删除对应的路由
 *
 * 参数：
 *	state		-	现在的物理连接状态
 *	i		-	物理接口索引
 */
static void changeWanRoute(int state, int i)
{
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    char wanDhcpGw[RT_IP_LEN];

    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIf, i);
    memset(wanDhcpGw, 0, sizeof(wanDhcpGw));
    if (ProfInstIsFree(profIf) == 0) {
	if(profIf->ether.connMode != ETH_CONN_PPPOE)
	{/*pppoe自带检测*/
	   
	    if (state != getOldLinkStatus(i))
	    {
		if(state !=0 )
		{/*未连接到已连接*/
		    /*DHCP则获取默认网关，若有则添加路由*/
		    if (profIf->ether.connMode == ETH_CONN_DHCP) {
			getWanDhcpDefaultRoute(i, wanDhcpGw);
		    }
		    if (((profIf->ether.connMode == ETH_CONN_STATIC) 
				|| (strlen(wanDhcpGw) >= 7)))
			InterfaceStatusChange(i, state);
		}
		else
		{
		    InterfaceStatusChange(i, state);
		}
	    }
	  
	}
    }
    return;
}
static int wanLinkPoll(unsigned int pollCount, PhyStateCount* phystate)
{
    int i = 0;
    int retVal = 0;
    int state = 0;
    int wanCount = 0;
#if (EASY_TASK == FYES)
    static int oldstate = 0;
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
#endif
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;

    wanCount = getWanSlotCount();
    for(i = 1; i <= wanCount; i++)
    {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, i);
	if(profIf->head.active != TRUE)
	{/*线路未配置*/
	    continue;
	}
	state=get_phylink_status(i);
#if (EASY_TASK == FYES)
	if (i == 1)
	{/*易管理只检测wan1口*/
	    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
	    uttSemP(SEM_CONNSTA_NO, 0);/*加锁*/
	    if ((ProfInstIsFree(prof) == 0) && (ProfInstIsFree(profIf) == 0) &&
		    (prof->ConnStatus != CONNECTED))
		/*由于多线程。这里应该防止 另一个线程已经置为连接中*/
	    {
		if ((prof->ConnStatus >= USER_PASS_ERROR) && (profIf->ether.connMode == ETH_CONN_PPPOE))
		{/*pppoe会自动拨号。所以连接异常一段时间。需置为连接中。增加用户体验*/
		    wanStaChaFlag++;
		}
		else
		{
		    wanStaChaFlag = 0;
		}
		if (wanStaChaFlag > 3)
		{
		    /*置为连接中。pppd重新拨号时。会把连接中置为连接异常*/
		    wanConnStatsChangeNPV(CONNECTING);	
		    wanStaChaFlag = 0;
		}
		if (state != oldstate)
		{
		    if (getValueByProfSys(1) == 0)/*出厂配置*/
		    {
			doSystem("killall uttLinkCheck >/dev/null 2>&1");
			if (state == 1)
			{/*wan口接入方式检测*/
			    doSystem("uttLinkCheck %s &", getWanIfName(1));
			}
		    }
		    else
		    {
			if ((state == 1) && (profIf->ether.connMode == ETH_CONN_PPPOE))
			{
			    wanConnStatsChangeNPV(CONNECTING);	
			}
		    }
		}
	    }
	    uttSemV(SEM_CONNSTA_NO, 0);/*解锁*/
	    oldstate = state;
	}
#endif
	/*状态变化*/
	if (state != phystate[i].workState)
	{
	    phystate[i].diffCount++;
	    /*一个统计周期内 100%未连接 或 30%已连接 则路由需改变 */
	    if( ((state == 0) && (phystate[i].diffCount == WAN_LINK_CHECK_INTERVAL))
		    || ((state != 0) && (phystate[i].diffCount * 10 / WAN_LINK_CHECK_INTERVAL >= 5))  )
	    {
		changeWanRoute(state, i);/*改变路由*/
		phystate[i].workState = state;/*更新工作状态*/
		phystate[i].diffCount = 0;/*统计清零*/
	    }
	}
	if( (pollCount % WAN_LINK_CHECK_INTERVAL) == 0)
	{/*一个周期清一次零*/
	    phystate[i].diffCount = 0;
	}
    }
    return retVal;
}

static void wanLinkCheckTask(void)
{
    unsigned int pollCount = 0 ;
    pthread_detach(pthread_self());/*线程退出后立即释放资源.若指定pthread_join可不用此举*/

    phyStaCounterInit();

    while(1)
    {
	if(WanLinkCheckTaskStatus == 0)
	{
	    pthread_exit(NULL);/*放在循环开头退出。保证锁资源被正常释放*/
	}
	else
	{
	    pollCount++;

	    /*
	     *此处为加上信号量锁定此操作
	     */
	    uttSemP(SEM_PHYDETECT_NO, 0);
	    wanLinkPoll(pollCount, phyStaCounter);
#if (defined(CONFIG_USER_CFGSER_LAN) && (FEATURE_SOHO_REPEATER != FYES))
	    LanLinkPollLanCfg();
#endif
	    /*
	     *此处为加上信号量锁定此操作
	     */
	    uttSemV(SEM_PHYDETECT_NO, 0);

	    sleep(1u);/*挂起等待*/
	}
    }
    return;
}


/**
 * 创建一个线程来检测WAN口连接状态，用于易管理模块
 * crOrRe : 1 creat a thread for Wan link check task
 *          0 remove the thread
 */
void wanLinkCheckTaskInit(int crOrRe)
{
    WanLinkCheckTaskStatus = crOrRe;

    if (crOrRe == 1)
    {
	if(pthread_create(&wan_link_check_task_thread, NULL, (void*)wanLinkCheckTask, NULL) != 0)
	{
	    printf("Create threat for Wan link check task error!\n");
	}
    }
    
    return;
}
