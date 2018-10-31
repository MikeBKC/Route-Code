/*
 * $Id: cli.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *      Copyright (c) 1995-1998 UTT Technologies, Inc.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of UTT Technologies, Inc.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise duplicated
 *      and the information herein may not be used, disseminated or
 *      otherwise disclosed, except with the prior written consent of
 *      UTT Technologies, Inc.
 */

#if ! defined( CLI_H )
# define CLI_H

    /*
     * The Command Line Interface (CLI) is composed of a general core,
     * low level drivers for every access method, and clients that
     * implement the specific commands.
     *
     * Command handlers register with the core at system startup.
     *
     * When a terminal session is initiated, an instance of the core is
     * created.  This instance is told which low level drivers to use
     * for input/output.
     *
     * The core (this module) implements the standard input and output
     * functions used by the registered command handlers.  Two tasks are
     * created for each terminal session, one that handles input and
     * and manages the session and one that processes queued writes from
     * handlers.  This guarantees that data output cannot be blocked by
     * input.  To guarantee that data input is not blocked by output,
     * a command handler using the CLI session task should only use
     * cliQueueData() for writes, or else dedicate a seperate task for 
     * output.
     */

# include "clidrv.h"

# define CLI_ERROR_HEADER	"error: "

# define CLI_DIAG_PROMPT	"DIAG> "
# define CLI_BOOT_PROMPT	"BOOT> "
# define CLI_NO_SERIAL_PROMPT	"UNSERIALIZED> "

# define CLI_WILD_CARD_PROMPT	"*"

# define CLI_DEFAULT_USER	"default"
# define CLI_ADMIN_USER		"admin"
# define CLI_SUPER_USER		"super"

# define CLI_MAX_PROMPT_LENGTH	20

# define CLI_TERM_WIDTH		80

    /*
     * Screen layout:
     *	top window is set to 9 lines
     *  bottom window and left window will adjust to status-length as 
     *	  defined in the user profile
     *
     * The minimum height of the whole status window is 18 lines which 
     * is the old, fixed-height, setting and the new default.
     */

# define CLI_MAX_CURSOR_ROW	999
# define CLI_TOP_WNDW_LENGTH	8	/* 8 lines, then divider */
# define CLI_MIN_TERM_LENGTH	24
# define CLI_MAX_TERM_LENGTH	CLI_MAX_CURSOR_ROW
# define CLI_MIN_STAT_TERM_SEP	6
# define CLI_MIN_STAT_LENGTH	CLI_MIN_TERM_LENGTH - CLI_MIN_STAT_TERM_SEP
# define CLI_MAX_STAT_LENGTH	CLI_MAX_TERM_LENGTH - CLI_MIN_STAT_TERM_SEP

# define CLI_DUMP_MAX_LENGTH     999999

    /*
     * The maximum number of characters we allow a user to enter at the 
     * primary CLI prompt is CLI_MAX_INPUT_LINE.  This number is used in
     * sizing buffers; the actual number of characters the user can type
     * is also currently affected by the screen space taken up by the prompt
     * and that fact that we don't allow typed lines to "wrap" down.
     */ 
# define CLI_MAX_INPUT_LINE	CLI_TERM_WIDTH

    /*
     * The maximum length of any single monitor message is defined below by
     * CLI_MAX_MESSAGE_LEN.  It is a somewhat random number that is used
     * just to allow static buffer allocation.  We tie it to MAX_INPUT_LINE
     * to accomodate messages that refer to command line input.
     */ 
# define CLI_MAX_MESSAGE_LEN	( CLI_MAX_INPUT_LINE * 2 )


    /*
     * CliAuth:
     *
     * Each registered command is associated with an authorization.  A user
     * can only see and select commands that s/he is authorized to use.
     * The password protected authorization set in use can be selected by
     * the user.
     *
     *	CLI_AUTH_USER:		Commands that any general user are allowed
     *				to use.
     *
     *	CLI_AUTH_TERMSERV:	Terminal server commands.
     *
     *	CLI_AUTH_SYSTEM:	Commands with this authentication are
     *				allowed to look (but not touch) at system
     *				parameters.  These are commands that a
     *				unit administrator would use, not a general
     *				user.  Passwords are NOT displayed using
     *				this authorization.
     *
     *	CLI_AUTH_DIAG:		Commands that a system administrator might
     *				use to diagnose problems.
     *
     *	CLI_AUTH_UPDATE:	Commands that update the configuration of
     *				the unit.  Also allows configurations to be
     *				saved to external devices.
     *
     *	CLI_AUTH_PASSWD:	Authorizes passwords to included when
     *				configurations are saved.
     *
     *	CLI_AUTH_CODE:		Authorizes the upload of code to a unit.
     *
     *	CLI_AUTH_DEBUG:		Commands that are meant for internal use.
     *
     *	CLI_AUTH_MFG:		Commands that are used by manufacturing to
     *				prepare the box for customer use.  May be
     *				in addition  to the command's primary level.
     */
