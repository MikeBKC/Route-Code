#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#if (WAN_3G_SWITCH  == FYES )
/*
 * utt_3g_help - �������
 */
static void utt_3g_help(void)
{
    printf("utt3gcat v | p");
    return;
}
/*
 * main - �ӽű��л��3G����Vendor����ProdID
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

    /*�����ʼ��*/
    memset(buf, 0, sizeof(buf));
    memset(ProdID, 0, sizeof(ProdID));
    memset(Vendor, 0, sizeof(Vendor));
    memset(command, 0, sizeof(command));

    /*��������ж�*/
    if((argc != 2) || ((strcmp(argv[1], "v") != 0)
		&& (strcmp(argv[1], "p") != 0))) {
	utt_3g_help();
	return -1;
    } 

    strcpy(command, "cat /proc/bus/usb/devices");
    fp = popen(command, "r");/*ִ������*/
    if (fp != NULL) {/*ִ�гɹ�*/
	while (fgets(buf, sizeof(buf), fp) != NULL) {
#if 0
	    p = strstr(buf, "Vendor=");
	    if(p != NULL) {
		count++;/*����Vendor����*/
		p = p + 7;/*������������*/
		if (count == 2) {/*��һ����������������ΪĬ����һ��Ϊ0��*/
		    memcpy(Vendor, p, 4);
		    p = p + 12;/*������Ʒ�Ŵ�*/
		    memcpy(ProdID, p, 4);
		}	  
	    }
#else
	    p = strstr(buf, "Vendor=");
	    if(p != NULL) {
		p = p + 7;/*������������*/
		memcpy(Vendor, p, 4);
		p = p + 12;/*������Ʒ�Ŵ�*/
		memcpy(ProdID, p, 4);
	    } else {
		p = strstr(buf, "Manufacturer=");
		if (p != NULL) {
		    /* Manufacturer��ΪReOS ��Ϊ3G�� */
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
	pclose(fp);/*�ر�ͨ��*/
    }

    if (strcmp(argv[1], "v") == 0) {/*���������*/
	printf("%s", Vendor);
    } else if (strcmp(argv[1], "p") == 0) {/*�����Ʒ��*/
	printf("%s", ProdID);
    } else {/*����������󣬸���������ʾ*/
	utt_3g_help();
    }
    return 0;	
}
#else
/*
 * main - �ӽű��л��3G����Vendor����ProdID
 */
int main(int argc, char** argv)
{
    return 0;	
}
#endif
