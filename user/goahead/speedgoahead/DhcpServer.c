
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "internet.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"
#if (DHCP_SERVER == FYES)
#if (MULTI_SSID_DHCP_POOL == FYES)
#include <linux/netfilter_ipv4/utt_netfilter.h>
#endif

extern void init_dnsmasq(int wanNo);

#define MIN_ACTIVE_TIME 1356998400u
#if (DHCP_AUTO_BIND == FYES)
#define UTT_AUTODEL_INTERVAL_INIT 1 /* 自动删除函数调用间隔初始1分钟*/
static int BackupInterval = 0;
static char BackupDel[50];
#define SWORD_DEBUG  0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#endif

extern char* converMac6To17Byte(MacAddr macAddr);

#if (DHCP_AUTO_BIND == FYES)
/********************************************************************
 * 函数： AutoDelCheck
 * 功能： 设置多长时间进行一次自动删除的计划任务
 * 创建： 2012-09-19
 * 参数： interval 检测间隔
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int AutoDelCheck(int interval)
{
    int flag = 1;
    char Autodel[50];
    memset(Autodel, 0, sizeof(Autodel));
    if(BackupInterval != interval)
    {
	if(interval < 1440)
	{
	    if(interval < 60)
	    {
		sprintf(Autodel,"*/%d * * * * killall -q -USR1 udhcpd", interval);
	    }
	    else
	    {
		if(interval % 60 == 0)
		{
		    sprintf(Autodel,"* */%d * * * killall -q -USR1 udhcpd", interval/60);
		}
		else
		{
		    flag = 0;
		    printf("大于60分钟的时间必须是60的倍数\n");
		}
	    }
	}
	else if(interval == 1440)
	{
	    strcpy(Autodel,"00 02 * * * killall -q -USR1 udhcpd");
	}
	else
	{
	    flag = 0;
	    printf("最长设置一天一次\n");
	}
	doSystem("cron-init.sh del \"%s\";cron-init.sh add \"%s\"", BackupDel, Autodel);
	SWORD_PRINTF("cron-init.sh del \"%s\";cron-init.sh add \"%s\"\n", BackupDel, Autodel);
	BackupInterval = interval;
	strcpy(BackupDel, Autodel);
    }
    return flag;
}
/********************************************************************
 * 函数： DhcpAutoBind
 * 功能： 当页面自动绑定开关由关闭到开始时绑定所有已分配ip
 * 创建： 2012-09-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void DhcpAutoBind(void)
{
    MibProfileType mibType = MIB_PROF_USER;
    char *userName = NULL;
    char *DhcpNameH = "dhcp_"; /*dhcp自动绑定默认用户名前缀*/
    int i = 0;
    int maxDeNameNo = 0;
    FILE *fp;
    char macStr[13];
    MacAddr macAddr;
    struct DhcpAutoBindProf lease;

    memset(&macAddr, 0, sizeof(macAddr));

    doSystem("killall -q -USR1 udhcpd");

    fp = fopen("/var/udhcpd.leases", "r");
    if (fp != NULL)
    {
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) 
	{
	    SWORD_PRINTF("%s------%d, ntohl(lease.ip) = %u\n", __func__, __LINE__, ntohl(lease.expires));
	    if(ntohl(lease.expires) > 60U)
	    {
		memset(macStr, 0, sizeof(macStr));
		for (i = 0; i < 6; i++)
		{
		    sprintf(macStr,"%s%02X",macStr, lease.mac[i]);
		}
		if(maxDeNameNo == 0)
		{
		    maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, DhcpNameH);/*获取最大默认用户名编号*/
		}
		userName = ProfBuildDefaultName(DhcpNameH, maxDeNameNo);/*生成默认用户名*/
		SWORD_PRINTF("%s------%d, lease.ip = %u, macStr = %s, userName = %s\n", __func__, __LINE__, lease.ip, macStr, userName);
		converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
		if(DhcpArpBindConfig(lease.ip, macAddr, userName) == 1)
		{
		    maxDeNameNo++;/*若默认用户名生成，并被使用了，则最大默认用户名编号加一*/
		}
	    }
	}
	fclose(fp);
    }

    return;
}

