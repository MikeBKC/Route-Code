/**
 * Copyright (C) 2002-2009 UTT Technologies Co., Ltd. 
 * All Rights Reserved. 
 *
 * File:	 rrcp.c
 * Author:   jmj, jyshen,
 * Date:
 * Version:            
 * Description:     
 *  This document provides API interface switch's cooperating
 *  function; the switch who sends the discover(the same as
 *  hello frame) frame is called master, and we call all of the
 *  other switchs at the same network which will reply the hello
 *  frame as slave; By parsing the received reply frame, the
 *  master can get the slave switch's IP address, MAC address,
 *  the source port; According to these infomation,the master
 *  can control the slave easily; we can change the IP address,
 *  restart the device and so on.
 *  
 *  
 *  �޸ģ�
 *  	�»���   2009-10
 *  
 */
#include <string.h>
#include <netinet/in.h>
#include "common.h"

#ifdef URCP_WEB
#include <switch.h>
#include <sw_cfg.h>
#include <urcp_ipc.h>
#endif

#include "urcp.h"
#include "urcp_hello.h"
#include "rrcp.h"
#include "packet.h"
#include <mibUrcp.h>

#define EVENTS_LOG_NO_OF_SUPPORTED 8
#define MAX_PORT 24
#define UIP_BUFSIZE     1514
#define FRAME_TYPE_OFFSET  (sizeof(struct uip_eth_hdr))

#define FRAME_TYPE(x) (x[FRAME_TYPE_OFFSET])

#define RRCPBUF ((RRCP_rx*)&uip_buf[0])

#define RMBUF ((RRCP_rx *)&uip_buf[0])	//add for RRCP receive 
#define RSBUF ((RRCP_tx a*)&uip_buf[0])	//add for RRCP send 

#define RMBUF2 ((RRCP_rx *)&rrcp_cache[0])	    
#define RSBUF2 ((RRCP_tx *)&rrcp_cache[0])	


RRCP_NODE rrcpNode[MAX_RRCPNODE];

#define FALSE 0
#define TRUE  1


static uint16 rrcp_frame_id_counter = 0;

uint16 rrcpCounts = 0;
volatile uint16 uip_len;

uchar uip_buf[UIP_BUFSIZE+2];
uchar  rrcp_cache[RRCP_MAX_SIZE];
uchar  rrcp_send_cache[RRCP_MAX_SIZE];

uchar result_mask = 0;

void rrcp_action_cmd(uint port);

extern uchar mac_addr_cooperate[MAC_ADDRESS_LEN];

void sendDiscover(void)
{
    char name[64];
    uchar mac[MAC_LEN];
    RRCP_rx *ptr;
    uchar hello_buf[RRCP_RX_MIN_LEN + ONE_INSTRUCTION_LEN] = {0};

    ptr = (RRCP_rx *)&hello_buf[0];
    
    uip_len = 0;
    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);
    memcpy(ptr->ethhdr.dest.addr, mac_addr_cooperate, MAC_ADDRESS_LEN);	    
    memcpy(ptr->ethhdr.src.addr, mac, MAC_ADDRESS_LEN);
					    
    ptr->ethhdr.type = htons(UIP_ETHTYPE_RRCP); 
    ptr->protocol = RRCP_PROTOCOL;		
    ptr->auth_key = htons(RRCP_AUTH_KEY);	    
    ptr->frame_type = RRCP_ACTIONS;		


    if(rrcp_frame_id_counter > 0xfffe)
    {
	rrcp_frame_id_counter = 0;
    }
    ptr->frame_id = rrcp_frame_id_counter++;//����ֻ��¼��ʹ�ñ��������ͳ�ȥ�����ݰ�
    ptr->cmd_no = 1;

    ptr->buff.operation[0].op_type.op_class = RRCP_HELLO;
    ptr->buff.operation[0].op_type.op_code.op_code_comm =  RRCP_CODE_COMM;
									    
    memcpy(RRCPBUF, hello_buf, sizeof(hello_buf) );
    uip_len = sizeof(hello_buf);

    printf_rrcp("rrcp hello send\n");
#if 1
    urcp_send(PORT_NULL, (uchar *)ptr, uip_len);
#else
    send_broadcast((uchar *)ptr, uip_len, 0xFFFFFF); 
#endif

    //-------------------
#if (RRCP_100M)
    sendDiscover_100M();
#endif
}



