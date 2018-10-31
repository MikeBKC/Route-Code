#include "uttMachine.h"
#if (L2TP_CLIENT == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#include "utt_conf_define.h"
#include "utt_pppoeClient.h" 

//#define L2TP_DEBUG

#ifdef L2TP_DEBUG
#define L2TP_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define L2TP_PRINT(fmt, args...)
#endif

#define IP_SEG_LEN  40
#define IP_LEN	    20

/*
 * 杀所有l2tp客户端进程
 */
static void killAllL2tpClient(void) {
    int fd = -1;
    struct st_uttPppdClient info;
    int st_size = sizeof(struct st_uttPppdClient);
    fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDONLY);

    if(fd != -1) {
        while (read(fd, &info, st_size) > 0) {
	    if (info.ConTime == 0) {
		doSystem("kill -INT %d", info.pid);
	    }
        }
	close(fd);
	doSystem("rm -f %s 2>/dev/null", UTT_L2TP_CLIENT_INFO_FILE);
    }
    return;
}


/*
 * l2tp client初始化
 */
int  checkl2tpCliExistOrNum(void) {
#if 1
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof=NULL;
    int min = 0, max = 0, i = 0;
    int count = 0;

    L2TP_PRINT("%s-%d: \n", __FUNCTION__,__LINE__);

    /*
     * 初始化账号配置
     */
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        prof = (L2TPCliProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    count++;
	}
    }

#endif
    L2TP_PRINT("%s-%d: count=%d\n",__func__,__LINE__,count);
    return count;
}

/*
 * addL2tpClient_xl2tpdConfig
 * 添加客户端账号的xl2tpd文件配置
 * */
void addL2tpClient_xl2tpdConfig(L2TPCliProfile *prof)
{
    if(prof!=NULL) 
    {
	doSystem("l2tpCliAccAdd.sh %s %s %s %s",prof->head.name,prof->userName,prof->serverHost,getEnumString(pppAuthEnum, prof->pppAuthMode));
	L2TP_PRINT("%s-%d: l2tpCliAccAdd.sh %s %s %s %s\n",__func__,__LINE__,prof->head.name,prof->userName,prof->serverHost,getEnumString(pppAuthEnum, prof->pppAuthMode));
    }
    return ;
}

/*
 * addL2tpClient_pppdfile
 * 添加客户端的pppd配置文件
 * */
void addL2tpClient_pppdfile(L2TPCliProfile *prof)
{
    if(prof!=NULL) {
	/*添加l2tp客户端的pppd配置文件*/
	if(prof->mtu > 0) {
	    doSystem("l2tp.sh %s %s %s %u %s >%s%s",prof->head.name,prof->userName,prof->passwd,prof->mtu,getEnumString(pppAuthEnum, prof->pppAuthMode),L2TP_CLI_FILE_PREFIX,prof->head.name);
	    L2TP_PRINT("%s-%d: l2tp.sh %s %s %s %u %s >%s%s\n",__func__,__LINE__,prof->head.name,prof->userName,prof->passwd,prof->mtu,getEnumString(pppAuthEnum, prof->pppAuthMode),L2TP_CLI_FILE_PREFIX,prof->head.name);
	} else {
	    doSystem("l2tp.sh %s %s %s %u %s >%s%s",prof->head.name,prof->userName,prof->passwd,1410,getEnumString(pppAuthEnum, prof->pppAuthMode),L2TP_CLI_FILE_PREFIX,prof->head.name);
	    L2TP_PRINT("%s-%d: l2tp.sh %s %s %s %u %s >%s%s\n",__func__,__LINE__,prof->head.name,prof->userName,prof->passwd,1410,getEnumString(pppAuthEnum, prof->pppAuthMode),L2TP_CLI_FILE_PREFIX,prof->head.name);
	}
	doSystem("echo 0 >/var/run/l2tp_%slink.status",prof->head.name);
    }
#if (VPN_PROXY == FYES)
    doSystem("iptables -t nat -D POSTROUTING -j pptp_default_nat;iptables -t nat -N pptp_default_nat;iptables -t nat -A POSTROUTING -j pptp_default_nat");
#endif
    return ;
}

/*
 * l2TpClient_ConnOrDel
 * 挂断此客户端连接
 * 参数：flag
 * 1 --- 建立连接
 * 0 --- 挂断连接
 * */
void l2TpClient_ConnOrDel(L2TPCliProfile *prof, int flag)
{
    if(prof!=NULL) {
	if(flag == 1) {
	    sleep(1);
	    doSystem("echo 'c %s' > %s",prof->head.name, L2TP_CONTROL_PIPE);
	    L2TP_PRINT("echo 'c %s' > %s",prof->head.name, L2TP_CONTROL_PIPE);
	} else {
	    doSystem("l2tp-down.sh l2tp_%s ",prof->head.name);
	    usleep(100000);
	    //doSystem("echo 'd %s' > %s",prof->head.name, L2TP_CONTROL_PIPE);
	    //L2TP_PRINT("echo 'd %s' > %s",prof->head.name, L2TP_CONTROL_PIPE);
	}
    }

    return ;
}

