#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include "linux/sockios.h"
#include "linux/mii.h"
#include <linux/ite8712.h>

#define GPIO_DEVICE "/dev/it8712-gpio"
#if 0
#ifdef CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION
#define RT_SWITCH_HELP		0
#define RT_TABLE_MANIPULATE	0
#else
#define RT_SWITCH_HELP		1
#define RT_TABLE_MANIPULATE	1
#endif
#endif
static void mii_6530G_close(int tmpfd)
{
	close(tmpfd);
}

static void mii_6530G_usage(char *cmd)
{
	printf("Usage:\n");
#if 0
	printf(" %s dump                                        - dump switch table\n", cmd);
	printf(" %s add [mac] [portmap]                         - add an entry to switch table\n", cmd);
	printf(" %s add [mac] [portmap] [vlan idx]              - add an entry to switch table\n", cmd);
	printf(" %s add [mac] [portmap] [vlan idx] [age]        - add an entry to switch table\n", cmd);
	printf(" %s del [mac]                                   - delete an entry from switch table\n", cmd);
	printf(" %s del [mac] [vlan idx]                        - delete an entry from switch table\n", cmd);
	printf(" %s vlan dump                                   - dump switch table\n", cmd);
	printf(" %s vlan set [vlan idx] [vid] [portmap]         - set vlan id and associated member\n", cmd);
	printf(" %s reg r [phyid] [phypage] [regnum]            - register read from offset\n", cmd);
#endif

	printf(" %s reg w [regnum] [value]           - register write value to offset\n", cmd);
	printf(" %s reg r [regnum]                   - register read value from offset\n", cmd);
	printf(" %s phy w [phyid]  [regnum] [value]  - phy register write value to offset\n", cmd);
	printf(" %s phy r [phyid]  [regnum]          - phy register read value from offset\n", cmd);
}

#if 0
int reg_read(int id, int page, int reg, int *value)
{
	struct ifreq ifr;
	struct mii_ioctl_data data;

	if (value == NULL)
		return -1;
	data.phy_id = (unsigned short)id;
	data.page = (unsigned short)page;
	data.reg_num = (unsigned short)reg;
	strncpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(mii_fd, SIOCGMIIREG, &ifr)) {
//	if (-1 == ioctl(mii_fd, SIOCGIFHWADDR, &ifr)) {
		perror("ioctl");
		close(mii_fd);
		exit(0);
	}
	*value = data.val_out;
	return 0;
}

int reg_write(int id, int page, int reg, int value)
{
	struct ifreq ifr;
	struct mii_ioctl_data data;

        data.phy_id = id;
        data.reg_num = reg;
        data.page = page;
        data.val_in = value;
	strncpy(ifr.ifr_name, "eth0", 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(mii_fd, SIOCSMIIREG, &ifr)) {
		perror("ioctl");
		close(mii_fd);
		exit(0);
	}
	return 0;
}
#endif

int main(int argc, char *argv[])
{
	int i=0;
	int val=0;
	int fd;
	struct st_8316_mii data;
	memset(&data,0 , sizeof(data));
#if 0	
	printf("%s-%d:argc=%d\n",__FUNCTION__,__LINE__,argc);
	printf("-------------------------------------\n");
	for(i=0;i<argc;i++)
	{
		printf("%s-%d:argv[%d]=%s\n",__FUNCTION__,__LINE__,i,argv[i]);
	}
	printf("-------------------------------------\n");
#endif
#if 1
	if (argc < 4)
	{
		mii_6530G_usage(argv[0]);
		exit(0);
	}
	else if (!strncmp(argv[1], "reg", 4)) {
		if (argv[2][0] == 'r') {
#if 1
			fd = open(GPIO_DEVICE, O_RDWR);
			if(fd < 0)
			{
				perror("open");
				exit(0);
			}

			data.reg = strtoul(argv[3], NULL, 16);
			if(ioctl(fd, IOC_GPIO_GET_8316_IN_REG, &data)==-1)
			{
				perror("ioctl");
				close(fd);
				exit(0);
			}
			printf("%s-%d:reg read data.reg=0x%x, data.data=0x%x\n",__FUNCTION__,__LINE__,data.reg,data.data);
#endif		   
		}
		else if (argv[2][0] == 'w') {
			if (argc != 5)
			{
				mii_6530G_usage(argv[0]);
				exit(0);
			}
#if 1
			fd = open(GPIO_DEVICE, O_WRONLY);
			if(fd < 0)
			{
				perror("open");
				exit(0);
			}
#endif
			data.reg = strtoul(argv[3], NULL, 16);
			data.data = strtoul(argv[4], NULL, 16);
			printf("%s-%d: reg write data.reg=0x%x, data.data=0x%x\n",__FUNCTION__,__LINE__,data.reg,data.data);
			//mii_6530G_reg_write();
			if(ioctl(fd, IOC_GPIO_SET_8316_IN_REG, &data)==-1)
			{
				perror("ioctl");
				close(fd);
				exit(0);
			}
		}
		else
		{
			mii_6530G_usage(argv[0]);
			exit(0);
		}
	}
	else if (!strncmp(argv[1], "phy", 4)) {
		if (argv[2][0] == 'r') {
#if 1
			if (argc < 5)
			{
				mii_6530G_usage(argv[0]);
				exit(0);
			}
			fd = open(GPIO_DEVICE, O_RDWR);
			if(fd < 0)
			{
				perror("open");
				exit(0);
			}

			data.addr = strtoul(argv[3], NULL, 16);
			data.reg = strtoul(argv[4], NULL, 16);
			if(ioctl(fd, IOC_GPIO_GET_8316_PHY_REG, &data)==-1)
			{
				perror("ioctl");
				close(fd);
				exit(0);
			}
			printf("%s-%d:phy read data.addr=0x%x data.reg=0x%x, data.data=0x%x\n",__FUNCTION__,__LINE__,data.addr,data.reg,data.data);
#endif		   
		}
		else if (argv[2][0] == 'w') {
#if 1
			if (argc != 6)
			{
				mii_6530G_usage(argv[0]);
				exit(0);
			}
			fd = open(GPIO_DEVICE, O_WRONLY);
			if(fd < 0)
			{
				perror("open");
				exit(0);
			}
#endif

			data.addr = strtoul(argv[3], NULL, 16);
			data.reg = strtoul(argv[4], NULL, 16);
			data.data = strtoul(argv[5], NULL, 16);
			printf("%s-%d: phy write data.addr=0x%x data.reg=0x%x, data.data=0x%x\n",__FUNCTION__,__LINE__,data.addr, data.reg,data.data);
			//mii_6530G_reg_write();
			if(ioctl(fd, IOC_GPIO_SET_8316_PHY_REG, &data)==-1)
			{
				perror("ioctl");
				close(fd);
				exit(0);
			}
		}
		else
		{
			mii_6530G_usage(argv[0]);
			exit(0);
		}
	}
	else
	{
		mii_6530G_usage(argv[0]);
	}
	if(fd>=0)
		close(fd);
	return 0;
#endif
}
