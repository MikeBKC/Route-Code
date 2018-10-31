
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

#if (DNS_AGENCY== FYES)

extern void DnsAgencyInit()
{
//	printf("--------line: %d in %s()\n", __LINE__, __func__);

	doSystem("config-dnsAgency.sh 0 0.0.0.0 0.0.0.0");

	return;
}

void DnsAgencyProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const DnsAgencyProfile* oldProfileDataPtr, const DnsAgencyProfile* newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:
            DnsAgencyInit();
            break;
    };
}

void funInitDnsAgency()
{
	registerForProfileUpdates(MIB_PROF_DNS_AGENCY, NULL, DnsAgencyProfileChange);
	DnsAgencyInit();
}

#endif
