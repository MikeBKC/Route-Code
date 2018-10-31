#include <stdio.h>
#include <unistd.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "mib.h"
#include "profacce.h"
#include "uttfunction.h"
#include "urcp_msg.h"
#include "translate.h"

#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    <uttMachine.h>
#include    "webmsg.h"
#include    "webs.h"

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#endif


#if  (AP_LOAD_BALANCE == FYES)

#ifdef  AP_LB_DEBUG
#define AP_LB_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define AP_LB_PRINTF(fmt, args...)
#endif



/********************************************************************
 * 函数： getIpGroupListData
 * 功能：
 * 创建： 2012-05-08
 * 参数：
 * 返回： 略
 * 输出： 略
 * 作者：
 ********************************************************************/
static int getApLbListData(int eid, webs_t wp, int argc, char **argv)
{

    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    ApLoadBalanceProfile *prof = NULL;
    SystemProfile *profSys = NULL;
    int min = 0, max = 0;
    int i,j,num=0;

    /*define web varliable*/
    websWrite(wp,"var APNames=new Array();\n");
    websWrite(wp,"var state=new Array();\n");
    websWrite(wp,"var ApCounts=new Array();\n");
    websWrite(wp,"var ThresValue=new Array();\n");
    websWrite(wp,"var DiffValue=new Array();\n");
    websWrite(wp,"var Detail=new Array();\n");
    websWrite(wp, T("var totalrecs;\n"));
    websWrite(wp, T("\n"));

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
	if(profSys->loadBalanceEn== FUN_ENABLE)
	{
	    websWrite(wp, "uttLoadBalanceEn=\"%s\";", T("on"));
	}
	else
	{
	    websWrite(wp, "uttLoadBalanceEn=\"%s\";",T("off"));
	}


    /*get inst max num and min num*/
    ProfInstLowHigh(mibType, &max, &min);
    for (j = min; j < max; j++) 
    {
        prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, j);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
        {
            websWrite(wp,"APNames[%d] = \"%s\";", num, prof->head.name);
//            websWrite(wp,"state[%d] = \"%s\";", num, prof->lbEn?"开启":"关闭");
            websWrite(wp,"state[%d] = \"%d\";", num, prof->lbEn);
            //            websWrite(wp,"ApCounts[%d] = \"%d\";", num, prof->num);

            websWrite(wp, "ApCounts[%d] = [ " ,num);
            for(i=0;i<prof->num;i++){
                websWrite(wp, "\"%u %s %s\"",prof->ap[i].sn,converMac6To12Byte(prof->ap[i].macAddr),prof->ap[i].dev_name );
                if(i+1<prof->num){
                    websWrite(wp, ",");
                }
            }
            websWrite(wp, " ];" );

            websWrite(wp,"ThresValue[%d] = \"%d\";", num, prof->threshold);

            num++;
        }
    }
    getTheErrorMsg(eid, wp, argc, argv);	/*get system error msg*/
    websWrite(wp,"var totalrecs=%d;",num);	/*output inst num*/
    websWrite(wp,"var max_totalrecs =%d;",max);	/*output max inst num*/

    return 0;
}
static void ApLbConfigAdd(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    ApLoadBalanceProfile *prof=NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    char_t *lbName = NULL, *lbEnable= NULL, *lbThre= NULL,*content=NULL;
    char *pList1=NULL;

    char macStr[20];
    char devName[MAX_DEV_NAME_LEN];
    uint32 sn;
    int i = 0;
    int min = 0, max = 0;
    int instIndex = 0;
    uint32 lbGroupNum;
    urcp_dev_t* dev=NULL;
    const char *ErrMsg = NULL;


    /*从页面空间获取值*/
    lbEnable = websGetVar(wp, T("enabledLoadBal"), T(""));
    lbName = websGetVar(wp, T("loadBalanceName"), T(""));
//    lbThre = websGetVar(wp, T("loadThresValue"), T(""));
    content = websGetVar(wp, T("temp1"), T(""));

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, i);	/*get inst pointer by index*/
        if((prof != NULL) && (strcmp(lbName, prof->head.name) == 0))
        {
//            setTheErrorMsgOnFree("已经有该名字的负载均衡组！");
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_EXIT_IN_GROUP);
            setTheErrorMsgOnFree((char *)ErrMsg);
	    /*出现重名，应直接跳转函数出口，而不是增加实例*/
	    goto OUT;
	}
    }


    AP_LB_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    instIndex = ProfGetInstIndexFree(mibType);/*获取一个空闲实例*/
    AP_LB_PRINTF("%s-%d:instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
    if(instIndex == PROFFAIL)
    {
//        setTheErrorMsgOnFree("已达最大条目！");
	    ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
	    setTheErrorMsgOnFree((char *)ErrMsg);
    }
    else
    {
        AP_LB_PRINTF("%s-%d , get free instIndex = %d\n", __func__, __LINE__, instIndex);
        prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, instIndex);
        if( prof != NULL ){
            memset(prof,0,sizeof(ApLoadBalanceProfile));
        }

        if(strcmp(lbName,"")!=0)
        {
            lbGroupNum = instIndex + 1;
            strcpy(prof->head.name,lbName);
            prof->head.active=1;
            prof->lbEn = !memcmp(lbEnable,"on",2);
//            prof->threshold = strtol(lbThre,NULL,10);
            prof->threshold = DEFAULT_THRESHOLD;
            prof->source_sum = MAX_SOURCE_SUM;
            if(strcmp(content, "")!=0)
            {
                i = 0;
                pList1=strtok(content, UTT_SPLIT_SIGN_RECORDS);
                AP_LB_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
                while(pList1!=NULL)
                {
                    if( pList1 != NULL ){
                        memset(devName,0,sizeof(devName));
                        memset(macStr,0,sizeof(macStr));
                        *(pList1+strlen(pList1)-2) = '\0';
                        sscanf(pList1,"%u %s %s",&sn,macStr,devName);
                        prof->ap[i].sn = sn;
                        converMac12To6Byte(macStr,&(prof->ap[i].macAddr));
                        memcpy(prof->ap[i].dev_name,devName,strlen(devName));

                        dev = urcp_dev_table_lookup(prof->ap[i].macAddr.addr,0u);
                        if( dev != NULL ){
                            prof->ap[i].isAlive = 1u;
                            prof->ap[i].client = dev->wl_info.clients;
                            prof->ap[i].isFull = 0u;
                            dev-> load_balance = lbGroupNum;
                        }else{
                            prof->ap[i].isAlive = 0u;
                            prof->ap[i].isFull = 0u;
                            prof->ap[i].client = 0u;
                        }

                    }
                    pList1=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
                    i++;
                    AP_LB_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
                }
            }
            prof->num = i;

            nvramWriteCommit(); /*save*/

            profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);

            if(profSys != NULL){
                if( prof->lbEn && (profSys->loadBalanceEn == FUN_ENABLE )){
                    /*发消息,通知交换机进程发包*/
                    sendwebmsgbuffer(MSG_TYPE_GET,SET_AP_LOAD_BALANCE, (char*)(&lbGroupNum), sizeof(lbGroupNum));
                }
            }
        }
    }
