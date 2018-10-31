#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "profacce.h"
#include "translate.h"
#include "syslog.h"
#if (NOTICE == FYES)
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define DAILYNOTICE	    0	
#define TIMENOTICE	    1	
#define DNSNOTICE	    2

#define URL_NOTICE		    "Notice.asp"
#define URL_PPPOENOTICE		    "PPPoENotice.asp"
#define URL_DNSNOTICE		    "DnsFilter_Notice.asp"

#define  STR_CHECKED  "on"

#if (DEBUG_DNS_NOTICE == FYES)
#define dns_debug_print(fmt, args...)    debug_print(&(nvramProfile->uttDebug.dnsDebug), fmt, ## args)
#else
#define dns_debug_print(fmt, args...)
#endif

static void formConfigNoticeConfig(webs_t wp, char_t *path, char_t *query);
static void formTimeNoticeConfig(webs_t wp, char_t *path, char_t *query);
static void formDnsNoticeConfig(webs_t wp, char_t *path, char_t *query);
static int getNoticeData(int eid, webs_t wp, int argc, char **argv);
static int getTimeNoticeData(int eid, webs_t wp, int argc, char **argv);
static int getDnsNoticeData(int eid, webs_t wp, int argc, char **agrv);
static int getNoticeContentData(int eid, webs_t wp, int argc, char **argv);
static int readConfigFromFile_skip(char *title, char *content, char *skiptime, char *skipurl, const char *path);
static int readConfigFromFile(char *title, char *content, const char *path);
static int getNoticeReceive(int eid, webs_t wp, int argc, char **argv);


#if (SYS_LOG_INFO == FYES)
#ifdef CONFIG_UTT_NOTICE_LOG
/*
 * isNoticeLogEn	-	�ж�ͨ����־�Ƿ���
 *
 * ����ֵ:
 *	ret		-	�Ƿ�����־
 *	1		-	����
 *	0		-	�ر�
 *
 */
