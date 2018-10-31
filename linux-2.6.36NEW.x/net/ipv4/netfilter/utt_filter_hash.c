#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/autoconf.h>
#include <linux/ip.h>
#include <net/netlink.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <linux/netfilter_ipv4/utt_module_init.h>
#endif

#define DEBUG_UTT_HASH 0
#if DEBUG_UTT_HASH
#define UTT_HASH_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_HASH_DEBUG(fmt, argx...)
#endif

#define ip_hash_key(ipaddr) ((ipaddr & 0xffu) ^ ((ipaddr >> 8u) & 0xffu))
#define UTT_HASH_POLL_INTERVAL 300*HZ /*hash��ÿ��300s�Զ���ʱ������*/

unsigned int staleTimeUser = 0;

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
extern void uttPolicyListDel(UttNfHashPolicy *list);
#endif
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
unsigned int triggerUser=0U;
extern int uttNlWebAuthAddRequest(u_long userIp);
#endif

/*ֱ��hash��ͷ*/
UttNfHashBuf *uttNfIpHashTable[MAX_UTTNF_HASH_TABLE];
static struct timer_list nfHashTimer;
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
#define UTT_HASH_WIFI_POLL_INTERVAL 1*HZ /*Զ����֤hash��ÿ1s�Զ���ʱ������*/
static struct timer_list nfHashWifiTimer;
#endif

static void nfHashTimeOut(u_long param);
static void nfHashPollInit(void);
static void uttNfHashConfDel(UttNfHashConf *hashConf);

void uttNfHashInit(void);
void uttNlHashConf(struct sk_buff *skb);

#if DEBUG_UTT_HASH
static void uttHashBufPrint(UttNfHashBuf *buf)
{
    static int i = 0;
    printk("%08x:%u ", buf->ip, buf->status);
    i++;
    if(i%8 == 0)
    {
	printk("\n");
    }
}
static void uttHashPrint(void)
{
    printk("\n_______________________________%s start\n\n", __func__);
    uttNfHashTraverse(uttHashBufPrint);
    printk("\n_______________________________%s end\n\n", __func__);

    return;
}
void UttHashDebug(void)
{
    int num = 0;
    uttNfHashBufGet(0x00001000u,&num);
    uttNfHashBufGet(0x10001000u,&num);
    uttNfHashBufGet(0x20001000u,&num);
    uttNfHashBufGet(0x30001000u,&num);
    uttNfHashBufGet(0x40001000u,&num);
    uttNfHashBufGet(0x00001500u,&num);

#if 0


    uttHashPrint();
    uttNfHashBufDel(0x00001000u);

    uttHashPrint();

    uttNfHashBufDel(0x30001000u);
    uttNfHashBufGet(0x00001000u,1);
    uttHashPrint();
    uttNfHashBufDel(0x40001000u);

    uttHashPrint();
    uttNfHashBufDel(0x00001000u);
    uttNfHashBufDel(0x10001000u);
    uttNfHashBufDel(0x20001000u);
    uttNfHashBufDel(0x30001000u);
    uttNfHashBufDel(0x40001000u);

    uttNfHashBufDel(0x00001500u);
#endif

   
    uttHashPrint();
    return;
}

#endif

#if 0
#define  TIMER_CALLBACK_TIMES 2
extern void updateStatsBehavior(UttStatsPerIp *stats, int clear);
#endif
/**
 * ��ʱ�������ߵ�ip��ַ����
 */
