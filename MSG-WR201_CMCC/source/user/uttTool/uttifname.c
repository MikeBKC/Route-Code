

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"          
#include "mib.h"
#include "ifmisc.h"

int main(int argc, char** argv)
{
    int index = 0;
    Uint32 i = 0, wanCount = 0;
    Uint8 *name = NULL;
    if(argc < 3) {
        printf("uttif gnbi|gibn para\n");
        exit(0);
    }
    ProfInit();
    if(strcmp(argv[1], "gnbi") == 0) {
        index = strtol(argv[2], NULL, 10);
        if(index == 0) {
            printf("%s", getLanIfName());
        } else {
            //name = getWanIfNamePPP(index); 
            name = getWanIfName(index); 
            if(name != NULL) {
                printf("%s", name);
            }
        }
    } else if(strcmp(argv[1], "gibn") == 0){
        wanCount = getWanSlotCount();
        for(i = 1; i <= wanCount; i++) {
            if(strcmp(argv[2], getWanIfName(i)) == 0) {
                printf("%d", i);
                break;
            }
        }
    }
    ProfDetach();
    exit(0);
}
