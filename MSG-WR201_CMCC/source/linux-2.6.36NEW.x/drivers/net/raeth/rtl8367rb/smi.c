/*
* Copyright c                  Realtek Semiconductor Corporation, 2006
* All rights reserved.
*
* Program : Control  smi connected RTL8366
* Abstract :
* Author : Yu-Mei Pan (ympan@realtek.com.cn)
*  $Id: smi.c,v 1.1 2015/02/28 12:42:36 yu.dehai Exp $
*/


#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <asm/uaccess.h>
#include <asm/rt2880/surfboardint.h>
#include <linux/delay.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
#include <asm/rt2880/rt_mmap.h>
#else
#include <linux/libata-compat.h>
#endif
#include "../ra2882ethreg.h"

#include "rtk_types.h"
#include "smi.h"
#include "rtk_error.h"

#define MDC_MDIO_DUMMY_ID           0
#define MDC_MDIO_CTRL0_REG          31
#define MDC_MDIO_START_REG          29
#define MDC_MDIO_CTRL1_REG          21
#define MDC_MDIO_ADDRESS_REG        23
#define MDC_MDIO_DATA_WRITE_REG     24
#define MDC_MDIO_DATA_READ_REG      25
#define MDC_MDIO_PREAMBLE_LEN       32

#define MDC_MDIO_START_OP          0xFFFF
#define MDC_MDIO_ADDR_OP           0x000E
#define MDC_MDIO_READ_OP           0x0001
#define MDC_MDIO_WRITE_OP          0x0003

#define SPI_READ_OP                 0x3
#define SPI_WRITE_OP                0x2
#define SPI_READ_OP_LEN             0x8
#define SPI_WRITE_OP_LEN            0x8
#define SPI_REG_LEN                 16
#define SPI_DATA_LEN                16

#define DELAY                        6u
#if 1
#define CLK_DURATION(clk)            { udelay(clk); }
#else
#define CLK_DURATION(clk)            { int i; for(i=0; i<clk; i++); }
#endif
#define _SMI_ACK_RESPONSE(ok)        { /*if (!(ok)) return RT_ERR_FAILED; */}



#define SMI_SCK    (1u<<23u)
#define SMI_SDA    (1u<<22u)
#define SMI_RST    (1u<<23u)  /* GPIO used for reset swtich */

#define ack_timer                    5
#define max_register                0x018A

#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
#define RT2880_GPIOMODE_REG	    ( RALINK_SYSCTL_BASE + 0x60u)
#define RT2880_REG(x)		(*((volatile u32 *)(x)))
#define RT2880_GPIODATA_REG		(RALINK_PIO_BASE + 0x20u)
#define RT2880_GPIODIR_REG		(RALINK_PIO_BASE + 0x24u)
#define RT2880_GPIOSET_REG		(RALINK_PIO_BASE + 0x2Cu)
#define RT2880_GPIORESET_REG		(RALINK_PIO_BASE + 0x30u)
#define RT2880_CLKCFG0_REG          (RALINK_SYSCTL_BASE + 0x2Cu)

#define RT2880_GPIO3924_DIR          (RALINK_PIO_BASE + 0x4Cu)
#define RT2880_GPIO3924_DATA          (RALINK_PIO_BASE + 0x48u)
#define RT2880_GPIO3924_SET          (RALINK_PIO_BASE + 0x54u)
#define RT2880_GPIO3924_RESET          (RALINK_PIO_BASE + 0x58u)

#define RT2880_GPIO7140_DIR          (RALINK_PIO_BASE + 0x74u)
#define RT2880_GPIO7140_DATA          (RALINK_PIO_BASE + 0x70u)
#define RT2880_GPIO7140_SET          (RALINK_PIO_BASE + 0x7Cu)
#define RT2880_GPIO7140_RESET          (RALINK_PIO_BASE + 0x80u)

