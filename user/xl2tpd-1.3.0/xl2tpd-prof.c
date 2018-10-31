/*
 * read config from profile
 */
#include	"uttMachine.h"
#if (L2TP_SERVER == FYES)
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
    printf("\nUsage: xl2pd-prof <userName> <key>\n");
    printf("return the key value\n");
    return;
}

/*
 * 输入账号名和成员对应的关键字，返回成员对应的值
 * argv[1]  - userName
 * argv[2]  - key
 */
int main(int argc, char *argv[]) {
    MibProfileType mibType = MIB_PROF_L2TP_SRV_ACCOUNT;
    L2TPSrvAccProfile *profL2tpA = NULL;
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
	profL2tpA = (L2TPSrvAccProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((profL2tpA != NULL) && (ProfInstIsFree(profL2tpA) == 0) && (strcmp(argv[1], profL2tpA->userName) == 0)) {
	    found = 1;
	    break;
	}
    }
#if 0
    profL2tpA = (L2TPSrvAccProfile *)ProfGetInstPointByName(mibType, argv[1]);
#endif
    if (found == 0) {
	printf("error");
	return -1;
    }

    if (strcmp(argv[2], "peerType") == 0) {
	printf("%s", getEnumString(vpnPeerTypeEnum, profL2tpA->peerType));
	return 0;
    } else if (strcmp(argv[2], "remoteLanIp") == 0) {
	addr.s_addr = profL2tpA->remoteLanIp;
	printf("%s", inet_ntoa(addr));
	return 0;
    } else if (strcmp(argv[2], "remoteLanNetmask") == 0) {
	addr.s_addr = profL2tpA->remoteLanNetmask;
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
