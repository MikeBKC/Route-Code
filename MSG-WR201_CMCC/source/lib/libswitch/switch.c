/*
 *
 *��д�����ڴ���������
 *�����������ж���ʹ�ñ��ļ�
 *KeHuatao
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <net/if.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <arpa/inet.h> /*inet_addr*/
#include <net/route.h>
#include <linux/if_packet.h>
#include <linux/switch/switch.h>
#include "switch.h"
#include "urcp_ipc.h"
#include "packet.h"

#include "sw_ioctl.h"

#define IP_LEN  4

#define SWVER_1 8
#define SWVER_2 8
#define SWVER_3 8
#define HWVER_1 4
#define HWVER_2 4


extern int32 CWUrcpSend(char *buffer, int len);
//static int32 _fd=-1;
static int32 _socketL2 =-1;
static int32 _socketL3 =-1;
/*
 * ��mac��ַת���ɱ�׼��ʽ
 *  
 **/
const char *str_macaddr(const unsigned char *macaddr)
{
    static char buf[32];
    int i;
    char *cp;

    cp = buf;
    for (i = 0; i < 6; i++) {
	sprintf(cp, "%02X-", macaddr[i]);
	cp += 3;
    }
    cp--;
    *cp = '\0';
    return buf;
}

/*************************************************************************/
void print_ip_addr (unsigned int ip)
/* ------------------------------------------------------------------------
 * Purpose     : Print ip address in format xxx.xxx.xxx.xxx.
 * Remarks     : ip_addr points to a 4-byte array holding ip address.
 * Restrictions:
 * See also    :
 * Example     :
 **************************************************************************/
{
    struct in_addr ip_addr;  
    unsigned int nIp;

    nIp = htonl(ip);/*ip�����������ֽ�˳��*/
    memcpy(&ip_addr, &nIp, IP_LEN); 
    printf("%s", inet_ntoa(ip_addr));
}

int validate_ip_addr (unsigned int ip_addr)
/*------------------------------------------------------------------------ --
* Purpose     : 
* Remarks     :
* Restrictions:
* See also    :
* Example     :
************************************************************************* */
{
    unsigned int ip;


    /*ȡ�����ֽڲ���*/
    ip = ip_addr >> 24u;

    /*�ж��Ƿ�Ϸ�*/
    if ((ip == 0u) || (ip == 127u) || (ip >= 224u)) {
	/*����*/
	return -1;
    }

    return 0;
}


int validate_netmask (unsigned int mask)
/*------------------------------------------------------------------------ --
* Purpose     : �ж�������Ч��
* Remarks     :
* Restrictions:
* See also    :
* Example     :
************************************************************************* */
{
    unsigned int i, width;
        
    width = 0u;
    /*�����жϸ�λ*/
    for (i = 0u; i < 32u; i++) {
	if (mask & (unsigned int)(1u<<(31u-i))) {/*�Ƿ����1*/
	    if (i != width) {/*����*/
		return -1;
	    }
	    width++;/*λ����*/
	}
    }

    return 0;
}

/*************************************************************************/
int retrieve_ip_addr (unsigned char *ip_buffer, unsigned int *nIp)
/*------------------------------------------------------------------------ --
* Purpose     : Retrieve ip address from command string.
* Remarks     : Module variable cmd_ptr must have been set to point to first 
*               non-space char after the "config ip" command in the command string, 
*               when this function is called.
*               The retrieved ip address is returned in ip_addr.
*               Returns 0, if successful, otherwise <> 0.
* Restrictions:
* See also    :
* Example     :
**************************************************************************/
{
    unsigned int j, val, number_flag;
    unsigned char ch;

    /*4���ֽ�*/
    for (j = 0u; j < 4u; j++) {
	number_flag = 0u;
	val = 0u;

	/*ȡһ���ֽ�*/
	while (1) {
	    ch = *ip_buffer;
	    if (!(ch >= '0' && ch <= '9')) {/*����*/
		break;
	    }
	    number_flag = 1u;/*ȡ������*/
	    ip_buffer++;/*��һ��*/
	    val = (val * 10) + (ch - '0');/*��ֵ*/
	    if (val > 255u) {/*ֵ����*/
		return -1;
	    }
	}

	/*ȡ��ֵ*/
	*nIp = (*nIp) << 8u;
	*nIp |= val;

	if (j < 3u) {/*�����һ�ֽ�*/
	    ch = *ip_buffer++;
	    if (ch != '.') {/*�����ַ�*/
		return -1;
	    }
	}
    }

    /*�Ƿ���4�ֽ�*/
    if ((j != 4) || (number_flag == 0u)) {
	return -1;
    }

    return 0;
}

