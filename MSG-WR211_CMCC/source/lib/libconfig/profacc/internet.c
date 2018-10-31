#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/types.h>

#include    <stdlib.h>
#include <linux/autoconf.h>

#ifndef CONFIG_RALINK_RT2880
#include    <net/if.h>
#endif
#include    <net/route.h>
#include    <dirent.h>
//
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/mii.h>
#include <linux/sockios.h>

#ifdef CONFIG_RALINK_RT2880
typedef unsigned long long u64; /* hack, so we may include kernel's ethtool.h */
typedef uint32_t u32;           /* ditto */
typedef uint16_t u16;           /* ditto */
typedef uint8_t u8;             /* ditto */
#endif
#include <linux/ethtool.h>

//#include <linux/if.h>

//#include <autoconf.h>
//#include <net/if.h>

#ifdef CONFIG_PPC_85xx
#include <fcntl.h>
#include <sys/stat.h>
#include "uttRtChipApi.h"
#endif

#if CONFIG_RALINK_RT3052 || CONFIG_RALINK_RT3883 || CONFIG_RALINK_RT5350 || CONFIG_RALINK_MT7620|| CONFIG_RALINK_MT7621 || CONFIG_RALINK_MT7628
#include "ra_ioctl.h"
#endif
#define IF_NAMESIZE 16
#define DEV_PPP_IF_NAME "/var/run/%s.ppp"
#define MAX_FILE_NAME_LEN 256
#define MAX_PPP_NAME_LEN 64U
/*
 * get wan interface count
 */
int getWanSlotCount(void)
{
#if (USER_DEF_WAN_COUNT == FYES)
    if(nvramProfile->systemProfile.wanIfCount == 0) {
        return MAX_WAN_IF_COUNT;
    } else {
        return nvramProfile->systemProfile.wanIfCount;
    }
#else 
    return MAX_WAN_IF_COUNT;
#endif
}
//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)
 /**
  * 获取无线接入点 端口名称
  * */
Uint8* getApIfName()
{
    static char* if_name = "ra0";
    return if_name;
}
/**
 * 获取lan口所对应的eth name（如eth2而非br0）
 */
Uint8* getEthLanIfName()
{ static char* if_name = "eth2"; return if_name;
}
#endif
char* getLanIfName(void)
{
#if (UTT_NV_1800N == FYES)
    static char *if_name = "br0";
#else
//#if|| (UTT_NV_510V2 == FYES)
#if (defined(UTT_NV_1540) || (UTT_NV_3520G == FYES) || (UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES)|| (UTT_NV_640EV2 == FYES)  || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES) || (UTT_NV_810V2 == FYES) || (UTT_NV_810V3 == FYES)|| (UTT_NV_830V2 == FYES) || (UTT_NV_840V2 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_WX1000 == FYES)|| (UTT_NV_518 == FYES) || (UTT_NV_640E == FYES) || (UTT_NV_D910 == FYES) || (UTT_NV_WX20S== FYES) || (UTT_U_1100 == FYES) || (UTT_U_1900 == FYES))
    static char *if_name = "eth2.1";
#elif ((UTT_MV_821 == FYES) || (UTT_MV_841 == FYES) || (UTT_QV_4320G == FYES) || (UTT_QV_4240G == FYES) || (UTT_QV_4220G == FYES)|| (UTT_QV_3640 == FYES) || (UTT_U_3000 == FYES) || (UTT_U_4000 == FYES) || (UTT_QV_N800 == FYES)  ||(UTT_QV_NE1200 == FYES) || (UTT_QV_4840 == FYES)  || (UTT_QV_N1800 == FYES) || (UTT_QV_2620G == FYES) || (UTT_QV_2610G == FYES) || (UTT_QV_3320G == FYES)||(UTT_QV_1210G == FYES)|| (UTT_QV_1220G == FYES))  
    static char *if_name = "eth0.1";
#elif ((UTT_NV_510W == FYES)||(UTT_NV_A755W == FYES ) || (UTT_NV_520W == FYES)  || (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_842W == FYES)|| (UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES)|| (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES) ||(UTT_U_1000N == FYES)|| (UTT_NV_518WPLUS == FYES)|| (UTT_NV_518WLITE == FYES)|| (UTT_NV_518WVPN == FYES) || (UTT_NV_750W == FYES) ||  (UTT_NV_A750W == FYES) || (UTT_NV_A650W == FYES)|| (UTT_NV_WBR_8001 == FYES) || (UTT_NV_521GW == FYES)||(UTT_NV_512WV2 == FYES) || (UTT_NV_515W == FYES) || (UTT_NV_518WV2 == FYES) || (UTT_NV_521GV2 == FYES)  || (UTT_NV_840GV2 == FYES)|| (UTT_NV_750GW == FYES) || (UTT_NV_842WV3 == FYES) || (UTT_NV_830V3 == FYES) || (UTT_NV_811V4 == FYES) || (UTT_NV_821V2 == FYES) || (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_A650W_INT == FYES) || (UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES))||(UTT_NV_517W == FYES) || (UTT_NV_U1000NV2 == FYES) || (UTT_NV_D904W == FYES) ||(UTT_NV_A655W == FYES)  || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES)||(UTT_NV_A658W == FYES)||(UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES) || (UTT_NV_D908W == FYES) || defined(CONFIG_BOARD_MTK7628NN_A_V1_0)
    static char *if_name = "br0";
#elif ((UTT_NV_840G == FYES) || (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_3320G == FYES) || (UTT_NV_840G_INT == FYES)||(UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)|| (UTT_NV_810V4 == FYES))
    static char *if_name = "eth2.1";
#elif (UTT_NV_WX100 == FYES)
    static char *if_name = "eth2";
#elif (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
    static char *if_name = "eth2";
#else
    static char *if_name = "eth0";
#endif
#endif
    return if_name;
}
    
