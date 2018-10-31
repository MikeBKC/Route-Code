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

#if ! defined( CMDLIST_H )                  /* prevent nested inclusions */
# define CMDLIST_H                          /* module name */


/*
 * SCCS ID:	$Id: cmdlist.h,v 1.2 2012/08/29 10:00:59 yu.dehai Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#define DOVERIRYCOM(x, y, z) \
    x *prof; \
    int index; \
    DOVERIFY{ \
        term->result = FAIL_CLI; \
        if(argGetInst(term->lastInput, GETCMD->level + 1, tmp) && (index = profileByNamei(GETCMD->cmdlist->profType, tmp, &prof)) != -1) {\
                term->inst[GETCMD->level] = index; \
                term->result = OK_CLI; \
		releaseProfilePointer(GETCMD->cmdlist->profType, prof);\
                return; }\
        for(i = 0;i < MAX_ROUTE_PROFILES;i++){ \
            prof = ProfileByNumber(i); \
            if(prof && prof->y) \
                _printf(term, " %s", prof->z);\
		releaseProfilePointer(GETCMD->cmdlist->profType, prof);}\
        _printf(term, "\r\nWhich %s?\r\n", GETCMD->cmdlist->name); \
	}\

#define DISABLE_ITEM(n) \
	for(__index = 0; cmdlist[__index].name; __index++) \
	    if(!strcmpi(cmdlist[__index].name, n)) {\
		cmdlist[__index].privilege |= CMD_HIDDEN; \
		break; }\

struct TermCmd;

extern TermCmd *root;

extern int first;
extern Boolean saveAll;

extern SystemProfile *sysProf;
//add by jxy8@2003-12-26 for WebUserRight
#if ( WEB_SERVER == FYES )
extern WebProfile *webConfigProf;
extern SecurityUserProfile *secUserProf;
extern SecurityGroupProfile *secGroupProf;
extern SecurityUrlProfile *secUrlProf;
#endif
//add end jxy8
extern FilterProfile *filterProf;
extern FilterEntry *filterEntry;

#if ( CBQ == FYES )
extern CbqClassProfile *cbqClassProf;
extern CbqFilterProfile *cbqFilterProf;
extern SfqQdiscProfile *sfqQdiscProf;
extern void cmdCbq(TermInfo *term);
#endif


/* ----------------------------- ENUM defines -------------------- */
/* webrom.c */
extern const LcdParamEnumValue webPageUrlsEnum[];

/* a2n.c */
#if ( PDB == FYES )
extern const LcdParamEnumValue policyEnum[];	//add by jyshen 060807
extern const LcdParamEnumValue dayEnum[];	//add by jyshen 060817
extern const LcdParamEnumValue timeEnum[];	//add by jyshen 060817
extern const LcdParamEnumValue pdbMode[];		//add by jyshen 060809
#endif //PDB
extern const LcdParamEnumValue languageEnum[];
extern const LcdParamEnumValue sanityCheckEnum[];

extern const LcdParamEnumValue ipMacRateEnum[];
extern const LcdParamEnumValue ipCreditDrrEnum[];
extern const LcdParamEnumValue ipRedDurationEnum[];

extern const LcdParamEnumValue ipCreditOverflowEnum[];
extern const LcdParamEnumValue ipCreditUnderflowEnum[];

extern const LcdParamEnumValue pdbGroupEnum[];
extern const LcdParamEnumValue filterAlgTypeEnum[];

extern const LcdParamEnumValue webFilterAlgTypeEnum[];

extern const LcdParamEnumValue ddnsTzoneTypeEnum[];

# if ( ENCRYPT_PASSWD == FYES )
extern const LcdParamEnumValue passwdTypeEnum[];
# endif

//add by jiangzhiwen@04-05-20 for User ,CBQ
extern const LcdParamEnumValue userTypeEnum[];
extern const LcdParamEnumValue arpCacheModeEnum[];

#if(CBQ==FYES)
extern const LcdParamEnumValue tqdiscEnum[];
extern const LcdParamEnumValue prototypeEnum[];
#endif

#define _switchTypeValues (LcdParamEnumValue *)switchTypeValues

//add by jxy8@2003-12-26 for WebUserRight
#if ( WEB_SERVER == FYES )
extern const LcdParamEnumValue webPageColorEnum[];
extern const LcdParamEnumValue webAccessMethEnum[];    
#endif
//add end jxy8
extern const LcdParamEnumValue enableEnum[];
extern const LcdParamEnumValue yesnoEnum[];