static int isNoticeLogEn(void)
{
	MibProfileType mibType = MIB_PROF_SYS_LOG_INFO;
	SysLogInfoProfile *prof = NULL;
	int ret = 0;

	prof = (SysLogInfoProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (ProfInstIsFree(prof) == 0) 
		if (prof->NoticeLogEnable == LOG_ENABLE) 
			ret = 1;

	return ret;
}
#endif
#endif

/*
 * ���ܣ�
 * �жϷ��ʵ�ҳ���Ƿ�ͨ��ҳ�棬�����ݲ�ͬҳ������ش���
 * ���룺
 * wp	-   ����goahead�û���Ϣ�ṹ��
 * ����ֵ��
 * 1	-   ����ͨ��ҳ��
 * 0	-   ���ʷ�ͨ��ҳ��
 */
extern int isNoticeWeb(webs_t wp) {
    int ret = 1;
    char_t *relIp = NULL;
    /*ͨ��url��¼������ip����ֹ���������ͷ���ͨ��ҳ��Ĳ�ͬ��
     * ��pppoe������������lan������
     * �����ͨ��ҳ�������lan ip*/
    relIp = websGetVar(wp, T("ip"), T(""));

    if(strcmp(relIp, "") == 0)
    {
	relIp = wp->ipaddr;
    }
    if (wp->path != NULL)
    {
	if(strcmp(wp->path, "/"NOTICE_HTML_1_NAME) == 0) { /* �ճ�ͨ�� */
	    if (ipsetTestIpStr(NOTICE_OK_GRP, relIp) == 0) { /* ����OK����, ����OK�� */
		ipsetAOrDIpStr(NOTICE_OK_GRP, relIp, IPSET_MEM_ADD);/*��¼����ͨ����*/
		/*���record��
		 *��ո������ʹ�������汾��ͨ����û��ӳ�ͨ��
		 *�����Լ����ڴ����ġ�ͬʱ����webserverѹ��*/
		ipsetFlushGrp(NOTICE_R_GRP);
	    }
#if (SYS_LOG_INFO == FYES)
#ifdef CONFIG_UTT_NOTICE_LOG
	    if (isNoticeLogEn())
		syslog(LOG_WARNING, "Give notice to user:%s", relIp);
#endif
#endif
	}
#if (PPPOE_ACCOUNT == FYES)
	else if(strcmp(wp->path, "/"NOTICE_HTML_2_NAME) == 0) { /*pppoe ͨ�� */
	    if (ipsetTestIpStr(POE_EXPIRING_ACC, relIp) != 0)
	    {/*������ͨ��*/
		if (ipsetTestIpStr(POE_NT_OK, relIp) == 0)
		{ 
		    ipsetAOrDIpStr(POE_NT_OK, relIp, IPSET_MEM_ADD);/*���ӵ���ͨ��*/
		    /*���record��
		     *��ո������ʹ�������汾��ͨ����û��ӳ�ͨ��
		     *�����Լ����ڴ����ġ�ͬʱ����webserverѹ��*/
		    ipsetFlushGrp(POE_NT_R);
		}
	    }

#if (SYS_LOG_INFO == FYES)
#ifdef CONFIG_UTT_NOTICE_LOG
	    if (isNoticeLogEn())
		syslog(LOG_WARNING, "Give pppoe notice to user:%s", relIp);
#endif
#endif

	}
#endif
#if (NET_SNIPER == FYES)
	else if(strcmp(wp->path, "/"NOTICE_HTML_4_NAME) == 0) { /*netsniper ͨ�� */
	}
#endif
	else
	{
	    ret = 0;
	}
    }
    else
    {
	ret = 0;
    }
    return ret;
}
/********************************************************************
 * ������ fromDefineMacTable
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2010-10-08
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� jfgu
 * ά���� jfgu
 *******************************************************************/
extern void websDefineNotice(void)
{
	websFormDefine(T("formConfigNoticeConfig"), formConfigNoticeConfig);
	websFormDefine(T("formTimeNoticeConfig"), formTimeNoticeConfig);
	websFormDefine(T("formDnsNoticeConfig"), formDnsNoticeConfig);
	websAspDefine(T("aspOutputNoticeData"), getNoticeData);
	websAspDefine(T("aspOutputTimeNoticeData"), getTimeNoticeData);
	websAspDefine(T("aspOutputNoticeContentData"), getNoticeContentData);     
	websAspDefine(T("aspOutputDnsFilterNoticeData"), getDnsNoticeData);     
	websAspDefine(T("aspNoticeReceive"), getNoticeReceive);     
}
/********************************************************************
 * ������ saveNoticeConfig 
 * ���ܣ� ����������profile
 * ������ 2012-03-28
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void saveNoticeConfig(webs_t wp,int instIndex)
{
#if 1
    char_t *enable = NULL;
    char_t *SubmitType = NULL;
    char_t *title = NULL; 
    char_t *content = NULL;
    char_t *skiptime = NULL;
    char_t *skipurl = NULL;
    
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
#if 0
    struProfAlloc *profhead = NULL;
#endif

    enable = websGetVar(wp, T("enabled"), T(""));/*ͨ�濪��*/
    SubmitType = websGetVar(wp, T("noticeType"), T(""));/*�ṩҳ�����*/
    title = websGetVar(wp, T("noticeTitle"), T(""));/*����*/
    content = websGetVar(wp, T("hidBdy"), T(""));/*֪ͨ����--δתΪҳ��֮ǰ*/ 
    skiptime = websGetVar(wp,T("skiptime"), T(""));
    skipurl = websGetVar(wp,T("skipurl"), T(""));

    SWORD_PRINTF("%s - %d, notice enable = %s, SubmitType = %s, title= %s\n", __FUNCTION__, __LINE__, enable, SubmitType, title);
    SWORD_PRINTF("%s - %d, content = %s\n", __FUNCTION__, __LINE__, content);
    SWORD_PRINTF("%s - %d, skiptime = %s, skipurl = %s\n", __func__, __LINE__, skiptime, skipurl);
#if 0
    instIndex = ProfGetInstIndexByName(mibType, noticetype);
    if(instIndex == PROFFAIL) 
    {
	setTheErrorMsgOnFree(T("�Ѵ������Ŀ"));
    }
    else
#endif
    SWORD_PRINTF("instIndex = %d\n", instIndex);
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    if (strcmp(enable, STR_CHECKED) == 0) 
	    {
		prof->head.active = 1;
	    }
	    else 
	    {
		prof->head.active = 0;
	    }
	    if(strcmp(SubmitType,"edithtml") == 0)
	    {
		prof->webSubmitType = NOTICE_WEB_EDIT;
	    }
	    else if(strcmp(SubmitType,"importhtml") == 0)
	    {
		prof->webSubmitType = NOTICE_WEB_IMPORT;
	    }
	}

    /* ������⣬����Աǩ����ͨ��������
     * etc_ro/web/Notify*.asp���ú�NOTICE_HTML_*��ʾ 
     */
    if(instIndex == DAILYNOTICE)
    {
	saveConfigToFile_skip(title, content, skiptime, skipurl, DNS_NOTIFY_HTM, NOTICE_HTML_1);
    }
    if(instIndex == TIMENOTICE)
    {
	saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_2);
    }
    if(instIndex == DNSNOTICE)
    {
	saveConfigToFile_skip(title, content, skiptime, skipurl, DNS_NOTIFY_HTM, NOTICE_HTML_3);
    }

