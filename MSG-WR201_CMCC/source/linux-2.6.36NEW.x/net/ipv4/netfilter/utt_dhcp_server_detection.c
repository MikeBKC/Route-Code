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


#define UTT_DHCP_SERVER_DETECTION_INTERVAL 7*HZ  /*与页面读取结果的时间间隔相同*/
static const u_char ic_bootp_cookie[4] = { 99, 130, 83, 99 };
static const u_char dst_mac[ETH_ALEN] =  { 0xFF, 0xFF, 0xFF, 0xFF,0xFF,0xFF };


char src_mac[ETH_ALEN];
static struct timer_list nfDhcpDetecTimer;

static void nfDhcpServerDetecTimeOut(u_long data);
static void nfDhcpDetecTimerInit(void);
#ifdef CONFIG_UTT_DEBUG
static int dhcp_collision_debug = 0;


/*
 * DHCP调试开关控制函数
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
 * DHCP冲突检测调试打印函数
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
 * send_by_skb		-	构造并发送dhcp包
 *
 */
void send_by_skb(void)
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

	nfDhcpDetecTimerInit();	/*无论发包成功与失败都要在指定时间后关闭*/
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

	/*在缓冲区的头部预留一定的空间*/
	skb_reserve(skb, LL_RESERVED_SPACE(netdev));
	skb->dev = netdev;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons(ETH_P_IP);
	skb->ip_summed = CHECKSUM_NONE;
	skb->priority = 0;

	/* 构造ethernet头*/
	eth = (struct ethhdr *) skb_put(skb, sizeof(struct ethhdr));
	memcpy(eth->h_dest, dst_mac, ETH_ALEN);
	memcpy(eth->h_source, src_mac, ETH_ALEN);
	eth->h_proto = htons(ETH_P_IP);

	/*ip头*/
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

	/*udp头*/
	skb_set_transport_header(skb, sizeof(struct ethhdr) + sizeof(struct iphdr));
	udph = (struct udphdr *)skb_put(skb, sizeof(struct udphdr));
	memset(udph, 0, sizeof(*udph));
	udph->source = htons(IPPORT_BOOTPC);
	udph->dest = htons(IPPORT_BOOTPS);
	udph->len = htons(sizeof(struct bootphdr) + 8);

	/*bootp头*/
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

	/*增加一个autoconfig option到DHCP包,只需在DISCOVER加，其他包不用。*/
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
 * 将utt_dhcp_server_detection置为0，停止检查
 */
static void nfDhcpServerDetecTimeOut(u_long data)
{
	utt_dhcp_server_detection = 0;
	return;
}
/**
 * 初始化定时器，指定时间后改变utt_dhcp_server_detection,停止检查dhcp冲突回复包
 */
static void nfDhcpDetecTimerInit(void)
{
    del_timer_sync(&nfDhcpDetecTimer);/*防止上次的定时器生效*/
    init_timer(&nfDhcpDetecTimer);
    nfDhcpDetecTimer.expires = jiffies + UTT_DHCP_SERVER_DETECTION_INTERVAL;   /* 定时器超时时的节拍数*/
    nfDhcpDetecTimer.data = 0;                /* 给定时器处理函数传入参数0*/
    nfDhcpDetecTimer.function = nfDhcpServerDetecTimeOut;   /* 定时器超时时调用的函数*/
    add_timer(&nfDhcpDetecTimer);/*激活定时器*/
    return;
}

EXPORT_SYMBOL(send_by_skb);
