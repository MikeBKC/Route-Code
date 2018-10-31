#ifndef NAT_H
#define NAT_H

#define UPNP_CHAIN_WAN  "upnp_nat"
#define UPNP_CHAIN_WAN1 "upnp_chain_wan1"
#define UPNP_CHAIN_WAN2 "upnp_chain_wan2"

#define DMZ_CHAIN                       "DMZ"
#define DMZ_LOOP_CHAIN                 "DMZ_loop" 
#define STATIC_NAT_CHAIN      "static_nat"
#define STATIC_NAT_LOOP_CHAIN     "static_nat_loop"
#define NAT_RULE_CHAIN "nat_rule_loop"
#define NAT_RULE_PRE_CHAIN "nat_rule"
#define DEFAULT_NAT_WAN_CHAIN "default_nat"
extern void iptablesStaticNatclear(void);
#endif
