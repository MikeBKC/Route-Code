#include "uttMachine.h"
#if (UTT_SMART_UI == FYES)
#if (CJSON == FYES)
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "webs.h"
#include "utils.h"
#include "uttfunction.h"
#include "comExeFun.h"
#include "translate.h"
#include "um.h"
#include "internet.h"
#include "wireless.h"
#include "ifmisc.h"
#include "cJSON.h"
#include <iconv-user.h>

#define USER_MAC_PROC_FILE "/proc/net/statsPerIp"
#define MAX_LINE_LEN        200  
#define MIN_ACTIVE_TIME 1356998400u

#define DEBUG_SWITCh 0
#if DEBUG_SWITCh
#define NETWORKSTATUS_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define NETWORKSTATUS_PRINTF(fmt, args...)
#endif

#define TEN_MINUTES (10 * 60)
#define TW_HOURS (12 * 60 * 60)
/*****************************��������  start*************************************/
extern int getWanGwByIndex(int wanIndex, char* gw);
extern int getpppoeStatus(int wannum);
extern int doSystem(char_t *fmt, ...);
static int getWanGetWay(int eid, webs_t wp, int argc, char_t **argv);

extern void formDefineNetworkStatus(void);
static void ajaxResponse(webs_t wp, char_t* respType, cJSON *respJSON);
static void formNetworkStatus(webs_t wp, char_t *path, char_t *query);
static void formWanInfo(webs_t wp, char_t *path, char_t *query);
static void formWifiInfo(webs_t wp, char_t *path, char_t *query);
static void formLanInfo(webs_t wp, char_t *path, char_t *query);
static void formLanArpCollisionDetectInfo(webs_t wp, char_t *path, char_t *query);
#if (APP_POLICY_REMOTE_UPDATE == FYES)
static void formAppCheck(webs_t wp, char_t *path, char_t *query);
static void formAppUpdate(webs_t wp, char_t *path, char_t *query);
#endif
static int getUserNum(void);
static int getLanInfo(cJSON *json);
static int getWanInfo(cJSON *json);
static int getWifiInfo(cJSON *json);
static int getLanCollisionDetectInfo(cJSON *json);
#if (SHOW_ALL_WIRE_MAC == FYES)
extern int getAllUserNum(void);
#endif
extern int getbridgeusers(void);
/*****************************��������  end***************************************/

/********************************************************************
 * ������ ajaxResponse
 * ���ܣ� �����������д�ؿͻ���
 * ������ 2013-10-16
 * ������ respType->httpͷ��content-type���ͣ�respStr->���ص�����
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void ajaxResponse(webs_t wp, char_t* respType, cJSON *respJSON)
{
    char *respStr;

    respStr = cJSON_Print(respJSON);/*��ȡjson�����װ����Ϣ*/
    cJSON_Delete(respJSON);/*����json����*/
    /*����ajax���ؽ��*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: %s\nPragma: no-cache\nCache-Control: no-cache\n\n"),respType);
    websWrite(wp, T("%s"), respStr);
    free(respStr);/*�ͷ�json��װ���ַ�����Ϣ*/
    
    websDone(wp, 200);
}

/*
 *	wireless_date_check	-	�ж��Ƿ���������Чʱ�䲢������
 *
 * ����ֵ��
 *	status	
 *	0			-	������Чʱ����
 *	1			-	����Чʱ����
 *	-1			-	ϵͳʱ�䲻��
 */
