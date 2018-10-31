#ifndef MIBDHCP6_SHARE_H
#define MIBDHCP6_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_DHCPV6 == FYES)
typedef struct ipv6_st_dhcp {
    mibHead head;
    int dhcpEnabled;   /*启用开关*/
    char sta_ipv6_addr[40];  /*起始IPv6地址*/
    char end_ipv6_addr[40];  /*结束IPv6地址*/
    int dhcpPrefixLength;    /*地址前缀长度*/
    char ipv6dhcpGateway[40]; /*网关地址*/
    unsigned long staleTime;  /*租用时间*/
    char ipv6DnsZ[40];        /*主DNS服务器*/
    char ipv6DnsB[40];        /*备DNS服务器*/
}Dhcp6Profile;
#endif
#endif
