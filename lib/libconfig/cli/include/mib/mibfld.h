/*
 * $Id: mibfld.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
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

#if ! defined( MIBFLD_H )
# define MIBFLD_H

# include "mibtype.h"

    /*
     * MibFieldAccess:
     *
     * MIB fields are either read-only or read-write.  The values for
     * each access type are defined here.
     */
typedef Uint32			MibFieldAccess;
# define MIB_READ_ONLY		0x00000001U
# define MIB_INTERNAL		0x00000002U
# define MIB_ALIAS		0x00000004U

    /*
     * MibFieldType:
     *
     * Every field type known to the system is assigned a type in this
     * enumeration.  The type is associated with input/output functions
     * that know how to convert between internal and external
     * representation, e.g. an IP address may be stored internally as
     * 0xCE41D42F, but is displayed externally as 206.65.212.47
     *
     *	MIB_FLD_TEXT		A text field.  The length of the field
     *				varies.  A list of valid characters may
     *				be specified for data entry purposes.
     *
     *	MIB_FLD_NUMBER		A numeric field.  The length of the
     *				internal representation may be 8, 16, or
     *				32 bits.  A range may be specified for
     *				data entry purposes.
     *
     *	MIB_FLD_ENUM		An enumerated field.  The valid values
     *				and their display strings are specified for
     *				data entry purposes.
     *
     *	MIB_FLD_BOOL		A boolean value. 0 is false and non-zero
     *				is true.  Internal representation may be
     *				an 8, 16, or 32 bit value.  The display
     *				values for true and false may be specified.
     *
     *	MIB_FLD_CBOOL		A compressed boolean value.  Several Boolean
     *				values can be compressed by assigning each
     *				value a specific bit within an 8, 16, or
     *				32-bit value.  A mask is required to specify
     *				the bit within the word.
     *				*** This field type is no longer supported ***
     *
     *	MIB_FLD_HEX		A field stored as an array of bytes but
     *				displayed and entered in hexadecimal
     *				notation (nn:nn:nn:...:nn).  Each nn is
     *				a hex value, 0 through f.
     *
     *	MIB_FLD_IP		A field stored as a 32 bit hex value, but
     *				displayed/entered in dotted decimal with
     *				optional netmask entry, e.g. 123.45.6.78/12
     *
     *  MIB_FLD_VAR		A field stored as a arbitrary length
     *				binary value, and displayed/entered
     *				as a BASE 64 encoding.
     *
     *	;;; lots more to add here.
     *
     *	MIB_FLD_COMPLEX		A complex field; One that is a structure
     *				composed of other field types.  The
     *				structure must be a profile of its own.
     */
typedef enum {
    MIB_FLD_TEXT,
    MIB_FLD_NUMBER,
    MIB_FLD_ENUM,
    MIB_FLD_BOOL,
    MIB_FLD_CBOOL_DEPRECATED,	/*** This field type is no longer supported ***/
    MIB_FLD_HEX,
    MIB_FLD_IP,
    MIB_FLD_VAR,
    /* ;;; */
    MIB_FLD_COMPLEX,
    MIB_FLD_MAX
} MibFieldType;

# define MIB_ELEMENT_UNIQUE	(Uint)-1

    /*
     * MibEnumLookup
     *
     * Enumerated types require a lookup table to convert the valid values
     * to external names and vice-versa.  The actual conversion tables
     * are created by the MIBGEN program while processing a profile meta-
     * file into profile headers and bodies.
     *
     *	enumValue:		The internal value that is stored for the
     *				enumeration.
     *
     *	enumText:		A pointer to the text that names the
     *				above value.
     *
     *	helpText:		A pointer to the text that describes the
     *				above value.
     */
typedef struct {
    Int				enumValue;
    const char*			enumText;
    const char*			helpText;
} MibEnumLookup;

    /*
     * MibFieldTextParams:
     *
     * Parameters used for text field I/O.
     *
     *	validChars:		A pointer to a string containing a list of
     *				valid or invalid characters for this text
     *				field.  If null, all characters are valid.
     *
     *	listIsValid:		If TRUE, the string above is the list of allowed
     *				characters.  If FALSE, it is the list of
     *				*disallowed* characters.
     *				Ignored if "validChars" is NULL.
     *
     *	allowMixedCase:		if FALSE, mibFieldSet() maps all supplied
     *				characters to lower case when setting the 
     *				field value.
     *	
     *	maskDisplay:		if TRUE, all characters in the string are
     *				returned by mibFieldGet() as "*", unless its
     *				"secure" parameter is set to TRUE.
     */
typedef struct {
    const char*			validChars;
    Bool			listIsValid;
    Bool			allowMixedCase;
    Bool			maskDisplay;
} MibFieldTextParams;

    /*
     * MibFieldNumberParams:
     *
     * Parameters used for number field I/O
     *
     *	minValue:		The minimum value for this field.  May
     *				be a negative number.
     *
     *	maxValue:		The maximum value for this field.
     */
typedef struct {
    Int32			minValue;
    Int32			maxValue;
} MibFieldNumberParams;

    /*
     * MibFieldEnumParams:
     *
     * Parameters used for enumerated field I/O
     *
     *	enumLookup:		A pointer to the lookup table to convert
     *				enumerated values.
     */
