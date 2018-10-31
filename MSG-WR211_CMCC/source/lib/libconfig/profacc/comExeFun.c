/**
 * ������Ч��������
 * ���Ӵ��ļ���Ϊ�˱�����Ϣ���ġ�����̼������µ����������쳣
 *
 * ��pppd�������¼Ʒ�profilfe �� ����Ϣ speedweb���¹���ʱ����������pppd����
 * profile�������࣬��ʱ����������� speedweb��Ϣ��������������޷���ʱ����������Ϣ
 */
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/autoconf.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "ipsetApi.h"
#include "comExeFun.h"
#include <time.h>
#include <../../libusb-user/usb-user.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include "base64.h"

//#define SWORD_DEBUG
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#if (PPPOE_ACCOUNT == FYES) 

#if 0
#if (NOTICE == FYES)
static void upPoeExpiringRule(PPPoESrvAccProfile *prof , char addOrDel);
#endif
static void upPoeExpiredRule(PPPoESrvAccProfile *prof , char addOrDel);
#else
#if (NOTICE == FYES)
static void upPoeExpiringRule(SessionList prof , char addOrDel);
#endif
static void upPoeExpiredRule(SessionList prof , char addOrDel);
#endif
int uttGlTime = 0;
extern int Base64Encode(char *OrgString, char *Base64String, int OrgStringLen );
/********************************************************************
 * ������ upPppAccStatus 
 * ���ܣ� ��ȡ�û��˺�״̬
 * ������ 2012-04-01
 * ������ PPPoE�Ʒ��û���profile
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
void upPppAccStatus(PPPoESrvAccProfile *prof)
{
#if (NOTICE == FYES)
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *profA = NULL;
#endif

    int tmCurrent = 0;
    if(uttGlTime != 0) 
    {
	tmCurrent = uttGlTime;
    }
    else
    {
	tmCurrent = time(NULL);
    }

    if((prof->tmStart < tmCurrent) && (tmCurrent < prof->tmStop))
    {
	prof->expired = PPPOE_ACC_NORMAL;
 #if (NOTICE == FYES) 
	profA = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 1);
	if((profA != NULL) && (ProfInstIsFree(profA) == 0))
	{
	    if(((prof->tmStop - (profA->remainDays) * 86400) < tmCurrent) && (tmCurrent < prof->tmStop))
	    {
		prof->expired = PPPOE_ACC_EXPIRING;
	    }
	}
#endif
    }
    else if((tmCurrent >= DELIVERY_START_TIME) && (tmCurrent <= DELIVERY_END_TIME))
    {/*��ʱ��Ϊ����ʱ�䵽����ʱ����2Сʱ���ж��Ƿ����*/
	prof->expired = PPPOE_ACC_NORMAL;
    }
    else if((tmCurrent < prof->tmStart) || (tmCurrent > prof->tmStop))
    {
	prof->expired = PPPOE_ACC_EXPIRED;
    }
#if 0
    printf("%s %d %d\n", __func__, __LINE__, prof->expired);
#endif

    return;
}

#if 0
#if (NOTICE == FYES)
/**
 * ��pppoe�����ڹ���
 */
static void upPoeExpiringRule(PPPoESrvAccProfile *prof , char addOrDel)
{
    ipsetAOrDIpStr(POE_EXPIRING_ACC, prof->ip, addOrDel);/*�彫������*/
    if(addOrDel == IPSET_MEM_DEL)
    {
	ipsetAOrDIpStr(POE_NT_OK, prof->ip, addOrDel);/*����ͨ���顣�������ߣ��ٴ�ͨ��*/
    }
    return;
}
#endif
/**
 * ���¹����˺Ź���
 */
static void upPoeExpiredRule(PPPoESrvAccProfile *prof , char addOrDel)
{
    ipsetAOrDIpStr(POE_EXPIRED_ACC, prof->ip, addOrDel);
    return;
}
/**
 * ����pppoe�û���ع���
 * prof:    pppoe�û�profileָ��
 * status : pppoe�û�״̬
 * isAdd  ��1 ��ӹ��� �� 0 ɾ������
 */
void upPppAccRule(PPPoESrvAccProfile *prof, emPppoeAccStatus status, int isAdd)
{
    char  ipsetAct = IPSET_MEM_ADD;
    if((strcmp(prof->device, (char *)getLanIfName()) == 0))
	/*ֻ�����lan��pppoe�û�*/
    {
	if(isAdd == 0) 
	{
	    ipsetAct = IPSET_MEM_DEL;
	}

	switch(status)
	{
	    case PPPOE_ACC_EXPIRED:
		upPoeExpiredRule(prof, ipsetAct);
		break;
#if (NOTICE == FYES)
	    case PPPOE_ACC_EXPIRING:
		upPoeExpiringRule(prof, ipsetAct);
		break;
#endif
	    default:
		break;

	}
    }
    return;
}
#else
#if (NOTICE == FYES)
/**
 * ��pppoe�����ڹ���
 */
static void upPoeExpiringRule(SessionList prof , char addOrDel)
{
    ipsetAOrDIpStr(POE_EXPIRING_ACC, prof.ip, addOrDel);/*�彫������*/
    if(addOrDel == IPSET_MEM_DEL)
    {
	ipsetAOrDIpStr(POE_NT_OK, prof.ip, addOrDel);/*����ͨ���顣�������ߣ��ٴ�ͨ��*/
    }
    return;
}
#endif
/**
 * ���¹����˺Ź���
 */
static void upPoeExpiredRule(SessionList prof , char addOrDel)
{
    SWORD_PRINTF("ipset -q -%c %s %s\n", addOrDel, POE_EXPIRED_ACC, prof.ip);
    ipsetAOrDIpStr(POE_EXPIRED_ACC, prof.ip, addOrDel);
    return;
}
/**
 * ����pppoe�û���ع���
 * prof:    �Ựָ��
 * status : pppoe�û�״̬
 * isAdd  ��1 ��ӹ��� �� 0 ɾ������
 */
