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
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#include "typedef.h"
#include "argcmd.h"
#include "stdio.h"
#include "cmd.h"
#define P_SIM(x)  	cmdDefault, P_SIM1(x)
#define P_SIM1(x)	CMD_LEAF|CMD_SIM, P_SIM2(x)
#define P_SIM2(x) 	(int)&x, sizeof(x)


extern TermCmd *root;

extern Boolean saveAll;
extern Boolean nvramInvalid;
extern Boolean cfgWritten;


int dmzPortNum = 2;

#if (DHCP_PLUS == FYES)
extern void cmdIpDhcpSvr(TermInfo *term);
extern void cmdIpDhcpConflict(TermInfo *term);
# endif

extern void cmdIpsec(TermInfo *term); /* add by lyu */
extern void cmdIsakmp(TermInfo *term);

# if ( FIREWALL == FYES )
extern void cmdFireWall(TermInfo *term);  /* add by cyh */
# endif

# if ( PDB == FYES )
extern void cmdPdb(TermInfo *term);  /* add by cyh */
# endif

# if ( ISP_CFG == FYES )
extern void cmdIsp(TermInfo *term);  /* add by cyh */
# endif

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang */
extern void cmdIpGroup(TermInfo *term);
extern void cmdPortGroup(TermInfo *term);
#endif
#endif

extern void cmdIpDhcpServer(TermInfo *term);

extern void cmdIpDdns(TermInfo *term);
extern void cmdIpPool(TermInfo *term);

extern void upnp_cli_cmdEntry(TermInfo *term);		// add by lyu for upnp

Uint32 dmzUpCount = 0;   	/* wanPort down callback */
Uint32 wanDownCount = 0 ; 	/* wanPort callback count */

static int __index;
Boolean ether2nat;
#if 0
Boolean ether2natAdminStatus = NAT_ADMIN_UP;
#endif
IpAddress ether2ipaddress;

Boolean ether3nat;
#if 0
Boolean ether3natAdminStatus = NAT_ADMIN_UP;
#endif
IpAddress ether3ipaddress;

void cmdSystemBase(TermInfo *term);
void cmdSystemHbeat(TermInfo *term);
void cmdIpHbeat(TermInfo *term);
void cmdSystemEventlog(TermInfo *term);
void cmdIpUdp(TermInfo *term);
void cmdIpTcp(TermInfo *term);
void cmdSessHistory(TermInfo *term);
void cmdSessActive(TermInfo *term);

#if 0
SystemVersion *baseProf = 0;
#endif
SystemProfile *sysProf = 0;

InternetProfile *inetProf = 0;
#if ( WEB_SERVER == FYES )
WebProfile *webConfigProf = 0;
AdvertiseProfile    *advertProf = 0;	    /* added by jfgu@090603 */ 
JoinButtonProfile   *joinbuttonProf = 0;    
SecurityUserProfile *secUserProf = 0;
SecurityGroupProfile *secGroupProf = 0;
SecurityUrlProfile *secUrlProf = 0;
#endif
/* add by brwang */
EtherFilterProfile *etherFilterProf = 0;

# if ( FIREWALLS == FYES )
FwallProfile *fwallProf = 0;
# endif

#if (SWITCHFUN == FYES)  /* add by zgd */
SwitchProfile *switchProf = 0;
Port2Profile *port2Prof = 0;
PortMacProfile *portmacProf = 0;
#if (TAGVlAN == FYES)
TagVlanProfile *tagvlanProf = 0;
#endif
# endif

#if(DOT1X == FYES)
Dot1xProf * dot1xProf = 0;
Dot1xUserProf * dot1xUserProf = 0;
#endif

#if (PPPOE_SRV == FYES)	/* added by llliu */
PppoeServerProfile *_pppoeSrvProf = 0;
MacFilterProfile *_macFilterProf = 0;
#endif

#if ( CBQ == FYES )
CbqClassProfile *cbqClassProf = 0;
CbqFilterProfile *cbqFilterProf = 0;
SfqQdiscProfile *sfqQdiscProf = 0;
#endif

#if (DHCP_PLUS == FYES)
RawOptionClassProfile* rawoptClassProf = 0;
DhcpPoolClassProfile* dhcpPoolClassProf = 0;
#endif

# if ( SNMP_TRAP == FYES )
TrapProfile *trapProf = 0;
void cmdSnmpTrap(TermInfo *term);
# endif

# if ( SNMP == FYES )
void cmdSystemSnmpAgent(TermInfo *term);
# endif

#if 0
/*
 * added by llliu@080428.
 */
DnsFilterProfile *dnsFilterProf = 0;
#endif


/*
 * added by llliu@081107.
 */

#if (POLICY_ROUTE == FYES)
PolicyRouteProfile *policyRouteProf = 0;
#endif



#if (FC == FYES)
RateLimitProfile *rateLimitProf = 0;
#endif

#if (PPPOE_SRV == FYES)
PppoeOnlyProfile *pppoeOnlyProf = 0;
#endif


//added by dhyu
#if (NOTICE == FYES)
NoticeProfile *noticeProf = 0;
#endif

#if 0
void cmdRunningConfig(TermInfo *term)
{
    int inst[MAX_LEVEL];
    INITLEAF;

    DOSHOW {
      inst[2] = 2;
      saveAll = FALSE;
      _printf(term, "\r\n%srebuilding running configuration...\r\n", CLI_COMMENT_LINE);
      _printf(term, "\r\n%scurrent running software %s\r\n", CLI_COMMENT_LINE, getVersionStr());
      if(nvramInvalid) _printf(term, "Attention! NVRAM may be corrupt.\r\n");
      _printf(term, "\r\n");
      cmd_ShowCfg(root, inst, term, NULL, 0, DEFAULT_RC);
      _printf(term, "\r\n");
    }

    DOCLEAR {
      addErrorStatusLog( TRUE, ERROR_RUNNING_CLEARED );
      makeFactoryProfiles();
    }


    /*
     * added by llliu@080424 for 'more' process.
     */
    DOMORE {
      inst[2] = 2;
      saveAll = FALSE;
      lcdTermSetMoreFlag(term->lcdTerm, TRUE);
      lcdTermMoreInit(term->lcdTerm, TRUE);


      _printf(term, "\r\n%srebuilding running configuration...\r\n", CLI_COMMENT_LINE);
      _printf(term, "\r\n%scurrent running software %s\r\n", CLI_COMMENT_LINE, getVersionStr());
      if(nvramInvalid) _printf(term, "Attention! NVRAM may be corrupt.\r\n");
      _printf(term, "\r\n");
      cmd_ShowCfg(root, inst, term, NULL, 0, MORE_RC);
      _printf(term, "\r\n");

      lcdTermSetMoreFlag(term->lcdTerm, FALSE);
    }



}
void cmdSimpleConfig(TermInfo *term)
{
    int inst[MAX_LEVEL];
    INITLEAF;

    DOSHOW {
      inst[2] = 2;
      saveAll = FALSE;
      _printf(term, "\r\n%sConcise version of running configuration...\r\n", CLI_COMMENT_LINE);
      _printf(term, "\r\n%scurrent running software %s\r\n", CLI_COMMENT_LINE, getVersionStr());

      if(nvramInvalid) _printf(term, "Attention! NVRAM may be corrupt.\r\n");
      _printf(term, "\r\n");
      cmd_ShowCfg(root, inst, term, NULL, 0, SIMPLE_RC);
      _printf(term, "\r\n");
    }

}


void cmdStoredConfig(TermInfo *term)
{
    char *nvram = NULL;
    char *nvram1 = NULL;

    INITLEAF;

    DOSHOW {
      _printf(term, "\r\n%srebuilding NVRAM configuration...\r\n", CLI_COMMENT_LINE);
      _printf(term, "\r\n%scurrent running software %s\r\n", CLI_COMMENT_LINE, getVersionStr());
      if(nvramInvalid) _printf(term, "Attention! NVRAM may be corrupt.\r\n");
      _printf(term, "\r\n");

      nvram1 = nvram = (char*)getNvramStr();

      if ( NULL == nvram1 ) {
        _printf(term, "Attention! NVRAM wrong format.\r\n");
	return;
      } 

      while(*nvram && *nvram != -1) {
	_printf(term, "%c", *nvram++);
      }
      _printf(term, "\r\n");
      FREE(nvram1);
    }

    DOCLEAR {
        addErrorStatusLog( TRUE, ERROR_NVRAM_CLEARED );
	buildNvram(TRUE);  /* make a nvram clear */
    }
}

void cmdFlashConfig(TermInfo *term)
{

  Uint32 size;
  Uint32 chipsize;
  Uint32 text, flash, used;
  Boolean valid = FALSE;

  INITLEAF;

#if (TWO_FLASH == FYES)
  Uint32 sec_used;

  flashStatus(NULL, NULL, &size);
  /* convert to Mbyte and back to Kbyte */
  chipsize = (((size>>20) + 1)<<10);
  _printf(term, "\r\nflash size %luKB first @0x%p and second @0x%p.\r\n",
	  chipsize,
	  FLASH_ADDR,
	  SECOND_FLASH_START_ADDR
	  );

  valid = imageStatus( NULL, NULL, &used);
  sec_used = getSecImgLength();
  _printf(term,
	  " image size %luKB, first used %luKB(%lu bytes) and second used %luKB(%lu bytes). stored image is %svalid.\r\n",
	  size >> 10,
	  used >> 10,
	  used,
	  sec_used >> 10,
	  sec_used,
	  valid ? "" : "in");

#else

  flashStatus( NULL, NULL, &size);
  /* convert to Mbyte and back to Kbyte */
  chipsize = (((size>>20) + 1)<<10);
  _printf(term, "\r\nflash size %luKB @0x%p.\r\n", chipsize, FLASH_ADDR);
  valid = imageStatus( NULL, NULL, &used);
  _printf(term, " image size %luKB, used %luKB(%lu bytes). stored image is %svalid.\r\n", size >> 10, used >> 10, used, valid ? "" : "in");
#endif

  nvramStatus( &text, &flash, &used);
  _printf(term, " nvram size %luKB, used %lu Bytes.\r\n", flash >> 10, used);
  if (  used <= 1) {
    	_printf(term, "Attention! NVRAM may be corrupt.\r\n");
  } else {
	_printf(term, " configuration%s textual %lu Bytes.\r\n", (text < used) ? "" : "(decompressed)", text);
  }
}

void cmdShowTech(TermInfo *term)
{

  INITLEAF;

  _printf(term, "\r\nsystem revision...\r\n");
  termSrvRevision( term->lcdTerm );
  _printf(term, "\r\nsystem date/time...\r\n");
  termSrvDateTime( term->lcdTerm );
  _printf(term, "\r\nsystem uptime..\r\n");
  termSrvUpTime( term->lcdTerm );

  _printf(term, "\r\nbuilding running configuration...\r\n");
  cmdRunningConfig( term );
  _printf(term, "\r\nbuilding NVRAM configuration...\r\n");
  cmdStoredConfig( term );

  _printf(term, "\r\nsystem event history...\r\n");
  cmdSessHistory(term);

   _printf(term, "\r\nsystem active calls..\r\n");
   termSrvViewCalls( term->lcdTerm );

  _printf(term, "\r\nsystem active users..\r\n");
  cmdSessActive(term);

  _printf(term, "\r\nsystem interface layer statistics...\r\n");
  termSrvViewInterfaceStatistics( term->lcdTerm, (char*)"up" );
  termSrvViewInterfaceStatistics2( term->lcdTerm, (char*)"up" );

  _printf(term, "\r\nsystem ip layer statistics...\r\n");
  cmdIpUdp( term );
  cmdIpTcp( term );
  termSrvViewIcmpStats( term->lcdTerm );
  termSrvViewIpStatistics( term->lcdTerm);
  _printf(term, "\r\nsystem ip interfaces..\r\n");
  termSrvViewIpAddrs( term->lcdTerm, (char*)"up" );
  _printf(term, "\r\nsystem ip route table..\r\n");
  termSrvViewIpRoutes( term->lcdTerm, (char*)nullMacAddr );

  _printf(term, "\r\nsystem ip arp table..\r\n");
  termSrvViewArpCache(term->lcdTerm);

#if (FC == FYES)
#else
  _printf(term, "\r\nsystem ip mac table..\r\n");
  termSrvViewBridgeInfo(term->lcdTerm);
#endif

  _printf(term, "\r\nsystem NAT layer statistics...\r\n");
  termSrvViewNatStatus( term->lcdTerm );
  _printf(term, "\r\nsystem nat statistics...\r\n");
  termSrvViewNatStatistics( term->lcdTerm );

}
#endif

void cmdShowDebug(TermInfo *term)
{
  INITLEAF;

  _printf(term, "\r\nsystem revision...\r\n");
  termSrvRevision( term->lcdTerm );
  termSrvUpTime( term->lcdTerm );
  termSrvDateTime( term->lcdTerm );

  _printf(term, "\r\nbuilding running configuration...\r\n");
  _printf(term, "\r\nsystem ip layer statistics...\r\n");
  termSrvViewIpStatistics( term->lcdTerm);
  _printf(term, "\r\nsystem NAT layer statistics...\r\n");
  termSrvViewNatStatus( term->lcdTerm );
  termSrvViewNatStatistics( term->lcdTerm );
}

void cmdDispConfig(TermInfo *term)
{
    INITLEAF;
    cmd_DispCfg(GETCMD, term);
}

extern int cpuInIdleTask( void );

# define DISP_RATE	189

extern void pstatGetProcCpu( int *DispCpu, int *DispMem);
extern int getSystemCpuLoad (void);

void cmdDispProcCpu(TermInfo *term)
{
  int Cpu = 100, Mem = 100;

  INITLEAF;

  pstatGetProcCpu ( &Cpu, &Mem);

  _printf(term, "\r\ncurrent cpu util %d%%, memory util %d%%, load %d%%\r\n"
		,getSystemCpuLoad()
		,Mem
		,Cpu
	);
}

void cmdDispProcMem(TermInfo *term)
{
  INITLEAF;
  _printf(term, "\r\nmem free %lu\r\n", sysUpTime());
}

#if ( CBQ == FYES )
void cmdSfqQdisc(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(sfqQdiscProf->active), ENUM, (Pointer)yesnoEnum},
			{"dev", P(sfqQdiscProf->comm.dev), STR, 0},
			{"parent", P(sfqQdiscProf->comm.parent), STR, 0},
			{"handle", P(sfqQdiscProf->comm.handle), STR, 0},
			{"quantum", P(sfqQdiscProf->quantum), INT, 0},
			{"perturb", P(sfqQdiscProf->perturb), INT, 0},
//			{"help", P(sfqQdiscProf->help), INT, 0},
			CMDEND};

    INITBRANCHI;
}


void cmdCbqFilter(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(cbqFilterProf->active), ENUM, (Pointer)yesnoEnum},
			{"dev", P(cbqFilterProf->dev), STR, 0},
			{"parent", P(cbqFilterProf->parent), STR, 0},
			{"classid", P(cbqFilterProf->flowid), STR, 0},
//			{"handle", P(cbqFilterProf->handle), STR, 0},
			{"priority", P(cbqFilterProf->priority), INT, 0},
			{"pkttype", P(cbqFilterProf->type2), ENUM, (Pointer)prototypeEnum},
			{"matchtype", P(cbqFilterProf->type1), ENUM, (Pointer)matchtypeEnum},
//			{"smac", P(cbqFilterProf->smac), STR, 0},
//			{"dmac", P(cbqFilterProf->dmac), STR, 0},
			{"sip", P(cbqFilterProf->sip), IPADDR, 0},
			{"sipmask", P(cbqFilterProf->sipmask), IPADDR, 0},
			{"dip", P(cbqFilterProf->dip), IPADDR, 0},
			{"dipmask", P(cbqFilterProf->dipmask), IPADDR, 0},
			{"sport", P(cbqFilterProf->sport), INT, 0},
			{"spmask", P(cbqFilterProf->spmask), INT, 0},
			{"dport", P(cbqFilterProf->dport), INT, 0},
			{"dpmask", P(cbqFilterProf->dpmask), INT, 0},
			{"protocol", P(cbqFilterProf->protocol), STR, 0},
			{"ipprot", P(cbqFilterProf->ip_prot), INT, 0},
			{"tos", P(cbqFilterProf->tos), INT, 0},
//			{"ihl", P(cbqFilterProf->ihl), INT, 0},
//			{"tfrag", P(cbqFilterProf->t_frag), INT, 0},
//			{"icmptype", P(cbqFilterProf->icmp_type), INT, 0},
//			{"icmpcode", P(cbqFilterProf->icmp_code), INT, 0},
			{"sipFrom", P(cbqFilterProf->sipFrom), IPADDR, 0},
			{"sipEnd", P(cbqFilterProf->sipEnd), IPADDR, 0},
			{"dipFrom", P(cbqFilterProf->dipFrom), IPADDR, 0},
			{"dipEnd", P(cbqFilterProf->dipEnd), IPADDR, 0},
			{"sportFrom", P(cbqFilterProf->sportFrom), INT, 0},
			{"sportEnd", P(cbqFilterProf->sportEnd), INT, 0},
			{"dportFrom", P(cbqFilterProf->dportFrom), INT, 0},
			{"dportEnd", P(cbqFilterProf->dportEnd), INT, 0},
			CMDEND};

    INITBRANCHI;
}


void cmdCbqClass(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(cbqClassProf->active), ENUM, (Pointer)yesnoEnum},
			{"dev", P(cbqClassProf->dev), STR, 0},
			{"parent", P(cbqClassProf->parent), STR, 0},
			{"classid", P(cbqClassProf->classid), STR, 0},
			{"rate", P(cbqClassProf->rate), INT, 0},
			{"bandwidth", P(cbqClassProf->bandwidth), INT, 0},
			{"minidle", P(cbqClassProf->minidle), INT, 0},
			{"minburst", P(cbqClassProf->minburst), INT, 0},
			{"maxburst", P(cbqClassProf->maxburst), INT, 0},
			{"bounded", P(cbqClassProf->bounded), ENUM, (Pointer)yesnoEnum},
			{"isolated", P(cbqClassProf->isolated), ENUM, (Pointer)yesnoEnum},
			{"ewma", P(cbqClassProf->ewma), INT, 0},
			{"cell", P(cbqClassProf->cell), INT, 0},
			{"prio", P(cbqClassProf->prio), INT, 0},
			{"allot", P(cbqClassProf->allot), INT, 0},
			{"avpkt", P(cbqClassProf->avpkt), INT, 0},
			{"mpu", P(cbqClassProf->mpu), INT, 0},
			{"weight", P(cbqClassProf->weight), INT, 0},
			{"split", P(cbqClassProf->split), STR, 0},
			{"defmap", P(cbqClassProf->defmap), INT, 0},
			{"tqdisc", P(cbqClassProf->t_qdisc), ENUM, (Pointer)tqdiscEnum},
//			{"help", P(cbqClassProf->help), INT, 0},
			CMDEND};

    INITBRANCHI;
}

void cmdCbq(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"class", cmdCbqClass, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "cbq class", MIB_PROF_CBQ_CLASS},
			{"filter", cmdCbqFilter, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "cbq filter", MIB_PROF_CBQ_FILTER},
			{"sfqqdisc", cmdSfqQdisc, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "queue policy", MIB_PROF_SFQ_QDISC},
			//{"status", F(termSrvViewCbqStat), CMD_ARG0},
			CMDEND};

    INITBRANCHB;
}

#endif /* CBQ == FYES */


# if ( DOT1Q_VLAN == FYES )

# if ( VIRTUAL_DIALER == FYES ) 

# if ( NUM_OF_ETHER_SLOTS < 9 )
  # error "ETHER SLOT COUNT MUST great than 8!!!"
