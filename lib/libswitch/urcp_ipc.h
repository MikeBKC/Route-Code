#ifndef __URCP_SHM_H__
#define __URCP_SHM_H__
#include <linux/switch/swioc_common.h>
#include <stdint.h>
#include <mib.h>
#include "../../config/user_oid.h"

#define MAX_STATUS_SIZE 10

#define MAX_DEV_NAME_LEN          12u  /*�豸��*/
#define MAX_USER_NAME_LEN         12u  /*�û���*/
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
#define GET_AP_BASIC_CONFIG	0x92u	/* ����AP�������� */
#define SET_AP_BASIC_CONFIG	0x93u	/* ����AP�������� */
#define SET_IMAGE   0x94u   /* Set AP image */
#define START_AP_SCAN_CLT 0x95u
#define SET_CONFIG_TEMPLATE 0x96u	/*����ģ��*/
#define URCP_SET_AP_JOIN_SZ	0x97u	/* AP��������� */
#define URCP_GET_AP_JOIN_SZ	0x98u	/* AP��������� */

#define REQUEST_AC_CHANGE   0x99u   /* AP����������AC�޸ı����AP��config */

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


/*�����·���������id*/
#define SYS_CONF         1u
#define VLAN_CONF        2u
#define AGGR_CONF        3u
#define PORT_CONF        4u
#define RATELIMIT_CONF   5u
#define ARP_CONF         6u
/*end �����·���������id*/


#define USB_LOG_PATH        "syslog/"  /*u���б�����־Ŀ¼*/
#define TFTP_LOG_ROOT      "/tmp/urcp/logs/"/*��־�ļ����Ŀ¼*/
#define RAM_LOG_PATH       "/tmp/syslog/"

#define USB_CONFIG_PATH     "configs/" /*u����������������ļ���Ŀ¼*/
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
 * �������  
 * ���ṹ��С����
 *
 **/
#define DEV_TYPE_SIZE          12u  /*�豸�ͺ�����*/
#define SW_VERSION_SIZE        32u  /*����汾��Ϣ*/
#define HW_VERSION_SIZE        16u  /*����汾��Ϣ*/
#define DEV_DESCRIP_SIZE       2u  /*�豸������Ϣ*/

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


/*misc buffer��������궨��
 *  *��Щ����Ҫ�����������misc buffer���Լ��ٷ��ʳ�ͻ
 *   *����һ��misc buffer�����϶����˶����ͬ�Ĳ��������趨���Ӧ�ĺ�
 *    * */
#define MISC_GET_GENERAL       0u  /*��������һ������µĶ�ȡ�������е� ��ȡ����*/
#define MISC_SET_GENERAL       1u  /*��������һ������µĶ�ȡ�������е� ���ò���*/

#define MISC_GET_VLAN  2u  /*����Զ���豸ʵ��vlan���ͻ�ȡvlan��Ϣ*/
#define MISC_GET_PVLAN 3u  /*��ȡport vlan��Ϣ������Զ���豸��ǰvlan����*/
#define MISC_SET_PVLAN 4u  /*����port vlan*/
#define MISC_GET_LOG   5u  /*��ȡ��־*/
#define MISC_GET_IMAGE 6u  /*get the image file*/

/*���º��������ֵ���������*/
#define MISC_REBOOT    2u
#define MISC_RESET     3u
#define MISC_SAFEBIND  4u

/*end misc buffer��������궨��*/

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

/*����������ȡ�ĸ�����Ϣ*/
#define MISC_MSG_LEN   16u