static void nfHashTimeOut(u_long param)
{
    int i = 0;
    UttNfHashBuf **hashBufPP = NULL;/*����ָ��*/
    UttNfHashBuf *hashBufP = NULL;
#if 0 /* �����û��ռ���Ϣ����ͳ����Ϣ, �˴�������Ҫ */
    /* ÿ����2��timeout,��Ҫ����һ���û���Ϊͳ�� */
    static int flag = 0;
    flag++;
#endif

#if DEBUG_UTT_HASH
    UTT_HASH_DEBUG("%s %d:timeout hash start\n", __func__, __LINE__);
    uttHashPrint();
#endif
    /*��������hash��*/
    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufPP = &(uttNfIpHashTable[i]);/*��λ����ͷ*/

	/*������ͻ��*/
	while(*hashBufPP != NULL)
	{
	    if((*hashBufPP)->status == 0)
	    {/*�ѳ�ʱ��ֱ��ɾ��*/
		UTT_HASH_DEBUG("%s %08x:timeout hash %08x\n", __func__, __LINE__, (*hashBufPP)->ip);
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
		UTT_HASH_DEBUG("%s %08x:timeout hash ip %08x, trigger %d\n", __func__, __LINE__,(*hashBufPP)->ip, (*hashBufPP)->trigger);
		/* ����ʱ�Ѿ����ô�����
		 * ��ʱ�ᷢ������Ϣ��Ӧ�ò�Ҷϴ�ip��Ӧ��web��֤�˺�
		 * */
		if((((*hashBufPP)->trigger)&(1U<< UTT_NF_HASH_WEB_AUTH_DES_NOTIFY)) ||
			(((*hashBufPP)->trigger)&(1U<< UTT_NF_HASH_WIFIDOG_DES_NOTIFY)))
		{
		    UTT_HASH_DEBUG("%s %d: send request for web auth account\n", __func__, __LINE__);
		    /*֪ͨӦ�ò�*/
		    uttNlWebAuthAddRequest((*hashBufPP)->ip);
		}
#endif
		hashBufP = *hashBufPP;/*��¼��ǰ�ڵ�λ��*/
		*hashBufPP = hashBufP->next;/*ָ��ǰ�ڵ��ָ�룬ָ��ǰ�ڵ��next*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
		/**
		 *������ģ�飬���ø�ģ����������
		 */
		uttTraverseIpNodeExit(hashBufP);
#endif
		uttNfHashConfDel(&(hashBufP->conf));/*ɾ����ǰ�ڵ��Ӧ������*/
		kfree(hashBufP);/*�ͷŵ�ǰ�ڵ������*/
		/*��ɾ���ڵ���Ѿ���������һ����ͻ�ڵ㡣��continue*/
		continue;
	    }
	    else
	    {/*δ��ʱ�ġ���ʱ��־��Ϊ0�����´���ѯʱ��ʱ*/
		(*hashBufPP)->status = 0;
#if 0
		/* �û�ͳ��, δɾ���Ľڵ��ͳ�� */
		if (flag >= TIMER_CALLBACK_TIMES) {
		    updateStatsBehavior(&((*hashBufPP)->conf.userStats), 1);
		}
#endif
	    }
	    hashBufPP = &((*hashBufPP)->next);/*������һ����ͻ��*/
	}
    }
#if 0
    if (flag >= TIMER_CALLBACK_TIMES) {
	flag = 0;
    }
#endif

#if DEBUG_UTT_HASH
    UTT_HASH_DEBUG("%s %d:timeout hash end\n", __func__, __LINE__);
    uttHashPrint();
#endif
    if(staleTimeUser > UTT_HASH_POLL_INTERVAL) {
    UTT_HASH_DEBUG("%s-%d: UTT_HASH_POLL_INTERVAL =%u, staleTimeUser =%u\n",__func__,__LINE__,UTT_HASH_POLL_INTERVAL,staleTimeUser);
    mod_timer(&nfHashTimer, jiffies + staleTimeUser);/*���¼��ʱ��*/
    } else {
    UTT_HASH_DEBUG("%s-%d: UTT_HASH_POLL_INTERVAL =%u, staleTimeUser =%u\n",__func__,__LINE__,UTT_HASH_POLL_INTERVAL,staleTimeUser);
    mod_timer(&nfHashTimer, jiffies + UTT_HASH_POLL_INTERVAL);/*���¼��ʱ��*/
    }
    return;
}
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
/*
 * Զ����֤���õĳ�ʱ����
 * ȥ��Apple, Wpϵͳ��url��֤����
 * */
