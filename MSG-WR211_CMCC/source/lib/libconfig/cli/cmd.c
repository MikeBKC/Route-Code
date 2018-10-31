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
 * SCCS ID:	$Id: cmd.c,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/


#include "typedef.h"
#include "profiles.h"
#include "argcmd.h"
#include "mibsys1.h"
#include "stdio.h"
#include "cmd.h"

static TermCmd *_cmdParent;
static unsigned int _cmdLevel;
Bool first;

static Boolean nomore;

#  if defined( CLI_CODE_DEBUG ) 

int  far _cliDebugDumping ;

    /*
     * _cliDebugCmd:
     *
     * function to to toggle run-time debug output
     */
static void
_cliDebugCmd( void )
{
    _cliDebugDumping = ! _cliDebugDumping;
    printf( "CLICMD: debug is now %s\n", _cliDebugDumping ? "ON" : "OFF" );
}

static void
_cliTestCmd ( void )
{
}
# endif 

TermCmd *root;
Boolean saveAll;

unsigned long MaxTermCmd = 0;			/* O for root */
unsigned long MaxTermLevel = 0;			/* 0 for root */

#if 0
PoolQ *cmdPool = 0;
PoolQ *termPool = 0;
#endif

void cmdRoot(TermInfo *term);

void cmdDefault(TermInfo *term)
{
    TermCmd *cmd = GETCMD;

    INITLEAF;
    char tmp[MAX_CMDLEN];
    void (*viewfunc)(LcdTerminalHdl) = 0;
    void (*viewfunc1)(LcdTerminalHdl, char *) = 0;
    
    
    DOSHOW
    {
     if( !(cmd->cmdlist->privilege & CMD_HIDDEN) ) {
	if(cmd->cmdlist->privilege & CMD_VIEWSTAT)
	{
	    if((int)cmd->cmdlist->enum1 == 1)
	    {
	    	argGet(term->lastInput, cmd->level + 2, tmp);
	    	viewfunc1 = (void (*)(LcdTerminalHdl, char *))cmd->cmdlist->offset;
	    	if(viewfunc1) {
                    viewfunc1(term->lcdTerm, tmp);
                }
	    }
	    else
	    	viewfunc = (void (*)(LcdTerminalHdl))cmd->cmdlist->offset;
	    	if(viewfunc) {
                    viewfunc(term->lcdTerm);
                }
	}
	else
	{
	    prints(term, TRUE, "%s :", cmd->cmdlist->name);
	    cmd_GetValueStr(cmd, term->inst, tmp);
	    _printf(term, "%s\r\n", tmp);
    	    CLIPRINTF(("%s:SHOW type:inst:offset:psize:itype: %d:%d:%d:%d:%d\n",
			__FUNCTION__,
			cmd->cmdlist->profType,
			*(term->inst),
			cmd->cmdlist->offset,
			cmd->cmdlist->psize,
			cmd->cmdlist->itype
	    ));
	}
      } /* HIDDEN */
      else 
      {
	CLIPRINTF(("%s: attemp to SHOW a HIDDEN item\n", __FUNCTION__));
      }
    }

    DOSET
    {
      if ( !(cmd->cmdlist->privilege & CMD_HIDDEN) ) {
	if( cmd->cmdlist->privilege & CMD_W )  {
            int ret = 0 ;
	    ret = profSetValue(cmd->cmdlist->profType, term->inst, cmd->cmdlist->offset, cmd->cmdlist->psize, cmd->cmdlist->itype, cmd->cmdlist->enum1, term, cmd->level + 2) ;
	    CLIPRINTF(("%s:SET type:inst:offset:psize:itype: %d:%d:%d:%d:%d\n",
			__FUNCTION__,
			cmd->cmdlist->profType,
			*(term->inst),
			cmd->cmdlist->offset,
			cmd->cmdlist->psize,
			cmd->cmdlist->itype
	    ));
	   if (ret == FAIL2_CLI )
		ERRMSG_NVRAM_FULL; 
        } /* CMD_W */
      } /* HIDEN */ 
      else 
      {
	CLIPRINTF(("%s: attemp to SET a HIDDEN item\n", __FUNCTION__));
      }
    }

    DOMORE
    {
     if( !(cmd->cmdlist->privilege & CMD_HIDDEN) ) {
	if(cmd->cmdlist->privilege & CMD_VIEWSTAT)
	{
	    if((int)cmd->cmdlist->enum1 == 1)
	    {
	    	argGet(term->lastInput, cmd->level + 2, tmp);
	    	viewfunc1 = (void (*)(LcdTerminalHdl, char *))cmd->cmdlist->offset;
	    	if(viewfunc1) {
                    lcdTermSetMoreFlag(term->lcdTerm, TRUE);
                    viewfunc1(term->lcdTerm, tmp);
                    lcdTermSetMoreFlag(term->lcdTerm, FALSE);
                }
	    }
	    else
	    	viewfunc = (void (*)(LcdTerminalHdl))cmd->cmdlist->offset;
	    	if(viewfunc) { 
                    lcdTermSetMoreFlag(term->lcdTerm, TRUE);
                    viewfunc(term->lcdTerm);
                    lcdTermSetMoreFlag(term->lcdTerm, FALSE);
                }
	}
	else
	{
	    prints(term, TRUE, "%s :", cmd->cmdlist->name);
	    cmd_GetValueStr(cmd, term->inst, tmp);
	    _printf(term, "%s\r\n", tmp);
    	    CLIPRINTF(("%s:SHOW type:inst:offset:psize:itype: %d:%d:%d:%d:%d\n",
			__FUNCTION__,
			cmd->cmdlist->profType,
			*(term->inst),
			cmd->cmdlist->offset,
			cmd->cmdlist->psize,
			cmd->cmdlist->itype
	    ));
	}
      } /* HIDDEN */
      else 
      {
	CLIPRINTF(("%s: attemp to SHOW a HIDDEN item\n", __FUNCTION__));
      }
    }


}

