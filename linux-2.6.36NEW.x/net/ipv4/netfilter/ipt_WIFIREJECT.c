/*
 * This is a module which is used for rejecting packets.
 */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/route.h>
#include <net/dst.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_WIFIREJECT.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#include <linux/netfilter_bridge.h>
#endif
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/ip_set.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include "signature.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("Xtables: packet \"rejection\" target for IPv4");

extern __be32 utt_local_address __read_mostly;
extern __be32 utt_wifi_online __read_mostly;
#if 0
static const char *redirect = "HTTP/1.1 302 Redirect to login page\r\nLocation: http://auth.greenwifi.com.cn:80/login.htm?sign=%s%s&gwaddress=%s&gwport=2060&deviceid=%s&ip=%s&mac=%s&timestamp=%s&url=%s\r\nContent-Length: 0\r\n\r\n";
#endif

#ifdef CONFIG_UTT_KNOWIFI
static const char *redirect_1 = "HTTP/1.1 302 Redirect to login page\r\nLocation: http://%s:2060/homeRouter?timestamp=%s\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
#else
static const char *redirect_1 = "HTTP/1.1 302 Redirect to login page\r\nLocation: http://%s/%ssign=%s%s%s&gwaddress=%s&gwport=2060&deviceid=%s&ip=%s&mac=%s&timestamp=%s&url=%s\r\nContent-Length: 0\r\n\r\n";
static const char *redirect_2 = "HTTP/1.1 302 Redirect to login page\r\nLocation: http://%s/%ssign=%s%s%s&gwaddress=%s&gwport=2060&deviceid=%s&ip=%s&mac=%s&timestamp=%s&unitPrice=%s&url=%s\r\nContent-Length: 0\r\n\r\n";
#endif
//"http://auth.greenwifi.com.cn:80/login.htm?sign=AF1117C972DC54C76F5B813AD08DE232CEEE53FF&gwaddress=192.168.1.1&gwport=2060&deviceid=14200155&ip=192.168.1.104&mac=50-e5-49-b4-32-d1&timestamp=20141020134942"
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
#define IP_LEN	15U
#define HTTP_DATA_LEN 800
#define MAX_GET_HOST_LEN 50
#define MAX_GET_URL_LEN 200
/* Host */
static __u8 HTTP_HOST_P[] = {
    0x48, 0x6f, 0x73, 0x74, 0x3a
};
static __u8 HTTP_UA_P[] = {
    0x55, 0x73, 0x65, 0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a
};
static __u8 HTTP_BODY_P[] = {
    0x3c, 0x2f, 0x62, 0x6f, 0x64, 0x79, 0x3e
};
extern __u8 *bm_strstr(__u8 *y, int n, __u8 *x, int m);
static unsigned int wifi_debug = 0;
static char *inet_ntoa_local(struct in_addr ina) {
    static char buf[IP_LEN + 1];
    unsigned char *ucp = (unsigned char *)&ina;
    sprintf(buf, "%d.%d.%d.%d", ucp[0] & 0xff, ucp[1] & 0xff, ucp[2] & 0xff, ucp[3] & 0xff);
    return buf;
}
extern  int wifi_debug_print(const char* fmt, ...)
{
#ifdef CONFIG_UTT_DEBUG
    va_list   args; 
    if (wifi_debug) {             
	va_start(args, fmt); 
	debug_kprint(fmt, args);			
	va_end(args);
    }                                   
#endif
    return 0;
}
#ifdef CONFIG_UTT_DEBUG
extern void utt_wifi_debug(UttNfConfType type)
{
    if (type == UTT_NFCONF_ADD) {
	wifi_debug = 1;
    } else {
	wifi_debug = 0;
    }
    return;
}
#endif
static unsigned int send_wifi_auth_redirect(struct sk_buff *skb, const struct ipt_wifireject_info *reject, int hook)
{
    struct rtable *rt = skb_rtable(skb);
    struct iphdr *oiph = NULL;
    struct tcphdr *otcph = NULL;
    __u16 iph_len = 0; 
    int tcplen = 0, tcpHeadLen=0, datalen=0;
    __u8 *data;
    ip_set_id_t ipsetindex = IP_SET_INVALID_ID;
    uint32_t flag = IPSET_SRC;

    struct sk_buff *ack_skb = NULL;
    struct sk_buff *nskb = NULL;
    struct iphdr *niph = NULL;
    struct tcphdr *ntcph = NULL;
    char httpData[HTTP_DATA_LEN+1];
    char *httpdataP = NULL;
    int httpDatalen = 0;

    char lanIp[IP_LEN + 1] = {0};
    char srcIp[IP_LEN + 1] = {0};
    struct in_addr addr; 
    char mac[18]; 
    char timeStr[15];
    int i=0,j=0;
    __u8 url[MAX_GET_HOST_LEN + 1] = {0};
    __u8 uri[MAX_GET_URL_LEN + 1] = {0};
    __u8 url_get[MAX_GET_HOST_LEN+MAX_GET_URL_LEN+7+1]= {0};
    __u8 *host = NULL;
    char *urlP = NULL;
    __u8 *uaP = NULL;
    int appleFlag = 0;
    int wpFlag = 0;

//wifi_debug_print("%s-%d: \n",__func__,__LINE__);
    /*
     * 获取skb的ip头，和tcp头信息
     * 用于填充构造的http应答包
     * */
    oiph = ip_hdr(skb);
    iph_len = ip_hdrlen(skb);
    otcph = (struct tcphdr *)(skb_network_header(skb) + iph_len);
    tcplen = skb->len - iph_len;
    tcpHeadLen = otcph->doff * 4;
    datalen = tcplen - tcpHeadLen;
    data = (__u8 *)((void *)otcph + tcpHeadLen);
#if 1
    if(ntohs(otcph->dest) != 80) {
	return XT_CONTINUE;
    }
    /*
     * syn 放过
     */
    if ((((unsigned char *)otcph)[13] & 0x2) != 0) {
	wifi_debug_print("hand shake!\n");
	return XT_CONTINUE;
    }
    if(datalen <= 0) {
	return XT_CONTINUE;
    }
    if(datalen < 4) {
	return NF_DROP;
    }
#endif
    if(utt_wifi_online != 1U) {
	wifi_debug_print("%s-%d: utt free auth.\n",__func__,__LINE__);
	    ipsetindex = ip_set_get_byname("wifi_free_auth_group");
	    if(ipsetindex != IP_SET_INVALID_ID) {

	    if(ip_set_testip_kernel(ipsetindex, skb, &flag)==0){ /*检测IP*/
		ip_set_addip_kernel(ipsetindex, skb, &flag);
	    }
	    ip_set_put_byindex(ipsetindex);
	    }
	
	return XT_CONTINUE;
    }
    /*
     * 无需判断是不是ip tcp可以在iptables命令在限制
     * 判断是不是http 请求
     */
    if((*((int*)data) != _httpCmdList[0]) && (*((int*)data) != _httpCmdList[1])){
	wifi_debug_print("not HTTP request.\n");
	return NF_DROP;
    }
    /*
     * 获得源IP
     */
    addr.s_addr = oiph->saddr;
    strncpy(srcIp, inet_ntoa_local(addr), IP_LEN);
    int xrg = 0;
    UttNfHashBuf * buf = uttNfHashBufGet(oiph->saddr, &xrg);
    if(buf !=NULL) {
	snprintf(mac,18,"%02x-%02x-%02x-%02x-%02x-%02x", buf->conf.userStats.mac[0],buf->conf.userStats.mac[1],buf->conf.userStats.mac[2],buf->conf.userStats.mac[3],buf->conf.userStats.mac[4],buf->conf.userStats.mac[5]);
    }

#if 1
    /*get url*/
    if((*((int*)data) == _httpCmdList[0]))
    {
    data += 4;
    }else if((*((int*)data) == _httpCmdList[1])) {
    data += 5;
    }
    for (i = 0; *data != 0x20; i++, data++)
    {
        if (i >= MAX_GET_URL_LEN)
        {
	    break;
        }
        uri[i] = *data;
    }
    uri[i] = 0;
    /*
     * 寻找访问主机
     */
    host = (__u8 *)bm_strstr(data, datalen, HTTP_HOST_P, sizeof(HTTP_HOST_P));
    if (host != NULL)
    {
	host += 6; 
	for (j = 0; *host != '\r'; j++, host++)
	{	
	    if (j >= MAX_GET_HOST_LEN)
	    {
		break;
	    }
	    if (_checkData(*host) == 1)
	    {
		url[j] = *host;
	    }
	}
	url[j] = 0;
    }
    snprintf(url_get, (MAX_GET_HOST_LEN+MAX_GET_URL_LEN+7+1),"http://%s%s%c",url,uri,'\0');
wifi_debug_print("%s-%d: host %s, uri %s\n",__func__,__LINE__,url,uri);
wifi_debug_print("%s-%d: url_get %s\n",__func__,__LINE__,url_get);
#if 1
    /* 修改移动端设备，多SSID未连接时切换问题
     * 如apple系统，WP系统
     * apple: User-Agent中包含wispr字段*/
    uaP = bm_strstr(data, datalen, HTTP_UA_P, sizeof(HTTP_UA_P));
    if(uaP !=  NULL)
    {
	__u8 *wispr[5] = {'w','i','s','p','r'};
	uaP += 12; 
	for (; *uaP != '\r'; uaP++)
	{
	    int k=0;
	    while((*uaP == wispr[k]) && (k<5)){
		k++;
		uaP++;
	    }
	    if(k>=5) {
		appleFlag = 1;
		wifi_debug_print("%s-%d: client ip %s,Apple phone  'wispr' find, buf count %u **********\n",__func__,__LINE__,srcIp,((buf!=NULL)?(buf->wifiInfo.count):(255)));
		break;
	    }
	}
    }
    if(appleFlag != 1) {
    if(bm_strstr(url, strlen(url), "www.msftncsi.com", 16) !=NULL)
    {
	if(bm_strstr(uri, strlen(uri), "ncsi.txt", 8) !=NULL)
	{
	    wpFlag = 1;
	    wifi_debug_print("%s-%d: client ip %s,Windows phone 'ncsi' find, buf count %u **********\n",__func__,__LINE__,srcIp,((buf!=NULL)?(buf->wifiInfo.count):(255)));
	}
    }
    }
    /*第一次的验证包做302处理，之后的放过*/
    if(buf !=NULL) {
	if((appleFlag == 1)) 
	{
	    if(buf->wifiInfo.count >= 1U) {
		buf->wifiInfo.count += 1U;
		return XT_CONTINUE;
	    } else {
		buf->wifiInfo.count += 1U;
		buf->wifiInfo.second = 0U;
	    }
	} else if(wpFlag == 1) {
	    if(buf->wifiInfo.count >= 2U) {
		buf->wifiInfo.count += 1U;
		return XT_CONTINUE;
	    } else {
		buf->wifiInfo.count += 1U;
		buf->wifiInfo.second = 0U;
	    }
	}
    }
#endif
    urlP = httpdUrlEncode(url_get);

#endif
#if 1
    /*参数签名部分实现开始*/
    sprintf(lanIp, "%u.%u.%u.%u", NIPQUAD(utt_local_address));

//wifi_debug_print("%s-%d: lanip %s, srcip %s, mac %s\n",__func__,__LINE__,lanIp,srcIp,mac);
    /*获取时间戳*/
    struct timex txc;
    struct rtc_time tm;
    do_gettimeofday(&(txc.time));
    time_to_tm(txc.time.tv_sec,0,&tm);
    memset(timeStr, 0, 15);
    snprintf(timeStr,15,"%04u%02u%02u%02u%02u%02u",tm.tm_year+1900,(tm.tm_mon+1), tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
    unsigned char hexData1[41];
    /*参数数字签名*/
    initParamval();
    set_secret_key(reject->secret_key);
    addParamValue("v", reject->server_version[0]?reject->server_version:NULL);
    addParamValue("deviceid", reject->redirect_val);
    addParamValue("gwaddress", lanIp);
    addParamValue("gwport", "2060");
    addParamValue("ip",srcIp);
    addParamValue("mac", mac);
    addParamValue("timestamp", timeStr);
    if(reject->unitPrice[0] != '\0')
    {/*׬Ǯwifi����*/
	addParamValue("unitPrice", reject->unitPrice);
    }
    if(urlP!=NULL)
    addParamValue("url",url_get);
    else
    addParamValue("url","");
    //getSha1Result();
    getSha1Result(hexData1);
    memset(httpData, 0, HTTP_DATA_LEN+1);
#ifdef CONFIG_UTT_KNOWIFI
	snprintf(httpData, HTTP_DATA_LEN, redirect_1, lanIp,timeStr);
#else
    if(reject->unitPrice[0]  != '\0')
    {
	if(urlP!=NULL) {
	    snprintf(httpData, HTTP_DATA_LEN, redirect_2, reject->auth_domain,reject->login_url,hexData1,reject->server_version[0]?"&v=":"",reject->server_version[0]?reject->server_version:"",lanIp,reject->redirect_val,srcIp,mac,timeStr,reject->unitPrice,urlP);
	} else {
	    snprintf(httpData, HTTP_DATA_LEN, redirect_2, reject->auth_domain,reject->login_url,hexData1,reject->server_version[0]?"&v=":"",reject->server_version[0]?reject->server_version:"",lanIp,reject->redirect_val,srcIp,mac,timeStr,reject->unitPrice,"");
	}
    }
    else
    {
	if(urlP!=NULL) {
	    snprintf(httpData, HTTP_DATA_LEN, redirect_1, reject->auth_domain,reject->login_url,hexData1,reject->server_version[0]?"&v=":"",reject->server_version[0]?reject->server_version:"",lanIp,reject->redirect_val,srcIp,mac,timeStr,urlP);
	} else {
	    snprintf(httpData, HTTP_DATA_LEN, redirect_1, reject->auth_domain,reject->login_url,hexData1,reject->server_version[0]?"&v=":"",reject->server_version[0]?reject->server_version:"",lanIp,reject->redirect_val,srcIp,mac,timeStr,"");
	}
    }  
#endif
    httpDatalen = strlen(httpData);
    wifi_debug_print("httpDatalen %d, redirect = %s\n", httpDatalen, httpData);
    if(urlP!=NULL)
	kfree(urlP);
    /*参数签名部分实现结束*/
#endif

#if 1
    /*首先回复一个ack确认包*/
    /*构造ACK确认包开始*/
    ack_skb = alloc_skb(sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr) + LL_MAX_HEADER, GFP_ATOMIC);
    if(!ack_skb)
	return NF_DROP;
    skb_reserve(ack_skb, LL_MAX_HEADER);

    skb_reset_network_header(ack_skb);
    /*ip头*/
    niph = (struct iphdr *)skb_put(ack_skb, sizeof(struct iphdr));
    niph->version       = 4;
    niph->ihl   = sizeof(struct iphdr) / 4;
    niph->tos   = 0;
    niph->id    = oiph->id;
    niph->frag_off = 0;
    niph->ttl = 160;
    niph->protocol = IPPROTO_TCP;
    niph->check = 0;
    niph->saddr = oiph->daddr;
    niph->daddr = oiph->saddr;

    /*tcp头*/
    ntcph = (struct tcphdr *)skb_put(ack_skb, sizeof(struct tcphdr));
    memset(ntcph, 0, sizeof(*ntcph));
    ntcph->source = otcph->dest;
    ntcph->dest = otcph->source;
    ntcph->doff = sizeof(struct tcphdr) >> 2;
    ntcph->window = 5000;
    if (otcph->ack) {
	ntcph->seq = otcph->ack_seq;
    //} else {
	ntcph->ack_seq = htonl(ntohl(otcph->seq) + otcph->syn + otcph->fin + 
		skb->len - ip_hdrlen(skb) - (otcph->doff << 2));                                                         
	ntcph->ack = 1;
    }
wifi_debug_print("%s-%d: ntcph seq %u, ack_seq %u, ack %u\n",__func__,__LINE__,ntcph->seq,ntcph->ack_seq,ntcph->ack);
    ntcph->check = ~tcp_v4_check(sizeof(struct tcphdr), niph->saddr, niph->daddr, 0);
    ack_skb->ip_summed = CHECKSUM_PARTIAL;
    ack_skb->csum_start = (unsigned char *)ntcph - ack_skb->head;
    ack_skb->csum_offset = offsetof(struct tcphdr, check);
    
    //skb_dst_set_noref(ack_skb, skb_dst(skb));
    /* ip_route_me_harder expects skb->dst to be set */
    dst_hold(&rt->dst);
	skb_dst_set(ack_skb, &rt->dst);

    ack_skb->protocol = htons(ETH_P_IP);
    if (ip_route_me_harder(ack_skb, RTN_LOCAL)) {
	nskb = ack_skb;
	goto free_nskb;
    }

    nf_ct_attach(ack_skb, skb);
    ip_local_out(ack_skb);
    /*构造ACK确认包开始*/
#endif 
    /*构造http响应包*/
    nskb = alloc_skb(sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr) + httpDatalen + LL_MAX_HEADER, GFP_ATOMIC);
    if(!nskb)
	return NF_DROP;

    skb_reserve(nskb, LL_MAX_HEADER);
    skb_reset_network_header(nskb);
    /*ip头*/
    niph = (struct iphdr *)skb_put(nskb, sizeof(struct iphdr));
    niph->version       = 4;
    niph->ihl   = sizeof(struct iphdr) / 4;
    niph->tos   = 0;
    niph->id    = oiph->id;
    niph->frag_off = 0;
    niph->ttl = 160;
    niph->protocol = IPPROTO_TCP;
    niph->check = 0;
    niph->saddr = oiph->daddr;
    niph->daddr = oiph->saddr;

    /*tcp头*/
    ntcph = (struct tcphdr *)skb_put(nskb, sizeof(struct tcphdr));
    memset(ntcph, 0, sizeof(*ntcph));
    ntcph->source = otcph->dest;
    ntcph->dest = otcph->source;
    ntcph->doff = sizeof(struct tcphdr) >> 2;
    ntcph->window = 5000;
    if (otcph->ack) {
	ntcph->seq = otcph->ack_seq;
    //} else {
	ntcph->ack_seq = htonl(ntohl(otcph->seq) + otcph->syn + otcph->fin + 
		skb->len - ip_hdrlen(skb) - (otcph->doff << 2));                                                         
	ntcph->ack = 1;
	ntcph->psh = 1;
    }
    /*http数据*/
    httpdataP = (unsigned char *)skb_put(nskb, (httpDatalen));
    sprintf(httpdataP, "%s",httpData);

