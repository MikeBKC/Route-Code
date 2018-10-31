#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/autoconf.h>
#include <linux/ip.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#define DEBUG_UTT_POLICY 0
#if DEBUG_UTT_POLICY
#define UTT_POLICY_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_POLICY_DEBUG(fmt, argx...)
#endif


static u_int uttNfUserPid = 0;
static u_int uttNfWifidogPid = 0;
static int uttNlPolicyAddRequest(u_long userIp);
static bool uttNfPolicyListMatch(UttNfCurTime *curTime, UttNfPolicyList *list, u_int appFlag);
static void uttNfPolicyDelAll(void);


extern bool uttNfPolicyHashMatch(struct sk_buff* skb);
extern void uttNlPolicyConf(struct sk_buff *skb);
#if DEBUG_UTT_POLICY
static void print_members(unsigned char *member, int  size)
{
    int i;

    for(i=0;i<size;i++)
    {
        printk("%02x", member[i]);
    }
    return;
}
static void uttPolicyListPrint(UttNfPolicyList *list)
{
    while(list != NULL) 
    {
	printk("\n");
	print_members(list->value.nfApps.appFlags, sizeof(UttNfApps));
	printk(" day:%u", list->value.nfTime.wday);
	printk(" timestart:%u", list->value.nfTime.timestart);
	printk(" timestop:%u", list->value.nfTime.timestop);
	printk("\n");

	list = list->next;/*ָ����һ��*/
    }
    return;
}
static void uttHashBufPolicyPrint(UttNfHashBuf *buf)
{
    printk("ip %pI4:status %u \n", &(buf->ip), buf->status);
    uttPolicyListPrint(buf->conf.policyConf.policyHead);
    printk("\n");
}
static void uttHashPolicyPrint(void)
{
    printk("\n_______________________________%s start\n\n", __func__);
    uttNfHashTraverse(uttHashBufPolicyPrint);
    printk("\n_______________________________%s end\n\n", __func__);

    return;
}

#if 0
static struct timer_list nfDebugTimer;
void UttPolicyDebugDelay(u_long param)
{
    if(param == 0u)
    {
	uttNfPolicyHashMatch(0x00001000u, 1);
	uttNfPolicyHashMatch(0x10001000u, 1);
	uttNfPolicyHashMatch(0x20001000u, 1);
	uttHashPolicyPrint();
	uttNfPolicyHashMatch(0x20001000u, 1);
	nfDebugTimer.expires = jiffies + 30*HZ;   /* ��ʱ����ʱʱ�Ľ�����*/
	nfDebugTimer.data     = 1;                /* ����ʱ���������������0*/
	nfDebugTimer.function = UttPolicyDebugDelay;      /* ��ʱ����ʱʱ���õĺ���*/
	add_timer(&nfDebugTimer);/*���ʱ��*/

    }
    else
    {
	uttNfPolicyHashMatch(0x00001000u, 1);
	uttNfPolicyHashMatch(0x00001000u, 2);
	uttNfPolicyHashMatch(0x00001000u, 9);
	uttHashPolicyPrint();
	nfDebugTimer.expires = jiffies + 10*HZ;   /* ��ʱ����ʱʱ�Ľ�����*/
	nfDebugTimer.data     = 1;                /* ����ʱ���������������0*/
	nfDebugTimer.function = UttPolicyDebugDelay;      /* ��ʱ����ʱʱ���õĺ���*/
	add_timer(&nfDebugTimer);/*���ʱ��*/


    }

    return;
}
void UttPolicyDebug(void)
{

    init_timer(&nfDebugTimer);
    nfDebugTimer.expires = jiffies + 30*HZ;   /* ��ʱ����ʱʱ�Ľ�����*/
    nfDebugTimer.data     = 0;                /* ����ʱ���������������0*/
    nfDebugTimer.function = UttPolicyDebugDelay;      /* ��ʱ����ʱʱ���õĺ���*/
    add_timer(&nfDebugTimer);/*���ʱ��*/
    return;
}
#endif
#endif

