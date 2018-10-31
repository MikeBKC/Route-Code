#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"          
#include "mib.h"
#include "ifmisc.h"
#include "syslog.h"
#include <sys/sysinfo.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/utsname.h>
#include <netdb.h> 

#if((SYSLOG == FYES) && (SYSLOG_HEARTBEAT == FYES))

#define PROC_IF_STATISTIC	"/proc/net/dev"
#define PROC_MEM_STATISTIC	"/proc/meminfo"
#define SERVER_IP           "192.168.1.111"
#define SERVER_PORT          514


#define BEATHEART_DEBUG 0
#if BEATHEART_DEBUG 
#define BEATHEART_DEBUG(fmt, args...)		printf(fmt, ## args)
#else
#define BEATHEART_DEBUG(fmt, args...)
#endif

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3
#define HAVEINTERFACE        4
#define MAX_SERIAL 65535

static int sockfd = 0;
static IpFlag = 1;
static struct sockaddr_in server_addr;
static LogHelloMsg hmsg;
static serial = 0;

/*cpu利用率*/
static inline char *skip_token(const char *p)
{
	while (isspace(*p))
		p++;
	while (*p && !isspace(*p))
		p++;
	return (char *) p;
}

long percentages(int cnt, int *out, register long *new,
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
/********************************************************************
 * 函数： readCpu
 * 功能： 获取CPU利用率
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
#define NCPUSTATES 5
static long cp_time[NCPUSTATES];
static long cp_old[NCPUSTATES];
static long cp_diff[NCPUSTATES];
static int cpu_states[NCPUSTATES];
static int readCpu()
{
	int fd, len;
	char *p;
	int i;
	char buffer[1024 + 1];

	for (i = 0; i < 4; i++) {
		cpu_states[i] = 0;
		cp_diff[i] = 0;
	}
    fd = open("/proc/stat", "r");
	len = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	buffer[len] = '\0';

	p = skip_token(buffer);	/* "cpu" */
	cp_time[0] = strtoul(p, &p, 0);
	cp_time[1] = strtoul(p, &p, 0);
	cp_time[2] = strtoul(p, &p, 0);
	cp_time[3] = strtoul(p, &p, 0);
	cp_time[4] = strtoul(p, &p, 0);
	percentages(NCPUSTATES, cpu_states, cp_time, cp_old, cp_diff);

	BEATHEART_DEBUG("cat /proc/stat \n");
    BEATHEART_DEBUG("Cpu利用率: %u\n",(int)cpu_states[0] / 10.0);

	return (int) (cpu_states[0] / 10);
}
/********************************************************************
 * 函数： readUptime
 * 功能： 系统运行时间
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int readUptime()
{   
	struct sysinfo info;
	sysinfo(&info);
	return (int)info.uptime;
}

char * strip_space(char *str)
{
    if(str != NULL)
    {
    while((*str ==' ') || (*str=='  '))
        str++;
    }
    return str;
}
/********************************************************************
 * 函数： readMem
 * 功能： 内存利用率
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int readMem()
{
	char buf[1024], *semiColon, *key, *value;
	int total = 1, free = 1, memStat;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if (!fp) {
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

    BEATHEART_DEBUG("内存利用率 ：%d  \t cat /proc/meminfo \n",((memStat < 85) ? ((int)(memStat*(((double)8)/17))) : (40+(memStat-85)*4)));
	
    return ((memStat < 85) ? ((int)(memStat*(((double)8)/17))) : (40+(memStat-85)*4));
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
/********************************************************************
 * 函数： getIfStatistic
 * 功能： 通过接口名称获取接口流量
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
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
/********************************************************************
 * 函数： readNatSessions
 * 功能： 获取nat会话数
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int readNatSessions()
{
    FILE *fp = NULL;
    int natsessions = 0;
    char buf[100] = {0};
#if 0
    fp = popen("cat /proc/net/nf_conntrack|wc -l", "r");
#else
    fp = popen("cat /proc/sys/net/ipv4/netfilter/ip_conntrack_count", "r");
#endif
    if(fp != NULL)
    {
        fgets(buf, 64, fp);
        natsessions = (Uint32)strtoul(buf, NULL, 10);
        
        BEATHEART_DEBUG("NAT会话数 ：%u \t cat /proc/net/nf_conntrack|wc -l\n",natsessions);
        pclose(fp);
    }
    return natsessions;
}
/********************************************************************
 * 函数： readNatUsers
 * 功能： 获取nat用户数
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
/*#define USER_MAC_PROC_FILE "/proc/net/statsPerIp"*/
/*#define USER_MAC_PROC_FILE "cat /proc/net/statsPerIp|wc -l"*/
static int readNatUsers()
{
    FILE *fp = NULL;
    int natUsers = 0;
    char buf[100] = {0};

    fp = popen("cat /proc/net/statsPerIp|wc -l", "r");
    if(fp != NULL)
    {
        fgets(buf, 64, fp);
        natUsers = (Uint32)strtoul(buf, NULL, 10);
        if(natUsers > 0)
        natUsers -= 1;
        
        BEATHEART_DEBUG("nat用户数 : %d \t cat /proc/net/statsPerIp\n",natUsers);
        pclose(fp);
    }
    return natUsers;
}
/********************************************************************
 * 函数： readArpSessions
 * 功能： 获取arp会话数
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int readArpSessions()
{
    FILE *fp = NULL;
    int arpsessions = 0;
    char buf[100] = {0};

    fp = popen("cat /proc/net/arp|wc -l", "r");
    if(fp != NULL)
    {
        fgets(buf, 64, fp);
        arpsessions = (Uint32)strtoul(buf, NULL, 10);
        arpsessions -= 1;
        
        BEATHEART_DEBUG("ARP会话数 :%u \t cat /proc/net/arp|wc -l\n",arpsessions);
        pclose(fp);
    }
    return arpsessions;
}
/**
 * 统计字符串中指定字符的个数
 * bhou
 */