/********************************************************************
 * 函数： StrTransform
 * 功能： 把字符串中所有旧字符替换成新字符
 * 创建： 2012-04-19
 * 参数： 
 * StrTrans 待转换字符串 
 * OldSplit 旧字符 
 * NewSplit 新字符
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int StrTransform(char *StrTrans,char *OldSplit,char *NewSplit)
{
    int i = 0;
    int size = 0;
    size = strlen(StrTrans);
    for(i = 0; i < size; i++)
    {
      if(*(StrTrans + i) == *OldSplit)
      {
          *(StrTrans + i) = *NewSplit;
      }
    }
    return 0;
}
char* get520WanIfName(int num)
{
    static char *if_520_name = NULL;
    int wannumber = 0;
    wannumber = getWanSlotCount();
    switch (num) {
	case 2:
	    if (wannumber == 2) {
		if_520_name = "apcli0";
	    } else {
		if_520_name = "eth2.3";
	    }
	    break;
	case 3:
	    if (wannumber == 3) {
		if_520_name = "apcli0";
	    } else {
		if_520_name = "eth2.4";
	    }
	    break;
	case 4:
	    if (wannumber == 4) {
		if_520_name = "apcli0";
	    } else {
		if_520_name = "eth2.5";
	    }
	    break;
	case 5:
	    if_520_name = "apcli0";
	    break;
	default:
	    break;
    }
    return if_520_name;
}
/*3G、 无线（apclient）和动态LANWAN同时生效获取wan口名字*/
char *get3GWanIfName(int num)
{
	static char *if_3G_WAN_name = NULL;
	int wannumber = 0;
	wannumber = getWanSlotCount();
	switch (num) {
		case 2:
			if (wannumber == 3) {
				if_3G_WAN_name = "ttyUSB0";
			} else {
				if_3G_WAN_name = "eth2.3";
			}
			break;
		case 3:
			if (wannumber == 3) {
				if_3G_WAN_name = "apcli0";
			} else if(wannumber == 4) {
				if_3G_WAN_name = "ttyUSB0";
			} else{
				if_3G_WAN_name = "eth2.4";
			}
			break;
		case 4:
			if (wannumber == 4) {
				if_3G_WAN_name = "apcli0";
			} else if(wannumber == 5) {
				if_3G_WAN_name = "ttyUSB0";
			} else {
				if_3G_WAN_name = "eth2.5";
			}
			break;
		case 5:
			if (wannumber == 5) {
				if_3G_WAN_name = "apcli0";
			} else if(wannumber == 6) {
				if_3G_WAN_name = "ttyUSB0";
			}
			break;
		case 6:
			if_3G_WAN_name = "apcli0";
			break;
		default:
			break;
	}
	return if_3G_WAN_name;
}
char* getWanIfName(int num)
{
    static char *if_name = NULL;
#if (UTT_NV_1800N == FYES)
	    if_name = "br0";
#else
    if(num < 1 || num > getWanSlotCount())
        return NULL;

    switch(num) {
        case 1:
#if (defined(UTT_NV_1540) ||(UTT_NV_A755W == FYES)|| (UTT_NV_3520G == FYES) || (UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_510W == FYES) || (UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES)|| (UTT_NV_640EV2 == FYES)  || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES) || (UTT_NV_810V2 == FYES) || (UTT_NV_810V3 == FYES)|| (UTT_NV_830V2 == FYES) || (UTT_NV_840V2 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_WX1000 == FYES) || (UTT_NV_520W == FYES) || (UTT_U_1000N == FYES) || (UTT_NV_A308W == FYES)|| (UTT_NV_A310Wv2 == FYES) || (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_518 == FYES)|| (UTT_NV_842W == FYES)|| (UTT_NV_518WPLUS == FYES)|| (UTT_NV_518WLITE == FYES) || (UTT_NV_640E == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_A750W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_521GW == FYES) || (UTT_NV_512WV2 == FYES) || (UTT_NV_515W == FYES) || (UTT_NV_518WV2 == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750GW == FYES) || (UTT_NV_842WV3 == FYES) || (UTT_NV_810V4 == FYES)|| (UTT_NV_830V3 == FYES)|| (UTT_NV_811V4 == FYES)|| (UTT_NV_821V2 == FYES)|| (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_A650W_INT == FYES) || (UTT_NV_D910 == FYES) || (UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)||(UTT_NV_517W == FYES) || (UTT_NV_WX20S== FYES) || (UTT_U_1100 == FYES) || (UTT_U_1900 == FYES) || (UTT_NV_U1000NV2 == FYES) || (UTT_NV_D904W == FYES)  ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES) ||(UTT_NV_A658W == FYES)||(UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES)|| (UTT_NV_D908W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
	    if_name = "eth2.2";
#elif (UTT_MV_821 == FYES) || (UTT_MV_811 == FYES) || (UTT_MV_841 == FYES)
	    if_name = "eth1";
#elif (UTT_LV_5830G == FYES || UTT_LV_6530G == FYES || UTT_LV_4330G == FYES || UTT_LV_N8000 == FYES || UTT_LV_NE6000 == FYES || UTT_LV_6830G == FYES || UTT_WX_2000 == FYES)
	    if_name = "eth1";
#elif ((UTT_NV_840G == FYES) || (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_WX100 == FYES) || (UTT_NV_3320G == FYES))
	    if_name = "eth3";
#elif (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	    if_name = "eth3.1";
#elif (UTT_QV_4320G == FYES) || (UTT_QV_2620G == FYES) || (UTT_QV_2610G == FYES) || (UTT_QV_3320G == FYES) || (UTT_QV_1210G == FYES)|| (UTT_QV_1220G == FYES)
	    if_name = "eth0.2";
#elif (UTT_QV_4220G == FYES) || (UTT_QV_N1800 == FYES)
	    if_name = "eth1.2";
#elif (UTT_QV_3640 == FYES) || (UTT_U_3000 == FYES) || (UTT_U_4000 == FYES) || (UTT_QV_N800 == FYES)||(UTT_QV_NE1200 == FYES) || (UTT_QV_4840 == FYES)
	    if_name = "eth1.2";
#elif (UTT_QV_4240G == FYES)
	    if_name = "eth1.2";
#elif (UTT_NV_840G_INT == FYES)
	    if (getWanSlotCount() > 1) {
		if_name = "eth3.2";
	    } else {
#if (TAG_VLAN == FYES)
#if (WAN_TAG_VLAN == FYES)
#define WAN_TAG_SLOT_LEN 16
		static char wanTagVlanSlot[WAN_TAG_SLOT_LEN + 1] = {0};
		WanTagVlanProfile *wanTagVlanProf = NULL;
		wanTagVlanProf = (WanTagVlanProfile *)ProfGetInstPointByIndex(MIB_PROF_WAN_TAG_VLAN, 0);
		if (wanTagVlanProf->head.active == TRUE) {
		    snprintf(wanTagVlanSlot, WAN_TAG_SLOT_LEN + 1, "eth3.%d", wanTagVlanProf->vid);
		    if_name = wanTagVlanSlot;
		} else 
#endif
#endif
		{
		    if_name = "eth3";
		}
	    }
#endif
            break;
        case 2:
#if (defined(UTT_NV_1540) ||(UTT_NV_A755W == FYES )|| (UTT_NV_3520G == FYES) || (UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES)|| (UTT_NV_640EV2 == FYES) || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES) || (UTT_NV_840V2 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_WX1000 == FYES)|| (UTT_NV_518 == FYES) || (UTT_NV_640E == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_A750W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_521GW == FYES) || (UTT_NV_521GV2 == FYES)  || (UTT_NV_840GV2 == FYES) || (UTT_NV_750GW == FYES) || (UTT_NV_842WV3 == FYES)|| (UTT_NV_821V2 == FYES)|| (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_A650W_INT == FYES) || (UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_512WV2 == FYES) || (UTT_NV_518WV2 == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES) || (UTT_NV_515W == FYES) || (UTT_NV_WX20S== FYES) || (UTT_NV_U1000NV2 == FYES) ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES) ||(UTT_NV_A658W == FYES)||(UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES)|| (UTT_NV_D908W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
	    if_name = "eth2.3";
#elif (UTT_MV_821 == FYES) 
	    if_name = "eth0.2";
#elif (UTT_MV_841 == FYES) 
	    if_name = "eth0.4";
#elif (UTT_LV_5830G == FYES || UTT_LV_6530G == FYES || UTT_LV_4330G == FYES || UTT_LV_N8000 == FYES || UTT_LV_NE6000 == FYES || UTT_LV_6830G == FYES || UTT_WX_2000 == FYES)
	    if_name = "eth2";
#elif ((UTT_NV_510W == FYES))
	    if_name = "ttyUSB0";
#elif ((UTT_NV_840G == FYES) || (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_3320G == FYES))
	    if_name = "eth2.2";
#elif (UTT_NV_WX100 == FYES)
	    if_name = "eth3.1";
#elif (UTT_NV_840G_INT == FYES)
	    if_name = "eth3.3";
#elif ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES)|| (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_D904W == FYES))
	    if_name = get520WanIfName(2);
#elif (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	    if_name = "eth3.2";
#elif (UTT_QV_4320G == FYES) || (UTT_QV_2620G == FYES) || (UTT_QV_3320G == FYES) || (UTT_QV_1210G == FYES)|| (UTT_QV_1220G == FYES)
	    if_name = "eth0.3";
#elif (UTT_QV_4220G == FYES) || (UTT_QV_N1800 == FYES)
	    if_name = "eth1.3";
#elif (UTT_QV_4240G == FYES)
	    if_name = "eth1.3";
#elif (UTT_QV_3640 == FYES) || (UTT_U_3000 == FYES) || (UTT_U_4000 == FYES)|| (UTT_QV_N800 == FYES)  ||(UTT_QV_NE1200 == FYES)|| (UTT_QV_4840 == FYES)
	    if_name = "eth1.3";
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	    if_name = get3GWanIfName(2);
#endif
            break;
        case 3:
#if( (defined(UTT_NV_1540))||(UTT_NV_A755W == FYES ) || (UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES)|| (UTT_NV_640EV2 == FYES) || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES) || (UTT_NV_840V2 == FYES)|| (UTT_NV_840V3 == FYES) || (UTT_NV_518 == FYES) || (UTT_NV_640E == FYES)  || (UTT_NV_750W == FYES)|| (UTT_NV_A750W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_521GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750GW == FYES) || (UTT_NV_842WV3 == FYES)|| (UTT_NV_821V2 == FYES)|| (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_A650W_INT == FYES) || (UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_512WV2 == FYES) || (UTT_NV_518WV2 == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES) || (UTT_NV_WX20S== FYES) || (UTT_NV_U1000NV2 == FYES) ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES) ||(UTT_NV_A658W == FYES)||(UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
	    if_name = "eth2.4";
#elif (UTT_LV_5830G == FYES || UTT_LV_6530G == FYES || UTT_LV_4330G == FYES || UTT_LV_N8000 == FYES || UTT_LV_NE6000 == FYES || UTT_LV_6830G == FYES || UTT_WX_2000 == FYES)
	    if_name = "eth3";
#elif (UTT_NV_510W == FYES)
	    if_name = "apcli0";
#elif (UTT_NV_515W == FYES) || (UTT_NV_D908W == FYES)
	    if_name = "ttyUSB0";
#elif ((UTT_NV_840G == FYES) || (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_3320G == FYES))
	    if_name = "eth2.3";
#elif (UTT_NV_WX100 == FYES)
	    if_name = "eth3.2";
#elif (UTT_NV_840G_INT == FYES)
	    if_name = "eth3.4";
#elif (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	    if_name = "eth3.3";
#elif (UTT_MV_841 == FYES)
	    if_name = "eth0.3";
#elif ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES)|| (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_D904W == FYES))
	    if_name = get520WanIfName(3);
#elif (UTT_QV_4320G == FYES) || (UTT_QV_3320G == FYES)
	    if_name = "eth0.4";
#elif (UTT_QV_4220G == FYES) || (UTT_QV_N1800 == FYES)
	    if_name = "eth1.4";
#elif (UTT_QV_4240G == FYES)
	    if_name = "eth1.4";
#elif (UTT_QV_3640 == FYES) || (UTT_U_3000 == FYES) || (UTT_U_4000 == FYES)|| (UTT_QV_N800 == FYES) ||(UTT_QV_NE1200 == FYES)|| (UTT_QV_4840 == FYES)
	    if_name = "eth1.4";
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	    if_name = get3GWanIfName(3);
#endif
            break;
        case 4:
#if( defined(UTT_NV_1540)||(UTT_NV_A755W == FYES) || (UTT_NV_510V2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES)|| (UTT_NV_640EV2 == FYES) || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES) || (UTT_NV_840V2 == FYES)|| (UTT_NV_840V3 == FYES) || (UTT_NV_518 == FYES) || (UTT_NV_640E == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_A750W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_521GW == FYES) || (UTT_NV_521GV2 == FYES)  || (UTT_NV_840GV2 == FYES) || (UTT_NV_750GW == FYES) || (UTT_NV_842WV3 == FYES)|| (UTT_NV_821V2 == FYES)|| (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_A650W_INT == FYES) || (UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_512WV2 == FYES) || (UTT_NV_518WV2 == FYES) || (UTT_NV_840DV3 == FYES)|| (UTT_NV_840EV3 == FYES) || (UTT_NV_WX20S == FYES) || (UTT_NV_U1000NV2 == FYES) ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES)||(UTT_NV_A658W == FYES)||(UTT_NV_1220GW == FYES)|| (UTT_NV_1220V2 == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
	    if_name = "eth2.5";
#elif ((UTT_NV_840G == FYES) || (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_3320G == FYES))
	    if_name = "eth2.4";
#elif (UTT_NV_WX100 == FYES)
	    if_name = "eth3.3";
#elif (UTT_NV_840G_INT == FYES)
	    if_name = "eth3.5";
#elif (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	    if_name = "eth3.4";
#elif (UTT_MV_841 == FYES)
	    if_name = "eth0.2";
#elif ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES)|| (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_D904W == FYES))
	    if_name = get520WanIfName(4);
#elif (UTT_QV_4320G == FYES) || (UTT_QV_3320G == FYES)
	    if_name = "eth0.5";
#elif (UTT_QV_4220G == FYES) || (UTT_QV_N1800 == FYES)
	    if_name = "eth1.5";
#elif (UTT_QV_4240G == FYES)
	    if_name = "eth0.5";
#elif (UTT_QV_3640 == FYES)||(UTT_U_3000 == FYES)|| (UTT_U_4000 == FYES) || (UTT_QV_N800 == FYES) ||(UTT_QV_NE1200 == FYES)|| (UTT_QV_4840 == FYES)
	    if_name = "eth0.5";
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	    if_name = get3GWanIfName(4);
#endif
            break;
	case 5:
#if ((UTT_NV_520W == FYES)||(UTT_U_1000N ==FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A310Wv2 == FYES)|| (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_D904W == FYES))
	    if_name = get520WanIfName(5);
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	    if_name = get3GWanIfName(5);
#endif
            break;
#if ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	case 6:
	    if_name = get3GWanIfName(6);
			break;
#endif
        default:
            break;
    }
#endif
#if (UTT_BRIDEG_MODE_FLAG==FYES)
    if_name = "br1";
#endif
    return if_name;
}