void cmd_Add(TermCmd *parent, TermCmd *newson)
{
    TermCmd *son;
    
    son = parent->sonHead;
    if(!son) 
	parent->sonHead = newson;
    else 
    {  
    /* maybe we can do sort at this point, so that we can re-arrange the CLI 
    */     
#define CMD_SORT      
#undef CMD_SORT      

#ifdef CMD_SORT
      while ( son->next ) { 
        CLIPRINTF(("son: %s,  newson: %s\n", son->cmdlist->name, newson->cmdlist->name));
        if ( strcmp( newson->cmdlist->name, son->cmdlist->name ) > 0 ) {
          CLIPRINTF(("son: %s, < newson: %s\n", son->cmdlist->name, newson->cmdlist->name));
          break ;
        }  
	son = son->next ;
       }	  
     tmp = son->next;
     son->next = newson;
     newson->next = tmp ;
       
#else 	
     while(son->next) son = son->next; 
     son->next = newson;
#endif
   }
}

void cmd_CmdStr(TermCmd *cmd, int *inst, char *buf)
{
    char tmp[1024];
    *buf = 0;
    while(cmd != root)
    {
	_strlcat(buf, " ");
	if(cmd->cmdlist->privilege & CMD_I)
	{
	    cmd_GetInstName(cmd, inst, tmp);
	    _strlcat(buf, tmp);
	    _strlcat(buf, (char *)"/");
	}
	_strlcat(buf, cmd->cmdlist->name);
	cmd = cmd->parent;
    }
}


TermCmd *cmd_Create( TermCmd *cmd_parent, int _level, CmdList *cmd )
{
    TermCmd *newterm = NULL;

#if 0
    newterm = GETBUFF(TermCmd, termPool);
#endif
    newterm = (TermCmd*)MALLOC(sizeof(TermCmd));
    ASSERT(newterm);
    memset(newterm, 0, sizeof(TermCmd));
    
    newterm->parent = cmd_parent;
    newterm->sonHead = 0;
    newterm->next = 0;
  
    newterm->attribute = 0;
    newterm->level = _level;
    
    newterm->cmdlist = cmd ;
    
    if(newterm->parent) 
      cmd_Add(newterm->parent, newterm);
    
    newterm->id = MaxTermCmd ;  /* 0 is for root, 1 is first user cmd */
    MaxTermCmd ++ ;

    return newterm;
}