#endif
    return;
}
/********************************************************************
 * ������ formConfigNoticeConfig 
 * ���ܣ� ͨ��ҳ�汣��ҳ�����ú���
 * ������ 2012-03-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formConfigNoticeConfig(webs_t wp, char_t *path, char_t *query)
{
#if 1
    char_t *FromAddr = NULL;
    char_t *EndAddr = NULL;
    char_t *group_day = NULL;
    char_t *group_timestart = NULL;
    char_t *group_timestop = NULL;
    char_t *group_datestart = NULL;
    char_t *group_datestop = NULL;

    struct in_addr addr_fromip, addr_endip;
    int instIndex = 0;
    const char *errMsg=NULL;

    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    FromAddr = websGetVar(wp, T("FromAddr"), T(""));/*��ʼ��ַ*/
    EndAddr = websGetVar(wp, T("EndAddr"), T(""));
    group_day=websGetVar(wp,T("day"), T(""));
    group_timestart=websGetVar(wp,T("timestart"), T(""));
    group_timestop=websGetVar(wp,T("timestop"), T(""));
    group_datestart= websGetVar(wp, T("year1"), T(""));
    group_datestop= websGetVar(wp, T("year2"), T(""));
    SWORD_PRINTF("%s - %d,FromAddr = %s, EndAddr = %s, group_day = %s, group_timestart = %s, group_timestop = %s, group_datestart = %s, group_datestop = %s\n", __FUNCTION__, __LINE__, FromAddr, EndAddr, group_day, group_timestart, group_timestop, group_datestart, group_datestop);

#if 0
    instIndex = ProfGetInstIndexByName(mibType, DAILYNOTICE);
    if(instIndex == PROFFAIL) 
    {
	setTheErrorMsgOnFree(T("�Ѵ������Ŀ"));
    }
    else
#endif
    SWORD_PRINTF("instIndex = %d\n", instIndex);
    inet_aton(FromAddr, &addr_fromip);
    inet_aton(EndAddr, &addr_endip);
    SWORD_PRINTF("%s------%d, ntohl(addr_endip.s_addr) = %d, ntohl(addr_fromip.s_addr) = %d, (ntohl(addr_endip.s_addr) - ntohl(addr_fromip.s_addr) = %d\n", __FUNCTION__, __LINE__, ntohl(addr_endip.s_addr), ntohl(addr_fromip.s_addr), (ntohl(addr_endip.s_addr) - ntohl(addr_fromip.s_addr)));
    if((ntohl(addr_endip.s_addr) - ntohl(addr_fromip.s_addr)) <= 65535)
    {
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);
	    prof->ipstart = addr_fromip.s_addr; 
	    prof->ipend = addr_endip.s_addr; 
	    prof->day= days2Byte(group_day); 
	    strcpy(prof->timestart, group_timestart); 
	    strcpy(prof->timestop, group_timestop); 
	    strcpy(prof->datestart, group_datestart); 
	    strcpy(prof->datestop, group_datestop); 
	}
    
	saveNoticeConfig(wp, DAILYNOTICE);

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/
	/*�ճ�ͨ����ok�飬��ֹ�û����߼��ϲ������ͨ���û��Ƿ���ͨ��*/
	ipsetFlushGrp(NOTICE_OK_GRP);/*mantis11374,���ok����speedweb��������ִ��*/
    }
    else
    {
	errMsg = getTransDataForC(C_LANG_INDEX_IP_NUM_MAX);
	setTheErrorMsgOnFree(errMsg);
    }
    websRedirect(wp, "Notice.asp");
