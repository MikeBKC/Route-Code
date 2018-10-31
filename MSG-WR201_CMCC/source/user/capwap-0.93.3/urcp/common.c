/*
 *
 *公共函数
 *KeHuatao
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/switch/switch.h>
#include <net/if.h>
#include <sys/utsname.h>
#include "common.h"
#include "urcp.h"
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include <usb-user.h>
#endif
#include <linux/sockios.h>

#ifdef URCP_WEB
#include <switch.h>
#include <packet.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#else
#include "packet.h"
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#endif

#if (CLIENT_HOST_NAME == FYES)
#include "dhcp.h"
#endif

#if defined(CONFIG_USER_CAPWAP_AC) || defined(CONFIG_USER_CAPWAP_WTP)
#include "../CWAC.h"
#include <ctype.h>
#include <signal.h>
#if defined(CONFIG_USER_CAPWAP_WTP)
#include "../CWWTP.h"
#endif
#endif
#include "urcp_debug.h"
//#ifdef URCP_AP
#if 1
#if 1
#ifdef UBOOT_HEADER

#define IH_MAGIC        0x27051956      /* Image Magic Number           */
#define IH_NMLEN        20          /* Image Name Length            */

typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	systemVer;	/* Image Creation Timestamp	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		name[IH_NMLEN];	/* Image Name		*/
	uint32_t    ih_reserve;
	uint32_t	ih_ksz;		/* Kernel Part Size		*/
} image_header_t;
#else
typedef struct image_header {
        uint32_t imageLen;
        uint32_t imageCrc;
        uint32_t arch;
        uint32_t fsLen;
        uint32_t os;
	    uint32_t systemVer;
        uint32_t imageComp;
        uint32_t loadAddr;
        uint32_t entryAddr;
        uint8_t  name[20];
        uint32_t kernelLen;
        uint32_t kernelCrc;
} image_header_t;
#endif
#endif
static int32 sw_io_nic_tx(int32 fd, uchar * frame, uint32 length);
#ifdef URCP_CMD_AP_SYS_INFO_GET
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

/*****************************************************************8
 * 函数名称 ： getSystemCpu(int *s_cpu)
 * 函数功能 ： 获取CPU使用率
 * 参    数 ： 使用率变量的指针         
 * 返 回 值 ： void
 * 作    者 ：            
 * 日    期 ：      
 ******************************************************************/ 
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

/*****************************************************************8
 * 函数名称 ： getSystemMemory(int *s_mem)
 * 函数功能 ： 获取内存使用率
 * 参    数 ： 使用率变量的指针         
 * 返 回 值 ： -1,失败。0，成功
 * 作    者 ：            
 * 日    期 ：      
 ******************************************************************/ 
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

/*获取速率*/
extern int get_ap_rf_rate(uint16 *rate, char *rf_ra)
{
    FILE *fp = NULL;
    char buf[1024] = "", *search = NULL;
    char str_ra[32] = "";
    int c_rate = 0;
    int ret = 0;

    sprintf(str_ra, "iwconfig %s0", rf_ra);
    fp = popen(str_ra, "r");
    if (fp != NULL)
    {
	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "Bit Rate")) )
	    {
		continue;
	    }
	    if ( (sscanf(search, "Bit Rate=%d", &c_rate) != -1) )
	    {
	    	*rate = (uint16)c_rate;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
	pclose(fp);
    }
    else
    {
	ret = -1;
    }
    return ret;
}

/*获取功率*/
extern int get_ap_power(uint16 *power, char *rf_ra)
{
    FILE *fp = NULL;
    char buf[1024] = "", *search = NULL;
    int c_power = 0;
    int ret = 0;
    char str_ra[32] = "";

    sprintf(str_ra, "iwpriv %s0 get TxPower", rf_ra);
    fp = popen(str_ra, "r");
    if (fp != NULL)
    {
	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "TxPower:")) )
	    {
		continue;
	    }
	    if ( (sscanf(search, "TxPower:%d", &c_power) != -1) )
	    {
	    	*power = (uint16)c_power;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
	pclose(fp);
    }
    else
    {
	ret = -1;
    }
    return ret;
}