/**
 * ��hash���н��в���ƥ��
 * ip :��Ӧ�� skb ip 
 * appFlag  ����ʶ��ʱ��ӵı�ǩ
 *
 * return ��true �������и�ip��Ӧ�ı�ǩ
 *          false �������޸�ip��Ӧ�ı�ǩ
 */
bool uttNfPolicyHashMatch(struct sk_buff *skb)
{
    UttNfHashConf* hashConf = NULL;
    struct iphdr *ipH = NULL;
    UttNfHashPolicy *hashConfPolicy = NULL;
    UttNfCurTime pakTime;
    bool ret = false;

    if(((skb->utt_app.secFlag) > 0)
	    && ((skb->utt_app.secFlag) <= UTT_MAX_KPDB_APPFLAGS))
    {
	if(isSkbComeFromLan(skb))
	{/*����lan�ڵİ��Ž���ƥ��*/
	    UTT_POLICY_DEBUG("%s %d %u\n", __func__, __LINE__, skb->utt_app.secFlag);
	    ipH = ip_hdr(skb);
	    /*����hashConf�Ķ�ȡҲ���Էŵ����ú����С�����ÿ�ζ�hash*/
	    hashConf = uttNfHashConfGet(ipH->saddr, 1);/*��hash���ȡ����*/
	    if(hashConf != NULL)
	    {
		hashConfPolicy = &(hashConf->policyConf);
		if(hashConfPolicy->policyHead != NULL)/*�����������*/
		{/*ע����ֻ�й�������ȫ��������ϲ��ܽ���ƥ�䡣�����ڴ��ж�*/
		    /*�ӹ��������н���ƥ��*/
		    UTT_POLICY_DEBUG("%s %d %pI4 match from list\n", __func__, __LINE__, &(ipH->saddr));
		    uttNfPolicyGetTime(skb, &pakTime);
		    ret = uttNfPolicyListMatch(&pakTime, hashConfPolicy->policyHead, skb->utt_app.secFlag);
		}
		else
		{
		    if(hashConfPolicy->buildStatus == UTT_NFNL_DONE_NONE)
		    {/*δ���й�����������*/
			if(uttNlPolicyAddRequest(ipH->saddr) >= 0)/*���û���������Ϣ����������*/
			{/*���ͳɹ�*/
			    /*��ip��Ӧ�����Ϊ�����С����ڹ�����ǰ�������ط�����*/
			    hashConfPolicy->buildStatus |= UTT_NFNL_RCV_ING;
			}
#if DEBUG_UTT_POLICY
			else
			{
			    UTT_POLICY_DEBUG("%s %d %pI4 request erro\n", __func__, __LINE__, &(ipH->saddr));
			}
#endif
		    }
#if DEBUG_UTT_POLICY
		    else
		    {
			UTT_POLICY_DEBUG("%s %d %pI4 in buiding\n", __func__, __LINE__, &(ipH->saddr));
		    }
#endif
		}
	    }
	}
    }
    return ret;

}

/**
 * �Ӳ��������н��в���ƥ��
 */
static bool uttNfPolicyListMatch(UttNfCurTime *curTime, UttNfPolicyList *list, u_int appFlag)
{
    register u_int flagsIndex;/*�Ĵ��������ӿ������ٶ�*/
    register u_char flagsBit;
    bool ret = false;

#if 0
    appFlag = appFlag - 1u;/*���Լ��ϴ��±�0��ʼ������־��1��ʼ*/
#endif
    flagsIndex = appFlag >> 3u ;/*������������ ��appFlag /8 */
    flagsBit = 1u << (appFlag  & 0x7u);/*����Ԫ��ֵ ��appFlag%8*/

    UTT_POLICY_DEBUG("%s %d  flagsIndex:%u flagsBit:%u flag:%u\n", __func__, __LINE__, flagsIndex, flagsBit, appFlag);
    while(list != NULL)
    {
	if ((list->value.nfTime.wday&curTime->week))
	{/*������*/
	    if (uttTimeIn(curTime->time , list->value.nfTime.timestart, list->value.nfTime.timestop))
	    {/*��ǰʱ����*/
		if((u_char)list->value.nfApps.appFlags[flagsIndex] & (u_char)flagsBit)
		{
		    UTT_POLICY_DEBUG("%s %d  %u match ok\n", __func__, __LINE__, appFlag);
		    ret = true;/*�ҵ�һ��ƥ��Ԫ��*/
		    break;
		}
	    }
	}

	list = list->next;
    }
    return ret;
}