#define MAX_URCP_PORT  32u /*�����յ�����Ч�˿ں����ֵ*/
#define MIN_URCP_PORT  1u  /*�����յ�����Ч�˿ں���Сֵ*/
#define MAX_URCP_PORT_NUM 32u
#define MAX_URCP_BUFF_VLAN_NUM 32u /*ÿ������������������vlan��*/
#define MAX_URCP_VID  512u  /*�����������豸�е����vid,�е����ֵ*/
#define MAX_URCP_WEB_TIME_OUT_SEC 5u/*ҳ��ȴ���������ʱ�����ʱʱ��*/
#define URCP_WEB_WAIT_INTERVAL_USEC 100000u/*��������ҳ��ȴ�ʱ��������λΪ΢��*/
#define URCP_SWAP_CONF_SIZE 1518u/*��������goahead �� switch�����ռ���������С*/

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
/*������֤������Ϣ*/
typedef struct _urcp_batch_msg_t{
    unsigned char dst_mac[MAC_LEN*MAX_URCP_BATCH_NUM];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_URCP_BATCH_NUM][MAX_PASSWORD_LEN];
    unsigned int  number;   /*Ҫ����Ŀ�Ľ���������*/
}urcp_batch_msg_t;
#define MAX_CONF_ITEM   6u  /*�����·���������Ŀ*/
typedef struct _urcp_send_conf_msg_t{
    urcp_batch_msg_t  batch_info;
    unsigned int inUSB;  /*�ļ��Ƿ����u����*/
    unsigned int serial_no; /* AP Serial Number */
    unsigned char item_id[MAX_CONF_ITEM];  /*������*/
    char src_file[64];  /*�ļ���*/
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

/*��������Ϣ�ṹ*/
typedef struct _urcp_cmd_alone_msg_t{
    urcp_batch_msg_t  batch_info;
    unsigned int  opt_type;/*��������*/
}urcp_cmd_alone_msg_t;

/*�����޸���Ϣ�ṹ*/
typedef struct _urcp_pwd_msg_t{
	urcp_batch_msg_t  batch_info;
	unsigned int opt_type; /*��������*/
	unsigned char dev_password[MAX_PASSWORD_LEN];/*Ҫ�޸ĵ���������������*/
}urcp_pwd_msg_t;

typedef struct _urcp_sta_msg_t{
    unsigned short staNum;
    unsigned char staMac[1][MAC_LEN];
}urcp_sta_msg_t;

typedef struct _urcp_black_list_t{
    char ssid[UTT_SSID_LENTH];   /* SSID����	    */
    unsigned char staMac[MAC_LEN];
}urcp_black_list_t;

typedef struct _urcp_black_list_msg_t{
    unsigned short mode;	    //1,��ӣ�2,ɾ������ģʽ,3ɾ��ȫ��
    unsigned short staNum;
    urcp_black_list_t black_list[1];
}urcp_black_list_msg_t;

typedef enum{
    URCP_STATUS_WAIT=1,
    URCP_STATUS_OK,
    URCP_STATUS_TIME_OUT,
    URCP_STATUS_AUTH_ERROR, /*�������*/
    URCP_STATUS_FAIL,         /*����ʧ�� */
    URCP_STATUS_FOUND_NONE,         /*����ʧ�� */
    URCP_STATUS_END/*����ʧ�� */
}urcp_status_t;

typedef struct _misc_header_t{
    int32 valid;
    uint32   user_ip;          /*��½�û�ip*/
    unsigned char  mac[MAC_LEN]; /*������ԭ���豸mac*/
    uint16   req_id;       /*���β�����Ӧ��request id*/
    uint32 data_type; /*��ʾ���ڵ���ͨ�������յ������ݻ���ҳ�����õ�����*/
    urcp_status_t status;       /*�������:�ɹ�,������󡢳�ʱ��ʧ��*/
}misc_header_t;


/*������������Щ��������ֻ���������ͣ����������ݡ����������ָ��������õȣ�*/
typedef struct _urcp_cmd_alone_buf_t{
    misc_header_t cmd_alone_header;
}urcp_cmd_alone_buf_t;

typedef struct _urcp_cmd_alone_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_cmd_alone_buf_t urcp_cmd_alone_buf[URCP_MISC_CMD_ALONE_SIZE];
}urcp_cmd_alone_struct_t;

/*����vlan*/
typedef struct _urcp_vlan_group_t
{
    uint16 vlan_no;
    char vlan_name[MAX_VLAN_NAME_LEN+1u];
    port_bit_mask_t member; 
}urcp_vlan_group_t;

typedef struct _urcp_vlan_conf_t
{
    uint32 port_num;        /*port number*/
    uint32 misc_type;/*�������misc bufferʱ�����ͻ���� �Լ�ȷ����ȡʲô���͵�vlan */
    uint32 vlan_type;/*���ܵ���vlan���ͼ�Զ���豸��ǰvlan����*/
    uint16 next_vlan_no;/*��һ�����ܵ�vlan ��š���tag vlan������Ҫ��Ƭ����*/
    urcp_vlan_group_t vlan_group[MAX_URCP_BUFF_VLAN_NUM];
    /*vlan��Ϣ�����ܲ�Ϊvlan_type���͵���Ϣ����һ��Ϊmisc_type���͵�vlan��Ϣ*/
}urcp_vlan_conf_t;

typedef struct _urcp_vlan_buf_t{
    misc_header_t vlan_header;
    urcp_vlan_conf_t vlan_conf;
}urcp_vlan_buf_t;

typedef struct _urcp_vlan_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_vlan_buf_t urcp_vlan_buf[URCP_MISC_VLAN_SIZE];
}urcp_vlan_struct_t;

/*tag vlan�˿�����*/
typedef struct _urcp_qvlan_pconf_group_t
{
    uint32 vlan_aware;/*vlan�豸*/
    uint32 frame_type;/*֡����*/
    uint16 pvid;/*pvid*/
}urcp_qvlan_pconf_group_t;
typedef struct _urcp_qvlan_pconf_conf_t
{
    uint32 port_num;        /*port number*/
    uint32 vlan_type;/*���ܵ���vlan���ͼ�Զ���豸��ǰvlan����*/
    urcp_qvlan_pconf_group_t pconf_group[MAX_URCP_PORT_NUM];
}urcp_qvlan_pconf_conf_t;

