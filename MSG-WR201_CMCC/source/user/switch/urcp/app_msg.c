/*
 *
 *
 *����Э��v3�洦��
 *
 *ke.huatao
 *2010-09-01
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h> /*inet_addr*/
#include <time.h>
#include <config/autoconf.h>
#include "common.h"

#ifdef URCP_WEB
#include <switch.h>
#include <sw_cfg.h>
#include <packet.h>
#else
#include "packet.h"
#endif
#include "urcp.h"

#include "app.h"
#include "app_msg.h"
#include "urcp_config.h"
#include "urcp_tftp.h"
#include "urcp_cmd.h"
#include "urcp_debug.h"
#ifdef URCP_AP
#include "swinfo.h"
#endif

#if (URCP_DISCONNECT_STA==FYES) 
#include "trap_sta.h"
#endif

#if (SSID_ENCODE_TYPE == FYES)
//#include <iconvs-user.h>
#endif

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#endif
#define TMP_FILE_NAME   16u


#ifdef URCP_WEB

void ProfInstLowHigh(MibProfileType profileType, int *max, int *min);
Pointer ProfGetInstPointByIndex(MibProfileType profileType, int index);
/*
 *��������һ���ȡ��Ϣ
 *bhou
 */
int32 urcp_msg_get_general(urcp_general_msg_t* urcp_general_msg, uint32 cmd_type, misc_opt_assist* misc_buff, void (*func_cb)(uint16 reqid, session_type_t type, urcp_status_t state))
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 id1 = 0u;
    uint32 ulen;
    int32 ret=-1;
    cmd_hdr_t* cmd;
	int iftag = 0;

    memset(buf, 0, sizeof(buf));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id1, func_cb)==0)
    {
	URCP_DEBUG(("%s\n", __func__));
	/*����Ϣ*/
	iftag = urcp_buid_header(buf, urcp_general_msg->dst_mac, (uint16)URCP_GET, id1, urcp_general_msg->password);

	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*������������*/
	ulen += add_cmd(cmd, cmd_type);
	/*��misc buffer�д�����¼*/

	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,id1,MISC_GET_GENERAL,misc_buff, (uchar*)NULL);

	/*����*/
	h2_send_frame(buf, ulen);
	ret = 0;
    }

    return ret;
}
/*
 *����һ���������õ���Ϣ
 *���ô˷����󣬱���misc buffer�д洢��conf��ϢΪת�����ֽ������Ϣ�������ֽ���
 *bhou
 */
int32 urcp_msg_set_general( urcp_general_msg_t* urcp_general_msg, uint32 cmd_type, misc_opt_assist* misc_buff,uchar* urcp_config, void (*func_cb)(uint16 reqid, session_type_t type, urcp_status_t state))

{
    uchar  buf[URCP_BUF_SIZE];
    uint16 id1 = 0u;
    uint32 ulen;
    int32 ret=0;
    cmd_hdr_t* cmd;
    uchar* cmd_data_ptr = NULL;
    uint16 conf_size = misc_buff->unit_size - sizeof(misc_header_t);/*���ݴ�С*/
	int iftag = 0;

    memset(buf, 0, sizeof(buf));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id1, func_cb)==0)
    {
	URCP_DEBUG(("%s\n", __func__));
	/*����Ϣ*/
	iftag = urcp_buid_header(buf, urcp_general_msg->dst_mac, (uint16)URCP_SET, id1, urcp_general_msg->password);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*������������*/
	ulen += add_cmd(cmd, cmd_type);
	/*������������*/
//	cmd_data_ptr = (uchar*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)]);
	cmd_data_ptr = (uchar*)(&buf[ulen]);
	memcpy(cmd_data_ptr, urcp_config,conf_size);
	/*��misc buffer�д�����¼*/
	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,id1,MISC_SET_GENERAL,misc_buff, urcp_config);/*�����urcp_configΪ�����ֽ���*/

/*��������*/
	ulen += conf_size;
	cmd -> length = ntohs(cmd->length);
	cmd -> length += conf_size;
	cmd -> length = htons(cmd->length);
	/*����*/
	URCP_DEBUG(("%s\n", __func__));
	h2_send_frame(buf, ulen);
    }
    else
    {
        ret = (int32)(-1);
    }

    return ret;
}
/***
 *�����������ݲ�����
 *����misc buffer�������ͻ�ȡurcp��������
 *���������Ͳ���ֻ����´˺��� 
 *bhou
 */
static uint32 get_urcp_cmd_alone_type(uint32 u_misc_type)
{
    uint32 u_cmd_type = 0u;
    switch(u_misc_type)
    {
	case MISC_REBOOT:/*����*/
	    u_cmd_type = CMD_REBOOT_CONFIG;
	    break;
	case MISC_RESET:/*�ָ���������*/
	    u_cmd_type = CMD_RESET_CONFIG;
	    break;
	case MISC_SAFEBIND:/*��ȫ��*/
	    u_cmd_type = CMD_SAFEBIND_CONFIG;
	    break;
	default:/*Ĭ�ϲ�����*/
	    break;

    }
    return u_cmd_type;
}
/*
 *������������������Ϣ
 *bhou
 */
int32 urcp_msg_cmd_alone_config(struct msgbuf_t *web_msg)
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen=0u, i=0u;
    int32 ret=0;
    urcp_cmd_alone_msg_t  urcp_cmd_alone_msg;
    uint32 u_misc_type=MISC_GET_GENERAL;/*Ĭ��Ϊ��*/
    misc_opt_assist misc_buff;
    uint32 urcp_cmd_type = 0u;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd; 

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(buf, 0, sizeof(buf));

    init_cmd_alone_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    /*������Ϣ*/
    memcpy(&urcp_cmd_alone_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_cmd_alone_msg_t));
    /*ͷ����Ϣ*/
	urcp_buid_header_with_8021q(buf, urcp_cmd_alone_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*ָ���ͷ ��tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*�������� */

    header ->flag = htons(FLAG_PASSWD);/*��Ҫ����*/

	cmd = (cmd_hdr_t*)(&buf[ulen]);

    /*��ȡ��������*/
    u_misc_type = urcp_cmd_alone_msg.opt_type;/*misc buffer��������*/
    urcp_cmd_type = get_urcp_cmd_alone_type(u_misc_type);/*��ȡurcp��������*/
    ulen += add_cmd(cmd, urcp_cmd_type);

    /*���η��Ͷ���� */
    for(i=0u;i<urcp_cmd_alone_msg.batch_info.number;i++)
    {
	/*����Ựid*/
	if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_cmd_alone_cb)==0)
	{
	    /*�޸�Ŀ��mac*/
	    memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_cmd_alone_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
	    /*��������id*/
	    header->request_id = htons(uid);
	    strncpy(header->password, urcp_cmd_alone_msg.batch_info.password[i], MAX_PASSWORD_LEN);
	    URCP_DEBUG(("%s: i:%d, passwd:%s\n", __func__, i, header->password));
	    /*��misc buffer�д�����¼*/
	    urcp_create_general_record(((eth_hdr_t*)buf)->dst_addr, urcp_cmd_alone_msg.batch_info.user_ip,uid,u_misc_type,&misc_buff,(uchar*)NULL);
		/*����*/
		urcp_send_by_vlan(buf,ulen);
	}
	else
	{
	    ret=(int32)(-1);
	    break;
	}
    }
    return ret;
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET_REPLY
/*
 *���������˿�������Ϣ
 *bhou
 */
int32 urcp_msg_get_port_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_port_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_PORT_CONFIG, &misc_buff,(cb_fp)urcp_port_conf_cb);/*��Ϣ����*/
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET_REPLY
/*
 *���������˿�������Ϣ
 *bhou
 */
int32 urcp_msg_set_port_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;
    urcp_port_conf_conf_t _port_config;
    uint32 i=0u;

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(&_port_config,0,sizeof(urcp_port_conf_conf_t));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_port_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/

    cfg_read_urcp_swap_conf((char*)(&_port_config), sizeof(urcp_port_conf_conf_t));/*��ȡ�����ڴ�������Ϣ*/

    _port_config.port_num = htonl(_port_config.port_num);/*�˿ڸ���*/
    for(i=0u;i<urcp_general_msg.port_ct;i++)/*ת���ֽ���*/
    {
	_port_config.port_conf_group[i].selected_mode = htonl(_port_config.port_conf_group[i].selected_mode);/*����ģʽ*/
	_port_config.port_conf_group[i].max_frame = htonl(_port_config.port_conf_group[i].max_frame);/*�������֡*/
    }
    return  urcp_msg_set_general(&urcp_general_msg,CMD_PORT_CONFIG,&misc_buff,(uchar*)(&_port_config), (cb_fp)urcp_port_conf_cb) ;
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
/*
 *����������������
 *bhou
 */
int32 urcp_msg_get_qos_rate_limit_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_QOS_RATE_LIMIT_CONFIG, &misc_buff,(cb_fp)urcp_qos_rate_limit_cb);/*��Ϣ����*/
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET_REPLY
/*
 *��������������������
 *bhou
 */
int32 urcp_msg_set_qos_rate_limit_config(struct msgbuf_t *web_msg)
{
    misc_opt_assist misc_buff;
    urcp_qos_rate_limit_msg_t urcp_qos_rate_limit_msg;
    urcp_general_msg_t* urcp_general_msg;
    urcp_qos_rate_limit_conf_t* conf_buff;
    uint32 i=0u;

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(&urcp_qos_rate_limit_msg,0,sizeof(urcp_qos_rate_limit_msg_t));

    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    memcpy(&urcp_qos_rate_limit_msg, web_msg->sw_msg.buffer, sizeof(urcp_qos_rate_limit_msg_t));/*��ȡ��Ϣ*/

    urcp_general_msg = &(urcp_qos_rate_limit_msg.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/
    conf_buff = &(urcp_qos_rate_limit_msg.qos_rate_limit_conf);/*ָ��������Ϣ�ṹ��*/

    conf_buff->rate_unit = htonl(conf_buff->rate_unit);/*������*/
    for(i=0u;i<conf_buff->port_num;i++)/*ת���ֽ���*/
    {
	conf_buff->qos_rate_limit_group[i].policer = htonl(conf_buff->qos_rate_limit_group[i].policer);/*ingress*/
	conf_buff->qos_rate_limit_group[i].shaper = htonl(conf_buff->qos_rate_limit_group[i].shaper);/*egress*/
    }
    conf_buff->port_num = htonl(conf_buff->port_num);/*�˿ڸ���*/

    return  urcp_msg_set_general(urcp_general_msg,CMD_QOS_RATE_LIMIT_CONFIG,&misc_buff,(uchar*)(conf_buff), (cb_fp)urcp_qos_rate_limit_cb) ;
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
/*
 *���������籩����
 *bhou
 */
int32 urcp_msg_get_storm_rate_limit_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_STORM_RATE_LIMIT_CONFIG, &misc_buff,(cb_fp)urcp_storm_rate_limit_cb);/*��Ϣ����*/
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET_REPLY
/*
 *���������籩����
 *bhou
 */
int32 urcp_msg_set_storm_rate_limit_config(struct msgbuf_t *web_msg)
{
    misc_opt_assist misc_buff;
    urcp_storm_rate_limit_msg_t urcp_storm_rate_limit_msg;
    urcp_general_msg_t* urcp_general_msg;
    urcp_storm_rate_limit_conf_t* conf_buff;

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(&urcp_storm_rate_limit_msg,0,sizeof(urcp_storm_rate_limit_msg_t));

    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    memcpy(&urcp_storm_rate_limit_msg, web_msg->sw_msg.buffer, sizeof(urcp_storm_rate_limit_msg_t));/*��ȡ��Ϣ*/

    urcp_general_msg = &(urcp_storm_rate_limit_msg.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/
    conf_buff = &(urcp_storm_rate_limit_msg.storm_rate_limit_conf);/*ָ��������Ϣ�ṹ��*/

    conf_buff->icmp_rate = htonl(conf_buff->icmp_rate);/*icmp*/
    conf_buff->bc_rate = htonl(conf_buff->bc_rate);/*�㲥*/
    conf_buff->mc_rate = htonl(conf_buff->mc_rate);/*�鲥*/
    conf_buff->uc_rate = htonl(conf_buff->uc_rate);/*����*/

    return  urcp_msg_set_general(urcp_general_msg,CMD_STORM_RATE_LIMIT_CONFIG,&misc_buff,(uchar*)(conf_buff), (cb_fp)urcp_storm_rate_limit_cb) ;
}
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
/*
 *��������ϵͳ��Ϣ��Ϣ
 *bhou
 */
int32 urcp_msg_get_sys_info_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_sys_info_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_SYS_INFO_CONFIG, &misc_buff,(cb_fp)urcp_sys_info_cb);/*��Ϣ����*/
}
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
/*
 *��������ϵͳ������Ϣ
 *bhou
 */
int32 urcp_msg_get_sys_conf_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_sys_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_SYS_CONF_CONFIG, &misc_buff,(cb_fp)urcp_sys_conf_cb);/*��Ϣ����*/
}
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_SET_REPLY
/*
 *��������ϵͳ������Ϣ
 *bhou
 */
