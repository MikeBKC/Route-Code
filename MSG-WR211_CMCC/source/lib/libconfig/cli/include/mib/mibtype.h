/*
 * $Id: mibtype.h,v 1.2 2012/08/29 10:01:13 yu.dehai Exp $
 *
 *      Copyright (c) 1995 UTT Technologies, Inc.
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
#if ! defined( MIBTYPE_H )
# define MIBTYPE_H

    /*
     * MibProfileType:
     *
     * Every profile in the system is assigned a profile name which maps
     * to a profile type.
     *
     * _NOTE_:  For the sake of system sanity, please respect the following
     *		guidelines.
     *		We want profile type values to remain constant once defined,
     *		regardless of new features added or old ones deleted (as if).
     *		It is convenient if the profile type values are the same on
     *		all platforms. 
     *
     *	DO NOT: - place conditionals (#if FOO) around new profile types.
     *  DO:     - place new profile types at the end of the list.
     *		- update the comment here with an explanation of your new type.
     *
     * The known types are:
     *
     *	MIB_PROF_BASE:		The profile that consists of the base
     *				unit configuration, include which features
     *				are enabled and which are not.  There is
     *				only one base profile.  See mibbase.h for
     *				details.
     *
     *	MIB_PROF_SNTP:		SNTP parameters for the TNT platform.
     *
     *	MIB_PROF_SYSTEM:	The profile that contains one-of general,
     *				system wide entries.  There is only one
     *				system profile.
     *
     *	MIB_PROF_T1_LINE_STAT:	The profile that consists of T1 line status
     *				information.
     *
     *	MIB_PROF_ACCT_CONF:	The profile that contains all one-of
     *				accounting configuration parameters.  There
     *				is only one accounting config profile.
     *
     *	MIB_PROF_IP_CONF		The profile that contains all one-of IP
     *				routing configuration parameters.  There is
     *				only one IP config profile.
     *
     *	MIB_PROF_IPX_CONF:	The profile that contains all one-of IPX
     *				routing configuration parameters.  There is
     *				only one IPX config profile.
     *
     *	MIB_PROF_BRIDGE_CONF:	The profile that contains all one-of bridging
     *				comfiguration parameters.  There is only one
     *				bridging config profile.
     *
     *	MIB_PROF_AUTH_CONF:	The profile that contains all one-of
     *				authentication configuration parameters.
     *				There is only one auth profile.
     *
     *	MIB_PROF_LOGIN:		The profile that contains security
     *				parameters selected by a user.  There are
     *				multiple instances of this profile indexed
     *				by assigned name.
     *
     *	MIB_PROF_SHELF:		The profile that contains information
     *				specific to shelf controllers.  There is one
     *				profile per shelf controller.
     *
     *	MIB_PROF_USER:		The profile that contains information
     *				specific to a particular user.  There are
     *				as many user profiles as are needed, indexed
     *				by the user name.
     *
     *  MIB_PROF_DBA_SESSION:   The profile that contains DBA information.
     *
     *	MIB_PROF_TPP:   	The profile that contains information specific
     *				to the structure TimePeriodParameters.
     *
     *  MIB_PROF_SN:            The profile that contains information
     *                          specific to the structure SlotNumber.
     *
     *  MIB_PROF_REL_PN:        The profile that contains information
     *                          specific to the structure RelativePortNumber.
     *
     *  MIB_PROF_REC_PN:        The profile that contains information
     *                          specific to the structure RectangularPortNumber.
     *
     *  MIB_PROF_NET_T1:        The profile that contains information for
     *                          each T1/PRI interface.
     *
     *  MIB_PROF_T1CC:          The profile that contains information
     *                          specific to the structure T1ChannelConfig.
     *
     *  MIB_PROF_T1LIF:         The profile that contains information
     *                          specific to the structure T1LineInterface.
     *
     *  MIB_PROF_NET_BRI_TE:    The profile that contains information
     *                          specific to the BRI TE network interface.
     *
     *  MIB_PROF_BCC:           The profile that contains information
     *                          specific to the structure BRIChannelConfig.
     *
     *  MIB_PROF_BLI:           The profile that contains information
     *                          specific to the structure BRILineInterface.
     *
     *  MIB_PROF_BRINT:         The profile that contains information
     *                          specific to the BRI NT network interface.
     *
     *  MIB_PROF_BHL:           The profile that contains information
     *                          specific to the structure BRIHostLine.
     *
     *  MIB_PROF_NET_DPNSS:     The profile that contains information
     *                          specific to the network.
     *
     *  MIB_PROF_DPNSSCC:       The profile that contains information
     *                          specific to the structure DPNSSChannelConfig.
     *
     *  MIB_PROF_DPNSSLI:       The profile that contains information
     *                          specific to the structure DPNSSLineInterface.
     *
     *  MIB_PROF_NET_S56:       The profile that contains information
     *                          specific to the network.
     *
     *  MIB_PROF_S56CC:         The profile that contains information
     *                          specific to the structure S56ChannelConfig.
     *
     *  MIB_PROF_S56LI:         The profile that contains information
     *                          specific to the structure S56LineInterface.
     *
     *  MIB_PROF_PORT:          This profile primarily concerns with routing
     *                          information.
     *
     *  MIB_PROF_SECURITY:      This profile primarily concerns with access
     *                          and security permissions.
     *
     *  MIB_PROF_SLT_HEADER:    This profile contains the header information
     *                          for all slot profiles.
     *
     *  MIB_PROF_D_AIM_S:       The profile that contains information
     *                          specific to the structure DualAIMSlotProfile.
     *
     *  MIB_PROF_AIM_6S:        The profile that contains information
     *                          specific to the structure AIM6SlotProfile.
     *
     *  MIB_PROF_S_WAN_S:       The profile that contains information
     *                          specific to the structure SerialWanSlotProfile.
     *
     *  MIB_PROF_D_HOST_S:      The profile that contains information
     *                          specific to the structure DualHostSlotProfile.
     *
     *  MIB_PROF_Q_HOST_S:      The profile that contains information
     *                          specific to the structure QuadHostSlotProfile.
     *
     *  MIB_PROF_ETHER_S:       The profile that contains information
     *                          specific to the structure EtherSlotProfile.
     *
     *  MIB_PROF_ETHER_DAT_S:   The profile that contains information
     *                          specific to the structure EtherDataSlotProfile.
     *
     *  MIB_PROF_LAN_MDM_S:     The profile that contains information
     *                          specific to the structure LanModemSlotProfile.
     *
     *  MIB_PROF_D_POTS_S:      The profile that contains information
     *                          specific to the structure DualPOTSSlotProfile.
     *
     *  MIB_PROF_NAILED_56:     The profile that contains information
     *                          specific to the structure Nailed56SlotProfile.
     *
     *  MIB_PROF_NAILED_T1:     The profile that contains information
     *                          specific to the structure NailedT1SlotProfile.
     *
     *
     *  MIB_PROF_V110_S:        The profile that contains information
     *                          specific to the structure V110SlotProfile.
     *
     *	MIB_PROF_BILLING:	The profile that contains billing statistics.
     *				These statistics need to be kept, even after
     *				power failures.
     *
     *	MIB_PROF_ROUTE:		The profile that contains the information used
     *				by the router to set up static routes.
     *
     *	MIB_PROF_IPX_ROUTE:	The profile that describes a static IPX route
     *				to an IPX server.
     *
     *	MIB_PROF_FILTER:	The profile that describes the filters used to
     *				control data in and out of an interface.
     *
     *  MIB_PROF_FLR_ENTRY:     The profile that contains information
     *                          specific to the structure FilterEntry.
     *
     *  MIB_PROF_GEN_FILT:      The profile that contains information
     *                          specific to the structure GenericFilter.
     *
     *  MIB_PROF_IP_FILT:       The profile that contains information
     *                          specific to the structure IpFilter.
     *
     *  MIB_PROF_IP_ROUTE_FILT: The profile that contains information
     *                          specific to the structure IpFilter.
     *
     *  MIB_PROF_AVAIL_1:	Available for future use
     *
     *  MIB_PROF_AVAIL_2:	Available for future use
     *
     *	MIB_PROF_INTERNET:	The Internet profile, which is similar to a
     *				call profile with WAN support.
     *
     *	MIB_PROF_OSPF:		The ospf profile, which is used in 
     *				the ether slot profile and the connection
     *                          profile.
     *
     *  MIB_PROF_AVAIL_3:	Available for future use
     *
     *	MIB_PROF_TRAP:  	The profile that contains the information
     *				required by the SNMP agent to generate traps.
     *
     *	MIB_PROF_BRIDGE:  	The profile that contains the ethernet numbers
     *
     *  MIB_PROF_DEST_NUM:      The profile that contains information
     *                          specific to the structure DestNumber.
     *
     *	MIB_PROF_DEST:  	The profile that contains the destination
     *				information.
     *
     *	MIB_PROF_PLAN:  	The profile that contains the plan information.
     *				that we are going to dial calls for.
     *
     *	MIB_PROF_FRMRL:  	The profile that contains the frame relay
     *				information.
     *
     *	MIB_PROF_NET_AT:	The profile that contains information
     *				specific to the network.
     *
     *	MIB_PROF_NAME_PASS:	Profiles that are simple subsets of 
     *				connection profiles that contain only
     *				the name/password and minimalistic
     *				things.
     *
     *	MIB_PROF_ANSWER:	This answer profile.
     *
     *	MIB_PROF_PHYS_ADDR:	This profile defines a data type used to
     *				specify a unique physical device within a TNT
     *				system.
     *
     *	MIB_PROF_LOGL_ADDR:	This profile defines a data type used to
     *				specify a unique logical entity associated
     *				with a physical device within a TNT
     *				system.
     *
     *	MIB_PROF_ETHER_INT:	This profile configures a single physical
     *				ethernet interface
     *
     *	MIB_PROF_IP_INT:	This profile configures a single logical
     *				IP interface
     *
     *	MIB_PROF_IP_GLOBAL:	This profile configures system-wide
     *				TCP/IP parameters
     *
     *	MIB_PROF_SNMP_STATE:	Profile contains the physical address to
     *				SNMP interface mapping and the current
     *				administrative state of the interface.
     *
     *	MIB_PROF_SLOT_STATE:	Profile contains the operational state
     *				of all slot cards on a shelf.  The profile
     *				is not persistant, but is managed by the
     *				shelf controller slot manager.
     *
     *	MIB_PROF_SLOT_TYPE:	Profile containing the type of slot card
     *				in a given shelf/slot.
     *
     *	MIB_PROF_ETHER_INT:	This profile configures a single physical
     *				serial interface
     *
     *  MIB_PROF_TIME:		This sub-profile is used to get and set the
     *				time.
     *
     *  MIB_PROF_DATE:		This sub-profile is used to get and set the
     *				date.
     *
     *  MIB_PROF_TIME_DATE:	This profile is holds the current date
     *				and time for the system.
     *
     *  MIB_PROF_TERM_SERV:	This profile holds system-wide
     *				terminal server parameters.
     *
     *  MIB_PROF_AT_MDM_S:	This profile that contains information
     *                          specific to the structure AtModemSlotProfile.
     *
     *	MIB_PROF_FWALL:		This profile contains a firewall.
     *
     *	MIB_PROF_LOG:		System wide logging control.
     *
     *	MIB_PROF_CRAP_INDEX:	The profile that is an index to the 
     *				Call Routing Attribute Profile.
     *
     *	MIB_PROF_CRAP:		Call Routing Attribute Profile.
     *
     *	MIB_PROF_DEV_STATE:	Internal profile that records the current
     *				and required state of a device.
     *
     *	MIB_PROF_EXTERNAL_AUTH	This profile holds information which
     *				specifies the type of external authentication
     *				to use.
     *
     *	MIB_PROF_RADAUTH_CLIENT	This profile holds information used by
     *				a radius authentication client to access a
     *				radius authentication server
     *
     *	MIB_PROF_RADACCT_CLIENT	This profile holds information used by
     *				a radius accounting client to access a
     *				radius accounting server
     *
     *	MIB_PROF_RADAUTH_SERVER	This profile holds information used by
     *				a radius authentication server to access a
     *				radius authentication client
     *
     *	MIB_PROF_TACAUTH_CLIENT	This profile holds information used by
     *				a tacacs authentication client to access a
     *				tacacs authentication server
     *
     *  MIB_PROF_V35:		The profile that contains information
     *                          specific to the structure V35lotProfile.
     *
     *  MIB_PROF_SERIAL_T1_CSU: The profile that contains information specific
     *				to the structure SerialT1CSU_SlotProfile.
     *
     *	MIB_PROF_SREG		This profile holds the S-Register values
     *				used by the p15
     *
     *	MIB_PROF_PHONE_NUM	This profile holds the Phone Numbers
     *				used by the p15
     *
     *	MIB_PROF_CALL_INFO:	Call information for currently active
     *				calls, e.g. switched call type,
     *				call-by-call value, nailed group, etc.
     *
     *	MIB_PROF_ARA_ANSWER:	Answer profile, ara defaults.
     *	MIB_PROF_COMB_ANSWER:	Answer profile, combinet defaults.
     *	MIB_PROF_EU_ANSWER:	Answer profile, eunet protocol defaults.
     *	MIB_PROF_FR_ANSWER:	Answer profile, frame relay defaults.
     *	MIB_PROF_IPX_ANSWER:	Answer profile, ipx routing defaults.
     *	MIB_PROF_IP_ANSWER:	Answer profile, ip routing defaults.
     *	MIB_PROF_MPP_ANSWER:	Answer profile, mpp defaults.
     *	MIB_PROF_MP_ANSWER:	Answer profile, mp defaults.
     *	MIB_PROF_PPP_ANSWER:	Answer profile, ppp defaults.
     *	MIB_PROF_SES_ANSWER:	Answer profile, session defaults.
     *	MIB_PROF_TCP_ANSWER:	Answer profile, tcp defaults.
     *	MIB_PROF_V120_ANSWER:	Answer profile, v120 defaults.
     *	MIB_PROF_X25_ANSWER:	Answer profile, x.25 defaults.
     *
     *	MIB_PROF_TS_MODEM:	Term Server, modem options
     *	MIB_PROF_TS_TERM:	Term Server, interactive options
     *	MIB_PROF_TS_MENU:	Term Server, menu mode options
     *	MIB_PROF_TS_TELNET:	Term Server, telnet options
     *	MIB_PROF_TS_RLOGIN:	Term Server, rlogin options
     *	MIB_PROF_TS_IMM:	Term Server, immediate mode options
     *	MIB_PROF_TS_PPP:	Term Server, ppp configuration options
     *	MIB_PROF_TS_SLIP:	Term Server, slip configuration options
     *	MIB_PROF_TS_DIAL:	Term Server, modem outdial options
     *
     *	MIB_PROF_ROUTE_ID:	The RouteID structure.
     *
     *	MIB_PROF_IN_ANS:	Internet Profile, answer options
     *	MIB_PROF_IN_ARA:	Internet Profile, ARA options
     *	MIB_PROF_IN_BRIDGE:	Internet Profile, bridging options
     *	MIB_PROF_IN_COMB:	Internet Profile, combinet options
     *	MIB_PROF_IN_EU:		Internet Profile, EUNET options
     *	MIB_PROF_IN_FR:		Internet Profile, frame relay options
     *	MIB_PROF_IN_IP:		Internet Profile, ip options
     *	MIB_PROF_IN_IPX:	Internet Profile, ipx options
     *	MIB_PROF_IN_MP:		Internet Profile, mp options
     *	MIB_PROF_IN_MPP:	Internet Profile, mpp options
     *	MIB_PROF_IN_PPP:	Internet Profile, ppp options
     *	MIB_PROF_IN_SES:	Internet Profile, session options
     *	MIB_PROF_IN_T3POS:	Internet Profile, T3POS options
     *	MIB_PROF_IN_TCP:	Internet Profile, tcp options
     *	MIB_PROF_IN_TELCO:	Internet Profile, telco options
     *	MIB_PROF_IN_X25:	Internet Profile, X.25 options
     *	MIB_PROF_IN_V120:	Internet Profile, v120 options
     *	MIB_PROF_IN_DTPT:	Internet Profile, T-Online pseudotunnel options
     *	MIB_PROF_OSPFG:		The global ospf profile, which is used in 
     *				the ether slot profile for box-wide settings. 
     *	MIB_PROF_IN_X75:	Internet Profile, X.75 options
     *	MIB_PROF_X75_ANSWER:	Answer profile, x.75 defaults.
     *  MIB_PROF_ERROR:		The fatal error log, implemented as profiles.
     *  MIB_PROF_DEBUG:		Debugging data we wish to preserve across
     *				system resets.  
     *  MIB_PROF_IMODEM_S:      The profile that contains information
     *                          specific to the structure IModemSlotProfile
     *				used by the p15.
     * 	MIB_PROF_SHASH		Secure Hash Profile
     *
     *
     *  MIB_PROF_IN_ATALK:	Internet Profile, appletalk router options
     *
     *	MIB_PROF_IN_USR_RAD:	This holds a per user profile info
     *				on a RADIUS Accounting Server.
     *
     *  MIB_PROF_IN_DHCP:	Internet Profile, DHCP Server options
     *
     *	MIB_PROF_SNMP:		This holds global SNMP information
     *				for the SYSTEM (TNT).
     *
     *	MIB_PROF_IPX_FILT	The profile that contains information
     *                          specific to the structure IpxFilter.
     *
     *	MIB_PROF_LAN_MODEM:	This holds LAN modem info
     *				for the SYSTEM (TNT).
     *
     *	MIB_PROF_SWANLC		The profile that contains information for
     *				each SwanLineConfig
     *
     *	MIB_PROF_SWANLIF	The profile that contains information for
     *				each  SwanLineInterface
     *
     *	MIB_PROF_NET_SWAN	The profile that contains information for
     *				each Serial Wan interface
     *
     *	MIB_PROF_SLOT_INFO:	Profile containing slot card serial number and
     *				software version info in a given shelf/slot.
     *
     *	MIB_PROF_SWAN_LINE_STAT:
     *				The profile that consists of SWAN line status
     *				information.
     *
     *  MIB_PROF_IP_POOLS	The local address assignment pools information
     *				
     *	MIB_PROF_NET_DS3	The profile that contains information for
     *				each DS3 interface
     *
     *	MIB_PROF_DS3_LINE_STAT:	The profile that consists of DS3 line status
     *				information.
     *
     *  MIB_PROF_IPX_INT        This profile configures a single logical
     *                          IPX interface
     *
     *  MIB_PROF_IPX_GLOBAL:	This profile configures system-wide
     *                          IPX parameters
     *
     *  MIB_PROF_V32_S:         The profile that contains information
     *                          specific to the structure V32SlotProfile.
     *
     *  MIB_PROF_B52_TEST	The profile for B52 testing features
     *
     *	MIB_PROF_SDSLLC		The profile that contains information for
     *				each SdslLineConfig
     *
     *	MIB_PROF_SDSLLIF	The profile that contains information for
     *				each  SdslLineInterface
     *
     *	MIB_PROF_NET_SDSL	The profile that contains information for
     *				each Sdsl interface
     *
     *	MIB_PROF_SDSL_LINE_STAT:The profile that consists of SDSL line status
     *				information.
     *
     *	MIB_PROF_SDSL_LINE_STATIST: profile that consists of SDSL line statistics
     *				information.
     *
     *	MIB_PROF_CADSLLC	The profile that contains information for
     *				each SdslLineConfig
     *
     *	MIB_PROF_CADSLLIF	The profile that contains information for
     *				each  SdslLineInterface
     *
     *	MIB_PROF_NET_CADSL	The profile that contains information for
     *				each Sdsl interface
     *
     *	MIB_PROF_CADSL_LINE_STAT:The profile that consists of CADSL line status
     *				information.
     *
     *	MIB_PROF_CADSL_LINE_STATISTICS:The profile that consists of CADSL
     *				 line status information.
     *
     *  MIB_PROF_ATALK_INT:	This profile configures a single logical
     *                          Appletalk interface
     *
     *  MIB_PROF_ATALK_GLOBAL:	This profile configures system-wide
     *                          Appletalk parameters
     *
     *	MIB_PROF_BRI_LINE_STAT:	The profile that consists of BRI line status
     *				information.
     *
     *  MIB_PROF_ETHER_X:       extension to MIB_PROF_ETHER_S because it
     *                          got over 255
     *
     *  MIB_PROF_TUNNEL_SERVER: These profiles contain information describing
     *				tunnel servers (L2TP/PPTP).
     *
     *  MIB_PROF_L2TUNNEL_GLOBAL: This profile contains global configuration
     *				for all layer 2 tunneling protocols (L2TP/PPTP).
     *
     *  MIB_PROF_ATMP:		This profile configures ATMP operation.
     *
     *  MIB_PROF_IN_TUNNEL:	Internet Profile, tunneling options.
     *
     *  MIB_PROF_DLCI_ID:	A sub-profile, unique ID for DLCIs.
     *
     *  MIB_PROF_FRPVC_STAT:	A system profile - state of a Frame Relay
     *				PVC segment.
     *
     *  MIB_PROF_DLCI_STAT:	A system profile - state of a DLCI which is
     *				a member of a PVC segment.
     *
     *	MIB_PROF_IN_X32:	Internet Profile, X.32 options
     *
     *  MIB_PROF_RAD_ACCT_CALL_LOG:This profile holds information used by
     *				a radius accounting call log client to access a
     *				radius accounting call log server.
     *
     *  MIB_PROF_VDSP_S:         The profile that contains information
     *                          specific to the structure VDSPSlotProfile.
     *
     *  MIB_PROF_SS7:		SS7 Gateway interface configuration. Defines
     *				primary and secondary signaling gateways
     *				and other information.
     *
     *  MIB_PROF_REBOOT_STATISTICS:	This profile holds statistics about
     *				types, numbers, and rates of reboot events
     *
     *  MIB_PROF_REDUNDANCY_STATISTICS:	This profile holds the identity of the
     *				entity for which statistics are being kept
     *				and a MIB_PROF_REBOOT_STATISTICS profile
     *
     *  MIB_PROF_REDUNDANCY_CONTEXT:	This profile holds the context of a
     *				single controller
     *
     *  MIB_PROF_REDUNDANCY:    This profile the MIB_PROF_REDUNDANCY_CONTEXTs
     *                          for all of the contollers in a system
     *
     *  MIB_PROF_EXT_TS_HOSTS:  Volatile profile to store termsrv menu banner
     *                          hosts information.  Sub-profile of
     *                          MIB_PROF_EXT_TS.
     *
     *  MIB_PROF_EXT_TS:        Volatile profile to store termsrv config
     *                          information that has been obtained from
     *                          RADIUS, like initial-banner and banner.
     *                          This profile is used to transfer this data
     *                          to the slot cards.
     *
     *	MIB_PROF_UDS3LC		The profile that contains information for
     *				each Uds3LineConfig
     *
     *	MIB_PROF_NET_UDS3	The profile that contains information for
     *				each Uds3 interface
     *
     *	MIB_PROF_UDS3_LINE_STAT:The profile that consists of UDS3 status
     *				information.
     *
     *	MIB_PROF_IPFAX		Profile for IP-FAX information
     *
     *	MIB_PROF_DADSLLC	The profile that contains information for
     *				each DadslLineConfig
     *
     *	MIB_PROF_NET_DADSL	The profile that contains information for
     *				each Dadsl interface
     *
     *	MIB_PROF_DADSL_LINE_STAT:The profile that consists of DADSL line status
     *				information.
     *
     *	MIB_PROF_DADSL_LINE_STATISTICS:The profile that consists of DADSL
     *				line status information.
     *
     *	MIB_PROF_IN_ATM:	Internet Profile, ATM options
     *
     *	MIB_PROF_DS3_ATM_LINE_CFG: The profile that contains information for
     *				each DS3 ATM Line Config
     *
     *	MIB_PROF_NET_DS3_ATM	The profile that contains information for
     *				each DS3 ATM interface
     *
     *	MIB_PROF_DS3_ATM_LINE_STAT:  The profile that consists of DS3 ATM 
     *                          line status information.
     *
     *  MIB_PROF_VDSLCC:        The profile that contains information
     *                          specific to the structure VdslChannelConfig.
     *
     *	MIB_PROF_VDSLLC		The profile that contains information for
     *				each VdslLineConfig
     *
     *	MIB_PROF_NET_VDSL	The profile that contains information for
     *				each Vdsl interface (SDSL voice)
     *
     *	MIB_PROF_VDSL_LINE_STAT:The profile that consists of VDSL status
     *				information.
     *
     *  MIB_PROF_OSPF_AREA_RANGE:The profile that defines ospf area range
     *                           parameters
     *
     *  MIB_PROF_OSPF_VIRTUAL_LINKS:
     *                           The profile that defines ospf virtual links
     *
     *	MIB_PROF_BOOTP_RELAY:  The profile that contains BOOTP RELAY setup
     *			 	information.
     *
     *  MIB_PROF_SYS_MODEM:      The profile that contains the modem related
     *                           information. From now on (7/15/98) modem
     *                           related stuff needs to be added here instead
     *                           of MIB_PROF_ETHER_X.
     *
     *  MIB_PROF_DEV_SUMMARY:	Summary information about the number of
     *				different devices in the system.
     *
     *  MIB_PROF_VOIP           Profile for VOIP user config items
     *
     *  MIB_PROF_SWAN_CLOCK:  The profile used for SWAN port clock generation.
     *
     *  MIB_PROF_LOAD_SELECT:	Selects card code images to be loaded from tarballs.
     *
     *  MIB_PROF_SNMP_STATE_SEC: Profile contains the conn hostname to
     *				SNMP interface mapping and the current
     *				administrative state of the interface.
     *				This profile function is similar to
     *				MIB_PROF_SNMP_STATE but for permanent
     *			       	iface such as PPP or FR over nailed group
     *
     *  MIB_PROF_DNIS_TAB:     A sub-profile of the Ethernet profile that 
     *                         is used for DNIS options.
     *
     *  MIB_PROF_CBQ_XXX: 	profile used for cbq
     *
     *
     *  **WARNING**: 
     *
     *		NVRAM profile storage depends on these values being
     *		constant if existing configurations are to remain
     *		valid across software versions.  For this reason,
     *		add new values at the *end* of the enum list!
     */