/***********************************************************************
 * 函 数 名：   getWanIfNamePPP
 * 功能描述：	取出wan口name,如果是ppp则取ppp name
 * 输入参数:    
 * 输出参数：	
 * 返回值：     wanifnameppp
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
char* getDevicePPPIfName(char* dev)
{
    FILE *fp;
    char buf[MAX_FILE_NAME_LEN] = {0};
    //unsigned int i = 0U, tmpLen = 0U;
    char* tmp;
    static char pppName[MAX_PPP_NAME_LEN] = {0};

    snprintf(buf, MAX_FILE_NAME_LEN, DEV_PPP_IF_NAME, dev);

    fp = fopen(buf, "r");
    if(fp != NULL) {
        fgets(buf, MAX_FILE_NAME_LEN, fp);
        fclose(fp);
        sscanf(buf, "%s", pppName);
        tmp = pppName;
    } else {
        tmp = "";
    }
    return tmp;
}

char* getWanIfNamePPP(int num)
{
    char buf[256] = {0};
    char *cm = NULL, *tmp = "";
    int wanCount = getWanSlotCount();
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    if(num <= wanCount) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, num);
        if(profIf->ether.connMode == ETH_CONN_PPPOE) {
            tmp = getDevicePPPIfName(getWanIfName(num));
    	} else {
            tmp = getWanIfName(num);
        }
    }
    return tmp;
}
/***********************************************************************
 * 函 数 名：   getIfIp
 * 功能描述：	get interface ip
 * 输入参数:    interface name,a 16-byte buffer to store ip address
 * 输出参数：	ip
 * 返回值：     0,-1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
int getIfIp(char *ifname, char *if_addr)
{
    struct ifreq ifr;
    int skfd = 0;
    
    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    	return -1;
    }
    
    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
    	close(skfd);
    	return -1;
    }
    if(if_addr != NULL) {
        strcpy(if_addr, (const char *)(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)));
    }
    close(skfd);
    return 0;
}

/***********************************************************************
 * 函 数 名：   getIfMac
 * 功能描述：	get interface mac addr
 * 输入参数:    interface name,a 18-byte buffer to store ip address
 * 输出参数：	mac addr
 * 返回值：     0,-1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
int getIfMac(char *ifname, char *if_hw)
{
    struct ifreq ifr;
    char *ptr;
    int skfd;
    
    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    	return -1;
    }
    
    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
    	close(skfd);
    	return -1;
    }
    
    ptr = (char *)&ifr.ifr_addr.sa_data;
    if(if_hw != NULL) {
        sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
    		(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
    		(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));
    }
    close(skfd);
    return 0;
}

/***********************************************************************
 * 函 数 名：   getIfNetmask
 * 功能描述：	get interface  netmask
 * 输入参数:    interface name,a 16-byte buffer to store subnet mask
 * 输出参数：	netmask
 * 返回值：     0,-1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       胡传香
 * 附加说明：	无
***********************************************************************/
int getIfNetmask(char *ifname, char *if_net)
{
    struct ifreq ifr;
    int skfd = 0;
    
    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    	return -1;
    }
    
    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
    	close(skfd);
    	return -1;
    }
    if(if_net != NULL) {
        strcpy(if_net, (const char *)(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)));
    }
    close(skfd);
    return 0;
}