#if 1
    /*tcp 校验和*/
    niph->tot_len = htons(nskb->len);
    ntcph->check = 0;
    nskb->csum = csum_partial((unsigned char *)ntcph, nskb->len - (niph->ihl * 4),0);
    ntcph->check = csum_tcpudp_magic(niph->saddr, niph->daddr, 
	    ntohs(niph->tot_len) - (niph->ihl * 4),niph->protocol, 
	    nskb->csum);
#else
    ntcph->check = ~tcp_v4_check(sizeof(struct tcphdr), niph->saddr, niph->daddr, 0);
    nskb->ip_summed = CHECKSUM_PARTIAL;
    nskb->csum_start = (unsigned char *)ntcph - nskb->head;
    nskb->csum_offset = offsetof(struct tcphdr, check);
#endif

    /* ip_route_me_harder expects skb->dst to be set */
    //skb_dst_set_noref(nskb, skb_dst(skb));
    /* ip_route_me_harder expects skb->dst to be set */
    dst_hold(&rt->dst);
	skb_dst_set(nskb, &rt->dst);

    nskb->protocol = htons(ETH_P_IP);
    if (ip_route_me_harder(nskb, RTN_LOCAL))
	goto free_nskb;

    nf_ct_attach(nskb, skb);
    ip_local_out(nskb);
