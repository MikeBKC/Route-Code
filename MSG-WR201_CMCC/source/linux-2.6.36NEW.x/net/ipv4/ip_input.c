/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The Internet Protocol (IP) module.
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Donald Becker, <becker@super.org>
 *		Alan Cox, <alan@lxorguk.ukuu.org.uk>
 *		Richard Underwood
 *		Stefan Becker, <stefanb@yello.ping.de>
 *		Jorge Cwik, <jorge@laser.satlink.net>
 *		Arnt Gulbrandsen, <agulbra@nvg.unit.no>
 *
 *
 * Fixes:
 *		Alan Cox	:	Commented a couple of minor bits of surplus code
 *		Alan Cox	:	Undefining IP_FORWARD doesn't include the code
 *					(just stops a compiler warning).
 *		Alan Cox	:	Frames with >=MAX_ROUTE record routes, strict routes or loose routes
 *					are junked rather than corrupting things.
 *		Alan Cox	:	Frames to bad broadcast subnets are dumped
 *					We used to process them non broadcast and
 *					boy could that cause havoc.
 *		Alan Cox	:	ip_forward sets the free flag on the
 *					new frame it queues. Still crap because
 *					it copies the frame but at least it
 *					doesn't eat memory too.
 *		Alan Cox	:	Generic queue code and memory fixes.
 *		Fred Van Kempen :	IP fragment support (borrowed from NET2E)
 *		Gerhard Koerting:	Forward fragmented frames correctly.
 *		Gerhard Koerting: 	Fixes to my fix of the above 8-).
 *		Gerhard Koerting:	IP interface addressing fix.
 *		Linus Torvalds	:	More robustness checks
 *		Alan Cox	:	Even more checks: Still not as robust as it ought to be
 *		Alan Cox	:	Save IP header pointer for later
 *		Alan Cox	:	ip option setting
 *		Alan Cox	:	Use ip_tos/ip_ttl settings
 *		Alan Cox	:	Fragmentation bogosity removed
 *					(Thanks to Mark.Bush@prg.ox.ac.uk)
 *		Dmitry Gorodchanin :	Send of a raw packet crash fix.
 *		Alan Cox	:	Silly ip bug when an overlength
 *					fragment turns up. Now frees the
 *					queue.
 *		Linus Torvalds/ :	Memory leakage on fragmentation
 *		Alan Cox	:	handling.
 *		Gerhard Koerting:	Forwarding uses IP priority hints
 *		Teemu Rantanen	:	Fragment problems.
 *		Alan Cox	:	General cleanup, comments and reformat
 *		Alan Cox	:	SNMP statistics
 *		Alan Cox	:	BSD address rule semantics. Also see
 *					UDP as there is a nasty checksum issue
 *					if you do things the wrong way.
 *		Alan Cox	:	Always defrag, moved IP_FORWARD to the config.in file
 *		Alan Cox	: 	IP options adjust sk->priority.
 *		Pedro Roque	:	Fix mtu/length error in ip_forward.
 *		Alan Cox	:	Avoid ip_chk_addr when possible.
 *	Richard Underwood	:	IP multicasting.
 *		Alan Cox	:	Cleaned up multicast handlers.
 *		Alan Cox	:	RAW sockets demultiplex in the BSD style.
 *		Gunther Mayer	:	Fix the SNMP reporting typo
 *		Alan Cox	:	Always in group 224.0.0.1
 *	Pauline Middelink	:	Fast ip_checksum update when forwarding
 *					Masquerading support.
 *		Alan Cox	:	Multicast loopback error for 224.0.0.1
 *		Alan Cox	:	IP_MULTICAST_LOOP option.
 *		Alan Cox	:	Use notifiers.
 *		Bjorn Ekwall	:	Removed ip_csum (from slhc.c too)
 *		Bjorn Ekwall	:	Moved ip_fast_csum to ip.h (inline!)
 *		Stefan Becker   :       Send out ICMP HOST REDIRECT
 *	Arnt Gulbrandsen	:	ip_build_xmit
 *		Alan Cox	:	Per socket routing cache
 *		Alan Cox	:	Fixed routing cache, added header cache.
 *		Alan Cox	:	Loopback didn't work right in original ip_build_xmit - fixed it.
 *		Alan Cox	:	Only send ICMP_REDIRECT if src/dest are the same net.
 *		Alan Cox	:	Incoming IP option handling.
 *		Alan Cox	:	Set saddr on raw output frames as per BSD.
 *		Alan Cox	:	Stopped broadcast source route explosions.
 *		Alan Cox	:	Can disable source routing
 *		Takeshi Sone    :	Masquerading didn't work.
 *	Dave Bonn,Alan Cox	:	Faster IP forwarding whenever possible.
 *		Alan Cox	:	Memory leaks, tramples, misc debugging.
 *		Alan Cox	:	Fixed multicast (by popular demand 8))
 *		Alan Cox	:	Fixed forwarding (by even more popular demand 8))
 *		Alan Cox	:	Fixed SNMP statistics [I think]
 *	Gerhard Koerting	:	IP fragmentation forwarding fix
 *		Alan Cox	:	Device lock against page fault.
 *		Alan Cox	:	IP_HDRINCL facility.
 *	Werner Almesberger	:	Zero fragment bug
 *		Alan Cox	:	RAW IP frame length bug
 *		Alan Cox	:	Outgoing firewall on build_xmit
 *		A.N.Kuznetsov	:	IP_OPTIONS support throughout the kernel
 *		Alan Cox	:	Multicast routing hooks
 *		Jos Vos		:	Do accounting *before* call_in_firewall
 *	Willy Konynenberg	:	Transparent proxying support
 *
 *
 *
 * To Fix:
 *		IP fragmentation wants rewriting cleanly. The RFC815 algorithm is much more efficient
 *		and could be made very efficient with the addition of some virtual memory hacks to permit
 *		the allocation of a buffer that can then be 'grown' by twiddling page tables.
 *		Output fragmentation wants updating along with the buffer management to use a single
 *		interleaved copy algorithm so that fragmenting has a one copy overhead. Actual packet
 *		output should probably do its own fragmentation at the UDP/RAW layer. TCP shouldn't cause
 *		fragmentation anyway.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#include <asm/system.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/slab.h>

