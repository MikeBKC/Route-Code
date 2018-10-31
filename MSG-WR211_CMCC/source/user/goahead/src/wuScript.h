#ifndef GOAHEAD_WUSCRIPT_H
#define GOAHEAD_WUSCRIPT_H

/*
 * 哪个pool类型
 * IP_POOL_DHCP:    dhcp server pool类型
 * IP_POOL_PPPOE:   pppoe server pool类型
 * IP_POOL_L2TP:    l2tp server pool类型
 * IP_POOL_PPTP:    pptp server pool类型
 * IP_POOL_NONE:    哪个pool类型也不是
 */
typedef enum {
    IP_POOL_NONE,
    IP_POOL_DHCP,
    IP_POOL_PPPOE,
    IP_POOL_L2TP,
    IP_POOL_PPTP
} IpPoolEnum;

#if (MULTI_SSID_DHCP_POOL == FYES)
typedef enum {
    IP_POOL_DHCP_WIRED,
    IP_POOL_DHCP_SSID1,
    IP_POOL_DHCP_SSID2,
    IP_POOL_DHCP_SSID1_5G,
    IP_POOL_DHCP_SSID2_5G,
} IpPoolType;
#endif
extern IpPoolEnum ipPoolConflict(IpPoolEnum poolType, Uint32 ipS, Uint32 ipE);
extern void setPoolConflictMsg(IpPoolEnum poolType);
extern int isOverlapInnerIp(Uint32 ip, Uint32 nm);

#endif