#if (CONFIG_ATHEROS_AR71XX == 1)
static int _ar7100_reg_read(int id, int page, int reg, int *value)
{
	struct ifreq ifr;
	struct mii_ioctl_data data;
        int mii_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (value == NULL)
		return -1;
	data.phy_id = (unsigned short)id;
	data.page = (unsigned short)page;
	data.reg_num = (unsigned short)reg;
	strncpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(mii_fd, SIOCGMIIREG, &ifr)) {
		perror("ioctl");
		close(mii_fd);
		exit(0);
	}
	*value = data.val_out;
    close(mii_fd);
	return 0;
}
#endif

#if (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)||(UTT_NV_517W == FYES)
#ifndef CONFIG_RTL8306E 
/* 840E mii read: wan口8306芯片寄存器 */
extern int _840E_mii_read(int phy_id, int page, int reg, int *value)
{
    struct ifreq ifr;
    ra_8306_mii_ioctl_data mii;
    int sk, i, ret;

    if (value == NULL)
	return -1;

    sk = socket(AF_INET, SOCK_DGRAM, 0);

    if (sk < 0) {
	printf("Open socket failed\n");
        return -1;
    }

    mii.phy_id = (unsigned int)phy_id;
    mii.page = (unsigned short)page;
    mii.reg_num = (unsigned int)reg;
    strncpy(ifr.ifr_name, "eth2", 5);
    ifr.ifr_data = (void *)&mii;

    ret = ioctl(sk, RAETH_MII_READ, &ifr);
    if(ret < 0) {
        close(sk);
        return -1;
    }

    *value = mii.val_out;
    close(sk);
    return 0;
}

extern int _840E_mii_write(int phy_id, int page, int reg, int value)
{
    struct ifreq ifr;
    ra_8306_mii_ioctl_data mii;
    int sk, i, ret;

    sk = socket(AF_INET, SOCK_DGRAM, 0);

    if (sk < 0) {
	printf("Open socket failed\n");
        return -1;
    }

    mii.phy_id = (unsigned int)phy_id;
    mii.page = (unsigned short)page;
    mii.reg_num = (unsigned int)reg;
    mii.val_in = value;
    strncpy(ifr.ifr_name, "eth2", 5);
    ifr.ifr_data = (void *)&mii;

    ret = ioctl(sk, RAETH_MII_WRITE, &ifr);
    if(ret < 0) {
        close(sk);
        return -1;
    }

    close(sk);
    return 0;
}
#endif

/* 840E gpio read: lan 8306芯片寄存器 */
extern int _840E_gpio_read(int phy_id, int page, int reg, int *value)
{
    struct ifreq ifr;
    ra_8306_mii_ioctl_data mii;
    int sk, i, ret;

    if (value == NULL)
	return -1;

    sk = socket(AF_INET, SOCK_DGRAM, 0);

    if (sk < 0) {
	printf("Open socket failed\n");
        return -1;
    }

    mii.phy_id = (unsigned int)phy_id;
    mii.page = (unsigned short)page;
    mii.reg_num = (unsigned int)reg;
    strncpy(ifr.ifr_name, "eth2", 5);
    ifr.ifr_data = (void *)&mii;

#ifdef CONFIG_RTL8306E
    ret = ioctl(sk, RAETH8306_ESW_REG_READ, &ifr);
#else
    ret = ioctl(sk, RAETH_ESW_REG_READ, &ifr);
#endif
    if(ret < 0) {
        close(sk);
        return -1;
    }

    *value = mii.val_out;
    close(sk);
    return 0;
}

extern int _840E_gpio_write(int phy_id, int page, int reg, int value)
{
    struct ifreq ifr;
    ra_8306_mii_ioctl_data mii;
    int sk, i, ret;

    sk = socket(AF_INET, SOCK_DGRAM, 0);

    if (sk < 0) {
	printf("Open socket failed\n");
        return -1;
    }

    mii.phy_id = (unsigned int)phy_id;
    mii.page = (unsigned short)page;
    mii.reg_num = (unsigned int)reg;
    mii.val_in = value;
    strncpy(ifr.ifr_name, "eth2", 5);
    ifr.ifr_data = (void *)&mii;

#ifdef CONFIG_RTL8306E
    ret = ioctl(sk, RAETH8306_ESW_REG_WRITE, &ifr);
#else
    ret = ioctl(sk, RAETH_ESW_REG_WRITE, &ifr);
#endif
    if(ret < 0) {
        close(sk);
        return -1;
    }

    close(sk);
    return 0;
}
#endif

/*
 * get_lan_status	-	获取LAN口连接状态
 *
 * 传入lan口索引(>=1)，计算出此口是否有网线连接
 * 
 * 返回值:
 *	status		-	连接状态
 *	0		-	未连接
 *	1		-	已连接
 *
 */
int get_lan_status(int lanIndex)
{
	int status = 0;
#if (CONFIG_RALINK_MT7620 == 1) && (CONFIG_P5_RGMII_TO_MT7530_MODE != 1)
	struct ifreq ifr;
	esw_reg reg;
	int esw_fd;
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		return 0;
	}
#if ((UTT_NV_A750W == FYES)||(UTT_NV_A755W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_A650W_INT == FYES) || (UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)||(UTT_NV_517W == FYES) ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES)||(UTT_NV_A658W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
	reg.off = 0x3008 + (lanIndex - 1) * 0x100;
#elif ((UTT_NV_A309W == FYES)|| (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A310Wv2 == FYES))
	reg.off = 0x3408 - lanIndex * 0x100;
#else
	reg.off = 0x3008 + lanIndex * 0x100;
#endif
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (void *)&reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		close(esw_fd);
		return 0;
	}
	close(esw_fd);
	status = reg.val & 0x1;
#elif (CONFIG_RALINK_MT7621 == 1 || CONFIG_P5_RGMII_TO_MT7530_MODE == 1) 
	struct ifreq ifr;
	esw_reg reg;
	int esw_fd;
	ra_mii_ioctl_data mii;
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		return 0;
	}
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &mii;

	mii.phy_id = 0x1f;
	mii.reg_num = 0x3008 + lanIndex * 0x100;

	if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
		close(esw_fd);
		return 0;
	}
	close(esw_fd);
	status =  mii.val_out & 0x1;
#elif(CONFIG_RALINK_MT7628 == 1)

	struct ifreq ifr;
	esw_reg reg;
	int esw_fd;
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		return 0;
	}

	reg.off = 0x80;
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	close(esw_fd);
#if(UTT_NV_A312W == FYES)
	if(lanIndex==1)
	{
		status = ((reg.val &0x08000000)>>27);
	}
	else if(lanIndex==2)
	{
		status = ((reg.val &0x10000000)>>28);
	}

#else
	if(lanIndex==1)
	{
	    status = ((reg.val &0x02000000)>>25);
	}
	else if(lanIndex==2)
	{
	    status = ((reg.val &0x04000000)>>26);
	}

#endif
#if(defined(CONFIG_BOARD_MTK7628NN_A_V1_0) && UTT_NV_A312W == FNO)
	else if(lanIndex==3)
	{
		status = ((reg.val &0x08000000)>>27);
	}
	else if(lanIndex==4)
	{
		status = ((reg.val &0x10000000)>>28);
	}
#endif
#endif
	return status;
}

