/* vi: set sw=4 ts=4 sts=4: */
/*
 *	utils.c -- System Utilities 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.c,v 1.58.14.20 2016/08/09 02:56:02 chen.kaiyu Exp $
 */
#include	<time.h>
#include	<signal.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#include        <sys/sysinfo.h>

#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>

#if CONFIG_RALINK_RT3052 || CONFIG_RALINK_RT3883 
#include	"ralink_gpio.h"
#endif
#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config

#include	"webs.h"
#include	"internet.h"
#include	"utils.h"
#include	"wireless.h"
#include	"uttMachine.h"
#include	"mib.h"
#include	"ifmisc.h"
#include    "profacce.h"

#include    <sys/mman.h>
#include    <memory.h>
#include	"uttfunction.h"
#include    <sys/utsname.h>
#include	"translate.h"
#include	"uttfunction.h"

//#ifdef SYS_USB
#undef CONFIG_USB
#undef CONFIG_USB_STORAGE
//#endif

#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
extern void setFirmwarePath(void);
#endif

#if 0
static int  getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv);
#endif
//static int  getCfg2General(int eid, webs_t wp, int argc, char_t **argv);
//static int  getCfg2NthGeneral(int eid, webs_t wp, int argc, char_t **argv);
//static int  getCfg2Zero(int eid, webs_t wp, int argc, char_t **argv);
//static int  getCfg2NthZero(int eid, webs_t wp, int argc, char_t **argv);
//static int  getCfg3General(int eid, webs_t wp, int argc, char_t **argv);
//static int  getCfg3Zero(int eid, webs_t wp, int argc, char_t **argv);
static int  getLangBuilt(int eid, webs_t wp, int argc, char_t **argv);
#if 0
static int  getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getMiiInicBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getDpbSta(int eid, webs_t wp, int argc, char_t **argv);
static int  getStationBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getPlatform(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysBuildTime(int eid, webs_t wp, int argc, char_t **argv);
static int  getSdkVersion(int eid, webs_t wp, int argc, char_t **argv);
static void forceMemUpgrade(webs_t wp, char_t *path, char_t *query);
static int  isOnePortOnly(int eid, webs_t wp, int argc, char_t **argv);
static void setOpMode(webs_t wp, char_t *path, char_t *query);
static int  getPortStatus(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int  getSysUptime(int eid, webs_t wp, int argc, char_t **argv);
#if 0
#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
static void ScanUSBFirmware(webs_t wp, char_t *path, char_t *query);
#endif
#endif
#if 0
static int getWebMenuConfig(int eid, webs_t wp, int argc, char_t **argv);
#endif

#if 0
/*********************************************************************
 * System Utilities
 */

void arplookup(char *ip, char *arp)
{
	char buf[256];
	FILE *fp = fopen("/proc/net/arp", "r");
	if(!fp){
		trace(0, T("no proc fs mounted!\n"));
		return;
	}
	strcpy(arp, "00:00:00:00:00:00");
	while(fgets(buf, 256, fp)){
		char ip_entry[32], hw_type[8],flags[8], hw_address[32];
		sscanf(buf, "%s %s %s %s", ip_entry, hw_type, flags, hw_address);
		if(!strcmp(ip, ip_entry)){
			strcpy(arp, hw_address);
			break;
		}
	}

	fclose(fp);
	return;
}
#endif


/*
 * description: kill process whose pid was recorded in file
 *              (va is supported)
 */
int doKillPid(char_t *fmt, ...)
{
	va_list		vargs;
	char_t		*pid_fname = NULL;
	struct stat	st;

	va_start(vargs, fmt);
	if (fmtValloc(&pid_fname, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		trace(0, T("doKillPid: lost data, buffer overflow\n"));
	}
	va_end(vargs);

	if (pid_fname) {
		if (-1 == stat(pid_fname, &st)) //check the file existence
			return 0;
		doSystem("kill `cat %s`", pid_fname);
		doSystem("rm -f %s", pid_fname);
		bfree(B_L, pid_fname);
	}
	return 0;
}

/*
 * description: parse va and do system
 */
int doSystem(char_t *fmt, ...)
{
	va_list		vargs;
	char_t		*cmd = NULL;
	int			rc = 0;

	va_start(vargs, fmt);
	if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		trace(0, T("doSystem: lost data, buffer overflow\n"));
	}
	va_end(vargs);

	if (cmd) {
		rc = system(cmd);
		bfree(B_L, cmd);
	}
	return rc;
}

/*
 * arguments: modname - module name
 * description: test the insertion of module through /proc/modules
 * return: -1 = fopen error, 1 = module inserted, 0 = not inserted yet
 */
int getModIns(char *modname)
{
	FILE *fp;
	char buf[128];
	int i;

	if (NULL == (fp = fopen("/proc/modules", "r"))) {
		error(E_L, E_LOG, T("getModIns: open /proc/modules error"));
		return -1;
	}

	while (NULL != fgets(buf, 128, fp)) {
		i = 0;
		while (!isspace(buf[i++]))
			;
		buf[i - 1] = '\0';
		if (!strcmp(buf, modname)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	//error(E_L, E_LOG, T("getModIns: module %s not inserted"), modname);
	return 0;
}

/*
 * arguments: index - index of the Nth value
 *            values - un-parsed values
 * description: parse values delimited by semicolon, and return the value
 *              according to given index (starts from 0)
 * WARNING: the original values will be destroyed by strtok
 */
char *getNthValue(int index, char *values)
{
	int i;
	static char *tok;

	if (NULL == values)
		return NULL;
	for (i = 0, tok = strtok(values, ";");
			(i < index) && tok;
			i++, tok = strtok(NULL, ";")) {
		;
	}
	if (NULL == tok)
		return "";
	return tok;
}

/*
 * arguments: index - index of the Nth value (starts from 0)
 *            old_values - un-parsed values
 *            new_value - new value to be replaced
 * description: parse values delimited by semicolon,
 *              replace the Nth value with new_value,
 *              and return the result
 * WARNING: return the internal static string -> use it carefully
 */
char *setNthValue(int index, char *old_values, char *new_value)
{
	int i;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
		memset(buf[i], 0, 256);

	//copy original values
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
			i < 8 && q != NULL                         ;
			i++, p = q + 1, q = strchr(p, ';')         )
	{
		strncpy(buf[i], p, q - p);
	}
	strcpy(buf[i], p); //the last one

	//replace buf[index] with new_value
	strncpy(buf[index], new_value, 256);

	//calculate maximum index
	index = (i > index)? i : index;

	//concatenate into a single string delimited by semicolons
	strcat(ret, buf[0]);
	for (i = 1; i <= index; i++) {
		strncat(ret, ";", 2);
		strncat(ret, buf[i], 256);
	}

	p = ret;
	return p;
}

/*
 * arguments: values - values delimited by semicolon
 * description: parse values delimited by semicolon, and return the number of
 *              values
 */
int getValueCount(char *values)
{
	int cnt = 0;

	if (NULL == values)
		return 0;
	while (*values++ != '\0') {
		if (*values == ';')
			++cnt;
	}
	return (cnt + 1);
}

/*
 * check the existence of semicolon in str
 */
int checkSemicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

/*
 * argument: ip address
 * return: 1 = the given ip address is within LAN's scope
 *         0 = otherwise
 */
int isInLan(char *radius_ip_addr)
{
	char lan_if_addr[16];
	char lan_if_netmask[16];

	struct in_addr lan_ip;
	struct in_addr lan_netmask;
	struct in_addr radius_ip;

	if ((getIfIp(getLanIfName(), lan_if_addr)) == -1) {
		printf("getLanIP error\n");
		return 0;
	}
	if ((getIfNetmask(getLanIfName(), lan_if_netmask)) == -1) {
		printf("getLanNetmask error\n");
		return 0;
	}

	inet_aton(lan_if_addr, &lan_ip);
	inet_aton(lan_if_netmask, &lan_netmask);
	inet_aton(radius_ip_addr, &radius_ip);

	printf("lan_ip:%08x\n", lan_ip.s_addr);
	printf("lan_netmask:%08x\n", lan_netmask.s_addr);
	printf("radius_ip:%08x\n", radius_ip.s_addr);

	if ((lan_ip.s_addr & lan_netmask.s_addr) == (radius_ip.s_addr & lan_netmask.s_addr) ){
		printf("in Lan\n");
		return 1;
	} else {
		printf("not in lan\n");
		return 0;
	}
}

/*
 * substitution of getNthValue which dosen't destroy the original value
 */
int getNthValueSafe(int index, char *value, char delimit, char *result, int len)
{
	int i=0, result_len=0;
	char *begin, *end;

	if(!value || !result || !len)
		return -1;

	begin = value;
	end = strchr(begin, delimit);

	while(i<index && end){
		begin = end+1;
		end = strchr(begin, delimit);
		i++;
	}

	//no delimit
	if(!end){
		if(i == index){
			end = begin + strlen(begin);
			result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
		}else
			return -1;
	}else
		result_len = (len-1) < (end-begin)? (len-1) : (end-begin);

	memcpy(result, begin, result_len );
	*(result+ result_len ) = '\0';

	return 0;
}

/*
 *  argument:  [IN]     index -- the index array of deleted items(begin from 0)
 *             [IN]     count -- deleted itmes count.
 *             [IN/OUT] value -- original string/return string
 *             [IN]     delimit -- delimitor
 */
int deleteNthValueMulti(int index[], int count, char *value, char delimit)
{
	char *begin, *end;
	int i=0,j=0;
	int need_check_flag=0;
	char *buf = strdup(value);

	begin = buf;

	end = strchr(begin, delimit);
	while(end){
		if(i == index[j]){
			memset(begin, 0, end - begin );
			if(index[j] == 0)
				need_check_flag = 1;
			j++;
			if(j >=count)
				break;
		}
		begin = end;

		end = strchr(begin+1, delimit);
		i++;
	}

	if(!end && index[j] == i)
		memset(begin, 0, strlen(begin));

	if(need_check_flag){
		for(i=0; i<strlen(value); i++){
			if(buf[i] == '\0')
				continue;
			if(buf[i] == ';')
				buf[i] = '\0';
			break;
		}
	}

	for(i=0, j=0; i<strlen(value); i++){
		if(buf[i] != '\0'){
			value[j++] = buf[i];
		}
	}
	value[j] = '\0';

	free(buf);
	return 0;
}



/*
 * nanosleep(2) don't depend on signal SIGALRM and could cooperate with
 * other SIGALRM-related functions(ex. setitimer(2))
 */
unsigned int Sleep(unsigned int secs)
{
	int rc;
	struct timespec ts, remain;
	ts.tv_sec  = secs;
	ts.tv_nsec = 0;

sleep_again:
	rc = nanosleep(&ts, &remain);
	if(rc == -1 && errno == EINTR){
		ts.tv_sec = remain.tv_sec;
		ts.tv_nsec = remain.tv_nsec;
		goto sleep_again;
	}	
	return 0;
}

/*
 * The setitimer() is Linux-specified.
 */
int setTimer(int microsec, void ((*sigroutine)(int)))
{
	struct itimerval value, ovalue;

	signal(SIGALRM, sigroutine);
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = microsec;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = microsec;
	return setitimer(ITIMER_REAL, &value, &ovalue);
}

void stopTimer(void)
{
	struct itimerval value, ovalue;

	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);
}


#if 0
/*
 * configure LED blinking with proper frequency (privatly use only)
 *   on: number of ticks that LED is on
 *   off: number of ticks that LED is off
 *   blinks: number of on/off cycles that LED blinks
 *   rests: number of on/off cycles that LED resting
 *   times: stop blinking after <times> times of blinking
 * where 1 tick == 100 ms
 */
static int gpioLedSet(int gpio, unsigned int on, unsigned int off,
		unsigned int blinks, unsigned int rests, unsigned int times)
{
	int fd;
	ralink_gpio_led_info led;

	//parameters range check
	if (gpio < 0 || gpio >= RALINK_GPIO_NUMBER ||
			on > RALINK_GPIO_LED_INFINITY ||
			off > RALINK_GPIO_LED_INFINITY ||
			blinks > RALINK_GPIO_LED_INFINITY ||
			rests > RALINK_GPIO_LED_INFINITY ||
			times > RALINK_GPIO_LED_INFINITY) {
		return -1;
	}
	led.gpio = gpio;
	led.on = on;
	led.off = off;
	led.blinks = blinks;
	led.rests = rests;
	led.times = times;

	fd = open("/dev/gpio", O_RDONLY);
	if (fd < 0) {
		perror("/dev/gpio");
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_LED_SET, &led) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int ledAlways(int gpio, int on)
{
	if (on)
		return gpioLedSet(gpio, RALINK_GPIO_LED_INFINITY, 0, 1, 1, RALINK_GPIO_LED_INFINITY);
	else
		return gpioLedSet(gpio, 0, RALINK_GPIO_LED_INFINITY, 1, 1, RALINK_GPIO_LED_INFINITY);
}

int ledWps(int gpio, int mode)
{
	switch (mode) {
		case WPS_LED_RESET:
			return gpioLedSet(gpio, 0, RALINK_GPIO_LED_INFINITY, 1, 1, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_PROGRESS:
			return gpioLedSet(gpio, 2, 1, RALINK_GPIO_LED_INFINITY, 1, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_ERROR:
			return gpioLedSet(gpio, 1, 1, RALINK_GPIO_LED_INFINITY, 1, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_SESSION_OVERLAP:
			return gpioLedSet(gpio, 1, 1, 10, 5, RALINK_GPIO_LED_INFINITY);
			break;
		case WPS_LED_SUCCESS:
			gpioLedSet(gpio, 3000, 1, 1, 1, 1);
			break;
	}
	return 0;
}
#endif

/*
 * concatenate a string with an integer
 * ex: racat("SSID", 1) will return "SSID1"
 */
char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

void websLongWrite(webs_t wp, char *longstr)
{
	char tmp[513] = {0};
	int len = strlen(longstr);
	char *end = longstr + len;

	while(longstr < end){
		strncpy(tmp, longstr, 512);
		websWrite(wp, T("%s"), tmp);
		longstr += 512;
	}
	return;
}

/*********************************************************************
 * Web Related Utilities
 */
static int getSystemInfoSysVersion(int eid, webs_t wp, int argc, char_t **argv)
{   
	char_t *tmp = NULL;
	struct utsname utsbuf;
	uname(&utsbuf);
	tmp = strtok(utsbuf.version, " ");
	tmp = strtok(NULL, " ");
#if(UTT_TOMAX == FYES)
	websWrite(wp, T("var revisions=\"%s-%s-%s\";"), utsbuf.version,UTT_HWMODEL,tmp);
#else
	websWrite(wp, T("var revisions=\"%s %s\";"), utsbuf.version, tmp);
#endif 
	tmp = strtok(NULL, " ");
	return 0;
}
#if (WECHAT_CWMP == FYES)
static int getSystemActiveCode(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int getSystemInfoMachineNo(int eid, webs_t wp, int argc, char_t **argv)
{   
	char_t *tmp = NULL;
#if(RAISECOM_MATCHINE == FYES)
    char_t *tmp_bak = NULL;
    char buf_bak[20] = {0};
#endif
	struct utsname utsbuf;
	//    char *tmscb = "&#8482";

	uname(&utsbuf);
#if(RAISECOM_MATCHINE == FYES)
    strcpy(buf_bak,utsbuf.machine);
#endif
	tmp = strtok(utsbuf.machine, " ");
	tmp = strtok(NULL, " ");

#if 1
	getSystemActiveCode(eid, wp, argc,argv);
#endif
	if(tmp != NULL)
	{
		//		websWrite(wp, T("var ProductID=\"%s%s %s\";\n"), utsbuf.machine, tmscb, tmp);
		websWrite(wp, T("var ProductID=\"%s %s\";\n"), utsbuf.machine, tmp);
	}
	else
	{
#if(RAISECOM_MATCHINE == FYES)
        tmp_bak = strtok(buf_bak,"-");
        tmp_bak = strtok(NULL,"-");
		websWrite(wp, T("var ProductID=\"%s %s\";\n"), buf_bak,tmp_bak);
#else
		websWrite(wp, T("var ProductID=\"%s\";\n"), utsbuf.machine);
#endif
	}
	websWrite(wp, T("var hardwareID=\"%s\";"), UTT_HWMODEL);
	return 0;
}


/*
 * 根据型号下载新软件
 * */
#if 0
static int getLinkDownload(int eid, webs_t wp, int argc, char_t **argv)
{   
#ifdef CONFIG_USER_GOAHEAD_LANG_EN
#if (BRAND_LEVEL_ONE  == FYES)
	websWrite(wp, T("var ProductLinkID=\"http://global.level1.com/downloads.php?action=init\";\n"));
#else
	websWrite(wp, T("var ProductLinkID=\"http://www.uttglobal.com/default.asp?id=3271\";\n"));
#endif
#else
	unsigned int productmodelid[3];/*0，1，2分别对应版本链接编号：中文，英文，繁体*/
	unsigned int filetypeid = 3;/*更新软件索引*/
	unsigned int Lang_Type = 0;/*数组索引*/
	char *LinkDownload[3] = {"http://www.utt.com.cn/downloadcenter.php","http://www.uttglobal.com/downloadcenter.php","http://www.utt.com.cn/downloadcenter.php"};
	MibProfileType mibType = MIB_PROF_LANG;
	LangProfile *prof;
	/*英文编号为0，表示暂时没有英文软件*/

	memset(productmodelid,0,sizeof(productmodelid));
#if((UTT_NV_810V2 == FYES) || (UTT_NV_810V3 == FYES) || (UTT_NV_810V4 == FYES)|| (UTT_NV_830V3 == FYES) || (UTT_NV_830V2 == FYES))
	{
		productmodelid[0]=83;
		productmodelid[1]=0;
		productmodelid[2]=83;
	}
#elif((UTT_NV_840V2 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_840V4 == FYES))
	{
		productmodelid[0]=93;
		productmodelid[1]=0;
		productmodelid[2]=93;
	}
#elif(UTT_LV_5830G == FYES)
	{
		productmodelid[0]=104;
		productmodelid[1]=0;
		productmodelid[2]=104;
	}
#elif(UTT_LV_6830G == FYES)
	{
		productmodelid[0]=105;
		productmodelid[1]=0;
		productmodelid[2]=105;
	}
#elif(UTT_LV_2200 == FYES)
	{
		productmodelid[0]=121;
		productmodelid[1]=0;
		productmodelid[2]=121;
	}
#elif(UTT_LV_4000 == FYES)
	{
		productmodelid[0]=122;
		productmodelid[1]=0;
		productmodelid[2]=122;
	}
#elif(UTT_LV_6000 == FYES)
	{
		productmodelid[0]=123;
		productmodelid[1]=0;
		productmodelid[2]=123;
	}
#elif((UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES))
	{
		productmodelid[0]=139;
		productmodelid[1]=144;
		productmodelid[2]=139;
	}
#elif(UTT_NV_640E == FYES)
	{
		productmodelid[0]=139;
		productmodelid[1]=144;
		productmodelid[2]=139;
	}
#elif(UTT_NV_510W == FYES)
	{
		productmodelid[0]=141;
		productmodelid[1]=166;
		productmodelid[2]=141;
	}
#elif((UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES))
	{
		productmodelid[0]=166;
		productmodelid[1]=139;
		productmodelid[2]=166;
	}
#elif(UTT_MV_821 == FYES)
	{
		productmodelid[0]=173;
		productmodelid[1]=0;
		productmodelid[2]=173;
	}
#elif(UTT_NV_1540 == FYES)
	{
		productmodelid[0]=177;
		productmodelid[1]=0;
		productmodelid[2]=177;
	}
#elif(UTT_NV_840G == FYES)
	{
		productmodelid[0]=178;
		productmodelid[1]=182;
		productmodelid[2]=178;
	}
#elif((UTT_NV_520W == FYES) || (UTT_NV_520WV3 == FYES))
	{
		productmodelid[0]=179;
		productmodelid[1]=0;
		productmodelid[2]=179;
	}
#elif(UTT_NV_588 == FYES)
	{
		productmodelid[0]=184;
		productmodelid[1]=83;
		productmodelid[2]=184;
	}
#elif(UTT_NV_540 == FYES)
	{
		productmodelid[0]=186;
		productmodelid[1]=0;
		productmodelid[2]=186;
	}
#elif(UTT_U_1000N == FYES)
	{
		productmodelid[0]=197;
		productmodelid[1]=0;
		productmodelid[2]=197;
	}
#elif(UTT_QV_N1800 == FYES)
	{
		productmodelid[0]=152;
		productmodelid[1]=0;
		productmodelid[2]=152;
	}

#elif(UTT_NV_3320G == FYES)
	{
		productmodelid[0]=198;
		productmodelid[1]=0;
		productmodelid[2]=198;
	} 
#elif((UTT_NV_512W == FYES) || (UTT_NV_512WV2 == FYES))
	{
		productmodelid[0]=199;
		productmodelid[1]=0;
		productmodelid[2]=199;
	}
#elif((UTT_NV_518W == FYES) || (UTT_NV_518WV2 == FYES))
	{
		productmodelid[0]=200;
		productmodelid[1]=0;
		productmodelid[2]=200;
	}
#elif(UTT_NV_842W == FYES)
	{
		productmodelid[0]=201;
		productmodelid[1]=0;
		productmodelid[2]=201;
	}
#elif(UTT_NV_842WV3 == FYES)
	{
		productmodelid[0]=201;
		productmodelid[1]=0;
		productmodelid[2]=201;
	}
#elif(UTT_NV_521G == FYES)
	{
		productmodelid[0]=202;
		productmodelid[1]=0;
		productmodelid[2]=202;
	}
#elif(UTT_LV_4330G == FYES)/*宏未知*/
	{
		productmodelid[0]=203;
		productmodelid[1]=0;
		productmodelid[2]=203;
	}
#elif(UTT_QV_NE1200 == FYES)
	{
		productmodelid[0]=120;
		productmodelid[1]=0;
		productmodelid[2]=120;
	}

#elif(UTT_QV_N800 == FYES)
	{
		productmodelid[0]=151;
		productmodelid[1]=0;
		productmodelid[2]=151;
	}

#elif(UTT_QV_4840 == FYES)
	{
		productmodelid[0]=112;
		productmodelid[1]=0;
		productmodelid[2]=112;
	}
#elif(UTT_NV_A307W == FYES)
	{
		productmodelid[0]=227;
		productmodelid[1]=0;
		productmodelid[2]=227;
	}
#elif((UTT_NV_A308W == FYES)|| (UTT_NV_WBR_6013 == FYES))
	{
		productmodelid[0]=204;
		productmodelid[1]=0;
		productmodelid[2]=204;
	}
#elif(UTT_NV_A309W == FYES)
	{
		productmodelid[0]=294;
		productmodelid[1]=0;
		productmodelid[2]=294;
	}
#elif(UTT_NV_A650W == FYES || UTT_NV_WBR_8001 == FYES)
	{
		productmodelid[0]=233;
		productmodelid[1]=0;
		productmodelid[2]=233;
	}
#elif(UTT_NV_A755W == FYES)
	{
		productmodelid[0]=282;
		productmodelid[1]=0;
		productmodelid[2]=282;
	}
#elif(defined(CONFIG_BOARD_MTK7628NN_A_V1_0)) /*具体值待定*/
	{
		productmodelid[0]=282;
		productmodelid[1]=0;
		productmodelid[2]=282;
	}

#elif(UTT_NV_A655W == FYES)/*具体值待定*/
	{
		productmodelid[0]=277;
		productmodelid[1]=0;
		productmodelid[2]=277;
	}

#elif(UTT_NV_A658W == FYES)/*具体值待定*/
	{
		productmodelid[0]=283;
		productmodelid[1]=0;
		productmodelid[2]=283;
	}

#elif((UTT_NV_A310W == FYES) || (UTT_NV_A310Wv2 == FYES))
	{
		productmodelid[0]=228;
		productmodelid[1]=0;
		productmodelid[2]=228;
	}
#elif(UTT_U_3000 == FYES) 
	{ 
		productmodelid[0]=195; 
		productmodelid[1]=0; 
		productmodelid[2]=195; 
	}   
#elif(UTT_U_4000 == FYES)
	{
		productmodelid[0]=194;
		productmodelid[1]=0;
		productmodelid[2]=194;
	}
#elif(UTT_NV_1220G == FYES)
	{
		productmodelid[0]=210;
		productmodelid[1]=0;
		productmodelid[2]=210;
	}
#elif(UTT_LV_6530G == FYES)
	{
		productmodelid[0]=103;
		productmodelid[1]=0;
		productmodelid[2]=103;
	}
#elif(UTT_NV_D910 == FYES)
	{
		productmodelid[0]=238;
		productmodelid[1]=0;
		productmodelid[2]=238;
	}
#elif(UTT_U_1900 == FYES)
	{
		productmodelid[0]=196;
		productmodelid[1]=0;
		productmodelid[2]=196;
	}
#elif(UTT_U_1100 == FYES)
	{
		productmodelid[0]=171;
		productmodelid[1]=0;
		productmodelid[2]=171;
	}

#elif defined (CONFIG_UTT_D915W)
	{
		productmodelid[0]=240;
		productmodelid[1]=0;
		productmodelid[2]=240;
	}
#elif(UTT_QV_4240G == FYES)
	{
		productmodelid[0]=91;
		productmodelid[1]=0;
		productmodelid[2]=91;
	}
#elif(UTT_QV_2620G == FYES)
	{
		productmodelid[0]=86;
		productmodelid[1]=0;
		productmodelid[2]=86;
	}
#endif
	prof = (LangProfile*)ProfGetInstPointByIndex(mibType, 0);

	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		if(!strcmp(prof->lang,"en"))  
			Lang_Type=1;
		else if(!strcmp(prof->lang,"zhtw"))  
			Lang_Type=2;
	}
	/*http://www.utt.com.cn/downloadcenter.php?filetypeid=3&productmodelid=173*/
	websWrite(wp, T("var ProductLinkID=\"%s?filetypeid=%d&productmodelid=%d\";\n"), LinkDownload[Lang_Type],filetypeid,productmodelid[Lang_Type]);
#endif
	return 0;
}
#endif 

static int getLinkDownload(int eid, webs_t wp, int argc, char_t **argv)
{   
#if 0 //def CONFIG_USER_GOAHEAD_LANG_EN
	websWrite(wp, T("var ProductLinkID=\"http://www.uttglobal.com/default.asp?id=3271\";\n"));
#else 
	unsigned int productmodelid[3];/*0，1，2分别对应版本链接编号：中文，英文，繁体*/
	unsigned int filetypeid = 3;/*更新软件索引*/
	unsigned int Lang_Type = 0;/*数组索引*/
	char_t *tmp = NULL,*p=NULL,*q=NULL;
	struct utsname utsbuf;
	uname(&utsbuf);
	tmp = strtok(utsbuf.version, " ");
	tmp = strtok(NULL, " ");

	char *LinkDownload[3] = {"http://www.utt.com.cn/downloadcenter.php","http://www.uttglobal.com/downloadcenter.php","http://www.utt.com.cn/downloadcenter.php"};
	MibProfileType mibType = MIB_PROF_LANG;
	LangProfile *prof;
	/*英文编号为0，表示暂时没有英文软件*/
	prof = (LangProfile*)ProfGetInstPointByIndex(mibType, 0);

	if((prof != NULL) && (ProfInstIsFree(prof) == 0))
	{
		if(!strcmp(prof->lang,"en"))  
			Lang_Type=1;
		else if(!strcmp(prof->lang,"zhtw"))  
			Lang_Type=2;
	}
	/*http://www.utt.com.cn/downloadcenter.php?filetypeid=3&productmodelid=173*/
	/* websWrite(wp, T("var ProductLinkID=\"%s?filetypeid=%d&productmodelid=%d\";\n"), LinkDownload[Lang_Type],filetypeid,productmodelid[Lang_Type]); */
	p=utsbuf.version;
	if(strstr(utsbuf.version,"ER")==NULL){//非ER系列
		p=p+2;
	}
	q=p;
	while((*p != 'v')&&(*p != 'V')&&(*p != 0)){
		p++;
	}
	*p = '\0';
	if(Lang_Type == 1)
	{
		/*国外版本链接*/
		websWrite(wp, T("var ProductLinkID=\"http://www.utt.com.cn/downloadcenter.php?filetypeid=%d&model=%s&lang=%s\";\n"),filetypeid , q,prof->lang);
	}
	else
	{
		websWrite(wp, T("var ProductLinkID=\"%s?filetypeid=%d&model=%s&lang=%s\";\n"),LinkDownload[Lang_Type],filetypeid, q,prof->lang);
	}
#endif
	return 0;
}

static int getSystemInfoSysUpTime(int eid, webs_t wp, int argc, char_t **argv)
{   
	int updays=0,uphours=0,upminutes=0,upsecs=0;
	struct sysinfo info;
	const char *com_day = NULL, *com_hour = NULL, *com_minute = NULL, *com_second = NULL;
	sysinfo(&info);
	updays = (int) info.uptime / (60*60*24);
	upminutes = (int) info.uptime / 60;
	uphours = (upminutes / 60) % 24;
	upminutes %= 60;
	upsecs =info.uptime % 60;
	com_day = getTransDataForC(C_LANG_INDEX_DAYS);
	com_hour = getTransDataForC(C_LANG_INDEX_HOURS);
	com_minute = getTransDataForC(C_LANG_INDEX_MINUTES);
	com_second = getTransDataForC(C_LANG_INDEX_SECONDS);
	if ( com_day != NULL &&
			com_hour != NULL &&
			com_minute != NULL &&
			com_second != NULL ) {
		websWrite(wp, T("var runtimes=\"%d %s %d %s %d %s %d %s \";"), updays, com_day, uphours, com_hour, upminutes, com_minute, upsecs, com_second);
	}
	return 0;

}

#if 0
static void formConfigWatchdog(webs_t wp, char_t *path, char_t *query)
{
	char_t *wdgEn, *oldWdgEn;
	oldWdgEn = nvram_bufget(RT2860_NVRAM,"WDGENABLE");
	wdgEn = websGetVar(wp, T("watchdog"), T(""));
	if(strcmp(wdgEn, oldWdgEn))
	{
		nvram_bufset(RT2860_NVRAM, "WDGENABLE", wdgEn);
		nvram_commit(RT2860_NVRAM);
		if(strcmp(wdgEn, "on") == 0){
			doSystem("insmod rt_timer");
		} else {
			doSystem("rmmod rt_timer");
		}
	}
	websRedirect(wp, "CallTrace.asp");
}

static int getWatchdogEn(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("var wdgEn = \"%s\";"), nvram_bufget(RT2860_NVRAM,"WDGENABLE"));
}

static int getSystemCallTrace(int eid, webs_t wp, int argc, char_t **argv)
{
	int fd, i;
	void * map_base;
	if((fd=open("/dev/mem",O_RDWR|O_SYNC))==-1){
		return(-1);
	}
	map_base=mmap(0,0xff,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0xbf030000);
	for(i = 0; i < 64; i++){
		if(*(((unsigned int *) map_base)+i) == 0xffffffff){
			break;
		}else{
			websWrite(wp, T("%x\n"),*(((unsigned int *) map_base)+i));
		}
	}
	munmap(map_base,0xff);
	close(fd);
	return 0;
}
#endif


extern unsigned int productSn;
static int getSystemInfoSn(int eid, webs_t wp, int argc, char_t **argv)
{   
	websWrite(wp, T("var productIDs = \"%u\";"), productSn);
	return 0;
}

#if (WIFIDOG_AUTH == FYES) || (MAKE_MONEY) || (WECHAT_CWMP == FYES)
char* DES_Encrypt(char *plain, char *keyStr,int *cipherlen);
char* UTT_DES_Encrypt(char *plain, char *keyStr,int *cipherlen);
#endif
/*
 * function: getSystemActiveCode
 * 页面调用函数，生成8位设备激活码
 * 如果激活码加密失败，则返回设备序列号
 * 返回值
 *		通过序列号des加密后数据取其前8为作为激活码
 *		若加密失败，返回序列号值
 * */
static int getSystemActiveCode(int eid, webs_t wp, int argc, char_t **argv)
{
#if (WIFIDOG_AUTH == FYES) || (MAKE_MONEY == FYES) || (WECHAT_CWMP == FYES)
	char *cipherStr = NULL;
	int cipherLen = 0;
	char snStr[10];
#if (WECHAT_CWMP == FYES)
	char param[64];
	char timeStr[20];
	time_t t = time(NULL);
	struct tm *tblock=localtime(&t);
	char *tmp_parm;
#endif

	memset(snStr, 0, 10);
	snprintf(snStr, 10, "%u",productSn);
	/*序列号des加密*/
	if((cipherStr = DES_Encrypt(snStr, snStr, &cipherLen)) != NULL) {
		*(cipherStr+6) = '\0';
		/*返回加密数据前8位做激活码*/
		websWrite(wp,T("var ActCode = \"%s\";"), cipherStr);
#if (WECHAT_CWMP == FYES)
		if(tblock != NULL){
			snprintf(timeStr,20,"%04d%02d%02d%02d%02d%02d",(tblock->tm_year+1900),(tblock->tm_mon+1),(tblock->tm_mday-1), tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
		}
		snprintf(param,64,"sn=%u&activecode=%s&timestamp=%s",productSn,cipherStr,timeStr);
		if((tmp_parm = UTT_DES_Encrypt(param,"1623`@wifi", &cipherLen)) != NULL) {
			websWrite(wp,T("var info = \"%s\";"),tmp_parm);
			free(tmp_parm);
		}else{
			websWrite(wp,T("var info = \"\";"));
		}
#endif
		free(cipherStr);
	} else {
		/*返回序列号*/
		websWrite(wp,T("var ActCode = \"%u\";"),productSn);
		websWrite(wp,T("var info = \"\";"));
	}
#else
	/*未开启wifidog。则返回空*/
	websWrite(wp,T("var ActCode = \"\";"));
	websWrite(wp,T("var info = \"\";"));
#endif
	return 0;
}

#if(WIFIDOG_AUTH == FYES)||(MAKE_MONEY == FYES)
static char convertTab[2]={'+','/'}; /*2b, 2f*/
static void convertActCode(const char *code, char *encode)
{
	int i=0, j=0;
	int len = 0;
	int flag =0;

	if((code != NULL) && (strlen(code) == 6) && (encode != NULL)) {

		for(i=0;i<6;i++)
		{
			len = strlen(encode);
			flag = 0;
			for(j=0;j<2;j++)
			{
				if(code[i] == convertTab[j])
				{
					snprintf(encode+len,20-len, "%c%x", '%', code[i]);
					flag = 1;
					break;
				}
			}
			if(flag == 0)
			{
				snprintf(encode+len, 20-len, "%c", code[i]);
			}
		}
	}
	return;
}
#endif

static int getWifiTip(int eid, webs_t wp, int argc, char **argv)
{
#if(WIFIDOG_AUTH == FYES) || (MAKE_MONEY == FYES)
	char *cipherStr = NULL;
	int cipherLen = 0;
	char snStr[10];
	char encodeStr[19];

	memset(encodeStr, 0, sizeof(encodeStr));
	memset(snStr, 0, 10);
	snprintf(snStr, 10, "%u",productSn);
	/*序列号des加密*/
	if((cipherStr = DES_Encrypt(snStr, snStr, &cipherLen)) != NULL) {
		*(cipherStr+6) = '\0';
		convertActCode(cipherStr, encodeStr);
		if(strlen(encodeStr) > 0) {
			websWrite(wp, T("sn=%u&code=%s"), productSn, encodeStr);
		} else {
			websWrite(wp, T("sn=%u&code=%s"), productSn, cipherStr);
		}
	} else {
		websWrite(wp, T("sn=%u&code=%u"), productSn, productSn);
	}
#else
	websWrite(wp, T(""));
#endif
}
static void getSaleWifiSn(int eid, webs_t wp, int argc, char **argv)
{
	char *cipherStr = NULL;
	int cipherLen = 0;
	char snStr[10];
	char encodeStr[19];

	memset(encodeStr, 0, sizeof(encodeStr));
	memset(snStr, 0, 10);
	snprintf(snStr, 10, "%u",productSn);
	/*序列号des加密*/
	if((cipherStr = DES_Encrypt(snStr, snStr, &cipherLen)) != NULL) {
		*(cipherStr+6) = '\0';
		convertActCode(cipherStr, encodeStr);
		if(strlen(encodeStr) > 0) {
			websWrite(wp, T("sn=%u&activecode=%s"), productSn, encodeStr);
		} else {
			websWrite(wp, T("sn=%u&activecode=%s"), productSn, cipherStr);
		}
	} else {
		websWrite(wp, T("sn=%u&activecode=%u"), productSn, productSn);
	}
}

#if (UTT_TOMAX == FYES)
static void getWirelessBaseInfo(int eid, webs_t wp, int argc, char_t **argv)
{
#if (WIRELESS == FYES)
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof= NULL,*prof_5g = NULL;
	prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
	if(prof == NULL){
		return 0;
	}

	websWrite(wp,"var wireless= 1;\n");
	if (prof->head.active == 1) {
		websWrite(wp,"var WrlessEnables=\"%s\";",T("on"));
	} else{
		websWrite(wp,"var WrlessEnables=\"%s\";",T("off"));
	}
	websWrite(wp,"var ssids=\"%s\";",prof->mBaseCf.SSID1);
#if(WLAN_MULTI_SSID == FYES)
	websWrite(wp,"var encodeType=\"%s\";",prof->mBaseCf.encodeType);
	websWrite(wp,"var ssid2s=\"%s\";",prof->mBaseCf.SSID2);
	websWrite(wp,"var encodeType2=\"%s\";",prof->mBaseCf.encodeType2);
#endif
	websWrite(wp,"var APWorkModes=\"%d\";",prof->mBaseCf.apWorkMode);
#if (WLAN_MULTI_SSID == FYES)
	websWrite(wp,"var multi_ssid = 1;");
#else
	websWrite(wp,"var multi_ssid = 0;");
#endif
	websWrite(wp,"var wrlessModes=\"%d\";",prof->mBaseCf.WirelessMode);
	websWrite(wp,"var broadcastEnables=\"%d\";",prof->mBaseCf.SSIDBroadCastEn);
	websWrite(wp,"var channels=\"%d\";",prof->mBaseCf.Channel);
	if (prof->mBaseCf.ChanWidth == 1) {
		if (prof->mBaseCf.HT_BSSCoexistence == 0) {
			websWrite(wp,"var chanWidths=\"%d\";\n", 2);
		} else {
			websWrite(wp,"var chanWidths=\"%d\";\n", 1);
		}
	} else {
		websWrite(wp,"var chanWidths=\"%d\";\n", 0);
	}    
#if(WIRELESS_5G == FYES)
	prof_5g = (WirelessProfile*)ProfGetInstPointByIndex(mibType,1);
	if(prof_5g == NULL){
		return 0;
	}
	websWrite(wp,"var wireless_5g= 1;\n");
	if (prof_5g->head.active == 1) {
		websWrite(wp,"var WrlessEnables_5g=\"%s\";",T("on"));
	} else{
		websWrite(wp,"var WrlessEnables_5g=\"%s\";",T("off"));
	}
	websWrite(wp,"var ssids_5g=\"%s\";",prof_5g->mBaseCf.SSID1);
#if(WLAN_MULTI_SSID == FYES)
	websWrite(wp,"var encodeType_5g=\"%s\";",prof_5g->mBaseCf.encodeType);
	websWrite(wp,"var ssid2s_5g=\"%s\";",prof_5g->mBaseCf.SSID2);
	websWrite(wp,"var encodeType2_5g=\"%s\";",prof_5g->mBaseCf.encodeType2);
#endif
	websWrite(wp,"var APWorkModes_5g=\"%d\";",prof_5g->mBaseCf.apWorkMode);
#if (WLAN_MULTI_SSID == FYES)
	websWrite(wp,"var multi_ssid_5g = 1;");
#else
	websWrite(wp,"var multi_ssid_5g = 0;");
#endif
	websWrite(wp,"var wrlessModes_5g=\"%d\";",prof_5g->mBaseCf.WirelessMode);
	websWrite(wp,"var broadcastEnables_5g=\"%d\";",prof_5g->mBaseCf.SSIDBroadCastEn);
	websWrite(wp,"var channels_5g=\"%d\";",prof_5g->mBaseCf.Channel);
	if (prof_5g->mBaseCf.ChanWidth == 1) {
		if (prof_5g->mBaseCf.HT_BSSCoexistence == 0) {
			websWrite(wp,"var chanWidths_5g=\"%d\";\n", 2);
		} else {
			websWrite(wp,"var chanWidths_5g=\"%d\";\n", 1);
		}
	} else {
		websWrite(wp,"var chanWidths_5g=\"%d\";\n", 0);
	}    
#else
	websWrite(wp,"var wireless_5g= 0;\n");
#endif
#else
	websWrite(wp,"var wireless= 0;");
#endif
}

static int aspOutPutApBasicInfoUrcp(int eid, webs_t wp, int argc, char **argv)
{
	return 0;
}
#endif


void formDefineUtilities(void)
{
	websAspDefine(T("outputSystemVersion"), getSystemInfoSysVersion);
	websAspDefine(T("outputProductID"), getSystemInfoMachineNo);
	websAspDefine(T("outputLinkID"), getLinkDownload);
	websAspDefine(T("outputSystemUpTime"), getSystemInfoSysUpTime);
	websAspDefine(T("outputSystemSn"), getSystemInfoSn);
	websAspDefine(T("outputActiveCode"), getSystemActiveCode);
	websAspDefine(T("outputwifiTip"), getWifiTip);
#if(UTT_TOMAX == FYES)
	websAspDefine(T("outputWlanBaseInfo"), getWirelessBaseInfo);
#if (!defined(FEATURE_AP_BASIC_INFO))
	websAspDefine(T("aspOutPutApBasicInfoUrcp"), aspOutPutApBasicInfoUrcp);
#endif
#endif

#if (MAKE_MONEY == FYES)
	websAspDefine(T("getSaleWifiSn"), getSaleWifiSn);
#endif
	websAspDefine(T("getLangBuilt"), getLangBuilt);
#if 0
	websAspDefine(T("aspOutCallTrace"), getSystemCallTrace);
	websAspDefine(T("aspOutWatchdogEn"), getWatchdogEn);
	websFormDefine(T("formConfigWatchdog"), formConfigWatchdog);
	websAspDefine(T("getCfgNthZero"), getCfgNthZero);
	websAspDefine(T("getCfgGeneral"), getCfgGeneral);
	websAspDefine(T("getCfgZero"), getCfgZero);
	websAspDefine(T("getCfgNthGeneral"), getCfgNthGeneral);
	websAspDefine(T("getDpbSta"), getDpbSta);
	websAspDefine(T("getMiiInicBuilt"), getMiiInicBuilt);
	websAspDefine(T("getStationBuilt"), getStationBuilt);
	websAspDefine(T("getPlatform"), getPlatform);
	websAspDefine(T("getSysBuildTime"), getSysBuildTime);
	websFormDefine(T("forceMemUpgrade"), forceMemUpgrade);
	websAspDefine(T("isOnePortOnly"), isOnePortOnly);
	websFormDefine(T("setOpMode"), setOpMode);
	websAspDefine(T("getPortStatus"), getPortStatus);
#endif
	websAspDefine(T("getSysUptime"), getSysUptime);
#if 0
#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
	websFormDefine(T("ScanUSBFirmware"), ScanUSBFirmware);
#endif
#endif
#if 0
	websAspDefine(T("getWebMenuConfig"), getWebMenuConfig);
#endif
}


#if 0
/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), value);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}
#endif

#if 0
/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), nth);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfgZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}
#endif

