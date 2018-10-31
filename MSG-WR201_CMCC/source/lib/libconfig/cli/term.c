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
 * SCCS ID:	$Id: term.c,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/




#include"argcmd.h"
#include"cmd.h"
#include"term.h"
#include"baeeprom.h"
#include"l2tunnel.h"
#include"nat.h"
#include"tftp.h"
#include"vrtr.h"
#include"crc32.h"

#include "nvram.h"
#include "platform.h"

#include "revision.h"

#include "lcdupdwn.h"

#if ( WEB_SERVER == FYES )
#include "webProf.h" //add by jxy7@2003-12-10 for WebUploadFile
#endif //WEB_SERVER


extern char cmdline[];
extern TermCmd *root;
extern int first;
extern int *pMasterClock;

extern Boolean monitorEcho; /* in file monitor.c, for monitor echo mode */

Boolean noPrompt; /* if we should prompt user the last command */
Boolean silent; /* no any output during cli parsing */

extern Boolean saveAll; /* show default configuration or not */

static int _inDebug;
TermInfo *debugTerm; 
static SEM_ID _sem;

	/* Now this huge buffer is fixed, looks waste of memory.
	   But I have no choice because cannot allocate such a
	   continuous large block of memory */

#define IMAGE_SEG_SIZE 		65536
#define IMAGE_HASH_SIZE 	2048		/* like ftp hash cmd */

#if (TWO_FLASH == FYES)
#define TFTP_BUFS		(31*2)		/* max image size is TFTP_BUFS*IMAGE_SEG_SIZE */
#else
#define TFTP_BUFS		(31)		/* max image size is TFTP_BUFS*IMAGE_SEG_SIZE */
#endif

/* v0 image file format is:  crc32 + length(not include hdr)  + binary 
			     4byte   4byte   			image   */

#define IMAGE_HDRSZ		(2 * sizeof(int))
#define IMAGE_CRCSZ		(1 * sizeof(int))
#define IMAGE_LENSZ		(1 * sizeof(int))

static Byte *_tftpbuf[TFTP_BUFS];
static Byte *_tftpbufp;
static Uint _tftpbufi, _tftplen;

SEM_ID getDebugSemaphore( LcdTerminalHdl);
extern Boolean mtMonitorDo(char *);
Bool tftpLoadCode(TermInfo *term);

#if (TWO_FLASH == FYES)
static Boolean needWriteToTheSecondFlash(Uint *first_image_len);
static void eraseTheSecondFlash(TermInfo *term);
#endif

Char * _termSrvGetLine(TermInfo *term, Bool echo, Bool menuMode
# if ( ASYNC == FYES )
               , Bool hunt,Bool checkCtrlC
# endif /* ASYNC */
                , Bool clear
               );

extern int recvDebugOn;

void cmdROOT();

#if ( SHELL == FYES )

//utt-zhangdl-added
#include "exec.h"

extern Bool tftpLoadWeb(TermInfo *term); 
extern Bool tftpLoadSymbol(TermInfo *term);
extern int termShellCmd(TermInfo *term);

# endif  /* SHELL == FYES */

VerbList verblist[] = { {"show",    VERB_SHOW, 0}, 
			{"more",    VERB_MORE, 0},     /* added by llliu@080424 */ 
			{"set",     VERB_SET, 1}, 
			{"write",   VERB_WRITEFLASH, 1},
			{"new",     VERB_NEW, 1},
	 		{"delete",  VERB_DELETE, 1},
			{"dial",    VERB_DIAL, 1},
			{"hangup",  VERB_HANGUP, 1},
			{"save",    VERB_SAVE, 1},
			{"clear",   VERB_CLEAR, 1},
			{"debug",   VERB_DEBUG, 1},
	    		{0, 0, 0}};


int termLogin(TermInfo *term, char *login, char *pass)
{
    const SecurityProfile *prof;
    int i;
    int result = FALSE;

    for(i = 0;i < MAX_SECURITY_PROFILES;i++)
    {
    	prof = requestSecurityProfilePointer(i);
	if( (prof->active) &&
	      !strcmp(login, prof->hostName) && 
		!strcmp(pass, prof->password) ) {
	   result = TRUE;
        }
	releaseSecurityProfilePointer(prof);	
	if ( result == TRUE) {
	  return result;
        }
    }
    _printf(term, "Invalid login\r\n\r\n");
    return FALSE;
}

void termClearScreen(TermInfo *term)
{
    char clrscn[] = {0x1b, '[', '2', 'J', 0};
    char curshome[] = {0x1b, '[', 'H', 0};

    _printf(term, "%s", clrscn);
    _printf(term, "%s", curshome);
}

TermCmd *termDo(TermInfo *term, int _verb)
{
    int found, tmplen;
    char tmp[MAX_CMDLEN];
    int inst[MAX_LEVEL];
    TermCmd *cmd1, *cmd2 = 0;
    TermCmd *cmd;
    volatile TermCmd *cmdd;

    cmdd = cmd = root;
    term->result = OK_CLI;
    term->inst = inst;
    inst[2] = 2;

/* Default SLOT_PROF instance is 2 (Ethernet) */

    while((tmplen = argGetCmd(term->lastInput, cmd->level + 2, tmp))) 
    {
	found  = 0;
	if(cmdd->sonHead) 
	{
	    cmdd = cmdd->sonHead;
	    cmd = (TermCmd*)cmdd;
	    for(cmd1 = cmd;cmd1;cmd1 = cmd1->next)
		if(!_strncmpi((char*)cmd1->cmdlist->name, tmp, tmplen) 
			&& (cmd1->cmdlist->privilege & (_verb | _verb << CMD_SHIFT)))
		{
		    cmd2 = cmd1;
		    found++;
		    if(_strlen((char*)cmd1->cmdlist->name) == tmplen)
		    {
			found = 1;
			break;
		    }
		}
	    if(found == 1)      
	    {
		cmdd = cmd = cmd2;
		if((cmd->cmdlist->privilege & CMD_INSTANCE) && _verb != VERB_NEW)
		{
		    term->verb = VERB_VERIFY;
		    term->cmd = cmd;
		    cmd->cmdlist->func(term);
		    if(term->result == FAIL_CLI)
			return cmd;
		    if(term->result == FAIL2_CLI)
			return 0;
		}
		continue;
	    }
	    if(!found) *tmp = 0;
	    if(cmd_ListBranch(cmd->parent, term, tmp, _verb))
	    	return cmd->parent;
	    else
		return 0;
	}
	else
	{
	    break;
	}
    }
    if(_verb & cmd->cmdlist->privilege)
    { 
	term->verb = _verb;
	term->cmd = cmd;
	cmd->cmdlist->func(term);
	if(term->result == FAIL_CLI)
	    return cmd;
	else
	    return 0;
    }
    else
    {
	if(cmd_ListBranch(cmd, term, (char *)"", _verb))
	    return cmd;
	else
	    return 0;
    }
    return cmd;
}

int termIsLastExist(TermInfo *term)
{
    if(term->cmdHisPutIndex < 0)
	return FALSE;
    else
	return TRUE;
}

int termGetLast(TermInfo *term, char *buf, int dir)
{
    if(!termIsLastExist(term)) return 0;
    if(dir == 1)
    {
	if(term->cmdHisGetIndex == -1)
	    term->cmdHisGetIndex = term->cmdHisPutIndex;
	if(++term->cmdHisGetIndex > term->cmdHisPutIndex)
	if(term->cmdHisGetIndex >= HISTORY_LEN || !term->cmdHisRewind)
	    term->cmdHisGetIndex = 0;
    }
    else
    {
	if(term->cmdHisGetIndex == -1)
	    term->cmdHisGetIndex = term->cmdHisPutIndex;
	else
	    if(--term->cmdHisGetIndex < 0)
		if(term->cmdHisRewind)
		    term->cmdHisGetIndex = HISTORY_LEN - 1;
		else
		    term->cmdHisGetIndex = term->cmdHisPutIndex;
    }
    _strcpy(buf, term->cmdHis[term->cmdHisGetIndex]);
    return _strlen(buf);
}



void termPut(TermInfo *term, char *buf)
{
    if(!strcmp(buf, term->cmdHis[term->cmdHisPutIndex])) 
    {
	term->cmdHisGetIndex = -1;
	return;
    }
    if(++term->cmdHisPutIndex >= HISTORY_LEN)
    {
	term->cmdHisRewind = TRUE;
	term->cmdHisPutIndex = 0;
    }
    _strcpy(term->cmdHis[term->cmdHisPutIndex], buf);
    term->cmdHisGetIndex = -1;
}

