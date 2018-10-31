
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

int main()
{
    MibProfileType mibtype = MIB_PROF_IP_GROUP;
    int max = 0, min = 0;
    int i;
    struProfAlloc *profhead = NULL;    
    IpGroupProfile *prof;
    ProfInit();
    ProfInstLowHigh(mibtype, &max, &min);
    printf("max %d, min %d\n",max,min);
    for(i = min; i < max; i++) {
        prof = (IpGroupProfile*)ProfGetInstPointByIndex(mibtype, i);
        if(prof != NULL) {
#if 1
            ProfBackupByIndex(mibtype, i, &profhead);
            strcpy(prof->head.name, "123");
#endif
        }
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    exit(0);
}