#endif
#if (MULTI_SSID_DHCP_POOL == FYES)
extern int spdNfNlSock;
char *dhcpIp2Str(unsigned int ip, char *ipStr)
{
    struct in_addr addr;

    addr.s_addr = ip;
    strcpy(ipStr, inet_ntoa(addr));

    return ipStr;
}

static int uttNfNlDhcpMulti(int dhcp_pool_num)
{
    UttNlNfGeneral  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_MULTI_SSID_DHCP_POOL;
    nlConf.confType = dhcp_pool_num;

    printf("%s %d: nlType=%d, dhcp pool number: %d\n", 
	    __func__, __LINE__, nlConf.msgHead.nlType, nlConf.confType);
    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );

    return 0;
}

int dhcpServerConfFile(char *fileName, 
	unsigned int start, unsigned int end, 
	char *ifName,
	unsigned int mask, unsigned int gw, 
	unsigned int pd, unsigned int sd, 
	int lease)
{
    char ipStr[20];
    MibProfileType mibType = MIB_PROF_USER;
    int i, max, min;
    UserProfile *prof;
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL)
	return 0;

    fprintf(fp, "\n");
    fprintf(fp, "start %s\n", dhcpIp2Str(start, ipStr));
    fprintf(fp, "end %s\n", dhcpIp2Str(end, ipStr));
    if (ifName)
	fprintf(fp, "interface %s\n", ifName);
    else
	fprintf(fp, "interface br0\n");
    fprintf(fp, "option subnet %s\n", dhcpIp2Str(mask, ipStr));
    fprintf(fp, "option dns %s", dhcpIp2Str(pd, ipStr));
    fprintf(fp, " %s\n", dhcpIp2Str(sd, ipStr));
    fprintf(fp, "option router %s\n", dhcpIp2Str(gw, ipStr));
    fprintf(fp, "option lease %d\n", lease);
    if (ifName)
	fprintf(fp, "lease_file /var/udhcpd.%s.leases\n", ifName);
    else
	fprintf(fp, "lease_file /var/udhcpd.leases\n");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0)
	{
	    if (ntohl(prof->ip) >= ntohl(start) && ntohl(prof->ip) <= ntohl(end))
		fprintf(fp, "static_lease %s %s\n", 
			converMac6To17Byte(prof->mac), 
			dhcpIp2Str(prof->ip, ipStr));
	}
    }

    fclose(fp);
    return 1;
}
/*
 *	wireless_date_check	-	判断是否在无线生效时间并做处理
 *
 * 返回值：
 *	status	
 *	0			-	不在生效时段内
 *	1			-	在生效时段内
 *	-1			-	系统时间不对
 */


void dhcpServerInit(void)
{
	int status =0;
    char ipStr1[20], ipStr2[20];
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *prof = (DhcpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    MibProfileType mibTypeW = MIB_PROF_WIRELESS;
    WirelessProfile *profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW, 0);
#if (WIRELESS_5G == FYES) 
    WirelessProfile *profW_5g = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW, 1);
#endif
    unsigned int flag = 0U;
    
    doSystem("killall udhcpd &");
    doSystem("rm /etc/udhcpd.conf &");
    doSystem("rm /etc/udhcpd.ra0.conf &");
    doSystem("rm /etc/udhcpd.ra1.conf &");
#if (WIRELESS_5G == FYES) 
    doSystem("rm /etc/udhcpd.rai0.conf &");
    doSystem("rm /etc/udhcpd.rai1.conf &");
#endif
    doSystem("rm /var/udhcpd.leases &");
    doSystem("rm /var/udhcpd.ra0.leases &");
    doSystem("rm /var/udhcpd.ra1.leases &");
