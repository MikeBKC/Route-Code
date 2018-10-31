#include "uttMachine.h"
#if (DDNS == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <syslog.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#include "ddns.h"

#ifdef DDNS_DBG
#define DDNS_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define DDNS_PRINTF(fmt, args...)
#endif

#define IP_LEN 15
#define FILE_LEN 30

static void AddDDNSRoute(DDNSProfile *prof)
{
    FILE *fp;
    char server_ip[IP_LEN+1];
    fp =fopen("/tmp/ddns/inadyn_ser_ip","r");
    if(fp != NULL){
	fscanf(fp,"%15s",server_ip);
	doSystem("config-Addnsroute.sh %s %s",server_ip,prof->ddnsIf);
	fclose(fp);
    }

}
extern int _getWanLinkState(int wanNo);
static int AddDDNS(DDNSProfile *prof)
{
    char conf[FILE_LEN+1],log[FILE_LEN+1];
    FILE *fp;
    int wanIndex = 1;
    /*
     * 供应商未配置
     */
    if ((strlen(prof->ddnsProvider) == 0) || (strcmp(prof->ddnsProvider, "none") == 0)) {
	DDNS_PRINTF("%s: provider=%s\n", __FUNCTION__, prof->ddnsProvider);
	return 0;
    }

    if (strncmp(prof->ddnsIf, "WAN", 3) != 0) {
	DDNS_PRINTF("%s: error if. if=%s\n", __FUNCTION__, prof->ddnsIf);
	return 0;
    }
    wanIndex = prof->ddnsIf[3] - '0';
    if(_getWanLinkState(wanIndex)==0)
    {/*wan口未连接*/
	return 0;
    }
    snprintf(conf,FILE_LEN,"/etc/ddns_%s.conf",prof->ddnsIf);
    snprintf(log,FILE_LEN,"/tmp/ddns/ddns_%s.log",prof->ddnsIf);

    if((strncmp(prof->ddnsProvider, "3322.org",8) == 0)
#if (DDNS_DYNDNS == FYES)
    ||(strncmp(prof->ddnsProvider, "dyndns.org",10) == 0)
#endif
	){

        doSystem("rm -rf /tmp/ddns/inadyn%s",prof->ddnsIf);
	doSystem("mkdir /tmp/ddns/inadyn%s",prof->ddnsIf);
        fp=fopen(conf,"w+");
        if(!fp) {
            return -1;
        }
        fprintf(fp,"--background --dyndns_system dyndns%c%s ", 64, prof->ddnsProvider);
        fprintf(fp,"-u %s ", prof->ddnsAccount);
        fprintf(fp,"-p %s ", prof->ddnsPasswd);
        fprintf(fp,"-a %s ", prof->ddnsHost);
        fprintf(fp,"--update_period_sec 360 --forced_update_period 864000 \
--log_file %s --cache_dir /tmp/ddns/inadyn%s --exec ddns_success",log,prof->ddnsIf);
        fclose(fp);
	doSystem("inadyn --input_file %s",conf);
        sleep(1);
	AddDDNSRoute(prof);
#if(DDNS_NO_IP == FYES)
	}else if(strncmp(prof->ddnsProvider, "no-ip.com",9) == 0){
	    doSystem("mkdir /tmp/ddns/inadyn%s",prof->ddnsIf);
	    fp=fopen(conf,"w+");
		if(!fp) {
			return -1;
		}
		fprintf(fp,"--background --dyndns_system default%c%s ", 64, prof->ddnsProvider);
		fprintf(fp,"-u %s ", prof->ddnsAccount);
		fprintf(fp,"-p %s ", prof->ddnsPasswd);
		fprintf(fp,"-a %s ", prof->ddnsHost);
		fprintf(fp,"--update_period_sec 360 --forced_update_period 864000 \
--log_file %s --cache_dir /tmp/ddns/inadyn%s --exec ddns_success",log,prof->ddnsIf);
		fclose(fp);
		doSystem("rm -rf /tmp/ddns/inadyn%s",prof->ddnsIf);
		doSystem("inadyn --input_file %s",conf);
		sleep(1);
//		fp =fopen("/tmp/ddns/inadyn_ser_ip","r");
//		fscanf(fp,"%s",server_ip);
//		doSystem("config-ddnsroute.sh %s %s",server_ip,prof->ddnsIf);
//		printf("config-ddnsroute.sh %s %s\n",server_ip,prof->ddnsIf);
//		fclose(fp);

		AddDDNSRoute(prof);
#endif
	} else if(strncmp(prof->ddnsProvider, "iplink.com.cn",13) == 0) {
        doSystem("iplink %s %s %s&", getWanIfNamePPP(wanIndex), prof->ddnsHost, prof->ddnsPasswd);
	} else if (strncmp(prof->ddnsProvider, "www.oray.net",12) == 0) {
	    fp=fopen(conf,"w+");
	    if(!fp) {
		return -1;
	    }
	    fprintf(fp, "[settings]\n");
	    fprintf(fp, "szHost = PhLinux3.Oray.Net\n");
	    fprintf(fp, "szUserID = %s\n", prof->ddnsAccount);
	    fprintf(fp, "szUserPWD = %s\n", prof->ddnsPasswd);
	    fprintf(fp, "nicName = %s\n", getWanIfNamePPP(wanIndex));
	    fprintf(fp, "szLog = /var/log/phddns_%s.log",prof->ddnsIf);
	    fclose(fp);
	    doSystem("/bin/phddns -i %s -c %s -d",prof->ddnsIf,conf);
    }

    return 0;
}

