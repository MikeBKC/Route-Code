#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdio.h>
#include <sys/time.h>
#include <linux/autoconf.h>
#include <sys/sysinfo.h>
#include <linux/switch/types.h>
#include <mib.h>
#include <uttMachine.h>

#define URCP_AP
#define MAX_MAC_TAB_SIZE 8192

#if defined(UTT_NV_WX1000) || defined(UTT_NV_WX100) || defined(UTT_WX_2000) || defined(UTT_NV_WX20S) || defined(UTT_NV_D908W)

#define URCP_AC         
#define URCP_WEB
#undef  URCP_AP
#undef  URCP_TFTP_OPEN

#define URCP_CMD_HELLO_GET
#define URCP_CMD_HELLO_GET_REPLY
#define URCP_CMD_SERIAL_NO_GET_REPLY
#define URCP_CMD_DEV_NAME_GET_REPLY
#define	URCP_CMD_AP_DHCP_EN_GET_REPLY
#define	URCP_CMD_AP_ROAMING_TH_GET_REPLY
#define URCP_CMD_SW_VERSION_GET_REPLY
#define URCP_CMD_HW_VERSION_GET_REPLY
#define URCP_CMD_PRODUCT_MODEL_GET_REPLY
#define URCP_CMD_PASSWD_AUTH_GET_REPLY
#define URCP_CMD_DISCOVER_GET
#define URCP_CMD_DISCOVER_GET_REPLY
#define URCP_CMD_AP_AFFIRM_GET_REPLY
#define URCP_CMD_AP_AUTH_REQ_GET
#define URCP_CMD_AP_AUTH_PRE_GET
#define URCP_CMD_AP_UPDATE_PROFILE_GET
#define URCP_CMD_AP_UPDATE_FIRMWARE_GET
#define URCP_CMD_AP_SSID_GET_REPLY
#define URCP_CMD_AP_CHANNEL_GET_REPLY
#define URCP_CMD_CURRENT_CLIENTS_GET_REPLY
#define URCP_CMD_MAX_CLIENTS_GET_REPLY
#define URCP_CMD_AP_BASIC_CONFIG_GET_REPLY
#define URCP_CMD_AP_BASIC_CONFIG_SET_GET_REPLY
#define URCP_CMD_AP_WORK_MODE_GET_REPLY
#define URCP_CMD_NETMASK_GET_REPLY
#define URCP_CMD_GATEWAY_GET_REPLY
#define URCP_CMD_AP_USER_NAME_GET_REPLY
#define URCP_CMD_AP_MAX_CLIENT_GET_REPLY
#define URCP_CMD_AP_EFFECT_CONF_GET_REPLY
#define URCP_CMD_PASSWORD_CONFIG_SET_REPLY
#define URCP_CMD_GET_CONFIG_GET_REPLY
#define URCP_CMD_GET_IMAGE_GET_REPLY
#define URCP_CMD_AIR_PROBE_GET_REPLY
#define URCP_CMD_WIRELESS_MODE_GET_REPLY
#define URCP_CMD_SET_IMAGE_SET_REPEY
#define URCP_CMD_WIRELESS_CLT_GET_REPLY
#define URCP_CMD_SET_CONFIG_TEMPLATE_SET_REPEY
#define URCP_CMD_CONFIG_TEMPLATE_SET_REPLY
#define URCP_CMD_AP_JOIN_SZ_SET_REPLY
#define URCP_CMD_AP_SZ_NAME_GET_REPLY
#define URCP_CMD_AP_SZ_NUM_GET_REPLY
#define URCP_CMD_AP_MAC_FILTER_SET_REPLY
#define URCP_CMD_WL_MAC_FILTER_SET_REPLY
#define URCP_CMD_CHANGE_CONF_GET
#define URCP_CMD_FAT_TO_FIT_GET
#define URCP_CMD_SET_AC_MODE_REPLY
#define URCP_CMD_CHANGE_CHANNEL_REPLY