int wireless_date_check(void)
{
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	unsigned int tmCurrent = 0u;
	unsigned int daytime = 0u, minute1 = 0u, minute2 = 0u, hour1 = 0u, hour2 = 0u, wday = 0u;
        unsigned int timestart = 0u, timestop = 0u;
	int status = 0;

	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
	tmCurrent = time(NULL);

	if (ProfInstIsFree(prof) != 0)
	{
	    status = -1;
	}
	else if (prof->mBaseCf.WlanTimeEn == FUN_DISABLE)
	{
	    status = 1;
	}
	else if (tmCurrent < MIN_ACTIVE_TIME)
		status = -1;
	else {
		/*��ȡ���ڣ�ʱ����*/
		wday = (4 + tmCurrent / 86400) % 7; 
		daytime = tmCurrent % 86400;


		/*�����촦��Ϊ������*/
		if (wday == 0)
			wday = 7;

		/*�жϴ����Ƿ�����Чʱ����*/
		if (((1u << (7u - wday)) & prof->mBaseCf.day)) {
			/*�ж��ǹ�����ʱ�λ�����ĩʱ��*/
			if ((wday >= 1) && (wday <= 5)) {
				sscanf(prof->mBaseCf.weekdayTimeStart, "%u:%u", &hour1, &minute1);/*����ʱ��*/
				sscanf(prof->mBaseCf.weekdayTimeStop, "%u:%u", &hour2, &minute2);/*����ʱ��*/
			} else {
				sscanf(prof->mBaseCf.weekendTimeStart, "%u:%u", &hour1, &minute1);/*����ʱ��*/
				sscanf(prof->mBaseCf.weekendTimeStop, "%u:%u", &hour2, &minute2);/*����ʱ��*/
			}
                        timestart = hour1 * 3600 + minute1 * 60;
                        timestop = hour2 * 3600 + minute2 * 60 + 60;
			/*�ж��Ƿ��ڿ�ʼʱ�䵽����ʱ��֮��*/
			if(timestart <= timestop)
			{
			    if ((daytime >= timestart ) && (daytime <= timestop))
			    {
				status = 1;
			    }
			}
			else{/*��һ�쿪ʼ���ڶ������*/
			    if((daytime >= timestart) || (daytime <= timestop))
			    {
				status = 1;
			    }
			}
		}
	}
	return status;
}

/********************************************************************
 * ������ getWifiInfo
 * ���ܣ� ��ȡwifi��״̬��Ϣ������װ��json��ʽ������
 * ������ 2013-10-16
 * ������ wifiInfo->json��ʽ��װ��wifi״̬��Ϣ
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int getWifiInfo(cJSON *json)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
#if(WIRELESS_5G == FYES)
    WirelessProfile *prof_5g= NULL;
#endif
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
#if(WIRELESS_5G == FYES)
    prof_5g = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
    
    char wifi_5g_switch[4];
    char wifi_5g_encryption[UTT_SEL_AUTHMODE_LEN+1];
#endif
    char wifi_switch[4];
    char wifi_encryption[UTT_SEL_AUTHMODE_LEN+1];
    char time_switch[4];
    char wireless_effect[4];

    if ((prof != NULL) && (prof->head.active == 1)) {
	strcpy(wifi_switch,"on");
    } else {
	strcpy(wifi_switch,"off");
    }
    if (prof->mBaseCf.WlanTimeEn == FUN_ENABLE){
	strcpy(time_switch, "on");
    } else {
	strcpy(time_switch, "off");
    }
    if (wireless_date_check() == 0){
	strcpy(wireless_effect, "0");
    } else {
	strcpy(wireless_effect, "1");
    }
    strcpy(wifi_encryption,prof->mSafeCf.ap.AuthMode.EncrypType);
    cJSON_AddStringToObject(json,"wifi_switch",(const char*)wifi_switch);
    cJSON_AddStringToObject(json,"wifi_encryption",(const char*)wifi_encryption);
    cJSON_AddStringToObject(json,"wireless_time",(const char*)time_switch);
    cJSON_AddStringToObject(json,"wireless_effect",(const char*)wireless_effect);
#if (WIRELESS_5G == FYES)
    if ((prof_5g != NULL) && (prof_5g->head.active == 1)) {
	strcpy(wifi_5g_switch,"on");
    } else {
	strcpy(wifi_5g_switch,"off");
    }
    strcpy(wifi_5g_encryption,prof_5g->mSafeCf.ap.AuthMode.EncrypType);
    cJSON_AddStringToObject(json,"wifi_5g_switch",(const char*)wifi_5g_switch);
    cJSON_AddStringToObject(json,"wifi_5g_encryption",(const char*)wifi_5g_encryption);
#endif

    NETWORKSTATUS_PRINTF("func:%s,line %d \n",__func__,__LINE__);
    return 0;
}

/********************************************************************
 * ������ formWifiInfo
 * ���ܣ� ��Ӧҳ�������wifi״̬��Ϣ
 * ������ 2013-10-16
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formWifiInfo(webs_t wp, char_t *path, char_t *query)
{
    char_t *respType = "application/json";
    cJSON *json;

    json = cJSON_CreateObject();/*����json�������ڷ�װwifi��Ϣ*/
    getWifiInfo(json);/*��ȡwifi��Ϣ����װ��json������*/
    NETWORKSTATUS_PRINTF("func:%s,line %d \n",__func__,__LINE__);
    ajaxResponse(wp,respType,json);/*��Ӧ��ҳ��*/
}