#include <linux/net.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <net/snmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/route.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/arp.h>
#include <net/icmp.h>
#include <net/raw.h>
#include <net/checksum.h>
#include <linux/netfilter_ipv4.h>
#include <net/xfrm.h>
#include <linux/mroute.h>
#include <linux/netlink.h>

#include <net/netfilter/nf_conntrack.h>
#if (defined CONFIG_UTT_DHCP_COLLISION) || (defined CONFIG_UTT_DHCP_SERVER_DETECTION)
#include <linux/netfilter_ipv4/utt_collision_detection.h>
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/utt_module_init.h>
#endif

/*
 *	Process Router Attention IP option (RFC 2113)
 */
int ip_call_ra_chain(struct sk_buff *skb)
{
	struct ip_ra_chain *ra;
	u8 protocol = ip_hdr(skb)->protocol;
	struct sock *last = NULL;
	struct net_device *dev = skb->dev;

	for (ra = rcu_dereference(ip_ra_chain); ra; ra = rcu_dereference(ra->next)) {
		struct sock *sk = ra->sk;

		/* If socket is bound to an interface, only report
		 * the packet if it came  from that interface.
		 */
		if (sk && inet_sk(sk)->inet_num == protocol &&
		    (!sk->sk_bound_dev_if ||
		     sk->sk_bound_dev_if == dev->ifindex) &&
		    net_eq(sock_net(sk), dev_net(dev))) {
			if (ip_hdr(skb)->frag_off & htons(IP_MF | IP_OFFSET)) {
				if (ip_defrag(skb, IP_DEFRAG_CALL_RA_CHAIN))
					return 1;
			}
			if (last) {
				struct sk_buff *skb2 = skb_clone(skb, GFP_ATOMIC);
				if (skb2)
					raw_rcv(last, skb2);
			}
			last = sk;
		}
	}

	if (last) {
		raw_rcv(last, skb);
		return 1;
	}
	return 0;
}

static int ip_local_deliver_finish(struct sk_buff *skb)
{
	struct net *net = dev_net(skb->dev);

	__skb_pull(skb, ip_hdrlen(skb));

	/* Point into the IP datagram, just past the header. */
	skb_reset_transport_header(skb);

	rcu_read_lock();
	{
		int protocol = ip_hdr(skb)->protocol;
		int hash, raw;
		const struct net_protocol *ipprot;

	resubmit:
		raw = raw_local_deliver(skb, protocol);

		hash = protocol & (MAX_INET_PROTOS - 1);
		ipprot = rcu_dereference(inet_protos[hash]);
		if (ipprot != NULL) {
			int ret;

			if (!net_eq(net, &init_net) && !ipprot->netns_ok) {
				if (net_ratelimit())
					printk("%s: proto %d isn't netns-ready\n",
						__func__, protocol);
				kfree_skb(skb);
				goto out;
			}

			if (!ipprot->no_policy) {
				if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
					kfree_skb(skb);
					goto out;
				}
				nf_reset(skb);
			}
			ret = ipprot->handler(skb);
			if (ret < 0) {
				protocol = -ret;
				goto resubmit;
			}
			IP_INC_STATS_BH(net, IPSTATS_MIB_INDELIVERS);
		} else {
			if (!raw) {
				if (xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
					IP_INC_STATS_BH(net, IPSTATS_MIB_INUNKNOWNPROTOS);
					icmp_send(skb, ICMP_DEST_UNREACH,
						  ICMP_PROT_UNREACH, 0);
				}
			} else
				IP_INC_STATS_BH(net, IPSTATS_MIB_INDELIVERS);
			kfree_skb(skb);
		}
	}
 out:
	rcu_read_unlock();

	return 0;
}