/***********************************************************************
 * 函 数 名：   get_linelink_status
 * 功能描述:    根据接入类型取出链接状态
 * 输入参数:    
 * 输出参数：	
 * 返回值：	    0,1
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：	无
***********************************************************************/
int get_phylink_status(int wanIndex)
{
#if (UTT_NV_1800N == FYES)
    return 1;
#endif
#if (CONFIG_RALINK_RT3052 == 1)
    struct ifreq ifr;
    esw_reg reg;
    int esw_fd;
    int status = 0;

//#ifdef CONFIG_USER_3G
#if (WIRELESS == FYES)
    if (wanIndex == get3GSlotIndex()) {
        return 1;
    } else  
//#endif
//#ifdef CONFIG_RT2860V2_AP_MODULE
    if (wanIndex == getAPCliSlotIndex()) {
	char *pos;
	int rc = 0;
	FILE *fp;
	char buf[1024];

        if((fp = popen("iwconfig apcli0", "r")) == NULL){
	    	return 0; 
	    }
	    rc = fread(buf, 1, 1024, fp);
	    pclose(fp);
	    if(rc == -1){
	    	return 0;
	    }else{
	    	if((pos = strstr(buf, "Not-Associated")) == NULL){
	    			  return 1;
	    	}
	    }
    } else
#endif
    {
        esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (esw_fd < 0) {
            return 0;
        }
	reg.off = 0x80;
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		close(esw_fd);
                return 0;
	}
	close(esw_fd);
#ifdef UTT_NV_3520G
        switch(wanIndex) {
            case 1:
                status = reg.val & 0x08000000;
                break;
            case 2:
                status = reg.val & 0x20000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_WX1000
        switch(wanIndex) {
            case 1:
                status = reg.val & 0x08000000;
                break;
            case 2:
                status = reg.val & 0x20000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_1540
        switch(wanIndex) {
            case 1:
                status = reg.val & 0x04000000;
                break;
            case 2:
                status = reg.val & 0x08000000;
                break;
            case 3:
                status = reg.val & 0x10000000;
                break;
            case 4:
                status = reg.val & 0x20000000;
                break;
            default:
                status = 0;
        }
#endif
#if (( UTT_NV_510V2 == FYES ) || (UTT_NV_540 == FYES) || (UTT_NV_588 == FYES))
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif
#if((UTT_NV_520V2 == FYES) || (UTT_NV_810V2 == FYES)|| (UTT_NV_830V2 == FYES) || (UTT_NV_840V2 == FYES) || (UTT_NV_640E == FYES))
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x20000000;
                break;
            case 2:
                status = reg.val & 0x10000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x04000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_510W
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x20000000;
                break;
            case 3:
                status = reg.val & 0x10000000;
                break;
            case 4:
                status = reg.val & 0x08000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_520W
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x20000000;
                break;
            case 2:
                status = reg.val & 0x10000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x04000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_512W
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_518W
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_518WVPN
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif

#ifdef UTT_NV_842W
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x20000000;
                //status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x10000000;
                //status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x04000000;
                //status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_518
       switch(wanIndex) {
            case 1:
                //status = reg.val & 0x20000000;
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                //status = reg.val & 0x10000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                //status = reg.val & 0x04000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_518WPLUS
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif
#ifdef UTT_NV_518WLITE
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif

#ifdef UTT_U_1000N
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x20000000;
                break;
            case 2:
                status = reg.val & 0x10000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x04000000;
                break;
            default:
                status = 0;
        }
#endif

    }
    return status?1:0;
#elif (CONFIG_RALINK_MT7621 == 1 || CONFIG_P5_RGMII_TO_MT7530_MODE == 1) 
    struct ifreq ifr;
    esw_reg reg;
    int esw_fd;
    int status = 0;

#if (WIRELESS == FYES)
//#ifdef CONFIG_USER_3G
    if (wanIndex == get3GSlotIndex()) {
        return 1;
    } else  
//#endif
//#ifdef CONFIG_RT2860V2_AP_MODULE
    if (wanIndex == getAPCliSlotIndex()) {
	char *pos;
	int rc = 0;
	FILE *fp;
	char buf[1024];

        if((fp = popen("iwconfig apcli0", "r")) == NULL){
	    	return 0; 
	    }
	    rc = fread(buf, 1, 1024, fp);
	    pclose(fp);
	    if(rc == -1){
	    	return 0;
	    }else{
	    	if((pos = strstr(buf, "Not-Associated")) == NULL){
	    			  return 1;
	    	}
	    }
    } else
#endif
    {
	ra_mii_ioctl_data mii;
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
	    return 0;
	}
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &mii;

	mii.phy_id = 0x1f;
	mii.reg_num = 0x3008 + (5 - wanIndex) * 0x100;

	if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
	    close(esw_fd);
	    return 0;
	}
	close(esw_fd);
	status = mii.val_out & 0x1;
    }

    return status?1:0;
#elif(CONFIG_RALINK_MT7628 == 1)
    struct ifreq ifr;
    esw_reg reg;
    int esw_fd;
    int status = 0;

#if (WIRELESS == FYES)
//#ifdef CONFIG_USER_3G
    if (wanIndex == get3GSlotIndex()) {
        return 1;
    } else  
//#endif
//#ifdef CONFIG_RT2860V2_AP_MODULE
    if (wanIndex == getAPCliSlotIndex()) {
	char *pos;
	int rc = 0;
	FILE *fp;
	char buf[1024];

        if((fp = popen("iwconfig apcli0", "r")) == NULL){
	    	return 0; 
	    }
	    rc = fread(buf, 1, 1024, fp);
	    pclose(fp);
	    if(rc == -1){
	    	return 0;
	    }else{
	    	if((pos = strstr(buf, "Not-Associated")) == NULL){
	    			  return 1;
	    	}
	    }
    } else
#endif
    {

	struct ifreq ifr;
	esw_reg reg;
	int esw_fd;
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		return 0;
	}

	reg.off = 0x80;
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}	
	close(esw_fd);
#if (CONFIG_WAN_PORT3 == 1)
	status = reg.val&0x10000000;
#else
	status = reg.val&0x20000000;
#endif
    }

    return status?1:0;
#elif (CONFIG_RALINK_MT7620 == 1)
    /*
     * add 512WV2
     */
    struct ifreq ifr;
    esw_reg reg;
    int esw_fd;
    int status = 0;

#if (WIRELESS == FYES)
//#ifdef CONFIG_USER_3G
    if (wanIndex == get3GSlotIndex()) {
        return 1;
    } else  
//#endif
//#ifdef CONFIG_RT2860V2_AP_MODULE
    if (wanIndex == getAPCliSlotIndex()) {
	char *pos;
	int rc = 0;
	FILE *fp;
	char buf[1024];

        if((fp = popen("iwconfig apcli0", "r")) == NULL){
	    	return 0; 
	    }
	    rc = fread(buf, 1, 1024, fp);
	    pclose(fp);
	    if(rc == -1){
	    	return 0;
	    }else{
	    	if((pos = strstr(buf, "Not-Associated")) == NULL){
	    			  return 1;
	    	}
	    }
    } else
#endif
    {
        esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (esw_fd < 0) {
            return 0;
        }
#if (UTT_NV_521GW == FYES)
    ra_mii_ioctl_data mii;
	mii.reg_num =  0x94 - (wanIndex * 4);
	strncpy(ifr.ifr_name, "eth2", 5);

	ifr.ifr_data = &mii;
	if (-1 == ioctl(esw_fd, RAETH_8337_READ, &ifr)) {
		close(esw_fd);
                return 0;
	}
	close(esw_fd);
	status = (mii.val_out>>8) & 0x1;
#elif defined(CONFIG_RTL8367RB)
	ra_mii_ioctl_data mii;
	memset(&mii, 0, sizeof(ra_mii_ioctl_data));
	mii.phy_id = 5 - wanIndex;
	mii.reg_num =  1;
	strncpy(ifr.ifr_name, "eth2", 5);

	ifr.ifr_data = &mii;
	if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
		close(esw_fd);
                return 0;
	}
	close(esw_fd);
	status = (mii.val_out>>2) & 0x1;
#else
#if ((UTT_NV_520W == FYES) || (UTT_NV_A308W == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES) || (UTT_NV_512WV2 == FYES) || (UTT_NV_518WV2 == FYES) || (UTT_NV_510V4 == FYES) || (UTT_NV_840V3 == FYES) || (UTT_NV_D904W == FYES))
	reg.off = 0x3008 + (wanIndex - 1) * 0x100;
#endif

