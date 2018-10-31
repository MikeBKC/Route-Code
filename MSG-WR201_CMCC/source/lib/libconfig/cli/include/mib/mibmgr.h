/*
 * $Id: mibmgr.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
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

#if ! defined( MIBMGR_H )
# define MIBMGR_H

# include "timedate.h"
# include "mibgen.h"

# if ( Q_TNT_SHELF_CARD )
#  if ! defined( MIBERROR_H )
#   include "miberror.h"
#  endif
# endif

# undef MIBMGR_TEST

    /*
     * The MIB (Management Information Base) Manager is responsible for
     * handling all requests for management information.  Primary clients
     * of this module are SNMP and the CLI (Command Line Interface).
     *
     * SNMP identifies management data by an object ID where the object
     * ID consists of two parts, an item identifier and an index.  The
     * item identifier specifies the exact field desired; the index
     * specifies the instance of the field.  Where the field is unique the
     * index is specified as zero.
     *
     * SNMP object IDs are hierarchical, with related items placed at the
     * same level of the hierarchy.  Items at the same level that only
     * define a single instance are termed a group.  Items that define
     * multiple instances are termed a table.  The SNMP MIB compiler
     * assumes that all items in the same group are processed by the same
     * function.  This is also true for items in the same table.
     *
     * This lends itself to mapping groups and tables to profiles.  The
     * difference is that there is only 1 profile per group, where a
     * table has many profiles, one per unique index.
     *
     * SNMP indexes for standard MIBs are varied.  MIB-II, for example,
     * uses SNMP interface number, IP addresses, strings, and object
     * itentifiers.  UTT    specific MIBs, at least those that are
     * associated with a specific hardware type, will tend to use the port
     * number as an index.
     *
     * The CLI will also use port numbers in addition to profile names.
     * Profiles, such as connection or filter profiles, are named as they
     * are not permanently associated with a specific port and may live
     * in an external database where the name is the access key.
     *
     * Profiles live in different places according to the type of data
     * stored.   Persistent data may be stored in local non-volatile memory
     * or may be stored in an external database.  Non-persistent data such
     * as usage statistics, instantaneous state, etc., is stored on the
     * device that updates the data.  This is to minimize card-to-card
     * traffic.
     *
     * This module, then, takes a request for a type of profile and a
     * profile index, converts the index from an external to internal
     * form where required, determines where the profile lives, and fetches
     * the profile.  Since all requests come through this module, some
     * cacheing may occur to improve performance.  Requests to write
     * a profile will only be accepted for internal profiles.  PROFILES
     * RETRIEVED FROM AN EXTERNAL DATABASE CAN NOT BE UPDATED.
     *
     * There are two versions of this module, one that runs on a shelf
     * controller and one that runs on slot cards.  They communicate
     * with each other over the control bus.  CALLS TO FUNCTIONS IN THIS
     * MODULE MAY BLOCK -- EVEN WHEN RUNNING ON THE MASTER SHELF CONTROLLER
     * AS THE PROFILE BEING REQUESTED MAY LIVE ON AN EXTERNAL DEVICE.
     * For this reason, there are two interfaces to each function, one that
     * is simple, but can block, and another non-blocking mechanism that
     * utilizes callbacks and allows a request to be cancelled.
     */

    /*
     * MibResult:
     *
     * An enumerated type that specifies the result of a request to the
     * MIB manager.
     *
     *	MIB_RESULT_OK:		The request completed and the response
     *				data area (see mibRequest) contains
     *				valid data.
     *
     *	MIB_RESULT_NO_MEM:	The request could not be serviced due to 
     *				a failure in allocating memory.
     *
     *	MIB_RESULT_DUP_NAME:	The request could not be serviced because it
     *				would duplicate an existing name.
     *
     *	MIB_RESULT_DUP_TYPE:	The request could not be serviced because it
     *				would duplicate an existing profile type.
     *
     *	MIB_RESULT_BAD_PARAM:	The request could not be serviced due to 
     *				an invalid parameter.
     *
     *	MIB_RESULT_NOT_FOUND:	The requested item was not present.
     *
     *	MIB_RESULT_NEED_INDEX:	The request for a profile must specify the 
     *				index.
     *
     *	MIB_RESULT_NOT_INDEXED:	The request for a profile should not specify
     *				an index.
     *
     *	MIB_RESULT_NAME_AMBGS:	The request for an item by name could not be
     *				serviced because the name was ambiguous.
     *
     *	MIB_RESULT_OUT_OF_SEQ:	The request could not be serviced; the profile
     *				to be written is not current with the one
     *				being replaced.
     *
     *	MIB_RESULT_READ_ONLY:	The request could not be serviced; the profile
     *				to be written is not writable.
     *
     *	MIB_RESULT_NO_FACTORY:	The request could not be serviced; the profile
     *				does not have a registered factory function.
     *
     *	MIB_RESULT_TOO_BIG:	The request could not be serviced; the profile
     *				does not fit in the buffer provided by the user.
     *
     *	MIB_RESULT_BAD_TYPE:	The request could not be serviced; the 
     *				requested profile type is unrecognized.
     *
     *	MIB_RESULT_CB_FAILURE:	The request could not be serviced due to
     *				a problem using the control bus.
     *
     *	MIB_RESULT_DUPLICATE:	The request could not be serviced; there is 
     *				a profile from a different source with a 
     *				duplicate index.
     */
typedef enum {
    MIB_RESULT_OK,
    MIB_RESULT_NO_MEM,
    MIB_RESULT_DUP_NAME,
    MIB_RESULT_DUP_TYPE,
    MIB_RESULT_BAD_PARAM,
    MIB_RESULT_NOT_FOUND,
    MIB_RESULT_NEED_INDEX,
    MIB_RESULT_NOT_INDEXED,
    MIB_RESULT_NAME_AMBGS,
    MIB_RESULT_OUT_OF_SEQ,
    MIB_RESULT_READ_ONLY,
    MIB_RESULT_NO_FACTORY,
    MIB_RESULT_TOO_BIG,
    MIB_RESULT_BAD_TYPE,
    MIB_RESULT_CB_FAILURE,
    MIB_RESULT_DUPLICATE,
    MIB_RESULT_NO_INSTANCE
} MibResult;

