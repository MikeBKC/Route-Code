#ifndef __UTT_MODULE_INIT_H__
#define __UTT_MODULE_INIT_H__

/**
 * ģ���ʼ����ָ��
 * ���½���һ��ģ��ʱ����Ҫע��ģ������Ĺ����������������ip����ʱ����Ҫ����ip��hash�ڵ����õ�uttNfMatch
 * ÿ��ģ���ӦuttNfMatch�е�һλ
 *
 */
#include <linux/types.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/rcupdate.h>

typedef void uttIpNodeOperFn(UttNfHashBuf *ipHash);

typedef struct{ 
    struct list_head list;
    struct rcu_head rcu;
    uttIpNodeOperFn  * init;/*���캯��*/
    uttIpNodeOperFn  * exit;/*��������*/
}UttIpNodeOps;

/*һ���ڹ��ܿ���ʱע�ᡣ�翪����������*/
extern int uttRegIpNodeOps (UttIpNodeOps * ops);

/*һ���ڹ��ܹر�ʱע������ر��������˹���*/
extern int uttUnRegIpNodeOps (UttIpNodeOps *ops);

/*ip hash�ڵ㴴��ʱ��ִ�б�������*/
extern void uttTraverseIpNodeInit (UttNfHashBuf *ipHash);

/*ip hash�ڵ�ɾ��ʱ��ִ�б�������*/
extern void uttTraverseIpNodeExit (UttNfHashBuf *ipHash);

/*ip hash�ڵ� list ��ʼ��*/
extern void ipNodeInit(void);
#endif
