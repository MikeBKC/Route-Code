//#include "ldra.h"
#ifndef LDRA2
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
#endif
#define     INDEX_RULEID    0 
#define     INDEX_TYPE      1
#define     INDEX_OUTIP     2 
#define     INDEX_INFROMIP  3
#define     INDEX_INENDIP   4 

#define     TYPE_EASYIP        "1" 
#define     TYPE_ONE2ONE       "2" 



static int NatRuleCounter=0; 
static int BindEthID=1;  //Ã¿Ìí¼ÓÒ»Ìõone2one¹æÔò¶¼Òª°ó¶¨ÐÂipµ½wan¿Ú
static int BindIpCT=0;  //NAT¹æÔòÔÚWAN¿Ú°ó¶¨µÄIPÊý

int getNatOutIpCnt(char *natList);
extern char * NextRecordStart(char*Record);
#ifndef LDRA2
/***********************************************************************
 * º¯ Êý Ãû£º   delAllBindIp
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/
void delAllBindIp(char* action)
{
    char_t *RuleList,*tmp,*wanName,type[2];
    unsigned int ethID,nInFromIp,nInEndIp,ipNum,i;
    RuleList = nvram_bufget(RT2860_NVRAM,"NatRuleList");
    tmp=RuleList;
    while(*tmp)
    {
        if(sscanf(tmp,"%*[^,],%[^-]-%x,%*[^,],%x,%x",type,&ethID,&nInFromIp,&nInEndIp)==4)//name,
        {
             wanName=getWanIfNamePPP(1);
             ipNum=1;
             if(!strcmp(type,TYPE_ONE2ONE)) 
             {
                 ipNum=nInEndIp-nInFromIp+1;
             }
             for(i=0;i<ipNum;i++)
	     {
		 DBGPRINT("ifconfig %s:%d %s\n",wanName,(i+ethID),action);
                 doSystem("ifconfig %s:%d %s",wanName,(i+ethID),action);
	     }
        }
        tmp=NextRecordStart(tmp);
    }
    BindIpCT=0;
}
/***********************************************************************
 * º¯ Êý Ãû£º   EditOneNatRule
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/
static int EditOneNatRule(char *natList,char action)
{
    char ip1[16]={0},ip2[16]={0},ip3[16]={0},wanIp[16]={0},*wanName, NatBind[8];
    unsigned int nOutIp,nInFromIp,nInEndIp,ethId,i,ipNum,nWanIp, wanIndex;
    unsigned char Type[2]={0};
    DBGPRINT("NatRule -%c :%s\n",action,natList);
    DBGPRINT("BindEthID=%d\n",BindEthID);
    if(sscanf(natList,"%*[^,],%[^-]-%x,%x,%x,%x,%[^;]",Type,&ethId,&nOutIp,&nInFromIp,&nInEndIp,NatBind)==6)//name,
    {
        DBGPRINT("ethId=%d\n",ethId);
        wanIndex=NatBind[3]-'0';
        wanName=getWanIfNamePPP(wanIndex);
        getIfIp(wanName,wanIp);
        nWanIp=ip2int(wanIp);
	ipNum=1;
        if(!strcmp((char*)Type,TYPE_ONE2ONE)) 
        {
            ipNum=nInEndIp-nInFromIp+1;
	}
        for(i=0;i<ipNum;i++)
        {
	    if(action==Iptables_ADD)
	    { 
	         if(nWanIp!=nOutIp+i)//Ìí¼ÓIP 
		 {
	              doSystem("ifconfig %s:%d down",wanName,(ethId+i));
	              doSystem("ifconfig %s:%d %s",wanName,(ethId+i),int2ip((nOutIp+i),ip1));
		 }
		 BindIpCT++;
            }
            else//delete
	    {
	         if(nWanIp!=nOutIp+i)
	         {
		      doSystem("ifconfig %s:%d down",wanName,(ethId+i));
		 }
                 BindIpCT--;
            }
            if(!strcmp((char*)Type,TYPE_ONE2ONE)) 
	    {
		doSystem("iptables -t nat -%c "PORT_FORWARD_RULE_CHAIN" -s %s -o %s -j SNAT --to %s",\
 		      action ,int2ip((nInFromIp+i),ip1),wanName,int2ip((nOutIp+i),ip2));
	    }
	    else
	    {
	        doSystem("iptables -t nat -%c "PORT_FORWARD_RULE_CHAIN" -m iprange --src-range %s-%s -o %s -j SNAT --to %s",\
                       action,int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2),wanName,int2ip(nOutIp,ip3));
	    }
        }
    }
    else
    {
        DBGPRINT("failed to modify\n");
        return -1;       
    }   
    return 0;
}
/***********************************************************************
 * º¯ Êý Ãû£º   init_NatRule
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/

void init_NatRule(char action)
{
    char_t*RuleList,*tmp; /* ,*opmode;*/

    RuleList = nvram_bufget(RT2860_NVRAM,"NatRuleList");
    tmp=RuleList;
    if(action==Iptables_ADD)
    {
	BindIpCT=0;
        BindEthID=1;
    }
    while(*tmp)
    {
        EditOneNatRule(tmp,action);
	tmp=NextRecordStart(tmp);
    }
    //EditDefaultNatRule(Iptables_ADD);
