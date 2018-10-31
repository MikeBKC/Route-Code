#ifndef COM_EXE_FUN_H
#define COM_EXE_FUN_H
#include <uttRtChipFun.h>
#if ((PPPOE_ACCOUNT == FYES) || (WEBAUTH_ACCOUNT == FYES))
#define DELIVERY_START_TIME 1293840000
#define DELIVERY_END_TIME 1293847200
#if (PPPOE_ACCOUNT == FYES)
#if 0
extern void upPppAccRule(PPPoESrvAccProfile *prof, emPppoeAccStatus status, int isAdd);
#else
extern void upPppAccRule(SessionList prof, emPppoeAccStatus status, int isAdd);
#endif
extern int uttGlTime;
extern void upPppAccStatus(PPPoESrvAccProfile *prof);
#if 0
void upPppoeAcc(PPPoESrvAccProfile *prof, SessionList Session);
#endif
int pppoeSesCheck(PPPoESrvAccProfile* prof);
#endif
#endif
#if (IP_GRP == FYES)
void userUpToIpGrp(char* username,char *ip, ipGrpEntryType userType, char addOrDel);
int ipGrpSearchUser(int grpProfIndex, char*userName, ipGrpEntryType userType, int*searched);
int ipGrpDepToSub(IpGrpProfile *rootProf, char* subName);
int ipGrpDepSearch(IpGrpProfile *rootProf, char *subName);

#if (FIREWALL == FYES)
extern int isIpGrpReredByFireWall(IpGrpProfile *ipProf);
#endif
#if (GROUP_BASE == FYES)
extern int isIpGrpReredByNf(IpGrpProfile *ipProf);
#endif
#if (WEB_AUTH == FYES)
extern int isIpGrpReredByWebAuth(IpGrpProfile *ipProf);
extern void webauthDelexceptIpGroup(WebAuthGlobalProfile *prof);
extern void IPGrpDelWebExIp(char* grpName);
#if (WEB_AUTH_SHARE == FYES)
extern int checkSessionIndex(int sessionIndex);
#endif
extern int webAuthLog(WebAuthProfile *prof,Uint32 ip, char *mac);
#if (WEBAUTH_AUTO == FYES)
int getSmsServerIp(char *ip);
#endif
#endif
#if (P2P_LIMIT == FYES)
extern int isIpGrpReredByP2PLimit(IpGrpProfile *ipProf);
#endif
#endif
#if (PPPOE_SERVER == FYES)
extern int SesIndexIsOk(int i);
#endif

#if (ARP_BINDS == FYES)
extern ARP_MATCH matchFromArpBinds(IPADDR ipAddr, MacAddr macAddr, int* arpIndex);
#endif
#if (DHCP_AUTO_BIND == FYES)
extern int DhcpArpBindConfig(IPADDR ip, MacAddr macAddr,char* userName);
#endif
#if (WEBAUTH_ACCOUNT == FYES)
extern char* getMacByIpIoctl(struct arpreq *parp);
#endif
#if (EASY_TASK == FYES)
extern int wanConnStatsChange(int status);
extern int getValueByProfSys(int num);
#endif
#endif