int32 urcp_msg_set_sys_conf_config(struct msgbuf_t *web_msg)
{
    misc_opt_assist misc_buff;
    urcp_sys_conf_msg_t urcp_sys_conf_msg;
    urcp_general_msg_t* urcp_general_msg;
    urcp_sys_conf_conf_t* conf_buff;

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(&urcp_sys_conf_msg,0,sizeof(urcp_sys_conf_msg_t));

    init_sys_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
    memcpy(&urcp_sys_conf_msg, web_msg->sw_msg.buffer, sizeof(urcp_sys_conf_msg_t));/*��ȡ��Ϣ*/

    urcp_general_msg = &(urcp_sys_conf_msg.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/
    conf_buff = &(urcp_sys_conf_msg.sys_conf_conf);/*ָ��������Ϣ�ṹ��*/

    /*ת���ֽ���*/
    conf_buff->dhcp_relay_enable = htonl(conf_buff->dhcp_relay_enable);/*dhcp�м�*/
    conf_buff->dhcp_client_enable = htonl(conf_buff->dhcp_client_enable);/*dhcp�ͻ���*/
    conf_buff->ip_addr = htonl(conf_buff->ip_addr);/*ϵͳip*/
    conf_buff->ip_mask = htonl(conf_buff->ip_mask);/*ip����*/
    conf_buff->gw_ip = htonl(conf_buff->gw_ip);/*ϵͳ����*/
    conf_buff->cpu_vid = htonl(conf_buff->cpu_vid);/*cpu vid*/
    conf_buff->age_time = htonl(conf_buff->age_time);/*�ϻ�ʱ��*/
    conf_buff->arp_enable = htonl(conf_buff->arp_enable);/*�Ƿ���arp*/

    return  urcp_msg_set_general(urcp_general_msg,CMD_SYS_CONF_CONFIG,&misc_buff,(uchar*)(conf_buff), (cb_fp)urcp_sys_conf_cb) ;
}
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
/*
 *��������ip������Ϣ
 *bhou
 */
int32 urcp_msg_get_ip_filter_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
    init_ip_filter_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/

    /*��Ϣ����*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_IP_FILTER_CONFIG, &misc_buff,(cb_fp)urcp_ip_filter_cb);
}
#endif
#ifdef URCP_CMD_AGGR_CONFIG_SET_REPLY
/*
 *����������۵���Ϣ
 *bhou
 */
int32 urcp_msg_aggr_config(struct msgbuf_t *web_msg)
{
    uchar  buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    uint32 misc_type=MISC_GET_GENERAL;/*Ĭ��Ϊ��*/
    session_type_t sess_type=SESSION_TYPE_GET;/*Ĭ��Ϊ��*/
    uint16 u_urcp_type=URCP_GET;/*Ĭ��Ϊ��*/
    urcp_aggr_msg_t  urcp_aggr_msg;
    urcp_aggr_conf_t* aggr_config=NULL;/*Ĭ��Ϊnull����ģʽ*/
    cmd_hdr_t* cmd;
    int32 ret = -1;
	int iftag = 0;
    memset(buff, 0, sizeof(buff));

    memcpy(&urcp_aggr_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_aggr_msg_t));
    misc_type = urcp_aggr_msg.opt_type;
    if(misc_type == MISC_SET_GENERAL)/*���ò���*/
    {
	sess_type = SESSION_TYPE_SET;
	u_urcp_type = URCP_SET;
    }
    /*����Ựid*/
    if(session_alloc(sess_type, 0u, URCP_CMD_TIME_OUT, &uid, urcp_aggr_cb)==0)/*����ɹ�*/
    {
	ret = 0;
	/*����Ϣ*/
	iftag = urcp_buid_header(buff, urcp_aggr_msg.dst_mac,u_urcp_type, uid, urcp_aggr_msg.password);
	URCP_DEBUG(("%s\n", __func__));
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*��ȡ���*/
	ulen += add_cmd(cmd, CMD_AGGR_CONFIG);
	if(misc_type == MISC_SET_GENERAL)/*���ò���*/
	{
//	    aggr_config = (urcp_aggr_conf_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)]);/*��������ͷ���ȣ�ָ�����������׵�ַ*/
	    aggr_config = (urcp_aggr_conf_t*)(&buff[ulen]);/*��������ͷ���ȣ�ָ�����������׵�ַ*/
	    aggr_config->port_num = htonl(urcp_aggr_msg.port_ct);/*�˿ڸ���*/
	    memcpy(&(aggr_config->aggr_group[0]) , &(urcp_aggr_msg.aggr_group),sizeof(urcp_aggr_group_t));/*��������һ��������У�Ŀǰһ������һ�����������ö����������������飩*/
	    aggr_config->aggr_group[0].index=htons(aggr_config->aggr_group[0].index);/*���*/
	    aggr_config->aggr_group[0].member=htonl(aggr_config->aggr_group[0].member);/*��Ա*/
	    ulen += sizeof(urcp_aggr_conf_t);/*��������*/
	    cmd -> length = ntohs(cmd->length);/*���������*/
	    cmd -> length += sizeof(urcp_aggr_conf_t);
	    cmd -> length = htons(cmd->length);

	}
	/*��misc buffer�д�����¼*/
	urcp_create_aggr_record(urcp_aggr_msg.dst_mac, urcp_aggr_msg.user_ip, uid,misc_type, (urcp_aggr_conf_t*)NULL);
	/*����*/
	h2_send_frame(buff, ulen);
    }
    return ret;
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET_REPLY
/*
 *��������vlan����Ϣ
 *bhou
 */
int32 urcp_msg_vlan_config(struct msgbuf_t *web_msg)
{
    uchar  buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    uint32 u_misc_type=MISC_GET_VLAN;/*Ĭ��Ϊ��*/
    session_type_t sess_type=SESSION_TYPE_GET;/*Ĭ��Ϊ��*/
    uint16 u_urcp_type=URCP_GET;/*Ĭ��Ϊ��*/
    urcp_vlan_msg_t  urcp_vlan_msg;
    urcp_vlan_conf_t* vlan_config=NULL;
    cmd_hdr_t* cmd;
    int32 ret = -1;
	int iftag = 0;
    memset(buff, 0, sizeof(buff));

    memcpy(&urcp_vlan_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_vlan_msg_t));
    /*����Ϣ*/
    u_misc_type = urcp_vlan_msg.opt_type;/*misc buffer��������*/
    if(u_misc_type == MISC_SET_PVLAN)/*���ò���*/
    {
	sess_type = SESSION_TYPE_SET;
	u_urcp_type = URCP_SET;
    }
    /*����Ựid*/
    if(session_alloc(sess_type, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_vlan_cb)>=0)/*����ɹ�*/
    {
	ret = 0;
	iftag = urcp_buid_header(buff, urcp_vlan_msg.dst_mac,u_urcp_type , uid,urcp_vlan_msg.password);/*��ʼ��ͷ����Ϣ*/

	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/*��ȡ*/
	ulen += add_cmd(cmd, CMD_VLAN_CONFIG);/*��һ������Ϊvlan���á�*/
//	vlan_config = (urcp_vlan_conf_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)]);/*��������ͷ���ȣ�ָ�����������׵�ַ*/
	vlan_config = (urcp_vlan_conf_t*)(&buff[ulen]);/*��������ͷ���ȣ�ָ�����������׵�ַ*/
	vlan_config->misc_type = htonl(u_misc_type);/*misc opt type*/
	vlan_config->next_vlan_no = htons(urcp_vlan_msg.start_vlan_no);/*���ĸ���ʼ��ȡ*/

	URCP_DEBUG(("%s next_vlan_no:%d\n ", __func__,urcp_vlan_msg.start_vlan_no));

	vlan_config->port_num = htonl(urcp_vlan_msg.port_ct);/*�˿ڸ���*/
	if(u_misc_type == MISC_SET_PVLAN)/*����port vlan����*/
	{
	    memcpy(&(vlan_config->vlan_group[0]) , &(urcp_vlan_msg.vlan_group),sizeof(urcp_vlan_group_t));/*��������һ�����У�Ŀǰһ������һ�����������ö�������������飩*/
	    vlan_config->vlan_group[0].vlan_no=htons(vlan_config->vlan_group[0].vlan_no);/*���*/
	    vlan_config->vlan_group[0].member=htonl(vlan_config->vlan_group[0].member);/*��Ա*/
	    vlan_config->vlan_type = htonl(PORT_VLAN);
	}
	ulen += sizeof(urcp_vlan_conf_t);/*��������*/
	cmd -> length = ntohs(cmd->length);/*���������*/
	cmd -> length += sizeof(urcp_vlan_conf_t);
	cmd -> length = htons(cmd->length);

	/*��misc buffer�д�����¼*/
	urcp_create_vlan_record(urcp_vlan_msg.dst_mac, urcp_vlan_msg.user_ip, uid,u_misc_type,(urcp_vlan_conf_t*)NULL);/*��Ϊ����misc buffer�����־�����ʲô���͵�vlan����������u_misc_type*/
	/*����*/
	h2_send_frame(buff, ulen);
    }
    return ret;
}
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
/*
 *����������ȡtag vlan�˿�����
 *bhou
 */
int32 urcp_msg_qvlan_port_config(struct msgbuf_t *web_msg)
{
    uchar  buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    urcp_qvlan_pconf_msg_t  urcp_qvlan_pconf_msg;
    cmd_hdr_t* cmd;
    int32 ret = -1;
	int iftag = 0;

    memset(buff, 0, sizeof(buff));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_qvlan_pconf_cb)>=0)
    {
	ret = 0;
	/*����Ϣ*/
	memcpy(&urcp_qvlan_pconf_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_qvlan_pconf_msg_t));
	iftag = urcp_buid_header(buff, urcp_qvlan_pconf_msg.dst_mac, (uint16)URCP_GET, uid, urcp_qvlan_pconf_msg.password);
	URCP_DEBUG(("%s\n", __func__));
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/*��ȡ*/
	ulen += add_cmd(cmd, CMD_QVLAN_PCONF_CONFIG);
	/*��misc buffer�д�����¼*/
	urcp_create_qvlan_pconf_record(urcp_qvlan_pconf_msg.dst_mac, urcp_qvlan_pconf_msg.user_ip, uid, MISC_GET_GENERAL, (urcp_qvlan_pconf_conf_t*)NULL);

	/*����*/
	h2_send_frame(buff, ulen);
    }

    return ret;
}
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
/*
 *����������ȡRSTP LACP����Ϣ
 *bhou
 */
int32 urcp_msg_get_rstp_lacp(struct msgbuf_t *web_msg)
{
    uchar  buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    urcp_rstp_lacp_msg_t  urcp_rstp_lacp_msg;
    cmd_hdr_t* cmd;
    int32 ret = -1;
	int iftag = 0;

    memset(buff, 0, sizeof(buff));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_rstp_lacp_cb)==0)
    {
	ret = 0;
	/*����Ϣ*/
	memcpy(&urcp_rstp_lacp_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_rstp_lacp_msg_t));
	iftag = urcp_buid_header(buff, urcp_rstp_lacp_msg.dst_mac, (uint16)URCP_GET, uid, urcp_rstp_lacp_msg.password);
	URCP_DEBUG(("%s\n", __func__));
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/*��ȡ*/
	ulen += add_cmd(cmd, CMD_RSTP_LACP_CONFIG);
	/*��misc buffer�д�����¼*/
	urcp_create_rstp_lacp_record(urcp_rstp_lacp_msg.dst_mac, urcp_rstp_lacp_msg.user_ip, uid, MISC_GET_GENERAL, (urcp_rstp_lacp_conf_t*)NULL);

	/*����*/
	h2_send_frame(buff, ulen);
    }

    return ret;
}
#endif
#ifdef URCP_CMD_PASSWORD_CONFIG_SET_REPLY
/*
 *��������������Ϣ
 *ChenShiyu
 */
