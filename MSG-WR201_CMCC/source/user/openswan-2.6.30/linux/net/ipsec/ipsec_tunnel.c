/*
 * IPSEC Tunneling code. Heavily based on drivers/net/new_tunnel.c
 * Copyright (C) 1996, 1997  John Ioannidis.
 * Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003  Richard Guy Briggs.
 * 
 * OCF/receive state machine written by
 * David McCullough <dmccullough@cyberguard.com>
 * Copyright (C) 2004-2005 Intel Corporation.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#define __NO_VERSION__
#include <linux/module.h>
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif	/* for CONFIG_IP_FORWARD */
#include <linux/version.h>
#include <linux/kernel.h> /* printk() */

#include "openswan/ipsec_param.h"

#ifdef MALLOC_SLAB
# include <linux/slab.h> /* kmalloc() */
#else /* MALLOC_SLAB */
# include <linux/malloc.h> /* kmalloc() */
#endif /* MALLOC_SLAB */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/file.h> 
#include <linux/interrupt.h> /* mark_bh */

#include <net/tcp.h>
#include <net/udp.h>
#include <linux/skbuff.h>

#include <linux/netdevice.h>   /* struct device, struct net_device_stats, dev_queue_xmit() and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */
#include <net/arp.h>
#include <linux/skbuff.h>

#include <openswan.h>

# include <linux/in6.h>
# define IS_MYADDR RTN_LOCAL
# include <net/dst.h>
# define PHYSDEV_TYPE

#ifndef NETDEV_TX_BUSY
# ifdef NETDEV_XMIT_CN
#  define NETDEV_TX_BUSY NETDEV_XMIT_CN
# else
#  define NETDEV_TX_BUSY 1
# endif
#endif

#include <net/icmp.h>		/* icmp_send() */
#include <net/ip.h>
#include <net/arp.h>
#ifdef NETDEV_23
# include <linux/netfilter_ipv4.h>
#endif /* NETDEV_23 */

#include <linux/if_arp.h>
#include <net/arp.h>

#include "openswan/ipsec_kversion.h"
#include "openswan/radij.h"
#include "openswan/ipsec_life.h"
#include "openswan/ipsec_xform.h"
#include "openswan/ipsec_eroute.h"
#include "openswan/ipsec_encap.h"
#include "openswan/ipsec_radij.h"
#include "openswan/ipsec_sa.h"
#include "openswan/ipsec_tunnel.h"
#include "openswan/ipsec_xmit.h"
#include "openswan/ipsec_rcv.h"
#include "openswan/ipsec_ipe4.h"
#include "openswan/ipsec_ah.h"
#include "openswan/ipsec_esp.h"

#include <openswan/pfkeyv2.h>
#include <openswan/pfkey.h>

#include "openswan/ipsec_proto.h"
#ifdef CONFIG_IPSEC_NAT_TRAVERSAL
#include <linux/udp.h>
#endif

static __u32 zeroes[64];

/* forward references */
DEBUG_NO_STATIC int ipsec_tunnel_attach(struct net_device *dev, struct net_device *physdev);
DEBUG_NO_STATIC int ipsec_tunnel_detach(struct net_device *dev);
extern const struct net_device_ops klips_device_ops;

#ifdef HAVE_UDP_ENCAP_CONVERT
DEBUG_NO_STATIC int ipsec_tunnel_udp_encap_prepare(int fd, int encap_type);
DEBUG_NO_STATIC void ipsec_tunnel_udp_encap_destruct(struct sock *sk);
DEBUG_NO_STATIC void ipsec_tunnel_upd_encap_cleanup(void);
#endif

#ifdef CONFIG_KLIPS_DEBUG
int debug_tunnel = 0;
#endif /* CONFIG_KLIPS_DEBUG */

DEBUG_NO_STATIC int
ipsec_tunnel_open(struct net_device *dev)
{
	struct ipsecpriv *prv = netdev_priv(dev);
	
	/*
	 * Can't open until attached.
	 */

	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_open: "
		    "dev = %s, prv->dev = %s\n",
		    dev->name, prv->dev?prv->dev->name:"NONE");

	if (prv->dev == NULL)
		return -ENODEV;
	
	KLIPS_INC_USE;
	return 0;
}

DEBUG_NO_STATIC int
ipsec_tunnel_close(struct net_device *dev)
{
	KLIPS_DEC_USE;
	return 0;
}

static inline int ipsec_tunnel_xmit2(struct sk_buff *skb)
{

#ifdef NETDEV_25	/* 2.6 kernels */
	return dst_output(skb);
#else
	return ip_send(skb);
#endif
}

#ifdef HAVE_NETDEV_HEADER_OPS

int klips_header(struct sk_buff *skb, struct net_device *dev,
		 unsigned short type,
		 const void *daddr, const void *saddr, unsigned len)
{
	struct ipsecpriv *prv = netdev_priv(dev);
	struct net_device *tmp;
	int ret;
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(skb == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no skb...\n");
		return -ENODATA;
	}

	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no device...\n");
		return -ENODEV;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel_hard_header: "
		    "skb->dev=%s dev=%s.\n",
		    skb->dev ? skb->dev->name : "NULL",
		    dev->name);
	
	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no private space associated with dev=%s\n",
			    dev->name ? dev->name : "NULL");
		return -ENODEV;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no physical device associated with dev=%s\n",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return -ENODEV;
	}

	if(prv->dev->header_ops == NULL ||
			prv->dev->header_ops->create == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "cannot revector dev=%s op=%p func=%p",
			    dev->name ? dev->name : "NULL",
				prv->dev->header_ops, prv->dev->header_ops ?
				prv->dev->header_ops->create : 0);
		/* don't fail here or devices without header ops (like ppp) fail */
		return 0;
	}

	/* check if we have to send a IPv6 packet. It might be a Router
	   Solicitation, where the building of the packet happens in
	   reverse order:
	   1. ll hdr,
	   2. IPv6 hdr,
	   3. ICMPv6 hdr
	   -> skb->nh.raw is still uninitialized when this function is
	   called!!  If this is no IPv6 packet, we can print debugging
	   messages, otherwise we skip all debugging messages and just
	   build the ll header */
	if(type != ETH_P_IPV6) {
		/* execute this only, if we don't have to build the
		   header for a IPv6 packet */
		if(!prv->dev) {
			KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
				    "klips_debug:ipsec_tunnel_hard_header: "
				    "physical device has been detached, packet dropped 0p%p->0p%p len=%d type=%d dev=%s->NULL ",
				    saddr,
				    daddr,
				    len,
				    type,
				    dev->name);
			KLIPS_PRINTMORE(debug_tunnel & DB_TN_REVEC,
					"ip=%08x->%08x\n",
					(__u32)ntohl(ip_hdr(skb)->saddr),
					(__u32)ntohl(ip_hdr(skb)->daddr) );
			stats->tx_dropped++;
			return -ENODEV;
		}
		
#define da ((struct net_device *)(prv->dev))->dev_addr
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "Revectored 0p%p->0p%p len=%d type=%d dev=%s->%s dev_addr=%02x:%02x:%02x:%02x:%02x:%02x ",
			    saddr,
			    daddr,
			    len,
			    type,
			    dev->name,
			    prv->dev->name,
			    da[0], da[1], da[2], da[3], da[4], da[5]);
		KLIPS_PRINTMORE(debug_tunnel & DB_TN_REVEC,
			    "ip=%08x->%08x\n",
			    (__u32)ntohl(ip_hdr(skb)->saddr),
			    (__u32)ntohl(ip_hdr(skb)->daddr) );
	} else {
		KLIPS_PRINT(debug_tunnel,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "is IPv6 packet, skip debugging messages, only revector and build linklocal header.\n");
	}                                                                       
	tmp = skb->dev;
	skb->dev = prv->dev;
	ret = prv->dev->header_ops->create(skb, prv->dev, type,
					   (void *)daddr, (void *)saddr, len);
	skb->dev = tmp;
	return ret;
}

int klips_header_parse(const struct sk_buff *skb, unsigned char *haddr)
{
	struct ipsecpriv *prv = netdev_priv(skb->dev);
	struct net_device_stats *stats;	/* This device's statistics */
	int ret;

	stats = (struct net_device_stats *) &(prv->mystats);
	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:klips_header_parse: "
			    "no physical device associated with dev=%s",
			    skb->dev->name ? skb->dev->name : "NULL");
		stats->tx_dropped++;
		return -ENODEV;
	}

	if(prv->dev->header_ops == NULL ||
			prv->dev->header_ops->parse == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:klips_header_parse: "
			    "cannot revector dev=%s op=%p func=%p",
			    skb->dev->name ? skb->dev->name : "NULL",
				prv->dev->header_ops, prv->dev->header_ops ?
				prv->dev->header_ops->parse : 0);
		stats->tx_dropped++;
		return -ENODEV;
	}

	
	{
#if 0	
		struct net_device *tmp;
		tmp = skb->dev;
		skb->dev = prv->dev; 
#endif
		ret = prv->dev->header_ops->parse(skb, haddr);
#if 0
		skb->dev = tmp;
#endif
	}
	return ret;
}