/*
 *  MibProfileInstance
 * 
 *	this structure is used to specify profiles by clients of mibmgr
 */
typedef struct mibinstance*	MibProfileInstance;

    /*
     * MibUpdateRegistrationId:
     *
     * MIB requests to be notified of profile udate events 
     * are identified by this type, with a new MibRegistrationId
     * assigned to each request.  The requestId is used to 
     * let the caller cancel an update-notification request.
     */
typedef Pointer			MibUpdateRegistrationId;

    /*
     * MibPriority:
     *
     * Various MIBs may live in various places.  We want the ability
     * to prefer one location over another.  Profile sources specify
     * their priority at registration time.  Ordering within the same
     * priority level is not specified.
     *
     *	MIB_PRIORITY_FIRST:		Try these locations first.
     *
     *	MIB_PRIORITY_ANY:		Try these locations if not found
     *					at the first location.
     *
     *	MIB_PRIORITY_LAST:		Try these locations only if the
     *					profile could not be found in
     *					one of the above locations.
     */
typedef enum {
    MIB_PRIORITY_FIRST,
    MIB_PRIORITY_ANY,
    MIB_PRIORITY_LAST
} MibPriority;

    /*
     * The format of a sequence number used for MIB overwrite detection
     * as described above.
     */
typedef Uint32			MibSequence;

# define MIB_NO_SEQUENCE		0xFFFFFFFF
# define MIB_LOWEST_SEQUENCE	0


    /*
     * MibFactoryFnc:
     *
     * A function of this type is registered with the MIB manager to provide 
     * a way for it to get default values for indexed profiles of a
     * given type
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The desired index of the profile.  If the 
     *				profile is a one-of-a-kind profile then this
     *				should be NULL.  NULL will also be specified
     *				if the caller wants a default index.  
     *				     If non-NULL, the function may wish
     *				to vary the values of other profile fields
     *				based on the index.  
     *				     Note: the index field value is actually 
     *				set by the MIBMGR and may be ignored by
     *				the function.
     *
     *	mibIndexLen:		The length of the index value.
     *
     * Returns:  a pointer to a profile with default values
     */
typedef ConstPtr
MibProfileFactoryFnc( MibProfileType	mibType,
		      ConstPtr		mibIndex,
		      Int		mibIndexLen );

    /*
     * MibProfileReadFnc:
     *
     * A function of this type is registered with the MIB manager to read
     * a profile of a given type and index.  The function may live on
     * a different slot card (in which case the address of the slot card
     * is passed to the registered control bus receive function).  The MIB
     * manager uses this function internally to read profiles.
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The index of the profile.  If the profile
     *				is a one-of-a-kind profile then this may
     *				be null.
     *
     *	mibIndexLen:		The length of the index value.
     *
     *	mibSequence:		Returns the sequence number of the profile 
     *				read.  A value of MIB_NO_SEQUENCE
     *				indicates the profile does not record sequence
     *				numbers.
     *
     *	data:			The address of the place to store the
     *				requested profile.
     *
     *	dataLen:		The length of data in octets.  If not
     *				large enough to store the requested profile
     *				the read fails.
     *
     * Returns MIB_RESULT_OK if the profile was found and returned, 
     * other values indicating an error condition.
     */
typedef MibResult
MibProfileReadFnc( MibProfileType	mibType,
		   ConstPtr		mibIndex,
		   Int			mibIndexLen,
	           MibSequence*		mibSequence,
		   Pointer		data,
		   Int			dataLen );

    /*
     * MibProfileWriteFnc:
     *
     * A function of this type is registered with the MIB manager by
     * modules that support profile writing -- modules may support
     * reading but not writing.  The function may live on a shelf controller
     * or a slot card.  This function is used by the MIB manager to update
     * profiles.
     *
     *	mibType:		The type of profile to update.
     *
     *	mibSequence:		The sequence number of the profile the profile
     *				to be written was based on.  The profile source
     *				should ensure it does not overwrite a newer
     *				version.  
     *				     A value of MIB_NO_SEQUENCE indicates
     *				the source should write the new profile
     *				regardless of sequence number.
     *				     A value of MIB_LOWEST_SEQUENCE indicates
     *				the source should never overwrite an existing
     *				profile.
     *				     Returns the new sequence number of the
     *				updated profile.
     *
     *	data:			A pointer to the data to write.
     *
     *	dataLen:		The length of the data to write in octets.
     *				If not the length of the profile to write,
     *				the update fails.
     *
     * Function returns MIB_RESULT_OK if the update was successful, other
     * values indicating an error condition.  MIB_RESULT_OUT_OF_SEQ indicates
     * the writer's profile is out of date as described above.
     */
typedef MibResult
MibProfileWriteFnc( MibProfileType	mibType,
		    Uint		mibIndexField,
	            MibSequence*	mibSequence,
		    ConstPtr		data,
		    Uint		dataLen );

    /*
     * MibProfileDeleteFnc:
     *
     * A function of this type is registered with the MIB manager to erase
     * a profile of a given type and index.  The function may live on
     * a different slot card (in which case the address of the slot card
     * is passed to the registered control bus receive function).  The MIB
     * manager uses this function internally to delete profiles.
     *
     *	mibType:		The type of profile to delete.
     *
     *	mibIndex:		The index of the profile.  If the profile
     *				is a one-of-a-kind profile then this may
     *				be null.
     *
     *	mibIndexLen:		The length of the index value.
     *
     * Returns MIB_RESULT_OK if the profile was found and deleted, 
     * other values indicating an error condition.
     */