/*获取beacon*/
extern int get_ap_beacon(uint16 *bea, char *rf_ra)
{
    FILE *fp = NULL;
    char buf[1024] = "", *search = NULL;
    int c_bea = 0;
    int ret = 0;
    char str_ra[32] = "";

    sprintf(str_ra, "iwpriv %s0 get Config", rf_ra);
    fp = popen(str_ra, "r");
    if (fp != NULL)
    {
	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "BeaconPeriod:")) )
	    {
		continue;
	    }
	    if ( (sscanf(search, "BeaconPeriod:%d", &c_bea) != -1) )
	    {
	    	*bea = (uint16)c_bea;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
	pclose(fp);
    }
    else
    {
	ret = -1;
    }
    return ret;
}

extern int get_ap_RTX_info(uint32 *p_tx_packet, uint32 *p_tx_bytes, uint32 *p_rx_packet, uint32 * p_rx_bytes, char *rf_ra)
{
    int ret = 0;
    FILE *fp = NULL;
    char buf[1024] = "", *search = NULL;
    uint32 tx_p = 0u, tx_b = 0u, rx_p = 0u, rx_b = 0u;
    char str_ra[32] = "";

    sprintf(str_ra, "iwpriv %s0 stat", rf_ra);
    fp = popen(str_ra, "r");
    if (fp != NULL)
    {
	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "Tx success")) )
	    {
		continue;
	    }
	    search = strchr(search, '=');
	    if ( (sscanf(search, "=%u", &tx_p) != -1) )
	    {
	    	*p_tx_packet = tx_p;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
    	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "Tx bytes")) )
	    {
		continue;
	    }
	    search = strchr(search, '=');
	    if ( (sscanf(search, "=%u", &tx_b) != -1) )
	    {
	    	*p_tx_bytes = tx_b;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "Rx success")) )
	    {
		continue;
	    }
	    search = strchr(search, '=');
	    if ( (sscanf(search, "=%u", &rx_p) != -1) )
	    {
	    	*p_rx_packet = rx_p;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
	while (fgets(buf, sizeof(buf), fp))
	{
	    if ( !(search = strstr(buf, "Rx bytes")) )
	    {
		continue;
	    }
	    search = strchr(search, '=');
	    if ( (sscanf(search, "=%u", &rx_b) != -1) )
	    {
	    	*p_rx_bytes = rx_b;
	    }
	    else
	    {
		ret = -1;
	    }
	    break;
	}
	
	pclose(fp);
    }
    else
    {
	ret = -1;
    }
    return ret;
}
#endif

/*****************************************************************8
 * 函数名称 ： set_ip(char *ip)
 * 函数功能 ： 设置系统IP地址 
 * 参    数 ： ip地址         
 * 返 回 值 ： 0 : 成功 ；  -1 :  失败 
 * 作    者 ： jfgu           
 * 日    期 ： 2010-10-22     
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
 * 函数名称 ： set_mask(char *ip)
 * 函数功能 ： 设置系统Mask地址
 * 参    数 ： mask 4个字节表示
 * 返 回 值 ： 0 : 成功 ；  -1 :  失败 
 * 作    者 ： jfgu
 * 日    期 ： 2010-10-22     
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
    {//为全零则只清空
	sprintf(cmd_str,"route add default gw %s &", gwaddr);
	system(cmd_str);
    }
}
/*
 * 将mac地址转换成标准格式
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


/*
 * mac compare
 * KeHuatao
 **/
int32 mac_cmp(unsigned char *mac1, unsigned char* mac2)
{
    /*类型转换*/
    unsigned short *a = (unsigned short *)mac1;
    unsigned short *b = (unsigned short *)mac2;

    /*比较*/
    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) )!=0;
}

#if 0
extern uint32 getSystemSn(void)    
{
    int32 fd;
    static uint32 sn=0, first = 1u;   
    void * map_base;

    if(first == 1u) {
	if((fd=open("/dev/mem",O_RDWR|O_SYNC))==-1){
	    perror("open /dev/mem:");
	    return (-1);
	}
	map_base=mmap(0,0xff,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0xbf000000);
	sn = *((volatile uint32 *) (map_base+0x2c));
	munmap(map_base, 0xff);
	close(fd);
	first = 0u;
    }

    return sn;
}
#endif

/*
 *返回switch 接口name
 *KeHuatao
 *
 */
