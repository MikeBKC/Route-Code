#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#include "common.h"
#ifdef URCP_WEB
#include <switch.h>
#include <sw_cfg.h>
#include "urcp.h"
#include "app_msg.h"
#include "urcp_config.h"
#include "urcp_hello.h"
#else
#include "urcp.h"
#endif

#ifdef URCP_WEB
#include "urcp_ap.h"
#endif

#if (UTT_NV_1800N == FYES)
#include "ap_msg.h"
#endif

extern void urcp_tftp_poll(void);
#ifdef URCP_WEB
extern int32 urcp_discover(void);
extern void sendDiscover(void);
//extern int32 topoy_discover(void);

static int msgId = -1;
static int semId = -1;
/*
 * ipc initial
 *KeHuatao
 */
void urcp_ipc_init(void)
{
    sw_shm_init();
    msgId = sw_msg_init();
    semId = sw_sem_init();
}

/*
 *message process
 *
 *KeHuatao
 */
static int msg_handle(void)
{
    int len;
    struct msgbuf_t msgbuf;

    len = msgrcv(msgId, &msgbuf, sizeof(msgbuf.sw_msg), 0, MSG_NOERROR | IPC_NOWAIT);
    if(len!=-1)
    {
	 if(msgbuf.mtype == SW_MSG_TYPE)
	 {
	     if(msgbuf.sw_msg.msg_type == MSG_TYPE_URCP)
	     {
		 switch(msgbuf.sw_msg.msg_code)
		 {
#ifdef URCP_CMD_MIRROR_CONFIG_GET_REPLY
		     case GET_MIRROR:
			 urcp_msg_get_mirror(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_SET_REPLY
		     case SET_MIRROR:
			 urcp_msg_set_mirror(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_PASSWD_AUTH_GET_REPLY
		     case GET_AUTH:
		     case GET_AUTH_TOPOY:
			 urcp_msg_get_passwd_auth(&msgbuf, msgbuf.sw_msg.msg_code);
			 break;
#endif
#if defined(URCP_CMD_AGGR_CONFIG_GET_REPLY) || defined(URCP_CMD_AGGR_CONFIG_SET_REPLY)
		     case AGGR_CONFIG:
			 urcp_msg_aggr_config(&msgbuf);
			 break;
#endif
#if defined(URCP_CMD_VLAN_CONFIG_GET_REPLY) || defined(URCP_CMD_VLAN_CONFIG_SET_REPLY)
		     case URCP_VLAN_CONFIG:
			 urcp_msg_vlan_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
		     case QVLAN_PORT_CONFIG:
			 urcp_msg_qvlan_port_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
		     case GET_RSTP_LACP:
			 urcp_msg_get_rstp_lacp(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET_REPLY
		     case URCP_GET_PORT_CONF:
			 urcp_msg_get_port_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
		     case URCP_SET_PORT_CONF:
			 urcp_msg_set_port_config(&msgbuf);
			 sw_sem_p(semId, 1);
			 break;
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
		     case URCP_GET_QOS_RATE_LIMIT:
			 urcp_msg_get_qos_rate_limit_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET_REPLY
		     case URCP_SET_QOS_RATE_LIMIT:
			 urcp_msg_set_qos_rate_limit_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
		     case URCP_GET_STORM_RATE_LIMIT:
			 urcp_msg_get_storm_rate_limit_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET_REPLY
		     case URCP_SET_STORM_RATE_LIMIT:
			 urcp_msg_set_storm_rate_limit_config(&msgbuf);
			 break;
#endif
		     case URCP_CMD_ALONE:
			 urcp_msg_cmd_alone_config(&msgbuf);
			 break;
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
		     case URCP_GET_SYS_CONF:
			 urcp_msg_get_sys_conf_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
		     case URCP_GET_SYS_INFO:
			 urcp_msg_get_sys_info_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_SET_REPLY
		     case URCP_SET_SYS_CONF:
			 urcp_msg_set_sys_conf_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
		     case URCP_GET_IP_FILTER:
			 urcp_msg_get_ip_filter_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_PASSWORD_CONFIG_SET_REPLY
		     case PASSWORD_CONFIG:
			 urcp_msg_password_config(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_GET_CONFIG_GET_REPLY
		     case GET_CONFIG:
			 urcp_msg_read_file(&msgbuf, FILE_ID_CONFIG);
			 break;
#endif
		     case SET_CONFIG:
			 urcp_msg_write_file(&msgbuf, FILE_ID_CONFIG);
//			 urcp_msg_write_conf(&msgbuf);
			 break;
#ifdef URCP_CMD_AP_BASIC_CONFIG_SET_GET_REPLY
		     case GET_AP_BASIC_CONFIG:
			 urcp_msg_get_ap_basic_config(&msgbuf); //处理联动设置AP基本配置的消息
			 break;
		     case SET_AP_BASIC_CONFIG:
			 urcp_msg_set_ap_basic_config(&msgbuf); //处理联动设置AP基本配置的消息
			 break;
#endif
#ifdef URCP_CMD_GET_IMAGE_GET_REPLY
			case GET_IMAGE:
			urcp_msg_read_file(&msgbuf, FILE_ID_IMAGE);
			break;
#endif
#ifdef URCP_CMD_SET_IMAGE_SET_REPEY
		     case SET_IMAGE:
			 urcp_msg_write_file(&msgbuf, FILE_ID_IMAGE);
			 break;
#endif
#ifdef URCP_CMD_SET_CONFIG_TEMPLATE_SET_REPEY
		     case SET_CONFIG_TEMPLATE:
			 urcp_msg_set_ap_config_template(&msgbuf);
			 break;
#endif
#ifdef URCP_CMD_AP_JOIN_SZ_SET_REPLY
		     case URCP_SET_AP_JOIN_SZ:
			urcp_msg_set_ap_join_sz(&msgbuf);
			break;
#endif
#ifdef URCP_CMD_AP_MAC_FILTER_SET_REPLY
		     case URCP_SET_AP_MAC_FILTER:
			urcp_msg_set_ap_mac_filter(&msgbuf);
			break;
#endif

#ifdef URCP_CMD_SET_AC_MODE_REPLY
		     case URCP_CMD_SET_AC_MODE:
			 urcp_msg_set_ac_work_mode(&msgbuf);
			 break;
#endif

#ifdef URCP_CMD_CHANGE_CHANNEL_REPLY
		     case CHANGE_CHANNEL:
			 urcp_msg_change_channel(&msgbuf);
			 break;
#endif

#if (AP_LOAD_BALANCE == FYES)
		     case URCP_CMD_LOAD_BALANCE:
			 urcp_msg_load_balacne(&msgbuf);
			 break;
#endif


		     default:
			 break;
		 }
	     }
	     else if(msgbuf.sw_msg.msg_type == MSG_TYPE_GET)
	     {
		 switch(msgbuf.sw_msg.msg_code)
		 {
		     case GET_URCP_DISCOVERY:
		        urcp_reset();
			urcp_discover();
			sendDiscover();
			 break;
#if 0
		     case GET_URCP_TOPOY:
			 topoy_discover();
			 break;
#endif
#ifdef URCP_AC
		     case START_AP_SCAN:
#if 1
		         urcp_reset();
#endif
			 urcp_ap_discover();
			 break;

			 case START_AP_SCAN_CLT:
			 urcp_ap_clt_discover();
			 break;
			 case SET_AP_SCAN_MODE:
			 ac_timer_init();
			 break;
#if (AP_LOAD_BALANCE == FYES)
			 case SET_AP_LOAD_BALANCE:
			 urcp_set_ap_load_balacne(&msgbuf);
			 break;
#endif
#if (URCP_DISCONNECT_STA==FYES) 
                     case URCP_MSG_DISCONNECT_STA:
			 urcp_msg_disconnect_sta(&msgbuf);
                         break;    
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
                     case URCP_MSG_ADD_BLACK_LIST:
			 urcp_msg_black_list(&msgbuf);
			 break;
		     case URCP_MSG_ADD_BLACK_LIST_SZ:
			urcp_msg_black_list_sz(&msgbuf);
			break;
#endif

#endif
		     default:
			 break;
		 }
	     }
         else if(msgbuf.sw_msg.msg_type == MSG_TYPE_URCP_DB)
         {
         switch(msgbuf.sw_msg.msg_code)
         {
             case URCP_DB_SET:
             urcp_set_debug(&msgbuf);
             break;
             case URCP_DB_GET:
             urcp_get_debug(&msgbuf);
             break;
             default:
             break;
         }
         }
	 }
    }

    return 0;
}
#endif

#define  SLEEPTIME (POLL_TIME*1000)
/*
 * webmsg task
 * KeHuatao
 */
void *urcp_tsk(void *arg)
{
#ifdef URCP_AC
#ifdef URCP_STA_MAC_TABLE 
     ac_sta_table_init();
#endif
    ac_timer_init();
#endif
#ifdef URCP_AP
    ap_timer_init();
#ifdef URCP_TRAP_CMD_AP_STAT
    ap_sendRate_init();
#endif
#endif
    while(1)
    {
#ifdef URCP_WEB
	/*message*/
	msg_handle();
#endif
#ifdef URCP_AP
	ap_msg_handle();
#endif
	/*urcp poll*/
	urcp_timer();
	/*tftp poll*/
	urcp_tftp_poll();
#ifdef URCP_AC
	ac_timer();
#ifdef URCP_TRAP_CMD_AP_STAT
	ac_save_ap_stat();
#endif
#if AP_WL_POWER_AUTO
    wl_power_auto();
#endif

#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
	ac_auto_collect_clts_to_file();
#endif

#ifdef URCP_STA_MAC_TABLE
        ac_sta_table_handle();
#endif

#endif
#ifdef URCP_AP
#ifdef URCP_TRAP_CMD_AP_STAT
	ap_stat();//在20s超时之前，需要记录下rx_byte和tx_byte
#endif
	ap_state_handle();
	config_backup_timer();
#if AP_WL_POWER_AUTO
    ap_collect_around_ap_signal();
#endif
#if AP_SEND_CLTS_AUTO
    ap_trap_sta_poll();
#endif
#endif
	/*sleep*/
	usleep(SLEEPTIME);
    }

    return ((void *)0);
}
#ifdef URCP_AC
/*
 *send message
 *KeHuatao
 */
int sendwebmsg(int msg_type, int web_id)
{
    struct msgbuf_t webmsg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));
    webmsg.mtype = SW_MSG_TYPE;
    webmsg.sw_msg.msg_type = msg_type;
    webmsg.sw_msg.msg_code = web_id;
    if(msgsnd(msgId, &webmsg, sizeof(webmsg.sw_msg), 0)==-1)
    {
	/*error*/
	perror("goahead: message send error:");
    }
    return 0;
}


/*
 *发送带参数的消息
 *len  参数长度
 *KeHuatao
 */
int sendwebmsgbuffer(int msg_type, int web_id, char* msg_buffer, unsigned int len)
{
    int l;
    struct msgbuf_t webmsg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));

    /*类型*/
    webmsg.mtype = SW_MSG_TYPE;
    webmsg.sw_msg.msg_type = msg_type;
    /*页面id*/
    webmsg.sw_msg.msg_code = web_id;
    /*buffer*/
    if(len<=MAX_MSG_SIZE)
    {
	memcpy(webmsg.sw_msg.buffer, msg_buffer, len);
	l = (int)(((struct msgbuf_t*)0)->sw_msg.buffer);
	l = l + len;
	/*发送消息*/
	if(msgsnd(msgId, &webmsg, l, 0)==-1)
	{
	    /*error*/
	    perror("goahead: message send error");
	}
    }
    else
    {
	printf("sendwebmsgbuffer: length error\n");
    }

    return 0;
}
#endif
