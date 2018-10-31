#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wsIntrn.h>
#include <um.h>
#include <utils.h>
#include <uttMachine.h>
#include "typedef.h"
#include "mibunion.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#if (FEATURE_SW_PORT_MIRROR == FYES)
#include "mibMirror.h"
#include "sw_ioctl.h"

/*
 * Description: get mirror profile, set mirror port, monitored ports  
 *				and mirror mode to switch register. 
 * 
 *	added by Chen.Shiyu, 2011.08.31
 */
void portMirrorConfig(const MirrorProfile *newProfileDataPtr)
{
	MirrorProfile *prof = newProfileDataPtr;
	/* set mirror configs to switch register */
	sw_mirror_egress_ingress_port_mode_set(prof->monitoredPort, prof->mirrorMode); 

	sw_mirror_monitor_port_set(prof->mirrorPort); /* set mirror port */
}

/*
 * Description: Init or restore mirror configures.
 * 
 * added by Chen.Shiyu, 2011.08.31
 *
 */
void portMirrorInit(void)
{
	BOOL monitored_ports[PORT_ARRAY_SIZE]; 
	Uint32 mirror_port = 0u;
	memset(monitored_ports, 0, sizeof(monitored_ports));
	
	MibProfileType mibType = MIB_PROF_MIRROR;
	MirrorProfile *prof = NULL;
	int pIndex = 0;

	prof = (MirrorProfile*)ProfGetInstPointByIndex(mibType, pIndex);

	/* restore mirror configures when profile is not NULL. otherwise,
	 * set it default value to register directly.
	 */
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
	{
		/* restore mirror configure from profile. */
		portMirrorConfig(prof);
	}
	else
	{
		/* set mirror configs to switch register */
		sw_mirror_egress_ingress_port_mode_set(monitored_ports,MIRROR_DISABLE);
		sw_mirror_monitor_port_set(mirror_port); /* set mirror port */
	}
}

/*
 * Description: According profile change type to exec mirror operations.
 *
 * added by Chen.Shiyu, 2011.08.31	
 */
void portMirrorProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType pChangeType, Uint32 index,
	const MirrorProfile *oldProfileDataPtr, const MirrorProfile *newProfileDataPtr)
{
	switch(pChangeType) {
		case PROF_CHANGE_ADD:
		case PROF_CHANGE_EDIT:
			portMirrorConfig(newProfileDataPtr);
			break;
		case PROF_CHANGE_DEL:
		case PROF_CHANGE_DELALL:
		default: /* default operation */
			break;
	}
}

/*
 * register port mirror profile change function and init mirror.
 * 
 * added by Chen.Shiyu, 2011.08.31
 */
void funInitPortMirror(void)
{
	registerForProfileUpdates(MIB_PROF_MIRROR, (Pointer)NULL, (ProfileUpdateFnc)portMirrorProfileChange);
	portMirrorInit();
}
#endif