typedef struct {
    const MibEnumLookup*	enumLookup;
} MibFieldEnumParams;

    /*
     * MibFieldBoolParams:
     *
     * Parameters used for Boolean fields.  Booleans are treated similarly
     * to enumerated fields and therefore use the same lookup table
     * structure.
     *
     *	boolLookup:		A pointer to the lookup table to convert
     *				boolean values.
     */
typedef struct {
    const MibEnumLookup*	boolLookup;
} MibFieldBoolParams;

    /*
     * MibFieldCboolParams:
     *
     * Parameters used for compressed Boolean fields.  Compressed Booleans
     * are treated similarly to enumerated fields, but also require a
     * mask field to specify which bit within the field is to be processed.
     *
     *	cboolLookup:		A pointer to the lookup table to convert
     *				compressed boolean values.
     *
     *	mask:			A mask of the bit within the field that
     *				should be tested.
     */
typedef struct {
    const MibEnumLookup*	cboolLookup;
    Uint32			mask;
} MibFieldCboolParams;

    /*
     * MibFieldHexParams:
     *
     * Parameters used for hex fields.
     *
     *	dummy:			A dummy field, hex data input/output
     *				requires no data at this time.
     */
typedef struct {
    Int				dummy;
} MibFieldHexParams;

    /*
     * MibFieldIpParams:
     *
     * Parameters used for IP field entry and display.
     *
     *	netmaskFieldOffset:	The offset from the IP address field
     *				of a field within the same
     *				profile where a netmask is found/put.  If
     *				zero, data I/O is limited to IP addresses.
     *				If non-zero data I/O can contain a /nn to
     *				specify a netmask where the netmask comes
     *				from this field offset.
     */
typedef struct {
    Int				netmaskFieldOffset;
} MibFieldIpParams;

    /*
     * MibFieldVarParams:
     *
     * Parameters used for VAR field entry and display.
     *
     *  blockSize:		The number of binary bytes to display
     *				in a single block.  We will get 4/3
     *				this number of characters as the 
     *				base64 representation.
     */
typedef struct {
    Int				blockSize;
} MibFieldVarParams;


    /*
     * MibFieldComplexParams:
     *
     * Contains a pointer to the mib definition table for the profile
     * type that defines the complex data type.
     *
     *	subProfileMibType:	The type of the profile which makes up
     *				the field.
     */
typedef struct {
    MibProfileType		subProfileMibType;
} MibFieldComplexParams;


    /*
     * MibFieldParams:
     *
     * A union of pointers to the I/O parameters for the type.
     *
     *	u:			A union of pointers to one of the param
     *				structures defined above.
     */
typedef const union uMibFieldParams{
    MibFieldTextParams		textParams;
    MibFieldNumberParams	numberParams;
    MibFieldEnumParams		enumParams;
    MibFieldBoolParams		boolParams;
    MibFieldCboolParams		cboolParams;
    MibFieldHexParams		hexParams;
    MibFieldIpParams		ipParams;
    MibFieldVarParams		varParams;
    /* ;;; add new types here */
    MibFieldComplexParams	complexParams;
} MibFieldParams;

    /*
     * Many profiles may use the same field editing/display parameters,
     * e.g. Yes/No for Bool fields, phone number digits for text fields,
     * etc.  The common values are defined here.
     *
     *   yesNoParams		Values Yes or No
     *
     *	 hexPasswordParams	Passwords that are hex values;
     *				forced to lower case, must be 0-f,
     *				not echoed.
     *
     *   passwordParams		Passwords; case-sensitive, not echoed.
     *
     *	 textIndexParams	Index fields that are plain text;
     *				case-INsensitive, forced to lower-case
     *				when written, echoed when typed.
     *
     *	 nameIndexParams	Index fields that are names;
     *				case-SENSITIVE, echoed when typed.
     *
     *	 phoneNumberParams	Phone numbers.
     *
     *	 trunkGroupParams	Trunk group numbers (0-9)
     */

extern const MibFieldBoolParams		yesNoParams;

extern const MibFieldTextParams		hexPasswordParams;

extern const MibFieldTextParams		passwordParams;

extern const MibFieldTextParams		textIndexParams;

extern const MibFieldTextParams		nameIndexParams;

extern const MibFieldTextParams		phoneNumberParams;

extern const MibFieldNumberParams       trunkGroupParams;

    /*
     * Some error messages are shared among the field editing modules.
     * The common messages are declared extern here and defined in
     * mibfld.c
     */
# if ( USERIF & CLI )
extern const char			errorAmbiguousName[];
extern const char			errorNoMatch[];
extern const char			errorAmbiguousNameParam[];
extern const char			errorNoMatchParam[];
extern const char			errorBadValue[];
# endif

//
// zhz@20041029, move here from mibglob.h
//
    /*
     * WAN host names can be arbitrarily long.  Since we use the name for
     * information more than anything else we will limit the name to a
     * reasonable size.  If the real name is longer than this size the
     * user will have to enter the wan address since DNS or YP/NIS wont
     * work.
     */
# define MAX_HOST_NAME_LENGTH		31

# define ETHER_ADDR_LEN			6

# define ETHER_VLANTAG_LEN     		2