int termSkipCmd(TermInfo *term)
{
    char *args = NULL;
    
    if ( term ) {
	args = term->lastInput;
   	while ( args && *args && isspace(*args)) {
        	args++;
    	}
        if ( ! (*args) ) {
	  return FALSE;
        } else {
    	    /* comment line handling */ 
    	    if ( (strncmp( args, CLI_COMMENT_LINE,strlen(CLI_COMMENT_LINE) ) == 0)
    	      && (strncmp( args, CLI_COMMENT_RESERVED, strlen(CLI_COMMENT_RESERVED) ) != 0) /* reserved for debug */
	       ) {
		    return TRUE;
    	    }
        }
    }
    return FALSE;
}
    	
int termInnerCmd(TermInfo *term, int mode)
{
    Uint32 storedSerialNum;
    char *pass;
    int i;

    /* internal cmd handling */

    switch ( mode ) {
	case TERM_PARSE_BYPASS:
    		return FALSE;	
		break;
	default:
    		if(!strcmp(term->lastInput, "revision")) {
			_printf(term, "%s\r\n", build_stamp);
    			storedSerialNum = getSystemVersionHandle()->serialNumber ;
			_printf(term, "MBID: %lu\r\n", storedSerialNum);
			_printf(term, "Feature enabled: %s\r\n", getFeatureStr());
			_printf(term, "Product ID: %s\r\n", getSystemPlatformName());
			_printf(term, "Software Revision: %s\r\n", getVersionStr());
			return TRUE;
    		}
    		if(!strcmp(term->lastInput, "su")) {
        		termSrvLocalOutput(term, "Pw :");
        		pass = _termSrvGetLine(term, FALSE, term->menuMode, TRUE);
        		i = strcmp(pass, "astpower");
        		deleteDynamicString(&pass);
        		if(!i) {	
            			if(_inDebug) mtMonitorMenu(debugTerm->lcdTerm);
            			_inDebug = TRUE;
	    			debugTerm = term;
	    			monitorEcho = TRUE;
            			mtMonitorMenu(term->lcdTerm);
	    			_printf(term, "Debug mode ON.\r\n");
			}
			return TRUE;
    		}
    		if(!strcmp(term->lastInput, "promptoff")) {
			noPrompt = TRUE;
			return TRUE;
    		}
    		if(!strcmp(term->lastInput, "prompton")) {
			noPrompt = FALSE;
			return TRUE;
    		}
    		if(!strncmp(term->lastInput, "ld", 2)) {
			if(tftpLoadCode(term))
    	    			termPut(term, term->lastInput);
			else
	    			_printf(term, "Usage: ld ip-address[:port] filename.\r\n");
			return TRUE;
    		}
    		if(!strcmp(term->lastInput, "image")) {
			_printf(term, "%s\r\n", imageName);
			return TRUE;
    		}
#if (SHELL == FYES)
     		if(!strncmp(term->lastInput, "sld", 3)) {
			if(tftpLoadSymbol(term)) {
    	    			;
			} else {
	    			_printf(term, "Usage: sld ip-address[:port] filename.\r\n");
			}
			termPut(term, term->lastInput);   
			return TRUE;
    		}
     		if(!strncmp(term->lastInput, "wld", 3)) {
			if(tftpLoadWeb(term)) {
    	   			;
			} else {
	    			_printf(term, "Usage: wld ip-address[:port] source filename dest filename.\r\n");
 			}
			termPut(term, term->lastInput);
			return TRUE;
    		}
# endif /* SHELL == FYES */
    
#if( EXECPHOOK == FYES )
    		if(!strncmp(term->lastInput, "hooktest", 8)) {
 			extern void undefHookTest();
 	
 			undefHookTest();
 			return TRUE;
    		}
#endif /* EXECPHOOK == FYES */
    
    		if(!strcmp(term->lastInput, "][")) {
        		commandNoLogReset(1);
        		return TRUE;
    		}
    
    		if(!strcmp(term->lastInput, "nvramc")) {
        		addErrorStatusLog( TRUE, ERROR_NVRAM_CLEARED );
			buildNewNvram();
			return TRUE;
    		}
    		if(!strcmp(term->lastInput, "nvbackup")) {
			nvramBackup();
			return TRUE;
    		}
    		if(!strcmp(term->lastInput, "deb")) {
        		if(_inDebug) {
	    			if(debugTerm != term) {
	        			_printf(term, "Only ONE debug session is allowed.\r\n");
	    			} 
				else {
					mtMonitorMenu(term->lcdTerm);
	        			_printf(term, "Debug mode OFF.\r\n");
                			_inDebug = FALSE;
	    			}
	    			return TRUE;
			}
        		termSrvLocalOutput(term, "Password :");
        		pass = _termSrvGetLine(term, FALSE, term->menuMode, TRUE);
        		i = strcmp(pass, "lakjas");
        		deleteDynamicString(&pass);
       	 		if(!i) {
            			_inDebug = TRUE;
	    			debugTerm = term;
	    			monitorEcho = TRUE;
            			mtMonitorMenu(term->lcdTerm);
	    			_printf(term, "Debug mode ON.\r\n");
        		} else { 
            			_printf(term, "Access Denied.\r\n");
			}
			return TRUE;
    		}

#if (TWO_FLASH == FYES && 0)
		/*
		 * added by llliu@080927 for erasing the second flash.
		 */
		if (!strcmp(term->lastInput, "erase")) {
		    eraseTheSecondFlash(term);
		}
#endif


#if ( SHELL == FYES )
   		return termShellCmd(term);  
# endif /* SHELL == FYES */
    		return FALSE;	
   } /* switch */
}
void termTerm(TermInfo *term)
{
    if(_inDebug && debugTerm == term)
    {
	mtMonitorMenu(term->lcdTerm);
	_inDebug = FALSE;
    }
}

int termParse(TermInfo *term, int mode, char *cmdlineprfx)
{
    int i, found, index, result=RESULT_UNFIN;  
    char arg[MAX_CMDLEN];
    char *tmp = NULL;
    char tmp1[MAX_CMDLEN];
    TermCmd *cmd, *cmd1;
    
    *cmdlineprfx = 0;

    /* skip comment line, if any */
    if(termSkipCmd(term)) return RESULT_DONE;

    /* do internal cmd if not startup procedure */
    if(!silent)
    {
    	if(termInnerCmd(term, mode)) return RESULT_DONE;

    	if(_inDebug && term == debugTerm)
	if(mtMonitorDo(term->lastInput))
	{
		/* Add a additional command prompt to console
		   because printf is asychronized output */

	    printf("%s", term->termPrompt);
    	    termPut(term, term->lastInput);
	    return RESULT_NOPROMPT;
	}
    }

    /* cli cmd parse */
    tmp = MALLOC(MAX_CMDLINE);
    ASSERT(tmp);
    _strcpy(tmp, term->lastInput);
    result = RESULT_DONE;
    do 
    {
    if(argGet(tmp, 1, arg))
    {
	found = 0;
	index = 0;
	for(i = 0;verblist[i].name;i++)
	    if(!_strncmpi((char *)verblist[i].name, arg, _strlen(arg)))
	    {
		index = i;
		found++;
	    }
	if(found)
	{
	    if(found == 1)
	    {
		argShift(tmp);
		cmd1 = termDo(term, verblist[index].verb);
		if(silent) break;
		if(cmd1 && !noPrompt )
		{
		    for(cmd = cmd1;cmd != root;cmd = cmd->parent)
		    {                   
		    	_strlcat(cmdlineprfx, " ");
		    	if((cmd->cmdlist->privilege & CMD_I) && argGetInst(tmp, cmd->level, tmp1))
			    if(cmd == cmd1 && term->result != FAIL_CLI || cmd != cmd1)
		    	    {
		    	    	_strlcat(cmdlineprfx, tmp1);
		    	    	_strlcat(cmdlineprfx, "/");
		    	    }
		    	_strlcat(cmdlineprfx, cmd->cmdlist->name);
		    }
		    if((cmd1->cmdlist->privilege & CMD_I) && term->result == FAIL_CLI)
			cmdlineprfx[_strlen(cmdlineprfx) - 1] = '/';
		    _strlcat(cmdlineprfx, " ");
		    _strlcat(cmdlineprfx, arg);
		    result = RESULT_PREFIX;
		}
		else
		{
		    _strlcat(tmp, " ");
		    _strlcat(tmp, arg);
		    termPut(term, tmp);
		}
	    }
	    if(silent) break;
	    if(found > 1 && !noPrompt)
	    {
		for(i = 0;verblist[i].name;i++)
		if(!_strncmpi((char *)verblist[i].name, arg, _strlen(arg)))
		    _printf(term, " %s", verblist[i].name);
		_printf(term, "\r\n");
	    }
	}
    	else
    	{	
	    if(silent) break;
    	    termPut(term, tmp);
    	    result = RESULT_UNFIN;
    	}
    }
    } while(0);
    FREE(tmp);
    return result;
}

# define CLI_ALIVE_SIGN		(0xF)

