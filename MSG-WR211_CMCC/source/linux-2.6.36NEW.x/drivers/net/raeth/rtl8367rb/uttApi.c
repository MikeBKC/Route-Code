#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include "../ra_ioctl.h"
#include "../uttRtChipApi.h"
#include "rtk_api.h"
#include "rtk_error.h"
#include "rtk_api_ext.h"
#include "rtl8367b_asicdrv_port.h"
#include "rtl8367b_asicdrv_vlan.h"

#define UTT_API_DEBUG 0
#if UTT_API_DEBUG
#define UTT_API_PRINT_D(fmt, argx...) printk(fmt, ##argx)
#else
#define UTT_API_PRINT_D(fmt, argx...)
#endif

#ifdef CONFIG_UTT_RDD_MIRROR
static void rtkRddMirrorSet(UttRddMirrorSt* mirror)
{
    rtk_portmask_t rtkMask;
    rtkMask.bits[0] = mirror->sourcePort;

    rtk_mirror_portBased_set(mirror->monitorPort, &rtkMask, &rtkMask);
    UTT_API_PRINT_D("%s %d monitorPort %d sourcePort %x", __func__, __LINE__,mirror->monitorPort,rtkMask.bits[0] );

    return;
}
#endif
#ifdef CONFIG_UTT_WANPORT_SET
#if defined(CONFIG_UTT_WX100) || defined(CONFIG_UTT_NV840G_INT)
//#define TAG_VLAN_DEB
/* NV840G_INT中wanPortsMask 是指wan口数量, 注意 */
static void rtkWanPortSet(unsigned int wanPortsMask)
{
	rtk_portmask_t portmask;
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
	int i;
	rtk_portmask_t Mbrmsk;
	rtk_portmask_t Untagmsk;
	rtk_fid_t Fid;
	int wanNum = wanPortsMask;
	int portnum, portbit;
	rtk_vlan_t vid;

	/* 初始化vlan */
	rtk_vlan_init(); 

	switch(wanNum) {
	    case 1:
		portmask.bits[0] = 0x10E;
		rtk_port_isolation_set(0, portmask);
		portmask.bits[0] = 0x10D;
		rtk_port_isolation_set(1, portmask);
		portmask.bits[0] = 0x10B;
		rtk_port_isolation_set(2, portmask);
		portmask.bits[0] = 0x107;
		rtk_port_isolation_set(3, portmask);
		portmask.bits[0] = 0x0f;
		rtk_port_isolation_set(8, portmask);

		portmask.bits[0] = 0x200;
		rtk_port_isolation_set(4, portmask);
		portmask.bits[0] = 0x10;
		rtk_port_isolation_set(9, portmask);

		break;
	    case 2:
		portmask.bits[0] = 0x106;
		rtk_port_isolation_set(0, portmask);
		portmask.bits[0] = 0x105;
		rtk_port_isolation_set(1, portmask);
		portmask.bits[0] = 0x103;
		rtk_port_isolation_set(2, portmask);
		portmask.bits[0] = 0x07;
		rtk_port_isolation_set(8, portmask);

		portmask.bits[0] = 0x210;
		rtk_port_isolation_set(3, portmask);
		portmask.bits[0] = 0x208;
		rtk_port_isolation_set(4, portmask);
		portmask.bits[0] = 0x18;
		rtk_port_isolation_set(9, portmask);

		break;
	    case 3:
		portmask.bits[0] = 0x102;
		rtk_port_isolation_set(0, portmask);
		portmask.bits[0] = 0x101;
		rtk_port_isolation_set(1, portmask);
		portmask.bits[0] = 0x03;
		rtk_port_isolation_set(8, portmask);

		portmask.bits[0] = 0x218;
		rtk_port_isolation_set(2, portmask);
		portmask.bits[0] = 0x214;
		rtk_port_isolation_set(3, portmask);
		portmask.bits[0] = 0x20c;
		rtk_port_isolation_set(4, portmask);
		portmask.bits[0] = 0x1c;
		rtk_port_isolation_set(9, portmask);

		break;
	    case 4:
		portmask.bits[0] = 0x100;
		rtk_port_isolation_set(0, portmask);
		portmask.bits[0] = 0x01;
		rtk_port_isolation_set(8, portmask);

		portmask.bits[0] = 0x21c;
		rtk_port_isolation_set(1, portmask);
		portmask.bits[0] = 0x21a;
		rtk_port_isolation_set(2, portmask);
		portmask.bits[0] = 0x216;
		rtk_port_isolation_set(3, portmask);
		portmask.bits[0] = 0x20e;
		rtk_port_isolation_set(4, portmask);
		portmask.bits[0] = 0x1e;
		rtk_port_isolation_set(9, portmask);

		break;
	    default:
		break;
	}
	/* 初始化lan在一个vlan中, eth2(port8)出包不去tag */
	Mbrmsk.bits[0] = 0x3ff;
	Untagmsk.bits[0] = 0x2ff;
	rtk_vlan_set(1, Mbrmsk, Untagmsk, 0);

	if (wanNum > 1) {
	    /* vlan */
	    for (i = 0; i < wanNum; i++) { /* i = 0 - wan1 - port4 */
		portnum = 4 - i;
		portbit = 0x1 << portnum;
		vid = i + 2; /* wan1 - port4 - eth3.2, wan2 - port3 - eth3.3 */
		Mbrmsk.bits[0] = 0x200 | portbit;
		Untagmsk.bits[0] = portbit;
		rtk_vlan_set(vid, Mbrmsk, Untagmsk, 0);
		rtk_vlan_portPvid_set(portnum, vid, 0);
		rtk_l2_limitLearningCnt_set(portnum, 0);    
	    }
	} else {
	    rtk_l2_limitLearningCnt_set(4,0);
	}

#if defined(TAG_VLAN_DEB)
	printk("===============%s, wanNum=%d\n", __func__, wanNum);
	rtk_port_isolation_get(0, &portmask);
	printk("port0 isolation portmask=0x%x\n", portmask.bits[0]);
	rtk_port_isolation_get(1, &portmask);
	printk("port1 isolation portmask=0x%x\n", portmask.bits[0]);
	rtk_port_isolation_get(2, &portmask);
	printk("port2 isolation portmask=0x%x\n", portmask.bits[0]);
	rtk_port_isolation_get(3, &portmask);
	printk("port3 isolation portmask=0x%x\n", portmask.bits[0]);
	rtk_port_isolation_get(4, &portmask);
	printk("port4 isolation portmask=0x%x\n", portmask.bits[0]);
	rtk_port_isolation_get(8, &portmask);
	printk("port8 isolation portmask=0x%x\n", portmask.bits[0]);
	rtk_port_isolation_get(9, &portmask);
	printk("port9 isolation portmask=0x%x\n", portmask.bits[0]);

	{
	    unsigned int pri;
	    rtk_vlan_portPvid_get(0, &vid, &pri);
	    printk("port0 pvid=%d\n", vid);
	    rtk_vlan_portPvid_get(1, &vid, &pri);
	    printk("port1 pvid=%d\n", vid);
	    rtk_vlan_portPvid_get(2, &vid, &pri);
	    printk("port2 pvid=%d\n", vid);
	    rtk_vlan_portPvid_get(3, &vid, &pri);
	    printk("port3 pvid=%d\n", vid);
	    rtk_vlan_portPvid_get(4, &vid, &pri);
	    printk("port4 pvid=%d\n", vid);
	    rtk_vlan_portPvid_get(8, &vid, &pri);
	    printk("port8 pvid=%d\n", vid);
	    rtk_vlan_portPvid_get(9, &vid, &pri);
	    printk("port9 pvid=%d\n", vid);
	}

	for (i = 1; i < 6; i++) {
	    rtk_vlan_get(i + 1, &Mbrmsk, &Untagmsk, &Fid);
	    printk("=======================%s, vid=%d, Mbrmsk=0x%x,Untagmsk=0x%x,Fid=%d\n", __func__, i+1, Mbrmsk.bits[0], Untagmsk.bits[0], Fid);
	}
#endif
#else
	portmask.bits[0] = 0x10E;
	rtk_port_isolation_set(0, portmask);
	portmask.bits[0] = 0x10D;
	rtk_port_isolation_set(1, portmask);
	portmask.bits[0] = 0x10B;
	rtk_port_isolation_set(2, portmask);
	portmask.bits[0] = 0x107;
	rtk_port_isolation_set(3, portmask);
	portmask.bits[0] = 0x0f;
	rtk_port_isolation_set(8, portmask);

	portmask.bits[0] = 0x200;
	rtk_port_isolation_set(4, portmask);
	portmask.bits[0] = 0x10;
	rtk_port_isolation_set(9, portmask);

    rtk_l2_limitLearningCnt_set(4,0);
#endif

	return;
}
#else
/**
 * 通过vlan实现 lan wan的划分
 * 注意：这里目前仅仅实现了 lan口从左 数起的情况
 */
