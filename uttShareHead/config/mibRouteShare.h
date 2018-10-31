#ifndef MIBROUTE_SHARE_H
#define MIBROUTE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*
 * 静态路由配置
 * head		    -   用户名，启用
 * dstAddr	    -	目的网络
 * netmask	    -	子网掩码
 * gwAddr	    -	网关
 * preference	    -   优先级
 * interfaceName    -	路由绑定的接口名称
 */
typedef struct st_staticRouteProfile {
    mibHead		head;
    IPADDR		dstAddr;
    IPADDR		netmask;
    IPADDR		gwAddr;
    Uint32		preference;
    char		interfaceName[UTT_INTERFACE_LEN + 1];
} StaticRouteProfile;

#endif