#endif
    return;
}
/********************************************************************
 * ������ formTimeNoticeConfig 
 * ���ܣ� �˺����ڵ���ͨ��ҳ�汣��ҳ�����ú���
 * ������ 2012-03-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void formTimeNoticeConfig(webs_t wp, char_t *path, char_t *query)
{
#if 1
    char_t *timeidle = NULL;

    int instIndex = 1;

    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    timeidle = websGetVar(wp,T("txtimeidle"), T(""));

    SWORD_PRINTF("%s - %d, timeidle = %s\n", __FUNCTION__, __LINE__, timeidle);
#if 0
    if(strcmp(timetype,"time") == 0)
    {
	   instIndex = ProfGetInstIndexByName(mibType, TIMENOTICE); 
    }
    else  if(strcmp(timetype,"time") == 0)
    {
	   instIndex = ProfGetInstIndexByName(mibType, DNSNOTICE);
	   saveNoticeConfig(DNSNOTICE);
    }
    
    if(instIndex == PROFFAIL) 
    {
	setTheErrorMsgOnFree(T("�Ѵ������Ŀ"));
    }
    else
#endif
    SWORD_PRINTF("instIndex = %d\n", instIndex);
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);
	    prof->remainDays = (Uint32)strtoul(timeidle, NULL, 10); 

	    SWORD_PRINTF("%s - %d, timeidle = %d\n", __FUNCTION__, __LINE__, prof->remainDays);
	}
   // }
    saveNoticeConfig(wp, TIMENOTICE);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    websRedirect(wp, "PPPoENotice.asp");
#endif
    return;
}
/*********************************************************************
 * ������ formDnsNoticeConfig
 * ���ܣ� ��������ͨ�汣�溯��
 * ������ 2013-01-31
 * ������ ��
 * ����:  ��
 * ����� ��
 * ���ߣ� Jianqing.Sun
 ********************************************************************/