static void rtkWanPortSet(unsigned int wanPortsMask)
{
    rtk_portmask_t mbrmsk, untagmsk;
    int curPort, curVlan, i;
    unsigned int lanmsk;
    static int init_port = 0;
    
    if(init_port==0)
    {
        mbrmsk.bits[0] = 0x7F;
	for(i=0;i<5;i++)
        {
	    rtk_port_isolation_set(i, mbrmsk);
	}
        init_port = 1;
    }
    if(wanPortsMask == 0)
    {
	return;
    }
    lanmsk = (~wanPortsMask) & 0x1F ;
    mbrmsk.bits[0] = 0x40 | lanmsk;
    untagmsk.bits[0] = lanmsk;
    //untagmsk.bits[0] = 1;
    /* for mantis 0009777
     *the vlan include all lan ports set differnt fid with wan ports
     * so that there will no conflict when lan ports and wan ports in same broadcast domain*/
    if(rtk_vlan_set(1, mbrmsk, untagmsk, 1)!=RT_ERR_OK)
    {
	printk("set vlan error\n");
    }

    curVlan = 2;
    for(curPort=4; curPort>=0; curPort--)
    {
	if((wanPortsMask & (0x1 << curPort)) != 0)
	{
	    mbrmsk.bits[0] = (0x40 | (0x1 << curPort));
	    untagmsk.bits[0] = (0x1 << curPort);
	    if(rtk_vlan_set(curVlan, mbrmsk, untagmsk, 0xFFFFu)!=RT_ERR_OK)
	    {
		printk("set vlan error\n");
	    }
	    rtk_vlan_portPvid_set(curPort, curVlan, 0);
	    /*disabled all wan port mac lerning funtion
	     * for mantis 0009777
	     * note: this bug can also be killed by set diffrent vlan or port based fid
	     *       and just diabled mac learn can reduce mac entry hash clash in lookup table
	     */
	    //rtk_l2_limitLearningCnt_set(curPort,0);    
	    curVlan++;
	}
	else
	{
	    /*lan port*/
	    rtk_vlan_portPvid_set(curPort, 1, 0);
	}
    }

    rtk_vlan_portPvid_set(6, 1, 0);

//    rtk_l2_limitLearningCnt_set(6,0);
    return;
}
#endif
#endif