/*****************************************************************8
 * �������� �� set_ip(char *ip)
 * �������� �� ����ϵͳIP��ַ
 * ��    �� �� ip��ַ
 * �� �� ֵ �� 0 : �ɹ� ��  -1 :  ʧ�� 
 * ��    �� �� jfgu
 * ��    �� �� 2010-10-22
 ******************************************************************/ 
extern int set_ip(char *ifname, uint32 sysIp)
{
    struct ifreq temp;
    struct sockaddr_in *addr;
    int fd = 0;
    int ret = -1;

    memset(&temp, 0, sizeof(struct ifreq));
    strcpy(temp.ifr_name,ifname);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	return -1;
    }
    
    addr = (struct sockaddr_in *)&(temp.ifr_addr);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = sysIp;
    
    ret = ioctl(fd, SIOCSIFADDR, &temp);
    close(fd);

    if(ret < 0)
    {
	return -1;
    }

    return 0;

}

/*****************************************************************8
 * �������� �� set_mask(char *ip)
 * �������� �� ����ϵͳMask��ַ
 * ��    �� �� mask 4���ֽڱ�ʾ
 * �� �� ֵ �� 0 : �ɹ� ��  -1 :  ʧ�� 
 * ��    �� �� jfgu
 * ��    �� �� 2010-10-22
 ******************************************************************/ 
extern int set_mask(char *ifname, uint32 mask)
{
    struct ifreq temp;
    struct sockaddr_in *mask_addr = NULL;
    int fd = 0;
    int ret = -1;

    memset(&temp, 0, sizeof(struct ifreq));

    strcpy(temp.ifr_name,ifname);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	return -1;
    }
    
    mask_addr = (struct sockaddr_in *)&(temp.ifr_addr);
    mask_addr->sin_family = AF_INET;
    mask_addr->sin_addr.s_addr = mask;
    
    ret = ioctl(fd, SIOCSIFNETMASK, &temp);
    close(fd);

    if(ret < 0)
    {
	return -1;
    }
    return 0;
}
/*
 * set gw
 *
 * */
extern void set_gw(char *gwaddr)
{
    char cmd_str[128]={'\0'};
    system("route del default 2>/dev/null &");
    if( strcmp(gwaddr,"0.0.0.0") !=0 )
    {//Ϊȫ����ֻ���
	sprintf(cmd_str,"route add default gw %s &", gwaddr);
	system(cmd_str);
    }
}
#if 0
/******************************************************************
 * ������ ɾ���豸���ʿ�����Ӧiptables����
 * ������ ifname:�豸�ӿ�,ipfrom����ʼԴ��ַ,ip_end������Դ��ַ
 * ���أ� ��
 * ���ߣ� bhou
 * ���ڣ� 2010-12-14
 * ****************************************************************/
extern void clear_access_ctrl(char *ifname,char * ip_from,char *ip_end)
{
    char cmd_str[256];
    char ip_dest[32];
    memset(ip_dest,0,32);
    memset(cmd_str,0,256);
    int2ip(cfg_read_pre_sys_ip(),ip_dest);

    if( (strcmp(ip_from,"0.0.0.0")!=0) && (strcmp(ip_end,"0.0.0.0")!=0) &&(strcmp(ip_dest,"0.0.0.0")!=0) )
    {/*Ϊ0�Ĺ��򲻿��ܴ���*/
	sprintf(cmd_str,"iptables -t filter -D INPUT -i %s -p tcp --dport 80 -m iprange ! --src-range %s-%s -d %s -j DROP",ifname,ip_from,ip_end,ip_dest);/*ɾ����Ӧiptables  http����*/
	system(cmd_str);/*ɾ������*/
	sprintf(cmd_str,"iptables -t filter -D INPUT -i %s -p tcp --dport 23 -m iprange ! --src-range %s-%s -d %s -j DROP",ifname,ip_from,ip_end,ip_dest);/*ɾ����Ӧiptables  telnet����*/
	system(cmd_str);/*ɾ������*/
	sprintf(cmd_str,"iptables -t filter -D INPUT -i %s -p tcp --dport 161 -m iprange ! --src-range %s-%s -d %s -j DROP",ifname,ip_from,ip_end,ip_dest);/*ɾ����Ӧiptables  snmp����*/
	system(cmd_str);/*ɾ������*/
	cfg_write_pre_sys_ip(0u);

    }

}
/******************************************************************
 * ������ �����豸���ʿ�����Ӧiptables����
 * ������ ifname:�豸�ӿ�,ipfrom����ʼԴ��ַ,ip_end������Դ��ַ
 * ���أ� ��
 * ���ߣ� bhou
 * ���ڣ� 2010-12-14
 * ****************************************************************/
