/*
 *	internet.h -- Internet Configuration Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.h,v 1.3 2014/06/26 19:25:17 hou.bo Exp $
 */
#define ARP_VALID_MIN_LEN 19 /*合法arp条目的最小长度*/
void formDefineInternet(void);
int getIfLive(char *ifname);
int getIfMac(char *ifname, char *if_hw);
int getIfIp(char *ifname, char *if_addr);
int getIfIsUp(char *ifname);
int getIfNetmask(char *ifname, char *if_net);
char* getWanIfNamePPP(int);
#if 0
char* getWanIfName(int);
char* getLanIfName(void);
#endif
char *getLanWanNamebyIf(char *ifname);
int initInternet(void);
void initUpnpServer();
char * utt_strtoup(char *str);