#ifdef CONFIG_UTT_RDD_PVLAN
static void rtkRddPvlanSet (UttRddPvlanSt *pvlan)
{
#if 1
    rtk_portmask_t mbrmsk;
    /*must forward to cpu but need not to self*/
//    mbrmsk.bits[0] = 0x100 | pvlan->dstPortMsk & (~(1u<<(pvlan->pno)));
    mbrmsk.bits[0] = 0x40 | pvlan->dstPortMsk & (~(1u<<(pvlan->pno)));
    rtk_port_isolation_set(pvlan->pno, mbrmsk);
#else
    rtk_portmask_t mbrmsk, untagmsk;
    rtk_vlan_t pvid;
    /**
     *  低 8位用于做 cpu 识别端口时添加的tag 如eth2.1
     *  高于8位用于port vlan使用 
     *  以达到 port vlan 和 tag vlan共存的目的
     *  这里注意这里的  8 必须和 netif_recv中修该的tag 对应起来
     */
    pvid = (((pvlan->pno+1) << 8u) | 1u);/*这里必须从0x101开始,因为pvid为1的给cpu口使用*/
    UTT_API_PRINT_D("%s %d pno %d pvid %x mbrmsk %x\n", __func__, __LINE__, pvlan->pno,pvid, pvlan->dstPortMsk);
    untagmsk.bits[0] = pvlan->dstPortMsk;
    mbrmsk.bits[0] = 0x100 | pvlan->dstPortMsk;/*cpu port must add tag*/
    rtk_vlan_set(pvid, mbrmsk, untagmsk, 1);/*lan ports fid set to 1*/
    rtk_vlan_portPvid_set(pvlan->pno, pvid, 0);
#endif
    return;
}
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
static void rtkPvidSet(UttPvidSt *pPvidConf) {
    rtk_vlan_portPvid_set(pPvidConf->port, pPvidConf->pvid, 0);
#if defined(TAG_VLAN_DEB)
    printk("%s,%d, set:port=%d,pvid=%d\n", __func__, __LINE__, pPvidConf->port, pPvidConf->pvid);
    {
	int i;
	unsigned int pri, pvid;
	for (i = 0; i < 5; i++) {
	    rtk_vlan_portPvid_get(i, &pvid, &pri);
	    printk("%s, %d, get: port=%d,pvid=%d,pri=%d\n", __func__, __LINE__, i, pvid, pri);
	}
    }
#endif
    return;
}

