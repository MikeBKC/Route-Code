#ifndef MIBQQFILTER_SHARE_H
#define MIBQQFILTER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define NORMAL_QQ  1
#define ENTERPRISE_QQ 2
#define	MAX_QQ_REMARK_LEN  30
typedef struct st_ExceptQQ_Profile {
    mibHead head;
    Uint32	    qqNumber;
    Uint32	    phoneHead;
    char    remark[MAX_QQ_REMARK_LEN+1];
} ExceptQQProfile;
#endif