//add by cyh@2004-4-28
#if(IP_SEC==FYES)
extern const LcdParamEnumValue ipsecMethod[];
extern const LcdParamEnumValue ipsecMode[];
extern const LcdParamEnumValue encryptESP[];
extern const LcdParamEnumValue authESP[];
extern const LcdParamEnumValue authAH[];
extern const LcdParamEnumValue authAH_1[];
extern const LcdParamEnumValue encryptESP_1[];
extern const LcdParamEnumValue negotMode[];
extern const LcdParamEnumValue groupType[];
extern const LcdParamEnumValue authenMethod[];
#endif
//end by cyh

#if (NETKEEPER == FYES)
extern const LcdParamEnumValue       nkDialModeEnum[];
#endif

//add by jxy5@2003-11-26 for WebProfFrame
extern const LcdParamEnumValue pppoeTypeEnum[];
extern const LcdParamEnumValue natPoolEnum[];
extern const LcdParamEnumValue natMethodEnum[];
//add end jxy5

extern const LcdParamEnumValue _activationModeEnum[];
extern const LcdParamEnumValue _isdnSwitchTypeValues[];
extern const LcdParamEnumValue _natLanEnum[];

extern const LcdParamEnumValue mgmtTypeEnumValues[];
extern const LcdParamEnumValue compressionEnum[];
extern const LcdParamEnumValue callModeEnum[];
extern const LcdParamEnumValue sntpEnabledEnum[];
extern const LcdParamEnumValue OffGMTEnum[];
extern const LcdParamEnumValue facilityEnum[];
extern const LcdParamEnumValue logCallInfoEnum[];
extern const LcdParamEnumValue pppSendAuthEnum[];
extern const LcdParamEnumValue pppLqmEnum[];
extern const LcdParamEnumValue ripEnum[];
extern const LcdParamEnumValue proxyEnum[];
extern const LcdParamEnumValue routingModeEnum[];
extern const LcdParamEnumValue channelUsageEnum[];
extern const LcdParamEnumValue dialoutPoisonEnum[];
extern const LcdParamEnumValue ripPolicyEnum[];
extern const LcdParamEnumValue l2TunnelTypeEnum[];
extern const LcdParamEnumValue l2tpModeEnum[];
extern const LcdParamEnumValue l2tpTransportModeEnum[];
extern const LcdParamEnumValue protoInfo[];
extern const LcdParamEnumValue switchTypeValues[];
extern const LcdParamEnumValue filterTypeEnum[];
extern const LcdParamEnumValue eqNeqEnum[];
extern const LcdParamEnumValue testEnum[];

extern const LcdParamEnumValue etherTypeEnum[];

extern const LcdParamEnumValue ipProtocolEnum[];
extern const LcdParamEnumValue ipAddressEnum[];
extern const LcdParamEnumValue ipNetmaskEnum[];
extern const LcdParamEnumValue udpPortEnum[];
extern const LcdParamEnumValue tcpPortEnum[];

extern const LcdParamEnumValue matchtypeEnum[];
extern const LcdParamEnumValue userActionEnum[];
extern const LcdParamEnumValue userSpeedEnum[];

#if (SWITCHFUN == FYES)  /* add by zgd */
extern const LcdParamEnumValue MirrFilEnum[];
extern const LcdParamEnumValue MirrAttEnum[];
extern const LcdParamEnumValue MirrDirEnum[];
extern const LcdParamEnumValue TrunkNumEnum[];
extern const LcdParamEnumValue TrunkModEnum[];
extern const LcdParamEnumValue InspeedModEnum[];

#if (TAGVlAN == FYES)
extern const LcdParamEnumValue VlanModEnum[];
extern const LcdParamEnumValue VlanComModEnum[];
extern const LcdParamEnumValue VlanPhyNumEnum[];
extern const LcdParamEnumValue VlanTypeEnum[];
extern const LcdParamEnumValue TagVlanInEnum[];
extern const LcdParamEnumValue TagVlanOutEnum[];
#endif

extern const LcdParamEnumValue PriModEnum[];
extern const LcdParamEnumValue PriRuleEnum[];
extern const LcdParamEnumValue PriValueEnum[];
extern const LcdParamEnumValue PortMacTypeEnum[];
extern const LcdParamEnumValue PortMacPortInEnum[];

# endif //SWITCHFUN