extern void cliParseData(char *str)
{
    static int parseCnt;
    static TermInfo term;
    static char tmp2[200], *str2;

    char tmp[200], *str1;

    memset(&term, 0x00, sizeof(TermInfo));
    memset(tmp, 0x00, sizeof(tmp));

    EnterCS();

    silent = TRUE;
    while(*str > 0)
    {
	/* aha, we are alive!, output msg to the console when booting with a big profile */
	parseCnt ++ ;
	if ( ( !profUpdate )
		&& ( 0 == (parseCnt % CLI_ALIVE_SIGN ) ) 
	   ) {
		bootPrint("+");
	}

        memset(tmp2, 0, sizeof(tmp));
        str2 = tmp2;
	for(str1 = str;*str1 != '\r' && *str1;str1++) {
	  *str2++ = *str1;
        }
    	term.lastInput = tmp2;
	CLIPRINTF(("%s: tmp2= %s", __FUNCTION__, tmp2));
    	termParse(&term, TERM_PARSE_BYPASS, tmp);
//	rescheduleTask(1);
	CLIPRINTF(("%s: tmp= %s", __FUNCTION__, tmp));
	str = str1 + 2;
    }
    silent = FALSE;

    if ( ( !profUpdate ) 
		&& ( 0 != parseCnt ) ) {
    	bootPrint("\ntotal %lu lines restored.", parseCnt);
    } 

    ExitCS();
}


void nvramBackup()
{
    cliParseData((char*)SRAM_END + 1);
    baeepromChanged( 0, 0x41737400 );
}

static int
_loadCodeCallback( int    status,
                  int     session,
                  void*   buf,
                  int     len,
                  int	tag )
{
    static int counter = 0;
    TermInfo *term = (TermInfo*)tag;
    int len1 = 0;

    if(status == TFTP_WRITE_DATA)
    {
	if(!_tftpbuf[_tftpbufi])
	{
            _tftpbuf[_tftpbufi] = (Byte*)MALLOC(IMAGE_SEG_SIZE);
            
            if(!_tftpbuf[_tftpbufi])
            {   
                _printf(term, "Fatalerr: no more memory for tftp load, failed.\r\n");           
		SemaphoreSignal(_sem);
		return TFTP_EUERROR;
            }
 	}

	if(!_tftpbufp) _tftpbufp = _tftpbuf[0];
	if(_tftpbufp + len <= _tftpbuf[_tftpbufi] + IMAGE_SEG_SIZE)
	{
	    memcpy(_tftpbufp, (Byte*)buf, len);
	    _tftpbufp += len;
	}
  	else 
	{
	    len1 = _tftpbufp + len - (_tftpbuf[_tftpbufi] + IMAGE_SEG_SIZE);
	    memcpy(_tftpbufp, (Byte*)buf, len - len1);
	    if(++_tftpbufi >= TFTP_BUFS)
	    {
		counter = 0;
		SemaphoreSignal(_sem);
		return TFTP_EUERROR;
	    }
	    _tftpbuf[_tftpbufi] = (Byte*)MALLOC(IMAGE_SEG_SIZE);
            if(!_tftpbuf[_tftpbufi])
            {
                _printf(term, "Fatalerr: no more memory for tftp load, failed.\r\n");
		SemaphoreSignal(_sem);
                return TFTP_EUERROR;
            }
	    memcpy(_tftpbuf[_tftpbufi], (Byte*)buf + len - len1, len1);
	    _tftpbufp = _tftpbuf[_tftpbufi] + len1;
	}
	_tftplen += len;
	if(!(counter++ % (IMAGE_HASH_SIZE/TFTP_SEGSIZE)))
	    _printf(term, ".");
	if(len < TFTP_SEGSIZE)
	{
	    counter = 0;
	    SemaphoreSignal(_sem);
	}
    	return TFTP_OK;
    } else
    {
	counter = 0;
	SemaphoreSignal(_sem);
	return TFTP_EUERROR;
    }
}

extern Uint getBoardSw();

#if (TWO_FLASH == FYES)
static Boolean _checkWebMagic(Int i, Int length)
{
    Int buf_left;
    buf_left = IMAGE_SEG_SIZE - length;
    Byte *cursor;
    cursor = _tftpbuf[i] + length;

    /*
     * 此_tftpbuf有足够的空间放置WEBL。
     */
    if (buf_left >= WEB_MAGIC_WIDTH) {
	if(IS_WEB_IMAGE(cursor, 0)) {
	    return TRUE;
	}
	ASSERT_WARN(0);
	return FALSE;
    }

    else if (buf_left == 0) {
        cursor = _tftpbuf[i+1];
        if (IS_WEB_IMAGE(cursor, 0)) {
	    return TRUE;
        }

	ASSERT_WARN(0);
	return FALSE;
    }

    else if (buf_left == 1) {

	if (cursor[0] == 'W') {
	    cursor = _tftpbuf[i+1];
	    if (cursor[0] == 'E' && cursor[1] == 'B' && cursor[2] == 'L') {
		    return TRUE;
	    }
		    
	}
	ASSERT_WARN(0);
	return FALSE;
    }

    else if (buf_left == 2) {

	if (cursor[0] == 'W' && cursor[1] == 'E') {
	    cursor = _tftpbuf[i+1];
	    if (cursor[0] == 'B' && cursor[1] == 'L') {
		    return TRUE;
	    }
	}

	ASSERT_WARN(0);
	return FALSE;
    }

    else if (buf_left == 3) {
	if (cursor[0] == 'W' && cursor[1] == 'E' && cursor[2] == 'B') {
	    cursor = _tftpbuf[i+1];
	    if (cursor[0] == 'L') {
		    return TRUE;
	    }
	ASSERT_WARN(0);
	return FALSE;
	}
    }
    else {
	ASSERT(0);
    }




    /*
     * never reach here.
     */
    ASSERT(0);
    return TRUE;
}

/*
 * 检查Web数据是否大于第二Flash的容量。
 */
static Boolean _checkWebLen(Int web_len, Int length)
{
    if ((web_len + WEB_HDR_WIDTH) > SECOND_FLASH_VALID_SIZE) {
	return FALSE;
    }

    return TRUE;

}

static Boolean _checkWebCrc(Int i,Int length, Int web_len)
{
    Int crc_web_high, crc_web_low;
    Uint32 crc_web;
    Uint32 crc_cal;
    Int web_offset;
    Int block_left;

    web_offset = length + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH + WEB_CRC_WIDTH;
    crc_cal = 0;
    
    /*
     * crc stored in web.
     */
    crc_web_high = LOAD_WORD(_tftpbuf[i]+length+WEB_MAGIC_WIDTH+WEB_LEN_WIDTH);
    crc_web_low = LOAD_WORD(_tftpbuf[i]+length+WEB_MAGIC_WIDTH+WEB_LEN_WIDTH+2);
    crc_web = (crc_web_high << 16) | crc_web_low;


    /*
     * 首先找到webrom在_tftpbuf中的位置。
     */
    if (web_offset < IMAGE_SEG_SIZE) {
        block_left = IMAGE_SEG_SIZE - web_offset;
        
	/*
	 * 如果在本块_tftpbuf已经包含了全部的webrom，那么直接使用web_len作为
	 * 剩余的大小，目前这种情况没有进行过单元测试。
	 */
	if (block_left > web_len) {
	    block_left = web_len;
	} 

	crc_cal = crc32(0, (Byte *)(_tftpbuf[i] + web_offset), block_left);
    }
    else {
	/*
	 * the next _tftpbuf.
	 */
	Int block_start;
	block_start = web_offset - IMAGE_SEG_SIZE;
	block_left = IMAGE_SEG_SIZE - block_start;

	if (block_left > web_len) {
	    block_left = web_len;
	}

	/*
	 * 前面一块_tftpbuf已经被使用完毕了，所以需要将索引增加1，目前这种情况没有进行过单元测试。
	 */
	i++;
	crc_cal = crc32(0,(Byte *)( _tftpbuf[i] + block_start), block_left);
    }

    /*
     * 那块“残缺”的_tftpbuf的crc已经被计算过了，所以将索引加1，指向下一个索引。
     * 同时，从整个web_len中减去已经计算过的webrom大小，即block_left.
     */
    i++;
    web_len -= block_left;

    for(;web_len > 0;i++, web_len -= IMAGE_SEG_SIZE) {
        if (_tftpbuf[i]) {
    	crc_cal = crc32( crc_cal, _tftpbuf[i], web_len > IMAGE_SEG_SIZE ? IMAGE_SEG_SIZE : web_len);
	}
    }


    if (crc_web != crc_cal) {
        printf("Error: mismatching webrom CRC(calculated: 0x%x while stored: 0x%x).\n",
                crc_cal, crc_web);
	return FALSE;
    }


    return TRUE;
}

#endif

/*
 * 检查_tftpBuf的尾部是否含有某段内存。
 */
