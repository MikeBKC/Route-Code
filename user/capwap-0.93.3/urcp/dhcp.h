/*

    Copyright (c) 2005 Vitesse Semiconductor Corporation "Vitesse".  
    All Rights Reserved.  Unpublished rights reserved under the copyright laws
    of the United States of America, other countries and international treaties.
    The software is provided without a fee. Permission to use, copy, store and 
    modify, the software and its source code is granted. Permission to integrate
    into other products, disclose, transmit and distribute the software in an
    absolute machine readable format (e.g. HEX file) is also granted. 

    The source code of the software may not be disclosed, transmitted or
    distributed without the written permission of Vitesse. The software and its
    source code may only be used in products utilizing a Vitesse VSC73xx product.
 
    This copyright notice must appear in any copy, modification, disclosure,
    transmission or distribution of the software. Vitesse retains all ownership,
    copyright, trade secret and proprietary rights in the software.  

    THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
    INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR USE AND NON-INFRINGEMENT.

*/
#ifndef __DHCPPORT_H__
#define __DHCPPORT_H__


typedef unsigned int dhcp_ipaddr_t;
typedef unsigned int vtss_common_linkstate_t;
#define VTSS_COMMON_LINKSTATE_DOWN      ((vtss_common_linkstate_t)0u)
#define VTSS_COMMON_LINKSTATE_UP        ((vtss_common_linkstate_t)1u)

#define ETH_NAME "eth2.1"
#define IPADDR_BROADCAST        0xFFFFFFFF
#define IP_HEADER_LEN            20u
#define DHCP_UDP_OVERHEAD       42u /*14(Ethernet header) + 20(IP header) + 8(UDP header) */
#define DHCP_SNAME_LEN          64
#define DHCP_FILE_LEN           128
#define DHCP_FIXED_NON_UDP      236/*dhcp结构最大长度*/
#define DHCP_FIXED_LEN          (DHCP_FIXED_NON_UDP + DHCP_UDP_OVERHEAD)
                        /* Everything but options. */
#define DHCP_MTU_MAX            1500
#define DHCP_OPTION_LEN         (DHCP_MTU_MAX - DHCP_FIXED_LEN)

#define BOOTP_MIN_LEN           212u  /*dhcp报文最小长度*/

#define MIN_HALFLIFE            60

#define DHCP_DISCOVER_TMOUT     2
#define DHCP_DISCOVER_RETRIES   5
#define DHCP_REQUEST_TMOUT      2
#define DHCP_REQUEST_RETRIES    3

#define DHCP_UDP_SERVER_PORT    67
#define DHCP_UDP_CLIENT_PORT    68

#define ETH_P_IP        0x0800
#define IP_PROTO_UDP   17
#define UIP_LLH_LEN    14u
#define DHCP_TIME_INFINITY      0xFFFFFFFF /* For lease time value */
#define MIN_PORT   0
#define MAX_PORT 24
#define VTSS_PORT_COUNT MAX_PORT
#define SYSTEM_MAC_ADDR    MAX_PORT
#define MAC_LEN         6u
#define VTSS_CHIPS 2                        /* Multiple internal chips */
typedef unsigned char mac_addr_t[MAC_LEN];
typedef uint vtss_port_no_t;

typedef uint vtss_vid_t; /* 0-4095 */
/* MAC Address */
typedef struct _vtss_mac_t {
    uchar addr[6]; /* Network byte order */
} vtss_mac_t;

typedef struct _vtss_io_state_t {
    unsigned char hwaddr[6];
    unsigned int ipaddr;
    unsigned int ipmask;
} vtss_io_state_t;

typedef struct _vtss_state_t {
    vtss_io_state_t io;               /* I/O layer state information */
    void            *al;
    void            *phy;
} vtss_state_t;

