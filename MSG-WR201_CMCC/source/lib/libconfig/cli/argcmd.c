/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: argcmd.c,v 1.2 2012/08/29 10:00:59 yu.dehai Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "argcmd.h"
#include "cmd.h"
#include "slotmgr.h"
#include "mibinet.h"
#include "mibroute.h"
#include "mibtype.h"
#include "profiles.h"

# undef	 CLI_ARGCMD_DEBUG
# define CLI_ARGCMD_DEBUG

# if defined( CLI_ARGCMD_DEBUG )

int cliArgCmdDumping = FALSE;

#define PRINTF( x )          do {                               \
                                    if ( cliArgCmdDumping ) {   \
                                        printf x ;              \
                                    }                           \
                                } while (0)
#else

#define PRINTF(x);

#endif

#if ( WEB_SERVER == FYES )
extern WebProfile *webConfigProf;
extern SecurityUserProfile *secUserProf;
extern SecurityGroupProfile *secGroupProf;
extern SecurityUrlProfile *secUrlProf;
extern AdvertiseProfile *advertiseProf;		//added by jfgu@090603
extern JoinButtonProfile *joinbuttonProf;	//added by dhyu@090611
#endif
 //add end jxy8
# if (OLDNAT)
extern natAddrPoolMap *natPool;
# else
# endif
extern FilterProfile *filterProf;
extern FilterEntry *filterEntry;

#if ( CBQ == FYES )
extern CbqClassProfile *cbqClassProf;
extern CbqFilterProfile *cbqFilterProf;
extern SfqQdiscProfile *sfqQdiscProf;
#endif

#if (DHCP_PLUS == FYES)
extern RawOptionClassProfile* rawoptClassProf;
extern DhcpPoolClassProfile* dhcpPoolClassProf;
#endif

extern Boolean silent;

#if (MDF_IKE == FYES && MDF_IP_SEC == FYES)
extern IPSecConfProfile *ipsecConfProf; 	/* add by lyu */
extern IPSecTransformSetProfile *ipsecTransformSetProf;
extern IsakmpConfProfile *isakmpConfProf;
extern IsakmpPolicyProfile *isakmpPolicyProf;
#endif

# if (FIREWALL == FYES)
extern FwAddressProfile *fwAddressProf;  /* add by cyh */
extern FwServiceProfile *fwServiceProf;
extern FwL7filterProfile *fwL7filterProf;
# endif

# if (PDB == FYES)
extern PdbProfile *pdbProf;  /* add by cyh */
# endif

#if (SWITCHFUN == FYES)  /* add by zgd */
extern SwitchProfile *switchProf;
extern Port2Profile *port2Prof;
extern PortMacProfile *portmacProf;
#if (TAGVlAN == FYES)
extern TagVlanProfile *tagvlanProf;
#endif
#endif

#if(DOT1X == FYES)
extern Dot1xProf * dot1xProf;
extern Dot1xUserProf * dot1xUserProf;
#endif

#if ( ISP_CFG == FYES )
extern IspProfile *ispProf;
#endif

volatile static char nullstr[] = "**NULL**";

#if (MDF_UPNP == FYES)
extern UpnpConfProfile *UpnpConfProf;
#endif

#if (FIREWALLS == FYES)
extern FwallProfile  *fwallProf;
#endif

# if ( SNMP_TRAP == FYES )
extern TrapProfile *trapProf;
# endif

#if (PPPOE_SRV == FYES) 	/* added by llliu */
extern PppoeServerProfile *_pppoeSrvProf;
extern MacFilterProfile *_macFilterProf;
#endif

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang */
extern PortGroupProfile *portGroupProf;
extern IpGroupProfile *ipGroupProf;
extern const IpGroupProfile * buildFactoryIpGroupProfile(  );
extern const PortGroupProfile * buildFactoryPortGroupProfile(  );
#endif
#endif

extern EtherFilterProfile *etherFilterProf;
extern const EtherFilterProfile * buildFactoryEtherFilterProfile(  );

/*
 * added by llliu.
 */
extern DnsFilterProfile *dnsFilterProf;


/*
 * added by llliu@081107
 */
#if (POLICY_ROUTE == FYES)
extern PolicyRouteProfile *policyRouteProf;
#endif


/*
 * added by llliu@081201.
 */

#if (FC == FYES)
extern RateLimitProfile *rateLimitProf;
#endif

extern NetActionManageProfile *netActionManageProf;

//added by dhyu
#if (NOTICE == FYES)
extern NoticeProfile *noticeProf;
#endif

#if (PPPOE_SRV == FYES)
extern PppoeOnlyProfile *pppoeOnlyProf;
#endif

static void _profInitParam(MibProfileType proftype, int *inst, int *index, SlotNumber *slot);

void hexValueToString( char* dest,
                   const Byte*  source,
                   int          destLen,
                   int          sourceLen );

extern void
hexStringToValue( Byte*         dest,
                  const char*   src,
                  int           destLen );

    
int bcdToInt(int bcd)
{
    int i, j;

    for(i = 0, j = 1;bcd;j *= 10, bcd >>= 4)
        i += (bcd & 0xf)*j;
    return i;
}

int intToBcd(int x)
{
    int i, j;
    for(i = 0, j = 0;x;j += 4, x /= 10)
        i |= x % 10 << j;
    return i;
}


char _tolower(char c)
{
    if(c >= 'A' && c <= 'Z')
	return c + 0x20;
    return c;
}
#if 0
int _strlen(char *string)
{
    char *str1;
    
    for(str1 = string;*str1 && str1 - string < MAX_CMDLINE;str1++);
    return str1 - string;
}

char *_strcpy(char *dest, const char *_src)
{
    char *str1, *src = (char *)_src;
    
    if(!src) 
    {
	*dest = 0;
	return dest;
    }
    for(str1 = dest;*src && str1 - dest < MAX_CMDLINE;str1++, src++) *str1 = *src;
    *str1 = *src;
    return dest;
}

char *_strncpy(char *dest, char *src, int len)
{
    int i;
    
    for(i = 0;i < len;i++) *(dest + i) = *(src + i);
    return dest;
}

int _strncmp(char *dest, char *src, int len)
{
    int i;
    
    for(i = 0;i < len;src++, dest++, i++)
    {
	if(*dest > *src) return 1;
	if(*dest < *src) return -1;
    }
    return 0;
}

int _strncmpi(char *dest, char *src, int len)
{
    int i;

    for(i = 0;i < len;src++, dest++, i++)
    {
	if(_tolower(*dest) > _tolower(*src)) return 1;
	if(_tolower(*dest) < _tolower(*src)) return -1;
    }
    return 0;
}
    
char *_strstr(char *dest, const char *_src)
{
    int len, i;
    char *src = (char *)_src;

    len = _strlen(src);
    for(i = 0;i < MAX_CMDLINE && *dest;dest++)
	if(!_strncmp(dest, src, len)) return dest;
    return 0;
}

char *_strcat(char *dest, char *src)
{
    char *str1;
    
    str1 = dest + _strlen(dest);
    _strcpy(str1, src);
    return dest;
}

char *_strlcat(char *dest, const char *_src)
{
    return _strlcat(dest, (const char*)_src);
}

char *_strlcat(char *dest, const char *_src)
{
    int i, destlen, srclen;
    char *src = (char *)_src;
    
    destlen = _strlen(dest);
    srclen = _strlen(src);
    if(!srclen) return dest;
    for(i = destlen + srclen;i >= srclen;i--)
	*(dest + i) = *(dest + i - srclen);
    return _strncpy(dest, src, srclen);
}

void prints(TermInfo *term, int title, const char *string, ...)
{
    va_list args;
    volatile int i, len;
    char *tmp;
    TermCmd *cmd = (TermCmd *)term->cmd;

    tmp = MALLOC(MAX_CMDLINE);
    ASSERT(tmp);

    for(i = 0;i < (len = (cmd->level - term->curLevel)*2);i++)
	_printf(term, " ");    
    va_start(args, string);
//    if ( islower(*(char*)*(void **)args) )
//	*(char*)*(void **)args = toupper(*(char*)*(void **)args) ;
    vsnprintf(tmp, MAX_CMDLINE-2, string, args);
    if(title)
	for(i = PREFIX_LEN - _strlen(tmp) - len;i > 0;i--)
	    _strcat(tmp, (char*)" ");    
    lcdTermPutStr(term->lcdTerm, tmp);
//    if ( isupper(*(char*)*(void **)args) )
//        *(char*)*(void **)args = tolower(*(char*)*(void **)args);
//    va_end(args);

    FREE(tmp);
}

void _printf(TermInfo *term, const char *string, ...)
{
    char *tmp = NULL;
    va_list args;

    if(silent) return;
    tmp = MALLOC(MAX_CMDLINE);
    ASSERT(tmp);
//    printf("MEMAV: %lu\n", memAvail());

    va_start(args, string);
    vsnprintf(tmp, MAX_CMDLINE-2, string, args);
    va_end(args);
    lcdTermPutStr(term->lcdTerm, tmp);
    
    FREE(tmp);
}
#endif

