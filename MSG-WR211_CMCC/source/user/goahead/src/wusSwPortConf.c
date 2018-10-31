#include        <linux/autoconf.h>
#include	<string.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"

#if (FEATURE_SW_PORT_CONF == FYES)
#include        "sw_ioctl.h"

extern int getAggrMember(webs_t wp);
/*页面控件名称*/
static char *select_mode[24] = {"R50","R51","R52","R53","R54","R55","R56","R57", "R58", "R59", "R5a","R5b","R5c","R5d","R5e","R5f","R60","R61","R62","R63","R64","R65", "R66", "R67"};

#ifdef CONFIG_VSC7390
static char *max_frames[24] = {"R11","R12","R13","R14","R15","R16","R17", "R18", "R19", "R1a","R1b","R1c","R1d","R1e","R1f","R20","R21","R22","R23","R24","R25", "R26", "R27", "R28"};
#endif

static char *flow_control[24] = {"Rd0","Rd1","Rd2","Rd3","Rd4","Rd5","Rd6","Rd7", "Rd8", "Rd9", "Rda","Rdb","Rdc","Rdd","Rde","Rdf","Re0","Re1","Re2","Re3","Re4","Re5", "Re6", "Re7"};
static char *port_Info[24] = {"portInfo1", "portInfo2", "portInfo3", "portInfo4", "portInfo5", "portInfo6", "portInfo7", "portInfo8", "portInfo9", "portInfo10", "portInfo11", "portInfo12", "portInfo13", "portInfo14", "portInfo15", "portInfo16", "portInfo17", "portInfo18", "portInfo19", "portInfo20", "portInfo21", "portInfo22", "portInfo23", "portInfo24"};


static void formPortConfigure(webs_t wp, char_t *path, char_t *query);
int getPortConfigure(int eid, webs_t wp, int argc, char **argv);

/********************************************************************
 * 函数： fromDefinePortStatics
 * 功能： 页面与交互转换函数
 * 创建： 2010-08-16
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： jfgu
 * 维护： jfgu
 * 日期： 2010-09-01
 *******************************************************************/
void formDefinePortConfigure(void)
{
	websAspDefine(T("aspOutPortConfigure"), getPortConfigure);
	websFormDefine(T("formPortConfigure"), formPortConfigure);	
}
/********************************************************************
 * 函数： getPortConfigure
 * 功能： 页面显示函数
 * 创建： 2010-08-24
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： jfgu
 * 维护： jfgu
 * 日期： 2010-09-01
 ********************************************************************/ 