typedef MibResult
MibProfileDeleteFnc( MibProfileType	mibType,
		     ConstPtr		mibIndex,
		     Int		mibIndexLen );

    /*
     * MibProfileInfoFnc:
     *
     * A function of this type is registered with the MIB manager to get
     * info on a profile of a given type and index.  The function may live on
     * a different slot card (in which case the address of the slot card
     * is passed to the registered control bus receive function).  The MIB
     * manager uses this function internally to get profile info.
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The index of the profile.  If the
     *				profile is a one-of-a-kind profile then this
     *				this parameter should be null.
     *
     *	mibIndexLen:		The length of the index value.
     *
     *	mibSequence:		Returns the current sequence number 
     *				of the profile, unless the parameter is NULL.
     *
     *	dataLen:		Returns the amount of storage consumed by
     *				the profile in bytes, unless the parameter 
     *				is NULL.
     *
     *	modDate:		Returns the time and date the profile was 
     *				last modified, unless the parameter is NULL.
     *
     * Returns MIB_RESULT_OK if the profile was found and its info returned, 
     * other values indicating an error condition.
     */
typedef MibResult
MibProfileInfoFnc( MibProfileType	mibType,
		   ConstPtr		mibIndex,
		   Int			mibIndexLen,
	           MibSequence*		mibSequence,
		   Int*			dataLen,
		   AbsoluteTime*	modDate );

    /*
     * MibProfileIndexFnc:
     *
     * A function of this type is registered with the MIB manager to iterate
     * the indexes of a given profile type.  The function may live on
     * a different slot card (in which case the address of the slot card
     * is passed to the registered control bus receive function).  The MIB
     * manager uses this function internally to find a profile's indexes.
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		Pointer to the previous index value.  The
     *				storage it points to is updated with the 
     *				new index value.
     *
     *	mibIndexLen:		The length of the index value/storage.
     *
     *  cookie:			Pointer to an opaque 32-bit dataum passed 
     *				to the index function on succesive calls;
     *				this datum is allocated by the caller.
     *
     *  getFirst:		If true, the first index is returned (the
     *				value at mibIndex is ignored).
     *
     * Returns: MIB_RESULT_OK if successful, MIB_RESULT_NOT_FOUND if there
     *   are no more indexes; other values indicate an error occurred.
     */
typedef MibResult
MibProfileIndexFnc( MibProfileType	mibType,
		    Pointer		mibIndex,
		    Int			mibIndexLen,
		    Uint32*		cookie,
		    Bool		getFirst );

    /*
     * mibRegisterProfile
     *
     * Profiles live in many places.  Profile sources must register with the
     * MIB manager so it knows where to go to retrieve profiles and where
     * to go to update profiles.  All registrants for a slot will be
     * un-registered if that slot should go down.  The slot may re-register
     * when running again.
     *
     *	mibType:		The type of profile being registered.
     *
     *	mibName:		The official name of the profile.
     *
     *	mibDescription:		A description of the profile's function.
     *
     *	mibSize			The size of the profile in memory.
     *
     *	mibIndexField:		The number of the field in the profile which
     *				stores its index.  MIB_NO_MATCH indicates that
     *				the profile is singular.
     *
     *	mibInfoField:		The number of the field in the profile which
     *				stores its info field.  MIB_NO_MATCH indicates
     *				no info field exists for the profile.
     *
     *	mibFactoryFnc:		The address of the function to call to get a
     *				default profile of the above type.
     *
     * Note: the above parameters are used only for the first registrant for
     *	a specific type; otherwise they are ignored.  The parameters below
     *	are significant for all registrants.
     *
     *	mibSrcName:		A name to identify the source.
     *
     *	mibSrcSavable:		If TRUE, the source's profiles should be
     *				be included in the saving of configuration.
     *
     *	mibPriority:		The priority of this specific registrant.
     *
     *	mibReadFnc:		The address of the function to call to
     *				read profiles of the above type.
     *
     *	mibWriteFnc:		The address of the function to call to
     *				write profiles of the above type.  This
     *				function may be NULL if profile updating
     *				is not available.
     *
     *	mibDeleteFnc:		The address of the function to call to
     *				delete profiles of the above type.  This
     *				function may be NULL if profile deletion
     *				is not available.
     *
     *	mibInfoFnc:		The address of the function to call to
     *				get information on profiles.
     *
     *	mibIndexFnc:		The address of the function to call to
     *				iterate the instances of an indexed profile
     *				type.  This function should be NULL for 
     *				singular profile types and for profile
     *				sources that don't report their indexes.
     *
     * Returns:  if successful, returns MIB_RESULT_OK
     */
extern MibResult
mibRegisterProfile( MibProfileType		mibType,
		    const char*			mibName,
		    const char*			mibDescription,
		    Uint			mibSize,
		    Uint			mibIndexField,
		    Uint			mibInfoField,
		    MibProfileFactoryFnc*	mibFactoryFnc,
		    const char*			mibSrcName,
		    Boolean			mibSrcSavable,
		    MibPriority			mibPriority,
		    MibProfileReadFnc*		mibReadFnc,
		    MibProfileWriteFnc*		mibWriteFnc,
		    MibProfileDeleteFnc*	mibDeleteFnc,
		    MibProfileInfoFnc*		mibInfoFnc,
		    MibProfileIndexFnc*		mibIndexFnc );

    /*
     * mibProfileSize:
     *
     * Returns information about the profile as a whole, specifically
     * the amount of memory required to hold the uncompressed profile.
     *
     *	mibType:		The type of profile who's name is desired.
     *
     * Returns:  the number of bytes required to hold the profile; 
     * zero if the type is unknown.
     */
extern Uint
mibProfileSize( MibProfileType	mibType );

    /*
     * mibProfileIndexField:
     *
     * Provides the field number of the profile which is the index.
     *
     *	mibType:		The type of profile who's name is desired.
     *
     * Returns: the index field number, or MIB_NO_MATCH if the profile type
     *	is unknown or not indexed.
     */
extern Uint
mibProfileIndexField( MibProfileType	mibType );

    /*
     * mibProfileInfoField:
     *
     * Provides the field number of the profile which is the info field.
     *
     *	mibType:		The type of profile who's name is desired.
     *
     * Returns: the info field number, or MIB_NO_MATCH if the profile type
     *	is unknown or not indexed.
     */
extern Uint
mibProfileInfoField( MibProfileType	mibType );

    /*
     * mibTypeToName:
     *
     * A profile-to-name/name-to-profile mapping table exists within the
     * MIB manager.  This table can be queried.
     *
     *	mibType:		The type of profile who's name is desired.
     *
     * Returns a pointer to the name.  If the mibType is unknown,
     * NULL will be returned.
     */
