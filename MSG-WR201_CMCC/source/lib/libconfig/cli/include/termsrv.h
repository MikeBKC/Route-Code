/*
 * $Id: termsrv.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *	Copyright (c) 1994-1998 UTT Technologies, Inc.
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

#if ! defined( TERMSRV_H )
# define TERMSRV_H
#define LCD 1
#define USERIF 1

# define TERMSRV_EOL "\r\n"

# if ( USERIF & CLI )
#  include "cli.h"
#  define TSTERM_HANDLE_TYPE	CliHandle
# elif ( USERIF && LCD )
#  define TSTERM_HANDLE_TYPE	LcdTerminalHdl
# endif

    /*
     * max # of banner lines/hosts that we may get from RADIUS.
     */
# define MAX_BANNER_LINES 	16
# define TSERV_MAX_HOSTS	10


    /*
     * TermSrvOutput:
     *
     * The terminal server output routine.
     *
     *	term:		Opaque type used to identify the terminal structure.
     *
     *	buffer:		Pointer to the buffer being output.
     *
     *	len:		Length of buffer.  If the value 0 then buffer is
     *			assumed to be a null terminated string and the
     *			entire string is output.
     */
typedef void
TermSrvOutputFnc( Pointer	term,
		  const char*	buffer,
		  Word		len );


    /*
     * TermSrvBufNotifyFnc:
     *
     * This is the type of a function that may be passed to termSrvBufAvail
     * If the caller wishes to be notified when buffers are again available.
     * Notification is once only.
     *
     *	tag:			An opaque data type only of meaning to
     *				function called.
     */
typedef void
TermSrvBufNotifyFnc( Pointer	tag );

    /*
     * Type of a func used for outputting data to the local end of a
     * terminal server session.
     *
     *  term		Opaque data type used by client to identify
     *			the session.
     *
     *  bufNotifyFnc	Callback to invoke when buffer space becomes
     *			available for output.
     *
     *	bufNotifyTag	Tag to pass back in bufNotifyFnc callback.
     */
typedef Word
TermSrvBufAvailFnc( Pointer			term,
		    TermSrvBufNotifyFnc*	bufNotifyFnc,
		    Pointer			bufNotifyTag );

    /*
     * TermSrvResetFnc:
     *
     * Type of a function to call to reset the I/O counters used to
     * track the amount of user data sent/rcvd.
     *
     *	term:		Opaque type used to identify the terminal structure.
     */
typedef void
TermSrvResetFnc( Pointer	term );

    /*
     * Determines if a given pointer is a pointer to a valid terminal
     * server session or not.
     */
extern Boolean
isTermSrvSession( Pointer  term );

    /* 
     * termSrvShell
     *
     * Terminal server subsitiute for keyboard task.
     *
     *	lcdTerm:	Pointer to terminal handle.
     *
     */
void 
termSrvShell( TSTERM_HANDLE_TYPE lcdTerm );

    /* 
     * termSrvOutputTask:
     *
     * Terminal server subsitiute for update task.
     *
     *	lcdTerm:	Pointer to terminal handle.
     *
     */
void 
termSrvOutputTask( TSTERM_HANDLE_TYPE	lcdTerm );

    /*
     * termSrvStart:
     *
     * The starting routine for a terminal server session.
     * This takes an existing terminal and turns on the
     * terminal server flag in the window strucutre. It also
     * creates a table entry and blocks until end of session.
     *
     *	term:		Terminal handle.
     *
     *	remote:		Started from the console(0) or via modem (1).
     */
Boolean
termSrvStart( TSTERM_HANDLE_TYPE	lcdTerm, 
	      Word	     remote );

    /*
     * TermSrvLocalInputFnc:
     *
     * This is the typedef for the terminal server input routine.
     * It's here because several functions use pointers to the terminal server
     * input routine and thus need to declare it properly.
     * Since the only known definition of such a function is static, you'll
     * find a discussion of the parameters at the function definition.
     */
typedef Byte
TermSrvLocalInputFnc ( Pointer      tag,
                       Boolean*     result );

    /* see comments in termsrv.c */
extern Byte
termSrvLocalInputTimeout( Pointer	tag,
		   	  Boolean*	result,
			  Word		timeOut );

    /*
     * TermSrvLocalOutputFnc:
     *
     * This is the typedef for the terminal server output routine for local
     * messages.  We have a typedef because pointers to the termSrvLocalOutput
     * function are passed throughout the code; this allows them to be
     * typechecked.
     *
     *	term:		Pointer to terminal server structure that was based
     *			on the call to telnet.
     *
     *	buffer:		Pointer to the stuff being output.
     *
     */
