#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/netfilter_ipv4/utt_module_init.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/rcupdate.h>

#if 0
#define UTT_MODULE_INIT_DEBUG(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_MODULE_INIT_DEBUG(fmt, argx...)
#endif

/*hash表节点创建和注销的时候会执行该链表上的操作*/
static LIST_HEAD(uttNfIpNodeOperList);
static DEFINE_SPINLOCK(uttIpNodeHookSpinLock);

void uttIpNodeHookLock(void)
{
    spin_lock_bh(&uttIpNodeHookSpinLock);
}

void uttIpNodeHookUnlock(void)
{
    spin_unlock_bh(&uttIpNodeHookSpinLock);
}

/*一般在功能开启时注册。如开启域名过滤*/
int uttRegIpNodeOps (UttIpNodeOps * ops)
{
    UttIpNodeOps *pos = NULL;
    int flag = 0;

    if (ops == NULL) {
	return 1;
    }
    /**
     *对于已经存在的hash节点，先进行一次遍历
     */
    uttNfHashTraverse(ops->init);

    UTT_MODULE_INIT_DEBUG("-------file:%s,func:%s,line:%d----------\n",__FILE__,__func__,__LINE__);
    uttIpNodeHookLock();
    if (!list_empty(&uttNfIpNodeOperList)) {
	UTT_MODULE_INIT_DEBUG("------file:%s,func:%s,line:%d- list not empty------\n",__FILE__,__func__,__LINE__);
	
	list_for_each_entry_rcu(pos,&uttNfIpNodeOperList,list){
	    UTT_MODULE_INIT_DEBUG("pos = %p, ops = %p; &pos->list = %p, &ops->list = %p\n",pos,ops,&pos->list,&ops->list);
	    if (pos == ops) {
		flag = 1;
		break;
	    }
	}
    }
    if (flag != 1) {
	list_add_rcu(&ops->list,&uttNfIpNodeOperList);
    }
    UTT_MODULE_INIT_DEBUG("-------file:%s,func:%s,line:%d----------\n",__FILE__,__func__,__LINE__);
    uttIpNodeHookUnlock();

    return 0;
}
EXPORT_SYMBOL(uttRegIpNodeOps);

void uttIpNodeHookOpsFree(struct rcu_head *rh)
{
    UTT_MODULE_INIT_DEBUG("===file:%s,func:%s,line:%d======\n",__FILE__,__func__,__LINE__);
}
EXPORT_SYMBOL(uttIpNodeHookOpsFree);

/*一般在功能关闭时注销。如关闭域名过滤功能*/
int uttUnRegIpNodeOps (UttIpNodeOps *ops)
{
    UttIpNodeOps *pos;

    if (ops == NULL) {
	return 1;
    }
   /**
    * 遍历hash表执行下ops->exit函数
    */
    uttNfHashTraverse(ops->exit);
    UTT_MODULE_INIT_DEBUG("-------file:%s,func:%s,line:%d----------\n",__FILE__,__func__,__LINE__);
    uttIpNodeHookLock();
    if (list_empty_careful(&uttNfIpNodeOperList)) {
	uttIpNodeHookUnlock();
	return 0;
    }

    list_for_each_entry_rcu(pos,&uttNfIpNodeOperList,list){
	if (pos == ops) {
	    list_del_rcu(&ops->list);
	    break;
	}
    }
    uttIpNodeHookUnlock();

    call_rcu_bh(&ops->rcu,uttIpNodeHookOpsFree);
    return 0;
}
EXPORT_SYMBOL(uttUnRegIpNodeOps);

void uttTraverseIpNodeInit (UttNfHashBuf *ipHash)
{
    UttIpNodeOps *pos;

    UTT_MODULE_INIT_DEBUG("----file:%s,func:%s,line:%d----\n",__FILE__,__func__,__LINE__);
    rcu_read_lock_bh();
    if (!list_empty(&uttNfIpNodeOperList)) {
	list_for_each_entry_rcu(pos,&uttNfIpNodeOperList,list){
	    UTT_MODULE_INIT_DEBUG("----file:%s,func:%s,line:%d-ipHash->ip=%lx---\n",__FILE__,__func__,__LINE__,ipHash->ip);
	    pos->init(ipHash);
	}
    }
    rcu_read_unlock_bh();
}
EXPORT_SYMBOL(uttTraverseIpNodeInit);

void uttTraverseIpNodeExit (UttNfHashBuf *ipHash)
{
    UttIpNodeOps *pos;

    UTT_MODULE_INIT_DEBUG("----file:%s,func:%s,line:%d----\n",__FILE__,__func__,__LINE__);
    rcu_read_lock_bh();
    if (!list_empty(&uttNfIpNodeOperList)) {
	list_for_each_entry_rcu(pos,&uttNfIpNodeOperList,list){
	    pos->exit(ipHash);
	}
    }
    rcu_read_unlock_bh();
}
EXPORT_SYMBOL(uttTraverseIpNodeExit);