static void nfHashWifiTimeOut(u_long param)
{
    int i = 0;
    UttNfHashBuf **hashBufPP = NULL;/*����ָ��*/
    UttNfHashBuf *hashBufP = NULL;

#if 0
    printk("%s %d:timeout hash start\n", __func__, __LINE__);
#endif
    /*��������hash��*/
    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufPP = &(uttNfIpHashTable[i]);/*��λ����ͷ*/

	/*������ͻ��*/
	while(*hashBufPP != NULL)
	{
#if 0
	    printk("%s %d: wifi timeout hash ip %08x, count %d, second %d\n", __func__, __LINE__,(*hashBufPP)->ip, (*hashBufPP)->wifiInfo.count,(*hashBufPP)->wifiInfo.second);
#endif
	    if((*hashBufPP)->wifiInfo.count > 0U)
	    {/*�ѳ�ʱ��ֱ��ɾ��*/
		if((*hashBufPP)->wifiInfo.second >= 30U) {
		    (*hashBufPP)->wifiInfo.count = 0U;
		    (*hashBufPP)->wifiInfo.second = 0U;
		} else {
		    (*hashBufPP)->wifiInfo.second += 1U;
		}
	    }
	    hashBufPP = &((*hashBufPP)->next);/*������һ����ͻ��*/
	}
    }

#if 0
    printk("%s-%d: UTT_HASH_WIFI_POLL_INTERVAL =%u\n",__func__,__LINE__,UTT_HASH_WIFI_POLL_INTERVAL);
#endif
    mod_timer(&nfHashWifiTimer, jiffies + UTT_HASH_WIFI_POLL_INTERVAL);/*���¼��ʱ��*/
    return;
}
#endif
/**
 * ��ѯ��ʼ���������
 */
static void nfHashPollInit(void)
{
    init_timer(&nfHashTimer);
    if(staleTimeUser > UTT_HASH_POLL_INTERVAL) {
    nfHashTimer.expires = jiffies + staleTimeUser;   /* ��ʱ����ʱʱ�Ľ�����*/
    } else {
    nfHashTimer.expires = jiffies + UTT_HASH_POLL_INTERVAL;   /* ��ʱ����ʱʱ�Ľ�����*/
    }
    nfHashTimer.data     = 0;                /* ����ʱ���������������0*/
    nfHashTimer.function = nfHashTimeOut;      /* ��ʱ����ʱʱ���õĺ���*/
    add_timer(&nfHashTimer);/*���ʱ��*/
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
    init_timer(&nfHashWifiTimer);
    nfHashWifiTimer.expires = jiffies + UTT_HASH_POLL_INTERVAL;   /* ��ʱ����ʱʱ�Ľ�����*/
    nfHashWifiTimer.data     = 0;                /* ����ʱ���������������0*/
    nfHashWifiTimer.function = nfHashWifiTimeOut;      /* ��ʱ����ʱʱ���õĺ���*/
    add_timer(&nfHashWifiTimer);/*���ʱ��*/
#endif
    return;
}

/**
 * ��hash���в���ָ����ip��ַ,��Ӧ��conf
 * isCreat: 1 ���Ҳ�����Ӧ�ı������
 *          0 ���Ҳ�����Ӧ�ı��������
 *
 * return:  NULL ����ʧ�� ��isCreatΪ1������ʧ��
 *          ��NULL �ҵ����½���hash�ṹ����ָ��
 *
 */
UttNfHashConf *uttNfHashConfGet(u_long ipaddr, int isCreat)
{
    UttNfHashBuf * buf = uttNfHashBufGet(ipaddr, &isCreat);
    if(buf != NULL)
    {
	return &(buf->conf);
    }
    return NULL;
}
#ifdef CONFIG_UTT_SKB_IIFTYPE
bool isWirelessIn(struct sk_buff *skb)
{
    struct net_device *device = NULL;
    bool ret = false;
    /*��ȡ����豸*/
    device = dev_get_by_index(sock_net(skb->sk),skb->skb_iif);
    if(device != NULL)
    {
	UTT_HASH_DEBUG("%s %d device %s,CONFIG_UTT_LAN2_IFNAME=%s: \n", __func__, __LINE__, device->name,CONFIG_UTT_LAN2_IFNAME);
	if(strcmp(device->name, CONFIG_UTT_LAN_IFNAME) != 0)
	{/*���Ϊ���߿�*/
	    ret = true;
	}
	dev_put(device);/*�ͷ��豸����*/
    }
    return ret;
}
#endif
#if 0
bool isSkbComeFromLan(struct sk_buff *skb)
{
    struct net_device *device = NULL;
    bool ret = false;
    /*��ȡ����豸*/
    device = dev_get_by_index(sock_net(skb->sk),skb->iif);
    if(device != NULL)
    {
	UTT_HASH_DEBUG("%s %d device %s: \n", __func__, __LINE__, device->name);
	if(strcmp(device->name, CONFIG_UTT_LAN_IFNAME) == 0)
	{/*���Ϊlan��*/
	    ret = true;
	}
	dev_put(device);/*�ͷ��豸����*/
    }
    return ret;
}
#endif
/**
 * ͨ����ǰskb������ ��ȡhash buf
 * isCreat���Ҳ����Ƿ񴴽�
 */
