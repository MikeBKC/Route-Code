    /*
     */
/*
 *
 *      Copyright (c) 1995-1997 UTT Technologies, Inc.
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


#if ! defined( MIBIP_H )
# define MIBIP_H

# include "mibgen.h"
# include "mibaddr.h"
# include "mibslot.h"
# include "mibitems.h"
# include "mibfiltr.h"
# include "mibdnstab.h"
# include "mibbootp.h"

    /* This file contains the internal representation of profiles used
     * to define the IP-GLOBAL and IP-INTERFACE configuration.
     */

extern const char* const		mibProfIpInt;

extern const char* const		mibProfIpGlobal;


extern ConstPtr
mibFactoryIpInterfaceProfile( MibProfileType	mibType,
		              ConstPtr		mibIndex,
		              Int		mibIndexLen );

extern ConstPtr
mibFactoryIpGlobalProfile( MibProfileType	mibType,
		           ConstPtr		mibIndex,
		           Int			mibIndexLen );



    /*
     * IpInterfaceProfile:
     *
     * Configure interface-specific parameters of a logical IP interface.
     *
     *  logicalAddress Identifies the IP interface within the TNT system.
     *
     *  ipAddress The IP Address of this interface.
     *
     *  netmask The Network Mask of this interface.
     *
     *  ipAddress2 Alternate IP address of this interface.
     *
     *  netmask2 Netmask for the alternate IP address.
     *
     *  proxyMode Specify how proxy arp is to be used on an interface.
     *
     *  ripMode Specify how RIP is to be used on this interface. Options are
     *                                  off, on, ignore received routing
     *                                  updates, don't send routing updates.
     *
     *  routeFilter The name of this filter. All filters are referenced by name
     *                                  so a name must be assigned to active
     *                                  filters.
     *
     *  ripMulticast Use multicast address for sending RIP V2 packets on this
     *                                  interface .
     *
     *  ospf The user configurable settings for OSPF
     *
     *  multicastAllowed Multicast traffic allowed.
     *
     *  multicastRateLimit Multicast traffic rate limit in seconds
     *
     *  multicastGroupLeaveDelay Multicast group leave dealy in seconds
     *
     *  allowDirectedBcast allow forwarding directed broadcast traffic
     *
     *  vRouterName Specifies the VRouter in which this IP interface profile
     *                                  belongs.
     *
     *  manageUseOnly set the interface for management use only
     */
typedef struct ipInterface {
   char                                name[ MAX_PROFILE_NAME_LENGTH + 1 ];
   Boolean                             active; 

    LogicalAddress                      logicalAddress;
#if ( MDF_IP == FYES )
    IpAddress                           ipAddress;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    IpAddress                           netmask;
#endif /* ( MDF_IP == FYES ) */
#if (( MDF_IP == FYES ) && ( PLATFORM & ( MPORT | P50 | PDSL | P25 | M200 | TNT)) &&( MDF_MULTI_IP_INTERF == FNO ) )
    IpAddress                           ipAddress2;
#endif /* (( MDF_IP == FYES ) && ( PLATFORM & ( MPORT | P50 | PDSL | P25 | M200 | TNT)) &&( MDF_MULTI_IP_INTERF == FNO ) ) */
#if (( MDF_IP == FYES ) && ( PLATFORM & ( MPORT | P50 | PDSL | P25 | M200 |TNT)) && ( MDF_MULTI_IP_INTERF == FNO ) )
    IpAddress                           netmask2;
#endif /* (( MDF_IP == FYES ) && ( PLATFORM & ( MPORT | P50 | PDSL | P25 | M200 |TNT)) && ( MDF_MULTI_IP_INTERF == FNO ) ) */
#if ( MDF_IP == FYES )
    ProxyMode                           proxyMode;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    RoutingMode                         ripMode;
#endif /* ( MDF_IP == FYES ) */
    char                                routeFilter[ MAX_FILTER_NAME_LENGTH + 1 ];
#if ( MDF_IP == FYES )
    Boolean                             ripMulticast;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_OSPF == FYES )
    MibProfOspf                         ospf;
#endif /* ( MDF_OSPF == FYES ) */
#if ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES))
    Boolean                             multicastAllowed;
#endif /* ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES)) */
#if ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES))
    Uint16                              multicastRateLimit;
