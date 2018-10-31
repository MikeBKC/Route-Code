#include "uttMachine.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "argcmd.h"
#include "ifmisc.h"
#include <sys/sysinfo.h>
#include "wuScript.h"
#include "wusL2tpServer.h"
#include "wusPptpServer.h"
#include "translate.h"
#include "uttSem.h"


#if (UTT_IPAUTOREP ==FYES)

#define	    TMPIPSTART   168430081 //10.10.10.1
#define     TMPIPEND     168453375 //10.10.100.255

#define      TEN_TIME    864000
/*
 * �ж�����ip��ַ���Ƿ��ص�
 * ���������
 *  ipS1,ipE1: ��һ��ip��ַ�ε���ʼip�ͽ���ip, �����ֽ���
 *  ipS2,ipE2: �ڶ���ip��ַ�ε���ʼip�ͽ���ip, �����ֽ���
 * ����ֵ:
 *  TRUE��     ����ip��ַ���ص�
 *  FALSE:     ����ip��ַ�β��ص�
 */
int ipSegmentOverlap(Uint32 ipS1, Uint32 ipE1, Uint32 ipS2, Uint32 ipE2) {

    if (ipS1 >= ipS2 && ipS1 <= ipE2) {
	return TRUE;
    }
    if (ipE1 >= ipS2 && ipE1 <= ipE2) {
	return TRUE;
    }
    if (ipS2 >= ipS1 && ipS2 <= ipE1) {
	return TRUE;
    }
    if (ipE2 >= ipS1 && ipE2 <= ipE1) {
	return TRUE;
    }
    return FALSE;
}

/*
 * �����ַ���Ƿ��ͻ
 * �������:
 *  poolType:	Ҫ���ĵ�ַ�����ͣ��������ipS��ipE������pool����dhcp,pppoe,l2tp or pptp
 *  ipS:	��ʼIp��ַ, �����ֽ���
 *  ipE:	����ip��ַ, �����ֽ���
 * ���������	��
 * ����ֵ��
 *  �������ַ�س�ͻ�ĵ�ַ������
 */
extern IpPoolEnum ipPoolConflict(IpPoolEnum poolType, Uint32 ipS, Uint32 ipE) {
    Uint32 ipStart = ntohl(ipS);
    Uint32 ipEnd = ntohl(ipE);

#if (DHCP_SERVER == FYES)
    /*
     * �Ƿ���dhcp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_DHCP) {
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_SERVER;
	DhcpServerProfile *profDhcp = NULL;
	profDhcp = (DhcpServerProfile *)ProfGetInstPointByIndex(mibTypeDhcp, 0);
	if ((profDhcp != NULL) && (ProfInstIsFree(profDhcp) == 0)) {
	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profDhcp->ipStart), ntohl(profDhcp->ipEnd)) == TRUE) {
		return IP_POOL_DHCP;
	    }
	}
    }
#endif

#if (DHCP_POOL == FYES)
    /*
     * �Ƿ���dhcp server��ַ�س�ͻ
     */
    int min = 0, max = 0, index = 0;
    if (poolType != IP_POOL_DHCP) {
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
	ProfInstLowHigh(mibTypeDhcp, &max, &min);

	for (index = min; index < max; index++) {
	    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
	    if (NULL != profDhcp) {
		if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profDhcp->ipStart), ntohl(profDhcp->ipEnd)) == TRUE) {
		    return IP_POOL_DHCP;
		} else {
		    continue;
		}
	    }
	}
    }
#endif

