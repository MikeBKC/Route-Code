/*
 * $Id: clidrv.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *      Copyright (c) 1995 UTT Technologies, Inc.
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

#if ! defined( CLIDRV_H )
# define CLIDRV_H
 
    /*
     * Many physical input/output drivers may exist to supply the low
     * level functions needed by the command line interface (CLI).  They
     * include various UARTs, telnet, UTT    remote management, etc.
     * They all implement this interface.
     */


    /*
     * An instance of a driver is identified by its handle.  The handle
     * is defined as:
     */
typedef Pointer			CliDrvHandle;

    /*
     * CliDrvStatus:
     *
     * A session may terminate in an unfriendly manner -- the driver
     * "goes away" as may happen with a a telnet daemon driver, modem
     * driver, etc.   Any blocking call will un-block and return when
     * the driver terminates.  These calls return this status type to
     * inform the caller of the reason for returning.
     *
     *	CLI_DRV_OK:		The I/O request proceeded in a normal
     *				manner.  Data was read or written.
     *
     *	CLI_DRV_TIMEOUT:	A read request terminated due to timeout.
     *
     *	CLI_DRV_BUSY:		A write request failed because a previous
     *				request has not yet terminated.
     *
     *	CLI_DRV_NO_DEVICE:	A read/write request failed because the
     *				instance of the driver (identified by the
     *				CliDrvHandle) does not exist any more.
     */
typedef enum {
    CLI_DRV_OK,
    CLI_DRV_TIMEOUT,
    CLI_DRV_BUSY,
    CLI_DRV_NO_DEVICE
} CliDrvStatus;

    /*
     * CliDrvStart:
     *
     * A function of this type is provided by a driver to perform any
     * required initialization before session I/O may start.
     *
     *	tag:			An opaque piece of data used by the driver
     *				to identify the session to start.  This
     *				value was passed to the CLI with the request
     *				to initiate a terminal session.
     *
     * Function returns the CliDrvHandle assigned to the session.  A null
     * handle may be returned if the session can not be started.
     */
typedef CliDrvHandle
CliDrvStart( Pointer		tag );


    /*
     * CliMetaChar:
     *
     * A driver may receive input that does not translate to an ASCII
     * character.  Also, a command handler may wish to have vt100 escape
     * sequences detected and translated.  CLI meta-characters are defined
     * here for these purposes.
     */
typedef enum {
    CLI_NO_META_CHAR = 0,
    CLI_LAST_ASCII = 255,
    CLI_TELNET_SYNCH,
    CLI_TELNET_BREAK,
    CLI_TELNET_INTERRUPT_PROCESS,
    CLI_TELNET_ABORT_OUTPUT,
    CLI_TELNET_ARE_YOU_THERE,
    CLI_TELNET_ERASE_CHARACTER,
    CLI_TELNET_ERASE_LINE,
    CLI_MODEM_BREAK,
    /* 
     * the following values are for use by the CLI core, not drivers
     */
    CLI_VT100_UP_ARROW,
    CLI_VT100_DN_ARROW,
    CLI_VT100_LF_ARROW,
    CLI_VT100_RT_ARROW,
    CLI_VT100_HOME,
    CLI_VT100_INS,
    CLI_VT100_END,
    CLI_VT100_PAGE_UP,
    CLI_VT100_PAGE_DN,
    CLI_VT100_UNKNOWN
} CliMetaChar;


    /*
     * CliDrvRead:
     *
     * A function of this type is provided by a driver to return data
     * read from the data source.
     *
     *	handle:			The CliDrvHandle of the session that data
     *				should be read from.
     *
     *	timeout:		The number of milliseconds to wait for
     *				data.  The call may block for up to this
     *				number of milliseconds.  If no data is
     *				received within this time period a timeout
     *				result is returned.  NOTE: A timeout of "1"
     *				will not block, but will return immediately.  
     *				This allows a caller to "poll" for the presence
     *				of data.  A timeout of "0" will not
     *				time out but will wait indefinitely.
     *
     *	buffer:			A pointer to a place to store the received
     *				data.
     *
     *	bufferLen:		Up to this many characters can be returned from
     *				a single call to this function.  If fewer
     *				are available the function will return
     *				what it can, it will only block if there is
     *				no data available.
     *					This count includes the meta-
     *				character; i.e., if the caller specifies
     *				bufferLen of 2, the most he can receive is
     *				two standard characters or one standard
     *				character and one meta-character.
     *					The function can return up
     *				to this number of characters, but may return
     *				less.  The value is updated with the number
     *				of standard characters (only) actually 
     *				returned.  For the examples above, bufferLen
     *				would be updatade to 2 and 1, respectively.
     *
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
     * Returns the driver status for the passed handle as described above.
     */
