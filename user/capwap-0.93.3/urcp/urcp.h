
#ifndef __URCP_H__
#define __URCP_H__

#include "common.h"

#ifdef URCP_WEB
//#include <urcp_ipc.h>
#endif

#define ETH_P_URCP  0x8899

#define DEBUG_URCP 1
#if DEBUG_URCP
#define URCP_DEBUG(args) printf args
#else
#define URCP_DEBUG(args) 
#endif

extern FILE*ffp;
#define URCP_FDEBUG(fmt, args...) 

#if 0
do { \
if(dbg==1) { \
ffp = fopen("/udbg","a+"); \
if(ffp!=NULL) \
{ \
    fprintf(ffp, fmt, ##args); \
    fclose(ffp); \
} \
} \
}while(0)
#endif
#define DBCKS 1
#if DBCKS
#define DBCK(fmt, args...) \
			{ \
				printf("%s, %d, %s(), "fmt, __FILE__, __LINE__, __func__, ##args); \
				printf("\n"); \
			}
#else
#define DBCK(fmt, args...)
#endif


#define ERROR_URCP 1
#if ERROR_URCP
#define URCP_ERROR(args) printf args
#else
#define URCP_ERROR(args) 
#endif


#define URCP_SWITCH         0u 
#define URCP_ROUTER         1u  /*不能被发现和操作*/

#define URCP_DEVICE        URCP_ROUTER 

#define URCP_VERSION           2u 
#define RRCP_VERSION           1u 
#define VERSION_MASK           0x0Fu 

#define AUTH_KEY_OFFSET        20u

#define ETH_URCP_OFFSET        14u


#define RRCP_AUTH_KEY          0x2379u 
#define URCP_AUTH_KEY          0x237Bu
#define URCP_AUTH_KEY_FRL      0x237cu

#if (UTT_TOMAX == FYES)
#undef URCP_AUTH_KEY
#define URCP_AUTH_KEY          0x135Bu
#endif
/*
 *
 *命令的错误状态
 *error_status
 */
#define URCP_OK                 0x0000u
#define URCP_ANY_ERROR          0x0001u
#define URCP_CMD_UNKNOW         0x0002u
#define URCP_CMD_IGNORE         0x0005u
/*严重错误从256开始*/
#define URCP_FATAL_ERROR        0x0100u
#define URCP_LENGTH_ERROR       0x0200u  /*长度错误*/
#define URCP_AUTH_ERROR         0x0300u  /*密码错误*/


/**end*/
/*报文首部flag意义*/
#define FLAG_NO_PASSWD             0x00u
#define FLAG_PASSWD             0x01u

#define URCP_BUF_SIZE          1522u
#define MAX_CMD_LEN            (URCP_BUF_SIZE - ETH_URCP_OFFSET - sizeof(urcp_hdr_t) - sizeof(uint32))
/*
 *
 * URCP PDU type
 *
 **/
#define URCP_GET              1u
#define URCP_GET_REPLY        2u
#define URCP_SET              3u
#define URCP_SET_REPLY        4u
#define URCP_TRAP             5u
#define URCP_TOPOY            6u
#define URCP_TFTP             7u


/**************************** AC - AP *****************************/

#define MAX_PASSWORD_LEN          18u


#define PACKET_HOST        0
#define PACKET_MULTICAST   1

/*
 *联动简单会话控制
 */
#define MAX_URCP_SESSION       32
#define PORT_NULL     0u

#define CPU_PORT     25
#define POLL_TIME 100
#define URCP_CMD_TIME_OUT      (2000/POLL_TIME) /*默认超时时间,即urcp_timer的执行次数*/
#define URCP_CMD_TIME_OUT2      (8000/POLL_TIME) /*默认超时时间,即urcp_timer的执行次数*/
#define AP_AGE_TIME_MANU_MS  150000u
#define AP_AGE_TIME_AUTO_MS   25000u
#define AP_DISCOVER_TIME_MS   40000u
#define AP_STATE_TIME_MS    1000
#define AP_CONFIG_BACKUP_TO_AC_TIME_MS 20000
#define AP_AUTH_TIME_MS  30000
#define AP_HEART_BEAT_TIME_MS    20000
#define AP_TIMER_DELAY_MS    10000
#if (CAPWAP_CONFIG_AP == FYES)
#define AP_IP_CHANGE_TIME_MS AP_CONFIG_BACKUP_TO_AC_TIME_MS
#define AP_IP_CHANGE_DEALY_MS 6000
#endif

#if AP_WL_POWER_AUTO
#define AP_POWER_AUTO_TIME_MS  30000u
#define AC_POWER_AUTO_TIME_MS  900000u
#endif

#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
#define AC_AUTO_COLLECT_CLTS_TIME_MS  180000u
#endif
extern int gSocketL2;
extern int gSocketL3;
/*
 *联动会话类型
 */
typedef enum{
    SESSION_TYPE_HELLO=1,      /*收到回复后，该类型会话不会马上结束，要等待超时, 如联动发现和拓扑发现*/
    SESSION_TYPE_GET,          /*收到回复后该会话就结束,get操作*/
    SESSION_TYPE_SET,          /*收到回复后该会话就结束, set 操作*/
}session_type_t;

#define SESSION_SINGLE    0x01u
/*数据结构定义*/
typedef struct _urcp_hdr{
    uint16  version;    /*只要前1字节*/
    uint16  type;       /*报文类型*/
    uint16  flag;
    uint16  auth_key;  /*6*/
    uint32  ip;         /*8*/ 
    uchar   password[MAX_PASSWORD_LEN]; /*12*/
    uint16  request_id;/*30*/
    uint16  error_status;   /*错误状态*/
    uchar   cmd_no;         /*命令条数*/
    uchar   next_header;    /*35 下一个首部*/
}urcp_hdr_t;


/*trap 报文结构*/
typedef struct _urcp_trap_hdr{
    uint16  version; /*协议版本*/
    uint16  type;   /*报文类型*/
    uint16  flag;
    uint16  auth_key;
    uint32  ip;   
    uint16  level;  /*报警级别*/
    uint16  code;   /*报警事件*/
}urcp_trap_hdr_t;

/*命令结构*/
typedef struct _cmd_hdr{
    uint32 type;  /*cmd_hdr结构和后面的数据部分的长度*/
    uint16 length;
    uint16 cmd_status;
}cmd_hdr_t;
typedef struct _cmd_hdr_gen_t{
    uint32 type;  /*cmd_hdr结构和后面的数据部分的长度*/
    uint16 length;
    uint16 cmd_status;
    uint32 value;
}cmd_hdr_gen_t;

typedef struct _cmd_hdr_sta_info_t{
    uint32 type;  /*cmd_hdr结构和后面的数据部分的长度*/
    uint16 length;
    uint16 cmd_status;
    uint32 value;
    sta_status  sta_status;
    uchar  sta_mac[MAC_LEN];
} cmd_hdr_sta_info_t;

typedef struct _cmd_sta_stat_t{
    uchar addr[MAC_LEN];
    uint16 phy;
    uint32 con_time;
    char  rssi;
    uchar  ssid_idx;
    uint16 rate;
    uint32 rx_bytes;
    uint32 rx_rate;
    uint32 tx_bytes;
    uint32 tx_rate;
} cmd_sta_stat_t;
typedef struct _cmd_hdr_sta_info_auto_t{
    uint32 type;  /*cmd_hdr结构和后面的数据部分的长度*/
    uint16 length;
    uint16 cmd_status;
//    uint32 table_offset;
    uint16 sta_num;
    uint16 table_offset;
    uint16 entry_num;
    uint16 entry_size;
    cmd_sta_stat_t sta_entry[0];
} cmd_hdr_sta_info_auto_t;
 
/*HCAP流量统计功能*/
typedef struct _cmd_hdr_stat_t{
	uint32 type;
	uint16 length;
	uint16 cmd_status;
	uint32 rx_byte;
	uint32 tx_byte;
}cmd_hdr_stat_t;

typedef struct _cmd_hdr_rate_t{
	uint32 type;
	uint16 length;
	uint16 cmd_status;
	uint32 rx_rate;
	uint32 tx_rate;
}cmd_hdr_rate_t;

typedef struct _sta_info_auto_t{
    uchar ssid[32];
    uint16 info_length;
    uint16 sta_num;
    uint16 sta_mac_offset;
} sta_info_auto_t;


typedef struct _urcp_infor{
    uchar*  raw;        /*指向原始以太网帧头部*/
    urcp_hdr_t*  urcp_header;       /*指向联动报文*/
    uchar*  cmd_buff;       /*指向联动报文中的具体操作命令*/
    uchar   mac[MAC_LEN];
    uint16  vid;
    uint32  source_port;
    uint32  length;
    uint32  packet_type;
    uint32  certified;/*是否通过密码验证*/
    uint32  session_id;
    uint32  error_state; /*分析包头时，检查出的错误信息*/
    int32  fromL3;
    uint16  rf;
}urcp_infor_t;

/*会话结构*/
typedef struct _urcp_session{
    uint16 req_id;
    urcp_status_t status; 
    int32 timer;
    session_type_t session_type;
    void (*cb_func)(uint16 reqid, session_type_t type, urcp_status_t state);/**/
}urcp_session_t;

/*保存所有urcp请求的状态*/
typedef struct _urcp_state{
    uint16 req_id_counter; /*发出的下一个包使用的id*/
    urcp_session_t session[MAX_URCP_SESSION];
}urcp_state_t;

/*mac地址定位包结构*/
typedef struct _mac_location{
    uchar target_mac;      /*要搜索的目标mac*/
    uchar hop;       /*路径跳数*/
    uchar target_port;     /*目标端口*/
    uchar port_enable;     /*开启or 关闭*/
    uchar single_mac;         /*该端口是否只有一个mac地址*/
}mac_location_t;

#ifdef URCP_WEB
/*统计端口的mac数*/
typedef struct _port_mac_number{
    uint32 num[PORT_COUNT];
}port_mac_number_t;
#endif



typedef void (*cb_fp)(uint16 reqid, session_type_t type, urcp_status_t state);
extern uchar mac_addr_cooperate[MAC_LEN];
extern uchar mac_addr_broadcast[MAC_LEN];
/*
 *函数声明
 *
 */
extern int urcp_build_reply_header(urcp_infor_t* infor, uchar buffer[URCP_BUF_SIZE]);
extern int32 urcp_send(const uint32 source_port, uchar*  frame, uint32  length
#ifdef URCP_AC
, int32 fromL3
#endif
);
extern int32 urcp_send_by_vlan(uchar* frame, uint32 length);
extern int32 session_alloc(session_type_t type, uint32 flag, int32 time_out, uint16 *request_id, cb_fp cb);
extern void urcp_init(void);
extern void urcp_reset(void);
extern void urcp_timer(void);
extern int32 check_request_id(urcp_infor_t* urcp_infor);
extern uint16 add_cmd(cmd_hdr_t* cmd_buffer, uint32 type);
extern void session_set(urcp_infor_t* urcp_infor, urcp_status_t state);
extern void urcp_unknow(urcp_infor_t* urcp_infor);


/*********************************** AC - AP ***********************************/

/* 发现类型 */

/* AP-AC发现 */
typedef struct _cmd_discover_t {
    uint32 type;    /*cmd_hdr结构和后面的数据部分的长度*/
    uint16 length;
    uint16 cmd_status;
    uint32 send_flag;
    uint32 recv_flag;
}cmd_discover_t;

/* AC向AP授权 */
typedef struct _cmd_ap_auth_t{
     uint32 type;	    /*cmd_hdr结构和后面的数据部分的长度*/
     uint16 length;
     uint16 cmd_status;
     uint32 auth;	    /*auth为1表示授权，为2表示解除授权*/
}cmd_ap_auth_t;

/* AP授权request */
typedef struct _cmd_ap_auth_req_t{
     uint32 type;	    /*cmd_hdr结构和后面的数据部分的长度*/
     uint16 length;
     uint16 cmd_status;
     uint32 serial_no;	    /* AP serial No.*/
}cmd_ap_auth_req_t;

/* AP更新相关 */
typedef struct _cmd_ap_update_t{
    uint32 type;	    /*cmd_hdr结构和后面的数据部分的长度*/
    uint16 length;
    uint16 cmd_status;
}cmd_ap_update_t;


typedef struct _urcp_ap_sysinfo_t {
    int32 product_sn;
    char  sw_version[32];
    char  hw_version[16];
    char  product_type[16];
}urcp_ap_sysinfo_t;


/* AP 授权 */
#define ALLOW_AUTH	1u  /*允许授权*/
#define CANCEL_AUTH	2u  /*解除授权*/

#ifdef URCP_AP

/* AP状态 */
typedef enum {
AP_STATE_INIT=0,		    /*初始状态*/
AP_STATE_DISCOVERY,	    /*发现状态*/
AP_STATE_UNAUTH,	    /*未授权状态*/
AP_STATE_AUTH,		    /*已授权状态*/
AP_STATE_SOFEUPDATE_REQ,	    /*软件更新状态*/
AP_STATE_SOFEUPDATE,	    /*软件更新状态*/
AP_STATE_CONFUPDATE_REQ,    /*配置更新状态*/
AP_STATE_CONFUPDATE,	    /*配置更新状态*/
AP_STATE_UNMANGEDED,	    /*不可管理状态*/
AP_STATE_NORMAL,	    /*正常状态*/
}ap_state_t;

typedef struct _ac_info_t{
    uint32 ip;
    uchar mac[6];
    uchar acMode;
    int32  fromL3;
}ac_info_t;

typedef struct _ap_info_t {
    ap_state_t state;	    /*ap状态*/
    ac_info_t ac_infor;    /*ac信息*/
    int32 joined;           /*是否发现AC*/
    uint32 current_timer;
    uint32 retransmissionCount;
    uint32 ap_auth;	    /*AC是否已经授权*/
    uint16 update_id;	    /*软件自动升级或者自动下载配置会话ID*/
    urcp_status_t update_status; /*下载软件或者配置过程完成的状态*/
}ap_info_t;

/*config backup state*/
typedef enum {
    NEED_NOT_BACKUP = 0,
    NEED_BACKUP,
}bakup_state_t;

#if (CAPWAP_CONFIG_AP == FYES)
typedef enum {
    NEED_NOT_CHANGE_IP = 0,
    NEED_CHANGE_STATIC_IP,	    //切换为静态IP
    NEED_CHANGE_DHCP_IP,	    //切换为动态IP
}ip_state_t;
#endif

#if AP_WL_POWER_AUTO
#define MAX_AROUND_SSID 128
#define SSID_HASH_SIZE 48

typedef struct _around_ssid_info_t {
    uchar action;
    int32 tb_next;
    uchar ra_mac[MAC_LEN];
    int32 rssi;
    uchar aged;
} around_ssid_info_t;

typedef struct _around_env_ssid_info_t {
    uint32 ssid_tb_hash[SSID_HASH_SIZE];
    around_ssid_info_t ssid_info[MAX_AROUND_SSID];
} around_env_ssid_info_t;
#endif

#define MAX_CONF_ITEM	6u
extern void change_state(ap_state_t newstate);
extern ap_state_t read_state(void);
extern void set_ap_authorize(uint32 ap_auth);
extern void init_ap_state(void);
extern void ac_ip_mac_set(uchar *ac_mac, uint32 ac_ip, int32 fromL3);
extern int32 urcp_send_ap_auth_request(void);
extern int urcp_send_update_fireware_reqest(void);
extern int urcp_send_config_update_reqest(void);

extern void config_backup_state(bakup_state_t enable);
extern void config_backup_timer_out_init(void);
extern void config_backup_timer(void);

#endif

#ifdef URCP_AC
typedef struct _ac_management_info_t {
    uint32  max_ap_auth; 
    uchar  scan_mode; 
    uchar  scan_all; /*scan all the wifi devices*/
}ac_management_info_t ;
#if AP_WL_POWER_AUTO
typedef enum{
    POWER_STAGE_INIT,
    POWER_STAGE_SURVEY,
    POWER_STAGE_COLLECT,
    POWER_STAGE_ADJUST,
}wl_power_stage_t;
#endif

#ifdef AP_CONFIG_GET_ONE_VLAN
#define MAX_AP_VLAN 4096
typedef struct _vlan_ap_info_t {
	vid_t vid;
	urcp_ap_sysinfo_t ap_info;
} vlan_ap_info_t;
#endif

extern int32 permit_authorized(uint32 serial_no);
#endif
extern int urcp_buid_header(unsigned char buffer[1], unsigned char dst_mac[MAC_LEN], uint16 type, uint16 id, unsigned char *passwd);
extern int urcp_buid_header_with_8021q(unsigned char buffer[1], unsigned char dst_mac[MAC_LEN], uint16 type, uint16 id, unsigned char *passwd);

extern void urcp_buid_tftp_header(uchar buffer[URCP_BUF_SIZE], uchar dst_mac[MAC_LEN], uint16 type, uint16 id, uchar *passwd,uint16 vid);

#ifdef URCP_AP
extern ap_info_t ap_infor;
#endif
/*********************************** AC - AP ***********************************/
#endif