static void formDnsNoticeConfig(webs_t wp, char_t *path, char_t *query)
{

    struProfAlloc *profhead = NULL;

    ProfBackupByIndex(MIB_PROF_NOTICE, PROF_CHANGE_EDIT, 2, &profhead);
    saveNoticeConfig(wp, DNSNOTICE);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit(); /*save*/
    websRedirect(wp, "DnsFilter_Notice.asp");
    return;
}
/********************************************************************
 * ������ getNoticeData 
 * ���ܣ� ҳ������������
 * ������ 2012-03-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int getNoticeData(int eid, webs_t wp, int argc, char **argv)
{

    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;

    UINT32 d_temp = 0U;
    char_t data[16];
    int instIndex = 0;
    
    char_t title[NOTICE_TILTLE_LEN + 1] = {0}; 
    char_t content[NOTICE_CONTENT_LEN + 1] = {0};
    char_t skiptime[UTT_SKIP_TIME + 1] = {0}; 
    char_t skipurl[UTT_URL_LEN + 1] = {0};

    SWORD_PRINTF("%s - %d, content = %s\n", __FUNCTION__, __LINE__, content);
#if 0
    instIndex = ProfGetInstIndexByName(mibType, DAILYNOTICE);
    if(instIndex == PROFFAIL) 
    {
	setTheErrorMsgOnFree(T("error"));
    }
    else
#endif
    SWORD_PRINTF("instIndex = %d\n", instIndex);
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	readConfigFromFile_skip(title, content, skiptime, skipurl, NOTICE_HTML_1);
	SWORD_PRINTF( "==== func:%s; line:%d\n", __FUNCTION__, __LINE__ );
	SWORD_PRINTF( "==== title--read1:%s\n==== content--read1:%s\n", title, content );
	websWrite(wp, "var htmlTitle=\"%s\";",title);
	websWrite(wp, "var skiptimes=\"%s\";", skiptime);
	websWrite(wp, "var skipurls=\"%s\";", skipurl);
	websWrite(wp, "var lively=\"%d\";", prof->head.active);
	websWrite(wp, "var ipStart=\"%N\";", prof->ipstart);
	websWrite(wp, "var ipEnd=\"%N\";", prof->ipend);
	websWrite(wp, "var group_timestarts=\"%s\";", prof->timestart);
	websWrite(wp, "var group_timestops=\"%s\";", prof->timestop);
	d_temp = prof->day;
	Byte2Days(d_temp,data);
	websWrite(wp, "var group_days=\"%s\";", data);
	websWrite(wp, "var group_datestarts=\"%s\";", prof->datestart);
	websWrite(wp, "var group_datestops=\"%s\";", prof->datestop);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
#if 0
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min;i < max;i++)
    {
	prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL)
	{
	    websWrite(wp, "var ipStart=\"%N\";", prof->ipstart);
	    websWrite(wp, "var ipEnd=\"%N\";", prof->ipend);
	    websWrite(wp, "var group_timestarts=\"%s\";", prof->timestart);
	    websWrite(wp, "var group_timestops=\"%s\";", prof->timestop);
	    d_temp = prof->day;
	    Byte2Days(d_temp,data);
	    websWrite(wp, "var group_days=\"%s\";", data);
	}
    }
#endif

    //getNoticeConfig(DAILYNOTICE);

}

/********************************************************************
 * ������ getTimeNoticeData
 * ���ܣ� ȡ���ڵ���ͨ�������д��ҳ��PPPoENotice.asp
 * ������ 2012-03-30
 * ������ ��
 * ����;  ��
 * ����� ��
 * ����; 
 *******************************************************************/
static int getTimeNoticeData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;

    Uint instIndex=1U;
    char_t title[NOTICE_TILTLE_LEN + 1] = {0}; 
    char_t content[NOTICE_CONTENT_LEN + 1] = {0};
#if 0    
    instIndex = ProfGetInstIndexByName(mibType, TIMENOTICE);
    if(instIndex == PROFFAIL) 
    {
	setTheErrorMsgOnFree(T("error"));
    }
    else
#endif
    SWORD_PRINTF("instIndex = %d\n", instIndex);
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	readConfigFromFile(title, content, NOTICE_HTML_2);
	SWORD_PRINTF( "==== func:%s; line:%d\n", __FUNCTION__, __LINE__ );
	SWORD_PRINTF( "==== title--read2:%s\n==== content--read2:%s\n", title, content );
	SWORD_PRINTF("%s - %d, title = %s, content = %s\n", __FUNCTION__, __LINE__, title, content);
	websWrite(wp, "var htmlTitle=\"%s\";", title);
	websWrite(wp, "var lively=\"%d\";", prof->head.active);
	websWrite(wp, "var timeidle=\"%d\";", prof->remainDays);
    }

    return 0;
}

/********************************************************************
 * ������ getDnsNoticeData
 * ���ܣ� ��ȡ��������ͨ��ҳ������
 * ����:  2012-03-30
 * ����:  ��
 * ����:  ��
 * ���:  ��
 * ����:  Jianqing.Sun
 ********************************************************************/
static int getDnsNoticeData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *prof = NULL;

    char_t title[NOTICE_TILTLE_LEN + 1] = {0}; 
    char_t content[NOTICE_CONTENT_LEN + 1] = {0};
    char_t skiptime[UTT_SKIP_TIME + 1] = {0}; 
    char_t skipurl[UTT_URL_LEN + 1] = {0};
    
    prof = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 2);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	readConfigFromFile_skip(title, content, skiptime, skipurl, NOTICE_HTML_3);
	dns_debug_print("%s - %d, title = %s, content = %s\n",__func__,__LINE__,title,content);
	websWrite(wp, "var htmlTitle=\"%s\";", title);
	websWrite(wp, "var lively=\"%d\";", prof->head.active);
	websWrite(wp, "var skiptimes=\"%s\";", skiptime);
	websWrite(wp, "var skipurls=\"%s\";", skipurl);
    }
    return 0;
}

