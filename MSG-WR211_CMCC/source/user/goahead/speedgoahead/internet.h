

/*
 * added by brwang 
 */
#include "uttMachine.h"
//#define DBG
#ifdef DBG
#define DBGPRINT(fmt, args...)		printf(fmt, ## args)
#else
#define DBGPRINT(fmt, args...)
#endif

#define T(s)                            s
#define PRE_DNS_INPUT_CHAIN        "pre_dns_input_chain"
#define DNS_INPUT_CHAIN        "dns_input_chain"
#define DNS_PRE_INPUT_CHAIN        "dns_pre_input_chain"
#define PRE_DNS_FORWARD_CHAIN        "pre_dns_forward_chain"
#define DNS_FORWARD_CHAIN       "dns_forward_chain"
#define DNS_PRE_FORWARD_CHAIN       "dns_pre_forward_chain"
#define DEFENSE_FILTER_CHAIN     "defense"
#define PRE_DROP_CHAIN         "preDrop"   /*高优先级drop链表。符合此链表的被优先丢弃，不进入系统*/

#define UPNP_CHAIN_WAN	"upnp_chain_wan"

#define DMZ_CHAIN                       "DMZ"
#define DMZ_CHAIN1			"DMZ1"
#define PORT_FORWARD_CHAIN      "port_forward"
#define PORT_FORWARD_CHAIN1     "port_forward1"
#define PORT_FORWARD_RULE_CHAIN "port_forward_rule"
#define PORT_FORWARD_WAN_CHAIN  "port_forward_wan"
#define CONNMARK_WAN_CHAIN      "connmark_wan"
#define UPNP_CHAIN_WAN	        "upnp_chain_wan"
#define GLOBAL_CHAIN		"global"

#define BLK_JAVA                0x01
#define BLK_ACTIVE              0x02
#define BLK_COOKIE              0x04
#define BLK_PROXY               0x08
#define PROTO_UNKNOWN   0
#define PROTO_TCP               1
#define PROTO_UDP               2
#define PROTO_TCP_UDP   3
#define PROTO_ICMP              4
#define PROTO_NONE              5
#define UTT_SPLIT_SIGN_RECORD   ';'
#define     Iptables_ADD         'A' 
#define     Iptables_DEL         'D' 
#define     Iptables_INS         'I' 
#define     Iptables_FLUSH	 'F' 
#define     Iptables_SPECIAL	 'S' 
#define     GLOBAL_ADD           'A'
#define     GROUP_ADD            'A'
#define     GROUP_DEL            'D'
#define     remCtrlName          "admin"
#define     TYPE_EASYIP        "1" 
#define     TYPE_ONE2ONE       "2" 
#define     INDEX_RULEID    0 
#define     ROUTE_ADD         "add" 
#define     ROUTE_DEL         "del" 

#define POLICY_MAX_LENGTH 200
#define NAME_MAX_LENGTH 18
#define UTT_SPLIT_SIGN_RECORDS  ";" 
#define PACKET_SIZE     32      
#define COEFFICIENT     *(1024/(PACKET_SIZE*8)) 

#if 0
struct st_nvram_msg_para{
    char* srcAddr;
    int offset;
    int len;
    int crc;
    int blockNum;
//    char buf[0x1f00];
};

typedef enum en_init_type{
    INIT_LAN = 0,
    INIT_WAN,
    INIT_INTERNET,
    INIT_DHCP
}speedInitType;

struct st_speed_msg_para{
    speedInitType initType;
};

typedef struct st_speed_msg{
    long mtype;
//    struct st_nvram_msg_para msgPara;
    struct st_speed_msg_para speedPara;
}speed_msg;

#endif
typedef char     char_t;
extern int doSystem(char *fmt, ...);
extern int getIfIp(char *ifname, char *if_addr);
//extern char* getLanIfName(void);
extern void Conversion2StdMac(char* mac);
#if 0
extern void restart8021XDaemon(int nvram);
#endif
extern void restart8021XDaemon(void);
#if (WIRELESS_5G == FYES)
extern void restart8021XDaemon_5g(void);
#endif
extern int deleteNthValueMulti(int index[], int count, char *value, char delimit);
extern int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
extern char* int2ip(unsigned int lip,char * ip);
extern unsigned int ip2int(char *ip);
extern unsigned int getInNet(char *netarea ,char *end);
extern void getDays(unsigned char day_bit,char days[]);
extern char* getWanIfNamePPP(int num);
extern int getOnePortOnly(void);
#if (MULTI_LAN == FYES)
extern char * get_netarea(char *lanIfName, char *netarea, char *natIp, int *num);
#else
extern char * get_netarea(char *wanIfName, char *netarea);
#endif
extern unsigned int getOutIp(char *ip);
extern char * NextRecordStart(char*Record);
extern int getIfNetmask(char *ifname, char *if_net);
extern unsigned char days2Byte(char* day);
#if 0
extern char * add_wanx(const char *name, int wanNo, char * nvram_name);
#endif
extern int getWanDhcpDefaultRoute(int wannum, char* ipaddr);