void upPppAccRule(SessionList prof, emPppoeAccStatus status, int isAdd)
{
    char  ipsetAct = IPSET_MEM_ADD;
    if((strcmp(prof.device, (char *)getLanIfName()) == 0))
	/*ֻ�����lan��pppoe�û�*/
    {
	if(isAdd == 0) 
	{
	    ipsetAct = IPSET_MEM_DEL;
	}

	switch(status)
	{
	    case PPPOE_ACC_EXPIRED:
		upPoeExpiredRule(prof, ipsetAct);
		break;
#if (NOTICE == FYES)
	    case PPPOE_ACC_EXPIRING:
		upPoeExpiringRule(prof, ipsetAct);
		break;
#endif
	    default:
		break;

	}
    }
    return;
}
#endif
#if 0
 /********************************************************************
 * ������ upPppoeAcc
 * ���ܣ� ͳ�Ƶ���pppoe�û���״̬����������ع���
 * ������ 2012-08-24
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
void upPppoeAcc(PPPoESrvAccProfile *prof, SessionList Session)
{
    emPppoeAccStatus oldSta = prof->expired;/*�����ϵ�״̬*/
    upPppAccStatus(prof);/*��������״̬*/
    SWORD_PRINTF("%s------%d, oldSta = %d, prof->expired = %d\n", __func__, __LINE__, oldSta, prof->expired);
    if(oldSta != prof->expired)
    {
	upPppAccRule(Session, oldSta, 0);/*ɾ��ԭ�й���*/
	upPppAccRule(Session, prof->expired, 1);/*����¹���*/
    }

    return;
}
#endif
 /********************************************************************
 * ������ pppoeSesCheck
 * ���ܣ� ѭ�������˺Ŷ�Ӧ�����лỰ�����¼Ʒ���Ϣ
 * ������ 2012-08-24
 * ������ 
 *	  prof     -    �˺Ŷ�Ӧprofile
 * ���أ� ��
 * ����� ��
 * return : 1 �Ʒ�״̬�仯 0 �Ʒ�״̬�ޱ仯
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
int pppoeSesCheck(PPPoESrvAccProfile* prof)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int i = 0, j = 0, ret = 0;

    profS = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);
    if(profS != NULL)
    {
	emPppoeAccStatus oldSta = prof->expired;/*�����ϵ�״̬*/
	upPppAccStatus(prof);/*��������״̬*/
	SWORD_PRINTF("%s------%d, oldSta = %d, prof->expired = %d\n", __func__, __LINE__, oldSta, prof->expired);
	if(oldSta != prof->expired)
	{  
            ret = 1;
	    j = prof->sesIndex;
	    if(SesIndexIsOk(j) == 1)
	    {
		for(i = 0; i < prof->actualSession; i++)
		{
		    SWORD_PRINTF("%s------%d, nvramProfile->Session[%d].ConTime = %d, nvramProfile->Session[%d].ip = %s\n", __func__, __LINE__, j, nvramProfile->Session[j].ConTime, j, nvramProfile->Session[j].ip);
		    if((nvramProfile->Session[j].ConTime == 0U) && (prof->chargeType !=  NONE_CHARG_MODE))/*���߲��ҿ����˼Ʒ�*/
		    {		   
			upPppAccRule(nvramProfile->Session[j], oldSta, 0);/*ɾ��ԭ�й���*/
			upPppAccRule(nvramProfile->Session[j], prof->expired, 1);/*����¹���*/
		    }
		    j = nvramProfile->Session[j].nextIndex;
		    if(SesIndexIsOk(j) == 0)
		    {
			break;
		    }
		}
	    }
	}
    }
    return ret;
}
#endif

#if (IP_GRP == FYES)
/**
 * ��ָ����ip���в��� ָ�����û�
 *
 * grpProfIndex : ip��profile����
 * userName     : Ҫ���ҵ��û���
 * userType     : �û����� 
 * searched     : �����������顣�û���֤ �����ҹ����鲻�ٱ��ظ�����
 *                ����ֵ 0 ����δ�����ҹ�  -1 �������Ҳ���
 *                1 �ڸ������ҵõ�
 *
 * return: -1 ip���в����и��û�
 *         1   ip���к��и��û�
 */
int ipGrpSearchUser(int grpProfIndex, char*userName, ipGrpEntryType userType, int*searched)
{
    int i = 0, ret = -1, subIndex = 0;
    IpGrpProfile* prof = NULL;
    MibProfileType mibType = MIB_PROF_IPGRP;

    ret = searched[grpProfIndex];/*�����Ƿ��Ѿ������˲���*/
    if(ret == 0)/*δ���ҹ�*/
    {
	ret = -1;/*���Ҳ���*/
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, grpProfIndex);
	if(ProfInstIsFree(prof) == 0)
	{
	    for(i=0; i< IPGRP_USER_ENTRYS; i++)
	    {/*�������г�Ա*/
		if(prof->ipEntry[i].entryType == userType)
		{	
		    if(strcmp(userName, prof->ipEntry[i].entryVal.name) == 0)
		    {
			ret = 1;/*�ҵ�*/
		    }
		}
		else 
		{

		    if(prof->ipEntry[i].entryType == IPENT_GRP)
		    {/*�������в���*/
			subIndex = ProfGetInstIndexByName(mibType, prof->ipEntry[i].entryVal.name);
			if(subIndex != PROFFAIL)/*����ʹ����*/
			{
			    ret = ipGrpSearchUser(subIndex, userName, userType, searched);
			}
		    }

		}
		if(ret == 1)
		{/*�ҵ����˳� */
		    break;
		}
	    }
	}
	searched[grpProfIndex] = ret;/*��¼���Ѳ���*/
    }

    return ret;
}
/**
�û�������ʱ���ж�Ӧ��ip����Ӧ��ipset��
Username�� �û���
Ip�� �û���Ӧ��ip
UserType��  pppoe�˺� �� web��֤�û�������
AddorDel:   IPSET_MEM_ADD ��������ʱ���
IPSET_MEM_DEL  ��������ʱɾ��
**/
void userUpToIpGrp(char* username,char *ip, ipGrpEntryType userType, char addOrDel)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;
    int i = 0, min = 0, max = 0;
    int searched[MAX_IPGRP_PROFILES];/*�����ռ䡣��֤ÿ����ֻ������һ�Ρ����Ч��*/
    memset(searched, 0,sizeof(searched));/*��ʼ��Ϊÿ���鶼δ���ҹ�*/

    ProfInstLowHigh(mibType, &max, &min);
    SWORD_PRINTF("%s------%d, user is %s,ip = %s\n", __func__, __LINE__, username, ip);
    /*��������profile*/
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0 && prof->grpType == IPGRP_USER)/*ֻ�����û���ip�� */
	{
	    if(searched[i] == 0)
	    {/*����δ�����ҹ�*/
		ipGrpSearchUser(i, username, userType, searched);
	    }
	    SWORD_PRINTF("%s-----%d,searched[i]\n", __func__, __LINE__);
	    if(searched[i] == 1)
	    {/*�ڸ����в��ҵ���grpName*/
		SWORD_PRINTF("%s------%d, prof->head.name is %s\n", __func__, __LINE__, prof->head.name);
		ipsetAOrDIpStr(prof->head.name, ip, addOrDel);/*������Ӧ��ipset��*/
	    }
	}
    }

    return;
}
/**
 * ����ip��ַ���� rootPorf������subName��������
 * rootProf: ָ��Ҫ���ҵĸ��ڵ�
 * subName:  Ҫ���ҵ�����
 *           ��ΪNULL����������������
 * return: 
 *         -1  rootProf������subName
 *        >=0 rootProf��subName�������ȣ�������rootProf��
 *
 */