int32 urcp_msg_password_config(struct msgbuf_t *web_msg)
{
		URCP_DEBUG(("\n%s\n", __func__));
	uchar  buff[URCP_BUF_SIZE];
	uint16 uid = 0u;
	uint32 ulen, i;
	uint32 misc_type_pwd = MISC_GET_GENERAL;/*Ĭ��Ϊ��*/
	session_type_t sess_type = SESSION_TYPE_GET;/*Ĭ��Ϊ��*/
	uint16 u_urcp_type = URCP_GET;/*Ĭ��Ϊ��*/
	urcp_pwd_msg_t  urcp_pwd_msg;
	urcp_hdr_t* header;
	unsigned char *dev_passwd = NULL;
	cmd_hdr_t* cmd;
	int32 ret = -1;
	uchar *dstAddr;
	uchar *passwd;

	memset(buff, 0, sizeof(buff));
	/*������Ϣ*/
	memcpy(&urcp_pwd_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_pwd_msg_t));
	misc_type_pwd = urcp_pwd_msg.opt_type;
	if(misc_type_pwd == MISC_SET_GENERAL)
	{
		u_urcp_type = URCP_SET;
		sess_type = SESSION_TYPE_SET;
	}
	/*ͷ����Ϣ*/
	urcp_buid_header_with_8021q(buff, urcp_pwd_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*ָ���ͷ ��tag*/
    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*�������� */
	cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

	header ->flag = htons(FLAG_PASSWD);/*��Ҫ����*/
	/*����*/

	/*��ȡ*/
	ulen += add_cmd(cmd, CMD_PASSWORD_CONFIG);
	
	if (misc_type_pwd == MISC_SET_GENERAL)/*���ò���*/
	{
		dev_passwd = (unsigned char *)( &buff[ulen]);
		memcpy(dev_passwd, urcp_pwd_msg.dev_password, MAX_PASSWORD_LEN);
		URCP_DEBUG(("%s:,device password:%s\n", __func__, dev_passwd));
		ulen += MAX_PASSWORD_LEN;/*��������*/
		cmd->length = ntohs((uint16)(cmd->length));/*���������*/
		cmd->length += MAX_PASSWORD_LEN;
		cmd->length = htons((uint16)(cmd->length));
	}

	/*���η��Ͷ���� */
	for (i = 0u; i < urcp_pwd_msg.batch_info.number; i++)
	{
		/*����Ựid*/
		if(session_alloc(sess_type, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_password_cb) == 0)
		{
			ret = 0;
			/*�޸�Ŀ��MAC��ַ*/
			memcpy(((eth_hdr_t *)buff)->dst_addr, &urcp_pwd_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);
			/*��������id*/
			header->request_id = htons(uid);
			passwd = header->password;
			strncpy(passwd, urcp_pwd_msg.batch_info.password[i], MAX_PASSWORD_LEN);
			URCP_DEBUG(("%s: i:%d, passwd:%s\n", __func__, i, header->password));
			/*��misc buffer�д�����¼*/
			dstAddr=((eth_hdr_t *)buff)->dst_addr;
			urcp_create_password_record(dstAddr, urcp_pwd_msg.batch_info.user_ip, \
					uid, MISC_SET_GENERAL, NULL);
			/*����*/
			urcp_send_by_vlan(buff,ulen);

		}
		else
		{
			ret=(int32)(-1);
			break;
		}
		
	}

    return ret;
}
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_GET_REPLY
/*
 *����������ȡ�������Ϣ
 *KeHuatao
 */
int32 urcp_msg_get_mirror(struct msgbuf_t *web_msg)
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 id1;
    uint32 len;
    int32 ret=0;
    urcp_mirror_msg_t  urcp_mirror_msg;
    cmd_hdr_t* cmd;
	int iftag = 0;

    URCP_DEBUG(("%s\n", __func__));
    memset(buf, 0, sizeof(buf));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_mirror_cb)==0)
    {
	    /*����Ϣ*/
	memcpy(&urcp_mirror_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_mirror_msg_t));
	iftag = urcp_buid_header(buf, urcp_mirror_msg.dst_mac, (uint16)URCP_GET, id1, urcp_mirror_msg.password);
	URCP_DEBUG(("%s passwd:%s\n", __func__, urcp_mirror_msg.password));
	if(iftag == 1)
	{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/*��ȡ����*/
	len += add_cmd(cmd, CMD_MIRROR_CONFIG);
	/*��misc buffer�д�����¼*/
	urcp_create_mirror_record(urcp_mirror_msg.dst_mac, urcp_mirror_msg.user_ip, id1, MISC_GET_GENERAL, (urcp_mirror_conf_t*)NULL);

	/*����*/
	h2_send_frame(buf, len);
    }
    else
    {
	ret = (int32)(-1);
    }

    return ret;
}
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_SET_REPLY
/*
 *�����������þ������Ϣ
 *KeHuatao
 */
int32 urcp_msg_set_mirror(struct msgbuf_t *web_msg)
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 id1;
    uint32 len;
    int32 ret=0;
    urcp_mirror_msg_t  urcp_mirror_msg;
    cmd_hdr_t* cmd; 
    urcp_mirror_conf_t* mirror_config;
	int iftag = 0;

    memset(buf, 0, sizeof(buf));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_mirror_cb)==0)
    {
	/*������Ϣ*/
	memcpy(&urcp_mirror_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_mirror_msg_t));
	iftag = urcp_buid_header(buf, urcp_mirror_msg.dst_mac, (uint16)URCP_SET, id1, urcp_mirror_msg.password);
	if(iftag == 1)
	{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*����*/
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/*���þ�����Ϣ*/
	len += add_cmd(cmd, CMD_MIRROR_CONFIG);
	mirror_config = (urcp_mirror_conf_t*)(&buf[len]);
	mirror_config->port_num = urcp_mirror_msg.port_ct;
	mirror_config->mirror_port = urcp_mirror_msg.mirror_port;
	mirror_config->mirror_source = urcp_mirror_msg.mirror_source;
	URCP_DEBUG(("mirr:%d, %08x\n", mirror_config->mirror_port, mirror_config->mirror_source));
	/*��misc buffer�д�����¼*/
	urcp_create_mirror_record(urcp_mirror_msg.dst_mac, urcp_mirror_msg.user_ip, id1, MISC_SET_GENERAL, mirror_config);
	/*��������*/
	len += sizeof(urcp_mirror_conf_t);
	cmd -> length = ntohs(cmd->length);
	cmd -> length += sizeof(urcp_mirror_conf_t);
	cmd -> length = htons(cmd->length);
	mirror_config->mirror_port = htonl(mirror_config->mirror_port);
	mirror_config->mirror_source = htonl(mirror_config->mirror_source);
	/*����*/
	URCP_DEBUG(("%s\n", __func__));
	h2_send_frame(buf, len);
    }
    else
    {
        ret = (int32)(-1);
    }

    return ret;
}
#endif

#ifdef URCP_CMD_AP_BASIC_CONFIG_SET_GET_REPLY
int32 urcp_msg_get_ap_basic_config(struct msgbuf_t *web_msg)
{
    uchar buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    int32 ret = -1;
    cmd_hdr_t* cmd;
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;
	int iftag = 0;

    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
    {/*����ɹ�*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));
	init_ap_basic_misc_opt_assist(&misc_buff);/*��ʼ��*/
	memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*��ȡ��Ϣ*/
	/*����Ϣ*/
	URCP_DEBUG(("%s %d\n", __func__,__LINE__));
	iftag = urcp_buid_header(buff, urcp_general_msg.dst_mac, (uint16)URCP_GET, uid, urcp_general_msg.password);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*����*/
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	ulen += add_cmd(cmd, CMD_AP_WORK_MODE);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_DHCP_EN);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_DEV_NAME);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_IP_ADDR);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_NETMASK);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_GATEWAY);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_USER_NAME);
#if 0
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_MAX_CLIENT);
#endif
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_MAX_CLIENT_NEW);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_CHANNEL);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_ROAMING_TH);
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_EFFECT_CONF);
	/*��misc buffer�д�����¼*/
	urcp_create_general_record(urcp_general_msg.dst_mac,urcp_general_msg.user_ip,uid,MISC_GET_GENERAL,&misc_buff,(uchar*)NULL);
	h2_send_frame(buff, ulen);	/* ���� */ 
	//frame_dump(buff, ulen);
	ret = 0;
    }

    return ret;
}
int32 urcp_msg_set_ap_basic_config(struct msgbuf_t *msgbuf)
{
    uchar buff[URCP_BUF_SIZE], *cmd_data_ptr;
    uint16 uid=0u, len;
    uint32 ulen;
    cmd_hdr_t* cmd;
    int32 ret = -1;
    misc_opt_assist misc_buff;
    urcp_ap_basic_conf_msg_t urcp_ap_basic_msg;
    urcp_general_msg_t* urcp_general_msg;
	int iftag = 0;
    uint32 size = 0u;

    URCP_DEBUG(("%s %d\n", __func__,__LINE__));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
    {/*����ɹ�*/
	/*����Ϣ*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));

	init_ap_basic_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
	memcpy(&urcp_ap_basic_msg, msgbuf->sw_msg.buffer, sizeof(urcp_ap_basic_conf_msg_t));
	urcp_general_msg = &(urcp_ap_basic_msg.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/

	iftag = urcp_buid_header(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, urcp_general_msg->password);
//	printf("%s, iftag:%d\n", __func__, iftag);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*����*/
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/* working mode */
	/*������������*/
	ulen += add_cmd(cmd, CMD_AP_WORK_MODE);
	/*������������*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	urcp_ap_basic_msg.ap_basic_conf.work_mode = htonl(urcp_ap_basic_msg.ap_basic_conf.work_mode);
	memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.work_mode), sizeof(urcp_ap_basic_msg.ap_basic_conf.work_mode));
	ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.work_mode);/* �������� */
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.work_mode);
	cmd->length = htons(cmd->length);

	if(urcp_ap_basic_msg.ap_basic_conf.work_mode==htonl(1))
	{
	/* device name */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_DEV_NAME);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	len = sizeof(urcp_ap_basic_msg.ap_basic_conf.dev_name);
	if(len>=MAX_DEV_NAME_LEN)
	{
	    len = MAX_DEV_NAME_LEN - 1; 
	}
	memcpy(cmd_data_ptr, urcp_ap_basic_msg.ap_basic_conf.dev_name, len);
	ulen += len;/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += len;
	cmd->length = htons(cmd->length);

	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_DHCP_EN);
	/*������������*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	DBCK("ac dhcp_en = %d", urcp_ap_basic_msg.ap_basic_conf.dhcp_en);
	urcp_ap_basic_msg.ap_basic_conf.dhcp_en = htonl(urcp_ap_basic_msg.ap_basic_conf.dhcp_en);
	memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.dhcp_en), sizeof(urcp_ap_basic_msg.ap_basic_conf.dhcp_en));
	ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.dhcp_en);/* �������� */
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.dhcp_en);
	cmd->length = htons(cmd->length);

	if(urcp_ap_basic_msg.ap_basic_conf.dhcp_en ==0) {
		/* ip address */
		cmd = (cmd_hdr_t*)(&buff[ulen]); 
		ulen += add_cmd(cmd, CMD_IP_ADDR);
		cmd_data_ptr = (uchar *)(&buff[ulen]);
		urcp_ap_basic_msg.ap_basic_conf.ip_addr = htonl(urcp_ap_basic_msg.ap_basic_conf.ip_addr);
		memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.ip_addr), sizeof(urcp_ap_basic_msg.ap_basic_conf.ip_addr));
		//*cmd_data_ptr = htonl(urcp_ap_basic_msg_t.ap_basic_conf.ip_addr);
		ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.ip_addr);
		cmd->length = ntohs(cmd->length);
		cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.ip_addr);
		cmd->length = htons(cmd->length);

		/* netmask */
		cmd = (cmd_hdr_t*)(&buff[ulen]); 
		ulen += add_cmd(cmd, CMD_NETMASK);
		cmd_data_ptr = (uchar *)(&buff[ulen]);
		urcp_ap_basic_msg.ap_basic_conf.sub_mask = htonl(urcp_ap_basic_msg.ap_basic_conf.sub_mask);
		memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.sub_mask), sizeof(urcp_ap_basic_msg.ap_basic_conf.sub_mask));
		//*cmd_data_ptr = htonl(urcp_ap_basic_msg_t.ap_basic_conf.ip_addr);
		ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.sub_mask);
		cmd->length = ntohs(cmd->length);
		cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.sub_mask);
		cmd->length = htons(cmd->length);

		/* gateway address */
		cmd = (cmd_hdr_t*)(&buff[ulen]); 
		ulen += add_cmd(cmd, CMD_GATEWAY);
		cmd_data_ptr = (uchar *)(&buff[ulen]);
		urcp_ap_basic_msg.ap_basic_conf.gate_way = htonl(urcp_ap_basic_msg.ap_basic_conf.gate_way);
		memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.gate_way), sizeof(urcp_ap_basic_msg.ap_basic_conf.gate_way));
		ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.gate_way);
		cmd->length = ntohs(cmd->length);
		cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.gate_way);
		cmd->length = htons(cmd->length);
	}

	if(urcp_ap_basic_msg.ap_basic_conf.pass_modify==1)
	{
		/* password */
		cmd = (cmd_hdr_t*)(&buff[ulen]); 
		ulen += add_cmd(cmd, CMD_AP_PASSWORD);
		cmd_data_ptr = (uchar *)(&buff[ulen]);
		len = strlen(urcp_ap_basic_msg.ap_basic_conf.password);
		if(len>=MAX_PASSWORD_LEN)
		{
		    len = MAX_PASSWORD_LEN - 1; 
		}
		memcpy(cmd_data_ptr, urcp_ap_basic_msg.ap_basic_conf.password, len);
		ulen += len;/*��������*/
		cmd->length = ntohs(cmd->length);
		cmd->length += len;
		cmd->length = htons(cmd->length);
	    }
#if 0 
	    /* max clients */
	    cmd = (cmd_hdr_t*)(&buff[ulen]); 
	    ulen += add_cmd(cmd, CMD_AP_MAX_CLIENT);
	    cmd_data_ptr = (uchar *)(&buff[ulen]);
	    urcp_ap_basic_msg.ap_basic_conf.max_clt = htonl(urcp_ap_basic_msg.ap_basic_conf.max_clt);
	    memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.max_clt), sizeof(urcp_ap_basic_msg.ap_basic_conf.max_clt));
	    //*cmd_data_ptr = htonl(urcp_ap_basic_msg_t.ap_basic_conf.max_clt);
	    ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.max_clt);
	    cmd->length = ntohs(cmd->length);
	    cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.max_clt);
	    cmd->length = htons(cmd->length);