/*
 * l2TpClient_up
 * 添加或者修改配置时，调用此函数
 * 在不重启xl2tpd进程的情况下，将client配置添加到
 * xl2tpd进程中，并启动此链接
 * */
void l2TpClient_up(L2TPCliProfile *prof)
{
    char cmd[1024];
    char authStr[70];
#if (VPN_PROXY == FYES)
    if(prof->head.active==1) 
    {
	prof->connectStatus=1;
    }
    else 
    {
	prof->connectStatus=0;
	return;
    }
#endif
    
    memset(cmd, 0, sizeof(cmd));
    memset(authStr, 0, sizeof(authStr));
    if(prof!=NULL) {
	addL2tpClient_pppdfile(prof);
	if(prof->pppAuthMode == PPP_AUTH_ANY) {
	    snprintf(authStr,70,"%s","require pap=yes require chap=yes require authentication=yes ");
	} else if(prof->pppAuthMode == PPP_AUTH_CHAP){
	    snprintf(authStr,70,"%s","require chap=yes refuse pap=yes require authentication=yes ");
	} else if(prof->pppAuthMode == PPP_AUTH_PAP) {
	    snprintf(authStr,70,"%s","require pap=yes refuse chap=yes require authentication=yes ");
	} else {    /*PPP_AUTH_NONE*/
	    snprintf(authStr,70,"%s","refuse pap=yes refuse chap=yes require authentication=yes ");
	}
	snprintf(cmd,1024,"xl2tpd-control add %s name=%s lns=%s %s length bit=yes pppoptfile=/etc/options.l2tp_%s ppp debug=yes autodial=yes",prof->head.name,prof->userName,prof->serverHost,authStr,prof->head.name); /*将账号配置添加到xl2tpd服务的laclist中客户端链表中*/
	L2TP_PRINT("%s-%d: cmd =%s\n",__func__,__LINE__,cmd);
	doSystem(cmd);
#if 0
	sleep(1);
	l2TpClient_ConnOrDel(prof,1);
#endif
    }

    return ;
}

/*
 * l2TpClient_remove
 * 修改配置时，调用此函数
 * 将xl2tpd进程中laclist客户端链表中查找并挂断连接，然后删除配置
 * */
void l2TpClient_remove(L2TPCliProfile *prof)
{
    char cmd[80];
    if(prof!=NULL) {
	doSystem("l2tpSrvUp.sh");
	l2TpClient_ConnOrDel(prof, 0);
	usleep(100000);
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd,80,"xl2tpd-control remove %s ",prof->head.name); /*xl2tpd的laclist中客户端链表中删除配置*/
	L2TP_PRINT("%s-%d: cmd =%s\n",__func__,__LINE__,cmd);
	doSystem(cmd);
	doSystem("rm -f %s%s &",L2TP_CLI_FILE_PREFIX,prof->head.name);
	//doSystem("l2tp-down.sh l2tp_%s &",prof->head.name);
    }

    return ;
}

/*
 * l2tp client初始化
 */
int  l2tpClientInit(void) {
#if 1
    MibProfileType mibType = MIB_PROF_L2TP_CLIENT;
    L2TPCliProfile *prof=NULL;
    int min = 0, max = 0, i = 0;
    int ret = 0;

    L2TP_PRINT("%s-%d: \n", __FUNCTION__,__LINE__);

    /*
     * 初始化账号配置
     */
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        prof = (L2TPCliProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    sleep(1);
	    addL2tpClient_xl2tpdConfig(prof);
#if (VPN_PROXY == FYES)
	    if(prof->head.active == 1)
	    {
		prof->connectStatus = 1;
	    }
	    else
	    {
		prof->connectStatus = 0;
		return 1;
	    }
#endif
	    addL2tpClient_pppdfile(prof);
	    ret = 1;
	}
    }

#endif
    L2TP_PRINT("%s-%d: \n",__func__,__LINE__);
    return ret;
}

int checkL2tpServerGloEn()
{
    L2TPSrvGlobalProfile *profL2tpS;
    MibProfileType mibTypeGlobal = MIB_PROF_L2TP_SRV_GLOBAL;
    profL2tpS = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, 0);
    if (profL2tpS->head.active == 1) {
	return 1;
    } else {
	return 0;
    }

}
/*
 * l2tp serve 账号profile change函数
 */