# define MAX_PROFILE_NAME_LENGTH 	11

    /*
     * Define constants based on rfc883
     */
# define MAX_DOMAIN_NAME_LEN_RFC883 256	/* maximum domain name per rfc 883 */

    /*
     * When DNS is used we need to know our own domain name.
     * The maximum length of a domain name supported by the
     * Pipeline is 63 characters.
     */
# define MAX_DOMAIN_NAME_LENGTH		63

# define MAX_IP_LIST			35	/* Max. # of IP addr. returned from DNS query */

/*
 * Below we define max. # of IP addresses that could be configured in
 * DNS Fallback Table configuration profile. Currently we define
 * it to be equal to MAX_IP_LIST.
 *
 * If we later decide MAX_IP_LIST is too big, we could simply
 * (re)define MAX_IP_LIST_IN_PROF to the desirable smaller number here.
 *
 * WARNING: MAX_IP_LIST_IN_PROF must be smaller than MAX_IP_LIST.
 *
 */
# define MAX_IP_LIST_IN_PROF MAX_IP_LIST

    /*
     * There are several places where we are concerned with percentage-
     * utilization.  This will always be a number between zero and 100.
     * It is just typedef'd for convenience here.
     */
typedef Word	Percentage;

# if ( MDF_FRAME_RELAY == FYES )
//#  define MAX_FR_RELAY_NAME_LEN	15
#  define MAX_FR_RELAY_NAME_LEN		MAX_PROFILE_NAME_LENGTH	
# endif /* MDF_FRAME_RELAY */

# if ((MDF_X25B == FYES) || (MDF_X25_DTE == FYES))
//#  define MAX_X25_NAME_LEN	   	15
#  define MAX_X25_NAME_LEN	     	MAX_PROFILE_NAME_LENGTH	
#  define MAX_X25_AUTO_CALL_LEN      	48
#  define MAX_X3_PROF_STR_LEN	   	64
#  define MAX_X25_RPOA_LEN	   	4
#  define MAX_X25_CUG_LEN		2
#  define MAX_X25_NUI_LEN		6
#  define MAX_X3_PARAM               	22
#  define MAX_PAD_BANNER_LEN         	32
#  define MAX_PAD_PROMPT_LEN         	12
#  define MAX_PAD_NUI_PROMPT_LEN     	20
#  define MAX_PAD_NUI_PW_PROMPT_LEN  	20
#  define MAX_PAD_ALIAS_LEN          	40
#  if ( MDF_X25_DIAG == FYES )
#   define MAX_PAD_DEFAULT_PW_LEN    	12
#  endif /* MDF_X25_DIAG */
# endif


    /*
     * The maximum number of digits in the transit Number are defined below.
     * I think the longest string actually used is 3 (i.e. 10-ATT-0).
     */
# define MAX_TRANSIT_NUMBER_LENGTH	8

# if (( MDF_ETHERNET == FYES ) || ( PLATFORM == TNT ) || ( MDF_IMODEM == FYES ))
    /*
     * Community Names can be up to 255 chars in length.  Arbitrarily
     * limit them for now.
     */

#  define MAX_COMM_NAME_LENGTH    	31

# endif /* (( MDF_ETHERNET==FYES )||( PLATFORM==TNT )||( MDF_IMODEM==FYES )) */


# if ( PLATFORM & TNT )
    /*
     * Names within the system are typically limited to 23 bytes
     * (24 when including the terminating null).
     */
#  define MAX_NAME_LEN			23
#  define MAX_UNIT_NAME_LEN		23

# elif ( ((PLATFORM & P25) && ( MDF_NAT == FYES )) || ( (PLATFORM & P50) && (NETIF & BRI) ))

    /*
     *Several profiles have names.  For the P25PX the
     *name is limited to 72 characters.  This does NOT include the NULL so
     *arrays should be structured MAX_NAME_LEN + 1
     */

#  define MAX_NAME_LEN			MAX_PROFILE_NAME_LENGTH	
#  define MAX_UNIT_NAME_LEN		72
# else
    /*
     * Several profiles have names.  Within the system any name is limited
     * to 16 characters.  This does NOT include the NULL so arrays should
     * be structured MAX_NAME_LEN+ 1
     */
#  define MAX_NAME_LEN			16
#  define MAX_UNIT_NAME_LEN		16

# endif

    /*
     * The maximum number of characters in the IP Addr Msg string.
     */
# define MAX_IP_ADD_MSG 		64

    /*
     * The maximum number of characters in the IP Netmask Msg string.
     */
# define MAX_IP_NETMASK_MSG 		64

    /*
     * The maximum number of characters in the IP Gateway Addr Msg string.
     */
# define MAX_IP_GATEWAY_ADD_MSG 	64

    /*
     * The maximum number of characters in a password are defined below.
     * This seems like way more than enough.
     */
# define MAX_PASSWORD_LENGTH			31

 
    /*
     * The maximum number of characters in a dba call profile or port profile
     * password are defined below. Keep to a minimum for resource constrained
     * platforms. 
     */
# define MAX_PORTPW_LEN           	9

# if ( PLATFORM & TNT )
    /*
     * Snmp textual info is typically limited to 83 characters (84
     * when including terminating null
     */
