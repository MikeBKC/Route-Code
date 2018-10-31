/*
 * $Id: tscore.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *	Copyright (c) 1996-1997 UTT Technologies, Inc.
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
     * This header file is NOT for general consumption.  It holds data
     * that is private to the terminal server function and should only
     * be included by termsrv.c and tntterm.c.  Public functions are
     * defined in termsrv.h.
     */

#if ! defined( TSCORE_H )

# define TSCORE_H
#include "challeng.h"
#include "termsrv.h"
#include "tspubs.h"
    /*
     * Terminal server defines:
     *
     *	MAX_TERM_SERVER:		Number of terminal server sessions
     * 
     *	TERMSRV_OUTPUT_SIZE:		Size of the output buffer.
     * 
     *	MAX_OUTPUT_QUEUES:		Number of output queues.
     *
     *	TERMSRV_MAX_NAME_LENGTH:	Max #chars allowed for user name.
     *					NOTE: Less than or equal to max for
     *					      User-Name in RADIUS.
     *
     *	TERMSRV_MAX_PASSWORD_LENGTH:	Max #chars allowed for password entry.
     *					Make same as TERMSRV_MAX_NAME_LENGTH.
     *
     *	MAX_RXBUF_LENGTH:		Input buffer size.
     *					NOTE: May be larger than MAX_CMD_LEN
     *					      (max length of a command)
     *					      because buffer must also
     *					      handle login & passwords.
     */
# define MAX_TERM_SERVER		NUM_IFF
# define TERMSRV_OUTPUT_SIZE	256
# define TERMSRV_MSG_SIZE	80
# if ( PLATFORM == TNT )
#  define MAX_OUTPUT_QUEUES	4
# else
#  define MAX_OUTPUT_QUEUES	8
# endif

    /*
     * Use new values if we can afford to burn more RAM
     */
# if ( PLATFORM & P220 )
#  define TERMSRV_MAX_NAME_LENGTH	31
#  define TERMSRV_MAX_PASSWORD_LENGTH	MAX_PASSWORD_LENGTH
#  define MAX_RXBUF_LENGTH		TERMSRV_MSG_SIZE + 2
# else
#  define TERMSRV_MAX_NAME_LENGTH		252
#  define TERMSRV_MAX_PASSWORD_LENGTH	TERMSRV_MAX_NAME_LENGTH
#  if ( TERMSRV_MAX_NAME_LENGTH > MAX_CMD_LEN )
#   define MAX_RXBUF_LENGTH		( TERMSRV_MAX_NAME_LENGTH + 1 )
#  else
#   define MAX_RXBUF_LENGTH		( MAX_CMD_LEN + 1 )
#  endif
# endif



    /*
     * TiTag:
     *
     *	Identification information for a TermInfo:
     *
     *		instance:	Unique instantiation identifier
     *
     *		id:		array position for quick lookup
     *
     * Rather than passing around TermInfo pointers we ought to pass
     * TiTags around, then use the id variable to lookup up the
     * appropriate TermInfo (quickly); the instance variable would allow
     * us to perform a quick validation.  For now, keep using the TermInfo
     * pointers and use the instance variable for another sanity check.
     *
     */
typedef struct TiTag {
    Uint32	instance;
    Uint	id;
} TiTag;

    /*
     * TermSrvOutPutQ:
     *
     * This is the strucuture used for queued output (telnet).
     * The way this scheme works is: there are MAX_OUTPUT_QUEUES
     * number of this structure. The state field determines whether
     * a buffer is available. If used then the buffer is attached
     * to the TermInfo outQueue field. Every call to termSrvOutput
     * will copy data into the buffer pointed to by buf. Field
     * buf is alloacted in termSrvStart and freed during quit.
     * The size is always TERMSR_OUTPUT_SIZE
     *
     *	next:			Next entry in the queue.
     *
     *	state:			State of the buffer.
     *
     *	buf:			Pointer to the data.
     *
     *	len:			Length of the data.
     *
     *	start:			Starting point of data.
     */
typedef struct outputQ	TermSrvOutPutQ;
struct outputQ {
    TermSrvOutPutQ*	next;
    int			state;
    char*		buf;
    Uint16		len;
    Uint16		start;
};

    /*
     * The list of Password Serving requests.
     *
     * next:		pointer to next request
     *
     * func:		function to invoke
     *
     * arg:		argument for the function
     *
     */
