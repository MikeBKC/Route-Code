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
 * �ر��������ƹ���
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
 * ���������ƹ��ܲ��޸����������
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
 * �½�connect_limit���й���
 * ��ע��Ŀǰ��������û�зַ���
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
 * �޸��������ƹ������õ����������
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

/* ��ʼ������ */
static void connLimitInit(void) {
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (profSys && (profSys->connLimit.connLimitEn == FUN_ENABLE)) {
	createConnLimitConfig(profSys);
    }
}

/* profile change ���� */
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
		/* ����->�ر� */
		if ((oldProf->connLimit.connLimitEn == FUN_ENABLE) 
			&& (newProf->connLimit.connLimitEn == FUN_DISABLE)) {
		    /* ���������ع��� */
		    closeConnLimit();
		} else if (newProf->connLimit.connLimitEn == FUN_ENABLE) {
		    /* �رյ����� */
		    if (oldProf->connLimit.connLimitEn == FUN_DISABLE) {
			createConnLimitConfig(newProf);
		    } 
		    /* ���������� */
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
 * ��ʼ������
 */
extern void funInitConnLimit(void) {
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, connLimitProfileChange);
    connLimitInit();

    return;
}
#endif
