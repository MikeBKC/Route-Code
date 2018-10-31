#ifndef MIBALIFILTER_SHARE_H
#define MIBALIFILTER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define MAX_ALI_ACC_LEN 25 
#define	MAX_ALI_REMARK_LEN  30
typedef struct st_ExceptAli_Profile {
    mibHead head;
    char    aliNumber[MAX_ALI_ACC_LEN+1];
    char    remark[MAX_ALI_REMARK_LEN+1];
} ExceptAliProfile;
#endif