void cmd_DispCfg(TermCmd *termcmd, TermInfo *term)
{
    termcmd = termcmd->parent;
    term->curLevel = termcmd->level;
    cmd_Enum(termcmd, term);
}
    
void cmd_Enum(TermCmd *termcmd, TermInfo *term)
{
#if 0
    char *tmp = NULL;
    char *buf = NULL;
#endif
    char tmp[1024];
    char buf[1024];
    
    CmdList *cmd = termcmd->cmdlist ;
    
#if 0
    tmp = GETBUFF(char, cmdPool);
    ASSERT(tmp);
    buf = GETBUFF(char, cmdPool);
    ASSERT(buf);
#endif
    term->cmd = termcmd;
    _strcpy(buf, cmd->name);
    if(cmd->privilege & CMD_I)
    {
		
	cmd_GetInstName(termcmd, term->inst, tmp);
	_strcat(buf, (char *)"/");
	_strcat(buf, tmp);
    }
    prints(term, FALSE, "%s\r\n", buf);
#if 0
    FREEBUFF(cmdPool, buf);
#endif
    for(termcmd = termcmd->sonHead;termcmd;termcmd = termcmd->next)
    {
	if(!(termcmd->cmdlist->privilege & (CMD_VIEWSTAT|CMD_INSTANCE)))
	{
	    term->cmd = termcmd;
	    termcmd->cmdlist->func(term);
	}
    }
#if 0
    FREEBUFF(cmdPool, tmp);
#endif
}

void cmd_EnumInst(TermCmd *cmd, TermInfo *term)
{
    TermCmd *cmd1;
    int index;
    char tmp[1024];
    int inst[MAX_LEVEL];

    for(index = 1;;index++)
    {
	inst[cmd->level] = index;
	if(cmd_GetInstName(cmd, inst, tmp) == FAIL_CLI) break;
	prints(term, FALSE, "%s/%s\r\n", cmd->cmdlist->name, tmp);
	for(cmd1 = cmd->sonHead;cmd1;cmd1 = cmd1->next)
	{
	    term->cmd = cmd;
	    cmd->cmdlist->func(term);
	}
    }
}

int cmd_GetInstName(TermCmd *cmd, int *inst, char *buf)
{
    TermInfo term;
    
    term.userPtr = (int)buf;
    term.verb = VERB_SHINST;
    term.inst = inst;
    term.cmd = cmd;
    cmd->cmdlist->func(&term);
    return term.result;
}

TermCmd *cmd_GetNext(TermCmd *cmd)
{
    if(cmd->sonHead)
	return cmd->sonHead;
    else
    if(cmd->next)
	return cmd->next;
    else
	while(cmd != root)
	    if(cmd->parent->next)
		return cmd->parent->next;
	    else
		cmd = cmd->parent;
    return cmd;
}

Boolean cmd_IsDefaultValue(TermCmd *cmd, int *inst)
{
    Pointer prof;
    static Pointer defProf = 0;
    static MibProfileType lastType = MIB_PROF_ERROR;
    static int lastinst = -1;
    static int lastslot = -1;
    SlotNumber slot;
//    RouteProfile *routeProf;
    Boolean result;
    int instindex;
    bool bflag = TRUE;

    if(saveAll) return FALSE;
    
    wuProfInitParam(cmd->cmdlist->profType, inst, &instindex, &slot);

    if(lastType != cmd->cmdlist->profType || instindex != lastinst || slot.slotNumber != lastslot)
    {
    	bflag = BuildDefaultFactoryValue(cmd->cmdlist->profType, instindex,slot, &defProf);
	lastType = cmd->cmdlist->profType;
	lastinst = instindex;
  	lastslot = slot.slotNumber;
    }
    
    if(cmd->cmdlist->privilege & CMD_C)
    {    
    	if(bflag) return TRUE;
	return FALSE;
    }
    if(cmd->cmdlist->privilege & CMD_W)
    { 
	if(cmd->cmdlist->profType == MIB_PROF_INTERNET && !strcmp(cmd->cmdlist->name, "direction")
	   && inst[1] == 0)
	    return TRUE;
    	prof = _profRead(cmd->cmdlist->profType, inst);
	if(!prof) return FALSE;

    	result = memcmp((Byte*)prof + cmd->cmdlist->offset, (Byte*)defProf + cmd->cmdlist->offset, cmd->cmdlist->psize) ?  FALSE : TRUE ;
	releaseProfilePointer(cmd->cmdlist->profType, prof);
	return result;
    }
    return FALSE;
}