#  define MAX_SNMP_INFO_LEN		83
# else
    /*
     * The maximum amount of SMNP info stored.  If we followed the spec
     * every display string would be 256 bytes.  This is more than we
     * feel we can afford.
     */
#  define MAX_SNMP_INFO_LEN	80
# endif

    /*
     * The maximum number of digits in a user-dialable phone number is 
     * defined here. This number is not used in any protocol headers so
     * it may be changed.
     */
# define MAX_PHONE_NUMBER_LENGTH	24

    /*
     * The maximum length of phone number the user can enter is defined 
     * below.  Note that this may be substantially longer than the actual
     * number so that two numbers can go in the array.
     */
# define MAX_USER_NUMBER_LENGTH	MAX_PHONE_NUMBER_LENGTH + 1 \
					+ MAX_PHONE_NUMBER_LENGTH


    /*
     * for the ErrorProfile, the size of the string holding the printed 
     * version number.
     */

# define MAX_VERSTRING_LEN		12

    /* 
     * Maximum size for the loadname string, as in the errror profile. 
     */


# define MAX_LOADNAME_LEN		40

    /*
     * The maximum number of digits in the billing number is the length of
     * a single phone number.  The length is defined below.
     */
# define MAX_BILLING_NUMBER_LENGTH	MAX_PHONE_NUMBER_LENGTH


#define MAX_FWNAME_LEN			MAX_PROFILE_NAME_LENGTH

    /*
     * The maximum length of a network profile name is defined below.  This
     * number, like the dba session profile name length, is somewhat arbitrary,
     * but must be reasonable for making lists on the display and storing in
     * NVRAM without being too piggy.
     */
# if PLATFORM & ( M200 )
#  define MAX_NET_PROFILE_NAME		31
# else
#  define MAX_NET_PROFILE_NAME		MAX_PROFILE_NAME_LENGTH	
# endif

    /*
     * The maximum length of the userstat format string.
     */

# define MAX_USERSTAT_FORMAT_LEN        72

# if (MDF_RADIUS == FYES)
    /*
     * The maximum length of the realm delimiter characters.
     * There are currently 4 such delimiters known, so allow room for three more.
     * It is unlikely that more than two would be used at any single site.
     */
#  define MAX_REALM_CHARS_LEN		7
    /*
     * The actual known realm delimiters 
     * this MUST fit in MAX_REALM_CHARS_LEN bytes.
     */
/*zhz@20020822 */
//#  define RAD_REALM_DELIMITERS		"/\\@%"
#  define RAD_REALM_DELIMITERS			"@!"
# endif /* (MDF_RADIUS == FYES) */

# if ( PLATFORM & (P50 | PDSL) )
#  define BRI_MAX_DSL_PER_SLOT			1
# elif ( PLATFORM == P25 )
#  define BRI_MAX_DSL_PER_SLOT			1
# elif ( PLATFORM == P15 )
#  define BRI_MAX_DSL_PER_SLOT			1
# elif ( PLATFORM & MPORT )
#  define BRI_MAX_DSL_PER_SLOT			8
# elif ( PLATFORM == TNT )
#  define BRI_MAX_DSL_PER_SLOT			32   /* was 8 before IDSL */
# elif ( PLATFORM & ( M200 ))
#  define BRI_MAX_DSL_PER_SLOT			1
# else
#  error Unknown PLATFORM
# endif

# if ( ( MDF_ETHERNET == FYES ) || ( MDF_IMODEM == FYES ) ) 
    /*
     * The type of an ETHERNET address is defined.  The first three bytes
     * (24 bits) of an ethernet number are an Organization Unique Identifier
     * (OUI).
     */
#  define ETHER_ADDRESS_LEN	ETHER_ADDR_LEN

typedef Byte			EtherAddress[ ETHER_ADDRESS_LEN ];

# endif

# if ( MDF_TFTPBOOT == FYES )
#  define MAX_TFTP_BOOT_BASE_LEN 80
# endif


# define MAX_TFTP_HOST_NAME_LEN	80
# define MAX_BOOT_FILE_PATH_LEN	80

    /*
     * mibFieldErrorLen
     *
     * Build the standard "field too long" message in the passed
     * output area.
     *
     *	errorStr:		The place to build the error message.
     *
     *	errorStrLen:		A pointer to the length of errorStr.
     *				This value is updated with the number
     *				of bytes actually stored.
     *
     *	lenLimit:		The limit (max chars) that can be stored
     *				in the variable that caused the error to
     *				be displayed.  This value is part of the
     *				error message.
     */
# if ( USERIF & CLI )
extern void
mibFieldErrorLen( char*		errorStr,
		  Uint*		errorStrLen,
		  Uint		lenLimit );

    /*
     * mibFieldErrorChars:
     *
     * Build the standard "invalid characters" message in the passed
     * output area.
     *
     *	errorStr:		The place to build the error message.
     *
     *	errorStrLen:		A pointer to the length of errorStr.
     *				This value is updated with the number
     *				of bytes actually stored.
     *
     *	badChar:		The character that does not belong in
     *				the string. This value is part of the
     *				error message.
     */
