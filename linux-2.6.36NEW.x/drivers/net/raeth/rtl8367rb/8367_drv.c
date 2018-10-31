#include <linux/kernel.h>
#include "rtk_api.h"
#include "rtk_error.h"
#include "rtk_api_ext.h"
#if 1
#include "rtl8367b_asicdrv_port.h"
#include "rtl8367b_asicdrv_phy.h"
#include "rtl8367b_asicdrv_vlan.h"
#else
#include "rtl8370_asicdrv_port.h"
#include "rtl8370_asicdrv_phy.h"
#include "rtl8370_asicdrv_vlan.h"
#endif
#include "smi.h"
#include <linux/autoconf.h>


#define RT2880_REG(x)			(*((volatile u32 *)(x)))

#ifdef CONFIG_UTT_ROUTER_CHIP_FUNC
extern void uttRtChipApiRegister(void);
#endif

static int led_init(void)
{
    rtk_portmask_t portmask;

    portmask.bits[0]=0x1f;
    rtk_led_enable_set(LED_GROUP_2, portmask);
    rtk_led_operation_set(LED_OP_PARALLEL);
    rtk_led_blinkRate_set(LED_BLINKRATE_96MS);
    rtk_led_groupConfig_set(LED_GROUP_2,LED_CONFIG_LINK_ACT);  

    return 0;
}

int switch_init(void)
{
    rtk_mode_ext_t mode;
    rtk_port_mac_ability_t mac_cfg;

#ifdef CONFIG_UTT_ROUTER_CHIP_FUNC
    uttRtChipApiRegister();
#endif

    if(smi_init() != 0)
	printk("smi init failed\n");

    if(rtk_switch_init() != 0)
	printk("rtk switch init failed\n");

    if(led_init() != 0)
	printk("led init failed\n");
#if 0
    for(i=0 ;i <= 4 ;i++)
    {/*reset all phy*/
	rtl8370_setAsicPHYReg(i, PHY_CONTROL_REG, 0x9140);
    }
#endif

    mode = MODE_EXT_RGMII;
    mac_cfg.forcemode = MAC_FORCE;
    mac_cfg.speed     = SPD_1000M;
    mac_cfg.duplex    = FULL_DUPLEX;
    mac_cfg.link      = PORT_LINKUP;
    mac_cfg.nway      = DISABLED;
    mac_cfg.txpause   = ENABLED;
    mac_cfg.rxpause   = ENABLED;
    if(rtk_port_macForceLinkExt0_set(mode, &mac_cfg)!=RT_ERR_OK)
    {
	printk("set gmac0 failed\n");
    }
    rtk_port_rgmiiDelayExt0_set(1, 0);

#if 0
    mode = MODE_EXT_RGMII;
    mac_cfg.forcemode = MAC_FORCE;
    mac_cfg.speed     = SPD_1000M;
    mac_cfg.duplex    = FULL_DUPLEX;
    mac_cfg.link      = PORT_LINKUP;
    mac_cfg.nway      = DISABLED;
    mac_cfg.txpause   = ENABLED;
    mac_cfg.rxpause   = ENABLED;
    if(rtk_port_macForceLinkExt1_set(mode, &mac_cfg)!=RT_ERR_OK)
    {
	printk("set gmac1 failed\n");
    }

    if(rtk_port_rgmiiDelayExt1_get(&tx_delay, &rx_delay)!=RT_ERR_OK)
    {
	printk("get delay1 failed\n");
    }
    else
    {
	printk("tx1 delay %d, rx1 delay %d\n", tx_delay, rx_delay);
    }
    rtk_port_rgmiiDelayExt1_set(0, 0);
#endif

#if 0
    rtk_cpu_enable_set(1);
    rtk_cpu_enable_get(&en);
    printk("%s: cpu enable %d\n", __func__, en);
    cpu_mode = CPU_INSERT_TO_NONE;
    rtk_cpu_tagPort_set(5, cpu_mode);
#endif
    if(rtk_vlan_init()!=0)
    {
	printk("switch vlan init error\n");
    }

#if 0
    portmask.bits[0] = 0x7F;
    for(i=0;i<5;i++)
    {
         rtk_port_isolation_set(i, portmask);
    }

    mbrmsk.bits[0] = 0x0FF;
    untagmsk.bits[0] = 0x0FF;
    if(rtk_vlan_set(1, mbrmsk, untagmsk, 0)!=RT_ERR_OK)
    {
	printk("set vlan error\n");
    }
    rtk_vlan_portPvid_set(0, 1, 0);
    rtk_vlan_portPvid_set(1, 1, 0);
    rtk_vlan_portPvid_set(2, 1, 0);
    rtk_vlan_portPvid_set(3, 1, 0);
    rtk_vlan_portPvid_set(4, 1, 0);
    rtk_vlan_portPvid_set(5, 1, 0);
    rtk_vlan_portPvid_set(6, 1, 0);
    rtk_vlan_portPvid_set(7, 1, 0);
  //  rtk_vlan_portPvid_set(8, 1, 0);
  //  rtk_vlan_portPvid_set(9, 1, 0);
#endif
#if 0
    udelay(2);

    if(rtk_l2_init()!=0)
    {
	printk("switch lut init error\n");
    }
    mac.octet[0]= 0x30;
    mac.octet[1]= 0x30;
    mac.octet[2]= 0x3a;
    mac.octet[3]= 0x41;
    mac.octet[4]= 0x41;
    mac.octet[5]= 0x3a;
    memset(&entry, 0, sizeof(entry));
    entry.port = 5;
    entry.ivl = 1;
    entry.cvid = 0;
    entry.fid = 0;
    entry.efid= 0;
    entry.is_static = 1;

    en = rtk_l2_addr_add(&mac, &entry);
    if(en!=0)
    {
	printk("add addr error %d\n", en);
    }
    for(en=0;en<7;en++)
    {
	if(rtk_vlan_portAcceptFrameType_get(en, &type)==0)
	    printk("port: %d, type %d\n", en, (unsigned int)type);
	else
	    printk("get type error %d\n");
	if(rtk_vlan_portIgrFilterEnable_get(en, &enable)==0)
	    printk("port: %d, filter %d\n", en, (unsigned int)enable);
	else
	    printk("get Ingr Flt error\n");

    }
#endif
    //rtk_stat_global_reset();
    return 0;
}

