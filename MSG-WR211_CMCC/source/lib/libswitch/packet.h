#ifndef __PACKET_H__
#define __PACKET_H__

#include <linux/switch/types.h>


#define ARP_REQUEST 1
#define ARP_REPLY   2 

#define ALL_PORT_MASK  0xffffffu
#define IP_HEADER_LEN            20u
#define IP_PROTO_TCP   6
#define IP_PROTO_UDP   17
#define ETH_LEN        12u
#define UIP_LLH_LEN    14u
#define DHCP_UDP_SERVER_PORT    67
#define DHCP_UDP_CLIENT_PORT    68

#define CHIP_VLAN_OFFSET        12
#define MAX_FRAME_LENGTH         1518u

#define NETBIOS_NAME_SERVICE_UDP_PORT 137

typedef struct _eth_hdr{
    uchar   dst_addr[MAC_LEN];
    uchar   src_addr[MAC_LEN];
    uint16  protocol;
}eth_hdr_t;

typedef struct _arp_hdr{
    eth_hdr_t eth_hdr;
    uint16  hwtype;
    uint16  protocol;
    uchar  hwlen;
    uchar  protolen;
    uint16  opcode;
    uchar  shwaddr[MAC_LEN];
    uint16  sipaddr[2];
    uchar  dhwaddr[MAC_LEN];
    uint16  dipaddr[2];
}arp_hdr_t;

typedef struct _ip_hdr{
#if 0
    uchar  dest[MAC_LEN];
    uchar  src[MAC_LEN];
    uint16 type;		
#endif
    /* IP header. */
    uchar  vhl;
    uchar  tos;       
    uint16 len;
    uint16 ipid;
    uint16 ipoffset;  
    uchar  ttl;
    uchar  proto;     
    uint16 ipchksum;
    ip_addr_t srcipaddr;
    ip_addr_t destipaddr;
}ip_hdr_t;

typedef struct _ip_udp_hdr{
#if 0
    uchar  dest[MAC_LEN];
    uchar  src[MAC_LEN];
    uint16 type;		
#endif
    /* IP header. */
    uchar  vhl;
    uchar  tos;       
    uint16 len;
    uint16 ipid;
    uint16 ipoffset;  
    uchar  ttl;
    uchar  proto;     
    uint16 ipchksum;
    ip_addr_t srcipaddr;
    ip_addr_t destipaddr;

    /* UDP header. */
    uint16 srcport;
    uint16 destport;
    uint16 udplen;
    uint16 udpchksum;
}ip_udp_hdr_t;

typedef struct _udp_pseudo_hd{
    ip_addr_t src_ip;
    ip_addr_t dst_ip;
    uchar pad;
    uchar proto;
    uint16 len;
}udp_pseudo_hdr_t;

#endif
