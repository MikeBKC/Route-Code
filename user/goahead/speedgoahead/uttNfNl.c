#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/autoconf.h>
#include <arpa/inet.h>
#ifdef CONFIG_RALINK_RT2880
#include <linux/types.h>
#endif
#include <linux/netlink.h>
#include <linux/socket.h>
#include "uttMachine.h"
#ifndef CONFIG_RALINK_RT2880
#include <linux/netfilter_ipv4/utt_netfilter.h>
#endif
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mib.h"
#include "profacce.h"
#include "unistd.h"

#if ((GROUP_BASE == FYES)||(WEB_AUTH == FYES))
static pthread_t nl_task_thread;
static int nl_sock_fd;
#if (WEBAUTH_ACCOUNT == FYES)
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
extern void uttNfWebAuthCancel_byIp(unsigned long ip);
#endif
#endif
#define UTT_NFNL_DEBUG 0

#ifdef CONFIG_UTT_CLOUD_CONFIG
static void updateNoticeMacList(MacAddr newMac,unsigned int accessType);
static NoticeMac *findNoticeMacList(MacAddr mac,int* isCreat);
#endif 

#if UTT_NFNL_DEBUG
#define DEBUG_UTT_NFNL(fmt, argx...) printf(fmt, ##argx)
#else
#define DEBUG_UTT_NFNL(fmt, argx...)
#endif

#if UTT_NFNL_DEBUG
static void print_members(unsigned char *member, int  size)
{
    int i;

    for(i=0;i<size;i++)
    {
        printf("%02x", member[i]);
    }
    return;
}

#endif

#if 0
/**
 * 用speedweb中的 netlintk任务线程 绑定的 sock_fd
 * 发送netlink消息
 */
int uttNfNlTaskSend(char *data,Uint32 dataLen, Uint32 dpid, Uint32 dgrp)
{
    /*用本线程中绑定的netlink，发送消息*/
   return uttNfNlSend(nl_sock_fd, data, dataLen, dpid, dgrp);
}
#endif

/**
 * utt netlink 初始化。
 * 告诉内核 主动通信的对象。可以让内核通过单播 主动和本进程通信
 */
void uttNfNlInit(int sock_fd)
{
    UttNlNfPolicy conf;
    conf.msgHead.nlType = UTT_NLNF_POLICY;
    conf.confType = UTT_NFCONF_INIT;
    uttNfNlSend(sock_fd, (char *)&conf, sizeof(conf), 0, 0);/*发送给内核自己的pid*/
    return;
}
#if 0
/*如果把接受消息放于speedweb中。则可以用此函数接受消息
 * 而用select函数监控 netlink 的sock 以及 speedweb本身的消息接受 句柄
 *
 * */
