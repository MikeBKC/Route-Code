#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> /*inet_addr*/
#include <linux/if_ether.h>
#include <sys/sysinfo.h>
#include <sys/sysinfo.h>
#include <linux/switch/swioc_common.h>

#include "packet.h"
#include "../urcp/common.h"
#include "netbios.h"
#include "host_name.h"

#define NAME_QUERY_REQUEST_PACKET_LENGTH 92u

ulong get_uptime(void);
uint16 uip_udpchksum(ip_udp_hdr_t* packet);

static uint16 _netbios_transid = 0u;
/********************************************
 * 函数：first_level_encoding
 * 功能：将netbios名字换成字符串，并返回名字类型
 * 参数：略
 * 返回：名字类型
 * 作者：jfgu
 * 日期：2011-04-07
 * ********************************************/
static void first_level_encoding(uchar str[], uint32 len, uchar namestr[])
{
    uchar tmp[NETBIOS_PACKET_NAME_LENGTH];
    uint32 i = 0u;
    uint32 j = 0u;

    memset(tmp, 0, NETBIOS_PACKET_NAME_LENGTH);

    /*the first byte is 0x20*/
    if (str[0] == 0x20)
    {
	memcpy(tmp, &(str[1]), NETBIOS_PACKET_NAME_LENGTH - 2u);
	len = len - 2u;
    }
    
    /*convert to string*/
    for (i = 0; i < len; i = i + 2u)
    {
	/*more than 0x41*/	
	if ((tmp[i] < 0x41) || ((tmp[i+1]) < 0x41))
	{
	    break;	
	}

		
	if (i < len - 2u)
	{
	    namestr[j] = ((tmp[i] - 0x41u) << 4) + (tmp[i + 1u] - 0x41u);	
	
	    if (namestr[j] == 0x20)
	    {
		namestr[j] = 0u;	
	    }
	}

	j++;
    }

    return;

}

/************************************************************
 * 函数：netbios_name_data
 * 功能：解析netbios数据包数据部分
 * 参数：包指针，长度，mac地址，包类型
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-07
 * *********************************************************/
static void netbios_name_data(netbios_header *frame, uint32 length, uchar mac_addr[], uint32 opcode)
{
    netbios_question *nbq = NULL;
    char q_name[NETBIOS_PACKET_NAME_LENGTH];
    netbios_resource *nbr = NULL; 
    uint16 rr_name = 0u;
    char namestr[16];
    uint16 rdlen = 0u;
    uint32 g_value = 0u;
    uint32 ont_value = 0u;
    uint16 rr_flag = 0u;
    
    /*init array*/
    memset(namestr, 0, 16);
    memset(q_name, 0, NETBIOS_PACKET_NAME_LENGTH);
    
    /*overwrite & regist packet*/
    nbq = (netbios_question *)((uchar *)frame + sizeof(netbios_header));
    
    memcpy(q_name, nbq->question_name, NETBIOS_PACKET_NAME_LENGTH);
    
    /*name type is not 0x00*/
    if ((q_name[NETBIOS_PACKET_NAME_LENGTH - 3] != 0x41)
	    || (q_name[NETBIOS_PACKET_NAME_LENGTH - 2] != 0x41))
    {
	return;
    }
    
    /*type error*/ 
    if (ntohs(nbq->question_type) != 0x0020u)
    {
	return;
    }

    /*class error*/
    if (ntohs(nbq->question_class) != 0x0001u)
    {
	return;
    }
   
    /*rr name*/
    rr_name = ntohs(*(uint16 *)((uchar *)nbq + sizeof(netbios_question)));
    
    /*additional*/
    if (((rr_name >> 14) & 0x03u) == 0x03u)
    {
	if ((rr_name & 0x3FFFu) == 0x000Cu)
	{
	    /*same with queries name*/
	    first_level_encoding(q_name, NETBIOS_PACKET_NAME_LENGTH, namestr);
	}
	else
	{
	    return;
	}
    }

    nbr = (netbios_resource *)((uchar *)nbq + 2u + sizeof(netbios_question));
    
    /*rr type error*/
    if (ntohs(nbr->rr_type) != 0x0020u)
    {
	return;
    }

    /*rr class error*/
    if (ntohs(nbr->rr_class) != 0x0001u)
    {
	return;
    }
    
    /*rdlen*/
    rdlen = ntohs(nbr->rdlength);

    if (rdlen != 0x0006u)
    {
	return;
    }
    
    /*flag*/
    rr_flag = ntohs(nbr->rdata_flags);

    /*g's value*/
    g_value = (rr_flag >> 15) & 0x01u;

    /*one value*/
    ont_value = ((rr_flag << 1) >> 12) & 0x03u;

    if (rr_flag != 0x0u)
    {
	return;
    }

    if (g_value == 0x0u)
    {
	/*unique name*/
	if (ont_value == 0x0u)
	{
	    /*b node*/
	    if (opcode == OPCODE_REGIST)
	    {
		/*记录名字 ip mac*/
		add_host_name(mac_addr, namestr, ntohl(nbr->rdata_address), REGIST_ADD);
	    }

	    if (opcode == OPCODE_RELEASE)
	    {
		/*DELETE IP MAC NAME*/
	    }
	}
    }
    return;

}