DEBUG_NO_STATIC int
klips_rebuild_header(struct sk_buff *skb)
{
	struct ipsecpriv *prv = netdev_priv(skb->dev);
	struct net_device *tmp;
	int ret;
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(skb->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "no device...");
		return -ENODEV;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "no private space associated with dev=%s",
			    skb->dev->name ? skb->dev->name : "NULL");
		return -ENODEV;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "no physical device associated with dev=%s",
			    skb->dev->name ? skb->dev->name : "NULL");
		stats->tx_dropped++;
		return -ENODEV;
	}

	if(prv->dev->header_ops == NULL ||
			prv->dev->header_ops->rebuild == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "cannot revector dev=%s op=%p func=%p",
			    skb->dev->name ? skb->dev->name : "NULL",
				prv->dev->header_ops, prv->dev->header_ops ?
				prv->dev->header_ops->rebuild : 0);
		stats->tx_dropped++;
		return -ENODEV;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel: "
		    "Revectored rebuild_header dev=%s->%s ",
		    skb->dev->name, prv->dev->name);
	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "ip=%08x->%08x\n",
		    (__u32)ntohl(ip_hdr(skb)->saddr),
		    (__u32)ntohl(ip_hdr(skb)->daddr) );
	tmp = skb->dev;
	skb->dev = prv->dev;
	
	ret = prv->dev->header_ops->rebuild(skb);
	skb->dev = tmp;
	return ret;
}

int klips_header_cache(const struct neighbour *neigh, struct hh_cache *hh)
{
	const struct net_device *dev = neigh->dev;
	struct ipsecpriv *prv = netdev_priv(dev);
	struct net_device_stats *stats;

	stats = (struct net_device_stats *) &(prv->mystats);
	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:klips_header_cache: "
			    "no physical device associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return -1;
	}

	if(prv->dev->header_ops == NULL ||
			prv->dev->header_ops->cache == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:klips_header_cache: "
			    "cannot revector dev=%s op=%p func=%p",
			    dev->name ? dev->name : "NULL",
				prv->dev->header_ops, prv->dev->header_ops ?
				prv->dev->header_ops->cache : 0);
		/* don't fail here or devices without header ops (like ppp) fail */
		return 0;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel: "
		    "Revectored cache_update\n");
	return prv->dev->header_ops->cache(neigh, hh);
}

DEBUG_NO_STATIC void
klips_header_cache_update(struct hh_cache *hh,
			  const struct net_device *dev,
			  const unsigned char *  haddr)
{
	struct ipsecpriv *prv = netdev_priv(dev);
	
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "no device...");
		return;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "no private space associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		return;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "no physical device associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return;
	}

	if(prv->dev->header_ops == NULL ||
			prv->dev->header_ops->cache_update == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "cannot revector dev=%s op=%p func=%p",
			    dev->name ? dev->name : "NULL",
				prv->dev->header_ops, prv->dev->header_ops ?
				prv->dev->header_ops->cache_update : 0);
		stats->tx_dropped++;
		return;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel: "
		    "Revectored cache_update\n");
	prv->dev->header_ops->cache_update(hh, prv->dev, haddr);
	return;
}


const struct header_ops klips_header_ops ____cacheline_aligned = {
	.create		= klips_header,
	.parse		= klips_header_parse,
	.rebuild	= klips_rebuild_header,
	.cache		= klips_header_cache,
	.cache_update	= klips_header_cache_update,
};

#endif /* HAVE_NETDEV_HEADER_OPS */

enum ipsec_xmit_value
ipsec_tunnel_strip_hard_header(struct ipsec_xmit_state *ixs)
{
	/* ixs->physdev->hard_header_len is unreliable and should not be used */
        ixs->hard_header_len = (unsigned char *)(ixs->iph) - ixs->skb->data;

	if(ixs->hard_header_len < 0) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_error:ipsec_xmit_strip_hard_header: "
			    "Negative hard_header_len (%d)?!\n", ixs->hard_header_len);
		ixs->stats->tx_dropped++;
		return IPSEC_XMIT_BADHHLEN;
	}

	/* while ixs->physdev->hard_header_len is unreliable and
	 * should not be trusted, it accurate and required for ATM, GRE and
	 * some other interfaces to work. Thanks to Willy Tarreau 
	 * <willy@w.ods.org>.
	 */
	if(ixs->hard_header_len == 0) { /* no hard header present */
		ixs->hard_header_stripped = 1;
		ixs->hard_header_len = ixs->physdev->hard_header_len;
	}

	if (debug_tunnel & DB_TN_XMIT) {
		int i;
		char c;
		
		printk(KERN_INFO "klips_debug:ipsec_xmit_strip_hard_header: "
		       ">>> skb->len=%ld hard_header_len:%d",
		       (unsigned long int)ixs->skb->len, ixs->hard_header_len);
		c = ' ';
		for (i=0; i < ixs->hard_header_len; i++) {
			printk("%c%02x", c, ixs->skb->data[i]);
			c = ':';
		}
		printk(" \n");
	}

	KLIPS_IP_PRINT(debug_tunnel & DB_TN_XMIT, ixs->iph);

	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_strip_hard_header: "
		    "Original head,tailroom: %d,%d\n",
		    skb_headroom(ixs->skb), skb_tailroom(ixs->skb));

	return IPSEC_XMIT_OK;
}

