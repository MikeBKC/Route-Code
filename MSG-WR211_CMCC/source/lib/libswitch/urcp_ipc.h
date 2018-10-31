#ifndef __URCP_SHM_H__
#define __URCP_SHM_H__
#include <linux/switch/swioc_common.h>
#include <stdint.h>
#include <mib.h>
#include "../../config/user_oid.h"

#define MAX_STATUS_SIZE 10

#define MAX_DEV_NAME_LEN          12u  /*设备名*/
#define MAX_USER_NAME_LEN         12u  /*用户名*/
#define MAX_PASSWORD_LEN          12u
#define MAX_REMARK_LEN 12
#define TMP_BUF_LEN    32


#define MAX_VLAN_NAME_LEN 11u



#define SW_MSG_TYPE     0x38
/*read data from switch*/
#define MSG_TYPE_GET     0x01u
/*write data to the switch*/
#define MSG_TYPE_SET     0x02u
#define MSG_TYPE_URCP    0x03u
#define MSG_TYPE_URCP_DB    0x04u

#define GET_URCP_DISCOVERY 0x01
#define GET_URCP_TOPOY     0x02
/*define the urcp message code*/
#define GET_AUTH         0x03
#define SET_MIRROR       0x04
#define GET_MIRROR       0x05 
#define GET_AUTH_TOPOY   0x06
#define GET_CONFIG       0x07
#define SET_CONFIG       0x08u
#define GET_IMAGE        0x09u
#define AGGR_CONFIG      0x80u
#define GET_RSTP_LACP    0x81u
#define URCP_VLAN_CONFIG   0x82u
#define QVLAN_PORT_CONFIG  0x83u
#define URCP_GET_PORT_CONF  0x84u
#define URCP_SET_PORT_CONF  0x85u
#define URCP_GET_QOS_RATE_LIMIT 0x86u
#define URCP_SET_QOS_RATE_LIMIT 0x87u
#define URCP_GET_STORM_RATE_LIMIT 0x88u
#define URCP_SET_STORM_RATE_LIMIT 0x89u
#define URCP_CMD_ALONE            0x8Au
#define URCP_GET_SYS_CONF   0x8Bu
#define URCP_SET_SYS_CONF   0x8Cu
#define URCP_GET_IP_FILTER   0x8Du
#define URCP_GET_SYS_INFO    0x8Eu
#define PASSWORD_CONFIG     0x8Fu
#define SET_AP_SCAN_MODE    0x90u
#define START_AP_SCAN       0x91u
#define GET_AP_BASIC_CONFIG	0x92u	/* 设置AP基本配置 */
#define SET_AP_BASIC_CONFIG	0x93u	/* 设置AP基本配置 */
#define SET_IMAGE   0x94u   /* Set AP image */
#define START_AP_SCAN_CLT 0x95u
#define SET_CONFIG_TEMPLATE 0x96u	/*配置模板*/
#define URCP_SET_AP_JOIN_SZ	0x97u	/* AP加入服务区 */
#define URCP_GET_AP_JOIN_SZ	0x98u	/* AP加入服务区 */

#define REQUEST_AC_CHANGE   0x99u   /* AP发命令请求AC修改保存的AP的config */

#define URCP_CMD_SET_AC_MODE 0x9Au
#define CHANGE_CHANNEL 0x9Bu
#define SET_AP_LOAD_BALANCE 0x9Cu
#define URCP_CMD_LOAD_BALANCE 0x9Du
#define URCP_SET_AP_MAC_FILTER 0x9Eu
#define URCP_MSG_DISCONNECT_STA 0x9Fu
#define URCP_MSG_ADD_BLACK_LIST 0xA0u
#define URCP_MSG_ADD_BLACK_LIST_SZ 0xA1u
#define URCP_MSG_DEL_LOG 0xA2u

#define URCP_DB_SET           0xB1u /*urcp debug commond*/
#define URCP_DB_GET           0xB2u


/*联动下发各配置项id*/
#define SYS_CONF         1u
#define VLAN_CONF        2u
#define AGGR_CONF        3u
#define PORT_CONF        4u
#define RATELIMIT_CONF   5u
#define ARP_CONF         6u
/*end 联动下发各配置项id*/


#define USB_LOG_PATH        "syslog/"  /*u盘中本机日志目录*/
#define TFTP_LOG_ROOT      "/tmp/urcp/logs/"/*日志文件存放目录*/
#define RAM_LOG_PATH       "/tmp/syslog/"

#define USB_CONFIG_PATH     "configs/" /*u盘中联动获得配置文件的目录*/
#define RAM_CONFIG_PATH    "/tmp/urcp/configs/"

#define USB_IMAGE_PATH      "images/"
#define RAM_IMAGE_PATH      "/tmp/urcp/images/"

#define USB_AP_CONFIG_ROOT  "/ap_configs/"
#define RAM_AP_CONFIT_ROOT  "/tmp/urcp/ap_configs/"

#define USB_DRAWING_PATH      "/drawing/"
#define RAM_DRAWING_PATH      "/tmp/urcp/drawing/"

#define URCP_DATA_INVALID  0u
#define URCP_DATA_VALID    1u

/*
 * 联动相关  
 * 各结构大小定义
 *
 **/
#define DEV_TYPE_SIZE          12u  /*设备型号描述*/
#define SW_VERSION_SIZE        32u  /*软件版本信息*/
#define HW_VERSION_SIZE        16u  /*软件版本信息*/
#define DEV_DESCRIP_SIZE       2u  /*设备描述信息*/

#define DEV_TABLE_SIZE         MAX_AP_MANAGED
#define DEV_HASH_SIZE          48u
#define MAX_TOPOY_DEV          64u
#define MAX_TOPOY_HOP          16u
#define NONE_NODE    0 

#define STA_TB_HASH_SIZE   32
#define STA_TB_SIZE    4096

#define DEV_HASH_USE_IP  0

#define MANAGED_DIS            0
#define MANAGED_EN             1


/*misc buffer操作所需宏定义
 *  *这些宏主要用于区别访问misc buffer，以减少访问冲突
 *   *若在一个misc buffer数组上定义了多个不同的操作，则需定义对应的宏
 *    * */
#define MISC_GET_GENERAL       0u  /*用于区别一般情况下的读取或设置中的 获取操作*/
#define MISC_SET_GENERAL       1u  /*用于区别一般情况下的读取或设置中的 设置操作*/

#define MISC_GET_VLAN  2u  /*按照远程设备实际vlan类型获取vlan信息*/
#define MISC_GET_PVLAN 3u  /*获取port vlan信息而不管远程设备当前vlan类型*/
#define MISC_SET_PVLAN 4u  /*设置port vlan*/
#define MISC_GET_LOG   5u  /*读取日志*/
#define MISC_GET_IMAGE 6u  /*get the image file*/

/*以下宏用于区分单命令类型*/
#define MISC_REBOOT    2u
#define MISC_RESET     3u
#define MISC_SAFEBIND  4u