#if (PPPOE_SERVER == FYES)
    /*
     * �Ƿ���pppoe server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_PPPOE) {
	MibProfileType mibTypePppoe = MIB_PROF_PPPOE_SRV_GLO;
	PPPoESrvGloProfile *profPppoe = NULL;
	Uint32 pppoeIpE;
	profPppoe = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypePppoe, 0);
	if ((profPppoe != NULL) && (ProfInstIsFree(profPppoe) == 0)) {
	    if (profPppoe->maxSessCnt == 0) {
		pppoeIpE = ntohl(profPppoe->ipStart);
	    } else {
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->maxSessCnt - 1;
	    }
	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profPppoe->ipStart), pppoeIpE) == TRUE) {
		return IP_POOL_PPPOE;
	    }
	}
    }
#endif
#if (L2TP_SERVER == FYES)
    /*
     * �Ƿ���l2tp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_L2TP) {
	MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
	L2TPSrvGlobalProfile *profL2tp = NULL;
	Uint32 l2tpIpE;
	profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
	if ((profL2tp != NULL) && (ProfInstIsFree(profL2tp) == 0)) {
	    if (profL2tp->poolIpCnt == 0) {
		l2tpIpE = ntohl(profL2tp->poolIpStart);
	    } else {
		l2tpIpE = ntohl(profL2tp->poolIpStart) + profL2tp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profL2tp->poolIpStart), l2tpIpE) == TRUE) {
		return IP_POOL_L2TP;
	    }
	}

    }
#endif
#if (PPTP_SERVER == FYES)
    /*
     * �Ƿ���pptp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_PPTP) {
	MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
	PPTPSrvGlobalProfile *profPptp = NULL;
	Uint32 pptpIpE;
	profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
	if ((profPptp != NULL) && (ProfInstIsFree(profPptp) == 0)) {
	    if (profPptp->poolIpCnt == 0) {
		pptpIpE = ntohl(profPptp->poolIpStart);
	    } else {
		pptpIpE = ntohl(profPptp->poolIpStart) + profPptp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profPptp->poolIpStart), pptpIpE) == TRUE) {
		return IP_POOL_PPTP;
	    }
	}

    }
#endif
    return IP_POOL_NONE;
}

#if (ARP_BINDS == FYES)
extern void arpBindProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const UserProfile* oldProfileDataPtr, const UserProfile* newProfileDataPtr );
#endif
#if (PPTP_SERVER == FYES)
extern void pptpSrvGlobalProfileChange(Pointer tag, MibProfileType profileType,    
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr );
#endif

#if (L2TP_SERVER == FYES)
extern void l2tpSrvGlobalProfileChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr );
#endif

#if (DHCP_SERVER == FYES)
extern  void dhcpServerProfileChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const DhcpServerProfile *oldProfileDataPtr, const DhcpServerProfile *newProfileDataPtr );
#endif

extern  void interfaceProfileChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const InterfaceProfile *oldProf, const InterfaceProfile *newProf );

static  void  getWanInfo(Uint32 *wanIp,Uint32 *wanMask,Uint32  *wanCount)
{
    Uint32  i;
    char if_addr[16] = {0};
    char if_nm[16] = {0};
    int count=0;
    for(i=0;i<*wanCount;i++)
    {

	if((getIfIp(getWanIfNamePPP(i+1), if_addr) == 0) && (getIfNetmask(getWanIfNamePPP(i+1), if_nm) == 0)) {
	    wanIp[count] = ntohl(inet_addr(if_addr));
	    wanMask[count] = ntohl(inet_addr(if_nm));
	    count++;       
	}
    }
    *wanCount=count;
}

static void getLanInfo(Uint32 *lanIp,Uint32 *lanMask,Uint32 *lanCount)
{
    char if_addr[16] = {0};
    char if_nm[16] = {0};


    if((getIfIp(getLanIfName(),if_addr)==0 )&&(getIfNetmask(getLanIfName(),if_nm)==0)){
	lanIp[0] = ip2int(if_addr);
	lanMask[0] = ip2int(if_nm);	
	*lanCount=1;
    }
#if(MULTI_LAN == FYES)//��ô��ȡ������ip�أ�
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if(profIF!=NULL){
	lanIp[1] = ntohl(profIF->ether.sta.ip2);
	lanMask[1] = ntohl(profIF->ether.sta.nm2); 
	lanIp[2] = ntohl(profIF->ether.sta.ip3);
	lanMask[2] =ntohl(profIF->ether.sta.nm3);
	lanIp[3] = ntohl(profIF->ether.sta.ip4);
	lanMask[3] = ntohl(profIF->ether.sta.nm4);
    }
    *lanCount = 4;
#endif
}

static Uint32  UntilFirstOne(Uint32 Mask)//���������������ı�lan��IP�Ĵ�С
{
    int  i=31;
    Uint32 M=Mask;
    while(i>=0){
	if(M&(0x1)==1){
	    break;
	}
	M=M>>1;
	i=i-1;
    }
    M=(0x1)<<(31-i);
    return M;
}
static int  ModifyLanWanConflict(Uint32 wanCount,Uint32 lanCount,Uint32 *wanIp,Uint32 *wanMask,Uint32 *lanIp,Uint32 *lanMask)
{
    int i,j;
    int Conflict=0;
    for(i=0;i<wanCount;i++){
	for(j=0;j<lanCount;j++){
	    if (((wanIp[i] & wanMask[i]) == (lanIp[j] & wanMask[i])) || ((wanIp[i] & lanMask[j]) == (lanIp[j] & lanMask[j]))){
		if(lanIp[j]>=TMPIPSTART && lanIp[j]<TMPIPEND){
		    if(wanMask[i]>=lanMask[j]){
			lanIp[j]=lanIp[j]+UntilFirstOne(lanMask[j]);
		    }
		    else{
			lanIp[j]=lanIp[j]+UntilFirstOne(wanMask[i]);//10.10.x+1.1��ʽ
		    }
		}else{
		    lanIp[j]=TMPIPSTART;
		}
		Conflict=Conflict+(1<<j);	
	    }
	}
    }
    return Conflict;
}

static void _ProfWiteToFlash(void)
{
    struct sysinfo info;
    static int count=0;
    static unsigned long firstWriteTime;
    unsigned long currentTime;
    if(count == 0){
	sysinfo(&info);
	firstWriteTime=info.uptime;
	currentTime=firstWriteTime;
    }
    else{
	sysinfo(&info);
	currentTime=info.uptime;
    }
    count++;
    if(count<=20){
	ProfWiteToFlash();
    }
    else if(count>20 &&(currentTime-firstWriteTime)<TEN_TIME){
    }
    else if(count>20 && (currentTime-firstWriteTime)>=TEN_TIME){
	count=1;
	sysinfo(&info);
	firstWriteTime=info.uptime;	
	currentTime=firstWriteTime;
	ProfWiteToFlash();	
    }
}
static IpPoolEnum isIpPoolConflict(Uint32 lanCounti,Uint32 *lanIps)
{
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    if(lanIps[lanCounti]!=0U){
	cfPoolType=ipPoolConflict(IP_POOL_NONE, htonl(lanIps[lanCounti]), htonl(lanIps[lanCounti]));
    }
    return cfPoolType;
}

static void compareLanMask(Uint32 oldLanIp,Uint32 lanIp,Uint32 *Mask)
{
    Uint32 lan1=oldLanIp;
    Uint32 lan2=lanIp;
    Uint32 first=0xff;
    int n=0;
    *Mask=0x0;
    while(n<4){
	if((lan1&0xff)!=(lan2&0xff)){
	    *Mask=*Mask+(first<<(n*8));
	}
	lan1=lan1>>8;
	lan2=lan2>>8;
	n++;
    }
}
#if (ARP_BINDS == FYES)
static void ArpBindChangeProfile(Uint32 oldLanIp,Uint32 lanIp,Uint32 Mask)
{
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int i = 0,max = 0,min = 0;
    unsigned int Ip=0;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) /*���������������*/
    {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) /*��������*/
	{
	    Ip=ntohl(prof->ip);
	    if((Ip&Mask)==(oldLanIp&Mask)){//ѡ��֮ǰ���ε�ip�󶨲����޸ġ�
		prof->ip=htonl((Ip&(~Mask))|(lanIp&Mask));//�޸�����
	    }
	}
    }
    arpBindProfileChange(NULL,MIB_PROF_USER,PROF_CHANGE_ADD_ALL,0,NULL,NULL);
}
#endif	
void IPClashAutoRepair()
{
    int Change_Flag=0;//���ڱ�ʾ�ڼ����ͻʱ����û�ж�ip�����޸ġ�  
    int Write_Flag=0;
    int i;
    unsigned int nIp, eIp, uNet, nIp1, eIp1;
#if (MULTI_SSID_DHCP_POOL == FYES)
    unsigned int ssid1_nIp, ssid1_eIp, ssid1_nIp1, ssid1_eIp1;
    unsigned int ssid2_nIp, ssid2_eIp, ssid2_uNet, ssid2_nIp1, ssid2_eIp1;
#if (WIRELESS_5G == FYES)
    unsigned int ssid1_nIp_5g, ssid1_eIp_5g, ssid1_nIp1_5g, ssid1_eIp1_5g;
    unsigned int ssid2_nIp_5g, ssid2_eIp_5g, ssid2_uNet_5g, ssid2_nIp1_5g, ssid2_eIp1_5g;
#endif
#endif
    IpPoolEnum cfPoolType = IP_POOL_NONE;

    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    InterfaceProfile oldProfIF;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    Uint32  oldLanIp=0;
    Uint32  Mask=0x0;
    Uint32 lanIp[4] = {0},lanMask[4] = {0};  
    Uint32 wanIp[32]={0},wanMask[32]={0};
    Uint32 wanCount = 0,lanCount=0;

#if(DHCP_SERVER == FYES)
    MibProfileType mibTypeDhcpS = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
    DhcpServerProfile oldProfDhcpS;
#endif
#if (DHCP_POOL == FYES)
    MibProfileType mibTypeDhcpP = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *dhcpPool = NULL;
    DhcpPoolProfile *dhcpPool1 = NULL;
    MibProfileType mibTypeDnsAg = MIB_PROF_DNS_AGENCY;
    DnsAgencyProfile *profDns = NULL;
#endif

#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
#endif

#if (L2TP_SERVER == FYES)
    MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvGlobalProfile *profL2tp = NULL;
    L2TPSrvGlobalProfile oldProfL2tp;
#endif

#if (PPTP_SERVER == FYES)
    MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptp = NULL;
    PPTPSrvGlobalProfile oldProfPptp;
#endif

#if (NET_SECURITY == FYES)
    SystemProfile *profSys = NULL;
#endif

    wanCount = getWanSlotCount();
    getLanInfo(lanIp,lanMask,&lanCount);
    oldLanIp=lanIp[0];
    getWanInfo(wanIp,wanMask,&wanCount);
    if(lanCount==0 ||wanCount ==0){//����ȡ����lan or wan ��ĿΪ0��ʱ���������ͻ
	return ;
    }
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if(profIF!=NULL){//����ȡInterfaceProfile�ṹ��ʧ��ʱ����
	if(profIF->ether.connMode!=ETH_CONN_DHCP){//��������DHCP�����ʱ�������޸�ip��ͻ���������û���������
	    return ;
	}
    }
    else{
	return ;
    }
    Change_Flag = ModifyLanWanConflict(wanCount,lanCount,wanIp,wanMask,lanIp,lanMask);//���ȼ�����Ƿ���ڳ�ͻ
    if(Change_Flag)
    {
	uttSemP(SEM_IPAUTOREP,0);
	while(Change_Flag){
	    Write_Flag = 1;
	    //��Ҫ�Ƚ���lan��ip��ַ��pppoe��l2tp��pptp3����ַ���Ƿ���ڳ�ͻ���������޸�lan��ip��ַ�������¼��lan��wan�Ƿ���ڳ�ͻ��ֱ�����޳�ͻ��ʱ���˳���
#if ((L2TP_SERVER == FYES) || (PPTP_SERVER == FYES) || (PPPOE_SERVER == FYES))
	    while(isIpPoolConflict(0,lanIp) !=IP_POOL_NONE){
		lanIp[0]=lanIp[0]+UntilFirstOne(lanMask[0]);
	    }
#if(MULTI_LAN == FYES)
	    for(i=1;i<4;i++){
		while(isIpPoolConflict(i,lanIp) !=IP_POOL_NONE){
		    lanIp[i]=lanIp[i]+UntilFirstOne(lanMask[i]);
		}	
	    }	
#endif
#endif
	    Change_Flag = ModifyLanWanConflict(wanCount,lanCount,wanIp,wanMask,lanIp,lanMask);
	}
#if(DHCP_SERVER == FYES)
	/*
	 * ���dhcp��ʼ����ip
	 */
	profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibTypeDhcpS, 0);
	if (profDhcpS != NULL) {
	    memcpy(&oldProfDhcpS,profDhcpS,sizeof(oldProfDhcpS)); 
	    nIp = ntohl(profDhcpS->ipStart);
	    eIp = ntohl(profDhcpS->ipEnd);
	    /* ������lan������, dhcp��ַ������ */
	    uNet = lanIp[0] & 0xffffff00;
	    /* dhcp��ַ����lanIp�仯��ĵ�ַ,�����ֽ��� */
	    nIp1 = uNet + (nIp & 0x000000ff);
	    eIp1 = uNet + (eIp & 0x000000ff);

	    if((lanIp[0] <=eIp1) && (lanIp[0] >= nIp1)){
		nIp1=uNet+100;
		eIp1=uNet+200;  	         
	    }



#if (MULTI_SSID_DHCP_POOL == FYES)
    /*if (profDhcpS->ssid1_dhcp_enable == PROF_ENABLE) {*/
	ssid1_nIp = ntohl(profDhcpS->ssid1_ipStart);
	ssid1_eIp = ntohl(profDhcpS->ssid1_ipEnd);
	ssid1_nIp1 = uNet + (ssid1_nIp & 0x000000ff);
	ssid1_eIp1 = uNet + (ssid1_eIp & 0x000000ff);
	if((lanIp[0] <= ssid1_eIp1) && (lanIp[0] >= ssid1_nIp1)){
	    /*if (ssid1_nIp1 != ssid1_nIp) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID1);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID1);
		setTheErrorMsg(errMsg);
	    }
	    goto out;*/
	    ssid1_nIp1=uNet+100;
	    ssid1_eIp1=uNet+200;
	}
   /* }*/
    /*if (profDhcpS->ssid2_dhcp_enable == PROF_ENABLE) {*/
	ssid2_nIp = ntohl(profDhcpS->ssid2_ipStart);
	ssid2_eIp = ntohl(profDhcpS->ssid2_ipEnd);
	ssid2_nIp1 = uNet + (ssid2_nIp & 0x000000ff);
	ssid2_eIp1 = uNet + (ssid2_eIp & 0x000000ff);
	if((lanIp[0] <= ssid2_eIp1) && (lanIp[0] >= ssid2_nIp1)){
	    /*if (ssid2_nIp1 != ssid2_nIp) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID2);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID2);
		setTheErrorMsg(errMsg);
	    }
	    goto out;*/
	    ssid2_nIp1=uNet+100;
	    ssid2_eIp1=uNet+200;

	}
    /*}*/

#if (WIRELESS_5G == FYES)
    /*if (profDhcpS->ssid1_dhcp_enable_5g == PROF_ENABLE) {*/
	ssid1_nIp_5g = ntohl(profDhcpS->ssid1_ipStart_5g);
	ssid1_eIp_5g = ntohl(profDhcpS->ssid1_ipEnd_5g);
	ssid1_nIp1_5g = uNet + (ssid1_nIp_5g & 0x000000ff);
	ssid1_eIp1_5g = uNet + (ssid1_eIp_5g & 0x000000ff);
	if((lanIp[0] <= ssid1_eIp1_5g) && (lanIp[0] >= ssid1_nIp1_5g)){
	    /*if (ssid1_nIp1_5g != ssid1_nIp_5g) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID1_5G);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID1_5G);
		setTheErrorMsg(errMsg);
	    }
	    goto out;*/
	    ssid1_nIp1_5g=uNet+100;
	    ssid1_eIp1_5g=uNet+200;
	}
    /*}*/
   /* if (profDhcpS->ssid2_dhcp_enable_5g == PROF_ENABLE) {*/
	ssid2_nIp_5g = ntohl(profDhcpS->ssid2_ipStart_5g);
	ssid2_eIp_5g = ntohl(profDhcpS->ssid2_ipEnd_5g);
	ssid2_nIp1_5g = uNet + (ssid2_nIp_5g & 0x000000ff);
	ssid2_eIp1_5g = uNet + (ssid2_eIp_5g & 0x000000ff);
	if((lanIp[0] <= ssid2_eIp1_5g) && (lanIp[0] >= ssid2_nIp1_5g)){
	    /*if (ssid2_nIp1_5g != ssid2_nIp_5g) {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_CHANGE_ERR_SSID2_5G);
		setTheErrorMsg(errMsg);
	    } else {
		errMsg = getTransDataForC(C_LANG_INDEX_DHCP_INCLUDE_LAN_SSID2_5G);
		setTheErrorMsg(errMsg);
	    }
	    goto out;*/
	    ssid2_nIp1_5g=uNet+100;
	    ssid2_eIp1_5g=uNet+200;
	}
   /* }*/
#endif
#endif

#if 0
#if (MULTI_SSID_DHCP_POOL == FYES)
	/*2.4G ssid dhcp pool*/
	/*if (profDhcpS->ssid1_dhcp_enable == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid1_nIp1), htonl(ssid1_eIp1));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
	/*if (profDhcpS->ssid2_dhcp_enable == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid2_nIp1), htonl(ssid2_eIp1));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
#if (WIRELESS_5G == FYES)
	/*5G ssid dhcp pool*/
	/*if (profDhcpS->ssid1_dhcp_enable_5g == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid1_nIp1_5g), htonl(ssid1_eIp1_5g));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
	/*if (profDhcpS->ssid2_dhcp_enable_5g == PROF_ENABLE) {*/
	    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(ssid2_nIp1_5g), htonl(ssid2_eIp1_5g));
	    if (cfPoolType != IP_POOL_NONE) {
		    setPoolConflictMsg(cfPoolType);
		    goto out;
	    }
	/*}*/
#endif
#endif
#endif
	    profAction = PROF_CHANGE_EDIT;
	    if(profDhcpS->gateway != htonl(lanIp[0]))
	    {
		/*
		 * IP��ַ���޸�
		 */
		profDhcpS->ipStart = htonl(nIp1);
		profDhcpS->ipEnd = htonl(eIp1);

		/*
		 * IP��ַ���޸�
		 */
#if (MULTI_SSID_DHCP_POOL == FYES)
		/*if (profDhcpS->ssid1_dhcp_enable == PROF_ENABLE) {*/
		    profDhcpS->ssid1_ipStart = htonl(ssid1_nIp1);
		    profDhcpS->ssid1_ipEnd = htonl(ssid1_eIp1);
		/*}*/
		/*if (profDhcpS->ssid2_dhcp_enable == PROF_ENABLE) {*/
		    profDhcpS->ssid2_ipStart = htonl(ssid2_nIp1);
		    profDhcpS->ssid2_ipEnd = htonl(ssid2_eIp1);
		/*}*/
#if (WIRELESS_5G == FYES)
		/*if (profDhcpS->ssid1_dhcp_enable_5g == PROF_ENABLE) {*/
		    profDhcpS->ssid1_ipStart_5g = htonl(ssid1_nIp1_5g);
		    profDhcpS->ssid1_ipEnd_5g = htonl(ssid1_eIp1_5g);
		/*}*/
		/*if (profDhcpS->ssid2_dhcp_enable_5g == PROF_ENABLE) {*/
		    profDhcpS->ssid2_ipStart_5g = htonl(ssid2_nIp1_5g);
		    profDhcpS->ssid2_ipEnd_5g = htonl(ssid2_eIp1_5g);
		/*}*/
#endif
#endif

		profDhcpS->gateway = htonl(lanIp[0]);
	    }
	    /*
	     * dns����������dns�޸�Ϊlan��ip��ַ
	     */
	    if (profDhcpS->DnsProxyEn == FUN_ENABLE) {
		profDhcpS->pDns = htonl(lanIp[0]);
	    }
	}
