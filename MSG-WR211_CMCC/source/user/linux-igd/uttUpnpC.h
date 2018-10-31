
#ifndef _CONFIG_UTTUPNP_
#define _CONFIG_UTTUPNP_

struct utt_Upnp_data{
    char action[0x10];
    char protocol[0x10];
    char dport[0x10];
    char innerIp[0x10];
    char innerPort[0x10];
};

#endif 
