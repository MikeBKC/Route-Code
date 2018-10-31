
#include <stdlib.h>
#include <stdio.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"

#if (PORT_MIRROR_6530G == FYES)

#ifdef  MIRROR_DEBUG
#define MIRROR_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define MIRROR_PRINTF(fmt, args...)
#endif
/***************
 * mirror6530GInit()
 * 2012-05-21
 */
void mirror6530GInit(void)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    /*get SystemProfile*/
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(profSys->Mirror_6530G_En == FUN_ENABLE) 
    {	/* open port mirror
	 * sniffer port 1, mirror port 2-4
	 */
	MIRROR_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	doSystem("mii_6530G reg w 0x78 0x10");
	doSystem("mii_6530G reg w 0x304 0x34004");
	doSystem("mii_6530G reg w 0x404 0x34004");
	doSystem("mii_6530G reg w 0x504 0x34004");
	doSystem("mii_6530G reg w 0x58 0x2000");
    }
    else
    {	/*close port mirror*/
	MIRROR_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	doSystem("mii_6530G reg w 0x78 0x0");
	doSystem("mii_6530G reg w 0x304 0x4004");
	doSystem("mii_6530G reg w 0x404 0x4004");
	doSystem("mii_6530G reg w 0x504 0x4004");
	doSystem("mii_6530G reg w 0x58 0x0");
    }
    return;
}

/****************
 * mirror6530GProfileChanged()
 * 2010-05-21
 */
void mirror6530GProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr)
{
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;
    MIRROR_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    /*portmirror enable changed*/
    if(oldProf->Mirror_6530G_En != newProf->Mirror_6530G_En) 
    {
	if(newProf->Mirror_6530G_En == FUN_ENABLE) 
	{   /* open port mirror
	     * sniffer port 1, mirror port 2-4
	     */
	    MIRROR_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    doSystem("mii_6530G reg w 0x78 0x10");
	    doSystem("mii_6530G reg w 0x304 0x34004");
	    doSystem("mii_6530G reg w 0x404 0x34004");
	    doSystem("mii_6530G reg w 0x504 0x34004");
	    doSystem("mii_6530G reg w 0x58 0x2000");
	}
	else
	{   /*close port mirror*/
	    MIRROR_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    doSystem("mii_6530G reg w 0x78 0x0");
	    doSystem("mii_6530G reg w 0x304 0x4004");
	    doSystem("mii_6530G reg w 0x404 0x4004");
	    doSystem("mii_6530G reg w 0x504 0x4004");
	    doSystem("mii_6530G reg w 0x58 0x0");
	}
    }
    return;
}

/****************
 * funInitMirror6530G()
 * 2012-05-21
 */
void funInitMirror6530G(void)
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, mirror6530GProfileChange); /*×¢²áProfileChangeº¯Êý*/
    mirror6530GInit(); /*³õÊ¼»¯*/
    MIRROR_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    return;
}
#endif
