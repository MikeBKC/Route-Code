/*
 * $Id: lcdmisc.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
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
     * This file contains things extracted from the LCD user interface
     * code.  The things are, unfortunately, used by non LCD user
     * interface things (e.g. RADIUS). This file MUST NOT contain
     * any definitions that are dependent upon any other file.  It
     * MUST stand alone (save the standard types from gendef).
     */

#if ! defined( PARAMS_H )
# define PARAMS_H


# define MAX_IPSTRING_LEN 15    /* xxx.xxx.xxx.xxx */

    /*
     * The digits that are valid for phone numbers and just plain numbers
     * are enumerated below.  These are used for text entry subset strings.
     */
extern const char lcdParamPhoneNumberDigits[];
extern const char lcdParamPhNumDigitsWithComma[];
extern const char lcdParamNumberDigits[];
extern const char lcdParamDateDigits[];
extern const char lcdParamTimeDigits[];
extern const char lcdParamMenuDigits[];
extern const char lcdParamLettersOnly[];
extern const char lcdParamIpAddressDigits[];
extern const char lcdParamHexDigits[];
extern const char lcdParamHostNameLetters[];

    /*
     * A terminal can be created within the system.  Any nunmber of terminals
     * can be created to represent local and remote VT100's and palmtop
     * displays.  Note that this is an incomplete definition, since the
     * structure is private to lcdterm.c.
     */
typedef struct LcdTerminalStruct * LcdTerminalHdl;


    /*
     * The 'channel' that is passed to the terminal is used to identify
     * connections to drivers that support multiple ports.  If the
     * If the chanel is or-ed with the CLOSABLE_TERM_FLAG it signifies
     * that the device is a logical device, say a network terminal, that
     * can be closed.
     */
# define CLOSABLE_TERM_FLAG	0x4000


    /*
     * At terminal creation time, when a terminal is initiated, a device
     * handle will be created and returned by the specific device. All
     * terminal access will then use this device handle as access param.
     * Note that the device handle struct is private to each device 
     * driver and is therefore defined as an opaque type.
     */
struct SccDriverStruct;
typedef struct SmcDriverStruct*	 DeviceDriverHdl;


    /*
     * The different types of parameters defined for the lcd display are:
     *
     * PARAM_ENUM:	One of an enumeration.
     *
     * PARAM_BOOLEAN:	A boolean.
     *
     * PARAM_NUMBER:	An unsigned number within a specified range.
     *
     * PARAM_LONG:	A signed long within a specified range.
     *
     * PARAM_ULONG:	An unsigned long within a specified range.
     *
     * PARAM_TEXT:	A textual string with anything in it at all.
     *
     * PARAM_SUB_MENU:	A sub-menu (place-holder for another level in 
     *			the tree).
     *
     * PARAM_TELEPHONE:	A telephone number.
     *
     * PARAM_TIME:	A time of day.
     *
     * PARAM_DATE:	A date (i.e. calendar)
     *
     * PARAM_TEXT_HIDDEN: A text field that may be hidden instead of
     *			being user accessible.  When it is hidden, it
     *			cannot be selected, and has a fixed value.
     *
     * PARAM_ENUM_SPARSE: One of an enumeration, whose values may be sparse
     *			to due other field values.
     *
     * PARAM_MIF_RONLY:	A MIF Read Only paramater -- used for menu items
     *			that need to be processed by the MIF.
     *
     * PARAM_MIF_WONLY:	A MIF Write Only parameter -- used for menu items
     *			that triger an action.  Only processed by the MIF.
     *
     * PARAM_MIF_RW:	A MIF Read/Write parameter.  Only processed by the MIF.
     *
     * PARAM_IP_ADDRESS	An IP address. Displayed in "dot notation", but stored
     *			in network byte order (big-endian).
     *
     * PARAM_HEX:	A field displayed as hex digits.
     *
     * PARAM_ENET:	A field assumed to contain a 48 bit ethernet number
     *			displayed as 12 hex digits.
     *
     * PARAM_SIDE_BY_SIDE: 
     *			A field with two 4 digit numeric values separated
     *			with a slash 
     *
     * PARAM_TEXT_STATIC:
     *			A text field that cannot be edited by the user.
     *
     * PARAM_VARDATA:
     *			A field with a 2-byte size in host byte order,
     *			followed by an arbitrary number of bytes of
     *			binary data.  The whole field's size is limited
     *			by the size of char array which it defines, and
     *			is expressed in the field's parameter structure.
     *
     * PARAM_GROUP_START_NONE_TRUE:
     * PARAM_GROUP_START_ANY_TRUE:
     * PARAM_GROUP_START_ALL_TRUE:
     * PARAM_GROUP_START_ANY_FALSE:
     * PARAM_GROUP_END:
     *			Indicates that the elements in a 'ParamNAInfo' list
     *			between 'PARAM...START...' and 'PARAM...END'
     *			are to be taken as a group.
     *
     *			The paramater will be marked as 'NA' in lcdparam.c,
     *			if the elements satisfy the 'PARAM...START...' 
     *			condition.
     *			
     *			See lcdparam.c:_isNA()
     *			and lcdconn.c: ..."force56"
     *
     *			EG:  '...NONE_TRUE' means that the parameter is
     *			     marked 'NA' if none of the elements are TRUE.
     *
     * PARAM_NET2NET_SLOT_PORT_CH:
     *			Uint16: 1000*slotNumber + 100*portNumber + channel
     *
     * PARAM_NULL:	A termination, or non-field.
     */
