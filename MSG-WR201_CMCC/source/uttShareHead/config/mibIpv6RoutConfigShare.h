#ifndef MIBIPV6ROUTCONFIG_SHARE_H
#define MIBIPV6ROUTCONFIG_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "typedefShare.h"
#if (IPV6_ROUTE == FYES)
typedef struct ipv6_st_routConf{
    mibHead head;
    char routeName[13];
    char desIP[40];
    int desNetPrefix;
    char GateWay[40];
    int priority;
    char port[5];
}Ipv6RoutConfProfile;
#endif
#endif