int ipGrpDepToSub(IpGrpProfile *rootProf, char* subName)
{
    IpGrpProfile *subGrp = NULL;
    int i = 0, dep = -1, subDep = 0 , maxEntrys = 0;

    if(rootProf->grpType == IPGRP_IP)
    {
	maxEntrys =  IPGRP_IP_ENTRYS;
    }
    else
    {
	maxEntrys = IPGRP_USER_ENTRYS;
    }


    if(subName == NULL)
    {
	dep = 1;/*���������顣����С���Ϊ1*/
    }
    else 
    {
	if((strcmp(subName, rootProf->head.name) == 0))
	{/*�Լ����Լ������Ϊ0*/	
	    dep = 0;
	}
    }

    if(dep != 0)/*�Լ�����������*/
    {
	/*ѭ�����������������Ŀ*/
	for(i = 0; i < maxEntrys; i++)
	{
	    if(rootProf->ipEntry[i].entryType == IPENT_GRP)
	    {/*����Ŀ����Ϊ������ַ��*/
		/*���������Ƿ����*/
		subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, rootProf->ipEntry[i].entryVal.name);
		if(ProfInstIsFree(subGrp) == 0)/*����ʹ����*/
		{
		    subDep = ipGrpDepToSub(subGrp, subName);/*������subName��������*/
		    if(subDep >= 0 )
		    {
			subDep = subDep + 1;/*�����������ڵ㵽subNma�����*/
			if(dep < subDep)
			{/*������е����ֵ*/
			    dep = subDep;
			}

		    }
		}

	    }

	}
    }
    return dep;

}
/**
 * ������ rootProf����ȣ��������ڵ㣩
 * rootProf: ָ��Ҫ��������
 * subName : ������ʱ�� �����Ƿ��и��������.
 * return��subNameΪNUll�򷵻�rootProf���
 *         subNae��ΪNULL��������rootProf�Լ��������ҵ�subName������0.���򷵻�rootProf���
 */
int ipGrpDepSearch(IpGrpProfile *rootProf, char *subName)
{
    IpGrpProfile *subGrp = NULL;
    int i = 0, dep = 1, subDep = 0, maxEntrys = 0;

    if(rootProf->grpType == IPGRP_IP)
    {
	maxEntrys =  IPGRP_IP_ENTRYS;
    }
    else
    {
	maxEntrys = IPGRP_USER_ENTRYS;
    }


    if((subName != NULL) && (strcmp(subName, rootProf->head.name) == 0))
    {/*������ͬ�����ҵ�����*/
	dep = 0;
    }
    else
    {
	/*ѭ�����������������Ŀ*/
	for(i = 0; i < maxEntrys; i++)
	{
	    if(rootProf->ipEntry[i].entryType == IPENT_GRP)
	    {/*����Ŀ����Ϊ������ַ��*/

		/*���������Ƿ����*/
		subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, rootProf->ipEntry[i].entryVal.name);
		if(ProfInstIsFree(subGrp) == 0)/*����ʹ����*/
		{
		    subDep = ipGrpDepSearch(subGrp, subName);/*�����Ƿ������������*/
		    if(subDep == 0)
		    {/*����������򷵻�0*/
			dep = 0;
		    }
		    else 
		    {
			subDep = subDep + 1;/*������ȼ�������*/
			if(dep < subDep)
			{/*���µ�ǰ���� ���*/
			    dep = subDep;
			}
		    }
		}
		if(dep == 0)
		{/*�ڸ�����������ҵ����ҵ���*/
		    break;
		}

	    }

	}
    }
    return dep;

}
#if (FIREWALL == FYES)
/**
 * �ж�ip���Ƿ񱻷��ʿ��Ʋ���������
 * return :
 *        1 ��������
 *        0 δ������
 */
int isIpGrpReredByFireWall(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int ret = 0;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i); 
	if(ProfInstIsFree(prof) == 0) {
	    if((strcmp(ipProf->head.name, prof->ipGrpName) == 0)
	      ||(strcmp(ipProf->head.name, prof->destIpGrpName) == 0))
	    {/*��������*/
		ret = 1;
		break;
	    }
	}
    }
    return ret;
}
#endif
#if (GROUP_BASE == FYES)
/**
 * �ж�ip���Ƿ���Ϊ����������
 * return :
 *        1 ��������
 *        0 δ������
 */
int isIpGrpReredByNf(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int ret = 0;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i); 
	if(ProfInstIsFree(prof) == 0) {
	    if(prof->groupType == GBT_ACT_CONTROL)
	    {/*������Ϊ����*/
		if(strcmp(ipProf->head.name, prof->ipGrpName) == 0)
		{/*��������*/
		    ret = 1;
		    break;
		}
	    }
	}
    }
    return ret;
}
#endif
#if (WEB_AUTH == FYES)
/***********************************************************8
 *
 *�ж��Ƿ�web��֤����Ϊ�����ַ��
 *return 1 ������
 *		0 δ������
 * *********************************************************/
int isIpGrpReredByWebAuth(IpGrpProfile *ipProf)
{
   MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
   WebAuthGlobalProfile *prof = NULL;
	int ret = 0;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0); 
	if(ProfInstIsFree(prof) == 0) {
		if(strcmp(ipProf->head.name, prof->exceptIpGroup) == 0)
		{/*��������*/
		    ret = 1;
		}
	    }
    return ret;
}
/*************************
 *
 *���ܣ�ɾ��OK���оɵ������ַ���ڵĳ�Ա
 *������WebAuthGlobalProfile *prof
 *ʱ�䣺20120327
 * *************************/
