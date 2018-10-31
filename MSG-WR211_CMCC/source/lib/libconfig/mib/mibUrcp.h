#ifndef __MIB_URCP_H__
#define __MIB_URCP_H__



/*urcp_dev_t.dev_type*/
#define DEV_TYPE_SWITCH      0x00u
#define DEV_TYPE_ROUTER      0x01u
#define DEV_TYPE_AC          0x02u
#define DEV_TYPE_FAT_AP      0x04u
#define DEV_TYPE_FIT_AP      0x08u
#define DEV_TYPE_WIRELESS    0x10u
#define DEV_TYPE_DRAWING_MANAGED    0x20u  /*该位1,则已被建筑图纸管理,0未被管理*/
#define DEV_TYPE_WIRELESS_5G 0x40u	/*5G标志*/
#define DEV_FLAG_AUTHED      0x80000000u  /*authorize*/
#define DEV_FLAG_MANAGE      0x40000000u  /*manage*/
/*urcp_dev_t.dev_type end*/


/*urcp_dev_t.wl_info.ap_flag*/
#define AP_PROFILE_LOCAL     0x0001u
#define AP_PROFILE_DOWNLOAD  0x0002u
#define AP_MODE_11BGN  0x0100u
#define AP_MODE_11B    0x0200u
#define AP_MODE_11G    0x0400u
#define AP_MODE_11N    0x0800u
#define AP_MODE_11A    0x1000u
#define AP_MODE_11BG   0x2000u
#define AP_MODE_11AN   0x4000u
#define AP_MODE_11GN   0x8000u
#define AP_MODE_11AC_AN_A   0x10000u
#define AP_MODE_11AC_AN     0x20000u
/*urcp_dev_t.wl_info.ap_flag end*/

/*MIB_PROF_AP_EFFECT_CONF*/
#define AP_CONF_AUTO   0
#define AP_CONF_LOCAL  1
/*MIB_PROF_AP_EFFECT_CONF en*/

#define RF_FLAG_24G      0x0 
#define RF_FLAG_5G       0x20
#define RF_FLAG_24G_5G   0x30

#endif
