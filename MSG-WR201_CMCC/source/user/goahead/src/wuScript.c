/*
 * goaheadʹ�õĽ����������������ģ�鹲ͬʹ�õĺ���
 */
#include "uttMachine.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include "internet.h"
#include "webs.h"
#include "utils.h"
#include "firewall.h"
#include "management.h"
#include "station.h"
#include "uttfunction.h"
#include "linux/mii.h"
#include "linux/sockios.h"

/*
 * kernel config
 */
#include "linux/autoconf.h"
#ifndef CONFIG_RALINK_RT2880
#include <net/if.h>
#endif
/*
 * user config
 */
#include "config/autoconf.h"

#include "utt_pppoeClient.h"

#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "argcmd.h"
#include "wuScript.h"

/*
 * �ж�����ip��ַ���Ƿ��ص�
 * ���������
 *  ipS1,ipE1: ��һ��ip��ַ�ε���ʼip�ͽ���ip, �����ֽ���
 *  ipS2,ipE2: �ڶ���ip��ַ�ε���ʼip�ͽ���ip, �����ֽ���
 * ����ֵ:
 *  TRUE��     ����ip��ַ���ص�
 *  FALSE:     ����ip��ַ�β��ص�
 */
int ipSegmentOverlap(Uint32 ipS1, Uint32 ipE1, Uint32 ipS2, Uint32 ipE2) {

    if (ipS1 >= ipS2 && ipS1 <= ipE2) {
	return TRUE;
    }
    if (ipE1 >= ipS2 && ipE1 <= ipE2) {
	return TRUE;
    }
    if (ipS2 >= ipS1 && ipS2 <= ipE1) {
	return TRUE;
    }
    if (ipE2 >= ipS1 && ipE2 <= ipE1) {
	return TRUE;
    }
    return FALSE;
}

/*
 * �����ַ���Ƿ��ͻ
 * �������:
 *  poolType:	Ҫ���ĵ�ַ�����ͣ��������ipS��ipE������pool����dhcp,pppoe,l2tp or pptp
 *  ipS:	��ʼIp��ַ, �����ֽ���
 *  ipE:	����ip��ַ, �����ֽ���
 * ���������	��
 * ����ֵ��
 *  �������ַ�س�ͻ�ĵ�ַ������
 */
extern IpPoolEnum ipPoolConflict(IpPoolEnum poolType, Uint32 ipS, Uint32 ipE) {
    Uint32 ipStart = ntohl(ipS);
    Uint32 ipEnd = ntohl(ipE);

#if (DHCP_SERVER == FYES)
    /*
     * �Ƿ���dhcp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_DHCP) {
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_SERVER;
	DhcpServerProfile *profDhcp = NULL;
	profDhcp = (DhcpServerProfile *)ProfGetInstPointByIndex(mibTypeDhcp, 0);
	if ((profDhcp != NULL) && (ProfInstIsFree(profDhcp) == 0)) {
	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profDhcp->ipStart), ntohl(profDhcp->ipEnd)) == TRUE) {
		return IP_POOL_DHCP;
	    }
	}
    }
#endif

#if (DHCP_POOL == FYES)
    /*
     * �Ƿ���dhcp server��ַ�س�ͻ
     */
	int min = 0, max = 0, index = 0;
    if (poolType != IP_POOL_DHCP) {
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
	DhcpPoolProfile *profDhcp = NULL;
	ProfInstLowHigh(mibTypeDhcp, &max, &min);

	for (index = min; index < max; index++) {
		profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
		if (NULL != profDhcp) {
			if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profDhcp->ipStart), ntohl(profDhcp->ipEnd)) == TRUE) {
				return IP_POOL_DHCP;
			} else {
				continue;
			}
		}
	}
	}
#endif

