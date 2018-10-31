
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

#if (AP_SET_HOST_NAME == FYES)
void ap_set_hostname(void)
{
    unsigned char buf[100] = {0}; 
    ApBasicConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType,0);
    if((prof != NULL) && (strcmp(prof->dev_name,"") != 0))
    {
	sprintf(buf,"hostname %s",prof->dev_name);
    }
    else
    {
#if (UTT_TOMAX == FYES)
	sprintf(buf,"hostname %s","ToMAX");
#elif(BRAND_LEVEL_ONE == FYES)
	sprintf(buf,"hostname %s","LEVELONE");
#else
	sprintf(buf,"hostname %s","UTT");
#endif
    }
    system(buf);
}
#endif

#if (AP_CONF == FYES)

void ApConfInit()
{
	char productId[12];
	char hardwareId[12];
	char revisions[32];
	MibProfileType mibType = MIB_PROF_AP_CONF;
	ApConfProfile *prof = NULL;
	char *tmp = NULL;
	char *tmscb = "&#8482";
	struct utsname utsbuf;

	memset(productId, '\0', sizeof(productId));
	memset(hardwareId, '\0', sizeof(hardwareId));
	memset(revisions, '\0', sizeof(revisions));

	uname(&utsbuf);
	tmp = strtok(utsbuf.version, " ");
	tmp = strtok(NULL, " ");
	sprintf(revisions, "%s%s", utsbuf.version, tmp);

	tmp = NULL;
	tmp = strtok(utsbuf.machine, " ");
	tmp = strtok(NULL, " ");
	if (tmp != NULL) {
		sprintf(productId, "%s%s %s", utsbuf.machine, tmscb, tmp);
	} else {
		sprintf(productId, "%s", utsbuf.machine);
	}
	sprintf(hardwareId, "%s", UTT_HWMODEL);

	prof = (ApConfProfile *)ProfGetInstPointByName(mibType, "default");
	if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {

		strcpy(prof->types, productId);
		strcpy(prof->softVersion, revisions);
		strcpy(prof->firmVersion, hardwareId);
		prof->serial = getSystemSn();
	}

//    nvramWriteCommit();

	return;
}

void funInitApConf()
{
	ApConfInit();
}

#endif
