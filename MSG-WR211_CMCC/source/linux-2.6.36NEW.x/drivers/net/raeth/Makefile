obj-$(CONFIG_RAETH) += raeth.o
raeth-objs := ra_mac.o mii_mgr.o ra_rfrw.o

#EXTRA_CFLAGS   += -DCONFIG_RAETH_MULTIPLE_RX_RING
ifeq ($(CONFIG_RTL8367RB),y)
EXTRA_CFLAGS  += -D_LITTLE_ENDIAN -DCHIP_RTL8367RB -DMTK7620_ASIC_BOARD
SUB_DIR := $(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth/rtl8367rb
EXTRA_CFLAGS  += -I$(SUB_DIR)
rtl8367rb-api := smi.o 8367_drv.o rtk_api.o rtl8367b_asicdrv.o rtl8367b_asicdrv_phy.o \
               rtl8367b_asicdrv_port.o rtl8367b_asicdrv_mib.o rtl8367b_asicdrv_portIsolation.o \
               rtl8367b_asicdrv_vlan.o rtl8367b_asicdrv_lut.o rtl8367b_asicdrv_dot1x.o rtl8367b_asicdrv_led.o 

ifeq ($(CONFIG_UTT_RDD_MIRROR),y)
rtl8367rb-api += rtl8367b_asicdrv_mirror.o
endif

ifeq ($(CONFIG_UTT_ROUTER_CHIP_FUNC),y)
rtl8367rb-api += uttApi.o 
raeth-objs +=  uttRtChipApi.o
endif

rtl8367rb-objs := $(foreach i, $(rtl8367rb-api), rtl8367rb/$(i))


raeth-objs += $(rtl8367rb-objs)

endif
ifeq ($(CONFIG_RAETH_QOS),y)
raeth-objs += ra_qos.o
endif

ifeq ($(CONFIG_RAETH_QDMA),y)
raeth-objs += raether_qdma.o
endif

ifneq ($(CONFIG_RAETH_QDMA),y)
raeth-objs += raether_pdma.o
endif

raeth-objs += raether.o

ifeq ($(CONFIG_ETHTOOL),y)
raeth-objs += ra_ethtool.o
endif

ifeq ($(CONFIG_RALINK_RT3052_MP2),y)
raeth-objs += mcast.o
endif

ifeq ($(CONFIG_RAETH_NETLINK),y)
raeth-objs += ra_netlink.o
endif

ifeq ($(CONFIG_AR8337N),y)
raeth-objs += athrs17_phy.o
endif

ifeq ($(CONFIG_RAETH_GMAC2),y)
EXTRA_CFLAGS += -DCONFIG_PSEUDO_SUPPORT
endif

ifeq ($(CONFIG_ETH_MEMORY_OPTIMIZATION),y)
EXTRA_CFLAGS   += -DMEMORY_OPTIMIZATION
endif

ifeq ($(CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION),y)
EXTRA_CFLAGS   += -DMEMORY_OPTIMIZATION
endif

ifeq ($(CONFIG_RA_NETWORK_WORKQUEUE_BH),y)
EXTRA_CFLAGS   += -DWORKQUEUE_BH
endif

ifeq ($(CONFIG_TASKLET_WORKQUEUE_SW),y)
EXTRA_CFLAGS   += -DTASKLET_WORKQUEUE_SW
endif