/********************************************************************
 * ������ getWanInfo
 * ���ܣ� ���ػ�ȡwan����Ϣ
 * ������ 2013-10-16
 * ������ wanInfo->��json��ʽ��װwan����Ϣ
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int getWanInfo(cJSON *json)
{
    InterfaceProfile *profIf = NULL;
    Uint32 state = 0U;
    int flag = 0;
    Uint32 connStatus = 0U;/*Ĭ��ֵ0��ʾwan�ڶϿ�,������ڶ�wan�ڣ���һ������������1*/
    Uint32 connType = 0U;/*Ĭ��Ϊ0��ʾδ֪���룬1��ʾDHCP���룬2��ʾ�̶����룬3��ʾPPPoE*/
    Uint32 phylinkStatus = 0U;/*������·����״̬��Ĭ��Ϊ0��ʾ����δ����*/
    char *wanName;
#if (EASY_TASK == FYES)
    MibProfileType mibType = MIB_PROF_EASY_TASK; 
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
	    NETWORKSTATUS_PRINTF("func:%s,line %d,wanName=%s \n",__func__,__LINE__,wanName);
	    if((flag = getIfIp(wanName,NULL)) == 0) {/*���DHCP��ȡ����IP,����1*/
		state = 1U;/*�����ȡ����IP�����Բ�������������״̬��Ȼ��1*/
	    }
	    NETWORKSTATUS_PRINTF("func:%s,line %d,state =%d,flag =%d \n",__func__,__LINE__,state,flag);
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

    NETWORKSTATUS_PRINTF("func:%s, line %d ,connStatus=%d, connType=%d, phylinkStatus=%d\n",__func__,__LINE__,connStatus,connType,phylinkStatus);
    cJSON_AddNumberToObject(json,"isWanConn",(int)connStatus);
    cJSON_AddNumberToObject(json,"connType",(int)connType);
    cJSON_AddNumberToObject(json,"phylinkStatus",(int)phylinkStatus);
    NETWORKSTATUS_PRINTF("func:%s,line %d \n",__func__,__LINE__);

    return 0;
}

/********************************************************************
 * ������ formWanInfo
 * ���ܣ� ��Ӧҳ�������wan����Ϣ
 * ������ 2013-10-16
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formWanInfo(webs_t wp, char_t *path, char_t *query)
{
    char_t *respType = "application/json";
    cJSON *json;

    json = cJSON_CreateObject();
    getWanInfo(json);
    NETWORKSTATUS_PRINTF("func:%s,line %d \n",__func__,__LINE__);
    ajaxResponse(wp,respType,json);
}

/********************************************************************
 * ������ getUserNum
 * ���ܣ� ��ȡ��ǰ�û�����
 * ������ 2013-10-16
 * ������ ��
 * ���أ� �û�����
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int getUserNum(void)
{
    FILE *fp;
    int userNum = 0;
    int tmp = 0;

    doSystem("wc %s -l > /tmp/userNum 2>/dev/null",USER_MAC_PROC_FILE);
    fp = fopen("/tmp/userNum","r");

    if (fp != NULL) {
	fscanf(fp,"%d",&tmp);
	if (tmp > 0) {
	    userNum = tmp - 1;
	}
    }

    fclose(fp);
    return userNum;
}

/********************************************************************
 * ������ getLanInfo
 * ���ܣ� ���ػ�ȡlan����Ϣ
 * ������ 2013-10-16
 * ������ lanInfo->��json��ʽ��װlan����Ϣ
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int getLanInfo(cJSON *json)
{
    int userNum;
    unsigned int index;
    int lanStatus[4];
    memset(lanStatus,0,sizeof(lanStatus));/*lan��Ĭ��״̬��Ϊδ����*/
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if(profSys->brideg_mode_flag == 0 )
    {
#endif
#if (SHOW_ALL_WIRE_MAC == FYES)
        userNum = getAllUserNum();
#else
        userNum = getUserNum();
#endif
#if (UTT_BRIDEG_MODE_FLAG == FYES)
    }else{
        userNum = getbridgeusers();
    }