extern const char*
mibTypeToName( MibProfileType	mibType );


    /*
     * mibNameToType:
     *
     * Given a pointer to a null terminated name find the profile type
     * that matches the name.  Case is not significant in the comparison
     * and a prefix of the full name will match, providing that it is
     * unambiguous.
     *
     *	mibName:		Null terminated name of the profile
     *				whose type is desired.
     *
     * Returns the MibProfileType that matches the passed name.  If no
     * match can be found then MIB_PROF_MAX is returned.
     */
extern MibProfileType
mibNameToType( const char*	mibName );


# ifdef NOT_USED

    /*
     * MibReqTypeToName:
     *
     * A structure of this type is passed to mibRequest to perform the
     * non-blocking version of mibTypeToName.  The return value is
     * placed in the requestStatus field of the structure.
     */
typedef struct {
    MibProfileType	mibType;
    char*		mibName;
    Int*		mibNameLen;
    Bool		requestStatus;
} MibReqTypeToName;

    /*
     * MibReqNameToType
     *
     * A structure of this type is passed to mibRequest to perform the
     * non-blocking version of mibNameToType.  The return value is
     * placed in the mibType field of the structure.
     */
typedef struct {
    char*		mibName;
    MibProfileType	mibType;
} MibReqNameToType;

# endif /* NOT_USED */

    /*
     * Several profile access functions are provided, depending upon the
     * indexes available.  Each profile access function is associated
     * with a profile update function.  To help detect the possibility
     * of two users updating the same profile, a sequence number is
     * assigned to each profile type, updated whenever the profile is
     * written.  This number is returned when profiles are read and
     * required when profiles are written.  A warning can then be issued
     * to the writer if out of sequence.  The writer may override the
     * warning.
     *
     * Profile access comes in two forms: the first returns a copy of the
     * profile and the second returns a pointer to the profile.  Since the
     * profile may not exist on system the pointer must be to temporary
     * memory.  Therefore, a release function is provided that must be
     * called when the user of a profile is finished to allow resources
     * to be released.
     */

    /*
     * mibPointerRelease:
     *
     * Release any memory used by the passed profile pointer.  Actually,
     * a reference counting scheme should be used so the memory is not
     * freed until the last user has released it.  This function exists
     * on all cards and does not block.
     *
     *	profile:		A pointer to a profile.
     */
extern void
mibPointerRelease( ConstPtr	profile );


    /*
     * mibPointerByIndex:
     *
     * Return a pointer to a profile of the indicated type and index.
     * May be called on slot cards.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles that
     *				do not have an index.
     *
     *	mibIndexLen:		The length of the index.
     *
     *	profileData:		Updated to point to a read-only copy of the
     *				profile data.  The data should be returned 
     *				by mibPointerRelease().
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibPointerByIndex( MibProfileType	mibType,
		   ConstPtr		mibIndex,
		   Int			mibIndexLen,
		   ConstPtr*		profileData );

    /*
     * mibFactoryPointer:
     *
     * Return a pointer to the factory profile of the indicated type.
     * May be called on slot cards.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles that
     *				do not have an index or if a default index
     *				is desired.
     *
     *	mibIndexLen:		The length of the index.
     *
     *	profileData:		Updated to point to a read-only copy of the
     *				profile data.  The data should be returned 
     *				by mibPointerRelease().
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibFactoryPointer( MibProfileType	mibType,
		   ConstPtr		mibIndex,
		   Int			mibIndexLen,
		   ConstPtr*		profileData );

    /*
     * mibDataByIndex:
     *
     * This function returns a copy of a profile to a buffer provided by
     * the caller.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The index of the profile being requested.
     *				Should be NULL for one-of-a-kind profiles
     *				that do not require an index.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     *	data:			Pointer to a location in memory where the
     *				profile can be copied to.
     *
     *	dataLen:		The maximum number of octets that can be
     *				written to data.
     *
     * Returns:	MIB_RESULT_OK if successful.  Returns MIB_RESULT_TOO_BIG if
     *   the caller's buffer is insufficient to hold the entire profile; 
     *   partial profiles are not returned.
     */
extern MibResult
mibDataByIndex( MibProfileType	mibType,
	        ConstPtr	mibIndex,
	        Int		mibIndexLen,
	        Pointer		data,
	        Int		dataLen );

    /*
     * mibFactoryData:
     *
     * Return a copy of the factory profile of the indicated type to
     * a buffer allocated by the caller.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles that
     *				do not have an index or if a default index
     *				is desired.
     *
     *	mibIndexLen:		The length of the index.
     *
     *	data:			Pointer to a location in memory where the
     *				profile can be copied to.
     *
     *	dataLen:		The maximum number of octets that can be
     *				written to data.
     *
     * Returns:	MIB_RESULT_OK if successful.  Returns MIB_RESULT_TOO_BIG if
     *   the caller's buffer is insufficient to hold the entire profile; 
     *   partial profiles are not returned.
     */