int get_switch_if_name(char * if_name)
{
    strcpy(if_name, AP_IF_NAME);
    
    return 0;
}
int get_switch_eth_name(char *if_name)
{
    strcpy(if_name,AP_ETH_NAME);
    return 0;
}
/***********************************************************************
 * 函 数 名：   get_ip
 * 功能描述：	get interface ip
 * 输入参数:    interface name,a 16-byte buffer to store ip address
 * 输出参数：	ip
 * 返回值：     0,-1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
extern int get_ip(char *ifname, uint32 *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("getIfIp: open socket error");
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
 * 函 数 名：   get_mac
 * 功能描述：	get interface mac addr
 * 输入参数:    interface name,a 18-byte buffer to store ip address
 * 输出参数：	mac addr
 * 返回值：     0,-1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
extern int get_mac(char *ifname, unsigned char *if_hw)
{
	struct ifreq ifr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("getIfMac: open socket error");
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
 * 函 数 名：   get_mask 
 * 功能描述：	get interface  netmask
 * 输入参数:    interface name,a 16-byte buffer to store subnet mask
 * 输出参数：	netmask
 * 返回值：     0,-1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
extern int get_mask(char *ifname, uint32 *if_net)
{
	struct ifreq ifr;
	int skfd = 0;
	int ret = 0;
	uint32 ip;

	ret = get_ip(ifname, &ip);
	if (ret != 0) {
		*if_net = 0;
		return 0;
	}

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("getIfNetmask: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		return -1;
	}

	*if_net = (uint32)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
	close(skfd);
	return 0;
}


#if (CLIENT_HOST_NAME == FYES)
static int dhcp_bag_handle(char *raw, char *data, ip_udp_hdr_t* ip_udp_packet, system_frame_header_t * sys_header)
{
    int ret = 0;
    if (ip_udp_packet->proto == IP_PROTO_UDP)
    {   
	if((ip_udp_packet->destport == htons(DHCP_UDP_CLIENT_PORT)) || /*处理dhcp包*/
		(ip_udp_packet->destport == htons(DHCP_UDP_SERVER_PORT)) ||
		(ip_udp_packet->srcport == htons(DHCP_UDP_CLIENT_PORT)) ||
		(ip_udp_packet->srcport == htons(DHCP_UDP_SERVER_PORT)))
	{   
	    /*包处理函数*/
	    sys_header->tag.vid = 0;
	    sys_header->raw = raw;
	    sys_header->packet_type = PACKET_DHCP; 
	    sys_header->data = data + sizeof(uint16) + sizeof(ip_udp_hdr_t);
	    ret = DHCP_UDP_OVERHEAD ;
	}
	else
	{
	    sys_header->raw = NULL;
	    sys_header->data = NULL;
	    ret = 0;
	}
    }
    else
    {
	sys_header->raw = NULL;
	sys_header->data = NULL;
	ret = 0;
    }
    return ret;
}
#endif
/*
 *如果收到数据带switch包头，则需剥离之
 *参数
 *    raw    -- 指向收到的原始数据
 *    sys_header  -- 返回剥离包头中包含的源端口等信息
 */
#define ETH_P_URCP 0x8899
int sw_frame_header_pull(char *raw, system_frame_header_t * sys_header)
{
    int ret=0;
    char *data;
#if (CLIENT_HOST_NAME == FYES)
    ip_hdr_t* ip_packet;
    ip_udp_hdr_t* ip_udp_packet;
#endif

	tagInfo_t	*pk_tag;
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
				sys_header->data = data + sizeof(uint16);
				ret = sizeof(uint32) * 2;
			}
#if (CLIENT_HOST_NAME == FYES)
			else if (pk_tag->proid_len == htons(ETH_P_IP))
			{
			    data += sizeof(uint32);
			    ip_packet = (ip_hdr_t *)(&raw[UIP_LLH_LEN + sizeof(uint32) + sizeof(uint32)]);
			    ip_udp_packet = (ip_udp_hdr_t *)ip_packet;
			    ret = dhcp_bag_handle(raw, data, ip_udp_packet, sys_header);
			    ret += sizeof(uint32) * 2;
			}
#endif
		}else if(pk_tag->proid_len == htons(ETH_P_URCP))
		{
			data += sizeof(uint32);
			sys_header->raw = raw;
			sys_header->data = data + sizeof(uint16);
			ret = sizeof(uint32);
		}