#if AP_WL_POWER_AUTO
#define URCP_CMD_AP_RA0_MAC_GET_REPLY
#define URCP_CMD_AP_RA0_SSID_GET_REPLY
#define URCP_TRAP_AP_RA0_MAC
#define URCP_CMD_AP_WL_POWER_GET_REPLY
#define URCP_TRAP_AP_WL_POWER
#define URCP_CMD_AP_WL_POWER_SET_REPLY
#define URCP_CMD_AP_SITE_SURVEY_SET_REPLY
#endif

#if AP_LOAD_BALANCE
#define URCP_TRAP_CMD_UPPER_LIMIT
#endif
#define URCP_TRAP_CMD_UPPER_LIMIT_SYSLOG
#define URCP_TRAP_DISCOVER
#define URCP_TRAP_PRODUCT_MODEL
#define URCP_TRAP_SERIAL_NO
#define URCP_TRAP_DEV_NAME
#define URCP_TRAP_SW_VERSION
#define URCP_TRAP_HW_VERSION
#define URCP_TRAP_AP_SSID
#define URCP_TRAP_AP_SSID_5G
#define URCP_TRAP_AP_CHANNEL
#define URCP_TRAP_AP_CHANNEL_5G
#define URCP_TRAP_CURRENT_CLIENTS
#define URCP_TRAP_CURRENT_CLIENTS_5G
#define URCP_TRAP_WIRELESS_MODE
#define URCP_TRAP_WIRELESS_MODE_5G
#define URCP_TRAP_AP_DHCP_EN
#define URCP_TRAP_AP_SZ_NUM
#define URCP_TRAP_CMD_STA_INFO_SYSLOG
#define URCP_TRAP_CMD_STA_INFO
#define URCP_TRAP_CMD_STA_INFO_AUTO
#define URCP_TRAP_CMD_SZONE_UPDATA_ACK
#if(HISTORY_FLOW_INFO == FYES)
#define URCP_TRAP_CMD_AP_STAT
#endif
#define URCP_TRAP_AP_SYS_INFO
#define URCP_TRAP_AP_SYS_INFO_5G
#if(FEATURE_AP_MAC_FILTER == FYES)
#define URCP_TRAP_AP_MF_NAME
#endif

//#elif (defined(UTT_NV_1800N) || defined(UTT_NV_510V2))
#elif defined(FEATURE_AP)

#define URCP_AP
#undef  URCP_AC
#undef  URCP_WEB
#define URCP_TFTP_OPEN

