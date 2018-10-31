/**
 * Copyright (C) 2002-2009 UTT Technologies Co., Ltd. 
 * All Rights Reserved. 
 *
 * File:	 rrcp.h
 * Author:   jmj, jyshen
 * Date:
 * Version:            
 * Description:     
 * 	 Header,struct and function prototype  definitions for RRCP.
 * 	
 */

#ifndef __RRCP_H__
#define __RRCP_H__


#define RRCP_100M 0
#define ETH_P_URCP            0x8899

#define AUTH_KEY_OFFSET 20u

#define RRCP_DEBUG 0

#if RRCP_DEBUG 
#define printf_rrcp(fmt, args...) printf(fmt, ## args)
#else
#define printf_rrcp(fmt, args...)
#endif

#define MAC_ADDRESS_LEN 6
#define MAX_SYSTEM_NAME_LEN 16

/*产品型号*/
#define SG_2124   1u
#define SG_3124R  2u 
#define SG_2124F  3u
#define SG_3124Fv1  4u  /*SG3124R的改版3124F*/
#define SG_3024   5u
#define GS_1624   6u  /*合勤OEM*/
#define DIS_5024TG     7u /*DLINK OEM*/

#define SG3124F  0x0Au
#define PRODUCT_MODEL  SG3124F 
typedef uchar system_name_t [MAX_SYSTEM_NAME_LEN + 1];

#ifndef __PACK__    
#define __PACK__    __attribute__((aligned(1), packed))
#endif
#define MAX_RRCP_SIZE               VTSS_MAXFRAMELENGTH_TAGGED
#define cmd_result_mask uchar
#define RRCP_NODE_SWVER_LENGTH          35
#define MAX_SYSTEM_NAME_LEN             16
#define MAX_RRCP_PASSWORD_LEN                16
#define RRCP_NODE_SWVER_LENGTH          35
#define MAX_PORT                        24

/*
h* Representation of a 48-bit Ethernet address.
 */
struct uip_eth_addr {
      uchar addr[6];
} __PACK__;


/*
 * The Ethernet header. 
 */
struct uip_eth_hdr {
    struct uip_eth_addr dest;
    struct uip_eth_addr src;
    uint16 type;
} __PACK__;


typedef uchar password_t [MAX_RRCP_PASSWORD_LEN + 1];

extern BOOL broadcast(uchar* dst_mac_addr);
extern volatile uchar need_srcport_flag;
extern volatile uchar srcport;
extern uchar  rrcp_send_cache[];
#define GENERATE_IPVALUE(group,value0,value1,value2,value3,option) \
    do {\
	switch(option)\
	{\
	    case IP_SET:\
	    group.ip_gate_net[IP_CURSOR].b[0] = value0;\
	    group.ip_gate_net[IP_CURSOR].b[1] = value1;\
	    group.ip_gate_net[IP_CURSOR].b[2] = value2;\
	    group.ip_gate_net[IP_CURSOR].b[3] = value3;\
	    break;\
	    case GATEWAY_SET:\
		group.ip_gate_net[GATEWAY_CURSOR].b[0] = value0;\
	    group.ip_gate_net[GATEWAY_CURSOR].b[1] = value1;\
	    group.ip_gate_net[GATEWAY_CURSOR].b[2] = value2;\
	    group.ip_gate_net[GATEWAY_CURSOR].b[3] = value3;\
	    break;\
	    case SUBNETMASK_SET:\
		group.ip_gate_net[SUBNETMASK_CURSOR].b[0] = value0;\
	    group.ip_gate_net[SUBNETMASK_CURSOR].b[1] = value1;\
	    group.ip_gate_net[SUBNETMASK_CURSOR].b[2] = value2;\
	    group.ip_gate_net[SUBNETMASK_CURSOR].b[3] = value3;\
	    break;\
	    default:\
		break;\
	}\
    }while(0)


