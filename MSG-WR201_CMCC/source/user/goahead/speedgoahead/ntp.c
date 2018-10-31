#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"


#if (FEATURE_NTP == FYES)
extern void ntp_sh(void);

static void ntp_init(int reset);

static void ntpProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const NtpProfile *oldProfileDataPtr, const NtpProfile *newProfileDataPtr )
{
    printf("%s\n", __func__);
    switch(changeType) {
        case PROF_CHANGE_EDIT:
            ntp_init(1);
	    break;
        default:
	    break;
    };
}
static void ntp_init(int reset)
{
    MibProfileType mibType = MIB_PROF_NTP;
    NtpProfile *prof;

    prof = (NtpProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
	if (prof->enable==TRUE) {
	    ntp_sh();
	} else {
	    if(reset==1)
	    {
		doSystem("killall -q ntpclient");
		doSystem("date -s %s", prof->timeSetting);
	    }
	}
    }
}
void funInitNtp(void)
{
    registerForProfileUpdates(MIB_PROF_NTP, NULL, (ProfileUpdateFnc)ntpProfileChange);
#if UTT_NV_WX20S
    ntp_init(1);
#endif
}
#endif
