#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <sys/un.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/types.h> 
#include <linux/netlink.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "uttMachine.h"

#if (WIFIDOG_AUTH == FYES )
//#if (SMS_ENABLE == 1)

#define SWORD_DEBUG 1
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define UEVENT_BUFFER_SIZE 4096

/*
 * init_hotplug_sock - ��ʼ���Ȳ��socket
 *
 * ��������Ҫ��ȡproc�ļ��µ�usb״̬�鿴�Ƿ���3G������
 * Ȼ��ͨ���ж��Ƿ��в��Ž������ж�3G����״̬
 *
 * ����ֵ��
 * sock_fd   -     socket������
 */
static int init_hotplug_sock(void) 
{ 
    struct sockaddr_nl snl; 
    int buffersize = UEVENT_BUFFER_SIZE / 4; 
    int sock_fd; 

    bzero(&snl, sizeof(struct sockaddr_nl)); 
    snl.nl_family = AF_NETLINK; 
    snl.nl_pid = getpid(); 
    snl.nl_groups = 1; 

    /*����socket*/
    sock_fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT); 
    if (sock_fd == -1) {
	perror("socket error !\n"); 
	return -1; 
    } 

    /*���ý��ջ�����*/
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, 
		(const char *)&buffersize, sizeof(int)) == -1) {
	perror("setsockopt error !\n"); 
	return -1;
    }

    /*��socket*/
    if (bind(sock_fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) == -1) {
	perror("bind error !\n"); 
	close(sock_fd); 
	return -1; 
    } 

    return sock_fd; 
}


/*
 * utt_3g_cat - ���豸�ļ��ж�ȡ3G���������Ͳ�Ʒ��
 *
 * ��������Ҫ��ȡproc�ļ��µ�usb״̬�鿴�Ƿ���3G������
 * Ȼ��ͨ���ж��Ƿ��в��Ž������ж�3G����״̬
 * ������ʶ��Ŀ����򽫳������Ͳ�Ʒ�Ŵ������������
 *
 * ���������
 *  vendor - ������
 *  prodid - �豸id
 * ����ֵ��
 *  0     3G��δ�����ʶ��
 *  1     �����ܹ�ʶ���3G��
 */
int utt_3g_cat(char *vendor ,char *prodid)
{
    FILE *fp = NULL;
    char *p = NULL;
    int count = 0, ret = 0;
    char buffer[256];
    char command[100];

    /*�����ʼ��*/
    memset(buffer, 0, sizeof(buffer));
    memset(command, 0, sizeof(command));

    strcpy(command,"cat /proc/bus/usb/devices");
    fp = popen(command,"r");/*ִ������*/
    if (fp != NULL) {/*ִ�гɹ�*/
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
#if(KERNEL_2_6_36 == FYES)
	    p = strstr(buffer, "Vendor=");
	    if(p != NULL) {
		p = p + 7;/*������������*/
		memcpy(vendor, p, 4);
		p = p + 12;/*������Ʒ�Ŵ�*/
		memcpy(prodid, p, 4);
	    } else {
		p = strstr(buffer, "Manufacturer=");
		if (p != NULL) {
		    /* Manufacturer��ΪReOS ��Ϊ3G�� */
		    if (memcmp(p + 13, "ReOS SE", 7) != 0) {
			ret = 1;
			break;
		    } else {
			memset(vendor, 0, 10);
			memset(prodid, 0, 10);
		    }
		}
	    }
#else
	    p = strstr(buffer,"Vendor=");
	    if(p != NULL) {
		count++;/*����Vendor����*/
		p = p + 7;/*������������*/
		if(count == 2) {/*��һ����������������ΪĬ����һ��Ϊ0��*/
		    memcpy(vendor, p, 4);
		    p= p + 12;/*������Ʒ�Ŵ�*/
		    memcpy(prodid, p, 4);
		}	  
	    }
#endif
	}
	pclose(fp);
    }

#if(KERNEL_2_6_36 == FYES)
#else
    if(count != 1)
    {
	ret = 1;
    }