extern MibResult
mibFactoryData( MibProfileType	mibType,
	        ConstPtr	mibIndex,
	        Int		mibIndexLen,
	        Pointer		data,
	        Int		dataLen );

    /*
     * mibWriteProfile:
     *
     * Write out the provided profile copy.  May be called on slot cards.
     * This function will *always* overwrite an existing profile
     * of the same type and index.  THIS FUNCTION MAY BLOCK.
     *
     * Note: the index of the profile to be written is read from the
     *  profile itself.
     *
     *	mibType:		The type of profile being written.
     *
     *	data:			A pointer to the data to write.
     *
     *	dataLen:		The number of octets to be written,
     *				(must match size of profile).
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibWriteProfile( MibProfileType	mibType,
	         ConstPtr	data,
	         Int		dataLen );


    /*
     * mibWriteProfileCb:
     *
     * Write out the provided profile to master shelf. 
     * This function will *always* overwrite an existing profile
     * of the same type and index.
     *
     * Note: the index of the profile to be written is read from the
     *  profile itself.
     *
     *	mibType:		The type of profile being written.
     *
     *	data:			A pointer to the data to write.
     *
     *	dataLen:		The number of octets to be written,
     *				(must match size of profile).
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibWriteProfileCb( MibProfileType	mibType,
	         ConstPtr	data,
	         Int		dataLen );

# if ( Q_TNT_SHELF_CARD )
    /*
     * mibWriteErrorProfile:
     *
     * Write out the provided error profile copy.  This function only exists
     * on the shelf controller.  This function will *always* overwrite an
     * existing profile of the same type and index.  THIS FUNCTION MAY BLOCK.
     *
     * Note: the index of the profile to be written is read from the
     *  profile itself.
     *
     *	mibType:		The type of profile being written.
     *
     *	data:			A pointer to the data to write.
     *
     *	dataLen:		The number of octets to be written,
     *				(must match size of profile).
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibWriteErrorProfile( MibProfileType	mibType,
		      ErrorProfile	*data,
		      Int		dataLen );
# endif

    /*
     * mibDeleteByIndex:
     *
     * This function deletes the specified profile.  May be called on slot
     * cards.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile to be deleted.
     *
     *	mibIndex:		The index of the profile to be deleted.
     *				Should be NULL for one-of-a-kind profiles
     *				that do not require an index.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibDeleteByIndex( MibProfileType	mibType,
	          ConstPtr		mibIndex,
	          Int			mibIndexLen );

    /*
     * mibGetNextPointer:
     *
     * Called iteratively to obtain a pointer to each profile of the indicated 
     * type.  May be called on slot cards.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibInstance:		An iteration handle obtained by calling
     *				mibAllocInstance().
     *
     *	profileData:		Updated to point to a read-only copy of the
     *				profile data.  The data should be returned 
     *				by mibPointerRelease().
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibGetNextPointer( MibProfileType	mibType,
		   MibProfileInstance   mibInstance,
		   ConstPtr*		profileData );

    /*
     * mibGetNextPointerMasked:
     *
     * Called iteratively to obtain a pointer of each profile of the
     * indicated type whos masked index matches a given index.
     * May be called on slot cards.  THIS FUNCTION MAY BLOCK.
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibInstance:		An iteration handle obtained by calling
     *				mibAllocInstance().
     *
     *	profileData:		Updated to point to a read-only copy of the
     *				profile data.  The data should be returned 
     *				by mibPointerRelease().
     *
     *	indexMask:		A pointer to a mask used to determine which
     *				fields of the profile's index will compare
     *				to determine if the function should return
     *				a pointer to the profile.  The mask is
     *				assumed to be the same length as the
     *				index.  The profile's index is masked
     *				one byte at a time with this value before
     *				being compared with the given index.
     *
     *	index:			The index to compare with the profiles
     *				masked value.
     *
     *  indexLength:		The length of the index in bytes
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibGetNextPointerMasked( MibProfileType		mibType,
			 MibProfileInstance	mibInstance,
			 ConstPtr*		profileData,
			 ConstPtr		indexMask,
			 ConstPtr		index,
			 Int			indexLength );

    /*
     * mibGetNextData:
     *
     * Called iteratively to obtain each profile of the indicated 
     * type.  May be called on slot cards.  THIS FUNCTION MAY BLOCK. 
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibInstance:		An iteration handle obtained by calling
     *				mibAllocInstance().
     *
     *	data:			Pointer to a location in memory where the
     *				profile can be copied to.
     *
     *	dataLen:		The maximum number of octets that can be
     *				written to data.
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibGetNextData( MibProfileType		mibType,
		MibProfileInstance	instance,
	        Pointer			data,
	        Int			dataLen );

    /*
     * mibGetNextDataMasked:
     *
     * Called iteratively to obtain each profile of the
     * indicated type who's masked index matches a given index.
     * May be called on slot cards.  THIS FUNCTION MAY BLOCK.
     *
     *	mibType:		The type of profile being requested.
     *
     *	mibInstance:		An iteration handle obtained by calling
     *				mibAllocInstance().
     *
     *	data:			Pointer to a location in memory where the
     *				profile can be copied to.
     *
     *	dataLen:		The maximum number of octets that can be
     *				written to data.
     *
     *	indexMask:		A pointer to a mask used to determine which
     *				fields of the profile's index will compare
     *				to determine if the function should return
     *				a pointer to the profile.  The mask is
     *				assumed to be the same length as the
     *				index.  The profile's index is masked
     *				one byte at a time with this value before
     *				being compared with the given index.
     *
     *	index:			The index to compare with the profiles
     *				masked value.
     *
     *  indexLength:		The length of the index in bytes
     *
     * Returns:	MIB_RESULT_OK if successful.
     */
extern MibResult
mibGetNextDataMasked( MibProfileType		mibType,
		      MibProfileInstance	mibInstance,
		      Pointer			data,
		      Int			dataLen,
		      ConstPtr			indexMask,
		      ConstPtr			index,
		      Int			indexLength );

# if ( Q_TNT_SHELF_CARD )
    /*
     * mibInitializeLocalInstance:
     *
     * Used in place of mibAllocInstance for initializing MibInstance
     * structures during fatal errors on the shelf controller.
     *
     * _instance: A a pointer to a MibProfileInstance structure.
     *
     * returns: _instance
     *
     */
extern MibProfileInstance
mibInitializeLocalInstance( MibProfileInstance );
# endif

    /*
     * mibAllocInstance:
     *
     * Used to allocate a new instance structure with standard defaults
     *
     * Returns: a pointer to the new instance.  The system crashes with
     * a fatal error 2 if there was no memory available.
     */
extern MibProfileInstance
mibAllocInstance( void );

    /*
     * mibSetInstanceType:
     *
     * Used to associate a profile type with an instance
     *
     *	instance:		The instance structure whose type shold be set
     *
     *  mibType:		The profile type to be associated
     *
     * Returns: MIB_RESULT_OK if the type is known, otherwise 
     *	MIB_RESULT_BAD_TYPE
     */