extern void add_access_ctrl(char *ifname,char * ip_from,char *ip_end)
{
    char cmd_str[256];
    char ip_dest[32];
    memset(ip_dest,0,32);
    memset(cmd_str,0,256);
    int2ip(cfg_read_sys_ip(),ip_dest);

    if( (strcmp(ip_from,"0.0.0.0")!=0) && (strcmp(ip_end,"0.0.0.0")!=0) &&(strcmp(ip_dest,"0.0.0.0")!=0) )
    {/*Ϊ0�Ĺ��򲻿��ܴ���*/
	sprintf(cmd_str,"iptables -t filter -A INPUT -i %s -p tcp --dport 80 -m iprange ! --src-range %s-%s -d %s -j DROP",ifname,ip_from,ip_end,ip_dest);/*����http����*/
	system(cmd_str);
	sprintf(cmd_str,"iptables -t filter -A INPUT -i %s -p tcp --dport 23 -m iprange ! --src-range %s-%s -d %s -j DROP",ifname,ip_from,ip_end,ip_dest);/*����telnet����*/
	system(cmd_str);
	sprintf(cmd_str,"iptables -t filter -A INPUT -i %s -p tcp --dport 161 -m iprange ! --src-range %s-%s -d %s -j DROP",ifname,ip_from,ip_end,ip_dest);/*����snmp����*/
	system(cmd_str);
	cfg_write_pre_sys_ip(cfg_read_sys_ip());

    }
}
#endif

/***********************************************************************
 * �� �� ����   get_mac
 * ����������	get interface mac addr
 * �������:    interface name,a 18-byte buffer to store ip address
 * ���������	mac addr
 * ����ֵ��     0,-1
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       ������
 * ����˵����	��
***********************************************************************/
extern int get_mac(char *ifname, unsigned char *if_hw)
{
	struct ifreq ifr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("getIfMac: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		printf("getIfMac: ioctl SIOCGIFHWADDR error for %s", ifname);
		return -1;
	}
	memcpy(if_hw, ifr.ifr_hwaddr.sa_data, MAC_LEN);
	close(skfd);
	return 0;
}

/***********************************************************************
 * �� �� ����   get_ip
 * ����������	get interface ip
 * �������:    interface name,a 16-byte buffer to store ip address
 * ���������	ip
 * ����ֵ��     0,-1
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       ������
 * ����˵����	��
***********************************************************************/
extern int get_ip(char *ifname, uint32 *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("getIfIp: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
#if 0
		printf("getIfIp: ioctl SIOCGIFADDR error for %s", ifname);
#endif
		return -1;
	}

	*if_addr = (uint32)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);

	close(skfd);
	return 0;
}

/***********************************************************************
 * �� �� ����   get_mask 
 * ����������	get interface  netmask
 * �������:    interface name,a 16-byte buffer to store subnet mask
 * ���������	netmask
 * ����ֵ��     0,-1
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       ������
 * ����˵����	��
***********************************************************************/
extern int get_mask(char *ifname, uint32 *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("getIfNetmask: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
#if 0
		printf("getIfNetmask: ioctl SIOCGIFNETMASK error for %s\n", ifname);
#endif
		return -1;
	}

	*if_net = (uint32)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
	close(skfd);
	return 0;
}

#if 0
/*
 *  Web Related Utilities      
 */    
extern int32 getSystemInfoSysVersion(char version[64])
{   
    static char software_rev[72]={0}; 
    static int  first=0u;

    if(first==0u)
    {
	FILE *fp = popen("uname -v", "r");
	if(!fp){
	    return -1;
	}
	fgets(software_rev, 64, fp);
	pclose(fp);
	strtok(software_rev,"\n");
	first=1u;
    }
    strncpy(version, software_rev, 64);

    return 0;
}

extern int32 getSystemInfoMachineNo(char product_id[64])
{
    static char ProductModel[72]={0}; 
    static int  first=0u;

    if(first==0u)
    { 
	FILE *fp = popen("uname -m", "r");
	if(!fp){
		return -1;
	}
	fgets(ProductModel, 64, fp);
	pclose(fp);
        strtok(ProductModel,"\n");
	first=1u;
    }
    strncpy(product_id, ProductModel, 64);

    return 0;
}
/******************************************************
 *������ hexStringToValue 
 *���ܣ� ��ʮ�������ַ���ת���ֽ�
 *������ �ַ��� 
 *����� ��
 *���أ� �ֽ�����ָ�� 
 *���ڣ� 2010-09-26
 *���ߣ� jfgu
 *******************************************************/
