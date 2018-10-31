#ifndef SPD_COMMON_FUN_H
#define SPD_COMMON_FUN_H

#if(UTT_DOMAINACCESS ==FYES)
extern int isWanLinkStatus;
#endif
extern char* getIpRangeForIptables(IPADDR fromIp, IPADDR endIp, bool isSrc);
extern char* getIpRangeForIptables_opposite(IPADDR fromIp, IPADDR endIp, bool isSrc);
extern char* getTimeForIptables(char* timeStart, char* timeEnd);
extern char* getDayForIptables(Uint32 day);
extern char* getDateForIptables(char* dateStart, char* dateEnd);

extern char* getTimeForCrond(Uchar* time);
extern char* getWeekForCrond(Uint32 week_bit);
extern Uint32 converTimeToSec(char *time);
#if (PPPOE_SERVER == FYES)
extern int AccAddIpStr(PPPoESrvAccProfile* prof, char* grpName, char addOrDel);
#endif
#if (EASY_TASK == FYES)
extern int editLANRedirectRule(char action, char *lanIp);
#endif
#if (UTT_SMART_UI == FYES)
extern int isHotelPnpEn(void);
extern int editDNSRedirectRule(void);
#endif
#endif