#if (CLIENT_HOST_NAME == FYES)
		else if (pk_tag->proid_len == htons(ETH_P_IP))
		{
		    data += sizeof(uint32);
		    ip_packet = (ip_hdr_t *)(&raw[UIP_LLH_LEN + sizeof(uint32)]);
		    ip_udp_packet = (ip_udp_hdr_t *)ip_packet;
		    ret = dhcp_bag_handle(raw, data, ip_udp_packet, sys_header);
		    ret += sizeof(uint32);
		}
#endif
		else{
			ret = 0;
		}
	}else if(pk_tag->type == htons(ETH_P_URCP)){
		sys_header->tag.vid = 0;
		sys_header->raw = raw;
		sys_header->data = data + sizeof(uint16);
		ret = 0;
	}
#if (CLIENT_HOST_NAME == FYES)
	else if(pk_tag->type == htons(ETH_P_IP))
	{
	    ip_packet = (ip_hdr_t*)(&raw[UIP_LLH_LEN]);/*指向ip包头*/
	    ip_udp_packet = (ip_udp_hdr_t*)(ip_packet);/*指针后移,包含UDP包头*/
	    ret = dhcp_bag_handle(raw, data, ip_udp_packet, sys_header);
	}
#endif
	else{
		sys_header->raw = NULL;
		sys_header->data = NULL;
		ret = 0;
	}

#endif
    return ret;
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

    /*通过设备名称获取index*/      
    strcpy(req.ifr_name, AP_IF_NAME);
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
	perror("bind socketaddr_ll error");
	return -1;
    }

    return fd;
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

        return sockfd;
}
int send_len(int fd, int len)
{ 
    int size, ret=0;
  
    size = len;
    ret = sw_io_nic_tx(fd, (uchar*)&size, sizeof(int));

    return ret;
} 
#endif
int CWAPUrcpSend(char *buffer, int len)
{
    int i, seqNum, fragmentsNum = 0;
    const int msgElemCount = 1;
    CWProtocolMessage *completeMsgPtr = NULL;
    CWProtocolMessage *msgElems;

    CW_CREATE_PROTOCOL_MSG_ARRAY_ERR(msgElems, msgElemCount, return -1;);
    CW_CREATE_PROTOCOL_MESSAGE(*msgElems, 4+len, {free(msgElems);return -1;});
    CWProtocolStore16(msgElems, CW_MSG_ELEMENT_URCP_PDU_CW_TYPE);
    CWProtocolStore16(msgElems, len);
    CWProtocolStoreRawBytes(msgElems, buffer, len);

    seqNum = CWGetSeqNum();

    if(CWAssembleMessage(&completeMsgPtr,
		    &fragmentsNum,
		    gWTPPathMTU,
		    seqNum,
		    CW_MSG_TYPE_VALUE_URCP_REQUEST,
		    msgElems,
		    msgElemCount,
		    NULL,
		    0,
#ifdef CW_NO_DTLS
                    CW_PACKET_PLAIN
#else
		    CW_PACKET_CRYPT
#endif
		    ))
    {
	    for (i = 0; i < fragmentsNum; i++) 
	    {
                CWThreadMutexLock(&urcpMutex);
#ifdef CW_NO_DTLS
		if (!CWNetworkSendUnsafeConnected(gWTPSocket, completeMsgPtr[i].msg, completeMsgPtr[i].offset))
#else
		if (!CWSecuritySend(gWTPSessiong, completeMsgPtr[i].msg, completeMsgPtr[i].offset))
#endif
		{
                    CWDebugLog("Failure sending URCP PDU");
		    CWThreadMutexUnlock(&urcpMutex);
                    break;
		}
		CWThreadMutexUnlock(&urcpMutex);
	    }
    }

    for (i = 0; i < fragmentsNum; i++)
    {
	CW_FREE_PROTOCOL_MESSAGE(completeMsgPtr[i]);
    }
    CW_FREE_OBJECT(completeMsgPtr);

    return 0;
}
/*
 *发送数据包
 *
 *
 */

int32 h2_send_frame(uchar*buf, uint32 len)
{
    int32 ret = -1;

    if((gSocketL3>=0) && (ap_infor.ac_infor.fromL3==1)) {
	//send_len(gSocketL3, len);
	//ret = sw_io_nic_tx(gSocketL3, buf, len);
	ret = CWAPUrcpSend(buf, len);
    }
    else if(gSocketL2>=0) {
	ret = sw_io_nic_tx(gSocketL2, buf, len);
    }

    return ret;
}