/*end misc buffer操作所需宏定义*/

#define URCP_MISC_CMD_ALONE_SIZE 64u
#define URCP_MISC_MIRROR_SIZE    16u
#define URCP_MISC_AGGR_SIZE      16u
#define URCP_MISC_VLAN_SIZE      64u
#define URCP_MISC_QVLAN_PCONF_SIZE 16u
#define URCP_MISC_RSTP_LACP_SIZE  16u
#define URCP_MISC_PASSWD_SIZE    30u
#define URCP_MISC_PORT_CONF_SIZE  16u
#define URCP_MISC_SYS_CONF_SIZE   16u
#define URCP_MISC_SYS_INFO_SIZE   16u
#define URCP_MISC_IP_FILTER_SIZE   16u
#define URCP_MISC_QOS_RATE_LIMIT_SIZE 16u
#define URCP_MISC_STORM_RATE_LIMIT_SIZE 16u
#define URCP_MISC_CONFIG_SIZE    30u
#define URCP_MISC_AP_BASIC_CONF_SIZE	30u
#define URCP_MISC_AP_CONF_TEMP_SIZE	30u
#define URCP_MISC_AP_JOIN_SZ_SIZE	30u
#define URCP_MISC_WL_MAC_FILTER_SIZE 16u
#define URCP_MISC_AP_MAC_FILTER_SIZE 30u

/*保存联动读取的各项信息*/
#define MISC_MSG_LEN   16u

#define MAX_URCP_PORT  32u /*联动收到的有效端口号最大值*/
#define MIN_URCP_PORT  1u  /*联动收到的有效端口号最小值*/
#define MAX_URCP_PORT_NUM 32u
#define MAX_URCP_BUFF_VLAN_NUM 32u /*每个联动包最大包含几个vlan组*/
#define MAX_URCP_VID  512u  /*各个可联动设备中的最大vid,中的最大值*/
#define MAX_URCP_WEB_TIME_OUT_SEC 5u/*页面等待联动操作时，最大超时时间*/
#define URCP_WEB_WAIT_INTERVAL_USEC 100000u/*联动操作页面等待时间间隔，单位为微秒*/
#define URCP_SWAP_CONF_SIZE 1518u/*联动操作goahead 与 switch交换空间设置区大小*/

#define MAX_MSG_SIZE    1024
struct sw_msg_t{
    int msg_type;
    int msg_code;
    char buffer[MAX_MSG_SIZE];
};

struct msgbuf_t{
    long mtype;
    struct sw_msg_t sw_msg;
};

#define	MAX_URCP_BATCH_NUM 30u 
/*密码验证操作消息*/
typedef struct _urcp_batch_msg_t{
    unsigned char dst_mac[MAC_LEN*MAX_URCP_BATCH_NUM];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_URCP_BATCH_NUM][MAX_PASSWORD_LEN];
    unsigned int  number;   /*要操作目的交换机数量*/
}urcp_batch_msg_t;
#define MAX_CONF_ITEM   6u  /*联动下发配置项数目*/
typedef struct _urcp_send_conf_msg_t{
    urcp_batch_msg_t  batch_info;
    unsigned int inUSB;  /*文件是否存在u盘中*/
    unsigned int serial_no; /* AP Serial Number */
    unsigned char item_id[MAX_CONF_ITEM];  /*配置项*/
    char src_file[64];  /*文件名*/
}urcp_send_conf_msg_t;

typedef struct _urcp_send_channel_msg_t{
    urcp_batch_msg_t  batch_info;
    int channel; /* AP Channel*/
}urcp_send_channel_msg_t;

typedef struct _urcp_send_load_balance_msg_t{
    urcp_batch_msg_t  batch_info;
    unsigned int client;
}urcp_send_load_balance_msg_t;

typedef struct _urcp_send_log_manage_msg_t{
    urcp_batch_msg_t  batch_info;
    unsigned int client;
} urcp_send_log_manage_msg_t;

/*单命令消息结构*/
typedef struct _urcp_cmd_alone_msg_t{
    urcp_batch_msg_t  batch_info;
    unsigned int  opt_type;/*操作类型*/
}urcp_cmd_alone_msg_t;

/*联动修改消息结构*/
typedef struct _urcp_pwd_msg_t{
	urcp_batch_msg_t  batch_info;
	unsigned int opt_type; /*操作类型*/
	unsigned char dev_password[MAX_PASSWORD_LEN];/*要修改的联动交换机密码*/
}urcp_pwd_msg_t;

typedef struct _urcp_sta_msg_t{
    unsigned short staNum;
    unsigned char staMac[1][MAC_LEN];
}urcp_sta_msg_t;

typedef struct _urcp_black_list_t{
    char ssid[UTT_SSID_LENTH];   /* SSID名称	    */
    unsigned char staMac[MAC_LEN];
}urcp_black_list_t;

typedef struct _urcp_black_list_msg_t{
    unsigned short mode;	    //1,添加，2,删除两种模式,3删除全部
    unsigned short staNum;
    urcp_black_list_t black_list[1];
}urcp_black_list_msg_t;

typedef enum{
    URCP_STATUS_WAIT=1,
    URCP_STATUS_OK,
    URCP_STATUS_TIME_OUT,
    URCP_STATUS_AUTH_ERROR, /*密码错误*/
    URCP_STATUS_FAIL,         /*操作失败 */
    URCP_STATUS_FOUND_NONE,         /*操作失败 */
    URCP_STATUS_END/*操作失败 */
}urcp_status_t;

typedef struct _misc_header_t{
    int32 valid;
    uint32   user_ip;          /*登陆用户ip*/
    unsigned char  mac[MAC_LEN]; /*操作的原程设备mac*/
    uint16   req_id;       /*本次操作对应的request id*/
    uint32 data_type; /*表示本节点是通过联动收到的数据还是页面设置的数据*/
    urcp_status_t status;       /*操作结果:成功,密码错误、超时、失败*/
}misc_header_t;


/*联动操作（这些操作仅仅只有命令类型，无命令数据。如重启，恢复出厂配置等）*/
typedef struct _urcp_cmd_alone_buf_t{
    misc_header_t cmd_alone_header;
}urcp_cmd_alone_buf_t;

typedef struct _urcp_cmd_alone_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_cmd_alone_buf_t urcp_cmd_alone_buf[URCP_MISC_CMD_ALONE_SIZE];
}urcp_cmd_alone_struct_t;

/*联动vlan*/
typedef struct _urcp_vlan_group_t
{
    uint16 vlan_no;
    char vlan_name[MAX_VLAN_NAME_LEN+1u];
    port_bit_mask_t member; 
}urcp_vlan_group_t;