typedef struct rrcp_node 
{	
    char* pName;/* reserved device name*/
    uchar uIndex;/* node index*/
    uchar mac[6];/* mac address*/
    uint ip;/* ip address*/

    uchar hostname[MAX_SYSTEM_NAME_LEN + 1];/*device name*/
    password_t password;/* password*/
    uchar bandwidth;/*swithch type 100M or 1000M*/
    ulong serialno;/*serial number*/

    uchar devicType;  /*型号*/
    uchar upPorts;    /*活动端口数*/
    uchar alarmLevel;  //报警级别
    uchar swVersion[RRCP_NODE_SWVER_LENGTH];   //软件版本信息
} RRCP_NODE;

typedef struct{
    ulong misc_mask;     //端口保护状态、各端口是否则插线,复用
    ushort rateUnit;              //端口限速的颗粒度  16Kbps,128Kbps,1Mbps,10Mbps,102Mbps
    union {
	ulong  l[54]; //限速值、统计接收发送字节数,复用
	uchar  c[MAX_PORT*9];
    } buff;
} RRCP_MISC;

typedef struct rrcp_result // command execute result
{
    uchar lock_state;// read or write protect
    uchar password_err_flag;// password error flag
    uchar exe_result;// result for all
    uchar exe_result_mask;// result map to every command
    ushort frame_id;// frame id
    uchar node_cursor;// node index
} rrcp_result_t;

typedef enum {   // action code that supported now
    RRCP_REBOOT, // reboot
    RRCP_RESTORE, // restore
    RRCP_SET_IP,  // set ip address

    RRCP_GET_PORT_PROTECTION,
    RRCP_SET_PORT_PROTECTION,  //设置端口保护
    RRCP_GET_RATE_LIMITE,
    RRCP_SET_RATE_LIMITE,      //端口限速
    RRCP_GET_RUN_STATE,       //读取运行状态
    RRCP_BIND_MAC_PORT,
    RRCP_BIND_IP_PORT,
    RRCP_LOG,
    RRCP_GET_GW_PORT,        //读取网关及其port
    RRCP_BIND_ALL,            //三元绑定 
    RRCP_SRC_IP,
    RRCP_MAC_NUM,
} action_code_t;

typedef struct cmd_entity // command entry
{	
    uchar cmd_index; // the index of the command 
    action_code_t action_code; // the operate type
} cmd_entity_t;

typedef struct ARP_PORT
{
    uint ipaddr;
    struct uip_eth_addr ethaddr;
    //struct arp_entry* arpptr;
    ushort counts;
} ARPPORT;
#if 0
typedef struct dev_per_port
{
    ushort counts;
    uint16 ipaddr[2];
    struct uip_eth_addr ethaddr;
    //struct arp_entry* arpptr;
} ip_per_port;    //统计各端口ip个数
#else
#define ip_per_port ARPPORT
#endif
void sendDiscover(void);// send discover frame to find the 1000M switch
void sendDiscover_100M(void);// send discover frame to find the 100M switch

#define MAX_RRCPNODE 	24        // can record total 24 switch
#define RRCP_UDP_PORT    8899     // rrcp frame type
#define MAX_INSTRUCTION  12        // max command can contained in one frame

extern RRCP_NODE rrcpNode[MAX_RRCPNODE];
extern uint16 rrcpCounts;	//added by jyshen
extern rrcp_result_t rrcpResult;
extern RRCP_MISC  rrcp_misc;     //kht
#define HELLO_REPLY 0x80		//station to server

#define  RRCP_TYPE_DATA    0XFF    //kht


//rrcp frame type
#define RRCP_UNKNOW 0	    //unknow type
#define RRCP_RESULTS 1	    //rrcp result frame
#define RRCP_ACTIONS 2	    //rrcp hello get set frame
#define RRCP_REPLY 3	    //rrcp hello reply, get reply, set reply frame


// rrcp class for rrcp action and reply type
#define RRCP_CLASS_UNKNOW 0
#define RRCP_HELLO 1
#define RRCP_GET 2
#define RRCP_SET 3
#define RRCP_HELLO_REPLY 4
#define RRCP_GET_REPLY 5
#define RRCP_SET_REPLY 6

