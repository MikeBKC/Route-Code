
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"

#if (SERVICE_ZONE == FYES)
#include <urcp_ipc.h>

#if (AC_WORK_MODE == FYES)
extern void add_route_and_dns(AcWorkModeProfile *prof);
#endif
static void ServiceZoneInit();
static void ServiceZoneAdd(ServiceZoneProfile *);
static void ServiceZoneEdit(ServiceZoneProfile *, ServiceZoneProfile *);
static void ServiceZoneDel(ServiceZoneProfile *);
static void initServiceZoneLan(void);

/* end 值一定要大于等于start，由调用函数保证*/
static int set_vid_bits(unsigned int vid_bits[128], unsigned int start, unsigned int end)
{
	int i, j, k, m, n;

	i = start >> 5;
	j = end >> 5;

	if (i == j) {
		m = start & 0x1fu;
		n = end & 0x1fu;

		m = (1u << m) - 1u;
		n = (1u << (n + 1)) - 1u;

		n &= ~m;
		vid_bits[i] |= n;
	} else {
		m = start & 0x1fu;
		n = end & 0x1fu;

		m = (1u << m) - 1u;
		n = (1u << (n + 1)) - 1u;

		vid_bits[i] |= ~m;
		vid_bits[j] |= n;

		for (k = i + 1; k < j; k++) {
			vid_bits[k] = 0xffffffffu;
		}
	}

	return 0;
}

static int numInStr(char a, char *str)
{
	int i, n = 0, num = 0;

	n = strlen(str);

	for (i = 0; i < n; i++) {
		if (a == str[i]) {
			num++;
		}
	}

	return num;
}

/* 解析页面传下来的VLANID字符串，返回0代表正常，返回-1代表字符串错误 */
static int parseVidStr(unsigned int vid_bits[128] , char str[128])
{
	int ret = 0;
	char *tmp = NULL, *ptr = NULL;
	char a;
	int n = 0;
	unsigned int vid, startVid, endVid;

	tmp = strtok_r(str, ",", &ptr);
	while (tmp != NULL) {
		n = numInStr('-', tmp);

		printf("n = %d\n", n);
		if (n == 0) {
			vid = strtol(tmp, NULL, 10);
			printf("vid = %u\n", vid);
			set_vid_bits(vid_bits, vid, vid);

		} else if (n == 1) {
			n = sscanf(tmp, "%u-%u", &startVid, &endVid);
			printf("n = %d\n", n);
			if (n == 2) {
				printf("startVid = %u, endVid = %u\n", startVid, endVid);
				if ((startVid > 4094) || (endVid > 4094) || (startVid > endVid)) {
					printf("%d in %s, error: %s\n", __LINE__, __func__, tmp);
					ret = -1;
				} else {
					set_vid_bits(vid_bits, startVid, endVid);
				}
			} else {
				printf("%d in %s, error: %s\n", __LINE__, __func__, tmp);
				ret = -1;
			}

		} else {
			printf("%d in %s, error: %s\n", __LINE__, __func__, tmp);
			ret = -1;
		}

		tmp = strtok_r(NULL, ",", &ptr);
	}


	return ret;
}
#if 0
/********************************************************************
 * 函数： initServiceZoneLan
 * 功能：
 * 创建： 2012-09-04
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
void initServiceZoneLan(void)
{
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
    ServiceZoneProfile *prof = NULL;

    unsigned int iIp0 = 0U, iIp1 = 0U, iIp2 = 0U, iIp3 = 0U, iIp4 = 0U,
					iIp5 = 0U, iIp6 = 0U, iIp7 = 0U, iIp8 = 0U, iIp9 = 0U;
    unsigned int iIpStart0 = 0U, iIpStart1 = 0U, iIpStart2 = 0U, iIpStart3 = 0U, iIpStart4 = 0U,
					iIpStart5 = 0U, iIpStart6 = 0U, iIpStart7 = 0U, iIpStart8 = 0U, iIpStart9 = 0U;
    char ip0[16], ip1[16], ip2[16], ip3[16], ip4[16],
			ip5[16], ip6[16], ip7[16], ip8[16], ip9[16];

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 0);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp0 = htonl(prof->gw);
		iIpStart0 = prof->gw & prof->netmask;
		if(iIp0 != 0U)
		{/*接口启用了*/
		    int2ip(iIp0, ip0);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 1);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp1 = htonl(prof->gw);
		iIpStart1 = prof->gw & prof->netmask;
		if(iIp1 != 0U)
		{/*接口启用了*/
		    int2ip(iIp1, ip1);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip1, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart1, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 2);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp2 = htonl(prof->gw);
		iIpStart2 = prof->gw & prof->netmask;
		if(iIp2 != 0U)
		{/*接口启用了*/
		    int2ip(iIp2, ip2);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip2, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart2, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 3);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp3 = htonl(prof->gw);
		iIpStart3 = prof->gw & prof->netmask;
		if(iIp3 != 0U)
		{/*接口启用了*/
		    int2ip(iIp3, ip3);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip3, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart3, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 4);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp4 = htonl(prof->gw);
		iIpStart4 = prof->gw & prof->netmask;
		if(iIp4 != 0U)
		{/*接口启用了*/
		    int2ip(iIp4, ip4);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip4, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart4, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 5);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp5 = htonl(prof->gw);
		iIpStart5 = prof->gw & prof->netmask;
		if(iIp5 != 0U)
		{/*接口启用了*/
		    int2ip(iIp5, ip5);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip5, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart5, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 6);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp6 = htonl(prof->gw);
		iIpStart6 = prof->gw & prof->netmask;
		if(iIp6 != 0U)
		{/*接口启用了*/
		    int2ip(iIp6, ip6);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip6, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart6, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 7);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp7 = htonl(prof->gw);
		iIpStart7 = prof->gw & prof->netmask;
		if(iIp7 != 0U)
		{/*接口启用了*/
		    int2ip(iIp7, ip7);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip7, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart7, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 8);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp8 = htonl(prof->gw);
		iIpStart8 = prof->gw & prof->netmask;
		if(iIp8 != 0U)
		{/*接口启用了*/
		    int2ip(iIp8, ip8);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip8, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart8, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}

    prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, 9);
    if ((prof != NULL) && (prof->head.active == 1) && (prof->serviceEn == PROF_ENABLE)) {
		iIp9 = htonl(prof->gw);
		iIpStart9 = prof->gw & prof->netmask;
		if(iIp9 != 0U)
		{/*接口启用了*/
		    int2ip(iIp9, ip9);
#if(IP_GRP == FYES)
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip9, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart9, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
#endif
		}
	}
    return;
}
#endif

