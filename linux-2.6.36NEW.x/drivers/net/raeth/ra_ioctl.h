#ifndef _RAETH_IOCTL_H
#define _RAETH_IOCTL_H

/* ioctl commands */
#define RAETH_ESW_REG_READ		0x89F1
#define RAETH_ESW_REG_WRITE		0x89F2
#define RAETH_MII_READ			0x89F3
#define RAETH_MII_WRITE			0x89F4
#define RAETH_ESW_INGRESS_RATE		0x89F5
#define RAETH_ESW_EGRESS_RATE		0x89F6
#define RAETH_QDMA_REG_READ		0x89F8
#define RAETH_QDMA_REG_WRITE		0x89F9
#define RAETH_QDMA_QUEUE_MAPPING        0x89FA
#define RAETH_QDMA_READ_CPU_CLK         0x89FB
#define RAETH_MII_READ_CL45             0x89FC
#define RAETH_MII_WRITE_CL45            0x89FD
#ifdef CONFIG_RTL8306E
#define RAETH8306_ESW_REG_READ 0x89FA
#define RAETH8306_ESW_REG_WRITE 0x89FB
#endif

#ifdef CONFIG_UTT_ROUTER_CHIP_FUNC 
#define UTT_ROUTER_CHIP_FUNC            0x89F7
#else
#define RAETH_ESW_PHY_DUMP		0x89F7
#endif

#if defined(CONFIG_AR8337N)
#define RAETH_8337_READ 0x89F8
#define RAETH_8337_WRITE 0x89F9
#endif

#if defined (CONFIG_RALINK_RT6855) || defined(CONFIG_RALINK_RT6855A) || \
    defined (CONFIG_RALINK_MT7620) || defined(CONFIG_RALINK_MT7621)

#define REG_ESW_WT_MAC_MFC              0x10
#define REG_ESW_ISC                     0x18
#define REG_ESW_WT_MAC_ATA1             0x74
#define REG_ESW_WT_MAC_ATA2             0x78
#define REG_ESW_WT_MAC_ATWD             0x7C
#define REG_ESW_WT_MAC_ATC              0x80 

#define REG_ESW_TABLE_TSRA1		0x84
#define REG_ESW_TABLE_TSRA2		0x88
#define REG_ESW_TABLE_ATRD		0x8C


#define REG_ESW_VLAN_VTCR		0x90
#define REG_ESW_VLAN_VAWD1		0x94
#define REG_ESW_VLAN_VAWD2		0x98


#define REG_ESW_VLAN_ID_BASE		0x100

//#define REG_ESW_VLAN_ID_BASE		0x50
#define REG_ESW_VLAN_MEMB_BASE		0x70
#define REG_ESW_TABLE_SEARCH		0x24
#define REG_ESW_TABLE_STATUS0		0x28
#define REG_ESW_TABLE_STATUS1		0x2C
#define REG_ESW_TABLE_STATUS2		0x30
#define REG_ESW_WT_MAC_AD0		0x34
#define REG_ESW_WT_MAC_AD1		0x38
#define REG_ESW_WT_MAC_AD2		0x3C

#else
/* rt3052 embedded ethernet switch registers */
#define REG_ESW_VLAN_ID_BASE		0x50
#define REG_ESW_VLAN_MEMB_BASE		0x70
#define REG_ESW_TABLE_SEARCH		0x24
#define REG_ESW_TABLE_STATUS0		0x28
#define REG_ESW_TABLE_STATUS1		0x2C
#define REG_ESW_TABLE_STATUS2		0x30
#define REG_ESW_WT_MAC_AD0		0x34
#define REG_ESW_WT_MAC_AD1		0x38
#define REG_ESW_WT_MAC_AD2		0x3C
#endif


#if defined(CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7628)
#define REG_ESW_MAX			0x16C
#elif defined (CONFIG_RALINK_RT6855) || defined(CONFIG_RALINK_RT6855A) || \
      defined (CONFIG_RALINK_MT7620)
#define REG_ESW_MAX			0x7FFFF
#else //RT305x, RT3350
#define REG_ESW_MAX			0xFC
#endif
#define REG_HQOS_MAX			0x3FFF


typedef struct rt3052_esw_reg {
	unsigned int off;
	unsigned int val;
} esw_reg;

typedef struct ralink_mii_ioctl_data {
	__u32   phy_id;
	__u32   reg_num;
	__u32   val_in;
	__u32   val_out;
	__u32   port_num;
	__u32   dev_addr;
	__u32   reg_addr;
} ra_mii_ioctl_data;

#ifdef CONFIG_RTL8306E 
typedef struct ralink_8306_mii_ioctl_data {
	__u32	phy_id;
	__u32	reg_num;
	__u32	page;
	__u32	val_in;
	__u32	val_out;
} ra_8306_mii_ioctl_data;
#endif
typedef struct rt335x_esw_reg {
	unsigned int on_off;
	unsigned int port;
	unsigned int bw;/*Mbps*/
} esw_rate;

#ifdef CONFIG_UTT_ROUTER_CHIP_FUNC
typedef enum{
    UTT_GENE_ROUTER_CHIP_FUNC = 0,
#ifdef CONFIG_UTT_RDD_MIRROR 
    UTT_RDD_MIRROR_SET,
#endif
#ifdef CONFIG_UTT_RDD_PVLAN 
    UTT_RDD_PVLAN_SET,
#endif
#ifdef CONFIG_UTT_WANPORT_SET
    UTT_WANPORT_SET,
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
    UTT_TAG_VLAN_SET,
    UTT_TAG_VLAN_DEL,
    UTT_PVID_SET,
#endif
    UTT_MAX_ROUTER_CHIP_FUNC
}UTT_ROUTER_CHIP_FUNC_TYPE;

#ifdef CONFIG_UTT_RDD_MIRROR
/*
 * 这里的所有端口 编号 指的是设备上自左到右的顺序
 * 且从0开始
 * 如果芯片跟这个不同。则芯片实现函数 需自行转换
 * */
typedef struct UttRddMirror_st{
    unsigned int monitorPort;
    unsigned int sourcePort;
}UttRddMirrorSt;
#endif
#ifdef CONFIG_UTT_RDD_PVLAN
typedef struct UttRddPvlan_st{
    unsigned int pno;/*chip port >=0*/
    unsigned int dstPortMsk;/*destination port mask*/
}UttRddPvlanSt;
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
typedef struct portmask_s
{
    unsigned int bits[1];
} portmask_t;

typedef struct UttTagVlan_st {
    unsigned int vid;
    portmask_t mbrmsk;
    portmask_t untagmsk;
}UttTagVlanSt;

typedef struct UttPvid_st {
    unsigned int port;
    unsigned int pvid;
}UttPvidSt;

#endif
typedef struct UttRtChipFunc_st{
    UTT_ROUTER_CHIP_FUNC_TYPE confType;
    union{
#ifdef CONFIG_UTT_RDD_MIRROR
	UttRddMirrorSt mirror;
#endif
#ifdef CONFIG_UTT_RDD_PVLAN
	UttRddPvlanSt pvlan;
#endif
#ifdef CONFIG_UTT_WANPORT_SET
        unsigned int wanPortsMask;
#endif
#if defined(CONFIG_UTT_TAG_VLAN_3662A)
	UttPvidSt pvidConf;
	UttTagVlanSt tagVlanConf;
#endif
    }confData;
} UttRtChipFuncSt;

#endif

#endif
