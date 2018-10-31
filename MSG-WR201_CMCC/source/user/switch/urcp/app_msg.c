/*
 *
 *
 *联动协议v3版处理
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
 *处理联动一般读取消息
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id1, func_cb)==0)
    {
	URCP_DEBUG(("%s\n", __func__));
	/*读消息*/
	iftag = urcp_buid_header(buf, urcp_general_msg->dst_mac, (uint16)URCP_GET, id1, urcp_general_msg->password);

	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*设置命令类型*/
	ulen += add_cmd(cmd, cmd_type);
	/*在misc buffer中创建记录*/

	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,id1,MISC_GET_GENERAL,misc_buff, (uchar*)NULL);

	/*发送*/
	h2_send_frame(buf, ulen);
	ret = 0;
    }

    return ret;
}
/*
 *处理一般联动设置的消息
 *调用此方法后，本地misc buffer中存储的conf信息为转换过字节序的信息（网络字节序）
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
    uint16 conf_size = misc_buff->unit_size - sizeof(misc_header_t);/*数据大小*/
	int iftag = 0;

    memset(buf, 0, sizeof(buf));
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id1, func_cb)==0)
    {
	URCP_DEBUG(("%s\n", __func__));
	/*读消息*/
	iftag = urcp_buid_header(buf, urcp_general_msg->dst_mac, (uint16)URCP_SET, id1, urcp_general_msg->password);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*设置命令类型*/
	ulen += add_cmd(cmd, cmd_type);
	/*设置命令内容*/
//	cmd_data_ptr = (uchar*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)]);
	cmd_data_ptr = (uchar*)(&buf[ulen]);
	memcpy(cmd_data_ptr, urcp_config,conf_size);
	/*在misc buffer中创建记录*/
	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,id1,MISC_SET_GENERAL,misc_buff, urcp_config);/*这里的urcp_config为网络字节序*/

/*调整长度*/
	ulen += conf_size;
	cmd -> length = ntohs(cmd->length);
	cmd -> length += conf_size;
	cmd -> length = htons(cmd->length);
	/*发送*/
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
 *单命令无数据操作中
 *根据misc buffer操作类型获取urcp命令类型
 *增加新类型操作只需更新此函数 
 *bhou
 */
static uint32 get_urcp_cmd_alone_type(uint32 u_misc_type)
{
    uint32 u_cmd_type = 0u;
    switch(u_misc_type)
    {
	case MISC_REBOOT:/*重启*/
	    u_cmd_type = CMD_REBOOT_CONFIG;
	    break;
	case MISC_RESET:/*恢复出厂配置*/
	    u_cmd_type = CMD_RESET_CONFIG;
	    break;
	case MISC_SAFEBIND:/*安全绑定*/
	    u_cmd_type = CMD_SAFEBIND_CONFIG;
	    break;
	default:/*默认不存在*/
	    break;

    }
    return u_cmd_type;
}
/*
 *处理联动单个命令消息
 *bhou
 */
int32 urcp_msg_cmd_alone_config(struct msgbuf_t *web_msg)
{
    uchar  buf[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen=0u, i=0u;
    int32 ret=0;
    urcp_cmd_alone_msg_t  urcp_cmd_alone_msg;
    uint32 u_misc_type=MISC_GET_GENERAL;/*默认为读*/
    misc_opt_assist misc_buff;
    uint32 urcp_cmd_type = 0u;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd; 

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(buf, 0, sizeof(buf));

    init_cmd_alone_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    /*复制消息*/
    memcpy(&urcp_cmd_alone_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_cmd_alone_msg_t));
    /*头部信息*/
	urcp_buid_header_with_8021q(buf, urcp_cmd_alone_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */

    header ->flag = htons(FLAG_PASSWD);/*需要密码*/

	cmd = (cmd_hdr_t*)(&buf[ulen]);

    /*获取命令类型*/
    u_misc_type = urcp_cmd_alone_msg.opt_type;/*misc buffer操作类型*/
    urcp_cmd_type = get_urcp_cmd_alone_type(u_misc_type);/*获取urcp命令类型*/
    ulen += add_cmd(cmd, urcp_cmd_type);

    /*依次发送多个包 */
    for(i=0u;i<urcp_cmd_alone_msg.batch_info.number;i++)
    {
	/*分配会话id*/
	if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_cmd_alone_cb)==0)
	{
	    /*修改目的mac*/
	    memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_cmd_alone_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
	    /*重新设置id*/
	    header->request_id = htons(uid);
	    strncpy(header->password, urcp_cmd_alone_msg.batch_info.password[i], MAX_PASSWORD_LEN);
	    URCP_DEBUG(("%s: i:%d, passwd:%s\n", __func__, i, header->password));
	    /*在misc buffer中创建记录*/
	    urcp_create_general_record(((eth_hdr_t*)buf)->dst_addr, urcp_cmd_alone_msg.batch_info.user_ip,uid,u_misc_type,&misc_buff,(uchar*)NULL);
		/*发包*/
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
 *处理联动端口设置消息
 *bhou
 */
int32 urcp_msg_get_port_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_port_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_PORT_CONFIG, &misc_buff,(cb_fp)urcp_port_conf_cb);/*消息处理*/
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET_REPLY
/*
 *处理联动端口设置消息
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

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_port_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/

    cfg_read_urcp_swap_conf((char*)(&_port_config), sizeof(urcp_port_conf_conf_t));/*读取共享内存设置信息*/

    _port_config.port_num = htonl(_port_config.port_num);/*端口个数*/
    for(i=0u;i<urcp_general_msg.port_ct;i++)/*转化字节序*/
    {
	_port_config.port_conf_group[i].selected_mode = htonl(_port_config.port_conf_group[i].selected_mode);/*设置模式*/
	_port_config.port_conf_group[i].max_frame = htonl(_port_config.port_conf_group[i].max_frame);/*允许最大帧*/
    }
    return  urcp_msg_set_general(&urcp_general_msg,CMD_PORT_CONFIG,&misc_buff,(uchar*)(&_port_config), (cb_fp)urcp_port_conf_cb) ;
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
/*
 *处理联动速率限制
 *bhou
 */
int32 urcp_msg_get_qos_rate_limit_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_QOS_RATE_LIMIT_CONFIG, &misc_buff,(cb_fp)urcp_qos_rate_limit_cb);/*消息处理*/
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET_REPLY
/*
 *处理联动速率限制设置
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

    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    memcpy(&urcp_qos_rate_limit_msg, web_msg->sw_msg.buffer, sizeof(urcp_qos_rate_limit_msg_t));/*读取消息*/

    urcp_general_msg = &(urcp_qos_rate_limit_msg.general_msg);/*指向通用结构体信息*/
    conf_buff = &(urcp_qos_rate_limit_msg.qos_rate_limit_conf);/*指向设置信息结构体*/

    conf_buff->rate_unit = htonl(conf_buff->rate_unit);/*颗粒度*/
    for(i=0u;i<conf_buff->port_num;i++)/*转化字节序*/
    {
	conf_buff->qos_rate_limit_group[i].policer = htonl(conf_buff->qos_rate_limit_group[i].policer);/*ingress*/
	conf_buff->qos_rate_limit_group[i].shaper = htonl(conf_buff->qos_rate_limit_group[i].shaper);/*egress*/
    }
    conf_buff->port_num = htonl(conf_buff->port_num);/*端口个数*/

    return  urcp_msg_set_general(urcp_general_msg,CMD_QOS_RATE_LIMIT_CONFIG,&misc_buff,(uchar*)(conf_buff), (cb_fp)urcp_qos_rate_limit_cb) ;
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
/*
 *处理联动风暴抑制
 *bhou
 */