int32 h2_send_frame_l2l3(uchar*buf, uint32 len)
{
    int32 ret = -1;

    if(gSocketL3>=0) {
	//send_len(gSocketL3, len);
	//ret = sw_io_nic_tx(gSocketL3, buf, len);
	ret = CWAPUrcpSend(buf, len);
    }

    if(gSocketL2>=0) {
	ret = sw_io_nic_tx(gSocketL2, buf, len);
    }

    return ret;
}

void apRandomSleep(void)
{
    unsigned int delay=0, sn=0;

    sn = getSystemSn();
    srand(sn);
    delay = rand();
    delay = delay % 7;
    db_info(URCP_MODULE_ID_STATE, "%s sleep:%d, sn:%d\n", __func__, delay, sn);
    sleep(delay);
}
/*
 * 获取产品型号 
 *
 * added by Shiyu.Chen, 2012.06.18
 */
void get_sysinfo_machine_model(char model[64])
{
    struct utsname utsbuf;

    uname(&utsbuf);

    strcpy(model, utsbuf.machine);

    return;
}


/*
 * 获取产品硬件型号
 *
 * added by Shiyu.Chen, 2012.06.18
 */
void get_sysinfo_hw_model(char hwmodel[64])
{
    strcpy(hwmodel, UTT_HWMODEL);

    return;
}

#if(UTT_TOMAX == FYES)
static int getSystemInfoSysVersion(char sw_version[64])
{   
    char *tmp = NULL;
    struct utsname utsbuf;
    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");
    tmp = strtok(NULL, " ");
    sprintf(sw_version,"%s-%s-%s", utsbuf.version,UTT_HWMODEL,tmp);
    tmp = strtok(NULL, " ");
    return 0;
}
#endif

/*
 * 获取软件版本信息
 *
 * added by Shiyu.Chen, 2012.06.18
 */
int get_sysinfo_sw_version(char sw_version[64])
{
#if (UTT_TOMAX == FYES)
    getSystemInfoSysVersion(sw_version);
#else
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
    strncpy(sw_version, software_rev, 64); 
#endif
    return 0;
}


/***********************************************************************
 * 函 数 名：   utt_tolower
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static char utt_tolower(char x)
{
    if(x <= 'Z' && x >= 'A')
        return (x | 0x20) ;
    else
        return x;

}

/***********************************************************************
 * 函 数 名：   strstri
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
char *strstri (const char *known, const char *substr)
{
    if (known == substr)
        return ((char *)known);

    while (*known) {
        /*
         * Check to see if we match on a character yet
         */
        if ( utt_tolower(*known++) == utt_tolower(*substr) ) {
            /*
             * We do, so see if the rest of the substr matches
             */
            char *runner     = (char *)substr + 1;
            char *matchpoint = (char *)known - 1;

            while (*runner) {
                /*
                 * Compare our strings
                 */
                if ( utt_tolower(*known) != utt_tolower(*runner) )
                    break;
                /*
                 * Step to the next comparison
                 */
                known++;
                runner++;
            }

            /*
             * Did we get to the end of the substring?
             */
            if (!(*runner))
                return(matchpoint);
        }
    }

    return(NULL);
}

/***********************************************************************
 * 函 数 名：   Conversion2StdMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
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

static unsigned int ap_dhcp_en(void)
{
    unsigned int dhcp_en = 0;
#if 0
    ApBasicConfProfile *prof = NULL;

    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF,0);
    if (NULL != prof) {
        dhcp_en = prof->dhcp_en;
    }
#endif
    return dhcp_en;
}
void get_ip_from_dhcp_server(uint32 ac_ip)
{
    char str[100];

    if ((1 == ap_dhcp_en()) && (ac_ip != 0)) {
        system("killall udhcpc 1>/dev/null 2>&1");
        memset(str, '\0', sizeof(str));
//        sprintf(str, "udhcpc -i br0 -r %s -s /sbin/udhcpc.sh -p /var/run/udhcpd.pid &", int2ip(ntohl(ac_ip),ip));
        sprintf(str, "udhcpc -i br0  -s /sbin/udhcpc.sh -p /var/run/udhcpd.pid &");
        system(str);
   }
}

#endif /*URCP_AP*/