/********************************************************
 * 函数：netbios_name_header
 * 功能：解析netbios数据包包头
 * 参数：数据包指针，长度，mac地址
 * 返回：略
 * 作者：jfgu
 * 日期：2011-04-07
 * ******************************************************/
static void netbios_name_header(netbios_header *frame, uint32 length, uchar mac_addr[])
{
    
    uint32 nm_flag = 0u;
    uint32 r_opcode = 0u;
    uint32 nm_aa = 0u;
    uint32 nm_tc = 0u;
    uint32 nm_rd = 0u;
    uint32 nm_ra = 0u;
    uint32 opcode = 0u; 


    /*opcode*/
    opcode = ((((uint32)(ntohs(frame->flags))) << 1u) >> 12u) & 0x0000000fu;

    /*opcode - R*/
    r_opcode = ((((uint32)(ntohs(frame->flags))) >> 16u) & 0x00000001u);

    /*respone*/
    if (r_opcode == 0x01u)
    {
	return;	
    }

    /*nm*/
    nm_flag = ((((uint32)(ntohs(frame->flags))) << 5u) >> 9u) & 0x0000007fu;
    if ((nm_flag & 0x01u) != 0x01u)
    {
	/*unicast*/
	return;
    }
    
    /*aa*/
    nm_aa = (nm_flag >> 6u) & 0x01u;
    nm_tc = ((nm_flag << 1u) >> 6u) & 0x01u;  /*tc*/
    nm_rd = ((nm_flag << 2u) >> 6u) & 0x01u;  /*td*/
    nm_ra = ((nm_flag << 3u) >> 6u) & 0x01u;	/*ra*/
    
    /*packet type*/
    if ((nm_aa == 0u) && (nm_tc == 0u) && (nm_rd == 0u) && (nm_ra == 0u))\
    {
	if ((ntohs(frame->qdcount) == 1u)
		&& (ntohs(frame->ancount) == 0u)
		&& (ntohs(frame->nscount) == 0u)
		&& (ntohs(frame->arcount) == 1u))
	{

    	    switch(opcode)
	    {
		case OPCODE_QUERY:
		    break;
		case OPCODE_REGIST:
		    netbios_name_data(frame, length, mac_addr, OPCODE_REGIST);
		    break;
		case OPCODE_RELEASE:
		    netbios_name_data(frame, length, mac_addr, OPCODE_RELEASE);
		    break;
		case OPCODE_WACK:
		case OPCODE_REFRESH:
		    break;
		default:
		    break;
	    }
	}
 
    }

    return;
}

/************************************************************
 * 函数：netbios_query_response_parse
 * 功能：解析query包的response包
 * 参数：包头 长度 mac ip
 * 返回：
 * 无
 * 作者：jfgu
 * 日期：2011-04-15
 * *********************************************************/