typedef Uint32			CliAuth;
# define CLI_AUTH_USER		0x00000001U
# define CLI_AUTH_TERMSERV	0x00000002U
# define CLI_AUTH_SYSTEM		0x00000004U
# define CLI_AUTH_DIAG		0x00000008U
# define CLI_AUTH_UPDATE		0x00000010U
# define CLI_AUTH_PASSWD		0x00000020U
# define CLI_AUTH_CODE		0x00000040U
# define CLI_AUTH_DEBUG		0x00000080U
# define CLI_AUTH_MFG		0x00000100U


    /*
     * CliHandle:
     *
     * An active terminal session is identified by a handle.  The handle
     * is defined here.
     */
typedef struct clisessioninfo*	CliHandle;

    /*
     * The first group of CLI functions start and stop (create and destroy)
     * instances of a command line terminal session.
     */

    /*
     * CliSessionType:
     *
     * There are various types of sessions within the system.  They
     * are enumerated here.
     *
     *	CLI_SESSION_CONSOLE:	The session attached to a shelf controller
     *				console port or slot card debug port at
     *				system/card startup. 
     *
     *	CLI_SESSION_DEBUG:	A session attached to a slot card over
     *				over the control bus.
     *
     *	CLI_SESSION_TELNET:	A remote console session.
     *
     *	CLI_SESSION_TERMSRV:	A session started on behalf of a user
     *				who has dialed into the unit.  This
     *				user runs the historical TERMSRV interface
     *				on top of the CLI.  It is hoped that this
     *				user will go away some day.
     *
     *	CLI_SESSION_TFTP_GET:	A TFTP session used to restore configuration.
     *
     *	CLI_SESSION_TFTP_PUT:	A TFTP session used to save configuration.
     *
     *	CLI_SESSION_CB_USER:	A user session over the control bus
     *				(eg. a dialin termsrv sess on slot card
     *				connects as this session type w shelf
     *				controller).
     *
     *	...
     *
     *	CLI_SESSION_NUM_TYPES:	The number of defined session types.
     */
typedef enum {
    CLI_SESSION_CONSOLE,
    CLI_SESSION_DEBUG,
    CLI_SESSION_TELNET,
    CLI_SESSION_TERMSRV,
    CLI_SESSION_TFTP_GET,
    CLI_SESSION_TFTP_PUT,
    CLI_SESSION_CB_USER,
    CLI_SESSION_NUM_TYPES
} CliSessionType;

    /*
     * cliStartSession:
     *
     * The function called to start a CLI session.
     *
     *	tag:			A value passed to the CliDrvStart function
     *				to help identify the session being started.
     *
     *	startFnc:		A pointer to the driver start function.
     *
     *	readFnc:		A pointer to the driver read function.
     *
     *	writeFnc:		A pointer to the driver write function.
     *
     *	writeCancelFnc:		A pointer to the driver write cancel
     *				function.
     *
     *	stopFnc:		A pointer to the driver stop function.
     *
     *  userProfile:		The name of the user profile to use for this 
     *				session.  If NULL, the user will be forced to
     *				log in.
     *
     *	loginSource:		A string identifying the source of this
     *				session.  Sources are "console", "modem"
     *				or the IP address of a remote user.
     *
     * Returns the CliHandle for the CLI session.  May return a null handle
     * if the session can not be started for any reason.
     */
extern CliHandle
cliStartSession( CliSessionType		sessionType,
		 Pointer		tag,
		 CliDrvStart*		startFnc,
		 CliDrvRead*		readFnc,
		 CliDrvWrite*		writeFnc,
		 CliDrvWriteCancel*	writeCancelFnc,
		 CliDrvStop*		stopFnc,
		 const char*		userProfile,
		 const char*		loginSource );


    /*
     * cliStopSession:
     *
     * The function called to perform a graceful shutdown of a CLI session.
     * The shutdown is not instantaneous.
     *
     *	handle:			The CLI handle of the session to stop.
     */