#if 0    
    opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
    if(!strcmp(opmode ,"3"))
	doSystem("ifconfig eth2.2 down");
#endif
}
/***********************************************************************
 * º¯ Êý Ãû£º   getNatOneRuleInfo
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/


static int getNatOneRuleInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name,*RuleList,*tmp,RuleName[12],Type[2],ip[16]={0},NatBind[8];
    unsigned int nOutIp,nInFromIp,nInEndIp;
    DBGPRINT("***********getNatOneRuleInfo**********\n");
    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	setTheErrorMsg(T("¶ÁÈ¡edit_Name±äÁ¿Ê§°Ü!"));
        goto out;
    }
    DBGPRINT("edit_name:%s\n",edit_name);
    if(!edit_name)
    {
	DBGPRINT("ÓÃ»§Ãû²»ÄÜÎª¿Õ!\n");    
        setTheErrorMsg(T("ÓÃ»§Ãû²»ÄÜÎª¿Õ!"));
        goto out;
    }
    RuleList = nvram_bufget(RT2860_NVRAM,"NatRuleList");
    tmp=strnmatch(RuleList,edit_name,INDEX_RULEID);

    if(!tmp)
    {
        setTheErrorMsg(T("ÕÒ²»µ½¸ÃÓÃ»§!"));
        goto out;
    }
    if(sscanf(tmp,"%[^,],%[^-]%*[^,],%x,%x,%x,%[^;]",RuleName,Type,&nOutIp,&nInFromIp,&nInEndIp,NatBind)==6)//name,
    {
        websWrite(wp,"var RuleIDs=\"%s\";",RuleName);
	websWrite(wp,"var NatTypes=\"%s\";",Type);
        int2ip(nOutIp,ip);
	websWrite(wp, "var OutIPs=\"%s\";",ip);
        int2ip(nInFromIp,ip);
	websWrite(wp, "var InFromIPs=\"%s\";",ip);
        int2ip(nInEndIp,ip);
	websWrite(wp, "var InEndIPs=\"%s\";",ip);
        websWrite(wp,"var Binds=\"%s\";",utt_strtoup(NatBind));
    }
out:
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/***********************************************************************
 * º¯ Êý Ãû£º   getNatRuleList
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/
static int getNatRuleList(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t RuleName[12],Type[2],ip[16]={0},*RuleList,*tmpRecord,NatBind[8];
    unsigned int num=0,nOutIp,nInFromIp,nInEndIp;
    char_t *RuleId=T("RuleIDs[%d] = \"%s\";");
    char_t *NatType=T("NatTypes[%d] = \"%s\";"); 
    char_t *OutIp =T("OutIPs[%d] = \"%s\";");
    char_t *InFromIp =T("InFromIPs[%d] = \"%s\";");
    char_t *InEndIp = T("InEndIPs[%d] = \"%s\"; ");
    char_t *NatBinds = T("Binds[%d] = \"%s\"; ");
    websWrite(wp,"var RuleIDs=new Array();");
    websWrite(wp,"var NatTypes = new Array();");
    websWrite(wp,"var OutIPs = new Array();");
    websWrite(wp,"var InFromIPs=new Array();");
    websWrite(wp,"var InEndIPs=new Array();");
    websWrite(wp,"var Binds=new Array();");

    RuleList = nvram_bufget(RT2860_NVRAM,"NatRuleList");
    tmpRecord =RuleList;
    while(*tmpRecord)
    {
        DBGPRINT("tmpRecord[%s]\n", tmpRecord);
        if(sscanf(tmpRecord,"%[^,],%[^-]%*[^,],%x,%x,%x,%[^;]",RuleName,Type,&nOutIp,&nInFromIp,&nInEndIp,NatBind)==6)//name,
	{
	    websWrite(wp,RuleId,num,RuleName);
	    websWrite(wp,NatType,num,Type);
            int2ip(nOutIp,ip);
            websWrite(wp, OutIp, num,ip); 
            int2ip(nInFromIp,ip);
	    websWrite(wp,InFromIp,num,ip);
            int2ip(nInEndIp,ip);
	    websWrite(wp,InEndIp,num,ip);
	    websWrite(wp,NatBinds,num,wanReName(utt_strtoup(NatBind)));
	}
        num++;
	tmpRecord=NextRecordStart(tmpRecord);
    }
    //ÏÔÊ¾Ä¬ÈÏ¹æÔòµ½ÁÐ±íÖÐ
#if	0 
    *ip=0;
    getIfIp(getWanIfNamePPP(1),ip);
    websWrite(wp,T("RuleIDs[%d] =\"default\";") ,num);
    websWrite(wp,T("NatTypes[%d] =\"1\";") ,num);
    websWrite(wp,T("OutIPs[%d] =\"%s\";") ,num,ip);
    websWrite(wp,T("InFromIPs[%d] =\"0.0.0.0\";") ,num);  //Ä¬ÈÏ¹æÔò£¬×ª»»ËùÓÐ´ÓWAN¿Ú³öÈ¥µÄ°üµÄÔ´IP
    websWrite(wp,T("InEndIPs[%d] =\"0.0.0.0\";") ,num);
    websWrite(wp,T("Binds[%d] =\"WAN1\";") ,num++);
#endif
    NatRuleCounter = num;
    DBGPRINT("counter of the Rule is %d\n",NatRuleCounter);
    websWrite(wp,"var totalrecs=%d;",num);
    BindIpCT=getNatOutIpCnt(RuleList);
    DBGPRINT("BindIpCT=%d\n",BindIpCT);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

#define AddRecord   strcat(newList,RuleId);\
                    strcat(newList,UTT_SPLIT_SIGN_STR);\
                    strcat(newList,NatType);\
                    sprintf(ip1,"-%x",BindEthID);\
                    strcat(newList,ip1); /*°ó¶¨Íø¿¨ºÅ*/\
                    strcat(newList,UTT_SPLIT_SIGN_STR);\
                    strcat(newList,sOutIp);\
                    strcat(newList,UTT_SPLIT_SIGN_STR);\
                    strcat(newList,sInFromIp);\
                    strcat(newList,UTT_SPLIT_SIGN_STR);\
                    strcat(newList,sInEndIp)