#if (WIRELESS_5G == FYES) 
    doSystem("rm /var/udhcpd.rai0.leases &");
    doSystem("rm /var/udhcpd.rai1.leases &");
#endif
    doSystem("ifconfig eth2.1 0.0.0.0 &");
    

    if((prof == NULL) || (ProfInstIsFree(prof)) || (prof->enable == FUN_DISABLE)) 
    {
	uttNfNlDhcpMulti(0);
	return;
    }
     status = wireless_date_check();
         if ((profW->head.active == 1) && (status != 0)) {
		doSystem("ifconfig ra0 0.0.0.0");
		doSystem("ifconfig ra1 0.0.0.0");
	/*ssid1 无线dhcp地址池开启*/
	if (prof->ssid1_dhcp_enable == PROF_ENABLE) {
	    if (dhcpServerConfFile("/etc/udhcpd.ra0.conf",
			prof->ssid1_ipStart, prof->ssid1_ipEnd,
			"ra0",
			prof->netmask, prof->gateway,
			prof->pDns, prof->sDns,
			prof->lease)) {
		doSystem("ifconfig ra0 169.254.225.200");
		doSystem("udhcpd /etc/udhcpd.ra0.conf");
		flag = 1U;
	    }
	}
	/*ssid2 无线dhcp地址池开启*/
	if (prof->ssid2_dhcp_enable == PROF_ENABLE) {
	    if (dhcpServerConfFile("/etc/udhcpd.ra1.conf",
			prof->ssid2_ipStart, prof->ssid2_ipEnd,
			"ra1",
			prof->netmask, prof->gateway,
			prof->pDns, prof->sDns,
			prof->lease)) {

		doSystem("ifconfig ra1 169.254.225.201");
		doSystem("udhcpd /etc/udhcpd.ra1.conf");
		flag = 1U;
	    }
	}
    }
#if (WIRELESS_5G == FYES) 
    if ((profW_5g->head.active == 1) && (status != 0)) {
		doSystem("ifconfig rai0 0.0.0.0");
		doSystem("ifconfig rai1 0.0.0.0");
	/*5G ssid1 无线dhcp地址池开启*/
	if (prof->ssid1_dhcp_enable_5g == PROF_ENABLE) {
	    if (dhcpServerConfFile("/etc/udhcpd.rai0.conf",
			prof->ssid1_ipStart_5g, prof->ssid1_ipEnd_5g,
			"rai0",
			prof->netmask, prof->gateway,
			prof->pDns, prof->sDns,
			prof->lease)) {

		doSystem("ifconfig rai0 169.254.225.202");
		doSystem("udhcpd /etc/udhcpd.rai0.conf");
		flag = 1U;
	    }
	}
	/*5G ssid2 无线dhcp地址池开启*/
	if (prof->ssid2_dhcp_enable_5g == PROF_ENABLE) {
	    if (dhcpServerConfFile("/etc/udhcpd.rai1.conf",
			prof->ssid2_ipStart_5g, prof->ssid2_ipEnd_5g,
			"rai1",
			prof->netmask, prof->gateway,
			prof->pDns, prof->sDns,
			prof->lease)) {

		doSystem("ifconfig rai1 169.254.225.203");
		doSystem("udhcpd /etc/udhcpd.rai1.conf");
		flag = 1U;
	    }
	}
    }
