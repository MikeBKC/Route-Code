#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <string.h>
#include <assert.h>
#include <net/if.h>
#include <linux/sockios.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "uttfunction.h"

#if (CAPWAP_CONFIG_AP == FYES)
#define CAPWAP_AP_SPEEDWEB_PID_FILE "/var/run/speedweb.pid"
#define CAPWAP_AP_CONFIG_FILE "/etc/config.ap"
#define CAPWAP_AP_SET_FILE "/etc/settings.ap"
int writeSpeedwebPid(void)
{
    FILE *fp; 

    fp = fopen(CAPWAP_AP_SPEEDWEB_PID_FILE, "w+");
    if (NULL == fp) 
    {
	return (-1);
    }    
    fprintf(fp, "%d", getpid());
    fclose(fp);
    return 0;
}

static void capwap_int_hander(int signo)
{
    FILE *fp = NULL;
    char buff[MAX_CAPWAP_AC_HOSTNAME+1]= "";
    ApCapwapConfigProfile *profac = NULL;
    MibProfileType mibTypeac = MIB_PROF_AP_CAPWAP_CONFIG;
    int i = 0;
    static int capwap_ap_init_flag = 0;

    if (signo == SIGUSR1)
    {
	/*收到更新文件的*/
	fp = fopen("/tmp/ac_addr", "r");
	if (fp != NULL)
	{
	    profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(mibTypeac, 0);
	    if (profac != NULL)
	    {
		while ((fgets(buff, sizeof(buff), fp) != NULL) && (i < 3))
		{
		    if ('\n' == buff[strlen(buff) - 1])
		    {
			buff[strlen(buff) - 1] = '\0';
		    }
		    profac->head.active = TRUE;
		    strncpy(profac->acHostInfo[i], buff, MAX_CAPWAP_AC_HOSTNAME);
		    i++;
		}
	    }
	    ap_set_capwap_config();
	    fclose(fp);
	    /*获取到ac地址*/
	    if (0 == capwap_ap_init_flag )
	    {
		doSystem("capwap_ap.sh 2 &");
		capwap_ap_init_flag = 1;
	    }
	}
	else
	{
	    /*未获取到ac地址*/
	    if (0 == capwap_ap_init_flag)
	    {
		doSystem("capwap_ap.sh 0 &");
		capwap_ap_init_flag = 1;
	    }
	}
    }
}

void ap_capwap_signal_init()
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);   
    act.sa_flags = 0;    
    act.sa_handler = capwap_int_hander;
    sigaction(SIGUSR1 , &act, NULL);
}


static int ap_set_capwap_config()
{
    FILE *fp_c = NULL, *fp_s = NULL;
    int i = 0, ret = -1, found=0;
    ApCapwapConfigProfile *profac = NULL;
    MibProfileType mibTypeac = MIB_PROF_AP_CAPWAP_CONFIG;
    profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(mibTypeac,0);
#if 0
    ApBasicConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType,0);