#if (DDNS_ADVANCE == FYES)

static void DDNSDelAll(void);
extern int init_DDNS(void)
{
    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;
    int min = 0, max = 0, index = 0;
    FILE *fp;
/*    doSystem("killall -q inadyn");
    doSystem("killall -q iplink");
    doSystem("killall -q phddns");
    doSystem("rm /etc/ddns.conf 2>/dev/null");
    doSystem("rm -f /tmp/ddns/inadyn*>/dev/null");
    doSystem("rm -f /tmp/ddns/phddns_*.cache");
    doSystem("rm -f /var/log/phddns*.log");
    doSystem("ip route flush table 100");
*/
    DDNSDelAll();
    ProfInstLowHigh(mibType, &max, &min);
    for(index=min;index<max;index++){
	prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, index);
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0) &&(prof->head.active == 1)){
	    AddDDNS(prof);
	}
    }
}
#else
extern int init_DDNS(void)
{
    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;

    FILE *fp;
    int wanIndex = 1;
    char tmp[50] ={0};
    unsigned int sn = 0;
    doSystem("killall -q inadyn");
    doSystem("killall -q iplink");
    doSystem("killall -q phddns");
#if (DDNS_UTTCARE == FYES)
    doSystem("killall -q uttcare");
    doSystem("rm -f /tmp/ddns/ddns_uttcare.log");
#endif
    doSystem("rm /etc/ddns.conf 2>/dev/null");
    doSystem("rm -f /tmp/ddns/phddns_*.cache");
    doSystem("rm -f /var/log/phddns.log");
#if 0
    ddns_provider=nvram_bufget(RT2860_NVRAM,"DDNSProvider");
    Profiles=nvram_bufget(RT2860_NVRAM,"DDNSProfiles");
#endif
    prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof == NULL) {
	DDNS_PRINTF("%s: ddnsProfile get error\n", __FUNCTION__);
	return 0;
    }

    /*
     * 供应商未配置
     */
    if ((strlen(prof->ddnsProvider) == 0) || (strcmp(prof->ddnsProvider, "none") == 0)) {
	DDNS_PRINTF("%s: provider=%s\n", __FUNCTION__, prof->ddnsProvider);
	return 0;
    }

    /*
     * 接口配置错误
     */
#if 0
    /* mantis 8410 */
    if ((strcmp(prof->ddnsIf, "WAN1") != 0) && (strcmp(prof->ddnsIf, "WAN2") != 0)) {
	DDNS_PRINTF("%s: error if. if=%s\n", __FUNCTION__, prof->ddnsIf);
	return 0;
    }
#endif
    if (strncmp(prof->ddnsIf, "WAN", 3) != 0) {
	DDNS_PRINTF("%s: error if. if=%s\n", __FUNCTION__, prof->ddnsIf);
	return 0;
    }
    wanIndex = prof->ddnsIf[3] - '0';
    if(_getWanLinkState(wanIndex)==0)
    {/*wan口未连接*/
	return 0;
    }

#if 0
    if( (ddns_provider==NULL) ||(strcmp(ddns_provider,T("none"))==0) ) {
	return 0;
    }