extern void hexStringToValue(unsigned char *dest, const char *src, int destLen)
{
    int         srcLen = 0;
    int         srcIx = 0;
    int         ix;
    int         nibble1;
    int         nibble2;

    if (src) 
    {
        srcLen = strlen(src);
    }
    for (ix = 0; ix < destLen; ++ix) 
    {
        if (srcIx < srcLen) 
	{
            if (src[srcIx] <= '9') 
	    {
                nibble1 = src[srcIx] & 0x0f;
            } 
	    else 
	    {
                nibble1 = (src[srcIx] & 0x0f) + 9;
            }

            srcIx += 1;
            
	    if (srcIx < srcLen) 
	    {
                if (src[srcIx] <= '9') 
		{
                    nibble2 = src[srcIx] & 0x0f;
                } 
		else 
		{
                    nibble2 = (src[srcIx] & 0x0f) + 9;
                }

                srcIx += 1;

            } 
	    else 
	    {
                nibble2 = 0;
            }

            ((unsigned char *)dest)[ix] = (unsigned char)(nibble1 << 4) + nibble2;

        } 
	else 
	{
            ((unsigned char *)dest)[ix] = 0;
        }
    }

    return;
}
extern int32 getSystemSn(void)    
{
    int32 fd;
    volatile uint32 *sn;   
    void * map_base;

    if((fd=open("/dev/mem",O_RDWR|O_SYNC))==-1){
	perror("open /dev/mem:");
	return (-1);
    }  
    map_base=mmap(0,0xff,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0xbf000000);
    sn = (volatile uint32 *) (map_base+0x2c);
    close(fd);
    return *sn;
}

extern int getSystemNowTime(char buf[64])
{
    char *tmp = NULL;

    FILE *fp = popen("date '+%s'", "r");
    
    if(!fp)
    {
	return 0;
    }

    fgets(buf, 64, fp);

    pclose(fp);

    tmp = strtok(buf,"\n");

    return 0;

}

extern void getSystemSysUpTime(char buf[64])
{
    
    int updays=0,uphours=0,upminutes=0,upsecs=0;
    struct sysinfo info;
    sysinfo(&info);
    updays = (int) info.uptime / (60*60*24);
    upminutes = (int) info.uptime / 60;
    uphours = (upminutes / 60) % 24;
    upminutes %= 60;
    upsecs =info.uptime % 60;
    
    
    sprintf(buf, "%d �� %d ʱ %d �� %d �� ", updays,uphours,upminutes,upsecs);
    
    return; 
}

#define NCPUSTATES 5
#define is_space(c) ((((c) == ' ') || (((unsigned int)((c) - 9)) <= (13 - 9))))
static long cp_time[NCPUSTATES];
static long cp_old[NCPUSTATES];
static long cp_diff[NCPUSTATES];
static int cpu_states[NCPUSTATES];

static inline char * skip_token(const char *p)
{
    while (is_space(*p)) p++;
    while (*p && !is_space(*p)) p++;
    return (char *)p;
}
static inline char *strip_space(char *str)
{
	while( *str == ' ')
		    str++;
	    return str;
}

long percentages(int cnt, int* out, register long *new,
        register long *old, long* diffs)
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
            change = (int) ((unsigned long)*new-(unsigned long)*old);
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
        *out++ = (int)((*diffs++ * 1000 + half_total) / total_change);
    }

    /* return the total in case the caller wants to use it */
    return(total_change);
}

extern void getSystemCpu(int *s_cpu)
{
    int fd,len;
    char *p;
    int i;
    char buffer[1024+1];

    for(i = 0; i < 4; i++){
        cpu_states[i] = 0;
        cp_diff[i] = 0;
    }

//    while(1){
        fd = open("/proc/stat", O_RDONLY);
        len = read(fd, buffer, sizeof(buffer)-1);
        close(fd);
        buffer[len] = '\0';
        
        p = skip_token(buffer); /* "cpu" */
        cp_time[0] = strtoul(p, &p, 0);
        cp_time[1] = strtoul(p, &p, 0);
        cp_time[2] = strtoul(p, &p, 0);
        cp_time[3] = strtoul(p, &p, 0);
        cp_time[4] = strtoul(p,&p,0);
        //DBGPRINT("f %ld %ld %ld %ld\n",cp_time[0],cp_time[1],cp_time[2],cp_time[3]);
        percentages(NCPUSTATES, cpu_states, cp_time, cp_old, cp_diff);
       
	*s_cpu = (int)(cpu_states[0]/10.0);

	return;
}
extern int getSystemMemory(int *s_mem)
{
    char buf[1024], *semiColon, *key, *value;
    int total = 1, free = 1, memStat;
    FILE *fp = fopen("/proc/meminfo", "r");
    if(!fp){
    	return -1;
    }
    
    while(fgets(buf, 1024, fp)){
    	if(!(semiColon = strchr(buf, ':')))
    		continue;
    	*semiColon = '\0';
    	key = buf;
    	value = semiColon + 1;
    	if(!strcmp(key, "MemTotal")){
    	    value = strip_space(value);
            total = atoi(value);
    	}else if(!strcmp(key, "MemFree")){
	    value = strip_space(value);
            free = atoi(value);
	}
    }
    fclose(fp);
    memStat = 100 - (free*100)/total;
#if 1
//    *s_mem = ((memStat < 85) ? ((int)(memStat*(((double)8)/17))) : (40+(memStat-85)*4));
    *s_mem = ((memStat < 85) ? ((int)(memStat*0.47)) : (40+(memStat-85)*4));
#else
    *s_mem = (memStat>50)?memStat-50:memStat;
#endif
    return 0;
}

