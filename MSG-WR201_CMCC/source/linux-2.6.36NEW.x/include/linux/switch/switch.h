/* ���ļ����� �ں���Ӧ�ó����õ����ݽṹ
 * оƬ�仯ʱ ���ļ����� ���ݽṹ��仯
 * bhou
 * 2011-08-17
 * * */
#ifndef _COM_SWITCH_H_
#define _COM_SWITCH_H_

#include <linux/switch/types.h>/*�����������Ͷ���*/
#include <linux/autoconf.h>

#if defined(CONFIG_UTT_WX100) || defined(CONFIG_UTT_NV840G_INT)
#define  SWITCH_IF_NAME "eth2"
#else
#define  SWITCH_IF_NAME "eth2.1"
#endif

#define  MAC_LEN  6u

#define  PORT_COUNT 24u

#define  NO_OF_PORTS PORT_COUNT    
#define  PORT_NO_START  1 
#define  PORT_NO_END    25


#define  PORT_ARRAY_SIZE (PORT_COUNT+1u) /*һ����±�1��ʼ��*/

#define  MIRROR_PORTS   0  /*�Ƿ�����ö������˿�*/

#define  EXT_CPU_VID  1u   /*���cpu vlan id*/

#define  MAX_LINK_STATUS_SIZE 32

/*����ģ�鿪��*/
#ifdef CONFIG_SWITCH_EXT

#define  SW_FEATURE_PORT_TRUNK	 1		
#define  SW_FEATURE_PORT_CONF    1
#define  SW_FEATURE_PORT_VLAN    1
#define  SW_FEATURE_PACKET       1 /*������cpu������꿪��*/
#define  SW_FEATURE_RSTP         0
#define  SW_FEATURE_NETCP        0
#define  SW_FEATURE_8021Q        0
#define  SW_FEATURE_NETBIOS      1
#define  SW_FEATURE_URCP         1


#ifdef CONFIG_IP1725
#define  SW_FEATURE_ACL        1
#define  SW_MAX_ACL            16

#define  SW_INGRESS_CPU_TAG        /*the received frame carry special cpu tag*/
#define  CPU_INGRESS_TAG_LEN  4
#define  CPU_TAG_TYPE        0x9126u

#define  SW_FEATURE_MAC_TABLE_OPT  1

#else

#define  SW_FEATURE_ACL        0

#define  SW_INGRESS_CPU_TAG        /*the received frame carry special cpu tag*/
#define  SW_EGRESS_CPU_TAG    /*the sent frame carry special cpu tag*/
#define  CPU_INGRESS_TAG_LEN  8
#define  CPU_EGRESS_TAG_LEN  4
#if 0
#define  SW_CPU_DECIDE_DEST_PORTS  /*cpu software decide dest ports that the frames come from cpu forward to */
#endif
#define  SW_FEATURE_MAC_TABLE_OPT  0
#define  SW_BC_CPU_CP      1   /*broadcast packets are copied to the cpu*/           
#endif


#define  SW_FEATURE_PORT_MIRROR			1
#define  SW_FEATURE_PORT_STATISTICS		1
#define	 SW_FEATURE_PORT_PRIORITY		1
#define  SW_FEATURE_STORM_CONTROL		1
#define  SW_FEATURE_RATE_LIMIT			1


#else  /*CONFIG_SWITCH_EXT*/

#define  SW_FEATURE_PORT_TRUNK	 		0
#define  SW_FEATURE_PORT_CONF			0
#define  SW_FEATURE_PORT_VLAN			0
#define  SW_FEATURE_PACKET			0  /*������cpu������꿪��*/
#define  SW_FEATURE_NETBIOS			0
#define  SW_FEATURE_ACL				0
#define  SW_FEATURE_MAC_TABLE_OPT		0
#define  SW_FEATURE_PORT_STATISTICS             0
#define  SW_FEATURE_PORT_MIRROR                 0
#define  SW_FEATURE_PORT_PRIORITY               0
#define  SW_FEATURE_STORM_CONTROL               0
#define  SW_FEATURE_RATE_LIMIT			0

#endif  /*CONFIG_SWITCH_EXT*/


typedef unsigned int port_no_t;
typedef unsigned int port_bit_mask_t;
typedef unsigned int vid_t;

#ifndef _TERMIOS_H     /* Ϊ�˲���/lib/include/termios.h �е�speed_t��ͻ */
#if SW_FEATURE_PORT_CONF
/* Speed type */
typedef enum _speed_t {
#ifdef CONFIG_VSC7390
    SPEED_1G,
#endif
    SPEED_100M,
    SPEED_10M
} speed_t;
#endif
#endif

#if SW_FEATURE_PORT_TRUNK

/*max trunk ��Ŀ*/
#ifdef CONFIG_VSC7390
#define MAX_TRUNK              24
#else
#define MAX_TRUNK               2
#endif

#define TRUNK_MODE_PORT_ID	0
#define TRUNK_MODE_SA		1
#define TRUNK_MODE_DA		2
#define TRUNK_MODE_SA_DA	3
#define TRUNK_MODE		TRUNK_MODE_SA_DA

#endif

#endif
