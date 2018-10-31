/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2007 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <linux/types.h>
#include <linux/kernel.h>

#define ATHR_PHY_MAX 5

extern uint32_t mii_mgr_write(uint32_t phy_addr, uint32_t phy_register, uint32_t write_data);
extern uint32_t mii_mgr_read(uint32_t phy_addr, uint32_t phy_register, uint32_t *read_data);

static void athrs17_header_read(uint32_t dev_id, uint32_t reg_addr, uint8_t *reg_data, uint32_t len);
static void athrs17_header_write(uint32_t dev_id, uint32_t reg_addr, uint8_t *reg_data, uint32_t len);

void phy_reg_write(int dev_id, uint32_t phy_addr, uint32_t reg_addr,uint32_t write_data)
{
    mii_mgr_write(phy_addr,reg_addr,write_data);
}

uint16_t phy_reg_read(int dev_id, uint32_t phy_addr, uint32_t reg_addr)
{
    uint32_t data;
    mii_mgr_read(phy_addr,reg_addr,&data);
    return ((uint16_t)(data & 0xffff));
}

void
athrs17_phy_write(uint32_t dev_id, uint32_t phy_addr, uint32_t reg, uint16_t data)
{
    phy_reg_write(dev_id, phy_addr, reg, data);
    return 0;
}

void
athrs17_phy_read(uint32_t dev_id, uint32_t phy_addr, uint32_t reg, uint16_t *data)
{
    *data = phy_reg_read(dev_id, phy_addr, reg);
    return 0;
}


unsigned int s17_rd_phy(int ethUnit,unsigned int phy_addr, unsigned int reg_addr);
void s17_wr_phy(int ethUnit,unsigned int phy_addr, unsigned int reg_addr, unsigned int write_data);
static void athrs17_reg_rmw(unsigned int reg_addr, unsigned int reg_val);

void athrs17_reg_write(unsigned int reg_addr, unsigned int reg_val);
unsigned int athrs17_reg_read(unsigned int reg_addr);


int athrs17_reg_init(void)
{
    int phy_addr;

    /* if using header for register configuration, we have to     */
    /* configure s17 register after frame transmission is enabled */


    /* configure the RGMII */
    athrs17_reg_write(0x620 , 0x4f0);
    athrs17_reg_write(0x624 , 0x007f7f7f);
    athrs17_reg_write(0x4   , 0x07600000);
    athrs17_reg_write(0xc   , 0x07600000);
    athrs17_reg_write(0x7c  , 0x000000c2);

    athrs17_reg_write(0x8   , 0x01000000);
    athrs17_reg_write(0x94   ,0x7e);
    /* AR8327/AR8328 fixup */
    for (phy_addr = 0x0; phy_addr <= ATHR_PHY_MAX; phy_addr++)
    {
        /* For 100M waveform */
        phy_reg_write(0, phy_addr, 0x1d, 0x0);
        phy_reg_write(0, phy_addr, 0x1e, 0x02ea);
        /* Turn On Gigabit Clock */
        phy_reg_write(0, phy_addr, 0x1d, 0x3d);
        phy_reg_write(0, phy_addr, 0x1e, 0x68a0);
    }

    /* */

    printk ("%s:done\n",__func__);

    return 0;
}

unsigned int
athrs17_reg_read(unsigned int reg_addr)
{
    uint32_t reg_word_addr;
    uint32_t phy_addr, tmp_val, reg_val;
    uint16_t phy_val;
    uint8_t phy_reg;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    reg_val = (uint32_t) phy_reg_read(0, phy_addr, phy_reg);

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    tmp_val = (uint32_t) phy_reg_read(0, phy_addr, phy_reg);
    reg_val |= (tmp_val << 16);

    return reg_val;
}

void
athrs17_reg_write(unsigned int reg_addr, unsigned int reg_val)
{
    uint32_t reg_word_addr;
    uint32_t phy_addr;
    uint16_t phy_val;
    uint8_t phy_reg;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* For S17 registers such as ARL and VLAN, since they include BUSY bit */
    /* in higher address, we should write the lower 16-bit register then the */
    /* higher one */

    /* write register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) (reg_val & 0xffff);
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) ((reg_val >> 16) & 0xffff);
    phy_reg_write(0, phy_addr, phy_reg, phy_val);
}

static void
athrs17_reg_rmw(unsigned int reg_addr, unsigned int reg_val)
{
    reg_val |= athrs17_reg_read(reg_addr);
    athrs17_reg_write(reg_addr,reg_val);
}

unsigned int s17_rd_phy(int ethUnit,unsigned int phy_addr, unsigned int reg_addr)
{
    return (phy_reg_read(ethUnit,phy_addr,reg_addr));

}

void s17_wr_phy(int ethUnit,unsigned int phy_addr, unsigned int reg_addr, unsigned int write_data)
{
    phy_reg_write(ethUnit,phy_addr,reg_addr,write_data);

}
