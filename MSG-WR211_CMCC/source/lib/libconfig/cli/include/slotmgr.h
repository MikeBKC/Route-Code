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
    Copyright 1992 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: slotmgr.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 * File:	slotmgr.h
 *
 * Overview:	Slot manager interface.
 */

#if ! defined( SLOTMGR_H )
# define SLOTMGR_H

# include "mibglob.h"

    /*
     * Manifest constants in use by both TNT and older product.
     */
# define SLOT_NUMBER_NONE	0xffff

# define SLOT_NUMBER_ANY		0

    /*
     * The actual number of slots (determined at startup) is stored where
     * it can be obtained where needed.  The macro slotCount should be
     * used to access the variable.
     */
extern Uint			slotCount_;
extern Uint			ethSlotCount_;
# define slotCount()		slotCount_
# define ethSlotCount()		ethSlotCount_

# define FIRST_SLOT_NUMBER	1

# if ( PLATFORM & TNT )

#  include "tntslot.h"

#  if ( Q_TNT_IDSL_CARD )

#   define LAST_SLOT_NUMBER       16

    /*
     * The SLOT_INDEX macro provides a shorthand for extracting the value of a
     * SlotNumber and adjusting it for use as an array index.
     */
#   define SLOT_INDEX( slot )	(Uint)( ( slot ).slotNumber - FIRST_SLOT_NUMBER )

    /*
     * Slot cards can be classified by type.  The classes currently defined
     * are:
     *
     *	SLOT_CLASS_SYSTEM:	The motherboard (not a slot!)
     *
     *	SLOT_CLASS_NETWORK:	A network card type.
     *
     *	SLOT_CLASS_HOST:	A host card type with physical ports.
     *
     *	SLOT_CLASS_ETHER:	A ethernet host card (doesn't have physical
     *				ports).
     *
     *  SLOT_CLASS_MODEM:	A digital modem or V.110 card type.
     *
     *  SLOT_CLASS_SERIAL_WAN:	A serial WAN card type.
     *
     *	SLOT_CLASS_POTS:	A POTS card type.
     *
     *	SLOT_CLASS_IMODEM:	A IMODEM card type, like an ethernet host 
     *				card (doesn't have physical ports).. 
     */

typedef enum {
    SLOT_CLASS_SYSTEM,
    SLOT_CLASS_NETWORK,
    SLOT_CLASS_HOST,
#   if ( ETHERNET == FYES )
    SLOT_CLASS_ETHER,
#   endif /* ETHERNET */
#   if ( LMODEM == FYES ) || ( V110 == FYES )
    SLOT_CLASS_LAN_MODEM,
#   endif
#   if ( SERIAL_WAN == FYES )
    SLOT_CLASS_SERIAL_WAN,
#   endif
#   if ( PRIMARY_NAILED_56 == FYES )
    SLOT_CLASS_PRIMARY_NAILED_56,
#   endif
#   if ( PRIMARY_NAILED_T1 == FYES )
    SLOT_CLASS_PRIMARY_NAILED_T1,
#   endif
#   if ( POTS == FYES )
    SLOT_CLASS_POTS,
#   endif
#   if ( V35 == FYES )
    SLOT_CLASS_V35,
#   endif
#   if ( IMODEM == FYES )
    SLOT_CLASS_IMODEM,
#   endif
    SLOT_NUMBER_OF_CLASSES
} SlotClass;

    /*
     * TNT slot code should really not be calling this function, but...
     * there is a design problem with the BRI code which we resolve
     * by hardcoding the return value for the IDSL card
     */
#   define slotQuerySlotCardType( SlotNumber )  SLOT_CARD_SLOT_BRI_LT

    /*
     * Return TRUE if the slot number passed is a valid slot number, and
     * FALSE otherwise.
     */
extern Boolean
slotNumberInRange( SlotNumber	slotNumber );

    /*
     * Return the class of card in the slot being queried.
     */