void webauthDelexceptIpGroup(WebAuthGlobalProfile *prof)
{

	IpGrpProfile* profs1 = NULL;
	IpGrpProfile* profs2 = NULL;
	int i=0,j=0;
	IPADDR ipfrom,ipto;

	if(prof->exceptIpGroup[0] != '\0')
	{
printf("%s,%d,prof->exceptIpGroup=%s\n",__func__,__LINE__,prof->exceptIpGroup);
	profs1 =(IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,prof->exceptIpGroup); /*��ȡʵ��*/


	while( i < IPGRP_ENTRYS )
			{
				if(profs1->ipEntry[i].entryType == IPENT_GRP)
				{
					profs2 = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,profs1->ipEntry[i].entryVal.name);

					while(j<IPGRP_ENTRYS){
					if(profs2->ipEntry[j].entryType == IPENT_RANGE){
					ipfrom = profs2->ipEntry[j].entryVal.range.ipFrom;
					ipto = profs2->ipEntry[j].entryVal.range.ipTo;
					SWORD_PRINTF("%s,%d,IPfrom=%x,ipto=%x\n",__func__,__LINE__,ipfrom,ipto);
					/*ɾ����ַ��*/
					ipsetAOrDIpRange(SYS_WEB_AUTH_OK, ipfrom, ipto, IPSET_MEM_DEL);
					}
					j++;
					}
				}
				else if (profs1->ipEntry[i].entryType == IPENT_RANGE) {
					ipfrom =  profs1->ipEntry[i].entryVal.range.ipFrom;
					ipto =  profs1->ipEntry[i].entryVal.range.ipTo;
					SWORD_PRINTF("%s,%d,IPfrom=%x,ipto=%x\n",__func__,__LINE__,ipfrom,ipto);
					ipsetAOrDIpRange(SYS_WEB_AUTH_OK, ipfrom, ipto, IPSET_MEM_DEL);
				}
				i++;
			}
		}
return ;
}
/*ɾ�������ַ��*/
void IPGrpDelWebExIp(char* grpName)
{

	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	int i=0;

	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)&&(strcmp(prof->exceptIpGroup,grpName) == 0))
	{
	SWORD_PRINTF("%s--------%d, del webauth ipgroup\n", __func__, __LINE__); 
	webauthDelexceptIpGroup(prof);
	}
	return;
}

#if (WEB_AUTH_SHARE == FYES)
/*
 * checkSessionImdex()
 * ���web��֤�˺Ź����Ự�������Ƿ���ȷ
 * 1 -- �Ự������ȷ
 * 0 -- �Ự������ȥ
 * */
int checkSessionIndex(int index)
{ 
    int flag = 0;        
    if((index >= 0)&&(index < MAX_WEB_AUTH_SESSION))
    {
	flag = 1;        
    }
    return flag;
} 
#endif
#endif
#if (P2P_LIMIT == FYES)
/**
 * �ж�ip���Ƿ�P2P����������
 * return :
 *        1 ��������
 *        0 δ������
 */
int isIpGrpReredByP2PLimit(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_P2P_LIMIT;
    P2PLimitProfile *prof = NULL;
    int ret = 0;
    prof = (P2PLimitProfile*)ProfGetInstPointByIndex(mibType, 0); 
    if(ProfInstIsFree(prof) == 0) {
	if(strcmp(ipProf->head.name, prof->ipGrpName) == 0)
	{/*��������*/
	    ret = 1;
	}
    }
    return ret;
}
#endif
#endif
#if (PPPOE_SERVER == FYES)
/********************************************************************
 * ������ SesIndexIsOk
 * ���ܣ� �жϻỰ�����±��Ƿ���ȷ
 * ������ 2012-08-23
 * ������ �Ự�����±�
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
int SesIndexIsOk(int i)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    int flag = 0;

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);

    if(prof != NULL)
    {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
	if((i >= 0) && (i < prof->ipcount))
#else
	if((i >= 0) && (i < prof->maxSessCnt))
#endif
	{/*�±�Ӧ�����㣬С��ϵͳ���Ự��*/
	    flag = 1;
	}
    }
    return flag;
}
#endif
#if (ARP_BINDS == FYES)
/**
 * ����: ��arp��̬����ƥ�� ip��mac������ƥ��״̬
 * ����:
 *       ip����ƥ���ip��ַ
 *       mac:��ƥ���mac��ַ
 *       arpIndex������ΪNULL�� ����ʱָ�򲻲��ҵ����������ʱ����Ϊƥ�䵽������
 * ����: bhou
 * ʱ��: 2011-10-26
 */
extern ARP_MATCH matchFromArpBinds(IPADDR ipAddr, MacAddr macAddr, int* arpIndex)
{
    int min = 0, max = 0;
    int i = 0, tmpIndex = 0;

    ARP_MATCH ret = MATCH_NONE;/*��ʼ��Ϊʲô����ƥ��*/

    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);/*��ȡ�����С����*/

    if(arpIndex != NULL)
    {
        tmpIndex = *arpIndex;/*��¼����ʱ�����ҵ�����*/
    }
    else
    {
        tmpIndex = PROF_UNUSED_INDEX;/*������������*/
    }
    /*��������*/
    for(i = min; i < max; i++) 
    {
        if(i != tmpIndex)/*�����ų�������Ӧ�ų�*/
        {
            prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);/*��ȡʵ��*/
            if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*��������*/
            {
                if(prof->ip == ipAddr)
                {
                    ret = MATCH_IP_ONLY;/*����ƥ��IP��ַ*/
                }
                if(macAddrCmp(&macAddr, &(prof->mac)) == 0)
                {
#if (DHCP_POOL == FYES)
                    DhcpPoolProfile *profPool= NULL;
                    profPool = (DhcpPoolProfile *)ProfGetInstPointByName(MIB_PROF_DHCP_POOL, prof->dhcpPoolName);
                    if ( (ntohl(ipAddr) & ntohl(profPool->netmask)) == (ntohl(profPool->ipStart) & ntohl(profPool->netmask)) ){
#endif
                        if(ret == MATCH_NONE)/*ip��ַ��ƥ��*/
                        {
                            ret = MATCH_MAC_ONLY;/*����ƥ��MAC��ַ*/
                        }
                        else
                        {
                            ret = MATCH_ALL;/*ƥ������*/
                        }
#if (DHCP_POOL == FYES)
                    }
#endif
                }
                if(ret != MATCH_NONE)
                {
                    tmpIndex = i;/*��¼�ҵ���ƥ����Ŀ����*/
                    /*���ҵ�һ��ƥ����Ϣ����ҽ���*/
                    break;
                }
            }
        }
    }

    if(arpIndex != NULL)
    {/*��¼ƥ�䵽������*/
        *arpIndex = tmpIndex;
    }
    return ret;
}
#endif