#endif /* ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES)) */
#if ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES))
    Uint16                              multicastGroupLeaveDelay;
#endif /* ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES)) */
#if ( MDF_IP_ROUTE == FYES )
    Boolean                             allowDirectedBcast;
#endif /* ( MDF_IP_ROUTE == FYES ) */
#if ( MDF_VROUTER == FYES )
    char                                vRouterName[ MAX_NAME_LEN + 1 ];
#endif /* ( MDF_VROUTER == FYES ) */
#if ( MDF_IP == FYES )
    Boolean                             manageUseOnly;
#endif /* ( MDF_IP == FYES ) */
#if ( IPTOS == FYES )
    MibProfIpTos                        tosOpt;
#endif /* ( IPTOS == FYES ) */
#line 301 "mibinip.mdf"
#if ( MDF_IPTOS_CLI == FYES )
    char                                tosFilter[ MAX_FILTER_NAME_LENGTH + 1 ];
#endif /* ( MDF_IPTOS_CLI == FYES ) */
#line 307 "mibinip.mdf"
#if ( MDF_IPTOS_LCD == FYES )
    Uint16                              itosFilter;
#endif /* ( MDF_IPTOS_LCD == FYES ) */
}
#if ( USERIF & CLI )
__PACK__
#endif
IpInterfaceProfile;

 	/* The field ID of the Index Field for this profile. */