typedef struct PwsQueueElem {
    struct PwsQueueElem	*next;
    ChallengeReq	func;
    PwsReqBlk		*arg;
} PwsQueueElem;


    /* 
     * TermInfo:
     *
     * Terminal server strucuture on a per terminal basis.
     * The fields  are:
     *
     *	tiTag:			Identification and lookup info for this
     *				TermInfo
     * 
     *	lcdTerm:		The terminal handle.
     *
     *	state:			State of the entry.
     *
     *	rxBuf:			Recvieve buffer.
     *
     *	inputPos:		Index to where the next character goes.
     *
     *  lastInput:		last full line of input
     *
     *	outBuf:			Table of output buffers.
     *
     *	outQueue:		The queue for the output buffers.
     *
     *	bufNotifyFnc:		Address of function to call to notify
     *				client when buffer space is again available.
     *
     *	bufNotifyTag:		Tag to pass to the bufNotifyFnc.
     *
     *	mailbox:		This guys mailbox.
     *
     *	remote:			Remote session. Used for local command.
     *
     *	authenticate:		TRUE if we are prompting for a login.
     *				This includes local profiles and RADIUS.
     *
     *	passwdTries:		Number of password tries.
     *
     *	sema4:			Authentication semaphore.
     *
     *	idleLimit:		Idle timer interval in seconds; currently
     *				used only during login, not at TS prompt.
     *
     *  tsIdleLimit:		Idle timer interval in seconds: used at TS
     *				prompt.
     *
     *  tsInactive_time:	Time that terminal has been inactive used to
     *				compare against idleTime.
     *
     *  tsIdleMode:		If this mode is TS_IDLE_INPUT_ONLY then the
     *				connection is idle if the user has not entered
     *				data in tsIdleLimit seconds. If this mode is
     *				TS_IDLE_INPUT_OUTPUT then the connection is
     *				idle if the user and the host side haven't
     *				sent data in idleTime seconds. And if it is
     *				TS_IDLE_NONE then we don't have an idle timeout
     *				at all.
     *
     *  bytes_recvd:		Used to determine if the terminal is inactive
     *
     *  bytes_sent:		Used to determine if the host is inactive
     *
     *  veryFirstTime:          This test detects when the first character to
     *                          the terminal server is a CR.
     *
     *	termType:		Pointer to the terminal type.
     *
     *	termPrompt:		Pointer to the terminal prompt.
     *
     *	loginName:		The name the user typed in at the login
     *				prompt.
     *
     *	menuMode:		TRUE if this terminal is using host menu
     *				mode, FALSE for the standard command line.
     *
     *	validated:		TRUE if this user has been validated this
     *				session.  Stops us from prompting for
     *				passwords every time we change modes.
     *
     *	hostCount:		The number of hosts displayed the last
     *				time a host prompt was displayed in menu
     *				mode.  Used to edit data entry.
     *
     *	hosts:			A table of host addresses to go with
     *				the hosts displayed in menu mode.
     *
     *	miscInfo:		A pointer to a place to store misc info 
     *				received from the validation server.
     *
     *	externalAuth:		TRUE if we authenticated from an external
     *				source -- implies that we need to tell
     *				the external source when the call goes
     *				down.
     *
     *	internalAuth:		TRUE if we authenticated from a local
     *				profile -- implies no logout required.
     *
     *	silentMode:		???
     *
     *	closeQuit:		TRUE if the terminal session should be
     *				terminated when a command finishes.
     *				currently only used by the tcp command.
     *
     *	nasPort:		The value of the radius Nas-Port attribute
     *				for this session.
     *
     *	dynamicPwServing:	TRUE if we are acting as a password
     *				server for dynamic passwords
     *
     *	pwsReqQueue		holds info for password serving
     *
     *	userServiceType		Indicates what user service is active
     *				(mainly telnet, telnet_binary, raw_tcp,
     *				or plain terminal server).
     *
     *	xferToProtocol		TRUE if exiting terminal server DUE to
     *				transfering sesssion to a framed protocol
     *				(eg. SLIP/PPP).
     *
     *	menuCount:		Number of menuItems
     *
     *	enableMsg:		Indicates whether its OK to inform user of
     *				events regarding the status of a virtual
     *				connection. Set TRUE when user suspends
     *				a session.
     *
     *	vcSessionHdl:		Virtual connect session handle. Non-zero
     *				value implies handle exists.
     *
     *	suspended:		TRUE if virtual connection session is
     *				currently suspended.
     *
     *	reason:			Holds reason for disconnect from telnet
     *
     *  clidAuthenticated:      Authentication performed via CLID.
     *
     *  mgmtType		cache mgmtType if PAD user
     *
     *  tsPadArgs		cache some params for PAD user
     *
     *  refCount:             	Reference count to indicate this TermInfo
     *				struct should not be freed.  The termsrv
     *				output task is responsible for freeing
     *				the struct but may need to wait until the
     *				input task quits.  The input task increments
     *				this counter initially and decrements it
     *				just before quiting.
     *				NOTE: Ultimately, #increments MUST ==
     *				      #decrements or output task will
     *				      wait forever.
     *
     *  refCountSem:          	Semaphore used to wait until refCount
     *                          goes to zero.
     *
     *	authData:		pointer to data needed during authentication
     *				(see authdispatch.h)
     *  framedOnly:		Only allow framed mode commands:
     *				(slip, cslip, ppp, quit).
     *
     *  lastChar:               last character received while in TERMSRV
     *                          authentication (used to ignore LF following
     *                          a CR)
     */