typedef enum {
    MIB_PROF_BASE               =   0,
    MIB_PROF_SNTP               =   1,
    MIB_PROF_T1_LINE_STAT       =   2,
    MIB_PROF_SYSTEM             =   3,
    MIB_PROF_ACCT_CONF          =   4,
    MIB_PROF_IP_CONF            =   5,
    MIB_PROF_IPX_CONF           =   6,
    MIB_PROF_BRIDGE_CONF        =   7,
    MIB_PROF_AUTH_CONF          =   8,
    MIB_PROF_LOGIN              =   9,
    MIB_PROF_SHELF              =  10,
    MIB_PROF_USER               =  11,
    MIB_PROF_DBA_SESSION        =  12,
    MIB_PROF_TPP                =  13,
    MIB_PROF_SN                 =  14,
    MIB_PROF_REL_PN             =  15,
    MIB_PROF_REC_PN             =  16,
    MIB_PROF_NET_T1             =  17,
    MIB_PROF_T1CC               =  18,
    MIB_PROF_T1LIF              =  19,
    MIB_PROF_NET_BRI_TE         =  20,
    MIB_PROF_BCC                =  21,
    MIB_PROF_BLI                =  22,
    MIB_PROF_NET_BRI_NT         =  23,
    MIB_PROF_BHL                =  24,
    MIB_PROF_NET_DPNSS          =  25,
    MIB_PROF_DPNSSCC            =  26,
    MIB_PROF_DPNSSLI            =  27,
    MIB_PROF_NET_S56            =  28,
    MIB_PROF_S56CC              =  29,
    MIB_PROF_S56LI              =  30,
    MIB_PROF_PORT               =  31,
    MIB_PROF_SECURITY           =  32,
    MIB_PROF_SLT_HEADER         =  33,
    MIB_PROF_SLOT               =  34,
    MIB_PROF_D_AIM_S            =  35,
    MIB_PROF_AIM_6S             =  36,
    MIB_PROF_S_WAN_S            =  37,
    MIB_PROF_D_HOST_S           =  38,
    MIB_PROF_Q_HOST_S           =  39,
    MIB_PROF_ETHER_S            =  40,
    MIB_PROF_ETHER_DAT_S        =  41,
    MIB_PROF_LAN_MDM_S          =  42,
    MIB_PROF_NAILED_56          =  43,
    MIB_PROF_NAILED_T1          =  44,
    MIB_PROF_D_POTS_S           =  45,
    MIB_PROF_V110_S             =  46,
    MIB_PROF_BILLING            =  47,
    MIB_PROF_DEST_NUM           =  48,
    MIB_PROF_DEST               =  49,
    MIB_PROF_ROUTE              =  50,
    MIB_PROF_IPX_ROUTE          =  51,
    MIB_PROF_IPXSAP_FLR_ENTRY   =  52,
    MIB_PROF_IPXSAP_FILT        =  53,
    MIB_PROF_FILTER             =  54,
    MIB_PROF_FLR_ENTRY          =  55,
    MIB_PROF_GEN_FILT           =  56,
    MIB_PROF_IP_FILT            =  57,
    MIB_PROF_AVAIL_1            =  58,
    MIB_PROF_AVAIL_2            =  59,
    MIB_PROF_INTERNET           =  60,
    MIB_PROF_AVAIL_3            =  61,
    MIB_PROF_TRAP               =  62,
    MIB_PROF_BRIDGE             =  63,
    MIB_PROF_PLAN               =  64,
    MIB_PROF_FRAME_RELAY        =  65,
    MIB_PROF_X25                =  66,
    MIB_PROF_POTS               =  67,
    MIB_PROF_NET_AT             =  68,
    MIB_PROF_NAME_PASS          =  69,
    MIB_PROF_OSPF               =  70,
    MIB_PROF_ANSWER             =  71,
    MIB_PROF_PHYS_ADDR          =  72,
    MIB_PROF_LOGL_ADDR          =  73,
    MIB_PROF_ETHER_INT          =  74,
    MIB_PROF_IP_INT             =  75,
    MIB_PROF_IP_GLOBAL          =  76,
    MIB_PROF_SNMP_STATE         =  77,
    MIB_PROF_SLOT_STATE         =  78,
    MIB_PROF_SLOT_TYPE          =  79,
    MIB_PROF_SERIAL_INT         =  80,
    MIB_PROF_TIME               =  81,
    MIB_PROF_DATE               =  82,
    MIB_PROF_TIME_DATE          =  83,
    MIB_PROF_TERM_SERV          =  84,
    MIB_PROF_AT_MDM_S           =  85,
    MIB_PROF_FWALL              =  86,
    MIB_PROF_LOG                =  87,
    MIB_PROF_CRAP_INDEX         =  88,
    MIB_PROF_CRAP               =  89,
    MIB_PROF_DEV_STATE          =  90,
    MIB_PROF_EXTERNAL_AUTH      =  91,
    MIB_PROF_RADAUTH_CLIENT     =  92,
    MIB_PROF_RADACCT_CLIENT     =  93,
    MIB_PROF_RADAUTH_SERVER     =  94,
    MIB_PROF_TACAUTH_CLIENT     =  95,
    MIB_PROF_V35                =  96,
    MIB_PROF_SERIAL_T1_CSU      =  97,
    MIB_PROF_SREG               =  98,
    MIB_PROF_PHONE_NUM          =  99,
    MIB_PROF_CALL_INFO          = 100,

    MIB_PROF_ARA_ANSWER         = 101,
    MIB_PROF_COMB_ANSWER        = 102,
    MIB_PROF_EU_ANSWER          = 103,
    MIB_PROF_FR_ANSWER          = 104,
    MIB_PROF_IPX_ANSWER         = 105,
    MIB_PROF_IP_ANSWER          = 106,
    MIB_PROF_MPP_ANSWER         = 107,
    MIB_PROF_MP_ANSWER          = 108,
    MIB_PROF_PPP_ANSWER         = 109,
    MIB_PROF_SES_ANSWER         = 110,
    MIB_PROF_TCP_ANSWER         = 111,
    MIB_PROF_V120_ANSWER        = 112,
    MIB_PROF_X25_ANSWER         = 113,

    MIB_PROF_TS_MODEM           = 114,
    MIB_PROF_TS_TERM            = 115,
    MIB_PROF_TS_MENU            = 116,
    MIB_PROF_TS_TELNET          = 117,
    MIB_PROF_TS_IMM             = 118,
    MIB_PROF_TS_PPP             = 119,
    MIB_PROF_TS_SLIP            = 120,
    MIB_PROF_TS_DIAL            = 121,
 
    MIB_PROF_ROUTE_ID           = 122,

    MIB_PROF_IN_ANS             = 123,
    MIB_PROF_IN_ARA             = 124,
    MIB_PROF_IN_BRIDGE          = 125,
    MIB_PROF_IN_COMB            = 126,
    MIB_PROF_IN_EU              = 127,
    MIB_PROF_IN_FR              = 128,
    MIB_PROF_IN_IP              = 129,
    MIB_PROF_IN_IPX             = 130,
    MIB_PROF_IN_MP              = 131,
    MIB_PROF_IN_MPP             = 132,
    MIB_PROF_IN_PPP             = 133,
    MIB_PROF_IN_SES             = 134,
    MIB_PROF_IN_TCP             = 135,
    MIB_PROF_IN_TELCO           = 136,
    MIB_PROF_IN_V120            = 137,
    MIB_PROF_IN_X25             = 138,
    MIB_PROF_OSPFG              = 139,
    MIB_PROF_IN_X75             = 140,
    MIB_PROF_X75_ANSWER         = 141,
    MIB_PROF_ERROR              = 142,
    /* 
     * It is suggested that new profiles be added specifically with 
     * the " = %d" notation. The reason being it makes it easy to see 
     * a mistake when you reconcile between branches.  some "new" profiles
     * will only go in the current branch, but other "new" profiles may 
     * need to go into older branches. It is imperative that the profile 
     * numbers be consistent across branches.
     */
    MIB_PROF_DEBUG 		= 143,
    MIB_PROF_IMODEM_S 		= 144,
    MIB_PROF_IN_ATALK 		= 145,
    MIB_PROF_IN_USR_RAD 	= 146,
    MIB_PROF_NET_BRI_LT 	= 147,
    MIB_PROF_BLT 		= 148,
    MIB_PROF_IN_DHCP		= 149,
    MIB_PROF_SNMP 		= 150,
    MIB_PROF_IN_IMOD  		= 151,
    MIB_PROF_IPX_FILT		= 152,
    MIB_PROF_LAN_MODEM		= 153,
    MIB_PROF_SWANLC		= 154,
    MIB_PROF_SWANLIF		= 155,
    MIB_PROF_NET_SWAN		= 156,
    MIB_PROF_SLOT_INFO		= 157,
    MIB_PROF_IN_T3POS		= 158,
    MIB_PROF_IN_DTPT		= 159,
    MIB_PROF_TACPLUSAUTH_CLIENT = 160,
    MIB_PROF_TACPLUSACCT_CLIENT = 161,
    MIB_PROF_SWAN_LINE_STAT	= 162,
    MIB_PROF_IP_POOLS		= 163,
    MIB_PROF_RAD_STAT		= 164,
    /*
     * 165 - 171 had been assigned to IPSEC but are no longer in use.
     */
    MIB_PROF_SHASH		= 172,
    MIB_PROF_NET_DS3		= 173,
    MIB_PROF_DS3_LINE_STAT	= 174,
    MIB_PROF_IPX_INT            = 175,
    MIB_PROF_IPX_GLOBAL         = 176,
    MIB_PROF_V32_S         	= 177,
    MIB_PROF_B52_TEST		= 178,
                                        /* 970804: 179 not assigned */
    MIB_PROF_SDSLLC		= 180,
    MIB_PROF_SDSLLIF		= 181,
    MIB_PROF_NET_SDSL		= 182,
    MIB_PROF_SDSL_LINE_STAT	= 183,
    MIB_PROF_SDSL_STATISTIC     = 184,
    MIB_PROF_SDSL_STATUS        = 185,
    MIB_PROF_CADSLLC		= 186,
    MIB_PROF_CADSLLIF		= 187,
    MIB_PROF_NET_CADSL		= 188,
    MIB_PROF_CADSL_LINE_STAT	= 189,
    MIB_PROF_CADSL_STATISTIC    = 190,
    MIB_PROF_CADSL_STATUS       = 191,
    MIB_PROF_NAT         	= 192,
    MIB_PROF_NAT_MAP_ENTRY      = 193,
    MIB_PROF_ATALK_INT		= 194,
    MIB_PROF_ATALK_GLOBAL	= 195,
    MIB_PROF_BRI_LINE_STAT      = 196,
    MIB_PROF_ETHER_X            = 197,
    MIB_PROF_TUNNEL_SERVER      = 198,
    MIB_PROF_ATMP		= 199,
    MIB_PROF_IP_ROUTE_FILT	= 200,
    MIB_PROF_IN_TUNNEL          = 201,
    MIB_PROF_DLCI_ID		= 202,
    MIB_PROF_FRPVC_STAT		= 203,
    MIB_PROF_DLCI_STAT		= 204,
    MIB_PROF_IN_X32             = 205,
    MIB_PROF_ETHER_INFO		= 206,
    MIB_PROF_RAD_ACCT_CALL_LOG  = 207,
    MIB_PROF_ANSWER_PLAN	= 208,
    MIB_PROF_DNS_FALLBACK_TAB   = 209,
    MIB_PROF_DNS_FALLBACK_CFG   = 210,
    MIB_PROF_VDSP_S             = 211,
    MIB_PROF_SS7		= 212,
    MIB_PROF_REBOOT_STATISTICS  = 213,
    MIB_PROF_REDUNDANCY_STATISTICS      = 214,
    MIB_PROF_REDUNDANCY_CONTEXT = 215,
    MIB_PROF_REDUNDANCY         = 216,
    MIB_PROF_EXT_TS_HOSTS       = 217,
    MIB_PROF_EXT_TS             = 218,
    MIB_PROF_UDS3LC		= 219,
    MIB_PROF_NET_UDS3		= 220,
    MIB_PROF_UDS3_LINE_STAT	= 221,
    MIB_PROF_VROUTER            = 222,
    MIB_PROF_IPFAX		= 223,
    MIB_PROF_DADSLLC		= 224,
    MIB_PROF_NET_DADSL		= 225,
    MIB_PROF_DADSL_LINE_STAT	= 226,
    MIB_PROF_ADSL_DMTAW_STATUS  = 227,
    MIB_PROF_ADSL_DMTAW_STATISTIC = 228,
    MIB_PROF_IN_ATM             = 229,
    MIB_PROF_DS3_ATMLC		= 230,
    MIB_PROF_NET_DS3_ATM	= 231,
    MIB_PROF_DS3_ATM_LINE_STAT	= 232,
    MIB_PROF_L2TUNNEL_GLOBAL    = 233,
    MIB_PROF_VDSLCC             = 234,
    MIB_PROF_VDSLLC		= 235,
    MIB_PROF_NET_VDSL		= 236,
    MIB_PROF_VDSL_LINE_STAT	= 237,
    MIB_PROF_OSPF_AREA_RANGE    = 238,
    MIB_PROF_OSPF_VIRTUAL_LINK  = 239,
    MIB_PROF_TS_RLOGIN		= 240,
    MIB_PROF_BOOTP_RELAY        = 241,
    MIB_PROF_FEATURES           = 242,
    MIB_PROF_IPTOS_FILT		= 243,
    MIB_PROF_IPTOS		= 244,
    MIB_PROF_SYS_MODEM		= 245,
    MIB_PROF_DEV_SUMMARY	= 246,
    MIB_PROF_VOIP		= 247,
    MIB_PROF_SWAN_CLOCK         = 248,
    MIB_PROF_LOAD_SELECT	= 249,
    MIB_PROF_SNMP_STATE_SEC	= 250,
    MIB_PROF_DNIS_TAB		= 262,

	/* add:Ast@20020401 */
    MIB_PROF_NAT_STATIC		= 273,
    MIB_PROF_FILTER_IN		= 274,
    MIB_PROF_FILTER_OUT		= 275,

	/* mib types for cbq */
    MIB_PROF_CBQ_CLASS		= 276,
    MIB_PROF_CBQ_FILTER		= 277,
    MIB_PROF_SFQ_QDISC		= 278,
    
    /* add by hqguan */
    MIB_PROF_IPSSG_FILT		= 279,
    /* add by chenyunhua*/
    MIB_PROF_TIMERANGE          = 280,
# if (OLDNAT)
    MIB_PROF_NAT_POOL	        = 281,
# else
    MIB_PROF_NAT_BINDING	= 281,
# endif
    /* add by lyu*/
    MIB_PROF_IPSEC_CONF		= 282,
    MIB_PROF_IPSEC_TRANSFORMSET	= 283,
    MIB_PROF_ISAKMP_CONF	= 284,
    MIB_PROF_ISAKMP_POLICY	= 285,

    // add by jxy8@2003-12-26 for WebUserRight
    MIB_PROF_WEB   = 286,
    MIB_PROF_SECURITY_USER   = 287,
    MIB_PROF_SECURITY_GROUP = 288,
    MIB_PROF_SECURITY_URL      = 289,
    //add end jxy8
    
#if (DHCP_PLUS == FYES)
   MIB_PROF_DHCP_RAWOPTION  	= 290,
   MIB_PROF_DHCP_POOL           = 291,
#endif
  
#if (UPNP == FYES)
    MIB_PROF_UPNP               = 292, 	/* add by lyu for upnp */
#endif  
    
#if (FIREWALL == FYES)
    MIB_PROF_FWADDRESS		= 293,  /* add by cyh for firewall */
    MIB_PROF_FWSERVICE		= 294,
    MIB_PROF_FWL7FILTER		= 295,
#endif

#if (PDB == FYES)
    MIB_PROF_PDB		= 296,
#endif

#if (ISP_CFG == FYES)
    MIB_PROF_ISP		= 297,
#endif

#if (SWITCHFUN == FYES)
    MIB_PROF_SWITCH              = 298,  /*add by zgd*/
    MIB_PROF_PORT2               = 299,
    #if (TAGVlAN == FYES)
    MIB_PROF_TAGVLAN             = 300, 
    #endif
#endif

#if (PPPOE_SRV == FYES)		/* added by llliu */
    MIB_PROF_PPPOE_SRV		= 301,
    MIB_PROF_MAC_FILTER		= 302,
#endif

#if(DOT1X == FYES)
    MIB_PROF_DOT1X		= 303,
#endif

#if (SWITCHFUN == FYES)
    MIB_PROF_PORTMAC            = 304,  /*add by zgd*/
#endif

#if(DOT1X == FYES)
     MIB_PROF_DOT1X_USER	= 305,
#endif
#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang */
    MIB_PROF_IP_GROUP           = 306,
    MIB_PROF_PORT_GROUP         = 307,
#endif
#endif
    MIB_PROF_ETHER_FILTER       = 308, /*add by brwang*/

    MIB_PROF_DNS_FILTER         = 309, /* added by llliu */

    MIB_PROF_TIMERANGE_POLICY   = 310, /* brwang  */

#if (POLICY_ROUTE == FYES)
    MIB_PROF_POLICY_ROUTE       = 311, /* added by llliu@081107 */
#endif

    MIB_PROF_PUBLIC_DDNS	= 312, /* added by llliu@081201 */
 
#if(WEB_SERVER == FYES)
    MIB_PROF_ADVERTISE		= 313,	/* added by jfgu@090603 */   
    MIB_PROF_JOIN_STARTMENU 	= 314, /* added by dhyu@090611*/

#endif    


#if (FC == FYES)
    MIB_PROF_RATE_LIMIT		= 315,
#endif

    MIB_PROF_CONNECTION_LIMIT   = 316,	

    /*Internet action management*/
    MIB_PROF_NET_ACTION_MANAGE	= 317,  
    
    /* allow exceptive qq connect internet*/
    MIB_PROF_QQ_EXCEPTION	= 318,

    MIB_PROF_DNS_RESOLVE	= 319,	

#if (PPPOE_SRV == FYES)
    MIB_PROF_PPPOE_ONLY		= 320,	 /*only pppoe connect to internete*/
#endif

#if (NOTICE == FYES)
    MIB_PROF_NOTICE		= 321, /* added by dhyu for notice*/
#endif

    /*  ^^^ _HERE_ Add NEW values (thank you.) ^^^ */
    /*  (e.g., MIB_PROF_NEW_INCREDIBLE_FEATURE	= 999, ) */

    MIB_PROF_MAX
} MibProfileType;

#endif /* MIBTYPE_H */
