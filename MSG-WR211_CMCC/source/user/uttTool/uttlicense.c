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

#if (UTT_LICENSE == FYES)

//#define LICENSE_DEBUG
#ifdef LICENSE_DEBUG
#define LICENSE_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define LICENSE_PRINT(fmt, args...)
#endif


static void usage(char *cmd)
{
    printf(" usage: \n");
    printf(" %s content		- show license content\n", cmd);
    printf(" %s status		- show license status\n", cmd);
}

/*
 * main(int argc, char** argv)
 *
 * 更新license 的运行时间
 *
 * 输出license的状态和内容
 */
int main(int argc, char** argv)
{
    MibProfileType mibtype = MIB_PROF_LICENSE;
    LicenseProfile *uttlicense=NULL;

    ProfInit();
    if(argc < 2)    /*argument cannot less than 2*/
    {
	LICENSE_PRINT("%s-%d: exit.\n",__func__,__LINE__);
	exit(0);
    }
    if(strcmp(argv[1], "update")==0) {
#if 0
	uttlicense = (LicenseProfile*)ProfGetInstPointByIndex(mibtype, 0);
	uttlicense->taskRunning = TRUE;
	LICENSE_PRINT("%s-%d: authResult=%d, taskRunning=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning);
	while(1)
	{
	    /* check authResult cnd license status
	     * if authResult is 0xff and timeflag is forever
	     * init taskRunning id false
	     */
	    if(uttlicense->authResult != 0xff || uttlicense->timeflag == FOREVER)
	    {
		uttlicense->taskRunning = FALSE;
		LICENSE_PRINT("%s-%d: authResult=%d, taskRunning=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning);
		break;
	    }
	    /*update every one hour*/
	    sleep(30);
	    /* one hour later
	     * runtime +1
	     * and save new runtime
	     */
	    uttlicense->runtime++;
	    saveLicenseRuntime(uttlicense->runtime);
	    /*check runtime and impowertime*/
	    if(uttlicense->runtime >= uttlicense->impowerTime * 30 * 24)
	    {
		uttlicense->authResult = 0;
		uttlicense->isOutdate = 1;

		uttlicense->taskRunning = FALSE; 
		LICENSE_PRINT("%s-%d: authResult=%d, taskRunning=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning);
		break;
	    }
	    //getRuntimeFromFlash(&gettime);
	    LICENSE_PRINT("%s-%d: authResult=%d, taskRunning=%d, runtime=%d\n",__func__,__LINE__,uttlicense->authResult,uttlicense->taskRunning,uttlicense->runtime);
	}
#endif
    } else if(strcmp(argv[1], "content")==0) {
	
	char iptmp[17]={0};
	
	uttlicense = (LicenseProfile*)ProfGetInstPointByIndex(mibtype, 0);

	//printf("License Content\r\n");
   
	printf("%20s: %d\n", "sequence num", uttlicense->num);
	/*
	 * print license type
	 * timecontrol
	 * functioncontrol
	 * or platcontrol
	 */
	if(uttlicense->controltype == TIME_CONTROL)
	{
	    printf("%20s: %s\n", "License Type", "timeLicense");
	}
	if(uttlicense->controltype == FUNCTION_CONTROL)
	{
	    printf("%20s: %s\n", "License Type", "featureLicense");
	}
	if(uttlicense->controltype == TERRACE_CONTROL)
	{
	    printf("%20s: %s\n", "License Type", "platformLicense");
	}
	/*
	 * print license status
	 * forever or tmp or lawless
	 */
	if(uttlicense->timeflag == FOREVER)
	{
	    printf("%20s: %s\n", "License Stat", "Forever");
	}
	if(uttlicense->timeflag == LAWLESS)
	{
	    printf("%20s: %s\n", "License Stat", "Lawless");
	}
	/*
	 * print tmp license
	 * impowertime runtime and remaintime 
	 */
	if(uttlicense->timeflag == EFFECTIVE)
	{
	    printf("%20s: %s\n", "License Stat", "Temporary");
	    printf("%20s: %d hours\n", "Impower Time", uttlicense->impowerTime * 30 * 24);
	    //printf("%20s: %d hours\n", "Run Time", uttlicense->runtime);
	}
	/*
	 * print license impower ip or not
	 * check ip or not
	 */
	if(uttlicense->includeip)
	{
	    sprintf(iptmp,"%d.%d.%d.%d",(uttlicense->ip&0xff000000)>>24,(uttlicense->ip&0x00ff0000)>>16,(uttlicense->ip&0x0000ff00)>>8,(uttlicense->ip&0x000000ff));
	    printf("%20s:%s\n", "IP", iptmp);
	    if(uttlicense->checkip)
	    {
		printf("%20s: %s\n", "checkip", "Yes");
	    }
	    else
	    {
		printf("%20s: %s\n", "checkip", "No");
	    }
	}
	else
	{
	    printf("%20s: null\n", "IP");
	}
	/*
	 * print license impower mac or not 
	 * check mac or mot 
	 */
	if(uttlicense->includemac)
	{
	    printf("%20s: %s\n", "MAC", uttlicense->mac);
	    if(uttlicense->checkmac)
	    {
		printf("%20s: %s\n", "checkmac", "Yes");
	    }
	    else
	    {
		printf("%20s: %s\n", "checkmac", "No");
	    }
	}
	else
	{
	    printf("%20s: null\n","MAC");
	}
	/*
	 * print license impower userinfor or not 
	 * check userinfo or not
	 */
	if(uttlicense->includeuser)
	{
	    printf("%20s: %s\n", "UserInfo", uttlicense->userinfor);
	    if(uttlicense->checkuser)
	    {
		printf("%20s: %s\n", "checkuserinfo", "Yes");
	    }
	    else
	    {
		printf("%20s: %s\n", "checkuserinfo", "No");
	    }
	}
	else
	{
	    printf("%20s: null\n", "UserInfo");
	}
    
    } else if(strcmp(argv[1], "status")==0) {
	
	char iptmp[17]={0};

	uttlicense = (LicenseProfile*)ProfGetInstPointByIndex(mibtype, 0);
	//printf("License Status\n");
	/*
	 * print license type
	 * timecontrol
	 * functioncontrol
	 * or platcontrol
	 */
	if(uttlicense->controltype == TIME_CONTROL)
	{
	    printf("%20s: %s\n", "License Type", "timeLicense");
	}
	if(uttlicense->controltype == FUNCTION_CONTROL)
	{
	    printf("%20s: %s\n", "License Type", "featureLicense");
	}
	if(uttlicense->controltype == TERRACE_CONTROL)
	{
	    printf("%20s: %s\n", "License Type", "platformLicense");
	}
	printf("%20s \n", "LicenseStat");
	/*
	 * print license status
	 * forever or tmp
	 */
	if(uttlicense->timeflag == FOREVER)
	{
	    printf("%20s: %s\n", "License Stat", "Forever");
	}
	if(uttlicense->timeflag == EFFECTIVE)
	{
	    printf("%20s: %s\n", "License Stat", "Temporary");
	    printf("%20s: %d hours\n", "Impower Time", uttlicense->impowerTime * 30 * 24);
	    printf("%20s: %d hours\n", "Run Time",uttlicense->runtime);
	    printf("%20s: %d hours\n", "Remain Time",uttlicense->impowerTime * 30 * 24 - uttlicense->runtime);
	}

	printf("%20s \n", "UserInfo");
	/*
	 * print license impower ip or not
	 * check ip or not
	 */
	if(uttlicense->includeip)
	{
	    sprintf(iptmp,"%d.%d.%d.%d",(uttlicense->ip&0xff000000)>>24,(uttlicense->ip&0x00ff0000)>>16,(uttlicense->ip&0x0000ff00)>>8,(uttlicense->ip&0x000000ff));
	    printf("%20s: %s ", "IP", iptmp);
	    if(uttlicense->checkip)
	    {
		printf("%20s: %s\n", "checkip", "Yes");
	    }
	    else
	    {
		printf("%20s: %s\n", "checkip", "No");
	    }
	}
	else
	{
	    printf("%20s: null\n", "IP");
	}
	/*
	 * print license impower mac or not 
	 * check mac or not 
	 */
	if(uttlicense->includemac)
	{
	    printf("%20s: %s", "MAC", uttlicense->mac);
	    if(uttlicense->checkmac)
	    {
		printf("%20s: %s\n", "checkmac", "Yes");
	    }
	    else
	    {
		printf("%20s: %s\n", "checkmac", "No");
	    }
	}
	else
	{
	    printf("%20s: null\n", "MAC");
	}
	/*
	 * print license impower userinfor or not
	 * check userinfor or not 
	 */
	if(uttlicense->includeuser)
	{	
	    printf("%20s: %s", "UserInfo", uttlicense->userinfor);
	    if(uttlicense->checkuser)
	    {
		printf("%20s: %s\n", "checkuserinfo", "Yes");
	    }
	    else
	    {
		printf("%20s: %s\n", "checkuserinfo", "No");
	    }
	}
	else
	{
	    printf("%20s: null\n", "UserInfo");
	}
    } else {
	usage(argv[0]);
    }
	
    ProfDetach();
    exit(0);
}
#else
int main(int argc, char** argv)
{
    exit(0);
}
#endif