UttNfHashBuf *uttNfHashBufGetBySkb(struct sk_buff*skb, int isCreat)
{
    struct iphdr *ipH = NULL;
    
    UTT_HASH_DEBUG("%s %d device:%s\n", __func__, __LINE__, skb->dev->name);

    ipH = ip_hdr(skb);
    if(isSkbComeFromLan(skb))
    {/*���Ϊlan��*/
	return uttNfHashBufGet(ipH->saddr, &isCreat);
    }
    else 
    {/*iifָ����豸��Ϊlan�ڡ�
      *������������豸�Ƚ����Ի�á�Ҫ�漰��pppoe��vlan�Լ�br����ӿ�
      *����lan�ڽ���ʱ����¼ip������ֻ�ǲ������¼ip*/
	UTT_HASH_DEBUG("%s %d out device:%s dip:%pI4\n", __func__, __LINE__, skb->dev->name, &(ipH->daddr));
	int xrg = 0 ;
	return uttNfHashBufGet(ipH->daddr, &xrg);
    }
    return NULL;
}

/**
 * ��hash���в��� lan�ڽ����İ���Ӧ��Դip��Ӧ�� hasn�� config
 * isCreat: 1 ���Ҳ�����Ӧ�ı������
 *          0 ���Ҳ�����Ӧ�ı��������
 *
 * return:  NULL ����ʧ�� ��isCreatΪ1������ʧ��
 *          ��NULL �ҵ����½���hash�ṹ����ָ��
 *
 */
UttNfHashConf *uttNfHashConfGetBySkb(struct sk_buff* skb, int isCreat)
{
    UttNfHashBuf * buf = uttNfHashBufGetBySkb(skb, isCreat);
    if(buf != NULL)
    {
	return &(buf->conf);
    }
    return NULL;
}
/**
 * ��hash���в���ָ����ip��ַ,��Ӧ��buffer
 * isCreat: 1 ���Ҳ�����Ӧ�ı������
 *          0 ���Ҳ�����Ӧ�ı��������
 * �޸�Ϊ��*isCreat ��NULL����ͬ��0.
 *		    ��NULL����ͬ��1������ʵ����int isCreat = 1. Ȼ����ȡ��ַ
 *
 * return:  NULL ����ʧ�� ��isCreatΪ1������ʧ��
 *          ��NULL �ҵ����½���hash�ṹָ��
 *
 */
