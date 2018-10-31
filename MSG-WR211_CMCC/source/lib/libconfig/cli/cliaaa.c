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
 * SCCS ID:	$Id: cliaaa.c,v 1.2 2012/08/29 10:00:59 yu.dehai Exp $
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#include "typedef.h"
#include "argcmd.h"
#include "profiles.h"
#include "stdio.h"
#include "cmd.h"



#include "cmdlist.h"

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang*/
const LcdParamEnumValue         oldOrnewFilter[] = {
    { "classic",        OLD_FILTER },
    { "advanced",       NEW_FILTER },
    { NULL,             0 }
};

#endif 
#endif

/* zhz@20020809, added for aaa */
#if ( RADIUS == FYES )

void cmdAaaAuth(TermInfo *term)
{
    static CmdList cmdlist[] = {
    	{ "protocol", P(slotProf->cardType.etherSlot.authType), ENUM, (Pointer)authTypeEnum },
    	{ "encaps", P(slotProf->cardType.etherSlot.ext.authRadCompat), ENUM, (Pointer)radCompatEnum},
    	{ "authKey", P(slotProf->cardType.etherSlot.authKey), STR, 0},
    	{ "srvPort",  P(slotProf->cardType.etherSlot.authPort), INT, 0},
    	{ "srcPort", P(slotProf->cardType.etherSlot.authSrcPort), INT, 0},
    	{ "timeout", P(slotProf->cardType.etherSlot.authTimeout), INT, 0},
    	{ "authFlow", P(slotProf->cardType.etherSlot.localProfileFirst), ENUM, (Pointer)lpfTypeEnum},
    	{ "rotateTime", P(slotProf->cardType.etherSlot.ext.authResetTime), INT, 0},
	{ "realmsUsers", P(slotProf->cardType.etherSlot.ext.authRadKeepUserName), ENUM, (Pointer)radKeepUserNameEnum},
	{ "delimitersOfRealms", P(slotProf->cardType.etherSlot.ext.authRadRealmDelimiters), STR, 0},
	{ "1stServer", P(slotProf->cardType.etherSlot.authHost), IPADDR, 0},
	{ "2ndServer", P(slotProf->cardType.etherSlot.authHost2), IPADDR, 0},
	{ "3rdServer", P(slotProf->cardType.etherSlot.authHost3), IPADDR, 0},
   	CMDEND};

    INITBRANCHB;
}

void cmdAaaAcct(TermInfo *term)
{
    static CmdList cmdlist[] = {
    { "protocol", P(slotProf->cardType.etherSlot.acctType), ENUM, (Pointer)acctTypeEnum},
    { "encaps", P(slotProf->cardType.etherSlot.ext.acctRadCompat), ENUM, (Pointer)radCompatEnum},
    { "acctkey", P(slotProf->cardType.etherSlot.acctKey), STR, 0},
    { "srvPort", P(slotProf->cardType.etherSlot.acctPort), INT, 0},
    { "srcPort", P(slotProf->cardType.etherSlot.acctSrcPort), INT, 0},
    { "timeout", P(slotProf->cardType.etherSlot.acctTimeout), INT, 0},
    { "rollback", P(slotProf->cardType.etherSlot.ext.acctResetTime), INT, 0},
    { "interval", P(slotProf->cardType.etherSlot.ext.acctCheckpoint), INT,0},
    { "acctFailedcall", P(slotProf->cardType.etherSlot.ext.acctStopOnly), ENUM, (Pointer)yesnoEnum},
    { "retryTimes", P(slotProf->cardType.etherSlot.ext.acctRetryLimit), INT, 0},
    { "1stServer", P(slotProf->cardType.etherSlot.acctHost), IPADDR, 0},
    { "2ndServer", P(slotProf->cardType.etherSlot.acctHost2), IPADDR, 0},
    { "3rdServer", P(slotProf->cardType.etherSlot.acctHost3), IPADDR, 0},
    CMDEND};

    INITBRANCHB;
}