#endif

    if (profac != NULL) 
    {
	fp_c = fopen(CAPWAP_AP_CONFIG_FILE, "w");
	if (fp_c != NULL);
	{
#if 0
	    fputs("<AC_ADDRESSES>\n", fp_c);
//	    fputs("localhost\n", fp_c);
	    /*增加AC的地址信息*/
	    for (i = 0; i < 3; i++)
	    {
//		printf("%s, ac_addr%d:%s\n", __func__, i, profac->acHostInfo[i]);
		if (profac->acHostInfo[i][0] != '\0')
		{
		    fprintf(fp_c, "%s\n", profac->acHostInfo[i]);
		}
	    }
	    fputs("localhost\n", fp_c);
	    fputs("</AC_ADDRESSES>\n", fp_c);
#if (AP_SET_HOST_NAME == FYES)
	    if((prof != NULL) && (strcmp(prof->dev_name,"") != 0))
	    {
		fprintf(fp_c, "</WTP_NAME>%s\n", prof->dev_name);
	    }
	    else
	    {
		fprintf(fp_c, "</WTP_NAME>%s\n", "UTT");
	    }
#else
	    fprintf(fp_c, "</WTP_NAME>My WTP 1");
#endif
	    fputs("</WTP_LOCATION>  Next to Fridge\n", fp_c);
	    fputs("</WTP_LEV3_PROTOCOL> IPv4\n", fp_c);
//	    fputs("</WTP_FORCE_MTU> 500\n", fp_c);
	    fputs("</AC_LOG_FILE_ENABLE> 1\n", fp_c);
	    fputs("</AC_LOG_FILE_SIZE> 30000\n", fp_c);
#endif
	    fprintf(fp_c, "<AC_ADDRESSES>\n");
	    for (i = 0; i < 3; i++)
	    {
		if (profac->acHostInfo[i][0] != '\0')
		{
		    fprintf(fp_c, "%s\n", profac->acHostInfo[i]);
                    found = 1;
		}
	    }
	    if((profac->head.active == FALSE) || (found==0)) {
	        fprintf(fp_c, "127.0.0.1\n");/*以免无法启动apd*/
	    }
	    fprintf(fp_c,"</AC_ADDRESSES>\n</WTP_NAME>UTT AP\n</WTP_LOCATION>Next to Fridge\n</WTP_LEV3_PROTOCOL> IPv4\n</AC_LOG_FILE_ENABLE> 1\n</AC_LOG_FILE_SIZE> 30000\n");
	    fclose(fp_c);
	}

	fp_s = fopen(CAPWAP_AP_SET_FILE, "w");
	if (fp_s != NULL)
	{
	    fputs("<IF_NAME>    br0\n", fp_s);
	    fclose(fp_s);
	}

	if(profac->head.active == TRUE)
	{
	    doSystem("echo 'nameserver %N' > /etc/resolv.conf",profac->pd);
	    doSystem("echo 'nameserver %N' >> /etc/resolv.conf",profac->sd);
        }

	ret = 0;
    }
    
    return ret;
}

void CapwapConfigProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const  ApCapwapConfigProfile* oldProfileDataPtr, const ApCapwapConfigProfile* newProfileDataPtr )
{
    int ret = 0;
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    break;
	case PROF_CHANGE_EDIT:
	    ret = ap_set_capwap_config();
	    if (ret != 0)
	    {
		doSystem("capwap_ap.sh 0 &");
	    }
	    else
	    {
		doSystem("capwap_ap.sh 2 &");
	    }
	    break;
	case PROF_CHANGE_DEL:
	    break;
	case PROF_CHANGE_DELALL:
	    break;
	default:
	    break;
    };
    return;
}

void funInitCapwapConfig(void)
{
    registerForProfileUpdates(MIB_PROF_AP_CAPWAP_CONFIG, (Pointer)NULL, (ProfileUpdateFnc)CapwapConfigProfileChange);
}

#endif

#if (CAPWAP_CONFIG_AC == FYES)
#define CAPWAP_AC_CONFIG_FILE "/etc/config.ac"
#define CAPWAP_AC_SET_FILE "/etc/settings.ac"

