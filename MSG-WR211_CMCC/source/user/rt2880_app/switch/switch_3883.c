#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/mii.h>
#include <linux/sockios.h>
#include <linux/autoconf.h>
#include "ra_ioctl.h"
/** 
 * add by bhou
 */
static void usage(char *cmd)
{
    printf("Usage:\n");
#if 0
    printf(" %s reg w [regnum] [value]           - register write value to offset\n", cmd);
    printf(" %s reg r [regnum]                   - register read value from offset\n", cmd);
#endif
#if UTT_RT3883_DEBUG
    printf(" %s mac dump                         - display all entrys in switch chip looktable\n", cmd);
    printf(" %s mac dump must enable prink funtion in kernel\n", cmd);
#endif
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
    printf(" %s phy w [phyid] [regnum] [page] [value]  - phy register write value to offset\n", cmd);
    printf(" %s phy r [phyid] [regnum] [page]         - phy register read value from offset\n", cmd);
#else
    printf(" %s phy w [phyid]  [regnum] [value]  - phy register write value to offset\n", cmd);
    printf(" %s phy r [phyid]  [regnum]          - phy register read value from offset\n", cmd);
#endif
    exit(0);
}

#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
#define RAETH_WRITE RAETH_ESW_REG_WRITE
#define RAETH_READ  RAETH_ESW_REG_READ
#else
#define RAETH_WRITE RAETH_MII_WRITE
#define RAETH_READ  RAETH_MII_READ
#endif

int main(int argc, char *argv[])
{
    struct ifreq ifr;
    int sk = 0;
    int ret = 0;
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
    ra_8306_mii_ioctl_data mii;
#else
    ra_mii_ioctl_data mii;
#endif
#if UTT_RT3883_DEBUG
    if (argc < 3)
    {
	usage(argv[0]);
    }
    else if (!strncmp(argv[1], "mac", 4) &&
	    !strncmp(argv[2], "dump", 5) )
    {
	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
	    printf("Open socket failed\n");
	    exit(0);
	}
	strncpy(ifr.ifr_name, "eth2", 5);
	if(ioctl(sk, RAETH_SW_MAC_DEBUG, &ifr) < 0)
	{
	    perror("ioctl : Are you sure UTT_RT3883_DEBUG in kernel is enabled?");
	    close(sk);
	    exit(0);
	}
    }
#else
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
    if (argc < 5)
#else
    if (argc < 4)
#endif
    {
	usage(argv[0]);
    }
#endif
    else if (!strncmp(argv[1], "phy", 4)) 
    {
	if (argv[2][0] == 'r') 
	{
#if 1
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	    if (argc < 6)
#else
	    if (argc < 5)
#endif
	    {
		usage(argv[0]);
	    }
	    sk = socket(AF_INET, SOCK_DGRAM, 0);
	    if (sk < 0) {
		printf("Open socket failed\n");
		exit(0);
	    }
	    strncpy(ifr.ifr_name, "eth2", 5);
	    ifr.ifr_data = &mii;
	    mii.phy_id = strtoul(argv[3], NULL, 16);
	    mii.reg_num = strtoul(argv[4], NULL, 16);
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	    mii.page = strtoul(argv[5], NULL, 16);
#endif
	    if(ioctl(sk, RAETH_READ, &ifr) < 0)
	    {
		perror("ioctl");
		close(sk);
		exit(0);
	    }
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	    printf("%s read phyid=0x%x reg=0x%x page=0x%x, value=0x%x\n",argv[0],mii.phy_id,mii.reg_num,mii.page,mii.val_out);
#else
	    printf("%s read phyid=0x%x reg=0x%x, value=0x%x\n",argv[0],mii.phy_id,mii.reg_num,mii.val_out);
#endif		   
#endif
	}
	else if (argv[2][0] == 'w') 
	{
#if 1
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	    if (argc != 7)
#else
	    if (argc != 6)
#endif
	    {
		usage(argv[0]);
	    }
	    sk = socket(AF_INET, SOCK_DGRAM, 0);
	    if (sk < 0) {
		printf("Open socket failed\n");
		exit(0);
	    }
	    strncpy(ifr.ifr_name, "eth2", 5);
	    ifr.ifr_data = &mii;
	    mii.phy_id = strtoul(argv[3], NULL, 16);
	    mii.reg_num = strtoul(argv[4], NULL, 16);
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	    mii.page = strtoul(argv[5], NULL, 16);
	    mii.val_in = strtoul(argv[6], NULL, 16);
#else
	    mii.val_in = strtoul(argv[5], NULL, 16);
#endif
	    if(ioctl(sk, RAETH_WRITE, &ifr) < 0)
	    {
		perror("ioctl");
		close(sk);
		exit(0);
	    }
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	    printf("%s write phyid=0x%x reg=0x%x page=0x%x, value=0x%x\n",argv[0],mii.phy_id,mii.reg_num,mii.page,mii.val_in);
#else
	    printf("%s write phyid=0x%x reg=0x%x, value=0x%x\n",argv[0],mii.phy_id,mii.reg_num,mii.val_in);
#endif
#endif		   
	}
	else
	{
	    usage(argv[0]);
	}
    }
    else
    {
	usage(argv[0]);
    }
    if(sk>=0)
	close(sk);
    return 0;
}