#if(DOT1X == FYES)
extern const LcdParamEnumValue Dot1xAuthSchemeEnum[];
extern const LcdParamEnumValue Dot1xPortMethodEnum[];
extern const LcdParamEnumValue Dot1xPortControlEnum[];
extern const LcdParamEnumValue Dot1xPortStatusEnum[];
extern const LcdParamEnumValue Dot1xSuppPAEStateEnum[] ;
extern const LcdParamEnumValue Dot1xAuthenticMethod[];
extern const LcdParamEnumValue Dot1xSessionTerminateCause[] ;
extern const LcdParamEnumValue Dot1xSuppBackendStateEnum[];
extern const LcdParamEnumValue Dot1xAuthBackendAuthStateEnum[];
#endif

#if(DHCP_PLUS==FYES)
extern const LcdParamEnumValue dhcpCastEnum[];
extern const LcdParamEnumValue RelayOptEnum[];
extern const LcdParamEnumValue RelayPolicyEnum[];
extern const LcdParamEnumValue OVERLOAD_FLAGSEnum[];
extern const LcdParamEnumValue RawOptPortEnum[];
extern const LcdParamEnumValue NBTTypeEnum[];
# endif

# if ( L2TP == FYES )
extern const LcdParamEnumValue pptpModeEnum[];
extern const LcdParamEnumValue tunnelTypeEnum[];
extern const LcdParamEnumValue tunnelProtocolEnum[];
# endif 

extern const LcdParamEnumValue hbeatModeEnum[];
extern const LcdParamEnumValue hbeatIcmpEnum[];
extern const LcdParamEnumValue wirespeedtypeEnum[];
extern const LcdParamEnumValue rtPolicyEnum[];
extern const LcdParamEnumValue etherIntEnum[];
extern const LcdParamEnumValue ifName2ifNumEnum[];
extern const LcdParamEnumValue dataIdleEnum[];

#if RADIUS_COND
# if ( (RADIUS == FYES) || (TACACS == FYES) || (SECURID == FYES) || ( DEFENDER == FYES ) )
extern const LcdParamEnumValue        radiusRefreshTypeEnum[];
extern const LcdParamEnumValue        authTypeEnum[];
extern const LcdParamEnumValue        lpfTypeEnum[];
extern const LcdParamEnumValue        acctTypeEnum[];
extern const LcdParamEnumValue        radKeepUserNameEnum[];
extern const LcdParamEnumValue        acctIDBaseEnum[];
extern const LcdParamEnumValue        radCompatEnum[];
extern const LcdParamEnumValue        radServAttrParams[];
# endif
# endif

#if ( FRAME_RELAY == FYES )
extern const LcdParamEnumValue                frNailedModeEnum[];
extern const LcdParamEnumValue                frTypeEnum[];
extern const LcdParamEnumValue                switchedServiceEnum[];
extern const LcdParamEnumValue                frLinkMgmtEnum[];
extern const LcdParamEnumValue                priNumberTypeEnum[];
# endif

#if ( X25B == FYES )
extern const LcdParamEnumValue                pktOptionsEnum[];
extern const LcdParamEnumValue                pktSizeNegoEnum[];
extern const LcdParamEnumValue                nailedModeEnum[];
extern const LcdParamEnumValue                priNumberTypeEnum[];
extern const LcdParamEnumValue                pktDTEFlagEnum[];
extern const LcdParamEnumValue                pktNetTypeEnum[];
extern const LcdParamEnumValue                frmDiscModeEnum[];
extern const LcdParamEnumValue                frmSeqNumberEnum[];
extern const LcdParamEnumValue                x25EncapsInfo[];
extern const LcdParamEnumValue                x25CallModeInfo[];
# endif

# if ( IP_SEC == FYES )
extern const LcdParamEnumValue ipsecMethod[];
extern const LcdParamEnumValue ipsecMode[];
extern const LcdParamEnumValue encryptESP[];
extern const LcdParamEnumValue authESP[];
extern const LcdParamEnumValue authAH[];
extern const LcdParamEnumValue authAH_1[];
extern const LcdParamEnumValue encryptESP_1[];
extern const LcdParamEnumValue negotMode[];
extern const LcdParamEnumValue groupType[];
extern const LcdParamEnumValue authenMethod[];
extern const LcdParamEnumValue noyesEnum[];
# endif

extern const LcdParamEnumValue ethFlowCtrlTypeEnum[];
extern const LcdParamEnumValue natDipFollowEnum[];