# define MIB_IP_INT_INDEX_FIELD	1


    /*
     * IpGlobalProfile:
     *
     * Configure system-global parameters for TCP/IP.
     *
     *  domainName The DNS domain name assigned to this unit.
     *
     *  primaryDnsServer The IP address of the primary DNS server.
     *
     *  secondaryDnsServer The IP address of the secondary DNS server. This
     *                                  server is used when the primary is not
     *                                  available.
     *
     *  systemIpAddress System ip address for the box.
     *
     *  softIpInterfaceAddress Field is deprecated; use the 'IP-INTERFACE'
     *                                  profile {{ any-shelf any-slot 0 }} to
     *                                  specify the soft IP address.
     *
     *  primaryNetBiosNS The IP address of the primary NetBios name server.
     *
     *  secondaryNetBiosNS The IP address of the secondary NetBios name server.
     *
     *  assignOnly Flag, when set indicates that addresses assigned from the
     *                                  address pool may NOT be overridden by
     *                                  addresses received from the far end.
     *
     *  poolSummary Flag, when set indicates that host addresses assigned from
     *                                  the pool should be marked as PRIVATE in
     *                                  the routing table and summarized to the
     *                                  world at large via a (constant) network
     *                                  advertisement for the whole pool.
     *
     *  poolOspfAdvType The OSPF advertisement type to be used with pool
     *                                  summaries.
     *
     *  assignBase The base address of a pool of addresses we can use to assign
     *                                  to clients.
     *
     *  assignCount The number of host addresses in the above. pool. The
     *                                  addresses are contiguous.
     *
     *  poolName The name of this pool
     *
     *  ripPolicy An Enum describing whether to use Poison reverse or Split
     *                                  Horizon policy. Global for the box.
     *
     *  ripSummary Summarize subnets in RIP broadcasts per RFC 1058/RFC 1009.
     *
     *  ripTrigger When set to TRUE (its default value) it causes RIP to send
     *                                  triggered (incremental) updates.
     *                                  Otherwise full table updates are sent
     *                                  when a change in the routing table is
     *                                  noticed.
     *
     *  bootpEnabled TRUE if this box is to use bootp to get params and check
     *                                  for a new load.
     *
     *  bootpRelay Configuration parameters for BOOTP RELAY agent.
     *
     *  ignoreIcmpRedirects If TRUE, incoming ICMP redirects will be ignored,
     *                                  otherwise, they will be processed.
     *
     *  dropSourceRoutedIpPackets If TRUE, do not forward IP packets with the
     *                                  source route option set.
     *
     *  ignoreDefRoute A boolean. When set to TRUE (its default value) we will
     *                                  ignore default addresses advertized by
     *                                  routing protocols.
     *
     *  proxyMode Specify how proxy arp is to be used.
     *
     *  rarpEnabled TRUE if interfaces are to RARP for their IP address.
     *
     *  udpCksum TRUE means that locally originated UDP packets will include a
     *                                  UDP checksum.
     *
     *  tcpTimeout The maximum time (in seconds) to spend in TCP
     *                                  retransmissions before deciding that
     *                                  there is no connection.
     *
     *  dialoutPoison Should dialout routes be poisoned if no trunks.
     *
     *  telnetPassword The password that must be used to be able to Telnet into
     *                                  the box.
     *
     *  userProfile The name of the default user profile associated with telnet
     *                                  sessions. If none is specified, the
     *                                  user must log in.
     *
     *  sharedProf When TRUE multiple users may share a connection profile, as
     *                                  long as IP addresses are not
     *                                  duplicated.
     *
     *  dnsListAttempt Should DNS List feature be used.
     *
     *  staticPref The default preference given to static routes.
     *
     *  ripPref The default preference given to rip routes.
     *
     *  ripQueueDepth The maximum number of RIP packets waiting to be serviced.
     *                                  Additional packets will be dropped.
     *                                  When setting this value, you're trading
     *                                  memory resources versus losing RIP
     *                                  updates (only when the box is busy and
     *                                  gets behind on servicing the RIP
     *                                  packets). Zero indicates there is no
     *                                  limit.
     *
     *  ospfPref The default preference given to ospf routes.
     *
     *  ospfAsePref The default preference given to ospf ASE routes.
     *
     *  ospfGlobal The user configurable settings for OSPF that are global in
     *                                  nature.
     *
     *  ripTag The tag to associate with RIP routes.
     *
     *  ripAseType The ASE type to associate with RIP routes.
     *
     *  ipRouteCache IP cache Enabled or Disabled.
     *
     *  ipRouteCacheSize Ip Routing cache size.
     *
     *  ipPortCache IP-Port route cache Enabled or Disabled.
     *
     *  suppressHostRoute Suppress host route in the routing advertisement.
     *
     *  sntpInfo Simple Network Time Protocol client parameters.
     *
     *  dnsListSize DNS List Size.
     *
     *  clientPrimaryDnsServer Default user IP address of the primary DNS
     *                                  server.
     *
     *  clientSecondaryDnsServer Default user IP address of the secondary DNS
     *                                  server. This server is used when the
     *                                  primary is not available.
     *
     *  allowAsClientDnsInfo This flag controls if main DNS info should be
     *                                  allowed as Client DNS info.
     *
     *  localDnsTableEnabled Note: 54 is obsoleted w/ 'cond = ( 0 )'. Do not
     *                                  resue it.
     *
     *  localDnsTableAutoUpdate Note: 55 is obsoleted w/ 'cond = ( 0 )'. Do not
     *                                  resue it
     *
     *  dnsFallbackCfg Local DNS table profile, stores host names & initial IP
     *                                  addresses.
     *
     *  multicastForwarding Multicast forwarding Enabled.
     *
     *  mboneProfile MBONE Wan interface.
     *
     *  mboneLanLogicalAddress MBONE Lan interface.
     *
     *  multicastHeartBeatAddr Multicast heart beat address.
     *
     *  multicastHeartBeatPort Multicast heart beat port.
     *
     *  multicastHeartBeatSlotTime Multicast heart beat time.
     *
     *  multicastHeartBeatSlotlength Multicast heart beat slot length.
     *
     *  multicastHeartBeatAlarmThreshold Multicast heart beat alarm threshold.
     *
     *  multicastHeartBeatSrcAddr Multicast heart beat source address.
     *
     *  multicastHeartBeatSrcAddrMask Multicast heart beat source address mask.
     *
     *  secDomainName The secondary DNS domain name assigned to this unit.
     *
     *  multicastMemberTimeout Multicast membership time out in seconds
     *
     *  fingerEnable Enable remote finger.
     *
     *  icmpReplyDirectedBcast reply directed bcast ICMP requests or not.
     *
     *  vRouterName Specifies the global VRouter.
     *
     *  unusedField unused field - can be reassigned at will (it was reserved
     *                                  but never used)
     */
typedef struct ipGlobal {
   char                                name[ MAX_PROFILE_NAME_LENGTH + 1 ];
   Boolean                             active; 

#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    char                                domainName[ MAX_DOMAIN_NAME_LENGTH + 1 ];
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    IpAddress                           primaryDnsServer;
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    IpAddress                           secondaryDnsServer;
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_IP_ROUTE == FYES ))
    IpAddress                           systemIpAddress;
