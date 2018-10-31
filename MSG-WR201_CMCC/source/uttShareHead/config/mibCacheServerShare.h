#ifndef MIB_CACHE_SERVER_SHARE_H
#define MIB_CACHE_SERVER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"


#define MAX_DOMAIN_NAME_LEN 71
typedef struct st_CacheServerProfile
{
    mibHead head;
	FunEnableEnum cacheServerEnable;
    char    cacheServerIp[MAX_DOMAIN_NAME_LEN + 1];
    Uint32  cacheServerPort;
} CacheServerProfile;
#endif
