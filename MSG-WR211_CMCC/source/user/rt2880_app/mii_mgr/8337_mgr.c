#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/mii.h>
#include <linux/types.h>

#include <linux/autoconf.h>
#include "ra_ioctl.h"

void show_usage(void)
{
#ifndef CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION
	printf("8337_mgr -g -r [register number]\n");
	printf("  Get: mii_mgr -g -r 4\n\n");
	printf("8337_mgr -s -r [register number] -v [0xvalue]\n");
	printf("  Set: mii_mgr -s -r 1 -v 0xff11\n\n");
#endif
}

int main(int argc, char *argv[])
{
	int sk, opt, ret;
	char options[] = "gsp:r:v:?t";
	int method;
	struct ifreq ifr;
	ra_mii_ioctl_data mii;

	if (argc < 4) {
		show_usage();
		return 0;
	}

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
		return -1;
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &mii;

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'g':
				method = RAETH_8337_READ;
				break;
			case 's':
				method = RAETH_8337_WRITE;
				break;
			case 'p':
				mii.phy_id = (__u16)atoi(optarg);
				break;
			case 'r':
				mii.reg_num = (__u16)strtoul(optarg, NULL, 16);
				break;
			case 'v':
				mii.val_in = (__u32)strtoul(optarg, NULL, 16);
                printf("mii.val_in: %x\n",mii.val_in);
				break;
			case '?':
				show_usage();
				break;
		}
	}

	ret = ioctl(sk, method, &ifr);
	if (ret < 0) {
		printf("mii_mgr: ioctl error\n");
	}
	else
	switch (method) {
		case RAETH_8337_READ:
			printf("Get: reg[0x%x] = %04x\n",
					 mii.reg_num, mii.val_out);
			break;
		case RAETH_8337_WRITE:
			printf("Set: reg[0x%x] = %04x\n",
					 mii.reg_num, mii.val_in);
			break;
	}

	close(sk);
	return ret;
}