UttNfHashBuf *uttNfHashBufGet(u_long ipaddr, int *isCreat)
{
    u_int key = ip_hash_key(ntohl(ipaddr));/*��ȡkey*/
    struct timespec uptime;
    UttNfHashBuf **hashBufPP = &(uttNfIpHashTable[key]);/*����ָ�룬��λ������ͷ��*/

    UTT_HASH_DEBUG("%s %d:hash key %d\n", __func__, __LINE__, key);
    /*������key��Ӧ������*/
    while(*hashBufPP != NULL)
    {
	if((*hashBufPP)->ip == ipaddr)
	{/*�ҵ�*/
	    if(*isCreat == 1)
	    {
		*isCreat  = 0;/*�ҵ������½�*/
		(*hashBufPP)->status = 1;/*״̬λ��Ϊ����*/
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
		if(triggerUser == (1U<<UTT_NF_HASH_WIFIDOG_DES_NOTIFY)) {
		    (*hashBufPP)->trigger |= triggerUser;
		}
#endif
	    }
	    UTT_HASH_DEBUG("%s %d:find ok hash %pI4, key %d\n", __func__, __LINE__, &ipaddr,key);
	    break;
	}
	hashBufPP = &((*hashBufPP)->next);/*������һ����*/
    }
    if(*isCreat == 1)
    {/*��Ҫ�½�����*/
	*hashBufPP = (UttNfHashBuf*) kmalloc(sizeof(UttNfHashBuf), GFP_ATOMIC);/*����һ������*/
	if(*hashBufPP != NULL)
	{/*�ڴ����ɹ�*/
	    memset(*hashBufPP, 0 ,sizeof(UttNfHashBuf));
	    (*hashBufPP)->status = 1;/*״̬λ��Ϊ����*/
	    (*hashBufPP)->ip = ipaddr;
#ifdef CONFIG_UTT_SKB_IIFTYPE
	    (*hashBufPP)->conf.iif = -1;/*��ʼ����ֵΪ-1*/
#endif
	    (*hashBufPP)->conf.userStats.realtimeflowS.lastClrJiffies = jiffies;

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
	    /*��¼��ǰ�뿪��������ʱ��
	     * ����ڲ�ͬ���ں���ʵ�ַ�����ͬ*/
	    do_posix_clock_monotonic_gettime(&uptime);
	    monotonic_to_bootbased(&uptime);
	    (*hashBufPP)->conf.userStats.stTime = (u_long)( uptime.tv_sec);
#endif
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	    if(triggerUser == (1U<<UTT_NF_HASH_WIFIDOG_DES_NOTIFY)) {
		(*hashBufPP)->trigger |= triggerUser;
	    }
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    /**
	     *������ģ�飬��ʼ��ip hash�ڵ������е����ȼ��ֶ�
	     * �޸ĵ�ip_input.c��ȥ��ʼ����by xrg.
	    uttTraverseIpNodeInit(*hashBufPP);*/
#endif

	    UTT_HASH_DEBUG("%s %d:new hash %pI4, key %d\n", __func__, __LINE__, &ipaddr,key);
	}
#if DEBUG_UTT_HASH 
	else
	{
	    UTT_HASH_DEBUG("%s %d: kernel memory alloced error\n", __func__, __LINE__);
	}
#endif
    }

    return *hashBufPP;

}
/*
 * ɾ��hash�����Ӧ������
 * �Լ������������
 * */
static void uttNfHashConfDel(UttNfHashConf *hashConf)
{
#ifdef CONFIG_NETFILTER_XT_TARGET_UTTPOLICY
    /*ɾ����ip��Ӧ�Ĺ�������*/
    uttPolicyListDel(hashConf->policyConf.policyHead);/*ɾ��������������*/
#endif
}
/**
 * ɾ��ipaddr��Ӧ�� hash����
 */
void uttNfHashBufDel(u_long ipaddr)
{
    u_int key = ip_hash_key(ntohl(ipaddr));/*��ȡkey*/
    UttNfHashBuf **hashBufPP = &(uttNfIpHashTable[key]);/*����ָ�룬��λ������ͷ��*/
    UttNfHashBuf *hashBufP = NULL;
    /*������key��Ӧ������*/
UTT_HASH_DEBUG("%s %08x:del hash %08x hash key %u\n", __func__, __LINE__, ipaddr,key);
    while(*hashBufPP != NULL)
    {
	if((*hashBufPP)->ip == ipaddr)
	{/*�ҵ�*/
	    UTT_HASH_DEBUG("%s %08x:del hash %08x\n", __func__, __LINE__, ipaddr);
	    hashBufP = *hashBufPP;/*��¼��ǰ�ڵ�λ��*/
	    *hashBufPP = hashBufP->next;/*ָ��ǰ�ڵ��ָ�룬ָ��ǰ�ڵ��next*/
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    /**
	     *������ģ�飬���ø�ģ����������
	     */
	    uttTraverseIpNodeExit(hashBufP);
#endif
	    uttNfHashConfDel(&(hashBufP->conf));/*ɾ����ǰ�ڵ��Ӧ������*/
	    kfree(hashBufP);/*�ͷŵ�ǰ�ڵ������*/
	    break;
	}
	hashBufPP = &((*hashBufPP)->next);/*������һ����*/
    }

    return;

}

