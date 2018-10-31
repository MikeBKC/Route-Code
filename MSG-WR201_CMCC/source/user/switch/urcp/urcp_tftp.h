/***********************************
 *
 *�����ļ�����Э��ͷ�ļ�
 *
 *KeHuatao
 *2011-04-01
 * *********************************/
#ifndef __URCP_TFTP_H__
#define __URCP_TFTP_H__
#include <stdio.h>
#include "urcp.h"

#define TFTP_BLOCK_LEN            1280u 
/*���ʱʱ��*/
#ifdef URCP_AP
#define TFTP_TIMEOUT_MS           (6400u/POLL_TIME)
#else
#define TFTP_TIMEOUT_MS           (4000u/POLL_TIME)
#endif
/*�ط���ʱʱ��*/
#ifdef URCP_AP
#define TFTP_MAX_TIMEOUT_MS        (8000u/POLL_TIME)
#else
#define TFTP_MAX_TIMEOUT_MS        (5000u/POLL_TIME)
#endif
#define TFTP_MAX_RETRIES          4u 
#define MAX_URCP_TFTP_SESSION     32u

/*��������--opcode*/
#define URCP_TFTP_READ_FILE       1u  
#define URCP_TFTP_WRITE_FILE      2u 
#define URCP_TFTP_READ_DATA       3u  
#define URCP_TFTP_WRITE_DATA      4u  
#define URCP_TFTP_DATA            5u  
#define URCP_TFTP_ACK             6u  
#define URCP_TFTP_ERROR           7u  
#define URCP_TFTP_OACK            8u  

/*����Ự�Ľ��̱�ʶ(urcp_tftp_hdr_t.remote_port)*/
#define PORT_NULL                 0u
#define PORT_CONFIG               1u
#define PORT_LOG                  2u
#define PORT_CONFIG_AUTO            3u
#define PORT_IMAGE                  4u
#define PORT_IMAGE_AUTO             5u
#define PORT_CONFIG_TEMPLATE        6u
#define PORT_CONFIG_TEMPLATE_AUTO   7u
/*end*/

/*�ļ�����󳤶�*/
#define MAX_FILE_NAME_LEN         63u

/**�Ự״̬*/
typedef enum{
    IDLE,
    WAIT_R_OACK,
    WAIT_OACK_ACK,
    WAIT_W_OACK,
    WAIT_DATA,
    WAIT_ACK,
    WAIT_FIN,
}urcp_tftp_state_t;

/*�Ựtype*/
typedef enum{
    TFTP_GET_FILE,   /*���Ͷ��ļ�����*/
    TFTP_SEND_FILE,  /*�յ����ļ�����������ļ�*/
    TFTP_PUT_FILE,  /*����д�ļ�����*/
    TFTP_REV_FILE,  /*�յ�д�ļ�����������ļ�*/

    TFTP_GET_DATA,   /*���Ͷ���������*/
    TFTP_SEND_DATA,  /*�յ�������������ļ�*/
    TFTP_PUT_DATA,  /*����д��������*/
    TFTP_REV_DATA,  /*�յ�д�����������������*/
}urcp_tftp_sess_type_t;

typedef void (*cb_tftp)(void *ptr, urcp_status_t state);

/********
 *
 *�Ự���ƽṹ
 *
 * ******/
typedef struct _urcp_tftp_session_info_t{
    uint16 remote_id;      /*Զ��id*/
    uint16 local_id;       /*����id*/
    uchar mac[MAC_LEN];    /*�ỰĿ�Ľ�����MAC��ַ*/
    urcp_tftp_sess_type_t  sess_type;       /*�Ự���ͣ���д�ļ�����ͨ����*/
    uint32 block_size;        /*���ݳ���*/    
    uint32 tftp_port;       /*���������ļ�ʱ��������յ����ļ��Ľ��̱�ʶ*/
    urcp_tftp_state_t state;  /*�Ự��ǰ״̬*/
    cb_tftp cb;
    int32 timer;           /*�Ự��ʱ��*/
    FILE* fd;              /*��������ļ�����ʾ��д�ļ�ָ�룬����ΪNULL*/
    uchar*  buffer;        /*������������ݣ���ʾ���껺������ʼ��ַ*/
    uchar*  buffer_ptr;        /*������������ݣ���ʾ��д����ָ��*/
    uint32  block_total;    /*�����ļ��������ܿ����������ж����һ����*/
    uint32  last_block_size;    /*���һ�����ݰ��ĳ���*/
    uint32  blockct;       /*�Ѿ����ͻ���յ��ļ������*/
    uint32  retries;        /*���ݰ��Ѿ��ط�����*/
    uchar*  last_pkt;      /*���ݵ���һ�η������ݰ�����ʼ��ַ*/
    uint32  last_pkt_len;   /*���ݵ���һ�η������ݰ��ĳ���*/
    uchar filename[MAX_FILE_NAME_LEN+1];        /*Ҫ��ȡ���ļ���*/
}urcp_tftp_session_info_t;

/*��������urcp tftp�Ự��״̬*/
typedef struct _urcp_tftp_session_t{
    uint16 id_counter;      /*��һ���Ự��ʹ�õ�id�����ڷ����»Ự*/
    urcp_tftp_session_info_t session[MAX_URCP_TFTP_SESSION];
}urcp_tftp_session_t;

typedef struct _urcp_tftp_hdr{
    uint32 opcode;
    uint16 lid;     /*�Ự�ı���id*/
    uint16 chksum;
    uint32 remote_port;  /*Զ�˵Ĵ�����̱�ʶ,�緢���������˺�Զ���輤�������*/
}urcp_tftp_hdr_t;

/*OACK����ͷ��*/
typedef struct _urcp_tftp_oack_hdr{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 blksize; /*�������ܵ�blocksize��С*/
    uint32 length; /*������ظ���OACK�����б�ʾ���ݻ��ļ�����*/
}urcp_tftp_oack_hdr_t;

/*���ļ�������Ļظ����ĸ�ʽ*/
typedef struct _urcp_tftp_file_request_t{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 blksize; /*�������ܵ�blocksize��С*/
    uint32 name_len; /*�ļ�������*/
    /*�ļ���*/
    /*д������л����ļ�����*/
}urcp_tftp_file_request_t;

typedef struct _urcp_tftp_data_request_t{
    urcp_tftp_hdr_t tftp_hdr;
    uint32 blksize; /*�������ܵ�blocksize��С*/
    uint32 data_type;
    uint32 data_len;/*д�����и�ֵ������*/
}urcp_tftp_data_request_t;
/*���ݰ���ʽ*/
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