#endif
    if (flag == 1U) {
	uttNfNlDhcpMulti(1);
	if (dhcpServerConfFile("/etc/udhcpd.eth2.1.conf",
		    prof->ipStart, prof->ipEnd,
		    "eth2.1",
		    prof->netmask, prof->gateway,
		    prof->pDns, prof->sDns,
		    prof->lease)) {

	    doSystem("ifconfig eth2.1 169.254.225.204");
	    doSystem("udhcpd /etc/udhcpd.eth2.1.conf");
	}
    } else {
	uttNfNlDhcpMulti(0);
	if (dhcpServerConfFile("/etc/udhcpd.conf",
		    prof->ipStart, prof->ipEnd,
		    NULL,
		    prof->netmask, prof->gateway,
		    prof->pDns, prof->sDns,
		    prof->lease)) {

	    doSystem("udhcpd /etc/udhcpd.conf");
	}
    }
    init_dnsmasq(1);
    return;
}
#else
void dhcpServerInit()
{
    FILE *fp = NULL;
    int min = 0, max = 0, i;
    UserProfile *prof;
    struct in_addr tmpAd;
    MibProfileType mibType = MIB_PROF_USER;


    fp = fopen("/etc/udhcpd.tmp","w+");
    if(fp != NULL) {
        fseek(fp, 0, SEEK_END);   
        ProfInstLowHigh(mibType, &max, &min);
        for(i = min; i < max; i++) {
            prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
            if(prof != NULL && ProfInstIsFree(prof) == 0) {
                tmpAd.s_addr = prof->ip;
       	        fprintf(fp, "static_lease %s %s\n", converMac6To17Byte(prof->mac), inet_ntoa(tmpAd));
            }
        }
        fclose(fp);
    }
    init_dnsmasq(1);
    doSystem("dhcp.sh");
#if 0
        doSystem(" cat /etc/udhcpd.tmp >> /etc/udhcpd.conf");
        doSystem("udhcpd  /etc/udhcpd.conf");
        doSystem("rm -f /etc/udhcpd.tmp");
#endif
    return;

}
#endif

#if (DHCP_SERVER_OPTION_43 == FYES)
void dhcpServerOption43Init(const DhcpServerProfile *prof)
{
    if(prof->option43Type == DHCP_OPTION_43_TYPE_DISABLE)
        return 0;

    struct in_addr addr;
    int i;
    char *file = "/tmp/udhcpdoption43";
    FILE *fp;
    char l_in_tlv[3];//tlv字段中的l表示长度

    fp = fopen(file, "w");
    if (fp)
    {
	if (prof->option43Type == DHCP_OPTION_43_TYPE_HEX)
	{
	    inet_aton(prof->option43Addr, &addr);
#if defined(CONFIG_BOARD_P1010_C)
	    fprintf(fp,"%02x %02x %02x %02x %02x %02x %02x %02x", 0x2b, 0x06, 0x01, 0x04, 
		    (addr.s_addr>>24)&0xff, (addr.s_addr>>16)&0xff,
		    (addr.s_addr>>8)&0xff, (addr.s_addr)&0xff);
#else
	    fprintf(fp,"%02x%02x%02x%02x%02x%02x%02x%02x", 0x2b, 0x06, 0x01, 0x04, 
		    (addr.s_addr)&0xff, (addr.s_addr>>8)&0xff,
		    (addr.s_addr>>16)&0xff, (addr.s_addr>>24)&0xff);
#endif


	}
	else if (prof->option43Type == DHCP_OPTION_43_TYPE_ASCII)
	{
#if defined(CONFIG_BOARD_P1010_C)
	    fprintf(fp, "%02x %02x %02x %02x", 
#else
	    fprintf(fp, "%02x%02x%02x%02x", 
#endif
		    0x2b, strlen(prof->option43Addr)+2, 
		    0x03, strlen(prof->option43Addr)); 
	    for (i=0; i<strlen(prof->option43Addr); i++)
#if defined(CONFIG_BOARD_P1010_C)
            fprintf(fp, " %02x", prof->option43Addr[i]);
#else
            fprintf(fp, "%02x", prof->option43Addr[i]);
#endif
	}
	else if (prof->option43Type == DHCP_OPTION_43_TYPE_COSTUM)
	{
#if defined(CONFIG_BOARD_P1010_C)
        if(strlen(prof->option43Addr) % 2 == 0 && strlen(prof->option43Addr) >= 6){
            fprintf(fp, "%c%c", prof->option43Addr[0], prof->option43Addr[1]);
            for(i = 2; i < strlen(prof->option43Addr); i += 2) {
                fprintf(fp, " %c%c", prof->option43Addr[i], prof->option43Addr[i+1]); 
            }
            for(i = 0; i < strlen(prof->option43Addr2); i +=2) {
                fprintf(fp, " %c%c", prof->option43Addr2[i], prof->option43Addr2[i+1]);
            }
        }
#else

        //if ((prof->option43Addr[2]-'0')*16+(prof->option43Addr[3]-'0') == (strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2){
        memset(l_in_tlv, 0, 3);
        l_in_tlv[0] = prof->option43Addr[2];
        l_in_tlv[1] = prof->option43Addr[3];
        if((strtol(l_in_tlv, NULL, 16) == strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2)
        {   fprintf(fp, "%s%s", prof->option43Addr, prof->option43Addr2);

        }
	    else
            printf("%s %d: costum option 43 error, length %d not compare %d\n", __func__, __LINE__, (prof->option43Addr[2]-'0')*16+(prof->option43Addr[3]-'0'),
                    (strlen(prof->option43Addr)+strlen(prof->option43Addr2))/2-2);
#endif
	}
	
        fclose(fp);
    }
    else {
        printf("cannot open file %s\n", file);
    }
}
#endif

void dhcpBindProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const UserProfile* oldProfileDataPtr, const UserProfile* newProfileDataPtr )
{
    printf("%s %d...\n", __func__, __LINE__);
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:
            dhcpServerInit();
            break;
    };
}