#if (PPPOE_SERVER == FYES)
    /*
     * �Ƿ���pppoe server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_PPPOE) {
	MibProfileType mibTypePppoe = MIB_PROF_PPPOE_SRV_GLO;
	PPPoESrvGloProfile *profPppoe = NULL;
	Uint32 pppoeIpE;
	profPppoe = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypePppoe, 0);
	if ((profPppoe != NULL) && (ProfInstIsFree(profPppoe) == 0)) {
	    if (profPppoe->maxSessCnt == 0) {
		pppoeIpE = ntohl(profPppoe->ipStart);
	    } else {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->ipcount - 1;
#else
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->maxSessCnt - 1;
#endif
	    }
	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profPppoe->ipStart), pppoeIpE) == TRUE) {
		return IP_POOL_PPPOE;
	    }
	}
    }
#endif
#if (L2TP_SERVER == FYES)
    /*
     * �Ƿ���l2tp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_L2TP) {
	MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
	L2TPSrvGlobalProfile *profL2tp = NULL;
	Uint32 l2tpIpE;
	profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
	if ((profL2tp != NULL) && (ProfInstIsFree(profL2tp) == 0)) {
	    if (profL2tp->poolIpCnt == 0) {
		l2tpIpE = ntohl(profL2tp->poolIpStart);
	    } else {
		l2tpIpE = ntohl(profL2tp->poolIpStart) + profL2tp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profL2tp->poolIpStart), l2tpIpE) == TRUE) {
		return IP_POOL_L2TP;
	    }
	}

    }
#endif
#if (PPTP_SERVER == FYES)
    /*
     * �Ƿ���pptp server��ַ�س�ͻ
     */
    if (poolType != IP_POOL_PPTP) {
	MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
	PPTPSrvGlobalProfile *profPptp = NULL;
	Uint32 pptpIpE;
	profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
	if ((profPptp != NULL) && (ProfInstIsFree(profPptp) == 0)) {
	    if (profPptp->poolIpCnt == 0) {
		pptpIpE = ntohl(profPptp->poolIpStart);
	    } else {
		pptpIpE = ntohl(profPptp->poolIpStart) + profPptp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlap(ipStart, ipEnd, ntohl(profPptp->poolIpStart), pptpIpE) == TRUE) {
		return IP_POOL_PPTP;
	    }
	}

    }
#endif
    return IP_POOL_NONE;
}

#if (MULTI_SSID_DHCP_POOL == FYES)
static char poolName[12];
static char dhcpPoolName[40];
extern char * getIpPoolName(IpPoolEnum poolType) 
{
    switch(poolType) {
	case IP_POOL_DHCP:
	    strcpy(poolName,"DHCP pool");
	    break;
	case IP_POOL_PPPOE:	    
	    strcpy(poolName,"PPPOE pool");
	    break;
	case IP_POOL_L2TP:
	    strcpy(poolName,"L2TP pool");
	    break;
	case IP_POOL_PPTP:
	    strcpy(poolName,"PPTP pool");
	    break;
	default:
	    break;
    }

    return poolName;
}
extern char * getIpDhcpPoolName(IpPoolType dhcpPoolType) 
{
    switch(dhcpPoolType) {
	case IP_POOL_DHCP_WIRED:
	    strcpy(dhcpPoolName,"wired DHCP pool");
	    break;
	case IP_POOL_DHCP_SSID1:	    
	    strcpy(dhcpPoolName,"Wireless 2.4G SSID1 DHCP pool");
	    break;
	case IP_POOL_DHCP_SSID2:
	    strcpy(dhcpPoolName,"Wireless 2.4G SSID2 DHCP pool");
	    break;
	case IP_POOL_DHCP_SSID1_5G:
	    strcpy(dhcpPoolName,"Wireless 5G SSID1 DHCP pool");
	    break;
	case IP_POOL_DHCP_SSID2_5G:
	    strcpy(dhcpPoolName,"Wireless 5G SSID2 DHCP pool");
	    break;
	default:
	    break;
    }

    return dhcpPoolName;
}
extern void setPoolConflictMsgNew(IpPoolEnum poolType, IpPoolType type) 
{
    char errmsg[MAX_LEN_ERROR_MSG];

    strcpy(errmsg,getIpDhcpPoolName(type));
    strcat(errmsg," conflict with ");
    strcat(errmsg,getIpPoolName(poolType));
    setTheErrorMsg(errmsg);
}
extern void setDhcpPoolConflictMsg(IpPoolType type1, IpPoolType type2) 
{
    char errmsg[MAX_LEN_ERROR_MSG];

    strcpy(errmsg,getIpDhcpPoolName(type1));
    strcat(errmsg," conflict with ");
    strcat(errmsg,getIpDhcpPoolName(type2));
    setTheErrorMsg(errmsg);
}
#endif
/*
 * ���õ�ַ�س�ͻ������Ϣ
 */