#define URCP_CMD_SERIAL_NO_GET
#define URCP_CMD_DEV_NAME_GET
#define URCP_CMD_DEV_NAME_SET
#define URCP_CMD_AP_DHCP_EN_GET
#define URCP_CMD_AP_ROAMING_TH_GET
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
#define URCP_CMD_AP_ROAMING_TH_5G_GET
#endif
#define URCP_CMD_AP_DHCP_EN_SET
#define URCP_CMD_SW_VERSION_GET
#define URCP_CMD_HW_VERSION_GET
#define URCP_CMD_PRODUCT_MODEL_GET
#define URCP_CMD_REBOOT_CONFIG_SET
#define URCP_CMD_RESET_CONFIG_SET
#define URCP_CMD_PASSWD_AUTH_GET
#define URCP_CMD_DISCOVER_GET
#define URCP_CMD_AP_AFFIRM_GET
#define URCP_CMD_DISCOVER_GET_REPLY
#define URCP_CMD_AP_AUTH_REQ_GET_REPLY
#define URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
#define URCP_CMD_AP_UPDATE_FIRMWARE_GET_REPLY
#define URCP_CMD_AP_SSID_GET
#define URCP_CMD_AP_CHANNEL_GET
#define URCP_CMD_AP_CHANNEL_SET
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
#define URCP_CMD_AP_CHANNEL_SET_5G
#endif
#define URCP_CMD_CURRENT_CLIENTS_GET
#define URCP_CMD_AP_BASIC_CONFIG_GET
#define URCP_CMD_AP_WORK_MODE_GET
#define URCP_CMD_AP_USER_NAME_GET
#define URCP_CMD_AP_MAX_CLIENT_GET
#define URCP_CMD_AP_EFFECT_CONF_GET
#define URCP_CMD_AP_BASIC_CONFIG_SET
#define URCP_CMD_AP_WORK_MODE_SET
#define URCP_CMD_IP_ADDR_SET
#define URCP_CMD_NETMASK_SET
#define URCP_CMD_NETMASK_GET
#define URCP_CMD_GATEWAY_GET
#define URCP_CMD_GATEWAY_SET
#define URCP_CMD_AP_PASSWORD_SET
#define URCP_CMD_AP_MAX_CLIENT_SET
#define URCP_CMD_AP_ROAMING_TH_SET
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
#define URCP_CMD_AP_ROAMING_TH_5G_SET
#endif
#define URCP_CMD_AP_EFFECT_CONF_SET
#define URCP_CMD_PASSWORD_CONFIG_SET
#define URCP_CMD_GET_IMAGE_GET_REPLY
#define URCP_CMD_AIR_PROBE_GET
#define URCP_CMD_WIRELESS_MODE_GET
#define URCP_CMD_WIRELESS_CLT_GET
#define URCP_CMD_WL_MAC_FILTER_SET
#define URCP_CMD_CONFIG_TEMPLATE_SET
#define URCP_CMD_SZ_STATUS_SET
#define URCP_CMD_SZ_NAME_SET
#define URCP_CMD_SZ_SSID_SET
#define URCP_CMD_SZ_SSID_BC_SET
#define URCP_CMD_SZ_SSID_ISOLATE_SET
#define URCP_CMD_SZ_VID_SET
#define URCP_CMD_SZ_AUTH_SET
#define URCP_CMD_SZ_WEP_SET
#define URCP_CMD_SZ_WPA_SET
#define URCP_CMD_SZ_PSK_SET
#define URCP_CMD_SZ_DHCP_SET
#define URCP_CMD_SZ_START_IP_SET
#define URCP_CMD_SZ_END_IP_SET
#define URCP_CMD_SZ_GW_SET
#define URCP_CMD_SZ_SUBMASK_SET
#define URCP_CMD_AP_SZ_NAME_GET
#define URCP_CMD_AP_SZ_NUM_GET
#define URCP_CMD_AC_CHANGE_CONF_SET
#define URCP_CMD_AC_MODE_SET
#define URCP_CMD_AP_SYS_INFO_GET

#if (SZ_SHARE_LIMIT_AP == FYES)
#define URCP_CMD_SZ_SHARE_LIMIT
#endif

#if(FIT_AP_MAC_FILTER == FYES)
#define URCP_CMD_MAC_FILTER_NUM_SET
#define URCP_CMD_MAC_FILTER_SET
#define URCP_CMD_AP_MF_NAME_GET
#define URCP_CMD_AP_MF_NUM 4
#endif

#if AP_WL_POWER_AUTO
#define URCP_CMD_AP_RA0_MAC_GET
#define URCP_CMD_AP_RA0_SSID_GET
#define URCP_CMD_AP_WL_POWER_GET
#define URCP_CMD_AP_WL_POWER_SET
#define URCP_CMD_AP_SITE_SURVEY_SET
#endif
#define URCP_CMD_SZONE_UPDATA_GET


#define URCP_CMD_AP_LB_MAX_CLIENT_SET
#define URCP_CMD_AP_DISCONNECT_STA_SET
#if(HISTORY_FLOW_INFO == FYES)
#define URCP_TRAP_CMD_AP_STAT
#endif

