#ifndef __UTT_EFFECTIVE_TIME_H__
#define __UTT_EFFECTIVE_TIME_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/time_set.h>

/*������Чʱ��*/
typedef struct st_uttEffectiveTime{
    u_long wday;/*���ڼ�*/
    u_long timestart;/*24*60*60.һ�����86400��*/
    u_long timestop;
} UttEffectiveTime;

/*��Чʱ�乳�ӽṹ�嶨������*/
typedef struct{
    struct list_head list; /*������Чʱ����ص�ȫ����Чʱ������Ĺ���*/
    rwlock_t lock;		
    atomic_t ref;	    /* �������ü��� */
    atomic_t onTimeRef;     /* ��ʾ��ʱ��hook��ʵ�����õļ���������0����˵�����ܽ���hook��ʱ��仯������ж��*/
    bool isOnTimeChangeList;/*��־��ʱ�������Ƿ���ص���ʱ��仯������*/
    UttEffectiveTime effectiveTime;      /*��Чʱ��*/
    struct list_head askerHookHead;/*������Чʱ��Ĺ���ģ�鹳������.��ʱ���鱣��һ��*/
    struct list_head delayExecList;/*���ظ�ʱ�����ϴ�����ִ�е��û���������*/
    uint16_t enable;/*��ʾ��ʱ���Ƿ���Ч*/
} UttTimeHookOps;

/*����effectiveTime����timeHookOpsָ��*/
extern UttTimeHookOps *uttGetTimeHookOpsRef (UttEffectiveTime *effectiveTime);

/*����UttTimeHookOpsָ��������ͷ�*/
extern void uttPutTimeHookOpsRef (UttTimeHookOps *timeHookOps); 

/*��Ӧ��ģ��������ӵ�timeHook��Чʱ����*/
extern bool uttRegToEffTimeHook (UttTimeHookOps *timeHook, void *conf, uttTimeSetAskerFn *fn);

/*���û�ģ�����Ӧ����Чʱ��������ɾ��*/
extern int uttUnRegFromTimeHook (UttTimeHookOps *timeHook , void *conf);
#endif
