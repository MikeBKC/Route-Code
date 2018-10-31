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

#define RTL8367M_DEVICE "/dev/utt_rtl8367m"

static void mii_8367m_usage(char *cmd)
{
	printf("Usage:\n");
#if 0
	printf(" %s reg_num w [reg_numnum] [value]           - reg_numister write value to offset\n", cmd);
	printf(" %s reg_num r [reg_numnum]                   - reg_numister read value from offset\n", cmd);
	printf(" %s wanport				     - set wanport is 2 .test\n", cmd);
#endif
	printf(" %s phy w [phyid]  [reg_numnum] [value]      - phy reg_numister write value to offset\n", cmd);
	printf(" %s phy r [phyid]  [reg_numnum]              - phy reg_numister read value from offset\n", cmd);
}


int main(int argc, char *argv[])
{
	int i=0;
	int val=0;
	int fd;
	rtl8367m_ioctl_data data;
	memset(&data,0 , sizeof(data));
#if 1	
	printf("%s-%d:argc=%d\n",__FUNCTION__,__LINE__,argc);
	printf("-------------------------------------\n");
	for (i = 0; i < argc; i++) {
		printf("%s-%d:argv[%d]=%s\n",__FUNCTION__,__LINE__,i,argv[i]);
	}
	printf("-------------------------------------\n");
#endif
#ifdef UTT_P1010_DEBUG
    if (argc < 3)
    {
        mii_8367m_usage(argv[0]);
    }
    else if (!strncmp(argv[1], "mac", 4) &&
            !strncmp(argv[2], "dump", 5) )
    {
        fd = open(RTL8367M_DEVICE, O_RDWR);
        if (fd < 0) {
            perror("open");
            exit(0);
        }

        if (ioctl(fd,UTT_MAC_DUMP_DEBUG, &data)==-1) {
            perror("ioctl");
            close(fd);
            exit(0);
        }
    }
    else
#endif

#if 0
	if (argc < 2) {
		mii_8367m_usage(argv[0]);
		exit(0);
	}
	if (strncmp(argv[1], "wanport", 7) == 0) {
		fd = open(RTL8367M_DEVICE, O_RDWR);
		if (fd < 0) {
			perror("open");
			exit(0);
		}
		printf("%s-%d, fd = %d\n", __func__, __LINE__, fd);

		UttRtChipFuncSt conf;
		memset(&conf, 0 ,sizeof(conf));
		conf.confType = UTT_WANPORT_SET;
		/*这里因注意所有的口自左到右 从0开始*/
		conf.confData.wanPortsMask =  0x18;

		printf("%s-%d, wanPortsMask = 0x%x\n", __func__, __LINE__, conf.confData.wanPortsMask);
		if (ioctl(fd, UTT_ROUTER_CHIP_FUNC, &conf) == -1) {
			perror("ioctl");
			close(fd);
		}
		close(fd);
		exit(0);
	}
#endif
#if 1
	if (argc < 4) {
		mii_8367m_usage(argv[0]);
		exit(0);
	}
#if 0
	else if (!strncmp(argv[1], "reg_num", 4)) {
		if (argv[2][0] == 'r') {
#if 1
			fd = open(RTL8367M_DEVICE, O_RDWR);
			if (fd < 0) {
				perror("open");
				exit(0);
			}

			data.reg_num = strtoul(argv[3], NULL, 16);
			if(ioctl(fd, RTL8367M_MII_READ, &data)==-1)
			{
				perror("ioctl");
				close(fd);
				exit(0);
			}
			printf("%s-%d:reg_num read data.reg_num=0x%x, data.val_out=0x%x\n",__FUNCTION__,__LINE__,data.reg_num,data.val_out);
#endif		   
		}
		else if (argv[2][0] == 'w') {
			if (argc != 5) {
				mii_8367m_usage(argv[0]);
				exit(0);
			}
#if 1
			fd = open(RTL8367M_DEVICE, O_WRONLY);
			if (fd < 0) {
				perror("open");
				exit(0);
			}
#endif
			data.reg_num = strtoul(argv[3], NULL, 16);
			data.val_in = strtoul(argv[4], NULL, 16);
			printf("%s-%d: reg_num write data.reg_num=0x%x, data.val_in=0x%x\n",__FUNCTION__,__LINE__,data.reg_num,data.val_in);
			if (ioctl(fd, RTL8367M_MII_WRITE, &data)==-1) {
				perror("ioctl");
				close(fd);
				exit(0);
			}
		} else {
			mii_8367m_usage(argv[0]);
			exit(0);
		}
	}
#endif
	else if (!strncmp(argv[1], "phy", 4)) {
		if (argv[2][0] == 'r') {
#if 1
			if (argc < 5) {
				mii_8367m_usage(argv[0]);
				exit(0);
			}
			fd = open(RTL8367M_DEVICE, O_RDWR);
			if (fd < 0) {
				perror("open");
				exit(0);
			}

			data.phy_id = strtoul(argv[3], NULL, 16);
			data.reg_num = strtoul(argv[4], NULL, 16);
			if (ioctl(fd, RTL8367M_MII_READ, &data)==-1) {
				perror("ioctl");
				close(fd);
				exit(0);
			}
			printf("%s-%d:phy read data.phy_id=0x%x data.reg_num=0x%x, data.val_out=0x%x\n",__FUNCTION__,__LINE__,data.phy_id,data.reg_num,data.val_out);
#endif		   
		} else if (argv[2][0] == 'w') {
#if 1
			if (argc != 6)
			{
				mii_8367m_usage(argv[0]);
				exit(0);
			}
			fd = open(RTL8367M_DEVICE, O_WRONLY);
			if (fd < 0) {
				perror("open");
				exit(0);
			}
#endif

			data.phy_id = strtoul(argv[3], NULL, 16);
			data.reg_num = strtoul(argv[4], NULL, 16);
			data.val_in = strtoul(argv[5], NULL, 16);
			printf("%s-%d: phy write data.phy_id=0x%x data.reg_num=0x%x, data.val_in=0x%x\n",__FUNCTION__,__LINE__,data.phy_id, data.reg_num,data.val_in);
			if (ioctl(fd, RTL8367M_MII_WRITE, &data) == -1) {
				perror("ioctl");
				close(fd);
				exit(0);
			}
		} else {
			mii_8367m_usage(argv[0]);
			exit(0);
		}
	} else {
		mii_8367m_usage(argv[0]);
	}
	if (fd >= 0)
		close(fd);
	return 0;
#endif
}