#ifdef CONFIG_UTT_CLOUD_CONFIG
/**
 *������Ϣ���û�̬
 *��֪�������ѵ�mac������
 * xrg
 ***/
extern int uttNlOnRemindToUser(unsigned char mac[],unsigned int noticeBanMask)
{
    int err = -1;
    UttNlNfOnRemind conf;
    conf.msgHead.nlType = UTT_NLNF_ONLIN_REMIND;
    conf.noticeBanMask = noticeBanMask;
    memcpy(conf.newMac,mac,ETH_ALEN);
    if(uttNfUserPid > 0)
    {
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfUserPid, 0);
    }
    return err;
}
#endif
/**
 * ����Ϣ���û�̬
 * ��Ӧ�ó������Ӧ userIp�� �������÷��͹���
 */
extern int uttNlPolicyAddRequest(u_long userIp)
{
    int err = -1;
    UttNlNfPolicy conf;
    conf.msgHead.nlType = UTT_NLNF_POLICY;
    conf.confType = UTT_NFCONF_ADD;
    conf.ip = userIp;/*�û�ip*/
    if(uttNfUserPid > 0)
    {
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfUserPid, 0);
    }
    return err;
}
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/**
 * ����Ϣ���û�̬
 * ��֪Ӧ�ò�ip����
 */
extern int uttNlWebAuthAddRequest(u_long userIp)
{
    int err = -1;
    UttNlNfHashConf conf;
    conf.msgHead.nlType = UTT_NLNF_HASH;
    conf.confType = UTT_NFCONF_DEL;
    conf.ip = userIp;/*�û�ip*/
    if(uttNfUserPid > 0)
    {
	UTT_POLICY_DEBUG("%s %d:-----request-----\n", __func__, __LINE__);
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfUserPid, 0);
    }
    return err;
}
#endif
/**
 * �жϲ����������Ƿ��Ѵ��� ָ����val
 * return : true ����
 *          false ������
 */
static bool isUttNfPolicyContain( UttNfPolicyList *list,UttNfPolicyVal *policyVal)
{
   while(list != NULL)
    {
	if (memcmp(&(list->value), policyVal, sizeof(UttNfPolicyVal)) == 0)
	{/*����value��ȫ��ͬ�Ľڵ�*/
	    UTT_POLICY_DEBUG("%s %d\n", __func__, __LINE__);
	    return true;
	}
	list = list->next;
    }
    return false;
}
/**
 * ����һ������
 */
static void uttNfPolicyAdd(u_long ip, UttNfPolicyVal *policyVal)
{
    UttNfHashConf* hashConf = NULL;
    UttNfPolicyList *newPolicy = NULL;
    hashConf = uttNfHashConfGet(ip, 0);/*��hash���ȡ����*/
    if(hashConf != NULL)
    {
	if(!isUttNfPolicyContain(hashConf->policyConf.policyHead, policyVal))
	{/*�����ڴ˲��ԡ����½�
	  *�������Ӵ��жϡ����� 
	  * 1 �����û�������������ͬ�Ĳ��ԡ����Ӱ�첻��̫��
	  * 2 �û�������ظ�������ĳЩ���ԡ��� ���� �ź������Ʋ��á����µ��ط������Ӱ��Ƚϴ�
	  */
	    newPolicy = (UttNfPolicyList*)kmalloc(sizeof(UttNfPolicyList), GFP_KERNEL);
	    if(newPolicy != NULL)
	    {/*�ڴ����ɹ�*/
		memcpy(&(newPolicy->value), policyVal, sizeof(UttNfPolicyVal));
		/*��������*/
		newPolicy->next = hashConf->policyConf.policyHead;
		hashConf->policyConf.policyHead = newPolicy;
	    }
	}

    }
#if DEBUG_UTT_POLICY
    uttHashPolicyPrint();
#endif
    return;
}
/**
 * ɾ��һ����������
 */