#define URCP_CMD_AP_ADD_MAC_BLACK
#else
#define URCP_CMD_SERIAL_NO_GET
#define URCP_CMD_SW_VERSION_GET
#define URCP_CMD_HW_VERSION_GET
#define URCP_CMD_AP_CHANNEL_GET
#define URCP_CMD_DISCOVER_GET
#define URCP_CMD_DISCOVER_GET_REPLY
#define URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
#define URCP_CMD_AP_AUTH_REQ_GET_REPLY
#define URCP_CMD_AP_UPDATE_FIRMWARE_GET_REPLY
#define URCP_CMD_PRODUCT_MODEL_GET
#define URCP_CMD_WIRELESS_MODE_GET
#define URCP_CMD_AP_SSID_GET
#if 0
#define URCP_CMD_AP_AUTH_REQ_GET
#define URCP_CMD_AP_AUTH_REQ_GET_REPLY
#define URCP_CMD_AP_UPDATE_PROFILE_GET
#define URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
#define URCP_CMD_DEV_NAME_GET
#define URCP_CMD_HELLO_GET
#define URCP_CMD_HELLO_GET_REPLY
#define URCP_CMD_SERIAL_NO_GET_REPLY
#define URCP_CMD_DEV_NAME_GET_REPLY
#define URCP_CMD_DEV_NAME_SET
#define URCP_CMD_UP_PORTS_GET
#define URCP_CMD_UP_PORTS_GET_REPLY
#define URCP_CMD_SW_VERSION_GET_REPLY
#define URCP_CMD_PRODUCT_MODEL_GET_REPLY
#define URCP_CMD_MIRROR_CONFIG_GET
#define URCP_CMD_MIRROR_CONFIG_GET_REPLY
#define URCP_CMD_MIRROR_CONFIG_SET
#define URCP_CMD_MIRROR_CONFIG_SET_REPLY
#define URCP_CMD_PASSWD_AUTH_GET
#define URCP_CMD_PASSWD_AUTH_GET_REPLY
#define URCP_CMD_AGGR_CONFIG_GET
#define URCP_CMD_AGGR_CONFIG_GET_REPLY
#define URCP_CMD_AGGR_CONFIG_SET
#define URCP_CMD_AGGR_CONFIG_SET_REPLY
#define URCP_CMD_RSTP_LACP_CONFIG_GET
#define URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
#define URCP_CMD_VLAN_CONFIG_GET
#define URCP_CMD_VLAN_CONFIG_GET_REPLY
#define URCP_CMD_VLAN_CONFIG_SET
#define URCP_CMD_VLAN_CONFIG_SET_REPLY
#define URCP_CMD_QVLAN_PCONF_CONFIG_GET
#define URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
#define URCP_CMD_PORT_CONFIG_GET
#define URCP_CMD_PORT_CONFIG_GET_REPLY
#define URCP_CMD_PORT_CONFIG_SET
#define URCP_CMD_PORT_CONFIG_SET_REPLY
#define URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET
#define URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
#define URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET
#define URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET_REPLY
#define URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET
#define URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
#define URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET
#define URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET_REPLY
#define URCP_CMD_REBOOT_CONFIG_SET
#define URCP_CMD_RESET_CONFIG_SET
#define URCP_CMD_SAFEBIND_CONFI_SET
#define URCP_CMD_SAFEBIND_CONFI_SET_REPLY
#define URCP_CMD_SYS_CONF_CONFIG_GET
#define URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
#define URCP_CMD_SYS_CONF_CONFIG_SET
#define URCP_CMD_SYS_CONF_CONFIG_SET_REPLY
#define URCP_CMD_IP_FILTER_CONFIG_GET
#define URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
#define URCP_CMD_SYS_INFO_CONFIG_GET
#define URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
#define URCP_CMD_PASSWORD_CONFIG_SET
#define URCP_CMD_PASSWORD_CONFIG_SET_REPLY
#define URCP_CMD_GET_CONFIG_GET_REPLY
#define URCP_CMD_MIRROR_CAN_SET
#define URCP_CMD_AP_AFFIRM_GET
#define URCP_CMD_AP_AFFIRM_GET_REPLY
#define URCP_CMD_AP_AUTH_SET
#define URCP_CMD_AP_AUTH_PRE_GET
#define URCP_CMD_AP_UPDATE_FIRMWARE_GET
#define URCP_CMD_AP_UPDATE_FIRMWARE_GET_REPLY
#define URCP_CMD_AP_SSID_GET
#define URCP_CMD_AP_SSID_GET_REPLY
#define URCP_CMD_AP_CHANNEL_SET
#define URCP_CMD_AP_CHANNEL_GET_REPLY
#define URCP_CMD_CURRENT_CLIENTS_GET
#define URCP_CMD_CURRENT_CLIENTS_GET_REPLY
#define URCP_CMD_MAX_CLIENTS_GET
#define URCP_CMD_MAX_CLIENTS_GET_REPLY
#define URCP_CMD_AP_BASIC_CONFIG_GET_REPLY
#define URCP_CMD_AP_BASIC_CONFIG_SET_GET_REPLY
#define URCP_CMD_AP_WORK_MODE_GET_REPLY
#define URCP_CMD_IP_ADDR_SET
#define URCP_CMD_NETMASK_SET
#define URCP_CMD_NETMASK_GET_REPLY
#define URCP_CMD_AP_USER_NAME_GET_REPLY
#define URCP_CMD_AP_MAX_CLIENT_GET_REPLY
#define URCP_CMD_AP_EFFECT_CONF_GET_REPLY
#define URCP_CMD_AP_BASIC_CONFIG_GET
#define URCP_CMD_AP_WORK_MODE_GET
#define URCP_CMD_AP_USER_NAME_GET
#define URCP_CMD_AP_MAX_CLIENT_GET
#define URCP_CMD_AP_EFFECT_CONF_GET
#define URCP_CMD_AP_BASIC_CONFIG_SET
#define URCP_CMD_AP_WORK_MODE_SET
#define URCP_CMD_AP_PASSWORD_SET
#define URCP_CMD_AP_MAX_CLIENT_SET
#define URCP_CMD_AP_ROAMING_TH_SET
#define URCP_CMD_AP_EFFECT_CONF_SET
#define URCP_CMD_SET_IMAGE_SET_REPEY
#define URCP_CMD_WIRELESS_CLT_GET
#define URCP_CMD_WIRELESS_CLT_GET_REPLY
#define URCP_CMD_SET_CONFIG_TEMPLATE_SET_REPEY
#define URCP_CMD_CONFIG_TEMPLATE_SET_REPLY
#define URCP_CMD_CONFIG_TEMPLATE_SET
#define URCP_CMD_AP_MAC_FILTER_SET_REPLY
#define URCP_CMD_MAC_FILTER_NUM_SET
#define URCP_CMD_MAC_FILTER_SET
#define URCP_CMD_AP_JOIN_SZ_SET_REPLY
#define URCP_CMD_AP_SZ_NAME_GET_REPLY
#define URCP_CMD_AP_SZ_NUM_GET_REPLY
#define URCP_CMD_SZ_STATUS_SET
#define URCP_CMD_SZ_NAME_SET
#define URCP_CMD_SZ_SSID_SET
#define URCP_CMD_SZ_SSID_BC_SET
#define URCP_CMD_SZ_SSID_ISOLATE_SET
#define URCP_CMD_SZ_VID_SET
#define URCP_CMD_SZ_AUTH_SET
#define URCP_CMD_SZ_WEP_SET
#define URCP_CMD_SZ_WPA_SET
#define URCP_CMD_SZ_PSK_SET
#define URCP_CMD_SZ_DHCP_SET
#define URCP_CMD_SZ_START_IP_SET
#define URCP_CMD_SZ_END_IP_SET
#define URCP_CMD_SZ_GW_SET
#define URCP_CMD_SZ_SUBMASK_SET
#define URCP_CMD_AP_SZ_NAME_GET
#define URCP_CMD_AP_SZ_NUM_GET
#define URCP_CMD_GATEWAY_GET_REPLY
#define URCP_CMD_GATEWAY_GET
#define URCP_CMD_GATEWAY_SET
#endif
#endif