static Boolean _isBufTailHasMagic(Int swlen, Int len, Byte *magic)
{
    Int which_one = swlen / IMAGE_SEG_SIZE;
    Int where = swlen % IMAGE_SEG_SIZE;
    Boolean rtn = FALSE;
    Byte *cursor = NULL;
    Int offset = 0;
    Byte *buf;
    buf = (Byte *)malloc(len);

    /*
     * 必须大于一块。
     */
    if (which_one < 1) {
	ASSERT_WARN(0);
	return FALSE;
    }

    ASSERT_WARN(where >= 0);


    /*
     * 完全在上块
     */
    if (where == 0) {
	cursor = _tftpbuf[which_one - 1];
	offset = IMAGE_SEG_SIZE - len;
	cursor += offset;
	memcpy(buf, cursor, len);
	CLIPRINTF(("%s-%d, completely in prev block, buf: %x-%x-%x-%x.\n",
		__FUNCTION__, __LINE__,
		buf[0], buf[1], buf[2], buf[3]
	      ));
    }
    /*
     * 完全在本块
     */
    else if (where >= len) {
	cursor = _tftpbuf[which_one];
	offset = where - len;
	cursor += offset;
	memcpy(buf, cursor, len);
	CLIPRINTF(("%s-%d, completely in current block, buf: %x-%x-%x-%x.\n",
		__FUNCTION__, __LINE__,
		buf[0], buf[1], buf[2], buf[3]
	      ));
    }
    /*
     * 上面那块有，下面那块也有。
     */
    else {
	Int prev;
	cursor = _tftpbuf[which_one - 1];
	prev = len - where;		    // 表明上面那块的个数。
	cursor += (IMAGE_SEG_SIZE - prev);
	memcpy(buf, cursor, prev);
	cursor = _tftpbuf[which_one];
	memcpy(buf + prev, cursor, where);
	CLIPRINTF(("%s-%d, both in prev and current block, buf: %x-%x-%x-%x.\n",
		__FUNCTION__, __LINE__,
		buf[0], buf[1], buf[2], buf[3]
	      ));
    }


    /*
     * 进行比较
     */
    if (memcmp(buf, magic, len) == 0) {
	rtn = TRUE;
    }

    free(buf);
    return rtn;
}

/*
 * added by llliu@090306
 */
#define DECADE_BUG_TAIL_LEN	    4
static Byte decade_bug_flag[DECADE_BUG_TAIL_LEN] = {0x32, 0x30, 0x30, 0x39};

static Boolean
_decadeSNCheck(Int length)
{
    /*
     * 首先判断此台设备是否是新设备，如果不是新设备，直接让其升级。
     */
    if (getSystemVersionHandle()->serialNumber <= MAX_VALID_SN_DECADE) {
	return TRUE;
    }

    /*
     * 此台设备是序列号大于9000000的设备，那么判断所导入的软件中是否含有“2009”关键字，
     *
     * 如果没有的话，则终止升级。
     */
    if (!_isBufTailHasMagic(length, DECADE_BUG_TAIL_LEN, decade_bug_flag)) {
	return FALSE;
    }

    return TRUE;

}


static Boolean
_rtcCheck(char *filename)
{
    if (HPL_IS_4240G() || HPL_IS_4210G()) {
	if (!rtcEnabled) {
	    if (strstr(filename, "vstart") ||
		    strstr(filename, "v2008-") ||
		    strstr(filename, "v2008pV2-")) {
		return FALSE;
	    }
	}
    }

    return TRUE;
}
 


#define NEW_HARDWARE_COUNTER_LEN    4
static unsigned char new_hardware_magic[2] = {
    0x41,
    0x41
};


static Uint16 getCurrentVersion(Int len)
{
    Byte *buf = (Byte *)FLASH_ADDR;
    
    buf += (((Uint *)(FLASH_ADDR))[0] + IMAGE_HDRSZ - DECADE_BUG_TAIL_LEN - NEW_HARDWARE_COUNTER_LEN);

     if (memcmp(buf, new_hardware_magic, 2)) {
	printf("current version magic error, len:%d, buf@%p, buf[0]:%02x, buf[1]:%02x!\n",
		len, buf, buf[0], buf[1]); 
	return 0;
    }

    return ((buf[2] << 8) | buf[3]);
}


static Boolean _newHardwareCheck(Int swlen)
{
    Int which_one = swlen / IMAGE_SEG_SIZE;
    Int where = swlen % IMAGE_SEG_SIZE;
    Byte *cursor = NULL;
    Int offset = 0;
    Byte *buf;
    Uint16 version;
    Uint16 cver;
    Int len = DECADE_BUG_TAIL_LEN + NEW_HARDWARE_COUNTER_LEN;

    buf = (Byte *)malloc(len);

    /*
     * 必须大于一块。
     */
    if (which_one < 1) {
	ASSERT_WARN(0);
	return FALSE;
    }

    ASSERT_WARN(where >= 0);


    /*
     * 完全在上块
     */
    if (where == 0) {
	cursor = _tftpbuf[which_one - 1];
	offset = IMAGE_SEG_SIZE - len;
	cursor += offset;
	memcpy(buf, cursor, len);
	CLIPRINTF(("%s-%d, completely in prev block, buf: %x-%x-%x-%x.\n",
		__FUNCTION__, __LINE__,
		buf[0], buf[1], buf[2], buf[3]
	      ));
    }
    /*
     * 完全在本块
     */
    else if (where >= len) {
	cursor = _tftpbuf[which_one];
	offset = where - len;
	cursor += offset;
	memcpy(buf, cursor, len);
	CLIPRINTF(("%s-%d, completely in current block, buf: %x-%x-%x-%x.\n",
		__FUNCTION__, __LINE__,
		buf[0], buf[1], buf[2], buf[3]
	      ));
    }
    /*
     * 上面那块有，下面那块也有。
     */
    else {
	Int prev;
	cursor = _tftpbuf[which_one - 1];
	prev = len - where;		    // 表明上面那块的个数。
	cursor += (IMAGE_SEG_SIZE - prev);
	memcpy(buf, cursor, prev);
	cursor = _tftpbuf[which_one];
	memcpy(buf + prev, cursor, where);
	CLIPRINTF(("%s-%d, both in prev and current block, buf: %x-%x-%x-%x.\n",
		__FUNCTION__, __LINE__,
		buf[0], buf[1], buf[2], buf[3]
	      ));
    }

    if (memcmp(buf, new_hardware_magic, 2)) {
	free(buf);
	printf("loaded file magic error!\n"); 
	return FALSE;
    }
    version = ((buf[2] << 8) | buf[3]);
    cver = getCurrentVersion(swlen);
    if (!cver || (cver > version)) {
	free(buf);
	printf("\nhardware version error, current version:%d, load version:%d!\n", cver, version); 
	return FALSE;
    }

    free(buf);
    return TRUE;
}

