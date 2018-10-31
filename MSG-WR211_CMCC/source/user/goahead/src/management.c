#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include "linux/autoconf.h"
#include "config/autoconf.h"	//user config
#include    "mib.h"
#include    "profacce.h"
#include    "uttfunction.h"
#include    "mibAdminConf.h"

#ifdef USER_MANAGEMENT_SUPPORT
#include "um.h"
#endif
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "wireless.h"
#include "management.h"
#include "wps.h"
#include "ifmisc.h"
#include "translate.h"

#if (DST == FYES)
#define DST_SWITCH "on"
#else
#define DST_SWITCH "off"
#endif

#define SAMPLE		2
#define u_int64_t unsigned long long
#if 0
#define DDNS_IPLINK_LOG "/tmp/ddns/ddns_iplink.log"
#endif
struct {
	/* cpu data  */
	int loadIndex;
	int samples;
	u_int64_t *load, *total;

	/* memory data  */
	u_int64_t mem_used;
	u_int64_t mem_max;
	u_int64_t swap_used;
	u_int64_t swap_max;
	unsigned int swap_percent;	/* swap used, in percent */
	unsigned int mem_percent;	/* memory used, in percent */
} msd;



extern void WPSRestart(void);
extern void formDefineWPS(void);


//#define COMMAND_MAX	1024
//static char system_command[COMMAND_MAX];
/*
 * getClockInfo
 * KeHuatao
 *
 */
static int getClockInfo(int eid, webs_t wp, int argc, char_t ** argv)
{

	char_t buf[64], *tmp;
	MibProfileType mibType = MIB_PROF_NTP;
	NtpProfile *prof;
        struct in_addr inaddr;
	FILE *fp = popen("date '+%s'", "r");
	if (!fp) {
		websWrite(wp, T("none"));
		return 0;
	}
	fgets(buf, 64, fp);
	pclose(fp);
	tmp = strtok(buf, "\n");
	websWrite(wp, T("var sysDateTimes=\"%s\";"), buf);
	websWrite(wp, T("var dst_switch=\"%s\";"), DST_SWITCH);

        prof = (NtpProfile*)ProfGetInstPointByIndex(mibType, 0);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    websWrite(wp, T("var Timezones=\"%d\";"), prof->time_zone);
#if (DST == FYES)
	    websWrite(wp, T("dst_enable=\"%d\";"), prof->dstEnable);
#endif
	    if(prof->enable!=0)
	    {
		websWrite(wp, T("var sntp_enables=\"on\";"));
	    }
	    else
	    {
		websWrite(wp, T("var sntp_enables=\"off\";"));
	    }
	    if(prof->NTPServerIP1!=0)
	    {
		inaddr.s_addr = prof->NTPServerIP1;  
		websWrite(wp, T("var server1s=\"%s\";"), inet_ntoa(inaddr));
	    }
	    else
	    {
		websWrite(wp, T("var server1s=\"0.0.0.0\";"));
	    }

	    if(prof->NTPServerIP2!=0)
	    {
		inaddr.s_addr = prof->NTPServerIP2;  
		websWrite(wp, T("var server2s=\"%s\";"), inet_ntoa(inaddr));
	    }
	    else
	    {
		websWrite(wp, T("var server2s=\"0.0.0.0\";"));
	    }
 
	    if(prof->NTPServerIP3!=0)
	    {
		inaddr.s_addr = prof->NTPServerIP3;  
		websWrite(wp, T("var server3s=\"%s\";"), inet_ntoa(inaddr));
	    }
	    else
	    {
		websWrite(wp, T("var server3s=\"0.0.0.0\";"));
	    }
	}
	else
	{
	    websWrite(wp, T("var Timezones=\"28800\";"));
	    websWrite(wp, T("var sntp_enables=\"0\";"));
	    websWrite(wp, T("var server1s=\"0.0.0.0\";"));
	    websWrite(wp, T("var server2s=\"0.0.0.0\";"));
	    websWrite(wp, T("var server3s=\"0.0.0.0\";"));
	}
#if (UTT_SMART_UI == FYES) 
           getTheRightMsg(eid,wp,argc,argv);
#endif

	return 0;
}

/*
 * goform/setSysAdm
 */
