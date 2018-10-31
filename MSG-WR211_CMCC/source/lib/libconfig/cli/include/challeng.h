/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be  copied,    ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
 * $Id: challeng.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *   Copyright 1991-1994 UTT Technologies, Inc.
 *   All rights reserved.
 *   Use of copyright notice does not imply publication.
 */


#if !defined(CHALLENGE_H)
# define CHALLENGE_H

    /*
     * password server input function:
     *
     * Function to read a character (if one is pending) from the password
     * server.  The read is non-blocking.
     *
     *	tag:		An opaque data type to identify the session
     *			from which input is desired.
     *
     *	result:		The result of the read request.  If
     *			TRUE then the function returned the byte
     *			read from teh keyboard.  If FALSE then
     *			the function returned 0 if there were no
     *			characters to read or 0xff if the terminal
     *			has been destroyed.
     *
     * Returns the character read or status of the session.
     */
typedef Uchar (* PwsInput)(Pointer tag, Boolean *result);

    /* 
     * password server output function:
     *
     * Send a NUL-terminated string to the password server
     *
     *	term:		Opaque Pointer to the session information.
     *
     *	outMess:	Output message
     *
     *	len:		length of output message
     */
typedef void (* PwsOutput)(Pointer term, const char *outMess, Uint16 len);

/*
 * Prototype for the Password Server callback function
 */
typedef void (* PwsCallback)(Pointer, Uchar *, Boolean, int);

/*
 * Struct passed to the challenge handler (and thence to the task which
 * gets the new password)
 */
typedef struct PwsReqBlk {
    Boolean		abort;		/* indicates rqst is not to be done */
    char		*hostname;	/* NUL-terminated name of remote host */
    Uchar		*challenge;	/* the challenge */
    Uint16		challengeLen;	/* length in bytes of challenge */
    Uchar		*password;	/* the resultant password */
    Uint16		passwordLen;	/* length in bytes of password */
    Pointer		pppId;		/* pointer to PPP_INFO */
    Pointer		msgTag;		/* pointer to session */
    PwsCallback		callback;	/* pointer to callback func */
} PwsReqBlk;

    /*
     * When the passwordLen field in a PwsReqBlk struct is set
     * to this symbol it indicates that the request has timed out
     * and that there is no password in the request block. This
     * therefore allows us to distinguish between zero-length
     * password requests (which might trigger a new challenge),
     * and timeouts, which terminate attempts to get a new
     * password.
     */
# define PRB_TIMEOUT_IND_LENGTH	((Uint16)-1)

typedef void (* ChallengeReq)(PwsReqBlk *);
typedef Boolean (* PwsRequest)(ChallengeReq, PwsReqBlk *);

    /* Password Server Types */
typedef enum PwsType {
	TERMINAL_PW_SERVER = 1,
	HOST_PW_SERVER,
} PwsType;

    /* Password Server Modes */
typedef enum PwsMode {
	DORMANT = 1,
	ACTIVE,
} PwsMode;

    /* Password Server Registration Information */
typedef Pointer	PwsSessionId;
typedef struct  PwsRegistration {
	PwsSessionId	pwsId;		/* session identifier */
	PwsMode		pwsMode;	/* ACTIVE or DORMANT */
	PwsType		pwsType;	/* HOST or TERMINAL */
	PwsInput	pwsIn;		/* input function */
	PwsOutput	pwsOut;		/* output function */
	PwsRequest	pwsCacheF;	/* cache request function */
} PwsRegistration;

# define MAX_PASSWORD_LEN	64	/* somewhat arbitrary */

/*
 * Register (or Deregister) the Password Server with the challenge
 * handler
 */
extern Boolean pwServerRegistration(PwsRegistration *);

/*
 * Given a password request, prompt the password server
 * and get a new password
 *
 * Returns PENDING Boolean:
 *	TRUE means request is queued and completion will invoke callback 
 *	FALSE means queueing failed and request will not be performed
 */
extern Boolean
processPasswordRequest(char*		hostname,
		       Uchar*		challenge,
		       Uint16		lenChallenge,
		       Uchar*		password,
		       Uint16		lenPassword,
		       Pointer		ppp,
		       PwsCallback	callback);

/*
 * Request messages to be displayed on the passsword server screen
 *
 */
Boolean challengeMsg(Pointer ppp, Uchar *msg, PwsCallback callback);

#endif	/* CHALLENGE_H */

