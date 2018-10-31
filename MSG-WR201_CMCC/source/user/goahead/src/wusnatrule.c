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
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "translate.h"
#ifdef NAT_DEBUG
#define NAT_PRNTF(fmt, args...)                printf(fmt, ## args)  
#else
#define NAT_PRNTF(fmt, args...)   
#endif

#if (NAT_RULE == FYES)

static int NatRuleCounter=0; 
static int BindEthID=1;  //每添加一条one2one规则都要绑定新ip到wan口
#if (NAT_BIND_IP_ALL == FYES)
static int BindIpCT=0;  //NAT规则在WAN口绑定的IP数
#endif

extern int checkWanIpInPool(struct in_addr addrStart, int cnt);
int getNatOutIpCnt(void);
/***********************************************************************
 * 函 数 名：   getNatOneRuleInfo
 * 创建日期：	2011-10-18
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static int getNatOneRuleInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name;
    const char *errMsg=NULL;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof= NULL;
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	errMsg = getTransDataForC(C_LANG_INDEX_READ_EDIT_NAME_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    NAT_PRNTF("edit_name:%s\n",edit_name);
    if(!edit_name)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_NULL);
	setTheErrorMsg(errMsg);
	goto out;
    }
    prof = (NatRuleProfile*)ProfGetInstPointByName(mibType, edit_name);
    if(prof == NULL) {
	errMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_THE_GROP_CONF);
	setTheErrorMsg(errMsg);
    } else {
	websWrite(wp,"var RuleIDs=\"%s\";",prof->head.name);
	websWrite(wp,"var NatTypes=\"%d\";",prof->NatruleType);
	websWrite(wp, "var OutIPs=\"%N\";",prof->OutIp);
	websWrite(wp, "var InFromIPs=\"%N\";",prof->InFromIP);
	websWrite(wp, "var InEndIPs=\"%N\";",prof->InEndIP);
	websWrite(wp,"var Binds=\"%s\";",utt_strtoup(prof->NatBind));
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * 函 数 名：   getNatRuleList
 * 创建日期：	2011-10-18
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static int getNatRuleList(int eid, webs_t wp, int argc, char_t **argv)
{
    unsigned int num=0;
    int min = 0, max = 0;
    int i = 0;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof= NULL;

    websWrite(wp,"var RuleIDs=new Array();");
    websWrite(wp,"var NatTypes = new Array();");
    websWrite(wp,"var OutIPs = new Array();");
    websWrite(wp,"var InFromIPs=new Array();");
    websWrite(wp,"var InEndIPs=new Array();");
    websWrite(wp,"var Binds=new Array();");

    char_t *RuleId=T("RuleIDs[%d] = \"%s\";");
    char_t *NatType=T("NatTypes[%d] = \"%d\";"); 
    char_t *OutIp =T("OutIPs[%d] = \"%N\";");
    char_t *InFromIp =T("InFromIPs[%d] = \"%N\";");
    char_t *InEndIp = T("InEndIPs[%d] = \"%N\"; ");
    char_t *NatBinds = T("Binds[%d] = \"%s\"; ");


    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (NatRuleProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    websWrite(wp, RuleId,num, prof->head.name);
	    websWrite(wp, NatType,num, prof->NatruleType);
	    websWrite(wp, OutIp,num, prof->OutIp);
	    websWrite(wp, InFromIp,num, prof->InFromIP);
	    websWrite(wp, InEndIp,num, prof->InEndIP);
	    websWrite(wp, NatBinds,num, utt_strtoup(prof->NatBind));
	    num++;
	}
    }
    NatRuleCounter = num;
    websWrite(wp,"var totalrecs=%d;",num);
    websWrite(wp,"var max_totalrecs=%d;",MAX_NAT_RULE_PROFILES);
#if (NAT_BIND_IP_ALL == FYES)
    BindIpCT=getNatOutIpCnt();
    NAT_PRNTF("BindIpCT=%d\n",BindIpCT);
#endif
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

/***********************************************************************
 * 函 数 名：   aspOutNatRulePassthroughDisPlay
 * 创建日期：	2014-2-19
 * 修改日期：
 * 作者：       赵冲
 * 附加说明：	无
 ***********************************************************************/
static int aspOutNatRulePassthroughDisPlay(int eid, webs_t wp, int argc, char_t **argv)
{
#if (NAT_PASSTHROUGH_NOT_DISPALY == FYES)
	websWrite(wp,"var passthroughNotDisplay=1;");
#else
	websWrite(wp,"var passthroughNotDisplay=0;");
#endif
    return 0;
}