/**
 *��ȡ�������״̬
 *total_space �ܿռ� unused ���ÿռ� 
 *��������治������total_spaceΪ0
 */
extern void getOuterFlashStat(long long *total_space, long long *unused_space)
{
    char device_path[256];

    memset(device_path, 0, sizeof(device_path));
    if(isUSBStorageExist(device_path)==1)
    {
	storageFree(device_path, total_space, unused_space);
    }
    return;
}
#endif
/*************************************************************
  * bit_array_to_int
  * tsize ���鳤�� array����ָ��
  * ����uint��ֵ
  *
  * jfgu
  * 2010-10-22
  ***********************************************************/ 
extern uint32 bit_array_to_int(unsigned char array[], uint32 tsize)
{
    uint32 i;
    uint32 value = 0u; 
    for (i = 0u; i < tsize; i++)
    {
	if (array[i] == 1u)
	{
	    value  = (value | (0x00000001u << i)) ;
	}
    }

    return value;
}
/*********************************************************************
  * int_to_bit_array
  * len ����  value ��ֵ 
  * ����char����member
  * jfgu
  * 2010-10-22
  ********************************************************************/
extern int int_to_bit_array(unsigned char member[], uint32 len, uint32 value)
{
    uint32 i;
    int count = 0;

    for (i = 0u; i < len; i++)
    {
	if ((value & (0x00000001u << i)) > 0)
	{
	    count++;
	    member[i] = 1u;
	}
	else
	{
	    member[i] = 0u;
	}
    }

    return count;
}
/***********************************************************************
 * �� �� ����   int2ip
 * �������ڣ�   2010-3-17     
 * �޸����ڣ�
 * ���ߣ�       KeHuatao
 * ����˵����   ��            
 ************************************************************************/
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

/***********************************************************************
 * �� �� ����   ip2int        
 * �������ڣ�   2010-3-17     
 * �޸����ڣ�                 
 * ���ߣ�       KeHuatao
 * ����˵����   ��
 ************************************************************************/