// the supported get operate code
#define RRCP_GET_UNKNOW 0
#define GET_REG_DATA 1
#define GET_PASSWORD 2
#define GET_DEVICE_NAME 3
#define GET_MAC_ADDR 4
#define GET_IP_ADDR 5
#define GET_SN 6
#define RRCP_GET_FAIL_REPLY 7
#define GET_REG_DATA_REPLY 8
#define GET_PASSWORD_REPLY 9
#define GET_DEVICE_NAME_REPLY 10
#define GET_MAC_ADDR_REPLY 11
#define GET_IP_ADDR_REPLY 12
#define GET_SN_REPLY 13
#define GET_PASSWORD_ERROR_REPLY 14
#define GET_MAC_ERROR_REPLY 15
#define GET_UNSUPPORTED_REPLY 16
    //v2
#define GET_PORT_PROTECTION 17
#define GET_PORT_PROTECT_REPLY 18



// the supported get operate code
#define RRCP_SET_UNKNOW 0
#define SET_IP_ADDR 1
#define SET_PASSORD 2
#define SET_REBOOT 3
#define SET_RESTORE_DEFAULT 4
#define SET_REG_DATA 5
#define RRCP_SET_FAIL_REPLY 6
#define SET_IP_ADDR_REPLY 7
#define SET_PASSWORD_REPLY 8
#define SET_REBOOT_REPLY 9
#define SET_RESTORE_DEFAULT_REPLY 10
#define SET_REG_DATA_REPLY 11
#define SET_PASSWORD_ERROR_REPLY 12
#define SET_MAC_ERROR_REPLY 13
#define SET_UNSUPPORTED_REPLY 14
#define SET_PORT_PROTECTION 15/*端口保护*/
#define SET_PORT_PROTECTION_REPLY 16
#define SET_BIND_ALL  17      //联动批量操作里面的安全绑定


// for common use operate code
#define RRCP_CODE_UNKNOW 0
#define RRCP_CODE_COMM 1

// the result code
#define RRCP_RESULT_UNKNOW 0
#define RRCP_FAIL 1
#define RRCP_SUCCESS 2
#define RRCP_PSWD_ERR 3


// we classify the frame type according to the frame_type-->operate_class-->operate_code
// it just like a tree struct
typedef struct {
    uchar op_class;
    union {
	uchar op_code_get;
	uchar op_code_set;
	uchar op_code_result;
	uchar op_code_comm;
    } __PACK__ op_code;
} __PACK__ op_type_t; 

// register data in 32 bits
typedef union {
    uchar  b [4];
    ushort w [2];
    ulong  l;
} __PACK__ ul_regdata_t;

typedef struct {
    ul_regdata_t ip_gate_net[3];
} __PACK__ ip_gate_net_t;

// the operation command struct
typedef struct {
    op_type_t op_type;		
    uint16 register_addr;
    ul_regdata_t register_data[3];//most of the time we use the register_data[0] only.
    uchar attach_flag;		// used in ip address setting
    uchar adjust_flag;		//please don't del this line;it is very important to h2_rrcp_send
} __PACK__ op_instruction;


#define RRCP_DATA_RATE_LIMITE_GET 0   //读取限速值
#define RRCP_DATA_RUN_STATE_GET 1     //运行状态
#define RRCP_DATA_GW_PORT 2           //读取网关及其所在端口
#define RRCP_DATA_LOG 3               //安全日志
#define RRCP_DATA_MISC 4              //软件版本，型号，活动端口数等信息
#define RRCP_DATA_MAC_NUM 5           //读取某端口中的MAC数
#define RRCP_DATA_SRC_IP 6            //读取某端口中source ip，供ip过滤使用
#define RRCP_DATA_RATE_LIMITE_SET 7    //设置限速值 
#define RRCP_DATA_MAC_PORT_BIND_SET 8  //MAC/PORT绑定
#define RRCP_DATA_IP_PORT_BIND_SET 9   //IP/PORT绑定
#define RRCP_DATA_CFG_BROADCAST 10//下发配置