#if 0
typedef struct _mtimer_t {
    struct timeval  timeout;
    struct timeval  now;
} mtimer_t;
#define MTIMER_START(timer,msec) { \
        gettimeofday(&((timer)->timeout),NULL); \
        (timer)->timeout.tv_usec+=msec*1000; \
        if ((timer)->timeout.tv_usec>=1000000) { (timer)->timeout.tv_sec+=(timer)->timeout.tv_usec/1000000; (timer)->timeout.tv_usec%=1000000; } \
        }

#define MTIMER_TIMEOUT(timer) (gettimeofday(&((timer)->now),NULL)==0 && timercmp(&((timer)->now),&((timer)->timeout),>))
#else
typedef struct _mtimer_t {
    struct sysinfo timeout;
    struct sysinfo now;
    void (*timer_handler)(void * arg);
} mtimer_t;
#define MTIMER_START(timer,msec) { \
        sysinfo(&((timer)->timeout)); \
        (timer)->timeout.uptime += ((msec)/1000); \
        }

#define MTIMER_TIMEOUT(timer) (sysinfo(&((timer)->now))==0 && ((timer)->now.uptime >= (timer)->timeout.uptime))

#endif
/*
 *
 *common.c中外部函数
 *
 */
#ifdef URCP_AP
#if 0
typedef struct _RT_STA_INFO {
    uchar StaMac[6];
} RT_STA_INFO;
typedef struct _RT_SSID_STA_INFO_ENTRY {
    char Ssid[32];
    uchar Bssid[6];
	uint16 StaNum;
    RT_STA_INFO StaInfo[32];
} RT_SSID_STA_INFO_ENTRY;
typedef struct _RT_SSID_STA_INFO_TABLE {
	unsigned long Num;
	RT_SSID_STA_INFO_ENTRY Entry[4];
} RT_SSID_STA_INFO_TABLE;
#endif
#ifndef MAC_LEN
#define MAC_LEN	    6u
#endif
#if defined UTT_WA_500N || defined UTT_WA_2000N || defined UTT_AP_801 || defined UTT_NV_1900N
#define HW_UTT_MODEL	"V1.0"
#else
#define HW_UTT_MODEL	"V2.0"
#endif
typedef enum{
    URCP_STATUS_WAIT=1,
    URCP_STATUS_OK,
    URCP_STATUS_TIME_OUT,
    URCP_STATUS_AUTH_ERROR, /*密码错误*/
    URCP_STATUS_FAIL,         /*操作失败 */
    URCP_STATUS_FOUND_NONE         /*操作失败 */
}urcp_status_t;