/*
 * 判断mac地址是否合法单播地址
 **/
int32 isValidMac(uchar*mac)
{	
    uchar nullMac [6]={'\0','\0','\0','\0','\0','\0'};
    int32 ret=1;

    if((mac_cmp((uchar*)nullMac, mac) == 0) /* 空地址*/
      || (*mac&0x1u) == 1u)/*多播地址*/ 
    { 
	ret = 0;   
    }  

    return ret; 
}

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
*判断文件大小
*KeHuatao
*
*/
int32 getFileSize(char *path, FILE* fd)
{
    int fileField;
    struct stat buf;
    int32 l=0;

    if(path!=NULL) 
    {
	/*根据文件名*/
        if(stat(path, &buf)==0)
        {
            l = buf.st_size;
	}
    }
    else if(fd!=NULL)
    {
	/*文件指针*/
        fileField = fileno(fd);
        if(fstat(fileField, &buf)==0)
        {
             l = buf.st_size;
	}
    }

    return l;
}
/*
 *创建一个临时文件
 *KeHuatao
 *
 */
void create_tmp_file(const char * preStr, char *fName)
{
    int32 l, fd=-1;

    l = strlen(fName);
    memset(fName, 0, l);
    if(preStr!=NULL)
    {
	strcpy(fName, preStr);
    }
    strcat(fName, "XXXXXX");
    /*生成文件名*/
    fd = mkstemp(fName);
    db_info(URCP_MODULE_ID_TFTP, "%s: %s\n", __func__, fName);
    if(fd>=0) {
        close(fd); /*这里只是为了生成唯一的文件名，暂不打开*/
    }

    return ;
}

/*
 *统计一个二进制形式的整数中1的个数
 *
 *KeHuatao
 */
unsigned short countOne(unsigned int num)
{
    unsigned short val=0;

    while(num>0)
    {
	num = num&(num-1);
	val++;
    }

    return val;
}
/*
 *计算校验和
 *KeHuatao
 *
 */
uint16 chk_sum(uint16 *buffer, uint32 size)
{
    ulong cksum;

    cksum = 0;

    while (size > 1)
    {
	cksum += *buffer++;
	size -= sizeof(ushort);        
    }

    if (size != 0)
    {
#if 0
	cksum += *(uchar*)buffer <<8; //考虑到大小端模式，要移位
#else
	cksum += *(uchar*)buffer;      
#endif
    }

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (uint16)cksum;
}

/*
 *  *将字符型十六进制数转换成数值型
 *   *KeHuatao
 *    */    
int char2hex(unsigned char* ch)
{
    int ret=0;                

    if((*ch>='a') && (*ch<='z'))   
    {  
	/*小写*/
	*ch = *ch - 'a' + 10; 
    }  
    else if((*ch>='A') && (*ch<='Z'))                                                                                               
    {  
	/*大写*/
	*ch = *ch - 'A' + 10; 
    }  
    else if((*ch>='0') && (*ch<='9'))                                                                                               
    {  
	/*数字*/              
	*ch = *ch - '0';
    }
    else
    {
	/*错误*/
	*ch = 0;
	ret=-1;
    }

    return ret;
}
int str2mac(unsigned char strMac[12], unsigned char stdMac[6])
{
    unsigned char ch;         
    unsigned int i, j;        

    /*clear*/
    memset(stdMac, 0, 6);     
    /*循环六次*/
    for(i=0u;i<17u;i++)        
    {  
	/*取2个字符*/         
	for(j=0u;j<2u;j++)    
	{
	    ch=strMac[i*3+j]; 
	    /*结束*/          
	    if(ch=='\0')      
	    {
		if(j==1u)     
		{             
		    /*后面补0*/
		    stdMac[i] = stdMac[i]*((unsigned char)16);
		}
		return 0;
	    }

	    if(char2hex(&ch)<0)
	    {
		return -1;
	    }
	    /*左移*/
	    stdMac[i] = stdMac[i]*((unsigned char)16) + ch;
	}
    }
    return 0;
}


#ifdef URCP_AP
/********************************************************
 * 获取软硬件版本
 *
 *
 *******************************************************/