# else
void cmdDot1qVlan(TermInfo *term)
{
  static CmdList cmdlist[] = {
        //{"1vlanName", P(slotProf->cardType.etherSlot.ext.vlanDot1qParams[0].name), STR, 0},
          {"1vlanEnabled", P(slotProf->cardType.etherSlot.ext.vlanDot1qParams[0].enable), ENUM, (Pointer)yesnoEnum},
          {"1vlanDescription", P(slotProf->cardType.etherSlot.ext.vlanDot1qParams[0].description), STR, 0},
          {"1vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qParams[0].vlanDot1qIfGroup), ENUM, (Pointer)&etherIntEnum},
          {"1vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qParams[0].vlanDot1qTagGroup), ENUMINT, (Pointer)&etherVlanTagEnumInt},
# if 0
typedef struct st_dot1q_cfg {
    char                name[MAX_PROFILE_NAME_LENGTH+1];
    Boolean             enable;
    char                description[MAX_PROFILE_NAME_LENGTH+1];
    Uint16              vLanDot1qType;
    Uint16              vLanDot1qEncapsType;
    Uint16              vlanDot1qIfGroup;
    Uint16              vlanDot1qTagGroup;
    Uint16              vlanDot1qMacGroup;
} dot1qParams;
# endif

# if 0
             {"1vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[1]), ENUM, (Pointer)etherIntEnum},
             {"1vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[1]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"2vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[2]), ENUM, (Pointer)etherIntEnum},
             {"2vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[2]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"3vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[3]), ENUM, (Pointer)etherIntEnum},
             {"3vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[3]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"4vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[4]), ENUM, (Pointer)etherIntEnum},
             {"4vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[4]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"5vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[5]), ENUM, (Pointer)etherIntEnum},
             {"5vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[5]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"6vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[6]), ENUM, (Pointer)etherIntEnum},
             {"6vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[6]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"7vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[7]), ENUM, (Pointer)etherIntEnum},
             {"7vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[7]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
             {"8vlanProfile", P(slotProf->cardType.etherSlot.ext.vlanDot1qPortGroup[8]), ENUM, (Pointer)etherIntEnum},
             {"8vlanTag", P(slotProf->cardType.etherSlot.ext.vlanDot1qTagGroup[8]), ENUMINT, (Pointer)&etherVlanTagEnumInt},
# endif
          CMDEND};

    INITBRANCHB;
}      
# endif

# else
# endif
# endif /* DOT1Q_VLAN == FYES */

#if 0
void cmdPort(TermInfo *term)
{
    static CmdList cmdlist[] = {
                        {"1portGrpId", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"2portGrpId", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[1]), INT, 0},
                        {"3portGrpId", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[2]), INT, 0},
                        {"4portGrpId", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[3]), INT, 0},
#if (LANPORT5 == FYES)
                        {"5portGrpId", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[4]), INT, 0},
#endif
                        {"1pMode", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0}, 
				/*auto, 100m-fd, 100m-hd, 10m-fd, 10m-hd, force-auto */
                        {"1pRxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"1pTxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"1pCap", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"1pLpCap", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"1pAd", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"2pMode", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"2pRxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"2pTxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"3pMode", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"3pRxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"3pTxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"4pMode", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"4pRxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
                        {"4pTxFlowCtrl", P(slotProf->cardType.etherSlot.ext.vlanPortGroup[0]), INT, 0},
			//{"status", F(termSrvViewCbqStat), CMD_ARG0},
			//{"statistics", F(termSrvViewCbqStat), CMD_ARG0},
                        CMDEND};

    INITBRANCHB;
}      
#endif

#if(DOT1X == FYES)
#define DOT1X_PORT_STATS_FORMAT  "    %-32s:%-30d\r\n"
#define DOT1X_PORT_CONFIG_FORMAT  "    %-32s:%-30s\r\n"
#define DOT1X_MAC_STR "%02x:%02x:%02x:%02x:%02x:%02x"
#define DOT1X_MAC_STR2(a)   (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
void cmdShowPortDot1xStatistics( TermInfo *term )
{
	Dot1xAuthStatsEntry  auth_stats;
	Dot1xSuppStatsEntry  supp_stats;
	int port_num;
	BYTE* mac; 
    	INITLEAF;
		
    	TermCmd *cmd = (GETCMD->parent)->parent;
	DOSHOW
	{		
		_printf(term,"\r\n");
		switch(cmd->cmdlist->name[0])
		{
		case 'p':
			port_num = term->inst[2];
			dot1xMibReadAuthStats(port_num,&auth_stats);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolFramesRx",auth_stats.authEapolFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolFramesTx",auth_stats.authEapolFramesTx);			
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolStartFramesRx",auth_stats.authEapolStartFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolLogoffFramesRx",auth_stats.authEapolLogoffFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolRespIdFramesRx",auth_stats.authEapolRespIdFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolRespFramesRx",auth_stats.authEapolRespFramesRx);				
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolReqIdFramesTx",auth_stats.authEapolReqIdFramesTx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapolReqFramesTx",auth_stats.authEapolReqFramesTx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthInvalidEapolFramesRx",auth_stats.authInvalidEapolFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthEapLengthErrorFramesRx",auth_stats.authEapLengthErrorFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "AuthLastEapolFrameVersion",auth_stats.authLastEapolFrameVersion);
			mac = auth_stats.authLastEapolFrameSource;
			_printf( term, "    %-32s:"DOT1X_MAC_STR"\r\n","AuthLastEapolFrameSource",DOT1X_MAC_STR2(mac));
			break;
		case 'e':
			port_num = term->inst[2]-2;
			dot1xMibReadSuppStats(port_num + LAN_PORT_NUM,&supp_stats);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolFramesRx",supp_stats.suppEapolFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolFramesTx",supp_stats.suppEapolFramesTx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolStartFramesTx",supp_stats.suppEapolStartFramesTx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolLogoffFramesTx",supp_stats.suppEapolLogoffFramesTx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolRespIdFramesTx",supp_stats.suppEapolRespIdFramesTx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolRespFramesTx",supp_stats.suppEapolRespFramesTx);
	
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolReqIdFramesRx",supp_stats.suppEapolReqIdFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapolReqFramesRx",supp_stats.suppEapolReqFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppInvalidEapolFramesRx",supp_stats.suppInvalidEapolFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppEapLengthErrorFramesRx",supp_stats.suppEapLengthErrorFramesRx);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppLastEapolFrameVersion",supp_stats.suppLastEapolFrameVersion);
			mac = supp_stats.suppLastEapolFrameSource;
			_printf( term, "    %-32s:"DOT1X_MAC_STR"\r\n","SuppLastEapolFrameSource",DOT1X_MAC_STR2(mac));		
			break;
		default:
			break;
		}
	}
	_printf(term,"\r\n");
	
}

const LcdParamEnumValue* dot1xGetEnumStr(const LcdParamEnumValue * enum_value,int value)
{
	while(enum_value->enumName !=NULL && enum_value->enumValue != value)
	{		
		enum_value ++;
	}
	return enum_value;
}

void cmdShowPortDot1xConfig( TermInfo *term )
{
	int port_num;
	const char * str;
	Dot1xPortProf port_config;;
    	INITLEAF;
		
    	TermCmd *cmd = (GETCMD->parent)->parent;
	str = NULL;
	
	DOSHOW
	{
		_printf(term,"\r\n");
		switch(cmd->cmdlist->name[0])
		{
		case 'p':
			port_num = term->inst[2];
			dot1xMibReadPortConfig(port_num,&port_config);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "PortID",port_config.pae_port.dot1xPaePortNumber);
			str = dot1xGetEnumStr(Dot1xPortMethodEnum,port_config.auth_config.ControlMethod)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT   , "PortMethod",str);
			str = dot1xGetEnumStr(Dot1xPortControlEnum,port_config.auth_config.PortControl)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "PortControl",str );
			//str = dot1xGetEnumStr(Dot1xAuthBackendAuthStateEnum,port_config.auth_config.BackendAuthState)->enumName;
			//_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "AuthBackendState",str );
			str = dot1xGetEnumStr(Dot1xPortStatusEnum,port_config.auth_config.AuthPortStatus)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "AuthPortStatus",str);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "MaxHost",port_config.auth_config.MaxHostNum);
			str = dot1xGetEnumStr(enableEnum,port_config.auth_config.ReAuthEnabled)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "ReAuthentication",str);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "ReauthPeriod",port_config.auth_config.ReAuthPeriod);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "QuietPeriod",port_config.auth_config.QuietPeriod);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SuppTimeout",port_config.auth_config.SuppTimeout);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "ServerTimeout",port_config.auth_config.ServerTimeout);
			break;
		case 'e':
			port_num = term->inst[2] -2;
			dot1xMibReadPortConfig(port_num + LAN_PORT_NUM,&port_config);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "PortID",port_config.pae_port.dot1xPaePortNumber);
			str = dot1xGetEnumStr(yesnoEnum,port_config.pae_port.dot1xPaePortSuppCapable)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "SuppCapable",str);
			str = dot1xGetEnumStr(Dot1xSuppPAEStateEnum,port_config.supp_config.SuppPaeState)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "SuppPaeState",str);
			str = dot1xGetEnumStr(Dot1xSuppBackendStateEnum,port_config.supp_config.SuppBackendState)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "SuppBackendState",str);
			str = dot1xGetEnumStr(Dot1xPortStatusEnum,port_config.supp_config.SuppControlledPortsStatus)->enumName;
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "SuppPortStatus",str);
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "UserName",port_config.supp_config.user_name);
			_printf( term, DOT1X_PORT_CONFIG_FORMAT  , "Password",port_config.supp_config.password);
			_printf( term, DOT1X_PORT_STATS_FORMAT  , "SessionTime",port_config.supp_config.session_time);
			break;
		default:
			break;
		}
	}
	_printf(term,"\r\n");

}

#define DOT1X_PORT_SESSION_FORMAT  "%3d  %15s  "DOT1X_MAC_STR"  %4d  %10d  %11s  %12s  %15s\r\n"
void cmdShowPortDot1xSessionStats( TermInfo *term )
{
	int port_num;
	int i;
	pDot1xAuthSessionStats sess;
	const char * method;
	const char * cause;
	const char * status;
    	INITLEAF;
		
	DOSHOW
	{	
		port_num = term->inst[2];
		_printf(term,"\r\n%3s  %15s  %17s  %4s  %10s  %11s  %12s  %15s\r\n",
			"num","username","mac_address","port","sess_time","auth_method","auth_status","terminate_cause");
		sess =  dot1xMibReadAuthSessionStats(port_num);
		i=0;
		while(sess)
		{
			i++;
			method = dot1xGetEnumStr(Dot1xAuthenticMethod,sess->authentic_method)->enumName;
			cause = dot1xGetEnumStr(Dot1xSessionTerminateCause,sess->terminate_cause)->enumName;
			status = dot1xGetEnumStr(Dot1xPortStatusEnum,sess->auth_status)->enumName;
			_printf(term,DOT1X_PORT_SESSION_FORMAT,i,sess->user_name,DOT1X_MAC_STR2(sess->mac_addr),sess->port_id,
				sess->session_time,method,status,cause);
			sess = sess->next;
		}
		_printf(term,"\r\n");		
	}
}

void cmdPortDot1xReauthenticate( TermInfo *term )
{
	int port_num;
    	INITLEAF;

	DOSET
	{
		port_num = term->inst[2];
		dot1xMibReauthSession(port_num,NULL);
	}
}
#define DOT1X_PORT_CMD_FORMAT "    %-22s < %-48s >\r\n"
void cmdDot1xPort(TermInfo *term)
{
	int i=0;
	static CmdList cmdlist[] = {
                        {"auth-capable", P(port2Prof->Dot1xMib.pae_port.dot1xPaePortAuthCapable), ENUM, (Pointer)yesnoEnum,"enable or disable the authenticator function"},
			   {"port-method", P(port2Prof->Dot1xMib.auth_config.ControlMethod), ENUM, (Pointer)Dot1xPortMethodEnum,"set the port method value"},
			   {"port-control", P(port2Prof->Dot1xMib.auth_config.PortControl), ENUM, (Pointer)Dot1xPortControlEnum,"set the port control value"},	
			   {"max-host", P(port2Prof->Dot1xMib.auth_config.MaxHostNum), INT,0,"set the max number of hosts connect to the port"},
			   {"max-req", P(port2Prof->Dot1xMib.auth_config.MaxReq), INT,0,"set the max number of EAP-request in one authentication process"},
			   {"re-authentication", P(port2Prof->Dot1xMib.auth_config.ReAuthEnabled), ENUM, (Pointer)enableEnum,"enable or disable periodicly reauthentication"},
			   {"reauth-period", P(port2Prof->Dot1xMib.auth_config.ReAuthPeriod), INT,0,"set the reauthentication period"},
			   {"quiet-period", P(port2Prof->Dot1xMib.auth_config.QuietPeriod), INT,0,"set quiet seconds following a failed authentication exchange"},
			   {"server-timeout", P(port2Prof->Dot1xMib.auth_config.ServerTimeout), INT, 0,"set the authentication server timeouts "},
			   {"supp-timeout", P(port2Prof->Dot1xMib.auth_config.SuppTimeout), INT,0,"set the supplicant timeouts"},
			   {"reauthenticate",cmdPortDot1xReauthenticate,CMD_W|CMD_LIST_W,CMDTL,"reauthenticate the port"},
			   {"statistics",cmdShowPortDot1xStatistics,CMD_LEAFVST,CMDTL,"show dot1x authenticator statistics"},	
			   {"session-stats", cmdShowPortDot1xSessionStats, CMD_LEAFVST,CMDTL,"show dot1x session statistics"},
			   {"configuration",cmdShowPortDot1xConfig,CMD_LEAFVST,CMDTL,"show dot1x authenticator configuration"},
			   CMDEND};
						
	INITBRANCHES;
	DOSHOW{ 
		_printf(term,"  dot1x\r\n");
		for ( i=0; cmdlist[i].name;  i++ ) 
		{
			_printf(term,DOT1X_PORT_CMD_FORMAT,cmdlist[i].name,cmdlist[i].helpInfo);	
		}
		
	}
}

void cmdDot1xSuppEthernet(TermInfo *term)
{
	int i=0;
	static CmdList cmdlist[] = {
                        {"supp-capable", P(slotProf ->cardType.etherSlot.Dot1xMib.pae_port.dot1xPaePortSuppCapable), ENUM, (Pointer)yesnoEnum,"enable or disable the supplicant function"},
			   {"user-name", P(slotProf ->cardType.etherSlot.Dot1xMib.supp_config.user_name), STR,0,"set the supplicant username"},
			   {"password", P(slotProf ->cardType.etherSlot.Dot1xMib.supp_config.password), PASSWD,0,"set the supplicant password"},
			   {"statistics",cmdShowPortDot1xStatistics,CMD_LEAFVST,CMDTL,"show dot1x supplicant statistics"},		
			   {"configuration",cmdShowPortDot1xConfig,CMD_LEAFVST,CMDTL,"show dot1x supplicant statistics"},	
			   CMDEND};
						
	INITBRANCHES;	
	DOSHOW{ 
		_printf(term,"  dot1x supplicant\r\n");
		for ( i=0; cmdlist[i].name;  i++ ) 
		{
			_printf(term,DOT1X_PORT_CMD_FORMAT,cmdlist[i].name,cmdlist[i].helpInfo);	
		}
		
	}
	i = term->inst[2]-2;
	DODIAL
    	{
	 	dot1xSuppDial(i + LAN_PORT_NUM);
    	}
    	DOHANGUP
    	{
    		dot1xSuppHang(i + LAN_PORT_NUM);
	}
    
}

void cmdDot1xUser(TermInfo *term)
{
   static CmdList cmdlist[] = {
                     {"enabled", P(dot1xUserProf->active), ENUM, (Pointer)yesnoEnum},
                     {"acct_enabled", P(dot1xUserProf->dot1x_acct_enable), ENUM, (Pointer)yesnoEnum},
#if ( ENCRYPT_PASSWD == FYES )
			{"passwd", P(dot1xUserProf->password), PASSWD, 0},
#else
			 {"passwd", P(dot1xUserProf->password), STR, 0},
#endif					
                     {"dot1x_max_user", P(dot1xUserProf->dot1x_max_user), INT, 0},
                     {"dot1x_port", P(dot1xUserProf->dot1x_port), INT, 0},
			{"macAddress", P(dot1xUserProf->macAddr), MACADDR2, 0},
			CMDEND};
    INITBRANCHI;
}

void cmdDot1x(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"system-auth-control", P(dot1xProf->auth_control), ENUM, (Pointer)enableEnum, "enable or disable port access control"},  
                        {"dhcp-launch", P(dot1xProf->dhcp_launch), ENUM, (Pointer)enableEnum, "enable or disable dhcp launch"},
                        {"user", cmdDot1xUser, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "local  dot1x user table", MIB_PROF_DOT1X_USER},
                        CMDEND};

	INITBRANCH;
}

void cmdAAADot1x(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"auth-scheme", P(dot1xProf->auth_scheme), ENUM, (Pointer)Dot1xAuthSchemeEnum,  "802.1x auth scheme configuration"},
			   CMDEND};

	INITBRANCH;

}

void cmdAAA(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"dot1x", cmdAAADot1x, CMD_N_RWCD, CMDTL, "802.1x aaa configuration", MIB_PROF_DOT1X},
			   CMDEND};

	INITBRANCHB;
}

void cmdRadiusServerPrimary(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"ip", P(dot1xProf->primary_radius.host_ip), IPADDR, 0},
			   {"auth-port", P(dot1xProf->primary_radius.auth_port), INT,0},
			   {"timeout", P(dot1xProf->primary_radius.timeout), INT,0},
			   {"retransmit", P(dot1xProf->primary_radius.retransmit), INT,0},
			   {"deadtime", P(dot1xProf->primary_radius.deadtime), INT,0},
			   {"source-ip", P(dot1xProf->primary_radius.source_ip), IPADDR,0},
			   {"key", P(dot1xProf->primary_radius.share_key), STR,0},
			   CMDEND};

	INITBRANCHB;

}

void cmdRadiusServerSecondary(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"ip", P(dot1xProf->secondary_radius.host_ip), IPADDR, 0},
			   {"auth-port", P(dot1xProf->secondary_radius.auth_port), INT,0},
			   {"timeout", P(dot1xProf->secondary_radius.timeout), INT,0},
			   {"retransmit", P(dot1xProf->secondary_radius.retransmit), INT,0},
			   {"deadtime", P(dot1xProf->secondary_radius.deadtime), INT,0},
			   {"source-ip", P(dot1xProf->secondary_radius.source_ip), IPADDR,0},
			   {"key", P(dot1xProf->secondary_radius.share_key), STR,0},
			   CMDEND};

	INITBRANCHB;

}

void cmdRadiusServer(TermInfo *term)
{
static CmdList cmdlist[] = {
                        {"primary", cmdRadiusServerPrimary, CMD_N_RWCD, CMDTL, "primary radius configuration", MIB_PROF_DOT1X},
			   {"secondary", cmdRadiusServerSecondary, CMD_N_RWCD, CMDTL, "secondary radius configuration", MIB_PROF_DOT1X},
			   CMDEND};

	INITBRANCHB;

}

#endif	//#if(DOT1X == FYES)

#if (SWITCHFUN == FYES)   /* add by zgd */

#if (TAGVlAN == FYES)
void cmdTagVlan(TermInfo *term)
{
  static CmdList cmdlist[] = {
                        {"enabled", P(tagvlanProf->active), ENUM, (Pointer)yesnoEnum},
                        {"id", P(tagvlanProf->VlanID), INT, 0},
                        {"interface", P(tagvlanProf->phynum), ENUM, (Pointer)VlanPhyNumEnum},
                        {"type", P(tagvlanProf->vlantype), ENUM, (Pointer)VlanTypeEnum},
                        {"compolicy", P(tagvlanProf->Policy), INT, 0},
                        {"supervlanname", P(tagvlanProf->supervlanname), STR, 0, "only used in sub vlan"},
                        {"ipaddress", P(tagvlanProf->VlanIpAddress), IPADDR, 0},
                        {"ipnetmask", P(tagvlanProf->VlanIpNetmask), IPADDR, 0},
                        {"p1include", P(tagvlanProf->tagvlanin[0]), ENUM, (Pointer)TagVlanInEnum},
                        {"p2include", P(tagvlanProf->tagvlanin[1]), ENUM, (Pointer)TagVlanInEnum},
                        {"p3include", P(tagvlanProf->tagvlanin[2]), ENUM, (Pointer)TagVlanInEnum},
                        {"p4include", P(tagvlanProf->tagvlanin[3]), ENUM, (Pointer)TagVlanInEnum},
                        {"p5include", P(tagvlanProf->tagvlanin[4]), ENUM, (Pointer)TagVlanInEnum},
                        {"p1tagout", P(tagvlanProf->tagvlanout[0]), ENUM, (Pointer)TagVlanOutEnum},
                        {"p2tagout", P(tagvlanProf->tagvlanout[1]), ENUM, (Pointer)TagVlanOutEnum},
                        {"p3tagout", P(tagvlanProf->tagvlanout[2]), ENUM, (Pointer)TagVlanOutEnum},
                        {"p4tagout", P(tagvlanProf->tagvlanout[3]), ENUM, (Pointer)TagVlanOutEnum},
                        {"p5tagout", P(tagvlanProf->tagvlanout[4]), ENUM, (Pointer)TagVlanOutEnum},
                        
                        CMDEND};

    INITBRANCHI;

}
#endif

