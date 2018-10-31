#ifndef MIB_ADFILTER_H_
#define MIB_ADFILTER_H_
#include "mibAdfilterShare.h"

#define PROXY_PORT 12345 /*代理服务器监听的端口号*/
#define DNS_REDIRECT_IP "2.2.2.2" /*取相对比较特殊的IP，以免跟网络中其他IP地址冲突*/

/*表示当前最大规则条目数，daughter.c中adfilter_tuple数组每添加一条规则，该宏就+1，当超过CONFIG_LEN时，就得修改CONFIG_LEN宏*/
#define MAX_ADFILTER_ITEMS   7

#endif