#endif

    if((strcmp(prof->ddnsProvider, "3322.org") == 0)
#if (DDNS_DYNDNS == FYES)
    ||(strcmp(prof->ddnsProvider, "dyndns.org") == 0)
#endif
	){
        fp=fopen("/etc/ddns.conf","w+");
        if(!fp) {
            return -1;
        }
        fprintf(fp,"--background --dyndns_system dyndns%c%s ", 64, prof->ddnsProvider);
        fprintf(fp,"-u %s ", prof->ddnsAccount);
        fprintf(fp,"-p %s ", prof->ddnsPasswd);
        fprintf(fp,"-a %s ", prof->ddnsHost);
        fprintf(fp,"--update_period_sec 360 --forced_update_period 864000 \
--log_file /tmp/ddns/ddns.log --cache_dir /tmp/ddns --exec ddns_success");
        fclose(fp);
        doSystem("rm -f /tmp/ddns/inadyn_*");
        doSystem("inadyn --input_file /etc/ddns.conf");
#if(DDNS_NO_IP == FYES)
	}else if(strcmp(prof->ddnsProvider, "no-ip.com") == 0){
		fp=fopen("/etc/ddns.conf","w+");
		if(!fp) {
			return -1;
		}
		fprintf(fp,"--background --dyndns_system default%c%s ", 64, prof->ddnsProvider);
		fprintf(fp,"-u %s ", prof->ddnsAccount);
		fprintf(fp,"-p %s ", prof->ddnsPasswd);
		fprintf(fp,"-a %s ", prof->ddnsHost);
		fprintf(fp,"--update_period_sec 360 --forced_update_period 864000 \
--log_file /tmp/ddns/ddns.log --cache_dir /tmp/ddns --exec ddns_success");
		fclose(fp);
		doSystem("rm -f /tmp/ddns/inadyn_*");
		doSystem("inadyn --input_file /etc/ddns.conf");
#endif
	} else if(strcmp(prof->ddnsProvider, "iplink.com.cn") == 0) {
        doSystem("iplink %s %s %s&", getWanIfNamePPP(wanIndex), prof->ddnsHost, prof->ddnsPasswd);
	} else if (strcmp(prof->ddnsProvider, "www.oray.net") == 0) {
	    fp=fopen("/etc/ddns.conf","w+");
	    if(!fp) {
		return -1;
	    }
	    fprintf(fp, "[settings]\n");
	    fprintf(fp, "szHost = PhLinux3.Oray.Net\n");
	    fprintf(fp, "szUserID = %s\n", prof->ddnsAccount);
	    fprintf(fp, "szUserPWD = %s\n", prof->ddnsPasswd);
	    fprintf(fp, "nicName = %s\n", getWanIfNamePPP(wanIndex));
	    fprintf(fp, "szLog = /var/log/phddns.log");
	    fclose(fp);
	    doSystem("/bin/phddns -c /etc/ddns.conf -d");
#if (DDNS_UTTCARE == FYES)
	} else if (strcmp(prof->ddnsProvider, "uttcare.com") == 0) {
	    sn = getSystemSn();
	    sprintf(tmp,"%u.uttcare.com",sn);
	    if(strcmp(prof->ddnsHost,tmp) == 0)
	    {
		doSystem("uttcare %s %s&", getWanIfNamePPP(wanIndex), prof->ddnsHost);
	    }
	    else
	    {
		doSystem("uttcare %s %s %s&", getWanIfNamePPP(wanIndex), prof->ddnsHost, prof->ddnsPasswd);
	    }
#endif
	}

    return 0;
}
#endif
/********************************************************************
 * * 函数： DDNSDel
 * * 功能： IPv6删除一条路由 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void DDNSDel(DDNSProfile *prof)
{
    char pidfile[30];
    char pid[6];
    FILE *fp;
    int wanIndex = 1;
    if(strcmp(prof->ddnsProvider, "www.oray.net") == 0){
	sprintf(pidfile,"/var/run/ddns_%s.pid",prof->ddnsIf);
    
	fp = fopen(pidfile,"r");
	if(fp == NULL){
	    return;
	}
	fscanf(fp,"%s",pid);
	doSystem("kill %s",pid);   
	doSystem("rm /etc/ddns_%s.conf 2>/dev/null",prof->ddnsIf);
	doSystem("rm -f /tmp/ddns/phddns_*_%s.cache",prof->ddnsIf);
	doSystem("rm -f /var/log/phddns_%s.log",prof->ddnsIf);
	doSystem("rm -f /var/run/phddns_%s.pid",prof->ddnsIf);
        fclose(fp);
    }
    else if(strcmp(prof->ddnsProvider, "iplink.com.cn") == 0){
	wanIndex=prof->ddnsIf[3]-'0';
/*	sprintf(pidfile,"/var/run/iplink_%s.pid",getWanIfNamePPP(wanIndex));

	fp = fopen(pidfile,"r");
	if(fp == NULL){
	    return;
	}
	fscanf(fp,"%s",pid);
	doSystem("kill %s",pid);   
	doSystem("rm -f /tmp/ddns/ddns_iplink_%s.log",getWanIfNamePPP(wanIndex));
	doSystem("rm -f /var/run/iplink_%s.pid",getWanIfNamePPP(wanIndex));
	fclose(fp);   
*/
	doSystem("killall iplink");   
        doSystem("rm -f /tmp/ddns/ddns_iplink_%s.log",getWanIfNamePPP(wanIndex));
    }

    else if ((strcmp(prof->ddnsProvider, T("3322.org")) == 0)
#if (DDNS_NO_IP == FYES)
	    || (strcmp(prof->ddnsProvider, T("no-ip.com")) == 0)
#endif
#if (DDNS_DYNDNS == FYES)
	    || (strcmp(prof->ddnsProvider, T("dyndns.org")) == 0)
#endif
	    ){  
	sprintf(pidfile,"/var/run/ddns_%s.pid",prof->ddnsIf);
    
	fp = fopen(pidfile,"r");
	if(fp == NULL){
	    return;
	}
	fscanf(fp,"%s",pid);
	doSystem("kill %s",pid);   
        fclose(fp);
	doSystem("rm -rf /tmp/ddns/inadyn%s",prof->ddnsIf);
	doSystem("rm -rf /tmp/ddns/ddns_%s.log",prof->ddnsIf);
	doSystem("rm -f /etc/ddns_%s.conf",prof->ddnsIf);
    }
}