OUT:
    websRedirect(wp, "apLoadBalanceList.asp");
    return;
}
/********************************************************************
 * 函数： IpGroupConfigAdd
 * 功能： 组管理修改组配置
 * 创建： 2012-05-08
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static void ApLbConfigModify(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    ApLoadBalanceProfile *prof=NULL;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;

    char_t *lbEnable=NULL,*lbName = NULL, *lbNameOld = NULL, *lbThre=NULL,*content = NULL;
    char_t *pList1=NULL;
    int i = 0;
    int min = 0, max = 0;
    char devName[MAX_DEV_NAME_LEN];
    char macStr[20];
    uint32 sn,lbGroupNum,isAlive = 0u;
    urcp_dev_t* dev=NULL;
    urcp_send_load_balance_msg_t urcp_send_load_balance_msg;
    memset(&urcp_send_load_balance_msg, 0, sizeof(urcp_send_load_balance_msg_t));
    const char *ErrMsg = NULL;

	lbEnable = websGetVar(wp, T("enabledLoadBal"), T(""));
	lbName = websGetVar(wp, T("loadBalanceName"), T(""));
    lbNameOld = websGetVar(wp,T("loadBalanceNameOld"),T(""));
//	lbThre = websGetVar(wp, T("loadThresValue"), T(""));
	content = websGetVar(wp, T("temp1"), T(""));

    if( strcmp(lbName,lbNameOld) ){
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, i);	/*get inst pointer by index*/
        if((prof != NULL) && (strcmp(lbName, prof->head.name) == 0))
        {
//            setTheErrorMsgOnFree("已经有该名字的负载均衡组！");
            ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_EXIT_IN_GROUP);
            setTheErrorMsgOnFree((char *)ErrMsg);
	    goto OUT;
        }
    }
    }

    prof = ProfGetInstPointByName(mibType, lbNameOld);
    lbGroupNum = ProfGetInstIndexByName(mibType,lbNameOld) + 1;

