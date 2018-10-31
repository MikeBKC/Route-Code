#include <linux/autoconf.h>
#ifdef CONFIG_UTT_ROUTER_CHIP_FUNC
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#ifdef CONFIG_PPC_85xx
#include <fcntl.h>
#include <sys/stat.h>
#include "uttRtChipApi.h"
#define RTL8367M_DEVICE "/dev/utt_rtl8367m"
static void rtChipFuncIocSet(UttRtChipFuncSt* conf)
{
	int fp = 0 ;
	fp = open(RTL8367M_DEVICE, O_RDWR, S_IRUSR|S_IWUSR);
	SWORD_PRINTF("%s-%d, fp = %d\n", __func__, __LINE__, fp);
	if (fp < 0) {
		perror("open");
		return;
	}
	if(ioctl(fp, UTT_ROUTER_CHIP_FUNC, conf) < 0) {
		perror("ioctl");
	}
	close(fp);
	return;
}
#else
#include "ra_ioctl.h"
static void rtChipFuncIocSet(UttRtChipFuncSt* conf)
{
    struct ifreq ifr;
    int sk = 0;
    sk = socket(AF_INET, SOCK_DGRAM, 0);
    if (sk >= 0) {
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = conf;
	if(ioctl(sk, UTT_ROUTER_CHIP_FUNC, &ifr) < 0)
	{
	    perror("ioctl : Are you sure UTT_RT3883_DEBUG in kernel is enabled?");
	}
	close(sk);
    }
    return;
}
#endif

#ifdef CONFIG_UTT_WANPORT_SET
/*wan port set in chip*/
void wanPortSet()
{
    UttRtChipFuncSt conf;
    Uint32 wanChipNums = 0u;
    SystemProfile* profSys = (SystemProfile*)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    memset(&conf, 0 ,sizeof(conf));
    conf.confType = UTT_WANPORT_SET;
    /*这里因注意所有的口自左到右 从0开始*/
#if defined(CONFIG_BOARD_P1010_C)
    /* P1010-C在这里不是划分所有wan口，而只是划分8367M的wan口, 要除去8336的wan口 */
    wanChipNums = P1010C_8367M_IF_COUNT - profSys->lanPortCount;
#else
    wanChipNums = LAN_WAN_PORT_COUNT - profSys->lanPortCount;
#endif
#if (TAG_VLAN == FYES)
    conf.confData.wanPortsMask =  wanChipNums;
#else
    conf.confData.wanPortsMask =  convertLenToBinMask(wanChipNums) << profSys->lanPortCount;
#endif
    rtChipFuncIocSet(&conf);
    SWORD_PRINTF("%s-%d, wanPortsMask = 0x%x\n", __func__, __LINE__, conf.confData.wanPortsMask);

    return;
}
#endif

#ifdef CONFIG_UTT_RDD_MIRROR
/* 
 * 端口镜像
 * 0号口监视其他的lan口
 * */
void rddMirrorSet(SystemProfile *profSys)
{

    UttRtChipFuncSt conf;
    memset(&conf, 0 ,sizeof(conf));
    conf.confType = UTT_RDD_MIRROR_SET;
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RTL8367RB)
    conf.confData.mirror.monitorPort = profSys->mirror.monitorPort - 1;
#else
    conf.confData.mirror.monitorPort = 0;/*监视口*/
#endif
    /*被监视口。不开启镜像则为全零*/
    if(profSys->portMirrorEn == FUN_ENABLE) 
    {
#if defined(CONFIG_RTL8367M) || defined(CONFIG_RTL8306_PHY) || defined(CONFIG_RTL8367RB)
	if(profSys->mirror.sourcePort > 0)
	{
	    conf.confData.mirror.sourcePort = 1u << (profSys->mirror.sourcePort-1);
	}
#else
	conf.confData.mirror.sourcePort = convertLenToBinMask(profSys->lanPortCount) & (~0x1u);
#endif
    }
    rtChipFuncIocSet(&conf);

    return;
}
#endif

#ifdef CONFIG_UTT_RDD_PVLAN
/**
 * port vlan
 * pno : >=0 
 * dPortsMsk : pno可通信的目的端口 掩码， 必须包含pno自身
 */
void rddPvlanSet(Uint32 pno,Uint32 dPortsMsk)
{

    UttRtChipFuncSt conf;
    memset(&conf, 0 ,sizeof(conf));
    conf.confType = UTT_RDD_PVLAN_SET;
    conf.confData.pvlan.pno = pno;
    conf.confData.pvlan.dstPortMsk = dPortsMsk;
    rtChipFuncIocSet(&conf);
    return;
}
#endif

#if (TAG_VLAN == FYES)
extern void pvidSet(Uint32 port, Uint32 pvid) {
    UttRtChipFuncSt conf;
    memset(&conf, 0 ,sizeof(conf));
    conf.confType = UTT_PVID_SET;
    conf.confData.pvidConf.port = port;
    conf.confData.pvidConf.pvid = pvid;
//    printf("%s, %d, port = %d, pvid = %d\n", __func__, __LINE__, conf.confData.pvidConf.port, conf.confData.pvidConf.pvid);
    rtChipFuncIocSet(&conf);
}

/*
 * op - 1 添加 修改; 0 删除
 */
extern void tagVlanConfig(int op, Uint32 vid, Uint32 mbrmsk, Uint32 untagmsk) {
    UttRtChipFuncSt conf;
    memset(&conf, 0 ,sizeof(conf));
    if (op == 1) {
	conf.confType = UTT_TAG_VLAN_SET;
    } else {
	conf.confType = UTT_TAG_VLAN_DEL;
    }
    conf.confData.tagVlanConf.vid = vid;
    conf.confData.tagVlanConf.mbrmsk.bits[0] = mbrmsk;
    conf.confData.tagVlanConf.untagmsk.bits[0] = untagmsk;
//    printf("%s, %d, vid=%d,mbrmsk=0x%x,untagmsk=0x%x\n", __func__, __LINE__, conf.confData.tagVlanConf.vid, conf.confData.tagVlanConf.mbrmsk.bits[0], conf.confData.tagVlanConf.untagmsk.bits[0]);
    rtChipFuncIocSet(&conf);
}
#endif

#endif
