/*
 * $Id: mibgen.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
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

#if ! defined( MIBGEN_H )
# define MIBGEN_H

# include "mibtype.h"
# include "mibfld.h"

    /*
     * A MIB has four representations, depending upon where it is
     * being used.
     *
     *	1) RADIUS representation: A unique attribute for each field.  The
     *	   dictionary defines the name and type of the field.  String
     *	   fields are un-sized
     *
     *	2) User Interface representation.  Field names (similar, if not
     *	   exactly the same as their RADIUS counterparts), unique to each
     *	   profile (RADIUS names are unique system wide.  The name
     *	   translates to a type, length, and offset within a structure.
     *
     *	3) NVRAM representation.  A compact representation used within
     *	   internal non-volatile memory.
     *
     *	4) Internal representation.  Each profile is a structure.  The
     *	   structure is built from one of the other representations and
     *	   passed around the system.
     *
     * A MIB description file is processed by the program mibgen.  Mibgen
     * builds a header file containing the description of the structure
     * used for internal representation and an implementation file that
     * contains the translation table used to convert between
     * representations. 
     *
     * These functions can be considered part of the mib manager.
     */

    /*
     * MibLookup:
     *
     * A lookup table using entries if this type exists for each profile.
     * The table is used to convert a MIB field from one representation to
     * another.  Mib format translation tables are built from entries
     * of this type.  An entry with a fieldNumber of 0 MUST terminate
     * each table.
     *
     *	fieldNumber:		The field number assigned to this field
     *				within a given profile.  Field numbers are
     *				used to store profiles in local non-volatile
     *				memory.  There may be multiple entries for
     *				a given field number if a field has aliases.
     *				When converting from number to name the
     *				name returned is always the first name in
     *				the table.
     *
     *	fieldOffset:		The offset of the field in bytes from the
     *				beginning of the structure used for the
     *				internal representation.
     *
     *	fieldLength:		The length of the field in bytes.
     *
     *	numberOfElements:	This field indicates whether the field is
     *				actually an array.  A one indicates a non-
     *				array field, while a greater value indicates
     *				the number of elements in the array.
     *
     *	attributeNumber:	The RADIUS attribute number for this field.
     *				If a field has aliases, only the field name
     *				that matches the RADIUS attribute name
     *				will contain an attributeNumber.  A value
     *				of zero indicates this is not the name of
     *				a RADIUS attribute.
     *
     *	fieldName:		A pointer to a constant, null terminated
     *				ascii string which is one of the names of
     *				a field.
     *
     *	fieldAccess:		The access mode of the field, read-only or
     *				read-write.
     *
     *	fieldType:		The type of field (for data entry and
     *				display purposes).
     *
     *	fieldParams:		A pointer to the I/O data entry and display
     *				params for the field.  The format of the
     *				data varies according to the fieldType.
     *
     *	fieldHelp:		A pointer to a constant, null terminated
     *				ascii string which describes the field.
     */
typedef struct {
    Uint16			fieldNumber;
    Uint16			fieldOffset;
    Uint16			fieldLength;
# if ( USERIF & CLI )
    Uint32			numberOfElements;
    Uint16			attributeNumber;
    const char*			fieldName;
    MibFieldAccess		fieldAccess;
    MibFieldType		fieldType;
    MibFieldParams*		fieldParams;
    const char*			fieldHelp;
# endif
} MibLookup;

    /*
     * The following functions are provided to convert between various
     * mib formats.
     */

    /*
     * The definition of a field number guaranteed to not match any
     * valid field number.
     */
# define MIB_NO_MATCH		0L

    /*
     * mibNameToField:
     *
     * Given a profile type and a pointer to a field name return the field
     * number of the field within that profile.  The passed name will
     * match if it compares (case not significant) the name of a field
     * or a leading substring of the name of a field when the substring
     * is unambiguous.
     *
     *	mibType:		The type of mib.  Specifies which mib
     *				translation table to use.
     *
     *	fieldName:		Pointer to the name of the field.
     *
     *  ambiguous:		TRUE if the name was a valid match for more
     *				than one field.
     *
     * Returns the field number of the field matching the passed name
     * according to the match criteria.  Returns MIB_NO_MATCH if the
     * field can not be matched.
     */
extern Uint
mibNameToField( MibProfileType	mibType,
	        const char*	fieldName,
		Bool*		ambiguous );

# if ( USERIF & CLI )

    /*
     * mibAttributeToField:
     *
     * Convert a RADIUS attribute number to a profile type and field
     * number within the profile.
     *
     *	mibType:		A pointer to the type of mib.  If the
     *				dereferenced value is MIB_PROF_MAX, all
     *				profile types will be searched for the
     *				attribute and the dereferenced value
     *				updated with the profile type containing
     *				the attribute.  If the value specifies
     *				a particular profile, only that profile
     *				will be searced for the attribute number.
     *
     *	attributeNumber:	The RADIUS attribute number to look for.
     *
     * Returns the field number of the passed attribute.  If the attribute
     * could not be found MIB_NO_MATCH is returned.
     */
