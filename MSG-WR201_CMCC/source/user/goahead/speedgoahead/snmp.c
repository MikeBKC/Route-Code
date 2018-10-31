
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "mibSnmp.h"
#include "internet.h"

#if (FEATURE_SW_SNMP == FYES)
/***************************************************
 * ������start_snmp_proc
 * ���ܣ�����snmp����
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-1-30
 **************************************************/ 
static void start_snmp_proc(void)
{
    char buf[128];

    memset(buf, 0, 128);
    sprintf(buf, "%s", "killall -q snmpd");
    doSystem(buf);

    sprintf(buf, "%s", "/bin/snmpd -c /etc/snmpd.conf &");
    doSystem(buf);
    return;
}

/**************************************************8
 * ������kill_snmp_proc
 * ���ܣ�ɾ��snmp����
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-1-30
 * *************************************************/
static void kill_snmp_proc(void)
{
    char buf[128];

    memset(buf, 0, 128);
    sprintf(buf, "%s", "killall -q snmpd");

    doSystem(buf);

    return;
}

/****************************************************
 * ������write_snmpd_conf
 * ���ܣ�д�����ļ�
 * ������trap ip��write community name��read community 
 *       name, trap community name
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-1-30
 ******************************************************/
static void write_snmpd_conf()
{
	char *r_com = NULL;
    char *w_com = NULL;
    char *t_com = NULL;
    char *trapip = NULL;
	MibProfileType mibType = MIB_PROF_SNMP;
	SnmpProfile *prof = NULL;

    FILE *fd = NULL; 
    char conf_buf[2048]; 
			
    memset(conf_buf, 0, 2048);
	prof = (SnmpProfile *)ProfGetInstPointByIndex(mibType,0);
    /*������*/
    r_com = prof->r_community;
    w_com = prof->w_community;
    t_com = prof->trap_community;
    trapip = prof->trap_ip;

    sprintf(conf_buf, "com2sec readUser default %s\ncom2sec writeUser default %s\n\ngroup readGroup v1 readUser\ngroup writeGroup v1 writeUser\ngroup readGroup v2c readUser\ngroup writeGroup v2c writeUser\n\nview all included .1 80\n\naccess readGroup \"\" any noauth exact all none none\naccess writeGroup \"\" any noauth exact all none none\n\ntrap2sink %s:162\ninformsink %s:162", r_com, w_com, trapip, trapip);

    fd = fopen("/etc/snmpd.conf", "w+"); 
	     
    if (fd == NULL) 
    {    
	return; 
    } 
						 
    fseek(fd, 0, SEEK_SET); 
    /* write some data to the file */ 
    fwrite(conf_buf, strlen(conf_buf), 1, fd); 

    fclose(fd); 
			     
    return; 

}
/********************************************************************
* ������ SnmpProfileChange
* ���ܣ� ���ر仯 ��Ч����
* ������ 2013-02-26
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� hau.yuye 
********************************************************************/
static void SnmpProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const SnmpProfile *oldProfileDataPtr, const SnmpProfile *newProfileDataPtr )        
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->snmp_enable == 0U) &&(newProfileDataPtr->snmp_enable == 1U))
	    {
		write_snmpd_conf();
        start_snmp_proc();
	    }
	    else if((oldProfileDataPtr->snmp_enable == 1U) && (newProfileDataPtr->snmp_enable == 0U))
	    {
		kill_snmp_proc();
	    }
		else if ((oldProfileDataPtr != newProfileDataPtr) && (newProfileDataPtr->snmp_enable == 1U))
		{
		write_snmpd_conf();
        start_snmp_proc();
		}
            break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }      
    return;
}

/****************************************************
 * ������init_snmp_conf
 * ���ܣ�snmp��ʼ��
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-1-30
 ***************************************************/
extern void funcInitSnmpConf(void)
{
    registerForProfileUpdates(MIB_PROF_SNMP, (Pointer)NULL, (ProfileUpdateFnc)SnmpProfileChange);/*��������*/
    Uint32 snmpEnable;
	MibProfileType mibType = MIB_PROF_SNMP;
	SnmpProfile *prof = NULL;
	prof = (SnmpProfile *)ProfGetInstPointByIndex(mibType,0);
    /*�������Ƿ���*/
    snmpEnable = prof->snmp_enable;
    /*д�����ļ�*/ 
    write_snmpd_conf();
    if (snmpEnable==1) 
    {
	/*����snmp*/
        start_snmp_proc();
    }
    else
    {
	/*δ����snmp*/
		kill_snmp_proc();
    }
       return;
}
#endif