#endif
#if (DHCP_POOL == FYES)
	int min,max,index;
	/*
	 * ���dhcp��ʼ����ip
	 */
	dhcpPool = (DhcpPoolProfile*)ProfGetInstPointByIndex(mibTypeDhcpP, 0);
	if (dhcpPool != NULL) {

	    nIp = ntohl(dhcpPool->ipStart);
	    eIp = ntohl(dhcpPool->ipEnd);
	    uNet = lanIp[0] & 0xffffff00;
	    /* dhcp��ַ����lanIp�仯��ĵ�ַ,�����ֽ��� */
	    nIp1 = uNet + (nIp & 0x000000ff);
	    eIp1 = uNet + (eIp & 0x000000ff);

	    if((lanIp[0] <= eIp1) && (lanIp[0] >= nIp1)){
		nIp1=uNet+100;
		eIp1=uNet+200;
	    }
	    ProfInstLowHigh(mibTypeDhcpP, &max, &min);
	    for (index = min; index < max; index++) {
		if (index != 0) {
		    dhcpPool1 = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcpP, index);
		    if(ipSegmentOverlap(nIp1, eIp1, ntohl(dhcpPool1->ipStart), ntohl(dhcpPool1->ipEnd)) == TRUE){
			nIp1=uNet+100;
			eIp1=uNet+200;
			break;
		    }
		}
	    }
	    dhcpPool->netmask = htonl(lanMask[0]);
	    if(dhcpPool->gateway != htonl(lanIp[0]))
	    {
		/*
		 * IP��ַ���޸�
		 */
		dhcpPool->ipStart = htonl(nIp1);
		dhcpPool->ipEnd = htonl(eIp1);
		dhcpPool->gateway = htonl(lanIp[0]);
	    }

