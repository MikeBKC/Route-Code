
#include <stdlib.h>
#include <stdio.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"

int main()
{
    MibProfileType mibtype = MIB_PROF_PORT_GROUP;
    int max = 0, min = 0;
    int i;
    int msgid;
    struct st_uttMsg msg;
    PortGroupProfile *prof;
    ProfInit();
    makeFactoryProfiles();
    ProfInstLowHigh(mibtype, &max, &min);
    for(i = min; i < max; i++) {
        prof = (PortGroupProfile*)ProfGetInstPointByIndex(mibtype,i);
        printf("%x   %s\n", prof->head.active, prof->head.name);
#if 0
        if(prof != NULL) {
            printf("sdfaf name:%d %s\n",i,prof->head.name);
        }
#endif
    }
    msgid = UttMsgInit();
    while(1) {
        msgrcv(msgid, &msg, sizeof(msg.msgPara), UTT_MSG_TYPE_1, 0);
        switch(msg.msgPara.mainType) {
            case MSG_PROF:
                printf("id %x \n",msg.msgPara.data.profChangeData.instIndex);
        }
    }
    exit(0);
}