typedef enum {
    PARAM_ENUM,
    PARAM_BOOLEAN,
    PARAM_NUMBER,
    PARAM_LONG,
    PARAM_ULONG,
    PARAM_TEXT,
    PARAM_SUB_MENU,
    PARAM_TELEPHONE,
    PARAM_TIME,
    PARAM_DATE,
    PARAM_TEXT_HIDDEN,
    PARAM_ENUM_SPARSE,
    PARAM_NUMBER_OR_GROUP,
    PARAM_MIF_RONLY,
    PARAM_MIF_WONLY,
    PARAM_MIF_RW,
# if ( IP == FYES )
    PARAM_IP_ADDRESS,
# endif
# if ( ETHERNET == FYES )
    PARAM_HEX,
    PARAM_ENET,
# endif
# if ( X25_DTE == FYES )
    PARAM_SIDE_BY_SIDE,
# endif
    PARAM_TEXT_STATIC,
# if ( FIREWALLS == FYES )
    PARAM_VARDATA,
# endif
    PARAM_GROUP_START_NONE_TRUE,
    PARAM_GROUP_START_ANY_TRUE,
    PARAM_GROUP_START_ALL_TRUE,
    PARAM_GROUP_START_ANY_FALSE,
    PARAM_GROUP_END,
# if ( MAXDAX == FYES )
    PARAM_NET2NET_SLOT_PORT_CH,
# endif
# if ( ( ETHERNET == FYES ) && ( IPX_ROUTE == FYES ) )
    PARAM_HEX_RIGHT_JUSTIFIED,
# endif
    PARAM_NULL
} LcdParameterType;

    /*
     * Each individual enumeration value in an enumeration selection has
     * the information contained in the structure below.  A full enumeration
     * field is made up of an array of these fields, terminated by one with
     * a NULL enumName pointer.
     *
     *	enumName:	The text to display to the user.
     *
     *	enumValue:	The numeric value associated with the name.
     */
typedef struct {
    const char	*enumName;
    int		enumValue;
} LcdParamEnumValue;

    /*
     * When a parameter is of type "number", the required information is
     * the minimum and maximum values.  This is the information in the 
     * number structure.
     *
     *	minValue:	The minimum value for the number.
     *
     *	maxValue:	The maximum value for the number.
     *
     *	titleRow:	The row the title is in.
     *
     *	titleColumn:	The column the title is in.
     */
typedef struct {
    Word	minValue;
    Word	maxValue;
    Byte	titleRow;
    Byte	titleColumn;
} LcdParamNumberRange;

    /*
     * When a parameter is of type "long", the required information is
     * the minimum and maximum values.  This is the information in the 
     * long number structure.
     *
     *	minValue:	The minimum value for the long.
     *
     *	maxValue:	The maximum value for the long.
     *
     *	titleRow:	The row the title is in.
     *
     *	titleColumn:	The column the title is in.
     */
typedef struct {
    LongWord	minValue;
    LongWord	maxValue;
    Byte	titleRow;
    Byte	titleColumn;
} LcdParamLongRange;

    /*
     * When a parameter is of type "unsigned long" we can use the same
     * structure, because it stores the min and max values in unsigned
     * long anyways.
     */
typedef LcdParamLongRange LcdParamULongRange;

    /*
     * For fields of type time or date, the information required is:
     *
     *	titleRow:	The row the title is in.
     *
     *	titleColumn:	The column the title is in.
     */
typedef struct {
    Byte		titleRow;
    Byte		titleColumn;
} LcdTimeDateInfo;


    /*
     * For fields of type text, the information required is:
     *
     *	maxLength:		The maximum length the text string.
     *
     *	subsetList:		The list of acceptable characters, or NULL if 
     *				all characters are acceptable.
     *
     *	titleRow:		The row the title is in.
     *
     *	titleColumn:		The column the title is in.
     *
     *	allowInlineEdit:	TRUE if the edit field should be moved to
     *				where the "parent list field has it" or
     *				FALSE if it should always be hard-coded to
     *				the titleRow starting point.
     */