int get_verinfo_from_file(char * file, int* hwVer, int* swVer){
	FILE * fp;
	int ret = -1;
	image_header_t head;
	int tmp;
	
	if ( fp = fopen( file, "r" ) ){
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

void get_verinfo_from_local(unsigned int* hwVer, unsigned int* swVer){
	unsigned int tmp;
	image_header_t local_head;
	memset(&local_head,0,sizeof(image_header_t));
	flash_read_mtdname("Kernel", &local_head, 0, sizeof(image_header_t) );
	tmp = (int)ntohl(local_head.systemVer);
	if( swVer ){
		*swVer = (tmp>>(HWVER_1+HWVER_2));
	}
	if( hwVer ){
		*hwVer = (tmp & (0xFFFFFFFF>>(SWVER_1+SWVER_2+SWVER_3)));
	}
}

/********************************************************
 * 把整型硬件版本号转换成可读的字符串形式 如 1.1
 * 参数 ：hwVer 硬件整型版本号
 *		  hwver_buf 转换存储到的字符串地址
 * 返回值 成功  返回 存储在hwver_buf中的版本号字节数
 *		  失败  返回 0
 *******************************************************/
int hwverinfo_to_str(unsigned int hwver, char* hwver_buf){
	int ret = 0;
	unsigned int major,minor;
	major = (hwver>>HWVER_2);
	minor = ((hwver & (0xFFFFFFFFu>>(HWVER_1+SWVER_1+SWVER_2+SWVER_3))) );
	ret = sprintf(hwver_buf,"V%d.%d",major,minor);  
	return ret;
}

/********************************************************
 * 把整型软件版本号转换成可读的字符串形式 如 1.1.1
 * 参数 ：hwVer 软件整型版本号
 *		  hwver_buf 转换存储到的字符串地址
 * 返回值 成功  返回 存储在hwver_buf中的版本号字节数
 *		  失败  返回 0
 *******************************************************/
int swverinfo_to_str(unsigned int swver, char* swver_buf){
	int ret = 0;
	unsigned int major,minor,minor2;
	major = (swver>>(SWVER_2+SWVER_3));
	minor = (swver>>SWVER_3) & (0xFFFFFFFFu>>(HWVER_1+HWVER_2+SWVER_1+SWVER_3));
	minor2 = swver & (0xFFFFFFFFu>>(HWVER_1+HWVER_2+SWVER_1+SWVER_2));
	ret = sprintf(swver_buf,"V%d.%d.%d",major,minor,minor2);
	return ret;
}
#endif
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
/*
 *将文件保存到u盘指定目录
 *KeHuatao
 *
 */
int saveFile2USB(char* file_name, char * src_path, char* dst_path)
{
	char tmp[256], mount_path[256];
	int error_index=-1;
	long long freeDisk=0, size;
	struct stat buf;

	/*每次都重新读取剩余空间大小*/
	memset(mount_path, 0, sizeof(mount_path));
	isUSBStorageExist(mount_path);
	if(storageFree(mount_path, NULL, &freeDisk)!=-1)
	{
		/*文件名*/
		memset(tmp, 0, sizeof(tmp));
		strcat(tmp, src_path); 
		strcat(tmp, file_name); 

		/*文件修改时间*/
		if((stat(tmp, &buf)==0) && (S_ISREG(buf.st_mode)))
		{
			size = (long long)(buf.st_size);
			/*判断大小*/
			if(freeDisk>=size)
			{
				/*将文件保存到u盘*/
				strcat(mount_path, dst_path); 
				strcat(mount_path, file_name); 
				if(copy_file(tmp, mount_path, 1, 0)==-1)
				{
				    error_index = WEB_MSG_FAIL;
				    db_info(URCP_MODULE_ID_TFTP, "%s: failed to save image file\n", __func__);
				}
			}
			else
			{
				/*剩余空间不够*/
				error_index = WEB_MSG_NOT_ENOUGH;
			}
		}
	}
	else
	{
        /*出错*/
		error_index = WEB_MSG_FAIL;
	}

	return error_index;
}
#endif
#ifdef CONFIG_WLAN_LAN_ISOLATION
int set_br_isolation(int enable)
{
    struct ifreq temp;
    int skfd, ret;

    memset(&temp, 0, sizeof(struct ifreq));
    strcpy(temp.ifr_name, "br0");

    if((skfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    return -1;
    }

    temp.ifr_flags = enable;
    ret = ioctl(skfd, SIOCBRISOLATION, &temp);
    if(ret < 0)
    {
	return -1;
    }
    close(skfd);

    return 0;
}
#endif
#if 0
//#ifdef URCP_AC
static void sendUrcpMessage(int wtpIdx, CWProtocolMessage **completeMsgPtr, CWProtocolMessage *msgElems, const int msgElemNum)
{
    int seqNum, i;
    int fragmentsNum = 0;

    seqNum = CWGetSeqNum();

    if(CWAssembleUrcpMessage(completeMsgPtr,
		&fragmentsNum,
		gWTPs[wtpIdx].pathMTU,
		seqNum,
		CW_MSG_TYPE_VALUE_URCP_REQUEST,
		msgElems,
		msgElemNum,
		NULL,
		0,
#ifdef CW_NO_DTLS
		CW_PACKET_PLAIN
#else
		CW_PACKET_CRYPT
#endif
		))
    {
	for (i = 0; i < fragmentsNum; i++) 
	{
	    CWThreadMutexLock(&gWTPs[wtpIdx].urcpMutex);
#ifdef CW_NO_DTLS
	    if (!CWNetworkSendUnsafeUnconnected(gWTPs[wtpIdx].socket, &gWTPs[wtpIdx].address, (*completeMsgPtr)[i].msg, (*completeMsgPtr)[i].offset))
#else
            if (!CWSecuritySend(gWTPs[wtpIdx].session, (*completeMsgPtr)[i].msg, (*completeMsgPtr)[i].offset))
#endif
            {
		    CWDebugLog("Failure sending URCP PDU");
		    CWThreadMutexUnlock(&gWTPs[wtpIdx].urcpMutex);
		    break;
            }
	    else {
	        CWThreadMutexUnlock(&gWTPs[wtpIdx].urcpMutex);
	    }
	}
    }

    for (i = 0; i < fragmentsNum; i++)
    {
	CW_FREE_PROTOCOL_MESSAGE((*completeMsgPtr)[i]);
    }
    CW_FREE_OBJECT(*completeMsgPtr);
}
int CWUrcpSend(char *buffer, int len)
{
    int i, broadcast;
    const int msgElemCount = 1;
    CWProtocolMessage *completeMsgPtr = NULL;
    CWProtocolMessage *msgElems;

    if((buffer[0] & 0x01u) == 0) {
        broadcast = 0;
    }
    else {
	broadcast = 1;
    }

    completeMsgPtr = NULL;

    CW_CREATE_PROTOCOL_MSG_ARRAY_ERR(msgElems, msgElemCount, return -1;);
    CW_CREATE_PROTOCOL_MESSAGE(*msgElems, 4+len, {free(msgElems);return -1;});
    CWProtocolStore16(msgElems, CW_MSG_ELEMENT_URCP_PDU_CW_TYPE);
    CWProtocolStore16(msgElems, len);
    CWProtocolStoreRawBytes(msgElems, buffer, len);

    for(i = 0; i < CW_MAX_WTP; i++) {
	if( gWTPs[i].isNotFree && gWTPs[i].WTPProtocolManager.name && (gWTPs[i].currentState==CW_ENTER_RUN)
		&& ( (broadcast==1) || mac_cmp(buffer, gWTPs[i].priv.WTPMac)==0)) {
	    sendUrcpMessage(i, &completeMsgPtr, msgElems, msgElemCount);
#if 0//def DEBUG_URCP

	    cprintf("found ap %d\n", i);
#endif
	    if(broadcast==0) {
		break;
	    }
	}
    }
#if 1//def URCP_DEBUG 
    if((i>=CW_MAX_WTP) && (broadcast==0))
	cprintf("found no ap\n");
#endif
    for(i = 0; i < msgElemCount; i++) {
	CW_FREE_PROTOCOL_MESSAGE(msgElems[i]);
    }
    CW_FREE_OBJECT(msgElems);

    return 0;
}

extern urcp_dev_t* urcp_dev_table_lookup(uchar* mac, uint32 ip);
/*
 *发送数据包
 *
 *
 */
int32 h2_send_frame(uchar* buf, uint32 len, int32 from)
{
    int32 ret = -1;
    urcp_dev_t* dev=NULL;

    if(from==0) {
        ret = sw_io_nic_tx(gSocketL2, buf, len);
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
                ret = sw_io_nic_tx(gSocketL2, buf, len);
	    }
	}
    }

    return ret;
}
#endif
