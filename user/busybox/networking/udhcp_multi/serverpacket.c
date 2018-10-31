/* vi: set sw=4 ts=4: */
/* serverpacket.c
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * Construct and send DHCP server packets
 */
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"

#include "common.h"
#include "dhcpc.h"
#include "dhcpd.h"
#include "options.h"
#include "syslog.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_SYSLOG(pri, fmt, args...)		syslog(pri, fmt, ## args)	
#else
#define SWORD_SYSLOG(pri, fmt, args...)
#endif

#if (DHCP_AUTO_BIND == FYES)
static void DhcpAutoBind(struct dhcpOfferedAddr *oldest);
#endif

#ifdef DHCP_SERVER_MULTIIF
extern struct _dhcp_pool_t *dhcp_pool_ptr;
#endif

/* send a packet to giaddr using the kernel ip stack */
static int send_packet_to_relay(struct dhcpMessage *payload)
{
	DEBUG("Forwarding packet to relay");

	return udhcp_send_kernel_packet(payload, server_config.server, SERVER_PORT,
			payload->giaddr, SERVER_PORT);
}

#if (DHCP_SERVER_MULTIIF == FYES && DHCP_SERVER_OPTION_43 == FYES)
/* get option43 data from /tmp/udhcpdoption43_vidxxxx */
#define FILE_PATH_LEN 30
#define BUFSIZE 256
char option43Data[140];
int read_option43_data(uint32_t vid)
{
    int i = 0, len;
    FILE *fp = NULL;
    char file_path[FILE_PATH_LEN], buf[BUFSIZE];
    char *p;
    
    memset(file_path, 0, sizeof(file_path));
    memset(buf, 0, sizeof(buf));
    memset(option43Data, 0, sizeof(option43Data));
    
    sprintf(file_path, "/tmp/udhcpdoption43_vid%d", vid);
    if((fp = fopen(file_path, "r")) == NULL)
    {
        return 0;
    }
    fgets(buf, sizeof(buf), fp);
    fclose(fp);
    if((len = strlen(buf)) == 0)
    {
        return 0;
    }
#if (CONFIG_BOARD_P1010_C)
    p = buf;
    while (*p)
    {
        option43Data[i] = strtol(p, &p, 16);
        i++;
    }
#else
    for(i = 0; i < len; i += 2)
    {
       sscanf(&buf[i], "%02x%*s", &option43Data[i/2]);
    }
#endif
    return 1;
}
#endif


