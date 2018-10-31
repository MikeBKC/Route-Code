#include <linux/string.h>
#include <linux/ctype.h>
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
#include <linux/netfilter/xt_UTTLOG.h>
#include <net/netfilter/nf_conntrack.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv4/ip_set.h>



/*
 * UTT_DEB_POLICY_ROUTE 必须为 1
 */
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
#define UTT_POLICY_ROUTE_DEBUG(fmt, argx...) policy_route_print(fmt, ##argx)
static unsigned int policy_route_debug = 0U;
#else
#define UTT_POLICY_ROUTE_DEBUG(fmt, argx...)
#endif

#if 0
#define LAN_GROUP_NAME "sys_lan_group"
#endif
PolicyRouteEntryVal *head = NULL;

#if 0
asmlinkage int printk_route(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}
#endif
static int policy_route_print(const char* fmt, ...)
{
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    va_list   args;
    if (policy_route_debug) {
	va_start(args, fmt);
	debug_kprint(fmt, args);
	va_end(args);
    }
#endif
#endif
    return 0;
}
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
extern void utt_policy_route_debug(UttNfConfType type)
{
    if (type == UTT_NFCONF_ADD) {
	policy_route_debug = 1U;
    } else {
	policy_route_debug = 0U;
    }
    return;
}
/********************************************************************
 * 函数： uttNlPolicyRouteListPrint
 * 功能： 策略路由调试输出所有策略路由配置
 * 创建： 2013-02-20
 * 参数： 无
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static void uttPolicyRouteListPrint(void)
{
#if 1
    int i = 0;
    PolicyRouteEntryVal *tmp = NULL;
    if(head != NULL) {
	tmp = head;
	while(tmp != NULL) {
	    UTT_POLICY_ROUTE_DEBUG("%s-%d: i = %d\n",__FUNCTION__,__LINE__,i++);
	    UTT_POLICY_ROUTE_DEBUG(" bindMark = 0x%x\n",tmp->bindMark);
	    UTT_POLICY_ROUTE_DEBUG(" srcIP :\n");
	    if(tmp->ip.srcIP.entryType==IP_RANGE){
		UTT_POLICY_ROUTE_DEBUG(" ipFrom = 0x%x, ipTo = 0x%x\n", tmp->ip.srcIP.entryVal.range.ipFrom,tmp->ip.srcIP.entryVal.range.ipTo);
	    }else{
		UTT_POLICY_ROUTE_DEBUG(" srcipGrpName=%s\n",(char *)(tmp->ip.srcIP.entryVal.name));
	    }
	    UTT_POLICY_ROUTE_DEBUG(" dstIP :\n");
	    if(tmp->ip.dstIP.entryType==IP_RANGE){
		UTT_POLICY_ROUTE_DEBUG(" ipFrom = 0x%x, ipTo = 0x%x\n", tmp->ip.dstIP.entryVal.range.ipFrom,tmp->ip.dstIP.entryVal.range.ipTo);
	    }else{
		UTT_POLICY_ROUTE_DEBUG(" dstipGrpName=%s\n",(char *)(tmp->ip.dstIP.entryVal.name));
	    }

	    UTT_POLICY_ROUTE_DEBUG(" protocol = %d, dstPortFrom = %d, dstPortTo = %d\n", tmp->apply.protocol,tmp->apply.dstFromPort,tmp->apply.dstEndPort);
	    UTT_POLICY_ROUTE_DEBUG(" wday = %d, timestart = %d, timestop = %d\n", tmp->prTime.wday,tmp->prTime.timestart,tmp->prTime.timestop);
	    tmp = tmp->next;
	}
    }else{
	    UTT_POLICY_ROUTE_DEBUG("%s-%d: null\n",__FUNCTION__,__LINE__);
    }
#endif
}

/********************************************************************
 * 函数： uttNlPolicyRouteListPrint
 * 功能： 策略路由调试输出所有策略路由配置
 * 创建： 2013-02-20
 * 参数： 无
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static void uttPolicyRouteConfOnePrint(struct sk_buff *skb,PolicyRouteEntryVal *tmp)
{
#if 1
    struct iphdr *ipH = NULL;
    if(tmp != NULL) {
	ipH = ip_hdr(skb);
	UTT_POLICY_ROUTE_DEBUG("%s-%d: -----match entry-----\n",__FUNCTION__,__LINE__);
	UTT_POLICY_ROUTE_DEBUG("ntohl(ipH->saddr) =0x%x,ntohl(ipH->daddr) =0x%x\n",ntohl(ipH->saddr),ntohl(ipH->daddr));
	UTT_POLICY_ROUTE_DEBUG(" bindMark = 0x%x\n",tmp->bindMark);
	UTT_POLICY_ROUTE_DEBUG(" srcIP :\n");
	if(tmp->ip.srcIP.entryType==IP_RANGE){
	    UTT_POLICY_ROUTE_DEBUG(" ipFrom = 0x%x, ipTo = 0x%x\n", tmp->ip.srcIP.entryVal.range.ipFrom,tmp->ip.srcIP.entryVal.range.ipTo);
	}else{
	    UTT_POLICY_ROUTE_DEBUG(" srcipGrpName=%s\n",(char *)(tmp->ip.srcIP.entryVal.name));
	}
	UTT_POLICY_ROUTE_DEBUG(" dstIP :\n");
	if(tmp->ip.dstIP.entryType==IP_RANGE){
	    UTT_POLICY_ROUTE_DEBUG(" ipFrom = 0x%x, ipTo = 0x%x\n", tmp->ip.dstIP.entryVal.range.ipFrom,tmp->ip.dstIP.entryVal.range.ipTo);
	}else{
	    UTT_POLICY_ROUTE_DEBUG(" dstipGrpName=%s\n",(char *)(tmp->ip.dstIP.entryVal.name));
	}

	UTT_POLICY_ROUTE_DEBUG(" protocol = %d, dstPortFrom = %d, dstPortTo = %d\n", tmp->apply.protocol,tmp->apply.dstFromPort,tmp->apply.dstEndPort);
	UTT_POLICY_ROUTE_DEBUG(" wday = %d, timestart = %d, timestop = %d\n", tmp->prTime.wday,tmp->prTime.timestart,tmp->prTime.timestop);
    }else{
	    UTT_POLICY_ROUTE_DEBUG("%s-%d: null\n",__FUNCTION__,__LINE__);
    }
#endif
}

#endif
#endif


/********************************************************************
 * 函数： uttNlPolicyRouteAdd
 * 功能： 增加一条策略路由配置
 * 创建： 2013-02-20
 * 参数： newEntry
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static int uttNlPolicyRouteAdd(PolicyRouteEntryVal *newEntry)
{
    PolicyRouteEntryVal *new = NULL, *tmp = NULL;

    if(newEntry != NULL) {
	new = (PolicyRouteEntryVal *)kmalloc(sizeof(PolicyRouteEntryVal), GFP_KERNEL);
	if(new == NULL){
	    return 0;
	}
	memcpy(new, newEntry, sizeof(PolicyRouteEntryVal));
	new->next = NULL;
	if(head == NULL) {
	    head = new;
	} else {
	    tmp = head;
	    while(tmp->next != NULL) {
		tmp = tmp->next;
	    }
	    tmp->next = new;
	}
    }
#if 0 //UTT_DEB_POLICY_ROUTE
    uttPolicyRouteListPrint();
#endif
    return 1;
}
/********************************************************************
 * 函数： uttNlPolicyRouteDelAll
 * 功能： 策略路由配置删除函数
 * 创建： 2013-02-20
 * 参数： 无
 * 返回： 略
 * 输出： 略
 * 作者： 
 ********************************************************************/ 