extern void
mibFieldErrorChars( char*	errorStr,
		    Uint*	errorStrLen,
		    char	badChar );

    /*
     * mibFieldErrorValue:
     *
     * Build the standard "invalid value" message in the passed
     * output area.
     *
     *	errorStr:		The place to build the error message.
     *
     *	errorStrLen:		A pointer to the length of errorStr.
     *				This value is updated with the number
     *				of bytes actually stored.
     *
     *	badValue:		The value that is invalid, which is 
     *				incorporated into the error message.
     */
extern void
mibFieldErrorValue( char*	errorStr,
		    Uint*	errorStrLen,
		    Int		badValue );


    /*
     * mibFieldCompare:
     *
     * Compare two strings, ignoring case, and return the number of
     * leading characters that matches.  String 1 is null terminated,
     * string 2 is not (but a length is provided)
     *
     *	str1:			String to compare
     *
     *	str2:			String to compare
     *
     *	str2Len:		Length of string 2.
     */
extern Uint
mibFieldCompare( const char*	str1,
		 const char*	str2,
		 Uint		str2Len );

    /*
     * mibFieldCopy:
     *
     * Copy source to dest for up to destLen bytes.  If the length of
     * source (a null terminated string) is greater that dest destLen
     * less 1 bytes are copied, followed by a + to indiate data
     * truncated.  A terminating null is NOT included.
     *
     *	dest:			Pointer to start of destination string.
     *
     *	destLen:		Pointer to maximum size of destination
     *				string.  This value is updated with the
     *				number of characters actually written
     *				to destination string.
     *
     *	source:			Pointer to source string.  May be null.
     *
     * Returns TRUE if data copied.  Returns FALSE if source is null
     * in which case the string "No value" is copied into dest.
     */
extern Bool
mibFieldCopy( char*		dest,
	      Int*		destLen,
	      const char*	source );


    /*
     * mibFieldGet:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * convert the field to a displayable format and store it in the passed
     * output area.  If the field does not fit in the output area copy
     * as many bytes as will fit less 1, and append a '+' at the end of the
     * string.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields).
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
     *	secure:			A boolean, set to TRUE if password fields
     *				can be displayed.  When not true password
     *				fields (text fields with an empty validChars
     *				string) are echoed as "***"
     *
     * Returns TRUE if the field was found and converted to a display
     * format, otherwise returns FALSE.
     */
extern Bool
mibFieldGet( MibProfileType	mibType,
	     ConstPtr		profile,
	     Uint		fieldNumber,
	     Uint		elementNumber,
	     char*		outputStr,
	     Int*		outputStrLen,
	     Bool		secure );


    /*
     * mibFieldGetLen:
     *
     * Given an mib type and field number return the number of bytes
     * necessary to completely display the field.
     *
     *	mibType:		The type of profile containing the field.
     *
     *	profile:		A pointer to the profile from which the field
     *				length will be calculated.
     *
     *	fieldNumber:		The field number within the profile.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields).
     *
     * Returns the number of bytes.  Zero is returned if the profile
     * and/or field are unknown.
     */
extern Uint
mibFieldGetLen( MibProfileType	mibType,
	        ConstPtr	profile,
	        Uint		fieldNumber,
	        Uint		elementNumber );

    /*
     * mibFieldSet:
     *
     * Convert a field value in display format to its internal
     * representation and stuff the field in the passed profile.
     *
     *	mibType:		The type of profile containing the field.
     *
     *	profile:		A pointer to the profile to update.
     *
     *	fieldNumber:		The number of the field to update within
     *				the profile.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields).
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
     * Returns TRUE if the set was performed.  Returns FALSE if the
     * set could not be performed with the (optional) error reason written
     * to the error string.
     */
extern Bool
mibFieldSet( MibProfileType	mibType,
	     Pointer		profile,
	     Uint		fieldNumber,
	     Uint		elementNumber,
	     const char*	inputStr,
	     Uint		inputStrLen,
	     char*		errorStr,
	     Uint*		errorStrLen );


    /*
     * MibMoreData:
     *
     * A function of this type is passed to mibFieldHelp to get the next line
     * for paged output.
     *
     *	handle:			The handle of the session that is
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
MibMoreData( Pointer		handle,
	     Pointer		tag,
	     Int*		dataLen );


    /*
     * MibMoreEnd:
     *
     * A function of this type is passed to mibFieldHelp.  It is called when
     * output is completed or cancelled.
     *
     *	handle:			The handle of the session that is
     *				doing paged output.
     *
     *	tag:			A tag that was received in the original
     *				request to do paged output.
     */
typedef void
MibMoreEnd( Pointer		handle,
	    Pointer		tag );



    /*
     * mibFieldHelp:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * generate text describing the field's allowed values and store it in the
     * passed output area.  If the help does not fit in the output area copy
     * as many bytes as will fit less 1, and append a '+' at the end of the
     * string.
     * Also, return a pointer to a text string describing the
     * meaning of this individual field.
     * Finally, for fields that support it, return functions to be used 
     * to return a list of items suitable for paged output.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields).
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
     *	textStr:		Unless NULL, this value is updated to point 
     *				to a NULL-terminated string of ASCII text
     *				describing the field.
     *				
     *	moreDataFnc:		This parameter may be updated to point to a
     *				function the caller can use to obtain a 
     *				series of strings suitable for paged output.
     *				The caller may specify NULL if he is not 
     *				interested in the facility.  A return value
     *				of NULL indicates the field does not support
     *				it.  If this field is updated, the two fields
     *				below will also be updated.
     *
     *	moreEndFnc:		Returns a pointer to a function which should
     *				be invoked when the caller is done using the
     *				moreDataFnc.
     *
     *	moreTag:		Updated with an opaque value to be passed
     *				into the two functions above.
     *
     * Returns TRUE if the field was found and help was generated,
     * otherwise returns FALSE.  WHEN false is returned the
     * outputStr contains a description of the error.
     */