typedef struct{
    uchar data_type;//数据帧类型
    uchar total;            //本次数据共分几帧传送,联动帧最大长度较小，数据可//能分几次传送
    uchar index;              //当前是第几帧
    ushort len;                //bytes中的数据长度
    uchar bytes[sizeof(op_instruction)*MAX_INSTRUCTION  - 5];
} __PACK__ op_buffer;

//数据帧中数据部分的起始偏移
#define RRCP_DATA_FRAME_BYTES_OFFSET  (RRCP_RX_MIN_LEN + 5)
// the received rrcp frame
typedef struct RRCP_rx_t {		//set ,get,hello,get_replay
    struct uip_eth_hdr ethhdr;		//DA,SA,protocol type;14 bytes
    uchar 	frame_type;		//FRAME_TYPE_OFFSET:14;fixed pos,please don't move;offset:sizeof(struct uip_eth_hdr)

    uint	rrcp_ipaddr;			//or take it in the register data
    uchar	 	protocol;		// 
    uint16 		auth_key;       // authenticate key
    ushort 		frame_id;

    uchar hostname[MAX_SYSTEM_NAME_LEN + 1];
    password_t password;		//common.h  146
    uchar      cmd_no;          //命令个数，如果为0xff，表明本帧为数据帧
    union {                     //op_instruction结构为18字节,在帧中的起始偏移为59
	op_instruction operation[MAX_INSTRUCTION];     // command pool，命令帧
	op_buffer   op_buff;         //数据帧
    } __PACK__ buff;
} __PACK__ RRCP_rx,*pRRCP_rx; 

// the send rrcp frame struct
typedef struct RRCP_tx_t {		//hello_replay
    struct uip_eth_hdr ethhdr;		//DA,SA,protocol type 14 bytes
    uchar frame_type;			//fixed pos,please don't move;offset:sizeof(struct uip_eth_hdr)	

    uint    rrcp_ipaddr;			//
    uchar	protocol;				// 1 byte
    uint16 	auth_key;				//
    ushort 	frame_id;
    uchar 	result;					//success/fail
    cmd_result_mask result_mask;	//bit[0-4];if 1 :fail;else success
    uchar  	pad[38];
} __PACK__ RRCP_tx, *pRRCP_tx;

// defined for 100M switch
typedef struct RRCP_100M_master_t {		//set ,get,hello,get_replay
    struct uip_eth_hdr ethhdr;		//DA,SA,protocol type;14 bytes
    uchar	 protocol;
    uchar opcode;		//7 bit,the HSB is reply_flag bit,used by reply_flag_mask: 0x80
    uint16 auth_key;	
    uint16 register_addr;
    ulong register_data;
    ulong reserved[2];
    uchar pad[32];

} __PACK__ RRCP_100M_master,*pRRCP_master;

typedef struct RRCP_100M_slave_t {		//hello_replay
    struct uip_eth_hdr ethhdr;	//DA,SA,protocol type 14 bytes
    uchar	 protocol;		// 1 byte
    uchar opcode;		//7 bit,the HSB is reply_flag bit,used by reply_flag_mask: 0x80
    uint16 auth_key;	//
    uchar downlink_port;
    uchar uplink_port;
    struct uip_eth_addr uplink_mac;	//6 bytes
    uint16 chip_id;
    ulong vender_id;
    uchar pad[28];
} __PACK__ RRCP_100M_slave,*pRRCP_slave;

#define CRC_LEN		4
#define ONE_INSTRUCTION_LEN  (sizeof(op_instruction))
#define RRCP_RX_LEN		(sizeof(RRCP_rx))
#define RRCP_TX_LEN		(sizeof(RRCP_tx))
#define RRCP_RX_MIN_LEN (RRCP_RX_LEN - (MAX_INSTRUCTION * ONE_INSTRUCTION_LEN))
#define RRCP_MIN_SIZE ((RRCP_RX_MIN_LEN)<(RRCP_TX_LEN)?(RRCP_RX_MIN_LEN):(RRCP_TX_LEN))
#define RRCP_MAX_SIZE ((RRCP_RX_LEN)>(RRCP_TX_LEN)?(RRCP_RX_LEN):(RRCP_TX_LEN))

