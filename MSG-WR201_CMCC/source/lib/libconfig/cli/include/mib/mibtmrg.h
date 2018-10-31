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

/*
 * SCCS ID:	$Id: mibtmrg.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#if ! defined ( _MIBTMRG_H )

# define _MIBTMRG_H

#define MAX_PERIOD 8

typedef enum tWinType{
	tmNone,
	tmEveryday=1,
	tmMonday,
	tmTuesday,
	tmWednesday,
	tmThursday,
	tmFriday,
	tmSaturday,
	tmSunday,
	tmWeekday,
	tmWeekend
}TmWindowType;

typedef struct st_time_period {
	TmWindowType window;
	Uint32	begin;
	Uint32	end;
}TM_PERIOD; 

typedef struct st_time_range{
	char name[MAX_PROFILE_NAME_LENGTH+1];
	Boolean active;
	Uint32 tmstart;                       
	Uint32 tmstop;
	TM_PERIOD period [ MAX_PERIOD ];
} TimerangeProfile;
extern TimerangeProfile *tmrgProf;
#endif /* _MIBTMRG_H */
