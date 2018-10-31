    /*
     */
/*
 *
 *      Copyright (c) 1996 UTT Technologies, Inc.
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

    /*
     * This file contains the internal representation of the BOOTP RELAY
     * profile (MIB_PROF_BOOTP_RLAY) for TNT.
     *
     */


#if (( ! defined( MIBBOOTP_H ) ) && ( MDF_BOOTP_RELAY == FYES ))
#define MIBBOOTP_H

    /*
     * How to make a default version of an BOOTP RELAY profile.
     */
extern ConstPtr
mibFactoryBootpRelay( MibProfileType	mibType,
		       Uint32		info );


    /*
     * Our parameter table is made external as the profile is used
     * within other profiles.
     */
extern const MibFieldComplexParams	mibBootpRelayParams;


    /*
     * BootpRelay:
     *
     * The BOOTP_RELAY profile. This defines the user tweekable state.
     *
     *  enabled Is BOOTP RELAY agent active.
     *
     *  servers List of server addresses where BOOTP packets will be forwarded
     *                                  to.
     */
typedef struct mibBootpRelay {
#if ( PLATFORM & TNT )
    Bool                                enabled;
#endif /* ( PLATFORM & TNT ) */
#if ( PLATFORM & TNT )
    IpAddress                           servers[ MAX_BOOTP_SERVERS ];
#endif /* ( PLATFORM & TNT ) */
}
#if ( USERIF & CLI )
__PACK__
#endif
BootpRelay;

#endif /* (( ! defined( MIBBOOTP_H ) ) && ( MDF_BOOTP_RELAY == FYES )) */


