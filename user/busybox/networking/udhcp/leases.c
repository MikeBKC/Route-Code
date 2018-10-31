/* vi: set sw=4 ts=4: */
/*
 * leases.c -- tools to manage DHCP leases
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 */
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"

#include "common.h"
#include "dhcpd.h"
#include "syslog.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"

#if (DHCP_AUTO_BIND == FYES)
extern void autodel(struct dhcpOfferedAddr *lease);
static void DhcpAutoDel(IPADDR ip, MacAddr macAddr);
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_SYSLOG(pri, fmt, args...)		syslog(pri, fmt, ## args)	
#else
#define SWORD_SYSLOG(pri, fmt, args...)
#endif
#endif

/* Find the oldest expired lease, NULL if there are no expired leases */
static struct dhcpOfferedAddr *oldest_expired_lease(void)
{
	struct dhcpOfferedAddr *oldest = NULL;
// TODO: use monotonic_sec()
#if 0
    /*edit by brwang*/
	unsigned long oldest_lease = time(0);
#else
	unsigned long oldest_lease = utt_getSysUpTime();
#endif
	unsigned i;

	for (i = 0; i < server_config.max_leases; i++)
		if (oldest_lease > leases[i].expires) {
			oldest_lease = leases[i].expires;
			oldest = &(leases[i]);
		}
	return oldest;
}


/* clear every lease out that chaddr OR yiaddr matches and is nonzero */
static void clear_lease(const uint8_t *chaddr, uint32_t yiaddr)
{
	unsigned i, j;

	for (j = 0; j < 16 && !chaddr[j]; j++)
		continue;

	for (i = 0; i < server_config.max_leases; i++)
		if ((j != 16 && memcmp(leases[i].chaddr, chaddr, 16) == 0)
		 || (yiaddr && leases[i].yiaddr == yiaddr)
		) {
			memset(&(leases[i]), 0, sizeof(leases[i]));
		}
}


/* add a lease into the table, clearing out any old ones */
struct dhcpOfferedAddr *add_lease(uint8_t *hostname, const uint8_t *chaddr, uint32_t yiaddr, unsigned long lease)
{
	struct dhcpOfferedAddr *oldest;

	/* clean out any old ones */
	clear_lease(chaddr, yiaddr);

	oldest = oldest_expired_lease();

	if (oldest) {
		if (hostname) {
			uint8_t length = *(hostname-1); 
			if (length>15) length = 15; 
			memcpy(oldest->hostname, hostname, length); 
			oldest->hostname[length] = 0; 
		}
		memcpy(oldest->chaddr, chaddr, 16);
		oldest->yiaddr = yiaddr;
#if 0
        /*edit by brwang*/
		oldest->expires = time(0) + lease;
#else
		oldest->expires = utt_getSysUpTime() + lease;
#endif
	}

	return oldest;
}


/* true if a lease has expired */
int lease_expired(struct dhcpOfferedAddr *lease)
{
	//return (lease->expires < (unsigned long) time(0));
#if 0
    /* edit by brwang*/
	if((lease->expires != 0) && (lease->expires < (unsigned long) time(0))){
#else
	if((lease->expires != 0) && (lease->expires < (unsigned long) utt_getSysUpTime())){
#endif
#if (DHCP_AUTO_BIND == FYES)
		autodel(lease);
#endif
        memset(lease,0,sizeof(struct dhcpOfferedAddr));
        return 1;
    } else {
        return 0;
    }
}


/* Find the first lease that matches chaddr, NULL if no match */
struct dhcpOfferedAddr *find_lease_by_chaddr(const uint8_t *chaddr)
{
	unsigned i;

	for (i = 0; i < server_config.max_leases; i++)
		if (!memcmp(leases[i].chaddr, chaddr, 16))
			return &(leases[i]);

	return NULL;
}


/* Find the first lease that matches yiaddr, NULL is no match */
struct dhcpOfferedAddr *find_lease_by_yiaddr(uint32_t yiaddr)
{
	unsigned i;

	for (i = 0; i < server_config.max_leases; i++)
		if (leases[i].yiaddr == yiaddr)
			return &(leases[i]);

	return NULL;
}

#if (SYS_LOG_INFO == FYES)
#ifdef CONFIG_UTT_DHCP_COLLISION
static int isDhcpLogEn(void)
{
    MibProfileType mibType = MIB_PROF_SYS_LOG_INFO;
    SysLogInfoProfile *prof = NULL;
	int ret = 0;

    prof = (SysLogInfoProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (ProfInstIsFree(prof) == 0) 
		if (prof->DhcpLogEnable == LOG_ENABLE) 
			ret = 1;
	
	return ret;
}
#endif
#endif

/* check is an IP is taken, if it is, add it to the lease table */
static int nobody_responds_to_arp(uint32_t addr)
{
	/* 16 zero bytes */
	static const uint8_t blank_chaddr[16] = { 0 };
	/* = { 0 } helps gcc to put it in rodata, not bss */

	struct in_addr temp;
	int r;

	r = arpping(addr, server_config.server, server_config.arp, server_config.interface);
	if (r)
		return r;

	temp.s_addr = addr;
	bb_info_msg("%s belongs to someone, reserving it for %u seconds",
		inet_ntoa(temp), (unsigned)server_config.conflict_time);
#if (SYS_LOG_INFO == FYES)
#ifdef CONFIG_UTT_DHCP_COLLISION
	/*DHCP冲突检测开启则发送冲突信息*/
	if (isDhcpLogEn() == 1)
		syslog(LOG_WARNING, "dhcp:ip conflicted arp:%s", inet_ntoa(temp));
#endif
#endif
	add_lease(blank_chaddr, blank_chaddr, addr, server_config.conflict_time);
	return 0;
}


/*added by brwang for mantis bug 8051*/
uint32_t utt_dhcpSrvNm = 0;
/* find an assignable address, if check_expired is true, we check all the expired leases as well.
 * Maybe this should try expired leases by age... */
uint32_t find_address(int check_expired)
{
	uint32_t addr, ret;
	struct dhcpOfferedAddr *lease = NULL;

	addr = server_config.start_ip; /* addr is in host order here */
	for (; addr <= server_config.end_ip; addr++) {
#if 0
		/* ie, 192.168.55.0 */
		if (!(addr & 0xFF))
			continue;
		/* ie, 192.168.55.255 */
		if ((addr & 0xFF) == 0xFF)
			continue;
#else
		/* ie, 192.168.55.0 */
		if (!(addr & (~utt_dhcpSrvNm)))
			continue;
		/* ie, 192.168.55.255 */
		if ((addr & (~utt_dhcpSrvNm)) == (~utt_dhcpSrvNm))
			continue;
#endif
		/* Only do if it isn't assigned as a static lease */
		ret = htonl(addr);
		if (!reservedIp(server_config.static_leases, ret)) {
			/* lease is not taken */
			lease = find_lease_by_yiaddr(ret);
			/* no lease or it expired and we are checking for expired leases */
			if ((!lease || (check_expired && lease_expired(lease)))
			 && nobody_responds_to_arp(ret) /* it isn't used on the network */
			) {
				return ret;
			}
		}
	}
	return 0;
}
#if (DHCP_AUTO_BIND == FYES)
/********************************************************************
 * 函数： autodel
 * 功能： 检查自动删除是否打开，打开则调用删除函数
 * 创建： 2012-09-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void autodel(struct dhcpOfferedAddr *lease)
{
	DhcpServerProfile *profDhcpS = NULL;
    int i = 0;
    char macStr[13];
    MacAddr macAddr;

	profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);
	if((profDhcpS != NULL) && (ProfInstIsFree(profDhcpS) == 0))
	{
		if(profDhcpS->DhcpDelAutobind == FUN_ENABLE)
		{
			SWORD_SYSLOG(LOG_NOTICE, "%s------%d, lease->expires = %u, lease->yiaddr = %x\n", __func__, __LINE__, lease->expires, lease->yiaddr);
			memset(macStr, 0, sizeof(macStr));
			for (i = 0; i < 6; i++)
			{
				sprintf(macStr,"%s%02X",macStr, lease->chaddr[i]);
			}
			SWORD_SYSLOG(LOG_NOTICE, "%s------%d, macStr = %s\n", __func__, __LINE__, macStr);
			converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
			DhcpAutoDel(lease->yiaddr, macAddr);
		}
	}
	return;
}
/********************************************************************
 * 函数： DhcpAutoDel
 * 功能： 根据ip删除dhcp自动绑定上的ip、mac
 * 创建： 2012-09-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
static void DhcpAutoDel(IPADDR ip, MacAddr macAddr)
{
	MibProfileType mibType = MIB_PROF_USER;
	UserProfile *prof = NULL;
    struProfAlloc *profList  = NULL;
	int min = 0, max = 0;
	int i = 0, flag = 0;
	SWORD_SYSLOG(LOG_NOTICE, "%s------%d, ip = %x\n", __func__, __LINE__, ip);

	ProfInstLowHigh(mibType, &max, &min);
	for(i = min; i < max; i++) 
	{
		prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
		if(prof != NULL && ProfInstIsFree(prof) == 0) 
		{
			if((prof->bindType == AUTO_BIND) && (ip == prof->ip) && (memcmp(&macAddr, &(prof->mac), sizeof(macAddr)) == 0))
			{ 
				SWORD_SYSLOG(LOG_NOTICE, "%s------%d, ip = %x, prof->ip = %x\n", __func__, __LINE__, ip, prof->ip);
				ProfBackupByIndex(mibType, PROF_CHANGE_DEL, i, &profList);
				ProfDelInstByIndex(mibType, i);/*删除已有实例*/
				flag = 1;
			}
		}
	}
	if(flag == 1)
	{
		ProfUpdate(profList);/*发消息删除*/
		ProfFreeAllocList(profList);/*释放备份空间*/
	}
	return;
}
#endif
