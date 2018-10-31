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
    Copyright 1991-1997 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:     $Id: lcdterm.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 * File:	lcdterm.h
 *
 * Overview:	Interfaces to lcdterm.c
 */

#if ! defined( LCDTERM_H )
# define LCDTERM_H

# if ( USERIF & LCD )

#  include "lcdmisc.h"


/*
 * enabled by llliu@080424.
 */
#define TERM_DEFAULT_LINE_PER_PAGE       	21
    /*
     * If, while receiving characters from the terminal and traversing
     * the receive character tree, there could be more characters in the
     * received sequence (i.e., there is a child node in the tree), we
     * determine whether to go further by using a timeout:  if another
     * character is received within the timeout, we proceed in the tree;
     * if not, we return the meta-character indicated by the current
     * node. The timeout we use, in milliseconds, is defined below.
     */
#  define TERM_RX_SEQ_TIMEOUT     250


    /*
     * Characters and character sequences received from the terminal which are
     * terminal dependent are translated to terminal independent "receive
     * meta-characters" by the terminal input routines in lcdterm.c. All higher
     * level routines use these receive meta-characters.
     *
     * These values are greater than 255, so they do not conflict with real
     * character values.
     *
     *	RMC_UP:			The up cursor key or equivalent.
     *
     *	RMC_DOWN:		The down cursor key or equivalent.  Also
     *				carriage-return and line-feed.
     *
     *	RMC_LEFT:		The left cursor key or equivalent.
     *
     *	RMC_RIGHT:		The right cursor key or equivalent.
     *
     *  RMC_RETURN:		The return key
     *
     *	RMC_PREV:		^O (back-tab).
     *
     *	RMC_NEXT:		tab.
     *
     *	RMC_EXIT:		The "cancel changes/exit" key.  Normally ESC.
     *
     *	RMC_ERROR:		A pseudo meta-character indicating an invalid
     *				input character sequence (e.g., one which
     *				starts with the leading characters of a valid
     *				sequence, but does not end validly).  May also
     *				indicate an input parity or framing error
     *				should checking for them be added.
     *
     *	RMC_BS:			The "backspace" key.  Normally backspace.
     *
     *	RMC_REPAINT:		The "repaint screen" key.  Normally ^L.
     *
     *	RMC_TIMEOUT:		A pseudo meta-character returned only from
     *				a terminal input operation with a timeout
     *				specified.  Indicates that no character was
     *				received from the terminal within the timeout
     *				period.
     *
     *	RMC_BOOT:		The special "reboot system" character
     *				sequence.  Normally ESC-[-ESC-ESC.
     *
     *	RMC_DIAGNOSTIC:		The special sequence to make the system act
     *				like it has diagnostics enabled.
     *
     *  RMC_DO:			The DO! key on the Palmtop or ^D for VT100.
     *
     *  RMC_STAT:		The STAT key on the Palmtop or ^T for VT100.
     *
     *  RMC_DELETE:		The DEL key.
     *
     *	RMC_ABORT:		The user is asking us to abort the major
     *				operation in progress.
     *
     *	RMC_MIF:		The user wants to enter the MIF
     *
     *	RMC_PGDN:		The page down key for getting through those
     *				long profiles, usually mapped to ^V.
     *
     *	RMC_TERMSRV:		The user is asking to jump to the terminal
     *				server from whereever she is in the menu
     *				system.
     *
     *	RMC_DESTROYED:		The keystroke returned to processes waiting
     *				for input from a terminal that has just been
     *				destroyed.
     */