#endif
#if (UTT_NV_A755W == FYES)
    for (index = 1U; index <= 2U; index++) {/*������ȡÿ��lan�ڵ�����״̬*/
	if (get_lan_status((int)index) == 1) {
	    lanStatus[index-1U] = 1;
	}
    }
    cJSON_AddNumberToObject(json,"isconn_lan1",lanStatus[0]);
    cJSON_AddNumberToObject(json,"isconn_lan2",lanStatus[1]);
    cJSON_AddNumberToObject(json,"isconn_lan3",0);
    cJSON_AddNumberToObject(json,"isconn_lan4",0);
    cJSON_AddNumberToObject(json,"devices_cnt",userNum);
    for (index = 0U; index < 2U; index++) {
	NETWORKSTATUS_PRINTF("func:%s,line %d,lanStatus[%d]=%d \n",__func__,__LINE__,index,lanStatus[index]);
    }

#else
#if (CONFIG_BOARD_MTK7620_X == 1)
/*for A652GW*/
    for (index = 0U; index <= 3U; index++) {/*������ȡÿ��lan�ڵ�����״̬*/
	if (get_lan_status((int)index) == 1) {
	    lanStatus[index] = 1;
	}
    }


    cJSON_AddNumberToObject(json,"isconn_lan1",lanStatus[1]);
    cJSON_AddNumberToObject(json,"isconn_lan2",lanStatus[2]);
    cJSON_AddNumberToObject(json,"isconn_lan3",lanStatus[3]);
#else
    for (index = 1U; index <= 4U; index++) {/*������ȡÿ��lan�ڵ�����״̬*/
	if (get_lan_status((int)index) == 1) {
	    lanStatus[index-1U] = 1;
	}
    }


    cJSON_AddNumberToObject(json,"isconn_lan1",lanStatus[0]);
    cJSON_AddNumberToObject(json,"isconn_lan2",lanStatus[1]);
    cJSON_AddNumberToObject(json,"isconn_lan3",lanStatus[2]);
    cJSON_AddNumberToObject(json,"isconn_lan4",lanStatus[3]);
#endif
    cJSON_AddNumberToObject(json,"devices_cnt",userNum);
    for (index = 0U; index < 4U; index++) {
	NETWORKSTATUS_PRINTF("func:%s,line %d,lanStatus[%d]=%d \n",__func__,__LINE__,index,lanStatus[index]);
    }
#endif
    return 0;
}

/********************************************************************
 * ������ formLanInfo
 * ���ܣ� ��Ӧҳ���Lan����Ϣ����������
 * ������ 2013-10-16
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formLanInfo(webs_t wp, char_t *path, char_t *query)
{
    char_t *respType = "application/json";
    cJSON *json;

    json = cJSON_CreateObject();
    getLanInfo(json);
    NETWORKSTATUS_PRINTF("func:%s,line %d \n",__func__,__LINE__);
    ajaxResponse(wp,respType,json);
}
/********************************************************************
 * ������ formNetworkStatus
 * ���ܣ� ��Ӧ��ҳ���󣬻�ȡ������Ҫ�����ݲ�����
 * ������ 2013-10-16
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formNetworkStatus(webs_t wp, char_t *path, char_t *query)
{
    cJSON *json;/*���챾�������json����*/
    char_t *respType = "application/json";

    json = cJSON_CreateObject();/*����json����*/

    getWifiInfo(json);
    getWanInfo(json);
    getLanInfo(json);
    getLanCollisionDetectInfo(json);

    NETWORKSTATUS_PRINTF("-----------------func:%s,line %d ---------------\n",__func__,__LINE__);
    ajaxResponse(wp,respType,json);
}


