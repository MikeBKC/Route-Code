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
 * ������·��� ��Ӧ��ѭһ��ԭ��
 * �� ������·���down���� ��· ������� �����ʵ�ʱ��up��·
 * �� ��������down����·�����û����õ���·���,�Ҽ���ַΪ����dns������������·��ⲻӦ��up��·
 */

extern int getWanDhcpDefaultRoute(int wannum, char* ipaddr);

#define WAN_LINK_CHECK_INTERVAL 10u
typedef struct __PhyStateCount{
    int workState;/*��ǰ����״̬������Ϊup�� wan��·�ɴ���*/
    int diffCount;/*ʵ��״̬���빤��״̬��ͬ�Ĵ�������*/
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
#define WAIT_TIME_INTERVAL 5/*��Ӻ����������߶ϼ��뵽�Ӽ�����ȷ��*/ 
static int isOnCfgLanStatus_flag = 0;
static int onStateCount = 0;
#endif




#ifdef EASY_TASK_DEBUG
#define EASY_TASK_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define EASY_TASK_PRINTF(fmt, args...)
#endif




#if (defined(CONFIG_USER_CFGSER_LAN) && (FEATURE_SOHO_REPEATER != FYES))
/*���lan�ڵ�״̬��1,2,3,4*/
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
 * ���� ������·��� ��������ֵ
 * wanIndex: wan������ ��Ӧ ������·״̬������
 * workState����ǰ����״̬
 */
extern phyStaCounterSet(int wanIndex, int workState)
{
    phyStaCounter[wanIndex].workState = workState;/*���¹���״̬*/
    phyStaCounter[wanIndex].diffCount = 0;/*ͳ������*/
    return;
}
/**
 * �������������ʼ��
 * */
static phyStaCounterInit(void)
{
    int wanCount = 0, wanIndex = 0;
    wanCount = getWanSlotCount();
    for(wanIndex = 1; wanIndex <= wanCount; wanIndex++)
    {
	/*����״̬Ĭ��Ϊ1 
	 * ��Ϊ�豸���� ���Զ���ʼ�� ���нӿ�*/
	phyStaCounter[wanIndex].workState = 1;
	phyStaCounter[wanIndex].diffCount = 0;
    }
    return;

}
/*
 * getOldLinkStatus - ��ȡԭ������·״̬
 *
 * ͨ��WAN��������ȡ����·ԭ����״̬
 * 
 * ������
 *	num	-	WAN������
 *
 * ����ֵ��
 *	ret	-	��·ԭ����״̬
 *	0	-	��·ԭ���Ͽ�
 *	1	-	��·ԭ��������
 *
 */
static int getOldLinkStatus(unsigned int num)
{
	int ret = 0;
	FILE *fp = NULL;
	char command[30];
	char buffer[10];

	/*�����ʼ��*/
	memset(command, 0, 30);
	memset(buffer, 0, 10);
	sprintf(command, "cat /var/run/Wan%dlink.status", num);

	/*��ͨ��*/
	if ((fp = popen(command, "r")) != NULL) {
		if (fgets(buffer, sizeof(buffer), fp) != NULL) 
			ret = (int)strtol(buffer, 0, 10);
		pclose(fp);/*�ر�ͨ��*/
	}
	return ret;
}

/*
 * changeWanRoute	-	�ı�WAN��·��
 *
 * ��WAN�ڵ���������״̬�ı�ʱ��ӻ�ɾ����Ӧ��·��
 *
 * ������
 *	state		-	���ڵ���������״̬
 *	i		-	����ӿ�����
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
	{/*pppoe�Դ����*/
	   
	    if (state != getOldLinkStatus(i))
	    {
		if(state !=0 )
		{/*δ���ӵ�������*/
		    /*DHCP���ȡĬ�����أ����������·��*/
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
	{/*��·δ����*/
	    continue;
	}
	state=get_phylink_status(i);
#if (EASY_TASK == FYES)
	if (i == 1)
	{/*�׹���ֻ���wan1��*/
	    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
	    uttSemP(SEM_CONNSTA_NO, 0);/*����*/
	    if ((ProfInstIsFree(prof) == 0) && (ProfInstIsFree(profIf) == 0) &&
		    (prof->ConnStatus != CONNECTED))
		/*���ڶ��̡߳�����Ӧ�÷�ֹ ��һ���߳��Ѿ���Ϊ������*/
	    {
		if ((prof->ConnStatus >= USER_PASS_ERROR) && (profIf->ether.connMode == ETH_CONN_PPPOE))
		{/*pppoe���Զ����š����������쳣һ��ʱ�䡣����Ϊ�����С������û�����*/
		    wanStaChaFlag++;
		}
		else
		{
		    wanStaChaFlag = 0;
		}
		if (wanStaChaFlag > 3)
		{
		    /*��Ϊ�����С�pppd���²���ʱ�������������Ϊ�����쳣*/
		    wanConnStatsChangeNPV(CONNECTING);	
		    wanStaChaFlag = 0;
		}
		if (state != oldstate)
		{
		    if (getValueByProfSys(1) == 0)/*��������*/
		    {
			doSystem("killall uttLinkCheck >/dev/null 2>&1");
			if (state == 1)
			{/*wan�ڽ��뷽ʽ���*/
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
	    uttSemV(SEM_CONNSTA_NO, 0);/*����*/
	    oldstate = state;
	}
#endif
	/*״̬�仯*/
	if (state != phystate[i].workState)
	{
	    phystate[i].diffCount++;
	    /*һ��ͳ�������� 100%δ���� �� 30%������ ��·����ı� */
	    if( ((state == 0) && (phystate[i].diffCount == WAN_LINK_CHECK_INTERVAL))
		    || ((state != 0) && (phystate[i].diffCount * 10 / WAN_LINK_CHECK_INTERVAL >= 5))  )
	    {
		changeWanRoute(state, i);/*�ı�·��*/
		phystate[i].workState = state;/*���¹���״̬*/
		phystate[i].diffCount = 0;/*ͳ������*/
	    }
	}
	if( (pollCount % WAN_LINK_CHECK_INTERVAL) == 0)
	{/*һ��������һ����*/
	    phystate[i].diffCount = 0;
	}
    }
    return retVal;
}

static void wanLinkCheckTask(void)
{
    unsigned int pollCount = 0 ;
    pthread_detach(pthread_self());/*�߳��˳��������ͷ���Դ.��ָ��pthread_join�ɲ��ô˾�*/

    phyStaCounterInit();

    while(1)
    {
	if(WanLinkCheckTaskStatus == 0)
	{
	    pthread_exit(NULL);/*����ѭ����ͷ�˳�����֤����Դ�������ͷ�*/
	}
	else
	{
	    pollCount++;

	    /*
	     *�˴�Ϊ�����ź��������˲���
	     */
	    uttSemP(SEM_PHYDETECT_NO, 0);
	    wanLinkPoll(pollCount, phyStaCounter);
#if (defined(CONFIG_USER_CFGSER_LAN) && (FEATURE_SOHO_REPEATER != FYES))
	    LanLinkPollLanCfg();
#endif
	    /*
	     *�˴�Ϊ�����ź��������˲���
	     */
	    uttSemV(SEM_PHYDETECT_NO, 0);

	    sleep(1u);/*����ȴ�*/
	}
    }
    return;
}


/**
 * ����һ���߳������WAN������״̬�������׹���ģ��
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
