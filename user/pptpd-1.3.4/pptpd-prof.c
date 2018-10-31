/*
 * read config from profile
 */
#include	"uttMachine.h"
#if (PPTP_SERVER == FYES)
#include	<stdio.h>

#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"argcmd.h"
#include	"a2n.h"

void usage(void) {
    printf("\nUsage: pptpd-prof <instName> <key>\n");
    printf("return the key value\n");
    return;
}

/*
 * 输入instName和成员对应的关键字，返回成员对应的值
 * argv[1]  - instName
 * argv[2]  - key
 */
int main(int argc, char *argv[]) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    struct in_addr addr;

    if (argc != 3) {
	printf("error");
	usage();
	return -1;
    }
    /*
     * 初始化profile共享内存
     */
    ProfInit();
    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByName(mibType, argv[1]);
    if (profPptpA == NULL) {
	printf("error");
	return -1;
    }

    if (strcmp(argv[2], "peerType") == 0) {
	printf("%s", getEnumString(vpnPeerTypeEnum, profPptpA->peerType));
	return 0;
    } else if (strcmp(argv[2], "remoteLanIp") == 0) {
	addr.s_addr = profPptpA->remoteLanIp;
	printf("%s", inet_ntoa(addr));
	return 0;
    } else if (strcmp(argv[2], "remoteLanNetmask") == 0) {
	addr.s_addr = profPptpA->remoteLanNetmask;
	printf("%s", inet_ntoa(addr));
	return 0;
    } else {
	printf("error");
	return -1;
    }

    return 0;
}
#else
int main(int argc, char *argv[]) {
    printf("L2TP_SERVER == FNO\n");
    return 0;
}
#endif
