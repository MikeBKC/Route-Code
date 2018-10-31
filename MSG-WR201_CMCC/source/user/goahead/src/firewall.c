/*
 *	firewall.c -- Firewall Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.c,v 1.9 2012/08/29 10:03:33 yu.dehai Exp $
 */

/*
 *	if  WAN or LAN ip changed, we must restart firewall.
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"

extern char* getLanIfName(void);
int currentDnsFilterCounter = 0;
char l7name[8192];						// export it for internet.c qos

int getGoAHeadServerPort(void);

/***********************************************************************
 * 函 数 名：   isMacValid
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
int isMacValid(char *str)
{
    int i, len = strlen(str);
    if(len != 17)
    	return 0;
    
    for(i=0; i<5; i++){
    	if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
    		return 0;
    }
    return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int isNumOnly(char *str){
    int i, len = strlen(str);
    for(i=0; i<len; i++){
    	if((str[i] >= '0' && str[i] <= '9'))
    		continue;
    	return 0;
    }
    return 1;
}

/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int isAllNumAndSlash(char *str){
    int i, len = strlen(str);
    for(i=0; i<len; i++){
    	if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
    		continue;
    	return 0;
    }
    return 1;
}

/***********************************************************************
 * 函 数 名：   isOnlyOneSlash
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int isOnlyOneSlash(char *str)
{
    int i, count=0;
    int len = strlen(str);
    for(i=0; i<len; i++) {
    	if( str[i] == '/') {
    	    count++;
        }
    }
    return count <= 1 ? 1 : 0;
}

/***********************************************************************
 * 函 数 名：   isIpNetmaskValid
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
int isIpNetmaskValid(char *s)
{
    char str[32];
    char *slash;
    struct in_addr addr;    // for examination
    
    if(!s || !strlen(s)){
    	return 0;
    }
    
    strncpy(str, s, sizeof(str));
    
    if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
        return 1;

    if (!isAllNumAndSlash(str)){
    	return 0;
    }
    
    if(!isOnlyOneSlash(str)){
    	return 0;
    }
    
    slash = strchr(str, '/');
    if(slash){
        int mask;
    
        *slash = '\0';
        slash++;
        if(!strlen(slash)){
        	return 0;
        }
    
        if(!isNumOnly(slash)){
        	return 0;
        }
    
        mask = atoi(slash);
        if(mask < 0 || mask > 32){
        	return 0;
        }
    }
    
    if(! (inet_aton(str, &addr))){
        return 0;
    }
    return 1;
}

#if 0
/*
 * unused , just del
 * by dhyu
 */
/***********************************************************************
 * 函 数 名：   iptablesForwardFilterClear
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
/*
 * hide the possible "error/warn" message when deleting a non-exist chain.
 */
static void iptablesForwardFilterClear(void)
{
    doSystem("iptables -F -t filter 1>/dev/null 2>&1");
}
#endif

/***********************************************************************
 * 函 数 名：   iptablesForwardFilterFlush
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
/*
static void iptablesForwardFilterFlush(void)
{
	doSystem("iptables -t filter -F FORWARD  1>/dev/null 2>&1");
}
*/

#if 0
/*
 * unused 
 * move to speedweb/firewall.c
 * by dhyu
 */