#if (RADIUS_ACCT_CALL_LOG == FYES )
void cmdAaaAudit(TermInfo *term)
{
    static CmdList cmdlist[] = {
    	{ "enabled", P(slotProf->cardType.etherSlot.ext.callLogEnable), ENUM, (Pointer)yesnoEnum },
    	{ "encaps", P(slotProf->cardType.etherSlot.ext.callLogRadCompat), ENUM, (Pointer)radCompatEnum},
    	{ "auditkey", P(slotProf->cardType.etherSlot.ext.callLogKey), STR, 0},
    	{ "srvPort", P(slotProf->cardType.etherSlot.ext.callLogPort), INT, 0},
    //	{ "srcport", P(slotProf->cardType.etherSlot.acctSrcPort), INT, 0},
    	{ "timeout", P(slotProf->cardType.etherSlot.ext.callLogTimeout), INT, 0},
    	{ "rollback", P(slotProf->cardType.etherSlot.ext.callLogResetTime), INT, 0},
   // 	{ "interval", P(slotProf->cardType.etherSlot.ext.acctCheckpoint), INT,0},
    	{ "auditfailedcall", P(slotProf->cardType.etherSlot.ext.callLogStopOnly), ENUM, (Pointer)yesnoEnum},
    	{ "retrytimes",	P(slotProf->cardType.etherSlot.ext.callLogRetryLimit), INT, 0},
    	{ "1stAuditServer", P(slotProf->cardType.etherSlot.ext.callLogHost1), IPADDR, 0},
    	{ "2ntAuditServer", P(slotProf->cardType.etherSlot.ext.callLogHost2), IPADDR, 0},
    	{ "3rdAuditServer", P(slotProf->cardType.etherSlot.ext.callLogHost3), IPADDR, 0},
    	CMDEND};

    INITBRANCHB;
}
#endif /* (RADIUS_ACCT_CALL_LOG == FYES ) */

#if (RADIUS_SERV == FYES )
void cmdAaaRadiusServer(TermInfo *term)
{
    static CmdList cmdlist[] = {
    	{ "enabled",P(slotProf->cardType.etherSlot.radServEnable), ENUM, (Pointer)yesnoEnum},
    	{ "srvPort",P(slotProf->cardType.etherSlot.radServPort), INT, 0},
    	{ "encaps",P(slotProf->cardType.etherSlot.ext.radServRadCompat), ENUM, (Pointer)radCompatEnum},
    	{ "sessionKeyMatch", P(slotProf->cardType.etherSlot.radServSessionKeyEnable), ENUM, (Pointer)yesnoEnum},
    	{ "attributesMatch",P(slotProf->cardType.etherSlot.radServAttrType), ENUM, (Pointer)radServAttrParams}, 
    	{ "1stClient", P(slotProf->cardType.etherSlot.radServClient[0]), IPADDR, 0},
    	{ "1stNetmask", P(slotProf->cardType.etherSlot.radServNetmask[0]), IPADDR, 0},
    	{ "1stKey", P(slotProf->cardType.etherSlot.radServKey[0]), STR, 0},
    	{ "2Client", P(slotProf->cardType.etherSlot.radServClient[1]), IPADDR, 0},
    	{ "2Netmask", P(slotProf->cardType.etherSlot.radServNetmask[1]), IPADDR, 0},
    	{ "2key", P(slotProf->cardType.etherSlot.radServKey[1]), STR, 0},
    	{ "3Client", P(slotProf->cardType.etherSlot.radServClient[2]), IPADDR, 0},
    	{ "3Netmask", P(slotProf->cardType.etherSlot.radServNetmask[2]), IPADDR, 0},
    	{ "3Key", P(slotProf->cardType.etherSlot.radServKey[2]), STR, 0},
    	CMDEND};

    INITBRANCHB;
}
#endif /* RADIUS_SERV == FYES */

void cmdAaa(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"enabled", P(slotProf->cardType.etherSlot.pwsEnabled), ENUM, (Pointer)yesnoEnum},
			{"refreshConfig", P(slotProf->cardType.etherSlot.authLoadConfig), ENUM, (Pointer)radiusRefreshTypeEnum},
			{"auth", cmdAaaAuth, CMD_LEAFDAD},
			{"acct", cmdAaaAcct, CMD_LEAFDAD},