#if SZONE_AUTO_UPDATA
void szone_auto_updata_edit(char *old_szone, char *new_szone)
{
    int i, j, k;
    uint32 tb_index;
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t *tmp = NULL; 
    SystemProfile *profSys = NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
#if 0
    ServiceZoneProfile *profSz = NULL;
#endif

    mem_shadow = get_shm_ptr();
    /*遍历hash*/
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);

    uttSemP(SEM_SZONE_UPDATA, 0);
    for(i=0; i < DEV_HASH_SIZE; i++)
    {
        tb_index = mem_shadow->dev_tb_hash[i];

        while(tb_index!=NONE_NODE)
        {
            tmp = &mem_shadow->dev_table[tb_index-1]; 
            for(j=0;j<UTT_SSID_NUM;j++){
                if( !strcmp(tmp->wl_info.szone_info.szone[j], old_szone)){
                    if( new_szone != NULL){
                        if( strcmp(new_szone, old_szone)){
                            memset(tmp->wl_info.szone_info.szone[j],0,sizeof(tmp->wl_info.szone_info.szone[j]));
                            strcpy(tmp->wl_info.szone_info.szone[j], new_szone);
                        }
                    }else{
                        memset(tmp->wl_info.szone_info.szone[j],0,sizeof(tmp->wl_info.szone_info.szone[j]));
                        for(k=j+1;k<UTT_SSID_NUM;k++){
                            if( tmp->wl_info.szone_info.szone[k][0] != '\0' ){
                                strcpy(tmp->wl_info.szone_info.szone[k-1], tmp->wl_info.szone_info.szone[k]);
                            }
                            memset(tmp->wl_info.szone_info.szone[k],0,sizeof(tmp->wl_info.szone_info.szone[k]));
                        }
                        tmp->wl_info.szone_info.szone_num -= 1;
#if 0
                        if(tmp->wl_info.szone_info.szone_num == 0){
                            profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(MIB_PROF_SERVICE_ZONE, 0);
                            tmp->wl_info.szone_info.szone_num = 1;
                            memcpy(tmp->wl_info.szone_info.szone[0],profSz->head.name,UTT_INST_NAME_LEN);
                        }
#endif
                    }
                    if(profSys->szoneAutoUpdata == FUN_ENABLE){
                        tmp->wl_info.szone_info.szone_changed = 1;
                    }else{
                        tmp->wl_info.szone_info.szone_changed = 0;
                    }
                    tmp->wl_info.szone_info.r_times = 0;
                    tmp->wl_info.szone_info.count = 0;
//                    break;
                }

		if( !strcmp(tmp->wl_info_5G.szone_info.szone[j], old_szone))
		{
		    if( new_szone != NULL)
		    {
			if( strcmp(new_szone, old_szone))
			{
			    memset(tmp->wl_info_5G.szone_info.szone[j],0,sizeof(tmp->wl_info_5G.szone_info.szone[j]));
			    strcpy(tmp->wl_info_5G.szone_info.szone[j], new_szone);
			}
		    }
		    else
		    {
			memset(tmp->wl_info_5G.szone_info.szone[j],0,sizeof(tmp->wl_info_5G.szone_info.szone[j]));
			for(k=j+1;k<UTT_SSID_NUM;k++)
			{
			    if( tmp->wl_info_5G.szone_info.szone[k][0] != '\0' )
			    {
				strcpy(tmp->wl_info_5G.szone_info.szone[k-1], tmp->wl_info_5G.szone_info.szone[k]);
			    }
			    memset(tmp->wl_info_5G.szone_info.szone[k],0,sizeof(tmp->wl_info_5G.szone_info.szone[k]));
			}
			tmp->wl_info_5G.szone_info.szone_num -= 1;
#if 0
			if(tmp->wl_info.szone_info.szone_num == 0){
			    profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(MIB_PROF_SERVICE_ZONE, 0);
			    tmp->wl_info.szone_info.szone_num = 1;
			    memcpy(tmp->wl_info.szone_info.szone[0],profSz->head.name,UTT_INST_NAME_LEN);
			}
#endif
		    }
		    if(profSys->szoneAutoUpdata == FUN_ENABLE)
		    {
			tmp->wl_info_5G.szone_info.szone_changed = 1;
		    }
		    else
		    {
			tmp->wl_info_5G.szone_info.szone_changed = 0;
		    }
		    tmp->wl_info_5G.szone_info.r_times = 0;
		    tmp->wl_info_5G.szone_info.count = 0;
		    break;
		}
            }

            tb_index = tmp->tb_next;
        }
    }
    uttSemV(SEM_SZONE_UPDATA, 0);

}
#endif