static Int
_imageValid(char *filename)
{
    Uint *image = (Uint *)_tftpbuf[0];
    Uint crc, crc1, i;
    int length;
    int image_cnt;


    /*
     * NULL pointer checking. 
     * mainly for web upgrading.
     */
    if (filename == NULL || image == NULL) {
        return IMAGE_UPGRADE_ERROR_PTR;
    }


    if(getBoardRev() == BOARD_1000)
    {
	if(*(Byte*)0 == 0xea)
	{
	    if(filename[0] != 'd') return IMAGE_UPGRADE_ERROR_D;
	}
	else
	    if(filename[0] != 'b') return IMAGE_UPGRADE_ERROR_B;
    }
    else if(getBoardRev() == BOARD_1011)
    {
	if(filename[0] != 'e') return IMAGE_UPGRADE_ERROR_E;
    }
    else if(getBoardRev() >= BOARD_3000)
    {
	Uint pid = getBoardSw();
	if((pid & 0xffff) == 0xffff)
	{
	    if(filename[0] != 'g') return IMAGE_UPGRADE_ERROR_G;
	}
	else if(image[2] != pid) {
            
            printf("Error: ID wrong(getBoardSw: 0x%x while stored: 0x%x).\n",
                    getBoardSw(), image[2]);
            return IMAGE_UPGRADE_ERROR_WRONG_ID;
        }
    }
	
    length = image[ 0 ] + IMAGE_HDRSZ;


    /*
     * 十年虫检查
     */
    if (!_decadeSNCheck(length)) {
	printf("Error: can't upgrade to older image, your SN %d exceed %d.\n",
		getSystemVersionHandle()->serialNumber,
		MAX_VALID_SN_DECADE		
	      );
	return IMAGE_UPGRADE_ERROR_DECADE_BUG;
    }

     if (!_rtcCheck(filename)) {
 	printf("Error: can't upgrade due to RTC issues, filename %s\n",
 		filename);
 	return IMAGE_UPGRADE_ERROR_RTC;
     }

     /*
      * 新硬件检查
      */

     if (!_newHardwareCheck(length)) {
	return IMAGE_UPGRADE_ERROR_HARDWARE;
     }




    //printf("\r\nsoftware image buildin length is %ld.\r\n", length);
    //printf("\r\nsoftware image received length is %ld.\r\n", _tftplen);

    if((Uint)length != _tftplen) {
	Boolean rtn;
	rtn = FALSE;
#if (TWO_FLASH == FYES)
	Uint first_image_len = _tftplen;
	rtn = needWriteToTheSecondFlash(&first_image_len);
#endif

	if (rtn) {
	}
	else {
	    printf("Error: mismatching image length(received: %ldBytes while stored: %ldBytes).\n",
                _tftplen, length);
	    return IMAGE_UPGRADE_ERROR_BAD_LEN;
	}
    }

    if ( length > FLASH_IMAGE_SIZE ) {
        printf("Error: Image too big(received: %ldBytes while flash size: %ldBytes).\n",
                length, FLASH_IMAGE_SIZE);
        return IMAGE_UPGRADE_ERROR_TOO_BIG; /* Length is bogus */
    }

    for (image_cnt = 1; 
            (FLASH_IMAGE_SIZE >= image_cnt * SINGLE_FLASH_SIZE) && 
            (length > (FLASH_IMAGE_SIZE - image_cnt * SINGLE_FLASH_SIZE));
            image_cnt++) {
        ;
    }
    if ( image_cnt != 1) {
        printf("Error: image can't be writen to block %d (length:%ld and FLASH_IMAGE_SIZE:%ld).\n",
                image_cnt, length, FLASH_IMAGE_SIZE);
        return IMAGE_UPGRADE_ERROR_SLEEP_FLASH;
    }


# if ( CHECK_SOFTWARE_ID == FYES )
//zhz@20050225
// targe/filename compare
//
    /*
     * sample getBoardSw():
     * 0x554b560a
     *
     * sample targetName:
     *kv4240NBv2008
     * 
     * sample filename:
     * kv4240NBv2008-080408.llliu
     *
     * sample getSystemPlatformName():
     * 4240NB
     */
    if( !strstr(targetName, getSystemPlatformName()) ) {
        printf("Error: no platformName(%s) in targetName(%s).\n",
                getSystemPlatformName, targetName);
        return IMAGE_UPGRADE_ERROR_BAD_TARGETNAME; /* target/platform is bogus */
    }

    if( !strstr(filename, getSystemPlatformName()) ) {
        printf("Error: no platformName(%s) in filename(%s).\n",
               getSystemPlatformName, filename);
        return IMAGE_UPGRADE_ERROR_BAD_FILENAME; /* filename/platform is bogus */
    }

# endif

    crc1 = image[1];
    crc = crc32( 0, _tftpbuf[0] + IMAGE_HDRSZ, IMAGE_SEG_SIZE - IMAGE_HDRSZ );
    length -= IMAGE_SEG_SIZE;
    for(i = 1;length > 0;i++, length -= IMAGE_SEG_SIZE)
    {
	crc = crc32( crc, _tftpbuf[i], length > IMAGE_SEG_SIZE ? IMAGE_SEG_SIZE : length);
	if (length <= IMAGE_SEG_SIZE) {
	    break;
	}
    }



    if (crc != crc1) {
        printf("Error: mismatching CRC(calculated: 0x%x while stored: 0x%x).\n",
                crc, crc1);
        return IMAGE_UPGRADE_ERROR_BAD_CRC;
    }


#if (TWO_FLASH == FYES)
    /*
     * check the second flash crc.
     */
    Uint dummy_len;
    dummy_len = 0;

    /*
     * 只有需要写入到第2块flash的时候，才做此crc检查.
     */
    if (needWriteToTheSecondFlash(&dummy_len)) {
	Int web_len_high, web_len_low;
	Int web_len;

	/*
	 * 检查是否含有WEBL关键字。
	 */
	if (!_checkWebMagic(i, length)) {
	    return IMAGE_BAD_WEB_MAGIC;
	}

	/*
	 * 检查长度是否正确。
	 */
	web_len_high = LOAD_WORD(_tftpbuf[i]+length+WEB_MAGIC_WIDTH);
	web_len_low = LOAD_WORD(_tftpbuf[i]+length+WEB_MAGIC_WIDTH+2);
	web_len = (web_len_high << 16) | web_len_low;

	if (!_checkWebLen(web_len, length)) {
	    return IMAGE_BAD_WEB_LEN;
	}


	/*
	 * 检查校验和是否正确。
	 */

	if (!_checkWebCrc(i, length, web_len)) {
	    return IMAGE_UPGRADE_ERROR_BAD_CRC;
	}

    }



#endif


    /*
     * this image is valid.
     */
    return IMAGE_UPGRADE_SUCCESS;
    
}

static Uint imgLength;

#if (TWO_FLASH == FYES)
static Uint32 secImgLength;
#endif

#if (TWO_FLASH == FYES)
static Boolean _webromCheckInFlash(void)
{
    Byte *web_image = (Byte *)SECOND_FLASH_START_ADDR;
    Int image_len_high, image_len_low, image_len;
    Uint32 crc_web_high, crc_web_low, crc_web;
    Boolean is_second_flash;

    Uint32 crc_cal;

    is_second_flash = FALSE;

    image_len_high = 0;
    image_len_low = 0;
    image_len = 0;

    crc_web_high = 0;
    crc_web_low = 0;
    crc_web = 0;

    crc_cal = 0;

    if (IS_WEB_IMAGE(web_image, 0)) {
	is_second_flash = TRUE;

	image_len_high = LOAD_WORD(web_image + WEB_MAGIC_WIDTH);
	image_len_low = LOAD_WORD(web_image + WEB_MAGIC_WIDTH + 2);
	image_len = (image_len_high << 16) | image_len_low;

	crc_web_high = LOAD_WORD(web_image + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH);
	crc_web_low = LOAD_WORD(web_image + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH + 2);
	crc_web = (crc_web_high << 16) | crc_web_low;

	crc_cal = crc32(0, (Byte *)(web_image + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH + WEB_CRC_WIDTH), image_len);

    }
    else if (IS_WEB_IMAGE(web_image, 1)) {
	is_second_flash = TRUE;

	image_len_high = LOAD_WORD(web_image + 1 + WEB_MAGIC_WIDTH);
	image_len_low = LOAD_WORD(web_image + 1 + WEB_MAGIC_WIDTH + 2);
	image_len = (image_len_high << 16) | image_len_low;

	crc_web_high = LOAD_WORD(web_image + 1 + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH);
	crc_web_low = LOAD_WORD(web_image + 1 + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH + 2);
	crc_web = (crc_web_high << 16) | crc_web_low;

	crc_cal = crc32(
		0,
		(Byte *)(web_image + 1 + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH + WEB_CRC_WIDTH),
		image_len
		);

    }

    if ((is_second_flash == TRUE) && (crc_cal != crc_web)) {
	printf("webrom image in flash checking failed! calculated 0x%x while stored 0x%x.\n",
		crc_cal,
		crc_web);

	return FALSE;
    }

    if (is_second_flash == TRUE) {
	secImgLength = image_len + WEB_MAGIC_WIDTH + WEB_LEN_WIDTH + WEB_CRC_WIDTH;
    }

    if (secImgLength > SECOND_FLASH_VALID_SIZE) {
	ASSERT_WARN(0);
	return FALSE;
    }

    return TRUE;


}
#endif

static Bool
_imageCheck()
{
    Boolean rtn;
    Uint *image = (Uint*)FLASH_ADDR;

    imgLength = 0;
#if (TWO_FLASH == FYES)
    secImgLength = 0;
#endif

    rtn = FALSE;

# if ( CHECK_SOFTWARE_ID == FYES )
//zhz@20050225
// targe/filename compare
//
    if( !strstr(targetName, getSystemPlatformName()) ) {
        return FALSE; /* target/platform is bogus */
    }
//
// serialNumber compare
//
    if ( ( getSystemVersionHandle()->serialNumber < MIN_VALID_SN )
	   || ( getSystemVersionHandle()->serialNumber > MAX_VALID_SN )
	) {
        return FALSE; /* serialNumber is bogus */
    } 

# endif

    imgLength = image[ 0 ];

    if ( imgLength < FLASH_IMAGE_SIZE - IMAGE_HDRSZ ) {
      if(image[ 1 ] == crc32( 0, (unsigned char *) ( image + 2 ), imgLength )) {
#if (TWO_FLASH == FYES)
	if(_webromCheckInFlash()) {
	    return TRUE;
	}
#else
	return TRUE;
#endif

      }
    }

    /* invalid img found */

    imgLength = 0;
#if (TWO_FLASH == FYES)
    secImgLength = 0;
#endif
    return FALSE;



}

extern int
imageStatus ( Uint32 *mode, Uint32 *size, Uint32 *used)
{
  Boolean stat = FALSE;

  if ( mode ) {
    *mode = 0;
  }
  if ( size ) {
    *size = FLASH_IMAGE_SIZE;
  }
  if ( used ) {
    stat = _imageCheck();
    *used = imgLength + IMAGE_HDRSZ;	
  }
  return stat;
}

#if (TWO_FLASH == FYES)
extern Uint32 getSecImgLength(void)
{
    return secImgLength;
}