/* Tag Priority */
typedef uint32 vtss_tagprio_t;   /* 0-7 */

/* Tag Control Information (according to IEEE 802.1Q) */
typedef struct _vtss_tci_t {
    uint32     vid;     /* VLAN ID */
    uchar           cfi;     /* Canonical Format Indicator */
    vtss_tagprio_t tagprio; /* Tag priority */
} vtss_tci_t;
typedef struct _system_frame_header_t{
    uint32                length;         /* Frame length excluding CRC */
    uint32                source_port_no; /* Source port number */
    uchar                 arrived_tagged; /* TRUE is frame was tagged */
    vtss_tci_t            tag;            /* VLAN tag from frame or VLAN port setup */
	uchar*				  raw;				/*指向原始二层数据包第一个字节*/
	uchar*				  data;				/*指向联动数据包头，即0x8899后面的第一个字节*/
#if (CLIENT_HOST_NAME == FYES)
	uint16 packet_type;		    /*包类型，URCP包或者IP包*/
#endif
	int32 				  fromL3;				/*指向联动数据包头，即0x8899后面的第一个字节*/
}system_frame_header_t;
/******************
 *type: packet ID. 8100 or 8899
 *value: proto info
 *protoid_len: packet ID or lenth
 * ***************/
typedef struct _tagInfo_t {
    uint16  type;
    uint16  value;
	uint16  proid_len;
}tagInfo_t;

