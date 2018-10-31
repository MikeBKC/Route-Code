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
    /*��������գ�Ȼ������Ƿ������ؽ�*/
    doSystem("iptables -t filter -F adfilter_chain >/dev/null 2>&1;iptables -t filter -D INPUT -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t filter -D FORWARD -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t filter -X adfilter_chain >/dev/null 2>&1;iptables -t nat -D PREROUTING -m uttdev --is-lan-in -p tcp -d %s --dport 80 -j REDIRECT --to-ports %d >/dev/null 2>&1;killall adfilter >/dev/null 2>&1",DNS_REDIRECT_IP,PROXY_PORT);
    if (flag) {
	doSystem("iptables -t filter -N adfilter_chain >/dev/null 2>&1;iptables -t filter -A INPUT -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t filter -A FORWARD -m uttdev --is-lan-in -p udp  --dport 53 -j adfilter_chain >/dev/null 2>&1;iptables -t nat -A PREROUTING -m uttdev --is-lan-in -p tcp -d %s --dport 80 -j REDIRECT --to-ports %d >/dev/null 2>&1;adfilter & >/dev/null 2>&1",DNS_REDIRECT_IP,PROXY_PORT);
    }
}
/********************************************************************
* ������ setAdfilterRule
* ���ܣ� ��������Ч����
* ������ 2014-06-11
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� �Գ�
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
* ������ adfilterProfileChange
* ���ܣ� ���������ñ仯 ��Ч����
* ������ 2014-06-11
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� �Գ�
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
		if (newProfileDataPtr->updateType == 0) {/*������������ʱ��*/
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
* ������ funInitAdfilter
* ���ܣ� ������ ��ʼ����ں���
* ������ 2014-06-11
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� �Գ� 
********************************************************************/
void funInitAdfilter(void)
{
    AdfilterProfile *adfilterProfile;
    MibProfileType mibType = MIB_PROF_ADFILTER;

    registerForProfileUpdates(MIB_PROF_ADFILTER, (Pointer)NULL, (ProfileUpdateFnc)adfilterProfileChange);/*��������*/

    adfilterProfile = (AdfilterProfile*)ProfGetInstPointByIndex(mibType, 0);
    
    if (adfilterProfile != NULL && adfilterProfile->head.active == TRUE) {
	initAdfilterChain(TRUE);
	setAdfilterRule(adfilterProfile);
	if (adfilterProfile->updateType == 0) {/*������������ʱ��*/
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
    Uint32 connStatus = 0U;/*Ĭ��ֵ0��ʾwan�ڶϿ�,������ڶ�wan�ڣ���һ������������1*/
    Uint32 connType = 0U;/*Ĭ��Ϊ0��ʾδ֪���룬1��ʾDHCP���룬2��ʾ�̶����룬3��ʾPPPoE*/
    Uint32 phylinkStatus = 0U;/*������·����״̬��Ĭ��Ϊ0��ʾ����δ����*/
    char *wanName;
    MibProfileType mibType = MIB_PROF_EASY_TASK; 
    char if_addr[16] = {0};
#if (EASY_TASK == FYES)
    EasyTaskProfile *prof = NULL; 
#endif

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    phylinkStatus = (Uint32)get_phylink_status(1);/*��ȡwan1��״̬*/

#if (EASY_TASK == FYES)
    prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
#endif

    switch(profIf->ether.connMode) {
	case ETH_CONN_DHCP:
	    connType = 1U;/*DHCPģʽ����*/
	    wanName = getWanIfName(1);
	    if((flag = getIfIp(wanName,if_addr)) == 0) {/*���DHCP��ȡ����IP,����1*/
		state = 1U;/*�����ȡ����IP�����Բ�������������״̬��Ȼ��1*/
	    }
	    state &= phylinkStatus;/*������߶Ͽ�����DHCP��ȡ����IP��ַ������Ϊ0*/ 	    
	    break;
	case ETH_CONN_STATIC:
	    state = phylinkStatus;/*����������������������õĹ̶�IP��������������ȻΪ1*/
	    connType = 2U;
	    break;
	case ETH_CONN_PPPOE:
	default:
#if (EASY_TASK == FYES)
	    if (ProfInstIsFree(prof) == 0) {
		state = prof->ConnStatus;
	    }
#else
	    state = getpppoeStatus(1);/*��ȡPPPoE������״̬*/
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
/*�ú���ÿ����ִ��һ��*/
int policyUpdateCheck(AdfilterProfile *adfilterProfile,unsigned char flag)
{
    SystemProfile *profSys = NULL;
    int status;

    if (!isWanConnected()) {/*���wan�ڲ�ͨ���߼������Ϊ0����ֱ�ӷ���*/
	return 1;
    }
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

    DEBUG_PRINTF("---func:%s-line:%d----status=%d--\n",__func__,__LINE__,status);
    if (flag == 0U) {/*ִ�м�����*/
	doSystem("uttAppCheckTool %d &",ADFILTER_UDTYPE);
    } else {
	status = profSys->appUpdateStatus[ADFILTER_UDTYPE];
	if (status == APP_WAITING_UPDATE) {/*�а汾����*/
	    doSystem("uttAppUpdateTool %d &",ADFILTER_UDTYPE);
	} else {
	    if (flag == 10U) {/*���������������⣬��ɽ�������*/
		doSystem("killall uttAppUpdateTool &; killall wget &");
	    }
	}
    }
    DEBUG_PRINTF("---func:%s-line:%d--timeCheckUpdatePoint=%d,policyCounter=%d----\n",__func__, __LINE__,timeCheckUpdatePoint,(policyCounter % (SECONDS_OF_DAY * adfilterProfile->checkCycle)));

    return 0;
}
#endif