/* return the position from [natList] that can containe the [ipNum] 
 * if the ip range from [outFromIp] to [outFromIp]+ipNum-1 is overlapd, -1 is returnd */
int getWanPoint(char *natList, unsigned int ipNum, unsigned int outFromIp)
{
    unsigned int natcnt=0U,i,j,wanPoint=0U, ipRange;
    struct sNatRule{
        unsigned char Type[2];
        unsigned int ethId;
        unsigned int nOutIp;
        unsigned int nInFromIp;
        unsigned int nInEndIp;
        unsigned char NatBind[8];
    }natRules[UTT_MAX_NAT_BIND_IP],natRule, natRule2;
    int found=0;
   
    /* initiate */
    while(sscanf(natList,"%*[^,],%[^-]-%x,%x,%x,%x,%[^;]",natRules[natcnt].Type,&natRules[natcnt].ethId,
                &natRules[natcnt].nOutIp,&natRules[natcnt].nInFromIp,&natRules[natcnt].nInEndIp,natRules[natcnt].NatBind)==6) {
        natcnt++;
        natList=NextRecordStart(natList);
    }
    DBGPRINT("natcnt[%d]\n",natcnt);

    /* add zero , make sure count from zero*/
    strcpy(natRules[natcnt].Type,TYPE_EASYIP);
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
        DBGPRINT("ethId[%d],nInFromIp[%x],nInEndIp[%x]\n",natRules[i].ethId, natRules[i].nInFromIp, natRules[i].nInEndIp);
        if(strcmp(natRules[i].Type, "1")==0) {
            ipRange = 1;
        } else {
            ipRange = natRules[i].nInEndIp-natRules[i].nInFromIp + 1U;
        }
        if (( (outFromIp+ipNum-1U < natRules[i].nOutIp) || (outFromIp > (natRules[i].nOutIp+ipRange-1U)) )==0) {
            DBGPRINT("ip range overlaped\noutFromIp[%x],ipNum[%d]\nnatRules.nOutIp[%x],ipRange[%d]\n",
                    outFromIp, ipNum, natRules[i].nOutIp, ipRange);
            wanPoint=-1U;
            break;
        }
        if(found){
            continue;
        }

        /*if(strcmp(natRules[i].Type, "1")==0) {
            wanPoint = natRules[i].ethId + 1U;
        } else {
            wanPoint=natRules[i].ethId + ipRange;
        }*/ 
        wanPoint=natRules[i].ethId + ipRange;

        if (i == natcnt-1U) {
            break;
        }
        DBGPRINT("free range[%d]-[%d]:[%d]\n",wanPoint,natRules[i+1U].ethId-1U, natRules[i+1U].ethId-wanPoint);
        if ( (wanPoint + ipNum - 1U) < natRules[i+1U].ethId) {
            found=1;
        }
    }
    return wanPoint;
}
#endif
/* get the used out-ip count */
int getNatOutIpCnt(char *natList)
{
    char  NatBind[8];
    unsigned int nOutIp,nInFromIp,nInEndIp,ethId,ipNum=0U,addnum;
    char Type[2];
    memset(Type, 0x00, sizeof(Type));
    /* sort the natList by ethId */
    while(sscanf(natList,"%*[^,],%[^-]-%x,%x,%x,%x,%[^;]",Type,&ethId,&nOutIp,&nInFromIp,&nInEndIp,NatBind)==6) {
        DBGPRINT("ipfrom[%x],ipend[%x]\n",nInFromIp,nInEndIp);
        if(strcmp(Type, "1")==0) {
            addnum=1U;
        } else {
            addnum=nInEndIp-nInFromIp+1U;
        }
        ipNum+=addnum;

        DBGPRINT("now Type[%s],NatBind[%s],ipNum[%d]\n",Type, NatBind, ipNum);

        natList=NextRecordStart(natList);
        if (natList == NULL ){
            break;
        }else {
            natList++;
        }
    }
    return (int)ipNum;
}
#ifndef LDRA2
/***********************************************************************
 * º¯ Êý Ãû£º   formNatRuleList
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/
static void formNatRuleList(webs_t wp, char_t *path, char_t *query)
{
    char_t *RuleList,*RuleId,*RuleIdOld,*NatType,*OutIp,*InFromIp,*InEndIp,*action,*NatBind,*newList,*p1,*p2;//,ip1[16]={0};
    char_t s1[9]={0},s2[9]={0},*tmp;
    unsigned int nOutIp,nInFromIp,nInEndIp,n1,n2,oldEthId;
    int tempLen;
    DBGPRINT("********formConfigNatRule********\n");
    a_assert(wp);
    RuleList = nvram_bufget(RT2860_NVRAM,"NatRuleList");
    RuleId=websGetVar(wp,T("RuleID"), T(""));
    NatType=websGetVar(wp,T("NatType"), T(""));
    OutIp=websGetVar(wp,T("OutIP"), T(""));
    InFromIp=websGetVar(wp,T("InFromIP"), T(""));
    InEndIp=websGetVar(wp,T("InEndIP"), T(""));
    action = websGetVar(wp, T("Action"), T("")); 
    NatBind = websGetVar(wp, T("Bind"), T("")); 
    utt_strtoup(NatBind); /* wanIndex */
    
    nOutIp=ip2int(OutIp);
    nInEndIp=ip2int(InEndIp);
    nInFromIp=ip2int(InFromIp);


    DBGPRINT("NatRuleList before change:%s\n",RuleList);
    DBGPRINT("new:RuleId:%s,Nattype:%s,outip:%x,InFromip:%x,InEndip:%x,Bind:%s\n",RuleId,NatType,nOutIp,nInFromIp,nInEndIp,NatBind);
    tempLen=strlen(RuleList)+strlen(RuleId)+38+strlen(NatBind);
    
    if(!strcmp(action, "add"))
    {
        if(NatRuleCounter >= UTT_MAX_NAT_RULE)
	{
            setTheErrorMsg(T("ÒÑ´ï×î´óÊýÄ¿£¡"));
	    goto out;
        }
	if( 
            strnmatch(RuleList,RuleId,INDEX_RULEID)
	  )
	{
            setTheErrorMsg(T("¹æÔòÃûÖØ¸´"));
	    goto out;
        }
#if 1 
        DBGPRINT("one2onect=%d\n",BindIpCT);
        if(!strcmp(NatType,TYPE_ONE2ONE))
	{
	    if( (BindIpCT+(nInEndIp-nInFromIp+1)) > UTT_MAX_NAT_BIND_IP)
	    {
                DBGPRINT("one2onect=%u\n",(BindIpCT + (nInEndIp-nInFromIp+1)));
                setTheErrorMsg(T("°ó¶¨Íâ²¿ipÊý¹ý¶à!"));
                goto out;
	    }
            BindEthID=getWanPoint(RuleList,nInEndIp-nInFromIp+1,nOutIp);
            if (BindEthID == -1) {
                setTheErrorMsg(T("Íâ²¿IPµØÖ·¶ÎÖØµþ"));
                goto out;
	    }
	}
	else
	{
            if( (BindIpCT+1) > UTT_MAX_NAT_BIND_IP)
	    {
                setTheErrorMsg(T("°ó¶¨Íâ²¿ipÊý¹ý¶à!"));
                goto out;
	    }
            BindEthID=getWanPoint(RuleList,1,nOutIp);
            if (BindEthID == -1) {
                setTheErrorMsg(T("Íâ²¿IPµØÖ·¶ÎÖØµþ"));
                goto out;
	    }
	}
#endif
	newList = (char*)malloc(sizeof(char) * tempLen);

        strcpy(newList, RuleList);
	p1=newList+strlen(RuleList);//¼ÇÂ¼ÐÂÌí¼Ó¹æÔòµÄÆðÊ¼Î»ÖÃ£¬×÷Îª²ÎÊýµ÷ÓÃº¯Êý EditOneNatRule()ý
        if(strlen(newList) != 0)
	{
	    strcat(newList, UTT_SPLIT_SIGN_RECORDS);
	    p1++;
	}
        sprintf(p1,"%s,%s-%x,%x,%x,%x,%s",RuleId,NatType,BindEthID,nOutIp,nInFromIp,nInEndIp,NatBind);
    }
    else //modify
    {
        RuleIdOld= websGetVar(wp, T("RuleIDold"), T("")); 
	p1=strnmatch(RuleList,RuleIdOld,INDEX_RULEID);
	if(p1==0)
	{
	    setTheErrorMsg(T("ÕÒ²»µ½¸ÃÌõ¹æÔò!"));
	    goto out;
	}
#if 1
        p2=getnstr(p1,INDEX_TYPE);
        tmp=strchr(p2, UTT_SPLIT_SIGN_CHAR);
        *tmp='\0';
        oldEthId=atoi(p2+2);
        *tmp=UTT_SPLIT_SIGN_CHAR;
        DBGPRINT("oldEthId[%d]\n", oldEthId);

	if(!strncmp(p2,TYPE_ONE2ONE,strlen(TYPE_ONE2ONE)))
        {
            memcpy(s1, getnstr(p1,INDEX_INFROMIP),8 );
            memcpy(s2, getnstr(p1,INDEX_INENDIP),8 );
	    sscanf(s1,"%x",&n1); 
	    DBGPRINT("n1=%s\n",s1);
	    sscanf(s2,"%x",&n2);
	    DBGPRINT("n2=%s\n",s2);
	    n1=n2-n1+1;
	}
	else
	    n1=1;/* old ip num */
	if(!strcmp(NatType,TYPE_ONE2ONE))
	{
	    n2=nInEndIp-nInFromIp+1;
	}
	else 
	    n2=1; /* new ip num */
        DBGPRINT("one2onect=%d,n1=%x,n2=%x\n",BindIpCT,n1,n2);
        if( (BindIpCT-n1+n2) > UTT_MAX_NAT_BIND_IP)
        {
            setTheErrorMsg(T("°ó¶¨Íâ²¿ipÊý¹ý¶à!"));
            goto out;
        }
#endif

	p2=p1;
	while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
	{
	    p2++;
	}//*p2=';'or '\0'
	if( (strcmp(RuleId,RuleIdOld)!=0) && strnmatch(RuleList,RuleId,INDEX_RULEID)// || strnmatch(RuleList,sOutIp,INDEX_OUTIP)
        //    || strnmatch(RuleList,sInFromIp,INDEX_INFROMIP) || strnmatch(RuleList,sInEndIp,INDEX_INENDIP)
	  )
	{
            setTheErrorMsg(T("¹æÔòÃûÖØ¸´"));
            goto out;
	}

        char oldRule[64];
        strncpy(oldRule, p1, 63);
        *p1=0;
        newList = (char*)malloc(sizeof(char) * tempLen);
	strcpy(newList,RuleList);            //copy the string before UserNameOld
	p1=newList+strlen(RuleList);//¼ÇÂ¼ÐÂÌí¼ÓµÄ¹æÔòµÄÆðÊ¼Î»ÖÃ£¬×÷Îª²ÎÊýµ÷ÓÃº¯Êý EditOneNatRule()ý
        sprintf(p1,"%s",p2);
        DBGPRINT("newList[%s]\n", newList);

        //if(n2 > n1) {
            DBGPRINT("nOutIp[%x]\n", nOutIp);
            BindEthID=getWanPoint(newList, n2, nOutIp);
            if (BindEthID == -1) {
                setTheErrorMsg(T("Íâ²¿IPµØÖ·¶ÎÖØµþ"));
                RuleList[strlen(RuleList)]=oldRule[0];
                free(newList);
                goto out;
	    }
        /*} else {
            BindEthID=oldEthId;
        }*/
        DBGPRINT("BindEthID[%d]\n", BindEthID);
        //EditOneNatRule(p1,Iptables_DEL);//down µôÀ´ÄÇ¸öIP 
        sendMsgForSpeed(EDIT_NAT_RULE, 0, Iptables_DEL, oldRule, strlen(RuleIdOld)+43);
        sprintf(p1,"%s,%s-%x,%x,%x,%x,%s%s",RuleId,NatType,BindEthID,nOutIp,nInFromIp,nInEndIp,NatBind,p2);
    }
    nvram_bufset(RT2860_NVRAM,"NatRuleList",newList);
    nvram_commit(RT2860_NVRAM);
    DBGPRINT("p1[%s]\n",p1);
    //EditOneNatRule(p1,Iptables_ADD);//down µôÀ´ÄÇ¸öIP 
    sendMsgForSpeed(EDIT_NAT_RULE, 0, Iptables_ADD, p1, strlen(RuleId)+43);
    free(newList);