/********************************************************************
 * ������ getWanGetWay
 * ���ܣ� ��ȡwan������
 * ������ 2013-10-23
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int getWanGetWay(int eid, webs_t wp, int argc, char_t **argv)                                  
{                                                                                                     
    int ret = 0, wanIndex;                                                                            
    char if_addr[16];                                                                                 
    if (ejArgs(argc, argv, T("%d"), &wanIndex) == 1) {/*���ȶ�ȡwan������*/                           
        if ( wanIndex <= getWanSlotCount()) {                                                         
            if ((ret = getWanGwByIndex(wanIndex, if_addr)) == 0) {/*����wan�������ҵ�wan������*/          
                websWrite(wp, T("%s"), if_addr);                                                              
            } else{                                                                                       
                websWrite(wp, T("%s"),"");                                                                       
            }                                                                                             
        }
    }                                                                                                 
    return ret;                                                                                       
}                                                                                                     
/********************************************************************
 * ������ getLanCollisionDetectInfo
 * ���ܣ� ���ػ�ȡ����ARP��ͻ��̽����Ϣ
 * ������ 2014-7-8
 * ������ ��json��ʽ��װ����ARP��ͻ��̽����Ϣ
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static int getLanCollisionDetectInfo(cJSON *json)
{
    FILE *fp;
    char line[200], *timeP,*ipP,*macP;
    char ip[16],mac[18];
    unsigned long time;
    time_t cur_time;
    struct tm arp_collision_tm;
    unsigned int flag = 0U, count = 0U;/*count��ʾʮ��������ƭ����*/
    char arp_collision_time[20] = {0};
    struct timeval tv;

    if (0 == gettimeofday(&tv,NULL)) {
	cur_time = tv.tv_sec;
    }

    fp = popen("logread", "r");
    if (fp == NULL) {
	perror("popen logread failed");
	cJSON_AddNumberToObject(json,"isCollided",0);
	return 1;
    }
    while (!feof(fp)) {
	memset(line,0,sizeof(line));
	fgets(line, sizeof(line), fp);
	timeP = strstr(line,"TIME");
	if (timeP != NULL) {
	    memset(ip,0,sizeof(ip));
	    memset(mac,0,sizeof(mac));
	    sscanf(timeP+5,"%lu",&time);
	    if ((unsigned long)cur_time - time >= TEN_MINUTES) {
		continue;
	    }
	    count++;
	    ipP = strstr(timeP,"IP");
	    macP = strstr(ipP,"MAC");
	    strncpy(ip,ipP + 3, macP - ipP - 4);
	    strncpy(mac,macP+4,17);
	    flag = 1U;
	}
    }
    pclose(fp);
    if (flag) {
	if (gmtime_r(&time,&arp_collision_tm) == NULL) {
	    cJSON_AddNumberToObject(json,"isCollided",0);
	    perror("gmtime_r error");
	    return 1;
	}
	snprintf(arp_collision_time, 20, "%d-%02d-%02d %02d:%02d:%02d",
		(1900 + arp_collision_tm.tm_year), (1 + arp_collision_tm.tm_mon), 
		arp_collision_tm.tm_mday,arp_collision_tm.tm_hour, arp_collision_tm.tm_min,
		arp_collision_tm.tm_sec);
	cJSON_AddNumberToObject(json,"isCollided",flag);
	cJSON_AddStringToObject(json,"arp_collision_time",(const char*)arp_collision_time);
	cJSON_AddStringToObject(json,"IP",(const char*)ip);
	cJSON_AddStringToObject(json,"MAC",(const char*)mac);
    }
    return 0;
}

