#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (HOTEL_PNP == FYES)
#if (UTT_SMART_UI == FYES)
/*新风格UI  dhcpserver冲突检测*/
static int dhcpServerDetection(int eid, webs_t wp, int argc, char_t **argv)
{
    doSystem("echo 1 > /proc/sys/net/ipv4/netfilter/utt_dhcp_server_detection");
    return 0;
}
/*新风格UI  获得检测结果*/
static void formDetectionResult(webs_t wp, char_t *path, char_t *query)
{
    int ret = 0;
    FILE *fp = NULL;
    char buffer[10];

    memset(buffer, 0, 10);

    /*打开通道*/
    if ((fp = popen("cat /proc/sys/net/ipv4/netfilter/utt_dhcp_server_detection", "r")) != NULL) {

	if (fgets(buffer, sizeof(buffer), fp) != NULL)
	    ret = (int)strtol(buffer, 0, 10);/*检测到冲突，值为2*/
	pclose(fp);/*关闭通道*/
    }

    /*ajax头*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

    websWrite(wp, T("ret = %d;"), ret);

    websDone(wp, 200);/*ajax尾*/
    doSystem("echo 0 > /proc/sys/net/ipv4/netfilter/utt_dhcp_server_detection");/*结束检测*/
}
#endif
/********************************************************************
 ** 函数： aspOutHotel                                             **
 ** 功能： 即插即用页面输出                                        **
 ** 创建： 2012-10-30                                              ** 
 ** 参数： 略                                                      **
 ** 返回： 略                                                      **
 ** 输出： 略                                                      **
 ** 作者： zheng.xiao                                              ** 
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
 ** 函数： formHotel                                               **
 ** 功能： 页面保存                                                **
 ** 创建： 2012-10-30                                              ** 
 ** 参数： 略                                                      **
 ** 返回： 略                                                      **
 ** 输出： 略                                                      **
 ** 作者： zheng.xiao                                              ** 
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
    	{/*根据页面传递的下拉选项on or off来设置prof对应项的值*/
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
 ** 函数： formDefineHotel                                         **
 ** 功能： 定义即插即入接口函数                                    **
 ** 创建： 2012-10-30                                              ** 
 ** 参数： 略                                                      **
 ** 返回： 略                                                      **
 ** 输出： 略                                                      **
 ** 作者： zheng.xiao                                              ** 
 ********************************************************************/ 
void formDefineHotel(void)
{
#if (UTT_SMART_UI == FYES)
    websAspDefine(T("dhcpServerDetection"), dhcpServerDetection); /*启动dhcpserver检测*/
    websFormDefine(T("formDetectionResult"), formDetectionResult);    /*获得检测结果*/
#endif
    websAspDefine(T("aspOutHotel"), aspOutHotel); /*define asp*/  
    websFormDefine(T("formHotel"), formHotel);    /*define form*/
}
#endif

