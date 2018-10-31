#ifndef __UTT_LIMIT_H__
#define __UTT_LIMIT_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter_ipv4/utt_effective_time.h>
#include <linux/ip.h>
#include <linux/netfilter_ipv4/ip_set.h>

#ifndef UTT_INST_NAME_LEN
#define UTT_INST_NAME_LEN 16
#endif

/**
 *
 *精细化限速内核配置结构
 */
typedef struct {
        u_int                   ucredit;
        u_int                   dcredit;
        u_int                   ulastjiffies;
        u_int                   dlastjiffies;
}UttLimitRunStat;

typedef struct {
    struct list_head list;
    rwlock_t lock;  
    uint32_t uRate; /*上传速率*/
    uint32_t dRate; /*下载速率*/
    uint32_t umaxCredit; /*上传速率*/
    uint32_t dmaxCredit; /*下载速率*/
    uint32_t limitPolicy;/*共享或独立*/
    atomic_t ref;	    /* 引用计数,记录该配置被多少个ip hash节点所引用 */
    char name[UTT_INST_NAME_LEN]; /* 对应页面的限速规则名 */
    IpGrpEntry ipSrcGrpEntry;  /*ip信息。这里注意ipset用index*/
    UttTimeHookOps *timeHookOps;/*该内核限速配置所引用的生效时间钩子*/
    uint32_t instIndex;/*记录规则的顺序*/
    bool isAvailable;/*表示该配置是否可被ip hash节点引用的*/
    UttLimitRunStat runStat;/*限速状态*/
}UttLimitConf;


/**
 *限速组内核配置结构（目前只有PPPoE的账号的限速会加入到限速组中）
 */
typedef struct {
    struct list_head list;
    rwlock_t lock;  
    unsigned char limitPolicy;/*1表示独享、2表示共享;此条目暂时没有用到，便于以后扩展*/
    uint32_t uRate; /*上传速率*/
    uint32_t dRate; /*下载速率*/

    uint32_t umaxCredit; /*上传速率*/
    uint32_t dmaxCredit; /*下载速率*/

    ip_set_id_t index;/*该限速组对应的ipset索引*/

    UttLimitRunStat runStat;
} LimitGrpConf;

static inline unsigned int utt_up_ratelimit(UttLimitRunStat *stat, UttLimitConf *limitConf, unsigned int len)
{
    int ret = UTT_IPT_CONTINUE;

    stat->ucredit += (jiffies - xchg(&stat->ulastjiffies, jiffies)) * limitConf->uRate;
    if(stat->ucredit > limitConf->umaxCredit) {
                stat->ucredit = limitConf->umaxCredit;
    }
    
    if(stat->ucredit > len) {
        stat->ucredit -= len;
    } else {
        ret = UTT_NF_DROP;
    }
    return ret;
}

static inline unsigned int utt_down_ratelimit(UttLimitRunStat *stat, UttLimitConf *limitConf, unsigned int len)
{
    int ret = UTT_IPT_CONTINUE;
    
    stat->dcredit += (jiffies - xchg(&stat->dlastjiffies, jiffies)) * limitConf->dRate;
    if(stat->dcredit > limitConf->dmaxCredit) {
                stat->dcredit = limitConf->dmaxCredit;
    }
    
    if(stat->dcredit > len) {
        stat->dcredit -= len;
    } else {
        ret = UTT_NF_DROP;
    }
    return ret;
}

static inline unsigned int utt_up_ratelimit_grp(UttLimitRunStat *stat, LimitGrpConf *limitConf, unsigned int len)
{
    int ret = UTT_IPT_CONTINUE;

    stat->ucredit += (jiffies - xchg(&stat->ulastjiffies, jiffies)) * limitConf->uRate;
    if(stat->ucredit > limitConf->umaxCredit) {
                stat->ucredit = limitConf->umaxCredit;
    }
    
    if(stat->ucredit > len) {
        stat->ucredit -= len;
    } else {
        ret = UTT_NF_DROP;
    }
    return ret;
}

static inline unsigned int utt_down_ratelimit_grp(UttLimitRunStat *stat, LimitGrpConf *limitConf, unsigned int len)
{
    int ret = UTT_IPT_CONTINUE;
    
    stat->dcredit += (jiffies - xchg(&stat->dlastjiffies, jiffies)) * limitConf->dRate;
    if(stat->dcredit > limitConf->dmaxCredit) {
                stat->dcredit = limitConf->dmaxCredit;
    }
    
    if(stat->dcredit > len) {
        stat->dcredit -= len;
    } else {
        ret = UTT_NF_DROP;
    }
    return ret;
}
#endif
