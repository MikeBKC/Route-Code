/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
#if ! defined ( _MIBFIREWALL_H )

# define _MIBFIREWALL_H

#define MAX_FW_ADDRESS_PROFILES 100
#define MAX_FW_ADDRESS_AREA_LENGTH 128

#define MAX_FW_SERVICE_PROFILES 100
#define MAX_FW_SERVICE_LENGTH 128

#define MAX_FW_L7FILTER_PROFILES 20
#define MAX_FW_L7FILTER_LINE_COUNT 8
#define MAX_FW_L7FILTER_LINE_LENGTH 32

typedef struct st_fwAddress_profile {
	char name[MAX_PROFILE_NAME_LENGTH+1];
	Boolean active;
	char profile[MAX_PROFILE_NAME_LENGTH + 1];
	char area[MAX_FW_ADDRESS_AREA_LENGTH+1];
} FwAddressProfile;
extern FwAddressProfile *fwAddressProf;


typedef struct st_fwService_profile {
        char name[MAX_PROFILE_NAME_LENGTH+1];
        Boolean active;
        char service[MAX_FW_SERVICE_LENGTH+1];
} FwServiceProfile;
extern FwServiceProfile *fwServiceProf;

typedef enum tL7filterType{
	url_alg=0,
	str_alg
}L7filterType;

typedef struct st_fwL7filter_profile {
        char name[MAX_PROFILE_NAME_LENGTH+1];
        Boolean active;
	L7filterType type;
        char content[MAX_FW_L7FILTER_LINE_COUNT][MAX_FW_L7FILTER_LINE_LENGTH];
} FwL7filterProfile;
extern FwL7filterProfile *fwL7filterProf;

#endif /* _MIBFIREWALL_H */