#endif
    return ret;
}
#if 0
#define MAX_PS_STR_LEN 2000
/*
 * test_comgt_sleep - ɱpppd����ǰ����Ƿ���3G���Ž���
 *
 * ������3G����ʱɱ���˽��̻ᵼ��goahead�Ƚ�����������
 * �ڷ�����3G���ź�˯��3�����ɱpppd����
 *
 * ����ֵ��
 *	0	-	û��3G���Ž�����
 *	1	-	���в��Ž���
 *
 */
static int test_comgt_sleep(void)
{
	FILE *fp;
	char *p = NULL;
	char ps_str[MAX_PS_STR_LEN + 1];
	int ret = 0;

	fp = popen("ps", "r");/*ִ������*/
	if (fp != NULL) {/*ִ�гɹ�*/
		memset(ps_str, 0, MAX_PS_STR_LEN + 1);
		fread(ps_str, 1, MAX_PS_STR_LEN, fp);
		/*����ʱ3G���ڲ��ţ���˯�ߵȴ�3�����ɱpppd���̣�
		 * ����������mantis11609��bug*/
		SWORD_PRINTF("%s-%d:ps_str = \n%s\n\n", __func__, __LINE__, ps_str);
		if ((p = strstr(ps_str, "/bin/comgt -d")) != NULL) { 
			SWORD_PRINTF("%s-%d:��3G�������ڲ��ţ�˯��3��\n", __func__, __LINE__);
			ret = 1;
			sleep(3u);
		}
		pclose(fp);
	}
	return ret;
}
#endif
/*
 * kill_smssend - 3G���γ���ɱpppd����
 *
 * �������������Զ�����Ȳ�Σ����û�б�Ҫ��3G
 * ���γ����������
 *
 */
static void kill_smssend(void)
{
	FILE *fp;
	char *p = NULL;
	char command[20];
	char buffer[50];
	char pid[10];

#if 0
	/*ֱ��û��3G���Ž��̲�ɱpppd����*/
	while(test_comgt_sleep())
		;
#endif
	fp = popen("ps", "r");/*ִ������*/
	if (fp != NULL) {/*ִ�гɹ�*/
		memset(buffer, 0, 50);
		while (fgets(buffer, sizeof(buffer), fp) != NULL) {
			if ((p = strstr(buffer,"smssend")) != NULL) {
				memset(pid, 0, 10);
				memset(command, 0, 20);
				sscanf(buffer, "%s", pid);
				SWORD_PRINTF("%s-%d:pid = %s\n", __func__, __LINE__, pid);
				sprintf(command, "kill -9 %s", pid);
				SWORD_PRINTF("%s-%d:command = %s\n", __func__, __LINE__, command);
				system(command);
				SWORD_PRINTF("%s-%d:command = %s\n", __func__, __LINE__, command);
			}
		}
		pclose(fp);
	}
	return;
}
/*
 * auto_dial_3g - 3G���Զ�����
 *
 * ���˽����յ������ں�ͨ��netlink���������ݺ�ʼ�Զ�����
 *
 * ���������
 *  Vendor_last - �ϴα��ݵĳ�����
 *  ProdID_last - �ϴα��ݵĲ�Ʒ��
 *
 */