extern void
cliStopSession( CliHandle	handle );


    /*
     * Most CLI functions return a status to notify the caller if the
     * request succeeded or not.  The status is also used to tell the
     * caller if the session has been destroyed from under it.
     *
     *	CLI_STATUS_OK:		Request completed without error.
     *
     *	CLI_STATUS_TIMEOUT:	A read timeout occured, no data returned.
     *
     *	CLI_STATUS_NO_SESSION:	The session no longer exists, thus the
     *				request could not be serviced.
     *
     *	CLI_STATUS_NO_MEM:	The request could not be serviced due 
     *				to a memory allocation failure.
     *
     *	CLI_STATUS_DUP_CMD:	The requested command could not be 
     *				registered because a command of the same
     *				name already exists.
     *
     *	CLI_STATUS_BAD_PARAM:	The request could not be serviced due 
     *				to an invalid passed-in value.
     */
typedef enum {
    CLI_STATUS_OK,
    CLI_STATUS_TIMEOUT,
    CLI_STATUS_NO_SESSION,
    CLI_STATUS_NO_MEM,
    CLI_STATUS_DUP_CMD,
    CLI_STATUS_BAD_PARAM
} CliStatus;


    /*
     * The next group of CLI functions allow command handlers to register
     * with the command line interface.  The help, auth, and quit commands
     * are the only commands implemented within the CLI module itself  All
     * other commands are provided by external modules.
     */

    /*
     * CliHelpHandler:
     *
     * A function of this type is included in command registration.  The
     * function is called to provide help for the registered command.
     *
     *	handle:			The CliHandle of the session who requested
     *				help for the passed command.
     *
     *	cmdLine:		A pointer to the remainder of the command
     *				line.  If the input command were
     *					help xxx yyy
     *				this pointer will point to the first "x"
     *				in "xxx".
     */
typedef void
CliHelpHandler( CliHandle	handle,
	        char*		cmdLine );


    /*
     * CliCommandHandler:
     *
     * A function of this type must be registered by command handlers.
     * The function is called from the session's input task.  The command
     * is free to block this task, if desired.  If it is not
     *
     *	handle:			The handle of the session who requested
     *				the command that this output handler is for.
     *
     *	cmdLine:		A pointer to the remainder of the command
     *				line.  If the input was
     *					<command> xxx 
     *				this pointer will point to the first "x"
     *				in "xxx".  The command line data is for
     *				the use of the command and may be modified.
     */
typedef void
CliCommandHandler( CliHandle	handle,
	           char*	cmdLine );


    /*
     * CliDestroyHandler:
     *
     * A function of this type must be registered by command handlers.
     * The function is called when the CLI core whenever a handle is
     * destroyed.
     *
     *	handle:			The handle of the session that is being
     *				destroyed.  This handle will not be valid
     *				once the destroy functions for all
     *				registered command handlers have been called.
     *				
     */