typedef struct st_TermInfo TermInfo;
struct st_TermInfo {
    TermInfo*		next;
    TiTag			tiTag;
    TSTERM_HANDLE_TYPE		lcdTerm;	/* out terminal */
    Uint16			state;
    Uint16			inputPos;
    char			rxBuf[ MAX_RXBUF_LENGTH + 1 ];
#if 0
#if defined( __GNUC__ )
    __ALIGN__(4) char		fill1[ 0 ];
#elif defined ( __ARMCC_VERSION )
    char		fill1[ 0 ];
#else
# error "align must be selected"
#endif
#endif
    char*			lastInput;
    TermSrvOutPutQ		outBuf[ MAX_OUTPUT_QUEUES ];
    TermSrvOutPutQ*		outQueue;
    TermSrvBufNotifyFnc*	bufNotifyFnc;
    Pointer			bufNotifyTag;
    Bool			remote;
    Bool			authenticate;
    Uint			passwdTries;
    Uint			idleLimit;
# if ( ASYNC == FYES )
    Uint			tsIdleLimit;
    Uint			tsInactiveTime;
    Uint			tsIdleMode;
    Uint32			bytesRecvd;
    Uint32			bytesSent;
    Boolean                     veryFirstTime;
# endif
    char			termType[ MAX_TS_TERM_LENGTH + 1 ];
    char			termPrompt[ MAX_TS_PROMPT_LENGTH + 1 ];
    char			loginName[ TERMSRV_MAX_NAME_LENGTH + 1 ];
#if 0
#if defined( __GNUC__ )
    __ALIGN__(4) char		fill12[ 0 ];
#elif defined ( __ARMCC_VERSION )
    char		fill12[ 0 ];
#else
# error "align must be selected"
#endif
#endif
    Bool			menuMode;
    Bool			validated;
    Uint			hostCount;
    char			hosts[ TERMSRV_MAX_MENU_HOSTS ] [ TS_TEXT_LEN + 1];
    Bool			externalAuth;
    Bool			internalAuth;
    Bool			silentMode;
    Bool			closeQuit;
    Uint32			nasPort;
# if ( LCP_CHALLENGE_CLIENT == FYES )
    Bool			dynamicPwServing;
    PwsQueueElem		*pwsReqQueue;
# endif
# if ( SNMP_ACCT == FYES ) || ( RADIUS_ACCT == FYES ) || ( ACCT_LIB == FYES )
    UserServiceType		userServiceType;
# endif
# if ( RADIUS_ACCT == FYES )
    Bool			xferToProtocol;
# endif
    Uint			menuCount;
# if ( LMODEM_TSDIALOUT == FYES )
    Bool			enableMsg;
    TsVCStatus			vcState;
    VcHandle			vcHandle;
# endif
# if ( SNMP_ACCT == FYES ) || ( RADIUS_ACCT == FYES ) || ( ACCT_LIB == FYES )
    DisconnectReasonType	reason;
# endif
# if ( CLID == FYES)
    Bool			clidAuthenticated;
# endif /* CLID */
# if ( ( X25PAD == FYES ) || ( X25_T3POS == FYES ) )
    CallMgmtType		mgmtType;
# endif	/* X25PAD || X25_T3POS */
# if ( X25PAD == FYES )
    TsPadArgs*			tsPadArgs;
# endif
# if ( X25_T3POS == FYES )
    TsT3PosArgs*		tsT3PosArgs;
# endif
    Uint32                    	refCount;

    Uchar			lastChar;
# if ( ASYNC == FYES )
    Boolean			framedOnly;
# endif /* ASYNC == FYES */
    Uint32                      vRouterID;

#define HISTORY_LEN 		10
    char			cmdHis[HISTORY_LEN][MAX_RXBUF_LENGTH + 1];
    int				cmdHisPutIndex;
    int 			cmdHisGetIndex;
    int				cmdHisRewind;
    int				verb;
    int 			result;
    int				userPtr;
    int				curLevel;
    int 			*inst;
    void			*cmd;
/* added by Ast 1/9/14 */
    Boolean			echoChar;    
/* added by Ast 2/2/1 */
} ;



#endif /* TSCORE_H */