typedef struct _urcp_qvlan_pconf_buf_t{
    misc_header_t qvlan_pconf_header;
    urcp_qvlan_pconf_conf_t qvlan_pconf_conf;
}urcp_qvlan_pconf_buf_t;

typedef struct _urcp_qvlan_pconf_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_vlan_buf_t urcp_qvlan_pconf_buf[URCP_MISC_QVLAN_PCONF_SIZE];
}urcp_qvlan_pconf_struct_t;

#define MAX_PORT_REMARK_LEN 16
typedef struct _urcp_port_conf_group_t
{
    char remark[MAX_PORT_REMARK_LEN];/*�˿�����*/
    char link_status[MAX_STATUS_SIZE];/*��·״̬*/
    int selected_mode;/*����ģʽ*/
    int max_frame;/*�������֡*/
    char rate_limit;/*����*/
    char protect;/*�˿ڱ���*/
    char flooding;/*����*/
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
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_port_conf_buf_t urcp_port_conf_buf[URCP_MISC_PORT_CONF_SIZE];
}urcp_port_conf_struct_t;


/*��������*/
typedef struct _urcp_qos_rate_limit_group_t
{
    unsigned long policer;
    unsigned long shaper;
}urcp_qos_rate_limit_group_t;

typedef struct _urcp_qos_rate_limit_conf_t
{
    uint32 port_num;        /*port number*/
    uint32 rate_unit;  /*������*/
    urcp_qos_rate_limit_group_t qos_rate_limit_group[MAX_URCP_PORT_NUM];
}urcp_qos_rate_limit_conf_t;
typedef struct _urcp_qos_rate_limit_buf_t{
    misc_header_t qos_rate_limit_header;
    urcp_qos_rate_limit_conf_t qos_rate_limit_conf;
}urcp_qos_rate_limit_buf_t;
typedef struct _urcp_qos_rate_limit_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_qos_rate_limit_buf_t urcp_qos_rate_limit_buf[URCP_MISC_QOS_RATE_LIMIT_SIZE];
}urcp_qos_rate_limit_struct_t;

/*�籩����*/
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
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_storm_rate_limit_buf_t urcp_storm_rate_limit_buf[URCP_MISC_STORM_RATE_LIMIT_SIZE];
}urcp_storm_rate_limit_struct_t;

/*ϵͳ����*/
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
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_sys_conf_buf_t urcp_sys_conf_buf[URCP_MISC_SYS_CONF_SIZE];
}urcp_sys_conf_struct_t;

/*ϵͳ��Ϣ*/
typedef struct _urcp_sys_info_conf_t
{
    char now_time[64];/*ϵͳʱ��*/
    char up_time[64];/*����ʱ��*/

    long long outer_flash_total_space;/*���flash�ܴ�С*/
    long long outer_flash_unused_space;/*���flash���ÿռ�*/

    int32 cpu_rate;/*cpuʹ����*/
    int32 mem_rate;/*�ڴ�ʹ����*/

    uint32 ip_addr;/*ϵͳip*/
    uint32 ip_mask;/*ip����*/
    uint32 gw_ip;/*����*/
    unsigned char mac_addr[MAC_LEN];

    int32 sys_serialno;/*���к�*/
    char sw_version[SW_VERSION_SIZE];/*����汾*/

    uint32 cpu_vid;
    char dev_name[MAX_DEV_NAME_LEN];
}urcp_sys_info_conf_t;
typedef struct _urcp_sys_info_buf_t{
    misc_header_t sys_info_header;
    urcp_sys_info_conf_t sys_info_conf;
}urcp_sys_info_buf_t;
typedef struct _urcp_sys_info_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_sys_info_buf_t urcp_sys_info_buf[URCP_MISC_SYS_INFO_SIZE];
}urcp_sys_info_struct_t;

/*ip��ַ����*/
typedef struct _urcp_ip_filter_conf_t
{
    uint32 port_num;
    uint32 filter_src_ip[MAX_URCP_PORT_NUM];/*Դip*/
    uint32 filter_src_ip_mask[MAX_URCP_PORT_NUM];/*Դip����*/
}urcp_ip_filter_conf_t;
typedef struct _urcp_ip_filter_buf_t{
    misc_header_t ip_filter_header;
    urcp_ip_filter_conf_t ip_filter_conf;
}urcp_ip_filter_buf_t;
typedef struct _urcp_ip_filter_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_ip_filter_buf_t urcp_ip_filter_buf[URCP_MISC_IP_FILTER_SIZE];
}urcp_ip_filter_struct_t;

/*��Կ��֤����buffer*/
typedef struct _urcp_passwd_auth_buf_t{
    misc_header_t  passwd_auth_header;
}urcp_passwd_auth_buf_t;
typedef struct _urcp_passwd_auth_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_passwd_auth_buf_t urcp_passwd_auth_buf[URCP_MISC_PASSWD_SIZE];
}urcp_passwd_auth_struct_t;

