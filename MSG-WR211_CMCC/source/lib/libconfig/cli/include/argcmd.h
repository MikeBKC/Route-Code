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
 * SCCS ID:	$Id: argcmd.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/


#ifndef ARGCMD_H

#define ARGCMD_H
#include "lcdmisc.h"
#include "tscore.h"
#include "mibtype.h"
#include "mibglob.h"
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
     *
     *  enumlist:	pointer to a list of valid EnumValue.
     */

typedef struct {
    UINT32	minValue;
    UINT32	maxValue;
    const LcdParamEnumValue *enumlist;
} LcdParamEnumIntValue;


#define MAX_CMDLEN 	80
#define MAX_CMDLENC 	15
#define MAX_CMDLINE 	MAX_RXBUF_LENGTH
#define MAX_LEVEL 	8
#define TEXTUAL_IMAGE_SIZE      (FLASHREGIONSIZE - SRAM_END_OFFSET)
#define PREFIX_LEN 	25

#define MAX_HEX_LEN	31

#define MA

typedef struct
{
    const char *name;
    int verb;
    int privilege ;
} VerbList;

typedef enum 
{
    TNONE,
    INT,
    IPADDR,
    MACADDR,
    MACADDR2,
    STR,
    STRNUM,
    ENUM,
    GROUP,
    DATETIME,
    TIME,
    ENUMINT,	
    PASSWD,	/* encoded string */
    HEX,	/* variable length hex data */
    VARDATA,	/* variable length raw data */
    BASESTR,	/* unicode string */
    ITypeMAX
} IType;
    
#define ERRMSG_NVRAM_FULL _printf(term, "NVRAM Full, operation failed.\r\n")

#define CLI_LIST_FMT	"  %-22s   < %-48s >\r\n"
#define BAD_CMD_FMT	"%s"
#define BAD_CMD_STR	"*** bad command ***\r\n"

#define TRUE 1
#define FALSE 0

#define OK_CLI 		1
#define FAIL_CLI 	-1
#define FAIL2_CLI 	-2


int _strlen(char *string);
char *_strcpy(char *dest, const char *src);
char *_strncpy(char *dest, char *src, int len);
char *_strcat(char *dest, char *src);
char *_strlcat(char *dest, const char *src);
char *_strstr(char *dest, const char *src);
int _strncmp(char *dest, char *src, int len);
int _strncmpi(char *dest, char *src, int len);

void _printf(TermInfo *term, const char *string, ...);
void prints(TermInfo *term, int title, const char *string, ...);
void sprintip(char *buf, int ip);
void sprintdatetime(char *buf, int datetime);
void sprinttime(char *buf, int time);

Pointer _profRead(MibProfileType proftype, int *inst);
Pointer _profRead2(MibProfileType proftype, int, SlotNumber);
Boolean _profWrite(MibProfileType proftype, int *inst, Pointer prof);
Boolean _profWrite2(MibProfileType proftype, int , SlotNumber, Pointer prof);
int profileBuildFactory(MibProfileType profType, Pointer *_prof);
Boolean profileUpdate(MibProfileType profType, int index, Pointer prof);
void profileInitParam(MibProfileType profType, int *namep, int *names, int *activep, int *actives);

int argGet(char *string, int num, char *buf);

int argGetInst(char *string, int num, char *buf);
int argGetInstInt(TermInfo *term);
int argGetCmd(char *string, int num, char *buf);
char *argShift(char *string);
void argStripLast(char *string);
void argAppend(char *string, char *str);

int bcdToInt(int);
int intToBcd(int);

int isDigit(char *str);
int _getInt(Pointer prof, int offset, int isize);
void _setInt(Pointer prof, int offset, int isize, int value);

const char *enumString(const LcdParamEnumValue *enum1, int value);
int enumValue(const LcdParamEnumValue *enum1, char *string);
void enumList(const LcdParamEnumValue *enum1, TermInfo *term, const char *string);

