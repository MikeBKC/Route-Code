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
    printf("\nUsage: pptpd-prof <userName> <key>\n");
    printf("return the key value\n");
    return;
}

/*
 * 输入userName和成员对应的关键字，返回成员对应的值
 * argv[1]  - usertName
 * argv[2]  - key
 */
int main(int argc, char *argv[]) {
    MibProfileType mibType = MIB_PROF_PPTP_SRV_ACCOUNT;
    PPTPSrvAccProfile *profPptpA = NULL;
    struct in_addr addr;
    int i ,min, max;
    int found = 0;

    if (argc != 3) {
	printf("error");
	usage();
	return -1;
    }
    /*
     * 初始化profile共享内存
     */
    ProfInit();
    
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profPptpA != NULL) && (ProfInstIsFree(profPptpA) == 0) && (strcmp(argv[1], profPptpA->userName) == 0)) {
	    found = 1;
	    break;
	}
    }

#if 0
    profPptpA = (PPTPSrvAccProfile *)ProfGetInstPointByName(mibType, argv[1]);
#endif

    if (found == 0) {
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