static int uttNlPolicyRouteDelAll(void)
{
    PolicyRouteEntryVal *tmp = NULL;

	for(tmp=head;tmp!=NULL;)
	{
	    head=tmp->next;
	    kfree(tmp);
	    tmp=head;
	}
#if 0 //UTT_DEB_POLICY_ROUTE
    uttPolicyRouteListPrint();
#endif
    return 1;
}
/**
 *
 */
/********************************************************************
 * 函数： uttNlPolicyRouteConf
 * 功能： netlink通信 策略路由配置函数
 * 创建： 2013-02-20
 * 参数： 无
 * 返回： 略
 * 输出： 略
 * 作者： 
 ********************************************************************/ 
void uttNlPolicyRouteConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfPolicyRouteConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfPolicyRouteConf*)NLMSG_DATA(nlh);/*取值*/

#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    UTT_POLICY_ROUTE_DEBUG("%s-%d: nlData->confType %d\n", __func__, __LINE__, nlData->confType);/* Policy Route */
#endif
#endif
    switch(nlData->confType)/*类别*/
    {
	case UTT_NFCONF_ADD:
	    uttNlPolicyRouteAdd(&(nlData->entryVal));
	    break;
	case UTT_NFCONF_DEL_ALL:
	    uttNlPolicyRouteDelAll();
	    break;
	default:/*test*/
	    break;

    }
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    switch(nlData->debugType)/*类别*/
    {
	case UTT_DEBUG_LIST_ALL:
	    uttPolicyRouteListPrint();
	    break;
	default:
	    break;
    }
