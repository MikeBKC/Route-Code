    /*
     * Notice: file generated by MIBMAKE from mibinipx.mdf
     * DO NOT EDIT THIS FILE!!!  Edit mibinipx.mdf
     */
#line 3 "mibinipx.mdf"
/*
 * $Id: mibinipx.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *      Copyright (c) 1996 UTT Technologies, Inc.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of UTT Technologies, Inc.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise duplicated
 *      and the information herein may not be used, disseminated or
 *      otherwise disclosed, except with the prior written consent of
 *      UTT Technologies, Inc.
 */

#line 32 "mibinipx.mdf"
#if ! defined( VERSION_H )
#endif

#if (( ! defined( MIBINIPX_H ) ) && ( MDF_IPX_ROUTE == FYES ))
# define MIBINIPX_H

# include "mibgen.h"
# include "mibglob.h"
# include "mibipxfl.h"

    /*
     * Maximum number of IPX SAP proxy HOME servers supported.
     */
# define MAX_SAP_PROXY_HS	6

    /*
     * How to build the factory instance of this profile.
     */
extern ConstPtr
mibFactoryIpxOptions( MibProfileType	mibType,
		      Uint32		info );

    /*
     * Our parameter table is made external as the profile is used
     * within other profiles.
     */
extern const MibFieldComplexParams	ipxOptionsParams;


    /*
     * IpxPeerMode:
     *
     * IPX Router Peer Mode:
     *
     *  IPX_PEER_ROUTER Route to this connection.
     *
     *  IPX_PEER_DIALIN No routing, use our Pool Net number.
     */
typedef enum IpxPeerMode {
#line 91 "mibinipx.mdf"
    IPX_PEER_ROUTER,
#line 94 "mibinipx.mdf"
    IPX_PEER_DIALIN
} IpxPeerMode;