/*************Revision  2.0********/
//RRCP_MISC  rrcp_misc;
//uchar miscBuff[sizeof(events_log_t) * EVENTS_LOG_NO_OF_SUPPORTED+1];  /*���Զ�̽�������־,��һ�ֽڷų���*/

/**
 * 
 * @brief 
 * 		handle the rrcp hello reply frame.
 */
static void rrcp_hello_reply_process(uchar cmd_index, uint port)
{
    cmd_index = cmd_index;

    urcp_dev_t *dev;
    
    printf_rrcp("%s\n", __func__);
    dev = dev_get(((pRRCP_rx)rrcp_cache)->ethhdr.src.addr, 0, ntohl(((pRRCP_rx)rrcp_cache)->rrcp_ipaddr),
	    (DEV_TYPE_SWITCH|DEV_FLAG_MANAGE|DEV_FLAG_AUTHED));
    if((dev!=NULL) && (dev->urcp_version<=RRCP_VERSION))
    {
	dev->urcp_version = RRCP_VERSION;
    memcpy(dev->dev_name, ((pRRCP_rx)rrcp_cache)->hostname, MAX_SYSTEM_NAME_LEN + 1);

    printf_rrcp("ip %x\n", ((pRRCP_rx)rrcp_cache)->rrcp_ipaddr);
    
    dev->serial_no = ntohl(((pRRCP_rx)rrcp_cache)->buff.operation[0].register_data[0].l);
    dev->link_port = port;

    printf_rrcp("hello reply process: mac %02x:%02x:%02x:%02x:%02x:%02x\n",
	    ((pRRCP_rx)rrcp_cache)->ethhdr.src.addr[0],
	    ((pRRCP_rx)rrcp_cache)->ethhdr.src.addr[1],
	    ((pRRCP_rx)rrcp_cache)->ethhdr.src.addr[2],
	    ((pRRCP_rx)rrcp_cache)->ethhdr.src.addr[3],
	    ((pRRCP_rx)rrcp_cache)->ethhdr.src.addr[4],
	    ((pRRCP_rx)rrcp_cache)->ethhdr.src.addr[5]);
    
    printf_rrcp("hello reply process: ip %x\n", dev->ip);
    
    rrcpCounts++;
    uip_len = 0;
    }
    return;
}



/**
 * 
 * @brief 
 * 		verify the actual total frame length is equal to the value 
 *		provided in the length field
 * @return if the length is right,then reurn 1. otherwise return 0.
 */
static uchar len_ok(uchar cmd_no)
{
    if (RRCP_RX_MIN_LEN + (cmd_no * ONE_INSTRUCTION_LEN) == uip_len)
	return 1;
    else
	return 0;    
}

#define MODEL_NUM   7

/**
 * 
 * @brief 
 * 		process the rrcp reply frame.
 *              
 */