#if (SERVICE_ZONE == FYES)
	    ServiceZoneProfile *profSz = NULL;
	    MibProfileType mibTypeSz = MIB_PROF_SERVICE_ZONE;
	    profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSz, 0);
	    profSz->gw = dhcpPool->gateway;
#endif
	    /*
	     * dns����������dns�޸�Ϊlan��ip��ַ
	     */
	    profDns = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibTypeDnsAg, 0);
	    if (NULL != profDns) {
		if (profDns->DnsProxyEn == FUN_ENABLE) {
		    dhcpPool->pDns = htonl(lanIp[0]);
		}
	    }
	}
#endif
#if (NET_SECURITY == FYES)
	profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	nIp = ntohl(profSys->sysNetSe.accessCtrlFromIP);
	eIp = ntohl(profSys->sysNetSe.accessCtrlEndIP);
	uNet = lanIp[0] & lanMask[0];
	/*���α仯�������豸���ʿ���*/
	if((((nIp&lanMask[0])!=uNet) || ((eIp&lanMask[0])!=uNet))
#if(MULTI_LAN == FYES)
		&& (((nIp&lanMask[1])!=uNet) || ((eIp&lanMask[1]!=uNet)) 
		    && (((nIp&lanMask[2])!=uNet) || ((eIp&lanMask[2])!=uNet))
		    && (((nIp&lanMask[3])!=uNet) || ((eIp&lanMask[3])!=uNet)))
#endif
	  )
	{
	    /*���ﲻ�豸��ʵ��������speedweb���豸������ز���
	     *���豸���ʿ�����ر仯�������speedweb�е�lan�ڳ�ʼ�������仯*/
	    profSys->sysNetSe.accessCtrlFromIP = 0u;
	    profSys->sysNetSe.accessCtrlEndIP = 0u;
	}
