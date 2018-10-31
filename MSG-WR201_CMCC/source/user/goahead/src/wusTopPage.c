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
#endif
static int get_wan_port_link_status(int wannum);
static void formStatForTopImage(webs_t wp, char_t *path, char_t *query);
int getTopPageInfo(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
void formDefineTopPage(void)
{
	websAspDefine(T("aspOutTopPageInfo"), getTopPageInfo);
	websFormDefine(T("formStatForTopImage"),formStatForTopImage);
}
/********************************************************************
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou 
 ********************************************************************/ 
int getTopPageInfo(int eid, webs_t wp, int argc, char **argv)
{
#ifdef CONFIG_SWITCH_EXT
    websWrite(wp, T("var wanIfCount=%d;"), getWanSlotCount());/*wan����*/
    websWrite(wp, "var numPorts = %u;\n", PORT_COUNT);/*lan����*/
#else
    /*ͨ��wan lan�ڶ�����Ϊ��������ʾ����ͼƬ*/
    websWrite(wp, T("var wanIfCount=%d;"), 0);/*wan����*/
    websWrite(wp, "var numPorts = %u;\n", 0);/*lan����*/
#endif
#ifdef FEATURE_AC
    websWrite(wp, "var dispUser = 1;\n");/*lan����*/
#else
    websWrite(wp, "var dispUser = 0;\n");/*lan����*/
#endif

    return 0;
}
/*
 * Function: get_wan_port_link_status
 *
 * Description: get the WAN port1 ~ port4 link status
 *
 * History: 2011.10.09	Chen.Shiyu created
 */
static int get_wan_port_link_status(int wannum)
{
    int rc, ret = 0;
    FILE *fp;
    char buf[1024];
    unsigned int flag=0U;
    char *pos;

    if((fp = popen("switch reg r 80", "r")) != NULL)
    {
	/* read the file value to buf */
	rc = fread(buf, 1, 1024, fp);
	pclose(fp);

	if(rc != -1)
	{
	    if((pos = strstr(buf, "switch reg read offset=80, value=")) != NULL)
	    {
		pos += strlen("switch reg read offset=80, value=");
		pos[8]='\0';
		/* get the value of ethernet switch register (offset:0x80) */
		sscanf(pos, "%x", &flag);
#if defined(CONFIG_VSC7390)
		/*7390��3052���ӡ�����3052��2,4����wan��*/
		if( ((wannum == 2) && (flag & 0x20000000)) ||
			((wannum == 1) && (flag & 0x08000000)) ) 
		{
		    ret = 1;
		} 
#elif defined(CONFIG_IP1725)
		/* Here judge wan port1 ~ port4 link status (except port0) */
		if( ((wannum == 1) && (flag & 0x04000000)) ||
			((wannum == 3) && (flag & 0x10000000)) ||
			((wannum == 4) && (flag & 0x20000000)) ||
			((wannum == 2) && (flag & 0x08000000)) ) 
		{
		    ret = 1;
		} 
#else
#endif
	    }
	}
    }

    return ret;
}
/********************************************************************
 * ������ formStatForTopImage 
 * ���ܣ� ҳ�涥�˶˿�ͼƬ״̬�������� 
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 * ���ڣ� 2011-09-16 
 *******************************************************************/
static void formStatForTopImage(webs_t wp, char_t *path, char_t *query)
{
#if (FEATURE_SW_PORT_CONF == FYES)
    int min = 0, max = 0;
#endif
    int i = 0, wanNum = 0;
    char m_buffer[64] ;
    char cur_speed[256] ;

    memset(m_buffer,0,sizeof(m_buffer));
    memset(cur_speed,0,sizeof(cur_speed));

    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

#if (FEATURE_SW_PORT_CONF == FYES)
    MibProfileType mibType = MIB_PROF_SW_PORT_CONF;
    SwPortConfProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);/*��ȡ��� �� ��С ����*/

    /*LAN��״̬*/
    for (i = min; i < max; i++)
    {
	/*�˿ڶ�Ӧ���������������*/
	prof = (SwPortConfProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL) 
	{
	    memset(m_buffer, 0, sizeof(m_buffer));
	    m_buffer[0]='\"';
	    sw_link_status_get(i+1, &m_buffer[1]);/*��ȡ�˿�״̬*/

	    /*�˿�ģʽΪ���� �� ״̬Ϊdown*/
	    if((prof->port_mode == PORT_MODE_DISABLED) && (strcmp(&m_buffer[1], "Down") == 0))
	    {
		strcpy(&m_buffer[1], "Disabled");/*�˿�״̬Ϊ����*/
	    }

	    /*���ŷָ���*/
	    sprintf(&m_buffer[strlen(m_buffer)], "\",");
	    strcat(cur_speed, m_buffer);
	}
    }
#endif

    websWrite(wp, "\ncurSpeed = new Array(%s", cur_speed);/*��ӡlan��״̬*/

    /*wan��״̬ӦΪ Down Disabled ��������ֻҪ������fiber���ɣ�*/
    /* get wan link status */
    wanNum =  getWanSlotCount();
    for(i = 1; i <= wanNum; i++)
    {
	/* read the wan port link status */
	if(get_wan_port_link_status(i) == 1)
	{
	    if (i == wanNum)
	    {
		websWrite(wp, "\"%s\");\n", "Up");
	    }
	    else
	    {
		websWrite(wp, "\"%s\",", "Up");
	    }
	}
	else
	{
	    if (i == wanNum)
	    {
		websWrite(wp, "\"%s\");\n", "Down");
	    }
	    else
	    {
		websWrite(wp, "\"%s\",", "Down");
	    }
	}
    }
    
    websDone(wp, 200);
}
