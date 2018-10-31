
/*
 * added by brwang for speed goahead
 */

#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    "internet.h"
#include    "utt_conf_define.h"

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"

//#define SWORD_DEBUG
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

extern int headbits(unsigned int nm);
extern char* getWanIfName(int num);
/*
 * arguments: ifname - interface name
 *            if_net - a 16-byte buffer to store subnet mask
 * description: fetch subnet mask associated to given interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
int getIfNetmask(char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}
/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

#if 0
/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
    static char *if_name = "br0";
    return if_name;
}
#endif

void Conversion2StdMac(char* mac)
{   
    char *p;
    p=mac+16;
    mac=mac+11;
    do
    {
        *p--=*mac--;
        *p--=*mac--;
        *p=':';
    }while(--p!=mac);
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

#if 0
void restart8021XDaemon(int nvram)
{
	int i, num, apd_flag = 0;
	char *auth_mode = nvram_bufget(nvram, "AuthMode");
	char *ieee8021x = nvram_bufget(nvram, "IEEE8021X");
	char *num_s = nvram_bufget(nvram, "BssidNum");
	if(!num_s)
		return;
	num = atoi(num_s);

	if(nvram == RT2860_NVRAM)
		doSystem("killall rt2860apd 1>/dev/null 2>&1");
	else if(nvram == RTINIC_NVRAM)
		doSystem("killall rtinicapd 1>/dev/null 2>&1");
	else if(nvram == RT2561_NVRAM)
		doSystem("killall rt61apd 1>/dev/null 2>&1");
	
	/*
	 * In fact we only support mbssid[0] to use 802.1x radius settings.
	 */
	for(i=0; i<num; i++){
		char tmp_auth[128];
		if( getNthValueSafe(i, auth_mode, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "WPA") || !strcmp(tmp_auth, "WPA2") || !strcmp(tmp_auth, "WPA1WPA2")){
				apd_flag = 1;
				break;
			}
		}

		if( getNthValueSafe(i, ieee8021x, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "1")){
				apd_flag = 1;
				break;
			}
		}
	}

	if(apd_flag){
		if(nvram == RT2860_NVRAM)
			doSystem("rt2860apd");	
		else if(nvram == RTINIC_NVRAM)
			doSystem("rtinicapd");
		else if(nvram == RT2561_NVRAM)
			doSystem("rt61apd");
	}
}
#endif
#if (WIRELESS == FYES)
/* 
 * 目前只用支持RT2860 
 * 只支持一个ssid, 多ssid需要重写
 */
void restart8021XDaemon(void)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;

    prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);
    if(prof == NULL){
	return;
    }

    doSystem("killall -9 rt2860apd 1>/dev/null 2>&1");
	
    if ((strcmp(prof->mSafeCf.SelAuthMode, "WPA") == 0) 
	    || (strcmp(prof->mSafeCf.SelAuthMode, "WPA2") == 0) 
	    || (strcmp(prof->mSafeCf.SelAuthMode, "WPA1WPA2") == 0)
	    || (prof->mSafeCf.ap.AuthMode.IEEE8021X == 1)) {
	doSystem("rt2860apd");	
    }
}

#if (WIRELESS_5G == FYES)
void restart8021XDaemon_5g(void)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;

    prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 1);
    if(prof == NULL){
	return;
    }

    doSystem("killall rtinicapd 1>/dev/null 2>&1");
	
    if ((strcmp(prof->mSafeCf.SelAuthMode, "WPA") == 0) 
	    || (strcmp(prof->mSafeCf.SelAuthMode, "WPA2") == 0) 
	    || (strcmp(prof->mSafeCf.SelAuthMode, "WPA1WPA2") == 0)
	    || (prof->mSafeCf.ap.AuthMode.IEEE8021X == 1)) {
	doSystem("rtinicapd");
    }
}
#endif
#endif
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

char* int2ip(unsigned int lip,char * ip)
{
    unsigned char i=0;
    char *tmp=ip;
    while(i++<3)
    {
        sprintf(tmp,"%u.",(lip&0xff000000)>>24);
        lip=lip<<8;
        tmp+=strlen(tmp);
    }
    sprintf(tmp,"%u",(lip&0xff000000)>>24);
    return ip;
}

unsigned int ip2int(char *ip)
{
    char *tmp,ipbak[16]={0},*ptr;
    unsigned int ret=0;
    unsigned int value;
    strcpy(ipbak,ip);
    tmp=strtok_r(ipbak,".",&ptr);
    while(tmp!=NULL)
    {
        value=atoi(tmp);
        ret=ret<<8;
        ret|=value;
        tmp=strtok_r(NULL,".",&ptr);
    }
    return ret;
}

