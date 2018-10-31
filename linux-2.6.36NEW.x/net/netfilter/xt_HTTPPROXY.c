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
#include <linux/netfilter/xt_HTTPPROXY.h>
#include <linux/netlink.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhao.chong@utt.com.cn");
MODULE_DESCRIPTION("x_tables HTTP PROXY modification module");
MODULE_ALIAS("ipt_HTTPPROXY");

#define DEBUG_HTTPPROXY 0
#if DEBUG_HTTPPROXY
#define UTT_HTTPPROXY_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_HTTPPROXY_DEBUG(fmt, argx...)
#endif


#define UTT_AD_INIT     1
#define UTT_AD_PLAYER 2
#define MAX_IP_PKT_LEN 1500

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


struct uttAdblockNlData {
        char    type;
	char    proxy_dir[HTTP_PROXY_DIR_LEN+1];
        int     tcpLen;
        char    data[MAX_IP_PKT_LEN + 20];
};

static int  uttAdDemonPid;
static struct sock *nl_sk = NULL;

static int uttAdblockNlSend(char *data, u_int dataLen, u_int dpid, u_int dgrp, char type, int tcpLen)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int nlLen = NLMSG_SPACE(dataLen);/*包含netlink消息头的长度*/
    int err = 0;

    UTT_HTTPPROXY_DEBUG("dpid = %d, dgrp = %d nlLen=%d\n",dpid,dgrp, nlLen);
    skb_1 = alloc_skb(nlLen,GFP_ATOMIC);
    if(skb_1) {/*分配成功*/
	nlh = nlmsg_put(skb_1,0,0,0,dataLen,0);

	NETLINK_CB(skb_1).pid = 0;/*源代pid*/
	NETLINK_CB(skb_1).dst_group = dgrp;/*目的组*/

	memcpy((char*)(NLMSG_DATA(nlh)), data, dataLen);

	if(dgrp != 0) {/*组播*/
	    netlink_broadcast(nl_sk, skb_1, 0, dgrp, GFP_KERNEL);
	} else {/*单播*/
	    err =  netlink_unicast(nl_sk,skb_1,dpid,MSG_DONTWAIT);
#if 0
	    UTT_HTTPPROXY_DEBUG("netlink_unicast MSG_DONTWAIT=0x%x;return value = %d\n",MSG_DONTWAIT,err);
#endif
	}
    } else {
	UTT_HTTPPROXY_DEBUG("alloc error\n");
	err = -1;
    }
    return err; 
}

static void uttAdblockNlRcv(struct sk_buff *__skb)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    struct uttAdblockNlData *msgHead = NULL;

    skb = skb_get(__skb);

    if(skb->len >= NLMSG_SPACE(0)) {
	nlh = nlmsg_hdr(skb);
	msgHead = (struct uttAdblockNlData *)NLMSG_DATA(nlh);
	switch(msgHead->type) {
	    case UTT_AD_INIT:
		uttAdDemonPid = nlh->nlmsg_pid;
		UTT_HTTPPROXY_DEBUG("uttAdDemonPid %d\n",uttAdDemonPid);
		break;
	    default:
		break;
	}
	kfree_skb(skb);
    }

    return;
}

static unsigned int
xt_httpproxy_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
	struct iphdr *iph = ip_hdr(skb);
	struct tcphdr *tcph;
        int iph_len; 
        struct uttAdblockNlData msgHead;
	unsigned int invalidRet= XT_CONTINUE;
	unsigned int tcplen;
        unsigned char *data = NULL;
        unsigned int datalen;
        unsigned int tcpHeadLen;
        unsigned int i = 0U;
	unsigned int urlParLen = 0U; 
        char type = 0;
	char *httpP = NULL;
	unsigned char *tmpData = NULL;
	int strLen;
	unsigned int parStartIndex, parEndIndex;
#if 1
	struct xt_httpproxy_info *http_proxy_info = (struct xt_httpproxy_info *)par->targinfo;
#endif
	int proxy_dir_len, proxy_file_len;
	unsigned char *tmpP = NULL;

	if (!skb_make_writable(skb, skb->len))
		return XT_CONTINUE;

        iph_len = ip_hdrlen(skb);

	tcplen = skb->len - iph_len;
	tcph = (struct tcphdr *)(skb_network_header(skb) + iph_len);
        tcpHeadLen = tcph->doff*4;

        data = (void *)tcph + tcpHeadLen;

	proxy_dir_len = strlen(http_proxy_info->proxy_dir);
	if (proxy_dir_len > HTTP_PROXY_DIR_LEN) {
	    UTT_HTTPPROXY_DEBUG("proxy_dir_len too long\n");
	    return XT_CONTINUE;
	}
	proxy_file_len = strlen(http_proxy_info->proxy_file);
	if (proxy_file_len > HTTP_PROXY_FILE_LEN) {
	    UTT_HTTPPROXY_DEBUG("proxy_file_len too long\n");
	    return XT_CONTINUE;
	}
	
	if(!strcmp(http_proxy_info->proxy_dir, "127.0.0.1"))
	{/*指向本地的包为了更改cache加速。所以若无法代理，可以直接drop*/
	    invalidRet = NF_DROP;
	}

        datalen = tcplen - (tcpHeadLen);
        if(datalen < 10 || skb->len > MAX_IP_PKT_LEN) {
            return invalidRet;
	}
        if(*(data + datalen - 1) != 0x0a)
	{/*get包分片*/   
            return invalidRet;
	}