void cmd_GetValueStr(TermCmd *cmd, int *inst, char *buf)
{

    switch(cmd->cmdlist->itype)
    {
	case INT:
	    sprintf(buf, "%d", profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize));
	    break;

	case GROUP:
	    sprintf(buf, "%d", profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize) & 0x7fff);
	    break;

	case STRNUM:
	case STR:
	case MACADDR:
	    snprintf(buf, MAX_CMDLEN-2, "%s", profGetStr(cmd->cmdlist->profType, inst, cmd->cmdlist->offset));
	    break;
	
	/* add by hqguan*/
	case MACADDR2:
	    snprintf(buf, MAX_CMDLEN-2, "%s", profGetMac(cmd->cmdlist->profType, inst, cmd->cmdlist->offset));
	    break;

	/* add by zhz*/
	case HEX:
	    snprintf(buf, MAX_CMDLEN-2, "%s", profGetHex(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize));
	    break;

	case IPADDR:
	    sprintip(buf, profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize));
	    break;

	case ENUM:
	    snprintf(buf, MAX_CMDLEN-2, "%s", enumString((LcdParamEnumValue *)cmd->cmdlist->enum1, profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize)));
	    break;

	case DATETIME:
	    sprintdatetime(buf, profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize));
	    break;

	case TIME:
	    sprinttime(buf, profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize));
	    break;

	case PASSWD:
	    sprintPasswd(buf, cliProfGetPasswd(cmd->cmdlist->profType, inst, cmd->cmdlist->offset));
	    break;

	case ENUMINT:
	    sprintEnumInt(buf, (LcdParamEnumIntValue *)cmd->cmdlist->enum1, profGetInt(cmd->cmdlist->profType, inst, cmd->cmdlist->offset, cmd->cmdlist->psize));
	    break;

	default:
	    break;
    }
    CLIPRINTF(("%s: inst %ld, out: %s\n", __FUNCTION__, *inst, buf));
}

/* build the CLI tree by depth firt */

void cmd_InitCommon(CmdList *cmdlist)
{
    TermCmd *tmpcmd, *tmpparent;
    int i ;

      /* if root is not set, It must be a error conidtion */
    if(!root) {
      printf("root not inited, halt\n");
      ASSERT(root);
    }	
   
    _cmdLevel ++;     /* 1 is first user cmd level */

  /* error condition */
    if ( _cmdLevel >= MAX_LEVEL ) {
	printf("too many levels, halt\n");
        ASSERT(0);
    }

    if ( MaxTermLevel < _cmdLevel )
      MaxTermLevel = _cmdLevel ;
   
  /* save current level's parent */
    tmpparent = _cmdParent ;
   
    if ( cmdlist ) {
      /* build level head and brothers*/
      for ( i=0; cmdlist->name; cmdlist++, i++ ) {
        if (cmdlist->privilege & CMD_HIDDEN) {
    	        CLIPRINTF(("%s:%d, skip hidden cmd %s\n", __FUNCTION__, __LINE__, cmdlist->name));
		continue;
	}
        tmpcmd = cmd_Create( _cmdParent, _cmdLevel, cmdlist );
        if ( cmdlist->helpInfo == NULL ) {
          tmpcmd->cmdlist->helpInfo = cmdlist->name;
        }  
        if( ! cmdlist->profType ) {
          tmpcmd->cmdlist->profType = _cmdParent->cmdlist->profType;
        }	     
        /* save current cmd level  */
        tmpparent = _cmdParent ;
	/* depth first, init son now */
	_cmdParent = tmpcmd ;
  	tmpcmd->cmdlist->func(0);
	/* restore saved cmd level, for next cmd at the same level */
        _cmdParent = tmpparent ; 	
      } /* for */			  
    } /* if */
    _cmdParent = tmpparent;
    _cmdLevel--;
}