static void rtkTagVlanSet(UttTagVlanSt *pTagVlanConf) {
    rtk_portmask_t mbrmsk, untagmsk;
    rtk_vlan_t vid;
    vid = pTagVlanConf->vid;
    mbrmsk.bits[0] = pTagVlanConf->mbrmsk.bits[0];
    untagmsk.bits[0] = pTagVlanConf->untagmsk.bits[0];

    rtk_vlan_set(vid, mbrmsk, untagmsk, 0);
#if defined(TAG_VLAN_DEB)
    printk("%s, %d, vid=%d, mbrmsk=0x%x,untagmsk=0x%x\n", __func__, __LINE__, vid, mbrmsk.bits[0], untagmsk.bits[0]);
    {
	int i;
	rtk_portmask_t mbrmsk1, untagmsk1;
	unsigned int fid1;
	rtk_vlan_t vid1;
	for (i = 0; i < 5; i++) {
	    rtk_vlan_get(i, &mbrmsk1, &untagmsk1, &fid1);
	    printk("%s %d, vid=%d,mbrmsk=0x%x,untagmsk=0x%x,fid=%d\n",__func__,__LINE__,i,mbrmsk1.bits[0], untagmsk1.bits[0], fid1);
	}
    }
#endif
    return 0;
}

static void rtkTagVlanDel(UttTagVlanSt *pTagVlanConf) {
    rtk_portmask_t mbrmsk, untagmsk;
    rtk_vlan_t vid;
    vid = pTagVlanConf->vid;
    mbrmsk.bits[0] = 0;
    untagmsk.bits[0] = 0;

    rtk_vlan_set(vid, mbrmsk, untagmsk, 0);
    return 0;
}

#endif

extern void uttRtChipApiRegister(void)
{
#ifdef CONFIG_UTT_RDD_MIRROR
    uttRtChipApi.pRddMirrorSet = rtkRddMirrorSet;
#endif
#ifdef CONFIG_UTT_WANPORT_SET
    uttRtChipApi.pWanPortSet = rtkWanPortSet;
#endif
#ifdef CONFIG_UTT_RDD_PVLAN
    uttRtChipApi.pRddPvlanSet = rtkRddPvlanSet ;
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
    uttRtChipApi.pPvidSet = rtkPvidSet;
    uttRtChipApi.pTagVlanSet = rtkTagVlanSet;
    uttRtChipApi.pTagVlanDel = rtkTagVlanDel;
#endif
    return;
}