#if (DHCP_AUTO_BIND == FYES)
/********************************************************************
 * ������ DhcpArpBindConfig
 * ���ܣ� ��ip��mac,���ڹر��Զ��󶨲���ɾ���󶨣��ʹرպ��ٴο���
 *	  ��ʱ���Ѱ󶨹��Ļ��ǲ��󶨣����ǲ��ܱ���
 * ������ 2012-09-13
 * ������ 
 *	  ip	      �󶨵�ip
 *	  macAddr     �󶨵�mac
 *	  userName    �󶨵��û���
 * ���أ�
 *        0           ��ʧ��
 *        1           �󶨳ɹ�
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
int DhcpArpBindConfig(IPADDR ip, MacAddr macAddr,char* userName)
{
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int instIndex = 0;
    const char *errMsg = NULL;
    struProfAlloc *profhead = NULL;
    int flag = 0;

    if(ProfGetInstIndexByName(mibType, userName) == PROFFAIL)
    {/*��ʵ�����Ƿ���ʹ���ж�*/
	if(matchFromArpBinds(ip, macAddr, NULL) == MATCH_NONE)
	{/*ip��mac�Ƿ��Ѱ��ж�*/
	    instIndex = ProfNewInst(mibType, userName, FALSE);/*�½�ʵ��*/

	    if(instIndex != PROFFAIL) /*�������ظ����жϡ�������ֻ����ʱ�ﵽ�����Ŀ*/
	    {
		flag = 1;
		ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*�����´����Ŀ�ʵ��*/
		prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*��ȡʵ��ָ��*/
		prof->bindEn = FUN_ENABLE;/*Ĭ��Ϊ�������*/
		prof->ip = ip;/*��̬��ip��ַ*/
		prof->mac = macAddr;/*��̬��mac��ַ*/
		prof->bindType = AUTO_BIND;
		ProfUpdate(profhead);/*����Ϣ��Ч*/
		ProfFreeAllocList(profhead);/*�ͷű��ݿؼ�*/
	    }
	}
    }
    SWORD_PRINTF("%s------%d, flag = %d\n", __func__, __LINE__, flag);

    return flag;
}
#endif

#if (FIREWALL == FYES)
/********************************************************************
 * ������ websDefineWebAuth
 * ���ܣ� �Ѿ�ϸ�������������ƶ�����ǰ
 * ������ 2012-08-08
 * ������ name �����������
 * ���أ� 1 - �������ƶ�
 *        0 - δ�ƶ�
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 *********************************************************************/
extern int moveToLast(char* name)
{
    int i = 0, min = 0, max = 0;
    int rst = 0;
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile* prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = (max - 1); i >= min; i--)
    {
        prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0)
        {/*�ҳ�����������,���������ƶ�����֮��*/
            if(strcmp(prof->head.name, name) != 0)/*ȷ�����������*/
            {
                moveInstToLast(name, prof->head.name, mibType);
		/*FW_PRINTF("%s-----%d, prof->head.name = %s\n", __func__, __LINE__, prof->head.name);*/
                rst = 1;
            } else {
                rst = 0;
            }
            break;
        }
    }
    return rst;
}

/***********************************************************************
 * ������������Ҫ�ƶ���ʵ�����ƶ���Ŀ��ʵ����֮��
 * ���������
 *          sID:        Ҫ�ƶ���ʵ���š�
 *          tID:        Ŀ��ʵ���š�
 *          proftype:   ʵ�����͡�
 * ���������   0 �ƶ�ʧ�ܣ����ں��棩
 *              1  �ƶ��ɹ�
 * �������ڣ�   2012-08-09
 * ���ߣ�       Jianqing.Sun
 ***********************************************************************/
extern int moveInstToLast(char *sID, char * tID, MibProfileType proftype)
{
    int tmp = 0, min = 0, max = 0, i = 0, tagIndex = 0, srcIndex = 0;
    struProfAlloc *proflist  = NULL;
    struProfAlloc *proflist2  = NULL;

    tagIndex = ProfGetInstIndexByName(proftype,tID);
    srcIndex = ProfGetInstIndexByName(proftype,sID);
    ProfInstLowHigh(proftype, &max, &min);
    /*FW_PRINTF("%s------%d,srcIndex = %d, tagIndex = %d\n", __func__, __LINE__, srcIndex, tagIndex);*/
    if(tagIndex != (srcIndex - 1))
    {     /*��Ҫ����λ��*/
        for(i = min; i < max; i++)
        {                                     /*  ��ͷ��ʼ����  */
            /* �����ҵ�Ŀ��λ�ã� �ͽ�Դʵ�����ݲ�    �뵽Ŀ��ʵ��λ��   ���Ҳ��Ҫ���� Ŀ��ʵ����������>
 * ����ɵ����������� */
            if(i == (tagIndex + 1))
            {
                ProfBackupByIndex(proftype,PROF_CHANGE_EDIT,srcIndex, &proflist);
            }
            if(i==srcIndex) continue;     /*Դʵ��λ�� ��tagIndexλ���Ѿ���>
�룬���ڲ���Ҫ������*/
            ProfBackupByIndex(proftype,100,i, &proflist);  /*100�����ݣ����ǲ�����profilechange*/
        }
        tmp = 1;
    }
    tagIndex=0;    /*�ָ�λ�ô�ͷ��ʼ*/
    proflist2 = proflist;
    while(proflist!= NULL)
    {     /*�ָ�  �ӿ�ʼλ�ûָ���*/
        _ProfRestore(proflist->proftype, tagIndex++, proflist->oldProf);
        proflist = proflist->next;
    }

    ProfUpdate(proflist2);

    ProfFreeAllocList(proflist);
    ProfFreeAllocList(proflist2);
    return tmp;
}
#endif
/***********************************************************************
 * �� �� ����   ip2int
 * �������ڣ�   2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
 ***********************************************************************/
extern unsigned int ip2int(char *ip)
{
    char *tmp,ipbak[16]={0},*ptr;
    unsigned int ret=0;
    unsigned int value;
    strcpy(ipbak,ip);
    tmp=strtok_r(ipbak,".",&ptr);
    while(tmp!=NULL)
    {
	// DBGPRINT("ip2int:%s\n",tmp);
        value=atoi(tmp);
        ret=ret<<8;
        ret|=value;
        tmp=strtok_r(NULL,".",&ptr);
    }
        //DBGPRINT("ip2int:%x\n",ret);
    return ret;
}

/******************************************************************
 * �������ܣ�ʹ��ioctl��ѯarp���õ���֪ip��Ӧmac
 * ����ʱ�䣺2013-02-17
 ****************************************************************/
char* getMacByIpIoctl(struct arpreq *parp)
{
    char *ret = NULL;
    int fd = 0;
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
	if (!(ioctl (fd, SIOCGARP, parp)))/*��ѯarp��õ���ǰip��Ӧ��mac��ַ*/
	{
	    if (parp->arp_flags != 0)/*�鵽arp����Flags����0,��mac��ַ����0��web��֤�Ʒ�ʱ���ж�mac��ַ*/
	    {
		ret = parp->arp_ha.sa_data;
	    }
	}
	close(fd);
    }
    return ret;
}

#if (EASY_TASK == FYES)
/*
 * ���ܣ�����SystemProfile�г�Ա��ֵ
 * ����ʱ�䣺2013-6-7
 * ���룺num -- ÿһ����Ŷ�Ӧһ����Ա
 * �����int -- ��Ӧ��Ա��ֵ
 * ��ע��ֻ�ܷ���int�ͳ�Ա��ֵ
 */