#if (RADIUS_ACCT_CALL_LOG == FYES )
			{"audit", cmdAaaAudit, CMD_LEAFDAD},
#endif /* (RADIUS_ACCT_CALL_LOG == FYES ) */
#if (RADIUS_SERV == FYES )
			{"radiusServer", cmdAaaRadiusServer, CMD_LEAFDAD},
#endif /* RADIUS_SERV == FYES */
			{"config", cmdDispConfig, CMD_LEAFVST},
			{"statistics", F(termSrvViewIpStatistics), CMD_ARG0},
			CMDEND};

    //INITBRANCH;
    INITBRANCHES;
}

#endif /* RADIUS == FYES */

#if ( FRAME_RELAY == FYES )

FrameRelayProfile *frProf = 0;

void cmdFR(TermInfo *term)
{
    static CmdList cmdlist[] = {
	{"config", cmdDispConfig, CMD_LEAFVST},
	{ "name", P(frProf->name), STR, 0},
	{ "enabled", P(frProf->active), ENUM, yesnoEnum},
	{ "calltype", P(frProf->nailedMode),ENUM, frNailedModeEnum},
    	{ "singallingtype", P(frProf->frType), ENUM, frTypeEnum},
	{ "group", P(frProf->nailedUpGroup), GROUP, 0},
	{ "datasvc", P(frProf->switchedCallType), ENUM, _switchTypeValues},
    	{ "nmsdlci", P(frProf->linkStatusDlci), INT, 0},
	{ "lmitype", P(frProf->linkMgmt), ENUM, frLinkMgmtEnum},
	{ "nni391FSRtimer", P(frProf->n391Val), INT, 0},
	{ "dcen392NMerrors", P(frProf->dceN392Val), INT, 0},
	{ "dcen393NMevents", P(frProf->dceN393Val), INT, 0},
	{ "dten392NMerrors", P(frProf->n392Val), INT, 0},
	{ "dten393NMevents", P(frProf->n393Val), INT, 0},
	{ "t391LIVtimer", P(frProf->t391Val), INT, 0},
	{ "t392SEMtimer", P(frProf->t392Val), INT, 0},
	{ "mru", P(frProf->n201), INT,0},
	{"statistics", F(termSrvViewFrrlyStatistics), CMD_ARG0},
	{"lmi", F(termSrvViewFrLMIStatistics), CMD_ARG0},
	{"dlci", F(termSrvViewFrDlciStatistics), CMD_ARG1},
//	{"links", F(termSrvViewFrDlciCircuits), CMD_ARG1},
	CMDEND};

    INITBRANCHB;
}
void cmdConnEncapsFR(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"frprofile", P(inetProf->fr.frameRelayProfile), STR, 0},
			{"dlci", P(inetProf->fr.dlci), INT, 0},
			CMDEND};

    INITBRANCH;
}


#endif /* FRAME_RELAY */

#if ( X25B == FYES )

extern void termSrvViewX25( TSTERM_HANDLE_TYPE term ); /* in rsysutil.c */
extern void termSrvViewPad( TSTERM_HANDLE_TYPE term ); /* in tspad.c */
extern void termSrvViewIpoX25( TSTERM_HANDLE_TYPE term );  /* in ipox25.c */


X25Profile *x25Prof = 0 ;