/********************************************************************
 * ������ formLanArpCollisionDetectInfo
 * ���ܣ� ��Ӧҳ�������ARP��ͻ̽����Ϣ����������
 * ������ 2014-7-8
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formLanArpCollisionDetectInfo(webs_t wp, char_t *path, char_t *query)
{
    char_t *respType = "application/json";
    cJSON *json;

    json = cJSON_CreateObject();
    getLanCollisionDetectInfo(json);
    NETWORKSTATUS_PRINTF("func:%s,line %d \n",__func__,__LINE__);
    ajaxResponse(wp,respType,json);
}
#if (APP_POLICY_REMOTE_UPDATE == FYES)
/********************************************************************
 * ������ formAppUpdate
 * ���ܣ� �������
 * ������ 2014-8-21
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formAppUpdate(webs_t wp, char_t *path, char_t *query)
{
    char_t *appTypeStr = NULL;/*��Ҫ���µ�App����*/
    int appType = 0;
    SystemProfile *profSys = NULL;
    char_t *respType = "application/json";
    cJSON *json = NULL;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    json = cJSON_CreateObject();

    appTypeStr = websGetVar(wp, T("appType"), T("NONE"));

    if (strcmp(appTypeStr,"NONE") != 0) {
	appType = atoi(appTypeStr);
	/*������ڸ��»��Ѹ��³ɹ����򲻵��ø��³���*/
	if ((profSys->appUpdateStatus[appType] != APP_UPDATING) &&
	    (profSys->appUpdateStatus[appType] != APP_UPDATE_SUCCESS)) {
	    doSystem("uttAppUpdateTool %d &",appType);
	}
#if 0
	if (profSys->appUpdateStatus[appType] == APP_UPDATE_SUCCESS) {
	    switch (appType) {
	    case SOFT_UDTYPE:
		jsonFileName = "firmwareUpdateFile"; 
		break;
	    case ADFILTER_UDTYPE:
		jsonFileName = "adfilterUpdateFile"; 
		break;
	    default:
		break;
	    }
	    doSystem("rm -rf %s &",jsonFileName);
	}
#endif
	cJSON_AddNumberToObject(json, "upStatus", profSys->appUpdateStatus[appType]);
    } else {
	cJSON_AddNumberToObject(json, "upStatus", APP_UPDATE_FAILED);
    }
    ajaxResponse(wp,respType,json);
}

/********************************************************************
 * ������ formAppCheck
 * ���ܣ� ������¼��
 * ������ 2014-8-11
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formAppCheck(webs_t wp, char_t *path, char_t *query)
{
    SystemProfile *profSys = NULL;
    char_t *appTypeStr = NULL;/*��Ҫ�����µ�App����*/
    int appType = 0;
    char_t *respType = "application/json";
    cJSON *json = NULL, *root = NULL, *fld = NULL, *rnJson;
    char curSv[3];
    int curBt;
    time_t curTime;
    struct timeval tv;
    static unsigned long lastTime;
    unsigned int start = 0U;
    int stopAjax = 0;
    unsigned int countUpdate = 0;/*��¼��Ҫ���µ�Ӧ�ø���*/
    char_t *action;
    int isWanConn = 0;
    char *rnStr;

#if (ADFILTER == FYES)
    AdfilterProfile *adfilterProfile = NULL;
    adfilterProfile = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER, 0);