#if 0
/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfg2General(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RTINIC_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), value);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfg2NthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RTINIC_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), nth);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfg2Zero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RTINIC_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfg2NthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RTINIC_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/* 
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfg3General(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = nvram_bufget(RT2561_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), value);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field' 
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfg3Zero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args"));
	}
	value = nvram_bufget(RT2561_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

static int getDpbSta(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RT2860V2_STA_DPB
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}
#endif

static int getLangBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *lang;

	if (ejArgs(argc, argv, T("%s"), &lang) < 1) {
		return websWrite(wp, T("0"));
	}
	if (!strncmp(lang, "en", 3))
#ifdef CONFIG_USER_GOAHEAD_LANG_EN
		return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
	else if (!strncmp(lang, "zhtw", 5))
#ifdef CONFIG_USER_GOAHEAD_LANG_ZHTW
		return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
	else if (!strncmp(lang, "zhcn", 5))
#ifdef CONFIG_USER_GOAHEAD_LANG_ZHCN
		return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif

	return websWrite(wp, T("0"));
}


#if 0
static int getMiiInicBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_INIC_MII)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write the current system platform accordingly
 */
static int getPlatform(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RAETH_ROUTER
	return websWrite(wp, T("RT2880 with IC+ MACPHY"));
#endif
#ifdef CONFIG_ICPLUS_PHY
	return websWrite(wp, T("RT2880 with IC+ PHY"));
#endif
#ifdef CONFIG_RT_MARVELL
	return websWrite(wp, T("RT2880 with MARVELL"));
#endif
#ifdef CONFIG_MAC_TO_MAC_MODE
	return websWrite(wp, T("RT2880 with Vitesse"));
#endif
#ifdef CONFIG_RT_3052_ESW
	return websWrite(wp, T("RT3052 embedded switch"));
#endif

	return 0;
}