int32 urcp_msg_get_storm_rate_limit_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_STORM_RATE_LIMIT_CONFIG, &misc_buff,(cb_fp)urcp_storm_rate_limit_cb);/*消息处理*/
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET_REPLY
/*
 *处理联动风暴抑制
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

    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    memcpy(&urcp_storm_rate_limit_msg, web_msg->sw_msg.buffer, sizeof(urcp_storm_rate_limit_msg_t));/*读取消息*/

    urcp_general_msg = &(urcp_storm_rate_limit_msg.general_msg);/*指向通用结构体信息*/
    conf_buff = &(urcp_storm_rate_limit_msg.storm_rate_limit_conf);/*指向设置信息结构体*/

    conf_buff->icmp_rate = htonl(conf_buff->icmp_rate);/*icmp*/
    conf_buff->bc_rate = htonl(conf_buff->bc_rate);/*广播*/
    conf_buff->mc_rate = htonl(conf_buff->mc_rate);/*组播*/
    conf_buff->uc_rate = htonl(conf_buff->uc_rate);/*单播*/

    return  urcp_msg_set_general(urcp_general_msg,CMD_STORM_RATE_LIMIT_CONFIG,&misc_buff,(uchar*)(conf_buff), (cb_fp)urcp_storm_rate_limit_cb) ;
}
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
/*
 *处理联动系统信息消息
 *bhou
 */
int32 urcp_msg_get_sys_info_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_sys_info_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_SYS_INFO_CONFIG, &misc_buff,(cb_fp)urcp_sys_info_cb);/*消息处理*/
}
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
/*
 *处理联动系统设置消息
 *bhou
 */
int32 urcp_msg_get_sys_conf_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_sys_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_SYS_CONF_CONFIG, &misc_buff,(cb_fp)urcp_sys_conf_cb);/*消息处理*/
}
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_SET_REPLY
/*
 *处理联动系统设置消息
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

    init_sys_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
    memcpy(&urcp_sys_conf_msg, web_msg->sw_msg.buffer, sizeof(urcp_sys_conf_msg_t));/*读取消息*/

    urcp_general_msg = &(urcp_sys_conf_msg.general_msg);/*指向通用结构体信息*/
    conf_buff = &(urcp_sys_conf_msg.sys_conf_conf);/*指向设置信息结构体*/

    /*转化字节序*/
    conf_buff->dhcp_relay_enable = htonl(conf_buff->dhcp_relay_enable);/*dhcp中继*/
    conf_buff->dhcp_client_enable = htonl(conf_buff->dhcp_client_enable);/*dhcp客户端*/
    conf_buff->ip_addr = htonl(conf_buff->ip_addr);/*系统ip*/
    conf_buff->ip_mask = htonl(conf_buff->ip_mask);/*ip掩码*/
    conf_buff->gw_ip = htonl(conf_buff->gw_ip);/*系统网关*/
    conf_buff->cpu_vid = htonl(conf_buff->cpu_vid);/*cpu vid*/
    conf_buff->age_time = htonl(conf_buff->age_time);/*老化时间*/
    conf_buff->arp_enable = htonl(conf_buff->arp_enable);/*是否开启arp*/

    return  urcp_msg_set_general(urcp_general_msg,CMD_SYS_CONF_CONFIG,&misc_buff,(uchar*)(conf_buff), (cb_fp)urcp_sys_conf_cb) ;
}
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
/*
 *处理联动ip过滤消息
 *bhou
 */
int32 urcp_msg_get_ip_filter_config(struct msgbuf_t *web_msg)
{
    urcp_general_msg_t urcp_general_msg;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
    init_ip_filter_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/

    /*消息处理*/
    return urcp_msg_get_general(&urcp_general_msg, CMD_IP_FILTER_CONFIG, &misc_buff,(cb_fp)urcp_ip_filter_cb);
}
#endif
#ifdef URCP_CMD_AGGR_CONFIG_SET_REPLY
/*
 *处理联动汇聚的消息
 *bhou
 */