typedef enum {
    RMC_UP = 256,
    RMC_DOWN,
    RMC_LEFT,
    RMC_RIGHT,
    RMC_RETURN,
    RMC_PREV,
    RMC_NEXT,
    RMC_EXIT,
    RMC_ERROR,
    RMC_BS,
    RMC_REPAINT,
    RMC_TIMEOUT,
    RMC_BOOT,
    RMC_DIAGNOSTIC,
    RMC_DO,
    RMC_STAT,
    RMC_DELETE,
    RMC_ABORT,
    RMC_MIF,
    RMC_TERMSRV,
    RMC_PGDN,
    RMC_DESTROYED
} RxMetaCharacter;


    /*
     * The types of physical terminal types supported are enumerated below.
     * When a terminal is created, one of the types must be specified.
     *
     * At any given time, either none or one terminal is active in the 
     * system.  The possible options are enumerated below.  Note that
     * any one of the values may be running in local or remote mode,
     * independent of the selection.
     *
     * The possible values are:
     *
     *  TERM_SEL_NONE:		No terminal is currently selected.  This
     *				is generally the state at startup, and
     *				is considered transitory as the system
     *				determines what type of terminal is
     *				available.
     *
     *	TERM_SEL_VT100:		The vt100 terminal driver is currently
     *				selected.
     *
     *	TERM_SEL_LCD:		The lcd terminal driver is currently
     *				selected.
     *
     *	TERM_SEL_SIMPLE:	A Simplified version of the VT100 user
     *				interface.
     *
     *	TERM_NUMBER_OF_TERMS:	The number of elements in this enumeration.
     */
typedef enum {
    TERM_SEL_NONE,
    TERM_SEL_VT100,
    TERM_SEL_LCD,
    TERM_SEL_SIMPLE,
    TERM_NUMBER_OF_TERMS
} TerminalSelection;


    /*
     * For each terminal, there is a function which performs the actual read
     * of a character from the terminal.  That function is defined below,
     * and is passed to the creation function for use internally to this
     * module.
     *
     * Parameters:
     *	deviceDriverHdl:	Device driver handle for which the terminal
     *				instance was created.
     *
     *	timeout_ms:		The number of milliseconds to wait for
     *				a byte to be read in.
     *
     *	buffer:			Pointer to location to store characters
     *				that were read in.
     *
     *  bufferLen:		The maximum number of characters to read
     *
     * Returns:
     *	The number of bytes read, or a negative number to indicate a
     *  failure.
     * 
     */
typedef int DriverReadBuffer( DeviceDriverHdl	deviceDriverHdl,
				     Word		timeout_ms,
				     char		*buffer,
				     Uint32		bufferLen );


    /*
     * For each terminal, there is a function which performs the actual
     * writing to the physical display.  That function is defined below,
     * and is passed to the creation function for use internally to this
     * module.
     *
     * Paramters:
     *	deviceDriverHdl:	Device driver handle for which the terminal
     *				instance was created.
     *
     *	bufPtr:			Pointer to buffer to output to the terminal
     *
     *	bufLen:			Number of bytes in the buffer to transfer.
     *
     *	remote:			flag to indicate remote data.
     */
typedef void DriverWriteCharacter( DeviceDriverHdl	deviceDriverHdl,
				   Byte const		*bufPtr,
				   Word			bufLen,
				   Boolean		remote );


    /*
     * Initialize a new terminal for operation.  This function is called
     * immediately after the terminal is created, and before the creation
     * function returns. This function builds the device driver structure
     * and returns its handle.
     *
     * Paramters:
     *  terminalSelection	Type of terminal for which the terminal
     *				instance was created.
     *
     *	routeID:		The routeID (if any) for which the terminal
     *				instance was created.
     *
     *	channel:		A generic parameter that may be needed by
     *				some of the drivers.
     *
     *	initialDataRate:	The data rate to program the device for,
     *				if this device has a meaningful data rate.
     */
#if 0
typedef DeviceDriverHdl
DriverInitFunction( TerminalSelection	terminalSelection,
		    RouteID		routeID,
		    Word		channel,
		    ControlPortBitRate	initialDataRate );
#endif

    /*
     * When a terminal instance is destroyed, this function is called.  The
     * driver should shut-down at this time.
     *
     * Parameters:
     *	deviceDriverHdl:	Device driver handle for which the terminal
     *				instance was crearted.
     */
