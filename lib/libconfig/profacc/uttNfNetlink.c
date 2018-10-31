#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/autoconf.h>
#include <linux/socket.h>
#include <errno.h>
#include "uttMachine.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "uttNfNetlink.h"

#ifdef CONFIG_UTT_NETFILTER
#include <include/linux/netfilter_ipv4/utt_netfilter.h>
#include <include/linux/netlink.h>
/**
 * netlink socket bind
 * pid : 源代pid
 * grp : 非零则加入相关的组（bit位表示组）
 *
 * return: -1 bind erro 
 *         not -1 , success binded sock id
 */
int uttNfNlSkBind(Uint32 pid, Uint32 grp)
{
    struct sockaddr_nl src_addr;
    int sock_fd;
    /*Create a socket*/
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_UTT_NETFILTER);
    if(sock_fd == -1){
	printf("error getting socket: %s", strerror(errno));
    }
    else
    {
	/* To prepare binding*/
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = pid; /*unicast*/
	src_addr.nl_groups = grp; /*broadcast*/
   int status = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
	       if(status==-1)
	{
	    printf("bind failed: %s", strerror(errno));
	    sock_fd = -1;
	    close(sock_fd);
	}
    }
    return sock_fd;
}

/**
 * netlink send message
 * data: 发送数据的首部指针
 * dataLen：发送数据的长度
 * dpid : 发送对象  0 则发送给内核
 * dgrp : 发送对象组
 * return : 0 send ok , -1 send erro
 */

int uttNfNlSend(int sock_fd, char *data,Uint32 dataLen, Uint32 dpid, Uint32 dgrp)
{
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char buf[UTT_MAX_NLMSGSIZE];
    struct nlmsghdr *nlh = NULL;
    int ret = 0;

    nlh = (struct nlmsghdr*) buf;
    if(NLMSG_SPACE(dataLen) <= UTT_MAX_NLMSGSIZE)
    {/*发送缓冲区不能溢出*/
	memset(buf, 0, sizeof(buf));
	nlh->nlmsg_len = NLMSG_SPACE(dataLen); /*发送数据包含nlmsg头的长度*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	nlh->nlmsg_flags |= NLM_F_REQUEST;
	nlh->nlmsg_type = 0x26;
#else
	nlh->nlmsg_flags = 0;
#endif
#if 1
	nlh->nlmsg_pid = pthread_self()<<16|getpid();/*自我pid*/
#else
	nlh->nlmsg_pid = getpid();/*自我pid*/
#endif
	memcpy(NLMSG_DATA(nlh), data, dataLen);/*把数据放入缓冲区*/

	memset(&dest_addr,0,sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = dpid;/*目的pid*/
	dest_addr.nl_groups = dgrp;/*目的组*/


	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(dataLen);

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

#if 0
	printf("__________________________send________________message spid is  %d \n", nlh->nlmsg_pid);
#endif

	if(sendmsg(sock_fd,&msg,0) == -1)
	{
	    printf("get error sendmsg = %s\n",strerror(errno));
	}
    }

    return ret;

}

/**
 * netlink 删除 内核hash表对应的ip表项
 */
void uttNfNlHashDelIpStr(int sock_fd, char * ipstr)
{
    struct in_addr addr;
    if(inet_aton(ipstr, &addr))
    {/*转化成功*/
	uttNfNlHashDelIp(sock_fd, addr.s_addr);
    }
    return;

}
/**
 * netlink 删除 内核hash表对应的ip表项
 */
void uttNfNlHashDelIp(int sock_fd, unsigned long ipaddr)
{
    UttNlNfHashConf conf;
    conf.msgHead.nlType = UTT_NLNF_HASH;
    conf.confType = UTT_NFCONF_DEL;
    conf.ip =  ipaddr;
    uttNfNlSend(sock_fd, (char *)&conf, sizeof(conf), 0, 0);/*发送信息给内核*/
    return;
}

#if (WEBAUTH_ACCOUNT == FYES)
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/**
 * netlink web认证上线时告知内核ip对应的hash表项的操作
 */
void uttNfNlHashEditTrigger(int sock_fd, unsigned long ipaddr)
{
    UttNlNfHashConf conf;
    conf.msgHead.nlType = UTT_NLNF_HASH;
    conf.confType = UTT_NFCONF_EDIT;
    conf.ip =  ipaddr;
    conf.trigger = 1U<< UTT_NF_HASH_WEB_AUTH_DES_NOTIFY;
    uttNfNlSend(sock_fd, (char *)&conf, sizeof(conf), 0, 0);/*发送信息给内核*/
    return;
}
#endif
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
int uttNfNlProfChangeSkBind(Uint32 pid, Uint32 grp)
{
    struct sockaddr_nl src_addr;
    int sock_fd;
    int status;
    /*Create a socket*/
    sock_fd = socket(PF_NETLINK,SOCK_DGRAM, NETLINK_UTT_PROF_CHANGE);
    if(sock_fd == -1){
	printf("error getting socket: %s", strerror(errno));
    }
    else
    {
	/* To prepare binding*/
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = pid; /*unicast*/
	src_addr.nl_groups = grp; /*broadcast*/
	bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
	if(status==-1)
	{
	    printf("bind failed: %s", strerror(errno));
	    sock_fd = -1;
	    close(sock_fd);
	}
    }
    return sock_fd;
}
#endif
#endif