#if (ADMIN_CONF == FYES)
void setSysAdm(webs_t wp, char_t * path, char_t * query)
{
	char *admuser, *admpass, *old_user;
	AdminConfProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_ADMIN_CONF;

	admuser = old_user = "admin";
	admpass = websGetVar(wp, T("passwd1"), T(""));
	if (!strlen(admuser)) {
		error(E_L, E_LOG,
		      T("setSysAdm: account empty, leave it unchanged"));
		return;
	}
	if (!strlen(admpass)) {
		error(E_L, E_LOG,
		      T("setSysAdm: password empty, leave it unchanged"));
		return;
	}
	prof  = (AdminConfProfile *)ProfGetInstPointByName(mibType, admuser);
        prof->head.active = TRUE;
        strcpy(prof->head.name, admuser);
        strcpy(prof->password, admpass);
        nvramWriteCommit();

	/* modify /etc/passwd to new user name and passwd */
	doSystem("sed -e 's/^%s:/%s:/' /etc/passwd > /etc/newpw", old_user,
		 admuser);
	doSystem("cp /etc/newpw /etc/passwd");
	doSystem("rm -f /etc/newpw");
	doSystem("chpasswd.sh %s %s", admuser, admpass);
#if (NET_SHARE_USER == FYES)
	doSystem("touch /etc/smb.conf; smbpasswd %s %s", admuser, admpass);
#endif

#ifdef USER_MANAGEMENT_SUPPORT
	if (umGroupExists(T("adm")) == FALSE)
#if (UTT_SMART_UI == FYES)
		umAddGroup(T("adm"), 0x07, AM_BASIC, FALSE, FALSE);
#else
		umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
#endif
	if (old_user != NULL && umUserExists(old_user))
		umDeleteUser(old_user);
	if (umUserExists(admuser))
		umDeleteUser(admuser);
	umAddUser(admuser, admpass, T("adm"), FALSE, FALSE);
#endif

}
#endif

/*
 *get Language
 *KeHuatao
 *
 */
static int getSysLang(int eid, webs_t wp, int argc, char_t ** argv)
{
	MibProfileType mibType = MIB_PROF_LANG;
	LangProfile *prof;

        prof = (LangProfile*)ProfGetInstPointByIndex(mibType, 0);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    websWrite(wp, T("var lang = \"%s\";"), prof->lang);
	}
	else
	{
	    websWrite(wp, T("var lang = \"zhcn\";"));
	}
	return 0;
}
/*
 * goform/setSysLang
 */
extern enum em_UTT_LANGUAGE trans_curLang;
static void setSysLang(webs_t wp, char_t * path, char_t * query)
{
	char_t *lang;
	MibProfileType mibType = MIB_PROF_LANG;
	LangProfile *prof;

	lang = websGetVar(wp, T("langSelection"), T(""));
        prof = (LangProfile*)ProfGetInstPointByIndex(mibType, 0);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
	    memset(prof->lang, 0, sizeof(prof->lang));
	    strncpy(prof->lang, lang, sizeof(prof->lang));
	    /*update*/
	    if(strcmp(prof->lang, "zhcn") == 0)
	    {
			trans_curLang=LANGUAGE_ZH;
	    }
	    else if(strcmp(prof->lang, "en") == 0)
	    {
			trans_curLang=LANGUAGE_EN;
	    }
	    else if(strcmp(prof->lang, "zhtw") == 0)
	    {
			trans_curLang=LANGUAGE_TW;
	    }
	    nvramWriteCommit();
	}

	websHeader(wp);
	websWrite(wp, T("<h2>Language Selection</h2><br>\n"));
	websWrite(wp, T("language: %s<br>\n"), lang);
	websFooter(wp);
	websDone(wp, 200);
}

/*
 * set NTP
 * KeHuatao
 */
static void setNtpFunc(webs_t wp, char_t * path, char_t * query)
{
	char *tz, *ntpServer;
	char *sysDateTime, *sntp_enable, *server2, *server3;
	int val = 0;
	MibProfileType mibType = MIB_PROF_NTP;
        struProfAlloc *profhead= NULL;
	NtpProfile *prof;
#if (DST == FYES)
	char *dstEnable;
#endif

	tz = websGetVar(wp, T("time_zone"), T(""));
	ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));
	sysDateTime = websGetVar(wp, T("SysDateTime1"), T(""));
	sntp_enable = websGetVar(wp, T("SntpEnable"), T(""));
	server2 = websGetVar(wp, T("server2"), T(""));
	server3 = websGetVar(wp, T("server3"), T(""));

	printf("tz=%s,ntpserver=%s,sysDateTime=%s,sntp_enable=%s,server2=%s,server3=%s\n",
		tz,ntpServer,sysDateTime,sntp_enable,server2,server3);

	if (!tz || !ntpServer)
		return;

	if (!strlen(tz))
		return;

	if (checkSemicolon(tz))
		return;

        ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
        prof = (NtpProfile*)ProfGetInstPointByIndex(mibType, 0);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
            /*time zone*/
	    val = strtoul(tz, NULL, 10);
	    prof->time_zone = val;
	    val = strtoul(sntp_enable, NULL, 10);

	    if(strcmp(sntp_enable, "on")==0)
	    {
		/*set ntp*/
		prof->enable= TRUE;

#if (DST == FYES)
		dstEnable = websGetVar(wp,T("dstEnable"),T(""));
		if (!strncmp(dstEnable,"on",2)) {
		    prof->dstEnable = TRUE;
		} else {
		    prof->dstEnable = FALSE;
		}
#endif
		if (!strlen(ntpServer)) {
		    // user choose to make  NTP server disable
		    prof->NTPServerIP1 = 0;
		} else {
		    if (checkSemicolon(ntpServer))
			return;
		    val = inet_addr(ntpServer);
		    prof->NTPServerIP1 = val;
		}

		if(strcmp(server2, "")!=0)
		{
		    val = inet_addr(server2);
		    prof->NTPServerIP2 = val;
		}
		else{
		    prof->NTPServerIP2 = 0;
		}

		if(strcmp(server3, "")!=0)
		{
		    val = inet_addr(server3);
		    prof->NTPServerIP3 = val;
		}
		else{
		    prof->NTPServerIP3 = 0;
		}
	    }
	    else
	    {
		/*set time*/
		if (0 == strcmp(prof->timeSetting, sysDateTime))
		{
		    doSystem("killall -q ntpclient");
		    doSystem("date -s %s", prof->timeSetting);
		}
		prof->enable= FALSE;
		val = sizeof(prof->timeSetting) ;
		memset(prof->timeSetting, 0, val);
		val--;
		strncpy(prof->timeSetting, sysDateTime, val);
	    }
	    /*update*/
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);   
	    nvramWriteCommit();
	}
	sleep(1);