typedef struct {
    Word		maxLength;
    const char		*subsetList;
    Byte		titleRow;
    Byte		titleColumn;
    Boolean		allowInlineEdit;
} LcdTextInfo;

    /*
     * Function called to determine whether the specific hidden field is
     * hidden or not.  If it is hidden, then the function should return TRUE,
     * and the string to be shown when it is hidden should be returned in
     * *hiddenString, null terminated.
     *
     * Parameters:
     *  userPtr:	A pointer defined by the user and passed in
     *			the calls to lcdParamUpdateText and
     *			lcdParamProcessListSelection.
     *
     *	hiddenString:	Location to place string to be displayed for hidden
     *			parameter, if function returns true.
     *
     *			Note that NULL may be passed as the hiddenString.
     *			If it is, then the string should _not_ be copied.
     *
     * Returns:
     *	<>		TRUE if field should be hidden and FALSE otherwise.
     *
     *	*hiddenString:	If function returns TRUE, string to be displayed 
     *			instead of field value.
     */
typedef Boolean FieldHiddenFnc( Pointer		userPtr,
				char		*hiddenString );


    /*
     * For hidden text fields, we have the standard text information
     * and the checkFunction.
     */
typedef struct {
    LcdTextInfo		textInfo;
    FieldHiddenFnc	*hiddenCheckFnc;
} LcdHiddenTextInfo;


    /*
     * Number or group is identical to number, but it is typedef'd for 
     * clarity.
     */
typedef LcdParamNumberRange	LcdNumberOrGroupInfo;


    /*
     * For fields of type Hex the information required is:
     *	
     *	maxLength:		The maximum length the hex string in
     *				octets.  The number of hex digits displayed
     *				is twice this number.
     *
     *	titleRow:		The row the title is in.
     *
     *	titleColumn:		The column the title is in.
     *
     *	allowInlineEdit:	TRUE if the edit field should be moved to
     *				where the "parent list field has it" or
     *				FALSE if it should always be hard-coded to
     *				the titleRow starting point.
     */
typedef struct {
    Word		maxLength;
    Byte		titleRow;
    Byte		titleColumn;
    Boolean		allowInlineEdit;
} LcdHexInfo;


    /*
     * For fields of type Ethernet Address, the information required is:
     *
     *	titleRow:	The row the title is in.
     *
     *	titleColumn:	The column the title is in.
     */
typedef struct {
    Byte		titleRow;
    Byte		titleColumn;
} LcdEthernetInfo;

    /*
     * IP Addresses are stored in 32 bits.
     */
typedef Uint32			IpAddress;


    /*
     * This functions attempts to convert an IP address in ASCII dot
     * with an optional netmask part to a pair of IpAddress.  Note:
     * An IpAddress is always stored in network byte order.
     *
     * Parameters:
     *
     *  string:		Pointer to a NULL terminated IP address in dot 
     *			notation followed by an optional /nn to indicate
     *			the number leading of bits in the netmask.
     * 
     *  ipAddress:	Pointer to an IpAddress where the converted
     *			address will be stored.
     *
     *	netmask:	Pointer to an IpAddress where the netmask
     *			will be stored.  If no netmask is passed as
     *			as part of the address the default netmask will
     *			be stored here.
     *
     * Returns:
     *	<>		TRUE if valid conversion, FALSE otherwise.
     *
     *	*ipAddress:	If function returns TRUE, the IP address in NBO.
     *	*netmask:	If function returns TRUE, the netmask in NBO.
     */
extern Boolean
ipAddressStringToValue( const char*	string,
			IpAddress*	ipAddress,
			IpAddress*	netmask );

    /*
     * Convert the IP Address from the source IpAddress to the destination
     * string, making sure not to copy more than maxLength valid characters.  
     *
     * Parameters:
     * 	destination:		Address to put string.
     *
     *	source:			address containing pointer to the IP Address.
     *
     *	netmask:		Pointer to the (optional) netmask associated
     *				with this address.  If non zero the number
     *				of bits in the netmask will be appended to the
     *				destination as /nn.
     *
     *	maxLength:		The maximum length of the string.
     *
     * Returns:
     *	*destination:		The converted string.
     */
extern void
convertIpAddressString( char		*destination, 
		        IpAddress	*source,
		        IpAddress	*netmask,
		        Byte		maxLength);


typedef enum {
    SECURE_OPS,
    EDIT_SECURITY,
    EDIT_SYSTEM,
    EDIT_LINE,
    EDIT_OWN_PORT,
    EDIT_ALL_PORT,
    EDIT_CUR_CALL,
    EDIT_OWN_CALL,
    EDIT_COM_CALL,
    EDIT_ALL_CALL,
    SYS_DIAG,
    OWN_PORT_DIAG,
    ALL_PORT_DIAG,
    DOWNLOAD,
    UPLOAD,
    FIELD_SERVICE,
    EDIT_OWN_SLOT,
    EDIT_ALL_SLOT
} SecurityItem;

# define   str2Ip(str,ip,mask)  ipAddressStringToValue(str,ip,mask)
# define   ip2Str(str,ip,mask,len)  convertIpAddressString(str, ip, mask, len)

#endif /* PARAMS_H */
