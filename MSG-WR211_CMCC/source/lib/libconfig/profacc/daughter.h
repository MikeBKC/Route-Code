

#ifndef __DAUGHTER_H
#define __DAUGHTER_H

extern bool BuildDefaultFactoryValue(MibProfileType proftype, int instindex, SlotNumber slot,Pointer *_prof);

extern SystemProfile* buildFactorySystemProfile(ProfileUnion *unionProf);
#if (IP_GRP == FYES)
extern IpGrpProfile* buildFactoryIpGrpProfile(ProfileUnion *unionProf);
#endif

#if (PORT_GRP == FYES)
extern PortGrpProfile* buildFactoryPortGrpProfile(ProfileUnion *unionProf);
#endif

#if (TIME_GRP == FYES)
extern TimeGrpProfile* buildFactoryTimeGrpProfile(ProfileUnion *unionProf);
#endif

#if (DHCP_SERVER == FYES)
extern DhcpServerProfile* buildFactoryDhcpServerProfile(ProfileUnion *unionProf);
#endif
extern UserProfile* buildFactoryUserProfile(ProfileUnion *unionProf);

#if (L2TP_SERVER == FYES)
extern L2TPSrvGlobalProfile *buildFactoryL2TPSrvGloProfile(ProfileUnion *unionProf);
extern L2TPSrvAccProfile *buildFactoryL2TPSrvAccProfile(ProfileUnion *unionProf);
#endif

#if (L2TP_CLIENT == FYES)
L2TPCliProfile *buildFactoryL2TPCliProfile(ProfileUnion *unionProf);
#endif

#if (PPTP_SERVER == FYES)
extern PPTPSrvGlobalProfile *buildFactoryPPTPSrvGloProfile(ProfileUnion *unionProf);
extern PPTPSrvAccProfile *buildFactoryPPTPSrvAccProfile(ProfileUnion *unionProf);
#endif

#if (PPTP_CLIENT == FYES)
extern PPTPCliProfile *buildFactoryPPTPCliProfile(ProfileUnion *unionProf);
#endif

#if (STATIC_ROUTE == FYES)
extern StaticRouteProfile *buildFactoryStaticRouteProfile(ProfileUnion *unionProf);
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
extern MirrorProfile* buildFactoryMirrorProfile(ProfileUnion *unionProf);
#endif
#if (GROUP_BASE == FYES)
extern GroupBaseProfile* buildFactoryGroupBaseProfile(ProfileUnion *unionProf);
#endif

#if (FEATURE_SW_PORT_TRUNK == FYES)
extern TrunkProfile* buildFactoryTrunkProfile(ProfileUnion *unionProf);
#endif

#if (UTTPDB == FYES)
extern PdbProfile* buildFactoryPdbProfile(ProfileUnion *unionProf);
#endif
extern bool BuildDefaultFactoryValue(MibProfileType proftype,int instindex,
	   	SlotNumber slot,Pointer *_prof);
#if (FEATURE_ACL == FYES)
extern AclProfile* buildFactoryAclProfile(ProfileUnion *unionProf);
extern AclStatusProfile* buildFactoryAclStatusProfile(ProfileUnion *unionProf);
#endif

#if (WIRELESS == FYES)
extern WirelessProfile *buildFactoryWirelessProfile(ProfileUnion *unionProf);
extern WirelessMacFilterProfile *buildFactoryWirelessMacFilterProfile(ProfileUnion *unionProf);
#if (WIRELESS_5G == FYES)
extern Wireless5gMacFilterProfile *buildFactoryWireless5gMacFilterProfile(ProfileUnion *unionProf);
#endif
#endif

#if (FEATURE_AP_SW_MNG == FYES)
extern ApSwMngProfile* buildFactoryApSwMngProfile(ProfileUnion *unionProf);
#endif

#endif