int32 urcp_msg_aggr_config(struct msgbuf_t *web_msg)
{
    uchar  buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    uint32 misc_type=MISC_GET_GENERAL;/*默认为读*/
    session_type_t sess_type=SESSION_TYPE_GET;/*默认为读*/
    uint16 u_urcp_type=URCP_GET;/*默认为读*/
    urcp_aggr_msg_t  urcp_aggr_msg;
    urcp_aggr_conf_t* aggr_config=NULL;/*默认为null即读模式*/
    cmd_hdr_t* cmd;
    int32 ret = -1;
	int iftag = 0;
    memset(buff, 0, sizeof(buff));

    memcpy(&urcp_aggr_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_aggr_msg_t));
    misc_type = urcp_aggr_msg.opt_type;
    if(misc_type == MISC_SET_GENERAL)/*设置操作*/
    {
	sess_type = SESSION_TYPE_SET;
	u_urcp_type = URCP_SET;
    }
    /*分配会话id*/
    if(session_alloc(sess_type, 0u, URCP_CMD_TIME_OUT, &uid, urcp_aggr_cb)==0)/*分配成功*/
    {
	ret = 0;
	/*读消息*/
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

	/*读取汇聚*/
	ulen += add_cmd(cmd, CMD_AGGR_CONFIG);
	if(misc_type == MISC_SET_GENERAL)/*设置操作*/
	{
//	    aggr_config = (urcp_aggr_conf_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)]);/*跳过各个头长度，指向命令内容首地址*/
	    aggr_config = (urcp_aggr_conf_t*)(&buff[ulen]);/*跳过各个头长度，指向命令内容首地址*/
	    aggr_config->port_num = htonl(urcp_aggr_msg.port_ct);/*端口个数*/
	    memcpy(&(aggr_config->aggr_group[0]) , &(urcp_aggr_msg.aggr_group),sizeof(urcp_aggr_group_t));/*拷贝到第一个汇聚组中（目前一次设置一个，若想设置多个，可用其他汇聚组）*/
	    aggr_config->aggr_group[0].index=htons(aggr_config->aggr_group[0].index);/*组号*/
	    aggr_config->aggr_group[0].member=htonl(aggr_config->aggr_group[0].member);/*成员*/
	    ulen += sizeof(urcp_aggr_conf_t);/*调整长度*/
	    cmd -> length = ntohs(cmd->length);/*调整命令长度*/
	    cmd -> length += sizeof(urcp_aggr_conf_t);
	    cmd -> length = htons(cmd->length);

	}
	/*在misc buffer中创建记录*/
	urcp_create_aggr_record(urcp_aggr_msg.dst_mac, urcp_aggr_msg.user_ip, uid,misc_type, (urcp_aggr_conf_t*)NULL);
	/*发送*/
	h2_send_frame(buff, ulen);
    }
    return ret;
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET_REPLY
/*
 *处理联动vlan的消息
 *bhou
 */
int32 urcp_msg_vlan_config(struct msgbuf_t *web_msg)
{
    uchar  buff[URCP_BUF_SIZE];
    uint16 uid=0u;
    uint32 ulen;
    uint32 u_misc_type=MISC_GET_VLAN;/*默认为读*/
    session_type_t sess_type=SESSION_TYPE_GET;/*默认为读*/
    uint16 u_urcp_type=URCP_GET;/*默认为读*/
    urcp_vlan_msg_t  urcp_vlan_msg;
    urcp_vlan_conf_t* vlan_config=NULL;
    cmd_hdr_t* cmd;
    int32 ret = -1;
	int iftag = 0;
    memset(buff, 0, sizeof(buff));

    memcpy(&urcp_vlan_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_vlan_msg_t));
    /*读消息*/
    u_misc_type = urcp_vlan_msg.opt_type;/*misc buffer操作类型*/
    if(u_misc_type == MISC_SET_PVLAN)/*设置操作*/
    {
	sess_type = SESSION_TYPE_SET;
	u_urcp_type = URCP_SET;
    }
    /*分配会话id*/
    if(session_alloc(sess_type, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_vlan_cb)>=0)/*分配成功*/
    {
	ret = 0;
	iftag = urcp_buid_header(buff, urcp_vlan_msg.dst_mac,u_urcp_type , uid,urcp_vlan_msg.password);/*初始化头部信息*/

	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/*读取*/
	ulen += add_cmd(cmd, CMD_VLAN_CONFIG);/*加一条命令为vlan设置。*/
//	vlan_config = (urcp_vlan_conf_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)]);/*跳过各个头长度，指向命令内容首地址*/
	vlan_config = (urcp_vlan_conf_t*)(&buff[ulen]);/*跳过各个头长度，指向命令内容首地址*/
	vlan_config->misc_type = htonl(u_misc_type);/*misc opt type*/
	vlan_config->next_vlan_no = htons(urcp_vlan_msg.start_vlan_no);/*从哪个开始获取*/

	URCP_DEBUG(("%s next_vlan_no:%d\n ", __func__,urcp_vlan_msg.start_vlan_no));

	vlan_config->port_num = htonl(urcp_vlan_msg.port_ct);/*端口个数*/
	if(u_misc_type == MISC_SET_PVLAN)/*设置port vlan操作*/
	{
	    memcpy(&(vlan_config->vlan_group[0]) , &(urcp_vlan_msg.vlan_group),sizeof(urcp_vlan_group_t));/*拷贝到第一个组中（目前一次设置一个，若想设置多个，可用其他组）*/
	    vlan_config->vlan_group[0].vlan_no=htons(vlan_config->vlan_group[0].vlan_no);/*组号*/
	    vlan_config->vlan_group[0].member=htonl(vlan_config->vlan_group[0].member);/*成员*/
	    vlan_config->vlan_type = htonl(PORT_VLAN);
	}
	ulen += sizeof(urcp_vlan_conf_t);/*调整长度*/
	cmd -> length = ntohs(cmd->length);/*调整命令长度*/
	cmd -> length += sizeof(urcp_vlan_conf_t);
	cmd -> length = htons(cmd->length);

	/*在misc buffer中创建记录*/
	urcp_create_vlan_record(urcp_vlan_msg.dst_mac, urcp_vlan_msg.user_ip, uid,u_misc_type,(urcp_vlan_conf_t*)NULL);/*因为是在misc buffer中区分具体是什么类型的vlan操作，故需u_misc_type*/
	/*发送*/
	h2_send_frame(buff, ulen);
    }
    return ret;
}
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
/*
 *处理联动读取tag vlan端口设置
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_qvlan_pconf_cb)>=0)
    {
	ret = 0;
	/*读消息*/
	memcpy(&urcp_qvlan_pconf_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_qvlan_pconf_msg_t));
	iftag = urcp_buid_header(buff, urcp_qvlan_pconf_msg.dst_mac, (uint16)URCP_GET, uid, urcp_qvlan_pconf_msg.password);
	URCP_DEBUG(("%s\n", __func__));
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/*读取*/
	ulen += add_cmd(cmd, CMD_QVLAN_PCONF_CONFIG);
	/*在misc buffer中创建记录*/
	urcp_create_qvlan_pconf_record(urcp_qvlan_pconf_msg.dst_mac, urcp_qvlan_pconf_msg.user_ip, uid, MISC_GET_GENERAL, (urcp_qvlan_pconf_conf_t*)NULL);

	/*发送*/
	h2_send_frame(buff, ulen);
    }

    return ret;
}
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
/*
 *处理联动读取RSTP LACP的消息
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_rstp_lacp_cb)==0)
    {
	ret = 0;
	/*读消息*/
	memcpy(&urcp_rstp_lacp_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_rstp_lacp_msg_t));
	iftag = urcp_buid_header(buff, urcp_rstp_lacp_msg.dst_mac, (uint16)URCP_GET, uid, urcp_rstp_lacp_msg.password);
	URCP_DEBUG(("%s\n", __func__));
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/*读取*/
	ulen += add_cmd(cmd, CMD_RSTP_LACP_CONFIG);
	/*在misc buffer中创建记录*/
	urcp_create_rstp_lacp_record(urcp_rstp_lacp_msg.dst_mac, urcp_rstp_lacp_msg.user_ip, uid, MISC_GET_GENERAL, (urcp_rstp_lacp_conf_t*)NULL);

	/*发送*/
	h2_send_frame(buff, ulen);
    }

    return ret;
}
#endif
#ifdef URCP_CMD_PASSWORD_CONFIG_SET_REPLY
/*
 *处理联动密码消息
 *ChenShiyu
 */