void rtlglue_drvMutexLock(void)
{
    ;
}
void rtlglue_drvMutexUnlock(void)
{
    ;
}
void _rtl865x_getGpioDataBit(u32 signal, u32 *u)
{
    u32 val;

    val = RT2880_REG(RT2880_GPIODATA_REG);		
    if((val & SMI_SDA)>0u) {
	*u = 1u;
    }
    else {
	*u = 0u;
    }
}
void _rtl865x_setGpioDataBit(u32 sigal, u32 val)
{
    if(val>0)
    {
	RT2880_REG(RT2880_GPIOSET_REG) = sigal;
    }
    else
    {
	RT2880_REG(RT2880_GPIORESET_REG) = sigal;
    }
}
#endif
#if defined(MDC_MDIO_OPERATION) || defined(SPI_OPERATION)
    /* No local function in MDC/MDIO mode */
#else
void _smi_start(void)
{
#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
    u32 val;

    /*set gpio dir*/
    val = RT2880_REG(RT2880_GPIODIR_REG);
    val |= SMI_SCK | SMI_SDA;
    RT2880_REG(RT2880_GPIODIR_REG) = val;
#else
    /* change GPIO pin to Output only */
    _rtl865x_initGpioPin(SMI_SDA, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    _rtl865x_initGpioPin(SMI_SCK, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
#endif
    /* Initial state: SCK: 0, SDA: 1 */
    _rtl865x_setGpioDataBit(SMI_SCK, 0);
    _rtl865x_setGpioDataBit(SMI_SDA, 1);
    CLK_DURATION(DELAY);

    /* CLK 1: 0 -> 1, 1 -> 0 */
    _rtl865x_setGpioDataBit(SMI_SCK, 1);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 0);
    CLK_DURATION(DELAY);

    /* CLK 2: */
    _rtl865x_setGpioDataBit(SMI_SCK, 1);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SDA, 0);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 0);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SDA, 1);
}



void _smi_writeBit(uint16 signal, uint32 bitLen)
{
    for( ; bitLen > 0; bitLen--)
    {
        CLK_DURATION(DELAY);

        /* prepare data */
        if ( signal & (1<<(bitLen-1)) )
            _rtl865x_setGpioDataBit(SMI_SDA, 1);
        else
            _rtl865x_setGpioDataBit(SMI_SDA, 0);
        CLK_DURATION(DELAY);

        /* clocking */
        _rtl865x_setGpioDataBit(SMI_SCK, 1);
        CLK_DURATION(DELAY);
        _rtl865x_setGpioDataBit(SMI_SCK, 0);
    }
}



void _smi_readBit(uint32 bitLen, uint32 *rData)
{
    uint32 u;

    /* change GPIO pin to Input only */
#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
    u32 val;

    val = RT2880_REG(RT2880_GPIODIR_REG);
    val &= ~SMI_SDA;
    RT2880_REG(RT2880_GPIODIR_REG) = val;
#else
    _rtl865x_initGpioPin(SMI_SDA, GPIO_PERI_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);
#endif

    for (*rData = 0; bitLen > 0; bitLen--)
    {
        CLK_DURATION(DELAY);

        /* clocking */
        _rtl865x_setGpioDataBit(SMI_SCK, 1);
        CLK_DURATION(DELAY);
        _rtl865x_getGpioDataBit(SMI_SDA, &u);
        _rtl865x_setGpioDataBit(SMI_SCK, 0);

        *rData |= (u << (bitLen - 1));
    }

    /* change GPIO pin to Output only */
#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
    val = RT2880_REG(RT2880_GPIODIR_REG);
    val |= SMI_SDA;
    RT2880_REG(RT2880_GPIODIR_REG) = val;
#else
    _rtl865x_initGpioPin(SMI_SDA, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
#endif
}



void _smi_stop(void)
{

    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SDA, 0);
    _rtl865x_setGpioDataBit(SMI_SCK, 1);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SDA, 1);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 1);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 0);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 1);

    /* add a click */
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 0);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(SMI_SCK, 1);


    /* change GPIO pin to Output only */