static void netbios_query_response_parse(netbios_header *frame, uint32 length, uchar mac_addr[], uint32 ipaddr)
{
    netbios_query_response *nqr = NULL;
    uchar q_name[NETBIOS_PACKET_NAME_LENGTH];
    uchar namestr[16];
    uint16 alen = 0u;

    /*init array*/
    memset(namestr, 0, 16);
    memset(q_name, 0, NETBIOS_PACKET_NAME_LENGTH);
   
    /*not response*/
    if ((ntohs(frame->qdcount) != 0u) || (ntohs(frame->ancount) != 0x0001u) 
	    || (ntohs(frame->nscount) != 0u) || (ntohs(frame->arcount) != 0u))
    {
	return;
    }

    nqr = (netbios_query_response *)((uchar *)frame + sizeof(netbios_header));
   
    memcpy(q_name, nqr->rr_name, NETBIOS_PACKET_NAME_LENGTH); 
  
    /*name type error*/
    if ((q_name[31] != 0x41) || (q_name[32] != 0x41))
    {
	return;
    }
    
    /*change to string*/
    first_level_encoding(q_name, NETBIOS_PACKET_NAME_LENGTH, namestr);
    
    
    /*rrtype or class error*/
    if ((ntohs(nqr->rr_type) != 0x0020u) || (ntohs(nqr->rr_class) != 0x0001u))
    {
	return;
    }

    /*len*/
    alen = ntohs(nqr->rdlen);
    

    if (length - sizeof(netbios_header) - sizeof(netbios_query_response) != alen)
    {
	return;
    }
    
    if ((alen % 6) == 0)
    {
	add_host_name(mac_addr, namestr, ipaddr, QUERY_ADD);
	return;
    }

    return;


}


/*************************************************************************
 * 函数：netbios_parse
 * 功能：netbios解析数据包入口函数
 * 参数：略
 * 返回：略
 * 作者：jfgu
 * 日期: 2011-04-07
 * ***********************************************************************/
extern void netbios_parse(system_frame_header_t* header, uchar* frame)
{
    netbios_header *nbh = NULL;
    uchar mac_addr[MAC_LEN];
    ip_hdr_t* ip_packet = NULL;
    uint32 src_ip_addr = 0u;

#if 0
#if CONFIG_VSC7390
    uchar if_name[32];
    ip_addr_t ip;
#endif
#endif

    ip_packet = (ip_hdr_t*)(&frame[UIP_LLH_LEN]);
    src_ip_addr = htonl(ip_packet->srcipaddr);

    memset(mac_addr, 0, MAC_LEN);

    memcpy(mac_addr, ((eth_hdr_t*)frame)->src_addr, MAC_LEN);

    nbh = (netbios_header *)(frame + 42u);

    if ((ntohs(nbh->flags)) == 0x8500)
    {

#if 1
	/*调试代码*/
	char test_buff[32];
	memset(test_buff,0,sizeof(test_buff));

	memset(test_buff,0,sizeof(test_buff));
#endif

	/*query response*/
	netbios_query_response_parse(nbh, header->length - 42u, mac_addr, src_ip_addr);
    }
    else
    {
	netbios_name_header(nbh, header->length - 42u, mac_addr);
    }

#if 0
#ifdef CONFIG_VSC7390
    get_switch_if_name(if_name);
    get_ip(if_name, &ip);
    if(ip_packet->destipaddr != ip)
    {
	/*转发收到的数据包*/
        h2_fwd_frame(frame, header->length, header->tag.vid, header->source_port_no);
    }
#endif
#endif

    return;
}


/*******************************************************
 * 函数：first_level_convert
 * 功能：将字符串换成fisrt level格式
 * 参数：字符串，返回的字符串
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-07
 * ******************************************************/