extern int strCountChar(char* str, char ch)
{
    int i = 0, retN = 0, len = 0;
    len = strlen(str);
    for(i = 0; i< len; i++)
    {
	if(str[i] == ch)
	{
	    retN++;
	}
    }
    return retN;
}
/**
 * 功能： 从linux系统arp表的一个条目中解析出来ip地址以及mac地址
 * 参数:  arpEntry:linux arp表的一行（该值在解析的过程中会被改变）
 *        ipStr   :得到的ip地址
 *        macStr  :得到的mac地址
 * 返回： 1解析正确 0非法条目
 * 作者:  bhou
 * 时间:  2011-11-01
 */
extern bool getIpMacFromSysArpEntry(char*arpEntry, char*ipStr ,char* macStr, char*ifName)
{
    char *ptr, *tmptok1, *split=" ";
    int ct=0;
    unsigned long flags = 0u;
    bool ret = 0, breakTag = 0;

    if (arpEntry != NULL)/*读取成功*/
    {
	tmptok1 = strtok_r(arpEntry,split,&ptr);
	while(tmptok1 != NULL)
	{
	    ct++;
	    if(ct == 1)/*第一项对应ipStr地址*/
	    {
		if(ipStr!=NULL)
		{
		    strcpy(ipStr,tmptok1);
		    /*系统arp条目一般不会出错，但需考虑arp表头等信息*/
		    if(strCountChar(ipStr, '.') < 3)
		    {/*非法ip地址*/
			breakTag = 1;
		    }
		}
	    }
	    else if(ct == 3)
	    {/*第3项为flags*/
		flags = strtoul(tmptok1,NULL,16);
		if(flags == 0u)
		{/*证明该arp缓存暂时过期*/
		    breakTag = 1;
		}
	    }
	    else if(ct==4) 
	    {/*第四项为对应的mac地址*/
		if(macStr!=NULL)
		{
		    strcpy(macStr,tmptok1);
		}
	    }
	    else if(ct == 6)
	    {/*第六项为接口*/
		strRemoveChar(tmptok1, '\n');/*移除多余的换行*/
#if (DHCP_POOL == FYES)
		ret = 1;
		breakTag = 1;
#else
		if(strcasecmp(tmptok1, ifName)==0)
		{
		    ret = 1;
		    breakTag = 1;
		}
#endif
	    }
	    if(breakTag != 0)
	    {
		break;/*一个循环仅一个break。LDRA要求*/
	    }
	    tmptok1 = strtok_r(NULL, split,&ptr);
	}
    }

    return  ret;
}
/********************************************************************
 * 函数： readMacUsers
 * 功能： 获取Mac用户数
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
#define ARP_VALID_MIN_LEN 19 /*合法arp条目的最小长度*/
static int readMacUsers()
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    int i = 0;
    FILE *fp;
    char *scannetstr = NULL;
    char buf[96], ipstr[32], macstr[32], tmpMacStr[32];
    MacAddr macAddr;
    struct in_addr addrS;
    IPADDR arpScanNet = 0u;
    IPADDR netMask = convertLenToNetMask(24u);/*转化为二进制ip地址形式掩码*/

    memset(ipstr, 0, sizeof(ipstr));
    memset(macstr, 0, sizeof(macstr));
    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    memset(&tmpMacStr, 0, sizeof(tmpMacStr));

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);

    if ((profIF != NULL) &&  ProfInstIsFree(profIF) == 0) {
        arpScanNet = profIF->ether.sta.ip;
    }

    fp=fopen("/proc/net/arp","r");/*读取系统arp表*/
    while(!feof(fp))
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), fp);/*读取一条arp条目*/
	    if(strlen(buf) >= ARP_VALID_MIN_LEN)/*合法arp条目*/
        {
            /*从arp条目中解析出来ip以及mac地址*/
            if(getIpMacFromSysArpEntry(buf, ipstr, macstr, getLanIfName())!=0)
            {
                if(inet_aton(ipstr, &addrS)!=0)/*转换为二进制ip地址*/
                {
                    if((addrS.s_addr & netMask) == (arpScanNet & netMask))/*与扫描ip同网段*/
                    {
                        i++;
                    }
                }
            }
        }
    }
    BEATHEART_DEBUG("MAC用户数 ：%d \t cat /proc/net/arp\n",i);
	fclose(fp);/*关闭文件流*/
    return i;
}
/********************************************************************
 * 函数： _getIfRxTxDataCounter
 * 功能： 获取接口上行下行数据
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void _getIfRxTxDataCounter(char *ifname, Uint32 *rxByte,
				  Uint32 *txByte, Uint32 *rxPacket,
				  Uint32 *txPacket)
{

	if (NULL != ifname) {
		long long data = 0;

		data = getIfStatistic(ifname, RXBYTE);
		*rxByte = htonl(data);

		data = getIfStatistic(ifname, TXBYTE);
		*txByte = htonl(data);

		data = getIfStatistic(ifname, RXPACKET);
		*rxPacket = htonl(data);

		data = getIfStatistic(ifname, TXPACKET);
		*txPacket = htonl(data);

	} else {
		*rxByte = 0;
		*txByte = 0;
		*rxPacket = 0;
		*txPacket = 0;
	}

	return;
}
/********************************************************************
 * 函数： fillRxTx
 * 功能： 获取上行下行数据
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void fillRxTx(Uint32  *rxPkt,Uint32 *rxByte,Uint32 *txPkt,Uint32 *txByte)
{
	int i = 0;
    int size = getWanSlotCount();

    if(size > MAX_UP_DOWN_DATA_LEN)
        size = MAX_UP_DOWN_DATA_LEN;

	_getIfRxTxDataCounter(getLanIfName(), rxByte, txByte, rxPkt, txPkt);

	for (i = 1; i <= size; i++) 
        _getIfRxTxDataCounter(getWanIfNamePPP(i), rxByte + i, txByte + i, rxPkt + i, txPkt + i);

}
/********************************************************************
 * 函数： getSystemInfoSysVersion
 * 功能： 获取软件名称
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static char* getSystemInfoSysVersion()
{   
    char *tmp = NULL;
    struct utsname utsbuf;
    static char buffer[50] = {0};
    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");
    tmp = strtok(NULL, " ");
	sprintf(buffer,"%s%s", utsbuf.version, tmp);
    tmp = strtok(NULL, " ");
    return buffer;
}
/********************************************************************
 * 函数： getSystemInfoMachineNo
 * 功能： 获取软件名称
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static char* getSystemInfoMachineNo()
{   
    char *tmp = NULL;
    struct utsname utsbuf;
    static char buffer[50] = {0};

    uname(&utsbuf);
    tmp = strtok(utsbuf.machine, " ");
    tmp = strtok(NULL, " ");

	if(tmp != NULL)
	{
        sprintf(buffer,"%s", tmp);
	}
    else
    {
        sprintf(buffer,"%s",utsbuf.machine);
    }
    return buffer;
}
/********************************************************************
 * 函数： buildLogHelloMsg
 * 功能： 封装数据
 * 创建： 2014-08-01
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int buildLogHelloMsg(LogHelloMsg *msg,Uint8 *extends,int interval)
{

    msg->feedstart = htonl(0xdeadbeef);
    msg->version   = htons(0x0002);
    msg->msgType   = htons(TYPE_HEARTBEAT);
    if(serial > MAX_SERIAL)
        serial = serial/MAX_SERIAL;
    msg->msgSn = htonl(serial++);

    /*时间间隔*/
    msg->msgInterval = htonl(interval);

    msg->mbid = htonl(getSystemSn());

    /*ReOS 版本号*/
    strcpy(msg->swVer,getSystemInfoMachineNo());

    /*ReOS 软件包名称*/
    BEATHEART_DEBUG("software : %s\n",getSystemInfoSysVersion());
    strcpy(msg->swName,getSystemInfoSysVersion());

    /*系统运行时间*/
    msg->uptime = htonl(readUptime());

    /*CPU利用率*/
    msg->cpu    = readCpu(); 

    /*内存利用率*/
    msg->mem    = readMem();

    /*当前nat会话数*/
    msg->natSessions = htonl(readNatSessions());

    /*当前nat用户数*/
    msg->natUsers = htonl(readNatUsers());

    /*当前arp会话数*/
    msg->arpSessions = htonl(readArpSessions());

    /*当前Mac用户数*/
    msg->macUsers = htonl(readMacUsers());

    BEATHEART_DEBUG("uptime:%u;mbid : %d; cpu:%d; mem:%d; arpSessions:%d; natSessions:%d; natUsers: %d; macUsers : %d\n",\
            msg->uptime,msg->mbid,msg->cpu,msg->mem,msg->arpSessions,msg->natSessions,msg->natUsers,msg->macUsers);

    /*有效的以太网接口*/
    msg->etherNums = htonl(getWanSlotCount() + 1);

    fillRxTx(msg->rxPkt,msg->rxByte,msg->txPkt,msg->txByte);

    msg->feedend = htonl(0xfeedface);

    msg->length  = htons(sizeof(LogHelloMsg) + sizeof(msg->optdata));

    return msg->length; 
}