enum ipsec_xmit_value
ipsec_tunnel_SAlookup(struct ipsec_xmit_state *ixs)
{
	unsigned int bypass;

	bypass = FALSE;

	/*
	 * First things first -- look us up in the erouting tables.
	 */
	ixs->matcher.sen_len = sizeof (struct sockaddr_encap);
	ixs->matcher.sen_family = AF_ENCAP;
	ixs->matcher.sen_type = SENT_IP4;
	ixs->matcher.sen_ip_src.s_addr = ixs->iph->saddr;
	ixs->matcher.sen_ip_dst.s_addr = ixs->iph->daddr;
	ixs->matcher.sen_proto = ixs->iph->protocol;
	ipsec_extract_ports(ixs->iph, &ixs->matcher);

	/*
	 * The spinlock is to prevent any other process from accessing or deleting
	 * the eroute while we are using and updating it.
	 */
	spin_lock_bh(&eroute_lock);
	
	ixs->eroute = ipsec_findroute(&ixs->matcher);

	if(ixs->iph->protocol == IPPROTO_UDP) {
		struct udphdr *t = NULL;

		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:udp port check: "
			    "fragoff: %d len: %d>%ld \n",
			    ntohs(ixs->iph->frag_off) & IP_OFFSET,
			    (ixs->skb->len - ixs->hard_header_len),
                            (unsigned long int) ((ixs->iph->ihl << 2) + sizeof(struct udphdr)));
		
		if((ntohs(ixs->iph->frag_off) & IP_OFFSET) == 0 &&
		   ((ixs->skb->len - ixs->hard_header_len) >=
		    ((ixs->iph->ihl << 2) + sizeof(struct udphdr))))
		{
			t =((struct udphdr*)((caddr_t)ixs->iph+(ixs->iph->ihl<<2)));
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:udp port in packet: "
				    "port %d -> %d\n",
				    ntohs(t->source), ntohs(t->dest));
		}

		ixs->sport=0; ixs->dport=0;

		if(ixs->skb->sk) {
#ifdef NET_26
#ifdef HAVE_INET_SK_SPORT
			ixs->sport = ntohs(inet_sk(ixs->skb->sk)->inet_sport);
			ixs->dport = ntohs(inet_sk(ixs->skb->sk)->inet_dport);
#else
			struct udp_sock *us;
			
			us = (struct udp_sock *)ixs->skb->sk;

			ixs->sport = ntohs(us->inet.sport);
			ixs->dport = ntohs(us->inet.dport);
#endif
#else
			ixs->sport = ntohs(ixs->skb->sk->sport);
			ixs->dport = ntohs(ixs->skb->sk->dport);
#endif

		} 

		if(t != NULL) {
			if(ixs->sport == 0) {
				ixs->sport = ntohs(t->source);
			}
			if(ixs->dport == 0) {
				ixs->dport = ntohs(t->dest);
			}
		}
	}

	/*
	 * practically identical to above, but let's be careful about
	 * tcp vs udp headers
	 */
	if(ixs->iph->protocol == IPPROTO_TCP) {
		struct tcphdr *t = NULL;

		if((ntohs(ixs->iph->frag_off) & IP_OFFSET) == 0 &&
		   ((ixs->skb->len - ixs->hard_header_len) >=
		    ((ixs->iph->ihl << 2) + sizeof(struct tcphdr)))) {
			t =((struct tcphdr*)((caddr_t)ixs->iph+(ixs->iph->ihl<<2)));
		}

		ixs->sport=0; ixs->dport=0;

		if(ixs->skb->sk) {
#ifdef NET_26
#ifdef HAVE_INET_SK_SPORT
			ixs->sport = ntohs(inet_sk(ixs->skb->sk)->inet_sport);
			ixs->dport = ntohs(inet_sk(ixs->skb->sk)->inet_dport);
#else
			struct tcp_tw_bucket *tw;
			tw = (struct tcp_tw_bucket *)ixs->skb->sk;
			ixs->sport = ntohs(tw->tw_sport);
			ixs->dport = ntohs(tw->tw_dport);
#endif
#else
			ixs->sport = ntohs(ixs->skb->sk->sport);
			ixs->dport = ntohs(ixs->skb->sk->dport);
#endif
		} 

		if(t != NULL) {
			if(ixs->sport == 0) {
				ixs->sport = ntohs(t->source);
			}
			if(ixs->dport == 0) {
				ixs->dport = ntohs(t->dest);
			}
		}
	}
	
	/* default to a %drop eroute */
	ixs->outgoing_said.proto = IPPROTO_INT;
	ixs->outgoing_said.spi = htonl(SPI_DROP);
	ixs->outgoing_said.dst.u.v4.sin_addr.s_addr = INADDR_ANY;
	KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
		    "klips_debug:ipsec_xmit_SAlookup: "
		    "checking for local udp/500 IKE packet "
		    "saddr=%x, er=0p%p, daddr=%x, er_dst=%x, proto=%d sport=%d dport=%d\n",
		    ntohl((unsigned int)ixs->iph->saddr),
		    ixs->eroute,
		    ntohl((unsigned int)ixs->iph->daddr),
		    ixs->eroute ? ntohl((unsigned int)ixs->eroute->er_said.dst.u.v4.sin_addr.s_addr) : 0,
		    ixs->iph->protocol,
		    ixs->sport,
		    ixs->dport); 

	/*
	 * cheat for now...are we udp/500? If so, let it through
	 * without interference since it is most likely an IKE packet.
	 */

	if (ip_chk_addr((unsigned long)ixs->iph->saddr) == IS_MYADDR
	    && (ixs->eroute==NULL
		|| ixs->iph->daddr == ixs->eroute->er_said.dst.u.v4.sin_addr.s_addr
		|| INADDR_ANY == ixs->eroute->er_said.dst.u.v4.sin_addr.s_addr)
	    && (ixs->iph->protocol == IPPROTO_UDP &&
		(ixs->sport == 500 || ixs->sport == 4500))) {
		/* Whatever the eroute, this is an IKE message 
		 * from us (i.e. not being forwarded).
		 * Furthermore, if there is a tunnel eroute,
		 * the destination is the peer for this eroute.
		 * So %pass the packet: modify the default %drop.
		 */

		ixs->outgoing_said.spi = htonl(SPI_PASS);
		if(!(ixs->skb->sk) && ((ntohs(ixs->iph->frag_off) & IP_MF) != 0)) {
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_SAlookup: "
				    "local UDP/500 (probably IKE) passthrough: base fragment, rest of fragments will probably get filtered.\n");
		}
		bypass = TRUE;
	}

	if (bypass==FALSE && ixs->eroute) {
		ixs->eroute->er_count++;
		ixs->eroute->er_lasttime = jiffies/HZ;
		if(ixs->eroute->er_said.proto==IPPROTO_INT
		   && ixs->eroute->er_said.spi==htonl(SPI_HOLD))
		{
			KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				    "klips_debug:ipsec_xmit_SAlookup: "
				    "shunt SA of HOLD: skb stored in HOLD.\n");
			if(ixs->eroute->er_last != NULL) {
				ipsec_kfree_skb(ixs->eroute->er_last);
				ixs->stats->tx_dropped++;
			}
			ixs->eroute->er_last = ixs->skb;
			ixs->skb = NULL;
			spin_unlock_bh(&eroute_lock);
			return IPSEC_XMIT_STOLEN;
		}
		ixs->outgoing_said = ixs->eroute->er_said;
		ixs->eroute_pid = ixs->eroute->er_pid;

		/* Copy of the ident for the TRAP/TRAPSUBNET eroutes */
		if(ixs->outgoing_said.proto==IPPROTO_INT
		   && (ixs->outgoing_said.spi==htonl(SPI_TRAP)
		       || (ixs->outgoing_said.spi==htonl(SPI_TRAPSUBNET)))) {
			int len;
			
			ixs->ips.ips_ident_s.type = ixs->eroute->er_ident_s.type;
			ixs->ips.ips_ident_s.id = ixs->eroute->er_ident_s.id;
			ixs->ips.ips_ident_s.len = ixs->eroute->er_ident_s.len;
			if (ixs->ips.ips_ident_s.len)
			{
				len = ixs->ips.ips_ident_s.len * IPSEC_PFKEYv2_ALIGN - sizeof(struct sadb_ident);
				KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
					    "klips_debug:ipsec_xmit_SAlookup: "
					    "allocating %d bytes for ident_s shunt SA of HOLD: skb stored in HOLD.\n",
					    len);
				if ((ixs->ips.ips_ident_s.data = kmalloc(len, GFP_ATOMIC)) == NULL) {
					printk(KERN_WARNING "klips_debug:ipsec_xmit_SAlookup: "
					       "Failed, tried to allocate %d bytes for source ident.\n", 
					       len);
					ixs->stats->tx_dropped++;
					spin_unlock_bh(&eroute_lock);
					return IPSEC_XMIT_ERRMEMALLOC;
				}
				memcpy(ixs->ips.ips_ident_s.data, ixs->eroute->er_ident_s.data, len);
			}
			ixs->ips.ips_ident_d.type = ixs->eroute->er_ident_d.type;
			ixs->ips.ips_ident_d.id = ixs->eroute->er_ident_d.id;
			ixs->ips.ips_ident_d.len = ixs->eroute->er_ident_d.len;
			if (ixs->ips.ips_ident_d.len)
			{
				len = ixs->ips.ips_ident_d.len * IPSEC_PFKEYv2_ALIGN - sizeof(struct sadb_ident);
				KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
					    "klips_debug:ipsec_xmit_SAlookup: "
					    "allocating %d bytes for ident_d shunt SA of HOLD: skb stored in HOLD.\n",
					    len);
				if ((ixs->ips.ips_ident_d.data = kmalloc(len, GFP_ATOMIC)) == NULL) {
					printk(KERN_WARNING "klips_debug:ipsec_xmit_SAlookup: "
					       "Failed, tried to allocate %d bytes for dest ident.\n", 
					       len);
					ixs->stats->tx_dropped++;
					spin_unlock_bh(&eroute_lock);
					return IPSEC_XMIT_ERRMEMALLOC;
				}
				memcpy(ixs->ips.ips_ident_d.data, ixs->eroute->er_ident_d.data, len);
			}
		}
	}

	spin_unlock_bh(&eroute_lock);
	return IPSEC_XMIT_OK;
}


enum ipsec_xmit_value
ipsec_tunnel_restore_hard_header(struct ipsec_xmit_state*ixs)
{
	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_restore_hard_header: "
		    "After recursive xforms -- head,tailroom: %d,%d\n",
		    skb_headroom(ixs->skb),
		    skb_tailroom(ixs->skb));

	if(ixs->saved_header) {
		if(skb_headroom(ixs->skb) < ixs->hard_header_len) {
			printk(KERN_WARNING
			       "klips_error:ipsec_xmit_restore_hard_header: "
			       "tried to skb_push hhlen=%d, %d available.  This should never happen, please report.\n",
			       ixs->hard_header_len,
			       skb_headroom(ixs->skb));
			ixs->stats->tx_errors++;
			return IPSEC_XMIT_PUSHPULLERR;

		}
		skb_push(ixs->skb, ixs->hard_header_len);
		{
			int i;
			for (i = 0; i < ixs->hard_header_len; i++) {
				ixs->skb->data[i] = ixs->saved_header[i];
			}
		}
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_CROUT,
		    "klips_debug:ipsec_xmit_restore_hard_header: "
		    "With hard_header, final head,tailroom: %d,%d\n",
		    skb_headroom(ixs->skb),
		    skb_tailroom(ixs->skb));

	return IPSEC_XMIT_OK;
}


/*
 * when encap processing is complete it call this for us to continue
 */

void
ipsec_tunnel_xsm_complete(
	struct ipsec_xmit_state *ixs,
	enum ipsec_xmit_value stat)
{
	if(stat != IPSEC_XMIT_OK) {
		if(stat == IPSEC_XMIT_PASS) {
			goto bypass;
		}
		
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
				"klips_debug:ipsec_tunnel_start_xmit: encap_bundle failed: %d\n",
				stat);
		goto cleanup;
	}

	ixs->matcher.sen_ip_src.s_addr = ixs->iph->saddr;
	ixs->matcher.sen_ip_dst.s_addr = ixs->iph->daddr;
	ixs->matcher.sen_proto = ixs->iph->protocol;
	ipsec_extract_ports(ixs->iph, &ixs->matcher);

	spin_lock_bh(&eroute_lock);
	ixs->eroute = ipsec_findroute(&ixs->matcher);
	if(ixs->eroute) {
		ixs->outgoing_said = ixs->eroute->er_said;
		ixs->eroute_pid = ixs->eroute->er_pid;
		ixs->eroute->er_count++;
		ixs->eroute->er_lasttime = jiffies/HZ;
	}
	spin_unlock_bh(&eroute_lock);

	KLIPS_PRINT((debug_tunnel & DB_TN_XMIT) &&
			/* ((ixs->orgdst != ixs->newdst) || (ixs->orgsrc != ixs->newsrc)) */
			(ixs->orgedst != ixs->outgoing_said.dst.u.v4.sin_addr.s_addr) &&
			ixs->outgoing_said.dst.u.v4.sin_addr.s_addr &&
			ixs->eroute,
			"klips_debug:ipsec_tunnel_start_xmit: "
			"We are recursing here.\n");

	if (/*((ixs->orgdst != ixs->newdst) || (ixs->orgsrc != ixs->newsrc))*/
			(ixs->orgedst != ixs->outgoing_said.dst.u.v4.sin_addr.s_addr) &&
			ixs->outgoing_said.dst.u.v4.sin_addr.s_addr &&
			ixs->eroute) {
		ipsec_xsm(ixs);
		return;
	}

#ifdef NAT_TRAVERSAL
	stat = ipsec_nat_encap(ixs);
	if(stat != IPSEC_XMIT_OK) {
		goto cleanup;
	}
#endif

	stat = ipsec_tunnel_restore_hard_header(ixs);
	if(stat != IPSEC_XMIT_OK) {
		goto cleanup;
	}

bypass:
	stat = ipsec_tunnel_send(ixs);

cleanup:
	ipsec_xmit_cleanup(ixs);
	ipsec_xmit_state_delete(ixs);
}


/*
 *	This function assumes it is being called from dev_queue_xmit()
 *	and that skb is filled properly by that function.
 */
