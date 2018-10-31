#ifndef __H_UTT_RATE_LIMITING_H__
#define __H_UTT_RATE_LIMITING_H__


static unsigned int utt_up_ratelimit(UttLimitRunStat *stat, UttLimitConf *limitConf, unsigned int len)
{
    int ret = 1;
    
    //printk("jiff: %u to %u\n", jiffies, stat->ulastjiffies);
    stat->ucredit += (jiffies - xchg(&stat->ulastjiffies, jiffies)) * limitConf->uRate;
    //printk("up len:%u, jiffiesstat->ucredit:%u, rate:%u, max:%u\n", len, stat->ucredit, limitConf->uRate,limitConf->umaxCredit);
    if(stat->ucredit > limitConf->umaxCredit) {
        stat->ucredit = limitConf->umaxCredit;
    }
    
    if(stat->ucredit > len) {
        stat->ucredit -= len;
    } else {
//	printk("up len:%u old credit:%u, jiff:%u, new: credit:%u jiff:%u\n",len, tmp1, tmp3, stat->ucredit, tmp2);
        ret = 0;
    }
//    printk("new stat->ucredit:%u\n", stat->ucredit);
    return ret;
}

static unsigned int utt_down_ratelimit(UttLimitRunStat *stat, UttLimitConf *limitConf, unsigned int len)
{
    int ret = 1;
    
    stat->dcredit += (jiffies - xchg(&stat->dlastjiffies, jiffies)) * limitConf->dRate;
  //  printk("len: %u, down stat->ucredit:%u, rate:%u, max:%u\n", len, stat->dcredit, limitConf->dRate,limitConf->dmaxCredit);
    if(stat->dcredit > limitConf->dmaxCredit) {
        stat->dcredit = limitConf->dmaxCredit;
    }
    
    if(stat->dcredit > len) {
        stat->dcredit -= len;
    } else {
//	printk("down len:%d, old credit:%u, jiff:%u, new: credit:%u jiff:%u\n",len, tmp1, tmp3, stat->dcredit, tmp2);
        ret = 0;
    }
   // printk("new stat->ucredit:%u\n", stat->dcredit);
    return ret;
}
#endif
