
#include "mib.h"
#include "profacce.h"

#if (AP_PERMISSION == FYES)
void ApPermissionInit()
{
	//TODO:

	printf("%s, %d\n", __func__, __LINE__);

	return;
}

void funInitApPermission()
{
	registerForProfileUpdates(MIB_PROF_AP_PERMISSION, NULL, ApPermissionInit);
}

#endif
