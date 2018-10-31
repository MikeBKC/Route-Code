/*
 * This is a module which is used for HTTP REDIRECT.
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
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_HTTPREDIRECT.h>
#if 0
#ifdef CONFIG_UTT_NOTICE_LOG
#include <linux/netfilter_ipv4/utt_collision_detection.h>
#endif
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables HTTP REDIRECT modification module");
MODULE_ALIAS("ipt_HTTPREDIRECT");

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
/*      
 * http redirect
 */     
#if 0
static const char *redirect_1 = "HTTP/1.1 302 Moved Temporarily\r\nLocation: http://%s/%s\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n"; //99
#else
static const char *redirect_1 = "HTTP/1.1 302 Moved Temporarily\r\nLocation: http://%s/%s?ip=%s&%x\r\nContent-Length: 0\r\n\r\n"; //138
#endif
#define REDIRECT_LEN 140U
#define IP_LEN	15U
static char *inet_ntoa_local(struct in_addr ina) {
    static char buf[IP_LEN + 1];
    unsigned char *ucp = (unsigned char *)&ina;
    sprintf(buf, "%d.%d.%d.%d", ucp[0] & 0xff, ucp[1] & 0xff, ucp[2] & 0xff, ucp[3] & 0xff);
    return buf;
}

static unsigned int
xt_httpredirect_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
    __be32 newdst;
    struct in_device *outdev;
    struct in_ifaddr *ifa;

    struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph;
    int iph_len; 
    unsigned int tcplen;
    unsigned char *data;
    unsigned int datalen;
    unsigned int tcpHeadLen;
    char redirect[REDIRECT_LEN];
    struct in_addr addr;
    struct xt_httpredirect_info *httpr_info = (struct xt_httpredirect_info *)par->targinfo;
    int dir_len;
    unsigned long ticks;
    char lanIp[IP_LEN + 1] = {0};
    char dstIp[IP_LEN + 1] = {0};
    int ex_ret = XT_CONTINUE; /* 异常退出何参数 */
#if 0
#ifdef CONFIG_UTT_NOTICE_LOG
    char ipaddr[MAX_IP_LEN];
#endif
#endif

    if (!skb_make_writable(skb, skb->len)) {
	return XT_CONTINUE;
    }

    /*
     * 设置异常退出值
     */
    switch (httpr_info->ex_op) {
	case XT_C:
	    ex_ret = XT_CONTINUE;
	    printk("return continue\n");
	    break;
	case XT_R:
	    ex_ret = XT_RETURN;
	    printk("return return\n");
	    break;
	case XT_D:
	    ex_ret = NF_DROP;
	    printk("return drop\n");
	    break;
	default:
	    printk("default: continue\n");
	    break;
    }

    iph_len = ip_hdrlen(skb);

    tcplen = skb->len - iph_len;
    tcph = (struct tcphdr *)(skb_network_header(skb) + iph_len);

    /*
     * syn 放过
     */
    if ((((unsigned char *)tcph)[13] & 0x2) != 0) {
	printk("hand shake!\n");
	return XT_CONTINUE;
    }

    /* 获得lan IP */
#if 1
    newdst = 0;
    if ((par != NULL) && (par->out != NULL)) {
	outdev = (struct in_device *)par->out->ip_ptr;
	if (outdev && (ifa = outdev->ifa_list)) {
	    newdst = ifa->ifa_local;
	}
    } else {
	printk("out == NULL\n");
	return ex_ret;
    }

    if (!newdst) {
	printk("newdst == 0\n");
	return ex_ret;
    }
    
    /*
     * 如果是lan口发出的数据包，不做http redirect处理
     */
    if (iph->saddr == newdst) {
	printk("saddr is lan ip\n");
	return XT_CONTINUE;
    }
    
    addr.s_addr = newdst;
    strncpy(lanIp, inet_ntoa_local(addr), IP_LEN);
#endif

    tcpHeadLen = tcph->doff*4;

    data = (void *)tcph + tcpHeadLen;

    datalen = tcplen - (tcpHeadLen);
    /* 判断长度是否够Redirect */
    if (datalen < REDIRECT_LEN) {
	printk("dataLen < %d, dataLen = %d\n", REDIRECT_LEN, datalen);
	return ex_ret;
    }

    /*
     * 无需判断是不是ip tcp可以在iptables命令在限制
     * 判断是不是http 响应
     */
    if(*((int*)data) != _httpCmdList[2]){
	printk("not HTTP reponse\n");
	return ex_ret;
    }

    /* redirect_dir是否超长 */
    dir_len = strlen(httpr_info->redirect_dir);
    if (dir_len > HTTP_R_LEN) {
	printk("dir_len too long\n");
	return ex_ret;
    }
    printk("saddr = 0x%x, daddr = 0x%x\n", iph->saddr, iph->daddr);
    /*
     * 获得目的IP
     */
    addr.s_addr = iph->daddr;
    strncpy(dstIp, inet_ntoa_local(addr), IP_LEN);

    /*
     * 将系统启动节拍数jiffies作为?参数加在url后面
     * 解决IE缓存问题
     * 保证每次GET请求返回的redirect都不一样
     */
    ticks = jiffies;
    snprintf(redirect, REDIRECT_LEN, redirect_1, lanIp, httpr_info->redirect_dir, dstIp, ticks);
    printk("redirect = %s\n", redirect);
#if 0
#ifdef CONFIG_UTT_NOTICE_LOG
    if ((utt_collision_detection & NOTICE_LOG) && (strcmp(httpr_info->redirect_dir, "Notice1.asp") == 0)) {
	    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->daddr)); 
	    printk_audit(KERN_WARNING "Give notice to user:%s", ipaddr);
    }
#endif
#endif

    /* 修改包 */
    memmove(data, redirect, REDIRECT_LEN);

    /* checksum */
    tcph->check = 0;
    skb->csum = csum_partial((unsigned char *)tcph, skb->len - iph->ihl * 4,0);
    tcph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, 
            ntohs(iph->tot_len) - iph_len,iph->protocol,
            skb->csum);

    printk("HTTPREDIRECT\n");
    return XT_CONTINUE;
}

static int
xt_httpredirect_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_httpredirect_reg[] = {
	{
		.family		= AF_INET,
		.name		= "HTTPREDIRECT",
		.checkentry	= xt_httpredirect_checkentry4,
		.target		= xt_httpredirect_target4,
		.targetsize	= sizeof(struct xt_httpredirect_info),
		.me		= THIS_MODULE,
	},
};

static int __init xt_httpredirect_init(void)
{
	return xt_register_targets(xt_httpredirect_reg, ARRAY_SIZE(xt_httpredirect_reg));
}

static void __exit xt_httpredirect_fini(void)
{
	xt_unregister_targets(xt_httpredirect_reg, ARRAY_SIZE(xt_httpredirect_reg));
}

module_init(xt_httpredirect_init);
module_exit(xt_httpredirect_fini);