#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
    u32 val;
    val = RT2880_REG(RT2880_GPIODIR_REG);
    val &= ~(SMI_SDA | SMI_SCK);
    RT2880_REG(RT2880_GPIODIR_REG) = val;
#else
    _rtl865x_initGpioPin(SMI_SDA, GPIO_PERI_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);
    _rtl865x_initGpioPin(SMI_SCK, GPIO_PERI_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);
#endif

}

#endif /* End of #if defined(MDC_MDIO_OPERATION) || defined(SPI_OPERATION) */

int32 smi_reset(
#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
	void
#else
	uint32 port, uint32 pinRST
#endif
	)
{
#if defined(MDC_MDIO_OPERATION) || defined(SPI_OPERATION)

#else

#ifdef RT3883_ASIC_BOARD
    int32  val;

    /*set gpio1 for reset */
    val = RT2880_REG(RT2880_CLKCFG0_REG);
    val &= ~(1u<<8u);
    RT2880_REG(RT2880_CLKCFG0_REG) = val;

    val = RT2880_REG(RT2880_GPIO3924_DIR);
    val |= SMI_RST;
    RT2880_REG(RT2880_GPIO3924_DIR) = val;

    /*reset sigal*/
    RT2880_REG(RT2880_GPIO3924_SET) = SMI_RST;

    udelay(10000);
    RT2880_REG(RT2880_GPIO3924_RESET) = SMI_RST;

    udelay(10000);
    RT2880_REG(RT2880_GPIO3924_SET) = SMI_RST;
    udelay(20000);

#elif defined(MTK7620_ASIC_BOARD)
    int32  val;

    /*set gpio1 for reset */
    val = RT2880_REG(RT2880_GPIOMODE_REG);
    val |= (1u<<10u);
    RT2880_REG(RT2880_GPIOMODE_REG) = val;

    val = RT2880_REG(RT2880_GPIO7140_DIR);
    val |= SMI_RST;
    RT2880_REG(RT2880_GPIO7140_DIR) = val;

    /*reset sigal*/
    RT2880_REG(RT2880_GPIO7140_SET) = SMI_RST;

    udelay(10000);
    RT2880_REG(RT2880_GPIO7140_RESET) = SMI_RST;

    udelay(10000);
    RT2880_REG(RT2880_GPIO7140_SET) = SMI_RST;
    udelay(20000);

#else
    gpioID gpioId;
    int32 res;

    /* Initialize GPIO port A, pin 7 as SMI RESET */
    gpioId = GPIO_ID(port, pinRST);
    res = _rtl865x_initGpioPin(gpioId, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (res != RT_ERR_OK)
        return res;
    smi_RST = gpioId;

    _rtl865x_initGpioPin(smi_RST, GPIO_PERI_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);
#endif
#endif
    return RT_ERR_OK;
}


int32 smi_init(
#if defined(RT3883_ASIC_BOARD) || defined(MTK7620_ASIC_BOARD)
	void
#else
	uint32 port, uint32 pinSCK, uint32 pinSDA
#endif
	)
{
#if defined(MDC_MDIO_OPERATION) || defined(SPI_OPERATION)

#else

#ifdef RT3883_ASIC_BOARD
    u32 val;

    /*set gpio mode*/
    val = RT2880_REG(RT2880_GPIOMODE_REG);
    val |= (1u<<7u);
    RT2880_REG(RT2880_GPIOMODE_REG) = val;

    /*set gpio dir*/
    val = RT2880_REG(RT2880_GPIODIR_REG);
    val |= SMI_SCK | SMI_SDA;
    RT2880_REG(RT2880_GPIODIR_REG) = val;

    //RT2880_REG(RT2880_GPIOSET_REG) = SMI_SCK;
    RT2880_REG(RT2880_GPIOSET_REG) = SMI_SDA;

#elif defined(MTK7620_ASIC_BOARD)
    u32 val;

    /*set gpio mode*/
    val = RT2880_REG(RT2880_GPIOMODE_REG);
    val &= ~(3u<<7u);
    val |= (1u<<8u);
    RT2880_REG(RT2880_GPIOMODE_REG) = val;

    /*set gpio dir*/
    val = RT2880_REG(RT2880_GPIODIR_REG);
    val |= SMI_SCK | SMI_SDA;
    RT2880_REG(RT2880_GPIODIR_REG) = val;

    //RT2880_REG(RT2880_GPIOSET_REG) = SMI_SCK;
    RT2880_REG(RT2880_GPIOSET_REG) = SMI_SDA;
#else
    gpioID gpioId;
    int32 res;

    /* change GPIO pin to Input only */
    /* Initialize GPIO port C, pin 0 as SMI SDA0 */
    gpioId = GPIO_ID(port, pinSDA);
    res = _rtl865x_initGpioPin(gpioId, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (res != RT_ERR_OK)
        return res;
    SMI_SDA = gpioId;


    /* Initialize GPIO port C, pin 1 as SMI SCK0 */
    gpioId = GPIO_ID(port, pinSCK);
    res = _rtl865x_initGpioPin(gpioId, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (res != RT_ERR_OK)
        return res;
    SMI_SCK = gpioId;


    _rtl865x_setGpioDataBit(SMI_SDA, 1);
    _rtl865x_setGpioDataBit(SMI_SCK, 1);
#endif
#endif

    return RT_ERR_OK;
}




int32 smi_read(uint32 mAddrs, uint32 *rData)
{
#if defined(MDC_MDIO_OPERATION)

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write read control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Read data from register 25 */
    MDC_MDIO_READ(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_READ_REG, rData);

    return RT_ERR_OK;

#elif defined(SPI_OPERATION)
    /* Write 8 bits READ OP_CODE */
    SPI_WRITE(SPI_READ_OP, SPI_READ_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Read 16 bits data */
    SPI_READ(rData, SPI_DATA_LEN);

#else
    uint32 rawData=0, ACK;
    uint8  con;
    uint32 ret = RT_ERR_OK;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370 */

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x1, 1);                        /* 1: issue READ command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing READ command*/
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK by RTL8369 */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_readBit(8, &rawData);                    /* Read DATA [7:0] */
    *rData = rawData&0xff;

    _smi_writeBit(0x00, 1);                        /* ACK by CPU */

    _smi_readBit(8, &rawData);                    /* Read DATA [15: 8] */

    _smi_writeBit(0x01, 1);                        /* ACK by CPU */
    *rData |= (rawData<<8);

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
#endif /* end of #if defined(MDC_MDIO_OPERATION) */
}



int32 smi_write(uint32 mAddrs, uint32 rData)
{
#if defined(MDC_MDIO_OPERATION)

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write data to register 24 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_WRITE_REG, rData);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write data control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

    return RT_ERR_OK;

#elif defined(SPI_OPERATION)
    /* Write 8 bits WRITE OP_CODE */
    SPI_WRITE(SPI_WRITE_OP, SPI_WRITE_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Write 16 bits data */
    SPI_WRITE(rData, SPI_DATA_LEN);
#else

/*
    if ((mAddrs > 0x018A) || (rData > 0xFFFF))  return    RT_ERR_FAILED;
*/
    int8 con;
    uint32 ACK;
    uint32 ret = RT_ERR_OK;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370*/

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x0, 1);                        /* 0: issue WRITE command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing WRITE command*/
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData&0xff, 8);                /* Write Data [7:0] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for writting data [7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData>>8, 8);                    /* Write Data [15:8] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                        /* ACK for writting data [15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
#endif /* end of #if defined(MDC_MDIO_OPERATION) */
}
    /*set gpio dir*/