extern Bool
mibFieldHelp( MibProfileType	mibType,
	      Uint		fieldNumber,
	      Uint		elementNumber,
	      char*		outputStr,
	      Int*		outputStrLen,
	      const char**	textStr,
	      MibMoreData**	moreDataFnc,
	      MibMoreEnd**	moreEndFnc,
	      Pointer*		moreTag );


    /*
     * mibFieldHelpLen:
     *
     * Given an mib type and field number return the number of bytes
     * necessary to completely display the field help.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields).
     *
     * Returns the number of bytes.  Zero is returned if the profile
     * and/or field are unknown.
     */
extern Uint
mibFieldHelpLen( MibProfileType	mibType,
	         Uint		fieldNumber,
	         Uint		elementNumber );


    /*
     * The above get and set functions re-vector the call to the
     * appropriate function for the field type.  Every field type must
     * supply field specific versions of the get, get length, and
     * set functions.  The types that define the functions follow.
     */

    /*
     * MibFieldGetFnc:
     *
     * A function of this type is registered with mibgen for every
     * known field type.  It knows how to convert the type to a
     * display format.
     *
     *	field:			A pointer to the start of the field
     *				to convert to display format
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	params:			A pointer to the per field instance params
     *				for the field to be converted.
     *
     *	outputStr:		A pointer to a character string where the
     *				field will be output to.  The field is
     *				_Not_ null terminated.
     *
     *	outputStrLen:		A pointer to the size of outputStr.  This
     *				value is updated with the actual number
     *				of bytes written to the string unless
     *				there is an error.
     *
     *	secure:			A boolean, set to TRUE if password fields
     *				can be displayed.  When not true password
     *				fields (text fields with an empty validChars
     *				string) are echoed as "***"
     *
     * Returns TRUE if the field was converted to a display format,
     * otherwise returns FALSE.  WHEN false is returned the outputStr
     * contains a description of the error. 
     */
typedef Bool
MibFieldGetFnc( ConstPtr	field,
	        Uint		fieldLen,
	        MibFieldParams*	params,
	        char*		outputStr,
	        Int*		outputStrLen,
	        Bool		secure );


    /*
     * MibFieldGetLenFnc:
     *
     * A function of this type is registered with mibgen for every known
     * field type.  It knows how to calculate the number of bytes required
     * to display the field value.
     *
     *	field:			A pointer to the start of the field
     *				to convert to display format
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	params:			A pointer to the per field instance params
     *				for the field to be converted.
     *
     * Returns the number of bytes to display the field.
     */
typedef Uint
MibFieldGetLenFnc( ConstPtr		field,
		   Uint			fieldLen,
		   MibFieldParams*	params );

    /*
     * MibFieldSetFnc:
     *
     * A function of this type is registered with mibgen for every
     * known field type.  It knows how to convert a string representation
     * of the field to it's internal format.
     *
     *	field:			A pointer to the start of the field
     *				to change.
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	params:			A pointer to the per field instance params
     *				for the field to be converted.
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
     * Returns TRUE if the set was performed.  Returns FALSE if the
     * set could not be performed with the (optional) error reason written
     * to the error string.
     */
typedef Bool
MibFieldSetFnc( Pointer		field,
	        Uint		fieldLen,
	        MibFieldParams*	params,
	        const char*	inputStr,
	        Uint		inputStrLen,
	        char*		errorStr,
	        Uint*		errorStrLen );


    /*
     * MibFieldHelpFnc:
     *
     * A function of this type is registered with mibgen for every
     * known field type.  It knows how to generate a help string
     * which describes the allowable values for a field.
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	params:			A pointer to the per field instance params
     *				for the field to be converted.
     *
     *	outputStr:		A pointer to a character string where the
     *				help will be output to.  It is
     *				_Not_ null terminated.
     *
     *	outputStrLen:		A pointer to the size of outputStr.  This
     *				value is updated with the actual number
     *				of bytes written to the string unless
     *				there is an error.
     *
     * Returns TRUE if the help for the field was returned,
     * otherwise returns FALSE.  WHEN false is returned the outputStr
     * contains a description of the error. 
     */
typedef Bool
MibFieldHelpFnc( Uint			fieldLen,
	         MibFieldParams*	params,
	         char*			outputStr,
	         Int*			outputStrLen,
	         MibMoreData**		moreDataFnc,
	         MibMoreEnd**		moreEndFnc,
	         Pointer*		moreTag );


    /*
     * MibFieldHelpLenFnc:
     *
     * A function of this type is registered with mibgen for every known
     * field type.  It knows how to calculate the number of bytes required
     * to display the field help.
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	params:			A pointer to the per field instance params
     *				for the field to be converted.
     *
     * Returns the number of bytes to display the help.
     */