void cmdPort2(TermInfo *term)
{
  static CmdList cmdlist[] = {
  #if(DOT1X == FYES)
  			   { "dot1x",cmdDot1xPort, CMD_LEAFDAD, CMDTL, "port dot1x configuration",MIB_PROF_PORT2},
  #endif

                        {"mirratt", P(port2Prof->MirrAtt), ENUM, (Pointer)MirrAttEnum},
                        {"mirrdir", P(port2Prof->MirrDir), ENUM, (Pointer)MirrDirEnum},
                        
                        {"portenable", P(port2Prof->PortEnabled), ENUM, (Pointer)enableEnum},
                        {"flowcontrlenable", P(port2Prof->FlowCrlEnabled), ENUM, (Pointer)enableEnum},
                        {"workmode",  P(port2Prof->wireSpeed), ENUM, (Pointer)wirespeedtypeEnum},
                        
                        {"trunknum",  P(port2Prof->TrunkNum), ENUM, (Pointer)TrunkNumEnum},
                        
                        {"inwidthmod",  P(port2Prof->InWidthMod), ENUM, (Pointer)InspeedModEnum, "choose the ingress data type be controled"},
                        {"inbandwidth",  P(port2Prof->ul_InBandWidth), INT, 0},
                        {"outwidthenabled", P(port2Prof->OutWidthEnabled), ENUM, (Pointer)enableEnum},
                        {"outbandwidth",  P(port2Prof->ul_OutBandWidth), INT, 0},
                        
                        #if (TAGVlAN == FYES)
                        {"pvid",  P(port2Prof->PVID), INT, 0},
                        #endif
                        
                        {"safemodeenable", P(port2Prof->SafeModEnabled), ENUM, (Pointer)enableEnum},
                        //{"MacBindEnable", P(port2Prof->PortMacBindEnable), ENUM, (Pointer)enableEnum},
                        {"privalue", P(port2Prof->privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag", P(port2Prof->pritag), INT, 0, "only useful when primode equal to 802.1p"},
                        
                        CMDEND};

    INITBRANCHI;
}

void cmdPortMac(TermInfo *term)
{
  static CmdList cmdlist[] = { 
                        {"enabled", P(portmacProf->active), ENUM, (Pointer)yesnoEnum},
                        {"type", P(portmacProf->type), ENUM, (Pointer)PortMacTypeEnum},
                        {"vid", P(portmacProf->vid), INT, 0},
                        {"filterenable", P(portmacProf->filtenable), ENUM, (Pointer)enableEnum},
                        {"port", P(portmacProf->port), INT, 0},
                        {"macaddress", P(portmacProf->macAddr), MACADDR2, 0},
                        {"p1include", P(portmacProf->portinclude[0]), ENUM, (Pointer)PortMacPortInEnum},
                        {"p2include", P(portmacProf->portinclude[1]), ENUM, (Pointer)PortMacPortInEnum},
                        {"p3include", P(portmacProf->portinclude[2]), ENUM, (Pointer)PortMacPortInEnum},
                        {"p4include", P(portmacProf->portinclude[3]), ENUM, (Pointer)PortMacPortInEnum},
                        {"p5include", P(portmacProf->portinclude[4]), ENUM, (Pointer)PortMacPortInEnum},
                        CMDEND};

    INITBRANCHI;
}

void cmdpritag(TermInfo *term)
{
   static CmdList cmdlist[] = {
                        {"pritag0", P(switchProf->PriTagVlaue[0].pritag), INT, 0},
                        {"privalue0", P(switchProf->PriTagVlaue[0].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag1", P(switchProf->PriTagVlaue[1].pritag), INT, 0},
                        {"privalue1", P(switchProf->PriTagVlaue[1].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag2", P(switchProf->PriTagVlaue[2].pritag), INT, 0},
                        {"privalue2", P(switchProf->PriTagVlaue[2].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag3", P(switchProf->PriTagVlaue[3].pritag), INT, 0},
                        {"privalue3", P(switchProf->PriTagVlaue[3].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag4", P(switchProf->PriTagVlaue[4].pritag), INT, 0},
                        {"privalue4", P(switchProf->PriTagVlaue[4].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag5", P(switchProf->PriTagVlaue[5].pritag), INT, 0},
                        {"privalue5", P(switchProf->PriTagVlaue[5].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag6", P(switchProf->PriTagVlaue[6].pritag), INT, 0},
                        {"privalue6", P(switchProf->PriTagVlaue[6].privalue), ENUM, (Pointer)PriValueEnum},
                        {"pritag7", P(switchProf->PriTagVlaue[7].pritag), INT, 0},
                        {"privalue7", P(switchProf->PriTagVlaue[7].privalue), ENUM, (Pointer)PriValueEnum},
                        CMDEND};

    INITBRANCHB;
}

void cmdSwitch(TermInfo *term)
{
  static CmdList cmdlist[] = {
                        {"mirrorenable", P(switchProf->MirrEnabled), ENUM, (Pointer)enableEnum},
                        {"filterin", P(switchProf->MirrInFilter), ENUM, (Pointer)MirrFilEnum, "choose ingress filter rules in mirror"},
                        {"filterout", P(switchProf->MirrOutFilter), ENUM, (Pointer)MirrFilEnum, "choose egress filter rules in mirror"},
                        {"inmacadr", P(switchProf->uc_IngressMac), MACADDR2, 0, "set ingress mac address"},
                        {"outmacadr", P(switchProf->uc_EgressMac), MACADDR2, 0, "set egress mac address"},
                        {"trunkenabled", P(switchProf->Trunkenabled), ENUM, (Pointer)enableEnum},
                        {"trunkmod",  P(switchProf->TrunkMod), ENUM, (Pointer)TrunkModEnum, "choose trunk mode"},
                        {"port", cmdPort2, CMD_LIST_RW | CMD_I, CMDTL, "Port Information", MIB_PROF_PORT2},
                        {"status",  F(termSrvViewPortIfo), CMD_ARG1},
                        {"statistics", F(termSrvViewPortStatistics), CMD_ARG1},
                        {"crtresult", F(termSrvViewPortCRT), CMD_ARG1, "0:ok 1:open 2:short 3:other err 4:failed"},
                        
                        #if (TAGVlAN == FYES)
                        {"vlancompolicyenable", P(switchProf->VlanComPolicy), ENUM, (Pointer)enableEnum},
                        {"subvlancompolicyenable", P(switchProf->SubVlanComPolicy), ENUM, (Pointer)enableEnum},
                        {"vlanlanmode", P(switchProf->VlanMode[0]), ENUM, (Pointer)VlanModEnum, "choose lan vlan mode"},
                        {"vlanlantype", P(switchProf->VlanComMode[0]), ENUM, (Pointer)VlanComModEnum, "choose lan vlan communicate"},
                        /*{"vlanwan1mode", P(switchProf->VlanMode[1]), ENUM, (Pointer)VlanModEnum, "choose wan1 vlan mode"},
                        {"vlanwan1commode", P(switchProf->VlanComMode[1]), ENUM, (Pointer)VlanComModEnum, "choose wan1 vlan communicate"},
                        {"vlanwan1outtag", P(switchProf->VlanWanTagOut[0]), ENUM, (Pointer)TagVlanOutEnum},
                        {"vlanwan2mode", P(switchProf->VlanMode[2]), ENUM, (Pointer)VlanModEnum, "choose wan2 vlan mode"},
                        {"vlanwan2commode", P(switchProf->VlanComMode[2]), ENUM, (Pointer)VlanComModEnum, "choose wan2 vlan communicate"},
                        {"vlanwan2outtag", P(switchProf->VlanWanTagOut[1]), ENUM, (Pointer)TagVlanOutEnum},
                        {"vlanwan3mode", P(switchProf->VlanMode[3]), ENUM, (Pointer)VlanModEnum, "choose wan3 vlan mode"},
                        {"vlanwan3commode", P(switchProf->VlanComMode[3]), ENUM, (Pointer)VlanComModEnum, "choose wan3 vlan communicate"},
                        {"vlanwan3outtag", P(switchProf->VlanWanTagOut[2]), ENUM, (Pointer)TagVlanOutEnum},
                        {"vlanwan4mode", P(switchProf->VlanMode[4]), ENUM, (Pointer)VlanModEnum, "choose wan4 vlan mode"},
                        {"vlanwan4commode", P(switchProf->VlanComMode[4]), ENUM, (Pointer)VlanComModEnum, "choose wan4 vlan communicate"},
                        {"vlanwan4outtag", P(switchProf->VlanWanTagOut[3]), ENUM, (Pointer)TagVlanOutEnum},
                        */{"vlan", cmdTagVlan, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "tagvlan information", MIB_PROF_TAGVLAN},
                        {"vlanlanstatus", F(termSrvViewTagVlanIfo), CMD_ARG1},
                        #endif
                        
                        {"macmanage", cmdPortMac, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "PortMacbinding Macfilter", MIB_PROF_PORTMAC},
                        
                        {"primode", P(switchProf->PriMode), ENUM, (Pointer)PriModEnum},
                        {"prirule", P(switchProf->PriRule), ENUM, (Pointer)PriRuleEnum},
                        {"pritag", cmdpritag, CMD_LIST_RW},
                        {"macoldtime", P(switchProf->MacOldTim), INT, 0},
                        {"singlemaclist", F(termSrvViewSingleMac), CMD_ARG1},
                        {"multimaclist", F(termSrvViewMultiMac), CMD_ARG1},
                        
                        CMDEND};

    INITBRANCHB;
}
#endif

#if 0
void cmdUsers(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P_SIM(nameProf->active), ENUM, (Pointer)yesnoEnum},
			{"description", P_SIM(nameProf->description), STR, 0},
			{"type", P_SIM(nameProf->userType), ENUM, (Pointer)userTypeEnum},
			{"feature", P_SIM(nameProf->userFeature), ENUM, (Pointer)userTypeEnum},
			{"action", P_SIM(nameProf->userAction), ENUM, (Pointer)userActionEnum},
			{"group", P_SIM(nameProf->userGroup), STR, 0},
			{"profile", P_SIM(nameProf->connProfile), STR, 0},
# if ( ENCRYPT_PASSWD == FYES )
			{"passwd", P_SIM(nameProf->recvPassword), PASSWD, 0},
# else
			{"passwd", P_SIM(nameProf->recvPassword), STR, 0},
# endif
			{"ipAddress", P_SIM(nameProf->ipAddr), IPADDR, 0},
			{"macAddress", P_SIM(nameProf->macAddr), MACADDR2, 0},
#if (DHCP_PLUS == FYES)
			{"remoteId",P_SIM(nameProf->RemoteID),STR,0},
			{"clientId",P_SIM(nameProf->ClientID),STR,0},
			{"dhcpPoolname",P_SIM(nameProf->dhcpPoolname),STR,0},
			{"hostname",P_SIM(nameProf->NbNsName),STR,0},
#endif
#if (SWITCHFUN == FYES) && (TAGVlAN == FYES) /* add by zgd */
			{"vlanportId", P_SIM(nameProf->vlanId), INT, 0},
			{"ifnumId", P_SIM(nameProf->ifnumId), INT, 0},
#endif
# if ( XUANLIN_ALG == FYES )
			{"xlUrl",P_SIM(nameProf->xlUrl),STR,0},
# endif

#if ( VROUTER == FYES )
			{"vRouter", P_SIM(nameProf->vRouterName), STR, 0},
#endif
                        {"srcFrom", P_SIM(nameProf->sourceAddrFrom), IPADDR},
                        {"srcEnd", P_SIM(nameProf->sourceAddrTo), IPADDR},
                        {"destFrom", P_SIM(nameProf->destAddrFrom), IPADDR},
                        {"destEnd", P_SIM(nameProf->destAddrTo), IPADDR},
# if ( IP_RATE == FYES )
# if ( PER_USER_IP_RATE == FYES )
//			{"timerange", P(nameProf->tmrgProfile), STR, 0},
//			{"inFilter", P(nameProf->inFilterProfile), STR, 0},
//			{"outFilter", P(nameProf->outFilterProfile), STR, 0},
//			{"poolId", P(nameProf->ipPoolIndex), INT, 0},
//			{"vlanId", P(nameProf->vlanId), INT, 0},
//			{"portId", P(nameProf->portId), INT, 0},
//			{"switchId", P(nameProf->switchId), INT, 0},
//			{"domainId", P(nameProf->domainId), INT, 0},
			{"rxBPS", P_SIM(nameProf->maxRxRate), ENUM, (Pointer)userSpeedEnum},
			{"rxPPS", P_SIM(nameProf->maxRxPkt), INT, 0},
			{"txBPS", P_SIM(nameProf->maxTxRate), ENUM, (Pointer)userSpeedEnum},
			{"txPPS", P_SIM(nameProf->maxTxPkt), INT, 0},
			{"natSession", P_SIM(nameProf->maxRxSession), INT, 0},
			{"natUdpSession", P_SIM(nameProf->maxRxUdpSession), INT, 0},
			{"natTcpSession", P_SIM(nameProf->maxRxTcpSession), INT, 0},
			{"natIcmpSession", P_SIM(nameProf->maxRxIcmpSession), INT, 0},
			{"txSession", P_SIM(nameProf->maxTxSession), INT, 0},
    		        {"rxQuota", P_SIM(nameProf->maxRxQuota), INT, 0},
    		        {"txQuota", P_SIM(nameProf->maxTxQuota), INT, 0},
    		        {"duration", P_SIM(nameProf->maxDuration), INT, 0},
			{"individuation", P_SIM(nameProf->individuation), ENUM, (Pointer)yesnoEnum},	//add by jyshen 060904
			{"group_indi", P_SIM(nameProf->group_indi), ENUM, (Pointer)yesnoEnum},	//add by jyshen 060904
			{"nick", P_SIM(nameProf->nick), STR, 0},	//add by jyshen 060904
#if (IPP2P == FYES)
#if 0
			{"antiBT", P_SIM(nameProf->antiP2pEnabled), ENUM, (Pointer)yesnoEnum},		//add by jyshen 060904
			{"antiQQ", P_SIM(nameProf->antiQQEnabled), ENUM, (Pointer)yesnoEnum},		//add by jyshen 060904
			{"antiMsn", P_SIM(nameProf->antiMsnEnabled), ENUM, (Pointer)yesnoEnum},	//add by jyshen 060904
#endif
			{"policy1", P_SIM(nameProf->policyName[0]), STR, 0},
			{"policy2", P_SIM(nameProf->policyName[1]), STR, 0},
			{"pdbPolicy", P_SIM(nameProf->pdbPolicy), INT, 0},		//add by jyshen 060904

#endif

			{"noLimitService", P_SIM(nameProf->noLimitGroup), STR, 0},	//add by jschen: for tx support
# endif /* PER_USER_IP_RATE == FYES */
# endif /* IP_RATE == FYES */
//			 {"mpenabled", P(inetProf->mp.mpEnabled), ENUM, (Pointer)yesnoEnum},
//			 {"mppenabled", P(inetProf->mpp.mppEnabled), ENUM, (Pointer)yesnoEnum},
//			 {"bacp", P(inetProf->mp.bacpEnable), ENUM, (Pointer)enableEnum},
//			 {"history", P(inetProf->mpp.secondsOfHistory), INT, 0},
//			 {"targetutil", P(inetProf->mpp.targetUtilization), ENUMINT, (Pointer)&percentEnumInt},
//			 {"splitcode", P(inetProf->ppp.splitCodeDotUserEnabled), ENUM, (Pointer)enableEnum},
//			{"autoQos", P(nameProf->maxRxRate), ENUM, (Pointer)userSpeedEnum},
//			{"history", P(nameProf->maxRxPkt), INT, 0},
//			{"hiUtil", P(nameProf->maxTxRate), ENUM, (Pointer)userSpeedEnum},
//			{"lowUtil", P(nameProf->maxTxRate), ENUM, (Pointer)userSpeedEnum},
//			{"peak", P(nameProf->maxTxPkt), INT, 0},
//			{"min", P(nameProf->maxRxSession), INT, 0},
//			{"max", P(nameProf->maxTxSession), INT, 0},
//			{"burst", P(nameProf->maxTxSession), INT, 0},
//			{"priorty", P(nameProf->maxTxSession), INT, 0},
//			{"adInterval", P(nameProf->maxTxRate), ENUM, (Pointer)userSpeedEnum},
//			{"rmInterval", P(nameProf->maxTxRate), ENUM, (Pointer)userSpeedEnum},
			CMDEND};

//int  p = (int)(&(nameProf->antiP2pEnabled));
//printf("cmdUsers(), antibt address: %d\n", p);


    INITBRANCHI;
}

void cmdSystemAdmin(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(secProf->active), ENUM, (Pointer)yesnoEnum},
			{"operator", P(secProf->editSystem), ENUM, (Pointer)yesnoEnum},
			{"supervisor", P(secProf->editSecurity), ENUM, (Pointer)yesnoEnum},
# if ( ENCRYPT_PASSWD == FYES )
			{"passwd", P(secProf->password), PASSWD, 0},
# else
			{"passwd", P(secProf->password), STR, 0},
#endif
			CMDEND};

    INITBRANCHI;
}
#endif

void cmdSystemDatetime(TermInfo *term)
{
    int len;

    INITLEAF;

    DOSHOW {
	termSrvDateTime(term->lcdTerm);
    }

    DOSET {
	if(setSystemDateTimeByString(argGet(term->lastInput, 4, &len)))
	    if (!noPrompt) {
	      _printf(term, "Invalid Date/Time format, format is: YYYY-MM-DD [hh:mm:ss]\r\n");	
	    } else {
	      _printf(term, BAD_CMD_FMT, BAD_CMD_STR);
            }
    }
}

#if 0
void cmdSystem(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"datetime", cmdSystemDatetime, CMD_LEAF, CMDTL, "real time clock of system"},
			{"timezone", P(slotProf->cardType.etherSlot.tzMode), ENUM, (Pointer)OffGMTEnum, "timezone setting", MIB_PROF_SLOT},
			{"process", cmdDispProcCpu, CMD_LEAFVST},
//			{"memory", cmdDispProcMem, CMD_LEAFVST},
			{"event", F(termSrvViewSyslog), CMD_ARG0},
			{"log", cmdSystemEventlog, CMD_LEAFDAD, CMDTL, "msg to syslog server", MIB_PROF_SLOT},
# if ( SNMP == FYES )
			{"snmp", cmdSystemSnmpAgent, CMD_LEAFDAD, CMDTL, "SNMP Agent", MIB_PROF_SLOT},
# endif
			{"admin", cmdSystemAdmin, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "system administrator", MIB_PROF_SECURITY},
			{"autologout", P(sysProf->autoLogout), ENUM, (Pointer)yesnoEnum},
			{"idlelogout", P(sysProf->idleLogout), ENUMINT, (Pointer)&idleLogoutEnumInt},
			{"tsPage", P(sysProf->tsPage), INT, 0},
			{"tsPrompt", P(slotProf->cardType.etherSlot.tsPrompt), STR, 0, "terminal server prompt setting", MIB_PROF_SLOT},
#if (L3SWITCH == FYES)
			{"l3switch", P(sysProf->l3switchEnabled), ENUM, (Pointer)enableEnum},
#endif

#if (SNIFFER_PORT == FYES)
#if (SWITCHFUN == FYES)  /*add by zgd*/
			{"mirror", P(sysProf->snifferEnabled), ENUM, (Pointer)enableEnum, "no use here,this function used in swtich"},
#else
			{"mirror", P(sysProf->snifferEnabled), ENUM, (Pointer)enableEnum},
#endif
#endif

# if ( ENCRYPT_PASSWD == FYES )
			{"passwdCrypt", P(sysProf->passwordProfile.encryptLevel), ENUM, (Pointer)passwdTypeEnum},
# endif 
# if ( HW_CRYPT == FYES )
			{"hwCrypt", P(sysProf->hardwareCrypt), ENUM, (Pointer)enableEnum},
# endif 
			{"syncRTC", P(sysProf->syncRTC), ENUM, (Pointer)enableEnum},
			{"sanityCheck", P(sysProf->sanityCheck), ENUM, (Pointer)sanityCheckEnum},
			{"scheduleDuration", P(sysProf->reloadInterval), ENUMINT, (Pointer)&sysReloadEnumInt},
			{"scheduleTmrg", P(sysProf->reloadTmrgProfile), STR, 0},
			{"cbtTmrg", P(sysProf->ipMacRateTmrgProfile[0]), STR, 0},
                        /* add by brwang 080520 */
			{"cbtTmrg1", P(sysProf->ipMacRateTmrgProfile[1]), STR, 0},
			{"cbtTmrg2", P(sysProf->ipMacRateTmrgProfile[2]), STR, 0},
                        /* end by brwang */
			{"language", P(sysProf->language), ENUM, (Pointer)languageEnum},
			{"revision", F(termSrvRevision), CMD_ARG0},
//			{"sessid", F(termSrvViewSessionId), CMD_ARG0},
			{"hostName", P(sysProf->name), STR, 0},
			{"contact", P(sysProf->contact), STR, 0},
			{"location", P(sysProf->location), STR, 0},
			{"uptime", F(termSrvUpTime), CMD_ARG0},
			{"dialoutTimeout", P(sysProf->maxDialoutTime), INT, 0},
			{"maxOutgoingCalls", P(sysProf->parallelDialing), INT, 0},
			{"aliveInterval", P(sysProf->logInterval), INT, 0},
#if (WATCHDOG == FYES)
			{"watchdog", P(sysProf->watchdog), INT, 0},
# endif /* WATCHDOG == FYES */
//			{"ipHotel", P(sysProf->ipHotelEnabled), ENUM, (Pointer)enableEnum},
//			{"ipSpoof", P(sysProf->ipArpSpoofEnabled), ENUM, (Pointer)enableEnum},
//			{"ipForward", P(sysProf->ipForwardEnabled), ENUM, (Pointer)enableEnum},
//			{"ipHelper", P(sysProf->ipHelperEnabled), ENUM, (Pointer)enableEnum},
//			{"ipSpf", P(sysProf->ipSpfTreeEnabled), ENUM, (Pointer)enableEnum},
			{"dialerPrefixLen", P(sysProf->numDigsTnkGrp), ENUMINT, (Pointer)&trunkGroupDigsEnumInt},
			{"dialerGroup", P(sysProf->useTrunkGroups), ENUM, (Pointer)enableEnum},
//			{"test", P(sysProf->sessionIdBase), TIME, 0},
	 		{"regionFrom", P(sysProf->segStart), HEX, 0},
	 		{"regionEnd", P(sysProf->segEnd), HEX, 0},
			{"blockSize", P(sysProf->blockSize), INT, 0},
# if ( IPP2P == FYES )
			{"antiArpSpoofing", P(sysProf->antiArpSpoofEnabled), ENUM, (Pointer)yesnoEnum},
			{"antiIpSpoofing", P(sysProf->antiIpSpoofEnabled), ENUM, (Pointer)yesnoEnum},
			{"antiBlaster", P(sysProf->antiBlasterEnabled), ENUM, (Pointer)yesnoEnum},
			{"antiSlammer", P(sysProf->antiSlammerEnabled), ENUM, (Pointer)yesnoEnum},
			{"antiSasser", P(sysProf->antiSasserEnabled), ENUM, (Pointer)yesnoEnum},
			{"pdbTmrg", P(sysProf->pdbPolicyTime), STR, 0},
#if 0
			{"antiBT", P(sysProf->antiP2pEnabled), ENUM, (Pointer)yesnoEnum},
			{"antiQQ", P(sysProf->antiQQEnabled), ENUM, (Pointer)yesnoEnum},
			{"antiMsn", P(sysProf->antiMsnEnabled), ENUM, (Pointer)yesnoEnum},
			{"p2pTmrg", P(sysProf->p2pTime), STR, 0},
			{"QQTmrg", P(sysProf->QQTime), STR, 0},
			{"msnTmrg", P(sysProf->msnTime), STR, 0},
#endif
# endif	//IPP2P
			{"pdbPolicy", P(sysProf->pdbPolicy), INT, 0},
			
			{"changehttphead", P(sysProf->httpGetUnrar), ENUM, (Pointer)yesnoEnum},
#if 0
			{"antiGameUrl", P(sysProf->antiGameUrl), ENUM, (Pointer)yesnoEnum},
			{"antiStockUrl", P(sysProf->antiStockUrl), ENUM, (Pointer)yesnoEnum},
#endif

			{"allowHostFrom", P(sysProf->allowAccessing), IPADDR, NULL},            //add by jyshen 061206
 			{"allowHostEnd", P(sysProf->allowAccessingEnd), IPADDR, NULL},          //add by jyshen 070123
#if (POLICY_ROUTE == FYES)
			{"prActive", P(sysProf->policyRouteActive), ENUM, (Pointer)yesnoEnum},
#endif
#if ((SK2 == FYES) && (SK2_WATCHDOG == FYES))
			{"watchdog", P(sysProf->watchDog), ENUM, (Pointer)yesnoEnum},
			{"watchdogTimer", P(sysProf->watchTimer), INT, 0},
#endif
			CMDEND};

# if ( DOT1Q_VLAN == FYES )
# if ( VIRTUAL_DIALER == FYES ) 
// enable dialerGroup  
# else
// enable dialerGroup if have DMZ port 
    if(!HFR_ENABLED (DEFAULT_FSET, HFR_MULIP)) DISABLE_ITEM("dialerPrefixLen");
    if(!HFR_ENABLED (DEFAULT_FSET, HFR_MULIP)) DISABLE_ITEM("dialerGroup");
# endif
# endif /* DOT1Q_VLAN == FYES */

#if (L3SWITCH == FYES)
    if ( getBoardRev() < BOARD_3000 ) { /* before gv hdw platform */
    	DISABLE_ITEM("l3switch");
    }
# endif 

#if (SNIFFER_PORT == FYES)
    if ( getBoardRev() < BOARD_3000 ) { /* before gv hdw platform */
        DISABLE_ITEM("mirror");
    }
# endif 

    INITBRANCHB;
}
#endif

# if ( VROUTER == FYES )
void cmdVrouter(TermInfo *term)
{
    static CmdList cmdlist[] = {
			CMDEND};

    INITBRANCHI;
}

void cmdIpInt(TermInfo *term)
{
    static CmdList cmdlist[] = {
			CMDEND};

    INITBRANCHI;
}

void cmdIpGlobal(TermInfo *term)
{
    static CmdList cmdlist[] = {
			CMDEND};

    INITBRANCHI;
}

# endif

//add by jxy8@2003-12-26 for WebUserRight
#if ( WEB_SERVER == FYES )
void cmdWebUser(TermInfo *term)
{
    static CmdList cmdlist[] = {
           {"enabled", P(secUserProf->active), ENUM, (Pointer)yesnoEnum},
# if ( ENCRYPT_PASSWD == FYES )
           {"passwd", P(secUserProf->password), PASSWD, 0},
# else
           {"passwd", P(secUserProf->password), STR, 0},
# endif
	   {"group", P(secUserProf->group), STR, 0},
	   {"prot",P(secUserProf->prot), INT, 0},			
	   CMDEND};

    INITBRANCHI;
}

void cmdWebGroup(TermInfo *term)
{
    static CmdList cmdlist[] = {
           {"enabled", P(secGroupProf->active), ENUM, (Pointer)yesnoEnum},
           {"privilege", P(secGroupProf->priv), INT, 0},
   	   {"prot",P(secGroupProf->prot), INT, 0},			
			CMDEND};
    INITBRANCHI;
}

void cmdWebUrl(TermInfo *term)
{
    static CmdList cmdlist[] = {
           {"enabled", P(secUrlProf->active), ENUM, (Pointer)yesnoEnum},
           {"group", P(secUrlProf->group), STR, 0},
  	   {"secure", P(secUrlProf->secure), INT, 0},
			CMDEND};
    INITBRANCHI;
}

/*
 *  added by jfgu@090603
 */
void cmdAdvertise(TermInfo *term)
{
    static CmdList cmdlist[] = {
	    {"enable", P(advertProf->enable), ENUM, (Pointer)yesnoEnum},
            //{"checkIP", P(advertProf->checkIp),IPADDR, 0},
	    {"icmpTimeout", P(advertProf->icmpTimeOut), INT, 0},
	    CMDEND};
    INITBRANCHB;
}

void cmdJoinButton(TermInfo *term)
{
    static CmdList cmdlist[] = {
	    {"url", P(joinbuttonProf->url), STR, 0},
	    {"output", P(joinbuttonProf->output), STR, 0},
	    CMDEND};
    INITBRANCHI;
}

void cmdWeb(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"user", cmdWebUser, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "Web security Users", MIB_PROF_SECURITY_USER},
			{"group", cmdWebGroup, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "Web security Groups", MIB_PROF_SECURITY_GROUP},
			{"url", cmdWebUrl, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "Web security Urls", MIB_PROF_SECURITY_URL},
			{"accessMethod", P(webConfigProf->method), ENUM, (Pointer)webAccessMethEnum},
			{"idleTimeout", P(webConfigProf->IdleTimeout), INT, 0},
			{"maxConnection", P(webConfigProf->MaxWebConns), INT, 0},
			{"refreshTimeout",P(webConfigProf->refreshTimeout), INT, 0},
			{"port",P(webConfigProf->port), ENUMINT, (Pointer)&tcpPortEnumInt},
                 	{"homeUrl",P(webConfigProf->defaultPageIndex), ENUM, (Pointer)webPageUrlsEnum},
			{"colorstyle",P(webConfigProf->colorstyle), ENUM, (Pointer)webPageColorEnum},
			{"internetAccess",P(webConfigProf->InternetAccess), ENUM, (Pointer)yesnoEnum},
			# if (PDB == FYES)
			{"pdbAutoCheck",P(webConfigProf->bIsPdbChk), ENUM, (Pointer)policyEnum},	//add by jyshen 060807
			{"pdbCheckDay",P(webConfigProf->pdbCheckDay), ENUM, (Pointer)dayEnum},	//add by jyshen 060817
			{"pdbCheckTime",P(webConfigProf->pdbCheckTime), ENUM, (Pointer)timeEnum},	//add by jyshen 060817
			#endif
			{"1AccessHost",P(webConfigProf->webAccessHost[0]), IPADDR, 0},
			{"1AccessMask",P(webConfigProf->webAccessMask[0]), IPADDR, 0},
			{"2AccessHost",P(webConfigProf->webAccessHost[1]), IPADDR, 0},
			{"2AccessMask",P(webConfigProf->webAccessMask[1]), IPADDR, 0},
			{"3AccessHost",P(webConfigProf->webAccessHost[2]), IPADDR, 0},
			{"3AccessMask",P(webConfigProf->webAccessMask[2]), IPADDR, 0},
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"advertise",cmdAdvertise, CMD_N_RWCD, CMDTL, "advertise control", MIB_PROF_ADVERTISE},	    //added by jfgu#090603
			{"jointostart", cmdJoinButton, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "join to start button control", MIB_PROF_JOIN_STARTMENU}, 
			CMDEND};
    INITBRANCHB;
}

#endif

#if 0
//add end jxy8
void cmdSessHistory(TermInfo *term)
{
    INITLEAF;

    Word		port = 0;
    Word		entry = 1;
    AbsoluteTime	timeStamp;
    ErrorType		errorType;
    Word		errorInfo;
    Word		lineNumber;
    Word		channelNumber;
    char		phoneNumber[ MAX_ERROR_LOG_MSG_LENGTH + 1 ];
    Boolean		entryValid;
    char		itemText[300];
    char		tmp[500];
    

    DOSHOW{
    	for(entry = 0; entry < NUMBER_OF_ERROR_LOG_ENTRIES; entry ++){
    	entryValid = errorLogGetErrorEntry( entry,
				        port,
    					&timeStamp, &errorType,
				        &errorInfo,
				        &lineNumber, &channelNumber, 
					phoneNumber );
        int                 explainLen;
        explainLen = 100;

	/* The first line contains the menu number and time stamp */

	if( !entryValid || errorType == ERROR_MARK_TRANSACTION) continue; 

        if (lineNumber == (Word)ERROR_NO_LINE_OR_CHANNEL || 
                channelNumber == (Word)ERROR_NO_LINE_OR_CHANNEL) {
            //explainLen += 5;
            //explainLen += 5;
            explainLen += 6;
        }

	//lcdParamCopyTimeValue( tmp, &timeStamp, 8, TRUE );/*edit by brwang for reos2008*/
	//_printf(term, "%s", ctime(&(time_t)timeStamp));
        strcpy(tmp,utt_ctime(&(time_t)timeStamp));	

	/* This line contains the error message, in 20 characters or less... */

	*itemText = '\t';
	
	errorLogTranslateError( errorType, errorInfo, phoneNumber,
				    39, itemText + 1,
				    NULL );
      	strcat(tmp, itemText);


	/* The last line has extra information that varies depending upon
	   the error type. */

	*itemText = ' ';
	errorLogTranslateError( errorType, errorInfo, phoneNumber,
				    explainLen, NULL,
				    itemText + 1 );
      	strcat(tmp, itemText);
	*itemText = 0;
	if (lineNumber != (Word)ERROR_NO_LINE_OR_CHANNEL) {
	    sprintf(itemText, ", on Line %2d", lineNumber);
	}
      	strcat(tmp, itemText);

	*itemText = 0;
	if (channelNumber != (Word)ERROR_NO_LINE_OR_CHANNEL) {
	    sprintf(itemText, ", on Channel %2d", channelNumber);
	}
      	strcat(tmp, itemText);

	_printf(term, "%s\r\n", tmp);	
    }}
 
    DOCLEAR {
	errorLogClear( port );
    }
}
#endif



void cmdSessActive(TermInfo *term)
{
    INITLEAF;

    DOSHOW
    {
	termSrvViewUsers(term->lcdTerm, -1, 0);
    }
}

#if 0
void cmdSess(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"callinfo", F(termSrvViewCalls), CMD_ARG0},
			{"userinfo", cmdSessActive, CMD_LEAFVST},
			{"history", cmdSessHistory, CMD_LEAFVST|CMD_CLEAR|CMD_LIST_CLEAR},
//			{"ipsec", F(termSrvViewIpsecCalls), CMD_ARG0},
//			{"ike", F(termSrvViewIkeCalls), CMD_ARG0},
//			{"l2tp", F(termSrvViewL2tpCalls), CMD_ARG0},
//			{"pptp", F(termSrvViewPptpCalls), CMD_ARG0},
//			{"pppoe", F(termSrvViewPPPoECalls), CMD_ARG0},
//			{"ipsecsa",F(termSrvViewIpsecsaCalls),CMD_ARG1},
//			{"log", F(termSrvViewSyslog), CMD_ARG0},
			CMDEND};
    INITBRANCHB;
}

void cmdIntBriHistory(TermInfo *term)
{
    INITLEAF;

    DOSHOW
    {
    	GlobDslID       globDsl;

#  if ( TOTAL_DSL_PER_SYSTEM == 1 )
                    /*
                     * Show the isdn history for DSL ID 0, independent of
                     * what is on the command line, since there is only
                     * one item on the command line.
                     */
        globDsl.id = 0;
        termSrvViewIsdnHistory( term->lcdTerm, globDsl );
#  else
        int     num = atoi( args );

        if(( num > 0 ) && ( num <= TOTAL_DSL_PER_SYSTEM )) {
            globDsl.id = num - 1;
            termSrvViewIsdnHistory( term->lcdTerm, globDsl );
        } else {
            termSrvLocalOutput( term, "Invalid line number. \r\n" );
        }
#  endif
    }
}

extern Boolean briActive();
extern void etherStatusDump(int, TermInfo *term);
extern Uint32 briResyncCount();

void cmdIntStatus(TermInfo *term)
{
    Byte status = 0;
    int instance;

    INITLEAF;

    TermCmd *cmd = GETCMD->parent;

    DOSHOW{
    instance = 1;
    switch(cmd->cmdlist->name[0])
    {
	case 'b':
	    status = briActive();
	    break;
	case 'e':
	    instance = term->inst[2] - 1;
	    etherStatusDump( instance, term );
	    return;
	    break;
# if ( V35 == FYES )
	case 's':
	    if( getDaughterBoardType() == DAUGHTER_BOARD_TYPE_NONE )
	    {
		_printf(term, "%s\r\n", "Uninstalled");
		return;
	    } 
	    status = ndbActive();
	    break;
# endif
	default:
	    break;
    }
    _printf(term, "Interface %s/%d\r\nStatus : ",cmd->cmdlist->name, instance);
    _printf(term, "%s\r\n", status ? "Up" : "Down"); 
    
    switch(cmd->cmdlist->name[0])
    {
# if ( V35 == FYES )
	case 's':
    	status = v35ReadStatus();
    	_printf(term, "\r\nDCD:%s   DSR:%s   CTS:%s\r\n\r\n",  
		status & V35_DCD_IN ? "Up" : "Down",
		status & V35_DSR_IN ? "Up" : "Down",
		status & V35_CTS_IN ? "Up" : "Down"); 
	break;
# endif
	case 'b':
	_printf(term, "\r\nResynchronize Counter : %d\r\n\r\n", 
		briResyncCount());	
	break;
	default:
	break;
    }
    }
}


void cmdIntBri(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"type", P(briProf->switchType), ENUM, (Pointer)_isdnSwitchTypeValues},
			{"b1phone", P(briProf->lineInterface[0].channelConfig[0].phoneNumber), STRNUM, 0},
			{"b2phone", P(briProf->lineInterface[0].channelConfig[1].phoneNumber), STRNUM, 0},
			{"b1channel", P(briProf->lineInterface[0].channelConfig[0].channelUsage), ENUM, (Pointer)channelUsageEnum},
			{"b2channel", P(briProf->lineInterface[0].channelConfig[1].channelUsage), ENUM, (Pointer)channelUsageEnum},
			{"b1DialerGroup", P(briProf->lineInterface[0].channelConfig[0].trunkGroup), INT, 0},
			{"b2DialerGroup", P(briProf->lineInterface[0].channelConfig[1].trunkGroup), INT, 0},
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"history", cmdIntBriHistory, CMD_LEAFVST},
			{"status", cmdIntStatus, CMD_LEAFVST}, 
			CMDEND};



    INITBRANCHIBI;

}
#endif