#if 0
    if( strcmp(prof->head.name,"")){
        ProfDelInstByName(mibType,prof->head.name);
    }
#endif

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);

    if((prof!=NULL) && (profSys!=NULL)){

        for(i=0;i<prof->num;i++){
            dev = urcp_dev_table_lookup(prof->ap[i].macAddr.addr,0u);
            if( dev != NULL ){
                dev->load_balance = 0u;
            }

            memcpy(&(urcp_send_load_balance_msg.batch_info.dst_mac[MAC_LEN*i]),prof->ap[i].macAddr.addr,MAC_LEN);
            memset(&prof->ap[i],0,sizeof(Balance));
        }
        urcp_send_load_balance_msg.batch_info.number = i;

        if((i>0) && (profSys->loadBalanceEn == FUN_ENABLE))
        {
            urcp_send_load_balance_msg.client= MAX_SOURCE_SUM;
            /*发消息,通知交换机进程发包*/
            sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_LOAD_BALANCE, (char*)(&urcp_send_load_balance_msg), sizeof(urcp_send_load_balance_msg_t));
        }


        prof->num = 0u;
        strcpy(prof->head.name,lbName);
        prof->head.active=1;
        prof->lbEn = !memcmp(lbEnable,"on",2);
//        prof->threshold = strtol(lbThre,NULL,10);
        prof->threshold = DEFAULT_THRESHOLD;
        prof->source_sum = urcp_send_load_balance_msg.client;
        if(strcmp(content, "")!=0)
        {
            i = 0;
            pList1=strtok(content, UTT_SPLIT_SIGN_RECORDS);
            AP_LB_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
            while(pList1!=NULL)
            {
                if( pList1 != NULL ){
                    memset(devName,0,sizeof(devName));
                    memset(macStr,0,sizeof(macStr));
                    *(pList1+strlen(pList1)-2) = '\0';
                    sscanf(pList1,"%u %s %s",&sn,macStr,devName);
                    prof->ap[i].sn = sn;
                    converMac12To6Byte(macStr,&(prof->ap[i].macAddr));
                    memcpy(prof->ap[i].dev_name,devName,strlen(devName));

                    dev = urcp_dev_table_lookup(prof->ap[i].macAddr.addr,0u);
                    if( dev != NULL ){
                        prof->ap[i].isAlive = 1u;
                        prof->ap[i].client = dev->wl_info.clients;
                        prof->ap[i].isFull = 0u;
                        dev-> load_balance = lbGroupNum;
                        isAlive++;
                    }else{
                        prof->ap[i].isAlive = 0u;
                        prof->ap[i].client = 0u;
                        prof->ap[i].isFull = 0u;
                    }

                }
                pList1=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
                i++;
                AP_LB_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
            }
        }
        prof->num = i;

        nvramWriteCommit(); /*save*/

        if( (prof->lbEn) && (isAlive >= 2) && (profSys->loadBalanceEn==FUN_ENABLE)){
            sendwebmsgbuffer(MSG_TYPE_GET,SET_AP_LOAD_BALANCE, (char*)(&lbGroupNum), sizeof(lbGroupNum));
        }
    }