typedef struct _urcp_vlan_conf_t
{
    uint32 port_num;        /*port number*/
    uint32 misc_type;/*用与访问misc buffer时解决冲突问题 以及确定获取什么类型的vlan */
    uint32 vlan_type;/*接受到的vlan类型即远程设备当前vlan类型*/
    uint16 next_vlan_no;/*下一个接受的vlan 组号。如tag vlan可能需要分片接受*/
    urcp_vlan_group_t vlan_group[MAX_URCP_BUFF_VLAN_NUM];
    /*vlan信息。可能不为vlan_type类型的信息，但一定为misc_type类型的vlan信息*/
}urcp_vlan_conf_t;

typedef struct _urcp_vlan_buf_t{
    misc_header_t vlan_header;
    urcp_vlan_conf_t vlan_conf;
}urcp_vlan_buf_t;

typedef struct _urcp_vlan_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_vlan_buf_t urcp_vlan_buf[URCP_MISC_VLAN_SIZE];
}urcp_vlan_struct_t;

/*tag vlan端口设置*/
typedef struct _urcp_qvlan_pconf_group_t
{
    uint32 vlan_aware;/*vlan设备*/
    uint32 frame_type;/*帧类型*/
    uint16 pvid;/*pvid*/
}urcp_qvlan_pconf_group_t;
typedef struct _urcp_qvlan_pconf_conf_t
{
    uint32 port_num;        /*port number*/
    uint32 vlan_type;/*接受到的vlan类型即远程设备当前vlan类型*/
    urcp_qvlan_pconf_group_t pconf_group[MAX_URCP_PORT_NUM];
}urcp_qvlan_pconf_conf_t;

typedef struct _urcp_qvlan_pconf_buf_t{
    misc_header_t qvlan_pconf_header;
    urcp_qvlan_pconf_conf_t qvlan_pconf_conf;
}urcp_qvlan_pconf_buf_t;

typedef struct _urcp_qvlan_pconf_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_vlan_buf_t urcp_qvlan_pconf_buf[URCP_MISC_QVLAN_PCONF_SIZE];
}urcp_qvlan_pconf_struct_t;

#define MAX_PORT_REMARK_LEN 16
typedef struct _urcp_port_conf_group_t
{
    char remark[MAX_PORT_REMARK_LEN];/*端口描述*/
    char link_status[MAX_STATUS_SIZE];/*链路状态*/
    int selected_mode;/*设置模式*/
    int max_frame;/*允许最大帧*/
    char rate_limit;/*流控*/
    char protect;/*端口保护*/
    char flooding;/*泛洪*/
}urcp_port_conf_group_t;
typedef struct _urcp_port_conf_conf_t
{
    uint32 port_num;        /*port number*/
    urcp_port_conf_group_t port_conf_group[MAX_URCP_PORT_NUM];
}urcp_port_conf_conf_t;
typedef struct _urcp_port_conf_buf_t{
    misc_header_t port_conf_header;
    urcp_port_conf_conf_t port_conf_conf;
}urcp_port_conf_buf_t;
typedef struct _urcp_port_conf_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_port_conf_buf_t urcp_port_conf_buf[URCP_MISC_PORT_CONF_SIZE];
}urcp_port_conf_struct_t;


/*速率限制*/
typedef struct _urcp_qos_rate_limit_group_t
{
    unsigned long policer;
    unsigned long shaper;
}urcp_qos_rate_limit_group_t;

typedef struct _urcp_qos_rate_limit_conf_t
{
    uint32 port_num;        /*port number*/
    uint32 rate_unit;  /*颗粒度*/
    urcp_qos_rate_limit_group_t qos_rate_limit_group[MAX_URCP_PORT_NUM];
}urcp_qos_rate_limit_conf_t;
typedef struct _urcp_qos_rate_limit_buf_t{
    misc_header_t qos_rate_limit_header;
    urcp_qos_rate_limit_conf_t qos_rate_limit_conf;
}urcp_qos_rate_limit_buf_t;
typedef struct _urcp_qos_rate_limit_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_qos_rate_limit_buf_t urcp_qos_rate_limit_buf[URCP_MISC_QOS_RATE_LIMIT_SIZE];
}urcp_qos_rate_limit_struct_t;

/*风暴抑制*/
typedef struct _urcp_storm_rate_limit_conf_t
{
    unsigned long icmp_rate;/*icmp*/
    unsigned long bc_rate;/*broadcast*/
    unsigned long mc_rate;/*multicast*/
    unsigned long uc_rate;/*unicast*/
}urcp_storm_rate_limit_conf_t;
typedef struct _urcp_storm_rate_limit_buf_t{
    misc_header_t storm_rate_limit_header;
    urcp_storm_rate_limit_conf_t storm_rate_limit_conf;
}urcp_storm_rate_limit_buf_t;
typedef struct _urcp_storm_rate_limit_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_storm_rate_limit_buf_t urcp_storm_rate_limit_buf[URCP_MISC_STORM_RATE_LIMIT_SIZE];
}urcp_storm_rate_limit_struct_t;

/*系统设置*/
typedef struct _urcp_sys_conf_conf_t
{
    uint32 dhcp_relay_enable;
    uint32 dhcp_client_enable;
    uint32 ip_addr;
    uint32 ip_mask;
    uint32 gw_ip;
    uint32 cpu_vid;
    uint32 age_time;
    uint32 arp_enable;
    char dev_name[MAX_DEV_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
}urcp_sys_conf_conf_t;
typedef struct _urcp_sys_conf_buf_t{
    misc_header_t sys_conf_header;
    urcp_sys_conf_conf_t sys_conf_conf;
}urcp_sys_conf_buf_t;
typedef struct _urcp_sys_conf_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_sys_conf_buf_t urcp_sys_conf_buf[URCP_MISC_SYS_CONF_SIZE];
}urcp_sys_conf_struct_t;

/*系统信息*/
typedef struct _urcp_sys_info_conf_t
{
    char now_time[64];/*系统时间*/
    char up_time[64];/*运行时间*/

    long long outer_flash_total_space;/*外接flash总大小*/
    long long outer_flash_unused_space;/*外接flash可用空间*/

    int32 cpu_rate;/*cpu使用率*/
    int32 mem_rate;/*内存使用率*/

    uint32 ip_addr;/*系统ip*/
    uint32 ip_mask;/*ip掩码*/
    uint32 gw_ip;/*网关*/
    unsigned char mac_addr[MAC_LEN];

    int32 sys_serialno;/*序列号*/
    char sw_version[SW_VERSION_SIZE];/*软件版本*/

    uint32 cpu_vid;
    char dev_name[MAX_DEV_NAME_LEN];
}urcp_sys_info_conf_t;
typedef struct _urcp_sys_info_buf_t{
    misc_header_t sys_info_header;
    urcp_sys_info_conf_t sys_info_conf;
}urcp_sys_info_buf_t;
typedef struct _urcp_sys_info_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_sys_info_buf_t urcp_sys_info_buf[URCP_MISC_SYS_INFO_SIZE];
}urcp_sys_info_struct_t;

