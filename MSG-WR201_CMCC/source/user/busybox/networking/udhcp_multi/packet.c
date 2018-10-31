/* vi: set sw=4 ts=4: */

#include <netinet/in.h>
#if (defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1) || defined _NEWLIB_VERSION
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif

#include "common.h"
#include "dhcpd.h"
#include "options.h"

#ifdef DHCP_SERVER_MULTIIF
extern uint8_t mac[MAC_LEN];
#endif

void udhcp_init_header(struct dhcpMessage *packet, char type)
{
	memset(packet, 0, sizeof(struct dhcpMessage));
	packet->op = BOOTREQUEST;
	switch (type) {
	case DHCPOFFER:
	case DHCPACK:
	case DHCPNAK:
		packet->op = BOOTREPLY;
	}
	packet->htype = ETH_10MB;
	packet->hlen = ETH_10MB_LEN;
	packet->cookie = htonl(DHCP_MAGIC);
	packet->options[0] = DHCP_END;
	add_simple_option(packet->options, DHCP_MESSAGE_TYPE, type);
}


/* read a packet from socket fd, return -1 on read error, -2 on packet error */
#ifdef DHCP_SERVER_MULTIIF
int udhcp_recv_kernel_packet(int port, uint32_t *vid, struct dhcpMessage *packet, int fd)
#else
int udhcp_recv_kernel_packet(struct dhcpMessage *packet, int fd)
#endif
{
	int bytes;
	unsigned char *vendor;


#ifdef DHCP_SERVER_MULTIIF
	char frame[MAX_FRAME_LENGTH + 1];
	char *data;
	struct udp_dhcp_packet *buf;
	tagInfo_t	*pk_tag;

	memset(frame, 0, sizeof(frame));

	bytes = safe_read(fd, frame, sizeof(frame));
	if (bytes < 0) {
		DEBUG("cannot read on listening socket, ignoring");
		return bytes; /* returns -1 */
	}

	if (0 == memcmp(frame + MAC_LEN, mac, MAC_LEN)) {
		return -2;
	}

	data = frame + 2 * MAC_LEN;
	pk_tag = (tagInfo_t *)data;
	if(pk_tag->type == htons(ETH_P_8021Q))
	{
		*vid = ntohs(pk_tag->value) & 0xfff;

		buf = (struct udp_dhcp_packet *)(data + sizeof(tagInfo_t) + sizeof(uint16_t));
	}
	else {
		*vid = 0u;

		buf = (struct udp_dhcp_packet *)(data + sizeof(uint16_t));
	}

	if (htons(port) != buf->udp.dest) {
		return -2;
	}

	memcpy(packet, &(buf->data), sizeof(struct dhcpMessage));

#else
	memset(packet, 0, sizeof(*packet));
	bytes = safe_read(fd, packet, sizeof(*packet));
	if (bytes < 0) {
		DEBUG("cannot read on listening socket, ignoring");
		return bytes; /* returns -1 */
	}
#endif

	if (packet->cookie != htonl(DHCP_MAGIC)) {
		bb_error_msg("received bogus message, ignoring");
		return -2;
	}
	DEBUG("Received a packet");

	if (packet->op == BOOTREQUEST) {
		vendor = get_option(packet, DHCP_VENDOR);
		if (vendor) {
#if 0
			static const char broken_vendors[][8] = {
				"MSFT 98",
				""
			};
			int i;
			for (i = 0; broken_vendors[i][0]; i++) {
				if (vendor[OPT_LEN - 2] == (uint8_t)strlen(broken_vendors[i])
						&& !strncmp((char*)vendor, broken_vendors[i], vendor[OPT_LEN - 2])
				   ) {
					DEBUG("broken client (%s), forcing broadcast",
							broken_vendors[i]);
					packet->flags |= htons(BROADCAST_FLAG);
				}
			}
#else
			if (vendor[OPT_LEN - 2] == (uint8_t)(sizeof("MSFT 98")-1)
					&& memcmp(vendor, "MSFT 98", sizeof("MSFT 98")-1) == 0
			   ) {
				DEBUG("broken client (%s), forcing broadcast", "MSFT 98");
				packet->flags |= htons(BROADCAST_FLAG);
			}
#endif
		}
	}

	return bytes;
}

int udhcp_recv_kernel_packet_client(struct dhcpMessage *packet, int fd)
{
	int bytes;
	unsigned char *vendor;

	memset(packet, 0, sizeof(*packet));
	bytes = safe_read(fd, packet, sizeof(*packet));
	if (bytes < 0) {
		DEBUG("cannot read on listening socket, ignoring");
		return bytes; /* returns -1 */
	}

	if (packet->cookie != htonl(DHCP_MAGIC)) {
		bb_error_msg("received bogus message, ignoring");
		return -2;
	}
	DEBUG("Received a packet");

	if (packet->op == BOOTREQUEST) {
		vendor = get_option(packet, DHCP_VENDOR);
		if (vendor) {
#if 0
			static const char broken_vendors[][8] = {
				"MSFT 98",
				""
			};
			int i;
			for (i = 0; broken_vendors[i][0]; i++) {
				if (vendor[OPT_LEN - 2] == (uint8_t)strlen(broken_vendors[i])
				 && !strncmp((char*)vendor, broken_vendors[i], vendor[OPT_LEN - 2])
				) {
					DEBUG("broken client (%s), forcing broadcast",
						broken_vendors[i]);
					packet->flags |= htons(BROADCAST_FLAG);
				}
			}
#else
			if (vendor[OPT_LEN - 2] == (uint8_t)(sizeof("MSFT 98")-1)
			 && memcmp(vendor, "MSFT 98", sizeof("MSFT 98")-1) == 0
			) {
				DEBUG("broken client (%s), forcing broadcast", "MSFT 98");
				packet->flags |= htons(BROADCAST_FLAG);
			}
#endif
		}
	}

	return bytes;
}

