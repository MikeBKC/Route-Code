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
#include <net/pkt_sched.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#ifdef CONFIG_UTT_CLOUD_CONFIG
extern int uttNlOnRemindToUser(unsigned char mac[],unsigned int noticeBanMask);
static unsigned int checkSkbFormDev(struct sk_buff *skb,int index);
#endif

struct statsPerIp_priv{
    int index;
    UttNfHashBuf *node;
};

extern UttNfHashBuf *uttNfIpHashTable[MAX_UTTNF_HASH_TABLE];
#if 0
static void clearBufUttStatsPerIp(UttNfHashBuf *buf);
#endif
#define DEBUG_UTT_STATSPERIP 0
#if DEBUG_UTT_STATSPERIP
#define UTT_STATSPERIP_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_STATSPERIP_DEBUG(fmt, argx...)
#endif

#ifdef CONFIG_UTT_CLOUD_CONFIG
#if 0
/**
 *������Ϣ���û�̬
 *��֪�������ѵ�mac������
 * xrg
 ***/
static int uttNlOnRemindToUser(unsigned char mac[],unsigned int noticeBanMask)
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
 *�ж�skb�����������ߣ����߻���׬Ǯwifi��
 * ����ֵ�ж�
 * 2������
 * 7��׬Ǯwifi������
 * 3������׬Ǯwifi����������
 * ���ֶ�Ӧ�û���������ƶ�λ��
 * **/
static unsigned int checkSkbFormDev(struct sk_buff *skb,int index)
{
    struct net_device *device = NULL;
    unsigned int ret = 0;

    /*��ȡ����豸*/
    device = dev_get_by_index(sock_net(skb->sk),index);
    if(device != NULL)
    {
	if(strcmp(device->name, CONFIG_UTT_LAN_IFNAME) != 0)
	{
	    UTT_STATSPERIP_DEBUG("xrg.line = %d,,,,.index =.%d..device->name =%s\n",__LINE__,index,device->name);
    	  if((strcmp(device->name, "ra2") == 0)||(strcmp(device->name, "rai2") == 0)||(strcmp(device->name, "ra3") == 0)||(strcmp(device->name, "rai3") == 0))
	    {
		ret = 7 ;/*׬Ǯwifi*/
	    }
	    else if((strcmp(device->name, "ra0") == 0)||(strcmp(device->name, "rai0") == 0))
	    {
		ret = 3 ;/*��׬Ǯwifi�����ͨwifi*/
	    }
	    
	}
	else
	{
	    ret = 2 ;/*���ߵ���1*/
	}
	dev_put(device);/*�ͷ��豸����*/
    }
    else
    {
	printk("not have device\n");
    }
    return ret;
}


#endif
/*
 * ����ǰip hash���е���Ϊͳ���ۼӵ�uttTotalStat
 */
extern void addIpHashBvStats(UttAppSta *uttTotalStat) {
    int i = 0, j = 0;
    UttNfHashBuf *hashBufP = NULL;

    for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++)
    {
	hashBufP = uttNfIpHashTable[i];/*��λ������ͷ��*/
	/*������key��Ӧ������*/
	while(hashBufP != NULL)
	{
	    /* �ۼ���Ϊͳ����Ϣ */
	    for (j = 0; j < (UTT_APP_MAX_ARR - 1); j++) {
		if ((hashBufP->conf.userStats.netBehavior)[j] != 0) {
		    uttTotalStat[j].conns++;
		}
	    }
	    hashBufP =hashBufP->next;/*������һ����*/
	}
    }
    return;
}
/*
 * �û���Ϊͳ��
 * ����skb��priFlag��ʶ��ͳ�Ƶ�stats��
 */
static void uttNfStatsBehavior(UttStatsPerIp *stats, const struct sk_buff *skb) {
    __u8 type = skb->utt_app.priFlag;

    if (!stats) {
	return;
    }
    /* 
     * ����Ҫͳ����������, priFlagΪ0����PDB_OTHERΪ��������,mibpdb.h 
     * type < UTT_APP_MAX_ARR ������type <= UTT_APP_MAX_ARR����Ϊ��
     * ����PDB_OTHER����
     */
    if ((type > 0) && (type < UTT_APP_MAX_ARR)) {
	if ((stats->netBehavior)[type - 1] == 0) {
	    (stats->netBehavior)[type - 1] ++;
	}
    }
    return;
}


