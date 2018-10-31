/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be  copied,    ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/

/*
    Copyright 2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: instmgr.c,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 * File:	template.c
 *
 * Overview:    CLI Inst mgr	
 *
 */

/* pls define CLI_DEBUG to turn on the debug
*/

# include "argcmd.h"
# include "cmd.h"

# include "nvram.h"

# include "platform.h"

Boolean clidumping ;

# if defined ( CLI_DEBUG ) 
# define INSTMGR_DEBUG
# endif 

# define INSTMGR_LIST
# undef  INSTMGR_LIST


#define NUM_INSTANCE		slotCount()
#define INVALID_INSTANCE	-1
#define INVALID_SLOT		-1

typedef struct{
    SlotNumber		slotNumber;
    SlotCardType	cardType;
    int			instNumber;
}INSTANCE;

static INSTANCE		_instList[NUMBER_OF_SLOTS];
static int		_instCount;
    
static SlotCardType _getCardTypeByStr(char *str);
static int _instMax( SlotCardType );

# if defined ( INSTMGR_DEBUG )

# define INST_DBG_CMD		"cliInstDebug"
# define INST_DBG_CMD_HELP	"Toggle cli instance debug "

static void
_instDbgCmd ( void )
{
    // added by llliu at REOS WSA.
    //TermCmd *tmp, *son;
    printf ( "cli debug toggled at: %s, %lu\n", __FILE__, __LINE__ );
    clidumping = !clidumping;
    printf ( "CLI debug is %s\n", clidumping ? "On" : "Off" );
    printf ( "MaxTermLevel: %lu, MaxTermCmd: %lu\n", MaxTermLevel, MaxTermCmd );
    printf ( "_instCount: %lu, NUM_INSTANCE: %lu\n", _instCount, NUM_INSTANCE);

# if defined( INSTMGR_LIST )
    for ( tmp = root; tmp; tmp->next ) {
    	if ( tmp->sonHead ) {
	  for ( son = tmp->sonHead; son; son = son->next) {
              CLIPRINTF(("id: %lu name: %s level: %lu, parent: %s sonhead: %s next: %s\n",
	      		 son->id,
			 son->cmdlist->name,
			 son->level,
			 son->parent->cmdlist->name ?,
			 son->sonHead->cmdlist->name,
			 son->next->cmdlist->name
		    ));
	  }
	} 
    }
# endif
    
}
# endif

static void
_cliArgCmdDbgCmd ( void )
{
    printf ( "cli argcmd debug toggled at: %s, %lu\n", __FILE__, __LINE__ );
    cliArgCmdDumping = !cliArgCmdDumping;;
    printf ( "CLI argcmd debug is %s\n", cliArgCmdDumping ? "On" : "Off" );
}


int cliSlotType[SLOT_NUMBER_OF_CARDS];

extern void
instMgrInit()
{
    unsigned int i;
    SlotCardType cardType;
    SlotNumber slot;

    memset(cliSlotType, 0, sizeof(cliSlotType));

    for( i = 0; i < NUM_INSTANCE; i++ )
    {
	slot.slotNumber = i + FIRST_SLOT_NUMBER;
	cardType = getSlotCardType(slot);
    	_instList[i].slotNumber = slot;
    	if(getBoardRev() != BOARD_1011 
	    || cardType != SLOT_CARD_SYS_BRI)
	{
	    _instList[i].cardType = cardType; 
	}
	_instList[i].instNumber = ++cliSlotType[cardType]; 
    }
    _instCount = i;

# if ( DOT1Q_VLAN == FYES )
# if ( VIRTUAL_DIALER == FYES )
# if ( HOST_ETH9 == FYES )
    if(getSystemPlatformHandle() == HIPER_4510NB)
    {
    	_instCount = ETHER2_SLOT_NUMBER;
    }
# if ( HOST_ETH25 == FYES )
    if(getSystemPlatformHandle() == HIPER_4520VF)
    {
    	_instCount = ETHER3_SLOT_NUMBER;
    }
# if ( HOST_ETH49 == FYES )
    if(getSystemPlatformHandle() == HIPER_4548)
    {
    	_instCount = ETHER5_SLOT_NUMBER;
    }
# endif /* HOST_ETH49 */
# endif /* HOST_ETH25 */
# endif /* HOST_ETH9 */
# else
# endif /* VIRTUAL_DIALER == FYES */
# endif /* DOT1Q_VLAN == FYES */

#  if defined ( INSTMGR_DEBUG )
   clidumping = FALSE;

   mtMonitorCreateCommand( INST_DBG_CMD,
                            INST_DBG_CMD_HELP,
                            _instDbgCmd,
			    FALSE);
#  endif

   mtMonitorCreateCommand( "clicrypto",
                           "toggle cli argcmd crypto debug",
			   _cliArgCmdDbgCmd,
			   FALSE);

}

