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


#if ! defined ( _MIBISP_H )

# define _MIBISP_H

typedef enum tIspType{
        //ispMin,
	ispPPPOE,
	ispFix=1,
	ispDyn,

        ispMax
}IspType;

typedef struct st_isps{
	char name[MAX_PROFILE_NAME_LENGTH+1];
	Boolean active;
	IspType type;
	char connName[MAX_PROFILE_NAME_LENGTH+1];
	char profile[MAX_PROFILE_NAME_LENGTH+1];
	char defRtName[MAX_PROFILE_NAME_LENGTH+1];                       
	char detRtName[MAX_PROFILE_NAME_LENGTH+1]; 
	char natBindName[MAX_PROFILE_NAME_LENGTH+1];
	char carrier[MAX_PROFILE_NAME_LENGTH+1];
	int natrefcnt;
	int isptx;
	int isprx;
	Uint32 inoctets;
	Uint32 outoctets;
} IspProfile;
extern IspProfile *ispProf;

#endif /* _MIBISP_H */