typedef Uint
MibFieldHelpLenFnc( Uint		fieldLen,
		    MibFieldParams*	params );


    /*
     * mibFieldRegister:
     *
     * At system startup the modules that implements the above field
     * conversion functions for the know mib types register with the
     * mib manager by calling this function.  There may only be one
     * registrant per field type.
     *
     *	fieldType:		The type of field for which conversion
     *				functions are being registered.
     *
     *	fieldGetFnc:		A pointer to the MibFieldGetFnc for this
     *				field type.
     *
     *	fieldGetLenFnc:		A pointer to the MibFieldGetLenFnc for this
     *				field type.
     *
     *	fieldSetFnc:		A pointer to the MibFieldSetFnc for this
     *				field type.
     *
     *	fieldHelpFnc:		A pointer to the MibFieldHelpFnc for this
     *				field type.
     *
     *	fieldHelpLenFnc:	A pointer to the MibFieldHelpLenFnc for this
     *				field type.
     */
extern void
mibFieldRegister( MibFieldType		fieldType,
		  MibFieldGetFnc*	fieldGetFnc,
		  MibFieldGetLenFnc*	fieldGetLenFnc,
		  MibFieldSetFnc*	fieldSetFnc,
		  MibFieldHelpFnc*	fieldHelpFnc,
		  MibFieldHelpLenFnc*	fieldHelpLenFnc );

    /*
     * mibFieldSetCopy:
     *
     * Convert a field value in display format to its internal
     * representation and copy the result to an arbitrary location.
     *
     *	mibType:		The type of profile containing the field.
     *
     *	field:			A pointer to the start of the field
     *				to change.
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	fieldNumber:		The number of the field to update within
     *				the profile.
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
     * Returns TRUE if the set was performed.  Returns FALSE if the
     * set could not be performed with the (optional) error reason written
     * to the error string.
     */
extern Bool
mibFieldSetCopy( MibProfileType	mibType,
	     Pointer		field,
	     Uint		fieldLen,
	     Uint		fieldNumber,
	     const char*	inputStr,
	     Uint		inputStrLen,
	     char*		errorStr,
	     Uint*		errorStrLen );

    /*
     * mibFieldGetFrom:
     *
     * Given a pointer to an arbitrarily located profile field,
     * convert the field to a displayable format and store it in the passed
     * output area.  If the field does not fit in the output area copy
     * as many bytes as will fit less 1, and append a '+' at the end of the
     * string.
     *
     *	mibType:		The type of profile that the field is
     *				assumed to come from.
     *
     *	field:			A pointer to the start of the field
     *				to convert to display format
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	fieldNumber:		The field number of the profile the field
     *				is assumed to correspond to.
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
     *	secure:			A boolean, set to TRUE if password fields
     *				can be displayed.  When not true password
     *				fields (text fields with an empty validChars
     *				string) are echoed as "***"
     *
     * Returns TRUE if the field was found and converted to a display
     * format, otherwise returns FALSE.  WHEN false is returned the
     * outputStr contains a description of the error.
     */
extern Bool
mibFieldGetFrom( MibProfileType	mibType,
	     	 ConstPtr	field,
	     	 Uint		fieldLen,
	     	 Uint		fieldNumber,
	     	 char*		outputStr,
	     	 Int*		outputStrLen,
	     	 Bool		secure );

#  if 0
/* We don't use this function.  To resurrect it, code needs to be added
 * to make it understand arrays */
    /*
     * mibFieldGetLenFrom:
     *
     * Given a pointer to a copy of a mib field, return the number of bytes
     * necessary to completely display the field.
     *
     *	mibType:		The type of profile containing the field.
     *
     *	field:			A pointer to the start of the field
     *				to convert to display format
     *
     *	fieldLen:		The length of the field.  This allows
     *				the same conversion code to be used
     *				for different sized fields, e.g. 16 bit
     *				integers and 32 bit integers.  The length
     *				is always given in octets.
     *
     *	fieldNumber:		The field number within the profile.
     *
     * Returns the number of bytes.  Zero is returned if the profile
     * and/or field are unknown.
     */
extern Uint
mibFieldGetLenFrom( MibProfileType	mibType,
	     	    ConstPtr		field,
	     	    Uint		fieldLen,
	     	    Uint		fieldNumber );
#  endif /* 0 */

    /*
     * mibFieldCheck:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * compare the field with an external copy to see if they are identical.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields
     *				or to specify the whole of an array).
     *
     *	fieldCopy:		A pointer to an external copy of the field.
     *
     * Returns TRUE if the fields match, otherwise FALSE.
     */
extern Bool
mibFieldCheck( MibProfileType	mibType,
	       ConstPtr		profile,
	       Uint		fieldNumber,
	       Uint		elementNumber,
	       ConstPtr		fieldCopy );

    /*
     * mibFieldDup:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * return a copy of the field in memory allocated by the function.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields
     *				or to specify the whole of an array).
     *
     * Returns: a pointer to the allocated copy, otherwise NULL.
     */
extern Pointer
mibFieldDup( MibProfileType	mibType,
	     ConstPtr		profile,
	     Uint		fieldNumber,
	     Uint		elementNumber );

    /*
     * mibFieldPtr:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * return a pointer to the field.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	profile:		A pointer to the profile from which the field
     *				will be extracted.
     *
     *	fieldNumber:		The field number of the field to extract.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields
     *				or to specify the whole of an array).
     *
     * Returns: a pointer to the specified field, otherwise NULL.
     */
