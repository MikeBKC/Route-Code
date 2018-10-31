#ifndef MIBDNSFILTER_SHARE_H
#define MIBDNSFILTER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define	UTT_DOMAINNAME_LEN  50
#define DROP_CHAIN "DROP"
#define RETURN_CHAIN "RETURN"
typedef struct st_DnsFilter_Profile {
    mibHead head;
    char    domainName[UTT_DOMAINNAME_LEN];
} DnsFilterProfile;
#endif
