#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ifmisc.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "conver.h"

int main(int argc, char **argv)
{
#if (WEBAUTH_ACCOUNT == FYES)
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *prof = NULL;

    ProfInit();
    prof = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (prof->flashWt & WEB_AUTH_FLASH_W == 1)/*使用了web认证时间计费且有数据没有保存到flash*/
    {
	nvramWriteCommit();/*保存*/
	prof->flashWt &= ~WEB_AUTH_FLASH_W;
    }
#endif
    
    return 0;
}