#endif
	    /* max clients */
	    cmd = (cmd_hdr_t*)(&buff[ulen]); 
	    ulen += add_cmd(cmd, CMD_AP_MAX_CLIENT_NEW);
	    cmd_data_ptr = (uchar *)(&buff[ulen]);
	    size = sizeof(urcp_ap_basic_msg.ap_basic_conf.sz_max_client.sz_num) +\
		   urcp_ap_basic_msg.ap_basic_conf.sz_max_client.sz_num * sizeof(urcp_ap_basic_conf_sz_info);
	    printf("%s, %d, %d\n", __func__, urcp_ap_basic_msg.ap_basic_conf.sz_max_client.sz_num, size);
	    urcp_ap_basic_msg.ap_basic_conf.sz_max_client.sz_num = htons(urcp_ap_basic_msg.ap_basic_conf.sz_max_client.sz_num);
	    memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.sz_max_client), size);
	    ulen += size;
	    cmd->length = ntohs(cmd->length);
	    cmd->length += size;
	    cmd->length = htons(cmd->length);

	    /* channel */
	    cmd = (cmd_hdr_t*)(&buff[ulen]); 
	    ulen += add_cmd(cmd, CMD_AP_CHANNEL);
	    cmd_data_ptr = (uchar *)(&buff[ulen]);
	    urcp_ap_basic_msg.ap_basic_conf.channel = htonl(urcp_ap_basic_msg.ap_basic_conf.channel);
	    memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.channel), sizeof(urcp_ap_basic_msg.ap_basic_conf.channel));
	    //*cmd_data_ptr = htonl(urcp_ap_basic_msg_t.ap_basic_conf.channel);
	    ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.channel);
	    cmd->length = ntohs(cmd->length);
	    cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.channel);
	    cmd->length = htons(cmd->length);

	    /* roaming */
	    cmd = (cmd_hdr_t*)(&buff[ulen]); 
	    ulen += add_cmd(cmd, CMD_AP_ROAMING_TH);
	    cmd_data_ptr = (uchar *)(&buff[ulen]);
	    urcp_ap_basic_msg.ap_basic_conf.dbm = htonl(urcp_ap_basic_msg.ap_basic_conf.dbm);
	    memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.dbm), sizeof(urcp_ap_basic_msg.ap_basic_conf.dbm));
	    //*cmd_data_ptr = htonl(urcp_ap_basic_msg_t.ap_basic_conf.channel);
	    ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.dbm);
	    cmd->length = ntohs(cmd->length);
	    cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.dbm);
	    cmd->length = htons(cmd->length);

	    /* effect configuration */
	    cmd = (cmd_hdr_t*)(&buff[ulen]); 
	    ulen += add_cmd(cmd, CMD_AP_EFFECT_CONF);
	    cmd_data_ptr = (uchar *)(&buff[ulen]);
	    urcp_ap_basic_msg.ap_basic_conf.effect_conf = htonl(urcp_ap_basic_msg.ap_basic_conf.effect_conf);
	    memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.effect_conf), sizeof(urcp_ap_basic_msg.ap_basic_conf.effect_conf));
	    ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.effect_conf);
	    cmd->length = ntohs(cmd->length);
	    cmd->length += sizeof(urcp_ap_basic_msg.ap_basic_conf.effect_conf);
	    cmd->length = htons(cmd->length);
	}    
	/* ���cmd->lengthת���������ֽ��� */
	/*��misc buffer�д�����¼*/
	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,uid,MISC_SET_GENERAL,&misc_buff, (uchar*)(&urcp_ap_basic_msg.ap_basic_conf));
	URCP_DEBUG(("%s %d %s ulen:%d\n", __func__,__LINE__, "h2_send_frame",ulen));
	h2_send_frame(buff, ulen);	/* ���� */ 
	//frame_dump(buff, ulen);
	ret = 0;
    }
    return ret;
}
#endif


#ifdef URCP_CMD_PASSWD_AUTH_GET_REPLY
/*
 *����������֤��
 *KeHuatao
 */
int32 urcp_msg_get_passwd_auth(struct msgbuf_t *web_msg, uint32 web_id)
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 id1;
    uint32 len, i;
    int32 ret=0;
    urcp_batch_msg_t  urcp_passwd_auth_msg;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd; 

    URCP_DEBUG(("%s\n", __func__));
    memset(buf, 0, sizeof(buf));
    /*������Ϣ*/
    memcpy(&urcp_passwd_auth_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_batch_msg_t));

    /*ͷ����Ϣ*/
    urcp_buid_header_with_8021q(buf, urcp_passwd_auth_msg.dst_mac, (uint16)URCP_GET, 0u, NULL);
    /*ָ���ͷ ��tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*�������� */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_PASSWD);/*��Ҫ����*/

    /*����*/
    if(web_id==GET_AUTH)
    {
	len += add_cmd(cmd, CMD_PASSWD_AUTH);
	URCP_DEBUG((" cmd->length : %d\n", ntohs(cmd->length)));
    }
    else
    {
	URCP_DEBUG(("%s: topoy\n", __func__));
	len += add_cmd(cmd, CMD_HELLO);

	/*��ȡ���к�*/
	cmd++;
	len += add_cmd(cmd, CMD_SERIAL_NO);
	header -> cmd_no += 1u; 

	/*��ȡ�豸����*/
	cmd++;
	len += add_cmd(cmd, CMD_DEV_NAME);
	header -> cmd_no += 1u; 

	/*��ȡ����汾 */
	cmd++;
	len += add_cmd(cmd, CMD_SW_VERSION);
	header -> cmd_no += 1u; 

	cmd++;
	len += add_cmd(cmd, CMD_PRODUCT_MODEL);
	header -> cmd_no += 1u; 
    }
    /*���η��Ͷ���� */
    for(i=0u;i<urcp_passwd_auth_msg.number;i++)
    {
	/*����Ựid*/
	if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_passwd_auth_cb)==0)
	{
	    /*�޸�Ŀ��mac��Ϣ*/
	    memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_passwd_auth_msg.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
	    /*��������id*/
	    header->request_id = htons(id1);
	    strncpy(header->password, urcp_passwd_auth_msg.password[i], MAX_PASSWORD_LEN);
	    URCP_DEBUG(("%s: i:%d, passwd:%s\n", __func__, i, header->password));
	    /*��misc buffer�д�����¼*/
	    urcp_create_passwd_auth_record(((eth_hdr_t*)buf)->dst_addr, urcp_passwd_auth_msg.user_ip, id1, MISC_GET_GENERAL);

		/*����*/
		urcp_send_by_vlan(&(buf[0]),len);
	}
	else
	{
	    ret=(int32)(-1);
	    break;
	}
    }

    return ret;
}
#endif

#ifdef URCP_CMD_SET_CONFIG_TEMPLATE_SET_REPEY
/*
 * Function: urcp_msg_set_ap_config_template()
 *
 * Description: send AP config template request.
 *
 * History: Shiyu.Chen, 2012.07.18
 */
int32 urcp_msg_set_ap_config_template(struct msgbuf_t *web_msg)
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 id1;
    uint32 len;
    int32 ret=0;
    cmd_hdr_t* cmd; 
    urcp_ap_conf_temp_msg_t conf_temp_msg;
    urcp_general_msg_t* urcp_general_msg = NULL;
    urcp_ap_conf_temp_conf_t *urcp_conf_temp = NULL, *tmp = NULL;
    misc_opt_assist misc_buff;
	int iftag = 0;

    URCP_DEBUG(("line %d\tfunction:%s()\n",__LINE__,  __func__));
    memset(&misc_buff,0,sizeof(misc_buff));
    init_ap_conf_temp_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/

    /*������Ϣ*/
    memset(&conf_temp_msg, 0, sizeof(conf_temp_msg));
    memcpy(&conf_temp_msg, web_msg->sw_msg.buffer, sizeof(urcp_ap_conf_temp_msg_t));
    urcp_general_msg = &(conf_temp_msg.general_msg);
    urcp_conf_temp = &(conf_temp_msg.conf_temp_conf);

    URCP_DEBUG(("tempname= %s", urcp_conf_temp->tempName));
    
    urcp_conf_temp->wireless = htonl(urcp_conf_temp->wireless);
    urcp_conf_temp->sgi = htonl(urcp_conf_temp->sgi);
    urcp_conf_temp->preamble = htonl(urcp_conf_temp->preamble);
    urcp_conf_temp->wmm = htonl(urcp_conf_temp->wmm);
    urcp_conf_temp->rate = htonl(urcp_conf_temp->rate);
    urcp_conf_temp->bw = htonl(urcp_conf_temp->bw);
    urcp_conf_temp->wlFre = htonl(urcp_conf_temp->wlFre);
    urcp_conf_temp->channel = htonl(urcp_conf_temp->channel);
    urcp_conf_temp->power = htonl(urcp_conf_temp->power);
    urcp_conf_temp->manPower = htonl(urcp_conf_temp->manPower);
    urcp_conf_temp->mode = htonl(urcp_conf_temp->mode);
    urcp_conf_temp->beaconPeriod = htonl(urcp_conf_temp->beaconPeriod);

    memset(buf, 0, sizeof(buf));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_ap_config_template_cb)==0)
    {
	iftag = urcp_buid_header(buf, urcp_general_msg->dst_mac, (uint16)URCP_SET, id1, urcp_general_msg->password);
	if(iftag == 1)
	{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*����*/
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	/*���þ�����Ϣ*/
	len += add_cmd(cmd, CMD_CONFIG_TEMPLATE);
	tmp = (urcp_ap_conf_temp_conf_t*)(&buf[len]);
	memcpy(tmp, urcp_conf_temp, sizeof(urcp_ap_conf_temp_conf_t));
	
	/*��misc buffer�д�����¼*/
	urcp_create_ap_conf_temp_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip, id1, MISC_SET_GENERAL);
	/*��������*/
	len += sizeof(urcp_ap_conf_temp_conf_t);
	cmd -> length = ntohs(cmd->length);
	cmd -> length += sizeof(urcp_ap_conf_temp_conf_t);
	cmd -> length = htons(cmd->length);
	
	/*����*/
	URCP_DEBUG(("%s\n", __func__));
	h2_send_frame(buf, len);
    }
    else
    {
        ret = (int32)(-1);
    }

    return ret;
}

#endif