wifi_debug_print("%s-%d: \n",__func__,__LINE__);
    return NF_DROP;

free_nskb:
    kfree_skb(nskb);
    return NF_DROP;
}

static unsigned int send_wifi_redirect(struct sk_buff *skb)
{

    return NF_DROP;
}

static unsigned int
wifireject_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct ipt_wifireject_info *reject = par->targinfo;
	int dir_len = 0;
	unsigned int ret = NF_DROP;
	
	dir_len = strlen(reject->redirect_dir);
	if(dir_len>0) {
	    ret = send_wifi_redirect(skb);
	} else {
	    ret = send_wifi_auth_redirect(skb, reject, par->hooknum);
	}

	return ret;
}

static int wifireject_tg_check(const struct xt_tgchk_param *par)
{
	const struct ipt_wifireject_info *rejinfo = par->targinfo;

	if (strlen(rejinfo->redirect_dir) <= 0) {
	    if (strlen(rejinfo->redirect_val) <= 0) {
		pr_info("WIFIREJECT invalid for redirect val sn.\n");
		return -EINVAL;
	    }
	    if (strlen(rejinfo->auth_domain) <= 0) {
		pr_info("WIFIREJECT invalid for auth server domain.\n");
		return -EINVAL;
	    }
	    if (strlen(rejinfo->login_url) <= 0) {
		pr_info("WIFIREJECT invalid for login request url.\n");
		return -EINVAL;
	    }
	}
	return 0;
}

static struct xt_target wifireject_tg_reg __read_mostly = {
	.name		= "WIFIREJECT",
	.family		= NFPROTO_IPV4,
	.target		= wifireject_tg,
	.targetsize	= sizeof(struct ipt_wifireject_info),
	.table		= "filter",
	.hooks		= (1 << NF_INET_LOCAL_IN) | (1 << NF_INET_FORWARD) |
			  (1 << NF_INET_LOCAL_OUT),
	.checkentry	= wifireject_tg_check,
	.me		= THIS_MODULE,
};

static int __init wifireject_tg_init(void)
{
	return xt_register_target(&wifireject_tg_reg);
}

static void __exit wifireject_tg_exit(void)
{
	xt_unregister_target(&wifireject_tg_reg);
}

module_init(wifireject_tg_init);
module_exit(wifireject_tg_exit);
