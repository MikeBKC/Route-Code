/*
 * This is a module which is used for UTT LOG.
 *
 * Copyright (C) 2000 Marc Boucher <marc@mbsi.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_UTTWEBIND.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables UTT WEB INDENTIFY modification module");
MODULE_ALIAS("ipt_UTTWEBIND");

//#define UTTWEBIND_DEBUG

#ifdef UTTWEBIND_DEBUG
#define DBG_PRINTK(fmt, args...)		printk(fmt, ## args)
#else
#define DBG_PRINTK(fmt, args...)
#endif

#define MAXDNAME 256
#define INDIR_MASK     0xc0

#if defined(CONFIG_UTT_NETFILTER)
extern DnsFilterEntryVal *dnsHashLookupData(char *domainName);
#endif

typedef  struct {
	__u16	id;		/* query identification number */
# ifdef BIG_ENDIAN
	/* fields in third byte */
	__u8	qr:1;		/* response flag */
	__u8	opcode:4;	/* purpose of message */
	__u8	aa:1;		/* authoritive answer */
	__u8	tc:1;		/* truncated message */
	__u8	rd:1;		/* recursion desired */
	/* fields in fourth byte */
	__u8	ra:1;		/* recursion available */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	unused:2;	/* unused bits */
	__u8	rcode:4;	/* response code */
# else
	/* fields in third byte */
	__u8	rd:1;		/* recursion desired */
	__u8	tc:1;		/* truncated message */
	__u8	aa:1;		/* authoritive answer */
	__u8	opcode:4;	/* purpose of message */
	__u8	qr:1;		/* response flag */
	/* fields in fourth byte */
	__u8	rcode:4;	/* response code */
	__u8	unused:2;	/* unused bits */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	ra:1;		/* recursion available */
# endif
	/* remaining bytes */
	__u16	qdcount;	/* number of question entries */
	__u16	ancount;	/* number of answer entries */
	__u16	nscount;	/* number of authority entries */
	__u16	arcount;	/* number of resource entries */
}DNS_HDR;

#if 0
static int  _httpCmdList [] = {
    /*LSB*/
# if defined (__LITTLE_ENDIAN_BITFIELD)
    0x20544547, /* GET */  
    0x54534F50, /* POST */
    0x50545448  /* HTTP */
# else
    0x47455420, /* GET */
    0x504F5354, /* POST */
    0x48545450  /* HTTP */
# endif
};
#endif

/* 解压Dns域名, Dns域名可能存在压缩情况，未细看 */
static int expandDnsName(unsigned char	*msg, unsigned char *eomorig, 
		unsigned char *comp_dn, unsigned char *exp_dn, int length)
{
    unsigned char *cp, *dn;
    int		n, c;
    unsigned char     *eom;
    int        len = -1, checked = 0;
    
    dn = exp_dn;
    cp = (unsigned char *) comp_dn;
    eom = exp_dn + length;
    
    /*
     * fetch next label in domain name
     */
    
    while ((n = *cp++) != 0)
      {
	  /*
	   * Check for indirection
	   */
	  switch (n & INDIR_MASK)
	    {
	      case 0:
		if (dn != exp_dn)
		  {
		      if (dn >= eom)
			return (-1);
				*dn++ = '.';
		  }
		if (dn + n >= eom)
		  return (-1);
		checked += n + 1;
		while (--n >= 0)
		  {
		      if ((c = *cp++) == '.')
			{
			    if (dn + n + 2 >= eom)
			      return (-1);
			    *dn++ = '\\';
			}
		      *dn++ = c;
		      if (cp >= eomorig)	/* out of range */
			return (-1);
		  }
		break;
		
	      case INDIR_MASK:
		if (len < 0)
		  len = cp - comp_dn + 1;
		cp = (unsigned char *) msg + (((n & 0x3f) << 8) | (*cp & 0xff));
		if (cp < msg || cp >= eomorig)	/* out of range */
		  return (-1);
		checked += 2;
		/*
		 * Check for loops in the compressed name; if we've
		 * looked at the whole message, there must be a loop.
		 */
		if (checked >= eomorig - msg)
		  return (-1);
		break;
		
	      default:
		return (-1);	/* flag error */
	    }
      }
    *dn = '\0';
    if (len < 0)
      len = cp - comp_dn;
    return (len);
}

