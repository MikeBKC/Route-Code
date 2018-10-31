#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <uttMachine.h>
#include "urcp_ap.h"
#include "urcp.h"

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)


//#define MAX_PASSWORD_LEN    18u
//#define	MAX_URCP_BATCH_NUM  10u

#define SW_MSG_KEY  0xAB00
#define RQ_MSG_KEY  0xAB09
#define SW_MSG_TYPE 0x38
#define MAX_MSG_SIZE 512

#ifndef MAC_LEN
#define MAC_LEN             6u
#endif

#define MSG_TYPE_URCP        0x03u
#define MSG_TYPE_URCP_DB    0x04u
#define REQUEST_AC_CHANGE          0x99u  /* AP发命令请求AC */
#define REQUEST_SEND_FILE_TO_AC    0x100u  /* AP发命令请求AC */

#define URCP_DB_SET           0xB1u /*urcp debug commond*/
#define URCP_DB_GET           0xB2u
/* AP 请求 AC命令分类 */

typedef struct _reply_cmd_msg_t {
	unsigned int opt_type;
	unsigned int status;
} reply_cmd_msg_t;

struct sw_msg_t {
	int msg_type;
	int msg_code;
	char buffer[MAX_MSG_SIZE];
};

struct msgbuf_t {
	long mtype;
	struct sw_msg_t sw_msg;
};
#if 0
/*密码验证操作消息*/
typedef struct _urcp_batch_msg_t {
    unsigned char dst_mac[MAC_LEN * MAX_URCP_BATCH_NUM];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_URCP_BATCH_NUM][MAX_PASSWORD_LEN];
    unsigned int  number;   /*要操作目的交换机数量*/
} urcp_batch_msg_t;
#endif
/*urcp change the config of ap*/
typedef struct _request_cmd_msg_t {
	unsigned int opt_type; /*操作类型*/
	unsigned int sn; /* AP序列号 */
	unsigned short name_len;
	unsigned short xml_len;
	char content[256];
} request_cmd_msg_t;

int sw_msg_init(void);
int sendwebmsgbuffer(int msg_type, int web_id, char* msg_buffer, unsigned int len);
int ap_msg_handle(void);
#endif