#endif /* (( MDF_IP == FYES ) && ( MDF_IP_ROUTE == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_IP_ROUTE == FYES ))
    IpAddress                           softIpInterfaceAddress;
#endif /* (( MDF_IP == FYES ) && ( MDF_IP_ROUTE == FYES )) */
#if ( MDF_IP == FYES )
    IpAddress                           primaryNetBiosNS;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    IpAddress                           secondaryNetBiosNS;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    Boolean                             assignOnly;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    Boolean                             poolSummary;
#endif /* ( MDF_IP == FYES ) */
#if (( MDF_OSPF == FYES ) && ( MDF_IP == FYES ))
    Int                                 poolOspfAdvType;
#endif /* (( MDF_OSPF == FYES ) && ( MDF_IP == FYES )) */
#if ( MDF_IP == FYES )
    IpAddress                           assignBase[ MAX_NVRAM_ASSIGN_POOL ];
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    Word                                assignCount[ MAX_NVRAM_ASSIGN_POOL ];
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    char                                poolName[ MAX_NVRAM_ASSIGN_POOL ][ POOL_NAME_LEN + 1 ];
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    RipPolicy                           ripPolicy;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    Boolean                             ripSummary;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    Boolean                             ripTrigger;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
#if ( MDF_BOOTP == FYES )
    Boolean                             bootpEnabled;
# endif
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
#if ( MDF_BOOTP_RELAY == FYES )
    BootpRelay                          bootpRelay;
# endif
#endif /* ( MDF_IP == FYES ) */
    Boolean                             ignoreIcmpRedirects;
    Boolean                             dropSourceRoutedIpPackets;
#if ( MDF_IP == FYES )
    Boolean                             ignoreDefRoute;
#endif /* ( MDF_IP == FYES ) */
#if (( INTERFACE_MGR == FNO ) && (MDF_IP == FYES ) )
    ProxyMode                           proxyMode;
#endif /* (( INTERFACE_MGR == FNO ) && (MDF_IP == FYES ) ) */
#if ( MDF_IP == FYES )
    Boolean                             rarpEnabled;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    Boolean                             udpCksum;
#endif /* ( MDF_IP == FYES ) */
#if (( MDF_IP == FYES ) && ( TELNET == FYES ))
    Word                                tcpTimeout;
#endif /* (( MDF_IP == FYES ) && ( TELNET == FYES )) */
#if ( MDF_IP == FYES )
    Boolean                             dialoutPoison;
#endif /* ( MDF_IP == FYES ) */
#if ( MDF_IP == FYES )
    char                                telnetPassword[ MAX_PASSWORD_LENGTH + 2 ];
#endif /* ( MDF_IP == FYES ) */
#if ( ( USERIF & CLI ) && ( MDF_IP == FYES ) )
    Byte                                userProfile[ MAX_NAME_LEN + 1 ];
#endif /* ( ( USERIF & CLI ) && ( MDF_IP == FYES ) ) */
#if ( MDF_IP == FYES )
    Boolean                             sharedProf;
#endif /* ( MDF_IP == FYES ) */
#if ((MDF_IP == FYES) && (MDF_DNS == FYES))
    Boolean                             dnsListAttempt;
#endif /* ((MDF_IP == FYES) && (MDF_DNS == FYES)) */
#if (MDF_IP_ROUTE == FYES)
    LongWord                            staticPref;
#endif /* (MDF_IP_ROUTE == FYES) */
#if (MDF_IP_ROUTE == FYES)
    LongWord                            ripPref;
#endif /* (MDF_IP_ROUTE == FYES) */
    Uint16                              ripQueueDepth;
#if ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES))
    LongWord                            ospfPref;
#endif /* ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES)) */
#if ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES))
    LongWord                            ospfAsePref;
#endif /* ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES)) */
#if ( MDF_OSPF == FYES )
    MibProfOspfg                        ospfGlobal;
#endif /* ( MDF_OSPF == FYES ) */
#if ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES))
    LongWord                            ripTag;
#endif /* ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES)) */
#if ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES))
    LongWord                            ripAseType;
#endif /* ((MDF_IP_ROUTE == FYES) && (MDF_OSPF == FYES)) */
#if ( (MDF_IP_ROUTE == FYES) && (MDF_IP_ROUTE_CACHE == FYES) )
    Boolean                             ipRouteCache;
