
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"

#if (AP_LOAD_BALANCE == FYES)

void apLoadBalanceInit()
{
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    ApLoadBalanceProfile *prof = NULL;
    int min = 0, max = 0;
	int i = 0, j = 0;

    ProfInstLowHigh(mibType, &max, &min);
	for (j = min; j < max; j++) {
        prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, j);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
            for(i=0; i<prof->num;i++){
				prof->ap[i].isAlive = 0u;
			}
		}

	}

	return;
}


void funInitApLoadBalance()
{
	apLoadBalanceInit();
}
#endif