extern SlotClass
slotQuerySlotClass( SlotNumber		slotNumber );

    /*
     * Some of the slots in the system are fixed in that they cannot be
     * removed (i.e. are on the motherboard) and some are removable modules.
     * The type of module for a particular slot is enumerated below.
     *
     * SLOT_MODULE_FIXED:	The slot cannot be removed.  It is part of
     *				the base system.
     *
     * SLOT_MODULE_REMOVABLE:	The slot can be inserted and removed without
     *				power cycling.
     */
typedef enum {
    SLOT_MODULE_FIXED,
    SLOT_MODULE_REMOVABLE,
    SLOT_MODULE_NUMBER_OF_TYPES,
} SlotModuleType;

    /*
     * Query the module type of a particular slot in the system.  If the
     * slotNumber passed is out of range (not between FIRST_SLOT_NUMBER
     * and LAST_SLOT_NUMBER), then the value returned will be 
     * SLOT_MODULE_NUMBER_OF_TYPES.
     *
     * Parameters:
     *	slotNumber:	The slot being queried.
     *
     * Returns:
     *	<>		The module type of the slot.
     */
extern SlotModuleType
slotQuerySlotModuleType( SlotNumber		slotNumber );

    /*
     * Return TRUE if the slot number passed in has a DSP on it that
     * can generate DTMF tones.  If it doesn't have that ability, return
     * FALSE.  This is used for dialing, echo tone, etc.
     */
extern Boolean
slotCanGenerateTones( SlotNumber	slotNumber );

#  endif /* Q_TNT_IDSL_CARD */

# else /* PLATFORM & TNT */


    /*
     * Note: Even though the Multiport is the only box with slots in it
     * we are going to treat the other boxes, for the sake of code
     * conformity, as having two slots.  These pseudo slots are always:
     *	slot 1 is the network
     *	slot 2 is the host
     */

/****************************************************************************
 *
 * The first part of this module defines the basic parameters and status
 * associated with a card/slot in the system.  These functions can be called
 * to query the card type and status.  This is general query information,
 * not providing any control mechanisms for the card.  The second part
 * will address slot/card control.
 *
 ****************************************************************************
 */

     
#  define LAST_SLOT_NUMBER	( slotCount_ + FIRST_SLOT_NUMBER - 1 )


    /*
     * We declare a global SlotNumber of NETWORK_SLOT_NUMBER here so we 
     * don't have to build it everywhere.
     */
extern const SlotNumber 	networkSlot;

    /*
     * The SLOT_INDEX macro provides a shorthand for extracting the value of a
     * SlotNumber and adjusting it for use as an array index.
     */
#  define SLOT_INDEX( slot )	(Uint) ( ( slot ).slotNumber - FIRST_SLOT_NUMBER )


    /*
     * Convert from SNMP slot index to a SlotNumber sturcture.
     */
extern __inline__ SlotNumber
SNMP_SLOT_INDEX_TO_SLOT_NUMBER( Int slotIndex ) 
{
    SlotNumber	s;

    s.slotNumber = slotIndex;

    return s;
}

    /*
     * Convert from a SlotNumber sturcture to an SNMP slot index.
     */
extern __inline__ Int
SLOT_NUMBER_TO_SNMP_SLOT_INDEX( SlotNumber slot ) 
{
    return slot.slotNumber;
}

    /*
     * Each slot in the system can be in any number of states, based on
     * its physical and start-up status.  This module manages the transition
     * amongst the states, and can be queried for the current status.
     *
     * SLOT_EMPTY:	The slot does not have a card physically in it.
     *
     * SLOT_DEAD:	The slot is there, but the card is dead.  There is
     *			something wrong with it that the system chose to
     *			leave it asleep.
     *
     * SLOT_RESET:	The slot is held reset by this module.
     *
     * SLOT_DIAGNOSTIC:	The slot is currently running its POST or some class
     *			of diagnostics.
     *
     * SLOT_RUNNING:	The slot is up and running real-live software / code.
     */