#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
/**
 * �޸�ipaddr��Ӧ�� hash����ı���trigger
 * ��������ⳬʱ��֪ͨӦ�ò�web��֤�˺�����
 */
void uttNfHashBufEditTrigger(UttNlNfHashConf *nfconf)
{
#if 1
    UttNfHashBuf *tmpNode = NULL;

    UTT_HASH_DEBUG("%s %08x:edit hash %08x,trigger %u\n", __func__, __LINE__, nfconf->ip,nfconf->trigger);
    int isCreate = 0;
    tmpNode = uttNfHashBufGet(nfconf->ip, &isCreate);
    if(tmpNode != NULL)
    {
	tmpNode->trigger = nfconf->trigger;
	UTT_HASH_DEBUG("%s %08x:edit hash trigger %d\n", __func__, __LINE__, tmpNode->trigger);
    }
#endif
#if 0
    u_int key = ip_hash_key(ntohl(nfconf->ip));/*��ȡkey*/
    UttNfHashBuf **hashBufPP = &(uttNfIpHashTable[key]);/*����ָ�룬��λ������ͷ��*/
    UttNfHashBuf *hashBufP = NULL;
UTT_HASH_DEBUG("%s %d:edit hash %08x hash key %d\n", __func__, __LINE__, nfconf->ip,key);
    /*������key��Ӧ������*/
    while(*hashBufPP != NULL)
    {
	if((*hashBufPP)->ip == nfconf->ip)
	{/*�ҵ�*/
	    UTT_HASH_DEBUG("%s %d:edit hash %08x,trigger %u\n", __func__, __LINE__, nfconf->ip,nfconf->trigger);
	    (*hashBufPP)->trigger = nfconf->trigger;/*�޸ĵ�ǰ�ڵ��Ӧ��trigger*/
	    UTT_HASH_DEBUG("%s %d:edit hash trigger %d\n", __func__, __LINE__, (*hashBufPP)->trigger);
	    hashBufP = *hashBufPP;/*��¼��ǰ�ڵ�λ��*/
	    *hashBufPP = hashBufP->next;/*ָ��ǰ�ڵ��ָ�룬ָ��ǰ�ڵ��next*/
	    break;
	}
	hashBufPP = &((*hashBufPP)->next);/*������һ����*/
    }
#endif
UTT_HASH_DEBUG("%s %08x: end edit hash %08x\n", __func__, __LINE__, nfconf->ip);

    return;

}
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
/**
 * ����hash��
 * processFun : ������
 */
void uttNfHashTraverseUserConf(void (*processFun)(UttNfHashBuf* buf, void *userConf), void *userConf)
{
    int i = 0;
    UttNfHashBuf *hashBufP = NULL;

    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufP = uttNfIpHashTable[i];/*��λ������ͷ��*/
	/*������key��Ӧ������*/
	while(hashBufP != NULL)
	{
	    processFun(hashBufP,userConf);
	    hashBufP =hashBufP->next;/*������һ����*/
	}
    }
    return;
}
EXPORT_SYMBOL(uttNfHashTraverseUserConf);
#endif
/**
 * ����hash��
 * processFun : ������
 */
void uttNfHashTraverse(void (*processFun)(UttNfHashBuf* buf))
{
    int i = 0;
    UttNfHashBuf *hashBufP = NULL;

    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufP = uttNfIpHashTable[i];/*��λ������ͷ��*/
	/*������key��Ӧ������*/
	while(hashBufP != NULL)
	{
	    processFun(hashBufP);
	    hashBufP =hashBufP->next;/*������һ����*/
	}
    }
    return;
}

#ifdef CONFIG_NETFILTER_XT_TARGET_UTTHOSTSTA
/*
 * ���ip hash���е���IP����Ϊͳ����Ϣ
 */
extern void clearIpBvStats(u_long ip) {
    UttNfHashBuf *hashBufP = NULL;
    u_int key;

    /* �Ƿ�ip */
    if (ip == 0) {
	return;
    }
    key = ip_hash_key(ntohl(ip));
    hashBufP = uttNfIpHashTable[key]; 

    while (hashBufP) {
	if (hashBufP->ip == ip) {
	    memset(hashBufP->conf.userStats.netBVStats, 0, sizeof(hashBufP->conf.userStats.netBVStats));
	    /* �����ǰ��Ϊ״̬ */
	    memset(hashBufP->conf.userStats.netBehavior, 0, sizeof(hashBufP->conf.userStats.netBehavior));
	    break;
	}
	hashBufP = hashBufP->next;
    }
    return;
}
#endif

