#include <stdio.h>
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
#if (FEATURE_SW_PORT_TRUNK == FYES)
#include "sw_ioctl.h"

/* Function: portTrunkDelect
 *
 * Description: To delect trunk group.
 *
 * History: 2011.09.08 Chen.Shiyu   created
 */
static void portTrunkDelect(int gIndex)
{
	BOOL members[PORT_ARRAY_SIZE];
	memset(members, 0, sizeof(members));
	sw_trunk_group_set(gIndex-1, members);
}

/* Function: portTrunkConfig
 *
 * Description: this function is used to set trunk configurations that to
 *		get from the Trunk profile to switch Lower Layer.
 *
 * History: 2011.09.08 Chen.Shiyu   created
 */
static void portTrunkConfig(const TrunkProfile *newProfileDataPtr)
{
	TrunkProfile *prof = newProfileDataPtr;
	/* Set trunk configure to switch lower layer */
	sw_trunk_group_set(prof->tGroup-1, prof->tMembers);
	sw_trunk_mode_set(TRUNK_MODE);
}

static void portTrunkAddConfigByIndex(Uint32 index)
{
	TrunkProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_TRUNK;
	prof=(TrunkProfile*)ProfGetInstPointByIndex(mibType, index);
	if((ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	{
		portTrunkConfig(prof);
	}
}

/* Function: portTrunkInit
 *
 * Description: This function is used to init trunk module when the DUT 
 *		cold or hot boot. If trunk profile can not get any configured 
 *		information, call lib switch APIs to set default value directly. 
 *		Otherwise, init the DUT according trunk profile configure.
 *
 * History: 2011.09.08 Chen.Shiyu   created
 */
static void portTrunkInit(void)
{
	BOOL members[PORT_ARRAY_SIZE];
	int max, min, group_no;
	MibProfileType mibType = MIB_PROF_TRUNK;
	TrunkProfile *prof = NULL;

	memset(members, 0, sizeof(members));

	ProfInstLowHigh(mibType, &max, &min);
	
	for(group_no = min; group_no < max; group_no++)
	{
		prof = (TrunkProfile*)ProfGetInstPointByIndex(mibType, group_no);
		if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
		{
			portTrunkConfig(prof);
		}
		else
		{
			/* Set trunk configure to switch lower layer */
			sw_trunk_group_set(group_no, members);
			sw_trunk_mode_set(TRUNK_MODE);
		}
	}
}

/* Function: portTrunkProfileChange
 *
 * Description: According profile change type to exec trunk operations.
 *
 * History: 2011.09.08 Chen.Shiyu   created
 */
void portTrunkProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType pChangeType, Uint32 index,
	const TrunkProfile *oldProfileDataPtr, const TrunkProfile *newProfileDataPtr)
{
	switch(pChangeType) {
		case PROF_CHANGE_ADD:
			portTrunkAddConfigByIndex(index);
			break;
		case PROF_CHANGE_EDIT:
		    portTrunkConfig(newProfileDataPtr);
		    break;
		case PROF_CHANGE_DEL:
			portTrunkDelect(oldProfileDataPtr->tGroup);
			break;
		case PROF_CHANGE_DELALL:
		default: /* default operation */
			break;
	}
}

/* Function: funInitPortTrunk
 *
 * Description: The function is used to register port Trunk Profile Change
 *				 function and init Trunk.
 * 
 * History: 2011.09.08 Chen.Shiyu   created
 */
void funInitPortTrunk(void)
{
	/* Register portTrunkProfileChange() function */
	registerForProfileUpdates(MIB_PROF_TRUNK, (Pointer)NULL, (ProfileUpdateFnc)portTrunkProfileChange);
	/* Init port trunk */
	portTrunkInit();
}
#endif
