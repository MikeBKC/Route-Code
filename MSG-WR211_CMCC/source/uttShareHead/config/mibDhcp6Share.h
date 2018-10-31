#ifndef MIBDHCP6_SHARE_H
#define MIBDHCP6_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_DHCPV6 == FYES)
typedef struct ipv6_st_dhcp {
    mibHead head;
    int dhcpEnabled;   /*���ÿ���*/
    char sta_ipv6_addr[40];  /*��ʼIPv6��ַ*/
    char end_ipv6_addr[40];  /*����IPv6��ַ*/
    int dhcpPrefixLength;    /*��ַǰ׺����*/
    char ipv6dhcpGateway[40]; /*���ص�ַ*/
    unsigned long staleTime;  /*����ʱ��*/
    char ipv6DnsZ[40];        /*��DNS������*/
    char ipv6DnsB[40];        /*��DNS������*/
}Dhcp6Profile;
#endif
#endif