OUT:
    websRedirect(wp, "apLoadBalanceList.asp");
    return;
}
/*******************************************************************
 * 函数： formIpGroupConfig
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formApLbConfig(webs_t wp, char_t *path, char_t *query)
{
	char_t *action = NULL;

	action = websGetVar(wp,T("Action"), T(""));

	AP_LB_PRINTF("%s-%d, action = %s\n", __func__, __LINE__, action);
	/*add inst*/
	if (strcmp(action, "add") == 0) 
	{
		AP_LB_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ApLbConfigAdd(wp);
	}
	else    /*modify inst*/
	{
		ApLbConfigModify(wp);
	}
	return;
}

static int getApLbOneInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    ApLoadBalanceProfile *prof = NULL;
    int min = 0, max = 0;
    int i,j;
    const char *ErrMsg = NULL;

    char *editName = NULL;
    /*define web varliable*/
    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*查看实例名是否存在*/ 
    {
//	setTheErrorMsg("不存在此实例");
            ErrMsg = getTransDataForC(C_LANG_INDEX_CHANGED_CASE_NOT_EXITS);
            setTheErrorMsgOnFree((char *)ErrMsg);
    }
    else
    {
	AP_LB_PRINTF("%s-%d:editName=%s\n",__FUNCTION__,__LINE__,editName);
	ProfInstLowHigh(mibType, &max, &min);
	for (j = min; j < max; j++) 
	{
	    prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, j);	/*get inst pointer by index*/
	    if((prof != NULL) && (strcmp(editName, prof->head.name) == 0)&& (prof->head.active == TRUE))
	    {	/* find inst
		 * write inst info
		 */
//		websWrite(wp, "enables = %d;", prof->lbEn);
		websWrite(wp, "enables = \"%s\" ;", prof->lbEn?"on":"");
		websWrite(wp, "loadBalanceNames = \"%s\";", prof->head.name);
		websWrite(wp, "loadThresValues = \"%d\";", prof->threshold);

		websWrite(wp, "loadBalanceList = { " );
        for(i=0;i<prof->num;i++){
		websWrite(wp, "\"%-12u%-16s%s \":%d ",prof->ap[i].sn,converMac6To12Byte(prof->ap[i].macAddr),prof->ap[i].dev_name,prof->ap[i].isAlive );
        if(i+1<prof->num){
		websWrite(wp, ",");
        }
        }

		websWrite(wp, " };" );

	    }
    }
    }
    websWrite(wp,"var max_apnum_in_lb_group = %d;\n",BAL_NUM);
    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/

    return 0;

}
static int getApEnvInfo(int eid, webs_t wp, int argc, char **argv)
{
    char buff[128],*p;
    MacAddr macaddr;
    urcp_shm_t *mem_shadow=NULL;
    int i,num=0;

    mem_shadow = get_shm_ptr();
    websWrite(wp,"var apLists=new Array();\n");
    for(i=0;i<DEV_TABLE_SIZE;i++){
        if( (mem_shadow->dev_table[i].used == 1) 
                && (mem_shadow->dev_table[i].load_balance == 0)
                && mem_shadow->dev_table[i].aged != 2 ){
            memset(buff,0,sizeof(buff));
            memcpy(macaddr.addr,mem_shadow->dev_table[i].mac,MAC_LEN);
            p =  buff;
            p += sprintf(p,"%-12u",mem_shadow->dev_table[i].serial_no);
//            p += sprintf(p,"%s ",str_macaddr(mem_shadow->dev_table[i].mac));
            p += sprintf(p,"%-16s",converMac6To12Byte(macaddr));
            p += sprintf(p,"%s ",mem_shadow->dev_table[i].dev_name);
            websWrite(wp,"apLists[%d]=\"%s\";\n",num,buff);
            num++;
        }
    }
    websWrite(wp,"var max_apnum_in_lb_group = %d;\n",BAL_NUM);

return 0;
}
/*******************************************************************
 * 函数： formIpGroupListDel
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formApLbListDel(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
	ApLoadBalanceProfile *prof = NULL;

	char_t *lbName = NULL; 
	char_t *tmp = NULL;
    urcp_dev_t* dev=NULL;
	const char *ErrMsg = NULL;
    int i;

    urcp_send_load_balance_msg_t urcp_send_load_balance_msg;
    memset(&urcp_send_load_balance_msg, 0, sizeof(urcp_send_load_balance_msg_t));

    a_assert(wp);
	/*
	 * 删除所有选择的PROFILE
	 */
	lbName = websGetVar(wp,T("delstr"),T("")); 
	AP_LB_PRINTF("%s-%d, get delstr = %s\n", __FUNCTION__, __LINE__, lbName);
	if (strcmp(lbName,"")==0) 
	{
//		setTheErrorMsg("请选择要删除的条目");
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_NAME_NOT_FIND);
		setTheErrorMsg(ErrMsg);
	}
	else
	{
		tmp = strtok(lbName, UTT_SPLIT_SIGN_STR);
		while(tmp != NULL) 
		{
			prof=(ApLoadBalanceProfile *)ProfGetInstPointByName(mibType, tmp);
            if(prof != NULL)
            {
                for(i=0;i<prof->num;i++){
                    dev = urcp_dev_table_lookup(prof->ap[i].macAddr.addr,0u);
                    if( dev != NULL ){
                        dev->load_balance = 0u;
                    }

                    memcpy(&(urcp_send_load_balance_msg.batch_info.dst_mac[MAC_LEN*i]),prof->ap[i].macAddr.addr,MAC_LEN);
                }
                urcp_send_load_balance_msg.batch_info.number = i;

                if(i>0)
                {
                    urcp_send_load_balance_msg.client= MAX_SOURCE_SUM;
                    /*发消息,通知交换机进程发包*/
                    sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_LOAD_BALANCE, (char*)(&urcp_send_load_balance_msg), sizeof(urcp_send_load_balance_msg_t));
                }


            ProfDelInstByName(mibType,tmp);

			}
			tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
		}

		nvramWriteCommit(); /*save*/
	}
	websRedirect(wp, "apLoadBalanceList.asp");

	return;	   
}
/*******************************************************************
 * 函数： formIpGroupListDelAll
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formApLbListDelAll(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType=MIB_PROF_AP_LOAD_BALANCE;
	ApLoadBalanceProfile *prof=NULL;

	int i=0,j=0;
	int min,max;
    urcp_dev_t* dev=NULL;
    urcp_send_load_balance_msg_t urcp_send_load_balance_msg;
    memset(&urcp_send_load_balance_msg, 0, sizeof(urcp_send_load_balance_msg_t));
	/*get inst max num and min num*/
	ProfInstLowHigh(mibType,&max,&min);
	for(i=min;i<max;i++)
	{
		prof=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
		if((prof!=NULL)&&(ProfInstIsFree(prof)==0)&& (prof->head.active == TRUE))
		{
			AP_LB_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);

            if( strcmp(prof->head.name,"")){

                for(j=0;j<prof->num;j++){
                    dev = urcp_dev_table_lookup(prof->ap[j].macAddr.addr,0u);
                    if( dev != NULL ){
                        dev->load_balance = 0u;
                    }

                    memcpy(&(urcp_send_load_balance_msg.batch_info.dst_mac[MAC_LEN*j]),prof->ap[j].macAddr.addr,MAC_LEN);
                }
                urcp_send_load_balance_msg.batch_info.number = j;
                if(j>0)
                {
                    urcp_send_load_balance_msg.client= MAX_SOURCE_SUM;
                    /*发消息,通知交换机进程发包*/
                    sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_LOAD_BALANCE, (char*)(&urcp_send_load_balance_msg), sizeof(urcp_send_load_balance_msg_t));
                }

				ProfDelInstByIndex(mibType, i);
            }
		}
	}
	AP_LB_PRINTF("%s-%d:-----\n",__FUNCTION__,__LINE__);
	nvramWriteCommit(); /*save*/

	websRedirect(wp, "apLoadBalanceList.asp");	/*页面跳转*/
	return;
}
/*******************************************************************
 * 函数： formIpGroupConfig
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formApLbGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *Enable = NULL,lbGroupNum;
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    FunEnableEnum enable = FUN_DISABLE;
	int min,max,i,j;
    urcp_dev_t* dev=NULL;
    uint32 isAlive = 0u;

	MibProfileType mibType=MIB_PROF_AP_LOAD_BALANCE;
	ApLoadBalanceProfile *prof=NULL;
    urcp_send_load_balance_msg_t urcp_send_load_balance_msg;

    memset(&urcp_send_load_balance_msg, 0, sizeof(urcp_send_load_balance_msg_t));

    Enable = websGetVar(wp, "GlobalEnable", T(""));
    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys != NULL)
    {
        if(strcmp(Enable, "on") == 0) {
            enable = FUN_ENABLE;
        }
        if(profSys->loadBalanceEn != enable){
	        ProfInstLowHigh(mibType,&max,&min);
            if( enable == FUN_ENABLE ){
                for(i=min;i<max;i++)
                {
                    prof=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
                    if((prof!=NULL)&&(ProfInstIsFree(prof)==0)&& (prof->head.active == TRUE))
                    {
                        AP_LB_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);

                        if( prof->lbEn ){
                            for(j=0;j<prof->num;j++){
                                if(prof->ap[j].isAlive){
                                    dev = urcp_dev_table_lookup(prof->ap[j].macAddr.addr,0u);
                                    if( dev != NULL ){
                                        prof->ap[i].client = dev->wl_info.clients;
                                        prof->ap[i].isFull = 0u;
                                    }
                                    isAlive++;
                                }
                            }

                            /*发消息,通知交换机进程发包*/
                            lbGroupNum = (char) (i+1);
                            AP_LB_PRINTF("%s-%d: load balance %d\n",__FUNCTION__,__LINE__,lbGroupNum);

                            if(isAlive >= 2){
                            sendwebmsgbuffer(MSG_TYPE_GET,SET_AP_LOAD_BALANCE, (char*)(&lbGroupNum), sizeof(lbGroupNum));
                            }
                        }
                    }
                }
            }else{
                for(i=min;i<max;i++)
                {
                    prof=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
                    if((prof!=NULL)&&(ProfInstIsFree(prof)==0)&& (prof->head.active == TRUE))
                    {
                        AP_LB_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);

                        if( prof->lbEn ){
                            for(j=0;j<prof->num;j++){
                                memcpy(&(urcp_send_load_balance_msg.batch_info.dst_mac[MAC_LEN*j]),prof->ap[j].macAddr.addr,MAC_LEN);
                            }
                            urcp_send_load_balance_msg.batch_info.number = j;
                            if(j>0)
                            {
                                urcp_send_load_balance_msg.client= MAX_SOURCE_SUM;
                                prof->source_sum = urcp_send_load_balance_msg.client;
                                /*发消息,通知交换机进程发包*/
                                sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_LOAD_BALANCE, (char*)(&urcp_send_load_balance_msg), sizeof(urcp_send_load_balance_msg_t));
                            }
                        }
                    }
                }

            }

            profSys->loadBalanceEn = enable;
            nvramWriteCommit();
        }
    }
    websRedirect(wp, "apLoadBalanceList.asp");/*页面跳转*/
    return;

}


extern void websDefineApLoadBalance(void)
{
	websFormDefine(T("formApLbGlobalConfig"), formApLbGlobalConfig);
	websFormDefine(T("formApLbConfig"), formApLbConfig);
	websFormDefine(T("formApLbListDel"), formApLbListDel);
	websFormDefine(T("formApLbListDelAll"), formApLbListDelAll);
	websAspDefine(T("aspOutputApLbListData"), getApLbListData);
	websAspDefine(T("aspOutputApLbOneInfo"), getApLbOneInfo);
	websAspDefine(T("aspOutputApEnvInfo"), getApEnvInfo);
}

#endif