/*Fit AP config template*/
typedef struct _urcp_ap_conf_temp_conf_t
{
    char tempName[16];	/* 模版名称 */
	char suitType[16];  /* 适用型号 */
	char firmware[16];  /* 硬件版本 */
    uint32 wireless;	/* 启用无线功能 */
    uint32 sgi;		/* short guard interval */
	uint32 cltIsolate;   /* 无线客户端隔离 */
    uint32 preamble;	/* short preamble */
    uint32 wmm;		/* 启用WMM */
    uint32 rate;	/* 无线速率 */
    uint32 bw;		/* 频道带宽 */
    uint32 channel;	/* 信道 */
    uint32 power;	/* 无线传输功率 */
    uint32 manPower;	/* 无线传输功率 手动设置 */
    uint32 mode;	/* 无线模式 */
	uint32 wlFre;   /*无线频段*/
	uint32 beaconPeriod;   /**/
    uint32 VChanWidth;/*VHT BW*/
}urcp_ap_conf_temp_conf_t;

#define USB_LOG_PATH        "syslog/"  /*u盘中本机日志目录*/
#define TFTP_LOG_ROOT      "/tmp/urcp/logs/"/*日志文件存放目录*/
#define RAM_LOG_PATH       "/tmp/syslog/"

#define USB_CONFIG_PATH     "configs/" /*u盘中联动获得配置文件的目录*/
#define RAM_CONFIG_PATH    "/tmp/urcp/configs/"

#define MAX_DEV_NAME_LEN          24u  /*设备名*/
#define MAX_PASSWORD_LEN          18u
#define DEV_TYPE_SIZE          12u  /*设备型号描述*/
#define SW_VERSION_SIZE        32u  /*软件版本信息*/
#define AP_IF_NAME      "br0"
#define AP_ETH_NAME	"br0"
/*联动发现设备结构*/
typedef struct _urcp_dev{
    int32    used;
    int32    tb_index;       /*在表中的索引*/
    int32    tb_next;       /*在表中的索引*/
    uint32   ip;
    uint32   serial_no;        /*serial number*/
    uint32   port_num;        /*port number*/
    char     urcp_version;     /* 联动版本*/
    unsigned char    mac[MAC_LEN];
    char     dev_name[MAX_DEV_NAME_LEN];         /*device name*/
    char     password[MAX_PASSWORD_LEN];
    char     devic_type[DEV_TYPE_SIZE];      /*型号*/

    unsigned char    managed_enable;                    /*是否可被管理*/
    unsigned char    link_port;                        /*端口*/
    unsigned char    up_ports;                       /*活动端口数*/
    unsigned char    alarm_level;                    /*报警级别*/

    unsigned char    sw_version[SW_VERSION_SIZE];     /*软件版本信息*/
	uint32			vid;		/*ap所属vlan id*/

}urcp_dev_t;

#define STA_MAX_NUM 32
typedef struct ssid_t {
    char ssid[UTT_SSID_LENTH+1];
    char szone[UTT_SZONE_LENTH+1];
    uchar mac[MAC_LEN];
}ssid_t;

