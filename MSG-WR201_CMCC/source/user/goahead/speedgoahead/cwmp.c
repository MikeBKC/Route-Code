#include <stdlib.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "comExeFun.h"
#include "flash_api.h"
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <dirent.h>

//#define SWORD_DEBU 1
#ifdef SWORD_DEBU
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#if (CWMP == FYES)
#define WIFI_READ_BUF_SIZE 1024
static char* getSystemInfoSysVersion()
{   
    static char version[50] = {0};

    if(strlen(version) != 0)
        goto OUT;

    char_t *tmp = NULL;
    struct utsname utsbuf;
    uname(&utsbuf);
    tmp = strtok(utsbuf.version, " ");
    tmp = strtok(NULL, " ");

	snprintf(version,sizeof(version),"%s%s", utsbuf.version, tmp);
OUT:
    return version;
}
static char* getSystemInfoMachineNo()
{   
    char_t *tmp = NULL;
    struct utsname utsbuf;

    static char productID[20] = {0};

    if(strlen(productID) != 0)
        goto OUT;

    uname(&utsbuf);
    tmp = strtok(utsbuf.machine, " ");
    tmp = strtok(NULL, " ");

	if(tmp != NULL)
	{
        sprintf(productID,"%s",tmp);
	}
	else
	{
        snprintf(productID,sizeof(productID),"%s",utsbuf.machine);
	}
OUT:
    return productID;
}
void cwmpConfFileConf(CwmpProfile *prof)
{
    char *file = "/etc/cwmp.conf";
    FILE *fp;

    unsigned int serno = 0;
    serno = getSystemSn();
    //doSystem("cp /etc/cwmp.conf.bak %s", file);
    doSystem("rm /etc/cwmp.conf -fr");
    fp = fopen(file, "a");
    fseek(fp,0,SEEK_SET);

    if (fp != NULL)
    {
#if 0
        fprintf(fp, "acs_username=%s\n", prof->username);
        fprintf(fp, "acs_password=%s\n\n", prof->password);
        fprintf(fp, "acs_url=http://%s:%d/mcp/tr069Servlet\n", prof->serverIp, prof->managePort);
        fprintf(fp, "cpe_pc=RG-BCR800W\n");
        fprintf(fp, "cpe_sn=%u\n", serno);
        fprintf(fp, "\n\n\n");
        fprintf(fp, "[cwmpd]\n");
        fprintf(fp, "httpd_port=5400\n");
        fprintf(fp, "http_timeout=2\n\n");
#else
        fprintf(fp, "[cwmp]\n");
        fprintf(fp, "enable=%d\n",(prof->head.active == 1)?1:0);
        fprintf(fp, "soap_env=SOAP-ENV\n");
        fprintf(fp, "soap_enc=SOAP-ENC\n");
        fprintf(fp, "event_filename=/etc/cwmpevent.bin\n");
            
        fprintf(fp,"ca_file=/etc/ssl/private/ca.pem\n");
        fprintf(fp,"ca_password=123456\n");

#if (WECHAT_CWMP == FYES)
        fprintf(fp, "acs_url=%s\n", (strlen(prof->acsUrl) != 0)?prof->acsUrl:"http://acs.greenwifi.com.cn:80/wx/tr069");
#else
        fprintf(fp, "acs_url=%s\n", (strlen(prof->acsUrl) != 0)?prof->acsUrl:"http://acs.greenwifi.com.cn:80/ACS/tr069");
#endif
        fprintf(fp, "acs_period=%d\n", prof->periodicInformInterval);

        fprintf(fp, "stun_period=%d\n",prof->stunPeriodicInterval);
#if 1
        fprintf(fp, "stun_url=%s\n", (strlen(prof->stunUrl) != 0)?prof->stunUrl:"stun.greenwifi.com.cn:3748");
#else
        fprintf(fp, "stun_url=%s:%d\n", (strlen(prof->stunIp) != 0)?prof->stunIp:"77.72.174.161",prof->stunPort);
#endif

        /*
         * 是否开启认证
         * */
        fprintf(fp, "acs_auth=%d\n",(prof->acsAuthEnable == 1)?1:0);
        fprintf(fp, "acs_username=%s\n", (strlen(prof->acsUsername) != 0)?prof->cpeUsername:"cwmpau");
        fprintf(fp, "acs_password=%s\n", (strlen(prof->acsPassword) != 0)?prof->acsPassword:"cwmpap");
        fprintf(fp, "cpe_auth=%d\n",(prof->cpeAuthEnable == 1)?1:0);
        fprintf(fp, "cpe_username=%s\n", (strlen(prof->cpeUsername) != 0)?prof->cpeUsername:"cwmpcu");
        fprintf(fp, "cpe_password=%s\n", (strlen(prof->cpePassword) != 0)?prof->cpePassword:"cwmpcp");
        fprintf(fp, "stun_auth=%d\n",(prof->stunAuthEnable == 1)?1:0);
        fprintf(fp, "stun_username=%s\n", (strlen(prof->stunUsername) != 0)?prof->stunUsername:"stunsu");
        fprintf(fp, "stun_password=%s\n", (strlen(prof->stunPassword) != 0)?prof->stunPassword:"stunsp");

        /*
         * 一些本机参数,进程运行过程中不会改变
         * */
        fprintf(fp, "cpe_pc=%s\n",getSystemInfoMachineNo());
        fprintf(fp, "cpe_sn=%u\n", serno);
        fprintf(fp, "cpe_manufacture=UTT Technologies Co., Ltd.\n");
        fprintf(fp, "cpe_oui=0022AA\n");
        fprintf(fp, "cpe_name=%s\n",getSystemInfoMachineNo());
        fprintf(fp, "cpe_specver=V1.0\n");
        fprintf(fp, "cpe_hwver=%s\n",UTT_HWMODEL);
        fprintf(fp, "cpe_version=%s\n",getSystemInfoSysVersion());

        fprintf(fp, "\n\n\n");
        fprintf(fp, "[cwmpd]\n");
        fprintf(fp, "httpd_port=5400\n");
        fprintf(fp, "http_timeout=2\n\n");
#endif
        fclose(fp);
    }
    else
    {
        SWORD_PRINTF("cannot open file %s\n", file);
    }

    return;
}

