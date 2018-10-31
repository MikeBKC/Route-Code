#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/types.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/route.h>
#include <net/dst.h>
#include <linux/autoconf.h>
#include <linux/netfilter_ipv4/utt_collision_detection.h>


static const u_char ic_bootp_cookie[4] = { 99, 130, 83, 99 };
static const u_char dst_mac[ETH_ALEN] =  { 0xFF, 0xFF, 0xFF, 0xFF,0xFF,0xFF };


char src_mac[ETH_ALEN];
static struct timer_list nfColliDetecTimer;

static void nfColliDetecTimeOut(u_long data);
static void nfColliDetecPollInit(void);
void uttNfColliDetecInit(void);
#ifdef CONFIG_UTT_DEBUG
static int dhcp_collision_debug = 0;


/*
 * DHCP���Կ��ؿ��ƺ���
 */
void utt_dhcp_collision_debug(UttNfConfType type)
{
    if (type == UTT_NFCONF_ADD) {
	dhcp_collision_debug = 1;
    } else {
	dhcp_collision_debug = 0;
    }
    return;
}
#endif

/*
 * DHCP��ͻ�����Դ�ӡ����
 */
int dhcpCollision_debug(const char* fmt, ...)
{
#ifdef CONFIG_UTT_DEBUG
    va_list   args; 
    if (dhcp_collision_debug) {             
	va_start(args, fmt); 
	debug_kprint(fmt, args);			
	va_end(args);
    }                                   
#endif
    return 0;
}

/*
 * send_by_skb		-	���첢����dhcp��
 *
 */
static void send_by_skb(void)
{
	struct net_device *netdev;
	struct sk_buff *skb;
	struct ethhdr *eth;
	struct iphdr *iph;
	struct udphdr *udph;
	struct bootphdr *bootph;
	u_long skb_net_len;
	u_long skb_len;
	int i;
	u_char *p = NULL;

	/* construct skb */
	netdev = dev_get_by_name(&init_net, utt_local_ifname);
	if (!netdev)
		return;
	dhcpCollision_debug("src_mac is ");
	for (i = 0; i < ETH_ALEN; i++) {
		src_mac[i] = netdev->dev_addr[i];
		dhcpCollision_debug("%02x ", src_mac[i]);
	}
	dhcpCollision_debug("\n");

	skb_net_len = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct bootphdr);
	skb_len = LL_RESERVED_SPACE(netdev) + skb_net_len + LL_MAX_HEADER;
	dhcpCollision_debug("LL_RESERVED_SPACE(netdev) : %d\n", LL_RESERVED_SPACE(netdev));
	dhcpCollision_debug("LL_MAX_HEADER: %d\n", LL_MAX_HEADER);
	dhcpCollision_debug("skb_len: %lu\n", skb_len);

	skb = dev_alloc_skb(skb_len);
	if (!skb)
		return;

	/*�ڻ�������ͷ��Ԥ��һ���Ŀռ�*/
	skb_reserve(skb, LL_RESERVED_SPACE(netdev));
	skb->dev = netdev;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons(ETH_P_IP);
	skb->ip_summed = CHECKSUM_NONE;
	skb->priority = 0;

	/* ����ethernetͷ*/
	eth = (struct ethhdr *) skb_put(skb, sizeof(struct ethhdr));
	memcpy(eth->h_dest, dst_mac, ETH_ALEN);
	memcpy(eth->h_source, src_mac, ETH_ALEN);
	eth->h_proto = htons(ETH_P_IP);

	/*ipͷ*/
	skb_set_network_header(skb, sizeof(struct ethhdr));
	iph = (struct iphdr *)skb_put(skb, sizeof(struct iphdr));
	memset(iph, 0, sizeof(*iph));
	iph->version	= 4;
	iph->ihl	= sizeof(struct iphdr) >> 2;
	iph->tot_len	= htons(skb_net_len);
#if 0
	iph->id	= id++;
#else
	__ip_select_ident(iph, skb_rtable((const struct sk_buff *)skb), 0);
#endif
#if 0
	iph->frag_off = 0;
