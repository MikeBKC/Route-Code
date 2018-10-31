/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * SCCS ID:	$Id: clibbb.c,v 1.2 2012/08/29 10:00:59 yu.dehai Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/


#include "typedef.h"
#include "profiles.h"
#include "argcmd.h"
#include "stdio.h"
#include "cmd.h"
#include "cmdlist.h"
#include "mibtmrg.h"

#if (MDF_IKE == FYES && MDF_IP_SEC == FYES)
IPSecConfProfile *ipsecConfProf = 0;	/* add by lyu */
IPSecTransformSetProfile *ipsecTransformSetProf=0;
IsakmpConfProfile *isakmpConfProf=0;
IsakmpPolicyProfile *isakmpPolicyProf=0;
#endif

# if ( FIREWALL == FYES )
FwAddressProfile *fwAddressProf=0;  /* add by cyh */
FwServiceProfile *fwServiceProf=0;
FwL7filterProfile *fwL7filterProf=0;
# endif

#if (PDB == FYES)
PdbProfile *pdbProf=0;  /* add by cyh*/
#endif

#if ( ISP_CFG == FYES )
IspProfile *ispProf=0;  /* add by cyh*/
#endif

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang */
IpGroupProfile *ipGroupProf=0; 
PortGroupProfile *portGroupProf=0;  /*add by brwang*/
#endif
#endif

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang*/
const LcdParamEnumValue         servicesEnum[] = {
    { "normal",         IP_NORMAL },
    { "key",            IP_KEY },
    { "url",            IP_URL },
    { "dns",            IP_DNS },
    { "mac",            IP_MAC },
    { NULL,             0 }
};
const LcdParamEnumValue         web_servicesEnum[] = {
    { "普通服务",         IP_NORMAL },
    { "关键字",            IP_KEY },
    { "URL",            IP_URL },
    { "DNS",            IP_DNS },
    { "MAC",            IP_MAC },
    { NULL,             0 }
};
const LcdParamEnumValue         ipgroupEnum[] = {
    { "eth1",           IP_LAN },
    { "eth2",           IP_WAN1 },
    { "eth3",           IP_WAN2 },
    { "eth4",           IP_WAN3 },
    { "eth5",           IP_WAN4 },
    { "allwan", IP_ALLWAN },
    { NULL,             0 }
};
const LcdParamEnumValue         web_ipgroupEnum[] = {
    { "LAN",           IP_LAN },
    { "WAN1",           IP_WAN1 },
    { "WAN2",           IP_WAN2 },
    { "WAN3",           IP_WAN3 },
    { "WAN4",           IP_WAN4 },
    { "ALL WAN", IP_ALLWAN },
    { NULL,             0 }
};


#endif
#endif



# if ( FIREWALL == FYES )
const LcdParamEnumValue fwL7filterType[] = {
        {"url_alg",              0},
        {"str_alg",              1},
        {NULL,                  0}
};
# endif

#if (PDB == FYES)
const LcdParamEnumValue pdbMode[] = {
        {"Predefined",              0},
        {"Userdefined",              1},
        {NULL,                  1}	//modify by jyshen 061010
};

const LcdParamEnumValue pdbType[] = {
        {"Filter",              0},
        {"Route",   1},
        {"AutoCmd", 2},	//add by jyshen 070308
        {"Dns", 3},	
        {"Url", 4},	
	{"Priority", 5},
        {NULL,                  0}
};
#endif

const LcdParamEnumValue ispType[] = {
        {"Pppoe",              0},
        {"Fix",   1},
	{"Dyn",   2},
        {NULL,                  1}
};


const LcdParamEnumValue ipsecMethod[] = {
	{"Manual",		0},
	{"Isakmp", 		1}, 
	{NULL,	 		1}
};
			
const LcdParamEnumValue ipsecMode[] = {
	{"Tunnel",		0},
	{"Transport",	1},
	{NULL,			0}
};
				
