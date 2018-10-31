#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (HOTEL_PNP == FYES)
#if (UTT_SMART_UI == FYES)
/*�·��UI  dhcpserver��ͻ���*/
static int dhcpServerDetection(int eid, webs_t wp, int argc, char_t **argv)
{
    doSystem("echo 1 > /proc/sys/net/ipv4/netfilter/utt_dhcp_server_detection");
    return 0;
}
/*�·��UI  ��ü����*/
static void formDetectionResult(webs_t wp, char_t *path, char_t *query)
{
    int ret = 0;
    FILE *fp = NULL;
    char buffer[10];

    memset(buffer, 0, 10);

    /*��ͨ��*/
    if ((fp = popen("cat /proc/sys/net/ipv4/netfilter/utt_dhcp_server_detection", "r")) != NULL) {

	if (fgets(buffer, sizeof(buffer), fp) != NULL)
	    ret = (int)strtol(buffer, 0, 10);/*��⵽��ͻ��ֵΪ2*/
	pclose(fp);/*�ر�ͨ��*/
    }

    /*ajaxͷ*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

    websWrite(wp, T("ret = %d;"), ret);

    websDone(wp, 200);/*ajaxβ*/
    doSystem("echo 0 > /proc/sys/net/ipv4/netfilter/utt_dhcp_server_detection");/*�������*/
}
#endif
/********************************************************************
 ** ������ aspOutHotel                                             **
 ** ���ܣ� ���弴��ҳ�����                                        **
 ** ������ 2012-10-30                                              ** 
 ** ������ ��                                                      **
 ** ���أ� ��                                                      **
 ** ����� ��                                                      **
 ** ���ߣ� zheng.xiao                                              ** 
 ********************************************************************/ 
static int aspOutHotel(int eid, webs_t wp, int argc, char_t **argv)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    if((profSys != NULL)) 
    {
    	if (profSys->hotelPnpEn == FUN_ENABLE) 
    	{
    	    websWrite(wp, "var hotelPnpEn=\"on\";");
    	}
    	else
    	{
	    websWrite(wp, "var hotelPnpEn=\"off\";");
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif
    return 0;
}
/********************************************************************
 ** ������ formHotel                                               **
 ** ���ܣ� ҳ�汣��                                                **
 ** ������ 2012-10-30                                              ** 
 ** ������ ��                                                      **
 ** ���أ� ��                                                      **
 ** ����� ��                                                      **
 ** ���ߣ� zheng.xiao                                              ** 
 ********************************************************************/ 
static void formHotel(webs_t wp, char_t *path, char_t *query)
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    struProfAlloc *profhead = NULL;
    char *enable=NULL;
    enable=websGetVar(wp,"hotelEnable",T(""));
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((profSys != NULL))    
    {
    	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
    	if (strcmp(enable, "on") == 0) 
    	{/*����ҳ�洫�ݵ�����ѡ��on or off������prof��Ӧ���ֵ*/
       	    profSys->hotelPnpEn = FUN_ENABLE;
    	}
    	else
    	{
	    profSys->hotelPnpEn = FUN_DISABLE;
        }
    	ProfUpdate(profhead);
    	ProfFreeAllocList(profhead);
    	nvramWriteCommit();
    }
#if (UTT_SMART_UI == FYES)
     setSuccessTips();
    websRedirect(wp, "hotel_Sui.asp");
#else
    websRedirect(wp, "hotel.asp");
#endif

    return;
}
/********************************************************************
 ** ������ formDefineHotel                                         **
 ** ���ܣ� ���弴�弴��ӿں���                                    **
 ** ������ 2012-10-30                                              ** 
 ** ������ ��                                                      **
 ** ���أ� ��                                                      **
 ** ����� ��                                                      **
 ** ���ߣ� zheng.xiao                                              ** 
 ********************************************************************/ 
void formDefineHotel(void)
{
#if (UTT_SMART_UI == FYES)
    websAspDefine(T("dhcpServerDetection"), dhcpServerDetection); /*����dhcpserver���*/
    websFormDefine(T("formDetectionResult"), formDetectionResult);    /*��ü����*/
#endif
    websAspDefine(T("aspOutHotel"), aspOutHotel); /*define asp*/  
    websFormDefine(T("formHotel"), formHotel);    /*define form*/
}
#endif