int ac_set_capwap_config()
{
    FILE *fp_c = NULL, *fp_s = NULL;
    char_t *tmp = NULL;
    struct utsname utsbuf;
    
    fp_c = fopen(CAPWAP_AC_CONFIG_FILE, "w");
    if (fp_c != NULL)
    {
#if 0
	fprintf(fp_c, "</AC_HW_VERSION> %s\n", UTT_HWMODEL);

	uname(&utsbuf);
	tmp = strtok(utsbuf.version, " ");
	tmp = strtok(NULL, " ");
	fprintf(fp_c, "</AC_SW_VERSION> %s%s\n", utsbuf.version, tmp);

	fprintf(fp_c, "</AC_MAX_STATIONS> %d\n", 200);
//	fprintf(fp_c, "</MAX_AP_MANAGED> %d\n", MAX_AP_MANAGED);
	fprintf(fp_c, "</AC_SECURITY> X509_CERTIFICATE\n");
	fprintf(fp_c, "</AC_LEV3_PROTOCOL> IPv4\n");
	fprintf(fp_c, "</AC_NAME> My AC\n");
	fprintf(fp_c, "<AC_MCAST_GROUPS> \n");
	fprintf(fp_c, "</AC_MCAST_GROUPS> \n");
	fprintf(fp_c, "</AC_LOG_FILE_ENABLE> %d\n", 1);
	fprintf(fp_c, "</AC_LOG_FILE_SIZE> %d\n", 3000);
#endif
	fprintf(fp_c, "</AC_HW_VERSION> 1235656\n</AC_SW_VERSION> 3256984\n</AC_MAX_STATIONS> 200\n</AC_MAX_WTPS> 15\n</AC_SECURITY> X509_CERTIFICATE\n</AC_LEV3_PROTOCOL> IPv4\n</AC_NAME> My AC\n<AC_MCAST_GROUPS>\n</AC_MCAST_GROUPS>\n</AC_LOG_FILE_ENABLE> 1\n</AC_LOG_FILE_SIZE> 30000\n");
	fclose(fp_c);
    }

    fp_s = fopen(CAPWAP_AC_SET_FILE, "w");
    if (fp_s != NULL)
    {
	fprintf(fp_s, "<CWMIN_VOICE> %d\n", 2 ); 
	fprintf(fp_s, "<CWMAX_VOICE> %d\n", 3 ); 
	fprintf(fp_s, "<AIFS_VOICE> %d\n", 1 );
	fprintf(fp_s, "#\n");
	fprintf(fp_s, "#\n");
	fprintf(fp_s, "<CWMIN_VIDEO> %d\n", 3); 
	fprintf(fp_s, "<CWMAX_VIDEO> %d\n", 4); 
	fprintf(fp_s, "<AIFS_VIDEO> %d\n", 1); 
	fprintf(fp_s, "#\n");
	fprintf(fp_s, "#\n");
	fprintf(fp_s, "<CWMIN_BEST_EFFORT> %d\n", 3); 
	fprintf(fp_s, "<CWMAX_BEST_EFFORT> %d\n", 10); 
	fprintf(fp_s, "<AIFS_BEST_EFFORT> %d\n", 2); 
	fprintf(fp_s, "#\n");
	fprintf(fp_s, "#\n");
	fprintf(fp_s, "<CWMIN_BACKGROUND> %d\n", 4); 
	fprintf(fp_s, "<CWMAX_BACKGROUND> %d\n", 10); 
	fprintf(fp_s, "<AIFS_BACKGROUND> %d\n", 7); 

	fclose(fp_s);
    }
    
    doSystem("capwap_ap.sh 3 &");
    return 0;
}
#endif

#if (CAPWAP_CONFIG_AP == FYES) || ( CAPWAP_CONFIG_AC == FYES)
void initCapwap()
{
#if (CAPWAP_CONFIG_AP == FYES)
    ApCapwapConfigProfile *profac = NULL;
    MibProfileType mibTypeac = MIB_PROF_AP_CAPWAP_CONFIG;
    InterfaceProfile *profIf = NULL;
    
    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE , 1);
    if (profIf != NULL)
    {
	if ((profIf->ether.connMode == ETH_CONN_STATIC) && (profIf->ether.sta.ip == htonl(0xc0a801fd)))
	{
	    /*如果是默认的条件，则设置为动态*/
	    profIf->ether.connMode = ETH_CONN_DHCP;
	}
    }

    profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(mibTypeac,0);
    if (profac != NULL)
    {
	ap_set_capwap_config();
	if (profac->head.active == TRUE)
	{
	    /*有AC地址数据*/
	//    ap_set_capwap_config();
	    doSystem("capwap_ap.sh 1 &");
	}
	else
	{
           doSystem("capwap_ap.sh 0 &");
	    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE , 1);
	    if( (profIf != NULL) && (profIf->ether.connMode == ETH_CONN_DHCP)) 
	    {
		writeSpeedwebPid();
		ap_capwap_signal_init();
		/*通过DHCP获取到地址后启动WTP urcp*/
	    }
#if 0
	    else
	    {
		/*内网发现*/
		doSystem("capwap_ap.sh 0 &");
	    }
#endif
	}
    }
#endif
#if (CAPWAP_CONFIG_AC == FYES)
    ac_set_capwap_config();
#endif
}
#if (CAPWAP_CONFIG_AP == FYES)
void set_capwap_dns(void)
{
    ApCapwapConfigProfile *profac = NULL;

    profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_CAPWAP_CONFIG,0);

    if ((profac != NULL) && (profac->head.active == TRUE))
    {
	if(profac->pd != 0){
            doSystem("echo 'nameserver %N' >> /etc/resolv.conf",profac->pd);
	}
	if(profac->sd != 0){
	    doSystem("echo 'nameserver %N' >> /etc/resolv.conf",profac->sd);
	}
    }
}
#endif
#endif