/* send a packet to a specific arp address and ip address by creating our own ip packet */
#ifdef DHCP_SERVER_MULTIIF
static int send_packet_to_client(struct dhcpMessage *payload, int force_broadcast, uint32_t vid)
#else
static int send_packet_to_client(struct dhcpMessage *payload, int force_broadcast)
#endif
{
	const uint8_t *chaddr;
	uint32_t ciaddr;

	if (force_broadcast) {
		DEBUG("broadcasting packet to client (NAK)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else if (payload->ciaddr) {
		DEBUG("unicasting packet to client ciaddr");
		ciaddr = payload->ciaddr;
		chaddr = payload->chaddr;
	} else if (ntohs(payload->flags) & BROADCAST_FLAG) {
		DEBUG("broadcasting packet to client (requested)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else {
		DEBUG("unicasting packet to client yiaddr");
		ciaddr = payload->yiaddr;
		chaddr = payload->chaddr;
	}
#ifdef DHCP_SERVER_MULTIIF
	return udhcp_send_raw_packet_vid(payload, server_config.server, SERVER_PORT,
			ciaddr, CLIENT_PORT, chaddr, server_config.ifindex, vid);
#else
	return udhcp_send_raw_packet(payload, server_config.server, SERVER_PORT,
			ciaddr, CLIENT_PORT, chaddr, server_config.ifindex);
#endif
}


/* send a dhcp packet, if force broadcast is set, the packet will be broadcast to the client */
#ifdef DHCP_SERVER_MULTIIF
static int send_packet(struct dhcpMessage *payload, int force_broadcast, uint32_t vid)
#else
static int send_packet(struct dhcpMessage *payload, int force_broadcast)
#endif
{
	if (payload->giaddr) {
		dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
		return send_packet_to_relay(payload);
	}
#ifdef DHCP_SERVER_MULTIIF
	return send_packet_to_client(payload, force_broadcast, vid);
#else
	return send_packet_to_client(payload, force_broadcast);
#endif
}


static void init_packet(struct dhcpMessage *packet, struct dhcpMessage *oldpacket, char type)
{
	udhcp_init_header(packet, type);
	packet->xid = oldpacket->xid;
	memcpy(packet->chaddr, oldpacket->chaddr, 16);
	packet->flags = oldpacket->flags;
	packet->giaddr = oldpacket->giaddr;
	packet->ciaddr = oldpacket->ciaddr;
	add_simple_option(packet->options, DHCP_SERVER_ID, server_config.server);
}


/* add in the bootp options */
static void add_bootp_options(struct dhcpMessage *packet)
{
	packet->siaddr = server_config.siaddr;
	if (server_config.sname)
		strncpy((char*)packet->sname, server_config.sname, sizeof(packet->sname) - 1);
	if (server_config.boot_file)
		strncpy((char*)packet->file, server_config.boot_file, sizeof(packet->file) - 1);
}


/* send a DHCP OFFER to a DHCP DISCOVER */
#ifdef DHCP_SERVER_MULTIIF
int send_offer(struct dhcpMessage *oldpacket, uint32_t vid)
#else
int send_offer(struct dhcpMessage *oldpacket)
#endif
{
	struct dhcpMessage packet;
	struct dhcpOfferedAddr *lease = NULL;
#ifdef DHCP_SERVER_MULTIIF
	uint32_t req_align, lease_time_align = dhcp_pool_ptr->lease;
#else
	uint32_t req_align, lease_time_align = server_config.lease;
#endif
	uint8_t *req, *lease_time;
	struct option_set *curr;
	struct in_addr addr;

	uint32_t static_lease_ip;

#ifdef CONFIG_XPORT_LOG
	unsigned int sn = getSystemSn();
#endif
	init_packet(&packet, oldpacket, DHCPOFFER);

#ifdef DHCP_SERVER_MULTIIF
	static_lease_ip = getIpByMac(dhcp_pool_ptr->static_leases, oldpacket->chaddr);
#else
	static_lease_ip = getIpByMac(server_config.static_leases, oldpacket->chaddr);
#endif

	/* ADDME: if static, short circuit */
	if (!static_lease_ip) {
		/* the client is in our lease/offered table */
		lease = find_lease_by_chaddr(oldpacket->chaddr);
		if (lease) {
			if (!lease_expired(lease))
#if 0
                /*edit by brwang*/
				lease_time_align = lease->expires - time(0);
#else
				lease_time_align = lease->expires - utt_getSysUpTime();
#endif
			packet.yiaddr = lease->yiaddr;
		/* Or the client has a requested ip */
		} else if ((req = get_option(oldpacket, DHCP_REQUESTED_IP))
                
		 /* Don't look here (ugly hackish thing to do) */
		 && memcpy(&req_align, req, 4)
		 /* and the ip is in the lease range */
#ifdef DHCP_SERVER_MULTIIF
		 && ntohl(req_align) >= dhcp_pool_ptr->start_ip
		 && ntohl(req_align) <= dhcp_pool_ptr->end_ip
#else
		 && ntohl(req_align) >= server_config.start_ip
		 && ntohl(req_align) <= server_config.end_ip
#endif
		 && !static_lease_ip /* Check that its not a static lease */
		 /* and is not already taken/offered */
#ifdef DHCP_SERVER_MULTIIF
		 && ((reservedIp(dhcp_pool_ptr->static_leases, (req_align)) == 0)
#else
		 && ((reservedIp(server_config.static_leases, (req_align)) == 0)
#endif
			&& ((!(lease = find_lease_by_yiaddr(req_align)))
                                  
			/* or its taken, but expired */ /* ADDME: or maybe in here */
			|| lease_expired(lease)))
		) {
			packet.yiaddr = req_align; /* FIXME: oh my, is there a host using this IP? */
			/* otherwise, find a free IP */
		} else {

			/* Is it a static lease? (No, because find_address skips static lease) */
			packet.yiaddr = find_address(0);
			/* try for an expired lease */
			if (!packet.yiaddr)
			{
				packet.yiaddr = find_address(1);
			}
		}

		if (!packet.yiaddr) {

#ifdef CONFIG_XPORT_LOG
#ifdef DHCP_SERVER_MULTIIF
			//syslog(LOG_ERR, "<132>%u@msglog: DHCPS: no ip in pool:, vid%u",sn,vid);
			syslog(LOG_ERR, "<132>%u@msglog: DHCPIPLACK:DHCP Server IP lack",sn);
#else
			//syslog(LOG_ERR, "<132>%u@msglog: DHCPS: no ip in pool:, default",sn);
			syslog(LOG_ERR, "<132>%u@msglog: DHCPIPLACK:DHCP Server IP lack",sn);
#endif
#endif
			bb_error_msg("no IP addresses to give - OFFER abandoned");
			return -1;
		}
#ifdef DHCP_SERVER_MULTIIF
		if (!add_lease(get_option(oldpacket, DHCP_HOST_NAME), packet.chaddr, packet.yiaddr, server_config.offer_time, dhcp_pool_ptr)) {
#else
		if (!add_lease(get_option(oldpacket, DHCP_HOST_NAME), packet.chaddr, packet.yiaddr, server_config.offer_time)) {
#endif
#ifdef CONFIG_XPORT_LOG
#ifdef DHCP_SERVER_MULTIIF
			//syslog(LOG_ERR, "<132>%u@msglog: DHCPS: no ip in pool:, vid%u",sn,vid);
			syslog(LOG_ERR, "<132>%u@msglog: DHCPIPLACK:DHCP Server IP lack",sn);
#else
			//syslog(LOG_ERR, "<132>%u@msglog: DHCPS: no ip in pool:, default",sn);
			syslog(LOG_ERR, "<132>%u@msglog: DHCPIPLACK:DHCP Server IP lack",sn);
#endif
#endif
            
			bb_error_msg("lease pool is full - OFFER abandoned");
			return -1;
		}
		lease_time = get_option(oldpacket, DHCP_LEASE_TIME);
		if (lease_time) {
			memcpy(&lease_time_align, lease_time, 4);
			lease_time_align = ntohl(lease_time_align);
#ifdef DHCP_SERVER_MULTIIF
			if (lease_time_align > dhcp_pool_ptr->lease)
				lease_time_align = dhcp_pool_ptr->lease;
#else
			if (lease_time_align > server_config.lease)
				lease_time_align = server_config.lease;
#endif
		}

		/* Make sure we aren't just using the lease time from the previous offer */
		if (lease_time_align < server_config.min_lease)
#ifdef DHCP_SERVER_MULTIIF

			lease_time_align = dhcp_pool_ptr->lease;
#else
			lease_time_align = server_config.lease;
#endif
		/* ADDME: end of short circuit */
	} else {

		/* It is a static lease... use it */
		packet.yiaddr = static_lease_ip;
	}

	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));

#ifdef DHCP_SERVER_MULTIIF
	curr = dhcp_pool_ptr->options;
#else
	curr = server_config.options;
#endif
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

#ifdef DHCP_SERVER_MULTIIF
#if (DHCP_SERVER_OPTION_43 == FYES)
    if(read_option43_data(vid)) {
        add_option_string(packet.options, option43Data);
    }
#endif
#else
    if (server_config.option43Data != NULL)
        add_option_string(packet.options, server_config.option43Data);
#endif

	add_bootp_options(&packet);

	addr.s_addr = packet.yiaddr;
	bb_info_msg("Sending OFFER of %s", inet_ntoa(addr));
#ifdef DHCP_SERVER_MULTIIF
	return send_packet(&packet, 0, vid);
#else
	return send_packet(&packet, 0);
#endif
}


#ifdef DHCP_SERVER_MULTIIF
int send_NAK(struct dhcpMessage *oldpacket, uint32_t vid)
#else
int send_NAK(struct dhcpMessage *oldpacket)
#endif
{
	struct dhcpMessage packet;

	init_packet(&packet, oldpacket, DHCPNAK);

	DEBUG("Sending NAK");
#ifdef DHCP_SERVER_MULTIIF
	return send_packet(&packet, 1, vid);
#else
	return send_packet(&packet, 1);
#endif
}


#ifdef DHCP_SERVER_MULTIIF
int send_ACK(struct dhcpMessage *oldpacket, uint32_t yiaddr, uint32_t vid)
#else
int send_ACK(struct dhcpMessage *oldpacket, uint32_t yiaddr)
#endif
{
	struct dhcpMessage packet;
	struct option_set *curr;
	uint8_t *lease_time;
#ifdef DHCP_SERVER_MULTIIF
	uint32_t lease_time_align = dhcp_pool_ptr->lease;
#else
	uint32_t lease_time_align = server_config.lease;
#endif
	struct in_addr addr;
#if (DHCP_AUTO_BIND == FYES)
	struct dhcpOfferedAddr *AutoBindProf;
#endif

	init_packet(&packet, oldpacket, DHCPACK);
	packet.yiaddr = yiaddr;

	lease_time = get_option(oldpacket, DHCP_LEASE_TIME);
	if (lease_time) {
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);
#ifdef DHCP_SERVER_MULTIIF
		if (lease_time_align > dhcp_pool_ptr->lease)
			lease_time_align = dhcp_pool_ptr->lease;
#else
		if (lease_time_align > server_config.lease)
			lease_time_align = server_config.lease;
#endif
		else if (lease_time_align < server_config.min_lease)
#ifdef DHCP_SERVER_MULTIIF
			lease_time_align = dhcp_pool_ptr->lease;
#else
			lease_time_align = server_config.lease;
#endif
	}

	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));