unsigned int ip2int(char *ip)
{
    char *tmp,ipbak[16]={0},*ptr;
    unsigned int ret=0;
    unsigned int value;

    strncpy(ipbak,ip, sizeof(ipbak)-1);
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


#if defined(SW_INGRESS_CPU_TAG) && (CONFIG_IP1725)

/*ingress cpu tag*/
typedef struct _tagInfoRx_t {
    uint16  type;
    uint16  value;
}tagInfoRx_t;
/*egress cpu tag*/
typedef struct _tagInfoTx_t{
    uint16 type;
    uint32 value;
}tagInfoTx_t;

#endif

/*
 *��ַ�ݼ���ʽ�����ڴ�
 *
 *KeHuatao
 */
void mem_dec_cpy(char *dst, char *src, int len)
{
    int i;

    for(i=0;i<len;i++)
    {
	*dst-- = *src--;
    }
}
#ifdef CONFIG_VSC7390
static ulong ht_cpu_ulong_from_buffer(const uchar *const buffer)
{
    return 
    ( buffer[0] << 24 ) |
    ( buffer[1] << 16 ) |
    ( buffer[2] <<  8 ) |
    ( buffer[3]       ) ;
}
#endif
/*
 *����յ����ݴ�switch��ͷ���������֮
 *����
 *    raw    -- ָ���յ���ԭʼ����
 *    sys_header  -- ���ذ����ͷ�а�����Դ�˿ڵ���Ϣ
 */
int sw_frame_header_pull(char *raw, system_frame_header_t * sys_header)
{
    int ret=0;
    char *data;
	tagInfo_t   *pk_tag;
#ifdef CONFIG_VSC7390
    ulong ifh0, ifh1;
#endif

#ifdef  SW_INGRESS_CPU_TAG
    data = raw + (2*MAC_LEN);
#if defined(CONFIG_IP1725)
    tagInfoRx_t *tagInfo;

    tagInfo = (tagInfoRx_t *)data;
    if(tagInfo->type == htons(CPU_TAG_TYPE))
    {
	sys_header -> source_port_no = htons(tagInfo->value);
	sys_header -> source_port_no = port2ext(sys_header->source_port_no);
    }
#elif defined(CONFIG_VSC7390)
    /* Dig the ifh - endian aware */
    ifh0 = ht_cpu_ulong_from_buffer(data);
    ifh1 = ht_cpu_ulong_from_buffer(data + sizeof(ulong));

    sys_header->source_port_no = port2ext(IFH_GET(ifh0, ifh1, PORT));
    sys_header->arrived_tagged = IFH_GET(ifh0, ifh1, TAGGED);
    sys_header->tag.tagprio = IFH_GET(ifh0, ifh1, UPRIO);
    sys_header->tag.cfi = IFH_GET(ifh0, ifh1, CFI);
    sys_header->tag.vid = IFH_GET(ifh0, ifh1, VID);
#endif
    mem_dec_cpy((data+CPU_INGRESS_TAG_LEN-1), (data-1), (2*MAC_LEN));
    *frame = raw + CPU_INGRESS_TAG_LEN;
    ret = CPU_INGRESS_TAG_LEN;

#else
    data = raw + (2*MAC_LEN);
	pk_tag = (tagInfo_t *)data;
	if(pk_tag->type == htons(ETH_P_8021Q))
	{
		sys_header->tag.vid = ntohs(pk_tag->value) & 0xfff ;
		if(pk_tag->proid_len == htons(ETH_P_8021Q))
		{
			data += sizeof(uint32);
			pk_tag = (tagInfo_t *)data;
			if(pk_tag->proid_len == htons(ETH_P_URCP))
			{
				data += sizeof(uint32);
				sys_header->raw = raw;
				sys_header->data = &data[sizeof(uint16)];
				ret = sizeof(uint32) * 2;
			}
		}else if(pk_tag->proid_len == htons(ETH_P_URCP))
		{
			data += sizeof(uint32);
			sys_header->raw = raw;
			sys_header->data = &data[sizeof(uint16)];
			ret = sizeof(uint32);
		}else{
			sys_header->raw = NULL;
			sys_header->data = NULL;
			ret = sizeof(uint32);
		}
	}else if(pk_tag->type == htons(ETH_P_URCP)){
		sys_header->tag.vid = 0;
		sys_header->raw = raw;
		sys_header->data = &data[sizeof(uint16)];
		ret = 0;
	}else{
		sys_header->raw = NULL;
		sys_header->data = NULL;
		ret = 0;
	}

#endif
    return ret;
}

/*
 *����switch �ӿ�name
 *KeHuatao
 *
 */
int get_switch_if_name(char * if_name)
{
    strcpy(if_name, SWITCH_IF_NAME);

    return 0;
}
/*
 *create raw socket
 *KeHuatao
 */
int create_raw_socket(int eth_proto)
{
    int fd, ret;
    struct ifreq req;
    struct sockaddr_ll myaddr_ll;

    /*ͨ���豸���ƻ�ȡindex*/      
    strcpy(req.ifr_name, SWITCH_IF_NAME);
    fd = socket(AF_INET, SOCK_DGRAM,0); 
    if(fd<0)
    {
	perror("create SOCK_DGRAM error");
	return -1;
    }

    /*get if index*/
    ret = ioctl(fd, SIOCGIFINDEX, &req);
    if (ret < 0)
    {
	perror("Get ifindex err");
	close(fd);
	return -1;
    }
    close(fd);

    /*set addr_ll*/
    memset(&myaddr_ll, 0, sizeof(myaddr_ll));
    myaddr_ll.sll_family  = PF_PACKET;
    myaddr_ll.sll_protocol = htons(eth_proto);
    myaddr_ll.sll_ifindex = req.ifr_ifindex;

    /*create raw socket*/
    if( (fd = socket(PF_PACKET, SOCK_RAW, htons(eth_proto))) < 0)
    {
	perror("create raw socket error");
	return -1;
    }

    /*bind*/
    if (bind(fd, (struct sockaddr*)&myaddr_ll, sizeof(myaddr_ll)) < 0)
    {
	close(fd);
	perror("bind socketaddr_ll error");
	return -1;
    }

    _socketL2 = fd;

    return _socketL2;
}
/*
 * mac compare
 * KeHuatao
 **/
int32 mac_cmp(unsigned char *mac1, unsigned char* mac2)
{
    /*����ת��*/
    unsigned short *a = (unsigned short *)mac1;
    unsigned short *b = (unsigned short *)mac2;

    /*�Ƚ�*/
    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) )!=0;
}
/*
 * �ڲ��˿ں�(��0��ʼ) ���߼��˿ںŵ�ת������һ����0��ʼ)
 * ** Define mapping between Luton24 port numbers and logical port numbers.
 * ** For each Luton24 port specify the logical port number and for each
 * ** logical port specify the Stapleford port number.
 *    Luton24 port number:  
 *    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
 * ---------------------------------------------------------------------- */

#define PORT_MAP_2_EXT  \
        1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u,10u,11u,12u,13u,14u,15u,16u,17u,18u,19u,20u,21u,22u,23u,24u
/*
 *  �߼��˿ںŵ��ڲ��˿ںŵ�ת��
 *  Logic port number:         
 *  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24                                                         
 * ---------------------------------------------------------------------- */