static void rrcp_reply_process(system_frame_header_t *head)
{
    const char * const model_code[MODEL_NUM]={"SG2124","SG3124R","SG2124F","SG3124F","SG3024","GS1624","DIS5024TG"};

    uchar cmd_numb, cmd_counter;
    urcp_dev_t *dev = NULL;
    uchar mac[MAC_LEN] = {0};
    uint ip = 0;
    
    uint port = head->source_port_no;

    cmd_numb = RMBUF2->cmd_no;

    printf_rrcp("%s\n", __func__);
    if (cmd_numb == RRCP_TYPE_DATA)	   //����֡
    {
	//print_str("reply data_frame\n");
	switch (RMBUF2->buff.op_buff.data_type)
	{
#if 0
	    case RRCP_DATA_RATE_LIMITE_GET:	   //����ֵ
		rrcp_misc.rateUnit = (RMBUF2->buff.op_buff.bytes[0]) | ((uint16)(RMBUF2->buff.op_buff.bytes[1])<<8);
		memcpy(rrcp_misc.buff.c, &(RMBUF2->buff.op_buff.bytes[2]),MAX_PORT*2);
		//for (i = 0;i < MAX_PORT*2;i++)
		{
		    //rrcp_misc.buff.[i] = RMBUF2->buff.op_buff.bytes[i+2];
		}
		break;
	    case RRCP_DATA_RUN_STATE_GET:	//����״̬
		memcpy(rrcp_misc.buff.c, RMBUF2->buff.op_buff.bytes, 9*MAX_PORT);//9*24�ֽ�
		break;
		//case RRCP_DATA_GW_PORT://��ȡԶ�̽��������ؼ�������port
		//memcpy(rrcp_misc.buff.c, RMBUF2->buff.op_buff.bytes, 11);//����ip��MAC��port
		//break;
	    case RRCP_DATA_LOG:     //��ȡԶ�̽���������־,��־����uip_buff�У�û�п���RMBUF2
		if ((RMBUF->buff.op_buff.len) > EVENTS_LOG_NO_OF_SUPPORTED)
		    RMBUF->buff.op_buff.len = EVENTS_LOG_NO_OF_SUPPORTED;
		miscBuff[0] = RMBUF->buff.op_buff.len;   //�¼�����
		
		memcpy(&miscBuff[1], RMBUF->buff.op_buff.bytes, sizeof(events_log_t)*(RMBUF->buff.op_buff.len));
		
		break;
#endif

	    case RRCP_DATA_MISC:      //�ͺţ�����汾����˿�����
		memcpy(mac, RMBUF2->ethhdr.src.addr, MAC_ADDRESS_LEN);
    
		printf_rrcp("get rrcp misc\n");
		ip = ntohl(RMBUF2->rrcp_ipaddr);  
		printf_rrcp("look up ip  %x\n", ip);
		printf_rrcp("look up mac %x\n", mac[5]);


		dev = urcp_dev_table_lookup(mac, ip);

		if ((dev == NULL) || (dev->urcp_version>RRCP_VERSION))
		{
		    printf_rrcp("lookup failed, mac[5] %x, ip %x\n", mac[5], ip);
		}
		else
		{
		    /*SG3124R���ڶ�İ�*/
		    if((RMBUF2->buff.op_buff.bytes[4])<=MODEL_NUM)
		    {
			strcpy(dev->dev_model, model_code[RMBUF2->buff.op_buff.bytes[4] - 1]);
		    }
		    else 
		    {
			switch (RMBUF2->buff.op_buff.bytes[4])
			{
			    case SG3124F:
				strcpy(dev->dev_model, "SG3124F");
				break;
			    default:
				break;
			}
                    }
		    dev->up_ports = RMBUF2->buff.op_buff.bytes[5];
		    printf_rrcp("up_ports %d\n", dev->up_ports);
		    dev->alarm_level = RMBUF2->buff.op_buff.bytes[6];
		    printf_rrcp("alarm_level %d\n", dev->alarm_level);

		    printf_rrcp("%s\n", &(RMBUF2->buff.op_buff.bytes[7]));
		    printf_rrcp("len: %d\n", ntohs(RMBUF2->buff.op_buff.len));
		    memcpy(dev->sw_version, &(RMBUF2->buff.op_buff.bytes[7]), ntohs(RMBUF2->buff.op_buff.len) - 7);

		    printf_rrcp("sw version %s\n",dev->sw_version);
		}
		
		break;
	    default:
		break;
	}
#if 0
	    case RRCP_DATA_MAC_NUM:
		memcpy(rrcp_misc.buff.c, RMBUF2->buff.op_buff.bytes, RMBUF2->buff.op_buff.len);
		break;
	    case RRCP_DATA_SRC_IP:
		memcpy(rrcp_misc.buff.c, RMBUF2->buff.op_buff.bytes, RMBUF2->buff.op_buff.len);
		break;
	    default:
		rrcp_reply_class_error(cmd_counter);
		break;
	}

#endif
	uip_len = 0;
	return;
    }
    if ((cmd_numb < 1) || (cmd_numb > MAX_INSTRUCTION))//cmd no can not be 0
    {
	uip_len = 0;
	return;
    }

    if (len_ok(cmd_numb) == 0)
    {
	uip_len = 0;
	return;
    }

    uip_len = 0;
    for (cmd_counter = 0; cmd_counter < cmd_numb; cmd_counter++)
    {
	switch (RMBUF2->buff.operation[cmd_counter].op_type.op_class)
	{
	    case RRCP_HELLO_REPLY:              
		rrcp_hello_reply_process(cmd_counter, port);
		break;
	    case RRCP_GET_REPLY:                
		//rrcp_get_reply_process(cmd_counter);
		break;
	    case RRCP_SET_REPLY:                
		//rrcp_set_reply_process(cmd_counter);
		break;
	    default:
		//rrcp_reply_class_error(cmd_counter);
		break;
	}
    }
    //RRCP_DEBUG_INFO(reply_process_over);
    //uip_len = 0;
}

/**
 * ���������ݵ�action֡
 * 
 * 
 * ����     ��
 * ����ֵ   �� 
 *  
 *  KeHuatao
 */