typedef enum {
    SLOT_EMPTY,
    SLOT_DEAD,
    SLOT_RESET,
    SLOT_DIAGNOSTIC,
    SLOT_RUNNING,
    SLOT_NUMBER_OF_STATUS
} SlotStatus;
 
    /*
     * Some of the slots in the system are fixed in that they cannot be
     * removed (i.e. are on the motherboard) and some are removable modules.
     * The type of module for a particular slot is enumerated below.
     *
     * SLOT_MODULE_FIXED:	The slot cannot be removed.  It is part of
     *				the base system.
     *
     * SLOT_MODULE_REMOVABLE:	The slot can be inserted and removed without
     *				power cycling.
     */
typedef enum {
    SLOT_MODULE_FIXED,
    SLOT_MODULE_REMOVABLE,
    SLOT_MODULE_NUMBER_OF_TYPES
} SlotModuleType;


    /*
     * Slot cards can be classified by type.  The classes currently defined
     * are:
     *
     *	SLOT_CLASS_SYSTEM:	The motherboard (not a slot!)
     *
     *	SLOT_CLASS_NETWORK:	A network card type.
     *
     *	SLOT_CLASS_HOST:	A host card type with physical ports.
     *
     *	SLOT_CLASS_ETHER:	A ethernet host card (doesn't have physical
     *				ports).
     *
     *  SLOT_CLASS_MODEM:	A digital modem or V.110 card type.
     *
     *  SLOT_CLASS_SERIAL_WAN:	A serial WAN card type.
     *
     *	SLOT_CLASS_POTS:	A POTS card type.
     *
     *	SLOT_CLASS_IMODEM:	A IMODEM card type, like an ethernet host 
     *				card (doesn't have physical ports).. 
     */
typedef enum {
    SLOT_CLASS_SYSTEM,
    SLOT_CLASS_NETWORK,
    SLOT_CLASS_HOST,
#  if ( ETHERNET == FYES )
    SLOT_CLASS_ETHER,
#  endif /* ETHERNET */
#  if ( LMODEM == FYES ) || ( V110 == FYES )
    SLOT_CLASS_LAN_MODEM,
#  endif
#  if ( SERIAL_WAN == FYES )
    SLOT_CLASS_SERIAL_WAN,
#  endif
#  if ( PRIMARY_NAILED_56 == FYES )
    SLOT_CLASS_PRIMARY_NAILED_56,
#  endif
#  if ( PRIMARY_NAILED_T1 == FYES )
    SLOT_CLASS_PRIMARY_NAILED_T1,
#  endif
#  if ( POTS == FYES )
    SLOT_CLASS_POTS,
#  endif
#  if ( V35 == FYES )
    SLOT_CLASS_V35,
#  endif
#  if ( IMODEM == FYES )
    SLOT_CLASS_IMODEM,
#  endif
    SLOT_NUMBER_OF_CLASSES
} SlotClass;


#  if ( PLATFORM & ( MPORT ))
    /*
     * Return TRUE if the slot number passed in has a DSP on it that
     * can generate DTMF tones.  If it doesn't have that ability, return
     * FALSE.  This is used for dialing, echo tone, etc.
     */
extern Boolean
slotCanGenerateTones( SlotNumber	slotNumber );

#  elif ! ( PLATFORM & ( P50 | PDSL | P25 | P15 | M200 ) )
#  endif

    /*
     * Does the security PROM allow usage of the given slot?
     *
     * For example, a Max4002 has two T1 slots but the customer isn't allowed
     * to use the second slot because he didn't pay for it.
     * Nevertheless, the code sometimes uses the hardware resources
     * that come with this slot regardless of whether or not the customer
     * is allowed to access it.  This function helps us draw the distinction
     * by indicating when end-user access is disallowed to such a slot.
     *
     * Return TRUE if end-user access is allowed; return FALSE if not.
     */
extern Boolean
slotUsageIsAllowed( SlotNumber		slotNumber );


    /*
     * Return the class of card in the slot being queried.
     */
