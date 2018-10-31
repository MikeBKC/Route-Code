
#include <stdlib.h>
#include <stdio.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"

#if (ANTI_SNIFFER == FYES)
#define WAN_ANTISNIFFER_CHAIN "antiSniffer"
/*
 * 判断时候有任何一个借口上的网络尖兵功能开启
 * 返回值：
 *  1	-   存在开启网络尖兵防御功能的接口
 *  0	-   全部关闭
 */
static int isAnyASOn(void) {
    int max;
    int i;
    int rst = 0;
    InterfaceProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;

    max = getWanSlotCount();

    for (i = 1; i <= max; i++) {
	prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(prof) == 0) {
	    if (prof->antiSnifEn == FUN_ENABLE) {
		rst = 1;
		break;
	    }
	}
    }

    return rst;
}

void antiSnifferInit()
{
    int i = 0, max = 0;
    max = getWanSlotCount();

    doSystem("iptables -t mangle -N "WAN_ANTISNIFFER_CHAIN);
    /* 根据开关建立postrouting中的antiSniffer链 */
    if (isAnyASOn()) {
	doSystem("iptables -t mangle -D POSTROUTING -j "WAN_ANTISNIFFER_CHAIN" >/dev/null 2>&1");
	doSystem("iptables -t mangle -A POSTROUTING -j "WAN_ANTISNIFFER_CHAIN" >/dev/null 2>&1");
    }
    for(i = 1; i <= max; i++) {
        doSystem("iptables -t mangle -N wan%d_"WAN_ANTISNIFFER_CHAIN, i);
        doSystem("iptables -t mangle -A antiSniffer -j wan%d_"WAN_ANTISNIFFER_CHAIN, i);
    }
}

void wanInitAntiSniff(int wanIndex)
{
    char *wanName = NULL;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;

    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, wanIndex);
    if(profIf->antiSnifEn == FUN_ENABLE) {
        wanName = getWanIfNamePPP(wanIndex);
        doSystem("iptables -t mangle -A wan%d_"WAN_ANTISNIFFER_CHAIN" -o %s -j IPCRACK --ttl-set 64",
                wanIndex, wanName);
        doSystem("iptables -t mangle -A wan%d_"WAN_ANTISNIFFER_CHAIN" -o %s -p tcp -j HTTPCRACK",
                wanIndex, wanName);
        doSystem("iptables -t mangle -A wan%d_"WAN_ANTISNIFFER_CHAIN" -o %s -p tcp --tcp-flags SYN,RST SYN -j TCPOPTION",
                wanIndex, wanName);
    }
}

void wanClearAntiSniff(int wanIndex)
{
    doSystem("iptables -t mangle -F wan%d_"WAN_ANTISNIFFER_CHAIN, wanIndex);
}

void antiSnifProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const InterfaceProfile *oldProf, const InterfaceProfile *newProf)
{
    if(oldProf->antiSnifEn != newProf->antiSnifEn) {
	/* 重建mangle antiSniffer链, 是否有简单办法只调用一次 */
	if (isAnyASOn()) {
	    doSystem("iptables -t mangle -D POSTROUTING -j "WAN_ANTISNIFFER_CHAIN" >/dev/null 2>&1");
	    doSystem("iptables -t mangle -A POSTROUTING -j "WAN_ANTISNIFFER_CHAIN" >/dev/null 2>&1");
	} else {
	    doSystem("iptables -t mangle -D POSTROUTING -j "WAN_ANTISNIFFER_CHAIN" >/dev/null 2>&1");
	}
        wanClearAntiSniff(index);
    }

    if(newProf->antiSnifEn == FUN_ENABLE && newProf->head.active == 1 
            && _getWanLinkState(index) == 1) {
		wanClearAntiSniff(index);
        wanInitAntiSniff(index);
    }
}

void funInitAntiSniffer()
{
    registerForProfileUpdates(MIB_PROF_INTERFACE, NULL, (ProfileUpdateFnc)antiSnifProfileChange);
    antiSnifferInit();
}
#endif