/*��������*/
typedef struct _urcp_passwd_buf_t{
    misc_header_t  passwd_header;
}urcp_passwd_buf_t;
typedef struct _urcp_passwd_buf_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_passwd_buf_t urcp_passwd_buf[URCP_MISC_PASSWD_SIZE];
}urcp_passwd_struct_t;
typedef struct _urcp_mirror_conf_t{
    uint32   port_num;        /*port number*/
    uint32 mirror_port;
    port_bit_mask_t mirror_source;/*1-32�˿�*/
}urcp_mirror_conf_t;
typedef struct _urcp_mirror_buf_t{
    misc_header_t  mirror_header;
    urcp_mirror_conf_t mirror_conf;
}urcp_mirror_buf_t;

typedef struct _urcp_mirror_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
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
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_aggr_buf_t urcp_aggr_buf[URCP_MISC_AGGR_SIZE];
}urcp_aggr_struct_t;

typedef struct _urcp_rstp_lacp_conf_t{
    uint32   port_num;        /*port number*/
    port_bit_mask_t rstp_enable;/*1-32�˿�*/
    port_bit_mask_t lacp_enable;
}urcp_rstp_lacp_conf_t;
typedef struct _urcp_rstp_lacp_buf_t{
    misc_header_t  rstp_lacp_header;
    urcp_mirror_conf_t rstp_lacp_conf;
}urcp_rstp_lacp_buf_t;
typedef struct _urcp_rstp_lacp_buf_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
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
    uint32 work_mode;    /* ����ģʽ 1-fit 2-fat*/
	uint32 dhcp_en;		/*AP DHCP Client*/
    uint32 ip_addr; /* IP��ַ */
    uint32 sub_mask;
    uint32 gate_way;
//    uint32 max_clt;  /* ���ͻ����� */
    uint32 channel;  /* �ŵ� */
    uint32 channel_5g;	    /*5G�ŵ�*/
	uint32 dbm;      /* ������ֵ */
	uint32 dbm_5g;      /* ������ֵ */
    uint32 effect_conf;	/* ��Ч���� 1-local ����ֵ��ΪAC�·�*/
    char pass_modify;   /* AP�û��� */
    char password[MAX_PASSWORD_LEN];	/* �޸ĵ������� */
    char dev_name[MAX_DEV_NAME_LEN];	/* AP �豸�� */
    urcp_ap_basic_conf_sz_max sz_max_client;
}urcp_ap_basic_conf_t;

typedef struct _urcp_ap_baisc_conf_buf_t{
    misc_header_t	ap_basic_conf_header;
    urcp_ap_basic_conf_t    ap_basic_conf;
}urcp_ap_basic_conf_buf_t;

typedef struct _urcp_ap_basic_conf_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_ap_basic_conf_buf_t urcp_ap_basic_conf_buf[URCP_MISC_AP_BASIC_CONF_SIZE];	
}urcp_ap_basic_conf_struct_t;

/*Fit AP config template*/
typedef struct _urcp_ap_conf_temp_conf_t
{
    char tempName[16];	/* ģ������ */
	char suitType[16];  /* �����ͺ� */
	char firmware[16];  /* Ӳ���汾 */
    uint32 wireless;	/* �������߹��� */
    uint32 sgi;		/* short guard interval */
	uint32 cltIsolate;   /* ���߿ͻ��˸��� */
    uint32 preamble;	/* short preamble */
    uint32 wmm;		/* ����WMM */
    uint32 rate;	/* �������� */
    uint32 bw;		/* Ƶ������ */
    uint32 channel;	/* �ŵ� */
    uint32 power;	/* ���ߴ��书�� */
    uint32 manPower;	/* ���ߴ��书�� �ֶ����� */
    uint32 mode;	/* ����ģʽ */
    uint32 wlFre;		/* ����Ƶ�� */
    uint32 beaconPeriod;		/* */
    uint32 VChanWidth;	    /*VHT BW*/
}urcp_ap_conf_temp_conf_t;

typedef struct _urcp_ap_conf_temp_buf_t{
    misc_header_t	ap_basic_conf_header;
    urcp_ap_conf_temp_conf_t    ap_conf_temp_conf;
}urcp_ap_conf_temp_buf_t;

typedef struct _urcp_ap_conf_temp_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
    urcp_ap_conf_temp_buf_t urcp_ap_conf_temp_buf[URCP_MISC_AP_CONF_TEMP_SIZE];	
}urcp_ap_conf_temp_struct_t;

/* Fit AP join service zone start */

typedef enum _auth_mode_t
{
    none,   /* ��	*/
    wep,    /* WEP	*/
    wpa,    /* WPA	*/
    wpa_psk /* WPA-PSK	*/
}auth_mode;

/************* WEP **************/

typedef enum _en_auth_Method_t
{
    auto_auth,	/* �Զ�	    */
    open_sys,	/* ����ϵͳ */
    share_key	/* ������Կ */
}auth_method;

typedef enum _en_key_type_t
{
    hexa,   /* 16����	*/
    ascii   /* ASCII��	*/
}key_type;

