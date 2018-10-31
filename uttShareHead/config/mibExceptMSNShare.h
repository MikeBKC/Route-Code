#ifndef MIBMSNFILTER_SHARE_H
#define MIBMSNFILTER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define MAX_MSN_LEN 30
#define	MAX_MSN_REMARK_LEN  30
typedef struct st_ExceptMSN_Profile {
    mibHead head;
    char    msnNumber[MAX_MSN_LEN+1];
    char    remark[MAX_MSN_REMARK_LEN+1];
} ExceptMSNProfile;
#endif
