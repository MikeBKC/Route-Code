#ifndef IPSET_API_H
#define IPSET_API_H
#include <linux/autoconf.h>

#if CONFIG_IP_NF_SET

#define    IPSET_SYS_GNAME_PRE "sys_" /*系统添加用户名前缀。该用户名前缀普通用户无法使用*/
#define    IPSET_TSYS_GNAME_PRE "t"IPSET_SYS_GNAME_PRE /*系统临时组名前缀。任何组不得使用*/
#define    IPSET_MEM_ADD 'A'
#define    IPSET_MEM_DEL 'D'
#define	   MAX_LINE_LENGTH 50  /*一行最大长度*/


#if (NOTICE == FYES)
extern int noticeOkGrpAdd(char* groupName);
extern int noticeWebGrpAdd(char* groupName);
extern int webauthOkGrpAdd(char* groupName);
extern int webauthWebGrpAdd(char* groupName);
extern int noticeRGrpAdd(char* groupName, char *ipFrom, char *ipTo);
extern int noticeRGrpEdit(char* groupName, char *ipFrom, char *ipTo);
#endif
#if (PPPOE_ACCOUNT == FYES)
extern int poeAccGrpAdd(char* groupName);
#endif

#if (IP_GRP == FYES)
extern int ipGrpIpAddSet(char* groupName);/*ip类型地址组*/
extern int ipGrpUserAddSet(char* groupName);/*账号类型地址组*/
extern int ipsetGetRefer(char* groupName);/*获取组被规则引用的次数*/
#endif


extern int ipsetGrpNameIsSys(char *groupName);
extern int ipsetFlushGrp(char* groupName);
extern int ipsetDelGrp(char* groupName, bool notWarn);

extern int ipsetAOrDIp(char* groupName, IPADDR ip, char addOrDel) ;
extern int ipsetAOrDIpStr(char* groupName, char* ipStr, char addOrDel);
extern int ipsetAOrDIpRange(char* groupName, IPADDR ipFrom, IPADDR ipTo, char addOrDel);
extern int ipsetAOrDIpStrRange(char* groupName, char* ipFromStr, char* ipToStr, char addOrDel);
#if 1
extern int ipsetAOrDNet(char* groupName, IPADDR ip, Uint32 cidr, char addOrDel);
extern int ipsetAOrDNetStr(char* groupName, char* ipStr, Uint32 cidr, char addOrDel) ;
#endif
extern int ipsetTestIpStr(char* groupName, char* ipStr);
extern int ipsetGetRefer(char* groupName);
extern int httpPortGrpAdd(char* groupName);
extern int httpPortAdd(char* groupName,Uint32 port);
#endif
#endif