#ifdef URCP_CMD_AP_JOIN_SZ_SET_REPLY
void ProfInstLowHigh(MibProfileType profileType, int *max, int *min);
Pointer ProfGetInstPointByIndex(MibProfileType profileType, int index);
int32 urcp_msg_set_ap_join_sz(struct msgbuf_t *msgbuf)
{
	DBCK();
    URCP_DEBUG(("\n< %s %d > begin\n", __func__,__LINE__));
    uchar buff[URCP_BUF_SIZE], *cmd_data_ptr;
    uint16 uid=0u;
    uint32 ulen;
    cmd_hdr_t* cmd;
    int i, j, n, min, max;
    int32 ret = -1;
    misc_opt_assist misc_buff;
    urcp_ap_join_sz_msg_t urcp_ap_join_sz_msg;
    urcp_general_msg_t* urcp_general_msg;
    char sz_name[(UTT_INST_NAME_LEN+1)*6];
    uchar tmp_sz_name[(UTT_INST_NAME_LEN+1)*6];
    char web_name[64];
    char *tmpName = NULL;
    char *ptr = NULL;
    char *index=NULL;
    char *index2=NULL;
    char sz_ssid[(UTT_SSID_LENTH+1)*4];
    uint32 sz_br[4];
    uint32 sz_isolate[4];
    uint32 sz_vid[4];
    uint32 sz_client[4];
    auth_mode sz_auth_mode[4];
    auth_wep sz_wep[4];
    auth_wpa sz_wpa[4];
    auth_wpa_psk sz_psk[4];
    ServiceZoneProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
#if(SSID_ENCODE_TYPE == FYES)
	char *ssid=NULL;
#endif
    urcp_dev_t* dev=NULL;
#if 0
    uint32 sz_dhcp[24];
    uint32 sz_startip[24];
    uint32 sz_endip[24];
    uint32 sz_gw[24];
    uint32 sz_netmask[24];
#endif

    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_join_sz_cb)==0)
    {/*����ɹ�*/
	/*����Ϣ*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));
	memset(web_name,0,sizeof(web_name));
	memset(&urcp_ap_join_sz_msg,0,sizeof(urcp_ap_join_sz_msg));
	memset(tmp_sz_name,0,sizeof(tmp_sz_name));

	init_ap_join_sz_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
	memcpy(&urcp_ap_join_sz_msg, msgbuf->sw_msg.buffer, sizeof(urcp_ap_join_sz_msg));
	urcp_general_msg = &(urcp_ap_join_sz_msg.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/
    //printf("\n< %s %d > mac:%s\n", __func__,__LINE__,str_macaddr(urcp_general_msg->dst_mac));

#if 1  /* Ϊ�˼���֮ǰ�İ汾����ʱ��Ҫִ������,֮��İ汾�·�������֮��AP ���Ӧһ��ACK, ��AP ������1.9.3 �汾�󣬿�ע�͵�����, ����ACK������������ */
#if SZONE_AUTO_UPDATA
    dev = urcp_dev_table_lookup(urcp_general_msg->dst_mac, 0u);
    if(dev != NULL){
        uttSemP(SEM_SZONE_UPDATA, 0);
        dev->wl_info.szone_info.szone_changed = 0;
        dev->wl_info.szone_info.r_times = 0;
        dev->wl_info.szone_info.count = 0;
        uttSemV(SEM_SZONE_UPDATA, 0);
    }
#endif
#endif

	urcp_buid_header_with_8021q(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, urcp_general_msg->password[0] ? urcp_general_msg->password : NULL);
	ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
#if 0
	memset(sz_status, 0, sizeof(sz_status));
#endif
	memset(sz_name, '\0', sizeof(sz_name));
	memset(sz_ssid, '\0', sizeof(sz_ssid));
	memset(sz_br, 0, sizeof(sz_br));
	memset(sz_isolate, 0, sizeof(sz_isolate));
	memset(sz_vid, 0, sizeof(sz_vid));
	memset(sz_client, 0, sizeof(sz_client));
	memset(sz_auth_mode, 0, sizeof(sz_auth_mode));
	memset(sz_wep, 0, sizeof(sz_wep));
	memset(sz_wpa, 0, sizeof(sz_wpa));
	memset(sz_psk, 0, sizeof(sz_psk));
#if 0
	memset(sz_dhcp, 0, sizeof(sz_dhcp));
	memset(sz_startip, 0, sizeof(sz_startip));
	memset(sz_endip, 0, sizeof(sz_endip));
	memset(sz_gw, 0, sizeof(sz_gw));
	memset(sz_netmask, 0, sizeof(sz_netmask));
#endif
	strncpy(tmp_sz_name, urcp_ap_join_sz_msg.sz_name, sizeof(tmp_sz_name));
	tmpName = strtok_r(urcp_ap_join_sz_msg.sz_name, ",", &ptr);
	ProfInstLowHigh(mibType, &max, &min);
	j=0;
	n=0;
	while(tmpName != NULL)
	{
	    for (i=0; i<max; i++)
	    {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, i);
		if ((prof != NULL) && (strcmp(tmpName, prof->head.name) == 0))
		{
		    if (j != 0)
		    {
			index = strchr(sz_name, '\0');
			index[0]=',';
			strncpy(&index[1], prof->head.name, (UTT_INST_NAME_LEN+1));
		    }
		    else
		    {
			strncpy(sz_name, prof->head.name, (UTT_INST_NAME_LEN+1));
		    }
		    if (j != 0)
		    {
			index2 = strchr(sz_ssid, '\0');
			index2[0]=',';
#if(SSID_ENCODE_TYPE == FYES)
               if(prof->ssid_encode_type == 1)// 1Ϊ�ֻ����ȣ�������ת��Ϊutf-8��ʽ
			   {
			       ssid = iconv_string(prof->ssid,"GBK","UTF-8");
			       strncpy(&index2[1], ssid, (UTT_SSID_LENTH+1));
			   }else{
			       strncpy(&index2[1], prof->ssid, (UTT_SSID_LENTH+1));
			   }
               if(ssid != NULL)
               {
                   free(ssid);
                   ssid = NULL;
               }
#else
			strncpy(&index2[1], prof->ssid, (UTT_SSID_LENTH+1));
#endif
		    }
		    else
		    {
#if(SSID_ENCODE_TYPE == FYES)
               if(prof->ssid_encode_type == 1)// 1Ϊ�ֻ����ȣ�������ת��Ϊutf-8��ʽ
			   {
			       ssid = iconv_string(prof->ssid,"GBK","UTF-8");
			       strncpy(sz_ssid, ssid, (UTT_SSID_LENTH+1));
			   }else{
			       strncpy(sz_ssid, prof->ssid, (UTT_SSID_LENTH+1));
			   }
               if(ssid != NULL)
               {
                   free(ssid);
                   ssid = NULL;
               }
#else
			strncpy(sz_ssid, prof->ssid, (UTT_SSID_LENTH+1));
#endif
		    }
		    sz_br[n] = htonl(prof->ssidBcast);
		    sz_isolate[n] = htonl(prof->ssidIsolate);
		    sz_vid[n] = htonl(prof->vid);
		    sz_client[n] = htonl(prof->client);
		    sz_auth_mode[n] = htonl(prof->securMode);
		    sz_wep[n].auMethod = htonl(prof->wepSecurity.auMethod);
		    sz_wep[n].kType = htonl(prof->wepSecurity.kType);
		    sz_wep[n].kNum1.key_state_action = htonl((prof->wepSecurity.kNum1.key_state_action==PROF_ENABLE?1:0));
		    strncpy(sz_wep[n].kNum1.passwd, prof->wepSecurity.kNum1.passwd, UTT_PAS_KEY_VALUE_LEN+1);
		    sz_wep[n].kNum1.kFmt = htonl(prof->wepSecurity.kNum1.kFmt);
		    sz_wep[n].kNum2.key_state_action = htonl((prof->wepSecurity.kNum2.key_state_action==PROF_ENABLE?1:0));
		    strncpy(sz_wep[n].kNum2.passwd, prof->wepSecurity.kNum2.passwd, UTT_PAS_KEY_VALUE_LEN+1);
		    sz_wep[n].kNum2.kFmt = htonl(prof->wepSecurity.kNum2.kFmt);
		    sz_wep[n].kNum3.key_state_action = htonl((prof->wepSecurity.kNum3.key_state_action==PROF_ENABLE?1:0));
		    strncpy(sz_wep[n].kNum3.passwd, prof->wepSecurity.kNum3.passwd, UTT_PAS_KEY_VALUE_LEN+1);
		    sz_wep[n].kNum3.kFmt = htonl(prof->wepSecurity.kNum3.kFmt);
		    sz_wep[n].kNum4.key_state_action = htonl((prof->wepSecurity.kNum4.key_state_action==PROF_ENABLE?1:0));
		    strncpy(sz_wep[n].kNum4.passwd, prof->wepSecurity.kNum4.passwd, UTT_PAS_KEY_VALUE_LEN+1);
		    sz_wep[n].kNum4.kFmt = htonl(prof->wepSecurity.kNum4.kFmt);
		    sz_wpa[n].wVersion = htonl(prof->wpaSecurity.wVersion);
		    sz_wpa[n].sAlg = htonl(prof->wpaSecurity.sAlg);
		    sz_wpa[n].radiusIp = prof->wpaSecurity.radiusIp;
		    sz_wpa[n].radiusPort = htonl(prof->wpaSecurity.radiusPort);
		    strncpy(sz_wpa[n].radiusPasswd, prof->wpaSecurity.radiusPasswd, UTT_RADIUS_KEY_LEN);
		    sz_wpa[n].updataTime = htonl(prof->wpaSecurity.updataTime);
		    sz_psk[n].wVersion = htonl(prof->wpaPskSecurity.wVersion);
		    sz_psk[n].sAlg = htonl(prof->wpaPskSecurity.sAlg);
		    strncpy(sz_psk[n].sharePasswd, prof->wpaPskSecurity.sharePasswd, UTT_RADIUS_KEY_LEN);
		    sz_psk[n].updataTime = htonl(prof->wpaPskSecurity.updataTime);
		    n++;
		    break;
		}
	    }
	    tmpName = strtok_r(NULL, ",", &ptr);
	    j++;
	}
#if 0
	for (i=0; i<4; i++)
	{
    URCP_DEBUG(("\n < %s %d > sz_wpa[%d].radiusPasswd:%s port:%d time:%d\n", __func__,__LINE__,i,sz_wpa[i].radiusPasswd,ntohl(sz_wpa[i].radiusPort), ntohl(sz_wpa[i].updataTime)));
	}

	for (i=0; i<num; i++)
	{
	    sz_status[i] = htonl((urcp_ap_join_sz_msg[i].ap_join_sz.serviceEn));
	    if (i != 0)
	    {
		sz_name[(UTT_INST_NAME_LEN+1)*i] = ',';
	    }
	    strncpy(sz_name, urcp_ap_join_sz_msg[i].ap_join_sz.szName, (UTT_INST_NAME_LEN+1));
	    if (i != 0)
	    {
		sz_ssid[(UTT_SSID_LENTH+1)*i] = ',';
	    }
	    strncpy(sz_ssid, urcp_ap_join_sz_msg[i].ap_join_sz.ssid, (UTT_SSID_LENTH+1));
	    sz_br[i] = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.ssidBcast);
	    sz_vid[i] = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.vid);
	    sz_auth_mode[i] = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.securMode);
	    sz_wep[i].auMethod = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.auMethod);
	    sz_wep[i].kType = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kType);
	    sz_wep[i].kNum1.key_state_action = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum1.key_state_action);
	    strncpy(sz_wep[i].kNum1.passwd, urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum1.passwd, UTT_PASSWD_LEN);
	    sz_wep[i].kNum1.kFmt = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum1.kFmt);
	    sz_wep[i].kNum2.key_state_action = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum2.key_state_action);
	    strncpy(sz_wep[i].kNum2.passwd, urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum2.passwd, UTT_PASSWD_LEN);
	    sz_wep[i].kNum2.kFmt = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum2.kFmt);
	    sz_wep[i].kNum3.key_state_action = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum3.key_state_action);
	    strncpy(sz_wep[i].kNum3.passwd, urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum3.passwd, UTT_PASSWD_LEN);
	    sz_wep[i].kNum3.kFmt = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wepSecurity.kNum3.kFmt);
	    sz_wpa[i].wVersion = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaSecurity.wVersion);
	    sz_wpa[i].sAlg = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaSecurity.sAlg);
	    sz_wpa[i].radiusIp = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaSecurity.radiusIp);
	    sz_wpa[i].radiusPort = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaSecurity.radiusPort);
	    strncpy(sz_wpa[i].radiusPasswd, urcp_ap_join_sz_msg[i].ap_join_sz.wpaSecurity.radiusPasswd, UTT_PASSWD_LEN);
	    sz_wpa[i].updataTime = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaSecurity.updataTime);
	    sz_psk[i].wVersion = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaPskSecurity.wVersion);
	    sz_psk[i].sAlg = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaPskSecurity.sAlg);
	    strncpy(sz_psk[i].sharePasswd, urcp_ap_join_sz_msg[i].ap_join_sz.wpaPskSecurity.sharePasswd, UTT_PASSWD_LEN);
	    sz_psk[i].updataTime = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.wpaPskSecurity.updataTime);
	    sz_dhcp[i] = htonl(urcp_ap_join_sz_msg[i].ap_join_sz.dhcpEn);
	    sz_startip[i] = urcp_ap_join_sz_msg[i].ap_join_sz.startIp;
	    sz_endip[i] = urcp_ap_join_sz_msg[i].ap_join_sz.endIp;  
	    sz_gw[i] = urcp_ap_join_sz_msg[i].ap_join_sz.gw;
	    sz_netmask[i] = urcp_ap_join_sz_msg[i].ap_join_sz.netmask;
	}
#endif
    
	/* sz name */
	ulen += add_cmd(cmd, CMD_SZ_NAME);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_name, sizeof(sz_name));
	ulen += sizeof(sz_name);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_name);
	cmd->length = htons(cmd->length);
#if 0
	/* sz status */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_STATUS);
	/*������������*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_status, sizeof(sz_status));
	ulen += sizeof(sz_status);/* �������� */
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_status);
	cmd->length = htons(cmd->length);
#endif
	/* ssid */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_SSID);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_ssid, sizeof(sz_ssid));
	ulen += sizeof(sz_ssid);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_ssid);
	cmd->length = htons(cmd->length);

	/* brodcast */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_SSID_BC);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_br, sizeof(sz_br));
	ulen += sizeof(sz_br);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_br);
	cmd->length = htons(cmd->length);

	/* isolate */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_SSID_ISOLATE);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_isolate, sizeof(sz_isolate));
	ulen += sizeof(sz_isolate);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_isolate);
	cmd->length = htons(cmd->length);

	/* vid */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_VID);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_vid, sizeof(sz_vid));
	ulen += sizeof(sz_vid);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_vid);
	cmd->length = htons(cmd->length);
	
#if 0
	/* Max client */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_MAX_CLIENT);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_client, sizeof(sz_client));
	ulen += sizeof(sz_client);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_client);
	cmd->length = htons(cmd->length);
#endif

	/* auth type */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_AUTH);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_auth_mode, sizeof(sz_auth_mode));
	ulen += sizeof(sz_auth_mode);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_auth_mode);
	cmd->length = htons(cmd->length);
	
	/* wep */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_WEP);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_wep, sizeof(sz_wep));
	ulen += sizeof(sz_wep);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_wep);
	cmd->length = htons(cmd->length);
	
	/* wpa */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_WPA);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_wpa, sizeof(sz_wpa));
	ulen += sizeof(sz_wpa);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_wpa);
	cmd->length = htons(cmd->length);
	
	/* psk */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_PSK);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_psk, sizeof(sz_psk));
	ulen += sizeof(sz_psk);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_psk);
	cmd->length = htons(cmd->length);
	
