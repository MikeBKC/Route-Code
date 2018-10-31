#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include "linux/sockios.h"
#include "linux/mii.h"

#ifdef CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION
#define RT_SWITCH_HELP		0
#define RT_TABLE_MANIPULATE	0
#else
#define RT_SWITCH_HELP		1
#define RT_TABLE_MANIPULATE	1
#endif

#define UTT_MACTABLE_DUMP  0

int mii_fd;

void switch_init(void)
{
	mii_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (mii_fd < 0) {
		perror("socket");
		exit(0);
	}
}

void switch_fini(void)
{
	close(mii_fd);
}

void usage(char *cmd)
{
#if RT_SWITCH_HELP
	printf("Usage:\n");
#if UTT_MACTABLE_DUMP
	printf(" %s dump                                        - dump switch table\n", cmd);
#endif
#if 0
	printf(" %s dump                                        - dump switch table\n", cmd);
	printf(" %s add [mac] [portmap]                         - add an entry to switch table\n", cmd);
	printf(" %s add [mac] [portmap] [vlan idx]              - add an entry to switch table\n", cmd);
	printf(" %s add [mac] [portmap] [vlan idx] [age]        - add an entry to switch table\n", cmd);
	printf(" %s del [mac]                                   - delete an entry from switch table\n", cmd);
	printf(" %s del [mac] [vlan idx]                        - delete an entry from switch table\n", cmd);
	printf(" %s vlan dump                                   - dump switch table\n", cmd);
	printf(" %s vlan set [vlan idx] [vid] [portmap]         - set vlan id and associated member\n", cmd);
#endif
	printf(" %s reg r [phyid] [phypage] [regnum]            - register read from offset\n", cmd);
	printf(" %s reg w [phyid] [phypage] [regnum] [value]    - register write value to offset\n", cmd);
#endif
	switch_fini();
	exit(0);
}

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

int setOneWan()
{
	struct ifreq ifr;
	strncpy(ifr.ifr_name, "eth0", 5);
        if (-1 == ioctl(mii_fd, SIOCGMIIPHY, &ifr)) {
		perror("ioctl");
		close(mii_fd);
		exit(0);
	}
	return 0;
}

#if 0
#if RT_TABLE_MANIPULATE
void table_dump(void)
{
	int i, j, value, mac;

	reg_write(REG_ESW_TABLE_SEARCH, 0x1);
	printf("hash  port(0:6)  vid  age  pxy  in   mac-address\n");
	for (i = 0; i < 0x400; i++) {
		while(1) {
			reg_read(REG_ESW_TABLE_STATUS0, &value);
			if (value & 0x1) { //search_rdy
				if ((value & 0x70) == 0) {
					printf("found an unused entry (age = 3'b000), please check!\n");
					return;
				}
				printf("%03x:  ", (value >> 22) & 0x3ff); //hash_addr_lu
				j = (value >> 12) & 0x7f; //r_port_map
				printf("%c", (j & 0x01)? '1':'-');
				printf("%c", (j & 0x02)? '1':'-');
				printf("%c", (j & 0x04)? '1':'-');
				printf("%c", (j & 0x08)? '1':'-');
				printf("%c ", (j & 0x10)? '1':'-');
				printf("%c", (j & 0x20)? '1':'-');
				printf("%c", (j & 0x40)? '1':'-');
				printf("    %2d", (value >> 7) & 0xf); //r_vid
				printf("    %1d", (value >> 4) & 0x7); //r_age_field
				printf("    %c", (value & 0x8)? 'y':'n'); //r_proxy
				printf("   %c", (value & 0x4)? 'y':'n'); //r_mc_ingress

				reg_read(REG_ESW_TABLE_STATUS2, &mac);
				printf("  %08x", mac);
				reg_read(REG_ESW_TABLE_STATUS1, &mac);
				printf("%04x\n", (mac & 0xffff));
				if (value & 0x2) {
					printf("end of table %d\n", i);
					return;
				}
				break;
			}
			else if (value & 0x2) { //at_table_end
				printf("found the last entry %d (not ready)\n", i);
				return;
			}
			usleep(5000);
		}
		reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
	}
}