typedef void DriverTerminateFnc( DeviceDriverHdl	deviceDriverHdl );


    /*
     * Function to enable/Disable XON/XOFF flow control of terminal.
     *
     * Parameters:
     *  deviceDriverHdl:	Device driver handler for which the
     *				terminal instance was created.
     *
     *  enable:			TRUE enables XON/XOFF flow control, 
     *				FALSE disables.
     */
typedef void DriverFlowControlFnc( DeviceDriverHdl  deviceDriverHdl,
				   Boolean	    enable );


    /*
     * When a terminal changes its data rate (if data rate is meaningful
     * to the terminal), the terminal should change it at this request.
     *
     * Parameters:
     *	deviceDriverHdl:	Device driver handler for which the terminal
     *				instance was created.
     *
     *	dataRate:		The new data rate for the port.
     */
#if 0
typedef void DriverSetRateFnc( DeviceDriverHdl		deviceDriverHdl,
			       ControlPortBitRate	dataRate );
#endif


    /*
     * Convert a terminal to remote master terminal operation mode and
     * vice versa.
     *
     * Parameters:
     *  lcdTerminal:		Device driver handler for which the
     *				terminal instance was created.
     *
     *  routeID:		The routeID associated with the remote
     *				terminal instance.
     *
     *  enable:			If TRUE, put terminal into remote master
     *				operational mode, if FALSE, put terminal
     *				back into normal operation mode.
     */
#if 0
typedef void DriverRemoteMasterFnc( DeviceDriverHdl	deviceDriverHdl,
				    RouteID		routeID,
				    Boolean		enable );
#endif


    /*
     * Look at the terminal selection and pick the proper terminal.  If a
     * VT100 type is being requested we may mutate it to a TERM_SEL_SIMPLE
     * depending upon the configuration and the allowMutate flag.
     *
     *	terminal:	the type of terminal to create
     *
     *	controlPort:	the control port of the associated terminal
     *
     *	allowMutate:	allow simple term configuration.
     *
     * Note: This function can only be used to create local terminals.
     */
extern LcdTerminalHdl
lcdTermPickTerminal( TerminalSelection	terminal,
		     Word		controlPort,
		     Boolean		allowMutate );

    /*
     * If this port should startup as a MIF terminal, then return TRUE.
     * Otherwise, return FALSE.
     *
     * termHdl:		The terminal being queried.
     *
     * Returns:
     *	<>		TRUE if it is to be a MIF terminal, FALSE otherwise.
     */
extern Boolean
lcdTermIsMIFPort( LcdTerminalHdl	termHdl );


    /*
     * Create a new terminal with the characteristics as follows:
     *
     *	terminalSelection:	The type of terminal being created.
     *
     *	termSvr:		This is for a terminal server session.
     *
     *  securityIndex:		The index of the default security
     *				profile to use with this terminal.
     *
     *	initialDataRate:	The control port data rate for the
     *				port initially (can be changed later).
     *
     *	channel:		Channel number to pass to different
     *				driver functions.  The value of this
     *				is based on the driver being used.
     *
     *  driverReadFnc:		The function passed for this driver.
     *
     *  driverWriteFnc:		The function passed for this driver.
     *
     *  driverInitFnc:		The function passed for this driver.
     *
     *  driverTerminateFnc:	The function passed for this driver.
     *
     *  driverFlowControlFnc:	The function passed for this driver.
     *
     *  driverSetRateFnc:	The function passed for this driver.
     *
     *  driverRemoteMasterFnc:	The function passed for this driver.
     *
     */
#if 0
extern LcdTerminalHdl
lcdTermCreateLcdTerminal( TerminalSelection	terminalSelection,
			  Boolean		termSvr,
			  Word			securityIndex,
			  RectangularPortNumber	portNumber,
			  ControlPortBitRate	initialDataRate,
			  Word			channel,
		          DriverReadBuffer	*driverReadFnc,
		          DriverWriteCharacter	*driverWriteFnc,
		          DriverInitFunction	*driverInitFnc,
		          DriverTerminateFnc	*driverTerminateFnc,
			  DriverFlowControlFnc	*driverFlowControlFnc,
			  DriverSetRateFnc	*driverSetRateFnc,
			  DriverRemoteMasterFnc	*driverRemoteMasterFnc
			 );
