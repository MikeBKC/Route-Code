#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"


#if (UPNP_GLOBAL_ENABLE == FYES)

struct portMap
{
  int m_PortMappingEnabled;
  long int m_PortMappingLeaseDuration;
  char m_RemoteHost[16];
  char m_ExternalPort[6];
  char m_InternalPort[6];
  char m_PortMappingProtocol[4];
  char m_InternalClient[16];
  char m_PortMappingDescription[50];

  int expirationEventId;
  long int expirationTime;

  struct portMap* next;
  struct portMap* prev;
} *pmlist_Head, *pmlist_Tail, *pmlist_Current;
/***********************************************************************
 * 函 数 名：   getUpnpTableAndConfig 
 * 功能描述：   返回upnp列表和配置
 * 输入参数:    eid,wp,argc,argv
 * 输出参数：	
 * 返回值：	    0 
 * 创建日期：	2011-10-28
 * 修改日期：
 * 作者：       yqyang	
 * 附加说明：	无
***********************************************************************/
static int getUpnpTableAndConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    FILE *fp;
    int fileLen, structLen,totalrece = 0;
    struct portMap tmp;
    int i;
 #if(UTT_SMART_UI==FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif
    fp = fopen("/tmp/upnp","rb");
    structLen = sizeof(struct portMap);
    websWrite(wp,"var upnpIntIp=new Array();");
    websWrite(wp,"var upnpRemoteIp=new Array();");
    websWrite(wp,"var upnpIntPort=new Array();");
    websWrite(wp,"var upnpExtPort=new Array();");
    websWrite(wp,"var upnpProt=new Array();");
    websWrite(wp,"var description=new Array();");
    websWrite(wp,"var upnpEnables=\"%s\";", "0");
    char *upnpIntIp = T("upnpIntIp[%d] = \"%s\"; ");
    char *upnpRemoteIp = T("upnpRemoteIp[%d] = \"%s\"; ");
    char *upnpIntPort = T("upnpIntPort[%d] = \"%s\"; ");
    char *upnpExtPort = T("upnpExtPort[%d] = \"%s\"; ");
    char *upnpProt = T("upnpProt[%d] = \"%s\"; ");
    char *upnpDesc = T("description[%d] = \"%s\"; ");
    int back = 0;
    for(i = 1; i <= getWanSlotCount(); i++) {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	if(profIf->UpnpActive == 1){
	    if(back == 0){
		websWrite(wp,"upnpEnables=\"%s\";", "1");
		back = 1;
	    }
	}
    }
    if(fp) {
        fseek(fp, 0, SEEK_END);
        fileLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        while(fileLen >= structLen)
        {
            fread(&tmp, structLen, 1, fp);

            websWrite(wp,upnpIntIp,totalrece,tmp.m_InternalClient);
            websWrite(wp,upnpRemoteIp,totalrece,tmp.m_RemoteHost);
            websWrite(wp,upnpIntPort,totalrece,tmp.m_InternalPort);
            websWrite(wp,upnpExtPort,totalrece,tmp.m_ExternalPort);
            websWrite(wp,upnpProt,totalrece,tmp.m_PortMappingProtocol);
            websWrite(wp,upnpDesc,totalrece,tmp.m_PortMappingDescription);
            fileLen -= structLen;
            totalrece++;
        }

    }else {
        websWrite(wp,"var totalrecs = %d;",totalrece);
        return 0;
    }
    websWrite(wp,"var totalrecs = %d;",totalrece);
    fclose(fp);
    return 0;
}
/***********************************************************************
 * 函 数 名：   formConfigUpnp
 * 功能描述：   upnp全局控制函数
 * 输入参数:    wp,path,query
 * 输出参数：	
 * 返回值：	    
 * 创建日期：	2011-10-28
 * 修改日期：
 * 作者：       yqyang	
 * 附加说明：	无
***********************************************************************/
void formConfigUpnp(webs_t wp, char_t *path, char_t *query)
{
	char *upnp_en;
	char *upnpEnbleOld;
	MibProfileType mibType = MIB_PROF_INTERFACE;
#if 0
	struProfAlloc *profhead = NULL;
	int back = 0;
#endif
	InterfaceProfile *profIf = NULL;
	upnp_en = websGetVar(wp, T("upnpEnblew"), T(""));
	upnpEnbleOld = websGetVar(wp, T("upnpEnbleOld"), T(""));
	if(strcmp(upnp_en,upnpEnbleOld) != 0){
	    int i;
	    for(i = 1; i <= getWanSlotCount(); i++) {
#if 0
		if(back == 0){
		    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);
		    back = 1;
		}
#endif
		profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
		profIf->UpnpActive = (int)strtol((const char*)upnp_en, (char**)NULL, 10);
	    }
	    doSystem("upnp.sh");
	    //	ProfUpdate(profhead);
	    //	ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
#if (UTT_SMART_UI == FYES)
	setSuccessTips();
	websRedirect(wp, "UPnP_Sui.asp");
#else
	websRedirect(wp, "UPnP.asp");
#endif
}
void formDefineUpnpConfig(void)
{
    websFormDefine(T("formConfigUpnp"), formConfigUpnp);
    websAspDefine(T("aspOutUpnpTableAndConfig"),getUpnpTableAndConfig);

}
#endif