const LcdParamEnumValue encryptESP[] = {
	{"None",		0},
	{"Des",			1},
	{"3Des",		2},
	{"Aes",			3},
	{"Rc4",			4},
#if (IP_SEC_AES192 != FNO)
	{"Aes192",		5},
#endif	
#if (IP_SEC_AES256 != FNO)
	{"Aes256",		6},
#endif	
	{NULL,			2}
};

			
const LcdParamEnumValue authESP[] = {
	{"None",		0},
	{"Md5",			1},
	{"Sha",			2},
	{NULL,			1}
};	
		
const LcdParamEnumValue authAH[] = {
	{"None",		0},
	{"Md5",			1},
	{"Sha",			2},
	{NULL,			1}
};				/* add by lyu */		
const LcdParamEnumValue authAH_1[] = {
        {"Md5",                 1},
        {"Sha",                 2},
        {NULL,                  1}
};     
//authAH_1,encryptESP_1 is shown for IKE config -- commented by rjcai        
const LcdParamEnumValue encryptESP_1[] = { 
        {"Des",                 1},
        {"3Des",             2},
        {"Aes",             3},
        {"Rc4",             4},
#if (IKE_AES192 != FNO)        
        {"Aes192",             5},
#endif
#if (IKE_AES256 != FNO)        
        {"Aes256",             6},
#endif        
        {NULL,                  2}
};
const LcdParamEnumValue negotMode[] = {
        {"Main",                0},
        {"Aggres",                 1},
        {NULL,                  0}
};              
const LcdParamEnumValue groupType[] = {
        {"Group1",                1},
        {"Group2",                 2},
        {"Group5",                 3},
        {NULL,                  2}
};              
const LcdParamEnumValue authenMethod[] = {
        {"Pre-share",                1},
        {"Rsa-encr",                 2},
        {"Rsa-sig",                 3},
        {NULL,                  1}
};
const LcdParamEnumValue noyesEnum[] = {
    {"yes", 1},
    {"no", 0},
    {NULL, 1}
};

#if (VRRP == FYES)

#include "vrrpd.h"

void cmdVrrp(TermInfo *term)
{
   static CmdList cmdlist[] = {
        		{"config", cmdDispConfig, CMD_LEAFVST},
                        {"enabled", P(slotProf->cardType.etherSlot.ext.vrrp.Enable), ENUM, (Pointer)yesnoEnum},
                        {"interface", P(slotProf->cardType.etherSlot.ext.vrrp.interface), STR, 0},
                        {"vrid", P(slotProf->cardType.etherSlot.ext.vrrp.vrid), INT, 0},
                        {"preempt", P(slotProf->cardType.etherSlot.ext.vrrp.Preempt), ENUM, (Pointer)yesnoEnum},
                        {"priority", P(slotProf->cardType.etherSlot.ext.vrrp.priority), INT, 0},
                        {"interval", P(slotProf->cardType.etherSlot.ext.vrrp.interval), INT, 0},
                        {"vrip", P(slotProf->cardType.etherSlot.ext.vrrp.vrip), IPADDR, 0},
                        {"profile", P(slotProf->cardType.etherSlot.ext.vrrp.Profile), STR, 0},
			{"status", F(showVrrpstate), CMD_ARG0,"display VRRP status"},
//			{"client", cmdIpDhcpClient, CMD_LEAFDAD},
			CMDEND};

    //INITBRANCHES;
    INITBRANCHB;
}
#endif

#define TM_PERIOD(x)    {"type",P(tmrgProf->period[x].window),ENUM,(Pointer)tmWindowEnum,"period type"},\
                        {"begin",P(tmrgProf->period[x].begin),TIME,0,"period start time"},\
                        {"end",P(tmrgProf->period[x].end),TIME,0,"period stop time"}

