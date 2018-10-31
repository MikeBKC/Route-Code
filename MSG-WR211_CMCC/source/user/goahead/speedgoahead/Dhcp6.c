#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"


#if (IPV6_DHCPV6 == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Dhcp6ServerChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Dhcp6Profile *oldProfileDataPtr, const Dhcp6Profile *newProfileDataPtr );

static void Dhcp6init(void);
static void Dhcp6SerConf(Dhcp6Profile *prof);

/********************************************************************
 * * ������ Dhcp6init
 * * ���ܣ� DHCP��ʼ������
 * * ������ 2014-09-30
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void Dhcp6init()
{
    MibProfileType mibType=MIB_PROF_DHCP6;
    Dhcp6Profile   *prof=NULL;
    prof=(Dhcp6Profile *)ProfGetInstPointByIndex(mibType, 0);
    Dhcp6SerConf(prof);
}

/********************************************************************
 * * ������ Dhcp6SerConf
 * * ���ܣ� DHCP������д�����ļ�����
 * * ������ 2014-09-30
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void Dhcp6SerConf(Dhcp6Profile *prof)
{
    FILE *fp;
    char *port = getLanIfName();
    if((fp = fopen("etc/dhcp6s.conf","w+")) == NULL)
    {    
	printf("open dhcp6s.conf is error!\n");
        return;
    }
    fprintf(fp,"interface %s\n",port);
    fprintf(fp,"{\n");
    fprintf(fp,"server-preference 255;\n");
    fprintf(fp,"renew-time %lu;\n",prof->staleTime/4);
    fprintf(fp,"rebind-time %lu;\n",prof->staleTime*2/5);
    fprintf(fp,"prefer-life-time %lu;\n",prof->staleTime/2);
    fprintf(fp,"valid-life-time %lu;\n",prof->staleTime);
    fprintf(fp,"allow rapid-commit;\n");
    fprintf(fp,"link %s {\n",port);
    fprintf(fp,"allow unicast;\n");
    fprintf(fp,"send unicast;\n");
    fprintf(fp,"pool{\n");
    fprintf(fp,"range %s to %s/%d;\n",prof->sta_ipv6_addr,prof->end_ipv6_addr,prof->dhcpPrefixLength);
    fprintf(fp,"prefix %s/%d;\n",prof->sta_ipv6_addr,prof->dhcpPrefixLength);
    fprintf(fp,"};\n");
    fprintf(fp,"};\n");
    fprintf(fp,"option dns_servers %s %s;\n",prof->ipv6DnsZ,prof->ipv6DnsB);
    fprintf(fp,"};\n");
    fclose(fp);
    doSystem("killall dhcp6s");

//    doSystem("echo 'nameserver %s' > /tmp/ipv6resolv.conf",prof->ipv6DnsZ);
//    doSystem("echo 'nameserver %s' >> /tmp/ipv6resolv.conf",prof->ipv6DnsB);
//    doSystem("cat /tmp/ipv4resolv.conf > /etc/resolv.conf");
//    doSystem("cat /tmp/ipv6resolv.conf >> /etc/resolv.conf");

    if(prof->dhcpEnabled == 1){
	doSystem("dhcp6s -c /etc/dhcp6s.conf -d %s &",port);
    }

}

/********************************************************************
 * * ������ Ipv6RadvdGlobalChange
 * * ���ܣ� RADVDȫ������Change����
 * * ������ 2014-09-25
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang 
 * ********************************************************************/
static void Dhcp6ServerChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Dhcp6Profile *oldProfileDataPtr, const Dhcp6Profile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    Dhcp6SerConf((Dhcp6Profile *)newProfileDataPtr);/*����д�����ļ�*/
            break;

	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;
}



/********************************************************************
 * * ������ funInitRadvd
 * * ���ܣ� Radvd ��ʼ����ں���
 * * ������ 2014-09-28
 * * ������ ��
 * * ���أ� ��
 * * ����� ��
 * * ���ߣ� xiao.yunyang
 * ********************************************************************/
void funInitDhcp6(void)
{
    
    registerForProfileUpdates(MIB_PROF_DHCP6, (Pointer)NULL, (ProfileUpdateFnc)Dhcp6ServerChange);/*DHCP���������ô�������*/
    Dhcp6init();/*��ʼ��*/
    return;
}

#endif