#define PORT_MAP_2_INT \
        0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u,10u,11u,12u,13u,14u,15u,16u,17u,18u,19u,20u,21u,22u,23u                              

static unsigned int port2ext_tab [PORT_COUNT] = { PORT_MAP_2_EXT };      

static unsigned int port2int_tab [PORT_COUNT + 1] = {
       0u,  /* dummy entry for index 0 */  
       PORT_MAP_2_INT
};

/**************************************************************************                                                        
 * 
 * Purpose     : Convert internal port number (physical GMII number) to 
 *               external (logic) port number.                                                                                      
 * Remarks     :
 * Restrictions:
 * See also    :
 * Example     :
 ************************************************************************** */                                                       
unsigned int port2ext(unsigned int port_no)
{
    return port2ext_tab[port_no];
}
/************************************************************************* 
 * Purpose     : Convert external (logic) port number to internal port 
 *               number (physical GMII number).
 * Remarks     :
 * Restrictions:
 * See also    :
 * Example     :
 ************************************************************************** */
unsigned int port2int(unsigned int port_no)
{
    return port2int_tab[port_no];
}

#if 0
static int32 sw_io_nic_tx(int32 fd, uchar * frame, uint32 length)
{
    int32 rc=0;
    rc = send(fd, frame, length, 0);
    if (rc != length) {
	perror("socket send error");
	return -1;
    }
    /* Roll your own interface to HW driver */
    return 0;
}
/*
 *��ָ���˿ڷ���
 *KeHuatao
 */
int32 h2_send_by_port(uint32 port_no, uchar *frame, uint32 length, vid_t vid)
{
    uchar localframe[MAX_FRAME_LENGTH + 4];
    uchar *fp;
    int32 ret = -1;
    uint32 val;

    memcpy(localframe, frame, CHIP_VLAN_OFFSET);
    fp = localframe + CHIP_VLAN_OFFSET;

    /* Insert VLAN tag */
    *fp++ = 0x81;
    *fp++ = 0x00;
    *fp++ = 0;
    *fp++ = EXT_CPU_VID;

    if((port_no>0) && (port_no<=PORT_COUNT)) {
#if defined(CONFIG_VSC7390)
	/* Insert Destination Port Tag */
	*fp++ = 0x81;
	*fp++ = 0x00;
	*fp++ = (7/*tagprio*/<<5) | (1/*cfi*/<<4); /* CFI bit set*/
	*fp++ = port2int(port_no);     
#elif defined(CONFIG_IP1725)
        *fp++ = CPU_TAG_TYPE >> 8;
        *fp++ = CPU_TAG_TYPE & 0xff;
	val = 1u<<port2int(port_no);
	*((uint32*)fp) = htonl(val);
	fp += 4;
#else
#endif
    }
#if 0
    else {
	/* port_no==0 */
	/* Let the switch decide egress ports */
    }
#endif

    if(vid != VID_NULL) {
	/* Insert VLAN tag */
	*fp++ = 0x81;
	*fp++ = 0x00;
	*fp++ = (0/*tagprio*/<<5) | (0/*cfi*/<<4) | (vid>>8);
	*fp++ = vid & 0xFF;
    }

    length -= CHIP_VLAN_OFFSET;
    memcpy(fp, frame + CHIP_VLAN_OFFSET, length);
    fp += length;

    /* 12 bytes (DMAC+SMAC) have been moved from frame to prefix */
    if(_fd>=0)
    {
	ret = sw_io_nic_tx(_fd, localframe, fp - localframe);
    }

    return ret;
}
#endif
#if 0
int send_len(int fd, int len)
{
    int size;

    size = len;
    sw_io_nic_tx(fd, &size, sizeof(int));
}

extern urcp_dev_t* urcp_dev_table_lookup(uchar* mac, uint32 ip);
/*
 *�������ݰ�
 *
 *
 */
int32 h2_send_frame(uchar* buf, uint32 len, int32 from)
{
    int32 ret = -1;
    urcp_dev_t* dev=NULL;

    if(from==0) {
        ret = sw_io_nic_tx(_socketL2, buf, len);
    }
    else if(from==1) {
//	send_len(_socketL3, len);
       // ret = sw_io_nic_tx(_socketL3, buf, len);
        ret = CWUrcpSend(buf, len);
    }
    else {
        dev = urcp_dev_table_lookup(buf,0);
        if(dev!=NULL) {
	    if(dev->fromL3==1) {
//		send_len(_socketL3, len);
//                ret = sw_io_nic_tx(_socketL3, buf, len);
                  ret = CWUrcpSend(buf, len);
	    }
	    else {
                ret = sw_io_nic_tx(_socketL2, buf, len);
	    }
	}
    }

    return ret;
}
#endif
/*
 * cpuת��
 * �������ݰ������ų������˿�
 *
 */
