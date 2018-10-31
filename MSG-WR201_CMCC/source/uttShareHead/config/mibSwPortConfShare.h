#ifndef MIB_SW_PORT_CONF_SHARE_H
#define MIB_SW_PORT_CONF_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

typedef struct st_swPortConf_profile {
    mibHead              head;
    char                 name[UTT_INST_NAME_LEN + 1];
#ifdef CONFIG_VSC7390
    Uint32               max_frame;/*�������֡*/
#endif
    SwPortConfMode       port_mode;/*�˿�ģʽ*/
    ProfileEnableEnum    flow_control;/*����*/
}SwPortConfProfile;

#endif