int
ipsec_tunnel_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ipsec_xmit_state *ixs = NULL;
	enum ipsec_xmit_value stat;

	KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
		    "\n\nipsec_tunnel_start_xmit: STARTING");

	stat = IPSEC_XMIT_ERRMEMALLOC;
	ixs = ipsec_xmit_state_new();
	if (! ixs) {
		netif_stop_queue(dev);
		return NETDEV_TX_BUSY;
	}

	ixs->dev = dev;
	ixs->skb = skb;

	stat = ipsec_xmit_sanity_check_dev(ixs);
	if(stat != IPSEC_XMIT_OK) {
		goto cleanup;
	}

	stat = ipsec_xmit_sanity_check_skb(ixs);
	if(stat != IPSEC_XMIT_OK) {
		goto cleanup;
	}

	stat = ipsec_tunnel_strip_hard_header(ixs);
	if(stat != IPSEC_XMIT_OK) {
		goto cleanup;
	}

	stat = ipsec_tunnel_SAlookup(ixs);
	if(stat != IPSEC_XMIT_OK) {
		KLIPS_PRINT(debug_tunnel & DB_TN_XMIT,
			    "klips_debug:ipsec_tunnel_start_xmit: SAlookup failed: %d\n",
			    stat);
		goto cleanup;
	}
	
	ixs->xsm_complete = ipsec_tunnel_xsm_complete;

	ipsec_xsm(ixs);
	return 0;

 cleanup:
	ipsec_xmit_cleanup(ixs);
	ipsec_xmit_state_delete(ixs);
	return 0;
}

DEBUG_NO_STATIC struct net_device_stats *
ipsec_tunnel_get_stats(struct net_device *dev)
{
	return &(((struct ipsecpriv *)netdev_priv(dev))->mystats);
}

/*
 * Revectored calls.
 * For each of these calls, a field exists in our private structure.
 */

DEBUG_NO_STATIC int
ipsec_tunnel_hard_header(struct sk_buff *skb, struct net_device *dev,
	unsigned short type, const void *daddr, const void *saddr, unsigned len)
{
	struct ipsecpriv *prv = netdev_priv(dev);
	struct net_device *tmp;
	int ret;
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(skb == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no skb...\n");
		return -ENODATA;
	}

	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no device...\n");
		return -ENODEV;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel_hard_header: "
		    "skb->dev=%s dev=%s.\n",
		    skb->dev ? skb->dev->name : "NULL",
		    dev->name);
	
	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no private space associated with dev=%s\n",
			    dev->name ? dev->name : "NULL");
		return -ENODEV;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "no physical device associated with dev=%s\n",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return -ENODEV;
	}

	/* check if we have to send a IPv6 packet. It might be a Router
	   Solicitation, where the building of the packet happens in
	   reverse order:
	   1. ll hdr,
	   2. IPv6 hdr,
	   3. ICMPv6 hdr
	   -> skb->nh.raw is still uninitialized when this function is
	   called!!  If this is no IPv6 packet, we can print debugging
	   messages, otherwise we skip all debugging messages and just
	   build the ll header */
	if(type != ETH_P_IPV6) {
		/* execute this only, if we don't have to build the
		   header for a IPv6 packet */
#ifdef HAVE_NETDEV_HEADER_OPS
		if(!prv->header_ops->create)
#else
		if(!prv->hard_header)
#endif
		{
			KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
				    "klips_debug:ipsec_tunnel_hard_header: "
				    "physical device has been detached, packet dropped 0p%p->0p%p len=%d type=%d dev=%s->NULL ",
				    saddr,
				    daddr,
				    len,
				    type,
				    dev->name);
#ifdef NET_21
			KLIPS_PRINTMORE(debug_tunnel & DB_TN_REVEC,
					"ip=%08x->%08x\n",
					(__u32)ntohl(ip_hdr(skb)->saddr),
					(__u32)ntohl(ip_hdr(skb)->daddr) );
#else /* NET_21 */
			KLIPS_PRINTMORE(debug_tunnel & DB_TN_REVEC,
					"ip=%08x->%08x\n",
					(__u32)ntohl(skb->ip_hdr->saddr),
					(__u32)ntohl(skb->ip_hdr->daddr) );
#endif /* NET_21 */
			stats->tx_dropped++;
			return -ENODEV;
		}
		
#define da ((struct net_device *)(prv->dev))->dev_addr
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "Revectored 0p%p->0p%p len=%d type=%d dev=%s->%s dev_addr=%02x:%02x:%02x:%02x:%02x:%02x ",
			    saddr,
			    daddr,
			    len,
			    type,
			    dev->name,
			    prv->dev->name,
			    da[0], da[1], da[2], da[3], da[4], da[5]);
#ifdef NET_21
		KLIPS_PRINTMORE(debug_tunnel & DB_TN_REVEC,
			    "ip=%08x->%08x\n",
			    (__u32)ntohl(ip_hdr(skb)->saddr),
			    (__u32)ntohl(ip_hdr(skb)->daddr) );
#else /* NET_21 */
		KLIPS_PRINTMORE(debug_tunnel & DB_TN_REVEC,
			    "ip=%08x->%08x\n",
			    (__u32)ntohl(skb->ip_hdr->saddr),
			    (__u32)ntohl(skb->ip_hdr->daddr) );
#endif /* NET_21 */
	} else {
		KLIPS_PRINT(debug_tunnel,
			    "klips_debug:ipsec_tunnel_hard_header: "
			    "is IPv6 packet, skip debugging messages, only revector and build linklocal header.\n");
	}                                                                       
	tmp = skb->dev;
	skb->dev = prv->dev;
#ifdef HAVE_NETDEV_HEADER_OPS
	ret = prv->header_ops->create(skb, prv->dev, type, (void *)daddr, (void *)saddr, len);
#else
	ret = prv->hard_header(skb, prv->dev, type, (void *)daddr, (void *)saddr, len);
#endif
	skb->dev = tmp;
	return ret;
}

DEBUG_NO_STATIC int
#ifdef NET_21
ipsec_tunnel_rebuild_header(struct sk_buff *skb)
#else /* NET_21 */
ipsec_tunnel_rebuild_header(void *buff, struct net_device *dev,
			unsigned long raddr, struct sk_buff *skb)
#endif /* NET_21 */
{
	struct ipsecpriv *prv = netdev_priv(skb->dev);
	struct net_device *tmp;
	int ret;
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(skb->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "no device...");
		return -ENODEV;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "no private space associated with dev=%s",
			    skb->dev->name ? skb->dev->name : "NULL");
		return -ENODEV;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "no physical device associated with dev=%s",
			    skb->dev->name ? skb->dev->name : "NULL");
		stats->tx_dropped++;
		return -ENODEV;
	}

#ifdef HAVE_NETDEV_HEADER_OPS
	if(!prv->header_ops->rebuild)
#else
	if(!prv->rebuild_header)
#endif
	{
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_rebuild_header: "
			    "physical device has been detached, packet dropped skb->dev=%s->NULL ",
			    skb->dev->name);
#ifdef NET_21
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "ip=%08x->%08x\n",
			    (__u32)ntohl(ip_hdr(skb)->saddr),
			    (__u32)ntohl(ip_hdr(skb)->daddr) );
#else /* NET_21 */
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "ip=%08x->%08x\n",
			    (__u32)ntohl(skb->ip_hdr->saddr),
			    (__u32)ntohl(skb->ip_hdr->daddr) );
#endif /* NET_21 */
		stats->tx_dropped++;
		return -ENODEV;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel: "
		    "Revectored rebuild_header dev=%s->%s ",
		    skb->dev->name, prv->dev->name);
#ifdef NET_21
	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "ip=%08x->%08x\n",
		    (__u32)ntohl(ip_hdr(skb)->saddr),
		    (__u32)ntohl(ip_hdr(skb)->daddr) );
#else /* NET_21 */
	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "ip=%08x->%08x\n",
		    (__u32)ntohl(skb->ip_hdr->saddr),
		    (__u32)ntohl(skb->ip_hdr->daddr) );
#endif /* NET_21 */
	tmp = skb->dev;
	skb->dev = prv->dev;

#ifdef HAVE_NETDEV_HEADER_OPS
	ret = prv->header_ops->rebuild(skb);
#else
#ifdef NET_21
	ret = prv->rebuild_header(skb);
#else /* NET_21 */
	ret = prv->rebuild_header(buff, prv->dev, raddr, skb);
#endif /* NET_21 */
#endif
	skb->dev = tmp;
	return ret;
}

#ifdef HAVE_SET_MAC_ADDR
DEBUG_NO_STATIC int
ipsec_tunnel_set_mac_address(struct net_device *dev, void *addr)
{
	struct ipsecpriv *prv = netdev_priv(dev);
	
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_set_mac_address: "
			    "no device...");
		return -ENODEV;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_set_mac_address: "
			    "no private space associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		return -ENODEV;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_set_mac_address: "
			    "no physical device associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return -ENODEV;
	}

	if(!prv->dev) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_set_mac_address: "
			    "physical device has been detached, cannot set - skb->dev=%s->NULL\n",
			    dev->name);
		return -ENODEV;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel_set_mac_address: "
		    "Revectored dev=%s->%s addr=0p%p\n",
		    dev->name, prv->dev->name, addr);
	return prv->set_mac_address(prv->dev, addr);

}
#endif /* HAVE_SET_MAC_ADDR */