void table_add(int argc, char *argv[])
{
	int i, j, value;
	char tmpstr[9];

	if (!argv[2] || strlen(argv[2]) != 12) {
		printf("MAC address format error, should be of length 12\n");
		return;
	}
	strncpy(tmpstr, argv[2], 8);
	tmpstr[8] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);

	strncpy(tmpstr, argv[2]+8, 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	if (!argv[3] || strlen(argv[3]) != 7) {
		printf("portmap format error, should be of length 7\n");
		return;
	}
	j = 0;
	for (i = 0; i < 7; i++) {
		if (argv[3][i] != '0' && argv[3][i] != '1') {
			printf("portmap format error, should be of combination of 0 or 1\n");
			return;
		}
		j += (argv[3][i] - '0') * (1 << i);
	}
	value = j << 12; //w_port_map

	if (argc > 4) {
		j = strtoul(argv[4], NULL, 0);
		if (j < 0 || 15 < j) {
			printf("wrong member index range, should be within 0~15\n");
			return;
		}
		value += (j << 7); //w_index
	}

	if (argc > 5) {
		j = strtoul(argv[5], NULL, 0);
		if (j < 1 || 7 < j) {
			printf("wrong age range, should be within 1~7\n");
			return;
		}
		value += (j << 4); //w_age_field
	}
	else
		value += (7 << 4); //w_age_field

	value += 1; //w_mac_cmd
	reg_write(REG_ESW_WT_MAC_AD0, value);

	for (i = 0; i < 20; i++) {
		reg_read(REG_ESW_WT_MAC_AD0, &value);
		if (value & 0x2) { //w_mac_done
			printf("done.\n");
			return;
		}
		usleep(1000);
	}
	if (i == 20)
		printf("timeout.\n");
}

void table_del(int argc, char *argv[])
{
	int i, j, value;
	char tmpstr[9];

	if (!argv[2] || strlen(argv[2]) != 12) {
		printf("MAC address format error, should be of length 12\n");
		return;
	}
	strncpy(tmpstr, argv[2], 8);
	tmpstr[8] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);

	strncpy(tmpstr, argv[2]+8, 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	value = 0;
	if (argc > 3) {
		j = strtoul(argv[3], NULL, 0);
		if (j < 0 || 15 < j) {
			printf("wrong member index range, should be within 0~15\n");
			return;
		}
		value += (j << 7); //w_index
	}

	value += 1; //w_mac_cmd
	reg_write(REG_ESW_WT_MAC_AD0, value);

	for (i = 0; i < 20; i++) {
		reg_read(REG_ESW_WT_MAC_AD0, &value);
		if (value & 0x2) { //w_mac_done
			printf("done.\n");
			return;
		}
		usleep(1000);
	}
	if (i == 20)
		printf("timeout.\n");
}

