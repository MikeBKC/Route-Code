#ifndef _URCP_CMD_H_
#define _URCP_CMD_H_
#include "common.h"
typedef enum {
    CMD_START = 0,
    CMD_HELLO = 1, 
    CMD_SERIAL_NO = 2, 
    CMD_DEV_NAME = 3,                  /*设备名*/
    CMD_UP_PORTS = 4,                  /*连接端口*/
    CMD_SW_VERSION = 5 ,                /*连接端口*/
    CMD_PRODUCT_MODEL = 6,             /*开启端口*/
    CMD_MIRROR_CONFIG = 7,             /*开启端口*/
    CMD_PASSWD_AUTH = 8,               /*开启端口*/
    CMD_AGGR_CONFIG = 9 ,               /*汇聚*/ 
    CMD_RSTP_LACP_CONFIG = 10,          /*rstp lacp*/
    CMD_VLAN_CONFIG = 11,               /*vlan*/ 
    CMD_QVLAN_PCONF_CONFIG = 12,        /*tag vlan端口设置*/ 
    CMD_PORT_CONFIG = 13,               /*端口设置*/
    CMD_QOS_RATE_LIMIT_CONFIG = 14,     /*速率限制*/
    CMD_STORM_RATE_LIMIT_CONFIG = 15,   /*风暴抑制*/
    CMD_REBOOT_CONFIG = 16,             /*重启*/
    CMD_RESET_CONFIG = 17,              /*恢复出厂配置*/
    CMD_SAFEBIND_CONFIG = 18 ,           /*安全绑定*/
    CMD_SYS_CONF_CONFIG = 19,           /*系统设置*/
    CMD_IP_FILTER_CONFIG = 20,          /*ip过滤*/
    CMD_SYS_INFO_CONFIG = 21,           /*系统信息*/
    CMD_PASSWORD_CONFIG = 22,           /*密码设置*/
    CMD_MIRROR_CAN = 23,                /*取消镜像设置*/
    /**************************** AC - AP *****************************/
    CMD_DISCOVER = 24,                  /*AP&AC发现*/
    CMD_AP_AUTH = 25,                   /*AC向AP授权*/
    CMD_AP_AUTH_REQ = 26,               /*AP向AC请求授权*/
    CMD_AP_UPDATE_FIRMWARE = 27,        /*AP软件更新请求*/
    CMD_AP_UPDATE_PROFILE = 28,         /*AP配置更新请求*/
    CMD_AP_SSID = 29,                   /*AP各ssid信息*/
    CMD_AP_CHANNEL = 30,                /*AP信道*/
    CMD_CURRENT_CLIENTS = 31,                   /*当前客户端数*/
    CMD_AP_MAX_CLIENT = 32,                   /*最大客户端数*/
    CMD_AP_WORK_MODE = 33u,		/* AP工作模式 */
    CMD_IP_ADDR = 34u,		/* AP IP地址 */
    CMD_NETMASK = 35u,
    CMD_AP_USER_NAME = 36u,		/* AP用户名称 */
    CMD_AP_PASSWORD = 37u,		/* AP密码 */
    CMD_AP_EFFECT_CONF = 38u,		/* AP生效配置 */
    CMD_WIRELESS_CLT   = 39u,      /*ac get the MAC of pc from ap*/
    CMD_AIR_PROBE = 40u,		/* AP生效配置 */
    CMD_WIRELESS_MODE = 41u,		/* AP生效配置 */
    CMD_WL_MAC_FILTER = 42u,		/* AP生效配置 */
    CMD_WL_MAC_FILTE = 45u,		/* AP生效配置 */
    CMD_CONFIG_TEMPLATE = 46u,           /* AP配置模板*/
#if 0
    CMD_SZ_STATUS = 47u,
#endif
    CMD_SZ_NAME = 48u,			/* 服务区名称 */
    CMD_SZ_SSID = 49u,			/* 服务区SSID */
    CMD_SZ_SSID_BC = 50u,		/* 服务区广播 */
    CMD_SZ_VID = 51u,			/* 服务区vlan id */
    CMD_SZ_AUTH = 52u,			/* 服务区认证模式 */
    CMD_SZ_WEP = 53u,			/* WEP认证 */
    CMD_SZ_WPA = 54u,			/* WPA认证 */
    CMD_SZ_PSK = 55u,			/* WPA-PSK认证 */
#if 0
    CMD_SZ_DHCP = 52u,
    CMD_SZ_START_IP = 53u,
    CMD_SZ_END_IP =54u,
    CMD_SZ_GW = 55u,
    CMD_SZ_SUBMASK = 56u,
#endif
    CMD_AP_SZ_NAME = 56u,		/* 服务区名称 */
    CMD_AP_SZ_NUM =57u,			/* 支持的最大服务区数目 */
    CMD_AP_SERVICE_ZONE = 58u,

    /**************************** AP - AC *****************************/
	REQUEST_CHANGE_CONF = 59u,
	REQUEST_FAT_TO_FIT  = 60u,
	REQUEST_SEND_FILE   = 61u,
    CMD_GATEWAY = 62u,
	CMD_AP_AFFIRM = 63u,
	CMD_AP_AUTH_PRE = 64u,
	CMD_AP_DHCP_EN = 65u,
    CMD_SETACMODE_CONFIG = 66u,
	CMD_AP_ROAMING_TH=67u,
    CMD_AP_LB_MAX_CLIENT=68u,
    CMD_AP_RA0_MAC = 69u,
    CMD_AP_RA0_SSID = 70u,
    CMD_AP_WL_POWER = 71u,
    CMD_AP_SITE_SURVEY = 72u,
    CMD_HW_VERSION = 73u ,                /*连接端口*/
    CMD_SZ_SSID_ISOLATE = 74u,		/* 服务区广播 */
    CMD_MAC_FILTER_NUM = 75u, /*ap mac地址过滤*/
    CMD_MAC_FILTER = 76u,
    CMD_SET_DISASSOC = 77u,
    CMD_AP_MAX_CLIENT_NEW = 78u,                   /*最大客户端数*/
    CMD_AP_ADD_MAC_BLACK = 79u,
    CMD_AP_CHANNEL_5G = 80u,                /*AP信道*/
    CMD_AP_ROAMING_TH_5G = 81u,                /*5G AP漫游阈值*/
    CMD_SZ_SHARE_LIMIT = 82u,                /*5G AP漫游阈值*/
    CMD_END
}urcp_cmd_t;