typedef void
CliDestroyHandler( CliHandle	handle );


    /*
     * cliRegisterHandler:
     *
     * Add a command to the command line interface.  The name of the command
     * must be unique or the request is rejected.
     *
     *	cmdName:		The name of the command -- this is the
     *				string the user types to initiate the	
     *				command.
     *
     *	authorization:		A bit mask of the authorizations that can
     *				be used with this command.  If the none
     *				of the users authorizations match the
     *				commands authorization the command is not
     *				shown in the help nor can it be run by
     *				the user.
     *
     *	helpText:		A pointer to a string of help text.  May be
     *				NULL.
     *
     *	helpHandler:		The function called under control of the
     *				users input task to request help for this
     *				command.  The function may call cliQueueData
     *				for output.  This parameter may be NULL.
     *
     *	cmdHandler:		The function called under control of the
     *				users input task to process the command.
     *
     *	destroyHandler:		The function called to notify the command
     *				that a terminal has been destroyed.  This
     *				parameter may be NULL.
     *
     * A handler may register help text for the CLI to display, register a
     * callback so that it can implement something fancier, or both.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliRegisterHandler( const char*		cmdName,
		    CliAuth		authorization,
		    const char*		helpText,
		    CliHelpHandler*	helpHandler,
		    CliCommandHandler*	cmdHandler,
		    CliDestroyHandler*	destroyHandler );


    /*
     * The last group of CLI functions provide services to the CLIs
     * registered functions.  These are commands that will be needed
     * by most command handlers, and are therefore provided in a common
     * place.
     */

    /*
     * cliGetData:
     *
     * Simple blocking function to get 1 or more characters.  The function
     * will not return until a character is read or the terminal is
     * destroyed.
     *
     *	handle:			The CliHandle of the session to read
     *				the data from.
     *
     *	buffer:			A place to store the read data.
     *
     *	bufferLen:		The maximum number of characters that can be
     *				returned.  This count includes the meta-
     *				character; i.e., if the caller specifies
     *				bufferLen of 2, the most he can receive is
     *				two standard characters or one standard
     *				character and one meta-character.
     *					The function can return up
     *				to this number of characters, but may return
     *				less.  The value is updated with the number
     *				of standard characters (only) actually 
     *				returned.
     *
     *	metaChar:		This pointer is used to return protocol-
     *				specific data received by the driver which
     *				can't be represented as characters in the
     *				data stream.  The call may return a meta-
     *				character instead of or in addition to
     *				normal character data.  The meta-character
     *				trails in sequence any normal characters 
     *				returned in the same call.  A returned value
     *				of NULL indicates that no meta-character
     *				was received.  The caller may pass NULL for
     *				the pointer value to indicate that no meta
     *				characters should be returned.
     *
     * Note: this function does not translate vt100 escape sequences,
     *  use cliGetMeta() in order to receive this functionality.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliGetData( CliHandle		handle,
	    char*		buffer,
	    Int*		bufferLen,
	    CliMetaChar*	metaChar );


    /*
     * cliGetDataCooked:
     *
     * Similar to cliGetData, but this function will not return until
     * a line terminator (^C, ^J, or ^M) has been received.  In addition,
     * a prompt may be specified and the line is pre-processed by:
     *	^H		erases the previous character.
     *	DEL, ^D		erases the current character.
     *	^W		erases the previous ( space delimited ) word
     *	^U		erases the entire line
     *	^K		erases the line from the cursor position right
     *	^C		echoes ^C, terminates the input, and returns the
     *			control C as the only character input.
     *	^M or ^J	terminates the line.  The terminator is not added
     *			to the buffer.
     *	^P		replaces the line with the previous line entered
     *			for this session using this function.  20 previous
     *			lines are kept.
     *	^N		selects the next line.  Only valid if ^L had been
     *			used to select a previous line.
     *	^B		backs up the cursor without deleting a character.
     *			Non-control characters typed will have the effect
     *			of inserting the typed character in the line.
     *	^F		moves the cursor one character to the right, unless
     *			at the end of a line.
     *
     *			vt100 arrow key escape sequences are also recognized
     *			as substitutes for the above four characters
     *
     *	^A		go to the beginning of the line
     *	^E		go to the end of the line
     *
     * All input is bounded by the given buffer length.  Inserted characters
     * that cause the data to exceed the given length result in the data
     * being truncated.
     *
     *	handle:			The CliHandle of the session to read
     *				the data from.
     *
     *	prompt:			The prompt for this input line.
     *
     *	echo:			TRUE if data entered should be echoed
     *				back to the user.  Passwords entry, for
     *				example, will set this field to FALSE.
     *
     *	buffer:			A pointer to the place to store the
     *				cooked input line.
     *
     *	bufferLen:		The original size of buffer.  Updated with
     *				the number of characters input upon
     *				return.
     *
     * Note: All output generated by this function is performed while under
     * the control of the output task.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliGetDataCooked( CliHandle	handle,
		  const char*	prompt,
		  Bool		echo,
		  char*		buffer,
		  Int*		bufferLen );


    /*
     * cliGetDataTimeout:
     *
     * Similar to cliGetData, but a timeout may be specified so the
     * function will return.  A timeout of 0 will return immediately.
     * The function returns as soon as any characters are available;
     * it does not wait to fill up the buffer.
     *
     *	handle:			The CliHandle of the session to read
     *				the data from.
     *
     *	timeout:		The number of milliseconds to wait for
     *				input before returning.  If zero, the
     *				function does not block, but returns
     *				immediately.
     *
     *	buffer:			A place to store the read data.
     *
     *	bufferLen:		The maximum number of characters that can be
     *				returned.  This count includes the meta-
     *				character; i.e., if the caller specifies
     *				bufferLen of 2, the most he can receive is
     *				two standard characters or one standard
     *				character and one meta-character.
     *					The function can return up
     *				to this number of characters, but may return
     *				less.  The value is updated with the number
     *				of standard characters (only) actually 
     *				returned.
     *
     *	metaChar:		This pointer is used to return protocol-
     *				specific data received by the driver which
     *				can't be represented as characters in the
     *				data stream.  The call may return a meta-
     *				character instead of or in addition to
     *				normal character data.  The meta-character
     *				trails in sequence any normal characters 
     *				returned in the same call.  A returned value
     *				of NULL indicates that no meta-character
     *				was received.  The caller may pass NULL for
     *				the pointer value to indicate that no meta
     *				characters should be returned.
     *
     * Note: this function does not translate vt100 escape sequences,
     *  use cliGetMetaTimeout() in order to receive this functionality.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliGetDataTimeout( CliHandle	handle,
		   Uint32	timeout,
		   char*	buffer,
		   Int*		bufferLen,
	    	   CliMetaChar*	metaChar );


    /*
     * CliDataSent:
     *
     * A function of this type must be passed with queued output requests.
     * The function is called once the physical output has occurred.
     *
     *	handle:			The CliHandle of the session the output
     *				was for.
     *
     *	tag:			An opaque data type passed with the output
     *				request.  May be used by the caller to help
     *				identify which of several requests.
     *
     *	buffer:			A pointer to the buffer that was sent.
     *
     *  writeResult:		The result of the write, contains the CLI
     *				session status as defined above.
     */
