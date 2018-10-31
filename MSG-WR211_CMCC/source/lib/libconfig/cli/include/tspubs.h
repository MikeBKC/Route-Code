/*
 * $Id: tspubs.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *	Copyright (c) 1996 UTT Technologies, Inc.
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
 *	 UTT Technologies, Inc.
 */

    /*
     * PUBLIC DEFINITIONS USED BY THE TERMINAL SERVER CODE
     */
    /*
     * When terminal server function is enabled the user can set
     * his/her own prompt.  The maximum prompt lenght is defined
     * here.
     */
#define MAX_TS_PROMPT_LENGTH		80

    /*
     * Another terminal server field is the default terminal type
     * used for terminal server sessions.   Define the max lenghth.
     */
#define MAX_TS_TERM_LENGTH		15

    /*
     * The terminal server menu mode allows a description for each
     * host.  The length of the description is defined here.
     */
#define TS_TEXT_LEN			31

    /*
     * The maximum length of the login banner
     */
#define MAX_BANNER_LEN			80

    /*
     * When terminal server function is enabled the user can set
     * their own Login Prompt. The maximum prompt length
     * is defined here.
     */
#define MAX_TS_LOGIN_PROMPT_LENGTH	32

    /*
     * When terminal server function is enabled the user can set
     * their own Password Prompt. The maximum prompt length
     * is defined here.
     */
#define MAX_TS_PASSWD_PROMPT_LENGTH	32

    /*
     * Maximum number of characters allowed in the 
     * AT-answer-string 
     */
#define MAX_AT_ANSWER_STRING_LEN   37

    /*
     * Miscellaneous defines used by the terminal server code.
     */
#define TERMSRV_EOL "\r\n"
#define MAX_CMD_LEN			80
#define TERMSRV_MSG_COUNT		15
#define TERMSRV_CNTRL_C			3
#define TERMSRV_MAX_PASSWORD_FAILURES	3

#define TERMSRV_LOGIN_IDLE_INTERVAL 	(60*5)	/* five minutes */
#define TERMINATE_DELAY_TIMEOUT		20	/* terminal delay 20 secs */

#define TERMSRV_MAX_MENU_HOSTS		10