int getValueByProfSys(int num)
{
    int retVal = -1;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *prof = NULL;

    prof = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == 0)
    {
	switch(num)
	{
	    case 1:
		retVal = prof->isDefaultConf;/*��������*/
		break;
	    default:
		/*null*/
		break;
	}
    }
    return retVal;
}

/*
 * ���ܣ��ı��׹�����WAN�ڵ�״̬����WAN��ʵ��״̬(��pv��)
 * ����ʱ�䣺2013-6-7
 * ���룺status -- �ı���״̬
 * �����0 -- �ɹ�  1 -- ʧ��
 */
int wanConnStatsChangeNPV(int status)
{
    int retVal = 0;
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;

    prof = (EasyTaskProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == 0)
    {
#if 0
	if (((prof->ConnStatus == CONNECTED) || (prof->ConnStatus == CONNECTING)) && ((status < CHECKING) || (status > CHECK_COMPLETE)))
#endif
	{
	    prof->ConnStatus = status;
	}
    }    

    return retVal;

}
/*
 * ���ܣ��ı��׹�����WAN�ڵ�״̬����WAN��ʵ��״̬
 * ����ʱ�䣺2013-6-7
 * ���룺status -- �ı���״̬
 * �����0 -- �ɹ�  1 -- ʧ��
 */
int wanConnStatsChange(int status)
{
    int retVal = 0;

    uttSemP(SEM_CONNSTA_NO, 0);/*����*/
    retVal = wanConnStatsChangeNPV(status);
    uttSemV(SEM_CONNSTA_NO, 0);/*����*/
    return retVal;
}
#endif
#if (WEB_AUTH == FYES)
#if (WEBAUTH_AUTO == FYES)
#define WEBAUTH_MAX_LOG_LEN 128
#define WEBAUTH_MAX_LOG_NUM 2
int isStorageExist(char usb_mount_path[][MAX_PATH_LEN],int *count,unsigned int flag)
{
    char buf[256];
    char device_path[256];
    char mount_path[256];
    FILE *fp=NULL, *fp2=NULL;
    int i = 0;
    int device_exist = 0;

    fp = fopen("/proc/mounts", "r");
    if(!fp){
	perror(__FUNCTION__);
	return 0;
    }

    while(fgets(buf, sizeof(buf), fp)){
        device_exist = 0;

        if((flag & USB_EXIST) &&(strstr(buf, USB_STORAGE_SIGN)) ){
            device_exist = 1;
        }
        if((flag & SD_EXIST) &&(strstr(buf, SD_STORAGE_SIGN)) ){
            device_exist = 1;
        }

        if(device_exist){
            sscanf(buf, "%s %s", device_path, mount_path);
            fp2 = fopen(device_path, "r");
            if(fp2!=NULL)
            {
                if(usb_mount_path[i] != NULL)
                {
                    strcpy(usb_mount_path[i], mount_path);
                    strcat(usb_mount_path[i], "/");
                    i++;
                }
                fclose(fp2);
            }
        }
    }

    *count = i;
    fclose(fp);

    if(*count>0){
        return 1;
    }else{
        return 0;
    }
}
/*
*�����ļ�ϵͳʣ��ռ�
*KeHuatqo
*2011-04-29
*/
int storageFree(const char * path, long long *disk_size, long long *disk_free)
{
    struct statfs buf;
    int ret=0;

    /*��ʼ��*/
    memset(&buf, 0, sizeof(struct statfs));
    if(statfs(path, &buf)==0)
    {
#if 0
	/*ÿ��block����������ֽ���*/
	printf("block size   = %ld\n", buf.f_bsize);
	printf("total blocks = %ld\n", buf.f_blocks);
	printf("free  blocks = %ld\n", buf.f_bfree);
	printf("total size   = %ld MB\n", (buf.f_bsize * buf.f_blocks)>>20); 
	printf("free size    = %ld MB\n", (buf.f_bsize * buf.f_bfree)>>20);
#endif
	if(disk_size!=NULL)
	{
	    *disk_size = (long long)(buf.f_bsize) * (long long)(buf.f_blocks);
	}
	/*����ʣ��ռ�*/
	if(disk_free!=NULL)
	{
	    *disk_free = (long long)(buf.f_bfree) * (long long)(buf.f_bsize);
	}
#if 0
	printf("disk free size = %lld B\n", freeDisk);
#endif
    }
    else
    {
	/*����*/
	ret = -1;
    }

    return ret;
}

#define  SERVER_PORT 12345
#define  SERVERIP    "116.236.120.162"
#define  KEY_LEN     10

struct  LOG_MSG{
    char key[KEY_LEN + 2];
    char productSn[KEY_LEN + 2];
    char data[0];
};

/***********************************************************************
 ** �� �� ����   ConnectNonb
 ** �������ڣ�   2014-2-22
 ** �޸����ڣ�
 ** ���ߣ�       xu.jian
 ** ����˵����	�������Զ�˷�������ʱ����
 ************************************************************************/
int ConnectNonb(int sockfd, struct sockaddr *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    errno = 0;

    if ((n = connect(sockfd, saptr, salen)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            return EXIT_FAILURE;
        }
    }
    /*�������κ������Ĳ���*/
    if (n == 0)
    {
        goto done; // һ����ͬһ̨�������ã��᷵�� 0
    }
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset; // ������� block copy
    tval.tv_sec = nsec;
    tval.tv_usec = 0;

    /*���nsec Ϊ0����ʹ��ȱʡ�ĳ�ʱʱ�䣬����ṹָ��Ϊ NULL*/
    /*���tval�ṹ�е�ʱ��Ϊ0����ʾ�����κεȴ������̷���*/
    if ((n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0)
    {
        close(sockfd);
        errno = 10;
        return EXIT_FAILURE;
    }
    if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
    {
        len = sizeof(error);/* ������ӳɹ����˵��÷��� 0*/
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR,&error,&len))
                return EXIT_FAILURE;
    }else{
        printf("select error: sockfd not set");
    }