void uttPolicyListDel(UttNfPolicyList *list)
{
    UttNfPolicyList *bakList = NULL;

    while(list != NULL) 
    {
	bakList = list;/*���ݵ�ǰ�ڵ��ָ��*/
	list = list->next;/*ָ����һ����ɾ�ڵ�*/
	kfree(bakList);/*�ͷŵ�ǰ�ڵ�ռ�*/
    }
    return;
}

/**
 * ɾ��һ��ip hash ��Ӧ�Ĳ���
 */
static void uttHashBufDelPolicy(UttNfHashBuf *buf)
{
    UttNfHashPolicy *hashPolicy = &(buf->conf.policyConf);/*ָ��������ýṹ*/
    uttPolicyListDel(hashPolicy->policyHead);/*ɾ��������������*/
    memset(hashPolicy, 0, sizeof(UttNfHashPolicy));/*����ڴ�*/
    return;

}
/**
 * ɾ�����й���
 */
static void uttNfPolicyDelAll(void)
{
    UTT_POLICY_DEBUG("%s %d\n", __func__, __LINE__);
    /*����hash����ɾ��ÿ��ip��Ӧ�Ĺ���*/
    uttNfHashTraverse(uttHashBufDelPolicy);
#if DEBUG_UTT_POLICY
    uttHashPolicyPrint();
#endif
    return;
}
/*��Ϊ����������ú���*/
void uttNlPolicyConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfPolicy  *nlData;
    u_int dpid = 0;
    nlh = nlmsg_hdr(skb);
    dpid = nlh->nlmsg_pid;
    nlData = (UttNlNfPolicy*)NLMSG_DATA(nlh);
    switch(nlData->confType)
    {
	case UTT_NFCONF_INIT:
	    UTT_POLICY_DEBUG("\n=========================%s %d init dpid %d\n", __func__, __LINE__, dpid) ;
	    uttNfUserPid = dpid;
	    break;
	case UTT_NFCONF_ADD:
	    uttNfPolicyAdd(nlData->ip, &(nlData->policyVal));
	    break;
	case UTT_NFCONF_DEL_ALL:
	    uttNfPolicyDelAll();
	    break;

    }
    return;
}

#ifdef CONFIG_WEBAUTH_MAC_VALIDATE
void uttNlWifidogSend(int type, unsigned int ip, unsigned char *mac)
{
    int err = -1;
    struct timex  txc;
    unsigned int now;
    static unsigned int count = 0;
    static unsigned int last_time = 0;

    if (count > 1000)
    {
	do_gettimeofday(&(txc.time));
	now = txc.time.tv_sec;
	if (last_time == 0 || now - last_time > 60)
	{
	    last_time = now;
	    count = 0;
	}
	else
	{
	    return;
	}
    }

    UttNlNfWifidogConf conf;
    conf.msgHead.nlType = UTT_NLNF_WIFIDOG;
    conf.confType = type;
    //memcpy(&conf.ip, ip, 4);
    conf.ip = ip;
    memset(conf.mac, 0, 6);
    if (mac)
	memcpy(conf.mac, mac, 6);
    if(uttNfWifidogPid > 0)
    {
	err = uttNlSend((char *)&conf, sizeof(conf), uttNfWifidogPid, 0);
	count++;
    }
    return err;
}

void uttNlWifidogConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfPolicy  *nlData;
    u_int dpid = 0;
    nlh = nlmsg_hdr(skb);
    dpid = nlh->nlmsg_pid;
    nlData = (UttNlNfPolicy*)NLMSG_DATA(nlh);
    printk("%s %d:  \n", __func__, __LINE__);
    switch(nlData->confType)
    {
	case UTT_NF_WIFIDOG_INIT:
	    printk("%s %d:  \n", __func__, __LINE__);
	    uttNfWifidogPid = dpid;
	    break;

    }
    return;
}
#endif