/* 
 * if the ip range from [outFromIp] to [outFromIp]+ipNum-1 is overlapd, -1 is returnd */
static int getWanPoint(unsigned int ipNum, unsigned int outFromIp, char *name)
{
    unsigned int natcnt=0U,i,j,wanPoint=0U, ipRange;
    struct sNatRule{
	int Type;
	unsigned int ethId;
	unsigned int nOutIp;
	unsigned int nInFromIp;
	unsigned int nInEndIp;
	char NatBind[8];
    }natRules[UTT_MAX_NAT_BIND_IP],natRule, natRule2;
    int found=0;

    int min = 0, max = 0;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof= NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (NatRuleProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if ((name != NULL) && strcmp(prof->head.name, name) == 0){
		/* 跳出旧实例 */
		NAT_PRNTF("name = %s\n", prof->head.name);
	    }
	    else{
		natRules[natcnt].Type = prof->NatruleType;
		natRules[natcnt].ethId = prof->EthId;
		natRules[natcnt].nOutIp = htonl(prof->OutIp);
		natRules[natcnt].nInFromIp =  htonl(prof->InFromIP);
		natRules[natcnt].nInEndIp = htonl(prof->InEndIP);
		strcpy(natRules[natcnt].NatBind,prof->NatBind);
		natcnt++;
	    }
	}
    }

    NAT_PRNTF("natcnt[%d]\n",natcnt);

    /* add zero , make sure count from zero*/
    natRules[natcnt].Type = EASYIP;
    natRules[natcnt].ethId=0U;
    natRules[natcnt].nOutIp=0x00000000U;
    natRules[natcnt].nInFromIp=0x00000000U;
    natRules[natcnt].nInEndIp=0x00000000U;
    strcpy(natRules[natcnt].NatBind, "WAN1");
    natcnt++;

    /* sort */
    for (i=natcnt-1U; i>=1U; i--) {
	for (j=0U;j<i ;j++) {
	    if (natRules[j].ethId > natRules[j+1].ethId) {
		memcpy((char*)&natRule,(char*)&natRules[j],sizeof(struct sNatRule));
		memcpy((char*)&natRule2,(char*)&natRules[j+1],sizeof(struct sNatRule));
		memcpy((char*)&natRules[j],(char*)&natRule2,sizeof(struct sNatRule));
		memcpy((char*)&natRules[j+1],(char*)&natRule,sizeof(struct sNatRule));
	    }
	}
    }

    /* find */
    for(i=0U; i<natcnt; i++) {
	NAT_PRNTF("ethId[%d],nInFromIp[%x],nInEndIp[%x]\n",natRules[i].ethId, natRules[i].nInFromIp, natRules[i].nInEndIp);
	if(natRules[i].Type == 1) {
	    ipRange = 1;
	} else {
	    ipRange = natRules[i].nInEndIp-natRules[i].nInFromIp + 1U;
	}
	if (( (outFromIp+ipNum-1U < natRules[i].nOutIp) || (outFromIp > (natRules[i].nOutIp+ipRange-1U)) )==0) {
	    NAT_PRNTF("ip range overlaped\noutFromIp[%x],ipNum[%d]\nnatRules.nOutIp[%x],ipRange[%d]\n",
		    outFromIp, ipNum, natRules[i].nOutIp, ipRange);
	    wanPoint=-1U;
	    break;
	}
	if(found){
	    continue;
	}

	wanPoint=natRules[i].ethId + ipRange;

	if (i == natcnt-1U) {
	    break;
	}
	NAT_PRNTF("free range[%d]-[%d]:[%d]\n",wanPoint,natRules[i+1U].ethId-1U, natRules[i+1U].ethId-wanPoint);
	if ( (wanPoint + ipNum - 1U) < natRules[i+1U].ethId) {
	    found=1;
	}
    }
    return wanPoint;
}
/* get the used out-ip count */
int getNatOutIpCnt(void)
{
    char  NatBind[8];
    unsigned int nOutIp,nInFromIp,nInEndIp,ethId,ipNum=0U,addnum;
    int Type = 0;
    int min = 0, max = 0, i = 0;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof= NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (NatRuleProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    Type = prof->NatruleType;
	    ethId = prof->EthId;
	    nOutIp = htonl(prof->OutIp);
	    nInFromIp =  htonl(prof->InFromIP);
	    nInEndIp = htonl(prof->InEndIP);
	    strcpy(NatBind,prof->NatBind);

	    NAT_PRNTF("ipfrom[%x],ipend[%x]\n",nInFromIp,nInEndIp);
	    if(Type == 1) {
		addnum=1U;
	    } else if(Type == 2){
		addnum=nInEndIp-nInFromIp+1U;
	    } else {
		addnum=0U;
	    }
	    ipNum+=addnum;

	    NAT_PRNTF("now NatBind[%s],ipNum[%d]\n",NatBind, ipNum);
	}
    }
    /* sort the natList by ethId */
    return (int)ipNum;
}

