#ifndef __UTT_MODULE_INIT_H__
#define __UTT_MODULE_INIT_H__

/**
 * 模块初始化是指：
 * 当新建立一个模块时，需要注册模块自身的构造和析构函数；当ip上线时，需要设置ip的hash节点配置的uttNfMatch
 * 每个模块对应uttNfMatch中的一位
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
    uttIpNodeOperFn  * init;/*构造函数*/
    uttIpNodeOperFn  * exit;/*析构函数*/
}UttIpNodeOps;

/*一般在功能开启时注册。如开启域名过滤*/
extern int uttRegIpNodeOps (UttIpNodeOps * ops);

/*一般在功能关闭时注销。如关闭域名过滤功能*/
extern int uttUnRegIpNodeOps (UttIpNodeOps *ops);

/*ip hash节点创建时，执行遍历函数*/
extern void uttTraverseIpNodeInit (UttNfHashBuf *ipHash);

/*ip hash节点删除时，执行遍历函数*/
extern void uttTraverseIpNodeExit (UttNfHashBuf *ipHash);

/*ip hash节点 list 初始化*/
extern void ipNodeInit(void);
#endif