extern Uint
mibAttributeToField( MibProfileType*	mibType,
		     Uint		attributeNumber );



    /*
     * mibFieldToName:
     *
     * Return a pointer to the field name of the given profile/field number.
     *
     *	mibType:		The type of the profile who's field name
     *				is desired.
     *
     *	fieldNumber:		The field number within the profile of
     *				who's name is desired.
     *
     * Returns a constant pointer to the canonical name of the given field.
     * Zero is returned if the profile or field can not be found.
     */
extern const char*
mibFieldToName( MibProfileType	mibType,
	        Uint		fieldNumber );

# endif /* CLI */


    /*
     * mibOffsetToField:
     *
     * Return the field number of the given profile/field offset.
     *
     *	mibType:		The type of profile to get field info for.
     *
     *	fieldOffset:		The field offset of the field for which
     *				the lookup table entry is desired.
     *
     * Returns a constant integer representing the field number.
     *
     * NOTE: This function is intended for use by the upload/download
     * procedures.
     */
extern Uint
mibOffsetToField( MibProfileType	mibType,
		  Uint			fieldOffset );


    /*
     * mibFieldLength:
     *
     * Return the length of a field in a profile.
     *
     *	mibType:		Identifies the profile.
     *
     *	fieldNumber:		The number of the field within the profile
     *				whose length is desired.
     *
     * Returns the length of the field in octets.  If the profile or field
     * are unknown, returns a zero length.
     */
extern Uint
mibFieldLength( MibProfileType	mibType,
	        Uint		fieldNumber );

    /*
     * MibValidateProfile:
     *
     * A function of this type is registered for each profile type.  The
     * function may be called to validate a profile, typically before
     * the profile is written.
     *
     *	profile:		A pointer to the profile to validate.
     *
     *	message:		A pointer to a place to store error
     *				messages describing why the profile is
     *				not valid.  May be null if the caller does
     *				not desire error messages.  String is
     *				not changed if the profile is valid.
     *
     *	messageLen:		A pointer to the maximum size of message.
     *				this value is updated with the number of
     *				bytes written to message by the
     *				validation function if the profile is not
     *				valid.
     *
     * TRUE is returned if the profile is valid, FALSE if the profile
     * is not valid, i.e. not internally consistent.
     */
typedef Bool
MibValidateProfile( ConstPtr		profile,
		    char*		message,
		    Int*		messageLen );


    /*
     * mibRegisterTransTable:
     *
     * This function is called by the files that provide the mib translation
     * tables.  It registers a translation table to a MibProfileType.
     *
     *	mibType:		The type of mib that this translation table
     *				is for.
     *
     *	transTable:		A pointer to the translation lookup table
     *				for the above type.
     *
     *	mibValidFnc:		A pointer to the validation function for
     *				the profile type.
     *
     *	internalProfile:	A boolean value.  When set to TRUE indicates
     *				that instances of profiles of this type
     *				can not be created from SNMP/User Interface
     *				code.
     *
     * Returns TRUE if the tranlation table was accepted.  Returns FALSE
     * if a translation table already exists for the passed mib type.
     */
extern Bool
mibRegisterTransTable( MibProfileType		mibType,
		       const MibLookup		transTable[],
		       MibValidateProfile*	mibValidFnc,
		       Bool			internalProfile );


    /*
     * mibValidateProfile:
     *
     * Function to validate a profile.  The registered MibValidateProfile
     * function for the passed mibType is called to process the request.
     * See the MibValidateProfile type definition for more info.
     *
     *	mibType:		The type of profile to validate.
     *
     *	profile:		A pointer to the profile to validate.
     *
     *  profileLen:		The length of the profile to validate
     *
     *	message:		A pointer to a place to store error
     *				messages describing why the profile is
     *				not valid.  May be null if the caller does
     *				not desire error messages.  String is
     *				not changed if the profile is valid.
     *
     *	messageLen:		A pointer to the maximum size of message.
     *				this value is updated with the number of
     *				bytes written to message by the
     *				validation function if the profile is not
     *				valid.
     *
     * TRUE is returned if the profile is valid, FALSE if the profile
     * is not valid, i.e. not internally consistent.
     */
extern Bool
mibValidateProfile( MibProfileType	mibType,
		    ConstPtr		profile,
		    Uint		profileLen,
		    char*		message,
		    Int*		messageLen );

    /*
     * mibGetInfoNextField:
     *
     * Returns information about the next field in a profile, given
     * the ID of the previous field.  Fields are returned in random
     * order, not in sequence by field number.
     * Used to walk through a MIB profile, deconstructing it into fields.
     * 
     *
     *	mibType:		The mibType for this query.
     *
     *	fieldNumber:		The field number last examined;
     *				a value of 0 means "the first."
     *
     *	fieldLen:		A paramater to be set on return with the
     *				length of this field.
     *
     *	fieldOffset:		A paramater to be set on return with the
     *				offset within the profile struct of this field.
     *
     *	showInternal:		If FALSE, no information will be returned
     *				about fields designated as MIB_INTERNAL.
     *
     * Returns:  value of the field ID for which the information is
     * 	         returned;  a value of 0 is invalid for a field ID, and
     *           is returned when no next-field exists.
     */
