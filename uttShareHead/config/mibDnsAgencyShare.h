#pragma once

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (DNS_AGENCY == FYES)
typedef struct st_dns_agency_profile {
	mibHead head;
	ProfileEnableEnum DnsProxyEn;
	IPADDR pDns;
	IPADDR sDns;
} DnsAgencyProfile;
#endif
