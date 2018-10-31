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
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	printf("mii_mgr -g -p [phy number] -r [register number] -a [page]\n");
	printf("  Get: mii_mgr -g -p 3 -r 4 -a 0\n\n");
	printf("mii_mgr -s -p [phy number] -r [register number] -a [page] -v [0xvalue]\n");
	printf("  Set: mii_mgr -s -p 4 -r 1 -a 0 -v 0xff11\n\n");
#else
	printf("mii_mgr -g -p [phy number] -r [register number]\n");
	printf("  Get: mii_mgr -g -p 3 -r 4\n\n");
	printf("mii_mgr -s -p [phy number] -r [register number] -v [0xvalue]\n");
	printf("  Set: mii_mgr -s -p 4 -r 1 -v 0xff11\n\n");
#endif
#endif
}

int main(int argc, char *argv[])
{
	int sk, opt, ret;
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	char options[] = "gsp:r:a:v:?t";
#else
	char options[] = "gsp:r:v:?t";
#endif
	int method;
	struct ifreq ifr;
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	ra_8306_mii_ioctl_data mii;
#else
	ra_mii_ioctl_data mii;
#endif

#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
	if (argc < 7) {
#else
	if (argc < 6) {
#endif
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
				method = RAETH_MII_READ;
				break;
			case 's':
				method = RAETH_MII_WRITE;
				break;
			case 'p':
				mii.phy_id = (__u16)atoi(optarg);
				break;
			case 'r':
				mii.reg_num = (__u16)atoi(optarg);
				break;
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
			case 'a':
				mii.page = (__u16)atoi(optarg);
				break;
#endif
			case 'v':
				mii.val_in = (__u16)strtol(optarg, NULL, 16);
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
		case RAETH_MII_READ:
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
			printf("Get: phy[%d].reg[%d].page[%d] = %04x\n",
					mii.phy_id, mii.reg_num, mii.page, mii.val_out);
#else
			printf("Get: phy[%d].reg[%d] = %04x\n",
					mii.phy_id, mii.reg_num, mii.val_out);
#endif
			break;
		case RAETH_MII_WRITE:
#if defined(CONFIG_UTT_NV840E) || defined(CONFIG_UTT_NV800) || defined(CONFIG_UTT_NV840D)
			printf("Set: phy[%d].reg[%d].page[%d] = %04x\n",
					mii.phy_id, mii.reg_num, mii.page, mii.val_in);
#else
			printf("Set: phy[%d].reg[%d] = %04x\n",
					mii.phy_id, mii.reg_num, mii.val_in);
#endif
			break;
	}

	close(sk);
	return ret;
}