#endif


    /*
     * The destroyer of a terminal may have other things to do, after the
     * terminal has been destroyed.  If it does it passes a LcdTermDestroyed
     * function as a callback when the terminal is destroyed.
     */
typedef void LcdTermDestroyed( void );



    /*
     * Return true if the terminal can be closed by user request.
     *
     *	lcdTerminal:		The terminal to query.
     */
extern Boolean
lcdTermIsClosable( LcdTerminalHdl	lcdTerminal );


    /*
     * This terminal is going out of business.  Destroy it, and release
     * its driver / window(s), please.
     *
     * Parameters:
     *	lcdTerminal:		The terminal to destroy.
     *
     *	termDestroyed:		Address of optional function to call once
     *				the destroy is complete.
     */
extern void
lcdTermDestroyTerminal( LcdTerminalHdl		lcdTerminal,
		        LcdTermDestroyed*	termDestroyed );

    /*
     * lcdTermIsDestroyed:
     *
     * A function to query if an lcdterm is valid or not.  If the passed
     * LcdTerminalHdl is invalid or if valid but the destroyFlag is
     * pending, the function will return TRUE, otherwise the function
     * returns FALSE.
     */
extern Boolean
lcdTermIsDestroyed( LcdTerminalHdl	lcdTerm );


    /*
     * Put a terminal into/out of remote master terminal operation mode.
     *
     * Parameters:
     *  lcdTerminal:		Terminal handle to operate remote terminal
     *				on.
     *
     *  routeID:		Route associated with remote terminal.
     *
     *	enable:			If TRUE, put terminal into remote terminal
     *				master operation mode, if FALSE, put 
     *				terminal back to normal operation.
     */
#if 0
extern void
lcdTermRemoteMaster( LcdTerminalHdl lcdTerminal,
		     RouteID 	    routeID,
		     Boolean	    enable );
#endif


    /*
     * Query the Device Driver associated with the terminal.
     */
extern DeviceDriverHdl
lcdTermQueryDevDrv( LcdTerminalHdl	lcdTerminal );


    /*
     * Query the PortNumber associated with the terminal.
     */
#if 0
extern LinearPortNumber
lcdTermQueryPortNumber( LcdTerminalHdl	lcdTerminal );
#endif


    /*
     * Query the default security profile index.
     */
extern Word
lcdTermQueryDefaultSecurityProf( LcdTerminalHdl	lcdTerminal );


    /*
     * Query the current security profile index.
     */
extern Word
lcdTermQueryCurrentSecurityProf( LcdTerminalHdl	lcdTerminal );


    /*
     * Set the current security profile index.
     */
extern void
lcdTermSetCurrentSecurityProf( LcdTerminalHdl	lcdTerminal,
			       Word			securityIndex );


    /*
     * Check whether security is in effect for the indicated item.
     * Returns TRUE if the indicated item is secure, i.e. can not
     * be used.
     */
extern Boolean
lcdTermIsItemSecure( LcdTerminalHdl	lcdTerminal,
		     SecurityItem	securityItem );


    /*
     * Change the data rate on the terminal passed to the rate passed.
     *
     * Paramters:
     *	lcdTerminal:		Terminal on which to change data rate.
     *
     *	dataRate:		The new data rate for the terminal.
     */
#if 0
extern void
lcdTermSetDataRate( LcdTerminalHdl	lcdTerminal,
		    ControlPortBitRate	dataRate );
#endif


    /*
     * Enable X-on/X-off flow control for the indicated terminal.
     */
extern void
lcdTermEnableFlowControl( LcdTerminalHdl	lcdTerminal );

    /*
     * Disable X-on/X-off flow control for the indicated terminal;
     */
