
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
#define URCP_ROUTER         1u  /*���ܱ����ֺͲ���*/

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
 *����Ĵ���״̬
 *error_status
 */
#define URCP_OK                 0x0000u
#define URCP_ANY_ERROR          0x0001u
#define URCP_CMD_UNKNOW         0x0002u
#define URCP_CMD_IGNORE         0x0005u
/*���ش����256��ʼ*/
#define URCP_FATAL_ERROR        0x0100u
#define URCP_LENGTH_ERROR       0x0200u  /*���ȴ���*/
#define URCP_AUTH_ERROR         0x0300u  /*�������*/


/**end*/
/*�����ײ�flag����*/
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
 *�����򵥻Ự����
 */
#define MAX_URCP_SESSION       32
#define PORT_NULL     0u

#define CPU_PORT     25
#define POLL_TIME 100
#define URCP_CMD_TIME_OUT      (2000/POLL_TIME) /*Ĭ�ϳ�ʱʱ��,��urcp_timer��ִ�д���*/
#define URCP_CMD_TIME_OUT2      (8000/POLL_TIME) /*Ĭ�ϳ�ʱʱ��,��urcp_timer��ִ�д���*/
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
 *�����Ự����
 */
typedef enum{
    SESSION_TYPE_HELLO=1,      /*�յ��ظ��󣬸����ͻỰ�������Ͻ�����Ҫ�ȴ���ʱ, ���������ֺ����˷���*/
    SESSION_TYPE_GET,          /*�յ��ظ���ûỰ�ͽ���,get����*/
    SESSION_TYPE_SET,          /*�յ��ظ���ûỰ�ͽ���, set ����*/
}session_type_t;

#define SESSION_SINGLE    0x01u
/*���ݽṹ����*/
typedef struct _urcp_hdr{
    uint16  version;    /*ֻҪǰ1�ֽ�*/
    uint16  type;       /*��������*/
    uint16  flag;
    uint16  auth_key;  /*6*/
    uint32  ip;         /*8*/ 
    uchar   password[MAX_PASSWORD_LEN]; /*12*/
    uint16  request_id;/*30*/
    uint16  error_status;   /*����״̬*/
    uchar   cmd_no;         /*��������*/
    uchar   next_header;    /*35 ��һ���ײ�*/
}urcp_hdr_t;


/*trap ���Ľṹ*/
typedef struct _urcp_trap_hdr{
    uint16  version; /*Э��汾*/
    uint16  type;   /*��������*/
    uint16  flag;
    uint16  auth_key;
    uint32  ip;   
    uint16  level;  /*��������*/
    uint16  code;   /*�����¼�*/
}urcp_trap_hdr_t;

/*����ṹ*/
typedef struct _cmd_hdr{
    uint32 type;  /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
    uint16 length;
    uint16 cmd_status;
}cmd_hdr_t;
typedef struct _cmd_hdr_gen_t{
    uint32 type;  /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
    uint16 length;
    uint16 cmd_status;
    uint32 value;
}cmd_hdr_gen_t;

typedef struct _cmd_hdr_sta_info_t{
    uint32 type;  /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
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
    uint32 type;  /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
    uint16 length;
    uint16 cmd_status;
//    uint32 table_offset;
    uint16 sta_num;
    uint16 table_offset;
    uint16 entry_num;
    uint16 entry_size;
    cmd_sta_stat_t sta_entry[0];
} cmd_hdr_sta_info_auto_t;
 
/*HCAP����ͳ�ƹ���*/
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
    uchar*  raw;        /*ָ��ԭʼ��̫��֡ͷ��*/
    urcp_hdr_t*  urcp_header;       /*ָ����������*/
    uchar*  cmd_buff;       /*ָ�����������еľ����������*/
    uchar   mac[MAC_LEN];
    uint16  vid;
    uint32  source_port;
    uint32  length;
    uint32  packet_type;
    uint32  certified;/*�Ƿ�ͨ��������֤*/
    uint32  session_id;
    uint32  error_state; /*������ͷʱ�������Ĵ�����Ϣ*/
    int32  fromL3;
    uint16  rf;
}urcp_infor_t;

/*�Ự�ṹ*/
typedef struct _urcp_session{
    uint16 req_id;
    urcp_status_t status; 
    int32 timer;
    session_type_t session_type;
    void (*cb_func)(uint16 reqid, session_type_t type, urcp_status_t state);/**/
}urcp_session_t;