typedef enum _en_key_format_t
{
    forbid, /* ���� */
    bit_64, /* 64λ */
    bit_128 /* 128λ*/
}key_format;

typedef struct _key_num_t
{
    uint32	key_state_action;	/* ʹ��״̬ */
    char	passwd[UTT_PAS_KEY_VALUE_LEN+1];	/* WEP��Կ  */
    key_format	kFmt;			/* ��Կ���� */
}key_num;

typedef struct _wep_security_t
{
    auth_method	auMethod;   /* ��֤����	*/
    key_type	kType;	    /* ��Կ��ʽ */
    key_num	kNum1;	    /* ��Կ 1	*/
    key_num	kNum2;	    /* ��Կ 2	*/
    key_num	kNum3;	    /* ��Կ 3	*/
    key_num	kNum4;	    /* ��Կ 4	*/
}auth_wep;

/************* WEP END **************/

/************* WPA **************/

typedef enum _en_wpaVersion_t
{
    auto_wpa,	/* �Զ�	*/
    wpa1,	/* WPA	*/
    wpa2	/* WPA2	*/
}wpa_version;

typedef enum _en_securityAlgorithm_t
{
    auto_sec,	/* �Զ�	*/
    tkip,	/* TKIP	*/
    aes		/* AES	*/
}security_alg;

typedef struct _wpa_security_t
{
    wpa_version	    wVersion;			    /* WPA�汾		*/
    security_alg    sAlg;			    /* �����㷨		*/
    uint32	    radiusIp;			    /* radius������IP   */
    uint32	    radiusPort;			    /* radius�������˿� */
    char	    radiusPasswd[UTT_RADIUS_KEY_LEN];   /* radius����	*/
    uint32	    updataTime;			    /* ��Կ��������	*/
}auth_wpa;

/************* WPA END **************/

/************* WPA-PSK **************/

typedef struct _wpa_psk_security_t
{
    wpa_version	    wVersion;			/* WPA-PSK�汾  */
    security_alg    sAlg;			/* �����㷨	*/
    char	    sharePasswd[UTT_RADIUS_KEY_LEN];/* Ԥ������Կ   */
    uint32	    updataTime;			/* ��Կ�������� */
}auth_wpa_psk;

/************* WPA-PSK END **************/

typedef struct _urcp_ap_join_sz_t
{
    uint32	serviceEn;		    /* ������״̬   */
    char	szName[UTT_INST_NAME_LEN + 1]; /* ����������*/
    char	ssid[UTT_SSID_LENTH + 1];   /* SSID����	    */
    uint32	ssidBcast;		    /* SSID�㲥	    */
    uint32	vid;			    /* vlan ID	    */
    uint32	client;			    /* Max client   */
    auth_mode	securMode;		    /* ���ܷ�ʽ	    */
    auth_wep	wepSecurity;		    /* WEP	    */
    auth_wpa	wpaSecurity;		    /* WPA	    */
    auth_wpa_psk    wpaPskSecurity;	    /* WPA-PSK	    */
#if 0
    uint32	dhcpEn;			    /* DHCP������   */
    uint32	startIp;		    /* ��ʼIP	    */
    uint32	endIp;			    /* ����IP	    */
    uint32	gw;			    /* ����	    */
    uint32	netmask;		    /* ��������	    */
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


/*һ������������Ϣ�ṹ����ɫ��������¶���*/
typedef struct _urcp_general_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 opt_type; /*misc buff��������*/
}urcp_general_msg_t;

/*���������Ϣ*/
typedef struct _urcp_mirror_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 mirror_port;
    port_bit_mask_t mirror_source;/*1-32�˿�*/
}urcp_mirror_msg_t;

/*��۲�����Ϣ�ṹ*/
typedef struct _urcp_aggr_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 opt_type; /*misc buff��������*/
    urcp_aggr_group_t aggr_group;/*�������Ϣ*/
}urcp_aggr_msg_t;

/*RSTP LACP������Ϣ�ṹ*/
typedef struct _urcp_rstp_lacp_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
}urcp_rstp_lacp_msg_t;

/*vlan������Ϣ�ṹ*/
typedef struct _urcp_vlan_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
    uint32 opt_type; /*misc buffer opt type*/
    uint16 start_vlan_no;/*���ĸ�vlan no��ʼ��ȡ*/
    urcp_vlan_group_t vlan_group;/*vlan����Ϣ*/
}urcp_vlan_msg_t;

/*tag vlan�˿����ò�����Ϣ�ṹ*/
typedef struct _urcp_qvlan_pconf_msg_t{
    unsigned char dst_mac[MAC_LEN];
    unsigned int  user_ip;   /*ҳ���½�û�ip*/
    unsigned char password[MAX_PASSWORD_LEN];
    uint32   port_ct;        /*port number*/
}urcp_qvlan_pconf_msg_t;

