#ifndef _NTP_SHARE_H_
#define _NTP_SHARE_H_

#include "uttMachine.h"
#define NTP_DEFAULT_TZ       28800
#define NTP_DEFAULT_SERV1    "202.108.6.95"
#define NTP_DEFAULT_SERV2    "24.56.178.140" 

typedef struct _ntp_profile{
    mibHead head;
#if (DST == FYES)
    int dstEnable;
#endif
    int enable;
    int time_zone;
    int NTPServerIP1;
    int NTPServerIP2;
    int NTPServerIP3;
    char timeSetting[32];
}NtpProfile;


#endif