#endif
#endif
    return;
}
/********************************************************************
 * 函数： uttNfIpMatch
 * 功能： 数据包IP地址的匹配函数
 * 创建： 2013-02-20
 * 参数： skb:   skb包
 *	  ipGrp: 要匹配的IP组
 *	  ip:	 数据包ip地址
 *	  flag:  取值为IPSET_SRC 或 IPSET_DST
 *		 如果是IPSET_SRC，则ip为数据包源ip
 *		 如果是IPSET_DST，则ip为数据包目的ip
 * 返回： 1  匹配成功
 *	  0  匹配失败
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
int UttNfIpMatch(struct sk_buff *skb, IpGrpEntry *ipGrp, u_long ip, uint32_t flag)
{
    ip_set_id_t ipsetindex = IP_SET_INVALID_ID;
    uint32_t ret = 0, testflag = flag;
#if 0
    char langroup[16];
#endif
    if(ipGrp != NULL) {
    if(ipGrp->entryType == IP_RANGE){ 
	/* ip 地址段匹配
	 * 当起始和结束ip都为0x0时，默认为是所有地址
	 * */
#if 1
	/*所有地址*/
	if((ipGrp->entryVal.range.ipFrom == 0U)&&(ipGrp->entryVal.range.ipTo == 0U))
	{
	    ret = 1;
	}else if((ip >= ipGrp->entryVal.range.ipFrom)&&(ip <= ipGrp->entryVal.range.ipTo)) {	/*地址段比较*/
	    ret = 1;
	}
#endif
    }else if(ipGrp->entryType == IP_GROUP){
	/* ip 组匹配
	 * 由组名，获取ipset的索引值
	 * 然后根据索引值检测ip是否在此ipset组中*/
#if 0
	//UTT_POLICY_ROUTE_DEBUG("%s-%d: ipsetindex = %d, ipGrpName = %s\n",__FUNCTION__,__LINE__, ipsetindex, ipGrp->entryVal.name);
#endif
#if 1
	if(strlen((char *)(ipGrp->entryVal.name))!=0){
	    ipsetindex = ip_set_get_byname((char *)(ipGrp->entryVal.name));
	    if(ipsetindex != IP_SET_INVALID_ID) {

	    if(ip_set_testip_kernel(ipsetindex, skb, &testflag)!=0){ /*检测IP*/
		ret = 1;
	    }
	    ip_set_put_byindex(ipsetindex);
	    }
	}
#endif
    }