int uttNLRcv(int sock_fd)
{
    struct sockaddr_nl nl_addr;
    struct iovec iov;
    struct msghdr msg;
    char buf[UTT_MAX_NLMSGSIZE];
    struct nlmsghdr *nlh = NULL;
    UttNlNfMsgHead *msgHead = NULL;

    memset(&msg, 0, sizeof(msg));
    memset(buf, 0 ,sizeof(buf));

    nlh = (struct nlmsghdr*) buf;
    iov.iov_base = (void *)nlh;
    iov.iov_len = UTT_MAX_NLMSGSIZE;
    msg.msg_name = (void *)&nl_addr;
    msg.msg_namelen = sizeof(nl_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if(recvmsg(sock_fd, &msg, MSG_DONTWAIT) > 0)
    {
	msgHead = (UttNlNfMsgHead*)NLMSG_DATA(nlh);
	printf("Message received in user %d, type:%d\n",getpid(), msgHead->nlType) ;
    }

    return 0;
}
#endif
#if (GROUP_BASE == FYES)
static void uttNfNlPolicyAdd(unsigned long ipaddr)
{
    UttNlNfPolicy  nlConf;
    char *ipstr = NULL;
    struct in_addr addrS;

    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int max, min, i;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_POLICY;/*类型*/
    /*增加*/
    nlConf.confType = UTT_NFCONF_ADD;
    nlConf.ip = ipaddr;

    addrS.s_addr = ipaddr;
    ipstr = inet_ntoa(addrS);/*转换为ip字串*/
    uttSemP(SEM_UTTNF_NO,0);/*加锁防止冲突*/
    ProfInstLowHigh(mibType, &max, &min);

    ipaddr = ntohl(ipaddr);
    DEBUG_UTT_NFNL("%s %d %s: Message received in user\n",__func__, __LINE__, ipstr) ;
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if((ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    if(prof->groupType == GBT_ACT_CONTROL)
	    {/*上网行为管理*/
		if(prof->addrTypeEn == ADDR_IPRANGE)
		{/*按网段进行限制*/
		    if((prof->ipStart != 0) && (prof->ipEnd != 0))
		    {
			if( (ipaddr < ntohl(prof->ipStart))  ||
				(ipaddr > ntohl(prof->ipEnd)) )
			{/*不在该网段内*/
			    continue;
			}
		    }
		}
#if (IP_GRP == FYES)
		else
		{/*组用户*/
		    if(prof->ipGrpName[0] != '\0')/*非所有用户*/
		    {
			if(ipsetTestIpStr(prof->ipGrpName, ipstr) != 1)
			{/*按组进行限制*/
			    continue;/*不再改组内*/
			}
		    }
		}
#endif
#if UTT_NFNL_DEBUG
		printf("\n\n%s %d  send policy  ip:%s policyname:%s in user and policy is :\n", 
			__func__, __LINE__, ipstr, prof->head.name);
		print_members(prof->pdbPolicy1, UTT_MAX_PDB_POLICY);
		printf("\n\n%s %d send end\n\n", __func__, __LINE__);
#endif
		memcpy( & nlConf.policyVal.nfApps.appFlags, prof->pdbPolicy1, UTT_MAX_PDB_POLICY );/*应用*/
		nlConf.policyVal.nfTime.wday = prof->day;/*星期*/
		nlConf.policyVal.nfTime.timestart = prof->sTimeStart;/*开始时间*/
		nlConf.policyVal.nfTime.timestop = prof->sTimeStop;/*结束时间*/
		uttNfNlSend(nl_sock_fd,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*发送消息给内核*/
	    }

	}

    }
    uttSemV(SEM_UTTNF_NO,0);/*释放锁*/
    return;
}
/*行为管理规则设置函数*/
static void uttNlConfReply(UttNlNfPolicy* confPtr)
{

    DEBUG_UTT_NFNL("%s %d : Message received in user\n",__func__, __LINE__) ;
    switch(confPtr->confType)
    {
	case UTT_NFCONF_ADD:/*内核请求增加配置*/
	    uttNfNlPolicyAdd(confPtr->ip);
	    break;
	default:
	    break;
    }
    return;
}
#endif
#if (WIFIDOG_AUTH == FYES)
extern Uint32 getWebauthType(void);
#endif
#if (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
extern void WifiDog_IpDown(char *ip);
#endif

#ifdef CONFIG_UTT_CLOUD_CONFIG
/**
 *增加上线提醒的处理函数
 *xrg
 *
 * **/
static void uttNlOnlineProcess(UttNlNfOnRemind* confPtr)
{
    MacAddr mac ;
    unsigned int accessType;
    uttSemP(SEM_CLOUD_CONFIG,0);/*dhcp中也会进行读写，加锁防止冲突*/

    memcpy(&mac,confPtr->newMac,ETH_ALEN);
    accessType = confPtr->noticeBanMask;
    updateNoticeMacList(mac,accessType);

    uttSemV(SEM_CLOUD_CONFIG,0);/*dhcp中也会进行读写，加锁防止冲突*/

    return ;
}

static void updateNoticeMacList(MacAddr newMac,unsigned int accessType)
{
    int isCreat = 1;
    int min = 0, max = 0;
    unsigned int i,j;
GroupBaseProfile* prof_limit;
 
    NoticeMac *noticeMac = findNoticeMacList(newMac,&isCreat);


    /*读取dhcp文件得到hostname*/


    noticeMac->accessType = accessType;
    if(isCreat == 1)
    {
	noticeMac->accessType |= (1u << UTT_ACCESS_NEW);	/*新用户左移0位*/
    }
    else
    {
	noticeMac->accessType |= (1u << UTT_ACCESS_AGAIN);  /*短期内再次接入，左移1位*/
    }
    /*查询限速列表*/
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++)
    {
	prof_limit = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof_limit != NULL && ProfInstIsFree(prof_limit) == 0)
	{
	   if(memcmp(&(prof_limit->mac_addr),&newMac,ETH_ALEN)==0)
	    {
		noticeMac->accessType |= (1u << UTT_ACCESS_MACLIMIT);/*mac限速左移6位*/		
	    }
	}
    }

    /*查询白名单列表*/
    mibType = MIB_PROF_MAC_FILTER_GLOBAL ;
    MacFilterGlobalProfile *profGlobal = NULL;
    profGlobal = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibType, 0);
    MacFilterProfile * prof_filter;
    if(profGlobal != NULL && ProfInstIsFree(profGlobal) == 0)
    {
	if(profGlobal->head.active == MAC_ALLOW)
	{
	    mibType = MIB_PROF_MAC_FILTER ;
	    ProfInstLowHigh(mibType, &max, &min);
	    for(i = min; i < max; i++)
	    {
		prof_filter = (MacFilterProfile*)ProfGetInstPointByIndex(mibType, i);
		if(prof_filter != NULL && ProfInstIsFree(prof_filter) == 0)
		{
		    if(memcmp(&(prof_filter->mac),&newMac,ETH_ALEN)==0)
		    {
			noticeMac->accessType |= (1u << UTT_ACCESS_WHITE);/*白名单用户左移4位*/		
		    }
		}
	    }

	}
    }

   /*查询微信备注列表*/ 
    mibType = MIB_PROF_CLOUD_CONFIG;
    CloudConfigProfile* prof_cloud;
    prof_cloud = (CloudConfigProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof_cloud != NULL && ProfInstIsFree(prof_cloud) == 0)
    {
	for(i = 0;i < MAX_FRIEND_NUM;i++)/*遍历备注列表数组*/
	{
	    if(memcmp(&(prof_cloud->friendMac[i]),&newMac,ETH_ALEN)==0)
	    {
		noticeMac->accessType |= (1u << UTT_ACCESS_REMARK);/*做过备注的用户，左移5位*/		
	    }
	}
    }
    if(noticeMac->accessType & (~prof_cloud->noticeBanMask))
    {
	if(noticeMac->sendFlag == 0)
	{
	    noticeMac->sendFlag = 1;
	    nvramProfile->needSendCount++;/*增加计数，表示需要发送消息给服务器*/
	}
    }

    return ;
}

