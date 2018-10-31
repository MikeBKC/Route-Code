#include <linux/timer.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>

#define MAX(A,B)       ( ((A) >= (B)) ? (A) : (B) )
/**
 * ���ú���
 */
/************************************************************************
 * �� �� ����	_checkData
 * ���������� 	����ַ��Ƿ�Ϸ�
 * �������:	������ַ�
 * ���������	��
 * ����ֵ��   	1��ʾ�Ϸ��� 0��ʾ���Ϸ�
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
 * �� skb�ĵ���ʱ�� ת��Ϊ ������ʶ��ʱ���ʽ
 * ע��˺����ڲ�ͬ���ں���ʵ�ֲ�ͬ
 */
extern void uttNfPolicyGetTime(struct sk_buff* skb, UttNfCurTime* curTime)
{
    s64 stamp;
#if 1
    if (skb->tstamp.tv64 == 0)
    {/*skb�ĵ���ʱ��δ��¼�����¼֮*/
	__net_timestamp((struct sk_buff *)skb);
    }
    stamp = ktime_to_ns(skb->tstamp);
#else
    struct timeval curTv;
    do_gettimeofday(&curTv);
    stamp = timeval_to_ns(&curTv);
#endif
    stamp = div_s64(stamp, NSEC_PER_SEC);/*΢��ת��Ϊ ��*/
    stamp -= 60u * sys_tz.tz_minuteswest;/*����ʱ��ƫ��*/
    curTime->time = (u_int)stamp % 86400u ;/*һ��86400��*/
    /*
     * 1970-01-01 (w=0) was a Thursday (4). 1u<<3
     */

    curTime->week = 0x40u >> ((3u + ((u_int)stamp /86400u)) % 7u);/*0x40��ʾ����һ������һ��*/
    //UTT_POLICY_DEBUG("%s %d curTime:%u curWeek:%02x\n", __func__, __LINE__, curTime->time, curTime->week);
    return;
}
/**
 * ʱ��Ƚ�
 * return : cuTime��ʱ��� timestart �� tiomestop�� true ����false
 */
extern  bool uttTimeIn(u_int curTime, u_int timeStart, u_int timeStop)
{
    bool ret = true;
    if(timeStart >timeStop)
    {/*����Ľ������ڶ���Ŀ�ʼ*/
	if ((curTime >  timeStop)  && (curTime < timeStart))
	{/*��ǰ�� ������ ��ʼ ��ƥ��*/
	    ret = false;
	}

    }
    else
    {/*��ǰ�Ŀ�ʼ������Ľ���*/
	if ((curTime <  timeStart) ||  
		(curTime > timeStop))
	{/*���ڴ�ʱ�������ƥ��*/
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

