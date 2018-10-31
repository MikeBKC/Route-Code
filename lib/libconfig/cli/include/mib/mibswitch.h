#if ! defined( MIBSWITCH_H )  /*add by zgd*/

#if(DOT1X == FYES)
#include "mibdot1x.h"
#endif

#define MIBSWITCH_H

#if (SWITCHFUN == FYES)  /* add by zgd */

#if (TAGVlAN == FYES)
#define H_MAXVLANNUM    MAX_TAGVLAN_PROFILES
#define H_MAXVIDNUM     0x1000    //2^12
#define H_MAXVIDMASK    (H_MAXVIDNUM-1)
#define H_MAXIFNUM      0x10000   //2^16
#define H_MAXIFMASK     (H_MAXIFNUM-1)
#define H_INVALIDIFNUM  0xFF
#define H_INVALIDTAG    0

#define H_VLANSTARTIFNUM 6

#define VLANTAG2LINEAR2(macport, vid) ((macport*H_MAXVIDNUM+vid)&H_MAXIFMASK)

extern Uint16 vlanTag2ifIndex2[H_MAXIFNUM];
extern Uint16 ifIndex2vlanTag2[H_MAXVLANNUM+H_VLANSTARTIFNUM];
extern Uint16 vlannum2ifIndex[H_MAXVLANNUM];
extern Uint16 vlannum2vlanTag2[H_MAXVLANNUM];
extern Uint8 vlancompolicy[MAX_TAGVLAN_PROFILES];
extern Uint8 subvlancompolicy[MAX_TAGVLAN_PROFILES];

#endif

typedef struct
{
  Uint32    TxOctets;
  Uint32    RxOctets;
  Uint32    TxUnicastPkts;
  Uint32    RxUnicastPkts;
  Uint32    TxMulticastPkts;
  Uint32    RxMulticastPkts;
  Uint32    TxBroadcastPkts;
  Uint32    RxBroadcastPkts;
  Uint32    TxPausePkts;
  Uint32    RxPausePkts;
  Uint32    TxSingleCollision;
  Uint32    RxFCSErrors;
  Uint32    TxLateCollision;
  Uint32    RxFragments;
  Uint32    RxUndersizePkts;
  Uint32    RxOversizePkts;
  Uint32    RxSAChanges;
  Uint32    Pkts65to127Octets;
  Uint32    Pkts128to255Octets;
  Uint32    Pkts256to511Octets;
  Uint32    Pkts512to1023Octets;
  Uint32    Pktsmorethan1024Octets;
} sn_PortStt;

typedef struct sn_CRTData
{
  Byte result[4];
  Byte len[4];
} sn_CRTData;

typedef struct sn_MacItem
{
  Byte  static0;
  Uint16 port;
  Uint16 Vid;
  Uint8 macAddr[ETHER_ADDR_LEN];
} sn_MacItem;

typedef struct sn_PirTagValue
{
  Byte pritag;
  NPW_PRIVALUE privalue;
} sn_PirTagValue;

#if (TAGVlAN == FYES)
typedef struct
{
  u_long  ip_addr;
  u_short port;
} VlanArpMessage;

typedef struct vlanconfighash
{
  Uint32 ip;
  Uint16 num;
  vlanconfighash *next;
}vlanconfighash;

typedef struct TagVlanProfile
{
  char name[MAX_PROFILE_NAME_LENGTH+1];
  Boolean active;
  //hardware
  Uint16 VlanID;
  NPW_VLANTAGIN tagvlanin[MAX_PORT2_PROFILES];
  NPW_VLANTAGOUT tagvlanout[MAX_PORT2_PROFILES];
  //software
  NPW_PHYNUM phynum;  
  NPW_VLANTYPE vlantype;   //0:normal 1:super vlan 2 sub vlan
  Uint8  Policy;  //bit,belong to which policy
  char supervlanname[MAX_PROFILE_NAME_LENGTH+1];  //belong to which subvlan;
  IpAddress VlanIpAddress;  //route:ip address
  IpAddress VlanIpNetmask;  //route:ip netmask
  
} TagVlanProfile;
#endif

typedef struct Port2Profile
{
  char name[MAX_PROFILE_NAME_LENGTH+1];
  Boolean active;
  
  NPW_MIRRATT MirrAtt;
  NPW_MIRRDIR MirrDir;
  
  Boolean PortEnabled;
  Boolean FlowCrlEnabled;
  Byte    wireSpeed;
  
  NPW_TRUNKNUM TrunkNum;  //0 no 1:Trunk1 2:Trunk2
  
  NPW_INSPEEDMOD InWidthMod;
  Uint32  ul_InBandWidth;
  
  Boolean OutWidthEnabled;
  Uint32  ul_OutBandWidth;
  
  sn_CRTData s_CRTData;
  
  sn_PortStt  s_PortStt;
  
  #if (TAGVlAN == FYES)
  Uint16 PVID;
  #endif

  #if(DOT1X == FYES)
  Dot1xPortProf Dot1xMib;
  #endif
  
  Boolean SafeModEnabled;
  Boolean PortMacBindEnable;
  NPW_PRIVALUE privalue;   //port
  Byte    pritag;   //802.1p
  
} Port2Profile;

typedef struct PortMacProfile
{
  char name[MAX_PROFILE_NAME_LENGTH+3];
  Boolean active;
  
  char username[MAX_PROFILE_NAME_LENGTH+3]; 
  NPW_PORTMACTYPE type; 
  Boolean filtenable;
  int vid;
  Byte port;
  Byte macAddr[ETHER_ADDR_LEN];
  NPW_PORTMACPORTIN portinclude[MAX_PORT2_PROFILES];
  
} PortMacProfile;

typedef struct SwitchProfile
{
  char name[MAX_PROFILE_NAME_LENGTH+1];
  Boolean active;
  
  Boolean MirrEnabled;
  NPW_FILTER MirrInFilter;
  NPW_FILTER MirrOutFilter;
  Uint8   uc_IngressMac[ETHER_ADDR_LEN];
  Uint8   uc_EgressMac[ETHER_ADDR_LEN];
  Boolean Trunkenabled;
  NPW_TRUNKMOD TrunkMod;
  sn_PortStt  s_PortStt;
  
  #if (TAGVlAN == FYES)
  Boolean VlanComPolicy;
  Boolean SubVlanComPolicy;
  NPW_VLANMODE     VlanMode[5]; //0:none 1:port 2:tag
  NPW_VLANCOMMODE  VlanComMode[5]; //0:none 1:arp agent 2:route
  NPW_VLANTAGOUT   VlanWanTagOut[4];  //wan
  #endif
  
  Uint16 MacOldTim;
  NPW_PRIMODE PriMode; //0:none 1:port 2:8021p 3:dscp
  NPW_PRIRULE PriRule;  //0:scale 1:fix
  sn_PirTagValue PriTagVlaue[8];  //7 dscp other Tag,vlaue
  
} SwitchProfile;

#endif
#endif //MIBSWITCH_H