static TermCmd *_sysRoot = NULL;
static char _cliRoot[] = "/" ;
static CmdList _rootList = { 	_cliRoot, 
  				cmdRoot, 
				CMD_WRITEFLASH|CMD_HANGUP|CMD_SAVE,
				0,
				0,
				(IType)0,
	                        (Pointer)0,
				(char*)0,
				(MibProfileType)0
			   } ;

int cmd_ListBranch(TermCmd *cmd, TermInfo *term, char *str, int _verb)
{
    TermCmd *cmd1;
    int len, i;
    
    if(noPrompt) {
        _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
	return 0;
    } 
    len = _strlen(str);
    i = FALSE;
    for(cmd1 = cmd->sonHead;cmd1;cmd1 = cmd1->next)
	if((len && !_strncmp(str, (char *)cmd1->cmdlist->name, len) || !len) && _verb << CMD_SHIFT & cmd1->cmdlist->privilege)
	{
	    i = TRUE;	    
//	    _printf(term, "  %s", cmd1->name);
	    _printf(term, CLI_LIST_FMT, cmd1->cmdlist->name, cmd1->cmdlist->helpInfo ? cmd1->cmdlist->helpInfo : "no help available");
	}
    if(i)
    {
    	_printf(term, "\r\n");
    }
    else
    {    
	_printf(term, "Not allowed.\r\n");
    }
    return i;
}


void strcatn(Astr *str, char *str1)
{
    int l, l1, l2;

    l = str->len;
    l1 = strlen((const char*)str1);
    if(l + l1 >= str->maxlen)
    {
	l2 = l1 - ( l + l1 - str->maxlen );
	strncpy(str->s + l, str1, l2 - 1);
	*(str->s + str->maxlen - 1) = 0;
	str->len = str->maxlen;
    }
    else
    {
	strcpy(str->s + l, str1);
	str->len += l1;
    }
}

	

void cmd_PrintSet(TermCmd *cmd, int *inst, TermInfo *term, Astr *str, int show_type)
{
    char tmp[1024]; 
    char tmp1[1024];

#if 0
    tmp = GETBUFF(char, cmdPool);
    ASSERT(tmp);
    tmp1 = GETBUFF(char, cmdPool);
    ASSERT(tmp1);
#endif
    tmp1[0] = 0;
    cmd_CmdStr(cmd, inst, tmp);
    _strlcat(tmp, "set ");
    cmd_GetValueStr(cmd, inst, tmp1);
    _strcat(tmp, tmp1);
    _strcat(tmp, (char *)"\r\n");
    if(str)
	strcatn(str, tmp);
    /*
     * mod by llliu@080424.
     */
    else {
        if (show_type == DEFAULT_RC || show_type == MORE_RC) {
            _printf(term, "%s", tmp);
        }
        else if ((show_type == SIMPLE_RC) && !(cmd->cmdlist->privilege & CMD_SIM)) {
            _printf(term, "%s", tmp);
        }
    }
} 

