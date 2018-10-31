#ifndef MIB_KNOWIFI_DOMAIN_SHARE_H
#define MIB_KNOWIFI_DOMAIN_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#if (UTT_KNOWIFI == FYES)
#define KNOWIFI_REDIRECT_CLIENT "knowifi_redirect_client"

typedef struct st_knowifiDomain{
    mibHead head;
    char deviceName[128];
    char username[128];
    char ip[16];
    char mac[18];
    int status;
    int conntype;
    char domain[128];
    char token[128];
    unsigned long tokenOverTime;
    Uint32 mode;
    int isFree;
    unsigned long timeStart;
    unsigned long timeEnd;
    unsigned int weekdays;
}KnowifiDomainProfile;

#endif
#endif
