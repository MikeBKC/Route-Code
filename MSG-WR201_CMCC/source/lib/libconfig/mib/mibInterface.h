#ifndef MIBINTERFACE_H
#define MIBINTERFACE_H

#include "mibInterfaceShare.h"

# if(UTT_DOMAINACCESS == FYES)
#if ((BRAND_UTT == FYES)||(BRAND_UTT_ADD == FYES))
#define DOMAIN_NAME "loginutt.com"
#define DOMAIN_NAME_ADD "uttlogin.cn"
#elif (BRAND_LEVEL_ONE == FYES)
#define DOMAIN_NAME "router.level1.com"
#else
#define DOMAIN_NAME "uttlogin.com"
#endif
#endif


#endif