/**
 * ����ipͳ��ÿ��ip����Ϣ
 */
void uttNfStatsPerIp(struct sk_buff *skb)
{
    UttNfHashConf* hashConf = NULL;
    struct iphdr *ipH = NULL;
    struct ethhdr *eth;
    UttStatsPerIp *stats = NULL;
    RealtimeFlowStatus *realtimeflowS;
    unsigned long old_skb_len = -1;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    UttNfHashBuf *ipHashNode =  skb->ipHashNode;
#endif

    ipH = ip_hdr(skb);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    if (ipHashNode == NULL) {
	return;
    }
    hashConf = &(ipHashNode->conf);
#endif
    if(isSkbComeFromLan(skb)) {/*�� lan�ڽ���İ�*/
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	/*����hashConf�Ķ�ȡҲ���Էŵ����ú����С�����ÿ�ζ�hash*/
	hashConf = uttNfHashConfGet(ipH->saddr, 1);/*��hash���ȡ����*/
	if(hashConf != NULL) {
#endif
	    stats = &(hashConf->userStats);/*��λ��״̬*/

#if !(CONFIG_UTT_MAC_LIMIT == FYES)
	    /*��mac��ַ�ĸ�ֵ�޸ĵ�ip_input.c�� 
					    by xrg*/
	    eth = eth_hdr(skb);/*ȡmacͷ*/
	    if (((unsigned char *)eth) + ETH_HLEN <= skb->data) { /* eth���ȡ��ȷ */
		UTT_STATSPERIP_DEBUG("eth get right!\n");
		memcpy(stats->mac, eth->h_source, ETH_ALEN);/*����ȥ�İ���Ҫ��¼mac����ֹ�仯*/
	    } else {/* ��ȡ���󣬽�ͳ��mac��0 */
		UTT_STATSPERIP_DEBUG("eth get error!\n");
		memset(stats->mac, 0, ETH_ALEN);
	    }
#endif
	   
	    stats->txBytes += skb->len;/*��¼��������*/
#if 1
	    /* �û���Ϊͳ�� */
	    uttNfStatsBehavior(stats, skb);
#endif
#ifdef CONFIG_UTT_SKB_IIFTYPE
	    if (skb->skb_iif != hashConf->iif) {/*skb��skb_iif���ϴε�skb_iif��ͬ*/
		hashConf->iif = skb->skb_iif;
		if (isWirelessIn(skb)) {/*��ra0����*/
		    stats->iftype = IS_WIRELESS;
		} else {
		    stats->iftype = DEFAULT_IFTYPE;
		}
	    }
#endif
	    realtimeflowS = &(stats->realtimeflowS);
	    if (time_before(realtimeflowS->lastClrJiffies+HZ,jiffies)) {/*������1���ڵ�ʵʱ����*/
		if (time_before(realtimeflowS->lastClrJiffies+2*HZ,jiffies)) {/*������ݰ�֮���ʱ�䳬��2��*/
		    realtimeflowS->rtTxBytes = 0;
		} else {
		    realtimeflowS->rtTxBytes = realtimeflowS->tmpTxBytes;
		}
		realtimeflowS->tmpTxBytes = skb->len;
		realtimeflowS->tmpRxBytes = 0;
		realtimeflowS->lastClrJiffies = jiffies;
	    } else {
		realtimeflowS->tmpTxBytes += skb->len;/*��û��1��ʱ*/
	    }
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	}
#endif
    } else {
	/*����lan�ڵİ���������lan�ڵİ������շ���lan����ʱ��������hash���ʱ�����˵�����wan�ڵ�*/
	/*����ֻ��¼ ������������Сcpu����*/
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	hashConf = uttNfHashConfGet(ipH->daddr, 0);/*��hash���ȡ����*/
	if(hashConf != NULL) {
#endif
	    stats = &(hashConf->userStats);

#ifdef CONFIG_UTT_CLOUD_CONFIG
	    old_skb_len = stats->rxBytes;
#endif 
	    stats->rxBytes += skb->len;
#ifdef CONFIG_UTT_CLOUD_CONFIG
	    if(unlikely((old_skb_len == 0)&&(stats->rxBytes !=0)))
	    {
		unsigned int noticeMask = 0x0;
		unsigned int ret = checkSkbFormDev(skb,hashConf->iif);
		if(ret>0)
		{
		    noticeMask |= (1u << ret);	
		    UTT_STATSPERIP_DEBUG("xrg send msg to user ret = %d,,noticeMask = %d\n",ret,noticeMask);
		    /*������Ϣmac(stats->mac)������*/
		    uttNlOnRemindToUser(stats->mac,noticeMask);		    
		}
		else
		{
		    UTT_STATSPERIP_DEBUG("ret = %d ,skb error,%d\n",ret,__LINE__);
		}

	    }
#endif
	    realtimeflowS = &(stats->realtimeflowS);
	    if (time_before(realtimeflowS->lastClrJiffies+HZ,jiffies)) {/*������1���ڵ�ʵʱ����*/
		if (time_before(realtimeflowS->lastClrJiffies+2*HZ,jiffies)) {/*������ݰ�֮���ʱ�䳬��2��*/
		    realtimeflowS->rtRxBytes = 0;
		} else {
		    realtimeflowS->rtRxBytes = realtimeflowS->tmpRxBytes;
		}
		realtimeflowS->tmpRxBytes = skb->len;
		realtimeflowS->tmpTxBytes = 0;
		realtimeflowS->lastClrJiffies = jiffies;
	    } else {
		realtimeflowS->tmpRxBytes += skb->len;/*��û��1��ʱ*/
	    }
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
	}
#endif
    }
    return;

}

