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
 *��ϸ�������ں����ýṹ
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
    uint32_t uRate; /*�ϴ�����*/
    uint32_t dRate; /*��������*/
    uint32_t umaxCredit; /*�ϴ�����*/
    uint32_t dmaxCredit; /*��������*/
    uint32_t limitPolicy;/*��������*/
    atomic_t ref;	    /* ���ü���,��¼�����ñ����ٸ�ip hash�ڵ������� */
    char name[UTT_INST_NAME_LEN]; /* ��Ӧҳ������ٹ����� */
    IpGrpEntry ipSrcGrpEntry;  /*ip��Ϣ������ע��ipset��index*/
    UttTimeHookOps *timeHookOps;/*���ں��������������õ���Чʱ�乳��*/
    uint32_t instIndex;/*��¼�����˳��*/
    bool isAvailable;/*��ʾ�������Ƿ�ɱ�ip hash�ڵ����õ�*/
    UttLimitRunStat runStat;/*����״̬*/
}UttLimitConf;


/**
 *�������ں����ýṹ��Ŀǰֻ��PPPoE���˺ŵ����ٻ���뵽�������У�
 */
typedef struct {
    struct list_head list;
    rwlock_t lock;  
    unsigned char limitPolicy;/*1��ʾ����2��ʾ����;����Ŀ��ʱû���õ��������Ժ���չ*/
    uint32_t uRate; /*�ϴ�����*/
    uint32_t dRate; /*��������*/

    uint32_t umaxCredit; /*�ϴ�����*/
    uint32_t dmaxCredit; /*��������*/

    ip_set_id_t index;/*���������Ӧ��ipset����*/

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