out:
    websRedirect(wp, "NatRule.asp");
}
/***********************************************************************
 * º¯ Êý Ãû£º   formNatRuleListDel
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/


static void formNatRuleListDel(webs_t wp, char_t *path, char_t *query)
{
        char_t *RuleId=NULL,*RuleList,*p1,*p2,*tmp,*ptr;
	DBGPRINT("**********formNatMapDel**********\n");
        a_assert(wp);
        tmp = nvram_bufget(RT2860_NVRAM,"NatRuleList");
	RuleList =(char *)malloc(strlen(tmp)+1);
	strcpy(RuleList,tmp);
        RuleId = websGetVar(wp,T("delstr"),T("")); 
	if(!strcmp(RuleId,T("")))
	{   
//	    setTheErrorMsg(T("ÕÒ²»µ½¸ÃÓÃ»§Ãû!\n"));
            goto out;
	}
        tmp=strtok_r(RuleId,UTT_SPLIT_SIGN_STR,&ptr);//¿ÉÄÜÉ¾³ý¶àÌõÓ³Éä
       // init_NatStaticMap(Iptables_DEL);
	while(tmp!=NULL)
	{
	    //DBGPRINT("½øÈëÑ­»·\n");
	    p1=strnmatch(RuleList,tmp,INDEX_RULEID);
            if(!p1)
            {
		if(!strcmp(tmp,"default"))
		{
		    setTheErrorMsg(T("²»ÄÜÉ¾³ýÄ¬ÈÏ¹æÔò"));
		    goto strtok;
		}
		else
		{
		    setTheErrorMsg(T("ÕÒ²»µ½¸Ã¹æÔò"));
	            goto out;
		}
            }
            p2=p1;
            sendMsgForSpeed(EDIT_NAT_RULE, 0, Iptables_DEL, p1, strlen(tmp)+43);
            //EditOneNatRule(p1,Iptables_DEL);
            while( (*p2!=UTT_SPLIT_SIGN_RECORD) && *p2)
            {
	        p2++;
            }//*p2=';'or '\0'
	    if(p1!=RuleList) p1--;   //before p1 there is the char';'
	    else if(*p2) p2++;    //if del the first record ,jump over the char ';'
            *p1=0;
	   // strcpy(newCList,CList);            //copy the string before UserNameOld
	    strcat(RuleList,p2);                             //copy the string after UserNameOld record
            NatRuleCounter-- ;
strtok:
            tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
	}
//	DBGPRINT("end\n");
        nvram_bufset(RT2860_NVRAM,"NatRuleList",RuleList);
        nvram_commit(RT2860_NVRAM);
        //init_NatStaticMap();
	//sendMsgForSpeed(INIT_WAN, 0, 0, NULL, 0);
	if(NatRuleCounter==1)
	{
	    BindEthID=1;
	}
out:
	free(RuleList);
        websRedirect(wp,"NatRule.asp");
}
/***********************************************************************
 * º¯ Êý Ãû£º   formNatRuleListDelAll
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/
static void formNatRuleListDelAll(webs_t wp, char_t *path, char_t *query)
{   
    int i,j;
    char buf[64];
    nvram_bufset(RT2860_NVRAM, "NatRuleList", "");
    nvram_commit(RT2860_NVRAM);
    //init_NatStaticMap();
    //init_DMZ();
    //sendMsgForSpeed(INIT_WAN, 0, 0, NULL, 0);
    doSystem("iptables -t nat -F port_forward_rule");
    BindEthID=1;
    memset(buf, 0x00, sizeof(buf));
    for (j=1; j <= getWanSlotCount(); j++ ) {
        char *wanConnMode = nvram_bufget(RT2860_NVRAM, add_wanx("ConnMode", j, buf));
        if (strlen(wanConnMode) != 0 ) {
            char *wanIfName=getWanIfNamePPP(j);
            for(i=0; i<UTT_MAX_NAT_BIND_IP*2; i++ ) {
                doSystem("ifconfig %s:%d down  2>/dev/null &",wanIfName, i);
            }
        }
    }

    websRedirect(wp, "NatRule.asp");
}
/***********************************************************************
 * º¯ Êý Ãû£º   formDefineNatRuleConfig
 * ´´½¨ÈÕÆÚ£º	2010-3-17
 * ÐÞ¸ÄÈÕÆÚ£º
 * ×÷Õß£º       °ØµÂÐã
 * ¸½¼ÓËµÃ÷£º	ÎÞ
***********************************************************************/

void formDefineNatRuleConfig(void)
{ 
    websFormDefine(T("formNatRule"),formNatRuleList);
    websFormDefine(T("formNatRuleDelAll"),formNatRuleListDelAll);
    websFormDefine(T("formNatRuleDel"),formNatRuleListDel);
    websAspDefine(T("aspOutNatRuleOneInfo"),getNatOneRuleInfo);
    websAspDefine(T("aspOutNatRule"),getNatRuleList);
}
#endif