#endif
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check	= ip_fast_csum((u_char *) iph, iph->ihl);
#if 0
	iph->saddr	= 0;
#endif
	iph->daddr	= htonl(INADDR_BROADCAST);

	/*udpͷ*/
	skb_set_transport_header(skb, sizeof(struct ethhdr) + sizeof(struct iphdr));
	udph = (struct udphdr *)skb_put(skb, sizeof(struct udphdr));
	memset(udph, 0, sizeof(*udph));
	udph->source = htons(IPPORT_BOOTPC);
	udph->dest = htons(IPPORT_BOOTPS);
	udph->len = htons(sizeof(struct bootphdr) + 8);

	/*bootpͷ*/
	bootph = (struct bootphdr *)skb_put(skb, sizeof(struct bootphdr));
	memset(bootph, 0, sizeof(*bootph));
	bootph->bp_op = BOOTP_REQ;
	bootph->bp_htype = HW_ETHER;
	bootph->bp_halen = ETH_ALEN;	
	bootph->bp_xid = randomize_range(0, 0xFFFFFFFF, 0);	
	bootph->bp_secs = htons(1);
	memcpy(bootph->bp_chaddr, src_mac, ETH_ALEN);	
	memcpy(bootph->bp_vend, ic_bootp_cookie, sizeof(ic_bootp_cookie));	
	p = &bootph->bp_vend[sizeof(ic_bootp_cookie)];

	*p++ = MESSAGE_TYPE_OPTION;
	*p++ = 1;
	*p++ = DHCPDISCOVER;

	/*����һ��autoconfig option��DHCP��,ֻ����DISCOVER�ӣ����������á�*/
	*p++ = AUTOCONFIG_OPTION;
	*p++ = 1;
	*p++ = 1;	

	*p++ = CLIENT_ID_OPTION;
	*p++ = 7;
	*p++ = (char)0x01;
	memcpy(p, src_mac, ETH_ALEN);
	p += ETH_ALEN;

	*p = 255;
	skb->csum = skb_checksum(skb, iph->ihl * 4, skb->len - iph->ihl * 4, 0);

	/* send packet */
	if (dev_queue_xmit(skb) < 0) {
		dev_put(netdev);
		kfree_skb(skb);
		dhcpCollision_debug("send packet by skb failed.\n");
		return;
	}
	dhcpCollision_debug("send packet by skb success.\n");
}
/*
 * nfColliDetecTimeOut	-	��ʱ����DHCP discover��
 *
 * ���������
 * data			-	�˴�����Ҫ���������������������һ��������
 *				���һֱΪ0
 */
static void nfColliDetecTimeOut(u_long data)
{
	dhcpCollision_debug("%s-%d:send collision detection packet start\n", __func__, __LINE__);

	/*DHCP��ͻ��⿪��*/
	if ((utt_collision_detection & DHCP_COLLISION_DETECTION)) 
		send_by_skb(); /*����dhcp discover��*/
	else 
		dhcpCollision_debug("%s-%d:dhcp collision detection switch is closed\n", __func__, __LINE__);

	dhcpCollision_debug("%s-%d:send collision detection packet end\n", __func__, __LINE__);
	mod_timer(&nfColliDetecTimer, jiffies + UTT_COLLISION_DETECTION_POLL_INTERVAL);/*���¼��ʱ��*/
	return;
}
/**
 * ��ѯ��ʼ���������
 */
static void nfColliDetecPollInit(void)
{
    init_timer(&nfColliDetecTimer);
    nfColliDetecTimer.expires = jiffies + UTT_COLLISION_DETECTION_POLL_INTERVAL;   /* ��ʱ����ʱʱ�Ľ�����*/
    nfColliDetecTimer.data = 0;                /* ����ʱ���������������0*/
    nfColliDetecTimer.function = nfColliDetecTimeOut;   /* ��ʱ����ʱʱ���õĺ���*/
    add_timer(&nfColliDetecTimer);/*���ʱ��*/
    return;
}



void uttNfColliDetecInit(void)
{
    nfColliDetecPollInit();/*��ѯ��ʼ��*/
}

