/*
 * Copyright (c) 2014-2015 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#ifndef __ZC_API_H__
#define __ZC_API_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    ZC_ERROR = -1,
    ZC_OK = 0,
};

enum
{
	ZC_FRAME_TYPE_PROBE_REQ = 0,
	ZC_FRAME_TYPE_CHECKED_IN = 1,
       ZC_FRAME_TYPE_AUTH = 2,
};

#define MAC_STR_LEN 18
#define MAC_HEX_LEN 6
#define ENROLLEE_ENTRY_MAX 5
#define DEVICE_ID_LEN_MAX 65
#define ZERO_CONFIG_IE_LEN_MAX 256
#define ALINK_IE_MAX_LEN    ZERO_CONFIG_IE_LEN_MAX

typedef struct
{
    char deviceid[DEVICE_ID_LEN_MAX];
    char mac[MAC_STR_LEN];
    char ie[ZERO_CONFIG_IE_LEN_MAX];
    int rssi;
    unsigned int status;//ZC_FRAME_TYPE_PROBE_REQ/ZC_FRAME_TYPE_CHECKED_IN
    unsigned long timestamp;//last report
    unsigned int period;//report period
}ZC_EnrolleeEntry_t;

typedef struct
{
    char mac[MAC_STR_LEN];
    unsigned int model_name_len;
	char  * model_name;
}zc_device_info_t;

int zc_init(void);
int zc_uninit(void);

typedef enum
{
    /* wireless tpsk & ie 处理消息 */
    NL_MSG_TPSK_UPGRADE = 14,                         //物连设备TPSK关联成功
    NL_MSG_IE_REPORT = 15,                                //ali私有IE上报

    /* dev online status 处理消息 */
    NL_MSG_DEV_ONLINE_STATUS = 16,                //设备在线状态消息
    NL_MSG_ENROLLEE_REPORT = 17,                    //zeroconfig ENROLLEE report 消息
    NL_MSG_ENROLLEE_AUTH_OK = 18,                    //zeroconfig ENROLLEE auth successfully消息
}alink_netlink_msg_type_t;

typedef struct
{
    alink_netlink_msg_type_t msg_type;
    int msg_len;
    void *body;
}alink_netlink_msg_t;

typedef struct
{
    char client_mac[MAC_STR_LEN];
    char ie_pkt[ALINK_IE_MAX_LEN];
}alink_ie_msg_t;

typedef struct
{
	int deviceType;
	char *traceToken;
	char *key;
	char *deviceId;
	char *algorithm;
}enrolleeCheckInInfo_t;

#ifdef __cplusplus
}
#endif

#endif // __ZC_API_H__