/*��������������Ϣ�ṹ��*/
typedef struct _urcp_qos_rate_limit_msg_t{
    urcp_general_msg_t general_msg;/*ͨ����Ϣ*/
    urcp_qos_rate_limit_conf_t qos_rate_limit_conf;/*���ٽṹ��*/
}urcp_qos_rate_limit_msg_t;
/*�����籩����*/
typedef struct _urcp_storm_rate_limit_msg_t{
    urcp_general_msg_t general_msg;/*ͨ����Ϣ*/
    urcp_storm_rate_limit_conf_t storm_rate_limit_conf;/*���ٽṹ��*/
}urcp_storm_rate_limit_msg_t;

/*����ϵͳ������Ϣ�ṹ��*/
typedef struct _urcp_sys_conf_msg_t{
    urcp_general_msg_t general_msg;/*ͨ����Ϣ*/
    urcp_sys_conf_conf_t sys_conf_conf;/*���ٽṹ��*/
}urcp_sys_conf_msg_t;

/* Զ��AP����������Ϣ�ṹ�� */
typedef struct _urcp_ap_basic_conf_msg_t{
    urcp_general_msg_t general_msg;/*ͨ����Ϣ*/
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
    urcp_general_msg_t general_msg;/*ͨ����Ϣ*/
    urcp_wl_mac_filter_t  wl_mac_filter_msg;
}urcp_wl_mac_filter_msg_t;

typedef struct _urcp_wl_mac_filter_struct_t {
	uint32 next_index;
	urcp_wl_mac_filter_buf_t  urcp_wl_mac_filter_buf[URCP_MISC_WL_MAC_FILTER_SIZE];
} urcp_wl_mac_filter_struct_t;

typedef struct _urcp_ap_conf_temp_msg_t{
    urcp_general_msg_t general_msg;/*ͨ����Ϣ*/
    urcp_ap_conf_temp_conf_t conf_temp_conf;
}urcp_ap_conf_temp_msg_t;

/* Զ��AP����������Ϣ�ṹ�� */

/* AP ��������� */
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

/* AP ��������MAC��ַ���� */
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


/*misc buffer��д���������ṹ*/
typedef struct _misc_opt_assist{
    unsigned char* misc_buffer_header;/*misc bufferͷ��ָ��*/
    uint32 unit_size;/*buffer��ÿ���ṹ��Ĵ�С*/
    uint32 misc_buffer_size;/*buffer�������ٽṹ��*/
}misc_opt_assist;
/*��ȡ�·����ò���buffer*/
typedef struct _urcp_config_buf_t{
    misc_header_t  config_header;
}urcp_config_buf_t;
typedef struct _urcp_config_struct_t{
    uint32 next_index;       /*�����һ����¼����λ��*/
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
    urcp_config_struct_t urcp_config_struct;/*��ȡ���ã��·�����*/
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
    uint16  channel_head;		//�ŵ�������ǰ�벿�֣���Ϊ0�����벿����Ч��������Ч
    uint16  channel_end;                //�ŵ������ĺ�벿�֣������ײ�
    uint32  power;
    uint32  clients;
    uint32  max_clients;
    uint32  ssid_num;
    uint16 joined_sz_num;
    uchar cpu_rate;	    //CPUʹ����
    uchar mem_rate;	    //�ڴ�ʹ����
    uint32 up_time;	    //����ʱ��
    uint16 bit_rate;	    //����
    uint16 chan_width;  //Ƶ��
    uint16 power_new;	    //����
    uint16 beacon;
    uint32 tx_packets;
    uint32 tx_bytes;
    uint32 rx_packets;
    uint32 rx_bytes;
	
#if SZONE_AUTO_UPDATA
    szone_info_t szone_info;
#endif
    uint32 profile_type;  /*��Ч���ã�����or AC�·�����*/
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
    uint16 bit_rate;	    //����
    uint16 chan_width;  //Ƶ��
    uint16 power_new;	    //����
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
/*AP����ͳ��-->��ʼ*/

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
	uint32 hour_pos;//����ȥ��������ʹ��hour_pos��������day_pos���ƶ�
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
	wl_stat_t wl_stat_current;//���һСʱ�շ�����
	wl_his_stat_t wl_his_stat;//������ʷ�շ�����
}wl_info_stat_t;
/*AP����ͳ��-->����*/
#endif