#endif /* ( (MDF_IP_ROUTE == FYES) && (MDF_IP_ROUTE_CACHE == FYES) ) */
#if (( MDF_IP_ROUTE == FYES ) && ( MDF_IP_ROUTE_CACHE == FYES ))
    int                                 ipRouteCacheSize;
#endif /* (( MDF_IP_ROUTE == FYES ) && ( MDF_IP_ROUTE_CACHE == FYES )) */
#if ( MDF_IP_PORT_CACHE == FYES )
    Boolean                             ipPortCache;
#endif /* ( MDF_IP_PORT_CACHE == FYES ) */
#if ( MDF_IP_ROUTE == FYES )
    Boolean                             suppressHostRoute;
#endif /* ( MDF_IP_ROUTE == FYES ) */
#if ( MDF_SNTP == FYES )
    SntpProfile                         sntpInfo;
#endif /* ( MDF_SNTP == FYES ) */
#if ((MDF_IP == FYES) && (MDF_DNS == FYES))
    int                                 dnsListSize;
#endif /* ((MDF_IP == FYES) && (MDF_DNS == FYES)) */
#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    IpAddress                           clientPrimaryDnsServer;
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    IpAddress                           clientSecondaryDnsServer;
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    Boolean                             allowAsClientDnsInfo;
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if ( 0 )
    Boolean                             localDnsTableEnabled;
#endif /* ( 0 ) */
#if ( 0 )
    Boolean                             localDnsTableAutoUpdate;
#endif /* ( 0 ) */
#if ( MDF_DNS_FALLBACK_TABLE == FYES )
    MibProfDnsFallbackCfg               dnsFallbackCfg;
#endif /* ( MDF_DNS_FALLBACK_TABLE == FYES ) */
#if ((MDF_MROUTING == FYES) || (MDF_MTROUTING == FYES))
    Boolean                             multicastForwarding;
#endif /* ((MDF_MROUTING == FYES) || (MDF_MTROUTING == FYES)) */
#if ((MDF_MROUTING == FYES) || (MDF_MTROUTING == FYES))
    char                                mboneProfile[ MAX_HOST_NAME_LENGTH + 1 ];
#endif /* ((MDF_MROUTING == FYES) || (MDF_MTROUTING == FYES)) */
#if ((MDF_MROUTING == FYES) || (MDF_MTROUTING == FYES))
    LogicalAddress                      mboneLanLogicalAddress;
#endif /* ((MDF_MROUTING == FYES) || (MDF_MTROUTING == FYES)) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    IpAddress                           multicastHeartBeatAddr;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    int                                 multicastHeartBeatPort;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    LongWord                            multicastHeartBeatSlotTime;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    LongWord                            multicastHeartBeatSlotlength;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    LongWord                            multicastHeartBeatAlarmThreshold;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    IpAddress                           multicastHeartBeatSrcAddr;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES ))
    IpAddress                           multicastHeartBeatSrcAddrMask;
#endif /* (( MDF_IP == FYES ) && ( MDF_MROUTING == FYES )) */
#if (( MDF_IP == FYES ) && ( MDF_DNS == FYES ))
    char                                secDomainName[ MAX_DOMAIN_NAME_LENGTH + 1 ];
#endif /* (( MDF_IP == FYES ) && ( MDF_DNS == FYES )) */
#if ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES))
    Uint16                              multicastMemberTimeout;
#endif /* ((MDF_MROUTING == FYES) && (MDF_IGMP == FYES)) */
#if ( MDF_FINGER == FYES )
    Boolean                             fingerEnable;
#endif /* ( MDF_FINGER == FYES ) */
#if ( MDF_IP_ROUTE == FYES )
    Boolean                             icmpReplyDirectedBcast;
#endif /* ( MDF_IP_ROUTE == FYES ) */
#if ( MDF_VROUTER == FYES )
    char                                vRouterName[ MAX_NAME_LEN + 1 ];
#endif /* ( MDF_VROUTER == FYES ) */
#if ( 0 )
    char                                unusedField;
#endif /* ( 0 ) */
}
#if ( USERIF & CLI )
__PACK__
#endif
IpGlobalProfile;

#endif /* MIBIP_H */