/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesIPPortFilterFlush(void){
    doSystem("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
}
#endif

#if 0
/*
 * unused, just del
 * by dhyu
 */
/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesIPPortFilterClear(void){
    doSystem("iptables -D FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
    doSystem("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
}
#endif

#if 0
/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesDMZFlush(void){
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1", DMZ_CHAIN);
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1", DMZ_CHAIN1);
}

#endif
/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void iptablesPortForwardFlush(void){
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN);
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN1);
}

#if 0
/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesDMZClear(void){
    doSystem("iptables -t nat -D PREROUTING -j %s 1>/dev/null 2>&1", DMZ_CHAIN);	// remove rule in PREROUTING chain
    doSystem("iptables -t nat -D POSTROUTING -j %s 1>/dev/null 2>&1", DMZ_CHAIN1);	// remove rule in PREROUTING chain
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1; iptables -t nat -X %s  1>/dev/null 2>&1", DMZ_CHAIN, DMZ_CHAIN);
    doSystem("iptables -t nat -F %s 1>/dev/null 2>&1; iptables -t nat -X %s  1>/dev/null 2>&1", DMZ_CHAIN1, DMZ_CHAIN1);
}

/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesPortForwardClear(void){
    doSystem("iptables -t nat -D PREROUTING -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN);
    doSystem("iptables -t nat -D POSTROUTING -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN1);
    doSystem("iptables -t nat -F %s  1>/dev/null 2>&1; iptables -t nat \
        -X %s  1>/dev/null 2>&1", PORT_FORWARD_CHAIN, PORT_FORWARD_CHAIN);
    doSystem("iptables -t nat -F %s  1>/dev/null 2>&1; iptables -t nat \
        -X %s  1>/dev/null 2>&1", PORT_FORWARD_CHAIN1, PORT_FORWARD_CHAIN1);
}

/***********************************************************************
 * 函 数 名：   getIfMac
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesPortForwardRuleClear(void){
    doSystem("iptables -t nat -D POSTROUTING -j %s 1>/dev/null 2>&1", PORT_FORWARD_RULE_CHAIN);
    doSystem("iptables -t nat -F %s  1>/dev/null 2>&1; iptables -t nat \
            -X %s  1>/dev/null 2>&1", PORT_FORWARD_RULE_CHAIN, PORT_FORWARD_RULE_CHAIN);
}

/*
 * unused 
 * never use, just del
 * by dhyu
 */
/***********************************************************************
 * 函 数 名：   iptablesAllFilterClear
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesAllFilterClear(void)
{
    iptablesForwardFilterClear();
    iptablesIPPortFilterClear();
    
    doSystem("iptables -P INPUT ACCEPT");
    doSystem("iptables -P OUTPUT ACCEPT");
    doSystem("iptables -P FORWARD ACCEPT");
}

/***********************************************************************
 * 函 数 名：   iptablesAllNATClear
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void iptablesAllNATClear(void)
{
    iptablesPortForwardClear();
    iptablesPortForwardRuleClear();
    iptablesDMZClear();
}
#endif

/***********************************************************************
 * 函 数 名：   insert 
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
#if 0
char *insert(char *subs, int index, char *str, char delimit)
{
	int i=0;
	char *begin, *end;
	char *result = (char *)calloc(1, sizeof(char ) * (strlen(str) + strlen(subs) + 1 + 1));

	begin = str;
	end = strchr(str, delimit);
	while(end){
		if(i == index)
			break;
		begin = end;
		end = strchr(begin+1, delimit);
		i++;
	}
	if(begin == str){
		if(strlen(str) == 0){
			strcpy(result, subs);
		}else{
			if(index == 0){
				sprintf(result, "%s%c%s", subs, delimit, str);
			}else{
				sprintf(result, "%s%c%s", str, delimit, subs);
			}
		}
	}else if(end == NULL && i != index){
		sprintf(result, "%s%c%s", str, delimit, subs);
	}else{
		strncpy(result, str, begin - str);
		sprintf(result, "%s%c", result, delimit);
		strcat(result, subs);
		strcat(result, begin);
	}
	return result;
}
/***********************************************************************
 * 函 数 名：   replace 
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/

char *replace(char *subs, int index, char *str, char delimit)
{
	int del[1];
	char *result;
	char *dup = strdup(str);
	del[0] = index;
	deleteNthValueMulti(del, 1, dup, delimit);
	result = insert(subs, index, dup, delimit);
	/ree(dup);
	return result;
}
#endif

#if 0
/***********************************************************************
 * 函 数 名：   getNums
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int getNums(char *value, char delimit)
{
    char *pos = value;
    int count=1;
    if(!pos) {
    	return 0;
    }
    while( (pos = strchr(pos, delimit))){
	pos = pos+1;
	count++;
    }
    return count;
}

/***********************************************************************
 * 函 数 名：   makeDMZRule
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void makeDMZRule(char *buf, int len, char *wan_name, char *ip_address)
{
    char_t lanIp[16]={0}, wanIp[16]={0};
    getIfIp(wan_name ,wanIp);
    getIfIp(getLanIfName(),lanIp);
    int rc = snprintf(buf, len, "iptables -t nat -A %s -j DNAT -d %s --to %s", DMZ_CHAIN, wanIp ,ip_address);
    snprintf(buf+rc, len, ";iptables -t nat -A %s -j SNAT -d %s --to %s", DMZ_CHAIN1, ip_address ,lanIp);
}

/***********************************************************************
 * 函 数 名：   makePortForwardRule1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void makePortForwardRule1(char *buf, int len, char *ip1, 
        char *ip_address, int proto, int prf_int, int prt_int,int in_prf_int,int in_prt_int)
{
    int rc = 0;
    char *pos = buf;
    
    rc = snprintf(pos, len-rc, 
        "iptables -t nat -A %s -j SNAT -s %s ", PORT_FORWARD_CHAIN1, ip_address);
    pos = pos + rc;
    
    // write protocol type
    if(proto == PROTO_TCP)
        rc = snprintf(pos, len-rc, "-p tcp ");
    else if (proto == PROTO_UDP)
        rc = snprintf(pos, len-rc, "-p udp ");
    else if (proto == PROTO_TCP_UDP)
        rc = snprintf(pos, len-rc, " ");
    pos = pos + rc;
    
    // write port
    if(prt_int != 0)
        rc = snprintf(pos, len-rc, "--dport %d:%d ", in_prf_int, in_prt_int);
    else
        rc = snprintf(pos, len-rc, "--dport %d ", in_prf_int);
    pos = pos + rc;
    
    // write remote ip
    rc = snprintf(pos, len-rc, "--to %s ", ip1);
}

/***********************************************************************
 * 函 数 名：   makePortForwardRule
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static void makePortForwardRule(char *buf, int len, char *wan_ip, 
        char *ip_address, int proto, int prf_int, int prt_int,int in_prf_int,int in_prt_int)
{
    int rc = 0;
    char *pos = buf;
    
    rc = snprintf(pos, len-rc, 
        "iptables -t nat -A %s -j DNAT -d %s  ", PORT_FORWARD_CHAIN, wan_ip);
    pos = pos + rc;
    
    // write protocol type
    if(proto == PROTO_TCP)
        rc = snprintf(pos, len-rc, "-p tcp ");
    else if (proto == PROTO_UDP)
        rc = snprintf(pos, len-rc, "-p udp ");
    else if (proto == PROTO_TCP_UDP)
        rc = snprintf(pos, len-rc, " ");
    pos = pos + rc;
    
    // write port
    if(prt_int != 0)
        rc = snprintf(pos, len-rc, "--dport %d:%d ", prf_int, prt_int);
    else
        rc = snprintf(pos, len-rc, "--dport %d ", prf_int);
    pos = pos + rc;
    
    // write remote ip
    rc = snprintf(pos, len-rc, "--to %s:%d-%d ", ip_address,in_prf_int,in_prt_int);
}
#endif



/***********************************************************************
 * 函 数 名：   getNameIntroFromPat
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
char *getNameIntroFromPat(char *filename)
{
    static char result[512];
    char buf[512], *begin, *end, *desh;
    char path_filename[512];
    char *rc;
    FILE *fp;
    
    sprintf(path_filename, "%s/%s", "/etc_ro/l7-protocols", filename);
    if(! (fp = fopen(path_filename, "r"))) {
    	return NULL;
    }
    result[0] = '\0';
    rc = fgets(buf, sizeof(buf), fp);
    if(rc) {
        begin = buf + 2;
        if(! ( desh = strchr(buf, '-'))){
            fclose(fp);
            return "N/A#N/A";
        }
        end = desh;
        if(*(end-1) == ' ') {
        	end--;
        }
        *end = '\0';
        strncat(result, begin, sizeof(result));
        strncat(result, "#", sizeof(result));
    
        // find intro
        if(!(end = strchr(desh+1, '\n'))){
            fclose(fp);
            return "N/A#N/A";
        }
        *end = '\0';
        strncat(result, desh + 2 , sizeof(result));
    } else {
        fclose(fp);
        return "N/A#N/A";
    }
    
    fclose(fp);
    return result;	
}


#if 0
static void init_SecDefense()
{
    char *DDosEn;
    DDosEn=nvram_bufget(RT2860_NVRAM ,"DDosEn");
    doSystem("iptables -D INPUT  -p tcp --syn -m limit --limit 12/s --limit-burst 24 -j ACCEPT");
    dosystem("iptables -D FORWARD -p tcp -o eth0.1 --syn -m limit --limit 1/s -j ACCEPT");
    if(strcmp(DDosEn, "on") == 0 ) {
        doSystem("iptables -A INPUT  -p tcp --syn -m limit --limit 12/s --limit-burst 24 -j ACCEPT");
        dosystem("iptables -A FORWARD -p tcp -o eth0.1 --syn -m limit --limit 1/s -j ACCEPT");
    }
}
#endif

#if 0
static void formSecDefense(webs_t wp, char_t *path, char_t *query)
{
    char *DDosEn, *WormEn, *PingEn;
    DDosEn=websGetVar(wp, T("DDOSEnable"), T(""));
    WormEn=websGetVar(wp, T("WormEnable"), T(""));
    PingEn=websGetVar(wp, T("PingDisable"), T(""));
    DBGPRINT("DDosDf[%s], WormDf[%s], PingDf[%s]\n", DDosEn, WormEn, PingEn);
    nvram_set(RT2860_NVRAM, "DDosDf", DDosEn);
    nvram_set(RT2860_NVRAM, "WormDf", WormEn);
    nvram_set(RT2860_NVRAM, "PingDf", PingEn);
    nvram_commit(RT2860_NVRAM);
    sendMsgForSpeed(INIT_SEC_DEFENSE, 0, 0, NULL, 0);
    websRedirect(wp, "FwBase.asp");
}
#endif

#if 0
static int aspOutSecDefense(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp,"var DDOSEnables=\"%s\";", nvram_bufget(RT2860_NVRAM,"DDosDf"));
    websWrite(wp,"var WormEnables=\"%s\";", nvram_bufget(RT2860_NVRAM,"WormDf"));
    websWrite(wp,"var PingDisables=\"%s\";", nvram_bufget(RT2860_NVRAM,"PingDf"));
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
/***********************************************************************
 * 函 数 名：   formDefineFirewall
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
void formDefineFirewall(void)
{
    websAspDefine(T("aspoutErrorMsg"), getTheErrorMsg);
 //   websAspDefine(T("aspOutFireWall"),getFilterList);
  //  websFormDefine(T("formFireWall"),formFilterList);
  //  websFormDefine(T("formFireWallDelAll"),formFilterListDelAll);
  //  websFormDefine(T("formFireWallDel"),formFilterListDel);
#if 0
    /* unused moved to src/firewall.c by dhyu */
    websAspDefine(T("aspOutOneFireWallRuleInfo"),getOneFilterInfo);
#endif
    websAspDefine(T("Form"), aspForm);
#if 0
    websFormDefine(T("formFwBase"),formSecDefense);
    websAspDefine(T("aspOutSecDefense"),aspOutSecDefense);
#endif
}

