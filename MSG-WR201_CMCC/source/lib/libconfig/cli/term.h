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

#if ! defined( TERM_H )                  /* prevent nested inclusions */
# define TERM_H                          /* module name */

/*
 * SCCS ID:	$Id: term.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#define RESULT_PREFIX   1
#define RESULT_DONE     2
#define RESULT_UNFIN    3
#define RESULT_NOPROMPT	4

typedef enum en_termParseMode_ {
	TERM_PARSE_CLI,
	TERM_PARSE_WEB,
	TERM_PARSE_BOOT,
	TERM_PARSE_BYPASS,
 	TERM_PARSE_MAX
} termParseMode;

TermCmd *termDo(TermInfo *term, int _verb);
int termGetLast(TermInfo *term, char *buf, int dir);
int termIsLastExist(TermInfo *term);
void termPut(TermInfo *term, char *buf);
void termTerm(TermInfo *term);
int termParse(TermInfo *term, int mode, char *cmdlineprfx);
int termLogin(TermInfo *term, char *login, char *pass);
void termClearScreen(TermInfo *term);   
extern Boolean cliBuildNvram(Pointer nvram, int length);
extern void cliParseData(char *str);

#if (TWO_FLASH == FYES)
extern Uint32 getSecImgLength(void);
#endif
	
# endif /* TERM_H  */ /* module name */