static __inline__ Bool
validIpxPeerMode (int x)
{
    switch (x) {
#line 91 "mibinipx.mdf"
    case IPX_PEER_ROUTER:
#line 94 "mibinipx.mdf"
    case IPX_PEER_DIALIN:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * IpxRipMode:
     *
     * IPX RIP Mode:
     *
     *  IPX_RIP_BOTH Send and Receive IPX RIP.
     *
     *  IPX_RIP_SEND Only Send IPX RIP.
     *
     *  IPX_RIP_RECV Only Recv IPX RIP.
     *
     *  IPX_RIP_OFF Don't Send and Receive IPX RIP.
     */
typedef enum IpxRipMode {
#line 118 "mibinipx.mdf"
    IPX_RIP_BOTH,
#line 121 "mibinipx.mdf"
    IPX_RIP_SEND,
#line 124 "mibinipx.mdf"
    IPX_RIP_RECV,
#line 127 "mibinipx.mdf"
    IPX_RIP_OFF
} IpxRipMode;


static __inline__ Bool
validIpxRipMode (int x)
{
    switch (x) {
#line 118 "mibinipx.mdf"
    case IPX_RIP_BOTH:
#line 121 "mibinipx.mdf"
    case IPX_RIP_SEND:
#line 124 "mibinipx.mdf"
    case IPX_RIP_RECV:
#line 127 "mibinipx.mdf"
    case IPX_RIP_OFF:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * IpxSapMode:
     *
     * IPX SAP Mode:
     *
     *  IPX_SAP_BOTH Send and Receive SAP.
     *
     *  IPX_SAP_SEND Only Send SAP.
     *
     *  IPX_SAP_RECV Only Recv SAP.
     *
     *  IPX_SAP_OFF Don't Send and Receive SAP.
     */
typedef enum IpxSapMode {
#line 151 "mibinipx.mdf"
    IPX_SAP_BOTH,
#line 154 "mibinipx.mdf"
    IPX_SAP_SEND,
#line 157 "mibinipx.mdf"
    IPX_SAP_RECV,
#line 160 "mibinipx.mdf"
    IPX_SAP_OFF
} IpxSapMode;


static __inline__ Bool
validIpxSapMode (int x)
{
    switch (x) {
#line 151 "mibinipx.mdf"
    case IPX_SAP_BOTH:
#line 154 "mibinipx.mdf"
    case IPX_SAP_SEND:
#line 157 "mibinipx.mdf"
    case IPX_SAP_RECV:
#line 160 "mibinipx.mdf"
    case IPX_SAP_OFF:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * IpxOptions:
     *
     * IPX protocol configuration options.
     *
     *  pppIpxNcp Enable/disable IPX routing for this connection.
     *
     *  fill1 filler to get to next 32 bit boundary.
     *
     *  ipxPeerMode Enable/disable full routing between peer or use the pool to
     *                                  assign a network number.
     *
     *  ipxRipMode This field specifies RIP traffic when peer is a Router.
     *
     *  ipxSapMode This field specifies SAP traffic when peer is a Router.
     *
     *  ipxDialQuery Indicate if we should dial to this connection if our SAP
     *                                  table contains no NetWare file servers,
     *                                  and a workstation makes a
     *                                  get-nearest-server query.
     *
     *  ipxNetNumber The network number of the other router. May be zero, if we
     *                                  don't know what it is.
     *
     *  ipxNetAlias The network number of the point to point link.
     *
     *  sapFilter The name of the IPX SAP filter, if any, to use with this
     *                                  connection.
     *
     *  sapFilter The number of the IPX SAP Filter assigned to this interface.
     *
     *  fill2 filler to get to next 32 bit boundary.
     *
     *  ipxSpoofing Selects the IPX spoofing mode when bridging.
     *
     *  ipxNetWareTimeOut IPX NetWare connection timeout value.
     *
     *  fill3 filler to get to 32 bit boundary
     *
     *  ipxSapHsProxy Use IPX SAP Home Server Proxy.
     *
     *  ipxSapHsProxyNet The Network# for IPX SAP Proxy Home Server.
     *
     *  ipxHeaderCompression Use IPX header compression if encapsulation
     *                                  supports it.
     */
typedef struct ipxOptions {
#line 184 "mibinipx.mdf"
    Boolean                             pppIpxNcp;
#line 189 "mibinipx.mdf"
    char                                fill1[ 3 ];
#line 195 "mibinipx.mdf"
    IpxPeerMode                         ipxPeerMode;
#line 201 "mibinipx.mdf"
    IpxRipMode                          ipxRipMode;
#line 206 "mibinipx.mdf"
    IpxSapMode                          ipxSapMode;
#line 211 "mibinipx.mdf"
    Boolean                             ipxDialQuery;
#line 218 "mibinipx.mdf"
    Uint32                              ipxNetNumber;
#line 224 "mibinipx.mdf"
    Uint32                              ipxNetAlias;
#line 229 "mibinipx.mdf"
#if ( PLATFORM & TNT )
    char                                sapFilter[ MAX_IPXSAP_FILTER_NAME_LENGTH + 1 ];
#endif /* ( PLATFORM & TNT ) */
#line 236 "mibinipx.mdf"
#if ( ! ( PLATFORM & TNT ))
    Uint16                              sapFilter;
#endif /* ( ! ( PLATFORM & TNT )) */
#line 242 "mibinipx.mdf"
#if ( ! ( PLATFORM & TNT ))
    Uint16                              fill2;
#endif /* ( ! ( PLATFORM & TNT )) */
#line 249 "mibinipx.mdf"
#if (( MDF_IPX_SPOOF == FYES ) & ( PLATFORM & TNT ))
    IpxSpoofMode                        ipxSpoofing;
#endif /* (( MDF_IPX_SPOOF == FYES ) & ( PLATFORM & TNT )) */
#line 255 "mibinipx.mdf"
#if (( MDF_IPX_SPOOF == FYES ) & ( PLATFORM & TNT ))
    Uint16                              ipxNetWareTimeOut;
#endif /* (( MDF_IPX_SPOOF == FYES ) & ( PLATFORM & TNT )) */
#line 261 "mibinipx.mdf"
#if (( MDF_IPX_SPOOF == FYES ) & ( PLATFORM & TNT ))
    Uint16                              fill3;
#endif /* (( MDF_IPX_SPOOF == FYES ) & ( PLATFORM & TNT )) */
#line 268 "mibinipx.mdf"
    Boolean                             ipxSapHsProxy;
#line 272 "mibinipx.mdf"
    LongWord                            ipxSapHsProxyNet[ MAX_SAP_PROXY_HS ];
#line 276 "mibinipx.mdf"
#if (( MDF_IPX_HEADER_COMPRESSION == FYES ) && !(MDF_NO_IPX_HEADER_COMPRESSION == FYES))
    Boolean                             ipxHeaderCompression;
#endif /* (( MDF_IPX_HEADER_COMPRESSION == FYES ) && !(MDF_NO_IPX_HEADER_COMPRESSION == FYES)) */
}
#if ( USERIF & CLI )
__PACK__
#endif
IpxOptions;
#line 357 "mibinipx.mdf"

# endif /* (( ! defined( MIBINIPX_H ) ) && ( MDF_IPX_ROUTE == FYES )) */