#endif
    if (0 == gettimeofday(&tv,NULL)) {
	curTime = tv.tv_sec;/*��¼��ǰʱ��*/
    }

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    appTypeStr = websGetVar(wp, T("appType"), T("NONE"));

    NETWORKSTATUS_PRINTF("-----------------func:%s,line %d ,appTypeStr=%s---------------\n",__func__,__LINE__,appTypeStr);
    json = cJSON_CreateObject();
    getWanInfo(json);
    isWanConn = cJSON_GetObjectItem(json,"isWanConn")->valueint;/*��ȡWAN������״̬*/
    cJSON_Delete(json);/*����json����*/
    json = cJSON_CreateObject();

    if (strcmp(appTypeStr,"NONE") != 0) {
	appType = atoi(appTypeStr);
    } else {
	for (appType = 0; appType < MAX_UDTYPE; appType++) {
#if (ADFILTER == FYES)
	    if (appType == ADFILTER_UDTYPE) {/*������*/
		/*����δ������������Ϊ�Զ����,������*/
		if (adfilterProfile->head.active == FALSE || adfilterProfile->updateType == 0) {
		    continue;
		}
	    }
#endif
	    if (profSys->appUpdateStatus[appType] == APP_INITIAL) {
		    start = 1U;/*�Ƿ����������̵ı�־*/
		    break;
	    } else if (profSys->appUpdateStatus[appType] == APP_STATS_UNCHECKED) {/*δ��⵽����*/
		if (curTime - lastTime > TW_HOURS) {/*���������12Сʱ*/
		    lastTime = (unsigned long)curTime;
		    /*����������*/
		    start = 1U;/*�Ƿ����������̵ı�־*/
		    break;
		}
	    } else if (profSys->appUpdateStatus[appType] == APP_CHECK_FAILED) {/*���ʧ��*/
		if ((isWanConn == 1) && (curTime - lastTime > TEN_MINUTES)) {/*wan�����ӳɹ��Ҽ��������10����*/
		    lastTime = (unsigned long)curTime;
		    start = 1U;/*�Ƿ����������̵ı�־*/
		    break;
		}
#if 0
	    } else if (profSys->appUpdateStatus[appType] >= APP_WAITING_UPDATE) {/*�и��£�֪ͨҳ��ֹͣajax���*/
#endif
	    } else {
		start = 0U;/*�Ƿ����������̵ı�־*/
	    }
	}
	root = cJSON_CreateArray();


	for (appType = 0; appType < MAX_UDTYPE; appType++) {
#if (ADFILTER == FYES)
	    if (appType == ADFILTER_UDTYPE) {/*������*/
		/*����δ������������Ϊ�Զ����,������*/
		if (adfilterProfile->head.active == FALSE || adfilterProfile->updateType == 0) {
		    continue;
		}
	    }
#endif

	    if (profSys->appUpdateStatus[appType] == APP_WAITING_UPDATE) {
		countUpdate++;
		cJSON_AddItemToArray(root,fld = cJSON_CreateObject());
		cJSON_AddNumberToObject(fld, "appType", appType);
		//cJSON_AddNumberToObject(fld, "upStatus", profSys->appUpdateStatus[appType]);
	    }
	}
	cJSON_AddNumberToObject(json, "countUpdate", countUpdate);
	if (countUpdate > 0) {
	    cJSON_AddItemToObject(json,"appList",root);
	}
	ajaxResponse(wp,respType,json);
	if (start) {
	    doSystem("uttAppCheckTool &");
	}
	return;
    }

    action = websGetVar(wp, T("action"), T("AUTO"));
    if (strcmp("AUTO",action) != 0) {//�ֶ����
	start = 1U;
    } else {
	if (profSys->appUpdateStatus[appType] == APP_INITIAL) {
		start = 1U;/*�Ƿ����������̵ı�־*/
	} else if (profSys->appUpdateStatus[appType] == APP_STATS_UNCHECKED) {/*δ��⵽����*/
	    if (curTime - lastTime > TW_HOURS) {/*���������12Сʱ*/
		lastTime = (unsigned long)curTime;
		/*����������*/
		start = 1U;/*�Ƿ����������̵ı�־*/
	    } else {/*�޸��£�����Ҫ�������*/
		start = 0U; 
	    }
	} else if (profSys->appUpdateStatus[appType] == APP_CHECK_FAILED) {/*���ʧ��*/
	    if ((isWanConn == 1) && (curTime - lastTime > TEN_MINUTES)) {/*wan�����ӳɹ��Ҽ��������10����*/
		lastTime = (unsigned long)curTime;
		start = 1U;/*�Ƿ����������̵ı�־*/
	    }
#if 0
	} else if (profSys->appUpdateStatus[appType] >= APP_WAITING_UPDATE) {/*�и��£�֪ͨҳ��ֹͣajax���*/
#endif 
	} else {
	    if ((appType == SOFT_UDTYPE) && (profSys->appUpdateStatus[appType] == APP_WAITING_UPDATE)) {/*�и���*/
		cJSON_Delete(json);/*����json����*/
		json = parseFile("firmwareUpdateFile");
		if (json == NULL) {
		    json = cJSON_CreateObject();
		    cJSON_AddStringToObject(json,"rn","��");
		}
		rnJson = cJSON_GetObjectItem(json,"rn");
		if (rnJson != NULL) {
		    rnStr = iconv_string(rnJson->valuestring,"GBK","UTF-8");;
		    cJSON_DeleteItemFromObject(json,"rn");
		    if (rnStr != NULL) {
			cJSON_AddStringToObject(json,"rn",rnStr);
			free(rnStr);
			rnStr = NULL;
		    }
		}
	    }
	    //stopAjax = 1;
	    start = 0U;/*�Ƿ����������̵ı�־*/
	}

    }
    /*������Ӧ��Ϣ*/
    cJSON_AddNumberToObject(json,"appType",appType);
    /*·����ҳ�����ݸ�״̬��ʾ��ͬ��Ϣ,�������ر�(����)��ʱ���Ĳ���*/
    cJSON_AddNumberToObject(json,"updateStatus",(int)profSys->appUpdateStatus[appType]);
    //cJSON_AddNumberToObject(json,"stopAjax",stopAjax);
    ajaxResponse(wp,respType,json);

    if (start == 0U) {/*��������������*/
	return;
    }
    /*���ü�����*/
    doSystem("uttAppCheckTool %d &",appType);

}

