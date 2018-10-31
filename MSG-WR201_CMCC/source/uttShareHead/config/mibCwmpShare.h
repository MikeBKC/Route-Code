#ifndef MIBCWMP_SHARE_H
#define MIBCWMP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define CWMP_LEN 50

typedef struct st_cwmpProfile
{
    mibHead head;
    unsigned int type;
    char acsUrl[CWMP_LEN];

    unsigned int acsAuthEnable;
    char acsUsername[CWMP_LEN];
    char acsPassword[CWMP_LEN];

    unsigned int periodicInformInterval;

    unsigned int cpeAuthEnable;
    char cpeUsername[CWMP_LEN];
    char cpePassword[CWMP_LEN];

    int  stunEnable;
    char stunUrl[CWMP_LEN];
    unsigned int stunPeriodicInterval;
    unsigned int stunAuthEnable;
    char stunUsername[CWMP_LEN];
    char stunPassword[CWMP_LEN];

}CwmpProfile;

#endif