extern void
lcdTermDisableFlowControl( LcdTerminalHdl	lcdTerminal );


    /*
     * Request the ownership of the overall terminal for purposes of
     * outputing data.  The ownership must be explicitly requested and
     * relinquished whenever a window or anything else must update the
     * physical display.  This function will block whenever the temrinal
     * is not available.
     *
     * Parameters:
     *	lcdTerminal:		The terminal to gain ownership of.
     */
extern void
lcdTermRequestLcdTerminal( LcdTerminalHdl	lcdTerminal );


    /*
     * After owning the terminal through the function above, relinquish
     * ownership of it.  This function must _always_ be called in pairs
     * with the terminal request function above.
     *
     * Parameters:
     *	lcdTerminal:		The terminal to release ownership of.
     */
extern void
lcdTermReleaseLcdTerminal( LcdTerminalHdl	lcdTerminal );


    /*
     * Redraw the vt100 display on the physical display.  This may be called
     * if the terminal type changes on us.
     */
extern void
lcdTermRepaint( LcdTerminalHdl		lcdTerminal );


    /*
     * Clear the given screen, set the cursor in the top left hand corner.
     */
extern void
lcdTermClearScreen( LcdTerminalHdl	lcdTerminal );


    /*
     * Clear the given row of the given screen to blanks.  Put the cursor
     * at the beginning of the cleared row.
     */
extern void
lcdTermClearLine( LcdTerminalHdl	lcdTerminal,
		  Byte			row );


    /*
     * Set the blink attribute on for those terminals that support blink,
     * otherwise a no-op
     */
extern void
lcdTermBlinkOn(LcdTerminalHdl lcdTerminal);


    /*
     * Set the blink attribute off for those terminals that support
     * blink, otherwise a no-op.
     */
extern void
lcdTermBlinkOff(LcdTerminalHdl lcdTerminal);


    /*
     * Return true if the terminal supports hardware blinking
     */
extern Boolean
lcdTermBlinks(LcdTerminalHdl lcdTermainl);


    /*
     * Turns display off/on for the given terminal handle, this is used
     * to support software blinking.
     *
     * Parameters:
     *   lcdTerminal - Handle of terminal to operate on
     *
     *	 blink	     - TRUE turns display on, FALSE turns display off
     */
extern void
lcdTermClearOnNextTick(LcdTerminalHdl	lcdTerminal,
		       Boolean		blink);


    /*
     * Returns the blink state of the given terminal handle.
     */
extern Boolean
lcdTermQueryClearOnNextTick(LcdTerminalHdl lcdTerminal);


    /*
     * Receive a character from the terminal and return it (as a receive
     * meta-character) to the caller. 
     */
extern RxMetaCharacter
lcdTermGetChar( LcdTerminalHdl	lcdTerminal );


    /*
     * Receive a character from the terminal and return it (as a receive
     * meta-character) to the caller.  Function determines if there is a
     * character there within the specified tiemout. If there is, then it
     * returns TRUE in *success and the actual character.  Otherwise, it 
     * returns 0, and *success is FALSE.
     *
     * Please note that some timeout value has special meaning:
     *
     *	timeout_ms = 0	the GetChar is a "block" call, and will not return
     *			until a char is read, it has the same effect as
     *			lcdTermGetChar.
     *
     *  timeout_ms = 1	a check and return call, i.e. no time out, returns
     *			right away if no char is available.
     */
extern RxMetaCharacter
lcdTermGetCharCond( LcdTerminalHdl	lcdTerminal,
		    Word		timeout_ms,
		    Boolean		*success );


    /*
    ** lcdTermGetBufferCond
    **
    ** Similar to lcdTermGetCharCond, except a buffer can be filled,
    ** rather than simply returning a single character.
    **
    **   lcdTerminal:			The associated terminal
    **
    **   buffer:			User supplied buffer
    **
    **   bufferLen:			The maximum number of characters
    **					we can place in buffer
    **
    **   timeout_ms:			The amount of time in msecs
    **					we're willing
    **					to wait for a character
    **
    ** Returns the number of characters stored in buffer, or a negative
    ** number to indicate an error occurred (such as the terminal is
    ** no longer valid).
    **
    **
    ** Please note that some timeout value has special meaning:
    **
    **	timeout_ms = 0	the GetChar is a "block" call, and will not return
    **			until a char is read, it has the same effect as
    **			lcdTermGetChar.
    **
    **  timeout_ms = 1	a check and return call, i.e. no time out, returns
    **			right away if no char is available.
    */