done:
         fcntl(sockfd, F_SETFL, flags); // �ָ�socket ����
    if (error)
    {
    close(sockfd);
    errno = error;
    return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int getSmsServerIp(char *ip)
{
    struct hostent *h;
    struct in_addr *p;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    printf("%s %d.........\n", __func__, __LINE__);
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((profG == NULL) || (ProfInstIsFree(profG)) || (profG->head.active == FALSE))
    {
	printf("%s %d: cannot get global parameters.\n", __func__, __LINE__);
	return 0;
    }

    printf("%s %d.........get host name: %s\n", __func__, __LINE__, profG->logServer);
    h = gethostbyname(profG->logServer);                            
    if (h != NULL)   
	strcpy(ip, inet_ntoa(*(struct in_addr *)h->h_addr));
    else                                                  
	return 0;                                          

    printf("%s %d.........get host ip: %s\n", __func__, __LINE__, ip);
    return 1;
}
/***********************************************************************
 ** �� �� ����   sendMsg2UTT
 ** �������ڣ�   2014-2-22
 ** �޸����ڣ�
 ** ���ߣ�       xu.jian
 ** ����˵����	����web��֤������־���͵�Զ��UTT������
 ************************************************************************/

int sendMsg2UTT(char *key,unsigned int productSn,char* data)
{
    int    sockfd;
    char   buffer[1024] = {0};
    struct sockaddr_in server_addr;
    int    nbytes;
    int    opt = SO_REUSEADDR;
    struct LOG_MSG  *sms;
    char   Base64String[1000] = {0};
    int    Base64StringLen = 0;
    sms =  (struct LOG_MSG*)buffer;
    char   serverIP[256] = {0};

    if(strlen(key) > KEY_LEN )
    {
        perror("argv error.\n");
        return 0;
    }
    memset(buffer,0,sizeof(buffer));

    memcpy(sms->key,key,strlen(key));
    sprintf(sms->productSn,"%d",productSn);
    memcpy(sms->data,data,strlen(data));

    Base64StringLen = Base64Encode(sms,Base64String,strlen(data) + sizeof(struct LOG_MSG));
    memcpy(buffer,Base64String,Base64StringLen);

    SWORD_PRINTF("%s------%dsizeof(struct LOG_MSG) :%d;strlen(data): %d ; Base64StringLen : %d\r\n", __func__, __LINE__,sizeof(struct LOG_MSG),strlen(data),Base64StringLen);

    if(!getSmsServerIp(serverIP))
    {
        memcpy(serverIP,SERVERIP,sizeof(SERVERIP));
    }
    if (inet_aton(serverIP, &server_addr.sin_addr) == 0) {
        fprintf(stderr, "the hostip is not right!");
        return 0;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket Error:%s\n\a", strerror(errno));
        return 0;
    }
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);
    if (ConnectNonb(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr),2) == -1) {
        fprintf(stderr, "Connect Error:%s\n\a", strerror(errno));
        return 0;
    }
    if(write(sockfd,buffer,Base64StringLen) == -1)
    {
        perror("send error.\n");
        return 0;
    }
    close(sockfd);
    return 0;
}

/***********************************************************************
 *  * �� �� ����   wevAuth2UTT
 *   * �������ڣ�   2014-2-13
 *    * �޸����ڣ�
 *     * ���ߣ�       ���Ĳ�
 *      * ����˵����	web��֤������־���͵�Զ��UTT������
 *       ***********************************************************************/
static void webAuth2UTT(int authType, char *authName, char *authMac, unsigned long authStartTime, unsigned long authStopTime)
{
#define MAX_AUTH_SEND_LEN 480
    static int sentFlag = 1;
    static unsigned char sendBuf[MAX_AUTH_SEND_LEN+100];

    if (sentFlag) {
	memset(sendBuf, 0, sizeof(sendBuf));
	sentFlag = 0;
    }

    sprintf(sendBuf, "%s%1d%02d%s%02X%02X%02X%02X%02X%02X%08X%08X",
	    sendBuf, authType, strlen(authName), authName, 
	    (unsigned char)authMac[0], (unsigned char)authMac[1], (unsigned char)authMac[2], 
	    (unsigned char)authMac[3], (unsigned char)authMac[4], (unsigned char)authMac[5], 
	    authStartTime, authStopTime);
#if 0
    printf("%s*******%d;%s%1d%02d%s%02X%02X%02X%02X%02X%02X%08X%08X\r\n",
        __func__,__LINE__,
	    sendBuf, authType, strlen(authName), authName, 
	    (unsigned char)authMac[0], (unsigned char)authMac[1], (unsigned char)authMac[2], 
	    (unsigned char)authMac[3], (unsigned char)authMac[4], (unsigned char)authMac[5], 
	    authStartTime, authStopTime);
#endif

    /* send out to UTT server */
#if 1
    SWORD_PRINTF("%s------%d\n,len:%d;sendBuf:%s\r\n", __func__, __LINE__,strlen(sendBuf),sendBuf);
#endif
    if (strlen(sendBuf) >= MAX_AUTH_SEND_LEN)
    {
	sentFlag = 1;
	sendMsg2UTT("UTT20",getSystemSn(),sendBuf);
	//printf("%s send: \n%s\n", __func__, sendBuf);
    }

    return;
}

/***********************************************************************
 *  * �� �� ����   webAuthLogWrite
 *   * �������ڣ�   2010-3-17
 *    * �޸����ڣ�
 *     * ���ߣ�       ���Ĳ�
 *      * ����˵����	web��֤������־д���ļ�
 *       ***********************************************************************/
static int webAuthLogWrite(int authType, char *logPath, char *logPathName, char *data, int dataLen)
{
    FILE *fp;
    long long total_space = 0, unused_space = 0;
    char  buf[WEBAUTH_MAX_LOG_LEN];

    /* ������ʣ��ռ��Ƿ��㹻 */
    storageFree(logPath, &total_space, &unused_space);  
    if (unused_space < 10*1024)
    {
	printf("%s %d: %s not enough server storage is available.\n", 
		__func__, __LINE__, logPath); 
	return 0;
    }

    /* ����ļ��Ƿ���ڣ��������ڣ����½� */
    fp = fopen(logPathName, "r");
    if(fp == NULL)
    {
	printf("%s %d: %s not exist, create it.\n", __func__, __LINE__, logPathName);
	fp = fopen(logPathName, "w");
	if (fp == NULL)
	{
	    printf("can't create file %s.\n", logPathName);
	    return 0;
	}

	if (authType == IDCARD_METHOD)
	    fprintf(fp, "%-18s\t%-17s\t%-19s\t%-19s\t%-8s\n------------------", "account", "mac", "startTime", "stopTime", "onlineTime");
	else if (authType == SMS_METHOD)
	    fprintf(fp, "%-11s\t%-17s\t%-19s\t%-19s\t%-8s\n-----------", "account", "mac", "startTime", "stopTime", "onlineTime");
	else if (authType == WECHAT_METHOD)
	    fprintf(fp, "%-30s\t%-17s\t%-19s\t%-19s\t%-8s\n------------------------------", "account", "mac", "startTime", "stopTime", "onlineTime");
	fprintf(fp, "------------------------------------------------------------------------------------\n");
    }
    fclose(fp);

    /* ��־��Ϣ�����ļ� */
    fp = fopen(logPathName, "a");
    if (fp == NULL)
    {
	printf("%s %d: can't open file %s.\n", __func__, __LINE__, logPathName);
	return 0;
    }
    if(fwrite(data, dataLen, 1, fp) != 1)
    {
	printf("write log error!\n");
    }

    fclose(fp);

    return 1;
}

