#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/autoconf.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/wifi_netlink.h>
#include <linux/socket.h>
#include <pthread.h>
#include <errno.h>
#include "urcp.h"
#include "urcp_ap.h"
#include "urcp_debug.h"

static pthread_t nl_wifi_thread;
static int nl_sock_fd;

void ap_sta_assoc(unsigned char *sta_mac, unsigned int sta_num, unsigned int flag)
{
	db_info(URCP_MODULE_ID_TRAP_STA, "++++++++++++++++assoc sta mac: %2x:%2x:%2x:%2x:%2x:%2x, sta_num = %u, is5G=%u\n", sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5], sta_num, flag);

	send_ap_clients(sta_num, sta_mac, AP_STA_CONNECT, flag);

	return;
}

void ap_sta_disassoc(unsigned char *sta_mac, unsigned int sta_num, unsigned int flag)
{
	db_info(URCP_MODULE_ID_TRAP_STA,"-----------------disassoc sta mac: %2x:%2x:%2x:%2x:%2x:%2x, sta_num = %u, is5G=%u\n", sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5], sta_num, flag);

	send_ap_clients(sta_num, sta_mac, AP_STA_DISCONNECT, flag);

	return;
}

void ap_sta_upper_limit(unsigned char *sta_mac, unsigned int sta_num, unsigned int flag)
{
	db_info(URCP_MODULE_ID_TRAP_STA, "****************AP sta upper limit: %2x:%2x:%2x:%2x:%2x:%2x, sta_num = %u, is5G=%u\n", sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5], sta_num, flag);

	send_ap_upper_limit_msg(sta_num, flag);

	return;
}

int uttWifiNlSkBind(Uint32 pid, Uint32 grp)
{
    struct sockaddr_nl src_addr;
    int sock_fd;
    /*Create a socket*/
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_WIFI);
    if(sock_fd == -1) {
		printf("error getting socket: %s", strerror(errno));
    } else {
		/* To prepare binding*/
		memset(&src_addr, 0, sizeof(src_addr));
		src_addr.nl_family = AF_NETLINK;
		src_addr.nl_pid = pid; /*unicast*/
		src_addr.nl_groups = grp; /*broadcast*/
		int status = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

	    if(status==-1) {
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
int uttWifiNlSend(int sock_fd, char *data,Uint32 dataLen, Uint32 dpid, Uint32 dgrp)
{
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char buf[UTT_MAX_NLMSGSIZE];
    struct nlmsghdr *nlh = NULL;
    int ret = 0;

    nlh = (struct nlmsghdr*) buf;

    if(NLMSG_SPACE(dataLen) <= UTT_MAX_NLMSGSIZE) {/*发送缓冲区不能溢出*/
		memset(buf, 0, sizeof(buf));
		nlh->nlmsg_len = NLMSG_SPACE(dataLen); /*发送数据包含nlmsg头的长度*/
#ifdef GLIBC_TOOLCHAIN
		nlh->nlmsg_pid = pthread_self()<<16|getpid();
#else
		nlh->nlmsg_pid = getpid();
#endif
		nlh->nlmsg_flags = 0;
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

		if(sendmsg(sock_fd,&msg,0) == -1) {
			printf("get error sendmsg = %s\n",strerror(errno));
		}
    }

    return ret;
}

void uttWifiNlInit(int sock_fd)
{
	UttNlWifiStr conf;
    conf.msgHead.nlType = UTT_NLWIFI_STA_INFO;
    conf.confType = UTT_NLWIFI_STA_INIT;
	DBCK();
    uttWifiNlSend(sock_fd, (char *)&conf, sizeof(conf), 0, 0);/*发送给内核自己的pid*/

    return;
}

static void uttWifiInfoReply(UttNlWifiStr *confPtr)
{
	switch (confPtr->confType) {
		case UTT_NLWIFI_STA_ASSOC:
			ap_sta_assoc(confPtr->sta_mac, confPtr->sta_num, confPtr->flag);
			break;

		case UTT_NLWIFI_STA_DISASSOC:
			ap_sta_disassoc(confPtr->sta_mac, confPtr->sta_num, confPtr->flag);
			break;

		case UTT_NLWIFI_STA_UPPER_LIMIT:
			if(confPtr->flag==0) {
			    ap_sta_upper_limit(confPtr->sta_mac, confPtr->sta_num, confPtr->flag);
			}
			break;

		default:
			break;
	}

	return ;
}

/**
 * urcp netlink 任务入口函数
 */
static void uttWifiNlTask(void)
{
    struct sockaddr_nl nl_addr;
    struct iovec iov;
    struct msghdr msg;
    char buf[UTT_MAX_NLMSGSIZE];
    struct nlmsghdr *nlh = NULL;
    UttNlWifiMsgHead *msgHead = NULL;

    pthread_detach(pthread_self());
    memset(&msg, 0, sizeof(msg));

    nlh = (struct nlmsghdr*) buf;
    iov.iov_base = (void *)nlh;
    iov.iov_len = UTT_MAX_NLMSGSIZE;
    msg.msg_name = (void *)&nl_addr;
    msg.msg_namelen = sizeof(nl_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

#ifdef GLIBC_TOOLCHAIN
    nl_sock_fd = uttWifiNlSkBind(pthread_self() << 16 | getpid(), 0);/*单播到本进程*/
#else
    nl_sock_fd = uttWifiNlSkBind( getpid(), 0);/*单播到本进程*/
#endif
    if(nl_sock_fd <  0) {
		pthread_exit(NULL);/*bind异常线程直接结束*/
    }
	uttWifiNlInit(nl_sock_fd);

    while(1) {
		memset(buf, 0 ,sizeof(buf));

		if(recvmsg(nl_sock_fd, &msg, 0) > 0) {/*阻塞方式接受消息*/
		    msgHead = (UttNlWifiMsgHead *)NLMSG_DATA(nlh);
		    /*处理收到的消息*/
		    switch(msgHead->nlType) {
			case UTT_NLWIFI_STA_INFO:/*行为管理规则设置*/
			    uttWifiInfoReply(NLMSG_DATA(nlh));/*处理内核消息*/
			    break;

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
int uttWifiNlTaskInit(void)
{
	int ret = 0;

    ret = pthread_create(&nl_wifi_thread, NULL, (void*)uttWifiNlTask, NULL);

	if (ret != 0) {
		printf("Create threat for wifi netlink error!\n");
    }

    return ret;
}