void cwmpInit(void)
{
    MibProfileType mibType = MIB_PROF_CWMP;
    CwmpProfile *prof = NULL;

    prof = (CwmpProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (prof != NULL)
    {
       // doSystem("killall cwmpd");
        if (prof->head.active)
        {
            cwmpConfFileConf(prof);
        //    doSystem("cwmpd &");
        }
        else
        {
         //   doSystem("killall cwmpd");
        }

    }
    else
    {
        SWORD_PRINTF("cannot find cwmp profile.\n");
    }

    return;
}
void check_cwmpd_by_name( char* pidName)
{
    DIR *dir=NULL;
    struct dirent *next;
    long pidList[10];
    int i=0;
    int count = 0;

    dir = opendir("/proc");
    if (!dir)
    {
	return ;
    }
    memset(pidList, 0, sizeof(pidList));
    while ((next = readdir(dir)) != NULL)
    {   
	FILE *status=NULL;
	char filename[WIFI_READ_BUF_SIZE];
	char buffer[WIFI_READ_BUF_SIZE];
	char name[WIFI_READ_BUF_SIZE];
	/* Must skip "" since that is outside /proc */
	if (strcmp(next->d_name, "..") == 0)
	    continue;
	/* If it isn't a number， we don't want it */
	if (!isdigit(*next->d_name))
	    continue;
	sprintf(filename, "/proc/%s/status", next->d_name);
	if (! (status = fopen(filename, "r")) )
	{
	    continue;
	}
	//Read first line in /proc/?pid?/status
	if (fgets(buffer,WIFI_READ_BUF_SIZE-1, status) == NULL)
	{
	    fclose(status);
	    continue;
	}
	fclose(status);
	// Buffer should contain a string like "Name： binary_name"a
	sscanf(buffer,"%*s %s", name);
	if ( name != NULL && name[0] != '\0')                           
	{
	    if (strcmp(name, pidName) == 0)
	    {
		if(i<10) {
		    pidList[i]=strtol(next->d_name, NULL, 0);
		}
		i++;
	    }
	}
    }
    closedir(dir);
    count = i;
    if(count <= 0)
    {
	syslog(LOG_INFO, "UTT TR069: CWMPD  restarting...\n");
//	kill_process("cwmpd");
	sleep(1);
//	doSystem("cwmpd &");
    }

    return ;
}

static void cwmpProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const CwmpProfile *oldProfileDataPtr, const CwmpProfile *newProfileDataPtr )        
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    cwmpInit();
            break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }

    return;
}

void funInitCwmp(void)
{
    registerForProfileUpdates(MIB_PROF_CWMP, (Pointer)NULL, (ProfileUpdateFnc)cwmpProfileChange);/*触发函数*/
    cwmpInit();
}
#endif