extern int getDomainName(DNS_HDR *dns_hdr, int dns_len, unsigned char *buffer)
{
    int question_num;
    int len;
    unsigned char *cp;
    unsigned char name[MAXDNAME + 1];
    int cp_len;
    int name_len;

    if (dns_hdr == NULL || dns_len == 0 || buffer == NULL) {
        return 0;
    }
#if 0
    if (dns_len == 0) {
        return 0;
    }

    if (buffer == NULL) {
        return 0;
    }
#endif

    /*
     * 指向dns头
     */
    cp = (unsigned char *)dns_hdr + sizeof(DNS_HDR);


    question_num = ntohs(dns_hdr->qdcount);

    /*
     * 只处理请求数是1的dns数据包
     */
    if (question_num != 1) {
#if 0
        DNS_FILTER_PRINTF("%s-%d. question_num(%d) not support!\n",
                __FUNCTION__,
                __LINE__,
                question_num
                );
        _getDomainNameBadNum++;
#endif
        return 0;
    }



    len = expandDnsName(
            (unsigned char *) dns_hdr,
            (unsigned char *) dns_hdr + dns_len,
            cp,
            name,
            sizeof(name) 
            );

    if (len < 0) {
        return 0;
    }

    name_len = strlen(name);

    if (name_len == 0) {
        return 0;
    }
    
    cp_len = min((MAXDNAME+1), min(name_len, dns_len));

    strncpy(buffer, name, cp_len);

    if (buffer[cp_len] != '\0') {
#if 0
        if (dns_len >= MAXDNAME + 1) {
            _getDomainNameOverflow1++;
        }
        if (name_len >= MAXDNAME + 1) {
            _getDomainNameOverflow2++;
        }

        if (dns_len > name_len) {
            _getDomainNameOverflow3++;

        }
        else {
            _getDomainNameOverflow4++;
        }
        _getDomainNameOverflow++;
#endif
        return 0;
    }

    
//    _getDomainNameSuccess++;
    return 1;
}
/* 
 * Return 1 for match, 0 for accept, -1 for partial. 
 * get from xt_webstr.c
 */
static int find_pattern2(const char *data, size_t dlen,
	const char *pattern, size_t plen,
	char term,
	unsigned int *numoff,
	unsigned int *numlen)
{
    size_t i, j, k;
    int state = 0;
    *numoff = *numlen = 0;

    DBG_PRINTK("%s: pattern = '%s', dlen = %u\n",__FUNCTION__, pattern, dlen);
    if (dlen == 0)
	return 0;

    if (dlen <= plen) {	/* Short packet: try for partial? */
	if (strnicmp(data, pattern, dlen) == 0)
	    return -1;
	else 
	    return 0;
    }
    for (i = 0; i <= (dlen - plen); i++) {
	/* DFA : \r\n\r\n :: 1234 */
	if (*(data + i) == '\r') {
	    if (!(state % 2)) state++;	/* forwarding move */
	    else state = 0;		/* reset */
	}
	else if (*(data + i) == '\n') {
	    if (state % 2) state++;
	    else state = 0;
	}
	else state = 0;

	if (state >= 4)
	    break;

	/* pattern compare */
	if (memcmp(data + i, pattern, plen ) != 0)
	    continue;

	/* Here, it means patten match!! */
	*numoff=i + plen;
	for (j = *numoff, k = 0; data[j] != term; j++, k++)
	    if (j > dlen) return -1 ;	/* no terminal char */

	*numlen = k;
	return 1;
    }
    return 0;
}

static unsigned int
xt_uttwebind_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
    struct sk_buff *skbin = (struct sk_buff *)skb;
    struct nf_conn *conntrack;
    struct iphdr *iph;
    char webstr[MAXDNAME + 1] = {0};
    int found = 0;
#if defined(CONFIG_UTT_NETFILTER)
    DnsFilterEntryVal *hashEntry = NULL;