extern int
lcdTermGetBufferCond( LcdTerminalHdl	lcdTerminal,
		      char		*buffer,
		      Uint32		bufferLen,
		      Word		timeout_ms );

    /*
     * Similar to lcdTermGetCharCond except the character is not 
     * translated to Metachar, this raw char is necessary to support
     * remote management io.
     */
extern Byte
lcdTermGetRawCharCond( LcdTerminalHdl	lcdTerminal,
		       Word		timeout_ms,
		       Boolean		*success );


    /*
     * The module initialization function called from initram.c.
     */
#  if ( SCAVENGE_RAM == FYES )
extern void
lcdTermInit( void )__attribute__ ((section (".init")));
#  else
extern void
lcdTermInit( void );
#  endif


    /*
     * Output the given buffer, which may contain only displayable characters,
     * newlines ('\n'), returns ('\r'), and tabs.  The buffer is not terminated
     * by a null.
     */
extern void
lcdTermPutBuf(	LcdTerminalHdl	lcdTerminal,
		Byte const	*bufPtr,
		Word		bufLen );


    /*
     * Similar to lcdTermPutBuf, except that in calling lower layer
     * driverWriteFnc it passes a TRUE parameter to indicate remote data.
     */
extern void
lcdRemoteTermPutBuf(	LcdTerminalHdl	lcdTerminal,
		Byte const	*bufPtr,
		Word		bufLen );


    /*
     * Display the given character, which must be valid for lcdTermPutBuf().
     */
extern void
lcdTermPutChar( LcdTerminalHdl		lcdTerminal,
		Byte 			c );


    /*
     * Display the given nul-terminated string, which must contain only
     * characters valid for lcdTermPutBuf().  Return the length of the string.
     */
extern Word
lcdTermPutStr( LcdTerminalHdl		lcdTerminal,
	       const char		*stringPtr );

    /*
     * Make the cursor non-visible on the lcd display.
     */
extern void
lcdTermHideCursor( LcdTerminalHdl		lcdTerminal );


    /*
     * Make the cursor visible on the lcd display.
     */
extern void
lcdTermShowCursor( LcdTerminalHdl		lcdTerminal );


    /*
     * Move the cursor to the given row and column screen.
     */
extern void
lcdTermCursorSet( LcdTerminalHdl	lcdTerminal,
		  Word		row,
		  Word		column );


    /*
     * The window manager has to register with the terminal manager in
     * order to let it know what functions to call when a terminal is
     * repainted or destroyed.  The functions passed are defined below,
     * and are called at the obvious times.
     */
typedef void TerminalRepaintFnc( LcdTerminalHdl	lcdTerminal );

typedef void TerminalStopFnc( LcdTerminalHdl	lcdTerminal );

typedef void TerminalDestroyFnc( LcdTerminalHdl	lcdTerminal );

typedef void TerminalCreateFnc( LcdTerminalHdl	  lcdTerminal,
				TerminalSelection terminalSelection,
				Boolean		  termSvr );

    /*
     * The function below should be called by the window manager to register
     * with the terminal manager.  The registration should be done at 
     * initialization time, with the window manager initializing _after_
     * the terminal manager.
     */
extern void
lcdTermRegisterWindowMgr( TerminalRepaintFnc	*repaintFnc,
			  TerminalCreateFnc	*createFnc,  
			  TerminalStopFnc	*stopFnc,
			  TerminalDestroyFnc	*destroyFnc );

    /*
     * Return the type of terminal that the terminal passed in is,
     * please.
     */