#endif

/*
 *
 */
static void eraseFlashInterzone(Uint32 startaddr, Uint32 endaddr, TermInfo *term)
{
    Word *p_endaddr;
    Word *p_eraseaddr;
    Int i;

    p_endaddr = (Word *)endaddr;
    p_eraseaddr = NULL;
    i = 0;

    while(1) {
	_printf(term, "e");
	p_eraseaddr = (Word *)(startaddr + (i++)*FLASHREGIONSIZE);

	/*
	 * end the loop.
	 */
	if (p_eraseaddr >= p_endaddr) {
	    break;
	}

	eraseFlash(p_eraseaddr, TRUE);
    }



}

#if (WEB_SERVER == FYES)
static void webEraseFlashInterzone(Uint32 startaddr, Uint32 endaddr)
{
    Word *p_endaddr;
    Word *p_eraseaddr;
    Int i;

    p_endaddr = (Word *)endaddr;
    p_eraseaddr = NULL;
    i = 0;

    while(1) {
	p_eraseaddr = (Word *)(startaddr + (i++)*FLASHREGIONSIZE);

	/*
	 * end the loop.
	 */
	if (p_eraseaddr >= p_endaddr) {
	    break;
	}

	eraseFlash(p_eraseaddr, TRUE);
    }



}
#endif



static void eraseTheFirstFlash(Uint32 startaddr, TermInfo *term)
{
	
    _printf(term, "\r\nErasing the first Flash(from 0x%p to 0x%p).\r\n", startaddr, (BOOT_ADDR + FLASH_SIZE));
	eraseFlashInterzone(startaddr, (BOOT_ADDR + FLASH_SIZE), term);

    _printf(term, "\r\nFirst Flash erasure done!\r\n");


}
static void writeToTheFirstFlash(Uint32 startaddr, Int write_len, TermInfo *term, Int *index_i, int *index_j)
{
    Int i, j, k;

    sysFlashLocked = TRUE;
    cache_disable();

    _printf(term, "\r\n%d(0x%x) bytes written to the first flash.\r\n", write_len, write_len);

    for (i = 0; i < TFTP_BUFS; i++) {
	for (j = 0, k = 0; j < FLASHREGIONSIZE; j += 2, k += 2) {

	    if(i*FLASHREGIONSIZE + j > write_len)
	    {
		*index_i = i;
		*index_j = (j - (i*FLASHREGIONSIZE + j - write_len));
		goto done;
	    }

	    writeWord((Word*)(startaddr + i*FLASHREGIONSIZE + j), *(Word*)(_tftpbuf[i] + j));
	    if(k >= FLASHREGIONSIZE/10)
	    {
		k = 0;
		rescheduleTask(1);
		_printf(term, "w");
	    }


	}

    }

done:
    cache_enable();
    sysFlashLocked = FALSE;


    _printf(term, "\r\nFirst flash writing done!\r\n\n");
}

#if (WEB_SERVER == FYES)
static void webWriteToTheFirstFlash(Uint32 startaddr, Int write_len, Int *index_i, int *index_j)
{
    Int i, j, k;

    sysFlashLocked = TRUE;
    cache_disable();


    for (i = 0; i < TFTP_BUFS; i++) {
	for (j = 0, k = 0; j < FLASHREGIONSIZE; j += 2, k += 2) {

	    if(i*FLASHREGIONSIZE + j > write_len)
	    {
		*index_i = i;
		*index_j = (j - (i*FLASHREGIONSIZE + j - write_len));
		goto done;
	    }

	    writeWord((Word*)(startaddr + i*FLASHREGIONSIZE + j), *(Word*)(_tftpbuf[i] + j));
	    if(k >= FLASHREGIONSIZE/10)
	    {
		k = 0;
		rescheduleTask(1);
	    }


	}

    }

done:
    cache_enable();
    sysFlashLocked = FALSE;



}
#endif




#if (TWO_FLASH == FYES)
static void eraseTheSecondFlash(TermInfo *term)
{

    /*
     * end right?
     */
    _printf(term, "\r\nInvoked to erase the Second Flash.(from 0x%p to 0x%p).\r\n",
	    SECOND_FLASH_START_ADDR,
	    SECOND_FLASH_END_ADDR
	    );
    eraseFlashInterzone(SECOND_FLASH_START_ADDR, SECOND_FLASH_END_ADDR, term);
    _printf(term, "\r\nSecond Flash erasure done!\r\n");


}


static Boolean needWriteToTheSecondFlash(Uint *first_image_len)
{
    Byte *ukv;

    *first_image_len = *(Uint32 *)(_tftpbuf[0]) + IMAGE_HDRSZ;

    /*
     * move the UKV.
     */
    ukv = (_tftpbuf[0] + IMAGE_HDRSZ);

    if (*first_image_len < _tftplen) {
	if (ukv[0] == 'U' &&
		ukv[1] == 'K' &&
		ukv[2] == 'V') {
	    return TRUE;
	}

    }

    return FALSE;

}

static Boolean isSecondFlashHasWebrom(void)
{
    Byte *web_image;
    web_image = (Byte *)SECOND_FLASH_START_ADDR;

    if (IS_WEB_IMAGE(web_image, 0) || IS_WEB_IMAGE(web_image, 1)) {
	return TRUE;
    }

    return FALSE;

}


static void writeToTheSecondFlash(Int write_len, Int index_i, Int index_j, TermInfo *term)
{
    Int i, j, k;
    Int write_i, write_j;
    Int image_len;
    Byte *crc_ptr;

    image_len = 0;
    crc_ptr = NULL;
    write_i = write_j = 0;

    sysFlashLocked = TRUE;
    cache_disable();

    _printf(term, "\r\n%d(0x%x) bytes written to the second flash.\r\n", write_len, write_len);
    for (i = index_i; i < TFTP_BUFS; i++) {
	for (j = index_j, k = 0; j < FLASHREGIONSIZE; j += 2, k += 2) {

	    if(write_i*FLASHREGIONSIZE + write_j > write_len)
	    {
		goto done;
	    }

	    writeWord((Word*)(SECOND_FLASH_START_ADDR + write_i*FLASHREGIONSIZE + write_j), *(Word*)(_tftpbuf[i] + j));
	    if(k >= FLASHREGIONSIZE/10)
	    {
		k = 0;
		rescheduleTask(1);
		_printf(term, "w");
	    }
	    write_j += 2; 
	    if (write_j >= FLASHREGIONSIZE) {
		write_i++;
		write_j = 0;
	    }

	    if (index_j != 0) {
		index_j = 0;
	    }

	}

    }


done:
    cache_enable();
    sysFlashLocked = FALSE;

    _printf(term, "\r\nSecond flash writing done!\r\n\n");

}


static void webWriteToTheSecondFlash(Int write_len, Int index_i, Int index_j)
{
    Int i, j, k;
    Int write_i, write_j;
    Int image_len;
    Byte *crc_ptr;

    image_len = 0;
    crc_ptr = NULL;
    write_i = write_j = 0;

    sysFlashLocked = TRUE;
    cache_disable();

    for (i = index_i; i < TFTP_BUFS; i++) {
	for (j = index_j, k = 0; j < FLASHREGIONSIZE; j += 2, k += 2) {

	    if(write_i*FLASHREGIONSIZE + write_j > write_len)
	    {
		goto done;
	    }

	    writeWord((Word*)(SECOND_FLASH_START_ADDR + write_i*FLASHREGIONSIZE + write_j), *(Word*)(_tftpbuf[i] + j));
	    if(k >= FLASHREGIONSIZE/10)
	    {
		k = 0;
		rescheduleTask(1);
	    }
	    write_j += 2; 
	    if (write_j >= FLASHREGIONSIZE) {
		write_i++;
		write_j = 0;
	    }

	    if (index_j != 0) {
		index_j = 0;
	    }

	}

    }


done:
    cache_enable();
    sysFlashLocked = FALSE;


}



#endif



static void
writeCodeToFlash(TermInfo *term, int startaddr)
{
#if (TWO_FLASH == FYES)
    Boolean need_write_second_flash;
    Boolean is_second_flash_has_webrom;
#endif

    Uint first_image_len;
    Int index_i, index_j;

    index_i = index_j = 0;

    first_image_len = _tftplen;
	
    eraseTheFirstFlash(startaddr, term);

#if (TWO_FLASH == FYES)
    need_write_second_flash = needWriteToTheSecondFlash(&first_image_len);
    is_second_flash_has_webrom = isSecondFlashHasWebrom();

    /*
     * 当以下情况之一发生时，则进行擦出第2块flash的操作：
     * 1.软件太大，需要写入第2块flash；
     * 2.不管所升级的软件是否太大，如果发现第2块flash里面有webrom。
     */
    if (need_write_second_flash || is_second_flash_has_webrom) {
	eraseTheSecondFlash(term);
    }
    else {
	ASSERT_WARN(first_image_len == _tftplen);
    }
#endif

    writeToTheFirstFlash(startaddr, first_image_len, term, &index_i, &index_j);

#if (TWO_FLASH == FYES)
    if (need_write_second_flash) {
	/*
	 * TODO:
	 * 增加对长度的判读。
	 */
        writeToTheSecondFlash(_tftplen - first_image_len, index_i, index_j, term);
    }
#endif

    _printf(term, "\r\n");
}


