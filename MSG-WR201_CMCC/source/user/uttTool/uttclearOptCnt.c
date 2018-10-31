

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

#if ((PPPOE_SELF_HELP == FYES) || (WEBAUTH_SELF_HELP == FYES))
static void usage()
{
    printf("    set [param] [value]\n\
    getbynm [param]\n\
        get [param]\n\
	make_ap_config wireless\n\
       inst [param]\n");
}

int main(int argc, char** argv)
{
#if (PPPOE_SELF_HELP == FYES)
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_ACC;
    PPPoESrvAccProfile *prof = NULL;
    struProfAlloc *profList  = NULL;
    int min = 0, max = 0, i = 0;
#endif
#if (WEBAUTH_SELF_HELP == FYES)
    MibProfileType wamibTypeS = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *waprofS = NULL;
    MibProfileType wamibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *waprof = NULL;
    struProfAlloc *profhead = NULL;
    int min1 = 0, max1 = 0, j = 0;
#endif
    ProfInit();
    if(argc < 2)
    {
	exit(0);
    }
    if(strcmp(argv[1], "pppoe") == 0)
    {
#if (PPPOE_SELF_HELP == FYES)
	profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
	if((profS->head.active==1)&&(profS->self_help == FUN_ENABLE))
	{
	    ProfInstLowHigh(mibType, &max, &min);
	    for(i=min;i<max;i++)
	    {
		prof = (PPPoESrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
		if((prof!=NULL)&&(ProfInstIsFree(prof) == 0))
		{
		    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profList);
		    prof->optCnt = 0U;
		}
	    }
	    ProfUpdate(profList);
	    ProfFreeAllocList(profList);
	    nvramWriteCommit();
	}
#endif
    }
    else if (strcmp(argv[1], "webauth") == 0)
    {
#if (WEBAUTH_SELF_HELP == FYES)
	waprofS = (WebAuthGlobalProfile *)ProfGetInstPointByIndex(wamibTypeS, 0);
	if((waprofS->enabled == 1)&&(waprofS->self_help == FUN_ENABLE))
	{
	    ProfInstLowHigh(wamibType, &max1, &min1);
	    for(j=min1;j<max1;j++)
	    {
		waprof = (WebAuthProfile *)ProfGetInstPointByIndex(wamibType, j);
		if((waprof!=NULL)&&(ProfInstIsFree(waprof) == 0))
		{
		    ProfBackupByIndex(wamibType, PROF_CHANGE_EDIT, j, &profhead);
		    waprof->optCnt = 0U;
		}
	    }
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	
	}
#endif
    }
    ProfDetach();
    exit(0);
}
#if 0
int main(int argc, char** argv)
{
    int i = 0, findInstType = 0;
    Pointer prof = NULL;
    struct st_xmlConfig *conf = rootConfig;
    MibProfileType mibType = -1;
    char buf[MAX_VALUE_LEN] = {0};
    int max, min;
    if(argc < 3) {
        usage();
    }

#if 1
    printf("%s-%d: argv[1]=%s,argv[2]=%s,argv[3]=%s\n",__FUNCTION__,__LINE__,argv[0],argv[1],argv[2]);
        for(; conf->name != NULL; conf++) {
            if(strcmp(conf->name, argv[1]) == 0) {
		printf("%s-%d: \n",__FUNCTION__,__LINE__);
                switch(conf->privilege) {
                    case INST:
                        mibType = conf->mibtype;
                        findInstType = 1;
                        break;
                    default:
                        break;
                }
                conf = conf->child;
                break;
            }
        }

    //ProfInit();
    /*
     * find the inst prof
     */
		printf("%s-%d: \n",__FUNCTION__,__LINE__);
    if(findInstType == 1) {
        ProfInstLowHigh(mibType, &max, &min);
	if ((strcmp(argv[1], "pppoeSrvAcc") == 0)) {
		printf("%s-%d: \n",__FUNCTION__,__LINE__);
	    for(i=min;i<max;i++)
	    {
		printf("%s-%d: \n",__FUNCTION__,__LINE__);
		prof = ProfGetInstPointByIndex(mibType, i);
		if(prof == NULL) {
		    ProfDetach();
		    exit(1);
		}
		//printf("%s-%d: optCnt=%d\n",__FUNCTION__,__LINE__,((PPPoESrvAccProfile *)prof)->optCnt);

		printf("%s-%d: \n",__FUNCTION__,__LINE__);
		for(; conf->name != NULL; conf++) {
		    if(strcmp(conf->name, argv[2]) == 0) {
			printf("%s-%d: \n",__FUNCTION__,__LINE__);
			if(conf->privilege == VALUE) {
			    printf("%s-%d: \n",__FUNCTION__,__LINE__);
			    parseProfileValue(conf, mibType, buf, prof);
			    //parseProfileValue2(OFFANDSIZE(PPPoESrvAccProfile, optCnt), TINT, buf, prof);
			    printf("%s-%d: buf=%s",__FUNCTION__,__LINE__, buf);
			    ProfDetach();
			} else {
			    if(conf->child != NULL) {
				conf = conf->child;
				break;
			    } else {
			    }
			}
		    }
		}
	    }
	} else if (strcmp(argv[1], "set") == 0) { /* set√¸¡Ó */
	} else if (strcmp(argv[1], "inst") == 0) { /* inst√¸¡Ó */
	    mibHead *prof1 = NULL;
	    for (i = min; i < max; i++) {
		prof1 = (mibHead *)ProfGetInstPointByIndex(mibType, i);
		if (ProfInstIsFree(prof1) == 0) {
		    printf("%s ", prof1->name);
		}
	    }
	} 
	else {

	}
    }
#endif
    exit(0);
}
#endif

#else
int main(int argc, char** argv)
{
    exit(0);
}
#endif
