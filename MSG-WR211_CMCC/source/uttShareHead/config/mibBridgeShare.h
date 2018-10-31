#ifndef MIBBRIDGE_SHARE_H
#define MIBBRIDGE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

/*
 *桥接口结构体
 * 
 */
typedef struct st_bridge_interface
{
	FunEnableEnum enable;
	Uchar ifName[UTT_INST_NAME_LEN+1];
	MacAddr mac;
};

typedef struct st_bridgeProfile
{
	mibHead head;
	IPADDR ip;
	IPADDR nm;
	IPADDR gw;
	IPADDR dns1;
	IPADDR dns2;
	struct st_bridge_interface interface[MAX_INTERFACE_PROFILES];
}BridgeProfile;

#endif	
	
