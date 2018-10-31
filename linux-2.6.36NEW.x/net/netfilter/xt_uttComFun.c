#include <linux/timer.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#define MAX(A,B)       ( ((A) >= (B)) ? (A) : (B) )
/**
 * 共用函数
 */
/************************************************************************
 * 函 数 名：	_checkData
 * 功能描述： 	检查字符是否合法
 * 输入参数:	待检查字符
 * 输出参数：	无
 * 返回值：   	1表示合法， 0表示不合法
***********************************************************************/
extern int _checkData(char a)
{
    int flag = 0;
    if (((a >= '0') && (a <= '9'))
	    || ((a >= 'a') && (a <= 'z'))
	    || ((a >= 'A') && (a <= 'Z'))
	    || (a == '.')
	    || (a == '_')
	    || (a == '-')
	    || (a == '@'))
	flag = 1;
    return flag;
}


/**
 * 从 skb的到达时间 转换为 规则认识的时间格式
 * 注意此函数在不同的内核上实现不同
 */
extern void uttNfPolicyGetTime(struct sk_buff* skb, UttNfCurTime* curTime)
{
    s64 stamp;
#if 1
    if (skb->tstamp.tv64 == 0)
    {/*skb的到达时间未记录。则记录之*/
	__net_timestamp((struct sk_buff *)skb);
    }
    stamp = ktime_to_ns(skb->tstamp);
#else
    struct timeval curTv;
    do_gettimeofday(&curTv);
    stamp = timeval_to_ns(&curTv);
#endif
    stamp = div_s64(stamp, NSEC_PER_SEC);/*微秒转换为 秒*/
    stamp -= 60u * sys_tz.tz_minuteswest;/*计算时区偏移*/
    curTime->time = (u_int)stamp % 86400u ;/*一天86400秒*/
    /*
     * 1970-01-01 (w=0) was a Thursday (4). 1u<<3
     */

    curTime->week = 0x40u >> ((3u + ((u_int)stamp /86400u)) % 7u);/*0x40表示星期一和配置一致*/
    //UTT_POLICY_DEBUG("%s %d curTime:%u curWeek:%02x\n", __func__, __LINE__, curTime->time, curTime->week);
    return;
}
/**
 * 时间比较
 * return : cuTime在时间段 timestart 和 tiomestop内 true 否则false
 */
extern  bool uttTimeIn(u_int curTime, u_int timeStart, u_int timeStop)
{
    bool ret = true;
    if(timeStart >timeStop)
    {/*当天的结束到第二天的开始*/
	if ((curTime >  timeStop)  && (curTime < timeStart))
	{/*当前的 结束到 开始 不匹配*/
	    ret = false;
	}

    }
    else
    {/*当前的开始到当天的结束*/
	if ((curTime <  timeStart) ||  
		(curTime > timeStop))
	{/*不在此时间段内则不匹配*/
	    ret = false;
	}

    }
    return ret;
}
static void pre_bmbc(__u8 *x, int m, int bmBc[])
{
   int i;
 
   for (i = 0; i < ASIZE; ++i)
      bmBc[i] = m;
   for (i = 0; i < m - 1; ++i){
      bmBc[x[i]] = m - i - 1;
   }
}
extern __u8 *bm_strstr(__u8 *y, int n, __u8 *x, int m)
{
   int i, j, bmBc[ASIZE];
   int t = n - m;
    if(t < 0 || n == 0 || m == 0)
        return 0;
   /* Preprocessing */
   pre_bmbc(x, m, bmBc);
 
   /* Searching */
   j = 0;
   while (j <= t) {
      for (i = m - 1; i >= 0 && x[i] == y[i + j]; --i);
      if (i < 0) {
           return &y[j];
      }
      else
         j += MAX(1 , bmBc[(int)(y[i + j])] - m + 1 + i);

   }
   return 0;
}

