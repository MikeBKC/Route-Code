
/*
 *netbios name 处理程序
 *
 *
 *KeHuatao
 *2011-09-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <sw_ioctl.h>
#include <switch.h>
#include <packet.h>
#include "host_name.h"

#define ETH_PROTOCOL        ETH_P_IP
#define TIMEOUT    100000u /*us*/

extern void netbios_parse(system_frame_header_t* header, uchar* frame);

static int _fd=-1;
static int recv_packet(int fd, char *frame, int max_length, unsigned long msec);
static void paket_process(char *frame, int len);

int main(void)
{
    int len;
    char frame[MAX_FRAME_LENGTH+1];

    if(fopen(NETBIOS_FILE, "w")==NULL)
    {
	return -1;
    }
    _fd = create_raw_socket(ETH_PROTOCOL);
    if(_fd<0)
    {
	printf("error\n");
    }
    else
    {
	while(1)
	{
	    memset(frame, 0, sizeof(frame));
	    len = recv_packet(_fd, frame, MAX_FRAME_LENGTH, 0);
	    if(len>0)
	    {
		paket_process(frame, len);
	    }
	    else
	    {
		if(len<0)
		{
		    perror("recv error");
		}
	    }

	    renew_host_name_table();
	    renew_host_name_table_timeout();
	    usleep(TIMEOUT);
	}
    }

    return 0;
}

static int recv_packet(int fd, char *frame, int max_length, unsigned long msec)
{
    int n=0;
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timerclear(&tv);
    tv.tv_usec = msec;
    n = select(fd + 1, &readfds, NULL, NULL, &tv);
    if (n > 0) {
	n = recv(fd, frame, max_length, 0);
    }

    return n;
}

static void paket_process(char *frame, int len)
{
    char *pd=NULL;
    int pull_len=0;
    unsigned short sport, dport;
    ip_udp_hdr_t *pkt;
    system_frame_header_t header;

    /*remove the ether header*/
    memset(&header, 0, sizeof(system_frame_header_t));
    pull_len = sw_frame_header_pull(frame, &pd, &header);
    header.length = len - pull_len;

    pkt = (ip_udp_hdr_t*)(pd+14);
    if(pkt -> proto == IP_PROTO_UDP)
    {
	sport = ntohs(pkt->srcport);
	dport = ntohs(pkt->destport);

	if( (sport==NETBIOS_NAME_SERVICE_UDP_PORT)
		&&(sport==NETBIOS_NAME_SERVICE_UDP_PORT)
	  )
	{
	    netbios_parse(&header, pd);
	}
    }
}

uint16 chk_sum(uint16 *buffer, uint32 size)
{
    ulong cksum;

    cksum = 0;

    while (size > 1)
    {
	cksum += *buffer++;
	size -= sizeof(ushort);        
    }

    if (size != 0)
    {
#if 0
	cksum += *(uchar*)buffer <<8; //考虑到大小端模式，要移位
#else
	cksum += *(uchar*)buffer;      
#endif
    }

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (uint16)cksum;
}

/*
 *计算udp头部校验和           
 *KeHuatao                    
 * 
 **/    
uint16 uip_udpchksum(ip_udp_hdr_t* packet)
{
    uint32 cksum;
    udp_pseudo_hdr_t udp_pseudo_header;

    /*构造udp伪首部*/         
    udp_pseudo_header.src_ip = packet->srcipaddr;/*源ip*/
    udp_pseudo_header.dst_ip = packet->destipaddr;/*目的ip*/
    udp_pseudo_header.pad = 0u;    
    udp_pseudo_header.proto = IP_PROTO_UDP;/*17*/
    udp_pseudo_header.len = packet->udplen;/*udp长度*/

    /*udp数据部分校验和*/
    cksum = chk_sum((uint16*)(&((uchar*)packet)[IP_HEADER_LEN]), ntohs(packet->udplen));

    /*加伪首部校验和*/
    cksum = cksum + chk_sum((uint16*)(&udp_pseudo_header), sizeof(udp_pseudo_hdr_t));

    /*调整数值*/
    cksum = (cksum >> 16u) + (cksum & 0xffffu);
    cksum += (cksum >>16u);

    return  (uint16)cksum;
}

