
#ifndef __TOPOY_H__
#define __TOPOY_H__

#define TOPOY_HELLO        1u 
#define TOPOY_REPLY        2u 

#define TYPE_SWITCH        0u
#define TYPE_ROUTER        1u

/*设备类型*/
#define DEV_MODEL       TYPE_ROUTER 

#define TOPOY_VERSION           0u 
#define TOPOY_HELLO             1u 

#define TOPOY_CMD_TIME_OUT      (2100/POLL_TIME) 

/*拓扑发现报文格式*/
typedef struct _topoy_hdr{
    uchar    version;
    uchar    type;
    uint16   flag;
    uchar    root_id[MAC_LEN];

    uchar    parent_id[MAC_LEN];
    uint16   parent_port_id;

    uchar    child_id[MAC_LEN];
    uint16   child_port_id;

    uint16   vlan_id;
    uint16   max_hop;
    uint16   hop;

    uint32   alarm_level;     /*告警级别*/
    uint32   parent_aggr_mask;  /*父节点汇聚端口*/
    uint32   child_aggr_mask;    /*本节点汇聚端口号*/
    uint16   dev_model;    /*设备类型*/
    uint16   up_ports;    /*活动端口数*/
    char     dev_type[DEV_TYPE_SIZE];       /*型号*/
}topoy_hdr_t;


typedef struct _topoy_state{
    uint32      dev_num;
}topoy_state_t;


extern int32 topoy_process(urcp_infor_t* infor);
extern void topoy_tb_form(topoy_t* root);
extern void topoy_init(void);
extern int32 topoy_discover(void);
extern void topoy_timer(uint16 reqId, session_type_t type, urcp_status_t urcp_state);
#endif
