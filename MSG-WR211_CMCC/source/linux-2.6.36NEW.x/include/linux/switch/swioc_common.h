/******
 *此头文件定义内核与应用程序ioctl操作公用数据结构
 *此文件结构定义应尽可能只与内核 ioctl操作相关，而与内核其他功能模块无关
 *从而达到应用程序与内核模块的独立
 *bhou
 *2011-08-11
 ******/
#ifndef _COM_SWIOC_COMMON_H_
#define _COM_SWIOC_COMMON_H_

#include <linux/switch/switch.h>
#include <linux/autoconf.h>

/*ioctl cmd type（需为整型，因为ioctl的cmd参数为int类型）*/
#define SWIOC_VLAN_TYPE_SET 1
#define SWIOC_VLAN_TYPE_GET 2

#if SW_FEATURE_PORT_VLAN
#define SWIOC_PORT_VLAN_SET 3
#define SWIOC_PORT_VLAN_GET 4
#endif

#define SWIOC_TAG_VLAN_GET  5
#define SWIOC_TAG_VLAN_SET  6

#define SWIOC_MAC_DEL 7
#define SWIOC_MAC_ADD  8
#define SWIOC_MAC_AGE_TIME_SET 9

#define SWIOC_MAC_AGE_TIME_GET 10
#define SWIOC_MAC_AGE_TIME_ENABLE_SET 11
#define SWIOC_MAC_AGE_TIME_ENABLE_GET 12
#define SWIOC_MORROR_EGRESS_ENGRESS_PORT_MODE_SET 13
#define SWIOC_MORROR_EGRESS_ENGRESS_PORT_MODE_GET 14
#define SWIOC_MORROR_MONITOR_PORT_SET 15
#define SWIOC_MORROR_MONITOR_PORT_GET 16

#if SW_FEATURE_PORT_CONF
#define SWIOC_PORT_CONF_SET  17
#define SWIOC_LINK_STATUS_GET 18
#endif

#if SW_FEATURE_PORT_TRUNK   /* port trunk */
#define SWIOC_TRUNK_GROUP_SET 	19
#define SWIOC_TRUNK_GROUP_GET 	20
#define SWIOC_TRUNK_GROUP_GET_BY_PORT 21	
#define SWIOC_TRUNK_MODE_SET	22
#define SWIOC_TRUNK_MODE_GET	23
#endif

#define SWIOC_ACL_ADD      24
#define SWIOC_ACL_DEL      25
#define SWIOC_ACL_GET      26
#define SWIOC_ACL_ENABLE   27
#define SWIOC_ACL_STATUS_GET 28

#if SW_FEATURE_PACKET
#if 0
#define SWIOC_BCP_DST_PORTS_GET 28
#endif
#define SWIOC_FWD_DST_PORTS_GET 29
#endif

#if SW_FEATURE_PORT_STATISTICS
#define SWIOC_PORT_COUNTER_GET	 30	
#define SWIOC_PORT_COUNTER_CLEAR 31	

#endif

#define SWIOC_SHOW_MAC_TABLE     32

#if SW_FEATURE_PORT_PRIORITY
#define SWIOC_PORT_PRIORITY_SET	    33
#endif
#if SW_FEATURE_STORM_CONTROL
#define SWIOC_BOARDCAST_STORM_CONTROL_SET	34
#endif
#if SW_FEATURE_RATE_LIMIT
#define SWIOC_RATE_LIMIT_SET    35
#define SWIOC_RATE_LIMIT_THROTTLE_SET     36
#endif
/*end ioctl cmd type define*/

#define  START_INDEX 1u   /*各数组第一个有效索引, 如 pvlan_table[PORT_ARRAY_SIZE]等有效元素下标可能不从0开始*/

typedef unsigned int poag_no_t;   /*汇聚组号类型*/

typedef uint32 mac_age_time_t;

/* mirror */
typedef struct _swioc_monitor_t
{
	int32  m_mode;
	BOOL pmonitor_no[PORT_ARRAY_SIZE]; /* all monitored ports */
} swioc_monitor_t;

typedef struct _mac_t
{
    unsigned char addr[MAC_LEN];
} mac_t;

typedef struct _vid_mac_t
{
    vid_t         vid;
    mac_t         mac;
} vid_mac_t;
typedef struct  _mac_table_entry_t {
    vid_mac_t     vid_mac;                      /*VLAN AND MAC addr */
    BOOL          destination[PORT_ARRAY_SIZE]; /* Dest. ports */
} mac_table_entry_t;

/*端口vlan*/
typedef uint32 pvlan_no_t;

#define VLAN_TYPE_NONE   0
#define VLAN_TYPE_PORT   1
#define VLAN_TYPE_8021Q  2

#if SW_FEATURE_PORT_VLAN
typedef struct _swioc_pvlan_t
{
    pvlan_no_t pvlan_no;
    BOOL member[PORT_ARRAY_SIZE];
} swioc_pvlan_t;
#endif

#if 0
/*tag vlan*/
typedef struct _swioc_qvlan_t;
{
    vid_t vid;
    BOOL member[PORT_ARRAY_SIZE];
} swioc_qvlan_t;

/* VLAN acceptable frame type */
typedef enum _vlan_frame_t {
    VLAN_FRAME_ALL,    /* Accept all frames */
    VLAN_FRAME_TAGGED  /* Accept tagged frames only */
} vlan_frame_t;

/*tag vlan端口设置*/
typedef struct _vlan_port_mode_t
{
    BOOL              aware;          /* VLAN awareness */
    vid_t             pvid;           /* Port VLAN ID (ingress) */
    vlan_frame_t frame_type;     /* Acceptable frame type (ingress) */
} vlan_port_mode_t;