void vlan_dump(void)
{
	int i, vid, value;

	printf("idx   vid  portmap\n");
	for (i = 0; i < 8; i++) {
		reg_read(REG_ESW_VLAN_ID_BASE + 4*i, &vid);
		reg_read(REG_ESW_VLAN_MEMB_BASE + 4*(i/2), &value);
		printf(" %2d  %4d  ", 2*i, vid & 0xfff);
		if (i%2 == 0) {
			printf("%c", (value & 0x00000001)? '1':'-');
			printf("%c", (value & 0x00000002)? '1':'-');
			printf("%c", (value & 0x00000004)? '1':'-');
			printf("%c", (value & 0x00000008)? '1':'-');
			printf("%c", (value & 0x00000010)? '1':'-');
			printf("%c", (value & 0x00000020)? '1':'-');
			printf("%c\n", (value & 0x00000040)? '1':'-');
		}
		else {
			printf("%c", (value & 0x00010000)? '1':'-');
			printf("%c", (value & 0x00020000)? '1':'-');
			printf("%c", (value & 0x00040000)? '1':'-');
			printf("%c", (value & 0x00080000)? '1':'-');
			printf("%c", (value & 0x00100000)? '1':'-');
			printf("%c", (value & 0x00200000)? '1':'-');
			printf("%c\n", (value & 0x00400000)? '1':'-');
		}
		printf(" %2d  %4d  ", 2*i+1, ((vid & 0xfff000) >> 12));
		if (i%2 == 0) {
			printf("%c", (value & 0x00000100)? '1':'-');
			printf("%c", (value & 0x00000200)? '1':'-');
			printf("%c", (value & 0x00000400)? '1':'-');
			printf("%c", (value & 0x00000800)? '1':'-');
			printf("%c", (value & 0x00001000)? '1':'-');
			printf("%c", (value & 0x00002000)? '1':'-');
			printf("%c\n", (value & 0x00004000)? '1':'-');
		}
		else {
			printf("%c", (value & 0x01000000)? '1':'-');
			printf("%c", (value & 0x02000000)? '1':'-');
			printf("%c", (value & 0x04000000)? '1':'-');
			printf("%c", (value & 0x08000000)? '1':'-');
			printf("%c", (value & 0x10000000)? '1':'-');
			printf("%c", (value & 0x20000000)? '1':'-');
			printf("%c\n", (value & 0x40000000)? '1':'-');
		}
	}
}

void vlan_set(int argc, char *argv[])
{
	int i, j, value;
	int idx, vid, pmap;

	if (argc != 6) {
		printf("insufficient arguments!\n");
		return;
	}
	idx = strtoul(argv[3], NULL, 0);
	if (idx < 0 || 15 < idx) {
		printf("wrong member index range, should be within 0~15\n");
		return;
	}
	vid = strtoul(argv[4], NULL, 0);
	if (vid < 0 || 0xfff < vid) {
		printf("wrong vlan id range, should be within 0~4095\n");
		return;
	}
	if (strlen(argv[5]) != 7) {
		printf("portmap format error, should be of length 7\n");
		return;
	}
	j = 0;
	for (i = 0; i < 7; i++) {
		if (argv[5][i] != '0' && argv[5][i] != '1') {
			printf("portmap format error, should be of combination of 0 or 1\n");
			return;
		}
		j += (argv[5][i] - '0') * (1 << i);
	}

	//set vlan identifier
	reg_read(REG_ESW_VLAN_ID_BASE + 4*(idx/2), &value);
	if (idx % 2 == 0) {
		value &= 0xfff000;
		value |= vid;
	}
	else {
		value &= 0xfff;
		value |= (vid << 12);
	}
	reg_write(REG_ESW_VLAN_ID_BASE + 4*(idx/2), value);

	//set vlan member
	reg_read(REG_ESW_VLAN_MEMB_BASE + 4*(idx/4), &value);
	if (idx % 4 == 0) {
		value &= 0xffffff00;
		value |= j;
	}
	else if (idx % 4 == 1) {
		value &= 0xffff00ff;
		value |= (j << 8);
	}
	else if (idx % 4 == 2) {
		value &= 0xff00ffff;
		value |= (j << 16);
	}
	else {
		value &= 0x00ffffff;
		value |= (j << 24);
	}
	reg_write(REG_ESW_VLAN_MEMB_BASE + 4*(idx/4), value);
}
#endif
#endif
#if UTT_MACTABLE_DUMP
/* mac表打印
 * add by bhou
 * 对于多播包 aged 和 sourcePort字段共同显示所有的源端口
 * 未解决的问题：mac地址显示的后两个bit不对(可能跟芯片mac hash算法有关系。)
 * */