#if 0
    /*当目的地址为lan口地址是，直接返回，防止到内网的包从wan口出去*/
    if((testflag == IPSET_DST)&&(ret == 1)) {
	ipsetindex = IP_SET_INVALID_ID;
	memset(langroup, 0, 16);
	strcpy(langroup, LAN_GROUP_NAME);
	if(strlen((char *)langroup)!=0){
	    ipsetindex = ip_set_get_byname((char *)langroup);
	    if(ipsetindex != IP_SET_INVALID_ID) {

	    if(ip_set_testip_kernel(ipsetindex, skb, &testflag)!=0){ /*检测IP*/
		ret = 0;
	    }
	    ip_set_put_byindex(ipsetindex);
	    }
	}
    }
#endif
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    UTT_POLICY_ROUTE_DEBUG("%s-%d: %s ip match %d\n",__FUNCTION__,__LINE__,((flag == IPSET_SRC)?"src":"dst"),ret);
#endif
#endif
    }
    return ret;
}
/********************************************************************
 * 函数： uttNfPortMatch
 * 功能： 数据包服务端口的匹配函数
 * 创建： 2013-02-20
 * 参数： srcport:  要匹配的起始端口
 *	  dstport:  要匹配的结束端口
 *	  port:	    数据包的源或目的端口
 *
 * 返回： 1  匹配成功
 *	  0  匹配失败
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
int UttNfPortMatch(unsigned short srcport, unsigned short dstport, unsigned short port)
{
    int ret = 0;
    
#if 1
    /*所有端口*/
    if ((srcport == 0U) && (dstport == 0U)) 
    {
	ret = 1;
    }
    else if((port >= srcport)&&(port <= dstport))   /*端口比较*/
    {
	ret = 1;
    }
#endif
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
    UTT_POLICY_ROUTE_DEBUG("%s-%d: port match %d\n",__FUNCTION__,__LINE__,ret);