extern MibResult
mibSetInstanceType( MibProfileInstance	instance,
		    MibProfileType	mibType );

    /*
     * mibReturnInstance:
     *
     * Used to dispose of an instance structure allocated by mibAllocInstance
     *
     *	instance:		The instance structre to be freed.
     *
     * Returns: nothing
     */
extern void
mibReturnInstance( MibProfileInstance	instance );

    /*
     * mibGetNextType:
     *
     * Used to get find out what profile types there are
     *
     *	instance:		A profile newly allocated instance used as an
     *				iteration handle.  Updated to specify the type.
     *
     * Returns: a pointer to the profile type's name, or NULL if no
     *   more profiles.
     */
extern const char*
mibGetNextType( MibProfileInstance	instance );

    /*
     * mibProfileUpdate
     *
     * Used to register with shelf (via control bus message) that
     * we want notified of profile updates.
     *
     *	mibType:		Profile type wanted.
     *
     * Returns: TRUE if request made it off of shelf.
     *
     */
extern Bool
mibProfileUpdate( MibProfileType mibType );	

    /*
     * mibFindTypeByName:
     *
     * Given a pointer to a null terminated name, find the profile type
     * that matches the name.  Case is not significant in the comparison
     * and a prefix of the full name will match, providing that it is
     * unambiguous.
     *
     *	mibName:		Null-terminated name string.
     *
     *	instance:		A profile instance.  Updated to specify
     *				the found type.
     *
     * Returns:  if successful, returns MIB_RESULT_OK
     */
extern MibResult
mibFindTypeByName( const char*		mibName,
    	       	   MibProfileInstance	instance );

    /*
     * mibGetNextIndex:
     *
     * Used to get find out what profile indexes exist for an indexed profile
     * type
     *
     *	instance:		A profile instance with the profile type set
     *				used as an iteration handle.  Updated to specify
     *				the index.
     *
     *	mibIndexPtr:		Updated to point to the new index value.
     *
     *	mibIndexLenPtr:		Updated with the length of the index field.
     *
     * Returns: MIB_RESULT_OK if successful, MIB_RESULT_NOT_FOUND if there
     *   are no more indexes, other results indicating an error.
     */
extern MibResult
mibGetNextIndex( MibProfileInstance	iteration,
		 Pointer*		mibIndexPtr,
		 Int*			mibIndexLenPtr );

    /*
     * mibGetInfo:
     *
     * Used to get profile info data for profile instance
     *
     *	instance:		A profile instance with the profile type set
     *				used as an iteration handle.
     *
     *	mibInfoPtr:		Updated to point to the new info value.
     *
     *	mibInfoLenPtr:		Updated with the length of the info field.
     *
     * Returns: MIB_RESULT_OK if successful.
     */
extern MibResult
mibGetInfo( MibProfileInstance	iteration,
	    Pointer*		mibInfoPtr,
	    Int*		mibInfoLenPtr );

    /*
     * mibFindProfile:
     *
     * Used to find if a specific profile exists and initialize an instance
     * to point to it.
     *
     *	instance:		A profile instance with the profile type set.
     *				Updated to specify the found profile source.
     *
     *	mibIndex:		The index of the profile being requested.
     *				NULL for singular profiles.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     * Returns:  if successful, returns MIB_RESULT_OK
     */
extern MibResult
mibFindProfile( MibProfileInstance	instance,
		ConstPtr		mibIndex,
		Int			mibIndexLen );

    /*
     * mibIndexSet:
     *
     * Convert an index field value in display format to its internal
     * representation and save it in an independent structure allocated
     * by the routine for the caller.
     *
     *	instance:		A profile instance with the profile type set.
     *
     *	indexLen:		Updated with the size of the allocated index.
     *
     *	inputStr:		A pointer to the string containing the
     *				external (display) representation of the
     *				field.
     *
     *	inputStrLen:		The length of inputStr.
     *
     *	errorStr:		Optional: If non-null then a pointer to
     *				a place were error messages can be
     *				copied to if the set could not be
     *				performed.
     *
     *	errorStrLen:		A pointer to the length of the error	
     *				string (null when errorStr is null).
     *				Updated with the actual length of the
     *				string written when FALSE is returned.
     *
     * Returns:  if successful, returns pointer to the allocated structure;
     *	otherwise, returns NULL.
     */
extern Pointer
mibIndexSet( MibProfileInstance		instance,
	     Int*			indexLen,
	     const char*		inputStr,
	     Uint			inputStrLen,
	     char*			errorStr,
	     Uint*			errorStrLen );


    /*
     * mibIndexGet:
     *
     * Convert a profile index into a displayable format stored in the
     * passed output area.  The index may be part of a profile or in an
     * independent structure.
     *
     *	instance:		A profile instance with the profile type set.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.  NULL if the index is to
     *				come from an independent field.
     *
     *	index:			A pointer to the profile index to be displayed. 
     *				Ignored unless profile is NULL.
     *
     *	indexLen:		The length of the index specified by "index".
     *
     *	outputStr:		A pointer to a character string where the
     *				field will be output to.  The field is
     *				_not_ null terminated.
     *
     *	outputStrLen:		A pointer to the size of outputStr.  This
     *				value is updated with the actual number
     *				of bytes written to the string unless
     *				there is an error.
     *
     * Returns TRUE if the field was found and converted to a display
     * format, otherwise returns FALSE.
     */
extern Bool
mibIndexGet( MibProfileInstance		instance,
	     ConstPtr			profile,
	     ConstPtr			index,
	     Int			indexLen,
	     char*			outputStr,
	     Int*			outputStrLen );

    /*
     * mibIndexCheck:
     *
     * Given a pointer to a structure of a given mib type, compare 
     * the profile index  with an external copy to see if they are identical.
     *
     *	instance:		A profile instance with the profile type set.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     *	fieldCopy:		A pointer to an external copy of the field.
     *
     * Returns TRUE if the fields match, otherwise FALSE.
     */
extern Bool
mibIndexCheck( MibProfileInstance	instance,
	       ConstPtr			profile,
	       ConstPtr			indexCopy );

    /*
     * mibIndexDup:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * return a copy of the field in memory allocated by the function.
     *
     *	instance:		A profile instance with the profile type set.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     * Returns: a pointer to the allocated copy, otherwise NULL.
     */