extern SlotClass
slotQuerySlotClass( SlotNumber		slotNumber );


    /*
     * Query the current status of a particular slot in the system.  If the
     * slotNumber passed is out of range (not between FIRST_SLOT_NUMBER
     * and LAST_SLOT_NUMBER), then the value returned will be 
     * SLOT_NUMBER_OF_STATUS.
     *
     * Parameters:
     *	slotNumber:	The slot being queried.
     *
     * Returns:
     *	<>		The current status of the slot.
     */
extern SlotStatus
slotQuerySlotStatus( SlotNumber		slotNumber );


    /*
     * Query the module type of a particular slot in the system.  If the
     * slotNumber passed is out of range (not between FIRST_SLOT_NUMBER
     * and LAST_SLOT_NUMBER), then the value returned will be 
     * SLOT_MODULE_NUMBER_OF_TYPES.
     *
     * Parameters:
     *	slotNumber:	The slot being queried.
     *
     * Returns:
     *	<>		The module type of the slot.
     */
extern SlotModuleType
slotQuerySlotModuleType( SlotNumber		slotNumber );


    /*
     * Query the type of card for a particular slot in the system.  If the
     * slotNumber passed is out of range (not between FIRST_SLOT_NUMBER
     * and LAST_SLOT_NUMBER), then the value returned will be 
     * SLOT_NUMBER_OF_CARDS.  If the type of card is unknown or empty, then
     * the value SLOT_NUMBER_OF_CARDS will be returned.
     *
     * Parameters:
     *	slotNumber:	The slot being queried.
     *
     * Returns:
     *	<>		The current type of card in the slot.
     */
extern SlotCardType
slotQuerySlotCardType( SlotNumber		slotNumber );


    /*
     * Return TRUE if the slot number passed is a valid slot number, and
     * FALSE otherwise.
     */
extern Boolean
slotNumberInRange( SlotNumber	slotNumber );


/****************************************************************************
 *
 * The second part of this module is the definition from which slots/cards
 * can be controlled and managed:
 * The card-type independent control of slots is all done within this module.
 * Card-type dependent processing is done externally.  For each type of card,
 * there exists three lists of routines to be called to start the card up, to
 * shut it down, and to process interrupts from it.  These lists are identified
 * to this module via the registration function below.
 *
 ****************************************************************************
 */


    /*
     * When an initialization function completes, it passes either a
     * PASS or FAIL enumeration value, as defined below.
     */
typedef enum {
    SLOT_INIT_SUCCESS,
    SLOT_INIT_FAIL
} SlotInitResult;


    /*
     * The startup, interrupt and termination functions for the 
     * module are all very simple.  They simply take the slot number 
     * and assume that all else is context sensitive...
     *
     * Interrupt service routines are called for the slots with just the
     * slotNumber.  Since one interupt serves multiple sources, each time
     * there is an interrupt, each of the ISRs is called.  It is up to
     * them to determine if the interrupt is for them and process it 
     * accordingly.
     */
typedef void
(* SlotMgrEventFunction)( SlotNumber	slotNumber );

    /*
     * When a startup function is run, it should always call the function below
     * when it completes.  Note that if the shutdown function is called before
     * startup completes, this function does not have to be called (but if it
     * is, there is no harm).
     */
#  if ( SCAVENGE_RAM == FYES )
extern void
slotMgrInitFunctionComplete( SlotNumber		slotNumber,
			     SlotInitResult	initResult )__attribute__ ((section (".init")));
#  else
extern void
slotMgrInitFunctionComplete( SlotNumber		slotNumber,
			     SlotInitResult	initResult );
#  endif

    /*
     * Register the lists of startup, shutdown, and interrupt functions, as
     * defined above, for a particular slot card type.
     */
extern void
slotRegisterCardType( SlotCardType			slotCardType,
		      const SlotMgrEventFunction	*startupList,
		      const SlotMgrEventFunction	*shutdownList,
		      const SlotMgrEventFunction	*interruptList );


    /*
     * The following is a slot manager startup function which is called from
     * the middle of the startup function list for a card.  All startup
     * functions prior to it run with the slot interrupt disabled.  This
     * function enables, and all following startup functions run with the slot
     * interrupt enabled.
     */