extern const LcdParamEnumValue ispType[];
extern const LcdParamEnumValue pdbType[];

/* ----------------------------- ENUMINT nefines -------------------- */

extern const LcdParamEnumIntValue ipNatPortRangeEnumInt;
extern const LcdParamEnumIntValue tcpTimeoutEnumInt;
extern const LcdParamEnumIntValue sysReloadEnumInt;

extern const LcdParamEnumIntValue idleLogoutEnumInt; 
extern const LcdParamEnumIntValue percentEnumInt; 
extern const LcdParamEnumIntValue etherSpeedEnumInt; 

extern const LcdParamEnumIntValue ipClassCEnumInt; 
extern const LcdParamEnumIntValue ipClassBEnumInt; 

extern const LcdParamEnumIntValue rtKeepAliveEnumInt; 
extern const LcdParamEnumIntValue rtKeepRetryEnumInt; 
extern const LcdParamEnumIntValue rtMetricEnumInt; 
extern const LcdParamEnumIntValue rtPrefEnumInt; 

extern const LcdParamEnumIntValue pppMruEnumInt; 
extern const LcdParamEnumIntValue pppLqmEnumInt; 

extern const LcdParamEnumIntValue ipProtocolEnumInt; 
extern const LcdParamEnumIntValue ipPortEnumInt; 
extern const LcdParamEnumIntValue tcpPortEnumInt; 
extern const LcdParamEnumIntValue udpPortEnumInt; 

extern const LcdParamEnumIntValue etherMtuEnumInt; 
extern const LcdParamEnumIntValue tunnelMtuEnumInt; 

extern const LcdParamEnumIntValue etherVlanTagEnumInt; 
extern const LcdParamEnumIntValue etherVlanGroupEnumInt; 

extern const LcdParamEnumIntValue etherTypeEnumInt; 

extern const LcdParamEnumIntValue dnsListEnumInt;

extern const LcdParamEnumIntValue filterGenOffsetEnumInt;
extern const LcdParamEnumIntValue filterGenLengthEnumInt;

extern const LcdParamEnumIntValue trunkGroupDigsEnumInt;

extern const LcdParamEnumValue nailedEnum[];
extern const LcdParamEnumValue tmWindowEnum[];/* add by chenyunhua*/
#if (PPPOE_SRV == FYES)		/* added by llliu */
extern const LcdParamEnumValue macFilterEnum[];
extern const LcdParamEnumValue bindPortEnum[];
extern const LcdParamEnumValue pppoeAccModeEnum[];
#endif 
#if 0
#if (OBJECT_GROUP == FYES)	/* add by brwang */
extern const LcdParamEnumValue         ipgroupEnum[];
extern const LcdParamEnumValue         web_ipgroupEnum[];
extern const LcdParamEnumValue         servicesEnum[];
extern const LcdParamEnumValue         web_servicesEnum[];
extern const LcdParamEnumValue         oldOrnewFilter[];
#endif
#endif
/*add by brwang */
extern const LcdParamEnumValue etherFilterTypeEnum[]; 
extern const LcdParamEnumValue pdbCategoryEnum[]; 


/*
 * added by llliu@080428.
 */
extern const LcdParamEnumValue filterPolicyEnum[];
extern const LcdParamEnumValue fcRedShiftEnum[];


#if (FC == FYES)
extern const LcdParamEnumValue fcClassEnum[];
extern const LcdParamEnumValue fbmPriorityEnum[];

#endif
//added by dhyu
#if (NOTICE == FYES)
extern const LcdParamEnumValue UrlOrHtmlEnum[];
extern const LcdParamEnumValue AutoOrManualEnum[];
#endif


/* ------------------------ END OF ENUMINT defines ----------------- */

/* ----------------------------- CLI CMD defines -------------------- */
 /* hbeat.c */
extern void termSrvViewHbeatStat ( TSTERM_HANDLE_TYPE term ); 

 /* filter.c */
extern void termSrvViewFilterStatus( TSTERM_HANDLE_TYPE term,  char *args);

