/*
 *switch HAL API struct
 *
 *KeHuatao
 *2011-07-26
 */
#ifndef _OS_SW_HWAPI_H_
#define _OS_SW_HWAPI_H_

#include <linux/skbuff.h>
#include <linux/switch/switch.h>
#include <linux/switch/swioc_common.h>

#define  HW_API_SUCCESS 0
#define  HW_API_GEN_ERR 1

typedef struct _hwApi_t{
    int32 (*pCmpIfName)(const char* name);
    int32 (*pGetIfindex)(void);

    int32 (*pMacAdd) (unsigned char* ifMask, uint32 vid, unsigned char *pMac);
    int32 (*pMacDel) (uint32 vid,unsigned char *pMac);

    int32(*pMacTableAgeTimeSet)(uint32 age_time);
    int32(*pMacTableAgeTimeGet)(uint32 *age_time);
    int32(*pMacAgeEnableSet)(uint32 enable);
    int32(*pMacAgeEnableGet)(uint32 *enable);


#if SW_FEATURE_PORT_VLAN
    /*vlan*/
    int32(*pPortVlanMembersSet)(uint32 pvlan_no, unsigned char* pvlan_member);
#if 0
    int32(*pPortVlanMembersGet)(uint32 pvlan_no, unsigned char* pvlan_member);
#endif
#endif

    int32(*pVlanTypeSet)(uint32 vlan_mode);
    uint32(*pVlanTypeGet)(void);

    void (*pMacAge)(void);
    void (*pUpdateMacTable)(void);
    void (*pDisplayMacTable)(void);
    uint32 (*pGetDstPortMask)(unsigned char *mac);
    int32 (*pollMac)(void);
    void (*pollPort)(void);
#if !SW_BC_CPU_CP
    void (*pSetIpAddr)(uint32 ip);
#endif
    void (*pSetMacAddr)(
#ifdef CONFIG_VSC7390 
	    unsigned char *old_mac, 
#endif
	    unsigned char *new_mac);

    int32 (*pIfRcvPkt)(uint32 ifUnit,struct sk_buff *pSkb);
    int32 (*pIfLinkChange)(uint32 ifUnit, int32 linkState);
    int32 (*pInsertSlot)(int32 slot);
    int32 (*pExtractSlot)(int32 slot);

#if SW_FEATURE_PORT_CONF
    /*¶Ë¿Ú¹ÜÀí*/
    int32 (*pPortConfSet)(port_no_t port_no, 
	    BOOL    _autoneg, 
	    speed_t _speed, 
	    BOOL    _fdx, 
	    BOOL    _enable, 
	    BOOL    _flow_control, 
#ifdef CONFIG_VSC7390
	    uint    _max_length,
#endif
	    BOOL    _protect,
	    BOOL    _flooding);
    int32(*pLinkStatusGet)(port_no_t port_no, char* link_status);
#endif

#if SW_FEATURE_ACL
    int32 (*pAclEnable)(sw_acl_enable_t* acl_enable);
    int32 (*pAclStatusGet)(sw_acl_enable_t* acl_enable);
    int32 (*pAclAdd)(sw_acl_t* acl);
    int32 (*pAclDel)(int32 acl);
    int32 (*pAclGet)(sw_acl_t* acl);
#endif

/* mirror */
#if SW_FEATURE_PORT_MIRROR
    int32 (*pMirrorEgressIngressPortModeSet)(BOOL port_no[PORT_ARRAY_SIZE], int m_mode);
    int32 (*pMirrorEgressIngressPortModeGet)(BOOL port_no[PORT_ARRAY_SIZE], int *m_mode);
    int32 (*pMirrorMonitorPortSet)(uint32 port_no);
    int32 (*pMirrorMonitorPortGet)(uint32 *port_no);
#endif

    /* Trunk */
#if SW_FEATURE_PORT_TRUNK
    int32 (*pTrunkGroupSet)(int32 group_no, BOOL members[PORT_ARRAY_SIZE]);
    int32 (*pTrunkGroupGet)(int32 group_no, BOOL members[PORT_ARRAY_SIZE]);
    int32 (*pTrunkGroupGetByPort)(port_no_t pno, BOOL members[PORT_ARRAY_SIZE]);
    int32 (*pTrunkModeSet)(int32 value);
    int32 (*pTrunkModeGet)(int32 *value);
#endif

#if SW_FEATURE_PACKET
#if 0
    int32 (*pBcpDstPortsGet)(port_no_t exclude_port, BOOL dst_ports[PORT_ARRAY_SIZE]);
#endif
    int32 (*pFwdDstPortsGet)(port_no_t exclude_port, vid_t* vid, unsigned char* dest_mac,BOOL vlan,BOOL dst_ports[PORT_ARRAY_SIZE]);
#endif

#if SW_FEATURE_PORT_STATISTICS 
    int32 (*pPortCounterGet)(port_no_t port_no, sw_port_stat_t * stat);
    int32 (*pPortCounterClear)(port_no_t port_no);
#endif
#if SW_FEATURE_MAC_TABLE_OPT
    void (*pShowMacTable)(void);
#endif
#if SW_FEATURE_PORT_PRIORITY
    int32 (*pPortPrioritySet)(uint32 portPrio[PORT_ARRAY_SIZE]);
#endif
#if SW_FEATURE_STORM_CONTROL
    int32 (*pBroadcastStormControlSet)(BOOL ctrl_port[PORT_ARRAY_SIZE], uint32 value, BOOL timer_sel);
#endif
#if SW_FEATURE_RATE_LIMIT
    int32 (*pPortBandwidthFuncSet)(port_no_t port_no, uchar type, uint32 tx_rate, uint32 rx_rate);
    int32 (*pPortBandwidthThrottleSet)(uchar throttle);
#endif
    int32 sw_ifindex; /*switch interface*/

}hwApi_t;

extern hwApi_t gHwFuncs;
#endif
