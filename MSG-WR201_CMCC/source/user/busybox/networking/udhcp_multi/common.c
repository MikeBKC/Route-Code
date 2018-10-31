/* vi: set sw=4 ts=4: */
/* common.c
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "common.h"
#include "libbb.h"

const uint8_t MAC_BCAST_ADDR[6] ALIGN2 = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

time_t utt_getSysUpTime()
{
	struct sysinfo info;
	sysinfo(&info);
    return (time_t)(info.uptime);
}

int utt_getMac(char *ifname, unsigned char *if_hw)
{
    struct ifreq ifr;
    int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("getIfMac: open socket error");
        return -1;
    }

    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
         close(skfd);
         printf("getIfMac: ioctl SIOCGIFHWADDR error for %s", ifname);
         return -1;
     }
     memcpy(if_hw, ifr.ifr_hwaddr.sa_data, MAC_LEN);
     close(skfd);
     return 0;
 }

#ifdef DHCP_SERVER_MULTIIF
uint32_t dhcp_server_hash(uint32_t vid)
{
	uint32_t key = 0;

	key = vid % DHCP_SERVER_HASH;

	return key;
}
#endif