#if 0
void rrcp_action_data(void)
{
    vtss_ip_config_t ip_cfg;
    struct DATE _date;
    struct TIME _time;

    if (FALSE == exclude_muticast_addr(RMBUF2->ethhdr.dest.addr))//�鲥��ַ
    {
	switch (RMBUF2->buff.op_buff.data_type)
	{
	    case RRCP_DATA_CFG_BROADCAST://�·�����(gateway+ʱ��+��)
		if ( RMBUF2->buff.op_buff.bytes[0]
			&& (RMBUF2->buff.op_buff.bytes[0] != 127)
			&& (RMBUF2->buff.op_buff.bytes[0] < 224))
		{
		    if (eeprom_read_ip_mode())//������ipģʽ
		    {
			get_ip_config(&ip_cfg);
			if (
				(ip2long(ip_cfg.ip_addr) & ip2long(ip_cfg.subnet_mask))
				== (ip2long(RMBUF2->buff.op_buff.bytes) & ip2long(ip_cfg.subnet_mask))
			   )
			{//���غͱ�����ͬһ���βŰ�
			    memcpy(ip_cfg.gateway, (RMBUF2->buff.op_buff.bytes), sizeof(ip_addr_t));
			    eeprom_write_gateway_ip_addr(ip_cfg.gateway);
			    set_ip_config(&ip_cfg);
			}
		    }
		}
		_date.year = *((ushort*)(RMBUF2->buff.op_buff.bytes + 4));
		_date.month = *((ushort*)(RMBUF2->buff.op_buff.bytes + 6));
		_date.day = *((ushort*)(RMBUF2->buff.op_buff.bytes + 8));
		_time.hour = *((ushort*)(RMBUF2->buff.op_buff.bytes + 10));
		_time.min = *((ushort*)(RMBUF2->buff.op_buff.bytes + 12));
		_time.sec = *((ushort*)(RMBUF2->buff.op_buff.bytes + 14));
		time_set(&_date, &_time);
		send_arprequest(1, MAX_PORT);
		//ʱ������
		//
		//rrcp_timer_type = (1<<BIT_BIND_IP_PORT) | (1<<BIT_BIND_MAC_PORT) | (1<<BIT_BIND_ARP);
		rrcp_timer_type = (1<<BIT_BIND_MAC_PORT) | (1<<BIT_BIND_ARP);
		bind_port_mask = 0xffffff;
		rrcp_timer_couter = 1;
		//bindAll();
		//arpBindAll();
		return;
	    default:
		result_mask = 1;
		return;
	
    }
    if (rrcp_password_ok(RMBUF2->password))
    {
	switch (RMBUF2->buff.op_buff.data_type)
	{
	    case RRCP_DATA_RATE_LIMITE_GET:	  //��ȡ�˿�����ֵ
		rrcp_send_rate_limite();
		break;
	    case RRCP_DATA_RATE_LIMITE_SET:	  //���ö˿�����ֵ
		rrcp_set_rate_limite();
		break;
	    case RRCP_DATA_RUN_STATE_GET:     //��ȡ����״̬
		rrcp_send_run_state();
		break;
	    case RRCP_DATA_MAC_PORT_BIND_SET:     //MAC/PORT��
		rrcp_bind_port_mac();
		break;
	    case RRCP_DATA_IP_PORT_BIND_SET:      //IP/PORT��
		rrcp_bind_port_ip();
		break;
		//case RRCP_DATA_GW_PORT:            //��ȡ���ؼ������ڶ˿�
		//   rrcp_send_gw_port();
		//	break;
	    case RRCP_DATA_LOG:                  //��ȡ��־
		rrcp_send_log();
		break;
	    case RRCP_DATA_MAC_NUM:          //��ȡĳ�˿��е�MAC��
		rrcp_send_mac_num();
		break;
	    case RRCP_DATA_SRC_IP:           //��ȡĳ�˿���source ip����ip����ʹ��
		rrcp_send_srcIp();
		break;
		//		case RRCP_DATA_PORT:
		//rrcp_port_set();
		//	break;
	    default:
		//((RRCP_rx xdata*)&rrcp_send_cache[0])->cmd_no = 1;//�յ���֡��RRCP_TYPE_DATA��
		//�����صĽ��֡������֡��ʽ����
		//rrcp_class_error(0);
		result_mask = 1;
		RRCP_DEBUG_INFO(unknow_operate_code);
		break;
	}
    }
    else
    {
	((RRCP_rx xdata*)(&rrcp_send_cache[0]))->cmd_no = 1;
	//�����صĽ��֡������֡��ʽ����
	get_password_error_notice(0);
	result_mask = 1;
	RRCP_DEBUG_INFO(password_err);
    }
}

#endif

#if (URCP_DEVICE != URCP_ROUTER)
/**
 * 
 * @brief 
 *		  build the reply frame's header.except the result reply frame,
 *		  all of the frame has the same header struct.
 *              
 */
static void rrcp_header_build(void)  //just suit for RRCP_ACTIONS frame	
{
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    RRCP_rx *ptr = NULL;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);
    get_ip(name, &ipaddr);
    memset(rrcp_send_cache, 0, sizeof(rrcp_send_cache));
    ptr = (RRCP_rx *)&rrcp_send_cache[0];

    memcpy(ptr->ethhdr.dest.addr, RMBUF2->ethhdr.src.addr, MAC_ADDRESS_LEN);

    printf_rrcp("src mac %x\n", RMBUF2->ethhdr.src.addr[5]);

    memcpy(ptr->ethhdr.src.addr, mac, MAC_ADDRESS_LEN);

    ptr->rrcp_ipaddr = ipaddr;

    ptr->ethhdr.type = htons(UIP_ETHTYPE_RRCP);	//0X8899
    ptr->protocol = RRCP_PROTOCOL;				//protocol
    ptr->auth_key = htons(RRCP_AUTH_KEY);		//auth_key 0x2379
    ptr->frame_type = RRCP_REPLY;               

    //eeprom_read_system_name(ptr->hostname);//jmj_device_name
    ptr->frame_id = RMBUF2->frame_id;			//frame_idͬ���յ��ı���,���ﱻ������
    ptr->cmd_no = RMBUF2->cmd_no;				//atleast 1
}
#endif
#if (URCP_DEVICE != URCP_ROUTER)
/**
 * 
 * @brief 
 * 		send a success frame for all of the command instructions execute
 *		without any exception
 * @return none
 */