#if ((UTT_NV_750W == FYES) || (UTT_NV_842WV3 == FYES) || (UTT_NV_810V4 == FYES)|| (UTT_NV_830V3 == FYES) || (UTT_NV_811V4 == FYES) || (UTT_NV_821V2 == FYES) || (UTT_NV_841V4 == FYES) || (UTT_NV_840V4 == FYES) || (UTT_NV_520WV3 == FYES) || (UTT_NV_D910 == FYES) || (UTT_NV_WX20S == FYES) || (UTT_U_1100 == FYES) || (UTT_U_1900 == FYES) || (UTT_NV_U1000NV2 == FYES))
	/* 750板子的接口顺序是错的 */
	if (wanIndex == 1) {
		reg.off = 0x3008 + 3 * 0x100;
	} else if (wanIndex == 2) {
		reg.off = 0x3008 + 2 * 0x100;
	} else if (wanIndex == 3) {
		reg.off = 0x3008 + 1 * 0x100;
	} else if (wanIndex == 4) {
		reg.off = 0x3008;
	} else {
	
	}
#endif
#if ((UTT_NV_840DV3 == FYES)||(UTT_NV_840EV3 == FYES)||(UTT_NV_517W == FYES))
	reg.off = 0x3008 + (5 - wanIndex) * 0x100;
#endif
#if ((UTT_NV_512WV3 == FYES) || (UTT_NV_518WV3 == FYES) || (UTT_NV_515W == FYES) || (UTT_NV_D908W == FYES))
	if (wanIndex == 1) {
	    reg.off = 0x3008 + 4 * 0x100;
	} else if (wanIndex == 2) {
	    reg.off = 0x3008 ;
	} else if (wanIndex == 3) {
	    reg.off = 0x3008 + 1 * 0x100;
	} else if (wanIndex == 4) {
	    reg.off = 0x3008 + 2 * 0x100;
	} else {

	}
#endif
#if ((UTT_NV_A750W == FYES)||(UTT_NV_A755W == FYES ) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_A650W_INT == FYES) ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES)||(UTT_NV_A658W == FYES)|| (UTT_NV_A310Wv2 == FYES)|| (UTT_NV_WBR_6013 == FYES) ||(UTT_NV_A309W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
	reg.off = 0x3408;
#endif
#if ((UTT_NV_518WLITE == FYES)||(UTT_NV_518WPLUS == FYES))
	reg.off = 0x3008 + (wanIndex - 2) * 0x100;
#endif
#if ((UTT_NV_810V3 == FYES) || (UTT_NV_520V3 == FYES) || (UTT_NV_640EV2 == FYES))
	reg.off = 0x3008 + (5 - wanIndex) * 0x100;
#endif
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &reg;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		close(esw_fd);
                return 0;
	}
	close(esw_fd);

	status = reg.val & 0x1;
#endif

	}

    return status?1:0;
#elif (CONFIG_RALINK_RT5350 == 1)
    struct ifreq ifr;
    esw_reg reg;
    int esw_fd;
    int status = 0;

    esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (esw_fd < 0) {
        return 0;
    }
    reg.off = 0x80;
    strncpy(ifr.ifr_name, "eth2", 5);
    ifr.ifr_data = &reg;
    if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
        close(esw_fd);
        return 0;
	}
	close(esw_fd);
#if 0
#if ( UTT_NV_510V4 == FYES )
       switch(wanIndex) {
            case 1:
                status = reg.val & 0x02000000;
                break;
            case 2:
                status = reg.val & 0x04000000;
                break;
            case 3:
                status = reg.val & 0x08000000;
                break;
            case 4:
                status = reg.val & 0x10000000;
                break;
            default:
                status = 0;
        }
#endif
#endif
    return status?1:0;

#elif (CONFIG_ATHEROS_AR71XX == 1)
    int value = 0;
    switch(wanIndex) {
#if (UTT_MV_811 == FYES)
        case 1:
            _ar7100_reg_read(4, 0, 1, &value);
            break;
#endif
#if (UTT_MV_821 == FYES)
        case 1:
            _ar7100_reg_read(4, 0, 1, &value);
            break;
        case 2:
            _ar7100_reg_read(3, 0, 1, &value);
            break;
#endif
#if (UTT_MV_841 == FYES)
        case 1:
            _ar7100_reg_read(4, 0, 1, &value);
            break;
        case 2:
            _ar7100_reg_read(3, 0, 1, &value);
            break;
        case 3:
            _ar7100_reg_read(2, 0, 1, &value);
            break;
        case 4:
            _ar7100_reg_read(1, 0, 1, &value);
            break;
#endif
        default:
            break;
    }

    value &= 0x4;
    if(value != 0) {
        value = 1;
    } else {
        value = 0;
    }
    return value;
#elif (CONFIG_X86_32 == 1)
    int fd, err;
    struct ethtool_value edata;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd >= 0) {
         //edata.cmd = ETHTOOL_GLINK;
         edata.cmd = 0x0000000a;
         ifr.ifr_data = (caddr_t)&edata;
         strncpy(ifr.ifr_name, getWanIfName(wanIndex), 10);
         err = ioctl(fd, SIOCETHTOOL, &ifr);
         close(fd);
         if(err == 0) {
             return edata.data;
         }
    }
    return 0;
#elif (CONFIG_RALINK_RT3883 == 1)
#if ((UTT_NV_840G == FYES) || (UTT_NV_521G == FYES) || (UTT_NV_1220G == FYES) || (UTT_NV_WX100 == FYES) || (UTT_NV_3320G == FYES) || (UTT_NV_840G_INT == FYES))
        struct ifreq ifr;
        int sk = 0;
        int ret = 0;
        int i=0;
        ra_mii_ioctl_data mii;
        switch(wanIndex) {
            case 1:
                mii.phy_id = 4;
                break;
            case 2:
                mii.phy_id = 3;
                break;
            case 3:
                mii.phy_id = 2;
                break;
            case 4:
                mii.phy_id = 1;
                break;
            default:
                return 0;
        }
        sk = socket(AF_INET, SOCK_DGRAM, 0);
        if (sk < 0) {
            printf("Open socket failed\n");
            return 0;
        }
        strncpy(ifr.ifr_name, "eth2", 5);
        ifr.ifr_data = &mii;
        mii.reg_num = 1;
        /*状态寄存器需读两次*/
	for(i = 0; i< 2; i++)
	{
	    ret = ioctl(sk, RAETH_MII_READ, &ifr);
	    if(ret < 0) {
		close(sk);
		return 0;
	    }
	}
	close(sk);
	if((mii.val_out & 0x4) == 0)
	{
	    return 0;
	}
	else
	{
	    return 1;
	}
#elif (UTT_NV_840E == FYES) || (UTT_NV_800 == FYES) || (UTT_NV_840D == FYES)
	int value = 0;
	/* 状态寄存器需要读2次 */
	switch (wanIndex) {
	    case 1:
		_840E_mii_read(0, 0, 1, &value);
		_840E_mii_read(0, 0, 1, &value);
		break;
	    case 2:
		_840E_mii_read(1, 0, 1, &value);
		_840E_mii_read(1, 0, 1, &value);
		break;
	    case 3:
		_840E_mii_read(2, 0, 1, &value);
		_840E_mii_read(2, 0, 1, &value);
		break;
	    case 4:
		_840E_mii_read(3, 0, 1, &value);
		_840E_mii_read(3, 0, 1, &value);
		break;
	    default:
		break;
	}
	if ((value &= 0x4) == 0) {
	    return 0;
	} else {
	    return 1;
	}
#endif
#elif CONFIG_PPC_85xx
#define RTL8367M_DEVICE "/dev/utt_rtl8367m"
#if defined(CONFIG_BOARD_P1010_B)
	int fp = 0;
	int ret = 0;
	int i=0;
	rtl8367m_ioctl_data mii;
	mii.phy_id = 5 - wanIndex;

	/*打开注册的字符设备*/
	fp = open(RTL8367M_DEVICE, O_RDWR, S_IRUSR|S_IWUSR);
	if (fp < 0) {
		perror("Open");
		return 0;
	}
	mii.reg_num = 1;
	/*状态寄存器需读两次*/
	for(i = 0; i< 2; i++) {
		ret = ioctl(fp, RTL8367M_MII_READ, &mii);
		if(ret < 0) {
			perror("ioctl");
			close(fp);
			return 0;
		}
	}
	close(fp);
	if((mii.val_out & 0x4) == 0) {
		return 0;
	} else {
		return 1;
	}