#if (UTT_SMART_UI == FYES)
	setSuccessTips();
	websRedirect(wp, "SysClockManage.asp");
#else
	websRedirect(wp, "SysClock.asp");
#endif
}

static int getSystemInfoNowTime(int eid, webs_t wp, int argc,
				char_t ** argv)
{
	char_t buf[64], *tmp;
	FILE *fp = popen("date '+%s'", "r");
	if (!fp) {
		websWrite(wp, T("none"));
		return 0;
	}
	fgets(buf, 64, fp);
	pclose(fp);
	tmp = strtok(buf, "\n");

	websWrite(wp, T("var sysDateTimes=\"%s\";"), buf);
	return 0;

}
long long _oldWlanRxByte, _oldWlanTxByte, _oldWlanRxPacket,
    _oldWlanTxPacket, _oldWanRxByte[MAX_INTERFACE_PROFILES],
    _oldWanTxByte[MAX_INTERFACE_PROFILES], _oldWanRxPacket[MAX_INTERFACE_PROFILES],
    _oldWanTxPacket[MAX_INTERFACE_PROFILES];
static void _getIfRxTxDataCounter(char *ifname, long long *rxByte,
				  long long *txByte, long long *rxPacket,
				  long long *txPacket)
{

	if (NULL != ifname) {
		long long data = 0;

		data = getIfStatistic(ifname, RXBYTE);
		*rxByte = data;

		data = getIfStatistic(ifname, TXBYTE);
		*txByte = data;

		data = getIfStatistic(ifname, RXPACKET);
		*rxPacket = data;

		data = getIfStatistic(ifname, TXPACKET);
		*txPacket = data;

	} else {
		*rxByte = 0;
		*txByte = 0;
		*rxPacket = 0;
		*txPacket = 0;
	}
	return;
}

/*
static void _getLanRxTxDataCounter(long long *rxByte,
					long long *txByte,
					long long *rxPacket,
					long long *txPacket)
{
	long long data = 0, data1 = 0, data2 = 0, data3 = 0;
	data = getIfStatistic("ra0", RXBYTE);
	*rxByte = data + data1 + data2 + data3;

	data = getIfStatistic("ra0", TXBYTE);
	*txByte = data + data1 + data2 + data3;

	data = getIfStatistic("ra0", RXPACKET);
	*rxPacket = data + data1 + data2 + data3;

	data = getIfStatistic("ra0", TXPACKET);
	*txPacket = data + data1 + data2 + data3;
}
*/

static void formConfigSysClearRxTx(webs_t wp, char_t * path,
				   char_t * query)
{

	int i = 0;
	for (i = 0; i < getWanSlotCount(); i++) {
		_getIfRxTxDataCounter
		    (getWanIfNamePPP(i + 1),
		     &(_oldWanRxByte[i]),
		     &(_oldWanTxByte[i]),
		     &(_oldWanRxPacket[i]), &(_oldWanTxPacket[i]));


	}

	_getIfRxTxDataCounter(getLanIfName(), &_oldWlanRxByte,
			      &_oldWlanTxByte,
			      &_oldWlanRxPacket, &_oldWlanTxPacket);

	websRedirect(wp, "FluxStat.asp");
}