extern Pointer
mibIndexDup( MibProfileInstance		instance,
	     ConstPtr			profile );

    /*
     * mibProfileInfo:
     *
     * Used to request miscellaneous information about a profile.
     *
     *	instance:		The profile instance for this query.
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles
     *				that do not require an index.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     *	mibSequence:		Returns the current sequence number 
     *				of the profile, unless the parameter is NULL.
     *				A value of MIB_NO_SEQUENCE indicates the 
     *				profile does not record sequence numbers.
     *
     *	dataLen:		Returns the amount of storage consumed by
     *				the profile in bytes, unless the parameter 
     *				is NULL.
     *
     *	modDate:		Returns the time and date the profile was 
     *				last modified, unless the parameter is NULL.
     *
     * Returns:  if successful, returns MIB_RESULT_OK
     */
extern MibResult
mibProfileInfo( MibProfileInstance	instance,
		ConstPtr		mibIndex,
		Int			mibIndexLen,
	        MibSequence*		mibSequence,
		Int*			dataLen,
		AbsoluteTime*		modDate );

    /*
     * mibProfileSourceName:
     *
     * Returns the descriptive name of the profile source.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  name of the profile source, NULL if unknown.
     */
extern const char *
mibProfileSourceName( MibProfileInstance	instance );

    /*
     * mibProfileIndexed:
     *
     * Tells whether the profile is indexed or singular.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  TRUE if the profile is indexed, FALSE if singular
     */
extern Bool
mibProfileIndexed( MibProfileInstance	instance );

    /*
     * mibProfileIndexLength:
     *
     * Tells the length of the profile's index field
     *
     *	instance:		The profile instance for this query.
     *
     * Returns: the lenght of the field in bytes; zero on error
     */
extern Uint
mibProfileIndexLength( MibProfileInstance	instance );

    /*
     * mibProfileSavable:
     *
     * Tells whether the profile should be included in the save output.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  TRUE if the profile is savable, FALSE otherwise
     */
extern Bool
mibProfileSavable( MibProfileInstance	instance );

    /*
     * mibProfileWritable:
     *
     * Tells whether the profile is read-only
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  TRUE if the profile is writable, FALSE otherwise
     */
extern Bool
mibProfileWritable( MibProfileInstance	instance );

    /*
     * mibProfileDeletable:
     *
     * Tells whether the profile can be deleted
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  TRUE if the profile is deletable, FALSE otherwise
     */
extern Bool
mibProfileDeletable( MibProfileInstance	instance );

    /*
     * mibProfileInstanceSize:
     *
     * Returns information about the profile as a whole, specifically
     * the amount of memory required to hold the uncompressed profile.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  the number of bytes required to hold the profile; 
     * zero if the type is unknown.
     */
extern Uint
mibProfileInstanceSize( MibProfileInstance	instance );

    /*
     * mibProfileType:
     *
     * Identifies the type of the profile instance.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  the profile type number
     */
extern MibProfileType
mibProfileType( MibProfileInstance	instance );

    /*
     * mibProfileName:
     *
     * Identifies the name that was registered for this type of profile.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  a pointer to the name string.
     */
extern const char*
mibProfileName( MibProfileInstance	instance );

    /*
     * mibProfileDescription:
     *
     * Used to retrieve the description registered for this type of profile.
     *
     *	instance:		The profile instance for this query.
     *
     * Returns:  a pointer to the description string.
     */
extern const char*
mibProfileDescription( MibProfileInstance	instance );

    /*
     * mibReadProfileInstance:
     *
     * Potentially blocking function used to get a copy of the specified
     * profile.  If successful, this function allocates memory which must
     * later be freed by the caller.
     *
     *	instance:		The profile instance for this query.
     *
     *	profilePtr:		Updated with a pointer to the profile data
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles
     *				that do not require an index.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     *	mibSequence:		The sequence number of the profile returned
     *				by the function.  A value of MIB_NO_SEQUENCE
     *				indicates the profile does not record sequence
     *				numbers.
     *
     * Returns:  MIB_RESULT_OK if the profile is successfully read and storage
     *  allocated, other results indicating an error condition
     */
extern MibResult
mibReadProfileInstance( MibProfileInstance	instance,
    			Pointer*		profilePtr,
		 	ConstPtr		mibIndex,
		 	Int			mibIndexLen,
	         	MibSequence*		mibSequence );

    /*
     * mibReadFactoryProfile:
     *
     * Supplies default values for an indexed profile.
     * If successful, this function allocates memory which must
     * later be freed by the caller.
     *
     *	instance:		The profile instance for this query.  Should
     *				be set with the desired profile type.
     *
     *	profilePtr:		Updated with a pointer to the profile data
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles that
     *				do not have an index or if a default index
     *				is desired.
     *
     *	mibIndexLen:		The length of the index.
     *
     * Returns:  MIB_RESULT_OK if the profile is successfully read and storage
     *  allocated, other results indicating an error condition
     */
extern MibResult
mibReadFactoryProfile( MibProfileInstance	instance,
    		       Pointer*		        profilePtr,
		       ConstPtr			mibIndex,
		       Int			mibIndexLen );

    /*
     * mibWriteProfileInstance:
     *
     * Potentially blocking function used to get a copy of the specified
     * profile.  If successful, this function allocates memory which must
     * later be freed by the caller.
     *
     *	instance:		The profile instance for this query.
     *
     *	profile:		A pointer to the profile data to be written
     *
     *	mibSequence:		The sequence number of the profile the profile
     *				to be written was based on.  The profile source
     *				should ensure it does not overwrite a newer
     *				version.
     *				     A value of MIB_NO_SEQUENCE indicates
     *				the new profile should be written 
     *				regardless of sequence number.
     *				     A value of MIB_LOWEST_SEQUENCE indicates
     *				the new profile should never overwrite an 
     *				existing profile.
     *				     Returns the new sequence number of the
     *				updated profile.
     *
     * Note:  the index of the profile to be written is inferred from 
     *   the profile's specified index field.
     *
     * Returns:  MIB_RESULT_OK if the profile is successfully written,
     *  other results indicating an error condition.
     *  MIB_RESULT_OUT_OF_SEQ indicates this profile has been updated
     *  and the operation would have overwritten those changes.
     */