typedef void
CliDataSent( CliHandle		handle,
	Pointer			tag,
	const char*		buffer,
	CliStatus		writeResult);

    /*
     * CliBufNotifyFnc:
     *
     * A function of this type can be passed in cliBufAvail().
     * Function is called when queued byte count drops below
     * HALF the given threshold loaded via cliBufAvail().
     * This is a one-shot callback.  Use cliBufAvail() to
     * re-load if necessary.
     *
     * NOTE: MUST match TermSrvBufAvailFnc!!!
     *
     *	tag:			Opaque data of value only to original
     *				requestor.  Usually use to identify
     *				the requestor.
     */
typedef void
CliBufNotifyFnc( Pointer	tag );


    /*
     * cliQueryQueuedBytes:
     *
     * Returns the number of bytes queued for output but not yet
     * processed by the CLI output task.
     *
     *	sessionHandle:		CliHandle of the session we're querying.
     */
Uint32
cliQueryQueuedBytes( CliHandle	sessionHandle );


    /*
     * cliBufAvail:
     *
     *	Returns 0 if the number of bytes queued equals or exceeds
     *	the given threshold 'queuedWriteThresh'; otherwise returns
     *	the difference.
     *
     *	If 0 and callback func 'bufNotifyFnc' is supplied then
     *	callback will be invoked when queued count drops below
     *	HALF the threshold.
     *
     *	Purpose of this facility is to impose some limits on queue
     *	usage since the CLI subsystem doesn't have such restrictions.
     *
     *	sessionHandle:		CliHandle of the session we're querying.
     *
     *	queuedWriteThresh:	Max number of queued bytes allowed.
     *				If exceeded func rets 0 and will
     *				inform requestor via 'bufNotifyFnc'
     *				when count drops below threshold.
     *
     *	bufNotifyFnc:		Callback funct to invoke when buffer
     *				space becomes available.
     *				If 0 then this func degenerates to
     *				query for amount of free space left.
     *
     *	bufNotifyTag:		Tag to supply on callback.
     */
Uint16
cliBufAvail( CliHandle		sessionHandle,
	     Uint32		queuedWriteThresh,
	     CliBufNotifyFnc*	bufNotifyFnc,
	     Pointer		bufNotifyTag );

    /*
     * cliQueueData:
     *
     * Schedule a data write to the output device.  This is a non-blocking
     * function.  The data is queued for output.  Output is done while under
     * control of the output task.
     *
     *	handle:			The CliHandle of the session the output
     *				is for.
     *
     *	buffer:			A pointer to the data to output.
     *
     *	bufferLen:		The number of bytes to output.
     *
     *	dataSentFnc:		Pointer to the function to call to notify
     *				the caller once the data has been sent.
     *
     *	tag:			A pointer passed back to the caller when the
     *				dataSentFnc is called.  Can be used by the
     *				caller to help identify which output just
     *				completed.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliQueueData( CliHandle	handle,
	      const char*	buffer,
	      Int		bufferLen,
	      CliDataSent*	dataSentFnc,
	      Pointer		tag );

	
    /*
     * cliPutData:
     *
     * A simple function to write data to the output device, blocking until
     * the data has been written.
     *
     *	handle:			The CliHandle of the session the output
     *				is for.
     *
     *	buffer:			A pointer to the data to output.
     *
     *	bufferLen:		The number of bytes to output.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliPutData( CliHandle		handle,
	    const char*		buffer,
	    Int			bufferLen );


    /*
     * cliBufferData:
     *
     * Buffer data for later output.  This function is non-blocking but should 
     * not be called from an interrupt context.
     * The data is queued for output when the buffer fills up, cliFlushData() 
     * is called, or no more than 100ms has passed.
     * 
     *	handle:			The CliHandle of the session the output
     *				is for.
     *
     *	buffer:			A pointer to the data to output.
     *
     *	bufferLen:		The number of bytes to output.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliBufferData( CliHandle	handle,
	       const char*	buffer,
	       Int		bufferLen );


    /*
     * cliFlushData:
     *
     *	Immediately queue for data output buffered by cliBufferData().  Should
     *  not be called from an interrupt context.
     * 
     *	handle:			The CliHandle of the session the output
     *				is for.
     *
     *	buffer:			A pointer to the data to output.
     *
     *	bufferLen:		The number of bytes to output.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliFlushData( CliHandle	handle );


    /*
     * CliMoreData:
     *
     * A function of this type is passed to cliMore to get the next line
     * for paged output.
     *
     *	handle:			The CliHandle of the session that is
     *				doing paged output.
     *
     *	tag:			A tag that was received in the original
     *				request to do paged output.
     *
     *	dataLen:		A pointer to a place to store the length
     *				of the returned string.
     *
     * Function returns a pointer to the start of the next line to
     * output, null if there are no more lines.  When non-null dataLen
     * is updated with the size the line.  The more function copies this
     * data, so it need not be maintained by this function.
     */