int getPortConfigure(int eid, webs_t wp, int argc, char **argv)
{
    int min = 0, max = 0;
    int i = 0;
    char m_buffer[64] = {0};

    char cur_speed[256] = {0};
#ifdef CONFIG_VSC7390
    char max_frame[256] = {0};
#endif
#if 0
    char str_protect[128] = {0};
#endif
    char speed_mode[128] = {0};
    char str_fc[128] = {0};
    char port_info[512] = {0};
#if 0
    char str_flood[128] = {0};
#endif

    MibProfileType mibType = MIB_PROF_SW_PORT_CONF;
    SwPortConfProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);

    websWrite(wp, "var numPorts = %u;\n", max);/*端口数等于最大实例数*/


    for (i = min; i < max; i++)
    {
	/*端口对应的配置索引不会变*/
	prof = (SwPortConfProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL) 
	{
	    /*模式*/
	    if (i == max - 1)
	    {
		sprintf(m_buffer, "\"%d\"", prof->port_mode );
	    }
	    else
	    {
		sprintf(m_buffer, "\"%d\",", prof->port_mode);
	    }
	    strcat(speed_mode, m_buffer);

#ifdef CONFIG_VSC7390
	    /*最大帧*/
	    if (i == max - 1)
	    {
		sprintf(m_buffer, "\"%d\"", prof->max_frame);
	    }
	    else
	    {
		sprintf(m_buffer, "\"%d\",", prof->max_frame);
	    }
	    strcat(max_frame, m_buffer);
#endif

	    /*流控*/
	    if (i == max - 1)
	    {
		sprintf(m_buffer, "%d", prof->flow_control);
	    }
	    else
	    {
		sprintf(m_buffer, "%d,", prof->flow_control);
	    }
	    strcat(str_fc, m_buffer);

#if 0
	    /*端口保护*/
	    WEB_PRINTF("port %d, protect %d\n", m_port_no, mem_shadow->port_config[(m_port_no - 1)].protect);
	    if (m_port_no == PORT_NO_END - 1)
	    {
		sprintf(m_buffer, "%d", mem_shadow->port_config[(m_port_no - 1)].protect);
	    }
	    else
	    {
		sprintf(m_buffer, "%d,", mem_shadow->port_config[(m_port_no - 1)].protect);
	    }
	    strcat(str_protect, m_buffer);

	    /*泛洪*/
	    WEB_PRINTF("port %d, flood %d\n", m_port_no, mem_shadow->port_config[(m_port_no - 1)].flooding);
	    if (m_port_no == PORT_NO_END - 1)
	    {
		sprintf(m_buffer, "%d", mem_shadow->port_config[(m_port_no - 1)].flooding);
	    }
	    else
	    {
		sprintf(m_buffer, "%d,", mem_shadow->port_config[(m_port_no - 1)].flooding);
	    }
	    strcat(str_flood, m_buffer);
#endif
	    /*端口状态*/
	    memset(m_buffer, 0, sizeof(m_buffer));
	    m_buffer[0]='\"';
	    sw_link_status_get(i+1, &m_buffer[1]);/*获取端口状态*/

	    if (i == max - 1)
	    {
		sprintf(&m_buffer[strlen(m_buffer)], "\"" );
	    }
	    else
	    {
		sprintf(&m_buffer[strlen(m_buffer)], "\",");
	    }
	    strcat(cur_speed, m_buffer);


	    /*端口描述*/
	    if (i == max - 1)
	    {
		sprintf(m_buffer, "'%s'", prof->name);
	    }
	    else
	    {
		sprintf(m_buffer, "'%s', ", prof->name);
	    }
	    strcat(port_info, m_buffer);
	}
    }
 
    websWrite(wp, "var curSpeed = new Array(%s);\n", cur_speed);
    websWrite(wp, "var speedMode = new Array(%s);\n", speed_mode);
    websWrite(wp, "var fc = new Array(%s);\n", str_fc);

#ifdef CONFIG_VSC7390
    websWrite(wp, "var maxFrame = new Array(%s);\n", max_frame);
#endif

    websWrite(wp, "var portInfo = new Array(%s); \n",  port_info);
#if 0
    websWrite(wp, "var portprotect = new Array(%s);\n", str_protect);
    websWrite(wp, "var floodings = new Array(%s); \n", str_flood); 
#else
    /*方便以后添加端口保护或泛洪时，页面需重改。故在此打印*/
    websWrite(wp, "var portprotect = new Array(%s);\n", "");
    websWrite(wp, "var floodings = new Array(%s); \n", ""); 
#endif

    if(strstr(wp->url, "ports.asp"))
    {
	getAggrMember(wp);
	getTheErrorMsg(eid, wp, argc, argv);
    }

    return 0;
}

/*******************************************************************
 *函数： formPortConfigure
 *功能： 页面提交函数
 *参数： 略
 *返回： 无
 *作者： jfgu
 *日期： 2010-8-24
 ******************************************************************/