typedef enum {
	URCP_TRAP_LEVEL_EMERG   = 0u,

	URCP_TRAP_LEVEL_ALERT   = 1u,

	URCP_TRAP_LEVEL_CRIT    = 2u,

	URCP_TRAP_LEVEL_ERR     = 3u,

	URCP_TRAP_LEVEL_WARING  = 4u,

	URCP_TRAP_LEVEL_NOTICE  = 5u,

	URCP_TRAP_LEVEL_INFO    = 6u,

	URCP_TRAP_LEVEL_DEBUG   = 7u,

} urcp_trap_level;

typedef enum {
	TRAP_START = 0u,

	TRAP_DISCOVER = 1u,

	TRAP_UPPER_LIMIT = 2u,

	TRAP_PRODUCT_MODEL = 3u,

	TRAP_SERIAL_NO = 4u,

	TRAP_DEV_NAME = 5u,

	TRAP_SW_VERSION = 6u,

	TRAP_AP_SSID = 7u,

	TRAP_AP_CHANNEL = 8u,

	TRAP_AP_CLIENTS = 9u,

	TRAP_WIRELESS_MODE = 10u,

	TRAP_AP_DHCP_EN = 11u,

	TRAP_AP_SZ_NUM = 12u,

	TRAP_STA_INFO = 13u,

	TRAP_AP_RA0_MAC = 14u,

	TRAP_AP_WL_POWER = 15u,

	TRAP_HW_VERSION = 16u,

    TRAP_STA_INFO_AUTO = 17u,

    TRAP_SZONE_UPDATA = 18u,

	TRAP_AP_MF_NAME = 19u,
	
	TRAP_AP_CHANNEL_NEW = 20u,

    TRAP_AP_MAC_TABLE = 21u,
	
	TRAP_CMD_AP_STAT = 22u,/*HC 收发字节数命令*/
	
	TRAP_CMD_AP_RATE = 23u,/*HC 收发速率命令*/
	
	TRAP_AP_SYS_INFO = 24u,

	TRAP_AP_RF_INFO = 25u,

	TRAP_AP_RF_RTX_INFO = 26u,

	TRAP_AP_SSID_5G = 27u,

	TRAP_AP_CHANNEL_NEW_5G = 28u,

	TRAP_AP_CLIENTS_5G = 29u,

	TRAP_WIRELESS_MODE_5G = 30u,

	TRAP_AP_SZ_NUM_5G = 31u,

	TRAP_AP_RF_INFO_5G = 32u,

	TRAP_AP_RF_RTX_INFO_5G = 33u,

	TRAP_END,
} urcp_trap_code;


typedef uint16 (*get_fp) (cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info);
typedef uint16 (*get_reply_fp) (cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info);
typedef uint16 (*set_fp) (cmd_hdr_t *cmd, uchar **app);
typedef void (*fp) (void);
typedef uint16 (*trap_set_fp) (cmd_hdr_t *cmd,urcp_dev_t **dev, urcp_infor_t *urcp_info);
typedef uint16 (*syslog_fp) (cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t *urcp_info);

typedef struct _urcp_trap_cmd_tb_t{
	urcp_trap_code   cmd_id;
	urcp_trap_level  cmd_level;
	syslog_fp        syslog;
    trap_set_fp      func_set;
}urcp_trap_cmd_tb_t;

typedef struct _urcp_cmd_tb_t{
    urcp_cmd_t  cmd_id;
    int32  need_auth;
    get_fp  func_get;
    get_reply_fp func_get_reply;
    set_fp  func_set;
    fp func_aux;
}urcp_cmd_tb_t;

extern urcp_cmd_tb_t cmd_tb[];
extern urcp_trap_cmd_tb_t trap_cmd_tb[];
#endif