/***********************************************************************
 * 函 数 名：   formNatRuleList
 * 创建日期：	2011-10-18
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formNatRuleList(webs_t wp, char_t *path, char_t *query)
{
    char_t *RuleId,*RuleIdOld,*NatType,*OutIp,*InFromIp,*InEndIp,*action,*NatBind;
    const char *errMsg=NULL;
    unsigned int nOutIp,nInFromIp,nInEndIp,oldEthId;
    struct in_addr addr_InFromIP,addr_InEndIp,addr_OutIP;
    int instIndex, min = 0, max = 0, i = 0;
    int nEditBindIpOld,nEditBindIpNew;
    char lan_addr[16]={0}, lan_mask[16]={0};
    in_addr_t lanIp, lanMask, in_InFromIp,in_InEndIp;
#if (MULTI_LAN == FYES)
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    unsigned int lanIp2 = 0U, lanIp3 = 0U, lanIp4 = 0U;
    unsigned int lanMask2 = 0U, lanMask3 = 0U, lanMask4 = 0U;
#endif

    a_assert(wp);

#if (SERVICE_ZONE == FYES)
    MibProfileType mibTypeSZ = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *profheadSZ = NULL;
#endif

    MibProfileType mibType = MIB_PROF_NAT_RULE;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction;
    NatRuleProfile *natRuleProfile = NULL;
    NatRuleProfile *prof= NULL;

    RuleId=websGetVar(wp,T("RuleID"), T(""));
    NatType=websGetVar(wp,T("NatType"), T(""));
    OutIp=websGetVar(wp,T("OutIP"), T(""));
    InFromIp=websGetVar(wp,T("InFromIP"), T(""));
    InEndIp=websGetVar(wp,T("InEndIP"), T(""));
    action = websGetVar(wp, T("Action"), T("")); 
    NatBind = websGetVar(wp, T("Bind"), T("")); 
    RuleIdOld= websGetVar(wp, T("RuleIDold"), T("")); 
    utt_strtoup(NatBind); /* wanIndex */

    nOutIp=ip2int(OutIp);
    nInEndIp=ip2int(InEndIp);
    nInFromIp=ip2int(InFromIp);

    NAT_PRNTF("outip = %x inendip = %x infronip = %x\n",nOutIp,nInEndIp,nInFromIp);
    if (strcmp(NatType, "3") != 0) {
    if((inet_aton(OutIp, &addr_OutIP) == 0) || (inet_aton(InFromIp, &addr_InFromIP) == 0) || (inet_aton(InEndIp, &addr_InEndIp) == 0)){  
	errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    } else {
    if((inet_aton(InFromIp, &addr_InFromIP) == 0) || (inet_aton(InEndIp, &addr_InEndIp) == 0)){  
	errMsg = getTransDataForC(C_LANG_INDEX_IP_PLAN_ADDR_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    }
    instIndex = ProfGetInstIndexByName(mibType, RuleIdOld);

    getIfIp(getLanIfName(), lan_addr);
    getIfNetmask(getLanIfName(), lan_mask);
    lanMask = inet_addr(lan_mask);
    lanIp = inet_addr(lan_addr);
#if(MULTI_LAN == FYES)
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF == NULL) || (profIF->head.active == 0)) {
	errMsg = getTransDataForC(C_LANG_INDEX_LAN_CONFIG_ERR);
	setTheErrorMsg(errMsg);
	goto out;
    }
    lanIp2 = profIF->ether.sta.ip2;
    lanIp3 = profIF->ether.sta.ip3;
    lanIp4 = profIF->ether.sta.ip4;
    lanMask2 = profIF->ether.sta.nm2;
    lanMask3 = profIF->ether.sta.nm3;
    lanMask4 = profIF->ether.sta.nm4;
#endif
    in_InFromIp = inet_addr(InFromIp);
    in_InEndIp = inet_addr(InEndIp);


#if (SERVICE_ZONE == FYES)
	ProfInstLowHigh(mibTypeSZ, &max, &min);

#if 0  /*mantis 12420 NAT内部地址可以和LAN口不在同一网段*/
	if ((in_InFromIp & lanMask) != (lanIp & lanMask))
	{
		for (i = min + 1; i < max; i++) {
			profheadSZ = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSZ, i);
				if ((profheadSZ != NULL) && (profheadSZ->head.active == TRUE) && (profheadSZ->serviceEn == PROF_ENABLE)){
					if ((in_InFromIp & profheadSZ->netmask) == (profheadSZ->gw & profheadSZ->netmask)) {
						break;
					}
				}
		}

		if (i >= max) {
			errMsg = getTransDataForC(C_LANG_INDEX_INNER_START_IP_LAN);
			setTheErrorMsg(errMsg);

		//	setTheErrorMsg(T("内部起始IP地址不和LAN口在同一网段!"));
			goto out;
		}
	}

	if ((in_InEndIp & lanMask) != (lanIp & lanMask))
	{
		for (i = min + 1; i < max; i++) {
			profheadSZ = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSZ, i);
				if ((profheadSZ != NULL) && (profheadSZ->head.active == TRUE) && (profheadSZ->serviceEn == PROF_ENABLE)){
					if ((in_InEndIp & profheadSZ->netmask) == (profheadSZ->gw & profheadSZ->netmask)) {
						break;
					}
				}
		}

		if (i >= max) {

			errMsg = getTransDataForC(C_LANG_INDEX_INNER_END_IP_LAN);
			setTheErrorMsg(errMsg);
		//	setTheErrorMsg(T("内部结束IP地址不和LAN口在同一网段!"));
			goto out;
		}
	}