#ifndef NET_21
DEBUG_NO_STATIC void
ipsec_tunnel_cache_bind(struct hh_cache **hhp, struct net_device *dev,
				 unsigned short htype, __u32 daddr)
{
	struct ipsecpriv *prv = (struct ipsecpriv *) netdev_priv(dev);
	
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_bind: "
			    "no device...");
		return;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_bind: "
			    "no private space associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		return;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_bind: "
			    "no physical device associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return;
	}

	if(!prv->header_cache_bind) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_bind: "
			    "physical device has been detached, cannot set - skb->dev=%s->NULL\n",
			    dev->name);
		stats->tx_dropped++;
		return;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel_cache_bind: "
		    "Revectored \n");
	prv->header_cache_bind(hhp, prv->dev, htype, daddr);
	return;
}
#endif /* !NET_21 */


DEBUG_NO_STATIC void
ipsec_tunnel_cache_update(struct hh_cache *hh, const struct net_device *dev,
				const unsigned char *  haddr)
{
	struct ipsecpriv *prv = netdev_priv(dev);
	
	struct net_device_stats *stats;	/* This device's statistics */
	
	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "no device...");
		return;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "no private space associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		return;
	}

	stats = (struct net_device_stats *) &(prv->mystats);

	if(prv->dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "no physical device associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		stats->tx_dropped++;
		return;
	}

#ifdef HAVE_NETDEV_HEADER_OPS
	if(!prv->header_ops->cache_update)
#else
	if(!prv->header_cache_update)
#endif
	{
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_cache_update: "
			    "physical device has been detached, cannot set - skb->dev=%s->NULL\n",
			    dev->name);
		return;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel: "
		    "Revectored cache_update\n");
#ifdef HAVE_NETDEV_HEADER_OPS
	prv->header_ops->cache_update(hh, prv->dev, haddr);
#else
	prv->header_cache_update(hh, prv->dev, haddr);
#endif
	return;
}

#ifdef HAVE_NETDEV_HEADER_OPS
const struct header_ops ipsec_tunnel_header_ops = {
	.create		= ipsec_tunnel_hard_header,
	.rebuild	= ipsec_tunnel_rebuild_header,
	.cache_update	= ipsec_tunnel_cache_update,
};
#endif

#ifdef NET_21
DEBUG_NO_STATIC int
ipsec_tunnel_neigh_setup(struct neighbour *n)
{
	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel_neigh_setup:\n");

        if (n->nud_state == NUD_NONE) {
                n->ops = &arp_broken_ops;
                n->output = n->ops->output;
        }
        return 0;
}

DEBUG_NO_STATIC int
ipsec_tunnel_neigh_setup_dev(struct net_device *dev, struct neigh_parms *p)
{
	KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
		    "klips_debug:ipsec_tunnel_neigh_setup_dev: "
		    "setting up %s\n",
		    dev ? dev->name : "NULL");

        if (p->tbl->family == AF_INET) {
                p->neigh_setup = ipsec_tunnel_neigh_setup;
                p->ucast_probes = 0;
                p->mcast_probes = 0;
        }
        return 0;
}
#endif /* NET_21 */

/*
 * We call the detach routine to detach the ipsec tunnel from another device.
 */

DEBUG_NO_STATIC int
ipsec_tunnel_detach(struct net_device *dev)
{
        int i;
	struct ipsecpriv *prv = netdev_priv(dev);

	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_detach: "
			    "no device...");
		return -ENODEV;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_detach: "
			    "no private space associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		return -ENODATA;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_detach: "
		    "physical device %s being detached from virtual device %s\n",
		    prv->dev ? prv->dev->name : "NULL",
		    dev->name);

	ipsec_dev_put(prv->dev);
	prv->dev = NULL;
	prv->hard_start_xmit = NULL;
	prv->get_stats = NULL;

#ifdef HAVE_NETDEV_HEADER_OPS
	prv->header_ops = NULL;
#else
	prv->hard_header = NULL;
	prv->rebuild_header = NULL;
	prv->header_cache_update = NULL;
#ifndef NET_21
	prv->header_cache_bind = NULL;
#else
/*	prv->neigh_setup        = NULL; */
#endif
#endif
	prv->set_mac_address = NULL;
	dev->hard_header_len = 0;

#ifdef DETACH_AND_DOWN
#ifdef HAVE_NETDEV_HEADER_OPS
	dev->header_ops = NULL;
#else
	dev->hard_header = NULL;
	dev->rebuild_header = NULL;
	dev->header_cache_update = NULL;
#ifndef NET_21
	dev->header_cache_bind = NULL;
#else
	dev->neigh_setup        = NULL;
#endif
#endif
#ifdef HAVE_SET_MAC_ADDR
	dev->set_mac_address = NULL;
#endif
	dev->mtu = 0;
#endif /* DETACH_AND_DOWN */
	
	prv->mtu = 0;
	for (i=0; i<MAX_ADDR_LEN; i++) {
		dev->dev_addr[i] = 0;
	}
	dev->addr_len = 0;
#ifdef PHYSDEV_TYPE
	dev->type = ARPHRD_VOID; /* ARPHRD_TUNNEL; */
#endif /*  PHYSDEV_TYPE */
	
	return 0;
}

/*
 * We call the clear routine to detach all ipsec tunnels from other devices.
 */
DEBUG_NO_STATIC int
ipsec_tunnel_clear(void)
{
	int i;
	struct net_device *ipsecdev = NULL, *prvdev;
	struct ipsecpriv *prv;
	int ret;

	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_clear: .\n");

	for(i = 0; i < IPSEC_NUM_IF; i++) {
   	        ipsecdev = ipsecdevices[i];
		if(ipsecdev != NULL) {
			if((prv = (struct ipsecpriv *)netdev_priv(ipsecdev))) {
				prvdev = (struct net_device *)(prv->dev);
				if(prvdev) {
					KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
						    "klips_debug:ipsec_tunnel_clear: "
						    "physical device for device %s is %s\n",
						    ipsecdev->name, prvdev->name);
					if((ret = ipsec_tunnel_detach(ipsecdev))) {
						KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
							    "klips_debug:ipsec_tunnel_clear: "
							    "error %d detatching device %s from device %s.\n",
							    ret, ipsecdev->name, prvdev->name);
						return ret;
					}
				}
			}
		}
	}
	return 0;
}

/* 
 * Used mostly for KLIPS to setup interface, for also with NETKEY when using
 * 2.6.23+ UDP XFRM code to mark sockets UDP_ENCAP_ESPINUDP_NON_IKE
 */
DEBUG_NO_STATIC int
ipsec_tunnel_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	/* struct ipsectunnelconf *cf = (struct ipsectunnelconf *)&ifr->ifr_data;*/
	/* overlay our struct ipsectunnel onto ifr.ifr_ifru union (hope it fits!) */
	struct ipsectunnelconf *cf=(struct ipsectunnelconf *)ifr->ifr_ifru.ifru_newname;
	struct ipsecpriv *prv = netdev_priv(dev);
	struct net_device *them; /* physical device */
#ifdef CONFIG_IP_ALIAS
	char *colon;
	char realphysname[IFNAMSIZ];
#endif /* CONFIG_IP_ALIAS */
	
	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_ioctl: "
			    "device not supplied.\n");
		return -ENODEV;
	}

	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_ioctl: "
		    "tncfg service call #%d for dev=%s\n",
		    cmd,
		    dev->name ? dev->name : "NULL");
	switch (cmd) {
#if defined(KLIPS)
	/* attach a virtual ipsec? device to a physical device */
	case IPSEC_SET_DEV:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_ioctl: "
			    "calling ipsec_tunnel_attatch...\n");
#ifdef CONFIG_IP_ALIAS
		/* If this is an IP alias interface, get its real physical name */
		strncpy(realphysname, cf->cf_name, IFNAMSIZ);
		realphysname[IFNAMSIZ-1] = 0;
		colon = strchr(realphysname, ':');
		if (colon) *colon = 0;
		them = ipsec_dev_get(realphysname);
#else /* CONFIG_IP_ALIAS */
		them = ipsec_dev_get(cf->cf_name);
#endif /* CONFIG_IP_ALIAS */

		if (them == NULL) {
			KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
				    "klips_debug:ipsec_tunnel_ioctl: "
				    "physical device %s requested is null\n",
				    cf->cf_name);
			return -ENXIO;
		}
		
#if 0
		if (them->flags & IFF_UP) {
			KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
				    "klips_debug:ipsec_tunnel_ioctl: "
				    "physical device %s requested is not up.\n",
				    cf->cf_name);
			ipsec_dev_put(them);
			return -ENXIO;
		}
#endif
		
		if (prv && prv->dev) {
			KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
				    "klips_debug:ipsec_tunnel_ioctl: "
				    "virtual device is already connected to %s.\n",
				    prv->dev->name ? prv->dev->name : "NULL");
			ipsec_dev_put(them);
			return -EBUSY;
		}
		return ipsec_tunnel_attach(dev, them);

	case IPSEC_DEL_DEV:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_ioctl: "
			    "calling ipsec_tunnel_detatch.\n");
		if (! prv->dev) {
			KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
				    "klips_debug:ipsec_tunnel_ioctl: "
				    "physical device not connected.\n");
			return -ENODEV;
		}
		return ipsec_tunnel_detach(dev);
	       
	case IPSEC_CLR_DEV:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_ioctl: "
			    "calling ipsec_tunnel_clear.\n");
		return ipsec_tunnel_clear();
#endif /* KLIPS */

#ifdef HAVE_UDP_ENCAP_CONVERT
	/*
	 * we get two int args in ifr_data, the socket fd (0)
	 * and the type of encapsulation needed (1).
	 */
	case IPSEC_UDP_ENCAP_CONVERT:
	{
		/*unsigned int *ifp =(unsigned int *)&ifr->ifr_data;*/
		/* overlay our struct ipsectunnel onto ifr.ifr_ifru union (hope it fits!) */
		unsigned int *ifp =(unsigned int *)ifr->ifr_ifru.ifru_newname;

		return ipsec_tunnel_udp_encap_prepare(ifp[0], ifp[1]);
	}