uint16_t udhcp_checksum(void *addr, int count)
{
	/* Compute Internet Checksum for "count" bytes
	 *         beginning at location "addr".
	 */
	int32_t sum = 0;
	uint16_t *source = (uint16_t *) addr;

	while (count > 1)  {
		/*  This is the inner loop */
		sum += *source++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if (count > 0) {
		/* Make sure that the left-over byte is added correctly both
		 * with little and big endian hosts */
		uint16_t tmp = 0;
		*(uint8_t*)&tmp = *(uint8_t*)source;
		sum += tmp;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}


/* Construct a ip/udp header for a packet, send packet */
#ifdef DHCP_SERVER_MULTIIF
int udhcp_send_raw_packet_vid(struct dhcpMessage *payload,
		uint32_t source_ip, int source_port,
		uint32_t dest_ip, int dest_port, const uint8_t *dest_arp, int ifindex, uint32_t vid)
{
	dbck("%d in %s of %s, vid %d\n", __LINE__, __func__, __FILE__, vid);
	struct sockaddr_ll dest;
	struct udp_dhcp_packet *packet = NULL;
	uint8_t frame[MAX_BUF_LENGTH];
	int fd, ret;
	int result = -1;
	const char *msg;
	struct ifreq interface;
    struct sockaddr_ll myaddr_ll;
    struct ifreq req;
	uint32_t length = 0;

	enum {
		IP_UPD_DHCP_SIZE = sizeof(struct udp_dhcp_packet) - CONFIG_UDHCPC_SLACK_FOR_BUGGY_SERVERS,
		UPD_DHCP_SIZE    = IP_UPD_DHCP_SIZE - offsetof(struct udp_dhcp_packet, udp),
	};

	fd = xsocket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    memset(&myaddr_ll, 0, sizeof(myaddr_ll));
    myaddr_ll.sll_family  = PF_PACKET;
    myaddr_ll.sll_protocol = htons(ETH_P_ALL);
    myaddr_ll.sll_ifindex = ifindex;

	xbind(fd, (struct sockaddr *)&myaddr_ll, sizeof(myaddr_ll));

	memset(frame, 0, sizeof(frame));

	memcpy(((eth_hdr_t*)frame)->dst_addr, dest_arp, MAC_LEN);/*DMAC*/
	memcpy(((eth_hdr_t*)frame)->src_addr, mac, MAC_LEN);/*SMAC*/

	if (vid > 0) {
		((eth_hdr_t*)frame)->protocol = htons((uint16_t)ETH_P_8021Q);
		*(uint16_t*)(&frame[ETH_IP_OFFSET]) = htons((uint16_t)((vid & 0xfff))|0x2000);
		*(uint16_t*)(&frame[ETH_IP_OFFSET + sizeof(uint16_t)]) = htons((uint16_t)ETH_P_IP);
		packet = (struct udp_dhcp_packet *)(&frame[ETH_IP_OFFSET + sizeof(uint32_t)]);
		length = IP_UPD_DHCP_SIZE + ETH_IP_OFFSET + sizeof(uint32_t);
	} else {
		((eth_hdr_t*)frame)->protocol = htons((uint16_t)ETH_P_IP);
		packet = (struct udp_dhcp_packet *)(&frame[ETH_IP_OFFSET]);
		length = IP_UPD_DHCP_SIZE + ETH_IP_OFFSET;
	}


	packet->data = *payload; /* struct copy */

	packet->ip.protocol = IPPROTO_UDP;
	packet->ip.saddr = source_ip;
	packet->ip.daddr = dest_ip;
	packet->udp.source = htons(source_port);
	packet->udp.dest = htons(dest_port);
	/* size, excluding IP header: */
	packet->udp.len = htons(UPD_DHCP_SIZE);
	/* for UDP checksumming, ip.len is set to UDP packet len */
	packet->ip.tot_len = packet->udp.len;
	packet->udp.check = udhcp_checksum(packet, IP_UPD_DHCP_SIZE);
	/* but for sending, it is set to IP packet len */
	packet->ip.tot_len = htons(IP_UPD_DHCP_SIZE);
	packet->ip.ihl = sizeof(packet->ip) >> 2;
	packet->ip.version = IPVERSION;
	packet->ip.ttl = IPDEFTTL;
	packet->ip.check = udhcp_checksum(&packet->ip, sizeof(packet->ip));

	/* Currently we send full-sized DHCP packets (zero padded).
	 * If you need to change this: last byte of the packet is
	 * packet.data.options[end_option(packet.data.options)]
	 */

	dbck("dest_arp  %02x:%02x:%02x:%02x:%02x:%02x\n", dest_arp[0], dest_arp[1], dest_arp[2], dest_arp[3], dest_arp[4], dest_arp[5]);
#if 0
	int i = 0u;
	dbck("frame length : %d\n", length);
	dbck("udp_checksum = %x, ip_checksum = %x\n", packet->udp.check, packet->ip.check);
	while (i < 80) {
		dbck(" %02x", *(frame + i));
		i++;

		if ((i % 16) == 0) {
			dbck("\n");
		}
	}
	dbck("\n");
	dbck("--------------------------------payload->xid = %x\n", packet->data.xid);
#endif
	result = send(fd, frame, length, 0);
	msg = "send";
 ret_close:
	close(fd);
	if (result < 0) {
 ret_msg:
		bb_perror_msg(msg, "PACKET");
	}
	return result;
}
#endif

int udhcp_send_raw_packet(struct dhcpMessage *payload,
		uint32_t source_ip, int source_port,
		uint32_t dest_ip, int dest_port, const uint8_t *dest_arp, int ifindex)
{
	struct sockaddr_ll dest;
	struct udp_dhcp_packet packet;
	int fd;
	int result = -1;
	const char *msg;

	enum {
		IP_UPD_DHCP_SIZE = sizeof(struct udp_dhcp_packet) - CONFIG_UDHCPC_SLACK_FOR_BUGGY_SERVERS,
		UPD_DHCP_SIZE    = IP_UPD_DHCP_SIZE - offsetof(struct udp_dhcp_packet, udp),
	};

	fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
	if (fd < 0) {
		msg = "socket(%s)";
		goto ret_msg;
	}

	memset(&dest, 0, sizeof(dest));
	memset(&packet, 0, sizeof(packet));
	packet.data = *payload; /* struct copy */

	dest.sll_family = AF_PACKET;
	dest.sll_protocol = htons(ETH_P_IP);
	dest.sll_ifindex = ifindex;
	dest.sll_halen = 6;
	memcpy(dest.sll_addr, dest_arp, 6);
	if (bind(fd, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
		msg = "bind(%s)";
		goto ret_close;
	}

	packet.ip.protocol = IPPROTO_UDP;
	packet.ip.saddr = source_ip;
	packet.ip.daddr = dest_ip;
	packet.udp.source = htons(source_port);
	packet.udp.dest = htons(dest_port);
	/* size, excluding IP header: */
	packet.udp.len = htons(UPD_DHCP_SIZE);
	/* for UDP checksumming, ip.len is set to UDP packet len */
	packet.ip.tot_len = packet.udp.len;
	packet.udp.check = udhcp_checksum(&packet, IP_UPD_DHCP_SIZE);
	/* but for sending, it is set to IP packet len */
	packet.ip.tot_len = htons(IP_UPD_DHCP_SIZE);
	packet.ip.ihl = sizeof(packet.ip) >> 2;
	packet.ip.version = IPVERSION;
	packet.ip.ttl = IPDEFTTL;
	packet.ip.check = udhcp_checksum(&packet.ip, sizeof(packet.ip));

	/* Currently we send full-sized DHCP packets (zero padded).
	 * If you need to change this: last byte of the packet is
	 * packet.data.options[end_option(packet.data.options)]
	 */
	result = sendto(fd, &packet, IP_UPD_DHCP_SIZE, 0,
				(struct sockaddr *) &dest, sizeof(dest));
	msg = "sendto";
 ret_close:
	close(fd);
	if (result < 0) {
 ret_msg:
		bb_perror_msg(msg, "PACKET");
	}
	return result;
}


/* Let the kernel do all the work for packet generation */
int udhcp_send_kernel_packet(struct dhcpMessage *payload,
		uint32_t source_ip, int source_port,
		uint32_t dest_ip, int dest_port)
{
	struct sockaddr_in client;
	int fd;
	int result = -1;
	const char *msg;

	enum {
		DHCP_SIZE = sizeof(struct dhcpMessage) - CONFIG_UDHCPC_SLACK_FOR_BUGGY_SERVERS,
	};

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		msg = "socket(%s)";
		goto ret_msg;
	}
	setsockopt_reuseaddr(fd);

	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(source_port);
	client.sin_addr.s_addr = source_ip;
	if (bind(fd, (struct sockaddr *)&client, sizeof(client)) == -1) {
		msg = "bind(%s)";
		goto ret_close;
	}

	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(dest_port);
	client.sin_addr.s_addr = dest_ip;
	if (connect(fd, (struct sockaddr *)&client, sizeof(client)) == -1) {
		msg = "connect";
		goto ret_close;
	}

	/* Currently we send full-sized DHCP packets (see above) */
	result = safe_write(fd, payload, DHCP_SIZE);
	msg = "write";
 ret_close:
	close(fd);
	if (result < 0) {
 ret_msg:
		bb_perror_msg(msg, "UDP");
	}
	return result;
}