#if 0
/*���ͳ���е� ����ͳ��*/
void clearUttStatsPerIp(void)
{
    uttNfHashTraverse(clearBufUttStatsPerIp);
    return;
}
/**
 * ���ip hash bufָ�������ͳ��
 */
static void clearBufUttStatsPerIp(UttNfHashBuf *buf)
{
    UttStatsPerIp *stats = NULL;
    stats = &(buf->conf.userStats);/*��λ��״̬*/
    stats->txBytes = 0u; /*��շ���*/
    stats->rxBytes = 0u;/*��ս���*/
    return;
}
#endif
static UttNfHashBuf* firstStatsPerIpNode(struct seq_file *seq)
{
    UttNfHashBuf *tmpNode;
    struct statsPerIp_priv *state = seq->private;
    int statsPerIpIndex = 0;
    for(statsPerIpIndex = 0; statsPerIpIndex < MAX_UTTNF_HASH_TABLE; statsPerIpIndex++) {
	tmpNode = rcu_dereference(uttNfIpHashTable[statsPerIpIndex]);
	if(tmpNode != NULL) {
	    state->node = tmpNode;
	    state->index = statsPerIpIndex;
	    return tmpNode;
	}
    }
    return NULL;
}
//static int statsPerIpIndex = 0;

static struct UttNfHashBuf* nextStatsPerIpNode(struct seq_file *seq)
{
    UttNfHashBuf *tmpNode;
    struct statsPerIp_priv *state = seq->private;
    int statsPerIpIndex = state->index;
    tmpNode = state->node;
    tmpNode = rcu_dereference(tmpNode->next);
    if(tmpNode != NULL) {
        state->node = tmpNode;
        state->index = statsPerIpIndex;
        return tmpNode;
    } else {
        for(statsPerIpIndex = (statsPerIpIndex + 1); statsPerIpIndex < MAX_UTTNF_HASH_TABLE; statsPerIpIndex++) {
            tmpNode = rcu_dereference(uttNfIpHashTable[statsPerIpIndex]);
            if(tmpNode != NULL) {
                state->node = tmpNode;
                state->index = statsPerIpIndex;
                return tmpNode;
            }
        }
    }
    return NULL;
}
static void *statsPerIp_seq_start(struct seq_file *seq, loff_t *pos)
    __acquires(RCU)
{
    UTT_STATSPERIP_DEBUG("%s %d %d\n", __func__, __LINE__, *pos);
    rcu_read_lock();
    if(*pos == 0)
    {
	return SEQ_START_TOKEN;
    }
    else if(*pos == 1)
    {
	return firstStatsPerIpNode(seq);
    }
    return nextStatsPerIpNode(seq);
}