/*��������urcp�����״̬*/
typedef struct _urcp_state{
    uint16 req_id_counter; /*��������һ����ʹ�õ�id*/
    urcp_session_t session[MAX_URCP_SESSION];
}urcp_state_t;

/*mac��ַ��λ���ṹ*/
typedef struct _mac_location{
    uchar target_mac;      /*Ҫ������Ŀ��mac*/
    uchar hop;       /*·������*/
    uchar target_port;     /*Ŀ��˿�*/
    uchar port_enable;     /*����or �ر�*/
    uchar single_mac;         /*�ö˿��Ƿ�ֻ��һ��mac��ַ*/
}mac_location_t;

#ifdef URCP_WEB
/*ͳ�ƶ˿ڵ�mac��*/
typedef struct _port_mac_number{
    uint32 num[PORT_COUNT];
}port_mac_number_t;
#endif



typedef void (*cb_fp)(uint16 reqid, session_type_t type, urcp_status_t state);
extern uchar mac_addr_cooperate[MAC_LEN];
extern uchar mac_addr_broadcast[MAC_LEN];
/*
 *��������
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

/* �������� */

/* AP-AC���� */
typedef struct _cmd_discover_t {
    uint32 type;    /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
    uint16 length;
    uint16 cmd_status;
    uint32 send_flag;
    uint32 recv_flag;
}cmd_discover_t;

/* AC��AP��Ȩ */
typedef struct _cmd_ap_auth_t{
     uint32 type;	    /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
     uint16 length;
     uint16 cmd_status;
     uint32 auth;	    /*authΪ1��ʾ��Ȩ��Ϊ2��ʾ�����Ȩ*/
}cmd_ap_auth_t;

/* AP��Ȩrequest */
typedef struct _cmd_ap_auth_req_t{
     uint32 type;	    /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
     uint16 length;
     uint16 cmd_status;
     uint32 serial_no;	    /* AP serial No.*/
}cmd_ap_auth_req_t;

/* AP������� */
typedef struct _cmd_ap_update_t{
    uint32 type;	    /*cmd_hdr�ṹ�ͺ�������ݲ��ֵĳ���*/
    uint16 length;
    uint16 cmd_status;
}cmd_ap_update_t;


typedef struct _urcp_ap_sysinfo_t {
    int32 product_sn;
    char  sw_version[32];
    char  hw_version[16];
    char  product_type[16];
}urcp_ap_sysinfo_t;


/* AP ��Ȩ */
#define ALLOW_AUTH	1u  /*������Ȩ*/
#define CANCEL_AUTH	2u  /*�����Ȩ*/

#ifdef URCP_AP

/* AP״̬ */
typedef enum {
AP_STATE_INIT=0,		    /*��ʼ״̬*/
AP_STATE_DISCOVERY,	    /*����״̬*/
AP_STATE_UNAUTH,	    /*δ��Ȩ״̬*/
AP_STATE_AUTH,		    /*����Ȩ״̬*/
AP_STATE_SOFEUPDATE_REQ,	    /*�������״̬*/
AP_STATE_SOFEUPDATE,	    /*�������״̬*/
AP_STATE_CONFUPDATE_REQ,    /*���ø���״̬*/
AP_STATE_CONFUPDATE,	    /*���ø���״̬*/
AP_STATE_UNMANGEDED,	    /*���ɹ���״̬*/
AP_STATE_NORMAL,	    /*����״̬*/
}ap_state_t;

typedef struct _ac_info_t{
    uint32 ip;
    uchar mac[6];
    uchar acMode;
    int32  fromL3;
}ac_info_t;

typedef struct _ap_info_t {
    ap_state_t state;	    /*ap״̬*/
    ac_info_t ac_infor;    /*ac��Ϣ*/
    int32 joined;           /*�Ƿ���AC*/
    uint32 current_timer;
    uint32 retransmissionCount;
    uint32 ap_auth;	    /*AC�Ƿ��Ѿ���Ȩ*/
    uint16 update_id;	    /*����Զ����������Զ��������ûỰID*/
    urcp_status_t update_status; /*��������������ù�����ɵ�״̬*/
}ap_info_t;

/*config backup state*/
typedef enum {
    NEED_NOT_BACKUP = 0,
    NEED_BACKUP,
}bakup_state_t;

#if (CAPWAP_CONFIG_AP == FYES)
typedef enum {
    NEED_NOT_CHANGE_IP = 0,
    NEED_CHANGE_STATIC_IP,	    //�л�Ϊ��̬IP
    NEED_CHANGE_DHCP_IP,	    //�л�Ϊ��̬IP
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