struct dhcp_packet {
    uchar      op;                  /* 0: Message opcode/type */
    uchar      htype;               /* 1: Hardware addr type (net/if_types.h) */
    uchar      hlen;                /* 2: Hardware addr length */
    uchar      hops;                /* 3: Number of relay agent hops from client */
    uint32     xid;                 /* 4: Transaction ID */
    uint16     secs;                /* 8: Seconds since client started looking */
    uint16     flags;               /* 10: Flag bits */
    dhcp_ipaddr_t  ciaddr;         /* 12: Client IP address (if already in use) */
    dhcp_ipaddr_t  yiaddr;         /* 16: Client IP address */
    dhcp_ipaddr_t  siaddr;         /* 20: IP address of next server to talk to */
    dhcp_ipaddr_t  giaddr;         /* 24: DHCP relay agent IP address */
    uchar      chaddr[16];          /* 28: Client hardware address */
    char sname[DHCP_SNAME_LEN];    /* 44: Server name */
    char file[DHCP_FILE_LEN];      /* 108: Boot filename */
    uchar options[DHCP_OPTION_LEN]; /* 236: Optional parameters
                                           (actual length dependent on MTU). */
};
typedef struct port_dhcpinfoex
{
    uint32 gipaddr;
    ip_addr_t srcip;
    uchar srcmac[MAC_LEN];
    ip_addr_t cip;
    uchar cmac[MAC_LEN];
} PORT_DHCPINFO;

typedef enum {
    VTSS_LINKEVENT_UP,  /* Link Went Up */
    VTSS_LINKEVENT_DOWN  /* Link Went Down */
} vtss_linkevent_t;

/*
typedef struct _udp_pseudo_hd{
    ip_addr_t src_ip;
    ip_addr_t dst_ip;
    uchar pad;
    uchar proto;
    uint16 len;
}udp_pseudo_hdr_t;
*/
/* BOOTP (rfc951) message types */
#define BOOTREQUEST                     1
#define BOOTREPLY                       2

/* Possible values for flags field... */
#define BOOTP_BROADCAST                 0x8000

/* Possible values for hardware type (htype) field... */
#define HTYPE_ETHER                     1 /* Ethernet 10Mbps */

/* Magic cookie validating dhcp options field (and bootp vendor
   extensions field). */
#define DHCP_OPTIONS_COOKIE             "\143\202\123\143"

/* Offsets for the options field */
#define DHCP_OPOFFS_TAG                 0 /* Offset for TAG field */
#define DHCP_OPOFFS_LEN                 1 /* Offset for LEN field */
#define DHCP_OPOFFS_DATA                2 /* Offset for DATA field */

/* DHCP Option codes: */

#define DHO_PAD                         0
#define DHO_SUBNET_MASK                 1
#define DHO_TIME_OFFSET                 2
#define DHO_ROUTERS                     3
#define DHO_TIME_SERVERS                4
#define DHO_NAME_SERVERS                5
#define DHO_DOMAIN_NAME_SERVERS         6
#define DHO_LOG_SERVERS                 7
#define DHO_COOKIE_SERVERS              8
#define DHO_LPR_SERVERS                 9
#define DHO_IMPRESS_SERVERS             10
#define DHO_RESOURCE_LOCATION_SERVERS   11
#define DHO_HOST_NAME                   12
#define DHO_BOOT_SIZE                   13
#define DHO_MERIT_DUMP                  14
#define DHO_DOMAIN_NAME                 15
#define DHO_SWAP_SERVER                 16
#define DHO_ROOT_PATH                   17
#define DHO_EXTENSIONS_PATH             18
#define DHO_IP_FORWARDING               19
#define DHO_NON_LOCAL_SOURCE_ROUTING    20
#define DHO_POLICY_FILTER               21
#define DHO_MAX_DGRAM_REASSEMBLY        22
#define DHO_DEFAULT_IP_TTL              23
#define DHO_PATH_MTU_AGING_TIMEOUT      24
#define DHO_PATH_MTU_PLATEAU_TABLE      25
#define DHO_INTERFACE_MTU               26
#define DHO_ALL_SUBNETS_LOCAL           27
#define DHO_BROADCAST_ADDRESS           28
#define DHO_PERFORM_MASK_DISCOVERY      29
#define DHO_MASK_SUPPLIER               30
#define DHO_ROUTER_DISCOVERY            31
#define DHO_ROUTER_SOLICITATION_ADDRESS 32
#define DHO_STATIC_ROUTES               33
#define DHO_TRAILER_ENCAPSULATION       34
#define DHO_ARP_CACHE_TIMEOUT           35
#define DHO_IEEE802_3_ENCAPSULATION     36
#define DHO_DEFAULT_TCP_TTL             37
#define DHO_TCP_KEEPALIVE_INTERVAL      38
#define DHO_TCP_KEEPALIVE_GARBAGE       39
#define DHO_NIS_DOMAIN                  40
#define DHO_NIS_SERVERS                 41
#define DHO_NTP_SERVERS                 42
#define DHO_VENDOR_ENCAPSULATED_OPTIONS 43
#define DHO_NETBIOS_NAME_SERVERS        44
#define DHO_NETBIOS_DD_SERVER           45
#define DHO_NETBIOS_NODE_TYPE           46
#define DHO_NETBIOS_SCOPE               47
#define DHO_FONT_SERVERS                48
#define DHO_X_DISPLAY_MANAGER           49
#define DHO_DHCP_REQUESTED_ADDRESS      50
#define DHO_DHCP_LEASE_TIME             51
#define DHO_DHCP_OPTION_OVERLOAD        52
#define DHO_DHCP_MESSAGE_TYPE           53
#define DHO_DHCP_SERVER_IDENTIFIER      54
#define DHO_DHCP_PARAMETER_REQUEST_LIST 55
#define DHO_DHCP_MESSAGE                56
#define DHO_DHCP_MAX_MESSAGE_SIZE       57
#define DHO_DHCP_RENEWAL_TIME           58
#define DHO_DHCP_REBINDING_TIME         59
#define DHO_VENDOR_CLASS_IDENTIFIER     60
#define DHO_DHCP_CLIENT_IDENTIFIER      61
#define DHO_NWIP_DOMAIN_NAME            62
#define DHO_NWIP_SUBOPTIONS             63
#define DHO_TFTP_SERVER_NAME            66
#define DHO_TFTP_BOOT_FILENAME          67
#define DHO_USER_CLASS                  77
#define DHO_FQDN                        81
#define DHO_DHCP_AGENT_OPTIONS          82
#define DHO_SUBNET_SELECTION            118 /* RFC3011! */