int32 urcp_msg_password_config(struct msgbuf_t *web_msg)
{
		URCP_DEBUG(("\n%s\n", __func__));
	uchar  buff[URCP_BUF_SIZE];
	uint16 uid = 0u;
	uint32 ulen, i;
	uint32 misc_type_pwd = MISC_GET_GENERAL;/*默认为读*/
	session_type_t sess_type = SESSION_TYPE_GET;/*默认为读*/
	uint16 u_urcp_type = URCP_GET;/*默认为读*/
	urcp_pwd_msg_t  urcp_pwd_msg;
	urcp_hdr_t* header;
	unsigned char *dev_passwd = NULL;
	cmd_hdr_t* cmd;
	int32 ret = -1;
	uchar *dstAddr;
	uchar *passwd;

	memset(buff, 0, sizeof(buff));
	/*复制消息*/
	memcpy(&urcp_pwd_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_pwd_msg_t));
	misc_type_pwd = urcp_pwd_msg.opt_type;
	if(misc_type_pwd == MISC_SET_GENERAL)
	{
		u_urcp_type = URCP_SET;
		sess_type = SESSION_TYPE_SET;
	}
	/*头部信息*/
	urcp_buid_header_with_8021q(buff, urcp_pwd_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

	header ->flag = htons(FLAG_PASSWD);/*需要密码*/
	/*长度*/

	/*读取*/
	ulen += add_cmd(cmd, CMD_PASSWORD_CONFIG);
	
	if (misc_type_pwd == MISC_SET_GENERAL)/*设置操作*/
	{
		dev_passwd = (unsigned char *)( &buff[ulen]);
		memcpy(dev_passwd, urcp_pwd_msg.dev_password, MAX_PASSWORD_LEN);
		URCP_DEBUG(("%s:,device password:%s\n", __func__, dev_passwd));
		ulen += MAX_PASSWORD_LEN;/*调整长度*/
		cmd->length = ntohs((uint16)(cmd->length));/*调整命令长度*/
		cmd->length += MAX_PASSWORD_LEN;
		cmd->length = htons((uint16)(cmd->length));
	}

	/*依次发送多个包 */
	for (i = 0u; i < urcp_pwd_msg.batch_info.number; i++)
	{
		/*分配会话id*/
		if(session_alloc(sess_type, 0u, URCP_CMD_TIME_OUT, &uid, (cb_fp)urcp_password_cb) == 0)
		{
			ret = 0;
			/*修改目的MAC地址*/
			memcpy(((eth_hdr_t *)buff)->dst_addr, &urcp_pwd_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);
			/*重新设置id*/
			header->request_id = htons(uid);
			passwd = header->password;
			strncpy(passwd, urcp_pwd_msg.batch_info.password[i], MAX_PASSWORD_LEN);
			URCP_DEBUG(("%s: i:%d, passwd:%s\n", __func__, i, header->password));
			/*在misc buffer中创建记录*/
			dstAddr=((eth_hdr_t *)buff)->dst_addr;
			urcp_create_password_record(dstAddr, urcp_pwd_msg.batch_info.user_ip, \
					uid, MISC_SET_GENERAL, NULL);
			/*发包*/
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
 *处理联动读取镜像的消息
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_mirror_cb)==0)
    {
	    /*读消息*/
	memcpy(&urcp_mirror_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_mirror_msg_t));
	iftag = urcp_buid_header(buf, urcp_mirror_msg.dst_mac, (uint16)URCP_GET, id1, urcp_mirror_msg.password);
	URCP_DEBUG(("%s passwd:%s\n", __func__, urcp_mirror_msg.password));
	if(iftag == 1)
	{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/*读取镜像*/
	len += add_cmd(cmd, CMD_MIRROR_CONFIG);
	/*在misc buffer中创建记录*/
	urcp_create_mirror_record(urcp_mirror_msg.dst_mac, urcp_mirror_msg.user_ip, id1, MISC_GET_GENERAL, (urcp_mirror_conf_t*)NULL);

	/*发送*/
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
 *处理联动设置镜像的消息
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_mirror_cb)==0)
    {
	/*复制消息*/
	memcpy(&urcp_mirror_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_mirror_msg_t));
	iftag = urcp_buid_header(buf, urcp_mirror_msg.dst_mac, (uint16)URCP_SET, id1, urcp_mirror_msg.password);
	if(iftag == 1)
	{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*长度*/
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/*设置镜像信息*/
	len += add_cmd(cmd, CMD_MIRROR_CONFIG);
	mirror_config = (urcp_mirror_conf_t*)(&buf[len]);
	mirror_config->port_num = urcp_mirror_msg.port_ct;
	mirror_config->mirror_port = urcp_mirror_msg.mirror_port;
	mirror_config->mirror_source = urcp_mirror_msg.mirror_source;
	URCP_DEBUG(("mirr:%d, %08x\n", mirror_config->mirror_port, mirror_config->mirror_source));
	/*在misc buffer中创建记录*/
	urcp_create_mirror_record(urcp_mirror_msg.dst_mac, urcp_mirror_msg.user_ip, id1, MISC_SET_GENERAL, mirror_config);
	/*调整长度*/
	len += sizeof(urcp_mirror_conf_t);
	cmd -> length = ntohs(cmd->length);
	cmd -> length += sizeof(urcp_mirror_conf_t);
	cmd -> length = htons(cmd->length);
	mirror_config->mirror_port = htonl(mirror_config->mirror_port);
	mirror_config->mirror_source = htonl(mirror_config->mirror_source);
	/*发送*/
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

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
    {/*分配成功*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));
	init_ap_basic_misc_opt_assist(&misc_buff);/*初始化*/
	memcpy(&urcp_general_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_general_msg_t));/*读取消息*/
	/*读消息*/
	URCP_DEBUG(("%s %d\n", __func__,__LINE__));
	iftag = urcp_buid_header(buff, urcp_general_msg.dst_mac, (uint16)URCP_GET, uid, urcp_general_msg.password);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*长度*/
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
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
	/*在misc buffer中创建记录*/
	urcp_create_general_record(urcp_general_msg.dst_mac,urcp_general_msg.user_ip,uid,MISC_GET_GENERAL,&misc_buff,(uchar*)NULL);
	h2_send_frame(buff, ulen);	/* 发送 */ 
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
    {/*分配成功*/
	/*读消息*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));

	init_ap_basic_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
	memcpy(&urcp_ap_basic_msg, msgbuf->sw_msg.buffer, sizeof(urcp_ap_basic_conf_msg_t));
	urcp_general_msg = &(urcp_ap_basic_msg.general_msg);/*指向通用结构体信息*/

	iftag = urcp_buid_header(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, urcp_general_msg->password);
//	printf("%s, iftag:%d\n", __func__, iftag);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*长度*/
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/* working mode */
	/*设置命令类型*/
	ulen += add_cmd(cmd, CMD_AP_WORK_MODE);
	/*设置命令内容*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	urcp_ap_basic_msg.ap_basic_conf.work_mode = htonl(urcp_ap_basic_msg.ap_basic_conf.work_mode);
	memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.work_mode), sizeof(urcp_ap_basic_msg.ap_basic_conf.work_mode));
	ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.work_mode);/* 调整长度 */
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
	ulen += len;/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += len;
	cmd->length = htons(cmd->length);

	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_DHCP_EN);
	/*设置命令内容*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	DBCK("ac dhcp_en = %d", urcp_ap_basic_msg.ap_basic_conf.dhcp_en);
	urcp_ap_basic_msg.ap_basic_conf.dhcp_en = htonl(urcp_ap_basic_msg.ap_basic_conf.dhcp_en);
	memcpy(cmd_data_ptr, &(urcp_ap_basic_msg.ap_basic_conf.dhcp_en), sizeof(urcp_ap_basic_msg.ap_basic_conf.dhcp_en));
	ulen += sizeof(urcp_ap_basic_msg.ap_basic_conf.dhcp_en);/* 调整长度 */
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
		ulen += len;/*调整长度*/
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
	/* 最后将cmd->length转换成网络字节序 */
	/*在misc buffer中创建记录*/
	urcp_create_general_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip,uid,MISC_SET_GENERAL,&misc_buff, (uchar*)(&urcp_ap_basic_msg.ap_basic_conf));
	URCP_DEBUG(("%s %d %s ulen:%d\n", __func__,__LINE__, "h2_send_frame",ulen));
	h2_send_frame(buff, ulen);	/* 发送 */ 
	//frame_dump(buff, ulen);
	ret = 0;
    }
    return ret;
}
#endif