typedef const char*
CliMoreData( CliHandle		handle,
	     Pointer		tag,
	     Int*		dataLen );


    /*
     * CliMoreEnd:
     *
     * A function of this type is passed to cliMore.  It is called when
     * output is completed or cancelled.
     *
     *	handle:			The CliHanfdle of the session that is
     *				doing paged output.
     *
     *	tag:			A tag that was received in the original
     *				request to do paged output.
     */
typedef void
CliMoreEnd( CliHandle		handle,
	    Pointer		tag );

    /*
     * cliMore:
     *
     * A function that outputs a page worth of data then pauses for user
     * input before continuing with the next page.  The function calls
     * the passed CliMoreData function to get the next line to output.
     *
     * Up to linesPerPage lines (where a line is whatever was returned
     * by the moreDataFnc) are displayed, then the "more" function
     * waits for user input.
     *
     * <control C>	cancels futher output.
     *
     * <space>		causes the next page to be displayed
     *
     * <return>		caused the next line to be displayed.
     *
     * The function continues until all data has been ouput or output
     * is cancelled.
     *
     *	handle:			The handle of the session to output
     *				data to.
     *
     *	linesPerPage:		The number of lines per page for this
     *				invocation.  Specify CLI_DEFAULT_LINES_PER_PAGE
     *				to receive the session default.
     *
     *	moreDataFnc:		The address of the function to call to
     *				get the next line if data to output.
     *
     *	moreEndFnc:		The address of the function to call when
     *				output is complete or has been cancelled.
     *				NULL may be passed to indicate that a final
     *				call-back is not required.
     *
     *	tag:			An opaque data type passed back to the
     *				caller in the moreDataFnc and moreEndFnc.
     *
     * Note: All output generated by this function is performed while under
     * the control of the output task.
     *
     * Returns the CLI session status as defined above.   The moreEndFnc
     * will not be called unless a status of CLI_STATUS_OK is returned.
     */
extern CliStatus
cliMore( CliHandle		handle,
	 Int			linesPerPage,
	 CliMoreData*		moreDataFnc,
	 CliMoreEnd*		moreEndFnc,
	 Pointer		tag );

    /*
     * cliDump:
     *
     * Similiar to cliMore() except that it does not page the output
     */
extern CliStatus
cliDump( CliHandle		handle,
	 CliMoreData*		dumpDataFnc,
	 CliMoreEnd*		dumpEndFnc,
	 Pointer		tag );

# define CLI_DEFAULT_LINES_PER_PAGE	0


    /*
     * cliGetMeta:
     *
     * Similar to cliGetData(), but it returns translated
     * driver-specific and vt100-escape meta-characters.
     *
     *	handle:			The CliHandle of the session to read
     *				the data from.
     *
     *	buffer:			A place to store the read data.
     *
     *	bufferLen:		The maximum number of *characters* that can be
     *				returned.  The function can return up
     *				to this number of characters, but may return
     *				less.  The value is updated with the number
     *				of characters actually returned.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliGetMeta( CliHandle	handle,
	   CliMetaChar	*buffer,
	   Int*		bufferLen );


    /*
     * cliGetMetaTimeout:
     *
     * Similar to cliGetDataTimeout, but it returns translated
     * driver-specific and vt100-escape meta-characters.
     *
     * A timeout of 0 will return immediately.
     * The function returns as soon as any characters are available;
     * it does not wait to fill up the buffer.
     *
     *	handle:			The CliHandle of the session to read
     *				the data from.
     *
     *	timeout:		The number of milliseconds to wait for
     *				input before returning.  If zero, the
     *				function does not block, but returns
     *				immediately.
     *
     *	buffer:			A place to store the read data.
     *
     *	bufferLen:		The maximum number of *characters* that can be
     *				returned.  The function can return up
     *				to this number of characters, but may return
     *				less.  The value is updated with the number
     *				of characters actually returned.
     *
     * Returns the CLI session status as defined above.
     */
