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
 * SCCS ID:	$Id: cmd.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#if ! defined( CMD_H )                  /* prevent nested inclusions */
# define CMD_H                          /* module name */

//modify by jxy5@2003-11-26 for WebProfFrame, argcmd.h must include before mibsys1.h
#include"argcmd.h"
#include"mibsys1.h"


#define _INITBASE(a) if(first){ cmd_InitCommon(a); return;} 

#define INITBRANCHES    _INITBASE(&cmdlist[0])

#define INITLEAF    _INITBASE(0)

#define GETCMDLIST	((TermCmd *)term->cmd->cmdlist)

#define GETCMD		((TermCmd *)term->cmd)

#define LEFTMARGIN_0    term->curLevel = GETCMD->level

#define CMDEND {0, 0, 0, 0, 0, TNONE, 0, 0}

#define CMDTL 0, 0, TNONE, 0

#define P(x)  cmdDefault, P1(x)
#define P1(x) CMD_LEAF, P2(x)
#define P2(x) (int)&x, sizeof(x)

#define F(x)  cmdDefault, CMD_LEAFVST, (int)&x, 0, TNONE
#define F_MORE(x)  cmdDefault, CMD_LEAFVST_MORE, (int)&x, 0, TNONE

/* INITBRANCH will enum the daughters while INITBRANCHES won't */
#define INITBRANCH  INITBRANCHES; \
	    DOSHOW{ \
	    cmd_Enum(GETCMD, term); }
	    
#define INITBRANCHB INITBRANCHES; \
	    DOSHOW{ \
	    LEFTMARGIN_0; \
	    cmd_Enum(GETCMD, term); }   

#define INITBRANCHI INITBRANCHB; \
	    DOSHINST{ instanceShow(term); }\
	    DOVERIFY{ instanceVerify(term); }\
	    DONEW{ instanceCreate(term); }\
	    DODELETE{ instanceDelete(term); }\
		

#define INITBRANCHIBI INITBRANCHB; \
	    DOSHINST{ intInstShow(term); }\
	    DOVERIFY{ intInstVerify(term); }\

typedef struct
{
	const char *name;
	void (*func)(TermInfo*);
	int privilege ;
	int offset;
	int psize;
	IType itype;
	Pointer enum1;
	const char *helpInfo;
	MibProfileType profType;
} CmdList;

typedef struct TermCmd
{	
	unsigned long id;
	int level;
	long attribute;
	CmdList *cmdlist ;
	struct TermCmd *parent;
	struct TermCmd *sonHead;
	struct TermCmd *next;
}TermCmd;

#define PP TermInfo* term

#define CMD_ARG0	(Pointer)0
#define CMD_ARG1	(Pointer)1

#define CMD_NONE        (0x0)
#define CMD_READ        (0x1)
#define CMD_WRITE       (0x1<<1)
#define CMD_CREATE      (0x1<<2)
#define CMD_DELETE      (0x1<<3)
#define CMD_WRITEFLASH  (0x1<<4)
#define CMD_INSTANCE    (0x1<<5)
#define CMD_VERIFY      (0x1<<6)

#define CMD_CLEAR	(0x1<<7)
#define CMD_SAVE	(0x1<<8)
#define CMD_SHINST  	(0x1<<9)
#define CMD_VIEWSTAT	(0x1<<10)
#define CMD_DIAL	(0x1<<11)
#define CMD_HANGUP	(0x1<<12)
#define CMD_DEBUG	(0x1<<13)

#define CMD_HIDDEN      (0x1<<14)

#define CMD_SIM		(0x1<<15)	        /* added by llliu */

/*
 * llliu@080424:
 *
 *  CMD_* runs out! However, CMD_SHINST is free of use, so I take it.
 */
#define CMD_MORE	(CMD_SHINST)

/* sanity check */

#define CMD_MIN		CMD_READ
#define CMD_MAX		CMD_SIM         /* mod by llliu@080424 */

#define CMD_R           CMD_READ
#define CMD_W           CMD_WRITE
#define CMD_C           CMD_CREATE
#define CMD_D           CMD_DELETE
#define CMD_I	        CMD_INSTANCE
#define CMD_RW          (CMD_READ|CMD_WRITE)
#define CMD_RI          (CMD_READ|CMD_INSTANCE)
#define CMD_CDRI        (CMD_CREATE|CMD_DELETE|CMD_INSTANCE|CMD_READ)
#define CMD_RWCDI       (CMD_READ|CMD_WRITE|CMD_CREATE|CMD_DELETE|CMD_INSTANCE)

#define CMD_SHIFT	16		/* mod by llliu@080424 */

# if ( (CMD_MIN << CMD_SHIFT) <= CMD_MAX )
# error "CMD sanity check failed, in cmd.h"
#endif

/*
 * added by llliu@080424 for detailed sanity checking.
 */
#define TRANSPOSE_OVERFLOW(x, y) (((x << y) >> y) != x)

#if (CMD_MAX << CMD_SHIFT >= 0xffffffff)
#error "Overflow when transposing CMD_SIM"
#endif