static void *statsPerIp_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    void *rc =  NULL;
    UTT_STATSPERIP_DEBUG("%s %d %d\n", __func__, __LINE__, *pos);
    if (v == SEQ_START_TOKEN) {
	rc =  firstStatsPerIpNode(seq);
    }
    else
    {
	rc = nextStatsPerIpNode(seq);
    }

    (*pos)++;
    return rc;
}

static int statsPerIp_seq_show(struct seq_file *seq, void *v)
{

    UttNfHashBuf *tmpNode;
    UttStatsPerIp *stats = NULL;
    int i;

    UTT_STATSPERIP_DEBUG("%s %d\n", __func__, __LINE__);
    if (v == SEQ_START_TOKEN) {
#ifdef CONFIG_UTT_SKB_IIFTYPE
	seq_puts(seq, "Ip    mac     startTime   rxBytes    txBytes IM P2P STOCK VIDEO GAME SHOP SNS WEBGAME MAIL FORUM rtInBytes rtOutBytes IFTYPE\n");
#else
	seq_puts(seq, "Ip    mac     startTime   rxBytes    txBytes IM P2P STOCK VIDEO GAME SHOP SNS WEBGAME MAIL FORUM rtInBytes rtOutBytes\n");
#endif
	return 0;
    }
    tmpNode = (UttNfHashBuf *)(v);
    if(tmpNode == NULL) {
	return 0;
    }

    stats = &(tmpNode->conf.userStats);/*��λ��״̬*/
    seq_printf(seq,"%pI4 %02x:%02x:%02x:%02x:%02x:%02x %lu %llu %llu",
	    &(tmpNode->ip),
	    stats->mac[0],stats->mac[1],stats->mac[2],stats->mac[3],stats->mac[4], stats->mac[5],
	    stats->stTime, stats->rxBytes, stats->txBytes
	    );
    for (i = 0; i < UTT_APP_MAX_ARR - 1; i++) {
	if (stats->netBehavior[i] == 0) {
	    seq_printf(seq, " %u", stats->netBVStats[i]);
	} else {
	    seq_printf(seq, " %u", stats->netBVStats[i] + 1);
	}
    }
    if (time_before(stats->realtimeflowS.lastClrJiffies+2*HZ,jiffies)) {/*������ݰ�֮���ʱ�䳬��2��*/
	seq_printf(seq, " %u %u", 0,0);
    } else {
	seq_printf(seq, " %u %u", stats->realtimeflowS.rtRxBytes,stats->realtimeflowS.rtTxBytes);
    }
#ifdef CONFIG_UTT_SKB_IIFTYPE
    seq_printf(seq, " %u", stats->iftype);
#endif
    seq_printf(seq, "\n");

    return 0;
}

    static void statsPerIp_seq_stop(struct seq_file *s, void *v)
__releases(RCU)
{

    UTT_STATSPERIP_DEBUG("%s %d\n", __func__, __LINE__);
    //    statsPerIpIndex = 0;
    rcu_read_unlock();
}
static const struct seq_operations statsPerIp_seq_ops = {
	.start  = statsPerIp_seq_start,
	.next   = statsPerIp_seq_next,
	.stop   = statsPerIp_seq_stop,
	.show   = statsPerIp_seq_show,
};
static int statsPerIp_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &statsPerIp_seq_ops, sizeof(struct statsPerIp_priv));
#if 0
	return seq_open(file, &statsPerIp_seq_ops);
#endif
}


static const struct file_operations statsPerIp_seq_fops = {
	.owner		= THIS_MODULE,
	.open           = statsPerIp_seq_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release	= seq_release_net,
#if 0
	.release	= seq_release
#endif
};
static int __net_init statsPerIp_net_init(struct net *net)
{
	if (!proc_net_fops_create(net, "statsPerIp", S_IRUGO, &statsPerIp_seq_fops))
		return -ENOMEM;
	return 0;
}

static void __net_exit statsPerIp_net_exit(struct net *net)
{
	proc_net_remove(net, "statsPerIp");
}
static struct pernet_operations statsPerIp_net_ops = {
	.init = statsPerIp_net_init,
	.exit = statsPerIp_net_exit,
};

/**
 * ע��proc�ļ������û����н���
 * */
void uttStatsPerIpInit(void)
{
    register_pernet_subsys(&statsPerIp_net_ops);
}