static void rrcp_send_success(void)
{
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    RRCP_tx *ptr;

    memset(rrcp_send_cache, 0, sizeof(rrcp_send_cache));
    ptr = (RRCP_tx *)&rrcp_send_cache[0];

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);
    get_ip(name, &ipaddr);
    //if (get_ip(ETHNAME, &ip) != -1)
    ptr->rrcp_ipaddr = ipaddr;
    memcpy(ptr->ethhdr.dest.addr, RMBUF2->ethhdr.src.addr, MAC_ADDRESS_LEN);           
    memcpy(ptr->ethhdr.src.addr, mac, MAC_ADDRESS_LEN);


    ptr->ethhdr.type = htons(UIP_ETHTYPE_RRCP); 
    ptr->protocol = RRCP_PROTOCOL;              
    ptr->auth_key = htons(RRCP_AUTH_KEY);       
    ptr->frame_type = RRCP_RESULTS;             

    ptr->frame_id = RMBUF2->frame_id;           
    ptr->result = RESULT_SUCCESS;
    ptr->result_mask = RESULT_MASK_NO_ERROR;

    memcpy(RMBUF, rrcp_send_cache, RRCP_TX_LEN);
    uip_len = RRCP_TX_LEN;
}
/**
 * 
 * @brief 
 * 		if happen some fatal error during the command instructions executing
 *		we should send out a fail result frame.
 * @remark see also rrcp_send_success
 */
static void rrcp_send_fail(void)
{
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    RRCP_tx *ptr;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);
    get_ip(name, &ipaddr);

    memset(rrcp_send_cache, 0, sizeof(rrcp_send_cache));
    ptr = (RRCP_tx *)&rrcp_send_cache[0];

    memcpy(ptr->ethhdr.dest.addr, RMBUF2->ethhdr.src.addr, MAC_ADDRESS_LEN);           
    memcpy(ptr->ethhdr.src.addr, mac, MAC_ADDRESS_LEN);

    ptr->rrcp_ipaddr = ipaddr;
    
    ptr->ethhdr.type = htons(UIP_ETHTYPE_RRCP); 
    ptr->protocol = RRCP_PROTOCOL;              
    ptr->auth_key = htons(RRCP_AUTH_KEY);       
    ptr->frame_type = RRCP_RESULTS;             

    ptr->frame_id = RMBUF2->frame_id;           
    ptr->result = RESULT_FAIL;
    ptr->result_mask = RESULT_MASK_FRAME_TYPE_ERROR;

    memcpy(RMBUF, rrcp_send_cache, RRCP_TX_LEN);
    uip_len = RRCP_TX_LEN;

}