static void table_dump(void)
{
	unsigned int i, j, value, mac1, mac2, mac3 , ctl;

	reg_read(0, 0, 16, &value);
	value = value | (1u <<13);
	reg_write(0,0 ,16, value);
	reg_read(4, 0, 16, &ctl);
	printf("mac            auth static aged sourcePort\n");
	for (i = 0; i <= 0x7ff; i++) {
	    ctl = ctl | 0x3 ;
	    ctl = ( ctl & (~(0x7ff << 4u)) ) | (i << 4u);
	    reg_write(4 ,0, 16, ctl);
		while(1) {
			usleep(1000);
			reg_read(4, 0, 16, &value);

			if ((value & 2u) == 0) { //search_rdy
				reg_read(4,0, 18, &mac1);
				reg_read(4,0, 19, &mac2);
				reg_read(4,0, 20, &mac3);
				if((mac1 | mac2 | mac3) != 0)
				{

				    printf("%02x", mac3 &0xff);
				    printf("%02x", mac3 >> 8);
				    printf("%02x", mac2 &0xff );
				    printf("%02x", mac2 >> 8);
				    printf("%02x", mac1 &0xff);
				    printf("%02x", mac1 >> 8);
				    reg_read(4,0, 17, &value);
				    printf("   %x", (value>>7u) &1u);
				    printf("   %x", (value>>6u) &1u);
				    printf("   %x", (value>>4u) &3u);
				    printf("   %x\n", value &0xf);
				}
				break;
			}
		}
	}
	reg_read(0, 0, 16, &value);
	value = value & (~(1u <<13));
	reg_write(0,0 ,16, value);
	return;

}
#endif
int main(int argc, char *argv[])
{
	int val=0;
        int id, reg, page;
	switch_init();

	if (argc < 2)
		usage(argv[0]);
#if UTT_MACTABLE_DUMP
	if (argc == 2) {
		if (!strncmp(argv[1], "dump", 5))
			table_dump();
		else
			usage(argv[0]);
	}
#endif
#if 0
	if (argc == 2) {
		if (!strncmp(argv[1], "dump", 5))
			table_dump();
		else
			usage(argv[0]);
	}
	else if (!strncmp(argv[1], "add", 4))
		table_add(argc, argv);
	else if (!strncmp(argv[1], "del", 4))
		table_del(argc, argv);
	else if (!strncmp(argv[1], "vlan", 5)) {
		if (argc < 3)
			usage(argv[0]);
		if (!strncmp(argv[2], "dump", 5))
			vlan_dump();
		else if (!strncmp(argv[2], "set", 4))
			vlan_set(argc, argv);
		else
			usage(argv[0]);
	}
#endif
	else if (!strncmp(argv[1], "reg", 4)) {
		if (argc < 6)
			usage(argv[0]);
		if (argv[2][0] == 'r') {
			id = strtoul(argv[3], NULL, 16);
			page = strtoul(argv[4], NULL, 16);
			reg = strtoul(argv[5], NULL, 10);
			reg_read(id, page, reg, &val);
			printf("switch reg read phy=0x%x, page=0x%x, reg=%d, value=0x%x\n", id, page, reg, val);
		}
		else if (argv[2][0] == 'w') {
			if (argc != 7)
				usage(argv[0]);
			id = strtoul(argv[3], NULL, 16);
			page = strtoul(argv[4], NULL, 16);
			reg = strtoul(argv[5], NULL, 10);
			val = strtoul(argv[6], NULL, 16);
			printf("switch reg read phy=0x%x, page=0x%x, reg=%d, value=0x%x\n", id, page, reg, val);
			reg_write(id, page, reg, val);
		}
		else
			usage(argv[0]);
	}
	else if(!strncmp(argv[1], "sow", 4)) {
			setOneWan();
        } else
		usage(argv[0]);

	switch_fini();
	return 0;
}