typedef struct _sta_info_t {
	unsigned char sta_mac[MAC_LEN];
}sta_info_t;
typedef struct _wl_ssid_t {
    char ssid[UTT_SSID_LENTH+1];
    char szone[UTT_SZONE_LENTH+1];
    uchar mac[MAC_LEN];
    unsigned short sta_num;
	sta_info_t sta_info[STA_MAX_NUM];
}wl_ssid_t;

extern const char *str_macaddr(const unsigned char *macaddr);
extern int mac_cmp(unsigned char *mac1, unsigned char* mac2);
extern uint32 getSystemSn(void); 
extern int get_ip(char *ifname, unsigned int *if_addr);
extern int get_switch_if_name(char * if_name);
extern int get_mac(char *ifname, unsigned char *if_hw);
extern int get_mask(char *ifname, uint32 *if_net);
extern int sw_frame_header_pull(char *raw, system_frame_header_t * sys_header);
extern int create_raw_socket(int eth_proto);
extern int32 h2_send_frame(uchar*buf, uint32 len);
extern int32 h2_send_frame_l2l3(uchar*buf, uint32 len);
extern void apRandomSleep(void);

extern void get_sysinfo_machine_model(char model[64]);
extern void get_sysinfo_hw_model(char hwmodel[64]);
extern int get_sysinfo_sw_version(char sw_version[64]);
extern char *strstri (const char *known, const char *substr);
extern void Conversion2StdMac(char* mac);
extern int set_ip(char *ifname, uint32 sysIp);
extern int set_mask(char *ifname, uint32 mask);
extern void set_gw(char *gwaddr);

extern int getSystemMemory(int *s_mem);
extern void getSystemCpu(int *s_cpu);
extern int get_ap_rf_rate(uint16 *rate, char *rf_ra);
extern int get_ap_power(uint16 *power, char *rf_ra);
extern int get_ap_beacon(uint16 *bea, char *rf_ra);
extern int get_ap_RTX_info(uint32 *p_tx_packet, uint32 *p_tx_bytes, uint32 *p_rx_packet, uint32 * p_rx_bytes, char *rf_ra);
#endif /*URCP_AP*/


#define TFTP_IMAGE_ROOT    "/tmp/urcp/images/"
#define TFTP_CONF_TEMP_ROOT "/tmp/urcp/templates"
#define TFTP_AP_CONFIG_ROOT    "/tmp/urcp/ap_configs/"
#define USB_AP_CONFIG_ROOT    "/ap_configs/"


/*usb*/
#define WEB_MSG_NO_DISK        0
#define WEB_MSG_NOT_ENOUGH     1
#define WEB_MSG_FAIL           2
/*end usb*/
extern int32 isValidMac(uchar*mac);

extern unsigned char getAlivePort(void);

extern int32 getFileSize(char *path, FILE* fd);
extern void create_tmp_file(const char * preStr, char *fName);
extern unsigned short countOne(unsigned int num);
extern uint16 chk_sum(uint16 *buffer, uint32 size);
extern int get_one_num(uint32 val);
extern int str2mac(unsigned char strMac[12], unsigned char stdMac[6]);
extern int get_verinfo_from_file(char* file, int* hwVer, int* swVer);
extern void get_verinfo_from_local(unsigned int* hwVer, unsigned int* swVer);
int hwverinfo_to_str(unsigned int hwver, char* hwver_buf);
int swverinfo_to_str(unsigned int swver, char* swver_buf);

extern int saveFile2USB(char* file_name, char * src_path, char* dst_path);

#ifdef URCP_AP
extern void get_ip_from_dhcp_server(uint32 ac_ip);
#endif
#define SWVER_1 8
#define SWVER_2 8
#define SWVER_3 8
#define HWVER_1 4
#define HWVER_2 4

typedef enum {
	AP_STA_DISCONNECT = 0u,

	AP_STA_CONNECT = 1u,
} sta_status;

#ifdef CONFIG_WLAN_LAN_ISOLATION
extern int set_br_isolation(int enable);
#endif


#endif