/********************************************************************
 * ������ getNoticeContentData 
 * ���ܣ� �������Flash�е�ͨ�����ݵ��������
 * ������ 2012-03-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int getNoticeContentData(int eid, webs_t wp, int argc, char **argv)
{
    char_t title[NOTICE_TILTLE_LEN + 1]; 
    char_t content[NOTICE_CONTENT_LEN + 1];
    char_t skiptime[UTT_SKIP_TIME + 1] = {0}; 
    char_t skipurl[UTT_URL_LEN + 1] = {0};
    char_t *write_content = NULL;
    int i = 0,j = 0;

    /* �Ӹ�ʽ���ļ��ж�ȡͨ����⣬����Աǩ����ͨ�����ݣ������ҳ�� */
    if(strcmp(&(wp->path[1]), URL_NOTICE) == 0)
    {
	readConfigFromFile_skip(title, content, skiptime, skipurl, NOTICE_HTML_1);
	SWORD_PRINTF( "==== func:%s; line:%d\n", __FUNCTION__, __LINE__ );
	SWORD_PRINTF( "==== title--read3:%s\n==== content--read3:%s\n", title, content );
    }
    if(strcmp(&(wp->path[1]), URL_PPPOENOTICE) == 0)
    {
	readConfigFromFile(title, content, NOTICE_HTML_2);
	SWORD_PRINTF( "==== func:%s; line:%d\n", __FUNCTION__, __LINE__ );
	SWORD_PRINTF( "==== title--read4:%s\n==== content--read4:%s\n", title, content );
    }
    if(strcmp(&(wp->path[1]), URL_DNSNOTICE) == 0)
    {
	readConfigFromFile_skip(title, content, skiptime, skipurl, NOTICE_HTML_3);
	SWORD_PRINTF( "==== func:%s; line:%d\n", __FUNCTION__, __LINE__ );
	SWORD_PRINTF( "==== title--read5:%s\n==== content--read5:%s\n", title, content );
    }
    SWORD_PRINTF("%s - %d, content = %s\n", __FUNCTION__, __LINE__, content);
     for(i = 0; content[i] != '\0'; i++)
     {
	 write_content = strstr(&content[i],"<br>");
	 if(write_content != NULL)
	 {
	     i = write_content - content;
	     content[i] = '\n';
	     for(j = i + 1; content[j + 3] != '\0';j++)
	     {
		 content[j] = content[j + 3];
	     }
	     content[j] = '\0';
	 }
     }
     for(i = 0; content[i] != '\0'; i++)
     {
	 write_content = strstr(&content[i],"&nbsp;");
	 if(write_content != NULL)
	 {
	     i = write_content - content;
	     content[i] = ' ';
	     for(j = i + 1; content[j + 5] != '\0';j++)
	     {
		 content[j] = content[j + 5];
	     }
	     content[j] = '\0';
	 }
     }
    SWORD_PRINTF("%s - %d, content = %s\n", __FUNCTION__, __LINE__, content);
    websWrite(wp, "%s", content);

    return 0;

}
/********************************************************************
 * ������ readConfigFromFile_skip
 * ���ܣ� ��ҳ���ļ��ж�ȡtitle,content,skiptime,skipurl������ע����3������Ҫ������
 * ������ 2012-03-21
 * ������ 
 *		  title: �������
 *		  content: ����ͨ������
 *		  skiptime: �Զ���תʱ��
 *		  skipurl: �Զ���תurl
 * ���أ� �ɹ���ʧ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 

static int readConfigFromFile_skip(char *title, char *content, char *skiptime, char *skipurl, const char *path)
{
    /* ���ļ�Notify1.asp�а���ʽ���ļ�Notify.htm��ȡtitle,content���� */
    FILE	*fp = NULL;
    char	buf[ NOTIFY_HTM1_SIZE ] = {0u};
    char	*pskiptimehead = NULL;
    char	*pskiptimetail = NULL;
    char	*pskipurlhead = NULL;
    char	*pskipurltail = NULL;
    char	*ptitlehead = NULL;
    char	*ptitletail = NULL;
    char	*pcontenthead = NULL;
    char	*pcontenttail = NULL;
    uint	rc = FAIL;