#define CMD_LIST_R      (CMD_R << CMD_SHIFT)
#define CMD_LIST_W      (CMD_W << CMD_SHIFT)
#define CMD_LIST_C      (CMD_C << CMD_SHIFT)
#define CMD_LIST_D      (CMD_D << CMD_SHIFT)
#define CMD_LIST_DIAL   (CMD_DIAL << CMD_SHIFT)
#define CMD_LIST_HANGUP (CMD_HANGUP << CMD_SHIFT)
#define CMD_LIST_CLEAR  (CMD_CLEAR << CMD_SHIFT)
#define CMD_LIST_DEBUG  (CMD_DEBUG << CMD_SHIFT)

#define CMD_LIST_RW     (CMD_LIST_R|CMD_LIST_W)
#define CMD_LIST_RWCD   (CMD_LIST_R|CMD_LIST_W|CMD_LIST_C|CMD_LIST_D)
#define CMD_LIST_RCD    (CMD_LIST_W|CMD_LIST_C|CMD_LIST_D)

#define CMD_LIST_MORE   (CMD_MORE << CMD_SHIFT)





#define CMD_LEAF        (CMD_RW|CMD_LIST_RW)
#define CMD_LEAFVST     (CMD_VIEWSTAT|CMD_READ|CMD_LIST_R)
#define CMD_LEAFVST_MORE (CMD_VIEWSTAT|CMD_READ|CMD_LIST_R|CMD_MORE|CMD_LIST_MORE)
#define CMD_LEAFDAD     (CMD_R|CMD_LIST_RW)

#define CMD_N_RWCD      (CMD_NONE|CMD_LIST_RWCD)
#define CMD_N_RW        (CMD_NONE|CMD_LIST_RW)
#define CMD_R_R         (CMD_R|CMD_LIST_R)

#define VERB_SHOW       (CMD_READ)
#define VERB_SET        CMD_WRITE
#define VERB_NEW        CMD_CREATE
#define VERB_DELETE     CMD_DELETE
#define VERB_WRITEFLASH CMD_WRITEFLASH
#define VERB_VERIFY     CMD_VERIFY
#define VERB_SAVE    	CMD_SAVE    
#define VERB_SHINST 	CMD_SHINST
#define VERB_DIAL	CMD_DIAL
#define VERB_HANGUP	CMD_HANGUP
#define VERB_CLEAR	CMD_CLEAR
#define VERB_DEBUG	CMD_DEBUG
#define VERB_MORE       CMD_MORE        /* added by llliu@080424 */



#define CASEVERB(x)     if(term->verb & x)
#define DOSHOW          CASEVERB(VERB_SHOW)
#define DOSET           CASEVERB(VERB_SET)
#define DONEW           CASEVERB(VERB_NEW)
#define DOVERIFY        CASEVERB(VERB_VERIFY)
#define DODELETE        CASEVERB(VERB_DELETE)
#define DOWRITEFLASH    CASEVERB(VERB_WRITEFLASH)
#define DOSHINST        CASEVERB(VERB_SHINST)
#define DODIAL		CASEVERB(VERB_DIAL)
#define DOHANGUP	CASEVERB(VERB_HANGUP)
#define DOSAVE   	CASEVERB(VERB_SAVE)
#define DOCLEAR   	CASEVERB(VERB_CLEAR)
#define DODEBUG   	CASEVERB(VERB_DEBUG)
#define DOMORE          CASEVERB(VERB_MORE)

/*
 * show run : DEFAULT_RC
 * show sim : SIMPLE_RC
 * more run : MORE_RC
 */
#define DEFAULT_RC	1
#define SIMPLE_RC	2
#define MORE_RC         3
typedef struct {
    int len;
    int maxlen;
    char *s;
} Astr;



#if 0
TermCmd *cmd_Create(char *cmd_name, TermCmd *cmd_parent, void (*function)(TermInfo*), int priv, int _level, MibProfileType profType, int offset, int isize, IType itype, Pointer enum1);
#endif
TermCmd *cmd_Create( TermCmd *cmd_parent, int _level, CmdList *cmd );
void cmd_Add(TermCmd *parent, TermCmd *newson);
int cmd_ListBranch(TermCmd *cmd, TermInfo *term, char *str, int _verb);
void cmdDefault(TermInfo *term);
void cmd_InitCommon(CmdList *cmdlist);
void cmd_InitStart();
void cmd_ShowCfg(TermCmd *cmd, int *inst, TermInfo *term, char *str, int len, int show_type);
void cmd_ShowCmd(TermCmd *cmd, int *inst, TermInfo *term, char *str, int len);
void cmd_DispCfg(TermCmd *cmd, TermInfo *term);
TermCmd *cmd_GetNext(TermCmd *cmd);
int cmd_GetInstName(TermCmd *cmd, int *inst, char *buf);
void cmd_Enum(TermCmd *cmd, TermInfo *term);
void cmd_EnumInst(TermCmd *cmd, TermInfo *term);
void cmd_CmdStr(TermCmd *cmd, int *inst, char *buf);
void cmd_GetValueStr(TermCmd *cmd, int *inst, char *buf);
void cmd_PrintSet(TermCmd *cmd, int *inst, TermInfo *term, Astr *str, int show_type);
void nvramBackup();

extern unsigned long MaxTermLevel;
extern unsigned long MaxTermCmd;
extern TermCmd *root;
extern Bool first;

#endif /* CMD_H */