static void formSoftDownload(webs_t wp, char_t *path, char_t *query)
{
    char_t *appTypeStr = NULL;/*��Ҫ���µ�App����*/
    char_t *respType = "application/json";
    cJSON *json = NULL;
    char *jsonFileName;
    FILE *fp = NULL;
    int ratio = 0;
    static unsigned int lastRatio = 0,count = 0;
    static char flag = 0U,errCount = 0;

    json = cJSON_CreateObject();
    if (access("/ratio",F_OK) != 0) {/*�ļ�û�з��ʵ�*/
	cJSON_AddNumberToObject(json, "isError", 1);
	if (errCount++ == 5U) {
	    cJSON_AddNumberToObject(json, "isError", 2);
	    errCount = 0U;
	}
	cJSON_AddNumberToObject(json, "ratio", 0);
    } else {
	errCount = 0U;
	if ((fp = fopen("/ratio","r")) != NULL) {
	    fscanf(fp,"%d",&ratio);
	    if (ratio == lastRatio) {
		count++;
		if (count > 5) {
		    /*�����˴���5�Σ��õ������ݶ���һ����,����Ϊ�����������*/
		    cJSON_AddNumberToObject(json, "isError", 2);
		    if  (count == 15) {/*����ٹ�ʮ���ӣ�������û�лָ�����ɱ������wget*/
			doSystem("killall wget &");
		    }
		}
	    } else {
		lastRatio = ratio;
		count = 0;
		cJSON_AddNumberToObject(json, "isError", 0);
	    }
	    cJSON_AddNumberToObject(json, "ratio", ratio);
	    fclose(fp);
	    fp = NULL;
	    if (ratio == 100) {
		doSystem("rm -rf /ratio &");
	    }
	} else {
	    if (flag == 1U) {
		cJSON_AddNumberToObject(json, "isError", 2);
	    } else {
		flag = 1U;
	    }
	}
    }
    ajaxResponse(wp,respType,json);
}
#endif
/********************************************************************
 * ������ formDefineNetworkStatus
 * ���ܣ� ����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 * ������ 2013-10-15
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
void formDefineNetworkStatus(void)
{
    doSystem("killall klogd & 1>/dev/null 2>1&;klogd &");
    websFormDefine(T("formNetworkStatus"), formNetworkStatus);
    websFormDefine(T("formWanInfo"), formWanInfo);
    websFormDefine(T("formWifiInfo"), formWifiInfo);
    websFormDefine(T("formLanInfo"), formLanInfo);
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    websFormDefine(T("formSoftDownload"), formSoftDownload);
#endif
#if 1
    websAspDefine(T("getWanGetWay"), getWanGetWay);
#endif
    websFormDefine(T("formLanArpCollisionDetectInfo"), formLanArpCollisionDetectInfo);
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    websFormDefine(T("formAppCheck"), formAppCheck);
    websFormDefine(T("formAppUpdate"), formAppUpdate);
#endif
}
#endif
#endif
