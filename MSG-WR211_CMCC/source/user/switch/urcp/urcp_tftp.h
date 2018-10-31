/***********************************
 *
 *联动文件传输协议头文件
 *
 *KeHuatao
 *2011-04-01
 * *********************************/
#ifndef __URCP_TFTP_H__
#define __URCP_TFTP_H__
#include <stdio.h>
#include "urcp.h"

#define TFTP_BLOCK_LEN            1280u 
/*最大超时时间*/
#ifdef URCP_AP
#define TFTP_TIMEOUT_MS           (6400u/POLL_TIME)
#else
#define TFTP_TIMEOUT_MS           (4000u/POLL_TIME)
#endif
/*重发超时时间*/
#ifdef URCP_AP
#define TFTP_MAX_TIMEOUT_MS        (8000u/POLL_TIME)
#else
#define TFTP_MAX_TIMEOUT_MS        (5000u/POLL_TIME)
#endif
#define TFTP_MAX_RETRIES          4u 
#define MAX_URCP_TFTP_SESSION     32u

/*报文类型--opcode*/
#define URCP_TFTP_READ_FILE       1u  
#define URCP_TFTP_WRITE_FILE      2u 
#define URCP_TFTP_READ_DATA       3u  
#define URCP_TFTP_WRITE_DATA      4u  
#define URCP_TFTP_DATA            5u  
#define URCP_TFTP_ACK             6u  
#define URCP_TFTP_ERROR           7u  
#define URCP_TFTP_OACK            8u  

/*处理会话的进程标识(urcp_tftp_hdr_t.remote_port)*/
#define PORT_NULL                 0u
#define PORT_CONFIG               1u
#define PORT_LOG                  2u
#define PORT_CONFIG_AUTO            3u
#define PORT_IMAGE                  4u
#define PORT_IMAGE_AUTO             5u
#define PORT_CONFIG_TEMPLATE        6u
#define PORT_CONFIG_TEMPLATE_AUTO   7u
/*end*/

/*文件名最大长度*/
#define MAX_FILE_NAME_LEN         63u

/**会话状态*/
typedef enum{
    IDLE,
    WAIT_R_OACK,
    WAIT_OACK_ACK,
    WAIT_W_OACK,
    WAIT_DATA,
    WAIT_ACK,
    WAIT_FIN,
}urcp_tftp_state_t;

/*会话type*/
typedef enum{
    TFTP_GET_FILE,   /*发送读文件请求*/
    TFTP_SEND_FILE,  /*收到读文件请求而发送文件*/
    TFTP_PUT_FILE,  /*发送写文件请求*/
    TFTP_REV_FILE,  /*收到写文件请求而接收文件*/

    TFTP_GET_DATA,   /*发送读数据请求*/
    TFTP_SEND_DATA,  /*收到读请求而发送文件*/
    TFTP_PUT_DATA,  /*发送写数据请求*/
    TFTP_REV_DATA,  /*收到写数据请求而接收数据*/
}urcp_tftp_sess_type_t;

typedef void (*cb_tftp)(void *ptr, urcp_status_t state);

/********
 *
 *会话控制结构
 *
 * ******/
typedef struct _urcp_tftp_session_info_t{
    uint16 remote_id;      /*远端id*/
    uint16 local_id;       /*本地id*/
    uchar mac[MAC_LEN];    /*会话目的交换机MAC地址*/
    urcp_tftp_sess_type_t  sess_type;       /*会话类型，读写文件或普通数据*/
    uint32 block_size;        /*数据长度*/    
    uint32 tftp_port;       /*被动接收文件时，处理接收到的文件的进程标识*/
    urcp_tftp_state_t state;  /*会话当前状态*/
    cb_tftp cb;
    int32 timer;           /*会话计时器*/
    FILE* fd;              /*如果传输文件，表示读写文件指针，否则为NULL*/
    uchar*  buffer;        /*如果传输大块数据，表示读完缓冲区起始地址*/
    uchar*  buffer_ptr;        /*如果传输大块数据，表示读写数据指针*/
    uint32  block_total;    /*发送文件或数据总块数，用于判断最后一个包*/
    uint32  last_block_size;    /*最后一块数据包的长度*/
    uint32  blockct;       /*已经发送或接收的文件块计数*/
    uint32  retries;        /*数据包已经重发次数*/
    uchar*  last_pkt;      /*备份的上一次发出数据包的起始地址*/
    uint32  last_pkt_len;   /*备份的上一次发出数据包的长度*/
    uchar filename[MAX_FILE_NAME_LEN+1];        /*要读取的文件名*/
}urcp_tftp_session_info_t;

/*保存所有urcp tftp会话的状态*/
typedef struct _urcp_tftp_session_t{
    uint16 id_counter;      /*下一个会话可使用的id，用于分配新会话*/
    urcp_tftp_session_info_t session[MAX_URCP_TFTP_SESSION];
}urcp_tftp_session_t;

typedef struct _urcp_tftp_hdr{
    uint32 opcode;
    uint16 lid;     /*会话的本地id*/
    uint16 chksum;
    uint32 remote_port;  /*远端的处理进程标识,如发送配置完了后，远端需激活该配置*/
}urcp_tftp_hdr_t;

/*OACK报文头部*/
typedef struct _urcp_tftp_oack_hdr{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 blksize; /*本机接受的blocksize大小*/
    uint32 length; /*读请求回复的OACK报文中表示数据或文件长度*/
}urcp_tftp_oack_hdr_t;

/*对文件请求包的回复包的格式*/
typedef struct _urcp_tftp_file_request_t{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 blksize; /*本机接受的blocksize大小*/
    uint32 name_len; /*文件名长度*/
    /*文件名*/
    /*写请求包中还带文件长度*/
}urcp_tftp_file_request_t;

typedef struct _urcp_tftp_data_request_t{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 blksize; /*本机接受的blocksize大小*/
    uint32 data_type;
    uint32 data_len;/*写报文中该值有意义*/
}urcp_tftp_data_request_t;
/*数据包格式*/
typedef struct _urcp_tftp_data{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 block_num;
    uint32 data_len;
}urcp_tftp_data_t;
typedef struct _urcp_tftp_ack{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 block_num;
}urcp_tftp_ack_t;

typedef struct _tftp_opt_t{
    urcp_tftp_sess_type_t op;
    cb_tftp cb_func;
    uint32 rport;
    char*  op_file_name;
}tftp_opt_t;

extern void urcp_tftp_init(void);
extern int32 urcp_tftp_process(urcp_infor_t* urcp_info);
extern uint16 urcp_tftp_send_file_request(uchar *macaddr, uchar* passwd, tftp_opt_t *opt);
extern int32 urcp_tftp_send_data_request(uchar *macaddr, uchar* passwd, uint32 type, const uchar* buffer, uint32 len, uint32 op);
extern void urcp_tftp_poll(void);
extern void urcp_tftp_session_clear_all(void);
extern void urcp_open_file(uchar* filename, uint32 len, const uchar* mode, FILE**fp);
#endif
