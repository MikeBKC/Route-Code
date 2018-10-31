#ifndef MIBPREFIXLIST_SHARE_H
#define MIBPREFIXLIST_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "typedefShare.h"

#if(IPV6_NOTLISTPREFIXS == FYES)
typedef struct ipv6_st_ipv6PrefixList{
mibHead head;
//    int AdvAutoEnabled;
    char DesPrefix[40];   /*通告前缀*/
    int DesPrefixLength;  /*通告前缀长度*/
    long PrefixstaleTime;  /*前缀生命周期*/
}Ipv6PrefixListProfile;
#endif
#endif