/*ip地址过滤*/
typedef struct _urcp_ip_filter_conf_t
{
    uint32 port_num;
    uint32 filter_src_ip[MAX_URCP_PORT_NUM];/*源ip*/
    uint32 filter_src_ip_mask[MAX_URCP_PORT_NUM];/*源ip掩码*/
}urcp_ip_filter_conf_t;
typedef struct _urcp_ip_filter_buf_t{
    misc_header_t ip_filter_header;
    urcp_ip_filter_conf_t ip_filter_conf;
}urcp_ip_filter_buf_t;
typedef struct _urcp_ip_filter_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_ip_filter_buf_t urcp_ip_filter_buf[URCP_MISC_IP_FILTER_SIZE];
}urcp_ip_filter_struct_t;

/*密钥验证操作buffer*/
typedef struct _urcp_passwd_auth_buf_t{
    misc_header_t  passwd_auth_header;
}urcp_passwd_auth_buf_t;
typedef struct _urcp_passwd_auth_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_passwd_auth_buf_t urcp_passwd_auth_buf[URCP_MISC_PASSWD_SIZE];
}urcp_passwd_auth_struct_t;

/*联动密码*/
typedef struct _urcp_passwd_buf_t{
    misc_header_t  passwd_header;
}urcp_passwd_buf_t;
typedef struct _urcp_passwd_buf_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_passwd_buf_t urcp_passwd_buf[URCP_MISC_PASSWD_SIZE];
}urcp_passwd_struct_t;
typedef struct _urcp_mirror_conf_t{
    uint32   port_num;        /*port number*/
    uint32 mirror_port;
    port_bit_mask_t mirror_source;/*1-32端口*/
}urcp_mirror_conf_t;
typedef struct _urcp_mirror_buf_t{
    misc_header_t  mirror_header;
    urcp_mirror_conf_t mirror_conf;
}urcp_mirror_buf_t;

typedef struct _urcp_mirror_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_mirror_buf_t urcp_mirror_buf[URCP_MISC_MIRROR_SIZE];
}urcp_mirror_struct_t;
typedef struct _urcp_aggr_group_t
{
    uint16 index;
    char remark[MAX_REMARK_LEN];
    port_bit_mask_t member; 
}urcp_aggr_group_t;
typedef struct _urcp_aggr_conf_t
{
    uint32   port_num;        /*port number*/
    urcp_aggr_group_t aggr_group[MAX_URCP_PORT_NUM]; 
}urcp_aggr_conf_t;
typedef struct _urcp_aggr_buf_t{
    misc_header_t  aggr_header;
    urcp_aggr_conf_t aggr_conf;
}urcp_aggr_buf_t;
typedef struct _urcp_aggr_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_aggr_buf_t urcp_aggr_buf[URCP_MISC_AGGR_SIZE];
}urcp_aggr_struct_t;

typedef struct _urcp_rstp_lacp_conf_t{
    uint32   port_num;        /*port number*/
    port_bit_mask_t rstp_enable;/*1-32端口*/
    port_bit_mask_t lacp_enable;
}urcp_rstp_lacp_conf_t;
typedef struct _urcp_rstp_lacp_buf_t{
    misc_header_t  rstp_lacp_header;
    urcp_mirror_conf_t rstp_lacp_conf;
}urcp_rstp_lacp_buf_t;
typedef struct _urcp_rstp_lacp_buf_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_rstp_lacp_buf_t urcp_rstp_lacp_buf[URCP_MISC_RSTP_LACP_SIZE];
}urcp_rstp_lacp_struct_t;

typedef struct _urcp_ap_basic_conf_sz_info_t
{
    uchar sz_name[UTT_INST_NAME_LEN];
    uchar user_num;
}urcp_ap_basic_conf_sz_info;
typedef struct _urcp_ap_basic_conf_sz_max_t
{
    uint16 sz_num;
    urcp_ap_basic_conf_sz_info sz[4];
}urcp_ap_basic_conf_sz_max;

/* Fit AP basic configuration */
typedef struct _urcp_ap_basic_conf_t
{
    uint32 work_mode;    /* 工作模式 1-fit 2-fat*/
	uint32 dhcp_en;		/*AP DHCP Client*/
    uint32 ip_addr; /* IP地址 */
    uint32 sub_mask;
    uint32 gate_way;
//    uint32 max_clt;  /* 最大客户端数 */
    uint32 channel;  /* 信道 */
    uint32 channel_5g;	    /*5G信道*/
	uint32 dbm;      /* 漫游阈值 */
	uint32 dbm_5g;      /* 漫游阈值 */
    uint32 effect_conf;	/* 生效配置 1-local 其余值均为AC下发*/
    char pass_modify;   /* AP用户名 */
    char password[MAX_PASSWORD_LEN];	/* 修改的新密码 */
    char dev_name[MAX_DEV_NAME_LEN];	/* AP 设备名 */
    urcp_ap_basic_conf_sz_max sz_max_client;
}urcp_ap_basic_conf_t;

typedef struct _urcp_ap_baisc_conf_buf_t{
    misc_header_t	ap_basic_conf_header;
    urcp_ap_basic_conf_t    ap_basic_conf;
}urcp_ap_basic_conf_buf_t;

typedef struct _urcp_ap_basic_conf_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_ap_basic_conf_buf_t urcp_ap_basic_conf_buf[URCP_MISC_AP_BASIC_CONF_SIZE];	
}urcp_ap_basic_conf_struct_t;

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
    uint32 wlFre;		/* 无线频段 */
    uint32 beaconPeriod;		/* */
    uint32 VChanWidth;	    /*VHT BW*/
}urcp_ap_conf_temp_conf_t;

typedef struct _urcp_ap_conf_temp_buf_t{
    misc_header_t	ap_basic_conf_header;
    urcp_ap_conf_temp_conf_t    ap_conf_temp_conf;
}urcp_ap_conf_temp_buf_t;

typedef struct _urcp_ap_conf_temp_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_ap_conf_temp_buf_t urcp_ap_conf_temp_buf[URCP_MISC_AP_CONF_TEMP_SIZE];	
}urcp_ap_conf_temp_struct_t;

/* Fit AP join service zone start */

typedef enum _auth_mode_t
{
    none,   /* 无	*/
    wep,    /* WEP	*/
    wpa,    /* WPA	*/
    wpa_psk /* WPA-PSK	*/
}auth_mode;

/************* WEP **************/

typedef enum _en_auth_Method_t
{
    auto_auth,	/* 自动	    */
    open_sys,	/* 开放系统 */
    share_key	/* 共享密钥 */
}auth_method;

typedef enum _en_key_type_t
{
    hexa,   /* 16进制	*/
    ascii   /* ASCII码	*/
}key_type;

typedef enum _en_key_format_t
{
    forbid, /* 禁用 */
    bit_64, /* 64位 */
    bit_128 /* 128位*/
}key_format;