/***********************************************************************
 *  * �� �� ����   _webAuthLog
 *   * �������ڣ�   2010-3-17
 *    * �޸����ڣ�
 *     * ���ߣ�       ���Ĳ�
 *      * ����˵����	web��֤������־д���ļ�
 *       ***********************************************************************/
static void _webAuthLog(char *logPath, int authType, char *authName, char *authMac, unsigned long authStartTime, unsigned long authStopTime)
{
    static unsigned long lastWriteTime = 0;
    static int idcardLogNum = 0, smsLogNum = 0, wechatLogNum = 0;
    static char smsLogData[WEBAUTH_MAX_LOG_LEN*WEBAUTH_MAX_LOG_NUM];
    static char wechatLogData[WEBAUTH_MAX_LOG_LEN*WEBAUTH_MAX_LOG_NUM];

    unsigned long usedTime = authStopTime - authStartTime;
    struct tm *tmStart = localtime(&authStartTime);
    struct tm *tmStop;
    struct tm *ptm;
    char logPathName[MAX_PATH_LEN];
    char *pData;

    if (authType == SMS_METHOD)
    {
	if (smsLogNum == 0)
	    memset(smsLogData, 0, sizeof(smsLogData));
	pData = smsLogData;
	sprintf(pData, "%s%-11s\t", pData, authName);
	smsLogNum++;
    }
    else if (authType == WECHAT_METHOD)
    {
	if (wechatLogNum == 0)
	    memset(wechatLogData, 0, sizeof(wechatLogData));
	pData = wechatLogData;
	sprintf(pData, "%s%-28s\t", pData, authName);
	wechatLogNum++;
    }
    else
    {
	printf("%s %d: authType %d error!\n", __func__, __LINE__, authType);
	return;
    }

    sprintf(pData, "%s%02X:%02X:%02X:%02X:%02X:%02X\t", pData, 
	    (unsigned char)authMac[0], (unsigned char)authMac[1], 
	    (unsigned char)authMac[2], (unsigned char)authMac[3], 
	    (unsigned char)authMac[4], (unsigned char)authMac[5]); 
    sprintf(pData, "%s%-04d-%02d-%02d %02d:%02d:%02d\t", pData, 
	    tmStart->tm_year+1900, tmStart->tm_mon+1, tmStart->tm_mday, 
	    tmStart->tm_hour, tmStart->tm_min, tmStart->tm_sec);
    tmStop = localtime(&authStopTime);
    sprintf(pData, "%s%-04d-%02d-%02d %02d:%02d:%02d\t", pData, 
	    tmStop->tm_year+1900, tmStop->tm_mon+1, tmStop->tm_mday, 
	    tmStop->tm_hour, tmStop->tm_min, tmStop->tm_sec);
    sprintf(pData, "%s%02d:%02d:%02d\n", pData, 
	    usedTime/3600, (usedTime%3600)/60, usedTime%60);

    if (idcardLogNum >= WEBAUTH_MAX_LOG_NUM)
    {
	sprintf(logPathName, "%sIdCardWebAuth_%04d%02d.txt", logPath, tmStop->tm_year+1900, tmStop->tm_mon+1);
	idcardLogNum = 0;
    }
    else if (smsLogNum >= WEBAUTH_MAX_LOG_NUM)
    {
	sprintf(logPathName, "%sSmsWebAuth_%04d%02d.txt", logPath, tmStop->tm_year+1900, tmStop->tm_mon+1);
	smsLogNum = 0;
    }
    else if (wechatLogNum >= WEBAUTH_MAX_LOG_NUM)
    {
	sprintf(logPathName, "%sWechatWebAuth_%04d%02d.txt", logPath, tmStop->tm_year+1900, tmStop->tm_mon+1);
	wechatLogNum = 0;
    } else {
	printf("%s %d: idcardLogNum: %d; smsLogNum: %d; wechatLogNum: %d\n", __func__, __LINE__, idcardLogNum, smsLogNum, wechatLogNum);
	return;
    }

    webAuthLogWrite(authType, logPath, logPathName, pData, strlen(pData));
    return;

}

/***********************************************************************
 *  * �� �� ����   webAuthLog
 *   * �������ڣ�   2014-2-13
 *    * �޸����ڣ�
 *     * ���ߣ�       ���Ĳ�
 *      * ����˵����	web��֤������־д���ļ�
 *       ***********************************************************************/
int webAuthLog(WebAuthProfile *prof, Uint32 ip, char *mac)
{
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int i, count = 0;

    unsigned int flag = 0u;
    unsigned long currentTime = time(0);

    static int firstFlag = 1;
    static char logPath[MAX_PATH_LEN];
    char  username[20];

    /* �״�����log��־����Ҫ�������ô��豸�Ƿ���� */
    if (firstFlag)
    {
	flag |= USB_EXIST;
	flag |= SD_EXIST;
	memset(device_path, 0, sizeof(device_path));
	memset(logPath, 0, sizeof(logPath));
	if(isStorageExist(device_path,&count,flag)==1){
	    strcpy(logPath, device_path[0]);
	    printf("%s %d: logPath: %s\n", __func__, __LINE__, logPath);
	} else {
	    printf("%s %d: no usb or sd card found.\n", __func__, __LINE__);
	}
	firstFlag = 0;
    }

    /* ���������֤�Ķ��������š�΢�����ɵ��˺� */
    memset(username, 0, sizeof(username));
    if (prof->autoAuthMethod == IDCARD_METHOD)
	strcpy(username, prof->idcode);
    else if ((prof->autoAuthMethod == SMS_METHOD) || (prof->autoAuthMethod == WECHAT_METHOD))
	strcpy(username, prof->user);
    else
	return;

#if 1
    printf("%s %d: user %s ip %#X mac: %02X%02X-%02X%02X-%02X%02X\n", 
	    __func__, __LINE__, username, ip, 
	    (unsigned char)mac[0], (unsigned char)mac[1], 
	    (unsigned char)mac[2], (unsigned char)mac[3], 
	    (unsigned char)mac[4], (unsigned char)mac[5]);
#endif
    /* ��־��Ϣд���ļ� */
    if (logPath[0])
    {
	_webAuthLog(logPath, prof->autoAuthMethod, username, mac, prof->timeStart, currentTime);
    }

    /* ��־��Ϣ���͵�UTT������ */
    webAuth2UTT(prof->autoAuthMethod, username, mac, prof->timeStart, currentTime);

    return;
}
#else
int webAuthLog(WebAuthProfile *prof, Uint32 ip, char *mac)
{
    return 0;
}
#endif
#endif
