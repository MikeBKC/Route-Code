
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#if (FEATURE_SW_PORT_CONF == FYES)
#include  "sw_ioctl.h"

static void convertPortMode(port_conf_t *conf, SwPortConfMode mode);
static void swPortConfSet(const SwPortConfProfile* prof);
static void swPortConfInit(void);
/*��Ϣ������
 * bhou*/
void swPortConfProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const SwPortConfProfile* oldProfileDataPtr, const SwPortConfProfile* newProfileDataPtr )
{/*���ڶ˿ڹ�������ʵ�����ô���*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*�༭*/
	    swPortConfSet(newProfileDataPtr);
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*Ĭ��*/
            break;
    }
}
/**
 * ת�������е� �˿�ģʽ ��������Ϊ ioctl�����ṩ����������
 * bhou
 * 2011-08-31
 */
static void convertPortMode(port_conf_t *conf, SwPortConfMode mode)
{
    switch(mode)
    {
	case PORT_MODE_DISABLED:/*link disabled*/
	    conf->enable = 0;/*port link disabled*/
	    break;
	case PORT_MODE_AUTO:/*auto negotiate*/
	    conf->enable = 1;
	    conf->autoneg = 1;
	    break;
	case PORT_MODE_10FDX:/*10Mbit/s full duplex*/
	    conf->enable = 1;/*link enable*/
	    conf->autoneg = 0;/*auto negotiate disabled*/
	    conf->speed = SPEED_10M;/*10Mbit/s*/
	    conf->fdx = 1;/*full duplex*/
	    break;
	case PORT_MODE_100FDX:/*100Mbit/s full duplex*/
	    conf->enable = 1;
	    conf->autoneg = 0;
	    conf->speed = SPEED_100M;/*100Mbit/s*/
	    conf->fdx = 1;/*full duplex*/
	    break;
#ifdef CONFIG_VSC7390
	case PORT_MODE_1GFDX:/*1000Mbit/s full duplex*/
	    conf->enable = 1;
	    conf->autoneg = 0;
	    conf->speed = SPEED_1G;
	    conf->fdx = 1;/*full duplex*/
	    break;
#endif
	case PORT_MODE_10HDX:/*10Mbit/s half duplex*/
	    conf->enable = 1;
	    conf->autoneg = 0;
	    conf->speed = SPEED_10M;
	    conf->fdx = 0;/*half duplex*/
	    break;
	case PORT_MODE_100HDX:/*100Mbit/s half duplex*/
	    conf->enable = 1;
	    conf->autoneg = 0;
	    conf->speed = SPEED_100M;
	    conf->fdx = 0;/*half duplex*/
	    break;

	default:
	    /*auto negotiate*/
	    conf->enable = 1;
	    conf->autoneg = 1;
	    break;

    }
}
/***
 * ������Ч����
 * bhou
 * 2011-09-06
 */
static void swPortConfSet(const SwPortConfProfile* prof)
{
    Uint32 uport_no = 0u;
    port_conf_t portConf;

    memset(&portConf, 0, sizeof(portConf));

    if(prof != NULL) 
    {
	/*������ת��Ϊ ioctl��ʶ��ṹ*/
	uport_no = (Uint32)strtoul(&(prof->head.name[4]), NULL, 10);/*����ǰ���port��ת��Ϊ���ֶ˿ں�*/
	portConf.flow_control = prof->flow_control;
#ifdef CONFIG_VSC7390
	portConf.max_length = prof->max_frame;
#endif
	convertPortMode(&portConf, prof->port_mode);/*�˿�ģʽ*/
	/*���ں�ͨ��ʹ������Ч*/
	sw_port_conf_set(uport_no, &portConf);
    }
    return;
}
/***
 * ������Ч����
 * bhou
 * 2011-08-31
 */
static void swPortConfInit(void)
{
    int i = 0, minIndex =0  , maxIndex = 0 ;
    port_conf_t conf;
    MibProfileType mibType = MIB_PROF_SW_PORT_CONF;
    SwPortConfProfile *prof = NULL;

    memset(&conf, 0, sizeof(conf));

    ProfInstLowHigh(mibType, &maxIndex, &minIndex);/*��ȡ��Χ*/
    for (i = minIndex; i < maxIndex; i++)
    {
	/*����*/
	prof = (SwPortConfProfile*)ProfGetInstPointByIndex(mibType, i);/*��ȡ����*/
	swPortConfSet(prof);/*��Ч*/
    }
    return;
}

/*��ʼ������
 * bhou*/
void funInitSwPortConf(void)
{
    registerForProfileUpdates(MIB_PROF_SW_PORT_CONF, (Pointer)NULL, swPortConfProfileChange);
    swPortConfInit();/*���ó�ʼ����Ч*/
    return;
}
#endif