typedef struct _key_num_t
{
    uint32	key_state_action;	/* 使用状态 */
    char	passwd[UTT_PAS_KEY_VALUE_LEN+1];	/* WEP密钥  */
    key_format	kFmt;			/* 密钥类型 */
}key_num;

typedef struct _wep_security_t
{
    auth_method	auMethod;   /* 认证类型	*/
    key_type	kType;	    /* 密钥格式 */
    key_num	kNum1;	    /* 密钥 1	*/
    key_num	kNum2;	    /* 密钥 2	*/
    key_num	kNum3;	    /* 密钥 3	*/
    key_num	kNum4;	    /* 密钥 4	*/
}auth_wep;

/************* WEP END **************/

/************* WPA **************/

typedef enum _en_wpaVersion_t
{
    auto_wpa,	/* 自动	*/
    wpa1,	/* WPA	*/
    wpa2	/* WPA2	*/
}wpa_version;

typedef enum _en_securityAlgorithm_t
{
    auto_sec,	/* 自动	*/
    tkip,	/* TKIP	*/
    aes		/* AES	*/
}security_alg;

typedef struct _wpa_security_t
{
    wpa_version	    wVersion;			    /* WPA版本		*/
    security_alg    sAlg;			    /* 加密算法		*/
    uint32	    radiusIp;			    /* radius服务器IP   */
    uint32	    radiusPort;			    /* radius服务器端口 */
    char	    radiusPasswd[UTT_RADIUS_KEY_LEN];   /* radius密码	*/
    uint32	    updataTime;			    /* 密钥更新周期	*/
}auth_wpa;

/************* WPA END **************/

/************* WPA-PSK **************/

typedef struct _wpa_psk_security_t
{
    wpa_version	    wVersion;			/* WPA-PSK版本  */
    security_alg    sAlg;			/* 加密算法	*/
    char	    sharePasswd[UTT_RADIUS_KEY_LEN];/* 预共享密钥   */
    uint32	    updataTime;			/* 密钥更新周期 */
}auth_wpa_psk;

/************* WPA-PSK END **************/

typedef struct _urcp_ap_join_sz_t
{
    uint32	serviceEn;		    /* 服务区状态   */
    char	szName[UTT_INST_NAME_LEN + 1]; /* 服务区名称*/
    char	ssid[UTT_SSID_LENTH + 1];   /* SSID名称	    */
    uint32	ssidBcast;		    /* SSID广播	    */
    uint32	vid;			    /* vlan ID	    */
    uint32	client;			    /* Max client   */
    auth_mode	securMode;		    /* 加密方式	    */
    auth_wep	wepSecurity;		    /* WEP	    */
    auth_wpa	wpaSecurity;		    /* WPA	    */
    auth_wpa_psk    wpaPskSecurity;	    /* WPA-PSK	    */
#if 0
    uint32	dhcpEn;			    /* DHCP服务器   */
    uint32	startIp;		    /* 起始IP	    */
    uint32	endIp;			    /* 结束IP	    */
    uint32	gw;			    /* 网关	    */
    uint32	netmask;		    /* 子网掩码	    */
#endif
}urcp_ap_join_sz_t;

typedef struct _urcp_ap_join_sz_buf_t
{
    misc_header_t	ap_join_sz_header;
    char		ap_join_sz[(UTT_INST_NAME_LEN+1)*6];
}urcp_ap_join_sz_buf_t;

typedef struct _urcp_ap_join_sz_struct_t
{
    uint32		    next_index;
    urcp_ap_join_sz_buf_t   urcp_ap_join_sz_buf[URCP_MISC_AP_JOIN_SZ_SIZE];
}urcp_ap_join_sz_struct_t;

/* Fit AP join service zone end */


/*一般联动操作消息结构，特色情况需重新定义*/
typedef struct _urcp_general_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 opt_type; /*misc buff操作类型*/
}urcp_general_msg_t;

/*镜像操作消息*/
typedef struct _urcp_mirror_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 mirror_port;
    port_bit_mask_t mirror_source;/*1-32端口*/
}urcp_mirror_msg_t;

/*汇聚操作消息结构*/
typedef struct _urcp_aggr_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 opt_type; /*misc buff操作类型*/
    urcp_aggr_group_t aggr_group;/*汇聚组信息*/
}urcp_aggr_msg_t;

/*RSTP LACP操作消息结构*/
typedef struct _urcp_rstp_lacp_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
}urcp_rstp_lacp_msg_t;

/*vlan操作消息结构*/
typedef struct _urcp_vlan_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 opt_type; /*misc buffer opt type*/
    uint16 start_vlan_no;/*从哪个vlan no开始获取*/
    urcp_vlan_group_t vlan_group;/*vlan组信息*/
}urcp_vlan_msg_t;

/*tag vlan端口设置操作消息结构*/
typedef struct _urcp_qvlan_pconf_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
}urcp_qvlan_pconf_msg_t;

/*联动速率限制消息结构体*/
typedef struct _urcp_qos_rate_limit_msg_t{
    urcp_general_msg_t general_msg;/*通用消息*/
    urcp_qos_rate_limit_conf_t qos_rate_limit_conf;/*限速结构体*/
}urcp_qos_rate_limit_msg_t;
/*联动风暴抑制*/
typedef struct _urcp_storm_rate_limit_msg_t{
    urcp_general_msg_t general_msg;/*通用消息*/
    urcp_storm_rate_limit_conf_t storm_rate_limit_conf;/*限速结构体*/
}urcp_storm_rate_limit_msg_t;

/*联动系统设置消息结构体*/
typedef struct _urcp_sys_conf_msg_t{
    urcp_general_msg_t general_msg;/*通用消息*/
    urcp_sys_conf_conf_t sys_conf_conf;/*限速结构体*/
}urcp_sys_conf_msg_t;

/* 远程AP基本设置消息结构体 */
typedef struct _urcp_ap_basic_conf_msg_t{
    urcp_general_msg_t general_msg;/*通用消息*/
    urcp_ap_basic_conf_t  ap_basic_conf;
}urcp_ap_basic_conf_msg_t;

#define MAX_MAC_NUM  6
typedef struct _urcp_wl_mac_filter_t{
    uint32 mac_num;
    uint32 enable;
    uchar macaddr[MAX_MAC_NUM][MAC_LEN];
}urcp_wl_mac_filter_t;

typedef struct _urcp_wl_mac_filter_buf_t {
	misc_header_t    wl_mac_filter_header;
	urcp_wl_mac_filter_t wl_mac_filter;
} urcp_wl_mac_filter_buf_t;

typedef struct _urcp_wl_mac_filter_msg_t{
    urcp_general_msg_t general_msg;/*通用消息*/
    urcp_wl_mac_filter_t  wl_mac_filter_msg;
}urcp_wl_mac_filter_msg_t;