int validateNumber(TermInfo *term, int cmdindex, int isize, char *buf);
int validateStr(TermInfo *term, int cmdindex, int isize, char *buf);
int validateInt(TermInfo *term, int cmdindex, int range);
int validateEnum(TermInfo *term, int cmdindex, LcdParamEnumValue *enum1);

const char *profGetStr(MibProfileType proftype, int *inst, int offset);
int profGetInt(MibProfileType proftype, int *inst, int offset, int isize);

int profSetValue(MibProfileType proftype, int *inst, int offset, int isize, IType itype, Pointer enum1, TermInfo *term, int cmdindex);
int profileByNamei(MibProfileType profType, char *name, Pointer *p);
Pointer profileByNumber(MibProfileType profType, int index);
void profileVerifyInst(TermInfo *term);

# define instanceMax(x)		getSystemFeatureProfileInstanceMax(PLATFORM_BUILDIN, x, NULL)
# define instanceMin(x)		getSystemFeatureProfileInstanceMin(PLATFORM_BUILDIN, x, NULL)

void instanceCreate(TermInfo *term);
void instanceShow(TermInfo *term);
void instanceDelete(TermInfo *term);
void instanceVerify(TermInfo *term);

void intInstShow(TermInfo *term);
void intInstVerify(TermInfo *term);


# if ( UTT_DEBUG == FYES )

# undef	CLI_CODE_DEBUG
# define CLI_CODE_DEBUG  

# undef CLI_DEBUG
# define CLI_DEBUG  

# endif


#  if defined( CLI_CODE_DEBUG ) 

    /*
     * Variable to control fun time debug output
     */
extern int far		_cliDebugDumping;

#   define CLIPRINTF(x)	do {				\
			    if ( _cliDebugDumping ) {		\
				printf x; 			\
			    }				  	\
			} while (0)


# else

# define CLIPRINTF(x)			

# endif 

extern Bool pppoeEnabled;
extern Bool l2tpEnabled;
extern Bool cbqEnabled;
extern Bool ipssgEnabled;
extern Bool mulipEnabled;
extern Bool hostLimit;
extern Bool vpnClient;
extern Bool vpnServer;

extern Boolean noPrompt;

void wuProfInitParam(MibProfileType proftype, int *inst, int *index, SlotNumber *slot);

const char *
profGetMac(MibProfileType proftype, int *inst, int offset);

const char *
profGetHex(MibProfileType proftype, int *inst, int offset, int isize);

const char *
profGetPasswd( Pointer prof, int offset);

void 
profSetPasswd(Pointer prof, int offset, int isize, const char *src);

int 
profValidatePasswd(int isize, const char *buf);

void 
sprintPasswd(char *buf, const char *pass);

#define CLI_SECURE_FMT1		"%s"
#define CLI_SECURE_STR1		"CRYPT1_"

const char *
cliProfGetPasswd(MibProfileType proftype, int *inst, int offset);

int 
cilprofValidatePasswd(TermInfo *term, int cmdindex, const char *buf, int isize);

extern int 
profValidateEnumInt(TermInfo *term, int cmdindex, LcdParamEnumIntValue *enum1);

extern int 
profGetEnumInt(MibProfileType proftype, int *inst, int offset, int isize);

extern void 	
profSetEnumInt(Pointer prof, int offset, int isize, int value);

extern void 	
sprintEnumInt(char *buf, LcdParamEnumIntValue *enum1, int value);

extern int cliArgCmdDumping;
extern Boolean cliDumping ;

//define the key of instname and active of the args

#define INSTNAMEKEY     "instName"
#define INSTACTKEY      "enabled"

#define INSTACT_FMT	"%s"

#define INSTACT_YES  	"Yes"
#define INSTACT_NO   	"No"

#define INSTNAMEKEY_BUILDIN	name
#define INSTACTKEY_BUILDIN	active


#define CLI_COMMENT_LINE	"!"
#define CLI_COMMENT_RESERVED	"!?"

#endif  /* ARGCMD_H */