/*���������豸�ṹ*/
typedef struct _urcp_dev{
    int32    used;
 //   int32    tb_index;       /*�ڱ��е�����*/
    int32    tb_next;       /*�ڱ��е�����*/
    uint32   ip;
    uint32   netmask;
    uint32   gateway;
    uint32   serial_no;        /*serial number*/
    uint32   port_num;        /*port number*/
    uint32   dhcp_en;        /*dhcp client*/
    uint32   dbm;            /* roaming */
    uint32   dbm_5g;            /* roaming */
    uint32   dev_type;       /*router , switch, fit ap, fat ap, and so on*/
    char     urcp_version;     /* �����汾*/
    unsigned char    mac[MAC_LEN];
#if AP_WL_POWER_AUTO
    unsigned char    ra0_mac[MAC_LEN];
#endif
    char     dev_name[MAX_DEV_NAME_LEN];         /*device name*/
    char     user[UTT_INST_NAME_LEN+1];
    char     dev_model[DEV_TYPE_SIZE];      /*�ͺ�*/

    unsigned char    link_port;                        /*�˿�*/
    unsigned char    up_ports;                       /*��˿���*/
    unsigned char    alarm_level;                    /*��������*/

    unsigned char    sw_version[SW_VERSION_SIZE];     /*����汾��Ϣ*/
    unsigned char    hw_version[HW_VERSION_SIZE];     /*Ӳ���汾��Ϣ*/
    uint32 load_balance; /* ���ؾ�����*/
    int32 aged;
    wl_info_t wl_info;
    wl_info_5G_t wl_info_5G;
    uint32           vid;        /*ap����vlan id*/
    int32           fromL3;      
#if (HISTORY_FLOW_INFO == FYES)
	uint32 	       updateStat;/*����Ƿ������AP������Ϣ*/
	uint32         updateRate;/*����Ƿ������AP����������*/
	wl_info_stat_t acStat; /*���AP����ʷ������Ϣ*/
#endif
}urcp_dev_t;
/**��������goahead �� switch �������ݻ���ṹ*/
typedef struct _urcp_swap_buf_t
{
    char urcp_conf[URCP_SWAP_CONF_SIZE];/*��������*/
}urcp_swap_buf_t;

typedef  struct _topoy_t{
    struct _topoy_t *first_child;    /*ָ���һ������*/
    struct _topoy_t *next_sibling;   /*ָ����һ���ֵ�*/
    struct _topoy_t *next;           /*ָ����һ���ڵ�*/
    int32    tb_index;       /*�ڱ��е�����*/
    int32    tb_next;       /*�ڱ��е�����*/
    uint16   parent_index;    /*���ڵ��������˽ṹͬһ���е�λ��*/
    uint16   parent_port;     /*���ڵ�˿ں�*/
    uint16   index;           /*�����˽ṹͬһ���λ��*/
    uint16   port;            /*�븸�ڵ������Ķ˿�*/
    uint32   ip;
    unsigned char    parent_mac[MAC_LEN];
    unsigned char    mac[MAC_LEN];
    uint16   vlan_id;         /*���ڵ�˿�vid*/
    uint16   hop;
    uint16   virtual_hop;     /*�ϲ�����ڵ���*/
    uint16   virtual_dev;     /*�Ƿ�ѳ�����豸*/
    uint32   alarm_level;                    /*��������*/
    uint32   parent_aggr_mask;  /*���ڵ��۶˿�*/
    uint32   child_aggr_mask;    /*���ڵ��۶˿ں�*/
    uint16   dev_model;     /*�豸����*/
    uint16   up_ports;    /*��˿���*/
    char     dev_type[DEV_TYPE_SIZE];       /*�ͺ�*/
}topoy_t;