#elif defined(CONFIG_BOARD_P1010_C)
	int fp = 0;
	int ret = 0;
	int i=0;
	rtl8367m_ioctl_data mii;
    if (wanIndex > P1010C_8336_IF_COUNT) {
	mii.phy_id = (P1010C_8367M_IF_COUNT + P1010C_8336_IF_COUNT) - wanIndex;

	/*打开注册的字符设备*/
	fp = open(RTL8367M_DEVICE, O_RDWR, S_IRUSR|S_IWUSR);
	if (fp < 0) {
		perror("Open");
		return 0;
	}
	mii.reg_num = 1;
	/*状态寄存器需读两次*/
	for(i = 0; i< 2; i++) {
		ret = ioctl(fp, RTL8367M_MII_READ, &mii);
		if(ret < 0) {
			perror("ioctl");
			close(fp);
			return 0;
		}
	}
	close(fp);
	if((mii.val_out & 0x4) == 0) {
		return 0;
	} else {
		return 1;
	}
    } 
#if defined(CONFIG_AR8337N)
    else {
	mii.reg_num =  0x94 - (wanIndex * 4);

	/*打开注册的字符设备*/
	fp = open(RTL8367M_DEVICE, O_RDWR, S_IRUSR|S_IWUSR);
	if (fp < 0) {
		perror("Open");
		return 0;
	}
	/*状态寄存器需读两次*/
	for(i = 0; i< 2; i++) {
		ret = ioctl(fp, AR8337N_REG_READ, &mii);
		if(ret < 0) {
			perror("ioctl");
			close(fp);
			return 0;
		}
	}
	close(fp);
	if((mii.val_out & 0x100) == 0) {
		return 0;
	} else {
		return 1;
	}
    }
#endif
#endif
#endif
}

/* if wanNo's link status is alive
 * On success: alive return 1
 *        sleep return 0
 * if fail return a negtive number */
int _getWanLinkState(int wanNo)
{
    FILE *fp;
    int retval=0;
    char filename[64];
    char status[2]={0,0};
    unsigned int rc=0U;

    memset(filename, 0x00, sizeof(filename));
    /* open link status file */
    sprintf(filename, "/var/run/Wan%dlink.status", wanNo);
    fp=fopen(filename, "r");
    if (fp == NULL ) {
        retval = -1;
    }else {
        /* the content of the file is a byte, which indicant 
         * the link status of the wanNo port. 
         * while '1' for up, and '0' for down */
        rc = fread((void *)status, 2U, 1U, fp);
        if (rc != 1U) {
            retval = -1;
        } else {
            retval = ((int)status[0]-0x30);
        }
        fclose(fp);
    }
    return retval;
}

int converMac12To6Byte(char *macStr, MacAddr* macAddr)
{
    char tmp[3] = {0};
    int i, j;
    char* destAddr = macAddr->addr;

    tmp[2] = 0;

    if(strlen(macStr) < 12) {
        return -1;
    }

    for(i = 0; i< 6; i++) {
        for(j = 0; j < 2; j++) {
            tmp[j] = *(macStr + i*2 +j);
            *(destAddr+i) = strtol(tmp, NULL, 16);
        }
    }
    return 0;
}

char* converMac6To12Byte(MacAddr macAddr)
{
    static char tmp[UTT_MAC_STR_LEN];
    char *mac = macAddr.addr;

    sprintf(tmp, "%02x%02x%02x%02x%02x%02x", (Uint8)*mac, (Uint8)*(mac+1),
        (Uint8)*(mac+2), (Uint8)*(mac+3), (Uint8)*(mac+4), (Uint8)*(mac+5)); 
    return tmp;
}

char* converMac6To17Byte(MacAddr macAddr)
{
    static char tmp[UTT_MAC_STR_LEN];
    char* mac = macAddr.addr;
    sprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x", (Uint8)*mac, (Uint8)*(mac+1),
        (Uint8)*(mac+2), (Uint8)*(mac+3), (Uint8)*(mac+4), (Uint8)*(mac+5)); 
    return tmp;
}
/* mac地址比较
 * return: 0 相等 ,1 mac1>mac2, -1 mac<mac2
 * bhou
 * */
int macAddrCmp(MacAddr* mac1, MacAddr* mac2)
{
    return memcmp(mac1, mac2, sizeof(MacAddr));
}
/**
 * 转换0-32位的掩码长度为ip地址二进制形式掩码（仅使用于IPV4）
 * bhou
 * 2011-10-31
 */
IPADDR convertLenToNetMask(Uint32 netMaskLen)
{
    IPADDR netMask = 0u;

    Uint32 i = 0u, len = 0u;

    len = 32u - netMaskLen;/*掩码中0的个数*/
    for(i = 0u; i< len; i++)
    {
	netMask |= 0x1u << i;
    }
    netMask = ~netMask;
    netMask = htonl(netMask);/*转换为网络字节序*/

    return netMask;
}
/**
 * 判断ip是否和设备的某个端口同网段
 * ipStr:要判断的ip地址字符串
 * ifName:接口名称
 * return: 1 同网段 0 不同网段 -1查询失败
 * bhou
 */
int isIpStrInIfNet(char *ipStr, char* ifName)
{
    struct in_addr ipAddr;
    int ret = -1;
    if(inet_aton(ipStr, &ipAddr) != 0)
    {
	ret = isIpInIfNet(ipAddr.s_addr, ifName);
    }
    return ret;
}
/**
 * 判断ip是否和设备的某个端口同网段
 * ip:要判断的ip地址。为网络字节序
 * ifName:接口名称
 * return: 1 同网段 0 不同网段 -1查询失败
 * bhou
 */
int isIpInIfNet(IPADDR ip,char *ifName)
{
    int ret=0;
    struct ifreq ifr;
    IPADDR ifIp = 0u;
    IPADDR ifMask = 0u;
    int skfd = -1;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) > 0) 
    {
	strncpy(ifr.ifr_name, ifName, IF_NAMESIZE);/*端口名*/

	/*取掩码*/
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) == 0) 
	{
	    ifMask = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
	}

	/*取ip地址*/
	if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0) 
	{
	    ifIp = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
	}

	if((ifIp&ifMask)!=0u)
	{
	    if( (ifIp&ifMask) == (ip&ifMask))
	    {/*同网段*/
		ret = 1;
	    }
	    else
	    {/*不同网段*/
		ret = 0;
	    }
	}

	close(skfd);
    }

    return ret;

}

/***********************************************************************
 * 函 数 名：   checkWanIpInPool
 * 创建日期：	2011-12-23
 * 修改日期：	  
 * 作者：       yqyang
 * 附加说明：	检查WAN口地址是否在地址范围内
		addrStart 起始IP地址 cnt 起始IP地址到结束IP地址的个数
		返回0表示没有找到  -1 表示已经找到
 ***********************************************************************/
extern int checkWanIpInPool(struct in_addr addrStart, int cnt)
{
    int i ;
    int ret = 0;
    char    if_addr[16];
    InterfaceProfile *profIf = NULL;
    Uint32 ifIp, wanCount;
    struct in_addr addr_ifip;
    /* 地址池不能包含接口ip */
    wanCount = getWanSlotCount();
    for (i = 0; i <= wanCount; i++) {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, i);
	if ((profIf == NULL) || (ProfInstIsFree(profIf) == 1) || ((profIf->ether.connMode == ETH_CONN_PPPOE) )) {
	    continue;
	}

	if(i == 0){
	     getIfIp(getLanIfName(), if_addr);

	}else{
	     getIfIp(getWanIfName(i), if_addr);
	}
	inet_aton(if_addr, &addr_ifip);
	ifIp = ntohl(addr_ifip.s_addr);

	//printf("ifIp[%d] = %x startip = %x endif = %x\n",i,ifIp,ntohl(addrStart.s_addr),ntohl(addrStart.s_addr) + cnt);
	if (ifIp >= ntohl(addrStart.s_addr) && ifIp < ntohl(addrStart.s_addr) + cnt) {
	    return -1;
	}
    }
    return 0;
}