/********************************************************************
 * 函数： IsValidIp
 * 功能： 发送UDP数据包给服务器
 * 创建： 2014-08-01
 * 参数： 数据，数据长度
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
int IsValidIp(char* ip)
{
    char c;
    int     dotTotal = 0;
    char *tmp = NULL;
    int  flag = 1;
    if((ip != NULL) && (strlen(ip) <= 15u) && (strlen(ip) != 0u))
    {   
        tmp = ip;
        c = *tmp;
        tmp = tmp + 1;
        while( (unsigned int)c != 0u )
        {   
            if((((unsigned int)c > 57u) || ((unsigned int)c < 48u)) && (c != '.'))
            {/*如果不是数字或者点分，则认为不合法*/
                flag = 0;
                break;
            }   
            /*点分作为判定IP的部分依据*/
            if(c == '.')
            {   
                dotTotal++;/*点分的个数增加*/
            }   
            c = *tmp;/*c保存当前指针的字符*/
            tmp = tmp + 1;/*指针向前移动一位*/
        }   
        if(dotTotal != 3) 
        {/*认为该字符串不合法*/
            flag = 0;
        }   
    }else{/*字符串为空，或者长度不合法*/
        flag = 0;
    }   
    return flag ;/*返回判断结果*/
}
/********************************************************************
 * 函数： initSocket
 * 功能： 发送UDP数据包给服务器
 * 创建： 2014-08-01
 * 参数： 数据，数据长度
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static bool initSocket(char* sip,Uint32 dport){
        int server_ip;

        memset(&server_addr,0,sizeof(server_addr));
        server_addr.sin_family=AF_INET;
        server_addr.sin_port=htons(dport);

        if(IsValidIp(sip))
        {
            if(inet_pton(AF_INET,sip,(void*)&server_ip) < 0)
                return FALSE;
            server_addr.sin_addr.s_addr=server_ip;
            IpFlag = 1; 
        }else{
            IpFlag = 0; 
        }

        sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(sockfd<0){
            BEATHEART_DEBUG("error: socket failed.");
            return FALSE;
        }
        return TRUE;
}
/********************************************************************
 * 函数： send2SyslogServer
 * 功能： 发送UDP数据包给服务器
 * 创建： 2014-08-01
 * 参数： 数据，数据长度
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static int send2SyslogServer(char *data,int len,char* sip,Uint32 dport){

    struct hostent *hp;

    if(data == NULL || sip == NULL || dport == 0 || strlen(sip)== 0)
    {
        return 0;
    }

    if(IpFlag == 0)
    {
        hp = gethostbyname(sip);
        if(hp == NULL) {
            return 0;
        }
        memcpy((char*)&server_addr.sin_addr,(char*)hp->h_addr,hp->h_length);
    }

    if(sendto(sockfd,data,len,0,(struct sockaddr*)&server_addr,sizeof(struct sockaddr))<0){
        BEATHEART_DEBUG("error: sendto error.");
        return 0;
    }
    return 1;
}
/********************************************************************
 * 函数： syslog_heartbeat
 * 功能： 每隔一定时间向syslog服务器发送心跳数据包
 * 创建： 2014-08-01
 * 参数： interval:时间间隔;  
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/ 
static void syslog_heartbeat(char* sip,Uint32 dport,int interval)
{
    memset(&hmsg,0,sizeof(hmsg));
    /*调用buildLogHelloMsg()构造数据包*/
    buildLogHelloMsg(&hmsg,NULL,interval);
    /*发送UDP包给服务器*/
    send2SyslogServer((char*)&hmsg,sizeof(LogHelloMsg),sip,dport);
}


void usage(void)
{
    printf("usage: \n");
    printf("\t%s $ServerIp $ServerPort $ServerInterval &\n","beatheart");
}

int main(int argc, char** argv)
{
    int interval = 0;
    int dport    = 0;

    if(argc >= 4) {
        interval = (Uint32)strtoul(argv[3], NULL, 10);
        dport    = (Uint32)strtoul(argv[2], NULL, 10);
        if(interval == 0 || dport == 0){
            fprintf(stderr,"beatheart : bad params.\n");
            exit(1);
        }
        ProfInit();
        if(initSocket(argv[1],dport) == FALSE)
        {
            fprintf(stderr,"beatheart : initSocket error.\n");
            exit(1);
        }

        while(1){
            syslog_heartbeat(argv[1],dport,interval);
            sleep(interval);
        }
        close(sockfd);
        ProfDetach();
    } else {
        usage();
    }
    exit(0);
}
#else
int main(int argc, char** argv)
{
    exit(0);
}
#endif
