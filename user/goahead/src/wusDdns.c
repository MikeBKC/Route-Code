#include "uttMachine.h"
#if (DDNS == FYES)
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"translate.h"
#define DDNS_DBG 1
#ifdef DDNS_DBG
#define DDNS_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define DDNS_PRINTF(fmt, args...)
#endif

#define DDNS_IPLINK_LOG "/tmp/ddns/ddns_iplink.log"
#define DDNS_UTTCARE_LOG "/tmp/ddns/ddns_uttcare.log"

#define CACHE_FILE_LEN 40
#define LOG_FILE_LEN   35
#define IP_LEN         15
#define TIME_LEN       20 
#define DOMAIN_LEN     30

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

#if (DDNS_ADVANCE == FYES)
/***********************************************************************
 * 函 数 名：   formDdnsConfig
 * 功能描述：	web调用保存DDNS配置的form函数
 * 输入参数:    wp,path,query
 * 输出参数：	DDNS配置 
 * 返回值：	无 
 * 创建日期：	2011-10-26
 * 修改日期：
 * 附加说明：	无
***********************************************************************/
static void formDdnsConfig(webs_t wp, char_t * path, char_t * query)
{
    MibProfileType mibType = MIB_PROF_DDNS;
    struProfAlloc *profhead = NULL;
    DDNSProfile *prof = NULL;
    DDNSProfile *_prof = NULL;
    int min,max,index;
    ProfChangeType profAction;
    int instIndex;
    char *ddns_provider = NULL, *ddns = NULL, *ddns_acc = NULL, *ddns_pass = NULL,*Profiles = NULL,*action = NULL;
    const char *errMsg=NULL;
    action = websGetVar(wp, T("Action"), "");
    ddns_provider = websGetVar(wp, T("DDNSProvider"), T("none"));
    ddns = websGetVar(wp, T("DDNS"), T(""));
    ddns_acc = websGetVar(wp, T("Account"), T(""));
    ddns_pass = websGetVar(wp, T("Password"), T(""));
    Profiles = websGetVar(wp, T("Profile"), T(""));

    ProfInstLowHigh(mibType, &max, &min);

    DDNS_PRINTF("%s:provider=%s,host=%s,acc=%s,passwd=%s,if=%s\n", 
	    __FUNCTION__, ddns_provider, ddns, ddns_acc, ddns_pass, Profiles);

    if ((ddns_provider == NULL) || (ddns == NULL) || (ddns_acc == NULL) || (ddns_pass == NULL) || (Profiles == NULL)) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    instIndex = ProfGetInstIndexByName(mibType, Profiles);
    for(index=min;index<max;index++)
    {
	_prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, index);
	if((_prof != NULL) && (ProfInstIsFree(_prof) == 0) && (index != instIndex))
	{
	    if((strcmp(_prof->ddnsProvider,ddns_provider)==0)&&(strcmp(_prof->ddnsAccount,ddns_acc)==0))
	    {
//		errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
                errMsg = "用户名配置重复，请修改账号";
		setTheErrorMsg(errMsg);
		goto out;

	    } 
	}
    }

    if (strncmp(action, "add",3) == 0) {
        if(instIndex != PROFFAIL) {
            errMsg = getTransDataForC(C_LANG_INDEX_DDNS_INTERFACE_REPEAT);
            setTheErrorMsg(errMsg);
            goto out;
        }   

        instIndex = ProfNewInst(mibType, Profiles, FALSE);

        if(instIndex == PROFFAIL) {
            errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
            setTheErrorMsg(errMsg);
            goto out;
        }
        profAction = PROF_CHANGE_ADD;
    } else {
        if(instIndex == PROFFAIL) {
            errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
            setTheErrorMsg(errMsg);
            goto out;
        }
        profAction = PROF_CHANGE_EDIT;
    }

    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);

    prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, instIndex);
    if (prof == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
	setTheErrorMsg(errMsg);
	goto out;
    }
    
    ProfSetNameByPoint(prof, Profiles);
    strncpy(prof->ddnsProvider, ddns_provider, UTT_DDNS_PROVIDER_LEN);
    strncpy(prof->ddnsHost, ddns, UTT_DDNS_HOST_LEN);
    strncpy(prof->ddnsAccount, ddns_acc, UTT_DDNS_ACCOUNT_LEN);
    strncpy(prof->ddnsPasswd, ddns_pass, UTT_DDNS_PASSWD_LEN);
    strncpy(prof->ddnsIf, Profiles, UTT_INTERFACE_LEN);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#if 0
	nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
	nvram_bufset(RT2860_NVRAM, "DDNS", ddns);
	nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
	nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
	nvram_bufset(RT2860_NVRAM, "DDNSProfiles", Profiles);
	nvram_commit(RT2860_NVRAM);
	
        sendMsgForSpeed(INIT_DDNS,0,0,NULL,0);