typedef void
TermSrvLocalOutputFnc( Pointer		term,
		       const char*	buffer );

    /*
     * termSrvLocalOutput:
     *
     * The terminal server output routine for local messages.
     * Parameters are described with the TermSrvLocalOutputFnc typedef above.
     */
TermSrvLocalOutputFnc
termSrvLocalOutput;

    /*
     * termSrvDestroyed
     *
     * Called to notify the terminal server that the logical terminal has
     * been destroyed.  Passed the TSTERM_HANDLE_TYPE of the terminal.
     */
extern void
termSrvDestroyed( TSTERM_HANDLE_TYPE	lcdTerm );

    /*
     * termSrvGetPort:
     *
     * Return the port number for the given session.  Returns TRUE
     * if the portNumber found, otherwise FALSE.
     *
     * infop: Pointer to the CombInfo object for this session.
     *
     * portNumber: Pointer to a port number to update.
     *
     */
/* Forward dcl.  */

#if 0
extern Boolean
termSrvGetPort( Pointer infop, LinearPortNumber *portNumber );
#endif


    /*
     * termSrvLocalInputBuffer:
     *
     * Function to read a character (if one is pending) from the terminal
     * associated with a terminal server session.  The read is non-blocking.
     *
     *	tag:			An opaque data type to identify the session
     *				that input is desired from.  This is assumed
     *				to be a pointer to the sessions TermInfo
     *				structure.
     *
     *  buffer:			The storage area for the read request.
     *
     *  bufferLen:		The maximum number of characters to read.
     *
     *	timeout:		Value passed to the getchar driver.
     *
     * Returns the number of characters read, or a negative number to indicate
     * an error condition
     *
     */
int
termSrvLocalInputBuffer(  Pointer	tag,
		   	  char *  	buffer,
			  Uint32	bufferLen,
			  Word		timeOut );


# if ( RADIUS == FYES )

#  include "radif.h"

    /*
     * termSrvRefesh
     *
     * Get new terminal server menu info from a central server.
     *
     *	wait:		If true the call will block until the request
     *			is processed.
     */
extern void
termSrvRefresh( Bool wait );

    /*
     * termSrvRefreshStatus
     *
     * Poll the status of the new terminal server menu retrieval.
     *
     * Returns the status.
     */
extern RadiusConfigStatus
termSrvRefreshStatus( void );

    /*
     * initialBannerRequest
     *
     * Get new initial banner from a central server.
     *
     * wait:		If true the call will block until the request
     *			is processed.
     */
extern void
initialBannerRequest( Bool wait );

    /*
     * termSrvRefreshStatus
     *
     * Poll the new initial banner request.
     *
     * Returns the status.
     *
     */
extern RadiusConfigStatus
initialBannerRequestStatus( void );

# endif /* ( RADIUS == FYES ) */

#if 0
    /*
     *  queryUserSrvByRoute
     *
     *  Returns the current user service given a route ID.
     *  Returns SERVICE_NOT_USED if no terminal server
     *  session associated with route can be found.
     */
extern UserServiceType
queryUserSrvByRoute( RouteID routeID );

    /*
     *  queryTermSrvUserInfoByRoute
     *
     *  Given a route ID, retrieves the current user's name, IP address,
     *  and subnet mask.  Returns FALSE if no terminal server
     *  session associated with route can be found.
     */
extern Boolean
queryTermSrvUserInfoByRoute( RouteID routeID, const char** name,
				IpAddress *ipAddr, IpAddress *netmask );

#endif
# if ( ACCT_EVNT == FYES )

    /*
     *  setUserTelnetSrv
     *
     *  Sets the current service depending on whether user is
     *  entering or exiting a telnet session.
     */
extern void
setUserTelnetSrv( Pointer term, Boolean entering, Boolean binaryMode );

    /*
     *  setUserTcpSrv
     *
     *  Sets the current service depending on whether user is
     *  entering or exiting a TCP session.
     */
extern void
setUserTcpSrv( Pointer term, Boolean entering );

# endif /* ACCT_EVENT */

# if ( RADIUS_ACCT == FYES )
    /*
     * updTermSrvAcctFinal:
     *
     * This routine tells the termsrv to close up its RADIUS accounting
     * records.
     *
     *	routeID:	RouteID of the session.
     */
extern void
updTermSrvAcctFinal( RouteID routeID );

    /*
     * termGetAcctStats:
     *
     * This routine gets the Pre authentication statistics from the
     * termsrver structure. Pointers passed are for octets only
     *
     *	routeID:	RouteID of the session.
     *
     *	inOctets:	Pointer to the in octets
     *
     *	outOctets:	Pointer to the out octets
     *
     */