#ifdef URCP_CMD_PASSWD_AUTH_GET_REPLY
/*
 *发送密码验证包
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
    /*复制消息*/
    memcpy(&urcp_passwd_auth_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_batch_msg_t));

    /*头部信息*/
    urcp_buid_header_with_8021q(buf, urcp_passwd_auth_msg.dst_mac, (uint16)URCP_GET, 0u, NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_PASSWD);/*需要密码*/

    /*命令*/
    if(web_id==GET_AUTH)
    {
	len += add_cmd(cmd, CMD_PASSWD_AUTH);
	URCP_DEBUG((" cmd->length : %d\n", ntohs(cmd->length)));
    }
    else
    {
	URCP_DEBUG(("%s: topoy\n", __func__));
	len += add_cmd(cmd, CMD_HELLO);

	/*读取序列号*/
	cmd++;
	len += add_cmd(cmd, CMD_SERIAL_NO);
	header -> cmd_no += 1u; 

	/*读取设备命令*/
	cmd++;
	len += add_cmd(cmd, CMD_DEV_NAME);
	header -> cmd_no += 1u; 

	/*读取软件版本 */
	cmd++;
	len += add_cmd(cmd, CMD_SW_VERSION);
	header -> cmd_no += 1u; 

	cmd++;
	len += add_cmd(cmd, CMD_PRODUCT_MODEL);
	header -> cmd_no += 1u; 
    }
    /*依次发送多个包 */
    for(i=0u;i<urcp_passwd_auth_msg.number;i++)
    {
	/*分配会话id*/
	if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_passwd_auth_cb)==0)
	{
	    /*修改目的mac信息*/
	    memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_passwd_auth_msg.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
	    /*重新设置id*/
	    header->request_id = htons(id1);
	    strncpy(header->password, urcp_passwd_auth_msg.password[i], MAX_PASSWORD_LEN);
	    URCP_DEBUG(("%s: i:%d, passwd:%s\n", __func__, i, header->password));
	    /*在misc buffer中创建记录*/
	    urcp_create_passwd_auth_record(((eth_hdr_t*)buf)->dst_addr, urcp_passwd_auth_msg.user_ip, id1, MISC_GET_GENERAL);

		/*发包*/
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
    init_ap_conf_temp_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/

    /*复制消息*/
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id1, (cb_fp)urcp_ap_config_template_cb)==0)
    {
	iftag = urcp_buid_header(buf, urcp_general_msg->dst_mac, (uint16)URCP_SET, id1, urcp_general_msg->password);
	if(iftag == 1)
	{
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*长度*/
		len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
	}

	/*设置镜像信息*/
	len += add_cmd(cmd, CMD_CONFIG_TEMPLATE);
	tmp = (urcp_ap_conf_temp_conf_t*)(&buf[len]);
	memcpy(tmp, urcp_conf_temp, sizeof(urcp_ap_conf_temp_conf_t));
	
	/*在misc buffer中创建记录*/
	urcp_create_ap_conf_temp_record(urcp_general_msg->dst_mac, urcp_general_msg->user_ip, id1, MISC_SET_GENERAL);
	/*调整长度*/
	len += sizeof(urcp_ap_conf_temp_conf_t);
	cmd -> length = ntohs(cmd->length);
	cmd -> length += sizeof(urcp_ap_conf_temp_conf_t);
	cmd -> length = htons(cmd->length);
	
	/*发送*/
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

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_join_sz_cb)==0)
    {/*分配成功*/
	/*读消息*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));
	memset(web_name,0,sizeof(web_name));
	memset(&urcp_ap_join_sz_msg,0,sizeof(urcp_ap_join_sz_msg));
	memset(tmp_sz_name,0,sizeof(tmp_sz_name));

	init_ap_join_sz_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
	memcpy(&urcp_ap_join_sz_msg, msgbuf->sw_msg.buffer, sizeof(urcp_ap_join_sz_msg));
	urcp_general_msg = &(urcp_ap_join_sz_msg.general_msg);/*指向通用结构体信息*/
    //printf("\n< %s %d > mac:%s\n", __func__,__LINE__,str_macaddr(urcp_general_msg->dst_mac));

#if 1  /* 为了兼容之前的版本，暂时需要执行这里,之后的版本下发服务区之后，AP 会回应一个ACK, 若AP 升级到1.9.3 版本后，可注释掉这里, 否则ACK包就无意义了 */
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
               if(prof->ssid_encode_type == 1)// 1为手机优先，将编码转换为utf-8格式
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
               if(prof->ssid_encode_type == 1)// 1为手机优先，将编码转换为utf-8格式
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
	ulen += sizeof(sz_name);/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_name);
	cmd->length = htons(cmd->length);