#if 0
unsigned getInNet(char *netarea ,char *end)
{
    char mask[16] ;
    unsigned int netarea_int = 0;
    unsigned int mask_int = 0;
    strcpy(mask ,nvram_bufget(RT2860_NVRAM, "lan_netmask"));
    strcpy(netarea ,nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
    netarea_int = ip2int(netarea);
    mask_int = ip2int(mask);
    if(!strcmp(end,"0"))
    {
	netarea_int = netarea_int & mask_int; 
	int2ip(netarea_int , netarea);
    }
    else
    {
	netarea_int = netarea_int | ~mask_int; 
	int2ip(netarea_int , netarea);
    }
    return netarea_int;
}
#endif

const char * day[]=
{
    "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat",
    "Sun"
};

void getDays(unsigned char day_bit,char days[])
{
    unsigned char i,j=0;
    for(i=0;i<7;i++)
    {
        if(day_bit&0x40)
        {
             strcat(days,day[i]);
	     *(days+(j<<2)+3)=',';
	     j++; 
        }
        day_bit<<=1;
    }
    *(days+(j<<2)-1)=0;
}

#if 0
/*
 * description: return WAN interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
char* getWanIfName(int num)
{
	static char *if_name;
    if_name = "";

    switch(num) {
        case 1:
		    if_name = "eth2.2";
            break;
        case 2:
		    if_name = "eth2.3";
            break;
        case 3:
		    if_name = "ttyUSB0";
            break;
        case 4:
		    if_name = "apcli0";
            break;
        default:
            break;
    }
    return if_name;
}
#endif

char pppName[MAX_PPP_NAME_LEN] = {0};
char* getDevicePPPIfName(char* dev)
{
    FILE *fp;
    char buf[MAX_FILE_NAME_LEN] = {0};
    char* tmp;

    snprintf(buf, MAX_FILE_NAME_LEN, DEV_PPP_IF_NAME, dev);

    fp = fopen(buf, "r");
    if(fp != NULL) {
        memset(buf, 0x00, sizeof(buf));
        fgets(buf, MAX_FILE_NAME_LEN, fp);
        fclose(fp);
	memset(pppName, 0x00, sizeof(pppName));
        sscanf(buf, "%s", pppName);

        tmp = pppName;
    } else {
        tmp = "";
    }
    return tmp;
}

int getOnePortOnly(void)
{
    return 0;
}

#if 0
/*
 * function for multi wan conf
 */
char * add_wanx(const char *name, int wanNo, char * nvram_name)
{
    snprintf(nvram_name, MAX_UTT_CONF_NAME_LEN, _CONF_PREFIX_WAN"%s", wanNo%10, name);
    return nvram_name;
}
#endif
#if (MULTI_LAN == FYES)
/********************************************************************
 * 函数： get_netarea
 * 功能： 多LAN口取得网段
 * 创建： 2012-09-04
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
char * get_netarea(char *lanIfName, char *netarea, char *natIp, int *num)
{
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    unsigned int iIp = 0U, iIp2 = 0U, iIp3 = 0U, iIp4 = 0U;
    unsigned int iNm = 0U, iNm2 = 0U, iNm3 = 0U, iNm4 = 0U;
    unsigned int iNatIp = 0U, iNatIp2 = 0U, iNatIp3 = 0U, iNatIp4 = 0U;


    /*
     * 获得lan口profile
     */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) || (profIF->head.active == 1)) 
    {/*判断lan口profile是否正常*/
	iIp = htonl(profIF->ether.sta.ip);/*取得lan口ip*/
	iIp2 = htonl(profIF->ether.sta.ip2);
	iIp3 = htonl(profIF->ether.sta.ip3);
	iIp4 = htonl(profIF->ether.sta.ip4);
	iNm = htonl(profIF->ether.sta.nm);
	iNm2 = htonl(profIF->ether.sta.nm2);
	iNm3 = htonl(profIF->ether.sta.nm3);
	iNm4 = htonl(profIF->ether.sta.nm4);
	SWORD_PRINTF("%s------%d,iIp = %x, iIp2 = %x, iIp3 = %x, iIp4 = %x\n", __func__, __LINE__, iIp, iIp2, iIp3, iIp4);
	SWORD_PRINTF("%s------%d,iNm = %x, iNm2 = %x, iNm3 = %x, iNm4 = %x\n", __func__, __LINE__, iNm, iNm2, iNm3, iNm4);
	iNatIp = ip2int(natIp);
	iIp &= iNm;
	iIp2 &= iNm;
	iIp3 &= iNm;
	iIp4 &= iNm;
	iNatIp &= iNm;
	iNatIp2 = iNatIp & iNm2;
	iNatIp3 = iNatIp & iNm3;
	iNatIp4 = iNatIp & iNm4;
	SWORD_PRINTF("%s------%d,iIp = %x, iIp2 = %x, iIp3 = %x, iIp4 = %x, iNatIp = %x, iNatIp2 = %x, iNatIp3 = %x, iNatIp4 = %x,\n", __func__, __LINE__, iIp, iIp2, iIp3, iIp4, iNatIp, iNatIp2, iNatIp3, iNatIp4);
	if(iNatIp == iIp)
	{/*nat网段和lan1网段*/
	    *num = 1;
	    int2ip(iIp, netarea);
	    sprintf(netarea+strlen(netarea), "/%d", headbits(iNm)%100);/*网段加掩码*/
	}
	else if(iNatIp2 == iIp2)
	{/*nat网段和lan2网段*/
	    *num = 2;
	    int2ip(iIp2, netarea);
	    sprintf(netarea+strlen(netarea), "/%d", headbits(iNm2)%100);/*网段加掩码*/
	}
	else if(iNatIp3 == iIp3)
	{/*nat网段和lan3网段*/
	    *num = 3;
	    int2ip(iIp3, netarea);
	    sprintf(netarea+strlen(netarea), "/%d", headbits(iNm3)%100);/*网段加掩码*/
	}
	else if(iNatIp4 == iIp4)
	{/*nat网段和lan4网段*/
	    *num = 4;
	    int2ip(iIp4, netarea);
	    sprintf(netarea+strlen(netarea), "/%d", headbits(iNm4)%100);/*网段加掩码*/
	}
	else
	{/*若都不在则返回lan1网段*/
	    *num = 1;
	    int2ip(iIp, netarea);
	    sprintf(netarea+strlen(netarea), "/%d", headbits(iNm)%100);/*网段加掩码*/
	}
    }

    SWORD_PRINTF("%s------%d, net area[%s], num = %d\n", __func__, __LINE__, netarea, *num);
    return netarea;
}
#else
char * get_netarea(char *lanIfName,char *netarea)
{
    char mask[16];
#if 0
    int i=0,num=0,num1=0,prefixNum;
#else
    int i=0,num=0,num1=0;
#endif
    unsigned int iIp=0U, iNm=0U;
    getIfIp(lanIfName,netarea);
    getIfNetmask(lanIfName,mask);
    DBGPRINT("ip addr[%s]\n", netarea);
    iIp=ip2int(netarea);
    iNm=ip2int(mask);
    iIp &= iNm;
    int2ip(iIp, netarea);
    DBGPRINT("net addr[%s]\n", netarea);

#if 1
    /* for support CIDR mode */
    sprintf(netarea+strlen(netarea), "/%d", headbits(ip2int(mask))%100);
    DBGPRINT("net area[%s]\n", netarea);
    return netarea;
#endif
    while(mask[i]!='\0')
    {
	if(mask[i]=='0')
	    num++;
	i++;
    }
    if(num==1)
    {
	i=0;
	while(netarea[i]!='\0')
	{
	    if(netarea[i]=='.')
		num1++;
	    i++;
	    if(num1==3)
		break;
	}
	netarea[i]='\0';
	strcat(netarea,"0/24");
	return netarea;
    }
    if(num==2)
    {
	i=0;
	num1=0;
	while(netarea[i]!='\0')
	{
	    if(netarea[i]=='.')
		num1++;
	    i++;
	    if(num1==2)
		break;
	}
	netarea[i]='\0';
	strcat(netarea,"0.0/16");
	return netarea;
    }
    if(num==3)
    {
	i=0;
	num1=0;
	while(netarea[i]!='\0')
	{
	    if(netarea[i]=='.')
		num1++;
	    i++;
	    if(num1==1)
		break;
	}
	netarea[i]='\0';
	strcat(netarea,"0.0.0/8");
	return netarea;
    }
    if(num==4)
    {
	netarea="0.0.0.0/0";
	return netarea;
    }
    if(num==0)
    {
	strcat(netarea,"/32");
	return netarea;
    }
    return NULL;
}
#endif