extern ConstPtr
mibFieldPtr( MibProfileType	mibType,
	     ConstPtr		profile,
	     Uint		fieldNumber,
	     Uint		elementNumber );

    /*
     * mibFieldLen:
     *
     * Given a mib type and a field number, determine the length of the 
     * specified field.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	fieldNumber:		The field number.
     *
     *	elementNumber:		The index for array fields (specify 
     *				MIB_ELEMENT_UNIQUE for non-array fields
     *				or to specify the whole of an array).
     *
     * Returns: the length of the specified field, otherwise zero.
     */
extern Int
mibFieldLen( MibProfileType	mibType,
	     Uint		fieldNumber,
	     Uint		elementNumber );

    /*
     * mibFindEnumValue:
     *
     * Given a lookup table and a text string, determine if the text string
     * un-ambiguously matches the name of one of the enum values.  If so, 
     * return TRUE, and set enumValue.  Otherwise, fill in the appropriate 
     * error message and return FALSE.
     *
     *	lookup:			A pointer to the lookup table to use to convert
     *				the enumerated value.
     *
     *	inputStr:		A pointer to the string to be matched.
     *
     *	inputStrLen:		The length of inputStr.
     *
     *	errorStr:		Optional: If non-null then a pointer to
     *				a place were error messages can be
     *				copied to if a match could not be
     *				performed.  The error message is *not*
     *				NULL-terminated.
     *
     *	errorStrLen:		A pointer to the length of the error	
     *				string (null when errorStr is null).
     *				Updated with the actual length of the
     *				string written when FALSE is returned.
     *
     *  enumValue:		updated with the matched value
     */
extern Bool
mibFindEnumValue( const MibEnumLookup*	lookup,
	          const char*		inputStr,
	          Uint			inputStrLen,
	          char*			errorStr,
	          Uint*			errorStrLen,
	          Uint32*		enumValue );

    /*
     * mibFindEnumText:
     *
     * Given a lookup table and an enum value, return the name associated
     * with the value.  If the enum value is known, returns a pointer to
     * a NULL-terminated string.  Otherwise, returns NULL.
     *
     *	lookup:		A pointer to the lookup table to use to convert
     *			the enumerated value.
     *
     *  enumValue:	the value to match
     */
extern const char*
mibFindEnumText( const MibEnumLookup*	lookup,
	         Uint32			enumValue );

    /*
     * mibFieldGetValue:
     *
     * Convert the passed field/fieldLen to an numerical value.
     * It is assumed the field is type MIB_FLD_NUMBER or MIB_FLD_ENUM.  
     *
     *  field:		a pointer to the profile field
     *
     *  fieldLen:	the length of the field in bytes
     *
     * The value is returned as a 32 bit unsigned integer for safety.
     */
extern Int32
mibFieldGetValue( ConstPtr	field,
	          Uint		fieldLen );

    /*
     * mibFieldSwap:
     *
     * Given a pointer to a field and its number,
     * perform byte reordering on the field, if it is appropriate.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	field:			A pointer to the field to be swapped.
     *
     *	fieldNumber:		The field number of the field to be swapped.
     *
     * Returns: TRUE if the field was successfully indentified and, if
     *	appropriate, swapped.  Otherwise, returns FALSE.
     */
extern Bool
mibFieldSwap( MibProfileType	mibType,
	      Pointer		field,
	      Uint		fieldNumber );

    /*
     * mibProfileSwap:
     *
     * Given a pointer to a structure of a given mib type and a field number,
     * perform byte reordering on all appropriate fields.
     *
     *	mibType:		The type of profile that the field comes
     *				from.
     *
     *	profile:		A pointer to the profile whose fields are to be
     *				swapped.
     *
     * Returns: TRUE if all fields were successfully indentified and, if
     *	appropriate, swapped.  Otherwise, returns FALSE.
     */
extern Bool
mibProfileSwap( MibProfileType	mibType,
	        Pointer		profile );

    /*
     * mibGenSet:
     *
     * This function parses a complex field. The complex field can be
     * a PhysicalAddress or a LogicalAddress.
     *
     *	fieldType:		The type of field to parse.
     *
     *	field:			A pointer to the data structure to copy the
     *				parameter into.
     *
     *	fieldLen:		Size of the data structure to hold the
     *				parsed parameter.
     *
     *	params:			Pointer to the I/O parameters for the field
     *				type.
     *
     *	inputStr:		Pointer to the field to parse.
     *
     *	inputStrLen:		Size of the field to parse.
     *
     *	errorStr:		Pointer to buffer to copy error message if any.
     *
     *	errorStrLen:		Size of error buffer. Update with the number
     *				of characters written to the error buffer.
     */
extern Bool
mibGenSet( MibFieldType		fieldType,
	   Pointer		field,
	   Uint			fieldLen,
	   MibFieldParams*	params,
	   const char*		inputStr,
	   Uint			inputStrLen,
	   char*		errorStr,
	   Uint*		errorStrLen );

# endif /* ( USERIF & CLI ) */

#endif /* MIBFLD_H */