void instanceVerify(TermInfo *term)
{
    Pointer prof;
    int index, i, found;
    int namep, names, activep, actives;
    char tmp[MAX_CMDLEN];
    TermCmd *cmd = GETCMD;

    term->result = FAIL_CLI;
    if(argGetInst(term->lastInput, cmd->level + 1, tmp) && (index = profileByNamei(cmd->cmdlist->profType, tmp, &prof)) != -1)
    {
 	term->inst[cmd->level] = index;
 	term->result = OK_CLI;
	releaseProfilePointer(cmd->cmdlist->profType,prof);
        return;
    }
    if(noPrompt) {
        _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
	return;
    }
    profileInitParam(cmd->cmdlist->profType, &namep, &names, &activep, &actives);
    found = 0;
    for(i = instanceMin(cmd->cmdlist->profType);i < instanceMax(cmd->cmdlist->profType);i++)
    {
        prof = profileByNumber(cmd->cmdlist->profType, i);
	if(!prof) continue;
        if(
# if ( ACTIVATE_PROF == FYES )
	   (0 != _strlen((char *)prof + namep))
# else
	   ((activep != -1) && (_getInt(prof, activep, actives) > 0)) 
	    || ((activep == -1) && (_strlen((char *)prof + namep) > 0))
# endif
	  )
        {
            found++;
	    if(namep == -1)
            	_printf(term, " %d", i + 1);
	    else
            	_printf(term, " %s", (char *)prof + namep);
        }
	releaseProfilePointer(cmd->cmdlist->profType, prof);
	//rescheduleTask(10);
    }
    if(found)
        _printf(term, "\r\nWhich %s?\r\n", cmd->cmdlist->name);
    else
    {
        _printf(term, "No %s now.\r\n", cmd->cmdlist->name);
        term->result = FAIL2_CLI;
    }
}

void instanceShow(TermInfo *term)
{
    int namep, names, activep, actives, i;
    Pointer prof;

    profileInitParam(GETCMD->cmdlist->profType, &namep, &names, &activep, &actives);

    term->result = FAIL_CLI;
    prof = profileByNumber(GETCMD->cmdlist->profType, i = term->inst[GETCMD->level]);
    if(prof)
    {
        if(
# if ( ACTIVATE_PROF == FYES )
	   (0 != _strlen((char *)prof + namep))
# else
   	   ((activep != -1) && (_getInt(prof, activep, actives) > 0)) 
	     || ((activep == -1) && (_strlen((char *)prof + namep) > 0))
# endif
	)
        {
	    if(namep == -1)
            	sprintf((char *)term->userPtr, "%d", i + 1);
	    else
            	_strcpy((char *)term->userPtr, (char *)prof + namep);
            term->result = OK_CLI;
        }
	releaseProfilePointer(GETCMD->cmdlist->profType, prof);
	//rescheduleTask(10);
    }
}

void instanceCreate(TermInfo *term)
{
    int namep, names, activep, actives, i, size;
    Pointer prof = NULL, prof1;
    TermCmd *cmd = GETCMD;
    char tmp[MAX_CMDLEN];

    if(!argGetInst(term->lastInput, cmd->level + 1, tmp))
         _printf(term, "Name not specified.\r\n");
    else
    {
        profileInitParam(cmd->cmdlist->profType, &namep, &names, &activep, &actives);
        if(_strlen(tmp) > names - 1)
        {
            _printf(term, "Name length too long.\r\n");
            return;
        }
        if(profileByNamei(cmd->cmdlist->profType, tmp, &prof) == -1 || strcmpi(tmp, (char *)prof + namep))
        {
            for(i = instanceMin(cmd->cmdlist->profType);i < instanceMax(cmd->cmdlist->profType);i++)
            {
                prof1 = profileByNumber(cmd->cmdlist->profType, i);
		if(!prof1) continue;
                if(
# if ( ACTIVATE_PROF == FYES )
	            (0 == _strlen((char *)prof1 + namep))
# else
		    ((activep != -1) && !_getInt(prof1, activep, actives)) ||
                    ((activep == -1) && !_strlen((char *)prof1 + namep))
# endif
		    )
                {
                    size = profileBuildFactory(cmd->cmdlist->profType, &prof);
                    memcpy(prof1, prof, size);
                    if(activep != -1)
                        _setInt(prof1, activep, actives, TRUE);
                    if(namep != -1)
		    {
                    	_strncpy((char *)prof1 + namep, tmp, names - 1);
			*((char *)prof1 + namep + names - 1) = 0;
		    }
                    if(!profileUpdate(cmd->cmdlist->profType, i, prof1))
			ERRMSG_NVRAM_FULL;
		    releaseProfilePointer(cmd->cmdlist->profType, prof1);
		    if(prof) releaseProfilePointer(cmd->cmdlist->profType, prof);
                    return;
                } 
		releaseProfilePointer(cmd->cmdlist->profType, prof1);
		//rescheduleTask(10);
            }
            _printf(term, "Cannot create more.\r\n");
        }
        else
            _printf(term, "Already exists.\r\n");
    }
    if(prof) releaseProfilePointer(cmd->cmdlist->profType, prof);
}

