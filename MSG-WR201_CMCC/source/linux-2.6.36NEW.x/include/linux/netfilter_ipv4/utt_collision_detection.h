#ifndef __UTT_COLLISION_DETECTION_H__
#define __UTT_COLLISION_DETECTION_H__
#include <linux/kernel.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>


/*冲突检测控制变量*/
extern __be32 utt_collision_detection __read_mostly;
#ifdef CONFIG_UTT_DHCP_SERVER_DETECTION
extern __be32 utt_dhcp_server_detection __read_mostly;
#endif
#define MAX_IP_LEN 16
#if (defined CONFIG_UTT_DHCP_COLLISION) || (defined CONFIG_UTT_DHCP_SERVER_DETECTION)
/*lan口接口名*/
extern char utt_local_ifname[IFNAMSIZ];
extern char src_mac[ETH_ALEN];

extern int dhcpCollision_debug(const char* fmt, ...);
extern void utt_dhcp_collision_debug(UttNfConfType type);

#define DHCP_COLLISION_DETECTION 0x1
#define UTT_COLLISION_DETECTION_POLL_INTERVAL 10*HZ /*每隔10s自动发送一个discover包*/
#define IPPORT_BOOTPC 68 	/*DHCP 客户端*/
#define IPPORT_BOOTPS 67	/*DHCP 服务端*/
#define BOOTP_REQ	1
#define BOOTP_REP	2
#define HW_ETHER	1
#define MESSAGE_TYPE_OPTION 53
#define DHCPDISCOVER	1
#define AUTOCONFIG_OPTION 116
#define CLIENT_ID_OPTION 61
#define MIN_DHCP_OFFER_LEN 287
#define ARP_COLLISION_DETECTION 0x4

/*
** Boot Protocol (BOOTP) Header. Per RFC 951.
*/

struct bootphdr {
    u_char          bp_op;			/* packet opcode type */
    u_char          bp_htype;			/* hardware addr type */
    u_char          bp_halen;			/* hardware addr length */
    u_char          bp_hops;			/* gateway hops */
    u_long          bp_xid;			/* transaction ID */
    u_short         bp_secs;			/* seconds since boot began */
    u_short         bp_flags;			/* See BOOTP_FLAGS_* above */
    u_long          bp_ciaddr;			/* client IP address */
    u_long          bp_yiaddr;			/* 'your' IP address */
    u_long          bp_siaddr;			/* server IP address */
    u_long          bp_giaddr;			/* gateway IP address */
    u_char          bp_chaddr[16];		/* client hardware address */
    u_char          bp_sname[64];		/* server host name */
    u_char          bp_file[128];		/* boot file name */
    u_char          bp_vend[20];		/* vendor-specific area */
} __attribute__ ((packed));

#endif

#if 0
#ifdef CONFIG_UTT_NOTICE_LOG
#define NOTICE_LOG (1 << 1)
#endif
#endif

#endif