#if 0
	/* dhcp fun */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_DHCP);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_dhcp, sizeof(sz_dhcp));
	ulen += sizeof(sz_dhcp);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_dhcp);
	cmd->length = htons(cmd->length);

	/* start ip */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_START_IP);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
 	memcpy(cmd_data_ptr, sz_startip, sizeof(sz_startip));
	ulen += sizeof(sz_startip);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_startip);
	cmd->length = htons(cmd->length);

	/* end ip */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_END_IP);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
 	memcpy(cmd_data_ptr, sz_endip, sizeof(sz_endip));
	ulen += sizeof(sz_endip);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_endip);
	cmd->length = htons(cmd->length);

	/* gateway */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_GW);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
 	memcpy(cmd_data_ptr, sz_gw, sizeof(sz_gw));
	ulen += sizeof(sz_gw);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_gw);
	cmd->length = htons(cmd->length);

	/* netmask */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_SUBMASK);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
 	memcpy(cmd_data_ptr, sz_netmask, sizeof(sz_netmask));
	ulen += sizeof(sz_netmask);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_netmask);
	cmd->length = htons(cmd->length);
	    
	/*��misc buffer�д�����¼*/
	for (i=0; i<4; i++)
	{
    URCP_DEBUG(("\n< %s %d > kType:%d\n", __func__,__LINE__,ntohl(sz_wep[i].kType)));
    URCP_DEBUG(("\n< %s %d > pw1:%s\n", __func__,__LINE__,sz_wep[i].kNum1.passwd));
    URCP_DEBUG(("\n< %s %d > pw2:%s\n", __func__,__LINE__,sz_wep[i].kNum2.passwd));
    URCP_DEBUG(("\n< %s %d > pw3:%s\n", __func__,__LINE__,sz_wep[i].kNum3.passwd));
    URCP_DEBUG(("\n< %s %d > pw4:%s\n", __func__,__LINE__,sz_wep[i].kNum4.passwd));
	}
#endif
#if 0
	for (i=0; i<6; i++)
	{
    URCP_DEBUG(("\n< %s %d > %02X\n", __func__,__LINE__,urcp_general_msg->dst_mac[i]));
	}
    URCP_DEBUG(("\n"));
#endif
    //printf("\n< %s %d > mac:%s ulen:%d sz_psk:%d ip:%s uid:%d tmp_sz_name:%s\n", __func__,__LINE__,str_macaddr(urcp_general_msg->dst_mac),ulen,sizeof(sz_psk),int2ip(urcp_general_msg->user_ip,ip),uid,tmp_sz_name);
//	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,uid,MISC_SET_GENERAL,&misc_buff, (uchar*)(tmp_sz_name));
	urcp_create_ap_join_sz_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,uid,MISC_SET_GENERAL);
    DBCK("ulen:%d\n", ulen);
	/*����*/
	urcp_send_by_vlan(buff,ulen);
	ret = 0;
    }
    URCP_DEBUG(("\n< %s %d > end\n", __func__,__LINE__));
    return ret;
}
#endif

#ifdef URCP_CMD_AP_MAC_FILTER_SET_REPLY
int32 urcp_msg_set_ap_mac_filter(struct msgbuf_t *msgbuf)
{
	DBCK();
    uchar buff[URCP_BUF_SIZE], *cmd_data_ptr;
	uchar mf[10][UTT_INST_NAME_LEN +1];
    uint16 uid=0u;
    uint32 ulen;
    cmd_hdr_t* cmd;
    int i, j, min, max, num = 0;
	char *tmp = NULL;
	char *ptr = NULL;
	uchar str[(UTT_INST_NAME_LEN + 1) * 4 + 4];
    int32 ret = -1;
	int iftag = 0;
    misc_opt_assist misc_buff;
    urcp_ap_mac_filter_msg_t urcp_ap_mac_filter_msg;
    urcp_general_msg_t* urcp_general_msg;
	ServiceZoneProfile *prof = NULL;

    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_mac_filter_cb)==0)
    {/*����ɹ�*/
	/*����Ϣ*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));
	memset(&urcp_ap_mac_filter_msg,0,sizeof(urcp_ap_mac_filter_msg));
	memset(str,0,sizeof(str));
//	memset(ssid, 0, sizeof(ssid));

	init_ap_mac_filter_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
	memcpy(&urcp_ap_mac_filter_msg, msgbuf->sw_msg.buffer, sizeof(urcp_ap_mac_filter_msg));
	urcp_general_msg = &(urcp_ap_mac_filter_msg.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/

	iftag = urcp_buid_header(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, urcp_general_msg->password);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*����*/
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*��λ��cmd*/
	}

	DBCK("ulen=%d,cmd_hdr_t len=%d\n",ulen,sizeof(cmd_hdr_t));
	strncpy(str, urcp_ap_mac_filter_msg.name, sizeof(str));
	DBCK("str=%s\n",str);
	tmp = strtok_r(str, ",", &ptr);
	DBCK("tmp=%s\n",tmp);
	while (tmp != NULL) {
		memcpy(&mf[num], tmp, UTT_INST_NAME_LEN + 1);
		num++;
		tmp = strtok_r(NULL, ",", &ptr);
	}

	ulen += add_cmd(cmd, CMD_MAC_FILTER_NUM);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, &num, sizeof(num));
	DBCK("mac_filter_num=%d\n",*(uint32*)cmd_data_ptr);
	ulen += sizeof(num);/*��������*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(num);
	cmd->length = htons(cmd->length);


	if (num > 0) {
		cmd = (cmd_hdr_t*)(&buff[ulen]);
		ulen += add_cmd(cmd, CMD_MAC_FILTER);

		ProfInstLowHigh(MIB_PROF_AP_MAC_FILTER, &max, &min);
		for (i = 0; i < num; i++) {
			for (j = min; j < max; j++) {
				prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_MAC_FILTER, j);
				if(prof->head.name !='\0'){
				if (0 == strncmp(prof->head.name, &mf[i], UTT_INST_NAME_LEN + 1)) {
					cmd_data_ptr = (uchar *)(&buff[ulen]);
					memcpy(cmd_data_ptr, prof, sizeof(ApMacFilterProfile));
					ulen += sizeof(ApMacFilterProfile);/*��������*/
					cmd->length = ntohs(cmd->length);
					cmd->length += sizeof(ApMacFilterProfile);
					cmd->length = htons(cmd->length);
				}
				}
			}
		}
	}else{
		cmd = (cmd_hdr_t*)(&buff[ulen]);
		ulen += add_cmd(cmd, CMD_MAC_FILTER);
	}

	urcp_create_ap_mac_filter_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,uid,MISC_SET_GENERAL);
    DBCK("ulen:%d\n", ulen);
	h2_send_frame(buff, ulen);	/* ���� */ 
	ret = 0;
    }
    URCP_DEBUG(("\n< %s %d > end\n", __func__,__LINE__));
    return ret;
}
#endif

#ifdef URCP_CMD_SET_AC_MODE_REPLY
/*
 */
int32 urcp_msg_set_ac_work_mode(struct msgbuf_t *web_msg)
{
    uchar  buffer[URCP_BUF_SIZE], *cmd_data_ptr;
    uint16 id;
    int32 ret=0;
    uint32 len;
    urcp_ac_set_mode_msg_t urcp_ac_set_mode_msg;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd; 


    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
	return -1;
    }

    memset(buffer, 0, sizeof(buffer));

    memcpy(&urcp_ac_set_mode_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_ac_set_mode_msg_t));

    urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_SET, id, NULL);
	/*ָ���ͷ ��tag*/
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*�������� */
	cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*����Ҫ����*/

    len += add_cmd(cmd, CMD_SETACMODE_CONFIG);

	cmd_data_ptr = (uchar *)(&buffer[len]);
    *cmd_data_ptr = urcp_ac_set_mode_msg.ac_mode;

	len += sizeof(urcp_ac_set_mode_msg.ac_mode);
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(urcp_ac_set_mode_msg.ac_mode);
	cmd->length = htons(cmd->length);

	urcp_send_by_vlan(buffer,len);

    return ret;

}
#endif

#ifdef URCP_CMD_CHANGE_CHANNEL_REPLY
/*
 */
int32 urcp_msg_change_channel(struct msgbuf_t *web_msg)
{
    uchar buf[URCP_BUF_SIZE];
    int32 *cmd_data_ptr;
    uint16 uid=0u;
    uint32 ulen;
    cmd_hdr_t* cmd;
    int32 ret = 0,i;
    urcp_hdr_t* header;

    urcp_send_channel_msg_t urcp_send_channel_msg;

    memset(buf, 0, sizeof(buf));

    /*������Ϣ*/
    memcpy(&urcp_send_channel_msg, web_msg->sw_msg.buffer, sizeof(urcp_send_channel_msg_t));
    /*ͷ����Ϣ*/
    urcp_buid_header_with_8021q(buf, urcp_send_channel_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
	/*ָ���ͷ ��tag*/
	header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

	ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*�������� */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_PASSWD);/*��Ҫ����*/

    ulen += add_cmd(cmd, CMD_AP_CHANNEL);
    cmd_data_ptr = (int32 *)(&buf[ulen]);
    *cmd_data_ptr = htonl(urcp_send_channel_msg.channel);
    ulen += sizeof(urcp_send_channel_msg.channel);
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(urcp_send_channel_msg.channel);
    cmd->length = htons(cmd->length);


    /*���η��Ͷ���� */
    for(i=0u;i<urcp_send_channel_msg.batch_info.number;i++)
    {
        /*����Ựid*/
        if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
        {
            /*�޸�Ŀ��mac*/
            memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_send_channel_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
            /*��������id*/
            header->request_id = htons(uid);
            strncpy(header->password, urcp_send_channel_msg.batch_info.password[i], MAX_PASSWORD_LEN);

            /*��misc buffer�д�����¼*/
            urcp_create_ap_basic_record(&(urcp_send_channel_msg.batch_info.dst_mac[i*MAC_LEN]), urcp_send_channel_msg.batch_info.user_ip, uid, MISC_SET_GENERAL);
			/*����*/
			urcp_send_by_vlan(buf,ulen);
        }
        else
        {
            ret=(int32)(-1);
            break;
        }
    }

    return ret;

}
#endif