#define DHO_END                         255
/*jyshen*/
#define MAX_REMOTEID_LEN	32
#define MAX_REMOTEID_LEN	32
#define DHCP_OFFSET	42

#define CIRCIUIT	1
#define REMOTEID 2

#define OPT82_LEN 11

typedef struct _agent_info_option {
	uchar 	OptType;
	uchar	Optlen;
	
	uchar	SubCirType;
	uchar	SubCirLen;
	uchar	SubCirVal[MAC_LEN];
	#if 1
	uchar	SubRIDType;
	uchar	SubRIDLen;
	uchar	SubRIDVal;
	uchar	SubRIDVal2[MAC_LEN];
	#endif
} AGENT_INFO_OPTION; 


/*end jyshen*/
/* DHCP message types. */
#define DHCPBOOTP       0       /* Means no DHCP message type -> must be BOOTP */
#define DHCPDISCOVER    1
#define DHCPOFFER       2
#define DHCPREQUEST     3
#define DHCPDECLINE     4
#define DHCPACK         5
#define DHCPNAK         6
#define DHCPRELEASE     7
#define DHCPINFORM      8

/* Relay Agent Information option subtypes: */
#define RAI_CIRCUIT_ID  1
#define RAI_REMOTE_ID   2
#define RAI_AGENT_ID    3

/* FQDN suboptions: */
#define FQDN_NO_CLIENT_UPDATE           1
#define FQDN_SERVER_UPDATE              2
#define FQDN_ENCODED                    3
#define FQDN_RCODE1                     4
#define FQDN_RCODE2                     5
#define FQDN_HOSTNAME                   6
#define FQDN_DOMAINNAME                 7
#define FQDN_FQDN                       8
#define FQDN_SUBOPTION_COUNT            8

/* DHCP State values */
#define DHCPSTATE_INITREBOOT            0
#define DHCPSTATE_INIT                  1
#define DHCPSTATE_REBOOTING             2
#define DHCPSTATE_SELECTING             3
#define DHCPSTATE_REQUESTING            4
#define DHCPSTATE_REBINDING             5
#define DHCPSTATE_BOUND                 6
#define DHCPSTATE_RENEWING              7

struct dhcp_interface {
    uchar dhcp_msgtype;
};

typedef struct {
    ip_addr_t ip_addr;
    ip_addr_t subnet_mask;
    ip_addr_t gateway;
} vtss_ip_config_t;

typedef struct {
    vtss_ip_config_t ipconf;
    ip_addr_t server;
    char server_name[DHCP_SNAME_LEN];
    uint32 lease;
    uint32 lease_time_left;
    uint32 t1_time_left;
    uint32 t2_time_left;
    uint32 halflife_time_left;
    uchar dhcpstate;
} vtss_dhcpclient_status_t;


extern int32 dhcpport_receive(system_frame_header_t* header);
#endif /* __DHCPPORT_H__ */