char * NextRecordStart(char*Record)
{
    while(*Record && *Record!=UTT_SPLIT_SIGN_RECORD) {
        Record++;
    }
    if(*Record)
        Record++;
    return Record;
}

#if 0
unsigned int getOutIp(char *ip)
{
    char * RuleList,*tmp,Type[8]={0};
    unsigned int nIp,nOutIp,nInFromIp,nInEndIp,i;
    RuleList = nvram_bufget(RT2860_NVRAM,"NatRuleList");
    tmp=RuleList;
    nIp=ip2int(ip);
    while(*tmp)
    {
        if(sscanf(tmp,"%*[^,],%[^-]-%*[^,],%x,%x,%x",Type,&nOutIp,&nInFromIp,&nInEndIp)==4)//name,
	{
            if((nIp>=nInFromIp) && (nIp<=nInEndIp))
            {
		i=0;
                if(!strcmp(Type,"2"))//one2one
		{
                   while( (nInFromIp+i)!=nIp )
		   {
		       i++;
		   }
		}
		return (nOutIp+i);
	    }
	}
	tmp=NextRecordStart(tmp);
    }
    return 0;
}
#endif

unsigned char days2Byte(char* day)
{
    unsigned char i=0,data=0;
    while(i<7)
    {
        data<<=1;
        if(*(day+i)!='0')
             data |=1;
	i++;
    }
    return data;
}

int getWanDhcpDefaultRoute(int wannum, char* ipaddr)
{
    FILE *fp;
    char buf[MAX_FILE_NAME_LEN];
    int returnval = 0;

    /*
     * set file name
     */
    sprintf(buf, DHCP_ROUTE_FILE, getWanIfName(wannum));

    fp = fopen(buf, "r");
    if(fp != NULL) {
        fgets(buf, 16, fp);
        fclose(fp);
        sscanf(buf, "%s", ipaddr);
        returnval = 0;
    } else {
        returnval = -1;
    }
    return returnval;
}