static void first_level_convert(char *str, char ret[])
{
    uint32 len = strlen(str);
    uchar *tmp = NULL;
    uint32 i;
    uint32 j = 0u;
    tmp = &(ret[1]);

    ret[0] = 0x20;

     
    for (i = 0; i < 32; i = i + 2)
    {
	
	if ((str[j] != '\0') && (len >= (i + 2) / 2))
	{
	    tmp[i] = ((str[j] >> 4) & 0x0f) + 0x41;
	    tmp[i + 1] = (str[j] & 0x0f) + 0x41;
	}
	else
	{
	    tmp[i] = 0x43;
	    tmp[i+1] = 0x41;
	}

	if (i == 30)
	{
	    tmp[i] = tmp[i+1] = 0x41;
	}


	j++;

    }

    return;

}


/*************************************************
 * 函数：netbios_query
 * 功能：发送query数据包
 * 参数：名字字符串
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-07
 * **********************************************/
extern void netbios_query(char *host_name)
{
    uchar if_name[32], macaddr[MAC_LEN], nqr_buf[NAME_QUERY_REQUEST_PACKET_LENGTH];
    uint32 buf_len = 0u;
    ip_addr_t ip, mask;
    ip_udp_hdr_t *udpbuf = NULL;
    eth_hdr_t *eth_header = NULL;
    netbios_query_request *nqr = NULL;
    uchar q_name[NETBIOS_PACKET_NAME_LENGTH];
    
    uchar bcast_mac[MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    memset(macaddr, 0, sizeof(macaddr));
    memset(if_name, 0, sizeof(if_name));
    memset(q_name, 0, NETBIOS_PACKET_NAME_LENGTH);
    memset(nqr_buf, 0, NAME_QUERY_REQUEST_PACKET_LENGTH);
    buf_len = NAME_QUERY_REQUEST_PACKET_LENGTH;
    get_switch_if_name(if_name);
    get_mac(if_name, macaddr);
    
    /*mac帧头*/
    eth_header = (eth_hdr_t *)nqr_buf;
    memcpy(eth_header->src_addr, macaddr, MAC_LEN);
    memcpy(eth_header->dst_addr, bcast_mac, MAC_LEN);
    eth_header->protocol = htons(0x0800);

    /*ip帧头*/
    udpbuf = (ip_udp_hdr_t *)((uchar *)nqr_buf + sizeof(eth_hdr_t));
    udpbuf->vhl = 0x45;
    udpbuf->tos = 0u;
    udpbuf->len = htons(buf_len - UIP_LLH_LEN);
    _netbios_transid++;
    udpbuf->ipid = htons(_netbios_transid);
    udpbuf->ipoffset = 0u;
    udpbuf->ttl = 128;
    udpbuf->proto = IP_PROTO_UDP;
    get_ip(if_name, &ip);
    get_mask(if_name, &mask);
    udpbuf->srcipaddr = ip;
    udpbuf->destipaddr = ip | (~mask);

    /*udp帧头*/
    udpbuf->srcport = htons(NETBIOS_NAME_SERVICE_UDP_PORT);
    udpbuf->destport = htons(NETBIOS_NAME_SERVICE_UDP_PORT);
    udpbuf->udplen = htons(htons(udpbuf->len) - 20u);

    /*netbios*/
    nqr = (netbios_query_request *)((uchar *)nqr_buf + 42u);
    _netbios_transid++;
    nqr->trans_id = htons(_netbios_transid);
    nqr->flags = htons(0x0110u);
    nqr->qdcount = htons(0x0001u);
    nqr->ancount = htons(0x0000u);
    nqr->nscount = 0u;
    nqr->arcount = 0u;
    first_level_convert(host_name, q_name);
    memcpy(nqr->question_name, q_name, NETBIOS_PACKET_NAME_LENGTH);
    nqr->question_type = htons(0x0020u);
    nqr->question_class = htons(0x0001u);

    /*check sum*/
    udpbuf->udpchksum = 0u;
    udpbuf->udpchksum = ~(uip_udpchksum(udpbuf));
    udpbuf->ipchksum = 0u;
    udpbuf->ipchksum = ~(chk_sum((uint16*)udpbuf, 20));
   
    /*send packet*/
    h2_send_frame(nqr_buf, buf_len);
}