typedef struct _swioc_vlan_port_mode_t {
    port_no_t         port_no;        /*port no*/
    vlan_port_mode_t  vlan_mode;
} swioc_vlan_port_mode_t;
#endif

#if SW_FEATURE_PORT_CONF
/*端口管理*/
typedef struct _port_conf_t
{
    BOOL      autoneg ;/*是否自动协商*/
    speed_t   speed ;/*接口速率*/
    BOOL      fdx ;/*是否全双工*/
    BOOL      enable ;/*端口是否启用*/
    BOOL      flow_control ;/*是否开启流控*/
#ifdef CONFIG_VSC7390
    uint32    max_length;/*最大帧*/
#endif
    BOOL      protect;/*端口保护*/
    BOOL      flooding;/*泛洪*/
}port_conf_t;
typedef struct _swioc_port_conf_t{
    port_no_t port_no;
    port_conf_t port_conf;
} swioc_port_conf_t;
typedef struct _swioc_link_status_t{
    port_no_t port_no;
    char link_status[MAX_LINK_STATUS_SIZE];
} swioc_link_status_t;
/*end端口管理*/
#endif

#if SW_FEATURE_PORT_TRUNK 	/* port trunk */
typedef struct _swioc_trunk_group_t
{
	int32 gp_no;/*group or port no*/
	BOOL members[PORT_ARRAY_SIZE];
} swioc_trunk_group_t;

#endif	/* End SW_FEATURE_PORT_TRUNK */



#if SW_FEATURE_ACL


typedef unsigned int sw_acl_ip_t;
typedef unsigned int sw_acl_ip_mask_t;
typedef unsigned int sw_acl_source_t;
typedef unsigned short sw_acl_udp_tcp_t;

/* ACL Action */
typedef enum {
    ACL_NONE=0,
    ACL_FORWARD,
    ACL_CPU,
    ACL_DROP,
    ACL_QUEUE0,
    ACL_QUEUE1,
    ACL_QUEUE2,
    ACL_QUEUE3,
    ACL_QINQ1,
    ACL_QINQ2,
    ACL_QINQ3,
    ACL_QINQ4,
}sw_acl_action_t;


#define ACL_SOURCE_PORT  0
#define ACL_SOURCE_VLAN  1
#define MAX_ACL_ACTION   11

typedef struct _sw_acl_t{
    int32  idx;

    sw_acl_action_t      action;    /* ACE action */

    sw_acl_source_t      source_entry;    /* ACE source entry*/

    struct {
	sw_acl_ip_t      sip;       /* Source IP address */
	sw_acl_ip_mask_t sip_mask_width;  /* Source IP address mask*/
	sw_acl_ip_t      dip;       /* Destination IP address */
	sw_acl_ip_mask_t dip_mask_width;  /* Source IP address mask*/
	sw_acl_udp_tcp_t sport;     /* UDP/TCP: Source port */
	sw_acl_udp_tcp_t dport;     /* UDP/TCP: Destination port */
    } ipv4;

}sw_acl_t;
typedef struct _swioc_acl_enable_t{
    int32 enable;
    int32 source_entry_mode;
}sw_acl_enable_t;
#endif

/*cpu处理包相关*/
#if SW_FEATURE_PACKET
/*start转发包目的端口查询*/
typedef struct _fwd_dst_ports_cond_t{
    port_no_t src_port;/*source port*/
    vid_mac_t vid_mac;/*vid 以及mac*/
    BOOL vlan;
}fwd_dst_ports_cond_t;/*unit cast packet destination ports query condition*/
typedef struct _fwd_dst_ports_ret_t{
    BOOL dest[PORT_ARRAY_SIZE];/*destination ports*/
    vid_t vid;/*vid*/
}fwd_dst_ports_ret_t;/*unit cast packet destination ports query result*/

typedef union{
    fwd_dst_ports_cond_t condition;/*查询条件*/
    fwd_dst_ports_ret_t  result;/*查询结果*/
}swioc_fwd_dst_ports_t;
/*end转发包目的端口查询*/


#if 0
/*start广播包目的端口查询*/
typedef union{
    port_no_t src_port;/*源端口或目的端口排除的端口（查询条件）*/
    BOOL dest[PORT_ARRAY_SIZE];/*目的端口（查询结果）*/
}swioc_bcp_dst_ports_t;/*broadcast packet destination ports query*/
/*end广播包目的端口查询*/
#endif
#endif

#if SW_FEATURE_PORT_STATISTICS

typedef struct _sw_port_stat_t{
    uint64 rxPkts;
    uint64 txPkts;

    uint64 rxOctets;
    uint64 txOctets;

#ifdef CONFIG_VSC7390
    uint64 rxBcastMcastPkts;
    uint64 txBcastMcastPkts;

    uint64 rxPkts64Octets;
    uint64 txPkts64Octets;

    uint64 rxPkts1024to1518Octets;
    uint64 txPkts1024to1518Octets;
#endif
    uint64 rxPktsErrors;
    uint64 txPktsErrors;
}sw_port_stat_t;
typedef struct _swioc_port_stat_t{
    port_no_t port_no;
    sw_port_stat_t port_stat;
}swioc_port_stat_t;
#endif /*End SW_FEATURE_PORT_STATISTICS*/

#if SW_FEATURE_STORM_CONTROL
typedef struct  _swioc_storm_control_t{
    BOOL  pStormCtrlBC[PORT_ARRAY_SIZE];
    uint32 pCtrlThreshold;
    BOOL pTimerSel;
}swioc_storm_control_t;
#endif

#if SW_FEATURE_RATE_LIMIT
typedef struct  _swioc_rate_control_t{
    port_no_t	port_no;
    uchar	rate_type;
    uint32	tx_rate;
    uint32	rx_rate;
}swioc_rate_ctrl_t;
#endif

#endif