#define RRCP_100M_SIZE (sizeof(RRCP_100M_slave))
//#define RRCP_100M_SIZE2 (sizeof(RRCP_100M_master))

// define for 100M switch
#define DEF_UPLINK_ADDR0    0x00  
#define DEF_UPLINK_ADDR1    0x00 
#define DEF_UPLINK_ADDR2    0x00 
#define DEF_UPLINK_ADDR3    0x00  
#define DEF_UPLINK_ADDR4    0x00 
#define DEF_UPLINK_ADDR5    0x00  

// const defined for rrcp
#define UIP_ETHTYPE_RRCP 0x8899 // frame type
//#define RRCP_AUTH_KEY 0X2379    // authenticate key
#define RRCP_PROTOCOL 	0X01    // protocol version
#define REPLY_FLAG_MASK 0x80
#define OPERATE_MASK 0x7f       // the MSB is used for direction
#define VENDER_ID 0x40414243   // 100M switch use
#define DEF_REG_DATA 0x00000000
#define RESV_PAD 0x00000000

// defined for 100M switch
#define HELLO_PACKET 0
//#define GET_CONFIG 1
//#define SET_CONFIG 2

// defined for hintsoft control frame
#define HINTSET_PACKET 3		//hintsoft
#define HINTSOFT_OK 1
#define HINTSOFT_FAIL 0
#define HINT_REG 0x50b0	//hintsoft 

//add for set rebot
#define REBOOT_REG_ADDR 0x0000
#define REBOOT_REG_DATA 0x00000000


#define BLOCK_MASK 0xd0
#define SUBBLOCK_MASK 0x0f
#define ADDR_MASK 0xff
#define HINTSOFT_SIZE 0x50  //hintsoft

// the mode select for ip address setting
#define IP_MODE_IP_ONLY  			0		//only ip
#define IP_MODE_GATEWAY_ONLY  		1
#define IP_MODE_SUBNETMASK_ONLY 	2

#define IP_MODE_IP_GATEWAY 			3
#define IP_MODE_IP_SUBNETMASK 		4
#define IP_MODE_GATEWAY_SUBNETMASK 	5
#define IP_MODE_ALL					6

// the location for the ip,subnet mask,gateway data
#define IP_CURSOR  			0
#define GATEWAY_CURSOR 		1
#define SUBNETMASK_CURSOR 	2

#define NULL_CURSOR  0xff
#define IP_SET  			0
#define GATEWAY_SET 		1
#define SUBNETMASK_SET 	2

// result type
#define RESULT_UNKNOW    0
#define RESULT_FAIL      1
#define RESULT_SUCCESS   2
#define RESULT_SOMEONE_FAIL 3

#define RESULT_MASK_FRAME_TYPE_ERROR	0x1f
#define RESULT_MASK_NO_ERROR           0

//define for result
#define PASSWORD_OK			0
#define EXE_SUCCESS			1
#define PASSWORD_FAIL		1
#define EXE_TIMEOUT	0
#define EXE_FAIL			2
#define STATE_UNLOCK		1
#define STATE_LOCK			0	
#define STATE_READONLY		2

#define SW_100M             1
#define SW_1000M			0


//define for mac/port binding of rrcp
#define BIND_MANU       1    //手动绑定mac/port ip/port
#define BIND_AUTO       2    //全部绑定mac/port ip/port
#define BIND_DEL        3    //全部删除mac/port ip/port


#define BIT_BIND_IP_PORT           0
#define BIT_BIND_MAC_PORT          1
#define BIT_BIND_ARP               2
#define BIT_DEL                    15


