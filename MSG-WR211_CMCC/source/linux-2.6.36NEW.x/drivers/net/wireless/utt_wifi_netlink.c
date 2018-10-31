#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/wifi_netlink.h>

u_int uttWifiUrcpPid = 0;
static struct sock *nl_sk = NULL;
static void uttNlRcv(struct sk_buff *__skb);
static int uttNlWifiInit(void);
static void uttNlWifiExit(void);


/**
 * netlink ��������
 *
 * data:ָ��Ҫ���͵�����
 * dataLen:�������ݵĳ���
 * dpid:Ŀ�Ľ���id
 * dgrp:Ŀ����.��0���鲥�����򵥲�
 *
 * return: < 0 ʧ��
 */
int uttWifiNlSend(char *data, u_int dataLen, u_int dpid, u_int dgrp)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int nlLen = NLMSG_SPACE(dataLen);/*����netlink��Ϣͷ�ĳ���*/
    int err = 0;
#if 0
    if(!data || !nl_sk)
    {
	return ;
    }
#endif

    skb_1 = alloc_skb(nlLen,GFP_KERNEL);
    if(skb_1)
    {/*����ɹ�*/
	nlh = nlmsg_put(skb_1,0,0,0,dataLen,0);

	NETLINK_CB(skb_1).pid = 0;/*Դ��pid*/
	NETLINK_CB(skb_1).dst_group = dgrp;/*Ŀ����*/

	memcpy(NLMSG_DATA(nlh), data, dataLen);

	if(dgrp != 0)
	{/*�鲥*/
	    netlink_broadcast(nl_sk, skb_1, 0, dgrp, GFP_KERNEL);
	}
	else
	{/*����*/
	   err =  netlink_unicast(nl_sk,skb_1,dpid,MSG_DONTWAIT);
	}
    }
    else
    {
	err = -1;
    }
    return err; 

}

void uttNlWifiConf(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	UttNlWifiStr    *nlData;
	u_int dpid = 0;
	nlh = (struct nlmsghdr *)skb->data;
	dpid = nlh->nlmsg_pid;
	nlData = (UttNlWifiStr *)NLMSG_DATA(nlh);

	switch (nlData->confType) {
		case UTT_NLWIFI_STA_INIT:
//			printk("%d in %s of %s, UTT_NLWIFI_STA_INIT\n", __LINE__, __func__, __FILE__);
			uttWifiUrcpPid = dpid;
			break;

		default:
			break;
	}

	return;
}

/*
 * netlink ���ݽ��պ���
 * */
static void uttNlRcv(struct sk_buff *__skb)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    UttNlWifiMsgHead *msgHead = NULL;

    skb = skb_get (__skb);
    if(skb->len >= NLMSG_SPACE(0))
    {
		nlh = nlmsg_hdr(skb);
		msgHead = (UttNlWifiMsgHead *)NLMSG_DATA(nlh);
//		printk("%d in %s of %s, Message receive\n", __LINE__, __func__, __FILE__);
		switch(msgHead->nlType)
		{
			case UTT_NLWIFI_STA_INFO:
				uttNlWifiConf(skb);
				break;

		    default:
				break;
		}
		kfree_skb(skb);
    }

	return;
}


/**
 * ��ʼ�����
 */
static int uttNlWifiInit(void)
{
    nl_sk = netlink_kernel_create(&init_net, NETLINK_WIFI, 0,
	    uttNlRcv, NULL, THIS_MODULE);

    if(!nl_sk){
		printk("wifi create netlink socket error.\n");
		return 1;
    }


    return 0;
}

void uttNlWifiExit(void)
{
    if(nl_sk != NULL){
	sock_release(nl_sk->sk_socket);
    }

	printk("wifi netlink module exit.\n");

	return;
}

module_init(uttNlWifiInit);
module_exit(uttNlWifiExit);

EXPORT_SYMBOL(uttWifiNlSend);
EXPORT_SYMBOL(uttWifiUrcpPid);

MODULE_AUTHOR("UTT");
MODULE_LICENSE("GPL");