#  if ( SCAVENGE_RAM == FYES )
extern void
slotMgrInitFncInitializeISR( SlotNumber	slotNumber )__attribute__ ((section (".init")));
#  else
extern void
slotMgrInitFncInitializeISR( SlotNumber	slotNumber );
#  endif

    /*
     * When a shutdown function completes, it must call the function below.
     * This is required when the shutdown is complete.
     */
extern void
slotMgrShutdownFunctionComplete( SlotNumber		slotNumber );


/****************************************************************************
 *
 * The third part of this module allows modules that need to know when card
 * states or types change to register with this module for updates on the card
 * state change.
 *
 * Examples of registrants would be the NVRAM manager that is required to
 * match the NVRAM parameters stored to the type of card in the slot.  
 *
 ****************************************************************************
 */


    /*
     * Whenever the state of any of the status variables for a slot changes
     * (i.e. the status or card type), all registered functions of the type
     * defined below are called.  The functions are called to allow any
     * necessary clean-up or modifications internally.
     *
     * Parameters:
     *  slotNumber:	The slot number whose status changed.
     *
     *  slotStatus:	The new status of the slot.
     *
     *  slotCardType:	The type of card in the slot, or SLOT_CARD_NO_CARD
     *			if there is no card in the slot.
     *
     * Returns:
     *  None.
     */
typedef void
SlotStateChangeFnc( SlotNumber		slotNumber,
                    SlotStatus		slotStatus,
                    SlotCardType	slotCardType );


    /*
     * Register with this module a function of type SlotStateChangeFnc to
     * be called whenever the status of a slot changes.
     */
extern void
slotRegisterSlotChangeFnc( SlotStateChangeFnc	*slotStateChangeFnc );


/****************************************************************************
 *
 * The fourth and final part of this module is special-purpose routines for
 * module and system initialization.
 *
 ****************************************************************************
 */


    /*
     * Called by POST when POST is complete but before the tasks waiting
     * on the POST completion semaphore are started.
     */
#  if ( SCAVENGE_RAM == FYES )
extern void 
slotMgrStart( void )__attribute__ ((section (".init")));
#  else
extern void 
slotMgrStart( void );
#  endif


    /*
     * Called by POST following call to slotMgrStart() to wait for completion
     * of initial poll and status update of all slots.
     */
#  if ( SCAVENGE_RAM == FYES )
extern void
slotWaitForInitialStatus( void )__attribute__ ((section (".init")));
#  else
extern void
slotWaitForInitialStatus( void );
#  endif


    /*
     * Called by POST to wait for the initial POST for each slot to complete.
     */
#  if ( SCAVENGE_RAM == FYES )
extern void
slotWaitForPostCompletion( SlotNumber slotNumber )__attribute__ ((section (".init")));
#  else
extern void
slotWaitForPostCompletion( SlotNumber slotNumber );
#  endif


#  if ( (PLATFORM & MPORT) && (DIGITAL_MODEMS == FYES) )
   /*
    * Called to force a particular slot up or down.
    */
extern void
slotMgrForceSlot( SlotNumber    slotNumber,
                  Boolean       down );
#   define FORCE_SLOT_DOWN         0L
#   define FORCE_SLOT_UP           1L
#  endif



    /*
     * Module initialization function, called from initram.
     */
#  if ( SCAVENGE_RAM == FYES )
extern void
slotMgrInit( void )__attribute__ ((section (".init")));
#  else
extern void
slotMgrInit( void );
#  endif


/* return the number of B channels per line based on slot type.
 */
extern Word
getBChannelsPerInterface( SlotNumber slot );


# endif /* PLATFORM & TNT */

    /*
     * Called by LCD or TRESTORE when the P220 user changes the Net IF
     * from T1-CSU to SERIAL WAN or vice versa.
     */
void
p220ChangeT1SWANCardType( int slot );

extern SlotCardType
getSlotCardType(SlotNumber);

#endif /* SLOTMGR_H */