static NoticeMac *findNoticeMacList(MacAddr mac,int* isCreat)
{
    MacAddr tmp;
    memset(&tmp,0,sizeof(MacAddr));
    unsigned int i;
    for(i = 0 ; i < MAX_NOTICE_MAC_LIST ; i++)
    {
	if(memcmp(nvramProfile->noticeMacList[i].macaddr.addr,&mac,ETH_ALEN)==0)
	{
    
	    *isCreat = 0;
	    return &(nvramProfile->noticeMacList[i]);
	} 
	else if(memcmp(nvramProfile->noticeMacList[i].macaddr.addr,&tmp,1)==0)
	{
	    memcpy(&(nvramProfile->noticeMacList[i].macaddr.addr),&mac,ETH_ALEN);
	    return &(nvramProfile->noticeMacList[i]);
	}
    }
    unsigned int ret = ((nvramProfile->noticemac_list_count)++)%MAX_NOTICE_MAC_LIST;
    memcpy(nvramProfile->noticeMacList[ret].macaddr.addr,&mac,ETH_ALEN);
    printf("the list over\n");
    return &(nvramProfile->noticeMacList[ret]);
    /*需要写满的情况在全局中增加一个计数器*/

}
#endif

#if ((WEBAUTH_ACCOUNT == FYES) || (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY))
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/*ip 下线处理函数*/
static void uttNlIpDownReply(UttNlNfHashConf* confPtr)
{

#if(WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)
    struct in_addr addr;
    char *ptr = NULL;
#endif
    DEBUG_UTT_NFNL("%s %d : Message received in user\n",__func__, __LINE__) ;
    switch(confPtr->confType)
    {
	case UTT_NFCONF_DEL:/*内核请求ip下线*/
	    DEBUG_UTT_NFNL("%s %d: confType del, del ip =0x%x\n",__func__, __LINE__,confPtr->ip) ;
#if(WIFIDOG_MAKE_MONEY == FYES)
	    /*通知wifidog下线*/
	    addr.s_addr=confPtr->ip;	
	    if((ptr = inet_ntoa(addr))!=NULL) {
		fprintf(stderr,"%s-%d: down ip: %s\n",__func__,__LINE__,ptr);
		WifiDog_IpDown(ptr);
	    }
#endif
#if (WIFIDOG_AUTH == FYES)
	    if(getWebauthType() == 2) {
		addr.s_addr=confPtr->ip;	
		if((ptr = inet_ntoa(addr))!=NULL) {
		    fprintf(stderr,"%s-%d: down ip: %s\n",__func__,__LINE__,ptr);
		WifiDog_IpDown(ptr);
		}
	    } else {
#if (WEBAUTH_ACCOUNT == FYES)
	    /*ip下线处理函数*/
	    uttNfWebAuthCancel_byIp(confPtr->ip);
#endif
	    }
#else
#if (WEBAUTH_ACCOUNT == FYES)
	    /*ip下线处理函数*/
	    uttNfWebAuthCancel_byIp(confPtr->ip);
#endif
#endif
	    break;
	default:
	    break;
    }
    return;
}
#endif
#endif
/**
 * utt netlink 任务入口函数
 */