#if (AP_LOAD_BALANCE == FYES )
int32 urcp_msg_load_balacne(struct msgbuf_t *web_msg)
{
    DBCK();
    uchar buf[URCP_BUF_SIZE];
    int32 *cmd_data_ptr;
    uint16 uid=0u;
    uint32 ulen;
    cmd_hdr_t* cmd;
    int32 ret = 0,i;
    urcp_hdr_t* header;

    urcp_send_load_balance_msg_t urcp_send_load_balance_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(buf, 0, sizeof(buf));

	init_ap_basic_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/

    /*������Ϣ*/
    memcpy(&urcp_send_load_balance_msg, web_msg->sw_msg.buffer, sizeof(urcp_send_load_balance_msg_t));
    /*ͷ����Ϣ*/
	urcp_buid_header_with_8021q(buf, urcp_send_load_balance_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*ָ���ͷ ��tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*�������� */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*��Ҫ����*/

    ulen += add_cmd(cmd, CMD_AP_LB_MAX_CLIENT);
    for(i=0;i<4;i++){
    cmd_data_ptr = (int32 *)(&buf[ulen]);
    *cmd_data_ptr = htonl(urcp_send_load_balance_msg.client);
    ulen += sizeof(urcp_send_load_balance_msg.client);
    }
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(urcp_send_load_balance_msg.client)*4;
    cmd->length = htons(cmd->length);

    DBCK("client: %d\n",urcp_send_load_balance_msg.client);
    DBCK("number: %d\n",urcp_send_load_balance_msg.batch_info.number);

    /*���η��Ͷ���� */
    for(i=0u;i<urcp_send_load_balance_msg.batch_info.number;i++)
    {
        /*����Ựid*/
        if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
        {
            DBCK("mac[%d]: %s\n",i,str_macaddr(&(urcp_send_load_balance_msg.batch_info.dst_mac[MAC_LEN*i])));
            /*�޸�Ŀ��mac*/
            memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_send_load_balance_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
            /*��������id*/
            header->request_id = htons(uid);

	        urcp_create_config_record(&(urcp_send_load_balance_msg.batch_info.dst_mac[i*MAC_LEN]), urcp_send_load_balance_msg.batch_info.user_ip, uid, MISC_SET_GENERAL);
			/*����*/
			urcp_send_by_vlan(buf,ulen);
        }
        else
        {
            ret=(int32)(-1);
            break;
        }
    }

    return ret;

}

int32 urcp_set_ap_lb_source_sum(ApLoadBalanceProfile *prof, uint32 client)
{
    uchar buf[URCP_BUF_SIZE];
    int32 *cmd_data_ptr;
    uint16 uid=0u;
    uint32 ulen;
    cmd_hdr_t* cmd;
    urcp_hdr_t* header;
    int32 ret=0;
    int i;

    DBCK("set AP source_sum %d\n",client);
    memset(buf, 0, sizeof(buf));
    /*ͷ����Ϣ*/
    urcp_buid_header_with_8021q(buf,prof->ap[0].macAddr.addr , (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*ָ���ͷ ��tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*�������� */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*��Ҫ����*/

    ulen += add_cmd(cmd, CMD_AP_LB_MAX_CLIENT);
    for(i=0;i<4;i++){
        cmd_data_ptr = (uint32 *)(&buf[ulen]);
        *cmd_data_ptr = htonl(client);
        ulen += sizeof(client);
    }
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(client)*4;
    cmd->length = htons(cmd->length);

    /*���η��Ͷ���� */
    for(i=0u;i<prof->num;i++)
    {
        if(prof->ap[i].isAlive ){

        /*����Ựid*/
        if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, NULL)==0)
        {
            DBCK("mac[%d]: %s\n",i,str_macaddr(&(prof->ap[i].macAddr.addr)));
            /*�޸�Ŀ��mac*/
            memcpy(((eth_hdr_t*)buf)->dst_addr, &(prof->ap[i].macAddr.addr), MAC_LEN);/*DMAC*/

            /*��������id*/
            header->request_id = htons(uid);

			/*����*/
			urcp_send_by_vlan(buf,ulen);
        }
        else
        {
            ret = (int32)-1;
            break;
        }
        }
    }
    return ret;
}
int32 urcp_set_ap_load_balacne(struct msgbuf_t *web_msg)
{
    uint32 lbGroudNum;
	DBCK();

    /*������Ϣ*/
    memcpy(&lbGroudNum, web_msg->sw_msg.buffer, sizeof(uint32));
    /*ͷ����Ϣ*/
    DBCK("lbGroudNum: %d\n",lbGroudNum);
    ap_load_balance_process(lbGroudNum);    /*�����ؾ���*/
    return 0;
}
#endif

#if AP_WL_POWER_AUTO
#ifdef URCP_CMD_AP_WL_POWER_SET_REPLY
int32 urcp_set_ap_wl_power(uchar *pMac, uint32 power)
{
    uchar buf[URCP_BUF_SIZE];
    uint32 *cmd_data_ptr;
    uint16 uid=0u;
    uint32 ulen;
    cmd_hdr_t* cmd;
    urcp_hdr_t* header;
	int iftag = 0;

    memset(buf, 0, sizeof(buf));

    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, NULL)==-1){
        return -1;
    }

    /*ͷ����Ϣ*/
    iftag = urcp_buid_header(buf,pMac , (uint16)URCP_SET, uid, (uchar*)NULL);
	if(iftag == 1)
	{
	    /*ָ���ͷ ��tag*/
	    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*�������� */
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*ָ���ͷ*/
	    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET]);

	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*�������� */
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    header ->flag = htons(FLAG_NO_PASSWD);/*��Ҫ����*/

    ulen += add_cmd(cmd, CMD_AP_WL_POWER );
    cmd_data_ptr = (uint32 *)(&buf[ulen]);
    *cmd_data_ptr = htonl(power);
    ulen += sizeof(power);
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(power);
    cmd->length = htons(cmd->length);

    h2_send_frame(buf, ulen);
    return 0;

}
#endif
#ifdef URCP_CMD_AP_SITE_SURVEY_SET_REPLY
extern urcp_shm_t* shm_ptr;
uint32 urcp_set_ap_site_survey()
{
    uchar buf[URCP_BUF_SIZE];
    uint32 *cmd_data_ptr,wait_sec = 0u,i;
    uint16 uid=0u;
    uint32 ulen;
    int32 tb_index;
    cmd_hdr_t* cmd;
    urcp_hdr_t* header;
    urcp_dev_t *tmp=NULL;

    memset(buf, 0, sizeof(buf));
    DBCK();

    /*ͷ����Ϣ*/
    urcp_buid_header_with_8021q(buf,mac_addr_cooperate, (uint16)URCP_SET, uid, (uchar*)NULL);
    /*ָ���ͷ ��tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*�������� */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*��Ҫ����*/

    ulen += add_cmd(cmd, CMD_AP_SITE_SURVEY);
    cmd_data_ptr = (uint32 *)(&buf[ulen]);
    ulen += sizeof(wait_sec);
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(wait_sec);
    cmd->length = htons(cmd->length);

    for(i = 0u; i < DEV_HASH_SIZE; i++)
    {
        tb_index = shm_ptr->dev_tb_hash[i];
        while(tb_index!=NONE_NODE) {
            tmp = &shm_ptr->dev_table[tb_index-1];
            if((tmp->used==1) && (tmp->aged != 2)) {
                if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, NULL)==0){
                    /*�޸�Ŀ��mac*/
                    memcpy(((eth_hdr_t*)buf)->dst_addr, &(tmp->mac), MAC_LEN);/*DMAC*/
                    /*��������id*/
                    header->request_id = htons(uid);
                    *cmd_data_ptr = htonl(wait_sec);

					/*����*/
					urcp_send_by_vlan(buf,ulen);
                    wait_sec += 3u;
                }
            }
            tb_index = tmp->tb_next;
        }
    }

    DBCK("wait_sec: %d\n",wait_sec);
    return wait_sec;

}
#endif
#endif



/*
 *���յ����ļ���������
 *KeHuatao
 */
static void rename_file(const char *path, uchar* name, char **file_name)
{
    char oldname[128], newname[128];
    uint32 tmp;
    time_t timep;
    struct tm *p;
    int32 a, b;

    memset(newname, 0, sizeof(newname));
    strcpy(newname, path);
    /*�����ļ���*/
    strncat(newname, name, MAX_FILE_NAME_LEN);
    if(sscanf(newname, "%*[^:]:%d", &timep)==1)
    {
	/*�������ļ���*/
	p=localtime(&timep);
	strtok(newname, ":");
	tmp = strlen(newname);
	/*��ʱ��д���ļ���*/
	a = 1900 + (p->tm_year);
	b = 1+(p->tm_mon);
	if (!strcmp(path, TFTP_CONFIG_ROOT)) {
		sprintf(&newname[tmp], "_%d%02d%02d_%02d%02d%02d.xml", a, b, (p->tm_mday),
			(p->tm_hour), (p->tm_min), (p->tm_sec));
	} else if (!strcmp(path, TFTP_IMAGE_ROOT)) {
		sprintf(&newname[tmp], "_%d%02d%02d_%02d%02d%02d.bin", a, b, (p->tm_mday),
			(p->tm_hour), (p->tm_min), (p->tm_sec));
	} else if (!strcmp(path, TFTP_LOG_ROOT)) {
		sprintf(&newname[tmp], "_%d%02d%02d_%02d%02d%02d.log", a, b, (p->tm_mday),
			(p->tm_hour), (p->tm_min), (p->tm_sec));
	}

	/*���ļ���*/
	memset(oldname, 0, sizeof(oldname));
	strcpy(oldname, TFTP_ROOT);
	/*�����ļ���*/
	strncat(oldname, name, MAX_FILE_NAME_LEN);
	/**/
	if(rename(oldname, newname)!=0)
	{
	    URCP_ERROR(("%s: rename error\n", __func__)); 
	}
	*file_name = newname;
    }
    URCP_DEBUG(("%s: %s to  %s\n", __func__, oldname, newname)); 
	DBCK("oldname = %s, newname = %s\n", oldname, newname);

    return;
}

/*
 *ɾ���Ự��������ʱ�ļ�
 *
 *KeHuatao
 */
static void del_urcp_tmp_file(char *file)
{
    char tp[128];

    URCP_DEBUG(("%s: %s\n", __func__, file));
    memset(tp, 0, sizeof(tp));
    /*��ȫ·��*/
    strcat(tp, TFTP_ROOT);
    strncat(tp, file, MAX_FILE_NAME_LEN);
    /*ɾ��*/
    unlink(tp);

    return;
}
/*
 *���ͻ��ȡ�ļ��Ļص�����
 *
 *keHuatao
 */
void urcp_file_cb(void *ptr, urcp_status_t urcp_state, const char*path)
{
	char tmp_name[256];
	char *src_name = NULL;
    urcp_tftp_session_info_t *sess;
#if (UTT_WX_2000 == FYES)
      char cmd[256];
#endif

	memset(tmp_name, '\0', sizeof(tmp_name));
    URCP_DEBUG(("%s:%d\n", __func__, urcp_state));
    sess = (urcp_tftp_session_info_t*)ptr;
    /*�ж�״̬*/
    switch(urcp_state)
    {
	case URCP_STATUS_OK:/*�ɹ�*/
	    if(sess->sess_type==TFTP_GET_FILE)
	    {
			DBCK();
			rename_file(path, sess->filename, &src_name);
			DBCK("src_name = %s", src_name);

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
			if ((1 == isUSBStorageExist(tmp_name)) && (src_name != NULL)) {
				DBCK("src_name + strlen(path) = %s, path = %s, path_in_U = %s\n", src_name + strlen(path), path, path + strlen("/tmp/urcp"));
				saveFile2USB(src_name + strlen(path), path, path + strlen("/tmp/urcp/"));
				sync();
			}
#elif (UTT_WX_2000 == FYES)
			memset(cmd, '\0', sizeof(cmd));
			sprintf(cmd, "cp -af %s %s%s", src_name, "/ff/", path + strlen("/tmp/urcp"));
			system(cmd);
			memset(cmd, '\0', sizeof(cmd));
			sprintf(cmd, "rm -rf %s", src_name);
			system(cmd);
			system(cmd);
			sync();
#endif
	    }
	case URCP_STATUS_FAIL:/*����ʧ��*/
	case URCP_STATUS_AUTH_ERROR:/*�������*/
	case URCP_STATUS_TIME_OUT:/*��ʱ*/
	    urcp_config_set_status(sess->local_id, urcp_state);
	    /*����ҳ����ʾ��Ϣ*/
	    break;
	default:
	    /*������������,�ɽ������ж�Ϊ��ʱ*/
	    break;
    }
    /*ɾ����ʱ�ļ�*/
    if((sess->sess_type==TFTP_PUT_FILE) && (strcmp(sess->filename, "tmp_")))
    {
        del_urcp_tmp_file(sess->filename);
    }

    return;
}

void urcp_copy_put_file(void *ptr, urcp_status_t urcp_state, uint32 serial_no)
{
    char tp[256], tmp_path[128];

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_AP_CONFIG_ROOT);
	} else {
		strcat(tmp_path, TFTP_AP_CONFIG_ROOT);
	}
#else
	strcat(tmp_path, TFTP_AP_CONFIG_ROOT);
#endif

    urcp_tftp_session_info_t *sess;

    URCP_DEBUG(("function:%s\t :%d\n", __func__, urcp_state));
    
    sess = (urcp_tftp_session_info_t*)ptr;

    /*�ж�״̬*/
    if((urcp_state == URCP_STATUS_OK) && (sess->sess_type==TFTP_PUT_FILE))
    {
	memset(tp, 0, sizeof(tp));
	sprintf(tp, "cp %s%s %s%d.xml -f",TFTP_ROOT, sess->filename, tmp_path, serial_no);
	DBCK("%s\n", tp);

	system(tp);
    }

    return;
}

/*
 *���������־�ļ������Ļص�����
 *KeHuatao
 *
 */
void urcp_get_log_cb(void *ptr, urcp_status_t urcp_state)
{
    urcp_file_cb(ptr, urcp_state, TFTP_LOG_ROOT);

    return;
}

void urcp_get_image_cb(void *ptr, urcp_status_t urcp_state)
{
	urcp_file_cb(ptr, urcp_state, TFTP_IMAGE_ROOT);

	return;
}

#if (URCP_DEVICE != URCP_ROUTER)
typedef struct _file_param_t{
    char* dst_file;
}file_param_t;


/*
 *�����ļ�
 *KeHuatao
 */
static void  scopy_file(char *srcFile, void*p)
{
    file_param_t *pa;

    pa = (file_param_t*)p;

    copy_file(srcFile, pa->dst_file, 0, 1);

    return;
}
/*
 *����ǰ��־д���ļ�
 *KeHuatao
 */
int32 create_log_file(char *file)
{
    char tp[64], ip[32], tmp[256];
    uchar mac[MAC_LEN];
    int32 ret=0, nip;
    FILE*p=NULL;
    file_param_t param;


    /*�ļ���*/
    memset(tp, 0, sizeof(tp));
    strcat(tp, TFTP_ROOT);
    create_tmp_file("log", file);
    strcat(tp, file);
    p=fopen(tp, "w");
    if(p!=NULL)
    {
	memset(ip, 0, sizeof(ip));
	fprintf(p, "#");
	/*��ӡip*/
	get_ip(SWITCH_IF_NAME, &nip);
	if(nip!=0)
	{
	    int2ip(ntohl(nip), ip);
	    fprintf(p, "%s ", ip);
	}
	/*��ӡmac*/
	memset(mac, 0, sizeof(mac));
	get_mac(SWITCH_IF_NAME, mac);
	fprintf(p, "%s\n", str_macaddr(mac));
	fclose(p);
    }
    
    /*�����ļ�*/
    memset(&param, 0, sizeof(file_param_t));
    param.dst_file=tp;
    ret = searchDir(RAM_LOG_PATH, (dir_pf)scopy_file, (void*)(&param));
    memset(tmp, 0, sizeof(tmp));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if(isUSBStorageExist(tmp)==1)
    {
	strcat(tmp, USB_LOG_PATH);
	ret = ret + searchDir(tmp, (dir_pf)scopy_file, (void*)(&param));
    }
#endif
    if(ret==0)
    {
	strcpy(file, "");
    }

    return ret;
}
#endif