static void auto_dial_3g(char *Vendor_last, char *ProdID_last)
{
    int ret = -1;
    char ProdID[10];
    char Vendor[10];

    /*�����ʼ��*/
    memset(Vendor, 0, sizeof(Vendor));
    memset(ProdID, 0, sizeof(ProdID));

    /*��ȡ���̺źͲ�Ʒ��*/
    ret = utt_3g_cat(Vendor ,ProdID);	

    SWORD_PRINTF("%s-%d:Vendor_last = %s, ProdID_last = %s\n", 
	    __func__, __LINE__, Vendor_last, ProdID_last);
    SWORD_PRINTF("%s-%d:Vendor = %s, ProdID = %s, ret = %d\n", 
	    __func__, __LINE__, Vendor, ProdID, ret);

    if((ret == 1) && (*Vendor != '\0') && (*ProdID != '\0') 
	    &&((strcmp(Vendor_last,Vendor) != 0) 
		|| (strcmp(ProdID_last,ProdID) != 0))) {
	/*����һ�μ�⵽�ĳ��������Ʒ�Ų�ͬ��ʼ����*/
	sleep(3u);/*����ǰ˯��һ�£��ȴ��ں˼�����*/
	system("/sbin/usb_modeswitch.sh");/*ִ�в��Žű�*/
	if(utt_3g_cat(Vendor ,ProdID) == 1) {
	    /*�����ϴβ��ŵĳ������Ͳ�Ʒ��*/
	    SWORD_PRINTF("%s-%d:Vendor = %s, ProdID = %s\n",
		    __func__, __LINE__, Vendor, ProdID);
	    strcpy(Vendor_last,Vendor);
	    strcpy(ProdID_last,ProdID);
	}
    } else if((ret == 0) && (*Vendor == '\0') && (*ProdID == '\0')
	    && (*Vendor_last != '\0') && (*ProdID_last != '\0')) { 
	/*usb�γ�ʱɱ�����Ž��̲���ձ���*/
	SWORD_PRINTF("%s-%d:usb has been removed !\n",
		__func__, __LINE__);
	kill_smssend();
	memset(Vendor_last, 0, sizeof(Vendor_last));
	memset(ProdID_last, 0, sizeof(ProdID_last));
	sleep(2u);/*ɱ�����̺�˯��һ�£��ȴ��ں˴�����usb�γ�*/
    } else {
	/*test*/
    }
    return;
}

/*
 * main - 3G���Զ���Ⲧ��������
 *
 * ���û�����3G���ź����ô˽��̼��3G�����Ȳ��
 * ��3G��������ʼ����
 *
 */
int main(int argc, char* argv[]) 
{ 
    int hotplug_sock; 
    fd_set fds;
    int select_ret = -1;
    char buf[UEVENT_BUFFER_SIZE];
    char ProdID_last[10];
    char Vendor_last[10];

    /*�����ʼ��*/
    memset(ProdID_last, 0, sizeof(ProdID_last));
    memset(Vendor_last, 0, sizeof(Vendor_last));
    memset(buf, 0, UEVENT_BUFFER_SIZE);

    hotplug_sock = init_hotplug_sock();
    if (hotplug_sock == -1) {
	return -1;
    }
    utt_3g_cat(Vendor_last ,ProdID_last);
    while (1) {
	printf("%s-%d: =====\n",__func__,__LINE__);
	FD_ZERO(&fds);/*���fds����*/
	FD_SET(hotplug_sock, &fds);/*��hotplug_sock����fds����*/
	select_ret = select(hotplug_sock + 1, &fds, NULL, NULL, NULL);
	if (select_ret == -1) {/*selectʧ��*/
	    printf("select failed !!!\n");
	    return -1;			
	}
	if (select_ret == 0) {/*select��ʱ*/
	    sleep(1u);
	}
	if (FD_ISSET(hotplug_sock, &fds)) {/*���hotplug_sock�Ƿ���fds������*/
	    recv(hotplug_sock, &buf, sizeof(buf), 0); 
	}
	if (*buf != '\0') {/*���յ�������*/
	    SWORD_PRINTF("%s-%d:recive :%s\n", __func__, __LINE__, buf);
	    if (strstr(buf, "usb") != NULL) {
		    auto_dial_3g(Vendor_last, ProdID_last);
	    }
	}
	memset(buf, 0, sizeof(buf));
	sleep(3);
    }
    return 0; 
}
//#endif
#else
/*
 * main - 3G���Զ���Ⲧ��������
 *
 * ���û�����3G���ź����ô˽��̼��3G�����Ȳ��
 * ��3G��������ʼ����
 *
 */
int main(int argc, char** argv)
{
    return 0;
}
#endif