#if 0
/*add by chenyunhua*/
void cmdTimerangePeriod1(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(0),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerangePeriod2(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(1),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerangePeriod3(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(2),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerangePeriod4(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(3),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerangePeriod5(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(4),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerangePeriod6(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(5),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerangePeriod7(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(6),
                        CMDEND};

   INITBRANCH;

}
void cmdTimerangePeriod8(TermInfo *term)
{
   static CmdList cmdlist[] = {
			TM_PERIOD(7),
                        CMDEND};
   INITBRANCH;

}
void cmdTimerange(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"enabled",P(tmrgProf->active),ENUM, (Pointer)(Pointer)yesnoEnum},
                        {"tmstart", P(tmrgProf->tmstart), DATETIME,0,"time-range start time"},
                        {"tmstop", P(tmrgProf->tmstop), DATETIME,0,"time-range stop time"},
                        {"1stPeriod",cmdTimerangePeriod1,CMD_LEAFDAD},
                        {"2ndPeriod",cmdTimerangePeriod2,CMD_LEAFDAD},
                        {"3rdPeriod",cmdTimerangePeriod3,CMD_LEAFDAD},
                        {"4thPeriod",cmdTimerangePeriod4,CMD_LEAFDAD},
                        {"5thPeriod",cmdTimerangePeriod5,CMD_LEAFDAD},
                        {"6thPeriod",cmdTimerangePeriod6,CMD_LEAFDAD},
                        {"7thPeriod",cmdTimerangePeriod7,CMD_LEAFDAD},
                        {"8thPeriod",cmdTimerangePeriod8,CMD_LEAFDAD},
                        CMDEND};

    INITBRANCHI;
}
#endif

#if (IP_SEC == FYES)

void cmdIPSecConf(TermInfo *term) 	/* add by lyu */
{
	static CmdList cmdlist[] = {
 		{"enabled", P(ipsecConfProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
		{"method",P(ipsecConfProf->method),ENUM, (Pointer)ipsecMethod,"IPSec method: manual or isakmp"},
     		{"mode",P(ipsecConfProf->mode),	ENUM, (Pointer)ipsecMode, "IPSec mode: Tunnel or Transport"},
		{"peer",P(ipsecConfProf->peer),STR, 0,"Peer IP Address or Hostname"},
		{"outfilter",P(ipsecConfProf->outfilter),STR,0,"Filter name for outbound"},
		{"infilter",P(ipsecConfProf->infilter),STR,0,"Filter name for inbound"},
		{"transform-set",P(ipsecConfProf->transformSet),STR,0,"transform-set profile names divided by : "},
		{"secsLifetime", P(ipsecConfProf->lifetime_secs),INT , 0,"Expected lifetime of IPSEC SA (secs)"},
        	{"kbytesLifetime", P(ipsecConfProf->lifetime_kbytes), INT,0, "Expected lifetime of IPSEC SA (kbytes)"},
		{"mtu",P(ipsecConfProf->mtu),INT,0,"MTU in ipsec layer"},
        	{"profile",P(ipsecConfProf->profile),STR,0,"the profile name of interface or connection"},
		{"isakmp-binding",P(ipsecConfProf->isakmprofile),STR,0,"the profile name of isakmp"},
                {"anti-replay",P(ipsecConfProf->anti_replay),ENUM, (Pointer)yesnoEnum,"anti-replay or no"},
		{"nat-keepalive",P(ipsecConfProf->nat_keepalive),INT,0,"interval for keepalive message"},
		{"dpd-heartbeat",P(ipsecConfProf->dpd_heartbeat),INT,0,"interval for dpd message"},
		{"dpd-timeout",P(ipsecConfProf->dpd_timeout),INT,0,"timeout for reply dpd message"},
		{"dpd-active",P(ipsecConfProf->dpd_active),INT,0,"whether IKE will del IPSec SA"},
        	{"keyencrypt",P(ipsecConfProf->encryptKey),STR,	0,"ESP Encrypted Key to manual config"},
        	{"keyespauth",P(ipsecConfProf->espAuthKey),STR,	0,"ESP Auth Key to manual config"},
        	{"espinspi",P(ipsecConfProf->espInSpi), INT,0,"Incoming ESP SPI to manual config"},
        	{"espoutspi",P(ipsecConfProf->espOutSpi),INT,0,"Outcoming ESP SPI to manual config"},
        	{"keyauth",P(ipsecConfProf->authKey),STR,0,"AH Auth Key to manual config"},
        	{"ahinspi",P(ipsecConfProf->ahInSpi),INT,0,"Incoming AH SPI to manual config"},
        	{"ahoutspi",P(ipsecConfProf->ahOutSpi),INT,0,"Outcoming AH SPI to manual config"},
     		{"usable", P(ipsecConfProf->usable), ENUM, (Pointer)yesnoEnum,"yes or no"},
	    CMDEND};
    
    INITBRANCHI;
}
void cmdIPSecTransformSet(TermInfo *term)
{
        static CmdList cmdlist[]={
 	{"enabled", P(ipsecTransformSetProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
        {"encrypt", P(ipsecTransformSetProf->encrypt), ENUM, (Pointer)encryptESP, "ipsec ESP Encrypt algorithms "},
        {"espAuth", P(ipsecTransformSetProf->espAuth), ENUM, (Pointer)authESP,"ipsec ESP hash algorithms " },
	{"auth", P(ipsecTransformSetProf->auth), ENUM, (Pointer)authAH,  "ipsec AH hash algorithms"},
        CMDEND};
        INITBRANCHI;
}
void cmdIpsec(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"config", cmdIPSecConf, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "ipsec general config", MIB_PROF_IPSEC_CONF},
			{"transform-set", cmdIPSecTransformSet, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "ipsec transform-set", MIB_PROF_IPSEC_TRANSFORMSET},
                        CMDEND};

    INITBRANCHB;
}

void cmdIsakmpConf(TermInfo *term)
{
	static CmdList cmdlist[]={
 	{"enabled", P(isakmpConfProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
 	{"mode", P(isakmpConfProf->mode), ENUM, (Pointer)negotMode, "Initiator negotMode"},
 	{"aggreI2encrypt", P(isakmpConfProf->aggre_I2_encrypt), ENUM, (Pointer)noyesEnum, "aggressive I2 packet encrypt yes or no"},
        {"presharekey", P(isakmpConfProf->presharekey), STR, 0,"Pre-shared key to negotiate"},
        {"policy", P(isakmpConfProf->policy), STR, 0,"Policy profile names divided by : "},
 	{"localuser",P(isakmpConfProf->localuser),STR,0,"local user"},
        {"remoteuser",P(isakmpConfProf->remoteuser),STR,0,"remote user"},
        {"lifetime", P(isakmpConfProf->lifetime),INT , 0,"Expected lifetime of IKE SA"},
        CMDEND};
	INITBRANCHI;
}
void cmdIsakmpPolicy(TermInfo *term)
{
	static CmdList cmdlist[]={
 	{"enabled", P(isakmpPolicyProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
 	{"authentication", P(isakmpPolicyProf->authentication), ENUM, (Pointer)authenMethod, "Isakmp authentication "},
 	{"encrypt", P(isakmpPolicyProf->encrypt), ENUM, (Pointer)encryptESP_1, "Encrypt algorithms "},
        {"hash", P(isakmpPolicyProf->hash), ENUM, (Pointer)authAH_1,"Hash algorithms " },
        {"group", P(isakmpPolicyProf->group), ENUM, (Pointer)groupType,"D-H group"},
        CMDEND};
	INITBRANCHI;
}
void cmdIsakmp(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"config", cmdIsakmpConf, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "isakmp general config", MIB_PROF_ISAKMP_CONF},
                        {"policy", cmdIsakmpPolicy, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "isakmp policy", MIB_PROF_ISAKMP_POLICY},
                        CMDEND};

    INITBRANCHB;
}

#endif


# if ( IP_SEC == FYES )
extern void showSaState(int debflag,TermInfo *term);
extern void clearOneSA_1(int debflag,TermInfo *term,char *outfilter);
extern void clearAllSA(TermInfo *term);
extern void showIpsecSp(TermInfo *term);
void cmdCryptoIpsecSp(TermInfo *term)
{
	INITLEAF;

        DOSHOW{
                showIpsecSp(term);
        }
        DOCLEAR {
                _printf(term,"Can't clear sp!\r\n");
        }
}
void cmdCryptoIpsecSa(TermInfo *term)
{
    INITLEAF;

        DOSHOW{
                showSaState(0,term);
        }
        DOCLEAR {
                char buf[20];
                int len;
                len=argGet(term->lastInput,5,buf);
                if(len==0)  {
                        _printf(term,"To clear ipsecsa:\r\n");
                        _printf(term,"if del all,command should be:  clear crypto ipsec sa 'all' \r\n");
                        _printf(term,"if del one,command should be:  clear crypto ipsec sa outfilter \r\n");

                }
                else {
                        if(strcmpi(buf,"all")==0)
                                clearAllSA(term);
                        else
                                clearOneSA_1(0,term,buf);
                }

        }


}
extern void showIpsecStatistics(TermInfo *term);
extern void   clearIpsecStatistics(void);
void cmdCryptoIpsecStatistics(TermInfo *term)
{
        INITLEAF;

        DOSHOW {
                showIpsecStatistics(term);
        }
        DOCLEAR {
                clearIpsecStatistics();
        }
}

void cmdCryptoIpsec(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"sp", cmdCryptoIpsecSp, CMD_LEAFVST|CMD_CLEAR|CMD_LIST_CLEAR},
                        {"sa", cmdCryptoIpsecSa, CMD_LEAFVST|CMD_CLEAR|CMD_LIST_CLEAR},
                        {"statistics", cmdCryptoIpsecStatistics, CMD_LEAFVST|CMD_CLEAR|CMD_LIST_CLEAR},
			{"nat-traverse-port",P(slotProf->cardType.etherSlot.ext.nat_t_port), INT, 0,"ipsec nat-traverse port ", MIB_PROF_SLOT},
                        CMDEND};
    INITBRANCHB;
}

/////////add bu lyu
#if defined(MDF_IKE) && (MDF_IKE == FYES)

extern void cmd_show_connections_status(TermInfo *term);
extern void cmd_delete_every_connection(void);
extern bool cmd_delete_connection_by_name(char *name);
extern void cmd_show_isakmp_sa(TermInfo *term);
extern void cmd_delete_all_states(void);
extern bool cmd_delete_states_by_name(char *name);
extern void cmd_show_events_status(TermInfo *term);

void cmdCryptoIsakmpConnection(TermInfo *term)
{
	//char buf[MAX_PROFILE_NAME_LENGTH];
	//int arglen = 0;
	INITLEAF;

    DOSHOW {
       	cmd_show_connections_status(term);
    }
    DOCLEAR {
    ;/*	arglen = argGet(term->lastInput, 5, buf);
        if (arglen == 0)
        	cmd_delete_every_connection();
     	else
			_printf(term, "connection \"%s\" %s\n\r", buf, cmd_delete_connection_by_name(buf) ? "is deleted" : "isn't exist");*/
    }
}

void cmdCryptoIsakmpSa(TermInfo *term)
{
	char buf[MAX_PROFILE_NAME_LENGTH];
	int arglen = 0;
	INITLEAF;

    DOSHOW {
       	cmd_show_isakmp_sa(term);
    }
    DOCLEAR {
    	arglen = argGet(term->lastInput, 5, buf);
        if (arglen == 0)
        	cmd_delete_all_states();
     	else
			_printf(term, "isakmp sa \"%s\" %s\n\r", buf, cmd_delete_states_by_name(buf) ? "is deleted" : "isn't exist");
    }
}

void cmdCryptoIsakmpEvent(TermInfo *term)
{
	INITLEAF;

    DOSHOW {
       	cmd_show_events_status(term);
    }
}

void cmdCryptoIsakmp(TermInfo *term)
{
    static CmdList cmdlist[] = {
    					{"connections", cmdCryptoIsakmpConnection, CMD_LEAFVST},
                        {"sa", cmdCryptoIsakmpSa, CMD_LEAFVST|CMD_CLEAR|CMD_LIST_CLEAR},
                        {"events", cmdCryptoIsakmpEvent, CMD_LEAFVST},
                        CMDEND};
    INITBRANCHB;
}
#endif
////////add by lyu

void cmdCrypto(TermInfo *term)
{
        static CmdList cmdlist[] = {
                        {"ipsec", cmdCryptoIpsec, CMD_LIST_RW|CMD_LIST_CLEAR, CMDTL, "ipsec information"}, 
#if defined(MDF_IKE) && (MDF_IKE == FYES)
                        {"isakmp", cmdCryptoIsakmp, CMD_LIST_RW|CMD_LIST_CLEAR, CMDTL, "isakmp information"},   
#endif
                        CMDEND
                       	};

        INITBRANCHES;
}

# endif

# if ( FIREWALL == FYES )
void cmdFirewallAddress(TermInfo *term)
{
        static CmdList cmdlist[]={
        {"enabled", P(fwAddressProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
        {"profile",P(fwAddressProf->profile),STR,0,"firewall address interface"},
        {"area",P(fwAddressProf->area),STR,0,"firewall address area,names divided by ;"},
        CMDEND};
        INITBRANCHI;
}
void cmdFirewallService(TermInfo *term)
{
        static CmdList cmdlist[]={
        {"enabled", P(fwServiceProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
        {"service",P(fwServiceProf->service),STR,0,"firewall services ,names divided by ;"},
        CMDEND};
        INITBRANCHI;
}
void cmdFirewallL7filter(TermInfo *term)
{
        static CmdList cmdlist[]={
        {"enabled", P(fwL7filterProf->active), ENUM, (Pointer)yesnoEnum, "enabled or no"},
	{"type",P(fwL7filterProf->type), ENUM, (Pointer)fwL7filterType, "L7 filter type: url or str..."},
	{"1stLine",P(fwL7filterProf->content[0]),STR,0,"L7 filter content information"},
	{"2ndLine",P(fwL7filterProf->content[1]),STR,0,"L7 filter content information"},
	{"3rdLine",P(fwL7filterProf->content[2]),STR,0,"L7 filter content information"},
	{"4thLine",P(fwL7filterProf->content[3]),STR,0,"L7 filter content information"},
	{"5thLine",P(fwL7filterProf->content[4]),STR,0,"L7 filter content information"},
	{"6thLine",P(fwL7filterProf->content[5]),STR,0,"L7 filter content information"},
	{"7thLine",P(fwL7filterProf->content[6]),STR,0,"L7 filter content information"},
	{"8thLine",P(fwL7filterProf->content[7]),STR,0,"L7 filter content information"},

        CMDEND};
        INITBRANCHI;
}

void  cmdFireWall(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"address", cmdFirewallAddress, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "firewall address config", MIB_PROF_FWADDRESS},
                        {"service", cmdFirewallService, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "firewall service config", MIB_PROF_FWSERVICE},
                        {"L7filter", cmdFirewallL7filter, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "firewall L7 filter config", MIB_PROF_FWL7FILTER},

                        CMDEND};

    INITBRANCHB;
}
# endif

# if (PDB == FYES)
void cmdPdbContent(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"1stLine",P(pdbProf->line[0]),STR,0,"pdb content information"},
                        {"2ndLine",P(pdbProf->line[1]),STR,0,"pdb content information"},
                        {"3rdLine",P(pdbProf->line[2]),STR,0,"pdb content information"},
                        {"4thLine",P(pdbProf->line[3]),STR,0,"pdb content information"},
                        {"5thLine",P(pdbProf->line[4]),STR,0,"pdb content information"},
                        {"6thLine",P(pdbProf->line[5]),STR,0,"pdb content information"},
                        {"7thLine",P(pdbProf->line[6]),STR,0,"pdb content information"},
                        {"8thLine",P(pdbProf->line[7]),STR,0,"pdb content information"},
                        {"9thLine",P(pdbProf->line[8]),STR,0,"pdb content information"},
                        {"10thLine",P(pdbProf->line[9]),STR,0,"pdb content information"},
                        {"11thLine",P(pdbProf->line[10]),STR,0,"pdb content information"},
                        {"12thLine",P(pdbProf->line[11]),STR,0,"pdb content information"},
                        {"13thLine",P(pdbProf->line[12]),STR,0,"pdb content information"},
                        {"14thLine",P(pdbProf->line[13]),STR,0,"pdb content information"},
                        {"15thLine",P(pdbProf->line[14]),STR,0,"pdb content information"},
                        {"16thLine",P(pdbProf->line[15]),STR,0,"pdb content information"},
                        {"17thLine",P(pdbProf->line[16]),STR,0,"pdb content information"},
                        {"18thLine",P(pdbProf->line[17]),STR,0,"pdb content information"},
			{"19thLine",P(pdbProf->line[18]),STR,0,"pdb content information"},
                        {"20thLine",P(pdbProf->line[19]),STR,0,"pdb content information"},
                        {"21thLine",P(pdbProf->line[20]),STR,0,"pdb content information"},
                        {"22thLine",P(pdbProf->line[21]),STR,0,"pdb content information"},
                        {"23thLine",P(pdbProf->line[22]),STR,0,"pdb content information"},
                        {"24thLine",P(pdbProf->line[23]),STR,0,"pdb content information"},
                        {"25thLine",P(pdbProf->line[24]),STR,0,"pdb content information"},
                        {"26thLine",P(pdbProf->line[25]),STR,0,"pdb content information"},
                        {"27thLine",P(pdbProf->line[26]),STR,0,"pdb content information"},
                        {"28thLine",P(pdbProf->line[27]),STR,0,"pdb content information"},
                        {"29thLine",P(pdbProf->line[28]),STR,0,"pdb content information"},
                        {"30thLine",P(pdbProf->line[29]),STR,0,"pdb content information"},
                        CMDEND};

    INITBRANCHI;   
}
void cmdPdb(TermInfo *term)
{
    static CmdList cmdlist[] = {
                     {"createtime", P(pdbProf->createtime), DATETIME, 0, "pdb createtime"},  //add by jyshen 060803
			{"enabled",P(pdbProf->active), ENUM, (Pointer)yesnoEnum},
                	{"mode",P(pdbProf->mode),ENUM, (Pointer)pdbMode,"pdb mode: Predefined or Userdefined"},
                	{"type",P(pdbProf->type), ENUM, (Pointer)pdbType, "pdb type: filter or route"},
                	{"category",P(pdbProf->category), ENUM, (Pointer)pdbCategoryEnum, "pdb category: p2p,im,dns,url,other"},
                	{"index",P(pdbProf->index), INT, 0, "pdb index"},
                	{"descript",P(pdbProf->description), STR, 0, ""},
			{"content",cmdPdbContent,CMD_LIST_RW, CMDTL, "pdb information"},
                        CMDEND};

    INITBRANCHI;
}
# endif

#if ( ISP_CFG == FYES )
void cmdIsp(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"enabled",P(ispProf->active),ENUM, (Pointer)yesnoEnum},
                        {"type",P(ispProf->type),ENUM, (Pointer)ispType,"isp type Pppoe or Fix or Dyn"},
			{"connName",P(ispProf->connName),STR,0,"pppoe dial name of pppoe"},
			{"profile",P(ispProf->profile),STR,0,"which phisics port for this isp"},
			{"defRtName",P(ispProf->defRtName),STR,0,"which default route for this isp"},
			{"dtRtName",P(ispProf->detRtName),STR,0,"which detect route for this isp"},
			{"natBindName",P(ispProf->natBindName),STR,0,"which natbinding for this isp"},
			{"carrier",P(ispProf->carrier),STR,0,"isp provider"},
			{"tx",P(ispProf->isptx),INT,0,"isp tx kbps"},
			{"rx",P(ispProf->isprx),INT,0,"isp rx kbps"},
                        CMDEND};

    INITBRANCHI;
}
#endif

#if 0
#if (OBJECT_GROUP == FYES)
void cmdIpGroupContent(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"1stLine",P(ipGroupProf->line[0]),STR,0,"ipGroup content information"},
                        {"2ndLine",P(ipGroupProf->line[1]),STR,0,"ipGroup content information"},
                        {"3rdLine",P(ipGroupProf->line[2]),STR,0,"ipGroup content information"},
                        {"4thLine",P(ipGroupProf->line[3]),STR,0,"ipGroup content information"},
                        {"5thLine",P(ipGroupProf->line[4]),STR,0,"ipGroup content information"},
                        {"6thLine",P(ipGroupProf->line[5]),STR,0,"ipGroup content information"},
                        {"7thLine",P(ipGroupProf->line[6]),STR,0,"ipGroup content information"},
                        {"8thLine",P(ipGroupProf->line[7]),STR,0,"ipGroup content information"},
                        {"9thLine",P(ipGroupProf->line[8]),STR,0,"ipGroup content information"},
                        {"10thLine",P(ipGroupProf->line[9]),STR,0,"ipGroup content information"},
                        CMDEND};

    INITBRANCHI;   
}

void cmdIpGroup(TermInfo *term)
{
    static CmdList cmdlist[] = {
	    		{"profile",P(ipGroupProf->flag),ENUM,(Pointer)ipgroupEnum},
                	{"enabled",P(ipGroupProf->active), ENUM, (Pointer)yesnoEnum},
                //	{"self",P(ipGroupProf->self), ENUM, (Pointer)yesnoEnum},
                	{"content",cmdIpGroupContent,CMD_LIST_RW, CMDTL, "ipGroup information"},
                        CMDEND};
    INITBRANCHI;
}

void cmdPortGroupContent(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"1stLine",P(portGroupProf->line[0]),STR,0,"serviceGroup content information"},
                        {"2ndLine",P(portGroupProf->line[1]),STR,0,"serviceGroup content information"},
                        {"3rdLine",P(portGroupProf->line[2]),STR,0,"serviceGroup content information"},
                        {"4thLine",P(portGroupProf->line[3]),STR,0,"serviceGroup content information"},
                        {"5thLine",P(portGroupProf->line[4]),STR,0,"serviceGroup content information"},
                        {"6thLine",P(portGroupProf->line[5]),STR,0,"serviceGroup content information"},
                        {"7thLine",P(portGroupProf->line[6]),STR,0,"serviceGroup content information"},
                        {"8thLine",P(portGroupProf->line[7]),STR,0,"serviceGroup content information"},
                        {"9thLine",P(portGroupProf->line[8]),STR,0,"serviceGroup content information"},
                        {"10thLine",P(portGroupProf->line[9]),STR,0,"serviceGroup content information"},
                        CMDEND};

    INITBRANCHI;
}

void cmdPortGroup(TermInfo *term)
{
    static CmdList cmdlist[] = {
	    		{"type",P(portGroupProf->flag),ENUM ,(Pointer)servicesEnum},
                        {"enabled",P(portGroupProf->active), ENUM, (Pointer)yesnoEnum},
                   //      {"self",P(portGroupProf->self), ENUM, (Pointer)yesnoEnum},
                        {"content",cmdPortGroupContent,CMD_LIST_RW, CMDTL, "serviceGroup information"},
                        CMDEND};
    INITBRANCHI;
}

#endif
#endif