static int getStationBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}
#endif

#if 0
/*
 * description: write System build time
 */
static int getSysBuildTime(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), __DATE__);
}
#endif

#if 0
/*
 * description: write RT288x SDK version
 */
static int getSdkVersion(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), RT288X_SDK_VERSION);
}
#endif


/*
 * description: write System Uptime
 */
static int getSysUptime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);

	if (utime->tm_hour > 0)
		return websWrite(wp, T("%d hour%s, %d min%s, %d sec%s"),
				utime->tm_hour, (utime->tm_hour == 1)? "" : "s",
				utime->tm_min, (utime->tm_min == 1)? "" : "s",
				utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	else if (utime->tm_min > 0)
		return websWrite(wp, T("%d min%s, %d sec%s"),
				utime->tm_min, (utime->tm_min == 1)? "" : "s",
				utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	else
		return websWrite(wp, T("%d sec%s"),
				utime->tm_sec, (utime->tm_sec == 1)? "" : "s");
	return 0;
}

#if 0
static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
#if (defined (CONFIG_RAETH_ROUTER) || defined CONFIG_RT_3052_ESW) && defined (CONFIG_USER_ETHTOOL)
	int port, rc;
	FILE *fp;
	char buf[1024];

	for(port=0; port<5; port++){
		char *pos;
		char link = '0';
		int speed = 100;
		char duplex = 'F';
		FILE *proc_file = fopen("/proc/rt2880/gmac", "w");
		if(!proc_file){
			websWrite(wp, T("-1"));		// indicate error
			return 0;
		}
		fprintf(proc_file, "%d", port);
		fclose(proc_file);

		if((fp = popen("ethtool eth2", "r")) == NULL){
			websWrite(wp, T("-1"));		// indicate error
			return 0;
		}
		rc = fread(buf, 1, 1024, fp);
		pclose(fp);
		if(rc == -1){
			websWrite(wp, T("-1"));		// indicate error
			return 0;
		}else{
			// get Link status
			if((pos = strstr(buf, "Link detected: ")) != NULL){
				pos += strlen("Link detected: ");
				if(*pos == 'y')
					link = '1';
			}
			// get speed
			if((pos = strstr(buf, "Speed: ")) != NULL){
				pos += strlen("Speed: ");
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == 'M')
					speed = 10;
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == '0' && *(pos+3) == '0' && *(pos+4) == 'M')
					speed = 1000;
			}
			// get duplex
			if((pos = strstr(buf, "Duplex: ")) != NULL){
				pos += strlen("Duplex: ");
				if(*pos == 'H')
					duplex = 'H';
			}

			websWrite(wp, T("%c,%d,%c,"), link, speed, duplex);
		}
	}
	return 0;