#if 0
	/* sz status */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_STATUS);
	/*设置命令内容*/
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_status, sizeof(sz_status));
	ulen += sizeof(sz_status);/* 调整长度 */
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
	ulen += sizeof(sz_vid);/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_vid);
	cmd->length = htons(cmd->length);
	
#if 0
	/* Max client */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_AP_MAX_CLIENT);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_client, sizeof(sz_client));
	ulen += sizeof(sz_client);/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_client);
	cmd->length = htons(cmd->length);
#endif

	/* auth type */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_AUTH);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_auth_mode, sizeof(sz_auth_mode));
	ulen += sizeof(sz_auth_mode);/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_auth_mode);
	cmd->length = htons(cmd->length);
	
	/* wep */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_WEP);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_wep, sizeof(sz_wep));
	ulen += sizeof(sz_wep);/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_wep);
	cmd->length = htons(cmd->length);
	
	/* wpa */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_WPA);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_wpa, sizeof(sz_wpa));
	ulen += sizeof(sz_wpa);/*调整长度*/
	cmd->length = ntohs(cmd->length);
	cmd->length += sizeof(sz_wpa);
	cmd->length = htons(cmd->length);
	
	/* psk */
	cmd = (cmd_hdr_t*)(&buff[ulen]); 
	ulen += add_cmd(cmd, CMD_SZ_PSK);
	cmd_data_ptr = (uchar *)(&buff[ulen]);
	memcpy(cmd_data_ptr, sz_psk, sizeof(sz_psk));
	ulen += sizeof(sz_psk);/*调整长度*/
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
	    
	/*在misc buffer中创建记录*/
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
	/*发送*/
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

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_mac_filter_cb)==0)
    {/*分配成功*/
	/*读消息*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));
	memset(&urcp_ap_mac_filter_msg,0,sizeof(urcp_ap_mac_filter_msg));
	memset(str,0,sizeof(str));
//	memset(ssid, 0, sizeof(ssid));

	init_ap_mac_filter_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
	memcpy(&urcp_ap_mac_filter_msg, msgbuf->sw_msg.buffer, sizeof(urcp_ap_mac_filter_msg));
	urcp_general_msg = &(urcp_ap_mac_filter_msg.general_msg);/*指向通用结构体信息*/

	iftag = urcp_buid_header(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, urcp_general_msg->password);
	if(iftag == 1)
	{
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*长度*/
		ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]); /*定位到cmd*/
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
	ulen += sizeof(num);/*调整长度*/
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
					ulen += sizeof(ApMacFilterProfile);/*调整长度*/
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
	h2_send_frame(buff, ulen);	/* 发送 */ 
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


    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
	return -1;
    }

    memset(buffer, 0, sizeof(buffer));

    memcpy(&urcp_ac_set_mode_msg, web_msg->sw_msg.buffer, sizeof(struct _urcp_ac_set_mode_msg_t));

    urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_SET, id, NULL);
	/*指向包头 带tag*/
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*联动发现 */
	cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*不需要密码*/

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

    /*复制消息*/
    memcpy(&urcp_send_channel_msg, web_msg->sw_msg.buffer, sizeof(urcp_send_channel_msg_t));
    /*头部信息*/
    urcp_buid_header_with_8021q(buf, urcp_send_channel_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
	/*指向包头 带tag*/
	header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

	ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*联动发现 */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_PASSWD);/*需要密码*/

    ulen += add_cmd(cmd, CMD_AP_CHANNEL);
    cmd_data_ptr = (int32 *)(&buf[ulen]);
    *cmd_data_ptr = htonl(urcp_send_channel_msg.channel);
    ulen += sizeof(urcp_send_channel_msg.channel);
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(urcp_send_channel_msg.channel);
    cmd->length = htons(cmd->length);


    /*依次发送多个包 */
    for(i=0u;i<urcp_send_channel_msg.batch_info.number;i++)
    {
        /*分配会话id*/
        if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
        {
            /*修改目的mac*/
            memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_send_channel_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
            /*重新设置id*/
            header->request_id = htons(uid);
            strncpy(header->password, urcp_send_channel_msg.batch_info.password[i], MAX_PASSWORD_LEN);

            /*在misc buffer中创建记录*/
            urcp_create_ap_basic_record(&(urcp_send_channel_msg.batch_info.dst_mac[i*MAC_LEN]), urcp_send_channel_msg.batch_info.user_ip, uid, MISC_SET_GENERAL);
			/*发包*/
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

	init_ap_basic_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/

    /*复制消息*/
    memcpy(&urcp_send_load_balance_msg, web_msg->sw_msg.buffer, sizeof(urcp_send_load_balance_msg_t));
    /*头部信息*/
	urcp_buid_header_with_8021q(buf, urcp_send_load_balance_msg.batch_info.dst_mac, (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*需要密码*/

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

    /*依次发送多个包 */
    for(i=0u;i<urcp_send_load_balance_msg.batch_info.number;i++)
    {
        /*分配会话id*/
        if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_ap_basic_cb)==0)
        {
            DBCK("mac[%d]: %s\n",i,str_macaddr(&(urcp_send_load_balance_msg.batch_info.dst_mac[MAC_LEN*i])));
            /*修改目的mac*/
            memcpy(((eth_hdr_t*)buf)->dst_addr, &urcp_send_load_balance_msg.batch_info.dst_mac[i*MAC_LEN], MAC_LEN);/*DMAC*/
            /*重新设置id*/
            header->request_id = htons(uid);

	        urcp_create_config_record(&(urcp_send_load_balance_msg.batch_info.dst_mac[i*MAC_LEN]), urcp_send_load_balance_msg.batch_info.user_ip, uid, MISC_SET_GENERAL);
			/*发包*/
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
    /*头部信息*/
    urcp_buid_header_with_8021q(buf,prof->ap[0].macAddr.addr , (uint16)URCP_SET, 0u, (uchar*)NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*需要密码*/

    ulen += add_cmd(cmd, CMD_AP_LB_MAX_CLIENT);
    for(i=0;i<4;i++){
        cmd_data_ptr = (uint32 *)(&buf[ulen]);
        *cmd_data_ptr = htonl(client);
        ulen += sizeof(client);
    }
    cmd->length = ntohs(cmd->length);
    cmd->length += sizeof(client)*4;
    cmd->length = htons(cmd->length);

    /*依次发送多个包 */
    for(i=0u;i<prof->num;i++)
    {
        if(prof->ap[i].isAlive ){

        /*分配会话id*/
        if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, NULL)==0)
        {
            DBCK("mac[%d]: %s\n",i,str_macaddr(&(prof->ap[i].macAddr.addr)));
            /*修改目的mac*/
            memcpy(((eth_hdr_t*)buf)->dst_addr, &(prof->ap[i].macAddr.addr), MAC_LEN);/*DMAC*/

            /*重新设置id*/
            header->request_id = htons(uid);

			/*发包*/
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

    /*复制消息*/
    memcpy(&lbGroudNum, web_msg->sw_msg.buffer, sizeof(uint32));
    /*头部信息*/
    DBCK("lbGroudNum: %d\n",lbGroudNum);
    ap_load_balance_process(lbGroudNum);    /*处理负载均衡*/
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

    /*头部信息*/
    iftag = urcp_buid_header(buf,pMac , (uint16)URCP_SET, uid, (uchar*)NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET]);

	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    header ->flag = htons(FLAG_NO_PASSWD);/*需要密码*/

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

    /*头部信息*/
    urcp_buid_header_with_8021q(buf,mac_addr_cooperate, (uint16)URCP_SET, uid, (uchar*)NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(uint32)]);

    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_hdr_t*)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    header ->flag = htons(FLAG_NO_PASSWD);/*需要密码*/

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
                    /*修改目的mac*/
                    memcpy(((eth_hdr_t*)buf)->dst_addr, &(tmp->mac), MAC_LEN);/*DMAC*/
                    /*重新设置id*/
                    header->request_id = htons(uid);
                    *cmd_data_ptr = htonl(wait_sec);

					/*发包*/
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
 *将收到的文件重新命名
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
    /*复制文件名*/
    strncat(newname, name, MAX_FILE_NAME_LEN);
    if(sscanf(newname, "%*[^:]:%d", &timep)==1)
    {
	/*设置新文件名*/
	p=localtime(&timep);
	strtok(newname, ":");
	tmp = strlen(newname);
	/*将时间写入文件名*/
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

	/*旧文件名*/
	memset(oldname, 0, sizeof(oldname));
	strcpy(oldname, TFTP_ROOT);
	/*复制文件名*/
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
 *删除会话产生的临时文件
 *
 *KeHuatao
 */
static void del_urcp_tmp_file(char *file)
{
    char tp[128];

    URCP_DEBUG(("%s: %s\n", __func__, file));
    memset(tp, 0, sizeof(tp));
    /*补全路径*/
    strcat(tp, TFTP_ROOT);
    strncat(tp, file, MAX_FILE_NAME_LEN);
    /*删除*/
    unlink(tp);

    return;
}
/*
 *发送或读取文件的回调函数
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
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_OK:/*成功*/
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
	case URCP_STATUS_FAIL:/*操作失败*/
	case URCP_STATUS_AUTH_ERROR:/*密码错误*/
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_config_set_status(sess->local_id, urcp_state);
	    /*设置页面提示信息*/
	    break;
	default:
	    /*其它不做处理,由交换互判断为超时*/
	    break;
    }
    /*删除临时文件*/
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

    /*判断状态*/
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
 *发送完读日志文件请求后的回调函数
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
 *复制文件
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
 *将当前日志写入文件
 *KeHuatao
 */
int32 create_log_file(char *file)
{
    char tp[64], ip[32], tmp[256];
    uchar mac[MAC_LEN];
    int32 ret=0, nip;
    FILE*p=NULL;
    file_param_t param;


    /*文件名*/
    memset(tp, 0, sizeof(tp));
    strcat(tp, TFTP_ROOT);
    create_tmp_file("log", file);
    strcat(tp, file);
    p=fopen(tp, "w");
    if(p!=NULL)
    {
	memset(ip, 0, sizeof(ip));
	fprintf(p, "#");
	/*打印ip*/
	get_ip(SWITCH_IF_NAME, &nip);
	if(nip!=0)
	{
	    int2ip(ntohl(nip), ip);
	    fprintf(p, "%s ", ip);
	}
	/*打印mac*/
	memset(mac, 0, sizeof(mac));
	get_mac(SWITCH_IF_NAME, mac);
	fprintf(p, "%s\n", str_macaddr(mac));
	fclose(p);
    }
    
    /*复制文件*/
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
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &uid, urcp_wl_mac_filter_cb)==0)
    {/*分配成功*/
	/*读消息*/
	memset(buff, 0, sizeof(buff));
	memset(&misc_buff,0,sizeof(misc_buff));

	init_mac_filter_misc_opt_assist(&misc_buff);/*初始化辅助结构体*/
	memcpy(&msg_buf, msgbuf->sw_msg.buffer, sizeof(urcp_wl_mac_filter_msg_t));
	urcp_general_msg = &(msg_buf.general_msg);/*指向通用结构体信息*/

	iftag = urcp_buid_header(buff, urcp_general_msg->dst_mac, (uint16)URCP_SET, uid, NULL);
	if(iftag == 1)
	{
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	/*mac filter*/
	/*设置命令类型*/
	ulen += add_cmd(cmd, CMD_WL_MAC_FILTER);
	cmd->length = ntohs(cmd->length);
	/*设置命令内容*/
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
	h2_send_frame(buff, ulen);	/* 发送 */ 
    }
    return 0;
}
#endif

#if (URCP_DISCONNECT_STA==FYES) 
#define MAX_DISCONNECT_AP 20u /*一次最多向多个AP发包*/
#define MAX_DISCONNECT_STA 30u /*一次最多向多个AP发包*/
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

    /*读消息*/
    memset(ap_discon_info, 0, sizeof(ap_discon_info));
    msg = (urcp_sta_msg_t*)(msgbuf->sw_msg.buffer);
    /*找出所有STA所属的AP,并分开存储*/
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
		   /*记录STA MAC的位置*/
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
    /*向各个AP发包*/
    for(i=0; i<apNum; i++)
    {
	memset(buff, 0, sizeof(buff));
	db_info(URCP_MODULE_ID_USER_LIST, "%s: send to ap: %s\n", __func__, str_macaddr(ap_discon_info[i].apMac));
	iftag = urcp_buid_header(buff, ap_discon_info[i].apMac, (uint16)URCP_SET, 0u, NULL);
	if(iftag == 1)
	{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	header->flag = htons(FLAG_NO_PASSWD);/*不需要密码*/
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
	h2_send_frame(buff, ulen);	/* 发送 */ 
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
    /*读消息*/
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
	    /*联动发现 */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}
	else
	{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	header->flag = htons(FLAG_NO_PASSWD);/*不需要密码*/
	cmd->type = htonl(CMD_AP_ADD_MAC_BLACK);
	cmd->length = sizeof(cmd_hdr_t);
	ulen += sizeof(cmd_hdr_t);
	data = &(buff[ulen]);
	msg_len = sizeof(msg->staNum) + sizeof(msg->mode) + sizeof(urcp_black_list_t)*staNum;
	msg->staNum = htons(msg->staNum);
	msg->mode = htons(msg->mode);
	memcpy(data, msg, msg_len);	    //拷贝黑名单列表数据
	cmd->length += msg_len;
	cmd->length = htons(cmd->length);
	cmd->cmd_status = htons(URCP_OK);
	ulen += msg_len;
	
//	printf("%s, msg_len = %d\n", __func__, msg_len);
	h2_send_frame(buff, ulen);	/* 发送 */
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

    /*读消息*/
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
	    /*联动发现 */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}
	else
	{
	    header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET]);
	    ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
	    cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
#else
	urcp_buid_header_with_8021q(buff, mac_addr_cooperate, (uint16)URCP_SET, 0u, NULL);
	/*指向包头 带tag*/
	header = (urcp_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(uint32)]);
	ulen = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	cmd = (cmd_hdr_t*)(&buff[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
#endif
	header->flag = htons(FLAG_NO_PASSWD);/*不需要密码*/
	cmd->type = htonl(CMD_AP_ADD_MAC_BLACK);
	cmd->length = sizeof(cmd_hdr_t);
	ulen += sizeof(cmd_hdr_t);
	data = &(buff[ulen]);
	msg_len = sizeof(msg->staNum) + sizeof(msg->mode) + sizeof(urcp_black_list_t)*staNum;
	msg->staNum = htons(msg->staNum);
	msg->mode = htons(msg->mode);
	memcpy(data, msg, msg_len);	    //拷贝黑名单列表数据
	cmd->length += msg_len;
	cmd->length = htons(cmd->length);
	cmd->cmd_status = htons(URCP_OK);
	ulen += msg_len;
	
//	printf("%s, msg_len = %d\n", __func__, msg_len);
//	h2_send_frame(buff, ulen);	/* 发送 */
	urcp_send_by_vlan(buff,ulen);
    }
}
#endif
