#include "uttMachine.h"
#if (CONNECT_LIMIT == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#define CONN_LIMIT_CHAIN    "connect_limit"
#define PATH	"/proc/sys/net/netfilter/"

static void connLimitConfigChange(SystemProfile *prof);

/*
 * 关闭连接限制功能
 */
static void closeConnLimit(void) {
    /*
    doSystem("iptables -D FORWARD -j %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
    doSystem("iptables -F %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
    doSystem("iptables -X %s >/dev/null 2>&1", CONN_LIMIT_CHAIN); 
    */
    doSystem("echo 0 > %sutt_connlimit_switch", PATH);
}

/*
 * 打开连接限制功能并修改最大连接数
 */
static void createConnLimitConfig(SystemProfile *prof) {
    /*
    doSystem("iptables -N %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
    doSystem("iptables -F %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
    */
    doSystem("echo 1 > %sutt_connlimit_switch", PATH);
    connLimitConfigChange(prof);
    /*
    doSystem("iptables -D FORWARD -j %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
    doSystem("iptables -I FORWARD -j %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
    */
    return;
}


/*
 * 新建connect_limit链中规则
 * 备注：目前连接限制没有分方向
 *
static void connLimitRuleCreate(SystemProfile *prof) {
    if (!prof) {
	return;
    }
    if (prof->connLimit.totalConnCount != 0) {
    	doSystem("iptables -A %s -m connlimit --connlimit-above %d -j DROP >/dev/null 2>&1" , CONN_LIMIT_CHAIN, prof->connLimit.totalConnCount);
    }
    if (prof->connLimit.TCPConnCount != 0) {
	doSystem("iptables -A %s -p tcp -m connlimit --connlimit-above %d -j DROP >/dev/null 2>&1" , CONN_LIMIT_CHAIN, prof->connLimit.TCPConnCount);
    }
    if (prof->connLimit.UDPConnCount != 0) {
	doSystem("iptables -A %s -p udp -m connlimit --connlimit-above %d -j DROP >/dev/null 2>&1" , CONN_LIMIT_CHAIN, prof->connLimit.UDPConnCount);
    }
    if (prof->connLimit.ICMPConnCount != 0) {
	doSystem("iptables -A %s -p icmp -m connlimit --connlimit-above %d -j DROP >/dev/null 2>&1" , CONN_LIMIT_CHAIN, prof->connLimit.ICMPConnCount);
    }
}
*/


/*
 * 修改连接限制功能配置的最大连接数
 */
static void connLimitConfigChange(SystemProfile *prof){
    if (!prof) {
	return;
    }
    doSystem("echo %d > %sutt_host_conn_max", prof->connLimit.totalConnCount, PATH);
    doSystem("echo %d > %sutt_host_tcp_conn_max", prof->connLimit.TCPConnCount, PATH);
    doSystem("echo %d > %sutt_host_udp_conn_max", prof->connLimit.UDPConnCount, PATH);
    doSystem("echo %d > %sutt_host_icmp_conn_max", prof->connLimit.ICMPConnCount, PATH);
}

/* 初始化函数 */
static void connLimitInit(void) {
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (profSys && (profSys->connLimit.connLimitEn == FUN_ENABLE)) {
	createConnLimitConfig(profSys);
    }
}

/* profile change 函数 */
static void connLimitProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr)
{
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;

    switch(changeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	    if (memcmp(&newProf->connLimit, &oldProf->connLimit, sizeof(SysConnLimit))!=0) {
		/* 开启->关闭 */
		if ((oldProf->connLimit.connLimitEn == FUN_ENABLE) 
			&& (newProf->connLimit.connLimitEn == FUN_DISABLE)) {
		    /* 清除所有相关规则 */
		    closeConnLimit();
		} else if (newProf->connLimit.connLimitEn == FUN_ENABLE) {
		    /* 关闭到开启 */
		    if (oldProf->connLimit.connLimitEn == FUN_DISABLE) {
			createConnLimitConfig(newProf);
		    } 
		    /* 开启到开启 */
		    else {
			/*
			doSystem("iptables -F %s >/dev/null 2>&1", CONN_LIMIT_CHAIN);
			*/
			connLimitConfigChange(newProf);
		    }
		}
	    }
	    break;
	default:
	    break;
    }
    return;
}

/*
 * 初始化函数
 */
extern void funInitConnLimit(void) {
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, connLimitProfileChange);
    connLimitInit();

    return;
}
#endif