static void uttNfNlTask(void)
{
    struct sockaddr_nl nl_addr;
    struct iovec iov;
    struct msghdr msg;
    char buf[UTT_MAX_NLMSGSIZE];
    struct nlmsghdr *nlh = NULL;
    UttNlNfMsgHead *msgHead = NULL;

    pthread_detach(pthread_self());/*线程退出后立即释放资源.若指定pthread_join可不用此举*/
    memset(&msg, 0, sizeof(msg));

    nlh = (struct nlmsghdr*) buf;
    iov.iov_base = (void *)nlh;
    iov.iov_len = UTT_MAX_NLMSGSIZE;
    msg.msg_name = (void *)&nl_addr;
    msg.msg_namelen = sizeof(nl_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

#if 1
    nl_sock_fd = uttNfNlSkBind(pthread_self() << 16 | getpid(), 0);/*单播到本进程*/
#else
    nl_sock_fd = uttNfNlSkBind( getpid(), 0);/*单播到本进程*/
#endif
    if(nl_sock_fd <  0)
    {
	pthread_exit(NULL);/*bind异常线程直接结束*/
    }
    uttNfNlInit(nl_sock_fd);/*初始化。发消息*/
 
    while(1)
    {
	memset(buf, 0 ,sizeof(buf));

	if(recvmsg(nl_sock_fd, &msg, 0) > 0)/*阻塞方式接受消息*/
	{
	    msgHead = (UttNlNfMsgHead*)NLMSG_DATA(nlh);
	    DEBUG_UTT_NFNL("%s %d : Message received in user %d, type:%d\n",__func__, __LINE__, getpid(), msgHead->nlType) ;
	    /*处理收到的消息*/
	    switch(msgHead->nlType)
	    {
#if (GROUP_BASE == FYES)
		case UTT_NLNF_POLICY:/*行为管理规则设置*/
		    uttNlConfReply(NLMSG_DATA(nlh));/*处理内核消息*/
		    break;
#endif
#if ((WEBAUTH_ACCOUNT == FYES) || (WIFIDOG_AUTH == FYES) || (WIFIDOG_MAKE_MONEY == FYES)) 
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
		case UTT_NLNF_HASH:
		    uttNlIpDownReply(NLMSG_DATA(nlh));/*处理内核消息ip下线处理*/
		    break;
#endif
#endif
#ifdef CONFIG_UTT_CLOUD_CONFIG
		case UTT_NLNF_ONLIN_REMIND:
		  uttNlOnlineProcess(NLMSG_DATA(nlh));/*上线提醒的处理*/  		    
		    break;
#endif
		default:
		    break;
	    }
	    /*end*/
	}
    }

    return;
}

/**
 * 创建一个线程 来 接受来自内核的netlink数据
 */
void uttNfNlTaskInit(void)
{
    if( pthread_create(&nl_task_thread, NULL, (void*)uttNfNlTask, NULL) != 0) 
    {
	printf("Create threat for utt netfilter netlink error!\n");
    }
    return;
}
#endif