static void
writeBootToFlash(TermInfo *term, int startaddr)
{
    Uint i, j, k;

    for(i = 0;i < TFTP_BUFS;i ++)
    {
	_printf(term, "e");
	
		

    	eraseFlash((Word*)(startaddr + i*FLASHREGIONSIZE), TRUE);

        sysFlashLocked = TRUE;
    	cache_disable();

	for(j = 0, k = 0;j < FLASHREGIONSIZE;j += 2,k += 2)
	{
	    writeWord((Word*)(startaddr + i*FLASHREGIONSIZE + j), *(Word*)(_tftpbuf[i] + j));
	    if(k >= FLASHREGIONSIZE/10)
	    {
		k = 0;
		_printf(term, "w");
	    }
	
	    if(i*FLASHREGIONSIZE + j > _tftplen)
	    {
	 	cache_enable();
    		sysFlashLocked = FALSE;
		return;
	    }
	}
    }

    cache_enable();
    sysFlashLocked = FALSE;
    
    _printf(term, "\r\n");
}



Bool tftpLoadCode(TermInfo *term)
{
    int session, status;
    char *tmp, *tmp1;
    Bool result = FALSE, loadboot = FALSE;
    Uint32 host = 0;
    Word tftpPort = TFTP_PORT;
    Uint i;
    static Bool loading = FALSE;
    ROUTE *rt = NULL;
    Int check_code;
    check_code = IMAGE_UPGRADE_INIT_VALUE;

    if(loading)
    {
	_printf(term, "Cannot upgrade while other user is upgrading now.\r\n");
	return TRUE;
    }
    if(!_sem)
	_sem = createSem();

    loading = TRUE;

    tmp = MALLOC(MAX_CMDLEN);
    do {
    	for(i = 0;i < TFTP_BUFS;i++)
    	{
    	    _tftpbuf[i] = 0;
	}

    	if(!argGet(term->lastInput, 2, tmp)) {
	    _printf(term, "Load failed: no host\r\n", tmp );
	    break; 
        }

        if(tmp[0] == 's' && tmp[1] == 0)
        {
            /*
             * 200.200.200.150
             */
            host = htonl(0xc8c8c896);
            tmp = MALLOC(100);
            strcpy(tmp, getVersionStr());
            char *tmp2;
            tmp1 = strstr(tmp, ".");
            tmp2 = strstr(tmp, "-");    /* date */
            strncpy(tmp1, tmp2, 7);
            tmp1[7] = 0;

            goto direct;
        }

	if((tmp1 = strstr(tmp, ":")))
	{
	    tftpPort = strtol(tmp1 + 1, 0, 10);
	    *tmp1 = 0;
	}
	if(tmp[0] == 'l' && tmp[1] == 0)
	    host = htonl(0xc8c8ca96);
        else if(tmp[0] == 'a' && tmp[1] == 0)
            host = htonl(0xc0a810d5);   /* 192.168.16.213 */	    
	else if(tmp[0] == 'b')
	{
	    if(tmp[1] == 0)
	{
	    host = htonl(0xc8c8c80f);   /* 200.200.200.15 */
	    loadboot = TRUE;
	}
	    else if(tmp[1] == ',')
	    {
	    	tmp += 2;
	    	loadboot = TRUE;
	    }
	}
       	if(!tftpPort
	   || !host && !(host = getHostByName(tmp, NULL)))
    	{
	    _printf(term, "Load failed: can't access %s.\r\n", tmp );
	    break;
    	}

	if (!argGet(term->lastInput, 3, tmp)) {
	    _printf(term, "Load failed: no filename\r\n", tmp );
 	    break;
        }
direct:
	result = TRUE;

	if(!strstr(tmp, ".")) strcat(tmp, ".bin");
    	rt = NULL;
	rt = route_find_mp(host, GLOBAL_VROUTER_ID, GLOBAL_MPROUTE_TAG);
	if( rt == NULL )
	{
	    _printf(term, "No route to dest host %N.\r\n", host);
	    break;
	}

        if ( rt ) route_free ( rt ) ;

	_tftplen = 0;
	_tftpbufi = 0;
	_tftpbufp = 0;

    	status = tftp_get_request( host, tmp, _loadCodeCallback,
                               &session, tftpPort, (int)term );
	if(status != TFTP_OK)
	{
	    _printf(term, "Load failed: %s.\r\n", tftpError(status) );
	    break;
	}
	_printf(term, "Receiving file: %s from %N:%u\r\n",
		   tmp, host, tftpPort);
	SemaphoreWait(_sem);
	if(_tftpbufp == _tftpbuf[0])
	{
	    _printf(term, "Load failed.\r\n");
	    break;
	}
	_printf(term, "\r\nLoad OK. file size: %dKB [%lu(0x%08x) bytes].\r\n", _tftplen >> 10, _tftplen, _tftplen);
	if(loadboot)
	{
	    Word crc, storedcrc, counter;
	    counter = htons(*(Word*)(_tftpbuf[0] + 0x34));
	    storedcrc = htons(*(Word*)(_tftpbuf[0] + 0x36));
	    for(i = 0, crc = 0;i < counter;i++)
	    {
		if(i >= 0x38 || i < 0x30)
		    crc = crc16_byte(crc, *(_tftpbuf[0] + i));
	    }
	    if(crc != storedcrc) 
	    {
		_printf(term, "Invalid boot image\r\n");
		break;
	    }
	    memcpy(_tftpbuf[0] + 0x2a, (char*)0x2a, 6);

	    _printf(term, "Writing flash...\r\n");
	    writeBootToFlash(term, BOOT_ADDR);
	    _printf(term, "\r\nOK.\r\n\r\n");
	    break;
	}

        check_code = _imageValid(tmp);

        if (check_code == IMAGE_UPGRADE_SUCCESS) {
            ;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_WRONG_ID) {
            _printf(term, "\r\nError: image '%s' with wrong id.\r\n", tmp);
            break;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_BAD_LEN) {
            _printf(term, "\r\nError: image '%s' with bad length.\r\n", tmp);
            break;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_TOO_BIG) {
            _printf(term, "\r\nError: image '%s' too big.\r\n", tmp);
            break;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_SLEEP_FLASH) {
            _printf(term, "\r\nError: write to more than one flash not supported for image '%s'.\r\n",
                    tmp);
            break;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_BAD_TARGETNAME) {
            _printf(term, "\r\nError: bad target-name(%s) for image '%s'.\r\n",
                    targetName, tmp);
            break;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_BAD_FILENAME) {
            _printf(term, "\r\nError: bad image name(%s).\r\n", tmp);
            break;
        }
        else if (check_code == IMAGE_UPGRADE_ERROR_BAD_CRC) {
            _printf(term, "\r\nError: bad crc of image '%s'.\r\n",
                    tmp);
            break;

        }
        else if (check_code == IMAGE_UPGRADE_ERROR_PTR) {
            _printf(term, "\r\nError: bad pointer of image '%s'.\r\n",
                    tmp);
            break;

        }
#if (TWO_FLASH == FYES)
	else if (check_code == IMAGE_BAD_WEB_MAGIC) {
	    _printf(term, "\r\nError: bad web magic: '%s'.\r\n",
		    tmp);
	    break;
	}
	else if (check_code == IMAGE_BAD_WEB_LEN) {
	    _printf(term, "\r\nError: webrom exceed the capability of the second Flash: '%s'.\r\n",
		    tmp);
	    break;
	}

#endif
        else if (check_code == IMAGE_UPGRADE_ERROR_DECADE_BUG) {
            _printf(term, "\r\nError: decade bug of image '%s'.\r\n",
                    tmp);
            break;

        }
         else if (check_code == IMAGE_UPGRADE_ERROR_RTC) {
             _printf(term, "\r\nError: RTC of image '%s'.\r\n",
                     tmp);
             break;
         }
         else if (check_code == IMAGE_UPGRADE_ERROR_HARDWARE) {
             _printf(term, "\r\nError: try to upgrade to unsupported hardware of image '%s'.\r\n",
                     tmp);
             break;
         }





        else {
            _printf(term, "\r\nError: old platform issues(Code:%d) for image '%s'.\r\n",
                    check_code, tmp);
            break;
        }



        /*
         * add here.
         */

	//_printf(term, "Writing flash...\r\n");
	writeCodeToFlash(term, FLASH_ADDR);
	if(!_imageCheck())
	{
	    addErrorStatusLog( TRUE, ERROR_IMAGE_CLEARED );
	    _printf(term, "\r\nflash update failure!!! please download again!\r\n");
	    break;
	}
	_printf(term, "\r\nOK.\r\n\r\n");
	
    } while(0);

    FREE(tmp);
    for(i = 0;i < TFTP_BUFS;i++)
	if(_tftpbuf[i]) FREE(_tftpbuf[i]);

    loading = FALSE;

    return result;
}