#endif

    iph = ip_hdr(skbin);
    /* 非ip包 */
    if (!iph) {
	DBG_PRINTK("iph == NULL\n");
	return XT_CONTINUE;
    }

    /* 数据包没有对应的连接 */
    if (!(conntrack = (struct nf_conn *)skbin->nfct)) {
	DBG_PRINTK("skbin has no conntrack\n");
	return XT_CONTINUE;
    }

    /* 已被识别 */
    if (conntrack->utt_app.priFlag && conntrack->utt_app.secFlag) {
	DBG_PRINTK("indentified conntrack, priFlag = %d, secFlag = %d\n",
		conntrack->utt_app.priFlag, conntrack->utt_app.secFlag);
	return XT_CONTINUE;
    }

    if (iph->protocol == IPPROTO_UDP) {/* UDP */
	struct udphdr *udph;
	DNS_HDR *dnsPkt;
	int pktlen;

	udph = (void *)iph + iph->ihl * 4;
	if (!udph) {
	    DBG_PRINTK("udph == NULL\n");
	    return XT_CONTINUE;
	}

	/* 非53 */
	if (ntohs(udph->dest) != 53) {
	    DBG_PRINTK("udph->dest = %d\n", ntohs(udph->dest));
	    return XT_CONTINUE;
	}

	/* 长度错误, 小于udp头长度加上dns头长度 */
	if (ntohs(udph->len) < 20) {
	    DBG_PRINTK("len < 20\n");
	    return XT_CONTINUE;
	}

	dnsPkt = (DNS_HDR *)(udph + 1);
	pktlen = ntohs(udph->len) - sizeof(struct udphdr);

	found = getDomainName(dnsPkt, pktlen, webstr);
	webstr[MAXDNAME] = '\0';
	DBG_PRINTK("udp: webstr = %s, found = %d\n", webstr, found);
    } else if (iph->protocol == IPPROTO_TCP) { /* TCP */
	char *data;
	unsigned int tcpHeadL, datalen;
	struct tcphdr *tcph;
	int offset, hostlen;

	tcph = (void *)iph + iph->ihl * 4;

	if (!tcph) {
	    DBG_PRINTK("tcph == NULL\n");
	    return XT_CONTINUE;
	}

	/* 非80 */
	if (ntohs(tcph->dest) != 80) {
	    return XT_CONTINUE;
	}

	tcpHeadL = tcph->doff*4;
	data = (void *)tcph + tcpHeadL;
	datalen = (skbin)->len - (iph->ihl * 4) - tcpHeadL;

	/*
	 * 非http头
	 * http头是否会被分片，暂时未考虑
	 */
	if ((memcmp(data, "GET ", sizeof("GET ") - 1) != 0) && 
		(memcmp(data, "POST ", sizeof("POST ") - 1) != 0)) {
	    DBG_PRINTK("not http get or post \n");
	    return XT_CONTINUE;
	}
	/* 查找'Host: '值 */
	found = find_pattern2(data, datalen, "Host: ", 
	    sizeof("Host: ") - 1, '\r', &offset, &hostlen);

	if ((found == 1) && (hostlen < MAXDNAME + 1)) {
	    strncpy(webstr, data + offset, hostlen);
	    webstr[hostlen] = 0;
	}
	DBG_PRINTK("tcp:webstr = %s\n",  webstr);
    } else {
	DBG_PRINTK("iph->proto = %d\n", iph->protocol);
	return XT_CONTINUE;
    }

    /* 查找hash,设置标志 */
#if defined(CONFIG_UTT_NETFILTER)
#if 1
    if (found) {
	hashEntry = dnsHashLookupData(webstr);
	if (hashEntry) {
	    DBG_PRINTK("hashEntry found!\n");
	    conntrack->utt_app.priFlag = hashEntry->priType;
	    conntrack->utt_app.secFlag = hashEntry->secType;

	    /* 当前包设置 */
	    skb->utt_app.priFlag = hashEntry->priType;
	    skb->utt_app.secFlag = hashEntry->secType;
	}
    }
#else
    if (found) {
	conntrack->utt_app.priFlag = 1;
        conntrack->utt_app.secFlag = 2;
    }
#endif
#endif
#if 0
    struct xt_uttwebind_info *info = (struct xt_uttwebind_info *)par->targinfo;
    enum ip_conntrack_info ctinfo;

    if (!(conntrack = nf_ct_get(skbin, &ctinfo))) {
	return XT_CONTINUE;
    }

    if (conntrack->utt_app.priFlag && conntrack->utt_app.secFlag) {
	printk("indentified conntrack, priFlag = %d, secFlag = %d\n",
		conntrack->utt_app.priFlag, conntrack->utt_app.secFlag);
	return XT_CONTINUE;
    }
    conntrack->utt_app.priFlag = info->priFlag;
    conntrack->utt_app.secFlag = info->secFlag;
#endif

    return XT_CONTINUE;
}

static int
xt_uttwebind_checkentry4(const struct xt_tgchk_param *par)
{
    return 0;
}


static struct xt_target xt_uttwebind_reg[] = {
	{
		.family		= AF_INET,
		.name		= "UTTWEBIND",
		.checkentry	= xt_uttwebind_checkentry4,
		.target		= xt_uttwebind_target4,
		.targetsize	= sizeof(int),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttwebind_init(void)
{
	return xt_register_targets(xt_uttwebind_reg, ARRAY_SIZE(xt_uttwebind_reg));
}

static void __exit xt_uttwebind_fini(void)
{
	xt_unregister_targets(xt_uttwebind_reg, ARRAY_SIZE(xt_uttwebind_reg));
}

module_init(xt_uttwebind_init);
module_exit(xt_uttwebind_fini);