extern Uint
mibGetInfoNextField( MibProfileType	mibType,
		     Uint		fieldNumber,
		     Uint		*fieldLen,
		     Uint		*fieldOffset,
		     Bool		showInternal );


    /*
     * mibGetNumberOfFields:
     *
     * Returns the number of fields in a MIB profile.
     * Used to allocate a working buffer large enough to handle the largest
     * conceivable NVRAM image of a given MIB profile.
     *
     *	mibType:		The type of profile.  
     *
     * Returns: the number of fields;
     *          0 for an invalid request of any kind.
     */
extern Uint
mibGetNumberOfFields( MibProfileType mibType );


    /*
     * mibGetFieldOffset:
     *
     * Returns the offset for a field, given the field ID.
     * 
     *
     *	mibType:		The type of profile containing the field.
     *
     *	fieldNumber:		The ID of the field for this query.
     *
     *	fieldOffset:		A param to be set on return with the offset 
     *				within the profile struct of this field.
     *
     * Returns:  TRUE if the information is returned,
     *           FALSE if no field with this ID found, etc..
     */
extern Bool
mibGetFieldOffset ( MibProfileType	mibType,
			  Uint			fieldNumber,
			  Uint*			fieldOffset );


# if ( USERIF & CLI )
    /*
     * mibGetFieldOffsetAndLen:
     *
     * Returns the offset and length for a field, given the field ID.
     * 
     *
     *	mibType:		The type of profile containing the field.
     *
     *	fieldNumber:		The ID of the field for this query.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields
     *				or to specify the whole of an array).
     *
     *	fieldOffset:		A param to be set on return with the offset 
     *				within the profile struct of this field.
     *
     *	fieldLen:		A param to be set on return with the length
     *				of this profile field.
     *
     * Returns:  TRUE if the information is returned,
     *           FALSE if no field with this ID found, etc..
     */
extern Bool
mibGetFieldOffsetAndLen ( MibProfileType	mibType,
			  Uint			fieldNumber,
	     		  Uint			elementNumber,
			  Uint*			fieldOffset,
		          Uint*			fieldLen );
# endif /* CLI */


    /*
     * mibGetFieldLookup:
     *
     * Return a pointer to the field entry in the lookup table for the
     * requested field.
     *
     *	mibType:		The type of profile to get field info for.
     *
     *	fieldNumber:		The field number of the field for which
     *				the lookup table entry is desired.
     *
     * Returns a pointer to the entry, 0 if the mib type ore field number
     * is invalid.
     *
     * NOTE: This function is intended for use by mibfld.c, a module
     * closely related to mibgen.
     */
extern const MibLookup*
mibGetFieldLookup( MibProfileType	mibType,
		   Uint			fieldNumber );


    /*
     * mibGetFieldSubProfile:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * determine if the field is MIB_FLD_COMPLEX.  If it is, return a pointer
     * to the profile which is the contents of the field.
     *
     *	mibType:		The type of profile that the field comes
     *				from.  Updated to hold the type of the sub-
     *				profile, if it exists.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		For array fields, the index.
     *
     * Returns a pointer to the sub-profile if the field exists and it
     * is indeed COMPLEX.  Otherwise, returns NULL.
     */
extern Pointer
mibGetFieldSubProfile( MibProfileType*	mibType,
	     	       ConstPtr		profile,
	  	       Uint		fieldNumber,
	  	       Uint		elementNumber );


    /*
     * mibFieldElements:
     *
     * Given a mib type and a field number, determine if it is an array 
     * field and the number of elements.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	fieldNumber:		The field number of interest.
     *
     * Returns the number of elements if the field is an array.
     * Otherwise, returns one (1).
     */
extern Uint16
mibFieldElements( MibProfileType	mibType,
	  	  Uint			fieldNumber );


    /*
     * mibFieldType:
     *
     * Given a mib profile type and a field number, determine the type of
     * the field.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	fieldNumber:		The field number of interest.
     *
     * Returns the field type of the specified field, if known; otherwise,
     * 	returns MIB_FLD_MAX.
     */
extern MibFieldType
mibFieldType( MibProfileType	mibType,
	      Uint		fieldNumber );


    /*
     * mibFieldReadOnly:
     *
     * Given a mib profile type and a field number, determine if
     * the field is read-only for user-interface purposes.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	fieldNumber:		The field number of interest.
     *
     * Returns TRUE if the field is known and read-only; otherwise FALSE
     */
extern Bool
mibFieldReadOnly( MibProfileType	mibType,
	      	  Uint			fieldNumber );


    /*
     * mibProfileIsInternal:
     *
     * Given a mib type, determine if it is internal; i.e. new instances
     * aren't allowed to be created via the UI.
     *
     *	mibType:		The type of profile that the field comes
     *				from.  Updated to hold the type of the sub-
     *				profile, if it exists.
     *
     * Returns TRUE if the profile is internal, otherwise FALSE.
     */
extern Bool
mibProfileIsInternal( MibProfileType	mibType );

#endif /* MIBGEN_H */
