/*
 * This is a module which is used for setting the MSS option in TCP packets.
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

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_TCPMSS.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables HTTP CRACK modification module");
MODULE_ALIAS("ipt_HTTPCRACK");


static int  _httpCmdList [] = {
    /*LSB*/
#if defined(__LITTLE_ENDIAN_BITFIELD)
    0x20544547, /* GET */  
    0x54534F50, /* POST */
    0x50545448  /* HTTP */
# else
    0x47455420, /* GET */
    0x504F5354, /* POST */
    0x48545450  /* HTTP */
# endif
};
unsigned char tp_userAgen[64] = {
    0x4d, 0x6f, 0x7a, 0x69, 0x6c, 0x6c, 0x61, 0x2f, 0x34, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x6f, 0x6d,
    0x70, 0x61, 0x74, 0x69, 0x62, 0x6c, 0x65, 0x3b, 0x20, 0x4d, 0x53, 0x49, 0x45, 0x20, 0x36, 0x2e, 
    0x30, 0x3b, 0x20, 0x57, 0x69, 0x6e, 0x64, 0x6f, 0x77, 0x73, 0x20, 0x4e, 0x54, 0x20, 0x35, 0x2e, 
    0x31, 0x29, 0x0d, 0x0a
};

unsigned char tp_userAgen1[3] = {0x43, 0x3a, 0x20};

/* Return 1 for match, 0 for accept, -1 for partial. */
static int find_pattern2(const char *data, size_t dlen,
	const char *pattern, size_t plen,
	char term,
	unsigned int *numoff,
	unsigned int *numlen)
{
    size_t i, j, k;
    int state = 0;
    *numoff = *numlen = 0;

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
	    if (j > dlen) break ;	/* no terminal char */

	*numlen = k;
	return 1;
    }
    return 0;
}

#if 0
static int
tcpmss_mangle_packet(struct sk_buff **pskb,
		     const struct xt_tcpmss_info *info,
		     unsigned int tcphoff,
		     unsigned int minlen)
{
	struct tcphdr *tcph;
	unsigned int tcplen, i;
	unsigned int found;
        unsigned char *data = NULL;
        unsigned int datalen;
        unsigned int tcpHeadLen;
        unsigned int offset = 0, numlen = 0;
        unsigned int edited = 0;
	__be16 oldval;
	u16 newmss;
	u8 *opt;

	if (!skb_make_writable(pskb, (*pskb)->len))
		return -1;

	tcplen = (*pskb)->len - tcphoff;
	tcph = (struct tcphdr *)((*pskb)->nh.raw + tcphoff);
        tcpHeadLen = tcph->doff*4;

        data = (void *)tcph + tcpHeadLen;

        datalen = tcplen - (tcpHeadLen);
        if(datalen < 10)
            return 0;

        printk("ff-test5\n");
        if(*((int*)data) != _httpCmdList[0] && *((int*)data) != _httpCmdList[1]){
            return 0;
        }

        printk("ff-test1 datalen %x\n", datalen);
        found = find_pattern2(data, datalen, "User-Agent: ", 
	    sizeof("User-Agent: ") - 1, '\r', &offset, &numlen);

        printk("ff-test4 offset %x numlen %x\n",offset, numlen);
        if(found) {
            printk("ff-test6 chk  %x \n", tcph->check);
            if(numlen >= 0x32) {
                memcpy(data + offset, tp_userAgen, 0x34);
                memcpy(data + offset + 0x34, tp_userAgen1, 0x3);
                edited = 1;
            } else {
                return edited;
            }
        } else {
            return edited;
        }

        printk("ff-test3\n");
        found = find_pattern2(data, datalen, "Cookie: ", 
	    sizeof("Cookie: ") - 1, '\r', &offset, &numlen);

        if(found) {
            if(numlen >= 2) {
                *(data+offset) = '\r';
                *(data+offset+1) = '\n';
                memcpy(data + offset + 0x2, tp_userAgen1, 0x3);
                edited = 1;
            }
        }
	return edited;
}
#endif

static unsigned int
xt_httpcrack_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
	struct iphdr *iph = ip_hdr(skb);
	struct tcphdr *tcph;
        int iph_len; 

	unsigned int tcplen;
	unsigned int found;
        unsigned char *data = NULL;
        unsigned int datalen;
        unsigned int tcpHeadLen;
        unsigned int offset = 0U, numlen = 0U;
        unsigned int edited = 0U;
        unsigned int i = 0U;

	if (!skb_make_writable(skb, skb->len))
		return XT_CONTINUE;

        iph_len = ip_hdrlen(skb);

	tcplen = skb->len - iph_len;
	tcph = (struct tcphdr *)(skb_network_header(skb) + iph_len);
        tcpHeadLen = tcph->doff*4;

        data = (void *)tcph + tcpHeadLen;

        datalen = tcplen - (tcpHeadLen);
        if(datalen < 10)
            return XT_CONTINUE;

        if(*((int*)data) != _httpCmdList[0] && *((int*)data) != _httpCmdList[1]){
            return XT_CONTINUE;
        }

        found = find_pattern2(data, datalen, "User-Agent: ", 
	    sizeof("User-Agent: ") - 1, '\r', &offset, &numlen);

        if(found) {
            if(numlen >= 0x34) {
                memcpy(data + offset, tp_userAgen, 0x34);
                if(numlen >= 0x37) {
                    memcpy(data + offset + 0x34, tp_userAgen1, 0x3);
                } else {
                    for(i = 0x34; i < numlen; i++) {
                        *(data+offset+i) = 0x20;
                    }
                }
                edited = 1;
            }
        }

        found = find_pattern2(data, datalen, "Cookie: ", 
	    sizeof("Cookie: ") - 1, '\r', &offset, &numlen);

        if(found) {
            if(numlen >= 2) {
                *(data+offset) = '\r';
                *(data+offset+1) = '\n';
                if(numlen >= 5) {
                    memcpy(data + offset + 0x2, tp_userAgen1, 0x3);
                } else {
                    for(i = 2; i < numlen; i++) {
                        *(data+offset+i) = 0x20;
                    }
                }
                edited = 1;
            }
        }
        if(edited == 1) {
            tcph->check = 0;
            skb->csum = csum_partial((unsigned char *)tcph, skb->len - iph->ihl * 4,0);
            tcph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, 
                    ntohs(iph->tot_len) - iph_len,iph->protocol,
                    skb->csum);
        }
	return XT_CONTINUE;
}

static bool
xt_httpcrack_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_httpcrack_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "HTTPCRACK",
		.checkentry	= xt_httpcrack_checkentry4,
		.target		= xt_httpcrack_target4,
		.targetsize	= sizeof(int),
		.proto		= IPPROTO_TCP,
		.me		= THIS_MODULE,
	},
};

static int __init xt_httpcrack_init(void)
{
	return xt_register_targets(xt_httpcrack_reg, ARRAY_SIZE(xt_httpcrack_reg));
}

static void __exit xt_httpcrack_fini(void)
{
	xt_unregister_targets(xt_httpcrack_reg, ARRAY_SIZE(xt_httpcrack_reg));
}

module_init(xt_httpcrack_init);
module_exit(xt_httpcrack_fini);