#if (defined CONFIG_UTT_DHCP_COLLISION) || (defined CONFIG_UTT_DHCP_SERVER_DETECTION)
/*
 * ���շ���·������DHCP��ͻ����
 */
static void utt_local_collision(struct sk_buff *skb)
{
	struct ethhdr *eth;
	struct iphdr *iph;
	struct udphdr *udph;
	struct bootphdr *bootph;
	char ipaddr[MAX_IP_LEN];

	/*�ж��Ƿ��lan�����İ�*/
	if (!isSkbComeFromLan(skb))
		return;

	iph = ip_hdr(skb);

	/* ��ip�� */
	if (!iph)
		return;

	/*��UDPЭ��*/
	if (iph->protocol != IPPROTO_UDP) 
		return;

	udph = (struct udphdr *) ((void *)iph + iph->ihl * 4);

	/*����Ƿ���udp����*/
	if (!udph) 
		return;

	/* ��67�˿�*/
	if (ntohs(udph->source) != IPPORT_BOOTPS) 
		return;

	/*���������Ƿ�dhcp offer����С����*/
	dhcpCollision_debug("htons(iph->tot_len) is %x\n", htons(iph->tot_len));
	if (htons(iph->tot_len) < MIN_DHCP_OFFER_LEN) 
		return;

	bootph = (struct bootphdr *) ((void *)udph + sizeof(*udph));

	/*����Ƿ���bootp����*/
	if (!bootph)
		return;

	if ((bootph->bp_op == BOOTP_REP) && (!memcmp(bootph->bp_chaddr, src_mac, ETH_ALEN))) {
		dhcpCollision_debug("The dhcp packet is offer to me\n");
		sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
		eth = eth_hdr(skb);
#ifdef CONFIG_UTT_DHCP_SERVER_DETECTION
		utt_dhcp_server_detection = 2;
#else
		printk_audit(KERN_WARNING "DHCPS: server conflict conflict ip=%s,mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
				ipaddr, eth->h_source[0], eth->h_source[1], 
				eth->h_source[2], eth->h_source[3], 
				eth->h_source[4], eth->h_source[5]);
#endif
	}
	return;
}
#endif
/*
 * 	Deliver IP Packets to the higher protocol layers.
 */
int ip_local_deliver(struct sk_buff *skb)
{
	/*
	 *	Reassemble IP fragments.
	 */

	if (ip_hdr(skb)->frag_off & htons(IP_MF | IP_OFFSET)) {
		if (ip_defrag(skb, IP_DEFRAG_LOCAL_DELIVER))
			return 0;
	}
#if CONFIG_UTT_DHCP_COLLISION
	/*���շ���·������DHCP��ͻ����*/
	if ((utt_collision_detection & DHCP_COLLISION_DETECTION)) 
		utt_local_collision(skb);
#endif
#ifdef CONFIG_UTT_DHCP_SERVER_DETECTION
	if (utt_dhcp_server_detection == 1)
	{
	    utt_local_collision(skb);
	}
#endif

	return NF_HOOK(NFPROTO_IPV4, NF_INET_LOCAL_IN, skb, skb->dev, NULL,
		       ip_local_deliver_finish);
}