typedef struct _topology_list_t{
    int32   first_node;    /*ָ��ĳһ��εĵ�һ���ڵ�*/
    topoy_t *last_node;     /*ָ�򱾲����һ���ڵ�*/
    int32    count;          /*����ڵ���*/
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
    uint32   con_time;		/*����ʱ��*/
    MACHTTRANSMIT_SETTING   phy;
    uchar    ap_mac[MAC_LEN]; /*����AP��MAC��ַ*/
    char     rssi;
    char     ssid_idx;       /*AP��SSID����*/
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
#define MAX_STA_LOG_POOL 4096	    /*��־�ڵ�ĸ���*/
#define MAX_UP_DOWN_SIZE 0x3FFFu    /*14λ�ܱ�ʾ���������*/
#define LOG_UP_DOWN_LIMIT   60u	    /*Ƶ���������ж�ʱ���׼*/
#define STA_LOG_FLAG     0x0u
#define USB_STA_LOG_PATH "staLog/"          /*U�����ļ���·��*/
#define LOG_1_DAY        86400u
#define LOG_3_DAY        259200u
#define LOG_7_DAY        604800u
#define LOG_1_MONTH      2628288u	/*30.42��*/
#define LOG_2_MONTH      5256576u	/*30.42*2��*/
#define log_1_YEAR	 31539456u	/*365��*/

typedef struct _log_sta_mac
{
    uchar sta_mac[MAC_LEN];
}log_sta_mac;
typedef struct _urcp_del_log_msg_t
{
    unsigned short mode;	    //1ɾ������ģʽ,0ɾ��ȫ��
    unsigned short staNum;
    log_sta_mac del_log_list[0];
}urcp_del_log_msg_t;

typedef struct _sta_log_file_head_t
{
    uint32 head;	/*�ļ�ͷ*/
    uint16 flag;	/*��־*/
    uint16 head_len;	/*ͷ����*/
    uint16 one_log_size;    /*һ����־�Ĵ�С*/
    uint16 log_num;	    /*�ļ�����־������*/
    uint32 free;	    /*4B�ı����ֽ�*/	
}log_file_head_t;

typedef struct _sta_log_file_info_t
{
    uchar ap_mac[MAC_LEN];
    uchar sta_mac[MAC_LEN];
    uchar  staAlias[MAX_STA_ALIAS_LEN+1]; /*���ø�sta�ı�ע��Ϣ���������û�����*/
    uint32 up_time;
    uint32 duration;
    uint16 upload; /*�ϴ�����,��2��bit��ʾ��λ��0-B��1-K��2-M��3-G����14bit��ʾ��ֵ*/
    uint16  download; /*ͬ��*/
}log_file_info_t;

typedef struct  _sta_log_t{
    tb_entry_hdr_t hdr;
    uchar  staAlias[MAX_STA_ALIAS_LEN+1]; /*���ø�sta�ı�ע��Ϣ���������û�����*/
    uint32 count;  /*��STA��־����*/
    uint32 logPoolIdx; /*ָ�򱣴���־�Ľڵ�*/
}sta_log_t;

typedef struct _log_info_t{
    uint16 used;	/*0�սڵ㣬 1sta���߽ڵ㣬2��sta�����߽ڵ㣬�����ڵ�*/
#if 0
    uint16 year;  /*ʱ��*/
    uchar month;
    uchar day;
#endif
    uchar apMac[MAC_LEN];
    uint32 upTime; /*����ʱ��*/ 
    uint32 duration; /*sta�˴����ߵ����ߵĳ�����ʱ������Ϊ��λ�������߶�ʱ�����ߴ���*/
    uint16 upload; /*�ϴ�����,��2��bit��ʾ��λ��0-B��1-K��2-M��3-G����14bit��ʾ��ֵ*/
    uint16  download; /*ͬ��*/
    uint32  staIdx;  /*�ýڵ�������STA����sta_log����*/
    uint32  earlierIdx;  /*����STA�У���һ���һ����־����+1��0��ʾ�������������־��*/
    uint32  newIdx;  /*����STA�У��ȱ�����־��һ�����־����+1��0��ʾ���������µ�*/
    uint32  next; /*��STA��������־����+1����usedΪ0ʱָ����һ�����нڵ�����+1*/
}log_info_t;

typedef struct _log_pool_t{
    int32  firstFree;  /*��һ������log_buffer����+1��0��ʾû�п��нڵ�*/
    int32  lastestIdx;/*����һ����־����+1*/
    int32  earliestIdx;/*�����һ����־����+1*/
    int32  ago1day;/*һ���ڵ�����һ����־����+1*/ 
    int32  ago3day;/*3���ڵ�����һ����־����+1*/ 
    int32  ago7day;/*7���ڵ�����һ����־����+1*/ 
    int32  ago1month;/*һ�����ڵ�����һ����־����+1*/ 
    int32  ago2month;/*2�����ڵ�����һ����־����+1*/ 
    int32  ago1year;/*1���ڵ�����һ����־����+1*/ 
    log_info_t logBuffer[MAX_STA_LOG_POOL];
}log_pool_t;
#endif

typedef struct _urcp_shm_t{
    uint32 dev_tb_hash[DEV_HASH_SIZE]; /*hash head*/
    urcp_dev_t dev_table[DEV_TABLE_SIZE]; /*���������豸��*/
    urcp_misc_t  urcp_misc;    /*���������ȡ���������豸��Ϣ*/
    urcp_swap_buf_t urcp_swap_buf;/*��������goahead��switch�����ռ�*/
    ap_log  ap_msg;/*���AP�쳣��Ϣ*/

#if AP_WL_POWER_AUTO
    int32 ra0_rssi[DEV_TABLE_SIZE][DEV_TABLE_SIZE]; /*�����ΧAP rssi֮���Ӱ��*/
#endif

#if 0
   // topoy_t topoy_table[MAX_TOPOY_DEV];/*���˷����豸��*/
   // topogy_list_t topoy_list[MAX_TOPOY_HOP];/*��ҳ����ʾ�ṹ*/
#endif
    wl_statis_t wl_statis;
    hash_entry_hdr_t sta_tb_hash[STA_TB_HASH_SIZE];
    sta_entry_t sta_tb[STA_TB_SIZE];
#if (STA_ONOFF_LOG == FYES)
    hash_entry_hdr_t sta_log_hash[MAX_STA_LOG_HASH_SIZE]; /*����Ϊ128*/
    sta_log_t  sta_log[MAX_STA_LOG_TB_SIZE ];/*����Ϊ4096*/
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
	uint32_t systemVer;  /* ǰ24λ������汾����16λ��Ӳ���汾����88844�������䣬��Ӧ 1.1.1  1.1  */
	uint32_t imageComp;
        uint32_t loadAddr;
        uint32_t entryAddr;
        uint8_t  name[20];
        uint32_t kernelLen;
        uint32_t kernelCrc;
} image_header_t;
#endif
#endif