#else
	websWrite(wp, T("-1"));
	return 0;
#endif

}

inline int getOnePortOnly(void)
{
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
	return 0;
#elif defined CONFIG_ICPLUS_PHY
	return 1;
#else
	return 0;
#endif
	return 0;
}

static int isOnePortOnly(int eid, webs_t wp, int argc, char_t **argv)
{
	if( getOnePortOnly() == 1)
		websWrite(wp, T("true"));
	else
		websWrite(wp, T("false"));		 
	return 0;
}
#endif

void redirect_wholepage(webs_t wp, const char *url)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("<html><head><script language=\"JavaScript\">"));
	websWrite(wp, T("parent.location.replace(\"%s\");"), url);
	websWrite(wp, T("</script></head></html>"));
}

int netmask_aton(const char *ip)
{
	int i, a[4], result = 0;
	sscanf(ip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	for(i=0; i<4; i++){	//this is dirty
		if(a[i] == 255){
			result += 8;
			continue;
		}
		if(a[i] == 254)
			result += 7;
		if(a[i] == 252)
			result += 6;
		if(a[i] == 248)
			result += 5;
		if(a[i] == 240)
			result += 4;
		if(a[i] == 224)
			result += 3;
		if(a[i] == 192)
			result += 2;
		if(a[i] == 128)
			result += 1;
		//if(a[i] == 0)
		//	result += 0;
		break;
	}
	return result;
}

#if 0
static void forceMemUpgrade(webs_t wp, char_t *path, char_t *query)
{
	char_t *mode  = websGetVar(wp, T("ForceMemUpgradeSelect"), T("0"));
	if(!mode)
		return;
	if(!strcmp(mode, "1"))
		nvram_bufset(RT2860_NVRAM, "Force_mem_upgrade", "1");
	else
		nvram_bufset(RT2860_NVRAM, "Force_mem_upgrade", "0");
	nvram_commit(RT2860_NVRAM);
	websHeader(wp);
	websWrite(wp, T("<h2>force mem upgrade</h2>\n"));
	websWrite(wp, T("mode: %s<br>\n"), mode);
	websFooter(wp);
	websDone(wp, 200);	
}

#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
static void ScanUSBFirmware(webs_t wp, char_t *path, char_t *query)
{
	setFirmwarePath();
	printf("%s enter\n", __FUNCTION__);

	websRedirect(wp, "adm/upload_firmware.asp");
}
#endif
#endif

#if 0
/* goform/setOpMode */
static void setOpMode(webs_t wp, char_t *path, char_t *query)
{
	char_t	*mode, *nat_en,*newmode;
	char	*old_mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	char	*old_nat = nvram_bufget(RT2860_NVRAM, "natEnabled");
	int		need_commit = 0;
	char *oldwanconn=nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
	char	*wan_ip, *lan_ip;

#endif
#ifdef CONFIG_RT2860V2_STA_DPB
	char_t	*econv = "";
#endif
#if defined INIC_SUPPORT || defined INICv2_SUPPORT
	char_t	*mii;
#endif

	mode = websGetVar(wp, T("opMode"), T("0")); 
	newmode=mode;
	if (!strncmp(newmode, "4", 2)) newmode=T("1");
	//if mode=4 then change mode=1 and wanconn=3G
	/*if (!strncmp(mode, "4", 2)) {
	  mode=T("1");

	  changmode=1;
	  }*/
	nat_en = websGetVar(wp, T("natEnbl"), T("0"));

	if (!strncmp(old_mode, "0", 2)) {
	}
	else if (!strncmp(old_mode, "1", 2) || !strncmp(old_mode, "3", 2)) {//gateway or ap client or 3g //add by cxhu
		if (!strncmp(mode, "0", 2)) {//bridge
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: gateway (or ap-client) -> bridge
			 * config: wan_ip(wired) overwrites lan_ip(bridge)
			 */
			wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", wan_ip);
			need_commit = 1;
#endif
		}
		if (!strncmp(mode, "2", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: gateway (or ap-client) -> ethernet-converter
			 * config: wan_ip(wired) overwrites lan_ip(wired) 
			 *         lan_ip(wireless) overwrites wan_ip(wireless)
			 */
			wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
			lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", wan_ip);
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", lan_ip);
			need_commit = 1;
#endif
		}
	}
	else if (!strncmp(old_mode, "2", 2)) {
		if (!strncmp(mode, "0", 2)) {
			/*
			 * mode: wireless-isp -> bridge
			 * config: lan_ip(wired) overwrites lan_ip(bridge) -> the same
			 */
		}
		else if (!strncmp(mode, "1", 2) || !strncmp(mode, "3", 2)|| !strncmp(mode, "4", 2)) {//add by cxhu
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: ethernet-converter -> gateway (or ap-client)
			 * config: lan_ip(wired) overwrites wan_ip(wired) 
			 *         wan_ip(wireless) overwrites lan_ip(wireless)
			 */
			wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
			lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", wan_ip);
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", lan_ip);
			need_commit = 1;
#endif
		}
	}

#ifdef CONFIG_RT2860V2_STA_DPB
	if (!strncmp(mode, "0", 2)) {
		char *old;

		econv = websGetVar(wp, T("ethConv"), T("0"));
		old = nvram_bufget(RT2860_NVRAM, "ethConvert");
		if (strncmp(old, econv, 2)) {
			nvram_bufset(RT2860_NVRAM, "ethConvert", econv);
			need_commit = 1;
		}
		if (!strncmp(econv, "1", 2)) {
			//disable dhcp server in this mode
			old = nvram_bufget(RT2860_NVRAM, "dhcpEnabled");
			if (!strncmp(old, "1", 2)) {
				nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
				need_commit = 1;
			}
		}
	}
#endif

	//new OperationMode
	//printf("mode=%s,ole_mode=%s\n",mode,old_mode);
	if (strncmp(mode, old_mode, 2)) {
		//printf("mode=%s,ole_mode=%s\n",mode,old_mode);
		if (!strncmp(mode,"4", 2)) {//add by cxhu
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", T("3G"));
		}
		if (!strncmp(mode, "1", 2)&&!strncmp(oldwanconn, "3G", 3))//add by cxhu
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", T("DHCP"));
		//from or to ap client mode
		if (!strncmp(mode, "3", 2)){
			if(!strncmp(oldwanconn, "3G", 3))
				nvram_bufset(RT2860_NVRAM, "wanConnectionMode", T("DHCP"));
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
			nvram_bufset(RT2860_NVRAM, "apworkmode", "5");
		}
		if (!strncmp(old_mode, "3", 2)){
			//printf("old_mode=%s\n",old_mode);
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
			nvram_bufset(RT2860_NVRAM, "apworkmode", "1");
		}
		nvram_bufset(RT2860_NVRAM, "OperationMode", newmode);
		need_commit = 1;
	}
	if(!strncmp(mode, old_mode, 2)&&!strncmp(mode, "1", 2)&&!strncmp(oldwanconn, "3G", 3)){
		need_commit = 1;
		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", T("DHCP"));
	}
	if (strncmp(nat_en, old_nat, 2)) {
		nvram_bufset(RT2860_NVRAM, "natEnabled", nat_en);
		need_commit = 1;
	}

	// For 100PHY  ( Ethernet Convertor with one port only)
	// If this is one port only board(IC+ PHY) then redirect
	// the user browser to our alias ip address.
	if( getOnePortOnly() ){
		//     old mode is Gateway, and new mode is BRIDGE/WirelessISP/Apcli/3g
		if (    (!strcmp(old_mode, "1") && !strcmp(mode, "0"))  ||
				(!strcmp(old_mode, "1")&& !strcmp(mode, "2"))  ||
				(!strcmp(old_mode, "1")&& !strcmp(mode, "3"))  ){
			char redirect_url[512];
			char *lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");

			if(! strlen(lan_ip))
				lan_ip = "10.10.10.254";
			snprintf(redirect_url, 512, "http://%s", lan_ip);
			redirect_wholepage(wp, redirect_url);
			goto final;
		}

		//     old mode is BRIDGE/WirelessISP/Apcli, and new mode is Gateway
		if (    (!strcmp(old_mode, "0") && (!strcmp(mode, "1")||!strcmp(mode, "4")))  ||
				(!strcmp(old_mode, "2") && (!strcmp(mode, "1")||!strcmp(mode, "4")))  ||
				(!strcmp(old_mode, "3") && (!strcmp(mode, "1")||!strcmp(mode, "4")))  ){
			redirect_wholepage(wp, "http://172.32.1.254");
			goto final;
		}
	}

#if defined INIC_SUPPORT || defined INICv2_SUPPORT
	mii = websGetVar(wp, T("miiMode"), T("0"));
	if (!strncmp(mode, "0", 2)) {
		char *old_mii = nvram_bufget(RTINIC_NVRAM, "InicMiiEnable");

		if (strncmp(mii, old_mii, 2)) {
			nvram_set(RTINIC_NVRAM, "InicMiiEnable", mii);
			need_commit = 1; //force to run initInternet
		}
	}
	else {
		nvram_set(RTINIC_NVRAM, "InicMiiEnable", "0");
		need_commit = 1; //force to run initInternet
	}
#endif

	websRedirect(wp, "OpMode.asp");

final:
	sleep(1);	// wait for websDone() to finish tcp http session(close socket)

	//restart internet if any changes
	if (need_commit) {
		nvram_commit(RT2860_NVRAM);
		updateFlash8021x(RT2860_NVRAM);
		initInternet();
	}
}
#endif
#if 0
/***********************************************************************
 * 从配置文件中读取页面菜单要显示相关变量
 * bhou
 ***********************************************************************/
static int getWebMenuConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	char ret[64];
	int len = 0;
	FILE *fp;
	fp=fopen("/etc_ro/web/webMenuConfig","r");/*菜单栏配置文件*/
	if(fp != NULL)
	{
		while(!feof(fp))
		{
			memset(ret, 0, sizeof(ret));
			fgets(ret,sizeof(ret),fp);/*读取一行内容*/

			strRemoveChar(ret, ' ');/*去除页面显示不必要的空格*/
			if(*ret != '#')/*非注释行*/
			{
				len = strlen(ret);
				if(ret[len - 1] == '\n')
				{
					ret[len - 1] = '\0';/*去除末尾的换行*/
				}

				/*合法行*/
				if(strlen(ret) > 0)
				{
					if(*ret!='/0')/*需要显示的菜单*/
					{
						websWrite(wp, "var %s;", ret);
					}
				}
			}
		}
		fclose(fp);
	}
	return 0;

}
#endif
#if (MULTI_LAN == FYES)
/********************************************************************
 * 函数： isIpInLanNet
 * 功能： 多LAN口判断操作ip是否在lan口网段
 * 创建： 2012-09-07
 * 参数： 略
 * 返回： 
 *	    在lan口网段    1
 *	    不在lan口网段  0
 *	    查询失败      -1
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int isIpInLanNet(char *ip)
{
	MibProfileType mibTypeIf = MIB_PROF_INTERFACE;
	InterfaceProfile *profIf = NULL;
	struct in_addr addr;
	int flag = -1;
	profIf = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIf, 0);
	if ((profIf != NULL) || (profIf->head.active == 1)) 
	{/*判断lan口profile是否正常*/
		if(inet_aton(ip, &addr) != 0)
		{
			if(((ntohl(profIf->ether.sta.ip) & ntohl(profIf->ether.sta.nm)) == (ntohl(addr.s_addr) & ntohl(profIf->ether.sta.nm)))
					|| ((ntohl(profIf->ether.sta.ip2) & ntohl(profIf->ether.sta.nm2)) == (ntohl(addr.s_addr) & ntohl(profIf->ether.sta.nm2)))
					|| ((ntohl(profIf->ether.sta.ip3) & ntohl(profIf->ether.sta.nm3)) == (ntohl(addr.s_addr) & ntohl(profIf->ether.sta.nm3)))
					|| ((ntohl(profIf->ether.sta.ip4) & ntohl(profIf->ether.sta.nm4)) == (ntohl(addr.s_addr) & ntohl(profIf->ether.sta.nm4))))
			{/*在4个lan口网段中任意一个*/
				flag = 1;
			}
			else
			{/*不在*/
				flag = 0;
			}
		}
	}
	return flag;
}
#endif
#if (UTT_SMART_UI == FYES)
/*设置保存成功时向页面输出的信息，字符串用于页面判断*/
void setSuccessTips()
{
	if (webErrorIsOk){
		setTheRightMsg("saveok");
	}
}
#endif
