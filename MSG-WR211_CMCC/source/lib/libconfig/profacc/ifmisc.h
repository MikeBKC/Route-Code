
#ifndef __IFMISC_H
#define __IFMISC_H
#define CROSS_SPLIT_SIGN_STR "-"
#define COLON_SPLIT_SIGN_STR ":"
extern int getWanSlotCount(void);
extern char* getLanIfName(void);
extern char* getWanIfName(int num);
extern int StrTransform(char *StrTrans,char *OldSplit,char *NewSplit);
extern int getIfIp(char *ifname, char *if_addr);
extern int getIfMac(char *ifname, char *if_hw);
extern int getIfNetmask(char *ifname, char *if_net);
extern int get_lan_status(int lanIndex);
extern int get_phylink_status(int wanIndex);
extern char* getWanIfNamePPP(int num);
extern int _getWanLinkState(int wanNo);
extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern char* converMac6To17Byte(MacAddr macAddr);
extern int macAddrCmp(MacAddr* mac1, MacAddr* mac2);
extern IPADDR convertLenToNetMask(Uint32 netMaskLen);

extern int isIpStrInIfNet(char *ipStr, char* ifName);
extern int isIpInIfNet(IPADDR ip,char *ifName);

#if (UTT_NV_1800N == FYES)
extern Uint8* getApIfName();
extern Uint8* getEthLanIfName();
#endif

extern Uint32 convertLenToBinMask(Uint32 maskLen);
#endif