#endif /* HAVE_UDP_ENCAP_CONVERT */

	default:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_ioctl: "
			    "unknown command %d.\n",
			    cmd);
		return -EOPNOTSUPP;

	}
}

#ifdef HAVE_UDP_ENCAP_CONVERT

#define IPSEC_TUNNEL_UDP_ENCAP_MAGIC 0x58c0b472
struct ipsec_tunnel_udp_encap_ctx {
	u32 magic;
	atomic_t refcnt;
	struct list_head link;
	struct sock *sk;
	void (*old_sk_destruct)(struct sock *sk);
};
#define list_head_to_udp_encap_ctx(lh) \
	list_entry(lh, struct ipsec_tunnel_udp_encap_ctx, link)

static spinlock_t ipsec_tunnel_udp_encap_lock = SPIN_LOCK_UNLOCKED;
static LIST_HEAD(ipsec_tunnel_udp_encap_list);

DEBUG_NO_STATIC int ipsec_tunnel_udp_encap_prepare(int fd, int encap_type)
{
	struct socket *sock = NULL;
	struct sock *sk;
	struct ipsec_tunnel_udp_encap_ctx *ctx = NULL;
	int err = 0;

	switch (encap_type) {
	case UDP_ENCAP_ESPINUDP:
	case UDP_ENCAP_ESPINUDP_NON_IKE:
		break;
	default:
		err = -EINVAL;
		printk ("ipsec: pid %d sent fd %d with invalid encap_type %d\n",
				fd, current->pid, encap_type);
		goto error;
	}

	/* translate descriptor to socket structure */
	err = -EBADF;
	sock = sockfd_lookup(fd, &err);
	if (!sock) {
		printk ("ipsec: failed to lookup socket for fd %d for pid %d\n",
				fd, current->pid);
		goto error;
	}
	sk = sock->sk;

	/* Quick sanity checks */
	err = -EAFNOSUPPORT;
	if (sock->ops->family != AF_INET) {
		printk ("ipsec: pid %d sent fd %d with wrong family, "
				"got %d, expected %d\n",
				current->pid, fd, sock->ops->family, AF_INET);
		goto error;
	}

	err = -EPROTONOSUPPORT;
	if (sk->sk_protocol != IPPROTO_UDP) {
		printk ("ipsec: pid %d sent fd %d with wrong protocol, "
				"got %d, expected %d\n",
				current->pid, fd, sk->sk_protocol, IPPROTO_UDP);
		goto error;
	}

	err = -EBUSY;
	if (udp_sk(sk)->encap_type) {
		printk ("ipsec: pid %d sent fd %d with encap_type "
				"assigned to %d",
				current->pid, fd, udp_sk(sk)->encap_type);
		goto error;
	}

	err = -EBUSY;
	ctx = (struct ipsec_tunnel_udp_encap_ctx*)sk->sk_user_data;
	if (ctx) {
		printk ("ipsec: pid %d sent fd %d with user_data assigned\n",
				current->pid, fd);
		goto error;
	}

	err = -ENOMEM;
	sk->sk_user_data = ctx = kzalloc(sizeof (*ctx), GFP_KERNEL);
	if (!ctx)
		goto error;

	/* setup the context */
	ctx->magic           = IPSEC_TUNNEL_UDP_ENCAP_MAGIC;
	ctx->sk              = sk;
	ctx->old_sk_destruct = sk->sk_destruct;
	wmb();

	/* one ref is for sk other for the list */
	atomic_set(&ctx->refcnt, 2);

	/* convert socket to use our (de)encapsulation routine */
	sk->sk_destruct        = ipsec_tunnel_udp_encap_destruct;
	udp_sk(sk)->encap_type = encap_type;
	udp_sk(sk)->encap_rcv  = klips26_udp_encap_rcv;

	/* add the tunnel to our list */
	spin_lock_bh(&ipsec_tunnel_udp_encap_lock);
	list_add(&ctx->link, &ipsec_tunnel_udp_encap_list);
	spin_unlock_bh(&ipsec_tunnel_udp_encap_lock);

	KLIPS_PRINT(debug_tunnel
			, "UDP socket: %u set to %s (0x%x) encap mode\n"
			, fd
			, encap_type == UDP_ENCAP_ESPINUDP_NON_IKE ?
			"UDP_ENCAP_ESPINUDP_NON_IKE" : "UDP_ENCAP_ESPINUDP_NON_ESP"
			, encap_type
		   );

	/* success */
	err = 0;

error:
	if (sock)
		sockfd_put(sock);
	return err;
}

DEBUG_NO_STATIC void ipsec_tunnel_udp_encap_restore(
		struct ipsec_tunnel_udp_encap_ctx *ctx)
{
	struct sock *sk = ctx->sk;

	/* revert the socket back */
	udp_sk(sk)->encap_type = 0;
	udp_sk(sk)->encap_rcv  = NULL;

	sk->sk_user_data = NULL;
	sk->sk_destruct = ctx->old_sk_destruct;

	if (sk->sk_destruct)
		sk->sk_destruct(sk);
}

DEBUG_NO_STATIC void ipsec_tunnel_udp_encap_destruct(struct sock *sk)
{
	struct ipsec_tunnel_udp_encap_ctx *ctx;

	if (!sk || !sk->sk_user_data)
		return;

	ctx = (struct ipsec_tunnel_udp_encap_ctx*)sk->sk_user_data;
	if (ctx->magic != IPSEC_TUNNEL_UDP_ENCAP_MAGIC) {
		printk ("ipsec: called to destroy ctx with wrong magic, "
				"got %08x, expected %08x\n",
				ctx->magic, IPSEC_TUNNEL_UDP_ENCAP_MAGIC);
		return;
	}

	ipsec_tunnel_udp_encap_restore(ctx);

	/* remove it from the list */
	spin_lock_bh(&ipsec_tunnel_udp_encap_lock);
	if (ctx->link.next) {
		/* we are on the list, remove and deref */
		list_del(&ctx->link);
		atomic_dec(&ctx->refcnt);
	} else {
		/* it was already removed, someone else will clean up */
		ctx = NULL;
	}
	spin_unlock_bh(&ipsec_tunnel_udp_encap_lock);

	/* if we removed it from the list, then we need to free it */
	if (ctx && atomic_dec_and_test(&ctx->refcnt))
		kfree(ctx);
}

DEBUG_NO_STATIC void ipsec_tunnel_upd_encap_cleanup(void)
{
	struct ipsec_tunnel_udp_encap_ctx *ctx;

	for (;;) {
		ctx = NULL;
		spin_lock_bh(&ipsec_tunnel_udp_encap_lock);
		if (!list_empty(&ipsec_tunnel_udp_encap_list)) {
			struct list_head *ent;
			ent = ipsec_tunnel_udp_encap_list.next;
			list_del(ent);
			ctx = list_head_to_udp_encap_ctx(ent);
		}
		spin_unlock_bh(&ipsec_tunnel_udp_encap_lock);

		/* are we done? */
		if (!ctx)
			break;

		ipsec_tunnel_udp_encap_restore(ctx);

		if (atomic_dec_and_test(&ctx->refcnt))
			kfree(ctx);
	}
}
#endif



struct net_device *ipsec_get_device(int inst)
{
  struct net_device *ipsec_dev;

  ipsec_dev = NULL;

  if(inst < IPSEC_NUM_IF) {
    ipsec_dev = ipsecdevices[inst];
  }

  return ipsec_dev;
}

int
ipsec_device_event(struct notifier_block *unused, unsigned long event, void *ptr)
{
	struct net_device *dev = ptr;
	struct net_device *ipsec_dev;
	struct ipsecpriv *priv;
	int i;

	if (dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "dev=NULL for event type %ld.\n",
			    event);
		return(NOTIFY_DONE);
	}

	/* check for loopback devices */
	if (dev && (dev->flags & IFF_LOOPBACK)) {
		return(NOTIFY_DONE);
	}

	switch (event) {
	case NETDEV_DOWN:
		/* look very carefully at the scope of these compiler
		   directives before changing anything... -- RGB */
#ifdef NET_21
	case NETDEV_UNREGISTER:
		switch (event) {
		case NETDEV_DOWN:
#endif /* NET_21 */
			KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
				    "klips_debug:ipsec_device_event: "
				    "NETDEV_DOWN dev=%s flags=%x\n",
				    dev->name,
				    dev->flags);
			if(strncmp(dev->name, "ipsec", strlen("ipsec")) == 0) {
				printk(KERN_CRIT "IPSEC EVENT: KLIPS device %s shut down.\n",
				       dev->name);
			}
#ifdef NET_21
			break;
		case NETDEV_UNREGISTER:
			KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
				    "klips_debug:ipsec_device_event: "
				    "NETDEV_UNREGISTER dev=%s flags=%x\n",
				    dev->name,
				    dev->flags);
			break;
		}
#endif /* NET_21 */
		
		/* find the attached physical device and detach it. */
		for(i = 0; i < IPSEC_NUM_IF; i++) {
			ipsec_dev = ipsecdevices[i];

			if(ipsec_dev) {
				priv = (struct ipsecpriv *)netdev_priv(ipsec_dev);
				if(priv) {
					;
					if(((struct net_device *)(priv->dev)) == dev) {
						/* dev_close(ipsec_dev); */
						/* return */ ipsec_tunnel_detach(ipsec_dev);
						KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
							    "klips_debug:ipsec_device_event: "
							    "device '%s' has been detached.\n",
							    ipsec_dev->name);
						break;
					}
				} else {
					KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
						    "klips_debug:ipsec_device_event: "
						    "device '%s' has no private data space!\n",
						    ipsec_dev->name);
				}
			}
		}
		break;
	case NETDEV_UP:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_UP dev=%s\n",
			    dev->name);
		break;