extern MibResult
mibWriteProfileInstance( MibProfileInstance	instance,
    			 ConstPtr		profile,
	         	 MibSequence*		mibSequence );

    /*
     * mibDeleteProfileInstance:
     *
     * Potentially blocking function used to erase the specified
     * profile.
     *
     *	instance:		The profile instance for this request.
     *
     *	mibIndex:		The index of the profile being requested.
     *				May be null for one-of-a-kind profiles
     *				that do not require an index.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     * Returns:  MIB_RESULT_OK if the profile is successfully found and
     *  deleted, other results indicating an error condition
     */
extern MibResult
mibDeleteProfileInstance( MibProfileInstance	instance,
		 	  ConstPtr		mibIndex,
		 	  Int			mibIndexLen );

    /*
     * When a module wants to know about updates made to a part of the 
     * active configuration, it registers with this module (see below).
     * When a change is made, a function of the type MibProfileUpdateFnc
     * is invoked with the new configuration.
     * 
     * Parameters:
     *
     *	tag:			The value passed when the registration was
     *				done.
     *
     *	mibType:		The type of the profile being updated.
     *
     *	mibIndex:		The index of the profile being updated.
     *				Null for one-of-a-kind profiles
     *				that do not require an index.
     *
     *	mibIndexLen:		The length of the mib index.
     *
     *	oldProfileDataPtr:	Pointer to a _read_only_ copy of the profile
     *				data that is about to be updated.
     *				If wantFactory was FALSE in the 
     *				registration, NULL indicates that this is a 
     *				new profile.  Otherwise, this parameter will
     *				always point to either the old or factory data.
     *
     *	newProfileDataPtr:	Pointer to a _read_only_ copy of the 
     *				updated profile data.
     *				If wantFactory was FALSE in the 
     *				registration, NULL indicates that the profile
     *				was deleted.  Otherwise, this parameter will
     *				always point to either the old or factory data.
     */
typedef void 
MibProfileUpdateFnc( Pointer		tag,
		     MibProfileType	mibType,
		     ConstPtr		mibIndex,
		     Int		mibIndexLen,
		     ConstPtr		oldProfileDataPtr,
		     ConstPtr		newProfileDataPtr );


    /*
     * Register the call-back function passed to be called whenever the
     * profile of the type specified is updated.  The parameters are:
     *
     *	mibType:	The type of profile the caller wishes to receive 
     *			update notification for.
     *
     *	updateTag:	Tag to pass back as "tag" field in the call-back.
     *
     *	updateFnc:	Call-back function to call whenever the profile
     *			is actually updated.
     *
     *	wantFactory:	Determines what to do if an old or new profile doesn't
     *			exist (i.e, this is a new profile, or the profile was
     *			deleted).  TRUE specifies that the call-back return a 
     *			pointer to the factory default profile in this case.
     *
     * Returns: the registration ID used for cancellation if successful,
     *	otherwise, NULL.
     */
MibUpdateRegistrationId
mibRegisterForProfileUpdates( MibProfileType		mibType,
			      Pointer			updateTag,
			      MibProfileUpdateFnc*	updateFnc,
			      Bool			wantFactory );


    /*
     * Cancel a request for profile update notification made by calling
     * mibRegisterForProfileUpdates()
     *
     *	mibType:	The type of profile for which the caller registered 
     *			to receive update notification.
     *
     *	registrationId: The value returned by the original call to 
     *			mibRegisterForProfileUpdates().
     *
     * Returns: MIB_RESULT_OK if the registration is found and cancelled, any
     *	other result indicating and error.
     */
extern MibResult
mibCancelUpdateRegistration( MibProfileType		mibType,
			     MibUpdateRegistrationId	registrationId );

# if ( Q_TNT_SHELF_CARD )

    /*
     * Report a profile update to the MIBMGR.  This function is normally
     * used internally to the MIBMGR, but it can be used by entities managing
     * external profile sources.
     *
     * mibReportProfileUpdate ()
     *
     *	mibType:	The type of profile that was modified.
     *
     *	mibIndex:	The index of the profile being updated.
     *			NULL for one-of-a-kind profiles
     *			that do not require an index.
     *
     *	mibIndexLen:    The length of the mib index.
     *
     *  oldDataPtr:	Pointer to the old profile instance (NULL if a profile
     *			was created).
     *
     *  newDataPtr:     Pointer to the updated profile instance (NULL if a
     *		 	profile was deleted).
     */
extern MibResult
mibReportProfileUpdate( MibProfileType	mibType,
		      	ConstPtr	mibIndex,
		 	Int		mibIndexLen,
		        ConstPtr	oldDataPtr,
		        ConstPtr	newDataPtr );

# endif /* ( Q_TNT_SHELF_CARD ) */

# ifdef MIBMGR_TEST

extern MibResult
readFnc( MibProfileType	mibType,
		   ConstPtr		mibIndex,
		   Int			mibIndexLen,
	           MibSequence*		mibSequence,
		   Pointer		data,
		   Int			dataLen );

extern MibResult
writeFnc( MibProfileType		mibType,
	            MibSequence*	mibSequence,
		    ConstPtr		data,
		    Int			dataLen );

extern MibResult
infoFnc( MibProfileType	mibType,
		   ConstPtr		mibIndex,
		   Int			mibIndexLen,
	           MibSequence*		mibSequence,
		   Int*			dataLen,
		   AbsoluteTime*	creationDate );

extern MibResult
indexFnc( MibProfileType	mibType,
	  Pointer		mibIndex,
	  Int			mibIndexLen,
	  Bool			getFirst );

extern MibResult
deleteFnc( MibProfileType	mibType,
	   ConstPtr		mibIndex,
	   Int			mibIndexLen );

# endif /*  MIBMGR_TEST */

#endif /* MIBMGR_H */