#endif
#if 1
	/*
	 * ����lan������
	 */
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
	if(profIF!=NULL){
	    memcpy(&oldProfIF,profIF,sizeof(oldProfIF));
	    profAction = PROF_CHANGE_EDIT;
	    profIF->ether.sta.ip = htonl(lanIp[0]);
	    profIF->ether.sta.nm = htonl(lanMask[0]);
#if (MULTI_LAN == FYES)
	    profIF->ether.sta.ip2 = htonl(lanIp[1]);
	    profIF->ether.sta.nm2 = htonl(lanMask[1]);
	    profIF->ether.sta.ip3 = htonl(lanIp[2]);
	    profIF->ether.sta.nm3 = htonl(lanMask[2]);
	    profIF->ether.sta.ip4 = htonl(lanIp[3]);
	    profIF->ether.sta.nm4 = htonl(lanMask[3]);
#endif
	}
#endif
#if (STATIC_NAT == FYES)
	/*
	 * Զ�̹���̬nat ����ip����lan�仯
	 */
	profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, remCtrlName);
	if (profNat != NULL) {
	    profNat->IP = htonl(lanIp[0]);
	}
#endif
#if (L2TP_SERVER == FYES)
	/* ����ip�仯 */
	profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
	memcpy(&oldProfL2tp,profL2tp,sizeof(oldProfL2tp));
	if (profL2tp != NULL) {
	    profAction = PROF_CHANGE_EDIT;
	    profL2tp->listenIp = htonl(lanIp[0]);
#if (STATIC_NAT == FYES)
	    /* l2tp��̬ӳ��仯 */
	    if (profL2tp->head.active == 1) {
		profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, L2TP_NAT_NAME);
		if (profNat != NULL) {
		    profNat->IP = htonl(lanIp[0]);
		}
	    }