#ifdef DHCP_SERVER_MULTIIF
	curr = dhcp_pool_ptr->options;
#else
	curr = server_config.options;
#endif
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

#ifdef DHCP_SERVER_MULTIIF
#if (DHCP_SERVER_OPTION_43 == FYES)
    if(read_option43_data(vid)) {
        add_option_string(packet.options, option43Data);
    }
#endif
#else
    if (server_config.option43Data != NULL)
        add_option_string(packet.options, server_config.option43Data);
#endif

	add_bootp_options(&packet);

	addr.s_addr = packet.yiaddr;
	bb_info_msg("Sending ACK to %s", inet_ntoa(addr));

#ifdef DHCP_SERVER_MULTIIF
	if (send_packet(&packet, 0, vid) < 0)
#else
	if (send_packet(&packet, 0) < 0)
#endif

		return -1;

#if (DHCP_AUTO_BIND == FYES)
	/*add_lease将ip等信息保存到地址池，返回保存地址*/
#ifdef DHCP_SERVER_MULTIIF
	AutoBindProf = add_lease(get_option(oldpacket, DHCP_HOST_NAME), packet.chaddr, packet.yiaddr, lease_time_align, dhcp_pool_ptr);
#else
	AutoBindProf = add_lease(get_option(oldpacket, DHCP_HOST_NAME), packet.chaddr, packet.yiaddr, lease_time_align);