static void formPortConfigure(webs_t wp, char_t *path, char_t *query)
{
    Uint32 is_wait=0u;
    int tmp=0;
#if 0
    unsigned char protect_var[PORT_NO_END-1u] = {0u};
    unsigned char  flooding_var[PORT_NO_END-1u] = {0u};
    char *protect = NULL;
    char *flooding = NULL;
#endif
    char *m_portInfo = NULL;
    char *m_rate_limit = NULL;
    int min = 0, max = 0, i =0;
    char tmpStr[32];

    MibProfileType mibType = MIB_PROF_SW_PORT_CONF;
    SwPortConfProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

    ProfInstLowHigh(mibType, &max, &min);


    /*发消息得到端口状态*/
#if 0
    protect = websGetVar(wp, "hiddenvalue", T(""));
    flooding = websGetVar(wp, "hiddenvalue2", T(""));
    WEB_PRINTF("port protect %s, flooding %s\n", protect, flooding);
    if ((protect != NULL) && (strcmp(protect ,"") != 0))
    {
	int_to_bit_array(protect_var, PORT_NO_END-1u, (uint32)(strtoul(protect, 0, 10)));
    }

    if ((flooding != NULL) && (strcmp(flooding, "") != 0))
    {
	int_to_bit_array(flooding_var, PORT_NO_END-1u, (uint32)(strtoul(flooding, 0, 10)));
    }


#endif 
    /*取出页面控件中的值*/
    for (i = min; i < max; i++)
    {
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);/*备份*/
	prof = (SwPortConfProfile*)ProfGetInstPointByIndex(mibType, i);

	if(prof != NULL) 
	{
	    /*取出配置模式中的值*/
	    tmp = (int)(strtoul(websGetVar(wp, select_mode[i], T("")),  NULL, 0));
	    if(prof->port_mode != tmp)
	    {
		prof->port_mode = tmp;
		is_wait = 1u;
	    }

#ifdef CONFIG_VSC7390
	    /*取出允许最大帧的值*/
	    tmp = (int)(strtoul(websGetVar(wp, max_frames[i], T("")), NULL, 0));
	    if(prof->max_frame != tmp)
	    {
		prof->max_frame = tmp;
		is_wait = 1u;
	    }
#endif

	    /*取出流控的值*/
	    m_rate_limit = websGetVar(wp, flow_control[i],  T(""));
	    /*存入共享内存中*/
	    tmp = (int)(strtoul(m_rate_limit, NULL, 0));
	    if(prof->flow_control !=tmp)
	    {
		prof->flow_control = tmp;
		is_wait = 1u;
	    }
#if 0
	    /*端口保护盒泛洪不能同时生效*/
	    if ((protect_var[(m_port_no-1)] == 1)
		    && (flooding_var[(m_port_no-1)] == 1))
	    {
		WEB_PRINTF("flooding and protect, must select only 1\n");
	    }
	    else
	    {
		/*取出端口保护的值*/
		WEB_PRINTF("port_protect %d, port %d\n", protect_var[(m_port_no-1)], m_port_no);
		mem_shadow->port_config[(m_port_no - 1)].protect = protect_var[(m_port_no-1)];

		/*flooding*/
		WEB_PRINTF("flooding %d, port %d\n", flooding_var[(m_port_no-1)], m_port_no);
		mem_shadow->port_config[(m_port_no - 1)].flooding = flooding_var[(m_port_no-1)];

	    }
#endif

	    /*端口备注*/
	    m_portInfo = websGetVar(wp, port_Info[i], T(""));

	    if (strcmp(prof->name, m_portInfo) != 0)
	    {
		strncpy(prof->name, m_portInfo, sizeof(prof->name)-1);
	    }

	    prof->head.active = 1;
	    memset(tmpStr, 0, sizeof(tmpStr));
	    sprintf(tmpStr, "port%d", i+1);
	    ProfSetNameByPoint(prof, tmpStr);
	}
    }

    /*只有当备份数据与配置不符，goahead才会发消息*/
    ProfUpdate(profhead);
    ProfFreeAllocList(profhead);
    nvramWriteCommit();

    if(is_wait > 0u)
    {
	sleep(4);/*这些操作可能导致端口重启，故需等待*/
    }

    websRedirect(wp, "switch/ports.asp");

}
#endif