extern CliStatus
cliGetMetaTimeout( CliHandle	handle,
		   Uint32	timeout,
		   CliMetaChar*	buffer,
		   Int*		bufferLen );


    /*
     * cliPrintf:
     *
     * Similar to printf(), but the CLI session handle is specified
     */
extern int
cliPrintf( CliHandle sessionHandle, const char *format, ... );


    /*
     * cliSessionsActive
     *
     * Returns TRUE if any sessions are active.  Since the console
     * session is always active on a shelf controller, this function
     * can be used to detect if the console has been initialized, yet.
     */
extern Bool
cliSessionsActive( void );


    /*
     * cliSetSessionInfo:
     *
     * This function allows a CLI client to associate data with a
     * a session handle.  Specify a keyword and a pointer you wish
     * to later retrieve.  If a keyword is already storing a pointer,
     * it is overwritten.
     */
CliStatus
cliSetSessionInfo( CliHandle	sessionHandle,
		   const char*	key,
	   	   Pointer	dataPtr );


    /*
     * cliGetSessionInfo:
     *
     * This function allows a CLI client to retrieve data associated with a
     * a session handle.  Specify the keyword for the data you wish
     * to retrieve.
     *
     * Returns the pointer to the data, or NULL if no data has been registered
     * under this keyword.
     */
Pointer
cliGetSessionInfo( CliHandle	sessionHandle,
		   const char*	key );


    /*
     * cliLevelIsAuthorized:
     *
     * Returns TRUE if the specified security authorization is in effect for
     *	the session.
     */
extern Bool
cliLevelIsAuthorized( CliHandle	sessionHandle,
		      CliAuth	authLevel );


    /*
     * cliSizeParameter:
     *
     * Determines the number of characters in the blank- or
     * zero-delimited field
     *
     *	cmdLine: 	pointer to the beginning of a parameter
     *
     *	doSetChars:	if TRUE, '=' and '?' are treated as field delimiting 
     *			characters
     *
     *	doBraces:	if TRUE, '{', '}', '[', and ']' are treated as field 
     *			delimiting characters, and spaces between matched
     *			braces or brackets are not treated as delimiters
     *
     * Returns: the number of characters before a space or zero occurs,
     *   as modified by the above rules
     */
extern Int
cliSizeParameter( const char*	cmdLine,
		  Bool		doSetChars,
		  Bool		doBraces );


    /*
     * cliSkipParameter:
     *
     * Finds the next blank- or zero-delimited field after the current one
     *
     *	cmdLine: 	pointer to the beginning of a parameter
     *
     *	doSetChars:	if TRUE, '=' and '?' are treated as field delimiting 
     *			characters
     *
     *	doBraces:	if TRUE, '{', '}', '[', and ']' are treated as field 
     *			delimiting characters, and spaces between matched
     *			braces or brackets are not treated as delimiters
     *
     * Returns: a pointer to the next blank- or zero- delimited field after the
     * 	current one, as modified by the above rules.  Returns a pointer to
     *  NULL if the end of the buffer is reached
     */
extern char*
cliSkipParameter( const char*	cmdLine,
		  Bool		doSetChars,
		  Bool		doBraces );


    /*
     * cliNextToken:
     *
     *  Given a string containing space-delimited tokens, return a pointer 
     *  to the first token, null-terminated, and a pointer to the remainder
     *  of the string, if any.
     *
     *	cmdLine: 	pointer to the token
     *
     *	remainder:	if not NULL, updated with a pointer to the next token.
     *			Updated with NULL if this is the last token in the 
     *			buffer.
     *			
     * Returns: the passed in pointer
     */
extern char*
cliNextToken( char*	cmdLine,
	      char**	remainder );
    

    /*
     * cliYesNoPrompt:
     *
     *  Prompt a CLI user for a yes/no response, returning the result.  This
     *  function blocks until a character is received from the user.  It is
     *	assumed the caller has already output text posing a question to the
     *  user.  The received response character is echoed, followed by CR/LF.
     *
     *	sessionHandle: 	specifies the CLI session
     *
     * Returns: TRUE if the first character the user typed was "Y" or "y",
     *	otherwise FALSE.
     */
extern Bool
cliYesNoPrompt( CliHandle	sessionHandle );


    /*
     * cliClearScreen:
     *
     *  Blank the specified session's screen by sending a special VT100 escape
     *  sequence, and move the cursor to the home position.
     *
     *	sessionHandle: 	specifies the CLI session
     *
     *  reset:		if TRUE, also send the sequence to reset the terminal's
     *			VT100 attributes
     */