#endif
    doSystem("sleep 1");	//等待ddns生效i
out:
    websRedirect(wp, "DDNSAdvance.asp");
    return;
}
#else

/***********************************************************************
 * 函 数 名：   formDdnsConfig
 * 功能描述：	web调用保存DDNS配置的form函数
 * 输入参数:    wp,path,query
 * 输出参数：	DDNS配置 
 * 返回值：	无 
 * 创建日期：	2011-10-26
 * 修改日期：
 * 附加说明：	无
***********************************************************************/
static void formDdnsConfig(webs_t wp, char_t * path, char_t * query)
{
    MibProfileType mibType = MIB_PROF_DDNS;
    struProfAlloc *profhead = NULL;
    DDNSProfile *prof = NULL;
    ProfChangeType profAction;

    char *ddns_provider = NULL, *ddns = NULL, *ddns_acc = NULL, *ddns_pass = NULL,*Profiles = NULL;
    const char *errMsg=NULL;

    ddns_provider = websGetVar(wp, T("DDNSProvider"), T("none"));
    ddns = websGetVar(wp, T("DDNS"), T(""));
    ddns_acc = websGetVar(wp, T("Account"), T(""));
    ddns_pass = websGetVar(wp, T("Password"), T(""));
    Profiles = websGetVar(wp, T("Profile"), T("WAN1"));

    DDNS_PRINTF("%s:provider=%s,host=%s,acc=%s,passwd=%s,if=%s\n", 
	    __FUNCTION__, ddns_provider, ddns, ddns_acc, ddns_pass, Profiles);

    if ((ddns_provider == NULL) || (ddns == NULL) || (ddns_acc == NULL) || (ddns_pass == NULL) || (Profiles == NULL)) {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_PARAMETER_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibType, profAction, 0, &profhead);

    prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
	setTheErrorMsg(errMsg);
	goto out;
    }

    strncpy(prof->ddnsProvider, ddns_provider, UTT_DDNS_PROVIDER_LEN);
    strncpy(prof->ddnsHost, ddns, UTT_DDNS_HOST_LEN);
    strncpy(prof->ddnsAccount, ddns_acc, UTT_DDNS_ACCOUNT_LEN);
    strncpy(prof->ddnsPasswd, ddns_pass, UTT_DDNS_PASSWD_LEN);
    strncpy(prof->ddnsIf, Profiles, UTT_INTERFACE_LEN);

    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
#if 0
	nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
	nvram_bufset(RT2860_NVRAM, "DDNS", ddns);
	nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
	nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
	nvram_bufset(RT2860_NVRAM, "DDNSProfiles", Profiles);
	nvram_commit(RT2860_NVRAM);
	
        sendMsgForSpeed(INIT_DDNS,0,0,NULL,0);
#endif
    doSystem("sleep 1");	//等待ddns生效i
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "DDNS_Sui.asp");
#else
    websRedirect(wp, "DDNS.asp");
#endif
    return;
}
#endif