void cmdX25Config(TermInfo *term)
{
    static CmdList cmdlist[] = {
    			{ "name", P(x25Prof->x25Name), STR, 0},
    			{ "enabled", P(x25Prof->active), ENUM, yesnoEnum},
    			{ "calltype", P(x25Prof->nailedMode), ENUM, _nailedModeEnum},
    			{ "group", P(x25Prof->nailedUpGroup), GROUP, 0},
    			{ "datasvc", P(x25Prof->switchedCallType), ENUM, _switchTypeValues},
# if ( X25_DCHAN == FYES )
			{ "tei#", P(x25Prof->tei), INT, 0},
# endif
    			{ "dial#", P(x25Prof->phoneNumber),STRNUM, 0},
    			{ "lapbT1", P(x25Prof->T1Val), INT, 0},
    			{ "lapbT2", P(x25Prof->T2Val), INT, 0},
    			{ "lapbN2", P(x25Prof->N2Val), INT, 0},
    			{ "lapbk", P(x25Prof->kVal), INT, 0},
    			{ "seqmode", P(x25Prof->frmSeqNumber), ENUM, _frmSeqNumberEnum},
    			{ "setupmode", P(x25Prof->frmDiscMode), ENUM, _frmDiscModeEnum},
    			{ "nodetype",  P(x25Prof->pktDteFlag), ENUM, _pktDTEFlagEnum},
    			{ "window", P(x25Prof->pktWindow), INT, 0},
    			{ "pktsize", P(x25Prof->pktSize), INT, 0},
    			{ "minpkt",  P(x25Prof->minPktSize), INT, 0},
    			{ "maxpkt", P(x25Prof->maxPktSize), INT, 0},
    			{ "minpvc", P(x25Prof->lowestPVC), INT, 0},
    			{ "maxpvc", P(x25Prof->highestPVC), INT, 0},
    			{ "minsvc", P(x25Prof->lowestSVC), INT, 0},
    			{ "maxsvc", P(x25Prof->highestSVC), INT, 0},
    			{ "srcX121addr", P(x25Prof->pktX121SrcAddr), STRNUM, 0},
    			CMDEND};

    INITBRANCHB;
}

void cmdX25Options(TermInfo *term)
{
    static CmdList cmdlist[] = {
    			{ "networktype", P(x25Prof->pktNetType), ENUM, _pktNetTypeEnum},
    			{ "vcsetuptimer", P(x25Prof->padVceTmrVal), INT, 0},
    			{ "allowoptions", P(x25Prof->pktOptions), ENUM, _pktOptionsEnum,},
    			{ "reversecharge", P(x25Prof->pktRevChargeAccept), ENUM, yesnoEnum},
    			{ "timerT20", P(x25Prof->pktT20), INT, 0},
    			{ "timerR20", P(x25Prof->pktR20), INT, 0},
    			{ "timerT21", P(x25Prof->pktT21), INT, 0},
    			{ "timerT22", P(x25Prof->pktT22), INT, 0},
    			{ "timerR22", P(x25Prof->pktR22), INT, 0},
    			{ "timerT23", P(x25Prof->pktT23), INT, 0},
    			{ "timerR23", P(x25Prof->pktR23), INT, 0},
    			{ "diagclear", P(x25Prof->clearDiag), ENUM, yesnoEnum},
    			{ "diagreset", P(x25Prof->resetDiag), ENUM, yesnoEnum},
    			{ "diagrestart", P(x25Prof->restartDiag), ENUM, yesnoEnum},   
    			CMDEND};
   INITBRANCHB;
}

#endif /* X25B */

#if ( IPSEC == FYES )
void cmdIpsec(TermInfo *term)
{
    static CmdList cmdlist[] = {
			{"ike", cmdCbq, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "ipsec ike config", MIB_PROF_CBQ_CLASS},
			{"ah", cmdCbq, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "ip ah config", MIB_PROF_CBQ_FILTER},
			{"esp", cmdCbq, CMD_CDRI|CMD_LIST_RWCD, CMDTL, "ip esp config", MIB_PROF_SFQ_QDISC},
			CMDEND};

    INITBRANCH;
}

#endif /* IPSEC == FYES */