void sprintip(char *buf, int ip)
{
    int i;
    char *tmp=NULL;

    *buf = 0;
    tmp = (char*)MALLOC(MAX_CMDLINE);
    ASSERT(tmp);

    ip = htonl(ip);
    for(i = 0;i < 4;i++)
    {
	snprintf(tmp, MAX_CMDLINE-2, "%d", (ip & 0xff000000) >> 24);
	_strcat(buf, tmp);
	ip <<= 8;
	if(i < 3) _strcat(buf, (char *)".");
    }
    FREE(tmp);
}

#if 0
void sprintdatetime(char *buf, int datetime)
{
    ExpandedTime expTime;
    ExpandedDate expDate;

    absoluteTimeToExpandedTime( (AbsoluteTime)datetime, &expTime, TRUE );
    absoluteTimeToExpandedDate( (AbsoluteTime)datetime, &expDate );
    sprintf( buf, "%d-%d-%d %2d:%02d:%02d",
             expDate.year, expDate.month, expDate.date,
             expTime.hours, expTime.minutes, expTime.seconds );
}

void sprinttime(char *buf, int datetime)
{
    ExpandedTime expTime;

    absoluteTimeToExpandedTime( (AbsoluteTime)datetime, &expTime, TRUE );
    sprintf( buf, "%2d:%02d:%02d",
             expTime.hours, expTime.minutes, expTime.seconds );
}
#endif

char *skipSpace(char *string)
{
    int i;
    for(i = 0;*string && *string == 0x20 && i < MAX_CMDLEN;i++) string++;
    return string;
}

#if 0
int argGetCmd(char *string, int num, char *buf)
{
    int i;
    char *strtmp;
    
    if((i = argGet(string, num, buf)))
    if((strtmp = _strstr(buf, "/")))
    {
	*strtmp = 0;
	i = strtmp - buf;
    }
    return i;
}

int argGetInstInt(TermInfo *term)
{
    char tmp[MAX_CMDLEN];

    if(argGetInst(term->lastInput, ((TermCmd*)term->cmd)->level + 1, tmp) && isDigit(tmp))
	return atoi(tmp);
    else
	return FAIL_CLI;
}

int argGetInst(char *string, int num, char *buf)
{
    char *strtmp;
    
    if(argGet(string, num, buf))
    {
	if((strtmp = _strstr(buf, "/")) && *++strtmp)
	{
	    _strcpy(buf, strtmp);
	    return _strlen(buf);
	}
    }
    return 0;
}


#endif
char *argGetInt(char *string, int num, int *len)
{
    int i=0, ii=0;
    char *recpos;
    
    recpos = string;
    for(i = 0;i < num && *string;i++)
    {
	string = skipSpace(string);
	recpos = string;
	for(ii = 0;*string && *string != 0x20 && ii < MAX_CMDLINE;ii++)string++;
    }
    if(i < num)
    {
	return 0;
    }
    else
    {
	*len = ii;
	return recpos;
    }
}

int argGet(char *string, int num, char *buf)
{
    int len;
    char *recpos;

    if(!(recpos = argGetInt(string, num, &len)))
    {
	*buf = 0;
	return 0;
    }
    len = MIN(len, MAX_CMDLEN - 1);
    strncpy(buf, recpos, len);
    buf[len] = 0;
    
    return len;
}

char *argShift(char *string)
{
    char *ptr, *string1;
    
    string1 = string;
    while(*string1 && *string1 != 0x20) string1++;
    string1 = skipSpace(string1);
    for(ptr = string;*string1;ptr++, string1++)
	*ptr = *string1;
    *ptr = 0;
    return string;
}

void argStripLast(char *string)
{
    int i;
    
    for(i = _strlen(string);i;i--)
	if(string[i] == 0x20)
	{
	    string[i] = 0;
	    return;
	}
    string[0] = 0;
}       

void argAppend(char *dest, char *src)
{
    if(*dest) _strcat(dest, (char *)" ");
    _strcat(dest, src);
} 

int isDigit(char *str)
{
    while(*str)
    {
	if((*str > '9' || *str < '0') && *str != '-') return FALSE;
	str++;
    }
    return TRUE;
}

    /*
     * Check if all of the characters in the textString can be found in the
     * subset list.  If so, return TRUE.  Otherwise, return FALSE.
     */

Boolean argMatchTextList( char         *textString,
                          const char   *subsetList )
{
    if (( ! subsetList ) || ( ! *subsetList )) {
        return TRUE;
    }
    while( *textString ) {
        if( ! strchr( subsetList, (int)(*textString)) ) {
            return FALSE;
        }
        textString++;
    }
    return TRUE;
}

int _getInt(Pointer prof, int offset, int isize)
{
    switch(isize)
    {
    case 1:
	return *(unsigned char*)((Byte*)prof + offset);
    case 2:
	return LOAD_WORD((Byte*)prof + offset);
    case 4:
    default:
	return LOAD_LONG((Byte*)prof + offset);
    }
    return FAIL_CLI;
}

void _setInt(Pointer prof, int offset, int isize, int value)
{
    switch(isize)
    {
    case 1:
	*(unsigned char*)((Byte*)prof + offset) = value;
	break;
    case 2:
	STORE_WORD((Byte*)prof + offset, value);
	break;
    case 4:
	STORE_LONG((Byte*)prof + offset, value);
	break;
    }
}

const char *enumString(const LcdParamEnumValue *enum1, int value)
{
    while ( enum1 && enum1->enumName )
    {
	if(enum1->enumValue == value)
	    return enum1->enumName;
	enum1++;
    }
    return NULL;
}

int enumValue(const LcdParamEnumValue *enum1, char *string)
{
    int i, found;
    const LcdParamEnumValue *enum2 = 0;

    found = 0;
    i = strlen(string);
    while ( enum1 && enum1->enumName )
    {
	PRINTF(("%s: %d Name %s str %s value %d\n", __FUNCTION__, __LINE__, (char *)enum1->enumName, string, enum1->enumValue));
	if(!_strncmpi((char *)enum1->enumName, string, i))
	{
	    found++;
	    enum2 = enum1;
	    if(_strlen((char *)enum1->enumName) == i)
	    {
		found = 1;
		break;
	    }
	}
        enum1++;
    }
    if(found == 1)
	return (int)enum2->enumValue;
    else
	return FAIL_CLI;
}

void enumList(const LcdParamEnumValue *enum1, TermInfo *term, const char *string)
{
    size_t i;

    i = strlen(string);
    while( enum1 && enum1->enumName )
    {
	if(!i || (i && !_strncmpi((char *)enum1->enumName, (char *)string, i)))
//	    _printf(term, "  %s", enum1->enumName);	    
	    _printf(term, CLI_LIST_FMT, enum1->enumName, "help message not available");
	enum1++;
    }
    _printf(term, "\r\n");
}

