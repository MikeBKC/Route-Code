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
#ifndef MIBMACFILTER_SHARE_H
#define MIBMACFILTER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define MAC_FILTER_CHAIN "mac_filter_input_chain"

/*
 * mac filter»´æ÷≈‰÷√
 */
typedef struct st_mac_filter_global_profile
{
    mibHead head;
    MacFilterActionEn action;
}MacFilterGlobalProfile;

typedef struct st_mac_filter_profile
{
    mibHead head;
    MacAddr mac;
}MacFilterProfile;
#endif