#if ( CBQ == FYES )

void cmdIntEtherCbq(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(slotProf->cardType.etherSlot.ext.cbq.active), ENUM, (Pointer)yesnoEnum},
			{"dev", P(slotProf->cardType.etherSlot.ext.cbq.comm.dev), STR, 0},
			{"parent", P(slotProf->cardType.etherSlot.ext.cbq.comm.parent), STR, 0},
			{"handle", P(slotProf->cardType.etherSlot.ext.cbq.comm.handle), STR, 0},
			{"bandwidth", P(slotProf->cardType.etherSlot.ext.cbq.bandwidth), INT, 0},
			{"avpkt", P(slotProf->cardType.etherSlot.ext.cbq.avpkt), INT, 0},
			{"ewma", P(slotProf->cardType.etherSlot.ext.cbq.ewma), INT, 0},
			{"cell", P(slotProf->cardType.etherSlot.ext.cbq.cell), INT, 0},
			{"mpu", P(slotProf->cardType.etherSlot.ext.cbq.mpu), INT, 0},
			{"allot", P(slotProf->cardType.etherSlot.ext.cbq.allot), INT, 0},
//			{"help", P(slotProf->cardType.etherSlot.ext.cbq.help), INT, 0},
			CMDEND};

    INITBRANCH;

}

#endif /* CBQ == FYES */

#if 0
/* 
 * add by brwang 
  */
void cmdMulticast(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(slotProf->cardType.etherSlot.filterSwitch.multicastFilterEnabled), ENUM, (Pointer)yesnoEnum},
			{"otherforward", P(slotProf->cardType.etherSlot.filterSwitch.multicastFilterOtherForward), ENUM, (Pointer)yesnoEnum},
			{"igmpDeny", P(slotProf->cardType.etherSlot.filterSwitch.igmpDenyEnabled), ENUM, (Pointer)yesnoEnum},
		CMDEND};
    INITBRANCH;

}


/*
 * added by llliu@080428.
 */
void cmdDnsFilterGlobal(TermInfo *term)
{
    static CmdList cmdlist[] = {
        {"enabled", P(slotProf->cardType.etherSlot.filterSwitch.dnsFilter.enabled), ENUM, (Pointer)yesnoEnum},
        {"filterPolicy", P(slotProf->cardType.etherSlot.filterSwitch.dnsFilter.fp), ENUM, (Pointer)filterPolicyEnum},

        CMDEND
    };

        INITBRANCH;
}

/*
 * add for using the function of exceptive qq 
 */
void cmdQQFilterGlobal(TermInfo *term)
{
    static CmdList cmdlist[] = {
	{"enabled", P(slotProf->cardType.etherSlot.filterSwitch.qqEnabled), ENUM, (Pointer)yesnoEnum},
	CMDEND
    };
    INITBRANCH;
}

