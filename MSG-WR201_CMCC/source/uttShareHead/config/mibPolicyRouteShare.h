#ifndef MIBPOLICYROUTE_SHARE_H
#define MIBPOLICYROUTE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (POLICY_ROUTE == FYES)
#define PR_IPGRP_NAME_LEN 31 /*ip组成员名长度。此名称应该是 组名 pppoe用户名 以及 web认证用户名长度中的较长者*/


/* ip type */
typedef enum{
    PR_IP_RANGE,
    PR_IP_GROUP
}prIpGrpType;
/* ip 地址段 */
typedef struct st_prIpRange {
    u_long ipFrom;	/*起始ip*/
    u_long ipTo;	/*结束ip*/
} prIpRange;
/*ip组条目内容*/
typedef struct st_pripGrpVal{
    prIpRange range;/*ip段*/
    char name[PR_IPGRP_NAME_LEN + 1];/*名称 可能为组名或 用户名*/
} prIpGrpVal;

/*ip组条目*/
typedef struct st_ipgrpEntry{
    prIpGrpType entryType;/*类型*/
    prIpGrpVal  entryVal;/*值*/
} prIpGrpEntry;

/*策略路由基于IP地址*/
typedef struct st_prIpGrp {
	prIpGrpEntry srcIP;
	prIpGrpEntry dstIP;
} prIpGrp;

/*策略路由基于应用*/
typedef struct st_prApply {
    unsigned int protocol;//协议
    unsigned short dstFromPort; //目的起始端口
    unsigned short dstEndPort;	//目的结束端口
} prApply;

/*规则生效时间*/
typedef struct st_prTime{
    u_long wday;/*星期几*/
    char timestart_str[UTT_TIME_LEN];
    char timestop_str[UTT_TIME_LEN];
    u_long timestart;/*24*60*60.一天最多86400秒*/
    u_long timestop;
}prTime;
/* policy route value */
typedef struct st_policyRouteProfile {
    mibHead head;
    char interfaceName[UTT_INTERFACE_LEN + 1];
    int bindMark;			/* 绑定接口mark */
    prIpGrp ip;				/* 基于IP */
    prApply apply;			/* 基于应用 */
    prTime prtime;			/* 生效时间 */
} PolicyRouteProfile;
#endif
#endif