/**
 * 
 * @brief 
 * 		send out a report of the execute state of the command instructions 
 */
static void rrcp_send_result(void)
{
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    RRCP_tx *ptr;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);
    get_ip(name, &ipaddr);
    memset(rrcp_send_cache, 0, sizeof(rrcp_send_cache));
    ptr = (RRCP_tx *)&rrcp_send_cache[0];

    memcpy(ptr->ethhdr.dest.addr, RMBUF2->ethhdr.src.addr, MAC_ADDRESS_LEN);           
    memcpy(ptr->ethhdr.src.addr, mac, MAC_ADDRESS_LEN);

    ptr->rrcp_ipaddr = ipaddr;

    ptr->ethhdr.type = htons(UIP_ETHTYPE_RRCP); 
    ptr->protocol = RRCP_PROTOCOL;              
    ptr->auth_key = htons(RRCP_AUTH_KEY);       
    ptr->frame_type = RRCP_RESULTS;             

    ptr->frame_id = RMBUF2->frame_id;           
    ptr->result = RESULT_SOMEONE_FAIL;
    ptr->result_mask = result_mask;

    memcpy(RMBUF, rrcp_send_cache, RRCP_TX_LEN);
    uip_len = RRCP_TX_LEN;
}
#endif

#if (URCP_DEVICE != URCP_ROUTER)
/**
 * 
 * @brief 
 *		  build the whole rrcp frame,the frame type can be get,set,and hello;
 * 		  actually, we can send a mixed frame include all of the alove instruction.
 *		  we can send at most 5 instructions in one frame. 
 *              
 */
static void rrcp_frame_build(uint port)	  //build whole frame
{
    uchar i;

    i = ((RRCP_rx *)rrcp_send_cache)->cmd_no;
    
    printf_rrcp("uip_len %d\n", uip_len);
    if (i != RRCP_TYPE_DATA )	//if ( RMBUF2->cmd_no!=RRCP_TYPE_DATA )
    {
	uip_len = RRCP_RX_MIN_LEN + i*ONE_INSTRUCTION_LEN;
    }
    //else ; ��������֡ʱȷ��
    printf_rrcp("uip_len2 %d\n", uip_len);
    if (uip_len>0)
    {
	memcpy(RMBUF, rrcp_send_cache, uip_len);
//	RRCP_BUF_DUMP(rrcp_send_cache, uip_len);
	printf_rrcp("send frame result\n");	
#if 1
	urcp_send(port, rrcp_send_cache, uip_len);
#else
	send_broadcast(rrcp_send_cache, uip_len, 1 << (port - 1));//send a reply packet //��Ҫʹ��h2_uip_send��������
#endif
    }

    uip_len = 0;

    //	print_line(15);
    //	print_cr_lf();
    //RRCP_DEBUG_INFO(result_notice);
    //	print_line(15);
    //	print_cr_lf();
    //send a result packet
    if (result_mask == 0)
    {
	rrcp_send_success();
    }
    else if (result_mask == RESULT_MASK_FRAME_TYPE_ERROR)
    {
	rrcp_send_fail();
    }
    else
    {
	rrcp_send_result();
    }
    //	if (uip_len>0)
    {
	//	h2_rrcp_send();//send a reply packet //��Ҫʹ��h2_uip_send��������
    }
    //	uip_len = 0;
    //RRCP_DEBUG_INFO(rrcp_result_frame);
    //RRCP_BUF_DUMP(rrcp_send_cache, uip_len);
}
#endif
#if (URCP_DEVICE != URCP_ROUTER)
/**
 * 
 * @brief 
 * 		hold the RRCP_ACTIONS type frame.
 *              
 */
