#include	<string.h>
#include	<stdarg.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"

#if 0
#define DBGPRINT(fmt, args...)	    printf(fmt,##args)
#else
#define DBGPRINT(fmt, args...) 
#endif

static void usage()
{
    printf("usage: webAuthSes  print \n");
    printf("                   print web auth account all session.\n");
    return;
}
/* ar8316 设置完口端口速率
 * 开机启动时使用此程序设置*/
int main(int argc, char **argv) 
{
#if (WEB_AUTH_SHARE == FYES)
    MibProfileType mibType = MIB_PROF_WEB_AUTH;
    WebAuthProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0, j = -1, k = -1;

    if(argc < 2) {
	usage();
	return -1;
    }
    if(strcmp(argv[1], "print")!=0) {
	usage();
	return -1;
    }
    /*共享内存初始化*/
    if(ProfInit() == PROFFAIL) 
    {
	return -1;
    }
 
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (WebAuthProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
		&& (prof->head.active == TRUE))
	{
	    printf("%s-%d: ----- web auth session print -----\n",__func__,__LINE__);
	    printf("%s-%d: prof->user =%s,prof->maxSession =%u,prof->actualSession =%u\n",__func__,__LINE__,
		    prof->user,prof->maxSession,prof->actualSession);
	    if(checkSessionIndex(prof->sesIndex) == 1)
	    {
		printf("%s-%d: ----- profile sesIndex is valid, sesIndex =%d. -----\n",__func__,__LINE__,prof->sesIndex);
		j = prof->sesIndex;
		for(k=0;k<prof->actualSession;k++)
		{
		    if(checkSessionIndex(j) != 1)
		    {
			break;
		    }
		    printf("%s-%d: session index =%d, nextIndex = %d\n",__func__,__LINE__,j,nvramProfile->session[j].nextIndex);
		    printf("%s-%d: session ip =0x%x, mac =%s, compareMacEn =%d\n",__func__,__LINE__,
			    nvramProfile->session[j].ip,nvramProfile->session[j].mac,nvramProfile->session[j].compareMacEn);
		    j = nvramProfile->session[j].nextIndex;
		    
		}
	    }else{
		printf("%s-%d: ----- profile sesIndex is not valid. -----\n",__func__,__LINE__);
	    }
	}
    }
    /*共享内存分离*/
    ProfDetach();
#endif
    return 0;
}