extern TerminalSelection
lcdTermQueryTerminalType( LcdTerminalHdl	lcdTerminal );

    /*
     * Functions to support limiting of user ports (as opposed to
     * console ports) to specific subsets of the menu system.
     *
     *	lcdTermIsConsole returns TRUE if the terminal is the console.
     *
     *	lcdTermIsLimited returns TRUE if the terminal is limited to
     *	a subset of the menu system.
     *
     *	lcdTermLimitedPort returns the rectangular port of the terminal
     *	when limited to a portion of the user interface.  It returns
     *	SLOT and PORT ANY when the term is not limited.
     *
     *	lcdTermToChannel returns the channel passed when the terminal was
     *	created.
     */
extern Boolean
lcdTermIsConsole( LcdTerminalHdl lcdTerminal );

extern Boolean
lcdTermIsLimited( LcdTerminalHdl lcdTerminal );

#if 0
extern RectangularPortNumber
lcdTermLimitedPort( LcdTerminalHdl lcdTerminal );
#endif

extern Word
lcdTermToChannel( LcdTerminalHdl lcdTerminal );

    /*
     * Functions to begin and end notification of current security
     * profile change.
     */
typedef void SecRefreshFnc( Pointer	userPtr );

extern void
lcdTermBeginSecLevelRefresh( LcdTerminalHdl	lcdTerminal,
			     Pointer		userPtr,
			     SecRefreshFnc	*secRefreshFnc );

extern void
lcdTermEndSecLevelRefresh( Pointer		userPtr );

    /*
     * Return the number of currently active terminals.
     */
extern Int
lcdTermCount( void );


#  if ( LMODEM_TSDIALOUT == FYES )
    /*
     * Set the IsImmediateModem flag associated with the terminal.
     */
extern void
lcdTermSetImmediateModem( LcdTerminalHdl lcdTerminal );

    /*
     * Is this LcdTerminal spaned by an immediateModem telnet ?
     */
extern Boolean
lcdTermQueryIsImmediateModem( LcdTerminalHdl lcdTerminal );
#  endif /* LMODEM_TSDIALOUT */


    /*
    ** lcdTermRoute
    **
    ** Retrieve the route associated with the given handle.
    **
    **    lcdTerminal:			Associated Handle 
    **
    **    routeIDPtr:			Store the route here
    **
    ** Returns TRUE if the handle is valid and *routeIDPtr is updated.
    **
    ** Returns FALSE if the handle is invalid.  *routeIDPtr is not
    ** changed in this case.
    **
    */
#if 0
extern Boolean
lcdTermRoute( LcdTerminalHdl lcdTerminal,
	      RouteID	    *routeID );

#endif
# endif 			/* ( USERIF & LCD ) */

    /*
     * Display the given formatted string, which must contain only
     * characters valid for lcdTermPutBuf().  Return the length of the string.
     */
extern Word
lcdTermPrintf( LcdTerminalHdl  		lcdTerminal, const char	*stringPtr, ... );

    /*
     * A 'more' procedure.  Pass in the address of a counter and the
     * number of entries per page.  The function blocks at end of page
     * until return is entered.  Returns TRUE if the list is to be aborted.
     */

extern void
lcdTermGetPageSetting( LcdTerminalHdl lcdTerminal, Uint32 *max, Uint32 *now);

extern void
lcdTermSetPageSetting( LcdTerminalHdl lcdTerminal, Uint32 *max, Uint32 *now);

extern Boolean
lcdTermMore( LcdTerminalHdl lcdTerminal );

extern Boolean
lcdTermMoreInit(LcdTerminalHdl lcdTerminal, Boolean clearScreen);

# define lcdTermMoreQuit 	lcdTermMore

/*
 * entries added by llliu@080424.
 */
extern void 
lcdTermSetMoreFlag(LcdTerminalHdl lcdTerminal, Boolean flag);

extern Boolean
lcdTermQueryIsMore(LcdTerminalHdl lcdTerminal);

#endif				/* LCDTERM_H */