extern Boolean
termGetAcctStats( RouteID	routeID,
		  unsigned long	*inOctets,
		  unsigned long	*outOctets );

# endif

# if ( RADIUS == FYES )

    /*
     * termSrvAcctFinal:
     * 
     * Notifies the termsrv to finish off RADIUS/LOGOUT disconnect codes
     *
     *	routeID:	RouteID of the connection.
     */
void
termSrvAcctFinal( RouteID	routeID );
# endif  

# if PLATFORM & ( MPORT )
    /*
     * killTermSrvBySession:
     * 
     * Terminates a terminal server session assoc w an accounting session .
     * Returns TRUE if session was found and terminated.
     *
     *	sessionRefNum:	session reference number of the session to terminate.
     *
     *	reason		Reason for disconnecting session.
     */
extern Boolean
killTermSrvBySession( unsigned long		sessionRefNum,
		    DisconnectReasonType	reason );

Boolean
killSessionBySessNum( unsigned long        sessionRefNum,
                      TSTERM_HANDLE_TYPE   lcdTerm,
                      DisconnectReasonType reason );
# endif  

# if ( RADIUS_SERV == FYES )
    /*
     * killTermSrvByRoute:
     * 
     * Terminates a terminal server session assoc w a given route.
     * Returns TRUE if session was found and terminated.
     *
     *	routeID:	RouteID of the session to terminate.
     *
     *	reason		Reason for disconnecting session.
     */
extern Boolean
killTermSrvByRoute( RouteID			routeID,
		    DisconnectReasonType	reason );

    /*
     * killTermSrvByUserName:
     * 
     * Terminates ALL terminal server sessions assoc w a
     * given user name.
     * Returns TRUE if at least one session was found and
     * terminated, FALSE otherwise.
     *
     *	userName	Name of user assoc w the session to
     *			terminate.  Null terminated.
     *
     *	userNameLen	Length of user name.
     *
     *	reason		Reason for disconnecting session.
     *			Provided for accounting purposes.
     */
extern Boolean
killTermSrvByUserName(	const char		*userName,
			int			nameLen,
			DisconnectReasonType	reason );
# endif

/* 
 * tsVCDataPending:
 *
 * Called to notify the terminal server  that incoming data is available.
 */
extern Boolean
termSrvPendingData( TSTERM_HANDLE_TYPE	lcdTerm );

# if ( CALL_DURATION == FYES )
/* 
 * termSrvClearCall:
 *	
 * This routines boots the termsrv session outta here! <use with caution>
 *
 *	term		termInfo struct ptr, cast to an unsigned long
 *
 *	reason		Reason for disconnecting session.
 *			Provided for accounting purposes.
 */
Bool
termSrvClearCall( unsigned long term, DisconnectReasonType reason );
# endif /* CALL_DURATION */

# if ( Q_TNT && ( ASYNC == FYES ))
/*
 * For slot cards termSrvExtTsrvInit must be called from tmdmbrouter
 * which is waiting for initialization to complete (waitForInitCompletion()).
 *
 * This function reads the EXT_TSERV profile and initializes the local 
 * data structures _init_banner, _banner and _hosts by calling the 
 * 'profile-changed' function.
 *
 * Registration for EXT-TSRV profile updates is also done here.
 */
extern void
termSrvExtTsrvInit( void );
# endif /* Q_TNT && ASYNC */

# if ( ASYNC == FYES && ( TCP_CLEAR == FYES || TELNET == FYES || RLOGIN == FYES ) ) 

extern void
termSrvStartSession(Pointer 		tag,
                    CallMgmtType        mgmtType );

# endif /* ( ASYNC == FYES && ( TCP_CLEAR == FYES || TELNET == FYES || RLOGIN == FYES ) ) */

# if ( RADIUS == FYES )

    /*
     * Reset the extended RADIUS retry counters.
     */
extern void
termSrvResetExtRadRetryCounts( void );

# endif /* ( RADIUS = FYES ) */

# if ( USERIF & CLI )
#  include "cli.h"
#  define TS_HANDLE_TYPE         CliHandle
#  define TS_OUT(x)   { cliPrintf x; }
#  define TSOUT(x)   { cliPrintf x; }
# else
# include "lcdterm.h"
#  define TS_HANDLE_TYPE         LcdTerminalHdl
#  define TS_OUT(x)  { lcdTermPrintf x;}
#  define TSOUT(x)  { lcdTermPrintf x;}
# endif /* ( USERIF & CLI ) */

#endif /* TERMSRV_H */