#endif
	}
#endif

#if (PPTP_SERVER == FYES)
	/* ����ip�仯 */
	profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
	memcpy(&oldProfPptp,profPptp,sizeof(oldProfPptp));
	if (profPptp != NULL) {
	    profAction = PROF_CHANGE_EDIT;
	    profPptp->listenIp = htonl(lanIp[0]);
#if (STATIC_NAT == FYES)
	    /* pptp��̬ӳ��仯 */
	    if (profPptp->head.active == 1) {
		profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, PPTP_NAT_NAME);
		if (profNat != NULL) {
		    profNat->IP = htonl(lanIp[0]);
		}
	    }
#endif
	}
#endif
	//Ϊ�˽���޸�lan��IPʱ���IP/MAC��̬�󶨳��ֳ�ͻ��ò��û����Ч
	if(oldLanIp!=lanIp[0]){
#if (ARP_BINDS == FYES)
	    compareLanMask(oldLanIp,lanIp[0],&Mask);	
	    ArpBindChangeProfile(oldLanIp,lanIp[0],Mask);
#endif
	}
	//profileChange����
#if(DHCP_SERVER == FYES)
	if(profDhcpS !=NULL){
	    dhcpServerProfileChange(NULL,MIB_PROF_DHCP_SERVER,PROF_CHANGE_EDIT,0,&oldProfDhcpS,profDhcpS);
	}
#endif
	if(profIF!=NULL){		    
	    interfaceProfileChange(NULL,MIB_PROF_INTERFACE,PROF_CHANGE_EDIT,0,&oldProfIF,profIF);
	}
#if (L2TP_SERVER == FYES)
	if(profL2tp!=NULL){
	    l2tpSrvGlobalProfileChange(NULL,MIB_PROF_L2TP_SRV_GLOBAL,PROF_CHANGE_EDIT,0,&oldProfL2tp,profL2tp);
	}
#endif
#if (PPTP_SERVER == FYES)
	if(profPptp!=NULL){
	    pptpSrvGlobalProfileChange(NULL,MIB_PROF_PPTP_SRV_GLOBAL,PROF_CHANGE_EDIT,0,&oldProfPptp,profPptp);
	}
#endif
	if(Write_Flag == 1){
	    _ProfWiteToFlash();
	}
	uttSemV(SEM_IPAUTOREP,0);
    }
}
#endif