typedef struct _urcp_wl_mac_filter_struct_t {
	uint32 next_index;
	urcp_wl_mac_filter_buf_t  urcp_wl_mac_filter_buf[URCP_MISC_WL_MAC_FILTER_SIZE];
} urcp_wl_mac_filter_struct_t;

typedef struct _urcp_ap_conf_temp_msg_t{
    urcp_general_msg_t general_msg;/*通用消息*/
    urcp_ap_conf_temp_conf_t conf_temp_conf;
}urcp_ap_conf_temp_msg_t;

/* 远程AP基本设置消息结构体 */

/* AP 加入服务区 */
typedef struct _urcp_ap_join_sz_msg_t
{
    urcp_general_msg_t	general_msg;
    char sz_name[64];
    int16 rf;
    //urcp_ap_join_sz_t	ap_join_sz;
    //uint32 num;
}urcp_ap_join_sz_msg_t;

typedef struct _urcp_ac_set_mode_msg_t
{
    urcp_general_msg_t	general_msg;
    char ac_mode;
}urcp_ac_set_mode_msg_t;

/* AP 设置无线MAC地址过滤 */
typedef struct _urcp_ap_mac_filter_msg_t
{
    urcp_general_msg_t	general_msg;
    char name[64];
}urcp_ap_mac_filter_msg_t;

typedef struct _urcp_ap_mac_filter_buf_t
{
    misc_header_t	ap_mac_filter_header;
    char		ap_mac_filter[(UTT_INST_NAME_LEN+1)*6];
}urcp_ap_mac_filter_buf_t;

typedef struct _urcp_ap_mac_filter_struct_t
{
    uint32		    next_index;
    urcp_ap_mac_filter_buf_t   urcp_ap_mac_filter_buf[URCP_MISC_AP_MAC_FILTER_SIZE];
}urcp_ap_mac_filter_struct_t;
/*MAC Filter End*/


/*misc buffer读写操作辅助结构*/
typedef struct _misc_opt_assist{
    unsigned char* misc_buffer_header;/*misc buffer头部指针*/
    uint32 unit_size;/*buffer中每个结构体的大小*/
    uint32 misc_buffer_size;/*buffer共含多少结构体*/
}misc_opt_assist;
/*读取下发配置操作buffer*/
typedef struct _urcp_config_buf_t{
    misc_header_t  config_header;
}urcp_config_buf_t;
typedef struct _urcp_config_struct_t{
    uint32 next_index;       /*标记下一条记录保存位置*/
    urcp_config_buf_t urcp_config_buf_t[URCP_MISC_CONFIG_SIZE];
}urcp_config_struct_t;

typedef struct _urcp_misc_t{
    urcp_mirror_struct_t urcp_mirror_struct;
    urcp_cmd_alone_struct_t urcp_cmd_alone_struct;
    urcp_port_conf_struct_t urcp_port_conf_struct;
    urcp_sys_conf_struct_t urcp_sys_conf_struct;
    urcp_sys_info_struct_t urcp_sys_info_struct;
    urcp_ip_filter_struct_t urcp_ip_filter_struct;
    urcp_qos_rate_limit_struct_t urcp_qos_rate_limit_struct;
    urcp_storm_rate_limit_struct_t urcp_storm_rate_limit_struct;
    urcp_aggr_struct_t urcp_aggr_struct;
    urcp_vlan_struct_t urcp_vlan_struct;
    urcp_qvlan_pconf_struct_t urcp_qvlan_pconf_struct;
    urcp_rstp_lacp_struct_t urcp_rstp_lacp_struct;
    urcp_passwd_auth_struct_t urcp_passwd_auth_struct;
    urcp_passwd_struct_t urcp_passwd_struct;
    urcp_config_struct_t urcp_config_struct;/*读取配置，下发配置*/
    urcp_ap_basic_conf_struct_t urcp_ap_basic_conf_struct;
    urcp_ap_conf_temp_struct_t	urcp_ap_conf_temp_struct;
    urcp_ap_join_sz_struct_t urcp_ap_join_sz_struct;
	urcp_wl_mac_filter_struct_t urcp_mac_filter_struct;
	urcp_ap_mac_filter_struct_t urcp_ap_mac_filter_struct;
}urcp_misc_t;




#define MAX_SSID_NUM   16
#define MAX_SZ_NUM     16
#define STA_MAX_NUM 32

typedef struct _ssid_t {
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
    unsigned short max_clients;
    unsigned short sta_num;
	sta_info_t sta_info[STA_MAX_NUM];
}wl_ssid_t;

#if SZONE_AUTO_UPDATA
typedef struct _szone_info_t {
    unsigned char auto_updata;
    unsigned char szone_changed;
    unsigned char r_times;
    unsigned char count;
    unsigned char szone_num;
    char szone[UTT_SSID_NUM][UTT_SSID_LENTH];
}szone_info_t;
#endif

typedef struct _wl_info_t {
    uint32  ap_flag; 
    uint16  channel_head;		//信道变量的前半部分，若为0，则后半部分有效，否则无效
    uint16  channel_end;                //信道变量的后半部分，辅佐首部
    uint32  power;
    uint32  clients;
    uint32  max_clients;
    uint32  ssid_num;
    uint16 joined_sz_num;
    uchar cpu_rate;	    //CPU使用率
    uchar mem_rate;	    //内存使用率
    uint32 up_time;	    //运行时间
    uint16 bit_rate;	    //速率
    uint16 chan_width;  //频宽
    uint16 power_new;	    //功率
    uint16 beacon;
    uint32 tx_packets;
    uint32 tx_bytes;
    uint32 rx_packets;
    uint32 rx_bytes;
	
#if SZONE_AUTO_UPDATA
    szone_info_t szone_info;
#endif
    uint32 profile_type;  /*生效配置：本地or AC下发配置*/
    wl_ssid_t ssid_info[UTT_SSID_NUM];
#if(FEATURE_AP_MAC_FILTER == FYES)
    uint32  ap_mac_filter_num;
    char mfNames[UTT_SSID_NUM][UTT_SSID_LENTH];
#endif
}wl_info_t;

typedef struct _wl_info_5G_t {
    uint32  ap_flag;
    uint16  channel_head;
    uint16  channel_end;
    uint32  power;
    uint32  clients;
    uint32  max_clients;
    uint32  ssid_num;
    uint16 bit_rate;	    //速率
    uint16 chan_width;  //频宽
    uint16 power_new;	    //功率
    uint16 beacon;
    uint32 tx_packets;
    uint32 tx_bytes;
    uint32 rx_packets;
    uint32 rx_bytes;
    uint16 joined_sz_num;
#if SZONE_AUTO_UPDATA
     szone_info_t szone_info;
#endif
    wl_ssid_t ssid_info[UTT_SSID_NUM];
#if(FEATURE_AP_MAC_FILTER == FYES)
    uint32  ap_mac_filter_num;
    char mfNames[UTT_SSID_NUM][UTT_SSID_LENTH];
#endif
}wl_info_5G_t;