#endif

	for (i = min + 1; i < max; i++) {
		profheadSZ = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSZ, i);
			if ((profheadSZ != NULL) && (profheadSZ->head.active == TRUE) && (profheadSZ->serviceEn == PROF_ENABLE)){
				if (strcmp(NatType, "2") == 0) { /* one to one */
					if ((ntohl(profheadSZ->gw) >= ntohl(addr_OutIP.s_addr)) && (ntohl(profheadSZ->gw) <= ntohl(addr_OutIP.s_addr) + (nInEndIp - nInFromIp + 1u))) {
					
					errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_NOT_INCLUDE);
					setTheErrorMsg(errMsg);
			//		setTheErrorMsg(T("外部IP地址段不能包含各接口IP地址！"));
						goto out;
					}
				} else if(strcmp(NatType, "1") == 0){ /* easy ip */
					if (profheadSZ->gw == addr_OutIP.s_addr) {
						errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_NOT_EQUAL);
						setTheErrorMsg(errMsg);
						//setTheErrorMsg(T("外部IP地址段不能等于各接口IP地址！"));
						goto out;
					}
				}

					
				
				if ((profheadSZ->gw >= addr_InFromIP.s_addr) && (profheadSZ->gw <= addr_InEndIp.s_addr)) {
						errMsg = getTransDataForC(C_LANG_INDEX_INNER_IP_NOT_INCLUDE);
						setTheErrorMsg(errMsg);
				//	setTheErrorMsg(T("内部IP地址段不能包含各接口IP地址！"));
					goto out;
				}
			}
	}
#else
#if 0  /*mantis 12420 NAT内部地址可以和LAN口不在同一网段*/
    if (((in_InFromIp & lanMask) != (lanIp & lanMask))
#if(MULTI_LAN == FYES)
	&& ((in_InFromIp & lanMask2) != (lanIp2 & lanMask2))
	&& ((in_InFromIp & lanMask3) != (lanIp3 & lanMask3))
	&& ((in_InFromIp & lanMask4) != (lanIp4 & lanMask4))
#endif
	)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_INNER_START_IP_LAN);
	setTheErrorMsg(errMsg);
	goto out;
    }

    if (((in_InEndIp & lanMask) != (lanIp & lanMask))
#if(MULTI_LAN == FYES)
	&& ((in_InEndIp & lanMask2) != (lanIp2 & lanMask2))
	&& ((in_InEndIp & lanMask3) != (lanIp3 & lanMask3))
	&& ((in_InEndIp & lanMask4) != (lanIp4 & lanMask4))
#endif
	)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_INNER_END_IP_LAN);
	setTheErrorMsg(errMsg);
	goto out;
    }