static inline int ip_rcv_options(struct sk_buff *skb)
{
	struct ip_options *opt;
	struct iphdr *iph;
	struct net_device *dev = skb->dev;

	/* It looks as overkill, because not all
	   IP options require packet mangling.
	   But it is the easiest for now, especially taking
	   into account that combination of IP options
	   and running sniffer is extremely rare condition.
					      --ANK (980813)
	*/
	if (skb_cow(skb, skb_headroom(skb))) {
		IP_INC_STATS_BH(dev_net(dev), IPSTATS_MIB_INDISCARDS);
		goto drop;
	}

	iph = ip_hdr(skb);
	opt = &(IPCB(skb)->opt);
	opt->optlen = iph->ihl*4 - sizeof(struct iphdr);

	if (ip_options_compile(dev_net(dev), opt, skb)) {
		IP_INC_STATS_BH(dev_net(dev), IPSTATS_MIB_INHDRERRORS);
		goto drop;
	}

	if (unlikely(opt->srr)) {
		struct in_device *in_dev = __in_dev_get_rcu(dev);

		if (in_dev) {
			if (!IN_DEV_SOURCE_ROUTE(in_dev)) {
				if (IN_DEV_LOG_MARTIANS(in_dev) &&
				    net_ratelimit())
					printk(KERN_INFO "source route option %pI4 -> %pI4\n",
					       &iph->saddr, &iph->daddr);
				goto drop;
			}
		}

		if (ip_options_rcv_srr(skb))
			goto drop;
	}

	return 0;
drop:
	return -1;
}

static int ip_rcv_finish(struct sk_buff *skb)
{
	const struct iphdr *iph = ip_hdr(skb);
	struct rtable *rt;

	/*
	 *	Initialise the virtual path cache for the packet. It describes
	 *	how the packet travels inside Linux networking.
	 */
	if (skb_dst(skb) == NULL) {
		int err = ip_route_input_noref(skb, iph->daddr, iph->saddr,
					       iph->tos, skb->dev);
		if (unlikely(err)) {
			if (err == -EHOSTUNREACH)
				IP_INC_STATS_BH(dev_net(skb->dev),
						IPSTATS_MIB_INADDRERRORS);
			else if (err == -ENETUNREACH)
				IP_INC_STATS_BH(dev_net(skb->dev),
						IPSTATS_MIB_INNOROUTES);
			else if (err == -EXDEV)
				NET_INC_STATS_BH(dev_net(skb->dev),
						 LINUX_MIB_IPRPFILTER);
			goto drop;
		}
	}

#ifdef CONFIG_NET_CLS_ROUTE
	if (unlikely(skb_dst(skb)->tclassid)) {
		struct ip_rt_acct *st = this_cpu_ptr(ip_rt_acct);
		u32 idx = skb_dst(skb)->tclassid;
		st[idx&0xFF].o_packets++;
		st[idx&0xFF].o_bytes += skb->len;
		st[(idx>>16)&0xFF].i_packets++;
		st[(idx>>16)&0xFF].i_bytes += skb->len;
	}
#endif

	if (iph->ihl > 5 && ip_rcv_options(skb))
		goto drop;

	rt = skb_rtable(skb);
	if (rt->rt_type == RTN_MULTICAST) {
		IP_UPD_PO_STATS_BH(dev_net(rt->dst.dev), IPSTATS_MIB_INMCAST,
				skb->len);
	} else if (rt->rt_type == RTN_BROADCAST)
		IP_UPD_PO_STATS_BH(dev_net(rt->dst.dev), IPSTATS_MIB_INBCAST,
				skb->len);

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	if (!isSkbComeFromLan(skb)) {/*��lan�ڽ����ģ�����Ŀ�ĵ�ַ��ѯ*/
	    int xrg = 0;
	    skb->ipHashNode = uttNfHashBufGet((u_long)iph->daddr, &xrg);
	}
#endif
	return dst_input(skb);

drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}


#if 0
static inline struct nf_conn *
nf_ct_get(const struct sk_buff *skb, enum ip_conntrack_info *ctinfo)
{
	*ctinfo = skb->nfctinfo;
	return (struct nf_conn *)skb->nfct;
}
#endif

#ifdef CONFIG_NF_CONNTRACK_MARK

static inline u_int32_t *nf_ct_get_mark(const struct sk_buff *skb,
					u_int32_t *ctinfo)
{
	struct nf_conn *ct = nf_ct_get(skb, ctinfo);

	if (ct)
		return &ct->mark;
	else
		return NULL;
}



static void utt_restore_CONNMARK(struct sk_buff *skb)
{
	u_int32_t ctinfo;
	u_int32_t *ctmark = nf_ct_get_mark(skb, &ctinfo);
        if(ctmark != NULL);
        skb->mark = *ctmark;
}
#endif /* CONFIG_NF_CONNTRACK_MARK */

/*
 * 	Main IP Receive routine.
 */
int ip_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
	struct iphdr *iph;
	u32 len;