/***********************************************************************
 * 函 数 名：	aspOutputDdnsConfigList
 * 功能描述：	输出配置和信息给DDNS页面
 * 输入参数:    wp,eid,argc,argv
 * 输出参数：	输出配置
 * 返回值：	0
 * 创建日期：	2011-10-26
 * 修改日期：
 * 附加说明：	无
***********************************************************************/
static int aspOutputDdnsConfigList(int eid, webs_t wp, int argc, char_t ** argv)
{
    char_t buffer[IP_LEN+1] = {0}, time[TIME_LEN+1] = {0},domain[DOMAIN_LEN+1];
    const char *errMsg=NULL;
    FILE *fp;
    long ltime;
    struct tm *p;
    int min,max,index=0;
    int num=0;
    int wanIndex = 1;
    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;
    char ip_cache[CACHE_FILE_LEN+1],domain_cache[CACHE_FILE_LEN+1],time_cache[CACHE_FILE_LEN+1];
    char logfile[LOG_FILE_LEN+1];
    ProfInstLowHigh(mibType, &max, &min);
    num=min;
    for(index=min;index<max;index++){
	prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, index);
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0)){
	    websWrite(wp, "DDNSs[%d]=\"%s\";\n",num, prof->ddnsHost);
	    websWrite(wp, "Profiles[%d]=\"%s\";\n",num, prof->ddnsIf);
	    websWrite(wp, "ddns_ips[%d]=\"\";",num);
	    websWrite(wp, "Connstatus[%d]=\"未连接\";",num);
	    websWrite(wp, "ddns_times[%d]=\"\";",num);
#if(DDNS_UTTCARE == FYES)
		websWrite(wp, " DDNS_UTTCARE_NUM=\"%d\";",1);
#else
		websWrite(wp, " DDNS_UTTCARE_NUM=\"%d\";",0);
#endif

	    if ((strncmp(prof->ddnsProvider, T("3322.org"),8) == 0)
#if (DDNS_NO_IP == FYES)
		    || (strncmp(prof->ddnsProvider, T("no-ip.com"),9) == 0)
#endif
#if (DDNS_DYNDNS == FYES)
		    || (strncmp(prof->ddnsProvider, T("dyndns.org"),10) == 0)
#endif
	       ){
		memset(ip_cache, 0, sizeof(ip_cache));
		memset(domain_cache, 0, sizeof(ip_cache));
		memset(time_cache, 0, sizeof(ip_cache));
                snprintf(ip_cache,CACHE_FILE_LEN,"/tmp/ddns/inadyn%s/inadyn_ip.cache",prof->ddnsIf);
                snprintf(time_cache,CACHE_FILE_LEN,"/tmp/ddns/inadyn%s/inadyn_time.cache",prof->ddnsIf);

		fp = fopen(ip_cache, "r");
		if (!fp) {
		    num++;
		    continue;
		}
		if (fscanf(fp, "%15s", buffer) == 1) {
		   AddDDNSRoute(prof); 
		    websWrite(wp, "ddns_ips[%d]=\"%s\";",num, buffer);
		    websWrite(wp, "Connstatus[%d]=\"已连接\";",num);
		} else {
//		    websWrite(wp, "ddns_ips[%d]=\"%s\";",index, T(""));
//		    websWrite(wp, "Connstatus[%d]=\"未连接\";",num);
		}
		fclose(fp);
		fp = fopen(time_cache, "r");
		if (!fp) {
		    num++;
		    continue;
		}
		if (fscanf(fp, "%ld", &ltime) == 1) {
#if 0
		    ltime += atoi(nvram_bufget(RT2860_NVRAM, "TIMEZONE"));
#endif
		    p = localtime(&ltime);
		    sprintf(time, "%d/%d/%d %02d:%02d:%02d", 
			    (1900 + p->tm_year),
			    (1 + p->tm_mon), p->tm_mday,
			    p->tm_hour, p->tm_min, p->tm_sec);
		    websWrite(wp, "ddns_times[%d]=\"%s\";",num, time);
		} else {
//		    websWrite(wp, "ddns_times[%d]=\"%s\";",num, T(""));
		}
		fclose(fp);
	    } else if (strncmp(prof->ddnsProvider, T("iplink.com.cn"),13) == 0) {
                wanIndex = prof->ddnsIf[3] - '0';
		snprintf(logfile,LOG_FILE_LEN,"/tmp/ddns/ddns_iplink_%s.log",getWanIfNamePPP(wanIndex));
		fp = fopen(logfile, "r");
		if (!fp) {
		    num++;
		    continue;
		}
		fscanf(fp, "%s\n", buffer);
		if (strncmp(buffer, "success",7)) {
		    num++;
		    continue;
		}
		if (fscanf(fp, "%ld\n", &ltime) == 1) {
#if 0
		    ltime += atoi(nvram_bufget(RT2860_NVRAM, "TIMEZONE"));
#endif
		    p = localtime(&ltime);
		    sprintf(time, "%d/%d/%d %02d:%02d:%02d", 
		    (1900 + p->tm_year),
		    (1 + p->tm_mon), p->tm_mday,
		    p->tm_hour, p->tm_min, p->tm_sec);
		    websWrite(wp, "ddns_times[%d]=\"%s\";",num, time);
		} else {
//		    websWrite(wp, "ddns_times[%d]=\"%s\";",num, T(""));
		}
		if (fscanf(fp, "%15s", buffer) == 1) {
		    websWrite(wp, "ddns_ips[%d]=\"%s\";",num, buffer);
		    websWrite(wp, "Connstatus[%d]=\"已连接\";",num);
		} else {
//		    websWrite(wp, "ddns_ips[%d]=\"%s\";",num, T(""));
//		    websWrite(wp, "Connstatus[%d]=\"未连接\";",num);
		}
	    } else if (strncmp(prof->ddnsProvider, T("www.oray.net"),12) == 0) {
		if (strncmp(prof->ddnsIf, T("WAN1"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_domains_WAN1.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN2"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_domains_WAN2.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN3"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_domains_WAN3.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN4"),4) == 0){
			fp = fopen("/tmp/ddns/phddns_domains_WAN4.cache", "r");
		}else{
		    errMsg = "绑定接口错误";
		    setTheErrorMsg(errMsg);
		    goto out;
		}
		if (!fp){
		    num++;
		    continue;
		}
		if (fscanf(fp, "%30s\n", domain) == 1) 
		    websWrite(wp, "DDNSs[%d]=\"%s\";",num, domain);
		fclose(fp);

		if (strncmp(prof->ddnsIf, T("WAN1"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_ip_WAN1.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN2"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_ip_WAN2.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN3"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_ip_WAN3.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN4"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_ip_WAN4.cache", "r");
		}else{
		    errMsg = getTransDataForC(C_LANG_INDEX_DDNS_INTERFACE_ERROR);
		    setTheErrorMsg(errMsg);
		    goto out;
		}

	//	fp = fopen("/tmp/ddns/phddns_ip.cache", "r");
		if (!fp) {
		    num++;
		    continue;
		}
		if (fscanf(fp, "%15s", buffer) == 1) {
		    websWrite(wp, "ddns_ips[%d]=\"%s\";",num, buffer);
		    websWrite(wp, "Connstatus[%d]=\"已连接\";",num);
		} else {
//		    websWrite(wp, "ddns_ips[%d]=\"%s\";",num, T(""));
//		    websWrite(wp, "Connstatus[%d]=\"未连接\";",num);
		}
		fclose(fp);

		if (strncmp(prof->ddnsIf, T("WAN1"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_time_WAN1.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN2"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_time_WAN2.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN3"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_time_WAN3.cache", "r");
		}
		else if (strncmp(prof->ddnsIf, T("WAN4"),4) == 0){
		    fp = fopen("/tmp/ddns/phddns_time_WAN4.cache", "r");
		}else{
		    errMsg = "绑定接口错误";
		    setTheErrorMsg(errMsg);
		    goto out;
		}

		if (!fp) {
		    num++;
		    continue;
		}
		if (fscanf(fp, "%ld", &ltime) == 1) {
		    p = localtime(&ltime);
		    sprintf(time, "%d/%d/%d %02d:%02d:%02d", 
			    (1900 + p->tm_year),
			    (1 + p->tm_mon), p->tm_mday,
			    p->tm_hour, p->tm_min, p->tm_sec);
		    websWrite(wp, "ddns_times[%d]=\"%s\";",num,time);
		} 
	    } 
	    num++;
	}else{
	    continue;
	}
    }   
out:    getTheErrorMsg(eid, wp, argc, argv);
	websWrite(wp, "var totalrecs=%d;",num);
	websWrite(wp,"var max_totalrecs=%d;",getWanSlotCount());/*最大条目数，即性能指标*/
	websWrite(wp,"maxrecs[0]=%d;",getWanSlotCount());/*最大条目数，即性能指标*/
	return 0;
}

/***********************************************************************
 * 函 数 名：	aspOutputDdnsConfig
 * 功能描述：	输出配置和信息给DDNS页面
 * 输入参数:    wp,eid,argc,argv
 * 输出参数：	输出配置
 * 返回值：	0
 * 创建日期：	2011-10-26
 * 修改日期：
 * 附加说明：	无
***********************************************************************/
static int aspOutputDdnsConfig(int eid, webs_t wp, int argc, char_t ** argv)
{
    char_t buffer[20] = {0}, time[30] = {0};
    FILE *fp;
    long ltime;
    struct tm *p;

    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;

    prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof == NULL) {
	DDNS_PRINTF("%s: get profile error\n", __FUNCTION__);
	return 0;
    }
 #if (UTT_SMART_UI == FYES)
     getTheRightMsg(eid,wp,argc,argv);
#endif
    websWrite(wp, "var DDNSProviders=\"%s\";\n", prof->ddnsProvider);
    websWrite(wp, "var Accounts=\"%s\";\n", prof->ddnsAccount);
    websWrite(wp, "var DDNSs=\"%s\";\n", prof->ddnsHost);
    websWrite(wp, "var Passwords=\"%s\";\n", prof->ddnsPasswd);
    websWrite(wp, "var Profiles=\"%s\";\n", prof->ddnsIf);

#if(DDNS_UTTCARE == FYES)
         websWrite(wp, " DDNS_UTTCARE_NUM=\"%d\";",1);
#else
         websWrite(wp, " DDNS_UTTCARE_NUM=\"%d\";",0);
#endif
#if (DDNS_NO_IP == FYES)
    websWrite(wp, "var DDNS_no_ip=1;\n");
#else
    websWrite(wp, "var DDNS_no_ip=0;\n");
#endif
#if (DDNS_DYNDNS == FYES)
    websWrite(wp, "var DDNS_dyndns=1;\n");
#else
    websWrite(wp, "var DDNS_dyndns=0;\n");
#endif
#if (DDNS_IP_LINK_HIDE == FYES)
	websWrite(wp, "var DDNS_ip_link=0;\n");
#else
	websWrite(wp, "var DDNS_ip_link=1;\n");
#endif
    if ((strcmp(prof->ddnsProvider, T("3322.org")) == 0)
#if (DDNS_NO_IP == FYES)
 || (strcmp(prof->ddnsProvider, T("no-ip.com")) == 0)
#endif
#if (DDNS_DYNDNS == FYES)
 || (strcmp(prof->ddnsProvider, T("dyndns.org")) == 0)
#endif
	){
	fp = fopen("/tmp/ddns/inadyn_ip.cache", "r");
	if (!fp) {
	   goto fail;
	}
	if (fscanf(fp, "%s", buffer) == 1) {
	    websWrite(wp, "var ddns_ips=\"%s\";", buffer);
	} else {
	    websWrite(wp, "var ddns_ips=\"%s\";", T(""));
	}
	fclose(fp);
	fp = fopen("/tmp/ddns/inadyn_time.cache", "r");
	if (!fp) {
	   goto fail;
	}
	if (fscanf(fp, "%ld", &ltime) == 1) {
#if 0
	    ltime += atoi(nvram_bufget(RT2860_NVRAM, "TIMEZONE"));
#endif
	    p = localtime(&ltime);
	    sprintf(time, "%d/%d/%d %02d:%02d:%02d", 
		    (1900 + p->tm_year),
		    (1 + p->tm_mon), p->tm_mday,
		    p->tm_hour, p->tm_min, p->tm_sec);
	    websWrite(wp, "var ddns_times=\"%s\";", time);
	} else {
	    websWrite(wp, "var ddns_times=\"%s\";", T(""));
	}
        fclose(fp);
    } else if (strcmp(prof->ddnsProvider, T("iplink.com.cn")) == 0) {
	fp = fopen(DDNS_IPLINK_LOG, "r");
	if (!fp) {
	    goto fail;
	}
	fscanf(fp, "%s\n", buffer);
	if (strcmp(buffer, "success")) {
	    goto fail;
	}
	if (fscanf(fp, "%ld\n", &ltime) == 1) {
#if 0
	    ltime += atoi(nvram_bufget(RT2860_NVRAM, "TIMEZONE"));
#endif
	    p = localtime(&ltime);
	    sprintf(time, "%d/%d/%d %02d:%02d:%02d", 
		    (1900 + p->tm_year),
		    (1 + p->tm_mon), p->tm_mday,
		    p->tm_hour, p->tm_min, p->tm_sec);
	    websWrite(wp, "var ddns_times=\"%s\";", time);
	} else {
	    websWrite(wp, "var ddns_times=\"%s\";", T(""));
	}
	if (fscanf(fp, "%s", buffer) == 1) {
	    websWrite(wp, "var ddns_ips=\"%s\";", buffer);
	} else {
	    websWrite(wp, "var ddns_ips=\"%s\";", T(""));
	}
    } else if (strcmp(prof->ddnsProvider, T("www.oray.net")) == 0) {
	fp = fopen("/tmp/ddns/phddns_domains.cache", "r");
	if (!fp)
	    goto fail;
	if (fscanf(fp, "%s\n", buffer) == 1) 
	    websWrite(wp, "var DDNSs=\"%s\";", buffer);
	fclose(fp);
	fp = fopen("/tmp/ddns/phddns_ip.cache", "r");
	if (!fp) {
	   goto fail;
	}
	if (fscanf(fp, "%s", buffer) == 1) {
	    websWrite(wp, "var ddns_ips=\"%s\";", buffer);
	} else {
	    websWrite(wp, "var ddns_ips=\"%s\";", T(""));
	}
	fclose(fp);
	fp = fopen("/tmp/ddns/phddns_time.cache", "r");
	if (!fp) {
	   goto fail;
	}
	if (fscanf(fp, "%ld", &ltime) == 1) {
	    p = localtime(&ltime);
	    sprintf(time, "%d/%d/%d %02d:%02d:%02d", 
		    (1900 + p->tm_year),
		    (1 + p->tm_mon), p->tm_mday,
		    p->tm_hour, p->tm_min, p->tm_sec);
	    websWrite(wp, "var ddns_times=\"%s\";", time);
	} else {
	    websWrite(wp, "var ddns_times=\"%s\";", T(""));
	}
#if (DDNS_UTTCARE == FYES)
    } else if (strcmp(prof->ddnsProvider, T("uttcare.com")) == 0) {
	fp = fopen(DDNS_UTTCARE_LOG, "r");
	if (!fp) {
	    goto fail;
	}
	fscanf(fp, "%s\n", buffer);
	if (strcmp(buffer, "success")) {
	    goto fail;
	}
	if (fscanf(fp, "%ld\n", &ltime) == 1) {
#if 0
	    ltime += atoi(nvram_bufget(RT2860_NVRAM, "TIMEZONE"));
#endif
	    p = localtime(&ltime);
	    sprintf(time, "%d/%d/%d %02d:%02d:%02d",
		    (1900 + p->tm_year),
		    (1 + p->tm_mon), p->tm_mday,
		    p->tm_hour, p->tm_min, p->tm_sec);
	    websWrite(wp, "var ddns_times=\"%s\";", time);
	} else {
	    websWrite(wp, "var ddns_times=\"%s\";", T(""));
	}
	if (fscanf(fp, "%s", buffer) == 1) {
	    websWrite(wp, "var ddns_ips=\"%s\";", buffer);
	} else {
	    websWrite(wp, "var ddns_ips=\"%s\";", T(""));
	}	
#endif
    } else {
	websWrite(wp, "var ddns_times=\"%s\";", T(""));
	websWrite(wp, "var ddns_ips=\"%s\";", T(""));
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
fail:
    websWrite(wp, "var ddns_times=\"%s\";", T(""));
    websWrite(wp, "var ddns_ips=\"%s\";", T(""));
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}


static int aspOutputDdnsType(int eid, webs_t wp, int argc, char_t ** argv)
{
#if (DDNS_NO_IP == FYES)
    websWrite(wp, "var DDNS_no_ip=1;\n");
#else
    websWrite(wp, "var DDNS_no_ip=0;\n");
#endif
#if (DDNS_DYNDNS == FYES)
    websWrite(wp, "var DDNS_dyndns=1;\n");
#else
    websWrite(wp, "var DDNS_dyndns=0;\n");
#endif
}
/***********************************************************************
 * 函 数 名：	aspOutputDdnsConfigOneInfo
 * 功能描述：	输出配置和信息给DDNS页面
 * 输入参数:    wp,eid,argc,argv
 * 输出参数：	输出一条配置
 * 返回值：	0
 * 创建日期：	2015-1-13
 * 修改日期：
 * 附加说明：	无
***********************************************************************/
static int aspOutputDdnsConfigOneInfo(int eid, webs_t wp, int argc, char_t ** argv)
{
    char_t buffer[20] = {0}, time[30] = {0};
    FILE *fp;
    long ltime;
    struct tm *p;

    MibProfileType mibType = MIB_PROF_DDNS;
    DDNSProfile *prof = NULL;
    int min = 0, max = 0, i = 0;
    char *editName;
    const char *errMsg=NULL;
    if (ejArgs(argc, argv, T("%s"), &editName) < 1) {
        errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
        setTheErrorMsg(errMsg);
    } else {
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
	    prof = (DDNSProfile *)ProfGetInstPointByIndex(mibType, i);
	    if((prof != NULL) && (ProfInstIsFree(prof) == 0) &&
		    (editName != NULL) && (strncmp(editName, prof->ddnsIf,4) == 0)) {
		websWrite(wp, "var DDNSProviders=\"%s\";\n", prof->ddnsProvider);
		websWrite(wp, "var Accounts=\"%s\";\n", prof->ddnsAccount);
		websWrite(wp, "var DDNSs=\"%s\";\n", prof->ddnsHost);
		websWrite(wp, "var Passwords=\"%s\";\n", prof->ddnsPasswd);
		websWrite(wp, "var Profiles=\"%s\";\n", prof->ddnsIf);
#if(DDNS_UTTCARE == FYES)
		websWrite(wp, " DDNS_UTTCARE_NUM=\"%d\";",1);
#else
        websWrite(wp, " DDNS_UTTCARE_NUM=\"%d\";",0);
#endif
		break;
	    }
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
static void formConfigDDNSListDel(webs_t wp, char_t *path, char_t *query)
{
    int num=0;
    int min, max, index = 0;
    char *ptr;
    MibProfileType mibType = MIB_PROF_DDNS;
    struProfAlloc *profhead = NULL;
    DDNSProfile *prof = NULL;
    const char *errMsg=NULL;
    char *route = NULL, *tmp = NULL;
    route = websGetVar(wp, T("delstr"), T(""));
    if (route == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_ROUTE_NAME_NOT_FIND);
	setTheErrorMsg(errMsg);
	goto out;
    } 
    if (strlen(route) != 0) {
	tmp=strtok_r(route, UTT_SPLIT_SIGN_STR,&ptr);
	while(tmp!=NULL){
	    ProfInstLowHigh(mibType, &max, &min);
	    for(index=min;index<max;index++)
	    {
		prof = (DDNSProfile *) ProfGetInstPointByIndex(mibType,index);
		if((prof!=NULL) && (ProfInstIsFree(prof) == PROFOK) &&(strcmp(prof->ddnsIf,tmp)==0))
		{
		    break;
		}
		
	    }
	    if(index<max){	
		ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
		ProfDelInstByIndex(mibType, index);
	    }	
	    else
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		setTheErrorMsgOnFree(errMsg);
	    }
/*	    prof= (DDNSProfile *)ProfGetInstPointByName(mibType, tmp);
	    if(prof!=NULL && ProfInstIsFree(prof) == PROFOK){	
		ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
		ProfDelInstByName(mibType, tmp);
	    }	
	    else
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		setTheErrorMsgOnFree(errMsg);
	    }
*/
	    tmp=strtok_r(NULL, ",",&ptr);
	}
    }
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();/*保存flash*/
out:
    websRedirect(wp, "DDNSAdvance.asp");
 
}
#if 1
static void formConfigDDNSListDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_DDNS;
    Pointer  *prof=NULL;
    struProfAlloc *profList = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = ProfGetInstPointByIndex(mibType, i);
	if(prof!=NULL && (ProfInstIsFree(prof) == 0)){
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back=1;
	    }
	    ProfDelInstByIndex(mibType, i);
	}
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "DDNSAdvance.asp");

}
#endif
/*
 * 定义页面句柄
 */
extern void websDefineDdns(void) {
    websAspDefine(T("outputDDNSListOneInfo"), aspOutputDdnsConfigOneInfo);
    websAspDefine(T("outputDDNSType"), aspOutputDdnsType);
    websAspDefine(T("outputDDNSList"), aspOutputDdnsConfigList);
    websFormDefine(T("ConfigDDNSListDel"), formConfigDDNSListDel);
    websFormDefine(T("ConfigDDNSListDelAll"), formConfigDDNSListDelAll);
    websAspDefine(T("outputDDNS"), aspOutputDdnsConfig);
    websFormDefine(T("DDNS"), formDdnsConfig);
    return;
}

#endif
