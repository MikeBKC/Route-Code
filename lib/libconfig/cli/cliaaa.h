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
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

#if ! defined( CLIAAA_H )                  /* prevent nested inclusions */
# define CLIAAA_H                          /* module name */

extern void cmdAaa(TermInfo *term);

#if ( IPSEC == FYES )
extern void cmdIpsec(TermInfo *term);
#endif

#if ( FRAME_RELAY == FYES )
extern FrameRelayProfile *frProf;
extern void cmdFR(TermInfo *term);
extern void cmdConnEncapsFR(TermInfo *term);
#endif

#if ( X25B == FYES )
extern X25Profile *x25Prof;
extern void cmdX25(TermInfo *term);
extern void cmdConnEncapsIpoX25(TermInfo *term);
#endif

#endif /* CLIAAA_H */