void cmdFilterSwitch(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"multicast",cmdMulticast,CMD_N_RW, CMDTL, "ipGroup information"},
//			{"multicast", P(slotProf->cardType.etherSlot.filterSwitch.multicastFilter), ENUM, (Pointer)enableEnum},
                        {"dns", cmdDnsFilterGlobal, CMD_N_RW, CMDTL, "Dns Filter Global conf"},
			{"qq", cmdQQFilterGlobal, CMD_N_RW, CMDTL, "exceptive qq global config"},
			CMDEND};
    INITBRANCH;
}
#endif

#if (HOTEL == FYES)
void cmdPNP(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enable",P(slotProf->cardType.etherSlot.ext.hotelSwitch.enable), ENUM, (Pointer)yesnoEnum},
			CMDEND};
    INITBRANCHI;
}
#endif


#if 0
void cmdIntEtherIp(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"address", P(slotProf->cardType.etherSlot.ipAddress), IPADDR, 0},
			{"netmask", P(slotProf->cardType.etherSlot.netmask), IPADDR, 0},
# if ( MULTI_IP_INTERF == FNO )
			{"address2", P(slotProf->cardType.etherSlot.ipAddress2), IPADDR, 0},
			{"netmask2", P(slotProf->cardType.etherSlot.netmask2), IPADDR, 0},
# endif
			{"mtu", P(slotProf->cardType.etherSlot.mtu), ENUMINT, (Pointer)&etherMtuEnumInt},
			{"arpProxy", P(slotProf->cardType.etherSlot.proxyMode), ENUM, (Pointer)proxyEnum},
			{"routing", P(slotProf->cardType.etherSlot.ripMode), ENUM, (Pointer)routingModeEnum},
			{"filter", P(slotProf->cardType.etherSlot.filter), ENUM, (Pointer)pdbGroupEnum},
			{"policy", P(slotProf->cardType.etherSlot.filter), ENUM, (Pointer)pdbGroupEnum},
			{"inACLgroup", P(slotProf->cardType.etherSlot.ext.inFilterGroup), STR, 0},
			{"outACLgroup", P(slotProf->cardType.etherSlot.ext.outFilterGroup), STR, 0},
//			{"gateway", P(slotProf->cardType.etherSlot.ext.clientDefNextHop), IPADDR, 0},
			{"arpCache", P(slotProf->cardType.etherSlot.ext.arpCacheMode), ENUM, (Pointer)arpCacheModeEnum},
			{"arpKeepalive", P(slotProf->cardType.etherSlot.ext.arpKeepalive), INT, 0},
			{"srcIpCheck", P(slotProf->cardType.etherSlot.ext.checkIpSrc), ENUM, (Pointer)yesnoEnum},
			{"dstIpCheck", P(slotProf->cardType.etherSlot.ext.checkIpDst), ENUM, (Pointer)yesnoEnum},
//			{"rarp", P(slotProf->cardType.etherSlot.rarpEnabled), ENUM, (Pointer)enableEnum},
//			{"bootpClient", P(slotProf->cardType.etherSlot.bootpEnabled), ENUM, (Pointer)enableEnum},
//			{"nat", P(slotProf->cardType.etherSlot.ext.natWan), ENUM, (Pointer)enableEnum},
//			{"hbeatkeepalive", P(slotProf->cardType.etherSlot.ext.hbeatMode), INT, 0},
#if(DHCP_PLUS == FYES)
			{"dhcpclient", P(slotProf->cardType.etherSlot.ext.dhcpClientOn), ENUM, (Pointer)enableEnum},
			{"dhcpclientpnp", P(slotProf->cardType.etherSlot.ext.dhcpClientPnpOn), ENUM, (Pointer)enableEnum},	
			{"dhcprequest", P(slotProf->cardType.etherSlot.ext.dhcprequest), ENUM, (Pointer)dhcpCastEnum},
			{"dhcpflags", P(slotProf->cardType.etherSlot.ext.dhcpflags), ENUM, (Pointer)dhcpCastEnum},
			{"clientid", P(slotProf->cardType.etherSlot.ext.identifier), STR, 0},
			{"autoipCoexist", P(slotProf->cardType.etherSlot.ext.KeepAutoIP), ENUM, (Pointer)yesnoEnum},
# else
			{"dhcpclient", P(slotProf->cardType.etherSlot.ext.dhcpClientEnabled), ENUM, (Pointer)enableEnum},
			{"dhcpclientpnp", P(slotProf->cardType.etherSlot.ext.dhcpClientPnpEnabled), ENUM, (Pointer)enableEnum},
#endif //(DHCP_PLUS == FYES)

#if (HOTEL == FYES)
			{"pnp", cmdPNP, CMD_N_RWCD|CMD_LIST_CLEAR|CMD_LIST_DEBUG|CMD_LIST_MORE, CMDTL, "hotel config", MIB_PROF_SLOT},
#endif
			CMDEND};

    INITBRANCH;

}
#endif

#if (DHCP_PLUS == FYES)
void cmdIntEtherRelay(TermInfo *term)
{
	 static CmdList cmdlist[] = {
                        {"enable", P(slotProf->cardType.etherSlot.ext.dhcpRelayOn), ENUM, (Pointer)yesnoEnum},
                        {"priServer", P(slotProf->cardType.etherSlot.ext.dhcpRelayPriSvr), IPADDR, 0},
                        {"secServer", P(slotProf->cardType.etherSlot.ext.dhcpRelaySecSvr),IPADDR, 0},
                        {"thiServer", P(slotProf->cardType.etherSlot.ext.dhcpRelayThrSvr), IPADDR, 0},
                        {"option", P(slotProf->cardType.etherSlot.ext.dhcpRelayOption),ENUM, (Pointer)RelayOptEnum},
                        {"policy", P(slotProf->cardType.etherSlot.ext.dhcpRelayPolicy),ENUM, (Pointer)RelayPolicyEnum},
                        {"packetLength", P(slotProf->cardType.etherSlot.ext.dhcpRelayLen), ENUMINT, (Pointer)&etherMtuEnumInt},
                        {"agentCircuitID", P(slotProf->cardType.etherSlot.ext.dhcpRelayCirId), STR, 0},
                        {"flags", P(slotProf->cardType.etherSlot.ext.dhcpRelayFlags),ENUM, (Pointer)OVERLOAD_FLAGSEnum},
			CMDEND};

    INITBRANCH;
}
#endif

#if 0
void cmdIntEther(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"description", P(slotProf->cardType.etherSlot.ext.description), STR, 0},
#if ( VROUTER == FYES )
			{"vRouter", P(slotProf->cardType.etherSlot.vRouterName), STR, 0},
#endif
			{"dialerGroup", P(slotProf->cardType.etherSlot.ext.trunkGroup), INT, 0},
			{"rxBandwidth", P(slotProf->cardType.etherSlot.ext.rxBandwidth), INT, 0},
			{"txBandwidth", P(slotProf->cardType.etherSlot.ext.txBandwidth), INT, 0},
			{"rxBPS", P(slotProf->cardType.etherSlot.ext.ingressBPS[0]), ENUM, (Pointer)userSpeedEnum},
			{"rxPPS", P(slotProf->cardType.etherSlot.ext.ingressPPS), INT, 0},
			{"txBPS", P(slotProf->cardType.etherSlot.ext.egressBPS[0]), ENUM, (Pointer)userSpeedEnum},
			{"txPPS", P(slotProf->cardType.etherSlot.ext.egressPPS), INT, 0},
                        /* add by brwang for more tmrg 080520*/
			{"rxBPS1", P(slotProf->cardType.etherSlot.ext.ingressBPS[1]), ENUM, (Pointer)userSpeedEnum},
			{"txBPS1", P(slotProf->cardType.etherSlot.ext.egressBPS[1]), ENUM, (Pointer)userSpeedEnum},
			{"rxBPS2", P(slotProf->cardType.etherSlot.ext.ingressBPS[2]), ENUM, (Pointer)userSpeedEnum},
			{"txBPS2", P(slotProf->cardType.etherSlot.ext.egressBPS[2]), ENUM, (Pointer)userSpeedEnum},
                        /* end by brwang */
//			{"bridging", P(slotProf->cardType.etherSlot.bridgingEnabled), ENUM, (Pointer)enableEnum},
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"ip", cmdIntEtherIp, CMD_LEAFDAD, CMDTL},
#if (DHCP_PLUS == FYES)
			{"dhcpRelay",cmdIntEtherRelay,CMD_LEAFDAD, CMDTL},
#endif
			{"mac", P(slotProf->cardType.etherSlot.ext.macAddress), MACADDR, 0},
# if ( DOT1Q_VLAN == FYES )
# if ( VIRTUAL_DIALER == FYES ) 
// disable config of 802.1q vlan DOT1Q  
# else
			{"group", P(slotProf->cardType.etherSlot.ext.vLanGroup), ENUMINT, (Pointer)&etherVlanGroupEnumInt},
			{"tagID", P(slotProf->cardType.etherSlot.ext.vLanType), HEX, 0},
			{"pVID", P(slotProf->cardType.etherSlot.ext.vLanTag), ENUMINT, (Pointer)&etherVlanTagEnumInt},
# endif 
# endif /* DOT1Q_VLAN == FYES */
			{"mode", P(slotProf->cardType.etherSlot.ext.wireSpeed), ENUM, (Pointer)wirespeedtypeEnum},
			{"rxFlowCtrl", P(slotProf->cardType.etherSlot.ext.rxFlowCtrl), ENUM, (Pointer)enableEnum},
			{"txFlowCtrl", P(slotProf->cardType.etherSlot.ext.txFlowCtrl), ENUM, (Pointer)enableEnum},
			{"status", cmdIntStatus, CMD_LEAFVST}, 
//			{"dhcpserver", cmdIpDhcpServer, CMD_LEAFDAD, CMDTL},
//			{"ddns", cmdIpDdns, CMD_N_RW, CMDTL},
//			{"pool", cmdIpPool, CMD_LEAFDAD, CMDTL},

#if ( CBQ == FYES )
			{"cbq", cmdIntEtherCbq, CMD_LEAFDAD, CMDTL},
#endif /* CBQ == FYES */
#if (DOT1X == FYES )
			{"dot1x-supp",cmdDot1xSuppEthernet,CMD_LIST_HANGUP|CMD_DIAL|CMD_HANGUP|CMD_LIST_DIAL|CMD_LEAFDAD, CMDTL,"ether port dot1x supplicant",MIB_PROF_SLOT},
#endif
                        {"filterSwitch",cmdFilterSwitch,CMD_LEAFDAD, CMDTL},
			CMDEND};

#if ( CBQ == FYES )
    if(!HFR_ENABLED (DEFAULT_FSET, HFR_CBQ)) DISABLE_ITEM("cbq");
#endif /* CBQ == FYES */

    if ( getBoardRev() < BOARD_3000 ) { /* before gv hdw platform */
        DISABLE_ITEM("rxBPS");
        DISABLE_ITEM("txBPS");
        DISABLE_ITEM("rxPPS");
        DISABLE_ITEM("txPPS");
    }

    INITBRANCHIBI;
}

void cmdIntSerial(TermInfo *term)
{
    static CmdList cmdlist[] = {
#if ( V35 == FYES )
			{"group", P(slotProf->cardType.v35Slot.nailedUpGroup), GROUP, 0},
			{"activation", P(slotProf->cardType.v35Slot.activation), ENUM, (Pointer)_activationModeEnum},
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"status", cmdIntStatus, CMD_LEAFVST}, 
#endif
			CMDEND};


    INITBRANCHIBI;

}

void cmdInt(TermInfo *term)
{
    static CmdList cmdlist[] = {
#if(DOT1X == FYES)
			{"ethernet", cmdIntEther, CMD_LIST_HANGUP|CMD_LIST_DIAL|CMD_I|CMD_LIST_RW, CMDTL, "ethernet interface", MIB_PROF_SLOT},
#else
			{"ethernet", cmdIntEther, CMD_I|CMD_LIST_RW, CMDTL, "ethernet interface", MIB_PROF_SLOT},
#endif
# if !defined( __arm__ )
			{"bri", cmdIntBri, CMD_I|CMD_LIST_RW, CMDTL, "isdn bri/s interface", MIB_PROF_NET_BRI_TE},
# endif
#if ( V35 == FYES )
			{"serial", cmdIntSerial, CMD_I|CMD_LIST_RW, CMDTL, "serial wan interface", MIB_PROF_SLOT},
#endif
			{"status", F(termSrvViewInterfaceStatistics), CMD_ARG1},
			{"statistics", F(termSrvViewInterfaceStatistics2), CMD_ARG1},
//			{"history", F(termSrvViewInterfaceHistory), CMD_ARG0},

			{"traffic", F(termSrvViewInterfaceTraffic), CMD_ARG1},
			
			CMDEND};
			
    INITBRANCHES;   
}


void cmdIpVpn(TermInfo *term)
{
    static CmdList cmdlist[] = {
#if L2TP==FYES
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"tunnelMode", P(slotProf->cardType.etherSlot.ext.l2TunnelType[0]), ENUM, (Pointer)l2TunnelTypeEnum},
			{"l2tpMode", P(slotProf->cardType.etherSlot.ext.l2tpMode), ENUM, (Pointer)l2tpModeEnum},
			{"l2tpRxWindow", P(slotProf->cardType.etherSlot.ext.l2tpRxWindow), INT, 0},
			{"l2tpAuth", P(slotProf->cardType.etherSlot.ext.l2tpAuthEnabled), ENUM, (Pointer)yesnoEnum},
			{"l2tpTransMode", P(slotProf->cardType.etherSlot.ext.l2tpTransMode), ENUM, (Pointer)l2tpTransportModeEnum},
			{"l2tpPort", P(slotProf->cardType.etherSlot.ext.l2tpSrvPort), INT, 0},
			{"l2tpQlen", P(slotProf->cardType.etherSlot.ext.l2tpQueueDepth), INT, 0},
			{"pptpmode", P(slotProf->cardType.etherSlot.pptpMode), ENUM, (Pointer)pptpModeEnum},
			{"pptpPort", P(slotProf->cardType.etherSlot.ext.pptpSrvPort), INT, 0},
			{"tunnelmtu", P(slotProf->cardType.etherSlot.ext.tunnelmtu), ENUMINT, (Pointer)&tunnelMtuEnumInt},
			{"tunnelPmtud", P(slotProf->cardType.etherSlot.ext.tunnelPMTUD), INT, 0},
			{"tunnelTcpMssAdj", P(slotProf->cardType.etherSlot.ext.tunnelAdjustTcpMSS), INT, 0},
			{"tunnelDFbit", P(slotProf->cardType.etherSlot.ext.tunnelSetDFbit), INT, 0},
			{"tunnelTearDown", P(slotProf->cardType.etherSlot.ext.tunnelTearDown), INT, 0},
#endif
			CMDEND};

    INITBRANCHB;
}


void cmdIpUdp(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"status", F(termSrvViewUdpListenTable), CMD_ARG0},
			{"statistics", F(termSrvViewUdpStats), CMD_ARG0},
			CMDEND};

    INITBRANCHES;
}

void cmdIpTcp(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"status", F(termSrvViewTcpConnectionTable), CMD_ARG0},
			{"statistics", F(termSrvViewTcpStats), CMD_ARG0},
			CMDEND};

    INITBRANCHES;
}
#endif

# if ( ( MROUTING == FYES ) || ( MTROUTING == FYES ) )
void cmdIpIgmp(TermInfo *term)
{
    static CmdList cmdlist[] = {
		       {"config", cmdDispConfig, CMD_LEAFVST},
                       {"memberTimeout", P(slotProf->cardType.etherSlot.multicastMemberTimeout), INT, 0},
                       {"clientEnabled", P(slotProf->cardType.etherSlot.multicastAllowed), ENUM, (Pointer)yesnoEnum},
                       {"mcatRateLimit", P(slotProf->cardType.etherSlot.multicastRateLimit), INT, 0},
                       {"grpLeaveDelay", P(slotProf->cardType.etherSlot.ext.multicastGroupLeaveDelay), INT, 0},
                       {"hbeatAddr", P(slotProf->cardType.etherSlot.multicastHeartBeatAddr), IPADDR, 0},
                       {"hbeatPort", P(slotProf->cardType.etherSlot.multicastHeartBeatPort), INT, 0},
                       {"hbeatInterval", P(slotProf->cardType.etherSlot.multicastHeartBeatSlotTime), INT, 0},
                       {"hbeatRetry", P(slotProf->cardType.etherSlot.multicastHeartBeatSlotlength), INT, 0},
                       {"hbeatAlarm", P(slotProf->cardType.etherSlot.multicastHeartBeatAlarmThreshold), INT, 0},
                       {"hbeatSrcAddr", P(slotProf->cardType.etherSlot.multicastHeartBeatSrcAddr), IPADDR, 0},
                       {"hbeatSrcMask", P(slotProf->cardType.etherSlot.multicastHeartBeatSrcAddrMask), IPADDR, 0},
		       {"client", F(termSrvViewIgmpClients), CMD_ARG0},
		       {"group", F(termSrvViewIgmpGroupTable), CMD_ARG0},
		CMDEND};

    //INITBRANCHES;
    INITBRANCH;
}
# endif /* ( MROUTING == FYES ) || ( MTROUTING == FYES ) ) */

# if ( ( MROUTING == FYES ) || ( MTROUTING == FYES ) )
void cmdIpMrouting(TermInfo *term)
{
    static CmdList cmdlist[] = {
		       {"config", cmdDispConfig, CMD_LEAFVST},
		       {"enabled", P(slotProf->cardType.etherSlot.multicastForwarding), ENUM, (Pointer)yesnoEnum},
		       {"mboneProfile", P(slotProf->cardType.etherSlot.mboneProfile), STR, 0},
		       {"igmp", cmdIpIgmp,  CMD_N_RW, CMDTL, "igmp config"},
		       {"statistics", F(termSrvViewIgmpStats), CMD_ARG0},
		CMDEND};

    //INITBRANCHES;
    //INITBRANCH;
    //INITBRANCHB;
    INITBRANCHES;
}
# endif /* ( MROUTING == FYES ) || ( MTROUTING == FYES ) ) */


#if 0
void cmdIpDns(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"priDomainName", P(slotProf->cardType.etherSlot.domainName), STR, 0},
			{"secDomainName", P(slotProf->cardType.etherSlot.secDomainName), STR, 0},
			{"priServer", P(slotProf->cardType.etherSlot.primaryDnsServer), IPADDR, 0},
			{"secServer", P(slotProf->cardType.etherSlot.secondaryDnsServer), IPADDR, 0},
			{"clientDnsSpoof", P(slotProf->cardType.etherSlot.allowAsClientDnsInfo), ENUM, (Pointer)yesnoEnum},
			{"dnsProxy", P(slotProf->cardType.etherSlot.dnsProxyEnabled), ENUM, (Pointer)enableEnum},
			{"priWinsServer", P(slotProf->cardType.etherSlot.primaryNetBiosNS), IPADDR, 0},
			{"secWinsServer", P(slotProf->cardType.etherSlot.secondaryNetBiosNS), IPADDR, 0},
			{"clientPriDomainName", P(slotProf->cardType.etherSlot.clientDomainName), STR, 0},
			{"clientSecDomainName", P(slotProf->cardType.etherSlot.clientSecDomainName), STR, 0},
			{"clientPriDnsServer", P(slotProf->cardType.etherSlot.clientPrimaryDnsServer), IPADDR, 0},
			{"clientSecDnsServer", P(slotProf->cardType.etherSlot.clientSecondaryDnsServer), IPADDR, 0},
			{"dnsListAttemp", P(slotProf->cardType.etherSlot.dnsListAttempt), ENUM, (Pointer)yesnoEnum},
			{"maxDnsListSize", P(slotProf->cardType.etherSlot.dnsListSize), ENUMINT, (Pointer)&dnsListEnumInt},
			{"locaDnsTable", P(slotProf->cardType.etherSlot.localDnsTableEnabled), ENUM, (Pointer)enableEnum},
			{"updateLocalDnsTable", P(slotProf->cardType.etherSlot.localDnsTableAutoUpdate), ENUM, (Pointer)yesnoEnum},
			/*added by jfgu@090728*/
			{"dnsAutoAssign", P(slotProf->cardType.etherSlot.dnsAutoAssign), ENUM, (Pointer)yesnoEnum},
		CMDEND};
    INITBRANCHB;
}
#endif

#if 0
/*
 * added by llliu@081201 for public ddns.
 */