#endif
#endif

    /* mantis 8710 */
    if (strcmp(NatType, "2") == 0) { /* one to one */
	if(checkWanIpInPool(addr_OutIP,nInEndIp-nInFromIp+1) == -1)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_NOT_INCLUDE);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
    } else if(strcmp(NatType, "1") == 0) { /* easy ip */
	if(checkWanIpInPool(addr_OutIP, 1) == -1)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_NOT_EQUAL);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
    }

    if(checkWanIpInPool(addr_InFromIP,nInEndIp-nInFromIp+1) == -1)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_INNER_IP_NOT_INCLUDE);
	setTheErrorMsg(errMsg);
	goto out;
    }

    if(!strcmp(action, "add"))
    {
	if(instIndex != PROFFAIL)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_RULE_NAME_REPEAT);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if(strtoul(NatType,0,10) ==  ONE2ONE)
	{
#if (NAT_BIND_IP_ALL == FYES)
	    if( (BindIpCT+(nInEndIp-nInFromIp+1)) > MAX_NAT_BIND_IP)
	    {
		NAT_PRNTF("1one2onect=%u\n",(BindIpCT + (nInEndIp-nInFromIp+1)));
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_BAND);
		setTheErrorMsg(errMsg);
		goto out;
	    }
#else
	    if( (nInEndIp-nInFromIp+1) > MAX_NAT_BIND_IP)
	    {
		NAT_PRNTF("1one2onect=%u\n",(nInEndIp-nInFromIp+1));
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_BAND);
		setTheErrorMsg(errMsg);
		goto out;
	    }
#endif
	    BindEthID=getWanPoint(nInEndIp-nInFromIp+1,nOutIp,NULL);
	    if (BindEthID == -1) {
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
	else if(strtoul(NatType,0,10) == EASYIP)
	{
#if (NAT_BIND_IP_ALL == FYES)
	    if( (BindIpCT+1) > MAX_NAT_BIND_IP)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_BAND);
		setTheErrorMsg(errMsg);
		goto out;
	    }
#endif
	    BindEthID=getWanPoint(1,nOutIp,NULL);
	    NAT_PRNTF("BindEthID = %d\n",BindEthID);
	    if (BindEthID == -1) {
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
	instIndex = ProfNewInst(mibType, RuleId, FALSE);
	if(instIndex == PROFFAIL) 
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	profAction = PROF_CHANGE_ADD;
    }
    else //modify
    {
	if(instIndex == PROFFAIL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_NAT_RULE_NOT_FIND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
	if (strcmp(RuleIdOld,RuleId) != 0){
	    int newIndex;
	    newIndex = ProfGetInstIndexByName(mibType, RuleId);
	    if(newIndex != PROFFAIL)
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_RULE_NAME_REPEAT);
		setTheErrorMsg(errMsg);
		goto out;
	    }
	}
	profAction = PROF_CHANGE_EDIT;
	prof = (NatRuleProfile*)ProfGetInstPointByIndex(mibType, instIndex);

	if(prof->NatruleType == ONE2ONE)
	{
	    nEditBindIpOld = htonl(prof->InEndIP)-htonl(prof->InFromIP)+1; /* old bingip number*/
	}
	else if(prof->NatruleType == EASYIP)
	{
	    nEditBindIpOld = 1; /* old bingip number*/ 	
	}


	if(strtoul(NatType,0,10) ==  ONE2ONE){
	    nEditBindIpNew = nInEndIp-nInFromIp+1; /* new bing ip number*/
	}
	else if(strtoul(NatType,0,10) == EASYIP)   
	{  
	    nEditBindIpNew = 1; /* new bing ip number*/
	}
	//NAT_PRNTF("one2onect=%d,nEditBindIpOld =%x,nEditBindIpNew =%x\n",BindIpCT,nEditBindIpOld,nEditBindIpNew);
	if(strtoul(NatType,0,10) != PASSTHROUGH) {
#if (NAT_BIND_IP_ALL == FYES)
	if( (BindIpCT-nEditBindIpOld+nEditBindIpNew) > MAX_NAT_BIND_IP)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_BAND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
#else
	if( (nEditBindIpNew) > MAX_NAT_BIND_IP)
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_BAND);
	    setTheErrorMsg(errMsg);
	    goto out;
	}