extern void setPoolConflictMsg(IpPoolEnum poolType) {
#if (CONFIG_USER_GOAHEAD_LANG_EN == FYES)
	switch(poolType) {
	case IP_POOL_DHCP:
	    setTheErrorMsg(T("Conflict with the DHCP server address pool"));
	    break;
	case IP_POOL_PPPOE:	    
	    setTheErrorMsg(T("Conflict with the PPPoE server address pool"));
	    break;
	case IP_POOL_L2TP:
	    setTheErrorMsg(T("Conflict with the L2TP server address pool"));
	    break;
	case IP_POOL_PPTP:
	    setTheErrorMsg(T("Conflict with the PPPTP server address pool"));
	    break;
	default:
	    break;
    }
#else
    switch(poolType) {
	case IP_POOL_DHCP:
	    setTheErrorMsg(T("��DHCP��������ַ�س�ͻ"));
	    break;
	case IP_POOL_PPPOE:	    
	    setTheErrorMsg(T("��PPPoE��������ַ�س�ͻ"));
	    break;
	case IP_POOL_L2TP:
	    setTheErrorMsg(T("��L2TP��������ַ�س�ͻ"));
	    break;
	case IP_POOL_PPTP:
	    setTheErrorMsg(T("��PPTP��������ַ�س�ͻ"));
	    break;
	default:
	    break;
    }
#endif
    return;
}

/*
 * �ж��Ƿ���lan����ip����pppoe server��ַ���ص�
 * ���������
 *  ip	-   IP��ַ , ������
 *  nm	-   ��������, ������
 * ����ֵ��
 *  1	-   �ص�
 *  0	-   ���ص�
 */
extern int isOverlapInnerIp(Uint32 ip, Uint32 nm) {
    int rst = 0;
    Uint32 ip_start, ip_end;
    Uint32 lan_ip_start = 0, lan_ip_end = 0;
#if (MULTI_LAN == FYES)
    Uint32 lan_ip_start2 = 0, lan_ip_end2 = 0;
    Uint32 lan_ip_start3 = 0, lan_ip_end3 = 0;
    Uint32 lan_ip_start4 = 0, lan_ip_end4 = 0;
#endif
    Uint32 poe_ip_start = 0, poe_ip_end = 0;
#if (PPPOE_SERVER == FYES)
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
#endif
    InterfaceProfile *profIf = NULL;
    MibProfileType mibTypeIf = MIB_PROF_INTERFACE;

    if (ip == 0) {
	return 1;
    }
    /* �����ʼ�ͽ���ip */
    ip_start = ip & nm;
    ip_end = ip_start + (~nm);
#if (PPPOE_SERVER == FYES)
    /* ���pppoe server ��ַ��IP */
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypeS, 0);
    if (profS != NULL) {
	poe_ip_start = ntohl(profS->ipStart);
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
	poe_ip_end = poe_ip_start + profS->ipcount - 1;
#else
	poe_ip_end = poe_ip_start + profS->maxSessCnt - 1;
#endif
	if (ipSegmentOverlap(ip_start, ip_end, poe_ip_start, poe_ip_end) == TRUE) {
	    rst = 1;
	}
    }
#endif

    if (rst == 0) {
	profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibTypeIf, 0);
	if (profIf != NULL) {
	    lan_ip_start = ntohl(profIf->ether.sta.ip) & ntohl(profIf->ether.sta.nm);
	    lan_ip_end = lan_ip_start + ~(ntohl(profIf->ether.sta.nm));
#if (MULTI_LAN == FYES)
	    lan_ip_start2 = ntohl(profIf->ether.sta.ip2) & ntohl(profIf->ether.sta.nm2);
	    lan_ip_end2 = lan_ip_start2 + ~(ntohl(profIf->ether.sta.nm2));
	    lan_ip_start3 = ntohl(profIf->ether.sta.ip3) & ntohl(profIf->ether.sta.nm3);
	    lan_ip_end3 = lan_ip_start3 + ~(ntohl(profIf->ether.sta.nm3));
	    lan_ip_start4 = ntohl(profIf->ether.sta.ip4) & ntohl(profIf->ether.sta.nm4);
	    lan_ip_end4 = lan_ip_start4 + ~(ntohl(profIf->ether.sta.nm4));
#endif
	    if ((ipSegmentOverlap(ip_start, ip_end, lan_ip_start, lan_ip_end) == TRUE) 
#if (MULTI_LAN == FYES)
		|| (ipSegmentOverlap(ip_start, ip_end, lan_ip_start2, lan_ip_end2) == TRUE)
		|| (ipSegmentOverlap(ip_start, ip_end, lan_ip_start3, lan_ip_end3) == TRUE)
		|| (ipSegmentOverlap(ip_start, ip_end, lan_ip_start4, lan_ip_end4) == TRUE)
#endif
		)
	    {
		rst = 1;
	    }
	}
    }
    return rst;
}
