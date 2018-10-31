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

#if (SNMP == FYES)
#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static void SNMPProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const SnmpInfoProfile *oldProfileDataPtr, const SnmpInfoProfile *newProfileDataPtr )        
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    doSystem("snmpdConfig.sh ");
	    doSystem("killall snmpd");
	    sleep(2);	/*等配置写完*/
	    if((newProfileDataPtr->snmp_enable == 1U))
	    {
		/*开启snmp*/
		doSystem("snmpd -c /etc/snmpd.conf");
	    }
            break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }      
    return;
}

static void snmpInit()
{
    doSystem("snmpdConfig.sh ");
    sleep(2);	/*等配置写完*/
    doSystem("snmpd -c /etc/snmpd.conf");
}
extern void funcInitSNMP(void)
{
    registerForProfileUpdates(MIB_PROF_SNMP_INFO, (Pointer)NULL, (ProfileUpdateFnc)SNMPProfileChange);/*触发函数*/
    Uint32 snmpEnable;
    MibProfileType mibType = MIB_PROF_SNMP_INFO;
    SnmpInfoProfile *prof = NULL;
    prof = (SnmpInfoProfile *)ProfGetInstPointByIndex(mibType,0);
    /*读配置是否开启*/
    snmpEnable = prof->snmp_enable;
    /*写配置文件*/ 
    if (snmpEnable==1) 
    {
	/*开启snmp*/
	snmpInit();
    }
    return;
}
#endif