#if (HISTORY_FLOW_INFO == FYES)
/*AP流量统计-->开始*/

#define MAX_AP_STATIS_DAY 7
#define MAX_AP_STATIS_DATA 24
typedef struct _stat_t {
	uint64 rx;
	uint64 tx;
}stat_t;
typedef struct _wl_his_t {
	uint32 year;
	uint32 mon;
	uint32 day;
	uint32 hour_pos;//不能去掉，必须使用hour_pos用来控制day_pos的移动
	stat_t stat_hour[MAX_AP_STATIS_DATA];
}wl_his_t;
typedef struct _wl_his_stat_t {
	uint32 day_pos;
	wl_his_t stat_day[MAX_AP_STATIS_DAY];
}wl_his_stat_t;
typedef struct _wl_stat_t {
	uint32 min_pos;
	stat_t stat_min[60];	
}wl_stat_t;
typedef struct _wl_info_stat_t {
	uint32 rx_rate;
	uint32 tx_rate;
	uint32 rx_byte;
	uint32 tx_byte;
	uint32 last_rx_byte;
	uint32 last_tx_byte;
	wl_stat_t wl_stat_current;//最近一小时收发流量
	wl_his_stat_t wl_his_stat;//保存历史收发流量
}wl_info_stat_t;
/*AP流量统计-->结束*/
#endif


/*联动发现设备结构*/
typedef struct _urcp_dev{
    int32    used;
 //   int32    tb_index;       /*在表中的索引*/
    int32    tb_next;       /*在表中的索引*/
    uint32   ip;
    uint32   netmask;
    uint32   gateway;
    uint32   serial_no;        /*serial number*/
    uint32   port_num;        /*port number*/
    uint32   dhcp_en;        /*dhcp client*/
    uint32   dbm;            /* roaming */
    uint32   dbm_5g;            /* roaming */
    uint32   dev_type;       /*router , switch, fit ap, fat ap, and so on*/
    char     urcp_version;     /* 联动版本*/
    unsigned char    mac[MAC_LEN];
#if AP_WL_POWER_AUTO
    unsigned char    ra0_mac[MAC_LEN];
#endif
    char     dev_name[MAX_DEV_NAME_LEN];         /*device name*/
    char     user[UTT_INST_NAME_LEN+1];
    char     dev_model[DEV_TYPE_SIZE];      /*型号*/

    unsigned char    link_port;                        /*端口*/
    unsigned char    up_ports;                       /*活动端口数*/
    unsigned char    alarm_level;                    /*报警级别*/

    unsigned char    sw_version[SW_VERSION_SIZE];     /*软件版本信息*/
    unsigned char    hw_version[HW_VERSION_SIZE];     /*硬件版本信息*/
    uint32 load_balance; /* 负载均衡组*/
    int32 aged;
    wl_info_t wl_info;
    wl_info_5G_t wl_info_5G;
    uint32           vid;        /*ap所属vlan id*/
    int32           fromL3;      
#if (HISTORY_FLOW_INFO == FYES)
	uint32 	       updateStat;/*标记是否更行了AP流量信息*/
	uint32         updateRate;/*标记是否更新了AP上下行速率*/
	wl_info_stat_t acStat; /*存放AP的历史流量信息*/
#endif
}urcp_dev_t;
/**联动操作goahead 与 switch 交换数据缓存结构*/
typedef struct _urcp_swap_buf_t
{
    char urcp_conf[URCP_SWAP_CONF_SIZE];/*设置数据*/
}urcp_swap_buf_t;

typedef  struct _topoy_t{
    struct _topoy_t *first_child;    /*指向第一个孩子*/
    struct _topoy_t *next_sibling;   /*指向下一个兄弟*/
    struct _topoy_t *next;           /*指向下一个节点*/
    int32    tb_index;       /*在表中的索引*/
    int32    tb_next;       /*在表中的索引*/
    uint16   parent_index;    /*父节点在在拓扑结构同一层中的位置*/
    uint16   parent_port;     /*父节点端口号*/
    uint16   index;           /*在拓扑结构同一层的位置*/
    uint16   port;            /*与父节点相连的端口*/
    uint32   ip;
    unsigned char    parent_mac[MAC_LEN];
    unsigned char    mac[MAC_LEN];
    uint16   vlan_id;         /*父节点端口vid*/
    uint16   hop;
    uint16   virtual_hop;     /*上层虚拟节点数*/
    uint16   virtual_dev;     /*是否殉虚拟设备*/
    uint32   alarm_level;                    /*报警级别*/
    uint32   parent_aggr_mask;  /*父节点汇聚端口*/
    uint32   child_aggr_mask;    /*本节点汇聚端口号*/
    uint16   dev_model;     /*设备类型*/
    uint16   up_ports;    /*活动端口数*/
    char     dev_type[DEV_TYPE_SIZE];       /*型号*/
}topoy_t;

typedef struct _topology_list_t{
    int32   first_node;    /*指向某一层次的第一个节点*/
    topoy_t *last_node;     /*指向本层最后一个节点*/
    int32    count;          /*本层节点数*/
}topogy_list_t;

#define MAX_WL_STATIS_DAY 7
#define MAX_WL_STATIS_DATA 24

typedef struct _wl_data_t{
    unsigned long client_num;
}wl_data_t;

typedef struct _wl_histroy_t{
    unsigned short year;
    unsigned char mon;
    unsigned char day;
    wl_data_t wl_data[MAX_WL_STATIS_DATA];
}wl_histroy_t;

typedef struct _wl_statis_t{
    unsigned int total_of_sta;
    unsigned int current_index;
    wl_histroy_t wl_histroy[MAX_WL_STATIS_DAY];
}wl_statis_t;

#define MAX_LOG_LENGTH    120
#define NUM_OF_AP_LOG     200

typedef struct _apErrLog {
	char msg[MAX_LOG_LENGTH + 1];
} ap_err_log;

typedef struct _apLog {
	uint32_t current_msg_index;
	ap_err_log   msg[NUM_OF_AP_LOG];
} ap_log;

typedef struct _tb_entry_hdr_t{
    int16 used;
    int16 tb_next;
    uchar mac_addr[MAC_LEN];
    uint16   aged;
}tb_entry_hdr_t;
typedef struct _hash_entry_hdr_t{
    int16 idx; 
}hash_entry_hdr_t;
typedef struct _sta_entry_t{
    tb_entry_hdr_t hdr;
    uint32   con_time;		/*持续时间*/
    MACHTTRANSMIT_SETTING   phy;
    uchar    ap_mac[MAC_LEN]; /*所连AP的MAC地址*/
    char     rssi;
    char     ssid_idx;       /*AP的SSID索引*/
    uint16   flag;
    uint32   rate;
    uint64   rx_bytes;
    uint64   tx_bytes;
    uint32   rx_rate;
    uint32   tx_rate;
    uint32   ip;
    uchar    host_name[32];
}sta_entry_t;

