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
#include "os/platform/platform.h"
#include "alink_export.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *alink_sample_mutex;
/*do your job here*/
struct virtual_dev {
	char wlanSwitchState;
	char wlanSwitchScheduler;
} virtual_device = {
0x01,  0};

char *device_attr[2] = { "wlanSwitchState", "wlanSwitchScheduler"};

const char *main_dev_params =
    "{\"wlanSwitchState\": { \"value\": \"%d\" }, \"wlanSwitchScheduler\": { \"value\": \"%d\" }}";
static char device_status_change = 1;


int get_device_state()
{
    int ret=0;
    platform_mutex_lock(alink_sample_mutex);
    ret = device_status_change;
    platform_mutex_unlock(alink_sample_mutex);
    return ret;
}

int set_device_state(int state)
{
    platform_mutex_lock(alink_sample_mutex);
    device_status_change = state;
    platform_mutex_unlock(alink_sample_mutex);
    return state;
}


#define buffer_size 512
static int alink_device_post_data(alink_down_cmd_ptr down_cmd)
{
    alink_up_cmd up_cmd;
    int ret = ALINK_ERR;
    char *buffer = NULL;
    if (get_device_state()) {
        set_device_state(0);
        buffer = (char *)platform_malloc(buffer_size);
        if (buffer == NULL)
            return -1;
        memset(buffer, 0, buffer_size);
        sprintf(buffer, main_dev_params, virtual_device.wlanSwitchState,
                virtual_device.wlanSwitchScheduler);
        up_cmd.param = buffer;
        if (down_cmd != NULL) {
            up_cmd.target = down_cmd->account;
            up_cmd.resp_id = down_cmd->id;
        } else {
            up_cmd.target = NULL;
            up_cmd.resp_id = -1;
        }
        ret = alink_post_device_data(&up_cmd);
        if (ret == ALINK_ERR) {
            printf("post failed!\n");
            platform_msleep(2000);
        } else {
            printf("dev post data success!\n");
            device_status_change = 0;
        }
        if (buffer)
            free(buffer);
    }
    return ret;

}

/* do your job end */
int sample_running = ALINK_TRUE;
int main_dev_set_device_status_callback(alink_down_cmd_ptr down_cmd)
{
    int attrLen = 0, valueLen = 0, value = 0, i = 0;
    char *valueStr=NULL, *attrStr=NULL;

    /* do your job here */
    printf("%s %d \n", __FUNCTION__, __LINE__);
    printf("%s %d\n%s\n", down_cmd->uuid, down_cmd->method,down_cmd->param);
    set_device_state(1);

    for (i = 0; i < 2; i++) {
        attrStr = alink_JsonGetValueByName(down_cmd->param, strlen(down_cmd->param), device_attr[i], &attrLen, 0);
        valueStr = alink_JsonGetValueByName(attrStr, attrLen, "value", &valueLen, 0);

        if (valueStr && valueLen>0) {
            char lastChar = *(valueStr+valueLen);
            *(valueStr+valueLen) = 0;
            value = atoi(valueStr);
            *(valueStr+valueLen) = lastChar;
            switch (i) {
                case 0:
                    virtual_device.wlanSwitchState = value;
                    break;
                case 1:
                    virtual_device.wlanSwitchScheduler = value;
                    break;
                default:
                	break;
            }
        }
    }
    // ��������
    return 0;
    /* do your job end! */
}
int main_dev_get_device_status_callback(alink_down_cmd_ptr down_cmd)
{
	/* do your job here */
	printf("%s %d \n", __FUNCTION__, __LINE__);
	printf("%s %d\n%s\n", down_cmd->uuid, down_cmd->method,down_cmd->param);
	set_device_state(1);
    // app �ǵȷ��ػ���ֱ�ӻ�ȡ���������?
    // �첽
	return 0;
	/*do your job end */
}





int alink_handler_systemstates_callback(void *dev_mac, void *sys_state)
{
	char uuid[33] = { 0 };
	char *mac = (char *)dev_mac;
	enum ALINK_STATUS *state = (enum ALINK_STATUS *)sys_state;
	switch (*state) {
	case ALINK_STATUS_INITED:
		break;
	case ALINK_STATUS_REGISTERED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		printf("ALINK_STATUS_REGISTERED, mac %s uuid %s \n", mac,
			uuid);
		break;
	case ALINK_STATUS_LOGGED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		printf("ALINK_STATUS_LOGGED, mac %s uuid %s\n", mac, uuid);
		break;
	default:
		break;
	}
	return 0;
}



void alink_fill_deviceinfo(struct device_info *deviceinfo)
{				/*fill main device info here */
	product_get_name(deviceinfo->name);
	product_get_sn(deviceinfo->sn);  // ��Ʒע�᷽ʽ �����sn, ��ô��Ҫ����snΨһ
	product_get_key(deviceinfo->key);
	product_get_model(deviceinfo->model);
	product_get_secret(deviceinfo->secret);
	product_get_type(deviceinfo->type);
	product_get_version(deviceinfo->version);
	product_get_category(deviceinfo->category);
	product_get_manufacturer(deviceinfo->manufacturer);
	product_get_debug_key(deviceinfo->key_sandbox);
	product_get_debug_secret(deviceinfo->secret_sandbox);
	platform_wifi_get_mac(deviceinfo->mac);//��Ʒע��macΨһ or snΨһ  ͳһ��д
	product_get_cid(deviceinfo->cid); // ʹ�ýӿڻ�ȡΨһchipid,��α���豸
	printf("DEV_MODEL:%s \n", deviceinfo->model);
}

void main( )
{

	struct device_info *main_dev; 
    main_dev = platform_malloc(sizeof(struct device_info));
    alink_sample_mutex = platform_mutex_init();
	memset(main_dev, 0, sizeof(struct device_info));
	alink_fill_deviceinfo(main_dev);	
	alink_set_loglevel(ALINK_LL_DEBUG |
			   ALINK_LL_ERROR );
	main_dev->sys_callback[ALINK_FUNC_SERVER_STATUS] = alink_handler_systemstates_callback;


	/*start alink-sdk */

	main_dev->dev_callback[ACB_GET_DEVICE_STATUS] = main_dev_get_device_status_callback;
	main_dev->dev_callback[ACB_SET_DEVICE_STATUS] = main_dev_set_device_status_callback;
	alink_start(main_dev);	/*register main device here */

    platform_free(main_dev);

    printf("wait main device login");
	/*wait main device login, -1 means wait forever */
	alink_wait_connect(NULL, ALINK_WAIT_FOREVER);

	while (sample_running) {

		alink_device_post_data(NULL);
		platform_msleep(5000);
	}

	alink_end();
	platform_mutex_destroy(alink_sample_mutex);
}

