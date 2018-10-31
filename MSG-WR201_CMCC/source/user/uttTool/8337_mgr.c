#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include "uttRtChipApi.h"
#include <sys/types.h>
#include <linux/mii.h>
#include <linux/types.h>
#include <linux/autoconf.h>

#define RTL8367M_DEVICE "/dev/utt_rtl8367m"

void show_usage(void)
{
	printf("8337_mgr -g -r [register number]\n");
	printf("  Get: 8337_mgr -g -r 4\n\n");
	printf("8337_mgr -s -r [register number] -v [0xvalue]\n");
	printf("  Set: 8337_mgr -s -r 4 -v 0xff11\n\n");

	printf("8337_mgr -g -p phy_id -r [register number]\n");
	printf("  Get: 8337_mgr -g -p 0 -r 2\n\n");
	printf("8337_mgr -s -p phy_id -r [register number] -v [0xvalue]\n");
	printf("  Set: 8337_mgr -s -p 0 -r 0 -v 0x3100\n\n");
}

int main(int argc, char *argv[])
{
	int fd, opt, ret;
	char options[] = "gsp:r:v:?t";
	int method;
	rtl8367m_ioctl_data data;
	memset(&data,0 , sizeof(data));

	if (argc < 4) {
		show_usage();
		return 0;
	}

    fd = open(RTL8367M_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(0);
    }


	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'g':
				method = AR8337N_REG_READ;
				break;
			case 's':
				method = AR8337N_REG_WRITE;
				break;
			case 'p':
                switch(method){
                    case AR8337N_REG_READ:
                        method = AR8337N_MII_READ;
                        break;
                    case AR8337N_REG_WRITE:
                        method = AR8337N_MII_WRITE;
                        break;
                    default:
                        show_usage();
                        return 0;
                        break;
                }
                data.phy_id = (__u16)strtoul(optarg, NULL, 16);
				break;
			case 'r':
				data.reg_num = (__u16)strtoul(optarg, NULL, 16);
				break;
			case 'v':
				data.val_in = (__u32)strtoul(optarg, NULL, 16);
                printf("data.val_in: %x\n",data.val_in);
				break;
			case '?':
				show_usage();
				break;
		}
	}

	ret = ioctl(fd, method, &data);
	if (ret < 0) {
		printf("8337_mgr: ioctl error\n");
	}
	else
	switch (method) {
		case AR8337N_MII_READ:
			printf("Get: phy_id[0x%x] = %04x\n",
					 data.phy_id, data.val_out);
            break;
		case AR8337N_MII_WRITE:
			printf("Set: phy_id[0x%x] = %04x\n",
					 data.phy_id, data.val_in);
            break;
		case AR8337N_REG_READ:
			printf("Get: reg[0x%x] = %04x\n",
					 data.reg_num, data.val_out);
			break;
		case AR8337N_REG_WRITE:
			printf("Set: reg[0x%x] = %04x\n",
					 data.reg_num, data.val_in);
			break;
	}

	close(fd);
	return ret;
}
