#include        <linux/autoconf.h>
#include	<string.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"internet.h"

#if (PORT_CONF_REDUCED == FYES)


#ifdef  PORT_REDUCED_DBG
#define PORT_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define PORT_PRINTF(fmt, args...)
#endif

extern int getpppoeStatus(int wannum);
extern int getAggrMember(webs_t wp);
/*ҳ��ؼ�����*/
static char *select_mode[24] = {"R0","R1","R2","R3","R4","R5","R6","R7", "R8", "R9", "R10","R11","R12","R13","R14","R15","R16","R17","R18","R19","R20","R21", "R22", "R23"};



/********************************************************************
 * ������ aspOutputPortConfigure
 * ���ܣ� ҳ����ʾ����
 * ������ 2010-08-24
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� jfgu
 * ά���� jfgu
 * ���ڣ� 2010-09-01
 ********************************************************************/ 
int aspOutputPortConfigure(int eid, webs_t wp, int argc, char **argv)
{
#ifdef CONFIG_USER_3G
    char_t ip[16];
#endif
    int i = 0;
    int state=0;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
#if 0
    char_t *wanMode=T("wanMode[%d]=%d;\n");
    websWrite(wp,"var wanMode=new Array();\n");
#endif
    char_t *PortNames=T("PortNames[%d]=\"%d\";\n");
    char_t *curSpeed=T("curSpeed[%d]=\"%s\";\n");
    char_t *speedMode=T("speedMode[%d]=%d;\n");
    char_t *maxSpeed=T("maxSpeed[%d]=%d;\n");
    char_t *ConnStatuss=T("ConnStatuss[%d]=%d;\n");
    websWrite(wp,"var numPorts;\n");
    websWrite(wp,"var PortNames=new Array();\n");
    websWrite(wp, "var curSpeed = new Array();\n");
    websWrite(wp, "var speedMode = new Array();\n" );
    websWrite(wp, "var maxSpeed = new Array();\n" );
    websWrite(wp,"var ConnStatuss=new Array();\n");

    websWrite(wp,"numPorts=%d;\n",getWanSlotCount()+1);
    PORT_PRINTF("%s-%d: wanslot=%d\n", __func__, __LINE__, getWanSlotCount());
    for(i = 0; i <= getWanSlotCount(); i++) {
        //arrayIndex = i - 1;
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);
	if ((profIf != NULL) || (profIf->head.active != 0)) 
	{
	    /*ģʽ*/
	    websWrite(wp, PortNames, i, i);
	    if(0==i)
		PORT_PRINTF("%s-%d: ifname=%s\n", __func__, __LINE__, getLanIfName());
	    else
		PORT_PRINTF("%s-%d: ifname=%s\n", __func__, __LINE__, getWanIfNamePPP(i));

	    websWrite(wp, curSpeed, i, "Down");
#if (WIRELESS == FYES)
	    if ((i == getAPCliSlotIndex()) && (i == get3GSlotIndex())) {
		//websWrite(wp, speedMode, i,-1);
		websWrite(wp, maxSpeed, i, -1);
	    }else
#endif
	    {
#ifdef CONFIG_SWITCH_EXT
		websWrite(wp, maxSpeed, i, -1);
#else
		PORT_PRINTF("%s-%d: \n", __func__, __LINE__);
		if(MAX_SPDMODE_RDD==10){
		    websWrite(wp, maxSpeed, i, 10);
		}else if(MAX_SPDMODE_RDD==100){
		    websWrite(wp, maxSpeed, i, 100);
		}else if(MAX_SPDMODE_RDD==1000){
		    websWrite(wp, maxSpeed, i, 1000);
		}
		PORT_PRINTF("%s-%d: \n", __func__, __LINE__);
#endif
		websWrite(wp, speedMode, i, profIf->ether.speedMode);
	    }
	    if(i==0)
		websWrite(wp, ConnStatuss, 0, 0);
	    if(i!=0)
	    {
		PORT_PRINTF("%s-%d: \n", __func__, __LINE__);
#ifdef CONFIG_USER_3G
//        if(wannum == 3){
		if (i == get3GSlotIndex()) {
		   if((getIfIp(getWanIfNamePPP(i),ip)==0) /*&& wanIsUp*/ ) {
	    		state=1;
		    }
		}else 
#endif 
		{
		    switch(profIf->ether.connMode) {
			case ETH_CONN_DHCP:
			    if(getIfIp(getWanIfName(i), NULL) == 0) {
			    state=1;
			    }
			    state &= get_phylink_status(i);
			    break;
			case ETH_CONN_STATIC:
			    state=get_phylink_status(i);
			    break;
			case ETH_CONN_PPPOE:
			    state = getpppoeStatus(i);
			    state &= get_phylink_status(i);
			    break;
			default:
			    break;
		    }
		}

		PORT_PRINTF("%s-%d: \n", __func__, __LINE__);
		websWrite(wp, ConnStatuss, i, state);
	    }

	}
    }

    return 0;
}

/*******************************************************************
 *������ formPortReducedConfigure
 *���ܣ� ҳ���ύ����
 *������ ��
 *���أ� ��
 *���ߣ� jfgu
 *���ڣ� 2010-8-24
 ******************************************************************/
#if 1
static void formPortReducedConfigure(webs_t wp, char_t *path, char_t *query)
{
    int tmp=-1;
    int i=0;

    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    struProfAlloc *profhead = NULL;


    PORT_PRINTF("%s-%d: wanSlot=%d\n",__func__,__LINE__,getWanSlotCount());
    /*ȡ��ҳ��ؼ��е�ֵ*/
    for (i = 0; i <= getWanSlotCount() ; i++)
    {
        profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, i);

	if(profIf != NULL) 
	{
	    /*ȡ������ģʽ�е�ֵ*/
	    tmp = (int)(strtoul(websGetVar(wp, select_mode[i], T("")),  NULL, 0));
	    if(profIf->ether.speedMode != tmp)
	    {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, i, &profhead);/*����*/
		profIf->ether.speedMode = tmp;
		PORT_PRINTF("%s-%d: \n",__func__,__LINE__);
		//is_wait=1;
	    }
	    PORT_PRINTF("%s-%d: i=%d, tmp=%d, profIf->ether.speedMode=%d\n",__func__,__LINE__,i, tmp,profIf->ether.speedMode);

	}
    }
    /*ֻ�е��������������ò�����goahead�Żᷢ��Ϣ*/
    if(profhead != NULL)
    {
	PORT_PRINTF("%s-%d: \n",__func__,__LINE__);
	ProfUpdate(profhead);
	PORT_PRINTF("%s-%d: \n",__func__,__LINE__);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
    }

#if 0
    if(is_wait > 0u)
    {
	sleep(4);/*��Щ�������ܵ��¶˿�����������ȴ�*/
    }
#endif
    websRedirect(wp, "portSpeed.asp");

}
#endif
/********************************************************************
 * ������ fromDefinePortStatics
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2010-08-16
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� jfgu
 * ά���� jfgu
 * ���ڣ� 2010-09-01
 *******************************************************************/
void formDefinePortReducedConf(void)
{
	websAspDefine(T("aspOutputPortConfigure"),aspOutputPortConfigure);
	websFormDefine(T("formPortReducedConfigure"),formPortReducedConfigure);
}
#endif