#if (CONFIG_UTT_MAC_LIMIT == FYES) 
	UttNfHashBuf *ipHashNode ;
	UttNfHashConf* hashConf ;
	UttStatsPerIp *stats ;
	struct ethhdr *eth;
	int is_Creat;
#endif

	/* When the interface is in promisc. mode, drop all the crap
	 * that it receives, do not try to analyse it.
	 */
	if (skb->pkt_type == PACKET_OTHERHOST)
		goto drop;


	IP_UPD_PO_STATS_BH(dev_net(dev), IPSTATS_MIB_IN, skb->len);

	if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL) {
		IP_INC_STATS_BH(dev_net(dev), IPSTATS_MIB_INDISCARDS);
		goto out;
	}

	if (!pskb_may_pull(skb, sizeof(struct iphdr)))
		goto inhdr_error;

	iph = ip_hdr(skb);

	/*
	 *	RFC1122: 3.2.1.2 MUST silently discard any IP frame that fails the checksum.
	 *
	 *	Is the datagram acceptable?
	 *
	 *	1.	Length at least the size of an ip header
	 *	2.	Version of 4
	 *	3.	Checksums correctly. [Speed optimisation for later, skip loopback checksums]
	 *	4.	Doesn't have a bogus length
	 */

	if (iph->ihl < 5 || iph->version != 4)
		goto inhdr_error;

	if (!pskb_may_pull(skb, iph->ihl*4))
		goto inhdr_error;

	iph = ip_hdr(skb);

	if (unlikely(ip_fast_csum((u8 *)iph, iph->ihl)))
		goto inhdr_error;

	len = ntohs(iph->tot_len);
	if (skb->len < len) {
		IP_INC_STATS_BH(dev_net(dev), IPSTATS_MIB_INTRUNCATEDPKTS);
		goto drop;
	} else if (len < (iph->ihl*4))
		goto inhdr_error;

	/* Our transport medium may have padded the buffer out. Now we know it
	 * is IP we can trim to the true length of the frame.
	 * Note this now means skb->len holds ntohs(iph->tot_len).
	 */
	if (pskb_trim_rcsum(skb, len)) {
		IP_INC_STATS_BH(dev_net(dev), IPSTATS_MIB_INDISCARDS);
		goto drop;
	}

	/* Remove any debris in the socket control block */
	memset(IPCB(skb), 0, sizeof(struct inet_skb_parm));

	/* Must drop socket now because of tproxy. */
	skb_orphan(skb);

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION

	if (isSkbComeFromLan(skb)) {/*lan�ڽ����ģ�����Դ��ַ��ַ��ѯ,�鲻���򴴽�*/
#if (CONFIG_UTT_MAC_LIMIT == FYES)

ip_hash_init:
	    is_Creat = 1;
	    skb->ipHashNode = uttNfHashBufGet((u_long)iph->saddr, &is_Creat);
	    if(skb->ipHashNode != NULL)
	    {
		ipHashNode =  skb->ipHashNode;
		hashConf = &(ipHashNode->conf);
		
		//hashConf = uttNfHashConfGet(iph->saddr, 0);
		stats = &(hashConf->userStats);/*��λ��״̬*/
		eth = eth_hdr(skb);/*ȡmacͷ*/
		if(is_Creat == 1)
		{		    
		    if (((unsigned char *)eth) + ETH_HLEN <= skb->data) { /* eth���ȡ��ȷ */
			memcpy(stats->mac, eth->h_source, ETH_ALEN);/*��iphash���mac��ַ��ֵ*/
		    } else {/* ��ȡ���󣬽�ͳ��mac��0 */
			memset(stats->mac, 0, ETH_ALEN);
		    }
		    uttTraverseIpNodeInit(skb->ipHashNode); /*������ģ�飬��ʼ��ip hash�ڵ������е����ȼ��ֶ�*/
		}
		else
		{
		    if(unlikely((((unsigned char *)eth) + ETH_HLEN <= skb->data)&&(memcmp(eth->h_source,stats->mac,ETH_ALEN)!=0)))
		    {
			printk_audit("hash and skb mac not equal\n");
			uttNfHashBufDel((u_long)iph->saddr);	
			goto ip_hash_init;
		    }		    
		}
	    }
#endif
	}
#endif
	return NF_HOOK(NFPROTO_IPV4, NF_INET_PRE_ROUTING, skb, dev, NULL,
		       ip_rcv_finish);

inhdr_error:
	IP_INC_STATS_BH(dev_net(dev), IPSTATS_MIB_INHDRERRORS);
drop:
	kfree_skb(skb);
out:
	return NET_RX_DROP;
}