static void	ServiceZoneInit()
{
#if 0
	int min, max, index = 0;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;

	ProfInstLowHigh(mibType, &max, &min);

	for (index = (min + 1); index < max; index++) {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
		if (prof != NULL) {
			if (prof->vlanType == 1) {
				parseVidStr(prof->apVidBits, prof->strApVid);
			} else {
				if (prof->head.active == 1) {
					ServiceZoneAdd(prof);
				}
			}

		}
	}
			return;
#endif
#if (SZ_SHARE_LIMIT_AC == FYES)
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
    ServiceZoneProfile *prof = NULL;
    ProfInstLowHigh(mibType, &max, &min);

    for (index = min; index < max; index++) 
    {
	prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
	if (prof != NULL) 
	{
	    if ((prof->head.active == TRUE) && (prof->limit_type == 0u)) 
	    {
		prof->limit_type = 2u;
	    } 
	}
    }
#endif
}

static void ServiceZoneAdd(ServiceZoneProfile *prof)
{
#if 0
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
	unsigned int iIpStart0 = 0U;
	char ip0[16];

	memset(ip0, 0, sizeof(ip0));
#endif

    InterfaceProfile *profIf = NULL;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口*/
    char tmpName[16];

	if ((prof->serviceEn == PROF_ENABLE) && (prof->vlanType == 0)) {
        doSystem("vconfig add %s %d", getLanIfName(), prof->vid);
        doSystem("ifconfig %s.%d up", getLanIfName(), prof->vid);
        doSystem("ifconfig %s.%d %N netmask %N", getLanIfName(), prof->vid, prof->virif, prof->netmask);


        if((profIf != NULL) && (ProfInstIsFree(profIf) == 0)) /*配置已用*/
        {
            if (profIf->arpAllowOtherEn == FUN_DISABLE){
                sprintf(tmpName,"%s.%d",getLanIfName(),prof->vid);
                doSystem("arptables -A INPUT -j DROP -i %s", tmpName);
                doSystem("ip neigh flush dev %s 2>/dev/null",tmpName);/*清除arp，让他立即生效*/
            }
        }

#if (IP_GRP == FYES)
        iIp0 = htonl(prof->virif);
		iIpStart0 = (prof->virif & prof->netmask);
		if(iIp0 != 0U)
		{
			/*接口启用了*/
		    int2ip(iIp0, ip0);
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_ADD);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
		}
#endif
#if (AC_WORK_MODE == FYES)
    AcWorkModeProfile *profWkMode = NULL;
    profWkMode = (AcWorkModeProfile*)ProfGetInstPointByIndex(MIB_PROF_AC_WORK_MODE, 0);
	if(profWkMode->workMode == PROF_ENABLE)
	{
		add_route_and_dns(profWkMode);
	}
#endif

	}

	return;
#endif
}