void  cmdPublicDdns(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(publicDdnsProf->active), ENUM, (Pointer)yesnoEnum},
			{"lively", P(publicDdnsProf->lively), ENUM, (Pointer)yesnoEnum},
			//{"interface", P(publicDdnsProf->interface), STR, 0},
			{"serverName", P(publicDdnsProf->serverName), STR, 0},
			{"serverIp", P(publicDdnsProf->serverIp), IPADDR},
			{"hostname", P(publicDdnsProf->hostname), STR, 0},
			{"username", P(publicDdnsProf->username), STR, 0},
#if (ENCRYPT_PASSWD == FYES)
			{"password", P(publicDdnsProf->password), PASSWD, 0},
#else
			{"password", P(publicDdnsProf->password), STR, 0},
#endif
			{"retryTimes", P(publicDdnsProf->retryTimes), INT, 0},
			{"retryInterval", P(publicDdnsProf->retryInterval), INT, 0},
			


			CMDEND};
    INITBRANCHI;



}


void cmdIpDdns(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"config", cmdDispConfig, CMD_LEAFVST},
    			{"enabled", P(slotProf->cardType.etherSlot.ext.ddns.Enable), ENUM, (Pointer)yesnoEnum},
			{"vendor", P(slotProf->cardType.etherSlot.ext.ddns.offset), ENUM, (Pointer)ddnsTzoneTypeEnum},
			{"serverName", P(slotProf->cardType.etherSlot.ext.ddns.serverName), STR, 0},
			{"priserver", P(slotProf->cardType.etherSlot.ext.ddns.Server1), IPADDR, 0},
			{"priservport", P(slotProf->cardType.etherSlot.ext.ddns.Port1), INT, 0},
			{"secserver", P(slotProf->cardType.etherSlot.ext.ddns.Server2), IPADDR, 0},
			{"secservport", P(slotProf->cardType.etherSlot.ext.ddns.Port2), INT, 0},
			{"hostname", P(slotProf->cardType.etherSlot.ext.ddns.Hostname), STR, 0},
			{"retrytimes", P(slotProf->cardType.etherSlot.ext.ddns.Updateretry), INT, 0},
			{"timeout", P(slotProf->cardType.etherSlot.ext.ddns.Updatetimeout), INT, 0},
			{"interval", P(slotProf->cardType.etherSlot.ext.ddns.UpdateInterval), INT, 0},
			{"keyname", P(slotProf->cardType.etherSlot.ext.ddns.Keyname), STR, 0},
			{"key", P(slotProf->cardType.etherSlot.ext.ddns.Key), STR, 0},
			{"enkey", P(slotProf->cardType.etherSlot.ext.ddns.enKey), STR, 0},
			{"profile", P(slotProf->cardType.etherSlot.ext.ddns.Profile), STR, 0},
			{"interface", P(slotProf->cardType.etherSlot.ext.ddns.interface), STR, 0},
//			{"lastupdatetime", P(slotProf->cardType.etherSlot.ext.ddns.LastUpdateTime), STR, 0},
			{"status",F(showUpdateTime), CMD_ARG0},

			{"public", cmdPublicDdns, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "public ddns", MIB_PROF_PUBLIC_DDNS},

			CMDEND};
    INITBRANCHB;
}
#endif

#if (SNTP == FYES)
void cmdIpSntp(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"enabled", P(slotProf->cardType.etherSlot.sntpEnabled), ENUM, (Pointer)sntpEnabledEnum},
//			{"timezone", P(slotProf->cardType.etherSlot.tzMode), ENUM, (Pointer)OffGMTEnum},
			{"interval", P(slotProf->cardType.etherSlot.sntpInterval), INT, 0},
			{"1stsntpserver", P(slotProf->cardType.etherSlot.sntpHost[0]), IPADDR, 0},
			{"2ndsntpserver", P(slotProf->cardType.etherSlot.sntpHost[1]), IPADDR, 0},
			{"3rdsntpserver", P(slotProf->cardType.etherSlot.sntpHost[2]), IPADDR, 0},
			CMDEND};
    INITBRANCHB;
}
#endif

# if ( SNMP == FYES )
void cmdSystemSnmpAgent(TermInfo *term)
{
    static CmdList cmdlist[] = {
		        {"config", cmdDispConfig, CMD_LEAFVST},
			{"enabled", P(slotProf->cardType.etherSlot.snmpEnabled), ENUM, (Pointer)yesnoEnum},
			{"restrictedHost", P(slotProf->cardType.etherSlot.snmpSecurity), ENUM, (Pointer)yesnoEnum},
			{"roCommunity", P(slotProf->cardType.etherSlot.readCommunity), STR, 0},
			{"1RoHost", P(slotProf->cardType.etherSlot.readAccessHost[0]), IPADDR, 0},
			{"1RoMask", P(slotProf->cardType.etherSlot.readAccessMask[0]), IPADDR, 0},
			{"2RoHost", P(slotProf->cardType.etherSlot.readAccessHost[1]), IPADDR, 0},
			{"2RoMask", P(slotProf->cardType.etherSlot.readAccessMask[1]), IPADDR, 0},
			{"3RoHost", P(slotProf->cardType.etherSlot.readAccessHost[2]), IPADDR, 0},
			{"3RoMask", P(slotProf->cardType.etherSlot.readAccessMask[2]), IPADDR, 0},
			{"allowSnmpSet", P(slotProf->cardType.etherSlot.ext.readWriteCommunityEnable), ENUM, (Pointer)yesnoEnum},
			{"rwCommunity", P(slotProf->cardType.etherSlot.readWriteCommunity), STR, 0},
			{"1RwHost", P(slotProf->cardType.etherSlot.writeAccessHost[0]), IPADDR, 0},
			{"1RwMask", P(slotProf->cardType.etherSlot.writeAccessMask[0]), IPADDR, 0},
			{"2RwHost", P(slotProf->cardType.etherSlot.writeAccessHost[1]), IPADDR, 0},
			{"2RwMask", P(slotProf->cardType.etherSlot.writeAccessMask[1]), IPADDR, 0},
			{"3RwHost", P(slotProf->cardType.etherSlot.writeAccessHost[2]), IPADDR, 0},
			{"3RwMask", P(slotProf->cardType.etherSlot.writeAccessMask[2]), IPADDR, 0},
			{"inQueueMax", P(slotProf->cardType.etherSlot.ext.queueDepth), INT, 0},
			CMDEND};
    INITBRANCHB;
}
# endif

# if ( SNMP_TRAP == FYES )
void cmdSnmpTrap(TermInfo *term)
{
    static CmdList cmdlist[] = {
		//	{"hostName", P(trapProf->hostName), STR, 0},
		//      {"config", cmdDispConfig, CMD_LEAFVST},
			{"enabled", P(trapProf->active), ENUM, (Pointer)yesnoEnum},
			{"hostIpAddr", P(trapProf->hostAddress), IPADDR, 0},
			{"securityEnabled", P(trapProf->securityEnabled), ENUM, (Pointer)yesnoEnum},
			{"trapCommunity", P(trapProf->communityName), STR, 0},
			{"enableSnmpAuthFail", P(trapProf->enableAuthentication), ENUM, (Pointer)yesnoEnum, 0},
			{"enableUTTPrivateTraps", P(trapProf->enableUTT), ENUM, (Pointer)yesnoEnum, 0},
			{"alarmEventEnabled", P(trapProf->alarmEnabled), ENUM, (Pointer)yesnoEnum},
			{"portChgedEnabled", P(trapProf->portEnabled), ENUM, (Pointer)yesnoEnum},
			{"enableColdStart", P(trapProf->enableColdStart), ENUM, (Pointer)yesnoEnum, 0},
			{"enableWarmStart", P(trapProf->enableWarmStart), ENUM, (Pointer)yesnoEnum, 0},
			{"enableLinkDown", P(trapProf->enableLinkDown), ENUM, (Pointer)yesnoEnum, 0},
			{"enableLinkUp", P(trapProf->enableLinkUp), ENUM, (Pointer)yesnoEnum, 0},
			{"enableConsole", P(trapProf->enableConsole), ENUM, (Pointer)yesnoEnum, 0},
			{"enableUseExceeded", P(trapProf->enableUseExceeded), ENUM, (Pointer)yesnoEnum, 0},
			{"enableTelnetSession", P(trapProf->enablePassword), ENUM, (Pointer)yesnoEnum, 0},
			{"enableFRLinkUp", P(trapProf->enableFRLinkUp), ENUM, (Pointer)yesnoEnum, 0},
			{"enableFRLinkDown", P(trapProf->enableFRLinkDown), ENUM, (Pointer)yesnoEnum, 0},
			{"eventLogOverwrite", P(trapProf->enableEventOverwrite), ENUM, (Pointer)yesnoEnum, 0},
			{"aAAChged", P(trapProf->enableRadiusChange), ENUM, (Pointer)yesnoEnum, 0},
			{"multicastLimit", P(trapProf->enableMcastMonitor), ENUM, (Pointer)yesnoEnum, 0},
			{"enableLanPort", P(trapProf->enableLanModem), ENUM, (Pointer)yesnoEnum, 0},
			{"enableDialTest", P(trapProf->enableDirdo), ENUM, (Pointer)yesnoEnum, 0},
			{"enableProfileChged", P(trapProf->enableSlotProfileChange), ENUM, (Pointer)yesnoEnum, 0},
			{"enablePowerFail", P(trapProf->enablePowerSupply), ENUM, (Pointer)yesnoEnum, 0},
			{"enableShelfFail", P(trapProf->enableMultiShelf), ENUM, (Pointer)yesnoEnum, 0},
			CMDEND};
    INITBRANCHI;
}
# endif

#if 0
void cmdSystemEventlog(TermInfo *term)
{
    static CmdList cmdlist[] = {
		        {"config", cmdDispConfig, CMD_LEAFVST},
			{"enabled", P(slotProf->cardType.etherSlot.logEnabled), ENUM, (Pointer)yesnoEnum},
			{"loghost", P(slotProf->cardType.etherSlot.logHost), STR, 0},
# if ( LOGPROF == FNO )
			{"logport", P(slotProf->cardType.etherSlot.ext.logPort), ENUMINT, (Pointer)&udpPortEnumInt},
# endif
			{"logfacility", P(slotProf->cardType.etherSlot.logFacility), ENUM, (Pointer)facilityEnum},
# if (RADIUS == FYES)
			{"eventtype", P(slotProf->cardType.etherSlot.ext.callInfo), ENUM, (Pointer)logCallInfoEnum},
# endif

#if (LOG == FYES)
			{"netActionEnabled", P(slotProf->cardType.etherSlot.logSwitch.netActionLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"telnetEnabled", P(slotProf->cardType.etherSlot.logSwitch.telnetLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"dhcpEnabled", P(slotProf->cardType.etherSlot.logSwitch.dhcpLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"arpEnabled", P(slotProf->cardType.etherSlot.logSwitch.arpLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"dialEnabled", P(slotProf->cardType.etherSlot.logSwitch.dialLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"natEnabled", P(slotProf->cardType.etherSlot.logSwitch.natLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"floodEnabled", P(slotProf->cardType.etherSlot.logSwitch.floodLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"otherEnabled", P(slotProf->cardType.etherSlot.logSwitch.otherLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"httpEnabled", P(slotProf->cardType.etherSlot.logSwitch.httpLogEnabled), ENUM, (Pointer)yesnoEnum},
			{"noticeEnabled", P(slotProf->cardType.etherSlot.logSwitch.noticeLogEnabled), ENUM, (Pointer)yesnoEnum},
#endif
			CMDEND};
    INITBRANCHB;
}

void cmdSystemBase(TermInfo *term)
{
    static CmdList cmdlist[] = {
		        {"config", cmdDispConfig, CMD_LEAFVST},
			{"romType", P(baseProf->romType), INT, 0},
			{"romVersion", P(baseProf->romVersion), INT, 0},
			{"romRevison", P(baseProf->romRevision), INT, 0},
			{"serialNumber", P(baseProf->serialNumber), INT, 0},
			{"buildString", P(baseProf->buildString), STR, 0},
			CMDEND};
    INITBRANCHB;
}

//void cmdSystemHbeat(TermInfo *term)
void cmdIpHbeat(TermInfo *term)
{
    static CmdList cmdlist[] = {
        {"config", cmdDispConfig, CMD_LEAFVST},
	{"heartbeatMode", P(slotProf->cardType.etherSlot.ext.hbeatConf.hbeatMode), ENUM, (Pointer)hbeatModeEnum},
	{"hbeatIcmpSend", P(slotProf->cardType.etherSlot.ext.hbeatConf.hbeatIcmp), ENUM, (Pointer)hbeatIcmpEnum},
	{"Status",F(termSrvViewHbeatStat), CMD_ARG0},
	CMDEND};

    //INITBRANCHB;
    INITBRANCHES;
}

void cmdIpDhcpServer(TermInfo *term)
{
	//DHCP_PLUS, disable old DHCP server
	
    static CmdList cmdlist[] = {
#if ( DHCP_SPOOF == FYES )
        		{"config", cmdDispConfig, CMD_LEAFVST},
			{"enabled", P(slotProf->cardType.etherSlot.dhcpSpoofEnabled), ENUM, (Pointer)yesnoEnum},
			{"pnp", P(slotProf->cardType.etherSlot.dhcpSpoofPnpEnabled), ENUM, (Pointer)enableEnum},
			{"renewaltimeout", P(slotProf->cardType.etherSlot.dhcpSpoofRenewalTime), INT, 0},
			{"actasdefaultgw", P(slotProf->cardType.etherSlot.dhcpSpoofBecomeDefaultRouter), ENUM, (Pointer)yesnoEnum},
//			{"forward", P(slotProf->cardType.etherSlot.dhcpSpoofDialIfLinkDown), ENUM, (Pointer)yesnoEnum},
//			{"spoofing", P(slotProf->cardType.etherSlot.alwaysSpoof), ENUM, (Pointer)yesnoEnum},
			{"poolstart", P(slotProf->cardType.etherSlot.dhcpSpoofIpAddress), IPADDR, 0},
			{"poolcount", P(slotProf->cardType.etherSlot.ext.groupOneCount), ENUMINT, (Pointer)&ipClassBEnumInt},
			{"poolnetmask", P(slotProf->cardType.etherSlot.dhcpSpoofNetmask), IPADDR, 0},
			{"pool2start", P(slotProf->cardType.etherSlot.ext.ipGroupTwo), IPADDR, 0},
			{"pool2count", P(slotProf->cardType.etherSlot.ext.groupTwoCount), ENUMINT, (Pointer)&ipClassBEnumInt},
			{"pool2netmask", P(slotProf->cardType.etherSlot.dhcpSpoofNetmask), IPADDR, 0},
			{"clientgateway", P(slotProf->cardType.etherSlot.dhcpSpoofGateway), IPADDR, 0},
			{"ipaddrvalidate", P(slotProf->cardType.etherSlot.ext.validateIP), ENUM, (Pointer)yesnoEnum},
			{"validatetimeout", P(slotProf->cardType.etherSlot.ext.maxNoReplyWait), INT, 0},
# endif
			CMDEND};

    INITBRANCHB; 
}
#endif

#if (DHCP_PLUS == FYES)
void cmdIpDhcpOption(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled",P(rawoptClassProf->active),ENUM, (Pointer)yesnoEnum},
			{"type", P(rawoptClassProf->type), INT},
			{"value", P(rawoptClassProf->value), STR},
			{"profile", P(rawoptClassProf->bindingport), ENUM, (Pointer)RawOptPortEnum},
			CMDEND};
    INITBRANCHI;
}
void cmdIpDhcpSvr(TermInfo *term)
{

    static CmdList cmdlist[] = {
			{"enabled", P(slotProf->cardType.etherSlot.ext.dhcpSvrOn), ENUM, (Pointer)yesnoEnum,"Turn on/off dhcp server",MIB_PROF_SLOT},
			{"status", F(termDhcpServerStatus), CMD_ARG0},
			{"statistics",F(termDhcpServerStatics),CMD_ARG0},
#if (DHCP_PLUS == FYES)
			/*added by jfgu@090626*/
			{"newautobind", P(slotProf->cardType.etherSlot.ext.newautobind), ENUM, (Pointer)yesnoEnum, "Turn on/off dhcp autobind", MIB_PROF_SLOT},
			{"delautobind", P(slotProf->cardType.etherSlot.ext.delautobind), ENUM, (Pointer)yesnoEnum, "Turn on/off dhcp del autobind", MIB_PROF_SLOT},
			{"dhcpSrvDetect", P(slotProf->cardType.etherSlot.ext.dhcpSrvDetect), ENUM, (Pointer)yesnoEnum, "Turn on/off dhcp server conflict", MIB_PROF_SLOT},
#endif
			CMDEND};
    INITBRANCHB;
    DOCLEAR {
	termClearDHCPSvrStatistics( 0 );
   }  
}
void cmdIpDhcpConflict(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"ping_packet", P(slotProf->cardType.etherSlot.ext.pingNum), INT},
			{"ping_timeout", P(slotProf->cardType.etherSlot.ext.pingTimeout), INT},
			CMDEND};

    INITBRANCHB;
}
void cmdIpDhcpPool(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled",P(dhcpPoolClassProf->active),ENUM, (Pointer)yesnoEnum},
			{"poolstart", P(dhcpPoolClassProf->poolstart), IPADDR},
			{"poolcount", P(dhcpPoolClassProf->poolcount), INT},
			{"netmask", P(dhcpPoolClassProf->netmask), IPADDR},
			{"gateway", P(dhcpPoolClassProf->gateway), IPADDR},
			{"priDNS", P(dhcpPoolClassProf->PriDNS), IPADDR},
			{"secDNS", P(dhcpPoolClassProf->SecDNS), IPADDR},
			{"priWINS", P(dhcpPoolClassProf->PriWINS), IPADDR},
			{"secWINS", P(dhcpPoolClassProf->SecWINS), IPADDR},
			{"autoipCoexist", P(dhcpPoolClassProf->bAutoIPcoexist), ENUM, (Pointer)yesnoEnum},
			{"netbiosType", P(dhcpPoolClassProf->NBTType), ENUM, (Pointer)NBTTypeEnum},
			{"domainName",P(dhcpPoolClassProf->domainName),STR,0},
			{"leaseTime", P(dhcpPoolClassProf->leasetime), INT},
			{"giaddr", P(dhcpPoolClassProf->giaddr), IPADDR},
			{"relayCircuitID",P(dhcpPoolClassProf->circuitId),STR,0},
			{"flags",P(dhcpPoolClassProf->flags),ENUM, (Pointer)OVERLOAD_FLAGSEnum},
			{"profile", P(dhcpPoolClassProf->bindingport), ENUM, (Pointer)RawOptPortEnum},//use RawOptPortEnum too
			CMDEND};
    INITBRANCHI;
}
void cmdIpDhcpClient(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"status", F(termDhcpClientStatus), CMD_ARG0},
			{"statistics",  F(termDhcpClientStatics), CMD_ARG0},
			CMDEND};
    INITBRANCHES;
    DOCLEAR {
    	termClearDHCPClientStatistics(0);
    }
}

void cmdIpDhcpRelay(TermInfo *term)
{
   static CmdList cmdlist[] = {
                        {"statistics",  F(termDhcpRelayStatistics), CMD_ARG0},
                        CMDEND};

    INITBRANCHES;
    DOCLEAR {
    	termClearDHCPRelayStatistics(0);
    }
}

#endif //#if (DHCP_PLUS == FYES)

/*
 * added by llliu@081107.
 */
#if (POLICY_ROUTE == FYES)

void cmdIpPrStrategy(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(policyRouteProf->active), ENUM, (Pointer)yesnoEnum},
			{"lively", P(policyRouteProf->lively), ENUM, (Pointer)yesnoEnum},
			{"interface", P(policyRouteProf->interface), STR, 0},
			{"ipFrom", P(policyRouteProf->ip_from), IPADDR, 0},
			{"ipEnd", P(policyRouteProf->ip_end), IPADDR, 0},
			{"portFrom", P(policyRouteProf->port_from), INT, 0},
			{"portEnd", P(policyRouteProf->port_end), INT, 0},
			{"protocol", P(policyRouteProf->protocol_id), INT, 0},
			{"pktSizeFrom", P(policyRouteProf->pkt_size_from), INT, 0},
			{"pktSizeEnd", P(policyRouteProf->pkt_size_end), INT, 0},

			CMDEND};

    INITBRANCHI;



}


void cmdIpPr(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"list", F(termSrvViewIpPrs), CMD_ARG1},
			{"strategy", cmdIpPrStrategy, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "policy route strategy", MIB_PROF_POLICY_ROUTE},
			CMDEND};

    INITBRANCHES;
}


#endif