/* nat*.c */
extern void termSrvViewNatStatistics( TSTERM_HANDLE_TYPE     term );
extern void termSrvClearNatStatistics( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewNatSession( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewNatAdmin( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewNatTrans( TSTERM_HANDLE_TYPE     term, char *arg );
extern void termSrvViewNatUpnp( TSTERM_HANDLE_TYPE     term, char *arg );
extern void termSrvViewNatFilter( TSTERM_HANDLE_TYPE term, char *args );
extern void termSrvViewNatStatus( TSTERM_HANDLE_TYPE     term );

extern void termSrvViewIpsecCalls( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewL2tpCalls( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewPptpCalls( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewPPPoECalls( TSTERM_HANDLE_TYPE     term );

extern void termSrvViewIkeCalls( TSTERM_HANDLE_TYPE     term );
extern void termSrvViewIpsecsaCalls(TSTERM_HANDLE_TYPE     term ,char *arg);


/* nbnt.c */
extern void termSrvViewIpNetbiosName(TSTERM_HANDLE_TYPE     term);
extern void termSrvClearIpNetbiosName(TSTERM_HANDLE_TYPE     term);
extern void termSrvViewNetbiosStat ( TSTERM_HANDLE_TYPE term );    

/* termsrv.c */
extern void termSrvRevision( TSTERM_HANDLE_TYPE term );
extern void termSrvUpTime( TSTERM_HANDLE_TYPE           term );
extern void termSrvDateTime( TSTERM_HANDLE_TYPE        term );

/* dhcp*.c */
#if (DHCP_PLUS == FYES)
extern void termDhcpClientStatics( TSTERM_HANDLE_TYPE term );
extern void termDhcpClientStatus( TSTERM_HANDLE_TYPE term );
extern void termDhcpServerStatics( TSTERM_HANDLE_TYPE term ) ;
extern void termDhcpServerStatus(TSTERM_HANDLE_TYPE term);
extern void termDhcpRelayStatistics(TSTERM_HANDLE_TYPE term );
extern void termClearDHCPSvrStatistics ( TSTERM_HANDLE_TYPE term );
extern void termClearDHCPClientStatistics ( TSTERM_HANDLE_TYPE term );
extern void termClearDHCPRelayStatistics ( TSTERM_HANDLE_TYPE term );
#endif


#if (FC == FYES)
extern void termSrvViewFcBand(TSTERM_HANDLE_TYPE term, char *args);
extern void termSrvClearFcInfo(TSTERM_HANDLE_TYPE term);
extern void termSrvViewREDStatus(TSTERM_HANDLE_TYPE term, char *args);
extern void termSrvViewREDDropRatio(TSTERM_HANDLE_TYPE term, char *args);

#else
/* bridge.c */
extern void termSrvViewBridgeInfo ( TSTERM_HANDLE_TYPE term ) ;
extern void termSrvClearBridgeInfo ( TSTERM_HANDLE_TYPE term ) ;

extern void termSrvViewBridgeRED ( TSTERM_HANDLE_TYPE term ) ;
extern void termSrvClearBridgeRED ( TSTERM_HANDLE_TYPE term ) ;

extern void termSrvViewBridgeBand( TSTERM_HANDLE_TYPE     term );

extern void termSrvViewBridgeCreditStatus( TSTERM_HANDLE_TYPE term );
extern void termSrvClearBridgeCreditStatus( TSTERM_HANDLE_TYPE term, char *args );

extern void termSrvViewBridgeCreditBand( TSTERM_HANDLE_TYPE term );

#endif


/* brouter.c */
void termSrvViewSyslog ( TSTERM_HANDLE_TYPE term ); 	/* brouter.c */


    /*
     * The names of fields we look for when building menus that do
     * not contain frame relay options.
     */
# if ( FRAME_RELAY == FYES )
#  define FR_OPTION             "FR"
# endif

# if ( XUANLIN_ALG == FYES )
extern void termSrvViewXLSession ( TSTERM_HANDLE_TYPE term );
# endif /* XUANLIN_ALG */

# if (PPPOE_SRV == FYES)
extern void termSrvViewPPPoEServerSession(TSTERM_HANDLE_TYPE term, char *args);
extern void termSrvViewMacFilter(TSTERM_HANDLE_TYPE term, char *args);
extern void termSrvViewPPPoEServerAccount(TSTERM_HANDLE_TYPE term, char *args);
# endif
extern void termSrvViewPPPoEEvent(TSTERM_HANDLE_TYPE term, char *args);
extern void cmdDispConfig(TermInfo *term);


#if (POLICY_ROUTE == FYES)
extern void termSrvViewIpPrs(TSTERM_HANDLE_TYPE term, char *args);

#endif

#endif /* CMDLIST_H */                  /* module name */