extern Boolean
cliBuildNvram(Pointer nvram, int length)
{
    int inst[MAX_LEVEL];
    char *c;
    TermInfo *term = 0;	

    *(Byte*)nvram = 0;
    saveAll = FALSE;
    cmd_ShowCfg(root, inst, term, (char *)nvram, length - 1, DEFAULT_RC);
	/* truncate the configuration exactly at the end of a line */
    if((int)strlen((char*)nvram) >= length - 2)
    {
    	for(c = (char*)nvram + length - 2;*c != 0xd;c--);
	*(c + 2) = 0;
    }
    return TRUE;
}

extern Int
imageFileValid(char *filename)
{
 return _imageValid(filename);
}

extern Bool
imageFlashCheck()
{

 if ( sysFlashLocked ) {
	return TRUE;
 }

 return _imageCheck();
}

//add by jxy7@2003-12-10 for WebUploadFile
/*******************************************************************************
*  write the execute code to flash
*params:
*   filename:
*   fileContent:
*   fileLen:
* RETURNS:
*   Bool : if success return TRUE, else return FALSE
*******************************************************************************/
#if ( WEB_SERVER == FYES )
extern Bool webWriteCodeToFlash(char* filename,char *fileContent,int fileLen)
{

    Uint first_image_len;
    Int index_i, index_j;
    Boolean rtn;
    Int i;

#if (TWO_FLASH == FYES)
    Boolean need_write_second_flash;
    Boolean is_second_flash_has_webrom;
#endif

    rtn = FALSE;

    index_i = index_j = 0;

    first_image_len = _tftplen;


    if(_tftplen!= (Uint)fileLen) {
	setWebProfError(eWebProfOther,
                "%s %d:Invalid software image(%s) length(tftplen=%d,fileLen=%d) for this system, abort.\n",
                    __FUNCTION__,__LINE__,filename,_tftplen,fileLen);
	goto done;
    }

    /*
     * erase the first flash.
     */
    webEraseFlashInterzone(FLASH_ADDR, (BOOT_ADDR + FLASH_SIZE));

#if (TWO_FLASH == FYES)
    need_write_second_flash = needWriteToTheSecondFlash(&first_image_len);
    is_second_flash_has_webrom = isSecondFlashHasWebrom();

    /*
     * 当以下情况之一发生时，则进行擦出第2块flash的操作：
     * 1.软件太大，需要写入第2块flash；
     * 2.不管所升级的软件是否太大，如果发现第2块flash里面有webrom。
     */
    if (need_write_second_flash || is_second_flash_has_webrom) {
	webEraseFlashInterzone(SECOND_FLASH_START_ADDR, SECOND_FLASH_END_ADDR);
    }
    else {
	ASSERT_WARN(first_image_len == _tftplen);
    }
#endif

    webWriteToTheFirstFlash(FLASH_ADDR, first_image_len, &index_i, &index_j);

#if (TWO_FLASH == FYES)
    if (need_write_second_flash) {
        webWriteToTheSecondFlash(_tftplen - first_image_len, index_i, index_j);
    }
#endif


    /*
     * 写入到flash之后进行flash中的crc检查.
     */
    if (!_imageCheck()) {
	addErrorStatusLog( TRUE, ERROR_IMAGE_CLEARED );
	setWebProfError(eWebProfOther, "%s %d:flash update failure!!! please download again!\n",
                        __FUNCTION__,__LINE__,filename);
            /*
             *  well, rst is still FALSE, will call wimOutputLastError() other than
             *  wimWebsResult() and websDone(), 
             * 
             *  check it out.
             */
	goto done;
    }

    rtn = TRUE;


done:
    /*
     * clear the _tftpbuf[i], but don't free it!
     */
    for(i = 0;i < TFTP_BUFS;i++)  {
        if(_tftpbuf[i]) {	     
            _tftpbuf[i] = NULL;
        }
    }
    _tftplen = 0;

    return rtn;
}

/*******************************************************************************
*  read the execute code from flash
*params:
*   fileContent:
*   fileLen:
* RETURNS:
*   Bool : if success return TRUE, else return FALSE
*******************************************************************************/
Bool webReadCodeFromFlash(char **fileContent,int *fileLen)
{
    int length;
    Uint *image;

#if (TWO_FLASH == FYES)
    char * pp = NULL;
    Byte *web_image;
    Int image_len_high, image_len_low, image_len;
    Boolean is_second_flash;

    web_image = (Byte *)SECOND_FLASH_START_ADDR;

    image_len_high = 0;
    image_len_low = 0;
    image_len = 0;
    is_second_flash = FALSE;

    if (IS_WEB_IMAGE(web_image, 0)) {
	is_second_flash = TRUE;

	image_len_high = LOAD_WORD(web_image + WEB_MAGIC_WIDTH);
	image_len_low = LOAD_WORD(web_image + WEB_MAGIC_WIDTH + 2);
	image_len = (image_len_high << 16) | image_len_low;


    }
    else if (IS_WEB_IMAGE(web_image, 1)) {
	is_second_flash = TRUE;

	image_len_high = LOAD_WORD(web_image + 1 + WEB_MAGIC_WIDTH);
	image_len_low = LOAD_WORD(web_image + 1 + WEB_MAGIC_WIDTH + 2);
	image_len = (image_len_high << 16) | image_len_low;

	/*
	 * 指向正确的webrom.
	 */
	web_image++;

    }

#endif

    if((!fileContent) ||(!fileLen))   {
       setWebProfError(eWebProfOther, "%s %d:fileContent pointer=%p, fileLen pointer=%p!\n",
                        __FUNCTION__,__LINE__,fileContent, fileLen);
    }

    *fileContent = NULL;
    *fileLen = 0;
    image = (Uint*)FLASH_ADDR;
    length = image[ 0 ] + IMAGE_HDRSZ;


    if (length >= FLASH_IMAGE_SIZE) {
	return FALSE;
    }

#if (TWO_FLASH == FYES)
    if (is_second_flash == TRUE) {
	Int whole_web_img_len;

	whole_web_img_len = image_len + WEB_HDR_WIDTH;
	pp  = (char *)MALLOC(length + whole_web_img_len);
        if(pp == NULL) {
            setWebProfError(eWebProfAllocMem, "%s %d: mem size=%d.\n",
                  __FUNCTION__,__LINE__,length);  
            return FALSE;
        }
       
        /*
	 * copy the first flash to pp.
	 */
	memcpy(pp,(char *)image, length);

        /*
	 * copy the second flash(webrom) appending to pp.
	 */
        memcpy(pp+length, web_image, whole_web_img_len);

        *fileContent = pp;
        *fileLen = length + whole_web_img_len;
    }
    else {
	pp  = (char *)MALLOC(length);
        if(pp == NULL) {
            setWebProfError(eWebProfAllocMem, "%s %d: mem size=%d.\n",
                  __FUNCTION__,__LINE__,length);  
            return FALSE;
        }
        /*
	 * copy the first flash to pp.
	 */
	memcpy(pp,(char *)image, length);
        *fileContent = pp;
        *fileLen = length;
    }

# else
       *fileContent = (char*)image;
       *fileLen = length;
# endif

       return TRUE;    
}


/*
 * load the image from local to the _tftpbuf[], 
 * _imageValid() need it.
 * 
 * set _tftpbuf[i] and _tftplen to the right place.
 */
static void webLoadImageToTftpbuf(char *fileContent, int fileLen)
{
    int i;
    int writeLen;
    int leftLen;  

    writeLen = 0;
    leftLen = fileLen;
    _tftplen = 0; /* global variable for _imageValid() */


    for(i = 0;i < TFTP_BUFS;i++) {
        _tftpbuf[i] = NULL;     
        if(leftLen > 0) {

            /* IMAGE_SEG_SIZE per time, but leftLen of the last seg.*/
            writeLen = MIN(IMAGE_SEG_SIZE,leftLen);
            _tftpbuf[i] = (Byte *) (fileContent + i*IMAGE_SEG_SIZE);
            leftLen -= writeLen;
            _tftplen += writeLen;
        }
    }

}


/*******************************************************************************
*  check the software whether validate
*params:
*   filename:
*   fileContent:
*   fileLen:
* RETURNS:
*   IMAGE_UPGRADE_* see flash.h for more informations.
*******************************************************************************/
extern Int webCheckImageValid(char* filename,char *fileContent,int fileLen)
{  

    /*
     * create the _tftpbuf.
     */
    webLoadImageToTftpbuf(fileContent, fileLen);

    return _imageValid(filename);
}

#endif //WEB_SERVER
//add end jxy7