#endif
#endif
    return ret;
}
#if 0
static void addTestEntry(void)
{
    PolicyRouteEntryVal *new = NULL;
    
    new = (PolicyRouteEntryVal *)kmalloc(sizeof(PolicyRouteEntryVal), GFP_KERNEL);
    if(new == NULL){
	return;
    }
    memset(new, 0x00, sizeof(PolicyRouteEntryVal));
    new->next =NULL;
    new->bindMark = 0x10U;
    new->ip.srcIP.entryType = IP_RANGE;
    new->ip.srcIP.entryVal.range.ipFrom = 0x189a8c0;
    new->ip.srcIP.entryVal.range.ipTo = 0xc889a8c0;
    new->ip.dstIP.entryType = IP_RANGE;
    new->ip.dstIP.entryVal.range.ipFrom = 0x0;
    new->ip.dstIP.entryVal.range.ipTo = 0x0;
    new->apply.protocol = 0x6;
    new->apply.dstFromPort = 80;
    new->apply.dstEndPort = 100;
    new->prTime.wday = 0x7f;
    uttNlPolicyRouteAdd(new);

    kfree(new);
    return;
}
#endif
/********************************************************************
 * 函数： uttNfPolicyRouteListMatch
 * 功能： 策略路由配置匹配
 * 创建： 2013-02-20
 * 参数： skb包
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
extern int uttNfPolicyRouteListMatch( struct sk_buff *skb )
{
    struct iphdr *ipH = NULL;
    struct tcphdr *tcp = NULL;
    struct udphdr *udp = NULL;
    struct nf_conn *conntrack;
    enum ip_conntrack_info ctinfo;
    UttNfCurTime pakTime;
    PolicyRouteEntryVal *entry = NULL;
    int apply_flag = 0;

    __u16 iph_len = 0; 
    entry = head;
    if(entry != NULL) {
	if(isSkbComeFromLan(skb)) { /*从Lan口来的包*/
	if (!(conntrack = nf_ct_get(skb, &ctinfo))) {
	    return XT_CONTINUE;
	}
	if(ctinfo == IP_CT_NEW) { /*新起连接*/
	uttNfPolicyGetTime(skb, &pakTime);  /*获取包到达时间*/
#if 0
	UTT_POLICY_ROUTE_DEBUG("%s-%d: curTime->week=0x%x,cunTime->time=%u\n",__FUNCTION__,__LINE__,pakTime.week, pakTime.time);
	UTT_POLICY_ROUTE_DEBUG("%s-%d: ipH->saddr = 0x%x,ipH->daddr=0x%x\n",__FUNCTION__,__LINE__,ipH->saddr,ipH->daddr);
	UTT_POLICY_ROUTE_DEBUG("%s-%d: conntrack->mark = 0x%x, ctinfo = 0x%x\n",__FUNCTION__,__LINE__,conntrack->mark,ctinfo);
#endif
	iph_len = ip_hdrlen(skb);
	ipH = ip_hdr(skb);
	/* 循环遍历策略路由配置 */
	while(entry != NULL) {
	if (entry->prTime.wday&pakTime.week) { /*星期*/
	if (uttTimeIn(pakTime.time , entry->prTime.timestart, entry->prTime.timestop)){  /*时间内*/
	    if(UttNfIpMatch(skb, &(entry->ip.srcIP),ntohl(ipH->saddr),IPSET_SRC)!=0) {
	    if(UttNfIpMatch(skb, &(entry->ip.dstIP),ntohl(ipH->daddr),IPSET_DST)!=0) {
#if 1
		if(entry->apply.protocol == 0U) {   /*所有协议*/
		    apply_flag = 1;
		} else {
		switch (ipH->protocol) 
		{
		    case PROT_TCP:
			if(entry->apply.protocol == PROT_TCP) {
			tcp = (struct tcphdr *)(skb_network_header(skb) + iph_len); 
			if(UttNfPortMatch(entry->apply.dstFromPort, entry->apply.dstEndPort, ntohs(tcp->dest))!=0){	/*端口匹配*/
			    apply_flag = 1;
			}
			}
			else
			{
			    /*null*/
			}
			break;
		    case PROT_UDP:
			if(entry->apply.protocol == PROT_UDP) {
#if 1
			udp = (struct udphdr *)(skb_network_header(skb) + iph_len);
			if(UttNfPortMatch(entry->apply.dstFromPort, entry->apply.dstEndPort, ntohs(udp->dest))!=0) {	/*端口匹配*/
			    apply_flag = 1;
			}
#endif
			}
			else
			{
			    /*null*/
			}
			break;
		    case PROT_ICMP:
			if(entry->apply.protocol == PROT_ICMP) {
			    apply_flag = 1;
			}
			break;
		    case PROT_AH:
			if(entry->apply.protocol == PROT_AH) {
			    apply_flag = 1;
			}
			break;
		    default:
			break;
		}
		}
#endif
#if 1
		if(apply_flag==1)
		{
#if 0
			    UTT_POLICY_ROUTE_DEBUG("%s-%d: curTime->week=%d,cunTime->time=%d\n",__FUNCTION__,__LINE__, pakTime.week, pakTime.time);
			    UTT_POLICY_ROUTE_DEBUG("%s-%d: conntrack->mark = 0x%x\n",__FUNCTION__,__LINE__,conntrack->mark);
#endif
#if 1
			    conntrack->mark &= (~POLICYROUTE_CONNMARK_MSK);
			    conntrack->mark |= (entry->bindMark);	/*给会话打mark*/
#ifdef CONFIG_UTT_DEBUG
#ifdef CONFIG_UTT_DEB_POLICY_ROUTE
			    uttPolicyRouteConfOnePrint(skb,entry);
#endif
#endif
			    break;
#endif
		}else{
		    /*null*/
		}
#endif
	    }
	    }
	}
	}
	    entry = entry->next;
	}
	}else{
	    return XT_CONTINUE;
	}
	}
    }
    return XT_CONTINUE;
}

