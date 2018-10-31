#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#if (WAN_3G_SWITCH  == FYES )
/*
 * utt_3g_help - 命令帮助
 */
static void utt_3g_help(void)
{
    printf("utt3gcat v | p");
    return;
}
/*
 * main - 从脚本中获得3G卡的Vendor或者ProdID
 */
int main(int argc, char** argv)
{
    FILE *fp = NULL;
    char *p = NULL;
    int count = 0;
    char buf[256];
    char ProdID[10];
    char Vendor[10];
    char command[100];

    /*数组初始化*/
    memset(buf, 0, sizeof(buf));
    memset(ProdID, 0, sizeof(ProdID));
    memset(Vendor, 0, sizeof(Vendor));
    memset(command, 0, sizeof(command));

    /*输入参数判断*/
    if((argc != 2) || ((strcmp(argv[1], "v") != 0)
		&& (strcmp(argv[1], "p") != 0))) {
	utt_3g_help();
	return -1;
    } 

    strcpy(command, "cat /proc/bus/usb/devices");
    fp = popen(command, "r");/*执行命令*/
    if (fp != NULL) {/*执行成功*/
	while (fgets(buf, sizeof(buf), fp) != NULL) {
#if 0
	    p = strstr(buf, "Vendor=");
	    if(p != NULL) {
		count++;/*计算Vendor个数*/
		p = p + 7;/*跳到厂商名处*/
		if (count == 2) {/*第一个厂商名跳过，因为默认有一个为0的*/
		    memcpy(Vendor, p, 4);
		    p = p + 12;/*跳到产品号处*/
		    memcpy(ProdID, p, 4);
		}	  
	    }
#else
	    p = strstr(buf, "Vendor=");
	    if(p != NULL) {
		p = p + 7;/*跳到厂商名处*/
		memcpy(Vendor, p, 4);
		p = p + 12;/*跳到产品号处*/
		memcpy(ProdID, p, 4);
	    } else {
		p = strstr(buf, "Manufacturer=");
		if (p != NULL) {
		    /* Manufacturer不为ReOS 即为3G卡 */
		    if (memcmp(p + 13, "ReOS SE", 7) != 0) {
			break;
		    } else {
			memset(Vendor, 0, sizeof(Vendor));
			memset(ProdID, 0, sizeof(ProdID));
		    }
		}
	    }
#endif
	}
	pclose(fp);/*关闭通道*/
    }

    if (strcmp(argv[1], "v") == 0) {/*输出厂商名*/
	printf("%s", Vendor);
    } else if (strcmp(argv[1], "p") == 0) {/*输出产品号*/
	printf("%s", ProdID);
    } else {/*输入参数错误，给出帮助提示*/
	utt_3g_help();
    }
    return 0;	
}
#else
/*
 * main - 从脚本中获得3G卡的Vendor或者ProdID
 */
int main(int argc, char** argv)
{
    return 0;	
}
#endif