static void ServiceZoneEdit(ServiceZoneProfile *oldProf, ServiceZoneProfile *newProf)
{
#if SZONE_AUTO_UPDATA
    if(newProf->serviceEn){
        szone_auto_updata_edit(oldProf->head.name,newProf->head.name);
    }else{
        szone_auto_updata_edit(oldProf->head.name,NULL);
    }
#endif
#if 0
		if ((PROF_ENABLE == oldProf->dhcpEn) && (PROF_ENABLE == newProf->dhcpEn)) {
			ServiceZoneDel(oldProf);
			ServiceZoneAdd(newProf);
		} else if ((PROF_DISABLE == oldProf->dhcpEn) && (PROF_DISABLE == newProf->dhcpEn)) {
			doSystem("vconfig rem %s.%d", getLanIfName(), oldProf->vid);
			doSystem("vconfig add %s %d", getLanIfName(), newProf->vid);
			doSystem("ifconfig %s.%d up", getLanIfName(), newProf->vid);
			doSystem("ifconfig %s.%d %N", getLanIfName(), prof->vid, prof->gw);
		} else if ((PROF_ENABLE == oldProf->dhcpEn) && (PROF_DISABLE == newProf->dhcpEn)) {
			ServiceZoneDel(oldProf);
			doSystem("vconfig add %s %d", getLanIfName(), newProf->vid);
			doSystem("ifconfig %s.%d up", getLanIfName(), newProf->vid);
			doSystem("ifconfig %s.%d %N", getLanIfName(), prof->vid, prof->gw);
		} else if ((PROF_DISABLE == oldProf->dhcpEn) && (PROF_ENABLE == newProf->dhcpEn)) {
			doSystem("vconfig rem %s.%d", getLanIfName(), oldProf->vid);
			ServiceZoneAdd(newProf);
		}
#endif

#if 0
		if ((oldProf->serviceEn == PROF_ENABLE) && (newProf->serviceEn == PROF_ENALBE)) {
			ServiceZoneDel(oldProf);
			ServiceZoneAdd(newProf);
		} else if ((oldProf->serviceEn == PROF_ENABLE) && (newProf->serviceEn == PROF_DISALBE)) {
		} else if ((oldProf->serviceEn == PROF_DISABLE) && (newProf->serviceEn == PROF_ENALBE)) {
		} else if ((oldProf->serviceEn == PROF_DISABLE) && (newProf->serviceEn == PROF_DISALBE)) {
		} else {
		}
#endif

#if 0
	if (0 != strcmp(oldProf->head.name, "default")) {
		if ((oldProf->vlanType == 0) && (newProf->vlanType == 1)) {
			ServiceZoneDel(oldProf);
		} else if ((oldProf->vlanType == 1) && (newProf->vlanType == 0)) {
			ServiceZoneAdd(newProf);
		} else if ((oldProf->vlanType == 0) && (newProf->vlanType == 0)) {
			ServiceZoneDel(oldProf);
			ServiceZoneAdd(newProf);
		} else if ((oldProf->vlanType == 1) && (newProf->vlanType == 1)) {
		} else {
		}

	}
#endif


	return;
}

static void ServiceZoneDel(ServiceZoneProfile *prof)
{
#if SZONE_AUTO_UPDATA
szone_auto_updata_edit(prof->head.name, NULL);
#endif
#if 0
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
	unsigned int iIpStart0 = 0U;
	char ip0[16];

	memset(ip0, 0, sizeof(ip0));
#endif

	if ((prof->serviceEn == PROF_ENABLE) && (prof->vlanType == 0)) {
#if (IP_GRP == FYES)
		iIp0 = htonl(prof->virif);
		iIpStart0 = (prof->virif & prof->netmask);
		if(iIp0 != 0U)
		{
			/*接口启用了*/
		    int2ip(iIp0, ip0);
		    ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_DEL);/*加入组*/
		    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_DEL);
		}
#endif
        doSystem("arptables -D INPUT -j DROP -i %s.%d 2>/dev/null",getLanIfName(),prof->vid);

		doSystem("vconfig rem %s.%d", getLanIfName(), prof->vid);
	}

	return;
#endif
}

void ServiceZoneChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const ServiceZoneProfile* oldProfileDataPtr, const ServiceZoneProfile* newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
			ServiceZoneAdd(newProfileDataPtr);
			break;
        case PROF_CHANGE_EDIT:
			ServiceZoneEdit(oldProfileDataPtr, newProfileDataPtr);
			break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
			ServiceZoneDel(oldProfileDataPtr);
			break;
        default:
            break;
    };
}

void funInitServiceZone()
{
	registerForProfileUpdates(MIB_PROF_SERVICE_ZONE, NULL, ServiceZoneChange);
	ServiceZoneInit();
}

#endif