#if 1
    if((NULL != title) && (NULL != content) && (NULL != path))
    {
	fp = fopen( path, "r" );
	if( (fp != NULL) ) 
	{
	    fread( (void*)buf, sizeof(char), NOTIFY_HTM1_SIZE, fp );
	    fclose( fp );
	    pskiptimehead = strstr(buf, "content=\"");
	    pskiptimetail = strstr(buf, ";url");
	    if ((pskiptimehead != NULL) && (pskiptimetail != NULL)) {
		pskiptimehead += strlen( "content=\"");
		*pskiptimetail = '\0';
		pskipurlhead = strstr(pskiptimetail + 1, "http://");
		pskipurltail = strstr(pskiptimetail + 1, "\">");
		if ((pskipurlhead != NULL) && (pskipurltail != NULL)) {
		    pskipurlhead += strlen( "http://");
		    *pskipurltail = '\0';
		    ptitlehead = strstr(pskipurltail + 1, "<h3 align=center>");
		    ptitletail = strstr(pskipurltail + 1, "</h3>");
		    if((ptitlehead != NULL) && (ptitletail != NULL))
		    {
			ptitlehead += strlen( "<h3 align=center>" );
			*ptitletail = '\0';
			pcontenthead = ptitletail + strlen("</h3>");
			pcontenttail = strstr((ptitletail+1), "<div>" );
			if( NULL != pcontenttail )
			{
			    *pcontenttail = '\0';
			    strcpy(title, ptitlehead);
			    strcpy(content, pcontenthead);
			    strcpy(skiptime, pskiptimehead);
			    strcpy(skipurl, pskipurlhead);
			    rc = SUCCESS;
			}
		    }
		}
	    } 
	}
    }
#endif
    return (int)rc;
}

/********************************************************************
 * ������ readConfigFromFile
 * ���ܣ� ��ҳ���ļ��ж�ȡtitle,manger��content������ע����3������Ҫ������
 * ������ 2012-03-21
 * ������ 
 *		  title: �������
 *		  content: ����ͨ������
 * ���أ� �ɹ���ʧ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int readConfigFromFile(char *title, char *content, const char *path)
{
#if 0
		/* ���ļ�Notify1.asp�а���ʽ���ļ�Notify.htm��ȡtitle,content���� */
#endif
	FILE	*fp = NULL;
#if 0
	Uchar	bufback[ NOTIFY_HTM1_MOUDLE_SIZE ] = {0u};
#endif
	char	buf[ NOTIFY_HTM1_SIZE ] = {0u};
	char	*ptitlehead = NULL;
	char	*ptitletail = NULL;
	char	*pcontenthead = NULL;
	char	*pcontenttail = NULL;
	uint	rc = FAIL;
#if 1
	if( (NULL != title) && (NULL != content) && (NULL != path) )
	{
	    fp = fopen( path, "r" );
	    if( (fp != NULL) ) 
	    {
		fread( (void*)buf, sizeof(char), NOTIFY_HTM1_SIZE, fp );
		fclose( fp );
		ptitlehead = strstr( buf, "<h3 align=center>" );
		ptitletail = strstr( buf, "</h3>" );
		if( (NULL != ptitlehead) && (NULL != ptitletail) )
		{
			ptitlehead += strlen( "<h3 align=center>" );
			*ptitletail = '\0';
			pcontenthead = ptitletail + strlen("</h3>");
			pcontenttail = strstr( (ptitletail+1), "</div>" );
			if( NULL != pcontenttail )
			{
				*pcontenttail = '\0';
				strcpy( title, ptitlehead );
				strcpy( content, pcontenthead );
				rc = SUCCESS;
			}
		}
	    }
	} 
#endif
	return (int)rc;
}
/********************************************************************
 * ������ getNoticeReceive
 * ���ܣ� �û����ܵ�ͨ��֮�󣬲���ͨ��ƥ�������
 * ������ 2012-03-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static int getNoticeReceive(int eid, webs_t wp, int argc, char **argv)
{
#if 0
	/* ���ú�ṩ�Ľӿں��� */
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