# if ( BOOTP == FYES )
void cmdIpBootpRelay(TermInfo *term)
{
    static CmdList cmdlist[] = {
        		{"config", cmdDispConfig, CMD_LEAFVST},
			{"priBootpServer", P(slotProf->cardType.etherSlot.bootpServers[0]), IPADDR, 0},
			{"secBootpServer", P(slotProf->cardType.etherSlot.bootpServers[1]), IPADDR, 0},
# if ( BOOTP_RELAY == FYES )
			{"relayAgent", P(slotProf->cardType.etherSlot.bootpRelayEnabled), ENUM, (Pointer)enableEnum},
# endif
			CMDEND};
 
    INITBRANCHB;
}
# endif



# if ( NAT_NETBIOS == FYES )
void cmdIpName(TermInfo *term)
{
    INITLEAF;
    DOSHOW {
	termSrvViewIpNetbiosName(term->lcdTerm);
    }
    DOCLEAR {
	termSrvClearIpNetbiosName (term->lcdTerm);
    }
}
void cmdIpNetBios(TermInfo *term)
{
    INITLEAF;
    DOSHOW {
	termSrvViewNetbiosStat(term->lcdTerm);
    }
    DOCLEAR {
	//termSrvClearNetbiosStat (term->lcdTerm);
    }
}
# endif


#if (PPPOE_SRV == FYES)
void cmdPppoeOnly(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(pppoeOnlyProf->enabled), ENUM, (Pointer)yesnoEnum},
			{"permit",  P(pppoeOnlyProf->permit), ENUM, (Pointer)yesnoEnum},
			{"exceptIpGroup", P(pppoeOnlyProf->exceptIpGroup), STR, 0},
			CMDEND};
    INITBRANCHI;			
}
#endif


# if ( MDF_RADIUS_ACCT == FYES )
void cmdConnAcct(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"type", P_SIM(inetProf->pppoe.pppoeProfType), ENUM, (Pointer)pppoeTypeEnum},
			{"servicename", P_SIM(inetProf->pppoe.serviceName), STR},
			{"servername", P_SIM(inetProf->pppoe.serverName), STR},
#if (PPPOE_SRV == FYES)
			{"auto_bind_mac", P_SIM(inetProf->pppoe.auto_bind_mac),ENUM, (Pointer)yesnoEnum},
			{"bindmac1", P_SIM(inetProf->pppoe.bind_mac[0]),MACADDR2},
			{"bindmac2", P_SIM(inetProf->pppoe.bind_mac[1]),MACADDR2},
			{"bindmac3", P_SIM(inetProf->pppoe.bind_mac[2]),MACADDR2},
			{"bindmac4", P_SIM(inetProf->pppoe.bind_mac[3]),MACADDR2},
			{"session", P_SIM(inetProf->pppoe.acc_session),INT},
			{"tmstart", P_SIM(inetProf->pppoe.acc_tmstart),DATETIME},
			{"tmstop", P_SIM(inetProf->pppoe.acc_tmstop),DATETIME},
#endif
			CMDEND};
    INITBRANCH;
}
# endif

# if ( L2TP == FYES ) 

void cmdConnTunnel(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"type", P_SIM(inetProf->tunnelOpts.tunnelProfType), ENUM, (Pointer)tunnelTypeEnum},
			{"protocol", P_SIM(inetProf->tunnelOpts.tunnelingProtocol), ENUM, (Pointer)tunnelProtocolEnum},
			{"serveraddress", P_SIM(inetProf->tunnelOpts.priTunnelServer), STR},
			{"2ndserveraddr", P_SIM(inetProf->tunnelOpts.secTunnelServer), STR},
# if ( ENCRYPT_PASSWD == FYES )
			{"secret", P_SIM(inetProf->tunnelOpts.tunnelServerPassword), PASSWD},
# else
			{"secret", P_SIM(inetProf->tunnelOpts.tunnelServerPassword), STR},
# endif
			CMDEND};
    INITBRANCH;
}

#endif
 

# if ( ( MROUTING == FYES ) || ( MTROUTING == FYES ) )
void cmdConnIpMrouting(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"mrouting", P_SIM(inetProf->ip.multicastAllowed), ENUM, (Pointer)yesnoEnum},
			{"mcastRate", P_SIM(inetProf->ip.multicastRateLimit), INT, 0},
			{"mcastLeaveDelay", P_SIM(inetProf->ip.multicastGroupLeaveDelay), INT, 0},
			CMDEND};

    INITBRANCH;
}
# endif

# if ( MPP == FYES )
void cmdConnEncapsAdvance(TermInfo *term)
{
    static CmdList cmdlist[] = {
			 {"mpenabled", P_SIM(inetProf->mp.mpEnabled), ENUM, (Pointer)yesnoEnum},
			 {"mppenabled", P_SIM(inetProf->mpp.mppEnabled), ENUM, (Pointer)yesnoEnum},
			 {"bacp", P_SIM(inetProf->mp.bacpEnable), ENUM, (Pointer)enableEnum},
			 {"history", P_SIM(inetProf->mpp.secondsOfHistory), INT, 0},
			 {"targetutil", P_SIM(inetProf->mpp.targetUtilization), ENUMINT, (Pointer)&percentEnumInt},
//			 {"splitcode", P_SIM(inetProf->ppp.splitCodeDotUserEnabled), ENUM, (Pointer)enableEnum},
			CMDEND};

    INITBRANCH;
}

void cmdConnEncapsPers(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"add", P_SIM(inetProf->mpp.addSecondsOfPersistence), INT, 0},
			{"remove", P_SIM(inetProf->mpp.removeSecondsOfPersistence), INT, 0},
			CMDEND};

    INITBRANCH;
}

void cmdConnEncapsChannel(TermInfo *term)
{
    static CmdList cmdlist[] = {
	    		{"base", P_SIM(inetProf->mp.baseChannelCount), INT, 0},
	    		{"min", P_SIM(inetProf->mp.minimumChannelCount), INT, 0},
	    		{"max", P_SIM(inetProf->mp.maximumChannelCount), INT, 0},
	    		CMDEND};

    INITBRANCH;
}

# endif

#if 0
void cmdConnEncapsLqm(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"active", P_SIM(inetProf->ppp.lqmEnable), ENUM, (Pointer)pppLqmEnum},
			{"min", P_SIM(inetProf->ppp.lqmMinPeriod), ENUMINT, (Pointer)&pppLqmEnumInt},
			{"max", P_SIM(inetProf->ppp.lqmMaxPeriod), ENUMINT, (Pointer)&pppLqmEnumInt},
			CMDEND};

    INITBRANCH;
}

void cmdConnEncapsRecv(TermInfo *term)
{
    static CmdList cmdlist[] = {
//			{"name", P(inetProf->ppp.substRecvName), STR, 0},
# if ( ENCRYPT_PASSWD == FYES )
			{"pw", P_SIM(inetProf->ppp.recvPassword), PASSWD, 0},
//			{"auxpw", P(inetProf->mpp.auxSendPassword), PASSWD, 0},
# else
			{"pw", P_SIM(inetProf->ppp.recvPassword), STR, 0},
//			{"auxpw", P(inetProf->mpp.auxSendPassword), STR, 0},
# endif
			CMDEND};
	
    INITBRANCH;
}

void cmdConnEncapsSend(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"authtype", P_SIM(inetProf->ppp.pppAuthMode), ENUM, (Pointer)pppSendAuthEnum},
			{"name", P_SIM(inetProf->ppp.substSendName), STR, 0},
# if ( ENCRYPT_PASSWD == FYES )
			{"pw", P_SIM(inetProf->ppp.sendPassword), PASSWD, 0},
# if ( MPP == FYES )
			{"auxpw", P_SIM(inetProf->mpp.auxSendPassword), PASSWD, 0},
# endif
# else
			{"pw", P_SIM(inetProf->ppp.sendPassword), STR, 0},
# if ( MPP == FYES )
			{"auxpw", P_SIM(inetProf->mpp.auxSendPassword), STR, 0},
# endif
# endif
#if (NETKEEPER == FYES)
			{"dialtype", P_SIM(inetProf->ppp.nkDialMode), ENUM, (Pointer)nkDialModeEnum},
#endif
			CMDEND};
	
    INITBRANCH;
}

void cmdConnEncaps(TermInfo *term)
{
    static CmdList cmdlist[] = {
			 {"type", P_SIM(inetProf->mgmtType), ENUM, (Pointer)mgmtTypeEnumValues},
			 {"send", cmdConnEncapsSend, CMD_LEAFDAD},
			 {"recv", cmdConnEncapsRecv, CMD_LEAFDAD},
			 {"mru", P_SIM(inetProf->ppp.myPppMru), ENUMINT, (Pointer)&pppMruEnumInt},
			 {"mtu", P_SIM(inetProf->ppp.myPppMtu), ENUMINT, (Pointer)&pppMruEnumInt},
			 {"lqm", cmdConnEncapsLqm, CMD_LEAFDAD},
			 {"comp", P_SIM(inetProf->ppp.compressionType), ENUM, (Pointer)compressionEnum},
# if ( MPP == FYES )
			 {"channel", cmdConnEncapsChannel, CMD_LEAFDAD},
//			 {"dynalg", cmdConnEncapsDynalg, CMD_LEAF},
			 {"pers", cmdConnEncapsPers, CMD_LEAFDAD},
			 {"advance", cmdConnEncapsAdvance, CMD_LEAFDAD},
# endif
#if ( FRAME_RELAY == FYES )
			 {"framerelay", cmdConnEncapsFR, CMD_LEAFDAD},
#endif
# if ( X25B == FYES )
			 {"ipox25", cmdConnEncapsIpoX25, CMD_LEAFDAD},
# endif
			 CMDEND};
    
    INITBRANCH;
    
}

void cmdConn(TermInfo *term)
{
    WanSession *sess;
//  LinearPortNumber linearPort;

    static CmdList cmdlist[] = {
		{"enabled", P_SIM(inetProf->active), ENUM, (Pointer)yesnoEnum},
		{"permit", P_SIM(inetProf->activeB), ENUM, (Pointer)yesnoEnum},
		{"description", P_SIM(inetProf->description), STR, 0},
		{"rxBandwidth", P_SIM(inetProf->telco.rxBandwidth), INT, 0},
		{"txBandwidth", P_SIM(inetProf->telco.txBandwidth), INT, 0},
# if ( MAX_INFORMATION_RATE == FYES )
		{"rxBPS", P_SIM(inetProf->session.maxRxDataRate), INT, 0},
		{"rxPPS", P_SIM(inetProf->session.maxRxDataRate), INT, 0},
		{"txBPS", P_SIM(inetProf->session.maxTxDataRate), INT, 0},
		{"txPPS", P_SIM(inetProf->session.maxTxDataRate), INT, 0},
//		{"rxBPS", P(slotProf->cardType.etherSlot.ext.ingressBPS), ENUM, (Pointer)userSpeedEnum},
//		{"rxPPS", P(slotProf->cardType.etherSlot.ext.ingressPPS), INT, 0},
//		{"txBPS", P(slotProf->cardType.etherSlot.ext.egressBPS), ENUM, (Pointer)userSpeedEnum},
//		{"txPPS", P(slotProf->cardType.etherSlot.ext.egressPPS), INT, 0},
# endif
#if ( VROUTER == FYES )
		{"vRouter", P_SIM(inetProf->vRouterName), STR, 0},
#endif
//              {"bridge", cmdConnBridge, CMD_LEAF, CMDTL},
//		{"callback", cmdConnCallback, CMD_LEAFDAD, CMDTL}, 
//		{"config", cmdDispConfig, CMD_LEAFVST},
//		{"filter", cmdConnPolicy, CMD_LEAFDAD, CMDTL},
		{"policy", cmdConnPolicy, CMD_LEAFDAD|CMD_SIM, CMDTL},
		{"dial", cmdConnDial, CMD_LEAFDAD|CMD_SIM, CMDTL},
		{"encaps", cmdConnEncaps, CMD_LEAFDAD|CMD_SIM, CMDTL},
		{"ip", cmdConnIp, CMD_LEAFDAD|CMD_SIM, CMDTL},
		{"line", cmdConnLine, CMD_LEAFDAD|CMD_SIM, CMDTL}, 
                {"group", cmdConnGroup, CMD_LEAF|CMD_SIM, CMDTL},
		{"pppoe", cmdConnPppoe, CMD_LEAFDAD|CMD_SIM, CMDTL}, 
# if ( L2TP == FYES ) 
		{"tunnel", cmdConnTunnel, CMD_LEAFDAD|CMD_SIM, CMDTL}, 
# endif
# if ( MDF_RADIUS_ACCT == FYES )
		{"acct", cmdConnAcct, CMD_LEAFDAD|CMD_SIM, CMDTL}, 
# endif
		/*added by jfgu0717*/
		{"remark", P_SIM(inetProf->remark), STR, 0}, 
		CMDEND};

    const InternetProfile *prof1 = NULL;
    
    if ( !HFR_ENABLED(DEFAULT_FSET, HFR_PPPOE) ) DISABLE_ITEM("pppoe");

# if ( L2TP == FYES ) 
    if ( !HFR_ENABLED(DEFAULT_FSET, HFR_L2TP) ) DISABLE_ITEM("tunnel");
# endif

    INITBRANCHI;

    DODIAL
    {
	prof1 = lanProfileByNumber(term->inst[GETCMD->level]);
	if(prof1->active && prof1->activeB)
//  && _strlen((char *)prof1->phoneNumber) > 0)
	{
		if ( ! wanDataCallByProfile(prof1, 0, NULL, NULL, FALSE) ) {
		  releaseInternetProfilePointer(prof1);
                }
	}
	else
	    _printf(term, "Invalid connection.\r\n");
    }
    DOHANGUP
    {
	prof1 = lanProfileByNumber(term->inst[GETCMD->level]);
	if(prof1->active && prof1->activeB) 
	{
	    if(wanDataSessionByName2( prof1->hostName, &sess ))
	    {
	       //lanPortClearCall( sess->connInfo->portNum );
	       wanDataClearCall( sess->ifNum, DIS_USER_CALL_CLEAR_REQUEST);
	       releaseInternetProfilePointer(prof1);
            }
	}
    }


}
#endif

/* added by llliu */
#if (PPPOE_SRV == FYES)

void cmdPort1MfConf(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->sec_ctl[0].mac_leach), ENUM, (Pointer)yesnoEnum},
		{"type", P(_pppoeSrvProf->sec_ctl[0].mac_leach_type), ENUM, (Pointer)macFilterEnum},
		CMDEND };
	INITBRANCH;
}
void cmdPort1Conf (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->port[0].active), ENUM, (Pointer)yesnoEnum},
		{"session", P(_pppoeSrvProf->port[0].port_sess_num),INT, 0},
		{"pridns",P(_pppoeSrvProf->port[0].pri_dns), IPADDR,0},
		{"secdns",P(_pppoeSrvProf->port[0].sec_dns), IPADDR,0},
		{"priwins",P(_pppoeSrvProf->port[0].pri_wins), IPADDR,0},
		{"secwins",P(_pppoeSrvProf->port[0].sec_wins), IPADDR,0},
		{"localip",P(_pppoeSrvProf->port[0].local_address), IPADDR,0},
		{"mac_filter", cmdPort1MfConf, CMD_LEAFDAD, CMDTL},
		{"detect_ac_spoof", P(_pppoeSrvProf->sec_ctl[0].detect_ac_spoof), ENUM, (Pointer)yesnoEnum},
		{"bind_mac_account", P(_pppoeSrvProf->sec_ctl[0].bind_mac_acc), ENUM, (Pointer)yesnoEnum},
		CMDEND};

	INITBRANCH;
}
void cmdPort2MfConf(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->sec_ctl[1].mac_leach), ENUM, (Pointer)yesnoEnum},
		{"type", P(_pppoeSrvProf->sec_ctl[1].mac_leach_type), ENUM, (Pointer)macFilterEnum},
		CMDEND };
	INITBRANCH;
}
void cmdPort2Conf (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->port[1].active), ENUM, (Pointer)yesnoEnum},
		{"session", P(_pppoeSrvProf->port[1].port_sess_num),INT, 0},
		{"pridns",P(_pppoeSrvProf->port[1].pri_dns), IPADDR,0},
		{"secdns",P(_pppoeSrvProf->port[1].sec_dns), IPADDR,0},
		{"priwins",P(_pppoeSrvProf->port[1].pri_wins), IPADDR,0},
		{"secwins",P(_pppoeSrvProf->port[1].sec_wins), IPADDR,0},
		{"localip",P(_pppoeSrvProf->port[1].local_address), IPADDR,0},
		{"mac_filter", cmdPort2MfConf, CMD_LEAFDAD, CMDTL},
		{"detect_ac_spoof", P(_pppoeSrvProf->sec_ctl[1].detect_ac_spoof), ENUM, (Pointer)yesnoEnum},
		{"bind_mac_account", P(_pppoeSrvProf->sec_ctl[1].bind_mac_acc), ENUM, (Pointer)yesnoEnum},
		CMDEND};

	INITBRANCH;
}
#if (NB_PORT >= 3)
void cmdPort3MfConf(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->sec_ctl[2].mac_leach), ENUM, (Pointer)yesnoEnum},
		{"type", P(_pppoeSrvProf->sec_ctl[2].mac_leach_type), ENUM, (Pointer)macFilterEnum},
		CMDEND };
	INITBRANCH;
}
void cmdPort3Conf (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->port[2].active), ENUM, (Pointer)yesnoEnum},
		{"session", P(_pppoeSrvProf->port[2].port_sess_num),INT, 0},
		{"pridns",P(_pppoeSrvProf->port[2].pri_dns), IPADDR,0},
		{"secdns",P(_pppoeSrvProf->port[2].sec_dns), IPADDR,0},
		{"priwins",P(_pppoeSrvProf->port[2].pri_wins), IPADDR,0},
		{"secwins",P(_pppoeSrvProf->port[2].sec_wins), IPADDR,0},
		{"localip",P(_pppoeSrvProf->port[2].local_address), IPADDR,0},
		{"mac_filter", cmdPort3MfConf, CMD_LEAFDAD, CMDTL},
		{"detect_ac_spoof", P(_pppoeSrvProf->sec_ctl[2].detect_ac_spoof), ENUM, (Pointer)yesnoEnum},
		{"bind_mac_account", P(_pppoeSrvProf->sec_ctl[2].bind_mac_acc), ENUM, (Pointer)yesnoEnum},
		CMDEND};

	INITBRANCH;
}
#if (NB_PORT >= 4)
void cmdPort4MfConf(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->sec_ctl[3].mac_leach), ENUM, (Pointer)yesnoEnum},
		{"type", P(_pppoeSrvProf->sec_ctl[3].mac_leach_type), ENUM, (Pointer)macFilterEnum},
		CMDEND };
	INITBRANCH;
}
void cmdPort4Conf (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->port[3].active), ENUM, (Pointer)yesnoEnum},
		{"session", P(_pppoeSrvProf->port[3].port_sess_num),INT, 0},
		{"pridns",P(_pppoeSrvProf->port[3].pri_dns), IPADDR,0},
		{"secdns",P(_pppoeSrvProf->port[3].sec_dns), IPADDR,0},
		{"priwins",P(_pppoeSrvProf->port[3].pri_wins), IPADDR,0},
		{"secwins",P(_pppoeSrvProf->port[3].sec_wins), IPADDR,0},
		{"localip",P(_pppoeSrvProf->port[3].local_address), IPADDR,0},
		{"mac_filter", cmdPort4MfConf, CMD_LEAFDAD, CMDTL},
		{"detect_ac_spoof", P(_pppoeSrvProf->sec_ctl[3].detect_ac_spoof), ENUM, (Pointer)yesnoEnum},
		{"bind_mac_account", P(_pppoeSrvProf->sec_ctl[3].bind_mac_acc), ENUM, (Pointer)yesnoEnum},
		CMDEND};

	INITBRANCH;
}
#if (NB_PORT >= 5)
void cmdPort5MfConf(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->sec_ctl[3].mac_leach), ENUM, (Pointer)yesnoEnum},
		{"type", P(_pppoeSrvProf->sec_ctl[3].mac_leach_type), ENUM, (Pointer)macFilterEnum},
		CMDEND };
	INITBRANCH;
}
void cmdPort5Conf (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"enable", P(_pppoeSrvProf->port[4].active), ENUM, (Pointer)yesnoEnum},
		{"session", P(_pppoeSrvProf->port[4].port_sess_num),INT, 0},
		{"pridns",P(_pppoeSrvProf->port[4].pri_dns), IPADDR,0},
		{"secdns",P(_pppoeSrvProf->port[4].sec_dns), IPADDR,0},
		{"priwins",P(_pppoeSrvProf->port[4].pri_wins), IPADDR,0},
		{"secwins",P(_pppoeSrvProf->port[4].sec_wins), IPADDR,0},
		{"localip",P(_pppoeSrvProf->port[4].local_address), IPADDR,0},
		{"mac_filter", cmdPort5MfConf, CMD_LEAFDAD, CMDTL},
		{"detect_ac_spoof", P(_pppoeSrvProf->sec_ctl[4].detect_ac_spoof), ENUM, (Pointer)yesnoEnum},
		{"bind_mac_account", P(_pppoeSrvProf->sec_ctl[4].bind_mac_acc), ENUM, (Pointer)yesnoEnum},
		CMDEND};

	INITBRANCH;
}
#endif /* NB_PORT >= 5 */
#endif /* NB_PORT >= 4 */
#endif /* NB_PORT >= 3 */

