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
struct statSniper_priv{
    int index;
    UttNfHashBuf *node;
};

extern UttNfHashBuf *uttNfIpHashTable[MAX_UTTNF_HASH_TABLE];
extern __u32 ntsr_global_control_time;
#define DEBUG_UTT_STATSNIPER 1
#if DEBUG_UTT_STATSNIPER
#define UTT_STATSNIPER_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_STATSNIPER_DEBUG(fmt, argx...)
#endif

#define NTSR_IP_TTL_FLAG    (1U)

/* 网络尖兵ip id识别标识 */
#define NTSR_IP_ID_FLAG     (1U << 1U)

/* 主机被识别为网络尖兵标识 */
#define NTSR_FLAG       (NTSR_IP_TTL_FLAG | NTSR_IP_ID_FLAG)

static __u32 get_current_time(void){
	struct timeval curTv;
	do_gettimeofday(&curTv);
	return curTv.tv_sec;
}

static __u32 rl_age(__u32 identify_time){
	return get_current_time() - identify_time;
}

static UttNfHashBuf* firstNode(struct seq_file *seq)
{
    UttNfHashBuf *tmpNode;
    struct statSniper_priv *state = seq->private;
    int index = 0;
    for(index = 0; index < MAX_UTTNF_HASH_TABLE; index++) {
	tmpNode = rcu_dereference(uttNfIpHashTable[index]);
	if(tmpNode != NULL&&(tmpNode->conf.netSniper.ntsr_flag == NTSR_FLAG)) {
	    state->node = tmpNode;
	    state->index = index;
	    return tmpNode;
	}
    }
    return NULL;
}
//static int statsPerIpIndex = 0;

static UttNfHashBuf* nextNode(struct seq_file *seq)
{
    UttNfHashBuf *tmpNode;
    struct statSniper_priv *state = seq->private;
    int index = state->index;
    tmpNode = state->node;
    tmpNode = rcu_dereference(tmpNode->next);
    if(tmpNode != NULL&&(tmpNode->conf.netSniper.ntsr_flag == NTSR_FLAG)) {
        state->node = tmpNode;
        state->index = index;
        return tmpNode;
    } else {
        for(index = (index + 1); index < MAX_UTTNF_HASH_TABLE; index++) {
            tmpNode = rcu_dereference(uttNfIpHashTable[index]);
			if(tmpNode != NULL&&(tmpNode->conf.netSniper.ntsr_flag == NTSR_FLAG)) {
                state->node = tmpNode;
                state->index = index;
                return tmpNode;
            }
        }
    }
    return NULL;
}
static void *statSniper_seq_start(struct seq_file *seq, loff_t *pos)
    __acquires(RCU)
{
    UTT_STATSNIPER_DEBUG("statSniper %s %d %d\n", __func__, __LINE__, *pos);
    rcu_read_lock();
    if(*pos == 0)
    {
	return SEQ_START_TOKEN;
    }
    else if(*pos == 1)
    {
	return firstNode(seq);
    }
    return nextNode(seq);
}


static void *statSniper_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    void *rc =  NULL;
    UTT_STATSNIPER_DEBUG("statSniper %s %d %d\n", __func__, __LINE__, *pos);
    if (v == SEQ_START_TOKEN) {
	rc =  firstNode(seq);
    }
    else
    {
	rc = nextNode(seq);
    }

    (*pos)++;
    return rc;
}

static int statSniper_seq_show(struct seq_file *seq, void *v)
{

    UttNfHashBuf *tmpNode;
    UttNetSniper *netSniper = NULL;
    int i;

    UTT_STATSNIPER_DEBUG("%s %d\n", __func__, __LINE__);
    if (v == SEQ_START_TOKEN) {
	seq_puts(seq, "Ip    ctlTime\n");
	return 0;
    }
    tmpNode = (UttNfHashBuf *)(v);
    if(tmpNode == NULL) {
	return 0;
    }

    netSniper = &(tmpNode->conf.netSniper);/*定位到状态*/
    seq_printf(seq,"%pI4 %lu",
	    &(tmpNode->ip),
		(ntsr_global_control_time - rl_age(netSniper->ntsr_identify_time))
	    );
    seq_printf(seq, "\n");

    return 0;
}

static void statSniper_seq_stop(struct seq_file *s, void *v)
__releases(RCU)
{
    rcu_read_unlock();
}
static const struct seq_operations statSniper_seq_ops = {
	.start  = statSniper_seq_start,
	.next   = statSniper_seq_next,
	.stop   = statSniper_seq_stop,
	.show   = statSniper_seq_show,
};
static int statSniper_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &statSniper_seq_ops, sizeof(struct statSniper_priv));
}


static const struct file_operations statSniper_seq_fops = {
	.owner		= THIS_MODULE,
	.open           = statSniper_seq_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release	= seq_release_net,
};
static int __net_init statSniper_net_init(struct net *net)
{
	if (!proc_net_fops_create(net, "statSniper", S_IRUGO, &statSniper_seq_fops))
		return -ENOMEM;
	return 0;
}

static void __net_exit statSniper_net_exit(struct net *net)
{
	proc_net_remove(net, "statSniper");
}
static struct pernet_operations statSniper_net_ops = {
	.init = statSniper_net_init,
	.exit = statSniper_net_exit,
};

/**
 * 注册proc文件。跟用户进行交互
 * */
void uttStatSniperInit(void)
{
    register_pernet_subsys(&statSniper_net_ops);
}