extern  void dhcpServerProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const DhcpServerProfile *oldProfileDataPtr, const DhcpServerProfile *newProfileDataPtr )
{
    printf("%s %d...\n", __func__, __LINE__);
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
#if (DHCP_SERVER_OPTION_43 == FYES)
	dhcpServerOption43Init(newProfileDataPtr);
#endif
#if (DHCP_AUTO_BIND == FYES)
	if((oldProfileDataPtr->DhcpNewAutobind == FUN_DISABLE) && (newProfileDataPtr->DhcpNewAutobind == FUN_ENABLE)) {/*自动绑定由关闭变为开启*/
		DhcpAutoBind();
	} 
	if ((oldProfileDataPtr->DhcpDelAutobind == FUN_ENABLE) && (newProfileDataPtr->DhcpDelAutobind == FUN_DISABLE)) {/*自动删除由开启变为关闭*/
	    doSystem("cron-init.sh del \"%s\"", BackupDel);
	} else if ((oldProfileDataPtr->DhcpDelAutobind == FUN_DISABLE) && (newProfileDataPtr->DhcpDelAutobind == FUN_ENABLE)) {/*自动删除由关闭变为开启*/
	    doSystem("cron-init.sh add \"%s\"", BackupDel);
	} else {
	    /*null*/
	}
#endif
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:
            dhcpServerInit();
            break;
    };
    return;
}

void funInitDhcpServer()
{
#if (DHCP_AUTO_BIND == FYES)
    DhcpServerProfile *prof = NULL;
#endif
    registerForProfileUpdates(MIB_PROF_USER, NULL, (ProfileUpdateFnc)dhcpBindProfileChange);
    registerForProfileUpdates(MIB_PROF_DHCP_SERVER, NULL, (ProfileUpdateFnc)dhcpServerProfileChange);
#if (DHCP_AUTO_BIND == FYES)
    strcpy(BackupDel,"*/1 * * * * killall -q -USR1 udhcpd");
    prof = (DhcpServerProfile*)ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {/*profile使用*/
#if (DHCP_SERVER_OPTION_43 == FYES)
	dhcpServerOption43Init(prof);
#endif
	if (prof->DhcpDelAutobind == FUN_ENABLE) {/*自动删除开启*/
	    AutoDelCheck(UTT_AUTODEL_INTERVAL_INIT);
	}
    }
#endif
}
#endif