#ifdef NET_21
	case NETDEV_REBOOT:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_REBOOT dev=%s\n",
			    dev->name);
		break;
	case NETDEV_CHANGE:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_CHANGE dev=%s flags=%x\n",
			    dev->name,
			    dev->flags);
		break;
	case NETDEV_REGISTER:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_REGISTER dev=%s\n",
			    dev->name);
		break;
	case NETDEV_CHANGEMTU:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_CHANGEMTU dev=%s to mtu=%d\n",
			    dev->name,
			    dev->mtu);
		break;
	case NETDEV_CHANGEADDR:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_CHANGEADDR dev=%s\n",
			    dev->name);
		break;
	case NETDEV_GOING_DOWN:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_GOING_DOWN dev=%s\n",
			    dev->name);
		break;
	case NETDEV_CHANGENAME:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "NETDEV_CHANGENAME dev=%s\n",
			    dev->name);
		break;
#endif /* NET_21 */
	default:
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_device_event: "
			    "event type %ld unrecognised for dev=%s\n",
			    event,
			    dev->name);
		break;
	}
	return NOTIFY_DONE;
}

/*
 *	Called when an ipsec tunnel device is initialized.
 *	The ipsec tunnel device structure is passed to us.
 */
 
int
ipsec_tunnel_init(struct net_device *dev)
{
	int i;

	KLIPS_PRINT(debug_tunnel,
		    "klips_debug:ipsec_tunnel_init: "
		    "allocating %lu bytes initialising device: %s\n",
		    (unsigned long) sizeof(struct ipsecpriv),
		    dev->name ? dev->name : "NULL");

#ifdef alloc_netdev
	dev->destructor         = free_netdev;
#endif

#ifndef HAVE_NETDEV_PRIV
	{
		struct ipsecpriv *priv_dev;
		priv_dev = kmalloc(sizeof(struct ipsecpriv), GFP_KERNEL);
		if (priv_dev == NULL)
			return;
		dev->priv = priv_dev;
	}
#endif
	memset((caddr_t)netdev_priv(dev), 0, sizeof(struct ipsecpriv));

	for(i = 0; i < sizeof(zeroes); i++) {
		((__u8*)(zeroes))[i] = 0;
	}
	
#ifdef HAVE_NETDEV_HEADER_OPS
	dev->header_ops		= NULL;
#else
	dev->hard_header	= NULL;
	dev->rebuild_header 	= NULL;
#ifndef NET_21
	dev->header_cache_bind 	= NULL;
#endif /* !NET_21 */
	dev->header_cache_update= NULL;
#endif
#ifdef HAVE_NET_DEVICE_OPS
	dev->netdev_ops         = &klips_device_ops;
#else
        dev->open               = ipsec_tunnel_open;
        dev->stop               = ipsec_tunnel_close;
        dev->hard_start_xmit    = ipsec_tunnel_start_xmit;
        dev->get_stats          = ipsec_tunnel_get_stats;
#ifdef HAVE_SET_MAC_ADDR
        dev->set_mac_address    = NULL;
#endif
        dev->do_ioctl           = ipsec_tunnel_ioctl;
        dev->neigh_setup        = ipsec_tunnel_neigh_setup_dev;
#endif

#ifdef NET_21
/*	prv->neigh_setup        = NULL; */
#endif /* NET_21 */
	dev->hard_header_len 	= 0;
	dev->mtu		= 0;
	dev->addr_len		= 0;
	dev->type		= ARPHRD_VOID; /* ARPHRD_TUNNEL; */ /* ARPHRD_ETHER; */
	dev->tx_queue_len	= 10;		/* Small queue */
#ifdef IFF_XMIT_DST_RELEASE
	dev->priv_flags	       &= ~IFF_XMIT_DST_RELEASE;
#endif
	memset((caddr_t)(dev->broadcast),0xFF, ETH_ALEN);	/* what if this is not attached to ethernet? */

	/* New-style flags. */
	dev->flags		= IFF_NOARP /* 0 */ /* Petr Novak */;

	/* We're done.  Have I forgotten anything? */
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Module specific interface (but it links with the rest of IPSEC)  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int
ipsec_tunnel_probe(struct net_device *dev)
{
	ipsec_tunnel_init(dev); 
	return 0;
}

#ifdef alloc_netdev
static void ipsec_tunnel_netdev_setup(struct net_device *dev)
{
}
#endif

struct net_device *ipsecdevices[IPSEC_NUM_IFMAX];
int ipsecdevices_max=-1;

int
ipsec_tunnel_createnum(int ifnum)
{
	char name[IFNAMSIZ];
	struct net_device *dev_ipsec;
	int vifentry;

	if(ifnum > IPSEC_NUM_IFMAX) {
		return -ENOENT;
	}

	if(ipsecdevices[ifnum]!=NULL) {
		return -EEXIST;
	}
	
	/* no identical device */
	if(ifnum > ipsecdevices_max) {
		ipsecdevices_max=ifnum;
	}
	vifentry = ifnum;
	
	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_init_devices: "
		    "creating and registering IPSEC_NUM_IF=%u device\n",
		    ifnum);

	sprintf(name, IPSEC_DEV_FORMAT, ifnum);
#ifdef alloc_netdev
	dev_ipsec = alloc_netdev(sizeof(struct ipsecpriv), name, ipsec_tunnel_netdev_setup);
#else
	dev_ipsec = (struct net_device*)kmalloc(sizeof(struct net_device), GFP_KERNEL);
#endif
	if (dev_ipsec == NULL) {
		printk(KERN_ERR "klips_debug:ipsec_tunnel_init_devices: "
		       "failed to allocate memory for device %s, quitting device init.\n",
		       name);
		return -ENOMEM;
	}
#ifndef alloc_netdev
	memset((caddr_t)dev_ipsec, 0, sizeof(struct net_device));
#ifdef NETDEV_23
	strncpy(dev_ipsec->name, name, sizeof(dev_ipsec->name));
#else /* NETDEV_23 */
	dev_ipsec->name = (char*)kmalloc(IFNAMSIZ, GFP_KERNEL);
	if (dev_ipsec->name == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_init_devices: "
			    "failed to allocate memory for device %s name, quitting device init.\n",
			    name);
		return -ENOMEM;
	}
	memset((caddr_t)dev_ipsec->name, 0, IFNAMSIZ);
	strncpy(dev_ipsec->name, name, IFNAMSIZ);
#endif /* NETDEV_23 */
#ifdef PAUL_FIXME
	dev_ipsec->next = NULL;
#endif
#endif /* alloc_netdev */
#ifndef USE_NETDEV_OPS
	dev_ipsec->init = &ipsec_tunnel_probe;
#else
	dev_ipsec->netdev_ops = &klips_device_ops;
#endif
	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_init_devices: "
		    "registering device %s\n",
		    dev_ipsec->name);
	
	/* reference and hold the device reference */
	ipsec_dev_hold(dev_ipsec);
	ipsecdevices[vifentry]=dev_ipsec;
	
	if (register_netdev(dev_ipsec) != 0) {
		KLIPS_PRINT(1 || debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_init_devices: "
			    "registering device %s failed, quitting device init.\n",
			    dev_ipsec->name);
		return -EIO;
	} else {
		KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
			    "klips_debug:ipsec_tunnel_init_devices: "
			    "registering device %s succeeded, continuing...\n",
			    dev_ipsec->name);
	}
	return 0;
}
	

int 
ipsec_tunnel_init_devices(void)
{
	int i;
	int error;
	
	KLIPS_PRINT(debug_tunnel & DB_TN_INIT,
		    "klips_debug:ipsec_tunnel_init_devices: "
		    "creating and registering IPSEC_NUM_IF=%u devices, allocating %lu per device, IFNAMSIZ=%u.\n",
		    IPSEC_NUM_IF,
		    (unsigned long) (sizeof(struct net_device) + IFNAMSIZ),
		    IFNAMSIZ);

	for(i = 0; i < IPSEC_NUM_IF; i++) {
		error = ipsec_tunnel_createnum(i);
		
		if(error) break;
	}
	return 0;
}

int
ipsec_tunnel_deletenum(int vifnum)
{
	struct net_device *dev_ipsec;
	
	if(vifnum > IPSEC_NUM_IFMAX) {
		return -ENOENT;
	}

	dev_ipsec = ipsecdevices[vifnum];
	if(dev_ipsec == NULL) {
		return -ENOENT;
	}

	/* release reference */
	ipsecdevices[vifnum]=NULL;
	ipsec_dev_put(dev_ipsec);
	
	KLIPS_PRINT(debug_tunnel, "Unregistering %s (refcnt=%d)\n",
		    dev_ipsec->name,
		    atomic_read(&dev_ipsec->refcnt));
	unregister_netdev(dev_ipsec);
	KLIPS_PRINT(debug_tunnel, "Unregisted %s\n", dev_ipsec->name);
#ifndef NETDEV_23
	kfree(dev_ipsec->name);
	dev_ipsec->name=NULL;
#endif /* !NETDEV_23 */
#ifndef alloc_netdev
	kfree(dev_ipsec->priv);
	dev_ipsec->priv=NULL;
#endif /* alloc_netdev */

	return 0;
}


