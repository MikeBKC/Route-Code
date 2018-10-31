#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uttMachine.h"
#include "typedef.h"          
#include "mib.h"
#include "ifmisc.h"

int main(int argc, char **argv) {
    if (argc < 2) {
	exit(1);
    }

    ProfInit();

    if (strcmp(argv[1], "NATSESSIONS") == 0) {
#if defined(MAX_NAT_SESSIONS)
	printf("%d", MAX_NAT_SESSIONS);
#else
	printf("50000"); /* nat最大会话数默认配置50000 */
#endif
    }

    ProfDetach();
    exit(0);
}