#if 0
int _8367_dump(void)
{
    int i;
    rtk_stat_port_cntr_t port_cntrs, *pPort_cntrs;
    rtk_mac_cnt_t ct;
    rtk_data_t en;
    unsigned int val, reg;
    rtk_mode_ext_t mode;
    rtk_port_mac_ability_t portability, *pPortability;

#if 0
    val = RT2880_REG(0xb0100000+0x60);
    printk("gdma2cfg: 0x%08x\n", val);
    val = RT2880_REG(0xb0100000+0x0c);
    printk("rst glo: 0x%08x\n", val);
    val = RT2880_REG(0xb0100000+0x40);
    printk("fq cfg: 0x%08x\n", val);
    val = RT2880_REG(0xb0000000+0x14);
    printk("syscfg: 0x%08x\n", val);
    val = RT2880_REG(0xb0000300+0x40);
    printk("ddrcfg0: 0x%08x\n", val);
    val = RT2880_REG(0xb0000300+0x44);
    printk("ddrcfg1: 0x%08x\n", val);
    val = RT2880_REG(0xb0100000+0x18);
    printk("mdiocfg: 0x%08x\n", val);
#endif
#if 1
    pPortability = &portability;
    rtk_port_macForceLinkExt0_get(&mode, pPortability);
    printk("rgmii0: mode = %d, forcemode %d, speed = %d, dup = %d, link %d, nway %d, %d, %d\n", mode, pPortability->forcemode, \
	    pPortability->speed,pPortability->duplex,pPortability->link,pPortability->nway, pPortability->txpause, pPortability->rxpause);

    rtk_port_macForceLinkExt1_get(&mode, pPortability);
    printk("rgmii1: mode = %d, forcemode %d, speed = %d, dup = %d, link %d, nway %d, %d, %d\n", mode, pPortability->forcemode, \
	    pPortability->speed,pPortability->duplex,pPortability->link,pPortability->nway, pPortability->txpause, pPortability->rxpause);
    //smi_init();
    //rtk_switch_init();
    //switch_init();
#endif
#if 1
#if 0
    reg = 0xb0100400;
    printk("tx1 good pkt: %u, rx1 good pkt: %u, rx1 fcs err pkt: %u %u %u %u\n", RT2880_REG(reg+0x73c), RT2880_REG(reg+0x724), \
	    RT2880_REG(reg + 0x728), RT2880_REG(reg + 0x72c), RT2880_REG(reg + 0x730), RT2880_REG(reg + 0x734));
    printk("tx2 good pkt: %u, rx2 good pkt: %u, rx2 fcs err pkt: %u %u %u %u\n", RT2880_REG(reg+0x76c), RT2880_REG(reg+0x764), \
	    RT2880_REG(reg + 0x768), RT2880_REG(reg + 0x76c), RT2880_REG(reg + 0x770), RT2880_REG(reg + 0x774));
#endif
    pPort_cntrs = &port_cntrs;
    for(i=0;i<8;i++)
    {
	printk("\n---------------port: %d ------------\n", i);
	memset(&port_cntrs, 0, sizeof(port_cntrs));
	if(rtk_stat_port_getAll(i, pPort_cntrs)!=0)
	{
	    printk("port %d: get error \n", i);
	}

	printk("ifInOctets: %u   ifInUcastPkts; %u  etherStatsOctets: %u\n", (unsigned int)(pPort_cntrs->ifInOctets), (unsigned int)(pPort_cntrs->ifInUcastPkts), (unsigned int)(pPort_cntrs->etherStatsOctets));

	printk("ifOutOctets: %u  etherStatsMcastPkts: %u  etherStatsBcastPkts: %u\n", (unsigned int)(pPort_cntrs->ifOutOctets), (unsigned int)(pPort_cntrs->etherStatsMcastPkts), (unsigned int)(pPort_cntrs->etherStatsBcastPkts));

	printk("etherStatsDropEvents: %u  etherStatsPkts64Octets: %u  etherStatsPkts65to127Octets: %u\n", (unsigned int)(pPort_cntrs->etherStatsDropEvents), (unsigned int)(pPort_cntrs->etherStatsPkts64Octets), (unsigned int)(pPort_cntrs->etherStatsPkts65to127Octets));

	printk("dot3StatsFCSErrors: %u  etherStatsFragments: %u  etherStatsCollisions: %u\n", (unsigned int)(pPort_cntrs->dot3StatsFCSErrors), (unsigned int)(pPort_cntrs->etherStatsFragments), (unsigned int)(pPort_cntrs->etherStatsCollisions));

	printk("dot3StatsSymbolErrors : %u  etherStatsJabbers: %u  dot3StatsExcessiveCollisions: %u\n", (unsigned int)(pPort_cntrs->dot3StatsSymbolErrors), (unsigned int)(pPort_cntrs->etherStatsJabbers), (unsigned int)(pPort_cntrs->dot3StatsExcessiveCollisions));
    }
#endif
    return 0;
}
#endif
