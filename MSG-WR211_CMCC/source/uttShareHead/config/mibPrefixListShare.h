#ifndef MIBPREFIXLIST_SHARE_H
#define MIBPREFIXLIST_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "typedefShare.h"

#if(IPV6_NOTLISTPREFIXS == FYES)
typedef struct ipv6_st_ipv6PrefixList{
mibHead head;
//    int AdvAutoEnabled;
    char DesPrefix[40];   /*ͨ��ǰ׺*/
    int DesPrefixLength;  /*ͨ��ǰ׺����*/
    long PrefixstaleTime;  /*ǰ׺��������*/
}Ipv6PrefixListProfile;
#endif
#endif