#endif
	}

	oldEthId  = prof->EthId;
            NAT_PRNTF("nOutIp[%x]\n", nOutIp);
	    if(strtoul(NatType,0,10) != PASSTHROUGH) {
            BindEthID=getWanPoint(nEditBindIpNew, nOutIp,RuleIdOld);
            if (BindEthID == -1) {
		errMsg = getTransDataForC(C_LANG_INDEX_EXTER_IP_REPEAT);
                setTheErrorMsg(errMsg);
		BindEthID = oldEthId;
                goto out;
	    }
	    }
    }
    ProfBackupByIndex(mibType, profAction, instIndex, &profhead);
    natRuleProfile= (NatRuleProfile*)ProfGetInstPointByIndex(mibType, instIndex);
    ProfSetNameByPoint(natRuleProfile, RuleId);
    natRuleProfile->NatruleType= strtoul(NatType,0,10);
    if(strtoul(NatType,0,10) != PASSTHROUGH){
    natRuleProfile->OutIp = addr_OutIP.s_addr;
    }
    natRuleProfile->EthId = BindEthID;
    natRuleProfile->InFromIP = addr_InFromIP.s_addr;
    natRuleProfile->InEndIP = addr_InEndIp.s_addr;
    strcpy(natRuleProfile->NatBind,NatBind);
    NAT_PRNTF("NatruleType = %d OutIp = %x EthId = %d InFromIP = %x InEndIP = %x NatBind =%s\n",natRuleProfile->NatruleType ,htonl(natRuleProfile->OutIp),natRuleProfile->EthId,htonl(natRuleProfile->InFromIP),htonl(natRuleProfile->InEndIP),natRuleProfile->NatBind);
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();
out:
    websRedirect(wp, "NatRule.asp");
}
/***********************************************************************
 * 函 数 名：   formNatRuleListDel
 * 创建日期：	2011-10-18
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formNatRuleListDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *RuleId=NULL,*tmp, *ptr;
    const char *errMsg=NULL;
    a_assert(wp);
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    struProfAlloc *profList  = NULL;
#if 0
    int back = 0;
#endif

    RuleId = websGetVar(wp,T("delstr"),T("")); 
    if(!strcmp(RuleId,T("")))
    {   
	errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	setTheErrorMsg(errMsg);
	goto out;
    }
    tmp=strtok_r(RuleId,UTT_SPLIT_SIGN_STR,&ptr);//可能删除多条映射
    while(tmp!=NULL)
    {

	ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
	ProfDelInstByName(mibType, tmp); 
	NatRuleCounter--;
	tmp=strtok_r(NULL, UTT_SPLIT_SIGN_STR,&ptr);
    }

    if(NatRuleCounter==1)
    {
	BindEthID=1;
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
out:
    websRedirect(wp,"NatRule.asp");
}
/***********************************************************************
 * 函 数 名：   formNatRuleListDelAll
 * 创建日期：	2011-10-18
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/
static void formNatRuleListDelAll(webs_t wp, char_t *path, char_t *query)
{   
    const char *tmp = NULL;
    int min = 0, max = 0;
    int i = 0;
    int back = 0;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    Pointer prof;
    struProfAlloc *profList  = NULL;
    a_assert(wp);
    ProfInstLowHigh(mibType, &max, &min);
    BindEthID=1;
    for(i = min; i < max; i++) {
	prof = ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(back == 0) {
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back = 1;
	    }
	    ProfBackupByIndex(mibType, PROF_CHANGE_SPECIAL, i, &profList);
	    tmp = ProfGetInstNameByIndex(mibType,i);
	    ProfDelInstByIndex(mibType, i);
	}
    }
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    nvramWriteCommit();
    websRedirect(wp, "NatRule.asp");
}
/***********************************************************************
 * 函 数 名：   formDefineNatRuleConfig
 * 创建日期：	2011-10-18
 * 修改日期：
 * 作者：       yqyang
 * 附加说明：	无
 ***********************************************************************/

void formDefineNatRuleConfig(void)
{ 
    websFormDefine(T("formNatRule"),formNatRuleList);
    websFormDefine(T("formNatRuleDelAll"),formNatRuleListDelAll);
    websFormDefine(T("formNatRuleDel"),formNatRuleListDel);
    websAspDefine(T("aspOutNatRuleOneInfo"),getNatOneRuleInfo);
    websAspDefine(T("aspOutNatRule"),getNatRuleList);
    websAspDefine(T("aspOutNatRulePassthroughDisPlay"),aspOutNatRulePassthroughDisPlay);
}
#endif