/**
 * netlink hash������
 */
void uttNlHashConf(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    UttNlNfHashConf  *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfHashConf*)NLMSG_DATA(nlh);/*��λ����������*/

    if(nlData->confType == UTT_NFCONF_DEL) {
    UTT_HASH_DEBUG("%s %08x:netlinke down ip %08x\n", __func__, __LINE__, nlData->ip);
    /*Ŀǰֻ��ɾ��һ�������͡�����С���ͣ����ڴ�����*/
    uttNfHashBufDel(nlData->ip);/*ɾ����Ӧ�ı���*/   
    } else if(nlData->confType == UTT_NFCONF_EDIT) {
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
    /*�޸�trigger*/
    UTT_HASH_DEBUG("%s %08x:netlinke edit ip %08x\n", __func__, __LINE__, nlData->ip);
    uttNfHashBufEditTrigger(nlData);
#endif
    } else if(nlData->confType == UTT_NFCONF_ADD) {
	if(nlData->staleEn == 1) {
	    staleTimeUser = (nlData->staleTime)*60*HZ;
#ifdef CONFIG_UTT_NETFILTER_HASH_TRIGGER
	    triggerUser = nlData->trigger;
	    UTT_HASH_DEBUG("%s-%d: staleTime = %u, staleTimeUser =%u,triggerUser =%u\n",__func__,__LINE__,nlData->staleTime,staleTimeUser,triggerUser);
#endif
	    UTT_HASH_DEBUG("%s-%d: staleTime = %u, staleTimeUser =%u\n",__func__,__LINE__,nlData->staleTime,staleTimeUser);
	}
    }
    return;
}
void uttNfHashInit(void)
{
    nfHashPollInit();/*��ѯ��ʼ��*/
#if 0
#if DEBUG_UTT_HASH
    UttHashDebug();
#endif
#endif
}

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
/********************************************************************
 * ������ uttIpMatch
 * ���ܣ� ���ݰ�IP��ַ��ƥ�亯��
 * ������ 2013-02-20
 * ������  ipGrp: Ҫƥ���IP��
 *	  ip:	 ���ݰ�ip��ַ
 * ���أ� 1  ƥ��ɹ�
 *	  0  ƥ��ʧ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/ 
int uttIpMatch(IpGrpEntry *ipGrp, uint32_t ip)
{
    uint32_t ret = 0;
    uint32_t ipHost = ntohl(ip);
    uint32_t size = sizeof(ipHost) << 1;

    if(ipGrp != NULL) {
	if(ipGrp->entryType == IP_RANGE){ 
	    /* ip ��ַ��ƥ��
	     * ����ʼ�ͽ���ip��Ϊ0x0ʱ��Ĭ��Ϊ�����е�ַ
	     * */
	    /*���е�ַ*/
	    if((ipGrp->entryVal.range.ipFrom == 0U)&&(ipGrp->entryVal.range.ipTo == 0U)) {
		ret = 1;
	    }else if((ipHost >= ipGrp->entryVal.range.ipFrom)&&(ipHost <= ipGrp->entryVal.range.ipTo)) {	/*��ַ�αȽ�*/
		ret = 1;
	    }
	}else if(ipGrp->entryType == IP_GROUP){
	    if (ipGrp->entryVal.index == CONFIG_ALL_USER) {/*�û���Ϊ�����û�ʱ*/
		ret = 1;
	    } else {
		/* ip ��ƥ��
		 * ����������ȡipset������ֵ
		 * Ȼ���������ֵ���ip�Ƿ��ڴ�ipset����*/
		if(ipGrp->entryVal.index != IP_SET_INVALID_ID) {
		    ret = ip_set_testip_by_index(ipGrp->entryVal.index,(void *)&ipHost,size);
		}
	    }
	}
    }
    return ret;
}
EXPORT_SYMBOL(uttIpMatch);
#endif
