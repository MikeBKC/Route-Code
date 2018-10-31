#ifndef MIB_SW_PORT_CONF_SHARE_H
#define MIB_SW_PORT_CONF_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct st_swPortConf_profile {
    mibHead              head;
    char                 name[UTT_INST_NAME_LEN + 1];
#ifdef CONFIG_VSC7390
    Uint32               max_frame;/*允许最大帧*/
#endif
    SwPortConfMode       port_mode;/*端口模式*/
    ProfileEnableEnum    flow_control;/*流控*/
}SwPortConfProfile;

#endif