static void l2tpCliAccountProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    L2TPCliProfile *newProf = (L2TPCliProfile *)newProfileDataPtr;
    L2TPCliProfile *oldProf = (L2TPCliProfile *)oldProfileDataPtr;

    switch(changeType) {
        case PROF_CHANGE_ADD:
	    /*
	     * 配置文件加一个账号
	     */
	    L2TP_PRINT("%s-%d: PROF_CHANGE_ADD\n",__func__,__LINE__);
	    L2TP_PRINT("%s-%d: newProf->head.active =%d, head.name =%s, newProf->username =%s, newProf->passwd =%s, \
		    newProf->pppAuthMode =%d, newProf->remoteLanIp =0x%x, newProf->remoteLanIpDis =0x%x, newProf->remoteNetMask =0x%x, \
		    newProf->serverHost =%s, newProf->mtu =%d\n",__func__,__LINE__,newProf->head.active,newProf->head.name,newProf->userName,\
		    newProf->passwd,newProf->pppAuthMode,newProf->remoteLanIp,newProf->remoteLanIpDis, newProf->remoteLanNetmask,\
		    newProf->serverHost,newProf->mtu);
	    if(checkl2tpCliExistOrNum() ==1) {
	    if(checkL2tpServerGloEn() == 0) {
		doSystem("l2tpSrvUp.sh");
		sleep(3);
	    }
	    }
	    l2TpClient_up(newProf);
	    break;
        case PROF_CHANGE_EDIT:
	    /*
	     * 先删除对应账号，再添加对应账号
	     */	    
	    L2TP_PRINT("%s-%d: PROF_CHANGE_EDIT\n",__func__,__LINE__);
	    L2TP_PRINT("%s-%d: oldProf->head.active =%d, head.name =%s, oldProf->username =%s, oldProf->passwd =%s, \
		    oldProf->pppAuthMode =%d, oldProf->remoteLanIp =0x%x, oldProf->remoteLanIpDis =0x%x, oldProf->remoteNetMask =0x%x, \
		    oldProf->serverHost =%s, oldProf->mtu =%d\n",__func__,__LINE__,oldProf->head.active,oldProf->head.name,oldProf->userName,\
		    oldProf->passwd,oldProf->pppAuthMode,oldProf->remoteLanIp,oldProf->remoteLanIpDis, oldProf->remoteLanNetmask,\
		    oldProf->serverHost,oldProf->mtu);
	    L2TP_PRINT("%s-%d: newProf->head.active =%d, head.name =%s, newProf->username =%s, newProf->passwd =%s, \
		    newProf->pppAuthMode =%d, newProf->remoteLanIp =0x%x, newProf->remoteLanIpDis =0x%x, newProf->remoteNetMask =0x%x, \
		    newProf->serverHost =%s, newProf->mtu =%d\n",__func__,__LINE__,newProf->head.active,newProf->head.name,newProf->userName,\
		    newProf->passwd,newProf->pppAuthMode,newProf->remoteLanIp,newProf->remoteLanIpDis, newProf->remoteLanNetmask,\
		    newProf->serverHost,newProf->mtu);
#if (VPN_PROXY == FYES)
	    if((oldProf->head.active == 0) && (newProf->head.active == 0)) break;
#endif
	    l2TpClient_remove(oldProf);
	    l2TpClient_up(newProf);
	    break;
        case PROF_CHANGE_DEL:
	    /*
	     * 配置文件删除对应参数
	     */
	    L2TP_PRINT("%s-%d: PROF_CHANGE_DEL\n",__func__,__LINE__);
	    L2TP_PRINT("%s-%d: oldProf->head.active =%d, head.name =%s, oldProf->username =%s, oldProf->passwd =%s, \
		    oldProf->pppAuthMode =%d, oldProf->remoteLanIp =0x%x, oldProf->remoteLanIpDis =0x%x, oldProf->remoteNetMask =0x%x, \
		    oldProf->serverHost =%s, oldProf->mtu =%d\n",__func__,__LINE__,oldProf->head.active,oldProf->head.name,oldProf->userName,\
		    oldProf->passwd,oldProf->pppAuthMode,oldProf->remoteLanIp,oldProf->remoteLanIpDis, oldProf->remoteLanNetmask,\
		    oldProf->serverHost,oldProf->mtu);
	    l2TpClient_remove(oldProf);
	    break;
        case PROF_CHANGE_DELALL:
	    /*
	     * 配置文件删除所有账号
	     */
	    L2TP_PRINT("%s-%d: PROF_CHANGE_DELALL\n",__func__,__LINE__);
	    L2TP_PRINT("%s-%d: oldProf->head.active =%d, head.name =%s, oldProf->username =%s, oldProf->passwd =%s ",
		    __func__,__LINE__,oldProf->head.active,oldProf->head.name,oldProf->userName, oldProf->passwd);
	    l2TpClient_remove(oldProf);
	    break;
        default:
	    break;
    };
}

/*
 * 初始化函数
 */
extern void funInitL2tpClient(void) {
    registerForProfileUpdates(MIB_PROF_L2TP_CLIENT, NULL, l2tpCliAccountProfileChange);
    //l2tpClientInit();
    return;
}
#endif