void instanceDelete(TermInfo *term)
{
    int index, namep, names, activep, actives;
    TermCmd *cmd = GETCMD;
    char tmp[MAX_CMDLEN];
    Pointer prof;
    Boolean bflag = FALSE;

    if(!argGetInst(term->lastInput, cmd->level + 1, tmp))
        _printf(term, "Name not specified.\r\n");
    else
    {
      profileInitParam(cmd->cmdlist->profType, &namep, &names, &activep, &actives);

      if((index = profileByNamei(cmd->cmdlist->profType, tmp, &prof)) != -1 && (namep == -1 || !strcmp(tmp, (char *)prof + namep)))
            {
    		Pointer defProf = 0;
    		SlotNumber slot;
    		int instindex;
    		wuProfInitParam(cmd->cmdlist->profType, term->inst, &instindex, &slot);

		bflag = BuildDefaultFactoryValue(cmd->cmdlist->profType, index, slot, &defProf);

            	if ( bflag ) {
		    _printf(term, "Cannot delete Factory profile: %s.\r\n", tmp);
		    releaseProfilePointer(cmd->cmdlist->profType, prof);
		    return;
		} else {
		    releaseProfilePointer(cmd->cmdlist->profType, prof);
                    profileBuildFactory(cmd->cmdlist->profType , &prof);
                    if(activep != -1)
                        _setInt(prof, activep, actives, FALSE);
                    if(!profileUpdate(cmd->cmdlist->profType, index, prof))
		        ERRMSG_NVRAM_FULL;
               }
            }
            else
                _printf(term, "Does not exist.\r\n");
    }
}

static SlotCardType _getCardTypeByStr(char *str)
{
    switch(str[0])
    {
        case 'b':
	    return SLOT_CARD_SYS_BRI;
            break;
        case 'e':
	    return SLOT_CARD_ETHER;
            break;
# if ( V35 == FYES )
        case 's':
	    return SLOT_CARD_V35;
	    break;
# endif
        default:
	    return SLOT_CARD_NO_CARD;
            break;
    }
} 

static int _instMax( SlotCardType cardType )
{
    int i, j;

    for( i = 0, j = 0; i < _instCount; i++ )
	if( _instList[i].cardType == cardType )
	    j++;
    return j;
}

static int _instGetSlotNumberByInst( SlotCardType cardType, int inst )
{
    int i;

    for( i = 0; i < _instCount; i++ )
	if( _instList[i].cardType == cardType 
	    && _instList[i].instNumber == inst )		
    	return _instList[i].slotNumber.slotNumber;

    return INVALID_INSTANCE;
}

static int _instGetInstBySlotNumber( int slotNumber, SlotCardType cardType )
{
    int i;

    for( i = 0; i < _instCount; i++ )
	if( _instList[i].slotNumber.slotNumber == slotNumber 
		&& _instList[i].cardType == cardType ) 
    	return _instList[i].instNumber;

    return INVALID_SLOT;
}
void intInstVerify(TermInfo *term)
{
    int i, slotNumber, inst, instmax;
    TermCmd *cmd = GETCMD;
    SlotCardType cardType;

    cardType = _getCardTypeByStr( (char*)cmd->cmdlist->name );
    inst = argGetInstInt(term);
    if( ( slotNumber = _instGetSlotNumberByInst( cardType, inst ) ) != INVALID_INSTANCE )
    {
        term->inst[cmd->level] = slotNumber;
        term->result = OK_CLI;
    }
    else
    {
	instmax = _instMax( cardType );
	if( instmax == 0 )
	{
	    term->result = FAIL2_CLI;
	    _printf(term, "No such interface installed.\r\n");
	}
	else
	{
            term->result = FAIL_CLI;
 	    for( i = 0; i < instmax; i++ )	
	    	_printf(term, "%d ", i + 1);
            _printf(term, "\r\nWhich %s interface?\r\n", cmd->cmdlist->name);
	}
    }
}

void intInstShow(TermInfo *term)
{
    int inst;
    TermCmd *cmd = GETCMD;
    SlotCardType cardType;

    cardType = _getCardTypeByStr( (char*)cmd->cmdlist->name );

    if( ( inst = _instGetInstBySlotNumber( term->inst[cmd->level], cardType ) )
		!= INVALID_SLOT )
    {
        sprintf((char *)term->userPtr, "%d", inst);
        term->result = OK_CLI;
    }
    else
        term->result = FAIL_CLI;
}

//add by jxy5@2003-11-26 for WebProfFrame
/***
*   get the slotnumber by instnumber
*Params:
*   instnumber   - instName is just the string of instnumber, such as "1" or "2" or "3"         
*Return:
*   SlotNumber -is used as  instIndex.    
*******************************************************************************/
extern int GetSlotNumber(SlotCardType cardType, int instnumber)
{
   return _instGetSlotNumberByInst(cardType, instnumber);
}

/***
*   get the instnumber by slotnumber
*Params:
*   instIndex   - the slotnumber
*Return:
*   instnumber -the string of it is just used as the instName.    
*******************************************************************************/
//get the slot intance 's number ,the instname is just as the string of instnumber
extern int GetSlotInstNumber(SlotCardType cardType, int instIndex)
{
   return _instGetInstBySlotNumber(instIndex,cardType);
}
//add end jxy5