struct net_device *
ipsec_tunnel_get_device(int vifnum)
{
	struct net_device *nd;
	
	if(vifnum < ipsecdevices_max) {
		nd = ipsecdevices[vifnum];

		if(nd) ipsec_dev_hold(nd);
		return nd;
	} else {
		return NULL;
	}
}

/* void */
int
ipsec_tunnel_cleanup_devices(void)
{
	int error = 0;
	int i;
	struct net_device *dev_ipsec;
	
	for(i = 0; i < IPSEC_NUM_IF; i++) {
   	        dev_ipsec = ipsecdevices[i];
		if(dev_ipsec == NULL) {
		  continue;
		}

		/* release reference */
		ipsecdevices[i]=NULL;
		ipsec_dev_put(dev_ipsec);

		KLIPS_PRINT(debug_tunnel, "Unregistering %s (refcnt=%d)\n",
			    dev_ipsec->name,
			    atomic_read(&dev_ipsec->refcnt));
		unregister_netdev(dev_ipsec);
		KLIPS_PRINT(debug_tunnel, "Unregisted %s\n", dev_ipsec->name);
#ifndef NETDEV_23
		kfree(dev_ipsec->name);
		dev_ipsec->name=NULL;
#endif /* !NETDEV_23 */
#ifndef alloc_netdev
		kfree(dev_ipsec->priv);
		dev_ipsec->priv=NULL;
#endif /* alloc_netdev */
	}

#ifdef HAVE_UDP_ENCAP_CONVERT
	/* once all devices are down, it's time to restore UDP connections to
	 * not use as for (de)encapsulation anymore */
	ipsec_tunnel_upd_encap_cleanup();
#endif

	return error;
}

/* ------------------------------------------------------------------------
 * this handles creating and managing state for xmit path
 */

static spinlock_t ixs_cache_lock = SPIN_LOCK_UNLOCKED;
#ifdef HAVE_KMEM_CACHE_MACRO
static struct kmem_cache *ixs_cache_allocator = NULL;
#else
static kmem_cache_t *ixs_cache_allocator = NULL;
#endif
static unsigned  ixs_cache_allocated_count = 0;

#if !defined(MODULE_PARM) && defined(module_param)
/*
 * As of 2.6.17 MODULE_PARM no longer exists, use module_param instead.
 */
#define	MODULE_PARM(a,b)	module_param(a,int,0644)
#endif

int ipsec_ixs_cache_allocated_count_max = 1000;
MODULE_PARM(ipsec_ixs_cache_allocated_count_max, "i");
MODULE_PARM_DESC(ipsec_ixs_cache_allocated_count_max,
	"Maximum outstanding transmit packets");

int
ipsec_xmit_state_cache_init (void)
{
        if (ixs_cache_allocator)
                return -EBUSY;

        spin_lock_init(&ixs_cache_lock);
#ifdef HAVE_KMEM_CACHE_MACRO
	/* ixs_cache_allocator = KMEM_CACHE(ipsec_ixs,0); */
        ixs_cache_allocator = kmem_cache_create ("ipsec_ixs",
                sizeof (struct ipsec_xmit_state), 0,
                0, NULL);
#else
        ixs_cache_allocator = kmem_cache_create ("ipsec_ixs",
                sizeof (struct ipsec_xmit_state), 0,
                0, NULL, NULL);
#endif
        if (! ixs_cache_allocator)
                return -ENOMEM;

        return 0;
}

void
ipsec_xmit_state_cache_cleanup (void)
{
        if (unlikely (ixs_cache_allocated_count))
                printk ("ipsec: deleting ipsec_ixs kmem_cache while in use\n");

        if (ixs_cache_allocator) {
                kmem_cache_destroy (ixs_cache_allocator);
                ixs_cache_allocator = NULL;
        }
        ixs_cache_allocated_count = 0;
}

struct ipsec_xmit_state *
ipsec_xmit_state_new (void)
{
	struct ipsec_xmit_state *ixs;

        spin_lock_bh (&ixs_cache_lock);

	if (ixs_cache_allocated_count >= ipsec_ixs_cache_allocated_count_max) {
		spin_unlock_bh (&ixs_cache_lock);
		KLIPS_PRINT(debug_tunnel,
			"klips_debug:ipsec_xmit_state_new: "
			"exceeded maximum outstanding TX packet cnt %d\n",
			ixs_cache_allocated_count);
		return NULL;
	}

        ixs = kmem_cache_alloc (ixs_cache_allocator, GFP_ATOMIC);

        if (likely (ixs != NULL))
                ixs_cache_allocated_count++;

        spin_unlock_bh (&ixs_cache_lock);

        if (unlikely (NULL == ixs))
                goto bail;

        /* initialize the object */
#if 1 /* optimised to only clear the required bits */
		memset((caddr_t)ixs, 0, sizeof(*ixs));
#else
		ixs->pass = 0;
		ixs->state = 0;
		ixs->next_state = 0;
		ixs->ipsp = NULL;
		ixs->sa_len = 0;
		ixs->stats = NULL;
		ixs->ips.ips_ident_s.data = NULL;
		ixs->ips.ips_ident_d.data = NULL;
		ixs->outgoing_said.proto = 0;
#ifdef NAT_TRAVERSAL
		ixs->natt_type = 0, ixs->natt_head = 0;
		ixs->natt_sport = 0, ixs->natt_dport = 0;
#endif
		ixs->tot_headroom = 0;
		ixs->tot_tailroom = 0;
		ixs->eroute = NULL;
		ixs->hard_header_stripped = 0;
		ixs->hard_header_len = 0;
		ixs->cur_mtu = 0; /* FIXME: can we do something better ? */

		ixs->oskb = NULL;
		ixs->saved_header = NULL;	/* saved copy of the hard header */
		ixs->route = NULL;
#endif /* memset */

bail:
        return ixs;
}

void
ipsec_xmit_state_delete (struct ipsec_xmit_state *ixs)
{
        if (unlikely (! ixs))
                return;

        if (ixs->dev && netif_queue_stopped(ixs->dev))
                netif_wake_queue(ixs->dev);

        spin_lock_bh (&ixs_cache_lock);

        ixs_cache_allocated_count--;
        kmem_cache_free (ixs_cache_allocator, ixs);

        spin_unlock_bh (&ixs_cache_lock);
}

#ifdef HAVE_NET_DEVICE_OPS
const struct net_device_ops klips_device_ops = {
	/* Add our tunnel functions to the device */
	.ndo_init               = ipsec_tunnel_probe,
	.ndo_open               = ipsec_tunnel_open,
	.ndo_stop		= ipsec_tunnel_close,
	.ndo_start_xmit 	= ipsec_tunnel_start_xmit,
	.ndo_get_stats  	= ipsec_tunnel_get_stats,
	.ndo_neigh_setup        = ipsec_tunnel_neigh_setup_dev,
	.ndo_do_ioctl		= ipsec_tunnel_ioctl,

#ifdef HAVE_SET_MAC_ADDR
	.ndo_set_mac_address = ipsec_tunnel_set_mac_address,
#endif
};
#endif

/*
 * We call the attach routine to attach another device.
 */

DEBUG_NO_STATIC int
ipsec_tunnel_attach(struct net_device *dev, struct net_device *physdev)
{
        int i;
	struct ipsecpriv *prv = netdev_priv(dev);

	if(dev == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_attach: "
			    "no device...");
		return -ENODEV;
	}

	if(prv == NULL) {
		KLIPS_PRINT(debug_tunnel & DB_TN_REVEC,
			    "klips_debug:ipsec_tunnel_attach: "
			    "no private space associated with dev=%s",
			    dev->name ? dev->name : "NULL");
		return -ENODATA;
	}

#ifdef HAVE_NETDEV_HEADER_OPS
	dev->header_ops = &klips_header_ops;                                    
#endif                                                                         
#ifdef HAVE_NET_DEVICE_OPS
	dev->netdev_ops = &klips_device_ops;
#else
# ifdef HAVE_SET_MAC_ADDR
	dev->set_mac_address = ipsec_tunnel_set_mac_address;
# endif
#endif /* HAVE_NET_DEVICE_OPS */
	prv->dev = physdev;

#ifdef HAVE_NET_DEVICE_OPS
	prv->hard_start_xmit = physdev->netdev_ops->ndo_start_xmit;
	prv->get_stats       = physdev->netdev_ops->ndo_get_stats;
#else
	prv->hard_start_xmit = physdev->hard_start_xmit;
	prv->get_stats       = physdev->get_stats;
#endif
	dev->hard_header_len = physdev->hard_header_len;

/*	prv->neigh_setup        = physdev->neigh_setup; */
	dev->mtu = 16260; /* 0xfff0; */ /* dev->mtu; */
	prv->mtu = physdev->mtu;

#ifdef PHYSDEV_TYPE
	dev->type = physdev->type; /* ARPHRD_TUNNEL; */
#endif /*  PHYSDEV_TYPE */

	dev->addr_len = physdev->addr_len;
	for (i=0; i<dev->addr_len; i++) {
		dev->dev_addr[i] = physdev->dev_addr[i];
	}
#ifdef CONFIG_KLIPS_DEBUG
	if(debug_tunnel & DB_TN_INIT) {
		printk(KERN_INFO "klips_debug:ipsec_tunnel_attach: "
		       "physical device %s being attached has HW address: %2x",
		       physdev->name, physdev->dev_addr[0]);
		for (i=1; i < physdev->addr_len; i++) {
			printk(":%02x", physdev->dev_addr[i]);
		}
		printk("\n");
	}
#endif /* CONFIG_KLIPS_DEBUG */

	return 0;
}


/*
 * Local Variables:
 * c-style: linux
 * End:
 */