#define PARAM_FILTER_IPSSG 	{"timeRange", P(filterEntry->ssgFilter.tmrgName),STR,0}, \
   			{"eType", P(filterEntry->ssgFilter.eType), HEX, 0},\
   			{"eNeq", P(filterEntry->ssgFilter.eNeq),ENUM, (Pointer)eqNeqEnum},\
			{"sMac", P(filterEntry->ssgFilter.sMac),MACADDR2, 0},\
   			{"sNeq", P(filterEntry->ssgFilter.sNeq), ENUM, (Pointer)eqNeqEnum},\
   			{"dMac", P(filterEntry->ssgFilter.dMac),MACADDR2,0},\
   			{"dNeq", P(filterEntry->ssgFilter.dNeq), ENUM, (Pointer)eqNeqEnum},\
			{"srcAddr", P(filterEntry->ssgFilter.sourceAddress), IPADDR},\
			{"srcMask", P(filterEntry->ssgFilter.sourceAddressMask), IPADDR},\
			{"srcFrom", P(filterEntry->ssgFilter.sourceAddrFrom), IPADDR},\
			{"srcEnd", P(filterEntry->ssgFilter.sourceAddrTo), IPADDR},\
			{"destAddr", P(filterEntry->ssgFilter.destAddress), IPADDR},\
			{"destMask", P(filterEntry->ssgFilter.destAddressMask), IPADDR},\
			{"destFrom", P(filterEntry->ssgFilter.destAddrFrom), IPADDR},\
			{"destEnd", P(filterEntry->ssgFilter.destAddrTo), IPADDR},\
			{"protocol", P(filterEntry->ssgFilter.protocol), ENUMINT, (Pointer)&ipProtocolEnumInt},\
			{"srcPort", P(filterEntry->ssgFilter.sourcePort), ENUMINT, (Pointer)&ipPortEnumInt},\
			{"srcPortCmp", P(filterEntry->ssgFilter.sourceTest), ENUM, (Pointer)testEnum},\
			{"sportFrom", P(filterEntry->ssgFilter.sourcePortFrom), ENUMINT, (Pointer)&ipPortEnumInt},\
			{"sportEnd", P(filterEntry->ssgFilter.sourcePortTo), ENUMINT, (Pointer)&ipPortEnumInt},\
			{"destPort", P(filterEntry->ssgFilter.destPort), ENUMINT, (Pointer)&ipPortEnumInt},\
			{"destPortCmp", P(filterEntry->ssgFilter.destTest), ENUM, (Pointer)testEnum},\
			{"dportFrom", P(filterEntry->ssgFilter.destPortFrom), ENUMINT,(Pointer)&ipPortEnumInt}, \
			{"dportEnd", P(filterEntry->ssgFilter.destPortTo), ENUMINT, (Pointer)&ipPortEnumInt}, \
			{"tcpEstab", P(filterEntry->ssgFilter.tcpEstab), ENUM, (Pointer)yesnoEnum}, \
			{"l7filter", P(filterEntry->ssgFilter.fAlgType), ENUM, (Pointer)filterAlgTypeEnum}, \
			{"l7Profile", P(filterEntry->ssgFilter.keyProfile), STR, 0}, \
			{"l7KeyCmp", P(filterEntry->ssgFilter.keyTest), ENUM, (Pointer)testEnum},\
    			{"l7Key", P(filterEntry->ssgFilter.keyName), PASSWD, 0}		/* last entry no , */

#if 0
void cmdFilterInIpssg(TermInfo *term)
{
    static CmdList cmdlist[] = {
			PARAM_FILTER_IPSSG,
#if (OBJECT_GROUP == FYES)	
			{"mode", P(filterEntry->ssgFilter.oldOrnew), ENUM, (Pointer)oldOrnewFilter},
                        {"serviceGroup",P(filterEntry->ssgFilter.portname),STR,0},
                        {"srcIpGroup",P(filterEntry->ssgFilter.srcipname),STR,0},
			{"destIpGroup",P(filterEntry->ssgFilter.destipname),STR,0},
#endif
			CMDEND};
    
    INITBRANCH;
}
void cmdFilterOutIpssg(TermInfo *term)
{
    static CmdList cmdlist[] = {
			PARAM_FILTER_IPSSG,
#if (OBJECT_GROUP == FYES)	
			{"mode", P(filterEntry->ssgFilter.oldOrnew), ENUM, (Pointer)oldOrnewFilter},
                        {"serviceGroup",P(filterEntry->ssgFilter.portname),STR,0},
                        {"srcIpGroup",P(filterEntry->ssgFilter.srcipname),STR,0},
			{"destIpGroup",P(filterEntry->ssgFilter.destipname),STR,0},
#endif

			CMDEND};
    
    INITBRANCH;
}

#endif