#if 0
void print_hex_offset(const uchar *buf, ushort offset);
void print_hex_struc_member(const uchar *addr, ushort offset, ushort count);		//jmj_rrcp

uchar len_ok(uchar cmd_no);
//uchar exclude_muticast_addr(struct uip_eth_addr *dest_mac);
uchar exclude_muticast_addr(uchar *dest_mac);
uchar rrcp_password_ok (uchar *entry_ptr);


void hello_reply(void);
void rrcp_receive(void);
void rrcp_100M(void);

void rrcp_action(void);
void rrcp_reply_process(void);


void rrcp_result(void);
void rrcp_drop(void);
void rrcp_send_success(void);
void rrcp_send_fail(void);
void rrcp_send_result(void);

void rrcp_class_error(uchar cmd_index);
void rrcp_reply_class_error(uchar cmd_index);

void get_password_error_notice(uchar cmd_index);
void set_password_error_notice(uchar cmd_index);

void get_unknow_op_type(uchar cmd_index);
void set_unknow_op_type(uchar cmd_index);


void rrcp_send_reg_data(uchar cmd_index);

uchar rrcp_set_reg_data(uchar cmd_index);
void rrcp_set_ipaddr(uchar cmd_index);//compl.
void rrcp_set_password(uchar cmd_index);		//compl.
void rrcp_set_reboot(uchar cmd_index);		//don't support now
void rrcp_set_restore(uchar cmd_index);		//don't support here


void rrcp_send_serial_no(uchar cmd_index);
uchar validate_rrcp_ip_addr (uchar *ip_addr);

void rrcp_send_password(uchar cmd_index);

void rrcp_send_dev_name(uchar cmd_index);
void rrcp_send_mac_addr(uchar cmd_index);
void rrcp_send_ip_addr(uchar cmd_index);


void rrcp_hello_process(uchar cmd_index);
void rrcp_get_mac_error(uchar cmd_index);
void rrcp_set_mac_error(uchar cmd_index);

void rrcp_hello_reply_process(uchar cmd_index);

void rrcp_get_process(uchar cmd_index);
void rrcp_get_reply_process(uchar cmd_index);

void rrcp_set_process(uchar cmd_index);
void rrcp_set_reply_process(uchar cmd_index);

ulong rrcp_reg_read(ushort reg_addr);
uchar rrcp_reg_write(ushort reg_addr, ulong reg_value);
uchar rrcp_ip_check(ul_regdata_t ul_ipaddr);

ushort rrcp_reg_addr_build(uchar block, uchar subblock, uchar addr);
void rrcp_header_build(void);//act at rrcp_send_cache	
void rrcp_frame_build(void);//build whole frame
uchar get_rrcpNode_cursor(RRCP_NODE* rrcp_node,uchar* src_mac);

// the following three steps should execute in strict order.
/*
   please  init the fallowing struct first:

   rrcpnode,action,ip_value

   rrcp_cmd_maker(rrcpnode,1,action,ip_value,attachment);
   rrcp_frame_maker(1);
 */
void rrcp_cmd_maker(RRCP_NODE *rrcpnode,uchar cmd_no_support, 
	cmd_entity_t *action,ip_gate_net_t *ip_value,uchar attachment);
uchar rrcp_header_maker(RRCP_NODE *rrcpnode, uchar cmd_no_support);
void rrcp_frame_maker(uchar cmd_no_support);
void rrcp_test_ip(void);
void rrcp_test_reboot(void);
void rrcp_test_restore(void);

void h2_uip_send2 (void);
void h2_rrcp_send2(port_bit_mask_t dst_port_mask, ushort vid);

void h2_rrcp_send (void);
void hintsoft(void);
extern void sendDiscover(void);
extern RRCP_NODE * getrrcpnode(void);
extern void bindAll(void);
extern uchar bind_all_ip_port(ulong portmask);
extern void rrcp_cfg_broadcast(void);
extern void rrcp_timer(void);
#endif

//the fallowing macro are called in the function
#endif
