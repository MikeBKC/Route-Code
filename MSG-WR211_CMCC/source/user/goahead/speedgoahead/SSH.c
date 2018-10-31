#include "uttMachine.h"
#if (SSH_CLI == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

extern void SSHRuleAdd(char *ip_from, char *ip_end)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	if(profSys->SSH.SSHEn == FUN_ENABLE)
	{
	    doSystem("iptables -t mangle -D %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*ɾ������*/
	    doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port %d,161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, profSys->SSH.Port, ip_from, ip_end, SYS_LAN_GROUP);/*���ӹ���*/
	}
    }
    return;
}
#if (NET_SECURITY == FYES)
/********************************************************************
 * ������ AccessCtr
 * ���ܣ� ���˿ڸı�ʱ�ı��豸���ʿ��Ƶ�
 * ������ 2012-11-07
 * ������ prof      -        �˿�
 *	  action    -        ��������ӻ���ɾ����
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void AccessCtr(Uint32 Port, char action, int flag)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    char ip_from[32];
    char ip_end[32];
    struct in_addr addrS;

    memset(ip_from, 0, 32);
    memset(ip_end, 0, 32);
    memset(&addrS, 0, sizeof(addrS));
    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	if((profSys->sysNetSe.accessCtrlFromIP != 0U) && (profSys->sysNetSe.accessCtrlEndIP != 0U))
	{/*�豸���ʿ��ƿ����Ÿı����*/
	    addrS.s_addr = profSys->sysNetSe.accessCtrlFromIP;
	    strcpy(ip_from, inet_ntoa(addrS));/*��ʼip�ִ�*/
	    addrS.s_addr = profSys->sysNetSe.accessCtrlEndIP;
	    strcpy(ip_end, inet_ntoa(addrS));/*����ip�ִ�*/
	    doSystem("iptables -t mangle -%c %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port %d,161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", action, DEFENSE_FILTER_CHAIN, Port, ip_from, ip_end, SYS_LAN_GROUP);/*�޸ķ��ʿ��ƹ���*/
	    if((action == Iptables_DEL) && (flag == 1))
	    {
		doSystem("iptables -t mangle -A %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*�޸ķ��ʿ��ƹ���*/
	    }
	    else if((action == Iptables_DEL) && (flag == 2))
	    {
		doSystem("iptables -t mangle -D %s -m uttdev --is-lan-in -p tcp -m multiport --destination-port 161,443,60023 -m iprange ! --src-range %s-%s -m set --set %s dst -j DROP", DEFENSE_FILTER_CHAIN, ip_from, ip_end, SYS_LAN_GROUP);/*�޸ķ��ʿ��ƹ���*/
	    }
	    else
	    {
		/*test*/
	    }
	}
    }
    return;
}
#endif
/********************************************************************
 * ������ SSHInit
 * ���ܣ� ��ʼ������
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void SSHInit(void) 
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    if((profSys != NULL) && (ProfInstIsFree(profSys) == 0)) 
    {
	doSystem("dropbear_init.sh %d %d", profSys->SSH.SSHEn, profSys->SSH.Port);/*���ýű�*/
    }
    return;
}

/********************************************************************
 * ������ SSHProfileChange
 * ���ܣ� profile change ����
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
static void SSHProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr)
{
    switch(changeType) 
    {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->SSH.SSHEn == FUN_ENABLE) && (newProfileDataPtr->SSH.SSHEn == FUN_DISABLE))
	    {
		AccessCtr(oldProfileDataPtr->SSH.Port, Iptables_DEL, 1);
		doSystem("killall dropbear");
	    }
	    if(((oldProfileDataPtr->SSH.SSHEn == FUN_DISABLE) && (newProfileDataPtr->SSH.SSHEn == FUN_ENABLE)) || (oldProfileDataPtr->SSH.Port != newProfileDataPtr->SSH.Port))
	    {
		doSystem("dropbear.sh %d", newProfileDataPtr->SSH.Port);
#if (NET_SECURITY == FYES)
		if((oldProfileDataPtr->SSH.SSHEn == FUN_DISABLE) && (newProfileDataPtr->SSH.SSHEn == FUN_ENABLE))
		{
		    AccessCtr(oldProfileDataPtr->SSH.Port, Iptables_DEL, 2);
		}
		else
		{
		    AccessCtr(oldProfileDataPtr->SSH.Port, Iptables_DEL, 0);
		}
		AccessCtr(newProfileDataPtr->SSH.Port, Iptables_ADD, 0);
#endif
	    }
	    break;
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	default:/*test*/
	    break;
    }
    return;
}

/********************************************************************
 * ������ funInitSSH
 * ���ܣ� ��ʼ������
 * ������ 2012-10-16
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
extern void funInitSSH(void) 
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)SSHProfileChange);
    SSHInit();
    return;
}
#endif