#ifdef URCP_CMD_WL_MAC_FILTER_SET_REPLY
int32 urcp_msg_set_wl_mac_filter(struct msgbuf_t *msgbuf)
{
    uchar buff[URCP_BUF_SIZE], *cmd_data_ptr;
    uint16 uid=0u;
    uint32 ulen, num, enable;
    cmd_hdr_t* cmd;
    misc_opt_assist misc_buff;
    urcp_wl_mac_filter_msg_t msg_buf;
    urcp_general_msg_t* urcp_general_msg;
	int iftag = 0;

    URCP_DEBUG(("%s %d\n", __func__,__LINE__));
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_wl_mac_filter_cb)==0)
    {/*����ɹ�*/
	/*����Ϣ*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));

	init_mac_filter_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��*/
	memcpy(&msg_buf, msgbuf->sw_msg.buffer, sizeof(urcp_wl_mac_filter_msg_t));
	urcp_general_msg = &(msg_buf.general_msg);/*ָ��ͨ�ýṹ����Ϣ*/

	iftag = urcp_buid_header(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, NULL);
	if(iftag == 1)
	{
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*�������� */
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*�������� */
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*mac filter*/
	/*������������*/
	ulen += add_cmd(cmd, CMD_WL_MAC_FILTER);
	cmd->length = ntohs(cmd->length);
	/*������������*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
    num = msg_buf.wl_mac_filter_msg.mac_num;
    msg_buf.wl_mac_filter_msg.mac_num = htonl(msg_buf.wl_mac_filter_msg.mac_num);
	memcpy(cmd_data_ptr, &(msg_buf.wl_mac_filter_msg.mac_num), sizeof(msg_buf.wl_mac_filter_msg.mac_num));
	cmd->length += sizeof(msg_buf.wl_mac_filter_msg.mac_num);
	ulen += sizeof(msg_buf.wl_mac_filter_msg.mac_num);

	cmd_data_ptr = (uchar *)(&buff[ulen]);
	enable = msg_buf.wl_mac_filter_msg.enable;
    msg_buf.wl_mac_filter_msg.enable = htonl(msg_buf.wl_mac_filter_msg.enable);
	memcpy(cmd_data_ptr, &(msg_buf.wl_mac_filter_msg.enable), sizeof(msg_buf.wl_mac_filter_msg.enable));
	cmd->length += sizeof(msg_buf.wl_mac_filter_msg.enable);
	ulen += sizeof(msg_buf.wl_mac_filter_msg.enable);

	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, msg_buf.wl_mac_filter_msg.macaddr, num*MAC_LEN);
	cmd->length += num * MAC_LEN;
	cmd->length = htons(cmd->length);
	ulen += num * MAC_LEN;

	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,uid,MISC_SET_GENERAL,&misc_buff,NULL);
	h2_send_frame(buff, ulen);	/* ���� */ 
    }
    return 0;
}
#endif

#if (URCP_DISCONNECT_STA==FYES) 
#define MAX_DISCONNECT_AP 20u /*һ���������AP����*/
#define MAX_DISCONNECT_STA 30u /*һ���������AP����*/
typedef struct _discon_ap_info_t{
    uint16 pos;
    uchar apMac[MAC_LEN];
    uchar staMac[MAX_DISCONNECT_STA];
}discon_ap_info_t;
void urcp_msg_disconnect_sta(struct msgbuf_t *msgbuf)
{
    uint16 staNum, apNum=0u, i, j, n, *cmd_data_ptr=NULL;
    int32 iftag = 0, ulen;
    uchar buff[URCP_BUF_SIZE], *data = NULL;
    urcp_sta_msg_t  *msg;
    cmd_hdr_t* cmd;
    urcp_hdr_t* header;
    sta_entry_t *entry;
    discon_ap_info_t ap_discon_info[MAX_DISCONNECT_AP];

    /*����Ϣ*/
    memset(ap_discon_info, 0, sizeof(ap_discon_info));
    msg = (urcp_sta_msg_t*)(msgbuf->sw_msg.buffer);
    /*�ҳ�����STA������AP,���ֿ��洢*/
    if (msg->staNum > MAX_DISCONNECT_STA) {
	staNum = MAX_DISCONNECT_STA;
	printf("%s: Too much MAC\n", __func__);
    }
    else {
	staNum = msg->staNum;
    }
    db_info(URCP_MODULE_ID_USER_LIST, "%s: sta: %d\n", __func__, staNum);
    for(i=0u; i<staNum; i++)
    {
       db_info(URCP_MODULE_ID_USER_LIST, "%s: i %d, %s\n", __func__, i, str_macaddr(msg->staMac[i]));
       entry = (sta_entry_t*)ac_sta_table_lookup(msg->staMac[i]);
       if(entry != NULL) 
       {
	   for(j=0u; j<apNum; j++)
	   {
	       if(mac_cmp(ap_discon_info[j].apMac, entry->ap_mac)==0) 
	       {
		   /*��¼STA MAC��λ��*/
		   ap_discon_info[j].staMac[ap_discon_info[j].pos] = i;
		   ap_discon_info[j].pos = ap_discon_info[j].pos + 1u;
		   break;
	       }
	   }
           db_info(URCP_MODULE_ID_USER_LIST, "%s: line: %d\n", __func__, __LINE__);
	   if(j>=apNum) 
	   {
	       if(apNum<MAX_DISCONNECT_AP) 
	       {
		   apNum++;
		   memcpy(ap_discon_info[j].apMac, entry->ap_mac, MAC_LEN);
		   ap_discon_info[j].staMac[ap_discon_info[j].pos] = i;
		   ap_discon_info[j].pos = ap_discon_info[j].pos + 1u;
	       }
	       else {
		   break;
	       }
	   }
       }
    }

    db_info(URCP_MODULE_ID_USER_LIST, "%s: apNum=%d, staNum=%d\n", __func__, apNum, staNum);
    /*�����AP����*/
    for(i=0; i<apNum; i++)
    {
	memset(buff, 0, sizeof(buff));
	db_info(URCP_MODULE_ID_USER_LIST, "%s: send to ap: %s\n", __func__, str_macaddr(ap_discon_info[i].apMac));
	iftag = urcp_buid_header(buff, ap_discon_info[i].apMac, (uint16)URCP_SET, 0u, NULL);
	if(iftag == 1)
	{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*�������� */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*�������� */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	header->flag = htons(FLAG_NO_PASSWD);/*����Ҫ����*/
	ulen += add_cmd(cmd, CMD_SET_DISASSOC);
	cmd_data_ptr = (uint16*)(&buff[ulen]);
        *cmd_data_ptr = htons(ap_discon_info[i].pos);
	cmd->length = sizeof(cmd_hdr_t) + sizeof(ap_discon_info[0].pos);
	ulen += sizeof(ap_discon_info[0].pos);
	data = (uchar*)((uchar*)cmd_data_ptr + sizeof(ap_discon_info[0].pos));
        for(j=0; j<ap_discon_info[i].pos; j++)
	{
	    n = ap_discon_info[i].staMac[j];
	    memcpy(data, msg->staMac[n], MAC_LEN);
	    data += MAC_LEN;
	    ulen += MAC_LEN;
	    cmd->length += MAC_LEN;
	    db_info(URCP_MODULE_ID_USER_LIST, "sta: %d %s ", n, str_macaddr(data-6));
	}
	db_info(URCP_MODULE_ID_USER_LIST, "%s: len %d ", __func__, cmd->length);
	cmd->length = htons(cmd->length);
	h2_send_frame(buff, ulen);	/* ���� */ 
    }
}
#endif

#if (AP_BLACK_WHITE_LIST == FYES)
void urcp_msg_black_list_sz(struct msgbuf_t *msgbuf)
{
    uint16 staNum, apNum=0u, i, j, n, *cmd_data_ptr=NULL;
    int32 iftag = 0, ulen = 0, msg_len = 0;
    uchar buff[URCP_BUF_SIZE], *data = NULL;
    cmd_hdr_t* cmd;
    urcp_hdr_t* header;
    urcp_black_list_msg_t *msg;
    urcp_black_list_t *list;
    unsigned char mac[MAC_LEN] = "";
    /*����Ϣ*/
    memcpy(mac, (unsigned char *)(msgbuf->sw_msg.buffer), MAC_LEN);
    msg = (urcp_black_list_msg_t*)((char *)(msgbuf->sw_msg.buffer) + MAC_LEN);
    staNum = msg->staNum; 
#if 0
    printf("%s,num:%d, mode:%d\n", __func__, msg->staNum, msg->mode);
    for(i=0u; i<staNum; i++)
    {
	list = &(msg->black_list[i]);
//	db_info(URCP_MODULE_ID_USER_LIST, "%s: i %d, %s\n", __func__, i, str_macaddr(&(list->staMac[i])));
	printf("ok,%s, i:%d, ssid:%s, mac:%s\n", __func__, i, list->ssid, str_macaddr(list->staMac));
    }
#endif
#if 1
    if (staNum > 0)
    {
	memset(buff, 0, sizeof(buff));
	iftag = urcp_buid_header(buff, mac, (uint16)URCP_SET, 0u, NULL);
	if(iftag == 1)
	{	
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*�������� */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}
	else
	{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*�������� */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	header->flag = htons(FLAG_NO_PASSWD);/*����Ҫ����*/
	cmd->type = htonl(CMD_AP_ADD_MAC_BLACK);
	cmd->length = sizeof(cmd_hdr_t);
	ulen += sizeof(cmd_hdr_t);
	data = &(buff[ulen]);
	msg_len = sizeof(msg->staNum) + sizeof(msg->mode) + sizeof(urcp_black_list_t)*staNum;
	msg->staNum = htons(msg->staNum);
	msg->mode = htons(msg->mode);
	memcpy(data, msg, msg_len);	    //�����������б�����
	cmd->length += msg_len;
	cmd->length = htons(cmd->length);
	cmd->cmd_status = htons(URCP_OK);
	ulen += msg_len;
	
//	printf("%s, msg_len = %d\n", __func__, msg_len);
	h2_send_frame(buff, ulen);	/* ���� */
    }
#endif
}

void urcp_msg_black_list(struct msgbuf_t *msgbuf)
{
    uint16 staNum, apNum=0u, i, j, n, *cmd_data_ptr=NULL;
    int32 iftag = 0, ulen = 0, msg_len = 0;
    uchar buff[URCP_BUF_SIZE], *data = NULL;
    cmd_hdr_t* cmd;
    urcp_hdr_t* header;
    urcp_black_list_msg_t *msg;
    urcp_black_list_t *list;

    /*����Ϣ*/
    msg = (urcp_black_list_msg_t*)(msgbuf->sw_msg.buffer);
    staNum = msg->staNum;
    db_info(URCP_MODULE_ID_USER_LIST, "%s: sta: %d\n", __func__, staNum);
#if 0
    printf("%s,num:%d, mode:%d\n", __func__, msg->staNum, msg->mode);
    for(i=0u; i<staNum; i++)
    {
	list = &(msg->black_list[i]);
	printf("ok,%s, i:%d, ssid:%s, mac:%s\n", __func__, i, list->ssid, str_macaddr(list->staMac));
    }
#endif

    if (staNum >= 0)
    {
	memset(buff, 0, sizeof(buff));
#if 0
	iftag = urcp_buid_header(buff, mac_addr_cooperate, (uint16)URCP_SET, 0u, NULL);
	if(iftag == 1)
	{	
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*�������� */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}
	else
	{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*�������� */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
#else
	urcp_buid_header_with_8021q(buff, mac_addr_cooperate, (uint16)URCP_SET, 0u, NULL);
	/*ָ���ͷ ��tag*/
	header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);
	ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
#endif
	header->flag = htons(FLAG_NO_PASSWD);/*����Ҫ����*/
	cmd->type = htonl(CMD_AP_ADD_MAC_BLACK);
	cmd->length = sizeof(cmd_hdr_t);
	ulen += sizeof(cmd_hdr_t);
	data = &(buff[ulen]);
	msg_len = sizeof(msg->staNum) + sizeof(msg->mode) + sizeof(urcp_black_list_t)*staNum;
	msg->staNum = htons(msg->staNum);
	msg->mode = htons(msg->mode);
	memcpy(data, msg, msg_len);	    //�����������б�����
	cmd->length += msg_len;
	cmd->length = htons(cmd->length);
	cmd->cmd_status = htons(URCP_OK);
	ulen += msg_len;
	
//	printf("%s, msg_len = %d\n", __func__, msg_len);
//	h2_send_frame(buff, ulen);	/* ���� */
	urcp_send_by_vlan(buff,ulen);
    }
}
#endif