#if  0
        if(*((int*)data) != _httpCmdList[0] && *((int*)data) != _httpCmdList[1]){
            return XT_CONTINUE;
        }
#else
        if(*((int*)data) != _httpCmdList[0]){
            return invalidRet;
        }
#endif
	UTT_HTTPPROXY_DEBUG("%s %d dataLen:%d\n", __func__, __LINE__, datalen);
	for(parStartIndex = 4U, parEndIndex = 4U; parEndIndex < datalen; parEndIndex++) {
	    if (*(data+parEndIndex) == '?') {	
		parStartIndex = parEndIndex;
	    } else if (*(data+parEndIndex) == (char)0x20) {/*遇到空格，url参数结束*/
		break;
	    }
	}

	UTT_HTTPPROXY_DEBUG("%s %d\n", __func__, __LINE__);
	urlParLen = parEndIndex - parStartIndex;

	type = UTT_AD_PLAYER;
#if 0
	UTT_HTTPPROXY_DEBUG("-------data=%s------\n",data);
#endif
	memset(&msgHead,0,sizeof(msgHead));
	memcpy(msgHead.data + 40,data,5);
	memcpy(msgHead.data + 45,http_proxy_info->proxy_file,proxy_file_len);
#if 1 
	if (parStartIndex != 4U) {
	    memcpy(msgHead.data + 45 + proxy_file_len,data + parStartIndex,urlParLen); 
	    proxy_file_len += urlParLen; 
	} 
#endif
	if ((httpP = strstr(data+4," HTTP")) == NULL) {
	    return invalidRet;
	}
	memcpy(msgHead.data + 45 + proxy_file_len,httpP,11);/*HTTP/1.x\r\n*/
	tmpData = httpP + 11;/*指向data的第二行*/
	i = 45 + proxy_file_len + 11;
#if 1
	UTT_HTTPPROXY_DEBUG("%s %d\n", __func__, __LINE__);
	while (tmpData - data < datalen) {
	    if (memcmp(tmpData,"Host",4) == 0) {
		memcpy(msgHead.data + i, "Host: ",6);
		i += 6;
		memcpy(msgHead.data +i, http_proxy_info->proxy_dir,proxy_dir_len);
		i += proxy_dir_len;
		memcpy(msgHead.data +i, "\r\n",2);
		i += 2;
	    } else if (memcmp(tmpData,"Connection",10) == 0) {
		memcpy(msgHead.data + i, "Connection: ",12);
		i += 12;
		memcpy(msgHead.data + i, "close\r\n",7);
		i += 7;
#if 0
	    } else if (memcmp(tmpData,"Cookie",6) != 0) {
		strLen = (unsigned char *)strchr(tmpData,0x0a) - tmpData + 1;
		memcpy(msgHead.data +i, tmpData, strLen);
		i += strLen;
#endif
	    } else {
		tmpP = (unsigned char *)strchr(tmpData,0x0a);
		if (tmpP == NULL) {
		    return invalidRet;
		}
		strLen = tmpP - tmpData + 1;
		memcpy(msgHead.data +i, tmpData, strLen);
		i += strLen;
		tmpData = tmpP + 1;
		continue;
	    }
	    if ((tmpP = (unsigned char *)strchr(tmpData,0x0a)) == NULL) {
		    return invalidRet;
	    }
	    tmpData = tmpP + 1;
	}
#endif
	UTT_HTTPPROXY_DEBUG("%s %d %d\n", __func__, __LINE__, tmpData-data);
        msgHead.type = type;
        msgHead.tcpLen = datalen;
	memcpy(msgHead.proxy_dir,http_proxy_info->proxy_dir,proxy_dir_len + 1);
        memcpy(msgHead.data, iph, 40);
        uttAdblockNlSend(&msgHead, sizeof(msgHead), uttAdDemonPid, 0, type, datalen);

	return NF_DROP;
}

static bool
xt_httpproxy_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_httpproxy_reg[] = {
	{
		.family		= AF_INET,
		.name		= "HTTPPROXY",
		.checkentry	= xt_httpproxy_checkentry4,
		.target		= xt_httpproxy_target4,
		.targetsize	= sizeof(struct xt_httpproxy_info),
#if 0
		.proto		= IPPROTO_TCP,
#endif
		.me		= THIS_MODULE,
	},
};

static int __init xt_httpproxy_init(void)
{
        nl_sk = netlink_kernel_create(&init_net, NETLINK_UTT_ADBLOCK, 0,
                            uttAdblockNlRcv, NULL, THIS_MODULE);                                                                                                                                     
        if(!nl_sk){
            return 1;
        }
	return xt_register_targets(xt_httpproxy_reg, ARRAY_SIZE(xt_httpproxy_reg));
}

static void __exit xt_httpproxy_fini(void)
{
        if(nl_sk != NULL){
            sock_release(nl_sk->sk_socket);           
        } 
	xt_unregister_targets(xt_httpproxy_reg, ARRAY_SIZE(xt_httpproxy_reg));
}

module_init(xt_httpproxy_init);
module_exit(xt_httpproxy_fini);