static void rrcp_action(system_frame_header_t* header, uchar* frame)
{
    uchar cmd_numb;
    
    memset(RMBUF2, 0, UIP_BUFSIZE+2);
    printf_rrcp("frame len:%d\n", header->length);
    printf_rrcp("frame port: %d\n", header->source_port_no);

    memcpy(RMBUF2, frame, header->length);
    cmd_numb = RMBUF2->cmd_no;
   
    if (cmd_numb != RRCP_TYPE_DATA)	 //������֡
    {
	if ( (cmd_numb < 1)  || (cmd_numb > MAX_INSTRUCTION))
	{
	    uip_len = 0;
	    printf_rrcp("cmd number error\n");
	    return;
	}
	if (len_ok(cmd_numb) == FALSE)
	{
	    uip_len = 0;
	    printf_rrcp("rrcp length error\n");
	    return;
	}
    }
    uip_len = 0;
    rrcp_header_build();//build a header first;rrcp_send_cache

    if (cmd_numb == RRCP_TYPE_DATA)	 //����֡
    {
#if 0
	rrcp_action_data();
#endif
    }
    else   //��ͨ����֡
    {
	rrcp_action_cmd(header->source_port_no);
    }
    rrcp_frame_build(header->source_port_no);
    

}
#endif
#if (URCP_DEVICE != URCP_ROUTER)
/**
 * ���ͱ���������汾���ͺţ���ȫ��־����߸澯���𣬻�˿��� 
 *  
 * KeHuatao 
 */
static void rrcp_send_misc(void)
{
    RRCP_rx *ptr;
    char sw_version[64] = {0};

    
    getSystemInfoSysVersion(sw_version);

    memset(rrcp_send_cache + RRCP_RX_MIN_LEN, 0, sizeof(rrcp_send_cache) - RRCP_RX_MIN_LEN);
    ptr = (RRCP_rx *)rrcp_send_cache;
    ptr->cmd_no = RRCP_TYPE_DATA;
    ptr->buff.op_buff.data_type = RRCP_DATA_MISC;
    ptr->buff.op_buff.total =  1;
    ptr->buff.op_buff.index =  0;
    *((ulong*)&(ptr->buff.op_buff.bytes)) = htonl(cfg_read_sys_serialno());  //�������кű��ڽ��ն��ҵ�rrcp_node�ж�Ӧ�Ľڵ㡣
    ptr->buff.op_buff.bytes[4] = PRODUCT_MODEL;   //�ͺ�
    ptr->buff.op_buff.bytes[5] = (uchar)(getAlivePort());      //��˿���
    ptr->buff.op_buff.bytes[6] =  3;      //��߸澯����
    memcpy(&(ptr->buff.op_buff.bytes[7]), sw_version, strlen(sw_version));

    ptr->buff.op_buff.len = 7+strlen(&(ptr->buff.op_buff.bytes[7]));
    *(ptr->buff.op_buff.bytes + ptr->buff.op_buff.len) = 0;
    uip_len = RRCP_DATA_FRAME_BYTES_OFFSET + ptr->buff.op_buff.len + 1;
    ptr->buff.op_buff.len = htons(ptr->buff.op_buff.len);
}
#endif

#if (URCP_DEVICE != URCP_ROUTER)
/**
 * 
 * @brief
 * 		process the rrcp hello frame
 */
static void rrcp_hello_process(uchar cmd_index, uint port)	//send a hello reply packet
{
    RRCP_rx *ptr;

    ptr = (RRCP_rx *)&rrcp_send_cache[0];
    ptr->buff.operation[cmd_index].op_type.op_class = RRCP_HELLO_REPLY;
    ptr->buff.operation[cmd_index].op_type.op_code.op_code_comm =  RRCP_CODE_COMM;
    ptr->buff.operation[0].register_data[0].l = htonl(cfg_read_sys_serialno());
    /*added by jyshen ������־��Ϣ*/
#if 0
    uart_redirect((uchar *)mac);
    print_mac_addr(&(uip_buf[6]));
    uart_redirect(0);
    uart_redirect((uchar xdata*)buf);
    vtss_printf(content, mac);
    uart_redirect(0);
    //print_str(buf);
    update_event_table2(buf, EVENT_DISCOVER);
    /*end jyshen*/
#endif
    uip_len = RRCP_RX_MIN_LEN + ONE_INSTRUCTION_LEN;
    memcpy(RMBUF, rrcp_send_cache, uip_len);
#if 1
    urcp_send(port, (uchar *)ptr, uip_len);
#else
    send_broadcast((uchar *)ptr, uip_len, 1 << (port - 1));
#endif
    rrcp_send_misc();
    printf_rrcp("send reply hello\n");
}
#endif
#if (URCP_DEVICE != URCP_ROUTER)
/**
 * 
 * @brief 
 * 		handle the class type error exception.
 */
