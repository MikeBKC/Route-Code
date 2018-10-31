#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * 进行数学计算工具
 * shell中进行数学计算比较麻烦，可以在此程序中进行
 */

void usage(void) {
    printf("Usage:\n\n");
    printf("mathCal is for utt Mathematical calculations.options:\n");
    printf("  -m   turn netmask to integer.input mask,output integer\n");
    printf("       eg: input 255.255.255.0, output 24\n");
}

int maskToInt(char *arg) {
    unsigned int ipInt;
    int i;
    int integer = 0;
    ipInt = ntohl(inet_addr(arg));
    for (i = 31; i >= 0; i--) {
	if ((ipInt & (1 << i)) != 0) {
	    integer++;
	} else {
	    break;
	}
    }
    return integer;
}

int main(int argc, char *argv[]) {
    int opt;
    char options[] = "m:";
    int intV;
    while ((opt = getopt(argc, argv, options)) != -1) {
	switch(opt) {
	    case 'm':
		intV = maskToInt(optarg);
		printf("%d",intV);
		break;
	    case '?':
		usage();
		break;
	    default:
		break;
	}
    }
    return 0;
}