int validateNumber(TermInfo *term, int cmdindex, int isize, char *buf)
{

    argGet(term->lastInput, cmdindex, buf);
    if(!isDigit(buf))
    {
	if (!noPrompt) {
          _printf(term, "Only digits allowed.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    if(_strlen(buf) > isize)
    {
	if (!noPrompt) {
	  _printf(term, "Number is too long.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }

    return OK_CLI;
}

int validateInt(TermInfo *term, int cmdindex, int range)
{
    char tmp[MAX_CMDLEN];

    argGet(term->lastInput, cmdindex, tmp);
    if(!isDigit(tmp))
    {
	if ( !noPrompt) {
	  _printf(term, "Invalid value.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    else 
	return atoi(tmp);
}

int validateEnum(TermInfo *term, int cmdindex, LcdParamEnumValue *enum1)
{
    char tmp[MAX_CMDLEN];
    int value;

    if(!argGet(term->lastInput, cmdindex, tmp) || (value = enumValue(enum1, tmp)) == FAIL_CLI)
    {
	if (!noPrompt) {
	  enumList(enum1, term, "");
	  _printf(term, "Invalid Enum value.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    else
    	return value;
}

int validateStr(TermInfo *term, int cmdindex, int isize, char *buf)
{
    argGet(term->lastInput, cmdindex, buf);
    if(_strlen(buf) >= isize)
    {
	if (!noPrompt) {
	  _printf(term, "String value is too long.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    return OK_CLI;
}

int validateIp(TermInfo *term, int cmdindex, int *value)
{
    IpAddress ip, mask;
    char tmp[MAX_CMDLEN];

    if(!argGet(term->lastInput, cmdindex, tmp) || !ipAddressStringToValue(tmp, &ip, &mask))
    {
	if (!noPrompt) {
	  _printf(term, "Invalid IP address.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    else
    {
	*value = (int)ip;
	return 1;
    }
}

/* added by zhz */
int validateHex(TermInfo *term, int cmdindex, int isize, char *buf)
{
    char tmp1[MAX_HEX_LEN*2+1];

    argGet(term->lastInput, cmdindex, buf);

    if(!strlen(buf)) return OK_CLI;

    if( ( strlen(buf) < ((2*(unsigned int)isize)+1) )
	  &&( strlen(buf) < sizeof(tmp1) )
            && argMatchTextList( buf, lcdParamHexDigits ) ) {
    	return OK_CLI;
    } else {
	if (!noPrompt) {
	  _printf(term, "Invalid hex. format is: %d hex digits[0-9,a-f,A-F].\r\n", min(2*isize, 2*MAX_HEX_LEN));
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
    	return FAIL_CLI; 
    }
}

int validateMac(TermInfo *term, int cmdindex, char *buf)
{
    Byte tmp[ETHER_ADDR_LEN];
    char tmp1[ETHER_ADDR_LEN*2+1];

    argGet(term->lastInput, cmdindex, buf);
    if(!strlen(buf)) return OK_CLI;
    hexStringToValue(tmp, buf, ETHER_ADDR_LEN);
    hexValueToString(tmp1, tmp, ETHER_ADDR_LEN*2, ETHER_ADDR_LEN);
    if(!strcmpi(buf, tmp1))
    	return OK_CLI;
    else
    {
	if (!noPrompt) {
	  _printf(term, "Invalid MAC. format is: 12 hex digits[0-9,a-f,A-F].\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
    	return FAIL_CLI; 
    }
}

#if 0
int validateDateTime(TermInfo *term, int cmdindex, AbsoluteTime *value)
{
    ExpandedDate date;
    ExpandedTime time;
    char *tmp;
    int i;

    if(!(tmp = argGet(term->lastInput, cmdindex, &i)) 
	|| dateStrToExpandedDateTime(tmp, &date, &time))
    {
	if ( !noPrompt ) {
          _printf(term, "Invalid datetime format. format is: [yyyy-mm-dd] [hh:mm:ss]\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    else
    {
	*value = expandedDateToAbsoluteDay(&date) * SECONDS_PER_DAY
                        + expandedTimeToAbsoluteTime(&time);
	return 1;
    }
}

int validateTime(TermInfo *term, int cmdindex, AbsoluteTime *value)
{
    ExpandedTime time;
    char *tmp;
    int i;

    if(!(tmp = argGet(term->lastInput, cmdindex, &i)) 
	|| timeStrValue(tmp, &time, 0))
    {
	if (!noPrompt) {
          _printf(term, "Invalid time format. format is: hh:mm:ss\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    else
    {
	*value = expandedTimeToAbsoluteTime(&time);
	return 1;
    }
}
#endif

static void _profInitParam(MibProfileType proftype, int *inst, int *index, SlotNumber *slot)
{
   
    slot->slotNumber = SLOT_NUMBER_ANY;
    *index = 0;
    switch(proftype)
    {
    case MIB_PROF_NAT:
    case MIB_PROF_FILTER:
    case MIB_PROF_WEB: //add by jxy8@2003-12-26 for WebUserRight
	*index = 0;
	break;
    case MIB_PROF_INTERNET:
    case MIB_PROF_TIMERANGE: /*add by chenyunhua*/
    case MIB_PROF_NAME_PASS:
#if (PDB == FYES)
    case MIB_PROF_PDB:       /* add by cyh */
#endif
#if (SWITCHFUN == FYES)  /*add by zgd*/
    case MIB_PROF_SWITCH:
#endif
#if(DOT1X == FYES)
    case MIB_PROF_DOT1X:
#endif
#if ( ISP_CFG == FYES )
    case MIB_PROF_ISP:
#endif
#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang*/
    case    MIB_PROF_PORT_GROUP:
    case    MIB_PROF_IP_GROUP:
#endif
#endif
	*index = inst[1];
	break;
    case MIB_PROF_ETHER_FILTER:    /* add by brwang */
    case MIB_PROF_TIMERANGE_POLICY:    /* add by brwang */
    case MIB_PROF_FILTER_IN:
    case MIB_PROF_FILTER_OUT:
    case MIB_PROF_SECURITY:
    case MIB_PROF_CBQ_CLASS:
    case MIB_PROF_CBQ_FILTER:
    case MIB_PROF_SFQ_QDISC:
    case MIB_PROF_IPSEC_CONF:
    case MIB_PROF_IPSEC_TRANSFORMSET:
    case MIB_PROF_ISAKMP_CONF:
    case MIB_PROF_ISAKMP_POLICY:
#if (FIREWALL == FYES)
    case MIB_PROF_FWADDRESS:   /* add by cyh */
    case MIB_PROF_FWSERVICE:   
    case MIB_PROF_FWL7FILTER:   
#endif
#if (SWITCHFUN == FYES)  /*add by zgd*/
    case MIB_PROF_PORT2:
    case MIB_PROF_PORTMAC:
    #if (TAGVlAN == FYES)
    case MIB_PROF_TAGVLAN:
    #endif
#endif
#if(DOT1X == FYES)
    case MIB_PROF_DOT1X_USER:
#endif
    //add by jxy8@2003-12-26 for WebUserRight
    case MIB_PROF_SECURITY_USER:
    case MIB_PROF_SECURITY_GROUP:
    case MIB_PROF_SECURITY_URL:
    //add end jxy8
#if (PPPOE_SRV == FYES)	/* added by llliu */
    case MIB_PROF_PPPOE_SRV:
    case MIB_PROF_MAC_FILTER:
#endif 
    /*
     * added by llliu@080428.
     */
    case MIB_PROF_DNS_FILTER:

    case MIB_PROF_NET_ACTION_MANAGE:

    case MIB_PROF_QQ_EXCEPTION:

#if (FC == FYES)
    case MIB_PROF_RATE_LIMIT:
#endif

#if (PPPOE_SRV == FYES)
    case MIB_PROF_PPPOE_ONLY:
#endif


	*index = inst[2];
	break;
#if (DHCP_PLUS == FYES)
   case MIB_PROF_DHCP_RAWOPTION:
	*index = inst[3];
	break;
   case MIB_PROF_DHCP_POOL:
	*index = inst[3];
	break;
#endif

    case MIB_PROF_ROUTE:
    case MIB_PROF_NAT_STATIC:
# if (OLDNAT)
    case MIB_PROF_NAT_POOL:
# else
    case MIB_PROF_NAT_BINDING:
# endif

    /*
     * added by llliu@081107.
     */
#if (POLICY_ROUTE == FYES)
    case MIB_PROF_POLICY_ROUTE:
#endif

    /*
     * added by llliu@081201.
     */
    case MIB_PROF_PUBLIC_DDNS:


	*index = inst[3];
	break;
    case MIB_PROF_NET_BRI_TE:
    case MIB_PROF_SLOT:
	slot->slotNumber = inst[2];
	break;

# if ( UPNP == FYES )
    case MIB_PROF_UPNP:
	*index = inst[2];
	break;
# endif

# if ( FIREWALLS == FYES )
    case MIB_PROF_FWALL:
	*index = inst[1];
	break;
# endif

    case MIB_PROF_TRAP:
	*index = inst[1];
	break;
#if 0
//added by dhyu@090611
    case MIB_PROF_JOIN_STARTMENU:
	*index = inst[2];
	break;
#endif

    case MIB_PROF_CONNECTION_LIMIT:
	*index = inst[3];
	break;

    case MIB_PROF_DNS_RESOLVE:
	*index = inst[2];
	break;
//added by dhyu
#if (NOTICE == FYES)
    case MIB_PROF_NOTICE:
	*index = inst[1];
	break;
#endif



	
    default:
	break;
    }
}

Pointer _profRead(MibProfileType proftype, int *inst)
{
    SlotNumber slot;
    int index;

    _profInitParam(proftype, inst, &index, &slot);
    return _profRead2(proftype, index, slot);
}

Pointer _profRead2(MibProfileType proftype, int index, SlotNumber slot)
{
    Pointer prof;

#if 0
    if(proftype == MIB_PROF_NAT_STATIC)
    {
	returnProfilePointer(slot, MIB_PROF_NAT, 0, &prof);
	return (Pointer)((int)prof + (int)natProf->staticMaps + index*sizeof(StaticMapEntry));
    }	
# if (OLDNAT)
    else if(proftype == MIB_PROF_NAT_POOL) {
# else
    else if(proftype == MIB_PROF_NAT_BINDING) {
# endif
	returnProfilePointer(slot, MIB_PROF_NAT, 0, &prof);
# if (OLDNAT)
	return (Pointer)((int)prof + (int)natProf->natPool + index*sizeof(natAddrPoolMap));
# else
	return (Pointer)((int)prof + (int)natProf->natBinding + index*sizeof(NatBinding));
# endif
    }
    else if(proftype == MIB_PROF_FILTER_IN)
    {
	returnProfilePointer(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, &prof);
	return (Pointer)((int)prof + (int)filterProf->inputFilters + index*sizeof(FilterEntry));
    }	
    else if(proftype == MIB_PROF_FILTER_OUT)
    {
	returnProfilePointer(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, &prof);
	return (Pointer)((int)prof + (int)filterProf->outputFilters + index*sizeof(FilterEntry));
    }	
    else
#endif
	returnProfilePointer(slot, proftype, index, &prof);
    return prof;
}

Boolean _profWrite(MibProfileType proftype, int *inst, Pointer prof)
{
    SlotNumber slot;
    int index;

    _profInitParam(proftype, inst, &index, &slot);
    return _profWrite2(proftype, index, slot, prof);
}

const char *profGetStr(MibProfileType proftype, int *inst, int offset)
{
    volatile static char tmp[MAX_CMDLEN];
    Pointer prof = _profRead(proftype, inst);
    if(prof) {
	CLIPRINTF(("%s: got %s ", __FUNCTION__, (const char*)prof+offset));
	memset((void*)tmp, 0, sizeof(tmp));
	strncpy((char*)tmp, (const char*)prof + offset, sizeof(tmp)-2 );
	releaseProfilePointer(proftype, prof);
	return (char*)tmp;
    } else {
	CLIPRINTF(("%s: got %s ", __FUNCTION__, nullstr));
	printf("%s:%d got %s ", __FUNCTION__, __LINE__, nullstr);
	return (char*)nullstr;
    }
}

/* add by zhz */
const char *profGetHex(MibProfileType proftype, int *inst, int offset, int isize)
{
    volatile static char hex[2*MAX_HEX_LEN+1];
    const char* phex;
    int i;
    
    if ( isize > MAX_HEX_LEN ) {
        isize = MAX_HEX_LEN;
    }

    Pointer prof = _profRead(proftype, inst);
    if(prof){
		i = 0;
    		memset((void*)hex, 0, sizeof(hex));
		phex = (const char*)prof + offset;
    		
		hexValueToString((char*)hex, (const Byte*)phex, 2*MAX_HEX_LEN, isize);

		releaseProfilePointer(proftype, prof);
		return (char*)hex;
	}
    else
	return (char*)nullstr;
	
}

/* add by hqguan */
const char *profGetMac(MibProfileType proftype, int *inst, int offset)
{
    volatile static char mac[2*ETHER_ADDR_LEN+1];
    const char* pmac;
    
    Pointer prof = _profRead(proftype, inst);
    if(prof){
    		memset((void*)mac, 0, sizeof(mac));
		pmac = (const char*)prof + offset;
    		
		hexValueToString((char*)mac, (const Byte*)pmac, 2*ETHER_ADDR_LEN, ETHER_ADDR_LEN);

		releaseProfilePointer(proftype, prof);
		return (char*)mac;
	}
    else
	return (char*)nullstr;
	
}

int profGetInt(MibProfileType proftype, int *inst, int offset, int isize)
{
    Pointer prof = _profRead(proftype, inst);
    if(prof) {
        int val = _getInt(prof, offset, isize);
	CLIPRINTF(("%s: got 0x%0lx ", __FUNCTION__, val));
	releaseProfilePointer(proftype, prof);
	return val;
    }
    return 0;
}

int profValidatePasswd(int isize, const char *buf)
{
    volatile static char tmp[MAX_CMDLEN];
    int len = 0;

    if ( buf && *buf ) {
      memset((void*)tmp, 0, sizeof(tmp));
      if ( strncmp(buf, CLI_SECURE_STR1, strlen(CLI_SECURE_STR1)) == 0 ) {
        PRINTF(("%s: encode %s len %d\n", __FUNCTION__, buf+strlen(CLI_SECURE_STR1), strlen(buf)-strlen(CLI_SECURE_STR1)));
        len = deBase64( (Uint8*)tmp, (const char*)buf+strlen(CLI_SECURE_STR1));
        PRINTF(("%s: decode %s len %d\n", __FUNCTION__, tmp, len));
      } else {
        len = _strlen((char*)buf);
      }
    }

  return len;

}

int cliProfValidatePasswd(TermInfo *term, int cmdindex, int isize, const char *buf)
{
    int len = 0;

    argGet(term->lastInput, cmdindex, (char*)buf);
    PRINTF(("%s: strp %s len %d\n", __FUNCTION__, buf, strlen(buf)));

    len = profValidatePasswd (isize, buf);

    if( len >= isize)
    {
	if (!noPrompt) {
	  _printf(term, "String value is too long.\r\n");
        } else {
	  _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        } 
	term->result = FAIL_CLI;
	return FAIL_CLI;
    }
    return OK_CLI;
}

const char *
profGetPasswd( Pointer prof, int offset)
{
    volatile static char tmp[MAX_CMDLEN];
    char *strp = NULL;
    char *dstp = NULL;
    
    if (prof) {
	strp = (char*)prof + offset;
	if ( strlen ( strp ) ) { /* not null string */
    		memset((void*)tmp, 0, sizeof(tmp));
		dstp = (char*)tmp;
		sprintf((char*)dstp,CLI_SECURE_FMT1, CLI_SECURE_STR1);
		dstp += sizeof(CLI_SECURE_STR1)-1;
      		PRINTF(("%s: strp %s len %d\n", __FUNCTION__, strp, strlen(strp)));
		enBase64((const Uint8*)strp, strlen(strp), dstp);
      		PRINTF(("%s: encode %s len %d\n", __FUNCTION__, dstp, strlen(dstp)));
      		PRINTF(("%s: chiper %s len %d\n", __FUNCTION__, tmp, strlen((char*)tmp)));
		return (char*)tmp;
	} else {
	  return strp;
        } 
    } else {
      return (char*)nullstr;
    }
	
}
const char *
cliProfGetPasswd(MibProfileType proftype, int *inst, int offset)
{
    
   Pointer prof = _profRead(proftype, inst);
   char *rst = (char*)nullstr;
   if ( prof ) {
     rst =  (char*)profGetPasswd ( prof, offset );
     releaseProfilePointer(proftype, prof);
   }
	
   return rst;
}

void profSetPasswd(Pointer prof, int offset, int isize, const char *src)
{
  volatile static char tmp[MAX_CMDLEN];
  int len = 0;

  if ( src ) {
    PRINTF(("%s: srcp %s len %d\n", __FUNCTION__, src, strlen(src)));
    memset((void*)tmp, 0, sizeof(tmp));
    if ( strncmp(src, CLI_SECURE_STR1, strlen(CLI_SECURE_STR1)) == 0 ) {
      PRINTF(("%s: srcp %s len %d\n", __FUNCTION__, src, strlen(src)));
      PRINTF(("%s: encode %s len %d\n", __FUNCTION__, src+strlen(CLI_SECURE_STR1), strlen(src)-strlen(CLI_SECURE_STR1)));
      len = deBase64( (Uint8*)tmp, (const char*)src+strlen(CLI_SECURE_STR1));
      _strncpy((char *)prof + offset, (char*)tmp, min(len,isize) );
      PRINTF(("%s: decode %s len %d\n", __FUNCTION__, tmp, len));
    } else {
      _strncpy((char *)prof + offset, (char*)src, isize);
    }
  }
}

void sprintPasswd(char *buf, const char *pass)
{
   snprintf(buf, MAX_CMDLEN-2, "%s", pass);
}

int profValidateEnumInt(TermInfo *term, int cmdindex, LcdParamEnumIntValue *enum1)
{
    char tmp[MAX_CMDLEN];
    int value;

    argGet(term->lastInput, cmdindex, tmp);

    if ( enum1 ) {
      if ( enum1->enumlist ) {
	if ( (value = enumValue(enum1->enumlist, tmp)) != FAIL_CLI) {
    	  return value;
 	}
      }
      if ( !isDigit(tmp) ) {
        if (!noPrompt) {
          _printf(term, "Invalid Enum Int.\r\n");
        } else {
          _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
        }
        term->result = FAIL_CLI;
        return FAIL_CLI;
      } else {
        value = atoi(tmp);
        if ( ( (unsigned int)value >= enum1->minValue)
	     && ( (unsigned int)value <= enum1->maxValue ) 
	  ) {
	    return value;
        } else {
          if (!noPrompt) {
            _printf(term, "Invalid Enum Int.\r\n");
	    if ( enum1->enumlist ) { 
		enumList(enum1->enumlist, term, "");
          	_printf(term, "please select one from list, or ");
	    }
            _printf(term, "valid range is [%lu - %lu]\r\n", enum1->minValue, enum1->maxValue);
          } else {
	    _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
          } 
        }
      }
    }
    term->result = FAIL_CLI;
    return FAIL_CLI;

}

int profGetEnumInt(MibProfileType proftype, int *inst, int offset, int isize)
{
  return profGetInt(proftype, inst, offset, isize);
}

void profSetEnumInt(Pointer prof, int offset, int isize, int value)
{
  _setInt(prof, offset, isize, value);
}

void sprintEnumInt(char *buf, LcdParamEnumIntValue *enum1, int value)
{
  char *strp = NULL;
 
  strp = (char*)enumString(enum1->enumlist, value);

  if ( strp ) 
	snprintf(buf, MAX_CMDLEN-2, "%s", strp);
  else
	snprintf(buf, MAX_CMDLEN-2, "%d", value);

}

void profileInitParam(MibProfileType profType, int *namep, int *names, int *activep, int *actives)
{
//
// zhz@20041026, some caller do not init the params, but
// expect we return the value, so init the result first
//

    if ( namep ) {
	*namep = -1;
    }

    if ( names ) {
	*names = 0;
    }

    if ( activep ) {
	*activep = -1;
    }

    if ( actives ) {
	*actives = 0;
    }

    switch(profType)
    {
#if 0
	/*add by chenyunhua*/
        case MIB_PROF_TIMERANGE:
            *namep=(int)&tmrgProf->name;
            *names=sizeof(tmrgProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&tmrgProf->active;
	    	*actives = sizeof(tmrgProf->active);
	    }
# else
            *activep=-1;
# endif
            break;
#endif
#if (DHCP_PLUS == FYES)
	case MIB_PROF_DHCP_RAWOPTION:
	    *namep = (int)&rawoptClassProf->name;
	    *names=sizeof(rawoptClassProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&rawoptClassProf->active;
	    	*actives = sizeof(rawoptClassProf->active);
	    }
# else
	    *activep=-1;
# endif
	    break;
	case MIB_PROF_DHCP_POOL:
	    *namep = (int)&dhcpPoolClassProf->name;
            *names=sizeof(dhcpPoolClassProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&dhcpPoolClassProf->active;
	    	*actives = sizeof(dhcpPoolClassProf->active);
	    }
# else
            *activep=-1;
# endif
            break; 
# endif
#if 0
	case MIB_PROF_ROUTE:
	    *namep = (int)&routeProf->hostName;
	    *names = sizeof(routeProf->hostName);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    { 
	    	*activep = (int)&routeProf->active;
	    	*actives = sizeof(routeProf->active);
	    }
# else
            *activep=-1;
# endif
	    break;
	case MIB_PROF_INTERNET:
	    *namep = (int)&inetProf->hostName;
	    *names = sizeof(inetProf->hostName);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&inetProf->active;
	    	*actives = sizeof(inetProf->active);
	    }
# else
            *activep=-1;
# endif
	    break;
	case MIB_PROF_SECURITY:
	    *namep = (int)&secProf->hostName;
	    *names = sizeof(secProf->hostName);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&secProf->active;
	    	*actives = sizeof(secProf->active);
	    }
# else
	    *activep = -1;
# endif
	    break;
//add by jxy8@2003-12-26 for WebUserRight
#if ( WEB_SERVER == FYES )
	case MIB_PROF_SECURITY_USER:
	    *namep = (int)&secUserProf->hostName;
	    *names = sizeof(secUserProf->hostName);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&secUserProf->active;
	    	*actives = sizeof(secUserProf->active);
	    }
# else
            *activep=-1;
# endif
	    break;
	case MIB_PROF_SECURITY_GROUP:
	    *namep = (int)&secGroupProf->name;
	    *names = sizeof(secGroupProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&secGroupProf->active;
	    	*actives = sizeof(secGroupProf->active);
	    }
# else
            *activep=-1;
# endif
	    break;    
	case MIB_PROF_SECURITY_URL:
	    *namep = (int)&secUrlProf->name;
	    *names = sizeof(secUrlProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&secUrlProf->active;
	    	*actives = sizeof(secUrlProf->active);
	    }
# else
            *activep=-1;
# endif
	    break;      
//added by dhyu@090611
	case MIB_PROF_JOIN_STARTMENU:
	    *namep = (int)&joinbuttonProf->name;
	    *names = sizeof(joinbuttonProf->name);
	    *activep = -1;
	    break;

#endif
//add end jxy8
	case MIB_PROF_NAME_PASS:
	    *namep = (int)&nameProf->hostName;
	    *names = sizeof(nameProf->hostName);
# if ( ACTIVATE_PROF == FYES )
	   if(TRUE == profUpdate)
	   {
	    	*activep = (int)&nameProf->active;
	    	*actives = sizeof(nameProf->active);
	   }
# else
            *activep=-1;
# endif
	    break;
	case MIB_PROF_NAT_STATIC:
	    *namep = (int)&natStatic->name;
	    *names = sizeof(natStatic->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&natStatic->active;
	    	*actives = sizeof(natStatic->active);
	    }
# else
            *activep=-1;
# endif
	    break;
	case MIB_PROF_FILTER_IN:
	case MIB_PROF_FILTER_OUT:
	    *namep = (int)&filterEntry->name;
	    *names = sizeof(filterEntry->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&filterEntry->active;
	    	*actives = sizeof(filterEntry->active);
	    }
# else
            *activep=-1;
# endif
	    break;
#if ( CBQ == FYES )
	case MIB_PROF_CBQ_CLASS:
	    *namep = (int)&cbqClassProf->name;
	    *names = sizeof(cbqClassProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&cbqClassProf->active;
	    	*actives = sizeof(cbqClassProf->active);
	    }
# else
	    *activep = -1;
# endif
	    break;
	case MIB_PROF_CBQ_FILTER:
	    *namep = (int)&cbqFilterProf->name;
	    *names = sizeof(cbqFilterProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&cbqFilterProf->active;
	    	*actives = sizeof(cbqFilterProf->active);
	    }
# else
	    *activep = -1;
# endif
	    break;
#endif
	case MIB_PROF_SFQ_QDISC:
	    *namep = (int)&sfqQdiscProf->name;
	    *names = sizeof(sfqQdiscProf->name);
# if ( ACTIVATE_PROF == FYES )
	   if(TRUE == profUpdate)
	   {
	   	 *activep = (int)&sfqQdiscProf->active;
	   	 *actives = sizeof(sfqQdiscProf->active);
	   }
# else
	    *activep = -1;
# endif
	    break;
#endif

# if (OLDNAT)
	case MIB_PROF_NAT_POOL:
	    *namep = (int)&natPool->name;
	    *names = sizeof(natPool->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&natPool->active;
	    	*actives = sizeof(natPool->active);
	    }
# else
	    *activep = -1;
# endif
	    break;
# else
#if 0
	case MIB_PROF_NAT_BINDING:
	    *namep = (int)&natBinding->name;
	    *names = sizeof(natBinding->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&natBinding->active;
	    	*actives = sizeof(natBinding->active);
	    }
# else
	    *activep = -1;
# endif
	    break;
#endif
# endif
/* zhz@20020816, 
   if profile have instance, add your new PROF_TYPE below, plese 
*/

#if (MDF_IKE == FYES && MDF_IP_SEC == FYES)
	case MIB_PROF_IPSEC_CONF:		/* add by lyu */
	    *namep = (int)&ipsecConfProf->name;
	    *names = sizeof(ipsecConfProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&ipsecConfProf->active;
	    	*actives = sizeof(ipsecConfProf->active);
	    }
# else
	    *activep = -1;
# endif
	    break;
	case MIB_PROF_IPSEC_TRANSFORMSET:               
            *namep = (int)&ipsecTransformSetProf->name;
            *names = sizeof(ipsecTransformSetProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&ipsecTransformSetProf->active;
	    	*actives = sizeof(ipsecTransformSetProf->active);
	    }
# else
            *activep = -1;
# endif
            break;
	case MIB_PROF_ISAKMP_CONF:
	    *namep = (int)&isakmpConfProf->name;
            *names = sizeof(isakmpConfProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&isakmpConfProf->active;
	    	*actives = sizeof(isakmpConfProf->active);
	    }
# else
            *activep = -1;
# endif
	    break;
	case MIB_PROF_ISAKMP_POLICY:
	    *namep = (int)&isakmpPolicyProf->name;
            *names = sizeof(isakmpPolicyProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&isakmpPolicyProf->active;
	    	*actives = sizeof(isakmpPolicyProf->active);
	    }
# else
            *activep = -1;
# endif
	    break;
#endif

# if ( FIREWALL == FYES )

	case MIB_PROF_FWADDRESS:   /* add by cyh */
	    *namep = (int)&fwAddressProf->name;
	    *names = sizeof(fwAddressProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&fwAddressProf->active;
                *actives = sizeof(fwAddressProf->active);
            }
# else
            *activep = -1;
# endif
            break;

        case MIB_PROF_FWSERVICE:  
            *namep = (int)&fwServiceProf->name;
            *names = sizeof(fwServiceProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&fwServiceProf->active;
                *actives = sizeof(fwServiceProf->active);
            }
# else
            *activep = -1;
# endif
            break;

        case MIB_PROF_FWL7FILTER:
            *namep = (int)&fwL7filterProf->name;
            *names = sizeof(fwL7filterProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&fwL7filterProf->active;
                *actives = sizeof(fwL7filterProf->active);
            }
# else
            *activep = -1;
# endif
            break;

# endif  /* FIREWALL */

# if ( PDB == FYES )
        case MIB_PROF_PDB:
            *namep = (int)&pdbProf->name;
            *names = sizeof(pdbProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&pdbProf->active;
                *actives = sizeof(pdbProf->active);
            }
# else
            *activep = -1;
# endif
            break;
# endif

#if (SWITCHFUN == FYES)  /* add by zgd */
        case MIB_PROF_SWITCH:
            *namep = (int)&switchProf->name;
            *names = sizeof(switchProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&switchProf->active;
                *actives = sizeof(switchProf->active);
            }
# else
            *activep = -1;
# endif
            break;
            
        case MIB_PROF_PORT2:
            *namep = (int)&port2Prof->name;
            *names = sizeof(port2Prof->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&port2Prof->active;
                *actives = sizeof(port2Prof->active);
            }
# else
            *activep = -1;
# endif
            break;
            
        case MIB_PROF_PORTMAC:
            *namep = (int)&portmacProf->name;
            *names = sizeof(portmacProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&portmacProf->active;
                *actives = sizeof(portmacProf->active);
            }
# else
            *activep = -1;
# endif
            break;
            
#if (TAGVlAN == FYES)
        case MIB_PROF_TAGVLAN:
            *namep = (int)&tagvlanProf->name;
            *names = sizeof(tagvlanProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&tagvlanProf->active;
                *actives = sizeof(tagvlanProf->active);
            }
# else
            *activep = -1;
#endif
            break;
#endif

#endif  /* SWITCHFUN */

#if(DOT1X == FYES)
	case MIB_PROF_DOT1X:
		*namep = (int)&dot1xProf->name;
		*names = sizeof(dot1xProf->name);
#if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&dot1xProf->active;
                *actives = sizeof(dot1xProf->active);
            }
#else
            *activep = -1;
#endif
		break;
	case MIB_PROF_DOT1X_USER:
		*namep = (int)&dot1xUserProf->name;
		*names = sizeof(dot1xUserProf->name);
#if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&dot1xUserProf->active;
                *actives = sizeof(dot1xUserProf->active);
            }
#else
            *activep = -1;
#endif
		break;
#endif /* DOT!X */


#if ( ISP_CFG == FYES )
        case MIB_PROF_ISP:
            *namep = (int)&ispProf->name;
            *names = sizeof(ispProf->name);
# if ( ACTIVATE_PROF == FYES )
            if(TRUE == profUpdate)
            {
                *activep = (int)&ispProf->active;
                *actives = sizeof(ispProf->active);
            }
# else
            *activep = -1;
# endif
            break;
#endif


#if (MDF_UPNP == FYES)
	case MIB_PROF_UPNP:
	    *namep = (int)&UpnpConfProf->name;
            *names = sizeof(UpnpConfProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&UpnpConfProf->active;
	    	*actives = sizeof(UpnpConfProf->active);
	    }
# else
    	    *activep = -1;
# endif
	    break;
#endif

#if (FIREWALLS == FYES)
	case MIB_PROF_FWALL:
	    *namep = (int)&fwallProf->name;
            *names = sizeof(fwallProf->name);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&fwallProf->active;
	    	*actives = sizeof(fwallProf->active);
	    }
# else
    	    *activep = -1;
# endif
	    break;
#endif

# if ( SNMP_TRAP == FYES )
	case MIB_PROF_TRAP:
	    *namep = (int)&trapProf->hostName;
	    *names = sizeof(trapProf->hostName);
# if ( ACTIVATE_PROF == FYES )
	    if(TRUE == profUpdate)
	    {
	    	*activep = (int)&trapProf->active;
	    	*actives = sizeof(trapProf->active);
	    }
# else
	    *activep = -1;
# endif
	    break;

# endif /* SNMP_TRAP == FYES */

#if (PPPOE_SRV == FYES) 	/* added by llliu */
	case MIB_PROF_PPPOE_SRV:
	    *namep = (int)&_pppoeSrvProf->hostName;
	    *names = sizeof(_pppoeSrvProf->hostName);
#if (ACTIVATE_PROF == FYES)
	    if (TRUE == profUpdate) {
		*activep = (int)&_pppoeSrvProf->active;
		*actives = sizeof(_pppoeSrvProf->active);
	    }
#else 
	    *activep = -1;
#endif
	    break;
	case MIB_PROF_MAC_FILTER:
	    *namep = (int)&_macFilterProf->hostName;
	    *names = sizeof(_macFilterProf->hostName);
#if (ACTIVATE_PROF == FYES)
	    if (TRUE == profUpdate) {
		*activep = (int)&_macFilterProf->active;
		*actives = sizeof(_macFilterProf->active);
	    }
#else 
	    *activep = -1;
#endif
	    break;

		
#endif	/* PPPOE_SRV */

#if 0
#if (OBJECT_GROUP == FYES)	/* add by brwang */
	case MIB_PROF_IP_GROUP:
          *namep = (int)&ipGroupProf->name;
          *names = sizeof(ipGroupProf->name);
# if ( ACTIVATE_PROF == FYES )
           if(TRUE == profUpdate) {
              *activep = (int)&ipGroupProf->active;
              *actives = sizeof(ipGroupProf->active);
	   }
# else
          *activep = -1;
# endif
          break;
   case MIB_PROF_PORT_GROUP:
          *namep = (int)&portGroupProf->name;
          *names = sizeof(portGroupProf->name);
# if ( ACTIVATE_PROF == FYES )
          if(TRUE == profUpdate) {
            *activep = (int)&portGroupProf->active;
            *actives = sizeof(portGroupProf->active);
          }
# else
          *activep = -1;
# endif
          break;

#endif		/* (OBJECT_GROUP == FYES) */
#endif

          /*
           * add by brwang
           */
   case MIB_PROF_ETHER_FILTER:
          *namep = (int)&etherFilterProf->name;
          *names = sizeof(etherFilterProf->name);
# if ( ACTIVATE_PROF == FYES )
          if(TRUE == profUpdate) {
            *activep = (int)&etherFilterProf->active;
            *actives = sizeof(etherFilterProf->active);
          }
# else
          *activep = -1;
# endif
          break;
          
          /*
           * added by llliu@080428.
           */
   case MIB_PROF_DNS_FILTER:
          *namep = (int)&dnsFilterProf->hostName;
          *names = sizeof(dnsFilterProf->hostName);
#if (ACTIVATE_PROF == FYES)
          if (TRUE == profUpdate) {

              *activep = (int)&dnsFilterProf->active;
              *actives = sizeof(dnsFilterProf->active);
          }
#else
          *activep = -1;
#endif
          break;


	  /*
	   *  added by llliu@081107.
	   */
#if (POLICY_ROUTE == FYES)
   case MIB_PROF_POLICY_ROUTE:
	  *namep = (int)&policyRouteProf->hostName;
	  *names = sizeof(policyRouteProf->hostName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate) {
	      *activep = (int)&policyRouteProf->active;
	      *actives = sizeof(policyRouteProf->active);
	  }
#else
	  *activep = -1;
#endif
	  break;
#endif

#if 0
	  /*
	   *  added by llliu@081201.
	   */
   case MIB_PROF_PUBLIC_DDNS:
	  *namep = (int)&publicDdnsProf->hostName;
	  *names = sizeof(publicDdnsProf->hostName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate) {
	      *activep = (int)&publicDdnsProf->active;
	      *actives = sizeof(publicDdnsProf->active);
	  }
#else
	  *activep = -1;
#endif
	  break;
#endif

#if (FC == FYES)
   case MIB_PROF_RATE_LIMIT:
	  *namep = (int)&rateLimitProf->indexName;
	  *names = sizeof(rateLimitProf->indexName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate) {
	      *activep = (int)&rateLimitProf->active;
	      *actives = sizeof(rateLimitProf->active);
	  }
#else
	  *activep = -1;

#endif
	  break;
#endif

    case MIB_PROF_NET_ACTION_MANAGE:
	  *namep = (int)&netActionManageProf->indexName;
	  *names = sizeof(netActionManageProf->indexName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate) {
	      *activep = (int)&netActionManageProf->enable;
	      *actives = sizeof(netActionManageProf->enable);
	  }
#else
	  *activep = -1;
#endif
	  break;

   
#if 0
    case MIB_PROF_QQ_EXCEPTION:
	  *namep = (int)&qqExceptionProf->indexName;
	  *names = sizeof(qqExceptionProf->indexName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate)
	  {
	      *activep = (int)&qqExceptionProf->enable;
	      *actives = sizeof(qqExceptionProf->enable);
	  }
#else
	  *activep = -1;
#endif
	  break;


    case MIB_PROF_CONNECTION_LIMIT:
	  *namep = (int)&connectionLimitProf->indexName;
	  *names = sizeof(connectionLimitProf->indexName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate)
	  {
	      *activep = (int)&connectionLimitProf->enable;
	      *actives = sizeof(connectionLimitProf->enable);
	  }
#else

	  *activep = -1;

#endif
	  break;

/*added by jfgu@090722*/
#if (PPPOE_SRV == FYES)
    case MIB_PROF_PPPOE_ONLY:
	  *namep = (int)&pppoeOnlyProf->indexName;
	  *names = sizeof(pppoeOnlyProf->indexName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE ==  profUpdate)
	  {
	    *activep = (int)&pppoeOnlyProf->enabled;
	    *actives = sizeof(pppoeOnlyProf->enabled);
	  }
#endif
	  *activep = -1;
	
#endif
	  break;



    case MIB_PROF_DNS_RESOLVE:
	  *namep = (int)&dnsResolveProf->indexName;
	  *names = sizeof(dnsResolveProf->indexName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate) {
	      *activep = (int)&dnsResolveProf->enable;
	      *actives = sizeof(dnsResolveProf->enable);
	  }
#else
	  *activep = -1;

#endif

	  break;

//added by dhyu  
#if (NOTICE == FYES)
   case MIB_PROF_NOTICE:
	  *namep = (int)&noticeProf->instName;
	  *names = sizeof(noticeProf->instName);
#if (ACTIVATE_PROF == FYES)
	  if (TRUE == profUpdate) {
	      *activep = (int)&noticeProf->enable;
	      *actives = sizeof(noticeProf->enable);
	  }
#else
	  *activep = -1;

#endif
	  break;
#endif
#endif


	default:
	    break;
    }
//    *activep = -1;
//    *actives = 0;
}

int profileByNamei(MibProfileType profType, char *name, Pointer *_prof)
{
    int i, len, found, ii = 0, namep = 0, names, activep, actives;
    Pointer prof, prof1 = 0;

    len = _strlen(name);
    if(!len) return 0;
    found = 0;
    profileInitParam(profType, &namep, &names, &activep, &actives);
    if(namep == -1)
    {
	i = atoi(name) - 1;
	if(i >= instanceMin(profType) && i < instanceMax(profType))
	{
	    prof = profileByNumber(profType, i);
 	    if(prof && activep != -1 && _getInt(prof, activep, actives))
	    {
		*_prof = prof;
		PRINTF(("%s:%d req profType %d profname %s return index %d\n", 
			__FUNCTION__, __LINE__, profType, name, i));
	    	return i;
	    }
	    releaseProfilePointer(profType, prof);
	}
	PRINTF(("%s:%d req profType %d profname %s return index %d\n", 
			__FUNCTION__, __LINE__, profType, name, -1));
	return -1;
    }
    for(i = instanceMin(profType);i < instanceMax(profType);i++)
    {
        prof = profileByNumber(profType, i);
# if ( ACTIVATE_PROF == FYES )
        if(prof && !_strncmpi((char *)prof + namep, name, len))	
# else
 	if(activep != -1 && _getInt(prof, activep, actives) > 0 ||
	    activep == -1 && _strlen((char *)prof + namep) > 0)
	if(_strlen((char *)prof + namep) > 0)
# endif
	{
	    prof1 = prof;
	    found++;
	    ii = i;
	    if(_strlen((char *)prof + namep) == len)
	    {
		found = 1;
		break;
	    }
  	}
	releaseProfilePointer(profType, prof);
    }
    if(found == 1)
    {
	*_prof = prof1;
	PRINTF(("%s:%d req profType %d profname %s return index %d\n", 
			__FUNCTION__, __LINE__, profType, name, ii));
	return ii;
    }
    PRINTF(("%s:%d req profType %d profname %s return index %d\n", 
		__FUNCTION__, __LINE__, profType, name, -1));
    return -1;
}

Pointer profileByNumber(MibProfileType profType, int index)
{
    SlotNumber slot;

    slot.slotNumber = SLOT_NUMBER_ANY;
    return _profRead2(profType, index, slot);
}

Boolean profileUpdate(MibProfileType profType, int index, Pointer prof)
{
    SlotNumber slot;

    slot.slotNumber = SLOT_NUMBER_ANY;
    return _profWrite2(profType, index, slot, prof);
}

int profileBuildFactory(MibProfileType profType, Pointer *_prof)
{
    int size = 0;
    static ProfileUnion unionProf;

    switch(profType)
    {
#if 0
#if (OBJECT_GROUP == FYES)
	case MIB_PROF_IP_GROUP:
            *_prof = (Pointer*)buildFactoryIpGroupProfile(&unionProf);
            size = sizeof(IpGroupProfile);
	    break;
#endif
#endif
        default:
	    break;
    }
    return size;
} 

# if ( NO_PROF_EDIT_CACHE == FYES )

Boolean _profWrite2(MibProfileType proftype, int index, SlotNumber slot, Pointer prof)
{
    Boolean ret;
    ProfileUnion *profBuf = NULL;

    /* allocate edit buf */    
    profBuf = (ProfileUnion*)MALLOC(sizeof(ProfileUnion));
    if ( !profBuf ) {
     return FALSE;
    }

    if(proftype == MIB_PROF_NAT_STATIC)
    {
        if ( !readNVRAMProfile(slot, MIB_PROF_NAT, 0, (Pointer)profBuf) ) {
          FREE(profBuf);
	  profBuf = NULL;
	  return FALSE;
        }
	memcpy((Pointer)((int)profBuf + (int)natProf->staticMaps + index*sizeof(StaticMapEntry)), prof, sizeof(StaticMapEntry));
    	ret = writeNVRAMProfile(slot, MIB_PROF_NAT, 0, profBuf);
    }
# if (OLDNAT)
    else if(proftype == MIB_PROF_NAT_POOL) {
# else
    else if(proftype == MIB_PROF_NAT_BINDING) {
# endif
	if ( !readNVRAMProfile(slot, MIB_PROF_NAT, 0, profBuf) ) {
          FREE(profBuf);
          profBuf = NULL;
          return FALSE;
        }
# if (OLDNAT)
	memcpy((Pointer)((int)profBuf + (int)natProf->natPool + index*sizeof(natAddrPoolMap)), prof, sizeof(natAddrPoolMap));
# else
	memcpy((Pointer)((int)profBuf + (int)natProf->natBinding + index*sizeof(NatBinding)), prof, sizeof(NatBinding));
# endif
    	ret = writeNVRAMProfile(slot, MIB_PROF_NAT, 0, profBuf);
    }
    else if(proftype == MIB_PROF_FILTER_IN)
    {
	if ( ! readNVRAMProfile(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, profBuf) ) {
          FREE(profBuf);
          profBuf = NULL;
          return FALSE;
        }
	memcpy((Pointer)((int)profBuf + (int)filterProf->inputFilters + index*sizeof(FilterEntry)), prof, sizeof(FilterEntry));
    	ret = writeNVRAMProfile(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, profBuf);
    }
    else if(proftype == MIB_PROF_FILTER_OUT)
    {
	if ( !readNVRAMProfile(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, profBuf) ) {
          FREE(profBuf);
          profBuf = NULL;
          return FALSE;
        }
	memcpy((Pointer)((int)profBuf + (int)filterProf->outputFilters + index*sizeof(FilterEntry)), prof, sizeof(FilterEntry));
    	ret = writeNVRAMProfile(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, profBuf);
    }
    else {
	if ( !readNVRAMProfile(slot, proftype, index, profBuf) ) {
          FREE(profBuf);
          profBuf = NULL;
          return FALSE;
        }
    	ret = writeNVRAMProfile(slot, proftype, index, prof);
    }

    if ( profBuf ) {
      FREE( profBuf);
    }

    return ret;
}

int profSetValue(MibProfileType proftype, int *inst, int offset, int isize, IType itype, Pointer enum1, TermInfo *term, int cmdindex)
{
    char *tmp = NULL;
    int value = FAIL_CLI;
    int ip, size;
#if 0
    AbsoluteTime dt;
#endif

    Pointer prof = NULL;
    Pointer prof2 = NULL;

    ProfileUnion *profBuf = NULL;

    /* allocate edit buf */    
    profBuf = (ProfileUnion*)MALLOC(sizeof(ProfileUnion));
    if ( !profBuf ) {
      return FAIL2_CLI;
    }

    tmp = (char *)MALLOC(size = isize < MAX_CMDLEN ? MAX_CMDLEN : isize);
    ASSERT(tmp);
    memset(tmp, 0, size);

    prof2 = _profRead(proftype, inst);
    if(!prof2)
	return FAIL2_CLI;
    memcpy( profBuf, prof2, sizeof(ProfileUnion));
    prof = profBuf;
    
    
    switch(itype)
    {
    case INT:
    	if((value = validateInt(term, cmdindex, 0)) != FAIL_CLI) 
	    _setInt(prof, offset, isize, value);
	break;

    case GROUP:
    	if((value = validateInt(term, cmdindex, 0)) != FAIL_CLI) 
	    _setInt(prof, offset, isize, value | 0x8000);
	break;

    case STR:
	if((value = validateStr(term, cmdindex, isize, tmp)) != FAIL_CLI)
	{
    	    _strncpy((char *)prof + offset, tmp, isize - 1);
	    *((char *)prof + offset + isize - 1) = 0;
	}
	break;

    case ENUM:
    	if((value = validateEnum(term, cmdindex, (LcdParamEnumValue *)enum1)) != FAIL_CLI)
    	{
	  #if (SWITCHFUN == FYES)  /* add by zgd */
	    Uint8 Flg = 0;
	    Uint8 MonNum=0xFF;
	    const Port2Profile *port2Prof;
	    Port2Profile port2Prof2;
	    
	    extern const Port2Profile *
            requestPort2ProfilePointer( Word index);
            extern void
            releasePort2ProfilePointer(const Port2Profile* profileData );
            
            if (value == NPW_MIRR_MON)
            {
              for (int i=0; i<MAX_PORT2_PROFILES; i++)
              {
                port2Prof = requestPort2ProfilePointer((Word)i);
                port2Prof2 = *port2Prof;
              
               if (port2Prof2.MirrAtt == NPW_MIRR_MON)
               {
                 MonNum = i;
                 MonNum += 49;
                 if (MonNum != *(Byte*)prof)
                 {
                   Flg = 0x01;
                 }
                 break;
               }
              }
            }
            /*this judge here is very frailness,need be careful*/
	    if ((proftype==MIB_PROF_PORT2) && (offset == 16) && (Flg == 0x01))
	    {
	      _printf(term, "Err:only one port can be set as a mirror port\r\n");
	    }
	    else
	    {
	      _setInt(prof, offset, isize, value);
	    }
	  #else
	    _setInt(prof, offset, isize, value);
	  #endif
	}
	break;

    case IPADDR:
    	if((value = validateIp(term, cmdindex, &ip)) != FAIL_CLI)
	    _setInt(prof, offset, isize, ip);
	break; 

    case MACADDR:
    	if((value = validateMac(term, cmdindex, tmp)) != FAIL_CLI)
    	    _strncpy((char *)prof + offset, tmp, isize - 1);
	    *((char *)prof + offset + isize - 1) = 0;
	break; 

    case MACADDR2:
	Byte *pmac;
	if((value = validateMac(term, cmdindex, tmp)) != FAIL_CLI){
		pmac = (Byte *)prof + offset;
    		hexStringToValue(pmac, tmp, ETHER_ADDR_LEN);
	}			
	break;

    case HEX:
	Byte *phex;
	if((value = validateHex(term, cmdindex, isize, tmp)) != FAIL_CLI){
		phex = (Byte *)prof + offset;
    		hexStringToValue(phex, tmp, min(isize, MAX_HEX_LEN));
	}			
	break;

    case STRNUM:
	if((value = validateNumber(term, cmdindex, isize, tmp)) != FAIL_CLI)
	{
    	    _strncpy((char *)prof + offset, tmp, isize - 1);
	    *((char *)prof + offset + isize - 1) = 0;
	}
	break;

#if 0
    case DATETIME:
    	if((value = validateDateTime(term, cmdindex, &dt)) != FAIL_CLI)
	    _setInt(prof, offset, isize, dt);
	break;

    case TIME:
    	if((value = validateTime(term, cmdindex, &dt)) != FAIL_CLI)
	    _setInt(prof, offset, isize, dt);
	break;
#endif

    case PASSWD:
	if((value = cliProfValidatePasswd(term, cmdindex, isize, tmp)) != FAIL_CLI)
	{
    	    profSetPasswd(prof, offset, isize - 1, tmp);
	    *((char *)prof + offset + isize - 1) = 0;
	}
	break;

    case ENUMINT:
	if((value = profValidateEnumInt(term, cmdindex, (LcdParamEnumIntValue *)enum1)) != FAIL_CLI)
	{
    	    profSetEnumInt(prof, offset, isize, value);
	}
	break;

    default:
	break;
    }	
    FREE(tmp);
    if(value != FAIL_CLI)
	if(!_profWrite(proftype, inst, prof))
	    value = FAIL2_CLI;
	else
	    value = OK_CLI;
    releaseProfilePointer(proftype, prof2);
    if ( profBuf ) {
	FREE(profBuf);
    }
    return value;
} 

# else

Boolean _profWrite2(MibProfileType proftype, int index, SlotNumber slot, Pointer prof)
{
    Pointer prof1;
    Boolean ret;

#if 0
    if(proftype == MIB_PROF_NAT_STATIC)
    {
	returnProfilePointer(slot, MIB_PROF_NAT, 0, &prof1);
	memcpy((Pointer)((int)prof1 + (int)natProf->staticMaps + index*sizeof(StaticMapEntry)), prof, sizeof(StaticMapEntry));
    	ret = writeNVRAMProfile(slot, MIB_PROF_NAT, 0, prof1);
	releaseProfilePointer(MIB_PROF_NAT, prof1);
    }
# if (OLDNAT)
    else if(proftype == MIB_PROF_NAT_POOL) {
# else
    else if(proftype == MIB_PROF_NAT_BINDING) {
# endif
	returnProfilePointer(slot, MIB_PROF_NAT, 0, &prof1);
	memcpy((Pointer)((int)prof1 + (int)natProf->natBinding + index*sizeof(NatBinding)), prof, sizeof(NatBinding));
    	ret = writeNVRAMProfile(slot, MIB_PROF_NAT, 0, prof1);
	releaseProfilePointer(MIB_PROF_NAT, prof1);
    }
    else if(proftype == MIB_PROF_FILTER_IN)
    {
	returnProfilePointer(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, &prof1);
	memcpy((Pointer)((int)prof1 + (int)filterProf->inputFilters + index*sizeof(FilterEntry)), prof, sizeof(FilterEntry));
    	ret = writeNVRAMProfile(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, prof1);
	releaseProfilePointer(MIB_PROF_FILTER, prof1);
    }
    else if(proftype == MIB_PROF_FILTER_OUT)
    {
	returnProfilePointer(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, &prof1);
	memcpy((Pointer)((int)prof1 + (int)filterProf->outputFilters + index*sizeof(FilterEntry)), prof, sizeof(FilterEntry));
    	ret = writeNVRAMProfile(slot, MIB_PROF_FILTER, NEW_FILTER_DATA, prof1);
	releaseProfilePointer(MIB_PROF_FILTER, prof1);
    }
    else
#endif
    	ret = writeNVRAMProfile(slot, proftype, index, prof);
    return ret;
}

int profSetValue(MibProfileType proftype, int *inst, int offset, int isize, IType itype, Pointer enum1, TermInfo *term, int cmdindex)
{
    Byte *phex;
    char *pmac;
    char *tmp = NULL;
    int value = FAIL_CLI;
    int ip, size;
#if 0
    AbsoluteTime dt;
#endif

    Pointer prof = NULL;

    tmp = (char *)MALLOC(size = isize < MAX_CMDLEN ? MAX_CMDLEN : isize);
    ASSERT(tmp);

    memset(tmp, 0, size);
    prof = _profRead(proftype, inst);

    if(!prof)
	return FAIL2_CLI;
    switch(itype)
    {

    case INT:
    	if((value = validateInt(term, cmdindex, 0)) != FAIL_CLI) 
	    _setInt(prof, offset, isize, value);
	break;

    case GROUP:
    	if((value = validateInt(term, cmdindex, 0)) != FAIL_CLI) 
	    _setInt(prof, offset, isize, value | 0x8000);
	break;

    case STR:
	if((value = validateStr(term, cmdindex, isize, tmp)) != FAIL_CLI)
	{
    	    _strncpy((char *)prof + offset, tmp, isize - 1);
	    *((char *)prof + offset + isize - 1) = 0;
	}
	break;

    case ENUM:
    	if((value = validateEnum(term, cmdindex, (LcdParamEnumValue *)enum1)) != FAIL_CLI)
	    _setInt(prof, offset, isize, value);
	break;

    case IPADDR:
    	if((value = validateIp(term, cmdindex, &ip)) != FAIL_CLI)
	    _setInt(prof, offset, isize, ip);
	break; 

    case MACADDR:
    	if((value = validateMac(term, cmdindex, tmp)) != FAIL_CLI)
    	    _strncpy((char *)prof + offset, tmp, isize - 1);
	    *((char *)prof + offset + isize - 1) = 0;
	break; 

    case MACADDR2:
	if((value = validateMac(term, cmdindex, tmp)) != FAIL_CLI){
		pmac = (Byte *)prof + offset;
    		hexStringToValue(pmac, tmp, ETHER_ADDR_LEN);
	}			
	break;

    case HEX:
	if((value = validateHex(term, cmdindex, isize, tmp)) != FAIL_CLI){
		phex = (Byte *)prof + offset;
    		hexStringToValue(phex, tmp, min(isize, MAX_HEX_LEN));
	}			
	break;

    case STRNUM:
	if((value = validateNumber(term, cmdindex, isize, tmp)) != FAIL_CLI)
	{
    	    _strncpy((char *)prof + offset, tmp, isize - 1);
	    *((char *)prof + offset + isize - 1) = 0;
	}
	break;

#if 0
    case DATETIME:
    	if((value = validateDateTime(term, cmdindex, &dt)) != FAIL_CLI)
	    _setInt(prof, offset, isize, dt);
	break;

    case TIME:
    	if((value = validateTime(term, cmdindex, &dt)) != FAIL_CLI)
	    _setInt(prof, offset, isize, dt);
	break;
#endif

    case PASSWD:
	if((value = cliProfValidatePasswd(term, cmdindex, isize, tmp)) != FAIL_CLI)
	{
    	    profSetPasswd(prof, offset, isize - 1, tmp);
	    *((char *)prof + offset + isize - 1) = 0;
	}
	break;

    case ENUMINT:
	if((value = profValidateEnumInt(term, cmdindex, (LcdParamEnumIntValue *)enum1)) != FAIL_CLI)
	{
    	    profSetEnumInt(prof, offset, isize, value);
	}
	break;

    default:
	break;
    }	
    FREE(tmp);
    if(value != FAIL_CLI)
	if(!_profWrite(proftype, inst, prof))
	    value = FAIL2_CLI;
	else
	    value = OK_CLI;
    releaseProfilePointer(proftype, prof);
    return value;
} 

# endif /* NO_PROF_EDIT_CACHE  == FYES */

//add by jxy12@2004-6-7 for WebUI
void wuProfInitParam(MibProfileType proftype, int *inst, int *index, SlotNumber *slot)
{
    _profInitParam(proftype, inst, index, slot);
}

//add end jxy12