static void rrcp_class_error(uchar cmd_index)
{
    RRCP_rx *ptr;

    ptr = (RRCP_rx *)&rrcp_send_cache[0];
    ptr->buff.operation[cmd_index].op_type.op_class = RRCP_CLASS_UNKNOW;
    ptr->buff.operation[cmd_index].op_type.op_code.op_code_comm =  RRCP_CODE_UNKNOW;

    result_mask = 1 << cmd_index;

}
#endif

#if (URCP_DEVICE != URCP_ROUTER)

/**
 * ������ͨ����֡
 */
void rrcp_action_cmd(uint port)
{
    uchar cmd_counter;
    for (cmd_counter = 0; cmd_counter < RMBUF2->cmd_no; cmd_counter++)
    {
	switch (RMBUF2->buff.operation[cmd_counter].op_type.op_class)
	{
	    case RRCP_HELLO:
		rrcp_hello_process(cmd_counter, port);//��������,�鲥������ַ����hello
		break;
#if 0
	    case RRCP_GET:
		if (exclude_muticast_addr(RMBUF2->ethhdr.dest.addr))//�ų��鲥��ַ
		{
		    rrcp_get_process(cmd_counter);//pswd check cpl.
		}
		else
		{
		    rrcp_get_mac_error(cmd_counter);
		}
		break;
	    case RRCP_SET:
		if (exclude_muticast_addr(RMBUF2->ethhdr.dest.addr))
		{
		    rrcp_set_process(cmd_counter);//pswd check cpl.
		}
		else
		{
		    rrcp_set_mac_error(cmd_counter);
		}
		break;
#endif
	    default:
		rrcp_class_error(cmd_counter);//cpl.
		break;
	}
    }
}
#endif
/**
 * 
 * @brief 
 * 		if we received a frame whit type 0x8899,we call this function to proecss the 
 *		rrcp frame.we distinguish 100M frame from 1000M frame by the uip_len. 
 *		the frames for 1000M switchs are classfied into 3 class: 
 *		RRCP_ACTIONS: get,set,hello
 *		RRCP_REPLY:	  set reply,get reply,hello reply
 *		RRCP_RESULTS: success,fail
 *              
 */
extern void rrcp_process(system_frame_header_t* header, uchar* frame)
{
    /*RRCP_100M_SIZE = 60 byte;1000M_SIZE = 77 byte*/
    uip_len = header -> length;


    if ((uip_len == RRCP_100M_SIZE))//process the 100M frame
    {
	//rrcp_100M();

	uip_len = 0;
	return;
    }
#if 0
    if (
	    ((uip_len < RRCP_MIN_SIZE) || (uip_len > RRCP_MAX_SIZE))
	    {
	    //vtss_printf("size:%d ,len:%d\n",RRCP_MAX_SIZE,uip_len);
	    RRCP_DEBUG_INFO(rrcp_len_error);
	    RRCP_BUF_DUMP(uip_buf,uip_len);
	    //uip_len = 0;
	    //return;
	    }
#endif

    /*�㲥���鲥*/
    if((frame[0] & 0x01u) == 1u)
    {
#ifdef CONFIG_VSC7390 
#if 1
       urcp_send(header->source_port_no, frame, header->length);
#else
	/*���ܴ�Դ�˿ڷ���*/
	port_mask = 1u << (header->source_port_no-1u); 
	port_mask = ~port_mask;
	port_mask = ALL_PORT_MASK & port_mask;
	send_broadcast(frame, header->length, port_mask);
#endif
#endif
    }
    //memcpy(ptr, (RRCP_rx xdata*)&rrcp_send_cache[0], RRCP_RX_MIN_LEN); 
    result_mask = RESULT_MASK_NO_ERROR;

    memset(rrcp_cache, 0, sizeof(rrcp_cache));//may be RMBUF, may be RSBUF
    memcpy(rrcp_cache, frame, uip_len);

    //printf_rrcp("Frame:%x\n",(uint16)FRAME_TYPE(frame));


    switch (FRAME_TYPE(frame))
    {
#if (URCP_DEVICE != URCP_ROUTER)
	case RRCP_ACTIONS:		//RMBUF;
	    rrcp_action(header, frame);
	    break;
#endif
	case RRCP_REPLY:		//RMBUF;
	    rrcp_reply_process(header);
	    break;
	case RRCP_RESULTS:		//RSBUF;ֻ��������ɹ�������ݰ�
	    //rrcp_result();
	    break;
	default:
	    //rrcp_drop();		//if drop,send a fail result frame
	    break;
    }
}