extern void
cliClearScreen( CliHandle	sessionHandle,
		Bool		reset );

    /*
     * cliTermIsDestroyed:
     *
     *	Determine if the sessionHandle in question is still valid.
     *
     * Returns: TRUE if the session is no longer around,
     *	otherwise FALSE.
     */
extern Boolean
cliTermIsDestroyed( CliHandle	sessionHandle );


    /*
     * cliUserName:
     *
     *  Provide the name of the user profile associated with this CLI session.
     *
     *	sessionHandle: 	specifies the CLI session
     *
     * Returns: a pointer to the NULL-terminated name string name, or to a 
     *	NULL string if no profile is associated with the session.
     */
extern const char *
cliUserName( CliHandle	sessionHandle );

    /*
     * cliSessionType:
     *
     *  Provide the type associated with this CLI session.
     *
     *	sessionHandle: 	specifies the CLI session
     *
     * Returns: the type of the session
     */
extern CliSessionType
cliSessionType( CliHandle	sessionHandle );

    /*
     * cliLoginSource:
     *
     *  Provide a description of the souce of this CLI session.
     *
     *	sessionHandle: 	specifies the CLI session
     *
     * Returns: a pointer to the NULL-terminated description string, or to a 
     *	NULL string if no source info is availabe for the session.
     */
extern const char *
cliLoginSource( CliHandle	sessionHandle );

    /*
     * cliUpdateStatusScreen:
     *
     *  Used internally by the CLI to tell the CLI output task to 
     *  redraw the status screen.
     *
     *	sessionHandle: 	specifies the CLI session
     */
extern void
cliUpdateStatusScreen( CliHandle	sessionHandle );

    /*
     * cliPreventBackgroundUpdates:
     *
     *  For use by command handlers that wish to completely take over all
     *  screen output.  Tells the CLI to block all output from other sources
     *  to the specified session.
     *
     *	sessionHandle: 	specifies the CLI session
     */
extern void
cliPreventBackgroundUpdates( CliHandle	sessionHandle );

    /*
     * cliAllowBackgroundUpdates:
     *
     *  Cancels a previous call to cliPreventBackgroundUpdates().
     *
     *	sessionHandle: 	specifies the CLI session
     */
extern void
cliAllowBackgroundUpdates( CliHandle	sessionHandle );

# if ( TNT_CARD & SHELF_CARD )

#  include "mibaddr.h"
#  include "miblog.h"

    /*
     * cliPrintLogEntry:
     *
     * Print log messages to users who have requested messages as long
     * as the level is not less than the users level.
     *
     *	logMsgLevel:		level of this message
     *
     *	shelfNum:		shelf where the message originated
     *
     *	slotNum:		slot where the message originated
     *
     *	msgText:		String to print
     *
     *	msgLen:			Length of string to send
     */
extern void
cliPrintLogEntry( LogMsgLevel		logMsgLevel,
		  TntShelfNumber	shelfNum,
		  TntSlotNumber		slotNum,
		  const char*		msgText,
		  Int			msgLen );

# endif /* #if ( TNT_CARD & SHELF_CARD ) */

    /*
     * Does synchronous output to local console port.
     * Interrupts are disabled during this process.
     * For slot cards, output is done only if UART
     * is present.
     *
     * Use for debugging purposes.
     */
extern void
cliSndUrgentStrF( const char *format, ... );

    /*
     * CliSessionStateChange:
     *
     * There are various types of state changes reported by a 
     * CliSessionStateChanged() function.
     *
     *	CLI_SESSION_UP:		A new session was created.
     *
     *	CLI_SESSION_DOWN:	The session was destroyed.
     *
     *	CLI_SESSION_AUTH:	The session changed to a new user-indentity/
     *				authentication level.
     */
typedef enum {
    CLI_SESSION_UP,
    CLI_SESSION_DOWN,
    CLI_SESSION_AUTH
} CliSessionStateChange;

    /*
     * CliSessionStateChanged:
     *
     * The type of a function registered with the CLI.  The
     * function will be called whenever a console is created and destroyed
     * or otherwise changes state.
     *
     *	sessionHandle:	The handle of the session that changed state.
     *
     *	stateChange:	The type of state change that has occurred.
     */
typedef void
CliSessionStateChanged( CliHandle		sessionHandle,
			CliSessionStateChange	stateChange );

    /*
     * cliRegisterForSessionChg:
     *
     * Register for CLI session state changes.
     *
     *	stateChangedFnc:	Pointer to the function to call to process
     *				state changes.
     */
extern CliStatus
cliRegisterForSessionChange( CliSessionStateChanged*	stateChangedFnc );

#endif /* CLI_H */