#if SW_FEATURE_PACKET
int32 h2_fwd_frame(uchar*buf, uint32 len, uint32 vid, uint32 exclude_port)
{
    int32 ret = -1;
    static fwd_dst_ports_cond_t cond;
    static fwd_dst_ports_ret_t  result;
    uint32 i = 0u;

    if(_fd!=-1)
    {
#ifdef CONFIG_IP1725
	if(((*buf)&0x01)==0)
	{
	    ret = sw_io_nic_tx(_fd, buf, len);
	}
	else
	{
#endif
	cond.src_port = exclude_port;/*Դ�˿�*/
	cond.vid_mac.vid = vid;/*vlan id*/
	memcpy(cond.vid_mac.mac.addr, buf, MAC_LEN);
	cond.vlan = 1u;

	sw_fwd_get_dst_ports(&cond, &result);/*query destination ports and vid*/

	for(i = START_INDEX; i< PORT_ARRAY_SIZE; i++)
	{
	    if(result.dest[i])/*��Ҫ��ö˿ڷ�������*/
	    {
		h2_send_by_port(i, buf, len, result.vid);
	    }
	}
#ifdef CONFIG_IP1725
	}
#endif
    }

    return ret;
}
#endif

void frame_dump(unsigned char* frame, int length)
{
    int i=0u;

    printf("frame dump length:%d\n", length);
    /*���ζ�ȡ����*/
    while(i<length)
    {
	/*print*/
	printf(" %02x", *(frame+i));
	i++;
	if((i%16)==0)
	{
	    printf("\n");/*����*/          
	}
    }
    printf("\n");/*����*/
}
/*
 *���ָ��Ŀ¼�������ļ�����
 *
 *KeHuatao
 *
 *
 */
int searchDir(char *path, dir_pf cb, void* param)
{
    int ret=0;
    unsigned char tmp[256];
    DIR* dir_info;
    struct dirent* dir_entry;
    struct stat buf1;

    dir_info = opendir(path);
    if(dir_info!=NULL)/*��ȡ�ɹ�*/
    {
	/*��ȡ�ļ�*/
	while(1)
	{
	    /*��ȡ�ļ�*/
	    dir_entry = readdir(dir_info);
	    if(dir_entry!=NULL)/*�ɹ�*/
	    {
		if((strcmp(dir_entry->d_name, "..")!=0) && (strcmp(dir_entry->d_name, ".")!=0))
		{
		    /*�ļ���*/
		    memset(tmp, 0, sizeof(tmp));
		    strcat(tmp, path);
		    strcat(tmp, dir_entry->d_name);
		    /*�ļ��޸�ʱ��*/
		    if((stat(tmp, &buf1)==0) && (S_ISREG(buf1.st_mode)))
		    {
		        cb(tmp, param);
			ret=1;
		    }
		}
	    }
	    else
	    {
		break;
	    }
	}
	/*�ر�*/
	closedir(dir_info);
    }

    return ret;
}

int get_verinfo_from_file(char * file, int* hwVer, int* swVer){
	FILE * fp; 
	int ret = -1; 
	image_header_t head;
	int tmp;

        fp = fopen(file, "r");
	if ( fp != NULL ){
		fread( &head, 1, sizeof(image_header_t), fp );
		tmp = (int)ntohl( head.systemVer );
		if ( swVer ){
			*swVer = (tmp>>(HWVER_1+HWVER_2));
		}
		if ( hwVer ){
			*hwVer = (tmp & (0xFFFFFFFF>>(SWVER_1+SWVER_2+SWVER_3)));
		}
		ret = 0;
		fclose(fp);
	}else{
		ret = -1;
	}
	return ret;
}
#if 0
#define CAPWAP_SOCKET  "/tmp/capwap_socket"
#define URCP_SOCKET    "/tmp/urcp_socket"
int connect_capwap(void)
{
        int sockfd=-1;
        struct  sockaddr_un capwap;

#ifdef CAPWAP_UDP
        struct  sockaddr_un urcp;

        if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
#else
        if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
#endif
        {
            return -1;
        }
        bzero(&capwap, sizeof(capwap));
        capwap.sun_family = AF_UNIX;
        strcpy(capwap.sun_path, CAPWAP_SOCKET);

#ifdef CAPWAP_UDP
	unlink(URCP_SOCKET);
        bzero(&urcp, sizeof(urcp));
        urcp.sun_family = AF_UNIX;
        strcpy(urcp.sun_path, URCP_SOCKET);
	if (bind(sockfd, (struct sockaddr *)&urcp, sizeof(urcp)) < 0){
            perror("failed to bind socket");
	    close(sockfd);
            return -1;
	}
#endif
        if (connect(sockfd, (struct sockaddr*)&capwap, sizeof(capwap)) < 0) {
            perror("failed to connect server");
	    close(sockfd);
            return -1;
        }

	_socketL3 = sockfd;

        return _socketL3;
}
#endif