static void EditDDNS(DDNSProfile *profnew,DDNSProfile *profold)
{
    DDNSDel(profold);
    AddDDNS(profnew);    
}

/********************************************************************
 * * 函数： DDNSDelAll
 * * 功能： IPv6静态路由表清空 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang                      还需修改
 * ********************************************************************/

static void DDNSDelAll()
{
    doSystem("killall -q inadyn");
    doSystem("killall -q iplink");
    doSystem("killall -q phddns");
    doSystem("rm /etc/ddns*.conf 2>/dev/null");
    doSystem("rm -f /tmp/ddns/phddns_*.cache>/dev/null");
    doSystem("rm /tmp/ddns/inadyn_ser_ip>/dev/null");
    doSystem("rm -rf /tmp/ddns/inadyn*");
    doSystem("rm -f /tmp/ddns/ddns*.log");
    doSystem("rm -f /var/log/phddns*.log");
    doSystem("rm -f /var/log/ddns_iplink*.log");
    doSystem("ip route flush table 100"); 
}

extern void Do_DDNS_By_Interface(char *interface,char *action)     //接口变化时调用该函数
{
    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;
    int min = 0, max = 0, index = 0;
    ProfInstLowHigh(mibType, &max, &min);
    for(index=min;index<max;index++){
	prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, index);
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0) &&(prof->head.active == 1) && (strcmp(prof->ddnsIf,interface) == 0)){
	    if(strcmp(action,"del") == 0){
		DDNSDel(prof);  
	    }else{
		AddDDNS(prof);
	    }
	}
    }
}

/*
 * ddns配置变更回调函数
 */
static void ddnsProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    DDNS_PRINTF("%s\n", __FUNCTION__);

  switch(changeType) { 
        case PROF_CHANGE_ADD:
#if (DDNS_ADVANCE == FYES)
	    AddDDNS((DDNSProfile *)newProfileDataPtr);
#endif
            break;
        case PROF_CHANGE_EDIT:
#if(DDNS_ADVANCE == FYES)
	    EditDDNS((DDNSProfile *)newProfileDataPtr,(DDNSProfile *)oldProfileDataPtr);
#else
	    init_DDNS();
#endif
	    break;
        case PROF_CHANGE_DEL:

#if (DDNS_ADVANCE == FYES)
//            init_DDNS();
	    DDNSDel((DDNSProfile *)oldProfileDataPtr);
#endif
	    break;
	case PROF_CHANGE_DELALL:
#if (DDNS_ADVANCE == FYES)
	    DDNSDelAll();
#endif
            break;
        default:
            break;
    };
    return;
}

/*
 * 初始化
 */
extern void funInitDdns(void) {
    registerForProfileUpdates(MIB_PROF_DDNS, NULL, ddnsProfileChange);
    return;
}

#endif
