#ifndef __UTTFUNCTION_H
#define __UTTFUNCTION_H

#include "webs.h"
#include "utt_conf_define.h"
#include "uttMachine.h"

#define UTT_SPLIT_SIGN_RECORD   ';' 
#define UTT_SPECIAL_SSID    "***###***"
#define UTT_SPLIT_SIGN_STR      ","
#define UTT_SPLIT_SIGN_CHAR     ','
#define UTT_SPLIT_SIGN_RECORDS  ";" 
#define UTT_SPLIT_DOUBLE_RECORDS  ";;" 
#define UTT_SPLIT_SIGN_COLONS    ":" 
#define UTT_SPLIT_SIGN_SPACES_CHAR  ' '
#define UTT_SPLIT_SIGN_SPACES    " " 
#define UTT_SPLIT_SIGN_STATS    "*" 

#define UTT_MAX_WLAN_MAC_FILTER_POSITION   1
#define UTT_MAX_WLAN_MAC_FILTER            50
#define UTT_MAX_WLAN_MULTI_SSID            4
#define UTT_MAX_DNS_FILTER                 100
#define UTT_MAX_DHCP_STATIC_CLIENT         50 
#define UTT_MAX_NAT_MAP                    50   //nat静态映射数
#define UTT_MAX_NAT_RULE                   8    //nat规则数
#define UTT_MAX_NAT_BIND_IP                20   //wan口最多绑定IP数
#define UTT_MAX_STATIC_ROUTE               253
#define UTT_MAX_ARP_BIND                   50
#define UTT_MAX_FIREWALL_POLICY            100 
#define UTT_MAX_USER               	   6
#define UTT_MAX_PPPOE_USER                 50
#define UTT_MAX_TASK_USER                 5
#define UTT_MAX_PPTP_CLIENT                2

#define     Iptables_ADD         'A' 
#define     Iptables_DEL         'D' 
#define     Iptables_INS         'I' 
#define     GLOBAL_ADD           'A'
#define     GROUP_ADD            'A'
#define     GROUP_DEL            'D'
#define     remCtrlName          "admin"

#define reset_priBitmap   memset(priBitmap,0,sizeof(priBitmap))
#define set_priBitmap(x)  ( priBitmap[(x)>>5] |= (1<<((x)&0x1f)) )
#define clr_priBitmap(x)  ( priBitmap[(x)>>5] &= (~(1<<((x)&0x1f))) )

#define MAX_LEN_ERROR_MSG   512

#define     FILTER_TYPE_IP  "1" 
#define     INDEX_ID        0 
#define     INDEX_PRI       4

//#define DBG
#ifdef DBG
#define DBGPRINT(fmt, args...)		printf(fmt, ## args)
#else
#define DBGPRINT(fmt, args...)
#endif

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3
#define HAVEINTERFACE        4


/**/
#define  PASSWD_SPLIT_STR   " "
#define  URCP_SPLIT_STR   ","
/*usb*/
#define WEB_MSG_NO_DISK        0
#define WEB_MSG_NOT_ENOUGH     1
#define WEB_MSG_FAIL           2
/*end usb*/
#define PAGE_TOPOY      "topoy"
#if 0
typedef enum en_init_type{
    INIT_LAN = 0,
    INIT_WAN,
    INIT_INTERNET,
    INIT_DHCP
}speedInitType;
struct st_speed_msg_para{
//        unsigned int initType;
    speedInitType initType;
};
typedef struct st_speed_msg{
        long mtype;
        //    struct st_nvram_msg_para msgPara;
        struct st_speed_msg_para speedPara;
}speed_msg;
#endif
#if (SNMP == FYES)
#define SNMP_NAT_NAME "SNMP"
#define SNMP_PORT1 161
#endif


extern int webErrorIsOk;
extern void wimSetScriptResult(int eid, char *fmt, ...);
extern void Conversion2StdMac(char* mac);
extern char * getLanName();
extern int aspForm(int eid, webs_t wp, int argc, char **argv);
extern int aspGetInstMaxNum(int eid, webs_t wp, int argc, char **argv);
extern void getDays(unsigned char day_bit,char days[]);
extern void setTheErrorMsg(const char* errorMsg);
extern void setTheErrorMsgOnFree(const char* errorMsg);
extern void setTheErrorMsgCat(char* errorMsg);
extern int getTheErrorMsg(int eid, webs_t wp, int argc, char_t **argv);
extern void setTheRightMsg(const char* errorMsg);
extern int getTheRightMsg(int eid, webs_t wp, int argc, char_t **argv);
extern char *strnmatch(const char *known,const char *substr,int n);
extern void Byte2Days(unsigned char byte,char days[]);
extern char * NextRecordStart(char*Record);
extern char* int2ip(unsigned int lip,char * ip);
extern unsigned char days2Byte(char* day);
extern char* getnstr(char* str,int n);
extern char *strstri (const char *known, const char *substr);
extern char * NextRecordStart(char*Record);
extern int bad_mask(char* str, unsigned int addr);
extern unsigned int getOutIp(char *ip);
extern void EditDefaultNatRule(char action);
extern void web_reload(webs_t wp, char_t *url);
extern void ntp_sh(void);
#if (MULTI_LAN == FYES)
extern char * get_netarea(char *lanIfName, char *netarea, char *natIp, int *num);
#else
extern char * get_netarea(char *wanIfName, char *netarea);
#endif
#if 0
extern char * add_wanx(char *name, int wanNo, char * nvram_name);
#endif

extern void web_print_port_list(char* buf, unsigned char member[1], unsigned int size);
extern int getAggrMember(webs_t wp);
extern int char2hex(unsigned char* ch);
extern int str2mac(unsigned char strMac[12], unsigned char stdMac[6]);
extern int aspSignalRemoteAuth(int eid, webs_t wp, int argc, char **argv);
extern void webs_write_port_list(webs_t wp, unsigned int mask);
extern void wimDownloadFile(webs_t wp, char * fileName, char *fileContent, int fileLen);
extern int uttIsNum(const char *str);
extern void deldir(char* path);
extern void web_print_port_mask(char* buf, unsigned int mask);
extern int test_32_bit(unsigned long value);
extern int saveFile2USB(char* file_name, char * src_path, char* dst_path);
extern int aspRemoteFileGet(webs_t wp, unsigned int file_id);
extern void mergeMulSpaEnter(char* textarea);
extern int strCountChar(char* str, char ch);
extern int get_ip(char *ifname, unsigned int *if_addr);
extern int get_mask(char *ifname, unsigned int *if_net);
extern int get_mac(char *ifname, unsigned char *if_hw);

typedef struct _strTime_t{
    int day;
    int hour;
    int min;
    int sec;
}strTime_t;
#if (VPN_ITEM_CONTROL == FYES)
extern int checkVpnFreeItems();
#endif

#if (MAC_COMPANY_NAME == FYES)
typedef struct _macOut_t{
    unsigned char oui[3];
    const char vendor[16];
}macOui_t;
#endif
#endif