void cmdHangUp (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"username", P(_pppoeSrvProf->sess_hangup.username), STR, 0},
		{"sid", P(_pppoeSrvProf->sess_hangup.sid), INT, 0},
		{"mac", P(_pppoeSrvProf->sess_hangup.hangup_mac), MACADDR2, 0},
		CMDEND};

	INITBRANCH;
}


#if (PPPOE_ACCOUNT == FYES)
void cmdPppoeAccount(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"writeInterval", P(_pppoeSrvProf->account.write_interval), INT, 0},
		{"hourDivisor", P(_pppoeSrvProf->account.hour_divisor), INT, 0},
		CMDEND};

	INITBRANCH;
}
#endif

		
void cmdSessionConf(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"systemnum", P(_pppoeSrvProf->max_sess_num), INT, 0},
		{"macnum", P(_pppoeSrvProf->max_mac_sess), INT, 0},
		//{"accountnum", P(_pppoeSrvProf->account_used_num), INT, 0},
		CMDEND};

	INITBRANCH;
}
# if 0
void cmdPppoeDns (TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"primary", P(_pppoeSrvProf->pppoe_dns.pri_dns), IPADDR, 0},
		{"second", P(_pppoeSrvProf->pppoe_dns.sec_dns), IPADDR, 0},
		CMDEND};

	INITBRANCH;
}
#endif
	
void cmdPppoeSrv (TermInfo *term)
{
	static CmdList cmdlist[] = {
		//{"hostName", P(_pppoeSrvProf->hostName), STR, 0},	
		{"enable", P(_pppoeSrvProf->active), ENUM, (Pointer)yesnoEnum},
		//{"vlan", P(_pppoeSrvProf->vlan_active), ENUM, (Pointer)yesnoEnum},
		{"session", cmdSessionConf, CMD_LEAFDAD, CMDTL},
		CLI_FIX_ETH,
		//{"hangup", cmdHangUp, CMD_LEAFDAD, CMDTL},
		{"hangup", cmdHangUp, CMD_LEAFDAD, CMDTL},
#if (PPPOE_ACCOUNT == FYES)
		{"account", cmdPppoeAccount, CMD_LEAFDAD, CMDTL},
#endif
		CMDEND};

	INITBRANCHI;
}
void cmdMacFilter (TermInfo *term)
{
	static CmdList cmdlist[] = {
		//{"hostName", P(_macFilterProf->hostName), STR, 0},
		{"enable", P(_macFilterProf->active),ENUM, (Pointer)yesnoEnum},
		{"type", P_SIM(_macFilterProf->type),ENUM, (Pointer)macFilterEnum},
		{"mac", P_SIM(_macFilterProf->mac), MACADDR2, 0},
		{"profile", P_SIM(_macFilterProf->bind_port), ENUM, (Pointer)bindPortEnum},
		CMDEND};
	INITBRANCHI;
}

	
		



void cmdMac(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"filter", cmdMacFilter, CMD_CDRI|CMD_LIST_RWCD|CMD_SIM, CMDTL, "mac filter", MIB_PROF_MAC_FILTER},
		CMDEND};
	INITBRANCHB;
}

#endif

#if 0
void cmdPppoe(TermInfo *term)
{
	static CmdList cmdlist[] = {
#if (PPPOE_SRV == FYES)	/* added by llliu */
		{"server", cmdPppoeSrv, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "pppoe server ",MIB_PROF_PPPOE_SRV},
		{"session", F(termSrvViewPPPoEServerSession), CMD_ARG1},
		{"mft", F(termSrvViewMacFilter), CMD_ARG1},
		{"account", F(termSrvViewPPPoEServerAccount), CMD_ARG1},
#endif
		{"event", F(termSrvViewPPPoEEvent), CMD_ARG1},
#if 0
		//{"client", cmdPppoeClient, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "pppoe client",MIB_PROF_PPPOE_CLIENT}, 
#endif
		CMDEND};

	INITBRANCHB;	

}
#endif

#if (FC == FYES)

void cmdTokenBucket(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"config", cmdDispConfig, CMD_LEAFVST},
		{"interval", P(slotProf->cardType.etherSlot.ext.fcRateLimit.tb.tbInterval), INT, 0},
		{"depth", P(slotProf->cardType.etherSlot.ext.fcRateLimit.tb.tbDepth), INT, 0},

                CMDEND};

    INITBRANCHB;

}

void cmdFlex(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"config", cmdDispConfig, CMD_LEAFVST},
		{"hosts", P(slotProf->cardType.etherSlot.ext.fcRateLimit.flex.hosts), INT, 0},
		{"lowLevel", P(slotProf->cardType.etherSlot.ext.fcRateLimit.flex.lowLevel), INT, 0},
		{"midLevel", P(slotProf->cardType.etherSlot.ext.fcRateLimit.flex.midLevel), INT, 0},
		{"highLevel", P(slotProf->cardType.etherSlot.ext.fcRateLimit.flex.highLevel), INT, 0},
                CMDEND};

    INITBRANCHB;

}


void cmdP2p(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"config", cmdDispConfig, CMD_LEAFVST},
		{"enable", P(slotProf->cardType.etherSlot.ext.fcRateLimit.p2p.enable), ENUM, (Pointer)yesnoEnum},
		{"share", P(slotProf->cardType.etherSlot.ext.fcRateLimit.p2p.share), ENUM, (Pointer)yesnoEnum},
		{"rx", P(slotProf->cardType.etherSlot.ext.fcRateLimit.p2p.rx), INT, 0},
		{"tx", P(slotProf->cardType.etherSlot.ext.fcRateLimit.p2p.tx), INT, 0},
		{"exceptionIpGroup", P(slotProf->cardType.etherSlot.ext.fcRateLimit.p2p.exceptionIpGroupName), STR, 0},
                CMDEND};

    INITBRANCHB;

}


#if (FC_PRIORITY == FYES)
void cmdNolimit(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"config", cmdDispConfig, CMD_LEAFVST},
		{"enable", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.enable), ENUM, (Pointer)yesnoEnum},
		{"icmp", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.icmp), ENUM, (Pointer)yesnoEnum},
		{"web", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.web), ENUM, (Pointer)yesnoEnum},
		{"vpn", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.vpn), ENUM, (Pointer)yesnoEnum},
		{"ack", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.ack), ENUM, (Pointer)yesnoEnum},
		{"game", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.game), ENUM, (Pointer)yesnoEnum},
		{"voip", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.voip), ENUM, (Pointer)yesnoEnum},
		{"mail", P(slotProf->cardType.etherSlot.ext.fcRateLimit.nolimit.mail), ENUM, (Pointer)yesnoEnum},
                CMDEND};

    INITBRANCHB;
}
#endif




#if 0

void cmdFcRateLimit(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"config", cmdDispConfig, CMD_LEAFVST},
		{"enable", P(slotProf->cardType.etherSlot.ext.fcRateLimit.enable), ENUM, (Pointer)yesnoEnum},
		{"account", P(slotProf->cardType.etherSlot.ext.fcRateLimit.account), ENUM, (Pointer)yesnoEnum},
#if (APP_TRAFFIC == FYES)
		{"l7Account", P(slotProf->cardType.etherSlot.ext.fcRateLimit.l7Account), ENUM, (Pointer)yesnoEnum},
#endif
		{"calHeader", P(slotProf->cardType.etherSlot.ext.fcRateLimit.calHeader), ENUM, (Pointer)yesnoEnum},
		{"ipOnly", P(slotProf->cardType.etherSlot.ext.fcRateLimit.ipOnly), ENUM, (Pointer)yesnoEnum},
		{"baseIf", P(slotProf->cardType.etherSlot.ext.fcRateLimit.baseIf), STR, 0},

		{"tokenBucket", cmdTokenBucket, CMD_CLEAR|CMD_LIST_CLEAR|CMD_LIST_RWCD|CMD_LIST_MORE},
		{"flex", cmdFlex, CMD_CLEAR|CMD_LIST_CLEAR|CMD_LIST_RWCD|CMD_LIST_MORE},
		{"p2p", cmdP2p, CMD_CLEAR|CMD_LIST_CLEAR|CMD_LIST_RWCD|CMD_LIST_MORE},
#if (FC_PRIORITY == FYES)
		{"nolimit", cmdNolimit, CMD_CLEAR|CMD_LIST_CLEAR|CMD_LIST_RWCD|CMD_LIST_MORE},
#endif
                CMDEND};

    INITBRANCHB;

}


void cmdFc(TermInfo *term)
{
	static CmdList cmdlist[] = {
		{"ratelimit", cmdFcRateLimit, CMD_CLEAR|CMD_LIST_CLEAR|CMD_LIST_RWCD|CMD_LIST_MORE},
                CMDEND};
    INITBRANCH;

}

void cmdRateLimitRule(TermInfo *term)
{
    static CmdList cmdlist[] = {
                	{"enabled",P(rateLimitProf->active), ENUM, (Pointer)yesnoEnum},
                	{"lively",P(rateLimitProf->enable), ENUM, (Pointer)yesnoEnum},
                	{"srcIpGroupName",P(rateLimitProf->srcIpGroupName), STR, 0},
                	{"dstIpGroupName",P(rateLimitProf->dstIpGroupName), STR, 0},
                	{"portGroupName",P(rateLimitProf->portGroupName), STR, 0},
                	{"gRxBandwidth",P(rateLimitProf->guaranteeRxBandwidth), INT, 0},
                	{"gTxBandwidth",P(rateLimitProf->guaranteeTxBandwidth), INT, 0},
                	{"mRxBandwidth",P(rateLimitProf->maxRxBandwidth), INT, 0},
                	{"mTxBandwidth",P(rateLimitProf->maxTxBandwidth), INT, 0},
                	{"outputIfName",P(rateLimitProf->outputIfName), STR, 0},
                	{"tmrgName",P(rateLimitProf->tmrgName), STR, 0},
                	{"priority",P(rateLimitProf->priority), ENUM, (Pointer)fbmPriorityEnum},
                	{"share",P(rateLimitProf->share), ENUM, (Pointer)yesnoEnum},
                	{"remark",P(rateLimitProf->remark), STR, 0},
                        CMDEND};
    INITBRANCHI;

}

static void cmdRlHost(TermInfo *term)
{


   INITLEAF;
           
    DOSHOW {
	termSrvViewRlHost(term->lcdTerm, argGet(term->lastInput, 4, (int *)NULL));
    }
    DOCLEAR {
	termSrvClearRlHost(term->lcdTerm);
    }
    DOMORE {
	lcdTermSetMoreFlag(term->lcdTerm, TRUE);
	lcdTermMoreInit(term->lcdTerm, TRUE);
	termSrvViewRlHost(term->lcdTerm, argGet(term->lastInput, 4, (int *)NULL));
	lcdTermSetMoreFlag(term->lcdTerm, FALSE);
    }

}



void cmdRateLimit(TermInfo *term)
{
    static CmdList cmdlist[] = {
	{"rule", cmdRateLimitRule, CMD_CDRI|CMD_CLEAR|CMD_LIST_CLEAR|CMD_LIST_RWCD},
	{"host", cmdRlHost, CMD_LEAFVST|CMD_CLEAR|CMD_LIST_CLEAR|CMD_MORE|CMD_LIST_MORE},

	{"figure", F(termSrvViewRlFigure), CMD_ARG1},
	{"share", F(termSrvViewRlShare), CMD_ARG1},
	{"iftb", F(termSrvViewRlIfTb), CMD_ARG1},

	CMDEND};
    INITBRANCHI;
}

			

#endif
#endif

#if (NOTICE == FYES)
//added by dhyu
void cmdNotice(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enable", P(noticeProf->enable), ENUM, (Pointer)yesnoEnum},
			{"htmlTitle", P(noticeProf->htmlTitle), STR, 0},
			{"line1", P(noticeProf->line1), STR, 0},
			{"line2", P(noticeProf->line2), STR, 0},
			{"line3", P(noticeProf->line3), STR, 0},
			{"line4", P(noticeProf->line4), STR, 0},
			{"line5", P(noticeProf->line5), STR, 0},
			{"line6", P(noticeProf->line6), STR, 0},
			{"line7", P(noticeProf->line7), STR, 0},
			{"line8", P(noticeProf->line8), STR, 0},
			{"line9", P(noticeProf->line9), STR, 0},
			{"line10", P(noticeProf->line10), STR, 0},
			{"line11", P(noticeProf->line11), STR, 0},
			{"line12", P(noticeProf->line12), STR, 0},
			{"line13", P(noticeProf->line13), STR, 0},
			{"manager", P(noticeProf->manager), STR, 0},
			{"urlContent", P(noticeProf->urlContent), STR, 0},
			{"UrlOrHtml", P(noticeProf->urlorhtml), ENUM, (Pointer)UrlOrHtmlEnum},
			{"IpGroupName", P(noticeProf->IpGroupName), STR, 0},
			{"preTime", P(noticeProf->preTime), STR, 0},
			{"lively", P(noticeProf->lively), ENUM, (Pointer)yesnoEnum},
			{"time", P(noticeProf->time), TIME, 0},
#if (PPPOE_SRV == FYES)
			{"pppoerxflow", P(noticeProf->pppoerxflow), INT, 0},
			{"pppoetxflow", P(noticeProf->pppoetxflow), INT, 0},
			{"pppoeday", P(noticeProf->pppoeday), INT, 0},
			{"pppoehour", P(noticeProf->pppoehour), INT, 0},
#endif
			CMDEND};
    INITBRANCHI;

}
#endif


void cmdRoot(TermInfo *term)
{
    int inst[MAX_LEVEL];
    int i; 
//  const InternetProfile *p_inetProf;
//  char tmp[MAX_CMDLINE];
//  Pointer prof;
    static CmdList cmdlist[] = {
#if 0
//	{"hbeat", cmdSystemHbeat, CMD_RW|CMD_LIST_RW, CMDTL, "system heart beat monitor configuration", MIB_PROF_SLOT},
	{"base", cmdSystemBase, CMD_R|CMD_LIST_R, CMDTL, "system base configuration (ReadOnly)", MIB_PROF_BASE},
	{"connection", cmdConn, CMD_CDRI|CMD_DIAL|CMD_HANGUP|CMD_LIST_DIAL|CMD_LIST_RWCD|CMD_SIM, CMDTL, "call setup", MIB_PROF_INTERNET}, 
	{"filter", cmdFilter, CMD_N_RWCD, CMDTL, "generic packet filter", MIB_PROF_FILTER},
	{"interface", cmdInt, CMD_N_RW, CMDTL, "system interface config"}, 
        {"ip", cmdIp, CMD_N_RWCD|CMD_LIST_CLEAR|CMD_LIST_DEBUG|CMD_LIST_MORE, CMDTL, "ip config", MIB_PROF_SLOT},
	{"session", cmdSess, CMD_LIST_R|CMD_LIST_CLEAR, CMDTL, "active session information"},
	{"system", cmdSystem, CMD_N_RWCD, CMDTL, "system config", MIB_PROF_SYSTEM},

	{"user", cmdUsers, CMD_CDRI|CMD_LIST_RWCD|CMD_SIM, CMDTL, "local user table", MIB_PROF_NAME_PASS},

# if ( PDB == FYES )
	{"pdb", cmdPdb, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "policy database config", MIB_PROF_PDB},
# endif

#if ( WEB_SERVER == FYES )
        {"web", cmdWeb, CMD_N_RWCD, CMDTL, "web config", MIB_PROF_WEB},
#endif
#endif
#if 0
#if (OBJECT_GROUP == FYES)	/* add by brwang*/
	{"ipgroup", cmdIpGroup, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "ip group config", MIB_PROF_IP_GROUP},
	{"servicegroup", cmdPortGroup, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "services group config", MIB_PROF_PORT_GROUP},
#endif
#endif

   CMDEND};
    
	/* disable function of vpn by flag */

#if ( CBQ == FYES )
    if(!HFR_ENABLED (DEFAULT_FSET, HFR_CBQ)) DISABLE_ITEM("cbq");
#endif /* CBQ == FYES */

#if (IP_SEC==FYES)
    if ( !HFR_ENABLED(DEFAULT_FSET, HFR_IPSEC) ) {
	DISABLE_ITEM("ipsec");
    	DISABLE_ITEM("isakmp");
    	DISABLE_ITEM("crypto");
    }
#endif

#if ( FRAME_RELAY == FYES )
    if( getDaughterBoardType() == DAUGHTER_BOARD_TYPE_NONE ) DISABLE_ITEM("framerelay");
#endif

    INITBRANCH; 

#if 0
    DOWRITEFLASH
    { 
        _printf(term, "Saving configuration...\r\n");

	cfgWritten = FALSE;
	baeepromChanged(0, 0x41737401);
	SemaphoreWait(writeFlashSem);
	if(cfgWritten)
            _printf(term, "[OK]\r\n");
	else {
            addErrorStatusLog( TRUE, ERROR_NVRAM_CLEARED );
            _printf(term, "[Write Error]\r\n");
	}
    }
#endif
#if 0
    DOHANGUP
    {
    	char *really ;
	int cnt;
	
	cnt = 0;

    	termSrvLocalOutput(term, "\r\nhangup all active calls, confirm ? (y/n)");
    	//really = _termSrvGetLine(term, FALSE, term->menuMode, TRUE);
    	really = termSrvGetLine(term);
    	if( tolower(*really) == 'y' ) {
#if(DOT1X == FYES)
		dot1xSuppHangAllSession();
#endif
		for(i = 1;i < MAX_INTERNET_PROFILES;i++)
		{
	    		if(wanDataSessionByNum( i, &sess ))
	    		{
				cnt ++ ;
				if ( !(cnt % 2) ) {
					rescheduleTask(500);
				}
	    			//lanPortClearCall( sess->connInfo->portNum );
	       			wanDataClearCall( sess->ifNum, DIS_USER_CALL_CLEAR_REQUEST);
            		}
		}
       		deleteDynamicString(&really);
    	} else {
       		deleteDynamicString(&really);
       		_printf(term, "hangup aborted.\r\n");
    	}
    }
#endif

    DOSAVE
    {
	saveAll = TRUE;
        cmd_ShowCfg(root, inst, term, NULL, 0, DEFAULT_RC);
    }
}

void termSrvViewIpsecCalls( TSTERM_HANDLE_TYPE     term )
{
}

void termSrvViewL2tpCalls( TSTERM_HANDLE_TYPE     term )
{
}

void termSrvViewPptpCalls( TSTERM_HANDLE_TYPE     term )
{
}

void termSrvViewPPPoECalls( TSTERM_HANDLE_TYPE     term )
{
}

void termSrvViewIkeCalls( TSTERM_HANDLE_TYPE     term )
{
}

//extern void getOneSAFromAddress(TSTERM_HANDLE_TYPE term,char *srcToDest);
extern void _tprintf( TSTERM_HANDLE_TYPE term, const char *string, ...);
extern void termSrvViewIpsecsaCalls( TSTERM_HANDLE_TYPE	  term, char *args  )
{
#if ( IP_SEC==FYES )
	
//	_tprintf(term,"=========%s==========\r\n",args);
//	showOneSA(term,args);
	
#endif
}

#if 0
void 
cmdHangAllSessions ( )
{

    const InternetProfile *prof1 = NULL;
    WanSession *sess = NULL;
    int ix;

      for ( ix = 1; ix < MAX_INTERNET_PROFILES; ++ix ) {
	prof1 = lanProfileByNumber(ix);
	if ( prof1 ) {
	  if(prof1->active && prof1->activeB) {
	      if(wanDataSessionByName2( prof1->hostName, &sess )) {
	        //lanPortClearCall( sess->connInfo->portNum );
	        wanDataClearCall( sess->ifNum, DIS_USER_CALL_CLEAR_REQUEST);
              }
	  }
	  releaseInternetProfilePointer ( prof1 );
        }
      }
}
#endif