typedef CliDrvStatus
CliDrvRead( CliDrvHandle	handle,
	    Uint32		timeout,
	    char*		buffer,
	    Int*		bufferLen,
	    CliMetaChar*	metaChar );

# define CLI_DRV_RD_POLLED_MODE		1
# define CLI_DRV_RD_DONT_TIMEOUT		0

    /*
     * CliDrvWriteStatus:
     *
     * A function of this type may be given to the driver with write
     * requests if it wishes notification of write complete but does
     * not whish to block.  This function is called to return the
     * status of the write.
     *
     *	handle:			The handle of the driver whose write
     *				terminated.
     *
     *	tag:			An opaque piece of data passed to the
     *				driver with the write request.
     *
     *	status:			The status of the write request.  See
     *				above.
     */
typedef void
CliDrvWriteStatus( CliDrvHandle	handle,
		   Pointer	tag,
		   CliDrvStatus	status );


    /*
     * CliDrvWrite:
     *
     * A function of this type is provided by the driver to output data
     * on the device associated with the passed handle.  The caller can
     * chose if the write is to be blocking or non-blocking.  There can
     * only be 1 queued request for output.  It is the callers
     * responsibility to insure this.  Requests to output data while
     * busy outputing data for the same handle will be rejected with
     * a status of CLI_DRV_BUSY.
     *
     *	handle:			The CliDrvHandle of the sesion that the
     *				output data is for.
     *
     *	writeStatus:		The optional address of a function to call
     *				when the write completes.  If null, then
     *				this function blocks until the write is
     *				complete.  If not null the function does
     *				not block, but calls the passed function
     *				with the write status.
     *
     *	tag:			An opaque data type passed to the writeStatus
     *				function to help identify the session whose
     *				write completed.
     *
     *	buffer:			A pointer to the data to write.
     *
     *	bufferLen:		The length of the data to write.
     *
     * Returns the driver status for the passed handle as described above.
     */
typedef CliDrvStatus
CliDrvWrite( CliDrvHandle	handle,
	     CliDrvWriteStatus*	writeStatus,
	     Pointer		tag,
	     const char*	buffer,
	     Int		bufferLen );

    /*
     * CliDrvWriteCancel:
     *
     * A function of this type is provided by the driver to cancel an
     * output request.  The active write request for the handle (if any)
     * is terminated.  The writeStatusFnc for this data is NOT called.
     *
     *	handle:			Handle of the session who's output is
     *				to be cancelled.
     *
     * Returns the driver status for the passed handle as described above.
     */
typedef CliDrvStatus
CliDrvWriteCancel( CliDrvHandle	handle );


    /*
     *
     *	DRIVER_TERMINATE	Terminate driver.  All I/O halted and
     *				driver resources may be freed.
     *
     *	DRIVER_STOP_INPUT	Terminate all input activity.  Driver
     *				resources ARE NOT freed. Usually done
     *				before DRIVER_TERMINATE to halt input
     *				side first.
     */
typedef enum {
    DRIVER_TERMINATE,
    DRIVER_STOP_INPUT,
} CliDrvStopOpts;
    /*
     * CliDrvStop:
     *
     * Stop the driver session indicated by the passed handle.  As a result
     * of calling this function:
     *
     *	- Any blocked receive function will be re-started and return a
     *	  status of CLI_DRV_NO_DEVICE.
     *	- Any pending output will be cancelled and, if for a non-blocking
     *	  write the write status function will be called with a status
     *	  of CLI_DRV_NO_DEVICE.
     *	- Any blocked write function will be re-started and return a
     *	  status of CLI_DRV_NO_DEVICE.
     *
     * The CliDrvHandle is no longer valid upon return of this function.
     */
typedef void
CliDrvStop( CliDrvHandle	handle,
	    CliDrvStopOpts	stopOpts );

#endif /* CLIDRV_H */