void cmd_traverse(TermCmd *cmd, int *inst, TermInfo *term, Astr *str, int show_type)
{
    TermCmd *cmd1, *cmd2;
    int i;
    char tmp[1024];
    char tmp1[1024];
    static int counter = 0;



    if ( (++counter > 10) 
// 	 && (profUpdate) 
     ) {
	counter = 0;
	rescheduleTask(1);
    }

#if 0
    tmp1 = GETBUFF(char, cmdPool);
    ASSERT(tmp1);
#endif

    for(cmd1 = cmd->sonHead;cmd1->level > cmd->level;cmd1 = cmd_GetNext(cmd1)) {
      if(cmd1->cmdlist->privilege & CMD_I)
      {
	for(i = instanceMin(cmd1->cmdlist->profType);i < instanceMax(cmd1->cmdlist->profType);i++)
	{
	    inst[cmd1->level] = i;
	    if(cmd_GetInstName(cmd1, inst, tmp1) == FAIL_CLI) continue;
	    if(cmd1->cmdlist->privilege & CMD_C)
	    if(!cmd_IsDefaultValue(cmd1, inst))
	    {
#if 0
    		tmp = GETBUFF(char, cmdPool);
    		ASSERT(tmp);
#endif
    		cmd_CmdStr(cmd1, inst, tmp);
    		if(str)
		{
		    strcatn(str, (char*)"new ");
		    strcatn(str, tmp);
		    strcatn(str, (char*)"\r\n");
		}
    		else {
			/*
			 * only printf privilege without CMD_SIM set.
			 */
			if ((show_type == DEFAULT_RC) )  {	/* mod by llliu */
		    		_printf(term, "new %s\r\n", tmp);
			}
                        else if (show_type == MORE_RC) {
                            if (nomore == TRUE || lcdTermMore(term->lcdTerm)) {
                                nomore = TRUE;
                                goto out;
                            }
                            _printf(term, "new %s\r\n", tmp);
                        }
			else if ((show_type == SIMPLE_RC) && !(cmd1->cmdlist->privilege & CMD_SIM)) {
		    		_printf(term, "new %s\r\n", tmp);
			}
                        else {
                            ;
                        }
		}
#if 0
		FREEBUFF(cmdPool, tmp);
#endif
	    }
	    cmd_traverse(cmd1, inst, term, str, show_type);
	}
	i = cmd1->level;
	while((cmd2 = cmd_GetNext(cmd1))->level > i)
	    cmd1 = cmd2; 
	inst[i] = 2;
      } else if(cmd1->cmdlist->privilege & CMD_W) {
	  if(!cmd_IsDefaultValue(cmd1, inst))
	  {
            if (show_type == MORE_RC) {
                if (nomore == TRUE || lcdTermMore(term->lcdTerm)) {
                    nomore = TRUE;
                    goto out;
                }
            }
	    cmd_PrintSet(cmd1, inst, term, str, show_type);
	  }
      } else  if(cmd1->cmdlist->privilege & CMD_VIEWSTAT) {
		;;;;;;;;;;;
	    	//cmd_PrintStat(cmd1, inst, term, str);
      } else  if(cmd1->cmdlist->privilege & CMD_CLEAR) {
		;;;;;;;;;;;
	    	//cmd_PrintClear(cmd1, inst, term, str);
      } else  if(cmd1->cmdlist->privilege & CMD_DEBUG) {
		;;;;;;;;;;;
	    	//cmd_PrintDebug(cmd1, inst, term, str);
      } else {
		;;;;;;;;;;;
      }
    } /* for */

out:
    return;
#if 0
  FREEBUFF(cmdPool, tmp1);
#endif
}


void cmd_ShowCfg(TermCmd *cmd, int *inst, TermInfo *term, char *str1, int len, int show_type)
{
    Astr str;

    if(str1)
    {
	str.maxlen = len;
	str.len = 0;
	str.s = str1;
    	cmd_traverse(cmd, inst, term, &str, show_type);
	CLIPRINTF(("str len = %d\n", str.len));
    }
    else if (show_type == MORE_RC) {
       	cmd_traverse(cmd, inst, term, NULL, show_type);
        nomore = FALSE;
    }
    else {

    	cmd_traverse(cmd, inst, term, NULL, show_type);
    }
}



void cliInit()
{
   first = TRUE;
   _cmdLevel = 0;
   _cmdParent = 0;
#if 0
   cmdPool = makeBuffPool( MAX_CMDLINE, 1, TRUE, 1, "cmdPool" );
   termPool = makeBuffPool( sizeof(TermCmd), 1, TRUE, 1, "TermCmdPool" );
#endif
   /* new a TermCmd for root */
   _sysRoot = cmd_Create ( _cmdParent, _cmdLevel, &_rootList );
   /* setup root */
   root = _sysRoot ;
   _cmdParent = root;
   /* do init */   
   _sysRoot->cmdlist->func(0);
   first = FALSE;

#    if defined( CLI_CODE_DEBUG ) 
    _cliDebugDumping = FALSE;
    mtMonitorCreateCommand( "cliCmdDebug", "toggle cli debug",
                            _cliDebugCmd, FALSE );

    mtMonitorCreateCommand( "cliTreeCmd", "cli tree stress test",
                            _cliTestCmd, FALSE );
#    endif

}