/*********************************************************************
 * 函数： convertLenToBinMask
 * 功能： 根据长度生成二进制掩码,如maskLen为4则 结果过0xf,为3则结果为0x7
 * 创建： 2012-09-11
 * 参数： 略
 * 返回:  略
 * 输出： 略
 * 作者： Jianqing.Sun
 ********************************************************************/
Uint32 convertLenToBinMask(Uint32 maskLen)
{
    Uint32 binMask = 0U; 
    Uint32 i = 0U; 
    for(i = 0U; i< maskLen; i++)
    {   
	binMask |= 0x1U << i;
    }   
    return binMask;
}

/*获取出厂配置接口数量*/
extern void getDefaultIfCount(int *wanCountPtr, int *lanCountPtr)
{
    int wanIfCount = 0, lanPortCount = 0;
#if (USER_DEF_WAN_COUNT == FYES)
#if (UTT_NV_840G == FYES)
    wanIfCount = 4;
    lanPortCount= 1;
#elif (UTT_NV_521G == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_840G_INT == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_WX100 == FYES)
    wanIfCount = 1;
    lanPortCount= 4;
#elif (UTT_NV_510V2 == FYES)
    wanIfCount = 1;
    lanPortCount= 4;
#elif (UTT_NV_510V4 == FYES)
    wanIfCount = 1;
    lanPortCount= 4;
#elif ((UTT_NV_520V2 == FYES) || (UTT_NV_520V3 == FYES) || (UTT_NV_640EV2 == FYES))
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_640E == FYES) 
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_540 == FYES)
    wanIfCount = 4;
    lanPortCount= 1;
#elif (UTT_NV_588 == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_3320G == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES))
    wanIfCount = 3;
    lanPortCount= 3;
#elif ((UTT_NV_A308W == FYES)|| (UTT_NV_A310Wv2 == FYES) || (UTT_NV_A309W == FYES) || (UTT_NV_WBR_6013 == FYES) || (UTT_NV_A307W == FYES) || (UTT_NV_A310W == FYES) || (UTT_NV_D904W == FYES))
    wanIfCount = 1;
    lanPortCount= 4;
#elif (UTT_NV_750W == FYES) || (UTT_NV_U1000NV2 == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_1220GW == FYES)|| (UTT_NV_1220V2 == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_842WV3 == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_520WV3 == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES)  || (UTT_NV_840GV2 == FYES))
    wanIfCount = 2;
    lanPortCount= 3;
#elif ((UTT_NV_A750W == FYES)||(UTT_NV_A755W == FYES) || (UTT_NV_A650W == FYES) || (UTT_NV_WBR_8001 == FYES) || (UTT_NV_A650W_INT == FYES) ||(UTT_NV_A655W == FYES) || (UTT_NV_WBR_8002 == FYES) || (UTT_NV_AC60 == FYES) ||(UTT_NV_A658W == FYES) ||defined(CONFIG_BOARD_MTK7628NN_A_V1_0))
    wanIfCount = 1;
    lanPortCount= 4;
#elif (UTT_NV_WX20S == FYES)
    wanIfCount = 0;
    lanPortCount= 5;
#elif (UTT_NV_521GW == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif ((UTT_NV_512W == FYES) || (UTT_NV_518W == FYES))
    wanIfCount = 3;
    lanPortCount= 3;
#elif ((UTT_NV_512WV2 == FYES) || (UTT_NV_518WV2 == FYES))
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_518WVPN == FYES)
    wanIfCount = 3;
    lanPortCount= 3;
#elif (UTT_NV_518 == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_NV_518WPLUS == FYES)
    wanIfCount = 4;
    lanPortCount= 3;
#elif (UTT_NV_515W == FYES) || (UTT_NV_D908W == FYES)
    wanIfCount = 3;
    lanPortCount= 3;
#elif (UTT_NV_518WLITE == FYES)
    wanIfCount = 4;
    lanPortCount= 3;
#elif (UTT_NV_842W == FYES)
    wanIfCount = 3;
    lanPortCount= 3;
#elif (UTT_QV_4320G == FYES) || (UTT_QV_2620G == FYES) || (UTT_QV_3320G == FYES)|| (UTT_QV_1220G == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#elif (UTT_QV_2610G == FYES)
    wanIfCount = 1;
    lanPortCount = 4;
#elif (UTT_QV_1210G == FYES)
    wanIfCount = 1;
    lanPortCount = 4;
#elif (UTT_QV_4220G == FYES) || (UTT_QV_N1800 == FYES)
    wanIfCount = 2;
    lanPortCount= 5;
#elif (UTT_QV_4240G == FYES)
    wanIfCount = 4;
    lanPortCount= 4;
#elif (UTT_QV_3640 == FYES) ||(UTT_U_3000 == FYES) || (UTT_U_4000 == FYES) || (UTT_QV_N800 == FYES) ||(UTT_QV_NE1200 == FYES)|| (UTT_QV_4840 == FYES)
    wanIfCount = 4;
    lanPortCount= 4;
#elif (UTT_NV_1220G == FYES)
    wanIfCount = 2;
    lanPortCount= 3;
#else
    wanIfCount = MAX_WAN_IF_COUNT;
    lanPortCount= MAX_LAN_PORT_COUNT;
#endif
#else
    wanIfCount = MAX_WAN_IF_COUNT;
    lanPortCount= MAX_LAN_PORT_COUNT;
#endif
    if(wanCountPtr != NULL)
    {
	*wanCountPtr = wanIfCount;
    }
    if(lanCountPtr != NULL)
    {
	*lanCountPtr = lanPortCount;
    }
    return;

}
#if (WIRELESS == FYES)
/* 获得3G线路接口index */
extern int get3GSlotIndex(void) {
    int index;
#if (UTT_NV_510W == FYES)
    index = 2;
#elif (UTT_NV_515W == FYES) || (UTT_NV_D908W == FYES)
    index = 3;
#elif((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	switch(getWanSlotCount()){
	case 3:
		index = 2;
		break;
	case 4:
		index = 3;
		break;
	case 5:
		index = 4;
		break;
	case 6:
		index = 5;
		break;
	default:
		index = -1;
		break;
	}
#else
    index = -1;
#endif
    return index;
}

/*根据wan口数量获取apclient接口索引*/
static int getAPCliSlotIndexByWIf(int wanIfCount)
{
    int index;
#if (UTT_NV_510W == FYES)
    index = 3;
#elif (UTT_NV_515W == FYES)
    index = 4;
#elif ((UTT_NV_520W == FYES)||(UTT_U_1000N == FYES)|| (UTT_NV_512W == FYES)|| (UTT_NV_518W == FYES)|| (UTT_NV_518WVPN == FYES)|| (UTT_NV_842W == FYES))
    switch(wanIfCount) {
        case 1:
	    index = -1;
	case 2:
	    index = 2;
	    break;
	case 3:
	    index = 3;
	    break;
	case 4:
	    index = 4;
	    break;
	case 5:
	    index = 5;
	    break;
	default:
	    break;
    }
#elif ((UTT_NV_518WPLUS == FYES)||(UTT_NV_518WLITE == FYES))
	switch(wanIfCount){
	case 3:
		index = 3;
		break;
	case 4:
		index = 4;
		break;
	case 5:
		index = 5;
		break;
	case 6:
		index = 6;
		break;
	default:
		index = -1;
		break;
	}
#else
    index = -1;
#endif
    return index;

}
/* 获得APCli线路接口index */
extern int getAPCliSlotIndex(void) {
    return getAPCliSlotIndexByWIf(getWanSlotCount());
}
/* 获得APCli线路接口index(出厂配置) */
extern int getDefAPCliSlotIndex(void) {
    int defWanIfCount = 0;
    getDefaultIfCount(&defWanIfCount, NULL);
    return getAPCliSlotIndexByWIf(defWanIfCount);
}
#endif




