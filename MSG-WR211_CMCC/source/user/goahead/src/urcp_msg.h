#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <typedef.h>
#include <mibunion.h>
#include <mib.h>
#include <conver.h>
#include <uttMachine.h>
#include "webs.h"

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)

#define DBCKS 0
#if DBCKS
#define DBCK(fmt, args...) \
			{ \
				printf("%s, %d, %s(), "fmt, __FILE__, __LINE__, __func__, ##args); \
				printf("\n"); \
			}
#else
#define DBCK(fmt, args...) 
#endif

typedef unsigned int uint32;
static int msg_id = -1;
static int sem_id = -1;

static int msgId = -1;

#define MAX_PASSWORD_LEN    18u
#define	MAX_URCP_BATCH_NUM  10u

#define SW_MSG_KEY  0xAB00
#define RQ_MSG_KEY  0xAB09
#define SW_MSG_TYPE 0x38
#define MAX_MSG_SIZE 512

#ifndef MAC_LEN
#define MAC_LEN             6u
#endif

#define MSG_TYPE_URCP        0x03u
#define REQUEST_AC_CHANGE    0x99u  /* AP发命令请求AC */
#define REQUEST_SEND_FILE_TO_AC  0x100u

/* AP 请求 AC命令分类 */
#define REQUEST_CHANGE_CONF 59u
#define REQUEST_FAT_TO_FIT  60u
#define REQUEST_SEND_FILE   61u

typedef enum {
    URCP_STATUS_WAIT=1,
    URCP_STATUS_OK,
    URCP_STATUS_TIME_OUT,
    URCP_STATUS_AUTH_ERROR, /*密码错误*/
    URCP_STATUS_FAIL,         /*操作失败 */
    URCP_STATUS_FOUND_NONE,         /*操作失败 */
    URCP_STATUS_END/*操作失败 */
} urcp_status_t;

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

/*密码验证操作消息*/
typedef struct _urcp_batch_msg_t {
    unsigned char dst_mac[MAC_LEN * MAX_URCP_BATCH_NUM];
    unsigned int  user_ip;   /*页面登陆用户ip*/
    unsigned char password[MAX_URCP_BATCH_NUM][MAX_PASSWORD_LEN];
    unsigned int  number;   /*要操作目的交换机数量*/
} urcp_batch_msg_t;

/*urcp change the config of ap*/
typedef struct _request_cmd_msg_t {
	unsigned int opt_type; /*操作类型*/
	unsigned int sn; /*序列号*/
	unsigned short name_len;
	unsigned short xml_len;
	char content[256];
} request_cmd_msg_t;

int sw_msg_init(void);
int urcp_ipc_init(void);
int sendwebmsgbuffer(int msg_type, int web_id, char* msg_buffer, unsigned int len);
void request_ac_send_msg(webs_t wp, uint32 request_type, uint32 u_misc_type);
int reply_ap_revice_msg(int *status);
#endif