#if (STA_ONOFF_LOG == FYES)
#define MAX_STA_ALIAS_LEN 15
#define MAX_STA_LOG_HASH_SIZE 128
#define MAX_STA_LOG_TB_SIZE 4096
#define MAX_STA_LOG_POOL 4096	    /*日志节点的个数*/
#define MAX_UP_DOWN_SIZE 0x3FFFu    /*14位能表示的最大数字*/
#define LOG_UP_DOWN_LIMIT   60u	    /*频繁上下线判断时间标准*/
#define STA_LOG_FLAG     0x0u
#define USB_STA_LOG_PATH "staLog/"          /*U盘中文件的路径*/
#define LOG_1_DAY        86400u
#define LOG_3_DAY        259200u
#define LOG_7_DAY        604800u
#define LOG_1_MONTH      2628288u	/*30.42天*/
#define LOG_2_MONTH      5256576u	/*30.42*2天*/
#define log_1_YEAR	 31539456u	/*365天*/

typedef struct _log_sta_mac
{
    uchar sta_mac[MAC_LEN];
}log_sta_mac;
typedef struct _urcp_del_log_msg_t
{
    unsigned short mode;	    //1删除两种模式,0删除全部
    unsigned short staNum;
    log_sta_mac del_log_list[0];
}urcp_del_log_msg_t;

typedef struct _sta_log_file_head_t
{
    uint32 head;	/*文件头*/
    uint16 flag;	/*标志*/
    uint16 head_len;	/*头长度*/
    uint16 one_log_size;    /*一条日志的大小*/
    uint16 log_num;	    /*文件中日志的条数*/
    uint32 free;	    /*4B的保留字节*/	
}log_file_head_t;

typedef struct _sta_log_file_info_t
{
    uchar ap_mac[MAC_LEN];
    uchar sta_mac[MAC_LEN];
    uchar  staAlias[MAX_STA_ALIAS_LEN+1]; /*设置该sta的备注信息，可以由用户输入*/
    uint32 up_time;
    uint32 duration;
    uint16 upload; /*上传流量,高2个bit表示单位：0-B，1-K，2-M，3-G，低14bit表示数值*/
    uint16  download; /*同上*/
}log_file_info_t;

typedef struct  _sta_log_t{
    tb_entry_hdr_t hdr;
    uchar  staAlias[MAX_STA_ALIAS_LEN+1]; /*设置该sta的备注信息，可以由用户输入*/
    uint32 count;  /*此STA日志条数*/
    uint32 logPoolIdx; /*指向保存日志的节点*/
}sta_log_t;

typedef struct _log_info_t{
    uint16 used;	/*0空节点， 1sta在线节点，2，sta已离线节点，完整节点*/
#if 0
    uint16 year;  /*时间*/
    uchar month;
    uchar day;
#endif
    uchar apMac[MAC_LEN];
    uint32 upTime; /*上线时间*/ 
    uint32 duration; /*sta此次上线到下线的持续的时长（秒为单位）、或者短时内上线次数*/
    uint16 upload; /*上传流量,高2个bit表示单位：0-B，1-K，2-M，3-G，低14bit表示数值*/
    uint16  download; /*同上*/
    uint32  staIdx;  /*该节点所属的STA所在sta_log索引*/
    uint32  earlierIdx;  /*所有STA中，早一点的一条日志索引+1，0表示本条是最早的日志了*/
    uint32  newIdx;  /*所有STA中，比本条日志新一点的日志索引+1，0表示本条是最新的*/
    uint32  next; /*该STA的下条日志索引+1，当used为0时指向下一个空闲节点索引+1*/
}log_info_t;

typedef struct _log_pool_t{
    int32  firstFree;  /*第一个空闲log_buffer索引+1，0表示没有空闲节点*/
    int32  lastestIdx;/*最新一条日志索引+1*/
    int32  earliestIdx;/*最早的一条日志索引+1*/
    int32  ago1day;/*一天内的最早一条日志索引+1*/ 
    int32  ago3day;/*3天内的最早一条日志索引+1*/ 
    int32  ago7day;/*7天内的最早一条日志索引+1*/ 
    int32  ago1month;/*一个月内的最早一条日志索引+1*/ 
    int32  ago2month;/*2个月内的最早一条日志索引+1*/ 
    int32  ago1year;/*1年内的最早一条日志索引+1*/ 
    log_info_t logBuffer[MAX_STA_LOG_POOL];
}log_pool_t;
#endif

typedef struct _urcp_shm_t{
    uint32 dev_tb_hash[DEV_HASH_SIZE]; /*hash head*/
    urcp_dev_t dev_table[DEV_TABLE_SIZE]; /*联动发现设备表*/
    urcp_misc_t  urcp_misc;    /*存放联动读取到的其它设备信息*/
    urcp_swap_buf_t urcp_swap_buf;/*联动操作goahead与switch交换空间*/
    ap_log  ap_msg;/*存放AP异常信息*/

#if AP_WL_POWER_AUTO
    int32 ra0_rssi[DEV_TABLE_SIZE][DEV_TABLE_SIZE]; /*存放周围AP rssi之间的影响*/
#endif

#if 0
   // topoy_t topoy_table[MAX_TOPOY_DEV];/*拓扑发现设备表*/
   // topogy_list_t topoy_list[MAX_TOPOY_HOP];/*供页面显示结构*/
#endif
    wl_statis_t wl_statis;
    hash_entry_hdr_t sta_tb_hash[STA_TB_HASH_SIZE];
    sta_entry_t sta_tb[STA_TB_SIZE];
#if (STA_ONOFF_LOG == FYES)
    hash_entry_hdr_t sta_log_hash[MAX_STA_LOG_HASH_SIZE]; /*定义为128*/
    sta_log_t  sta_log[MAX_STA_LOG_TB_SIZE ];/*定义为4096*/
    log_pool_t  logPool;
#endif
}urcp_shm_t;


#if (UTT_AP_801 == FYES)
typedef struct image_header {
	    uint32_t imageLen;
		uint32_t imageCrc;
		uint32_t arch;
		uint32_t fsLen;
        uint32_t loadAddr;
		uint32_t systemVer;
        uint32_t entryAddr;
	    uint32_t os;
	    uint32_t imageComp;
		uint8_t  name[20];
        uint32_t kernelLen;
        uint32_t kernelCrc;
} image_header_t;
#else
typedef struct image_header {
	uint32_t imageLen;
	uint32_t imageCrc;
	uint32_t arch;
	uint32_t fsLen;
	uint32_t os;
	uint32_t systemVer;  /* 前24位是软件版本，后16位是硬件版本。按88844比例分配，对应 1.1.1  1.1  */
	uint32_t imageComp;
        uint32_t loadAddr;
        uint32_t entryAddr;
        uint8_t  name[20];
        uint32_t kernelLen;
        uint32_t kernelCrc;
} image_header_t;
#endif
#endif
