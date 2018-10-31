#ifndef STATICROUTE_H
#define STATICROUTE_H

/*
 * 路由配置绑定接口的lan接口前缀和wan接口前缀
 */
#define RT_LAN_PRE "LAN"
#define RT_WAN_PRE "WAN"
#define RT_PPTP_PRE "(PPTP"
#define RT_PPTP_SERVER_PRE "(PPTPS)"
#define RT_PPTP_CLIENT_PRE "(PPTPC)"

//#define DBG_SR
#ifdef DBG_SR
#define SR_PRINTF(fmt, args...)		printf(fmt, ## args)
#else 
#define SR_PRINTF(fmt, args...)
#endif

/*
 * 命令长度，像replace, rule, add, delete等
 */
#define RT_ACTION_LEN	10

#define RT_ACTION_ADD	"add"
#define RT_ACTION_REP	"replace"
#define RT_ACTION_DEL	"delete"

#define RT_CMD_LEN  128
#define RT_TMP_LEN  32
#define RT_IP_LEN   20
#define RT_PDB_IP_LEN	8 /* 描述ip的路由策略长度 */
#define RT_PDB_MASK_LEN 2 /* 描述掩码的路由策略长度 */
#define RT_PDB_LEN (RT_PDB_IP_LEN + RT_PDB_MASK_LEN) /* 一条pdb路由策略的长度 */
#define RT_FILE_LINE_LEN    127	/* read_file_line能读取的最多字符 */

#if 0
extern void AddUserRoute(int wanNo);
extern void FlushUserRoute(int wanNo);
#else
/* for mantis 8175 */
extern void FlushUserRoute(void);
extern void TIRouteChg(void);
#endif
extern void ModifyDefaultRoute(int wanNo, int action);
extern void delOneWanDefaultRoute(int wanIndex);
extern void addOneWanDefaultRoute(int wanIndex);
extern void ISPRouteTableFlush(int wanIndex);
extern void ISPRouteTableAdd(int wanIndex);
extern void delet_dns_route(int wanNo);
extern void add_dns_route(int wanNo);
#if 0
extern void LinkChkChg(int wanNo);
#endif
extern void wanIpRuleFun(int wanIndex, int action, IPADDR ipS, IPADDR ipE);
extern void _modifyLanIpRuleAndRoute(int action, Uint32 delIp);
extern int isWanInUse(int wanNo);

#endif
