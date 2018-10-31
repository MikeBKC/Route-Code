/*
 *
 *	Copyright (c) 1993 UTT Technologies, Inc.
 *	All rights reserved.
 *	Use of copyright notice does not imply publication.
 *
 *
 *			CONFIDENTIAL INFORMATION
 *			------------------------
 *	This Document contains Confidential Information or Trade Secrets,
 *	or both, which are the property of UTT Technologies, Inc.
 *	This document may not be copied, reproduced, reduced to any
 *	electronic medium or machine readable form or otherwise duplicated
 *	and the information herein may not be used, disseminated or
 *	otherwise disclosed, except with the prior written consent of
 *	UTT Technologies, Inc.
 */

/*
 * A "profile" is a configuration.  Some profiles are generic to the
 * Multiband product family, while others are specific to a particular
 * hardware platform.  To simplify profile selection in all other modules
 * they need only include this module.  It pulls in all necessary profile
 * related files in the proper sequence.  Profiles for new hardware platform
 * must be added here (and *only* here)
 */
#if ! defined( PROFILES_H )
# define PROFILES_H

    /*
     * All platforms & netifs use DSLs, so all will need
     * to know what a DSL is...
     */

    /*
     * Pull in the definition of MAX_DSL and B_CHANNELS_PER_DSL
     */

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang*/
#include "mibipgroup.h"
#endif
#endif

    /*
     * Some profiles need to know about slot and/or port numbers.  Pulling in
     * routmgr does the trick.  Version is also needed
     */
    /*
     * lanprof.h needs to know about AbsoluteTime for some systems
     */
//# if ( (FIREWALLS == FYES) && ( INTERFACE_MGR == FYES ) )
//# endif

# include "mibglob.h"

#endif /* PROFILES_H */

