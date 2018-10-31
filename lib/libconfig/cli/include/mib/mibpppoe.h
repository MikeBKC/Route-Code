/*
 * $Id: mibpppoe.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *      Copyright (c) 1997 UTT Technologies, Inc.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of UTT Technologies, Inc.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise duplicated
 *      and the information herein may not be used, disseminated or
 *      otherwise disclosed, except with the prior written consent of
 *      UTT Technologies, Inc.
 */

#if ! defined( MIBINPPPOE_H )
# define MIBINPPPOE_H

# include "mibglob.h"

    /*
     * How to build the factory instance of this profile.
     */
extern ConstPtr
mibFactoryPppoeOptions( MibProfileType	mibType,
			 Uint32		info );

    /*
     * Our parameter table is made external as the profile is used
     * within other profiles.
     */
extern const MibFieldComplexParams	pppoeOptionsParams;


    /*
     * PppoeProfType:
     *
     * PPPOE profile type
     *
     *  PPPOE_PROF_DISABLED This profile does not describe a pppoe
     *
     *  PPPOE_PROF_CLIENT This profile is a client
     *
     *  PPPOE_PROF_SERVER This profile is a server
     *                                  
     */
typedef enum PppoeProfType {
    PPPOE_PROF_DISABLED = 0,
    PPPOE_PROF_CLIENT,
    PPPOE_PROF_SERVER
} PppoeProfType;


static __inline__ Bool
validPppoeProfType (int x)
{
    switch (x) {
    case PPPOE_PROF_DISABLED:
    case PPPOE_PROF_CLIENT:
    case PPPOE_PROF_SERVER:
        return TRUE;
    default:
        return FALSE;
    }
}


    /*
     * PppoeOptions:
     *
     * Pppoe configuration options
     *
     *  pppoeProfType The type of connection this profile describes
     *
     *  serverName The name of the AC server
     *
     *  serviceName The name of the service by force 
     *
     */


typedef struct pppoeOptions {
    PppoeProfType                      pppoeProfType;
    char                               pppoeServer[ MAX_HOST_NAME_LENGTH + 1 ];
    char                               pppoeService[ MAX_HOST_NAME_LENGTH + 1 ];
}
PppoeOptions;

#endif /* MIBINPPPOE_H */