#endif
	if(AutoBindProf != NULL)
	{/*若地址不为空，即保存成功*/
		DhcpAutoBind(AutoBindProf);
	}
#else
#ifdef DHCP_SERVER_MULTIIF
	add_lease(get_option(oldpacket, DHCP_HOST_NAME), packet.chaddr, packet.yiaddr, lease_time_align, dhcp_pool_ptr);
#else
	add_lease(get_option(oldpacket, DHCP_HOST_NAME), packet.chaddr, packet.yiaddr, lease_time_align);
#endif
#endif
	if (ENABLE_FEATURE_UDHCPD_WRITE_LEASES_EARLY) {
		/* rewrite the file with leases at every new acceptance */
		write_leases();
	}

	return 0;
}

#if (DHCP_AUTO_BIND == FYES)
/********************************************************************
 * 函数： DhcpAutoBind
 * 功能： 自动绑定开启时生成用户名，并保存
 * 创建： 2012-09-12
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void DhcpAutoBind(struct dhcpOfferedAddr *oldest)
{
	MibProfileType mibType = MIB_PROF_USER;
	DhcpServerProfile *profDhcpS = NULL;
	char macStr[13];
	char *userName = NULL;
	char DhcpNameH[6] = "dhcp_"; /*dhcp自动绑定默认用户名前缀*/
	MacAddr macAddr;
	int i = 0;
	int maxDeNameNo = 0;
	profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);
	SWORD_SYSLOG(LOG_NOTICE, "%s------%d, ProfInstIsFree(profDhcpS) = %d\n", __func__, __LINE__, ProfInstIsFree(profDhcpS));
	if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0))
	{/*DHCP的profile存在*/
		if(profDhcpS->DhcpNewAutobind == FUN_ENABLE)
		{/*如果自动绑定开启了*/

			memset(macStr, 0, sizeof(macStr));
			memset(&macAddr, 0, sizeof(macAddr)); 
			for (i = 0; i < 6; i++)
			{
				sprintf(macStr,"%s%02X",macStr, oldest->chaddr[i]);
			}
			SWORD_SYSLOG(LOG_NOTICE, "%s------%d, macStr = %s\n", __func__, __LINE__, macStr);
			converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
			maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, DhcpNameH);/*获取最大默认用户名编号*/
			SWORD_SYSLOG(LOG_NOTICE, "%s------%d, maxDeNameNo = %d\n", __func__, __LINE__, maxDeNameNo);
			userName = ProfBuildDefaultName(DhcpNameH, maxDeNameNo);/*生成默认用户名*/
			SWORD_SYSLOG(LOG_NOTICE, "%s------%d, oldest->yiaddr = %u, macStr = %s, userName = %s\n", __func__, __LINE__, oldest->yiaddr, macStr, userName);
			DhcpArpBindConfig(oldest->yiaddr, macAddr, userName);/*绑定*/
		}
	}
	return;
}
#endif

#ifdef DHCP_SERVER_MULTIIF
int send_inform(struct dhcpMessage *oldpacket, uint32_t vid)
#else
int send_inform(struct dhcpMessage *oldpacket)
#endif
{
	struct dhcpMessage packet;
	struct option_set *curr;

	init_packet(&packet, oldpacket, DHCPACK);

#ifdef DHCP_SERVER_MULTIIF
	curr = dhcp_pool_ptr->options;
#else
	curr = server_config.options;
#endif
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

	add_bootp_options(&packet);

#ifdef DHCP_SERVER_MULTIIF
	return send_packet(&packet, 0, vid);
#else
	return send_packet(&packet, 0);
#endif
}