#if 0
static int getWanTxRXPacketByte(int eid, webs_t wp, int argc,
				char_t ** argv)
{
	char_t buf[32], *wanConnMode, buftmp[MAX_UTT_CONF_NAME_LEN];
	int i = 0;
	int wanIndex = 0;
	long long rxByte, txByte, rxPacket, txPacket;

	for (i = 0; i < getWanSlotCount(); i++) {
		wanIndex = i + 1;
                add_wanx(_CONF_WAN_CONNECT_MODE, wanIndex, buftmp);
                wanConnMode = nvram_bufget(RT2860_NVRAM, buftmp);
                if (strlen(wanConnMode) == 0 ) {
		    websWrite(wp, T("sendBytes[%d]=\"\";\n"), wanIndex-1 );
                    continue;
                }

		_getIfRxTxDataCounter(getWanIfNamePPP(wanIndex),
				       &rxByte, &txByte, &rxPacket,
				       &txPacket);
                printf("%d:%s.\n",wanIndex,getWanIfNamePPP(wanIndex));

		if (rxByte - _oldWanRxByte[i] < 0)
			_oldWanRxByte[i] = 0;
		snprintf(buf, 32, "%lld", rxByte - _oldWanRxByte[i]);
		websWrite(wp, T("\nrecivBytes[%d]=\"%s\";\n"),
			  wanIndex-1, buf);
                
                printf("recivBytes[%d]=%s.\n",wanIndex-1,buf);


		if (txByte - _oldWanTxByte[i] < 0)
			_oldWanTxByte[i] = 0;
		snprintf(buf, 32, "%lld", txByte - _oldWanTxByte[i]);
		websWrite(wp, T("sendBytes[%d]=\"%s\";\n"), wanIndex-1, buf);

                
                printf("sendBytes[%d]=%s.\n",wanIndex-1,buf);
		
                if (rxPacket - _oldWanRxPacket[i] < 0)
			_oldWanRxPacket[i] = 0;
		snprintf(buf, 32, "%lld", rxPacket - _oldWanRxPacket[i]);
		websWrite(wp, T("recivPacks[%d]=\"%s\";\n"),
			  wanIndex-1, buf);

                printf("recivBytes[%d]=%s.\n",wanIndex-1,buf);
		
                if (txPacket - _oldWanTxPacket[i] < 0)
			_oldWanTxPacket[i] = 0;
		snprintf(buf, 32, "%lld", txPacket - _oldWanTxPacket[i]);
		websWrite(wp, T("sendPacks[%d]=\"%s\";\n\n"),
			  wanIndex-1, buf);

                printf("sendBytes[%d]=%s.\n",wanIndex-1,buf);
	
        }
	 getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
#endif

static int getLanTxRXPacketByte(int eid, webs_t wp, int argc,
				char_t ** argv)
{
	char_t buf[32];
	long long rxByte, txByte, rxPacket, txPacket;
	_getIfRxTxDataCounter(getLanIfName(), &rxByte, &txByte, &rxPacket,
			      &txPacket);
        
        printf("lan:%s.\n",getLanIfName());
	if (rxByte - _oldWlanRxByte < 0)
		_oldWlanRxByte = 0;
	snprintf(buf, 32, "%lld", rxByte - _oldWlanRxByte);
	websWrite(wp, T("\nrecivBytes[4]=\"%s\";\n"), buf);

        printf("recivBytes[4]=%s.\n",buf);
	
        if (txByte - _oldWlanTxByte < 0)
		_oldWlanTxByte = 0;
	snprintf(buf, 32, "%lld", txByte - _oldWlanTxByte);
	websWrite(wp, T("sendBytes[4]=\"%s\";\n"), buf);

        printf("sendBytes[4]=%s.\n",buf);
	
        if (rxPacket - _oldWlanRxPacket < 0)
		_oldWlanRxPacket = 0;
	snprintf(buf, 32, "%lld", rxPacket - _oldWlanRxPacket);
	websWrite(wp, T("recivPacks[4]=\"%s\";\n"), buf);

        printf("recivBytes[4]=%s.\n",buf);
	
        if (txPacket - _oldWlanTxPacket < 0)
		_oldWlanTxPacket = 0;
	snprintf(buf, 32, "%lld", txPacket - _oldWlanTxPacket);
	websWrite(wp, T("sendPacks[4]=\"%s\";\n"), buf);

        printf("sendBytes[4]=%s.\n",buf);
        
        getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
static void formConfigRebootMachine(webs_t wp, char *path, char *query)
{
#if (EASY_TASK == FYES)
	char *page = NULL;
	page = websGetVar(wp, T("page"), T(""));
#if 0
	printf("%s--%d:%s\n", __func__, __LINE__, page);
#endif
	if (strcmp(page, "EasyTask") == 0)
	{ 
#if (UTT_SMART_UI == FYES)
	    /*构造ajax返回结果*/
	    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	    websWrite(wp, T("%s"), "EasyTask");
	    
	    websDone(wp, 200);
	    //web_reload(wp, "reload_Sui.asp?id=EasyTask");
#else
	    web_reload(wp, "reload.asp?id=EasyTask");
#endif
	}
	else
	{
#endif
#if (UTT_SMART_UI == FYES)
	    web_reload(wp, "reload_Sui.asp");
#else
	    web_reload(wp, "reload.asp");
#endif
#if (EASY_TASK == FYES)
	}
#endif
} 
static int rebootSystem(int eid, webs_t wp, int argc, char_t ** argv)
{
#if (WEBAUTH_ACCOUNT == FYES)
	doSystem("rebootS -d 3 \\&");
#else
	doSystem("reboot -d 3 &");
#endif
	return 0;
}
static int updateError(int eid, webs_t wp, int argc, char_t ** argv)
{
	system("sleep 3 && killall -71 goahead &");
	return 0;
}


#if 0
static void SystemCommand(webs_t wp, char_t * path, char_t * query)
{
	char *command;

	command = websGetVar(wp, T("command"), T(""));

	if (!command)
		return;

	if (!strlen(command))
		snprintf(system_command, COMMAND_MAX,
			 "cat /dev/null > %s", SYSTEM_COMMAND_LOG);
	else
		snprintf(system_command, COMMAND_MAX,
			 "%s 1>%s 2>&1", command, SYSTEM_COMMAND_LOG);

	if (strlen(system_command))
		doSystem(system_command);

	websRedirect(wp, "adm/system_command.asp");

	return;
}

static void repeatLastSystemCommand(webs_t wp, char_t * path,
				    char_t * query)
{
	if (strlen(system_command))
		doSystem(system_command);

	websRedirect(wp, "adm/system_command.asp");

	return;
}

int showSystemCommandASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	FILE *fp;
	char buf[1024];

	fp = fopen(SYSTEM_COMMAND_LOG, "r");
	if (!fp) {
		websWrite(wp, T(""));
		return 0;
	}

	while (fgets(buf, 1024, fp)) {
		websWrite(wp, T("%s"), buf);
	}
	fclose(fp);

	return 0;
}
#endif

static inline char *strip_space(char *str)
{
	while (*str == ' ')
		str++;
	return str;
}
char *getField(char *a_line, char *delim, int count)
{
	int i = 0;
	char *tok;
	tok = strtok(a_line, delim);
	while (tok) {
		if (i == count)
			break;
		i++;
		tok = strtok(NULL, delim);
	}
	if (tok && isdigit(*tok))
		return tok;

	return NULL;
}

/*
 *   C version. (ASP version is below)
 */
	// static long long getIfStatistic(char *interface, int type)
long long getIfStatistic(char *interface, int type)
{
	int found_flag = 0;
	int skip_line = 2;
	char buf[1024], *field, *semiColon = NULL;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if (!fp) {
		return -1;
	}

	while (fgets(buf, 1024, fp)) {
		char *ifname;
		if (skip_line != 0) {
			skip_line--;
			continue;
		}
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if (!strcmp(ifname, interface)) {
			found_flag = 1;
			break;
		}
	}
	fclose(fp);
	if (found_flag == 0)
		return 0;

	semiColon++;

	switch (type) {
	case TXBYTE:
		if ((field = getField(semiColon, " ", 8))) {
			return strtoll(field, NULL, 10);
		}
		break;
	case TXPACKET:
		if ((field = getField(semiColon, " ", 9))) {
			return strtoll(field, NULL, 10);
		}
		break;
	case RXBYTE:
		if ((field = getField(semiColon, " ", 0))) {
			return strtoll(field, NULL, 10);
		}
		break;
	case RXPACKET:
		if ((field = getField(semiColon, " ", 1))) {
			return strtoll(field, NULL, 10);
		}
		break;
	}
	return -1;
}

/*
 *     getIfStatistic()   ASP version
 */
int getIfStatisticASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	int found_flag = 0;
	int skip_line = 2;
	char *interface, *type, *field, *semiColon = NULL;
	char buf[1024], result[32];
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if (!fp) {
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	if (ejArgs(argc, argv, T("%s %s"), &interface, &type) != 2) {
		websWrite(wp, T("Wrong argument.\n"));
		return -1;
	}

	while (fgets(buf, 1024, fp)) {
		char *ifname;
		if (skip_line != 0) {
			skip_line--;
			continue;
		}
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if (!strcmp(ifname, interface)) {
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	if (!strcmp(type, T("TXBYTE"))) {
		if ((field = getField(semiColon, " ", 8))) {
			snprintf(result, 32, "%lld",
				 strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	} else if (!strcmp(type, T("TXPACKET"))) {
		if ((field = getField(semiColon, " ", 9))) {
			snprintf(result, 32, "%lld",
				 strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	} else if (!strcmp(type, T("RXBYTE"))) {
		if ((field = getField(semiColon, " ", 0))) {
			snprintf(result, 32, "%lld",
				 strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	} else if (!strcmp(type, T("RXPACKET"))) {
		if ((field = getField(semiColon, " ", 1))) {
			snprintf(result, 32, "%lld",
				 strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	} else {
		websWrite(wp, T("unknown type."));
		return -1;
	}
	return -1;
}

#if 0
int getWANRxByteASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
int getWANRxPacketASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
int getWANTxByteASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
int getWANTxPacketASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
#endif
int getLANRxByteASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
int getLANRxPacketASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
int getLANTxByteASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}
int getLANTxPacketASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

#if 0
/*
 * This ASP function is for javascript usage, ex:
 *
 * <script type="text/javascript">
 *   var a = new Array();
 *   a = [<% getAllNICStatisticASP(); %>];         //ex: a = ["lo","10","1000", "20", "2000","eth2"];
 *   document.write(a)
 * </script>
 *
 * Javascript could get info with  getAllNICStatisticASP().
 *
 * We dont produce table-related tag in this ASP function .It's
 * more extensive since ASP just handle data and Javascript present them,
 * although the data form is only for Javascript now.
 *
 * TODO: a lot, there are many ASP functions binding with table-relted tag...
 */ int getAllNICStatisticASP(int eid, webs_t wp, int argc,
			      char_t ** argv)
{
	char result[1024];
	char buf[1024];
	int rc = 0, pos = 0, skip_line = 2;
	int first_time_flag = 1;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if (!fp) {
		DBGPRINT("no proc?\n");
		return -1;
	}

	while (fgets(buf, 1024, fp)) {
		char *ifname, *semiColon;
		long long if_rc;
		if (skip_line != 0) {
			skip_line--;
			continue;
		}
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';

		ifname = buf;
		ifname = strip_space(ifname);

		/* try to get statistics data */
		if (getIfStatistic(ifname, RXPACKET) >= 0) {
			/* a success try */
			if (first_time_flag) {
				pos =
				    snprintf(result + rc,
					     1024 - rc, "\"%s\"", ifname);
				rc += pos;
				first_time_flag = 0;
			} else {
				pos =
				    snprintf(result + rc,
					     1024 - rc, ",\"%s\"", ifname);
				rc += pos;
			}

		} else		/* failed and just skip */
			continue;

		pos =
		    snprintf(result + rc, 1024 - rc, ",\"%lld\"",
			     getIfStatistic(ifname, RXPACKET));
		rc += pos;
		pos =
		    snprintf(result + rc, 1024 - rc, ",\"%lld\"",
			     getIfStatistic(ifname, RXBYTE));
		rc += pos;
		pos =
		    snprintf(result + rc, 1024 - rc, ",\"%lld\"",
			     getIfStatistic(ifname, TXPACKET));
		rc += pos;
		pos =
		    snprintf(result + rc, 1024 - rc, ",\"%lld\"",
			     getIfStatistic(ifname, TXBYTE));
		rc += pos;
	}
	fclose(fp);

	websWrite(wp, T("%s"), result);
	return 0;
}

static void cpu_mem_init()
{
	int i;
	u_int64_t load = 0, total = 0;

	msd.samples = SAMPLE;
	if (msd.load) {
		load = msd.load[msd.loadIndex];
		free(msd.load);
	}
	if (msd.total) {
		total = msd.total[msd.loadIndex];
		free(msd.total);
	}

	msd.loadIndex = 0;
	msd.load = malloc(msd.samples * sizeof(u_int64_t));
	msd.total = malloc(msd.samples * sizeof(u_int64_t));
	for (i = 0; i < msd.samples; i++) {
		msd.load[i] = load;
		msd.total[i] = total;
	}
}

static int getSystem_cpu(int eid, webs_t wp, int argc, char_t ** argv)
{
	unsigned int cpuload;
	u_int64_t load, total, oload, ototal;
	u_int64_t ab, ac, ad, ae;
	int i;
	FILE *stat;
	cpu_mem_init();
	if ((stat = fopen("/proc/stat", "r")) == NULL) {
		fprintf(stderr, "failed to open /proc/stat. Exiting\n");
		exit(1);
	}

	fscanf(stat, "%*s %Ld %Ld %Ld %Ld", &ab, &ac, &ad, &ae);
	fclose(stat);

	/* Find out the CPU load */
	/* user + sys = load
	 * total = total */
	load = ab + ac + ad;	/* cpu.user + cpu.sys; */
	total = ab + ac + ad + ae;	/* cpu.total; */

	/* "i" is an index into a load history */
	i = msd.loadIndex;
	oload = msd.load[i];
	ototal = msd.total[i];

	msd.load[i] = load;
	msd.total[i] = total;
	msd.loadIndex = (i + 1) % msd.samples;

	if (ototal == 0)
		cpuload = 0;
	else
		cpuload = (100 * (load - oload)) / (total - ototal);

	websWrite(wp, T("var cpuStat=\"%d\";"), cpuload);
	return 0;
}
#endif
#define NCPUSTATES 5
static long cp_time[NCPUSTATES];
static long cp_old[NCPUSTATES];
static long cp_diff[NCPUSTATES];
static int cpu_states[NCPUSTATES];

static inline char *skip_token(const char *p)
{
	while (isspace(*p))
		p++;
	while (*p && !isspace(*p))
		p++;
	return (char *) p;
} long percentages(int cnt, int *out, register long *new,
		   register long *old, long *diffs)
{
	register int i;
	register long change;
	register long total_change;
	register long *dp;
	long half_total;

	/* initialization */
	total_change = 0;
	dp = diffs;

	/* calculate changes for each state and the overall change */
	for (i = 0; i < cnt; i++) {
		if ((change = *new - *old) < 0) {
			/* this only happens when the counter wraps */
			change =
			    (int) ((unsigned long) *new -
				   (unsigned long) *old);
		}
		total_change += (*dp++ = change);
		*old++ = *new++;
	}

	/* avoid divide by zero potential */
	if (total_change == 0) {
		total_change = 1;
	}

	/* calculate percentages based on overall change, rounding up */
	half_total = total_change / 2l;
	half_total = 0;
	for (i = 0; i < cnt; i++) {
		//DBGPRINT("dd %ld %ld\n",(*diffs* 1000 + half_total),total_change);
		*out++ =
		    (int) ((*diffs++ * 1000 + half_total) / total_change);
	}

	/* return the total in case the caller wants to use it */
	return (total_change);
}

static int getSystem_cpu(int eid, webs_t wp, int argc, char_t ** argv)
{
	int fd, len;
	char *p;
	int i;
	char buffer[1024 + 1];

	for (i = 0; i < 4; i++) {
		cpu_states[i] = 0;
		cp_diff[i] = 0;
	}
//    while(1){ 
        fd = open("/proc/stat", O_RDONLY);
	len = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	buffer[len] = '\0';

	p = skip_token(buffer);	/* "cpu" */
	cp_time[0] = strtoul(p, &p, 0);
	cp_time[1] = strtoul(p, &p, 0);
	cp_time[2] = strtoul(p, &p, 0);
	cp_time[3] = strtoul(p, &p, 0);
	cp_time[4] = strtoul(p, &p, 0);
	//DBGPRINT("f %ld %ld %ld %ld\n",cp_time[0],cp_time[1],cp_time[2],cp_time[3]);
	percentages(NCPUSTATES, cpu_states, cp_time, cp_old, cp_diff);

	websWrite(wp, T("var cpuStat=\"%d\";"), (int) (cpu_states[0] / 10.0));
	return 0;
}

static int getSystem_memory(int eid, webs_t wp, int argc, char_t ** argv)
{
	char buf[1024], *semiColon, *key, *value;
	int total = 1, free = 1, memStat;
#if ((UTT_SMART_UI == FYES) && (NET_SHARE_MANAGE == FYES))
	doSystem("sync;echo 3 > /proc/sys/vm/drop_caches");
#endif
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if (!fp) {
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while (fgets(buf, 1024, fp)) {
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if (!strcmp(key, "MemTotal")) {
			value = strip_space(value);
			total = atoi(value);
		} else if (!strcmp(key, "MemFree")) {
			value = strip_space(value);
			free = atoi(value);
		}
	}
	fclose(fp);
	memStat = 100 - (free * 100) / total;
	websWrite(wp, T("var memStat=\"%d\";"),
		  ((memStat < 85) ? ((int)(memStat*(((double)8)/17))) : (40+(memStat-85)*4)));
	return 0;
}


int getMemTotalASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if (!fp) {
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while (fgets(buf, 1024, fp)) {
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if (!strcmp(key, "MemTotal")) {
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);

	return -1;
}

int getCurrentTimeASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char_t buf[64];
	FILE *fp = popen("date", "r");
	if (!fp) {
		websWrite(wp, T("none"));
		return 0;
	}
	fgets(buf, 64, fp);
	pclose(fp);

	websWrite(wp, T("%s"), buf);
	return 0;
}

int getMemLeftASP(int eid, webs_t wp, int argc, char_t ** argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if (!fp) {
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while (fgets(buf, 1024, fp)) {
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if (!strcmp(key, "MemFree")) {
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);
	return -1;
}

#ifdef CONFIG_USER_USB_DISK
static int getFlashStat(int eid, webs_t wp, int argc, char_t ** argv)
{
    char dat[32]={'\0'};
    long long outer_flash_total_space=0;
    long long outer_flash_unused_space=0;

    /*外接闪存使用状态*/
    getOuterFlashStat(&outer_flash_total_space,&outer_flash_unused_space);
    sprintf(dat, "%lld", outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", dat);/*总空间*/
    sprintf(dat, "%lld", outer_flash_unused_space);
    websWrite(wp, "var outerFlashUnusedSpace = \"%s\";\n", dat);/*未使用*/

    return 0;
}
#else
static int getFlashStat(int eid, webs_t wp, int argc, char_t ** argv)
{
    websWrite(wp, "var outerFlashTotalSpace = \"0\";");/*总空间*/
    websWrite(wp, "var outerFlashUnusedSpace = \"0\";");/*未使用*/
    return 0;
}
#endif

/*
 * callee must free memory.
 */
/*
static char *getLog(char *filename)
{
	FILE *fp;
	struct stat filestat;
	char *log;

	if(stat(filename, &filestat) == -1)
		return NULL;

//	DBGPRINT("%d\n", filestat.st_size);
	log = (char *)malloc(sizeof(char) * (filestat.st_size + 1) );
	if(!log)
		return NULL;

	if(!(fp = fopen(filename, "r"))){
		return NULL;
	}

	if( fread(log, 1, filestat.st_size, fp) != filestat.st_size){
		DBGPRINT("read not enough\n");
		free(log);
		return NULL;
	}

	log[filestat.st_size] = '\0';

	fclose(fp);
	return log;
}
*/

#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
static void clearlog(webs_t wp, char_t * path, char_t * query)
{
	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
	doSystem("syslogd -C8 1>/dev/null 2>&1");
	doSystem("klogd 1>/dev/null 2>&1");

	websRedirect(wp, "adm/syslog.asp");
}
#endif


#if 0
#define LOG_MAX (16384)
static void syslog(webs_t wp, char_t * path, char_t * query)
{
	FILE *fp = NULL;
	char linestr[200];
	char *log;

	websWrite(wp,
		  T
		  ("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	fp = popen("logread", "r");
	if (!fp) {
		websWrite(wp, "-1");
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if (!log) {
		websWrite(wp, "-1");
		goto error;
	}
	memset(log, 0, LOG_MAX);
#if 0
	fread(log, 1, LOG_MAX, fp);
#endif
	while(!feof(fp))
	{
	    memset(linestr, 0, sizeof(linestr));
	    if(fgets(linestr, sizeof(linestr), fp) != NULL)
	    {
		if((strstr(linestr,"kernel") == NULL) && (strlen(linestr) + strlen(log) < LOG_MAX))
		{
		    strcat(log, linestr);
		}
	    }
	}
	websLongWrite(wp, log);

	free(log);
      error:
	if (fp)
		pclose(fp);
	websDone(wp, 200);
}
#endif
/*
void management_init(void)
{
	ntp_sh();
#ifdef CONFIG_USER_GOAHEAD_GreenAP
	doSystem("greenap.sh init");
#endif
	init_DDNS();
//      WPSRestart();

#if 0
	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
	doSystem("syslogd -C8 1>/dev/null 2>&1");
	doSystem("klogd 1>/dev/null 2>&1");
#endif
} 
*/
void management_fini(void)
{
//      doSystem("killall -q klogd");
//      doSystem("killall -q syslogd");
} void formDefineManagement(void)
{
	websAspDefine(T("rebootSystem"), rebootSystem);
	websAspDefine(T("updateError"), updateError);
#if 0
	websAspDefine(T("outputWANTxRXPacketByte"), getWanTxRXPacketByte);
#endif

	websFormDefine(T("ConfigClearRxTxPacketByte"),
		       formConfigSysClearRxTx);
	websAspDefine(T("outputLanTxRXPacketByte"),
		      getLanTxRXPacketByte);


	websAspDefine(T("getClockInfo"), getClockInfo);
	websAspDefine(T("outputSystemNowTime"), getSystemInfoNowTime);
#if ((ADMIN_CONF == FYES) || (EASY_TASK == FYES))
	websFormDefine(T("setSysAdm"), setSysAdm);
#endif
	websFormDefine(T("setSysLang"), setSysLang);
	websAspDefine(T("getSysLang"), getSysLang);
	websFormDefine(T("formRebootMachine"), formConfigRebootMachine);
	websFormDefine(T("NTP"), setNtpFunc);
	websAspDefine(T("getCurrentTimeASP"), getCurrentTimeASP);

	websAspDefine(T("outputSystemCpu"), getSystem_cpu);
	websAspDefine(T("outputSystemMemory"), getSystem_memory);
	websAspDefine(T("getMemLeftASP"), getMemLeftASP);
	websAspDefine(T("getMemTotalASP"), getMemTotalASP);
	websAspDefine(T("outputFlashStat"), getFlashStat);
#ifdef CONFIG_USB
#endif
	//websAspDefine(T("getWANRxByteASP"), getWANRxByteASP);
	//websAspDefine(T("getWANTxByteASP"), getWANTxByteASP);
	websAspDefine(T("getLANRxByteASP"), getLANRxByteASP);
	websAspDefine(T("getLANTxByteASP"), getLANTxByteASP);
	//websAspDefine(T("getWANRxPacketASP"), getWANRxPacketASP);
	//websAspDefine(T("getWANTxPacketASP"), getWANTxPacketASP);
	websAspDefine(T("getLANRxPacketASP"), getLANRxPacketASP);
	websAspDefine(T("getLANTxPacketASP"), getLANTxPacketASP);

//      websAspDefine(T("getAllNICStatisticASP"), getAllNICStatisticASP);
#if 0
	websAspDefine(T("ouputDDNS"), getDDNSConfig);	//add by cxhu
#endif

#if 0
	websAspDefine(T("showSystemCommandASP"), showSystemCommandASP);
	websFormDefine(T("SystemCommand"), SystemCommand);
	websFormDefine(T("repeatLastSystemCommand"),
		       repeatLastSystemCommand);
#endif

#if 0
	websFormDefine(T("LoadDefaultSettings"), LoadDefaultSettings);
#endif

#if 0
	websFormDefine(T("syslog"), syslog);
#endif
#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
	websFormDefine(T("clearlog"), clearlog);
#endif

#if 0
	formDefineWPS();
#endif
}
