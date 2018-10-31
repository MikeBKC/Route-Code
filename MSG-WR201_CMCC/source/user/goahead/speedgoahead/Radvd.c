#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"

#if (IPV6_RADVD == FYES)

#include "Radvd.h"

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Ipv6RadvdPrefixChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Ipv6PrefixListProfile *oldProfileDataPtr, const Ipv6PrefixListProfile *newProfileDataPtr );
static void Ipv6RadvdGlobalChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const RadvdProfile *oldProfileDataPtr, const RadvdProfile *newProfileDataPtr );
static void RadvdIsatapAdd(IsatapProfile *prof);
static void RadvdPrefixinit(void);
extern void Radvdinit(void);
static void RadvdPrefixAdd(Ipv6PrefixListProfile *prof);
static void RadvdPrefixAddLan(Ipv6InterfaceProfile *prof);
static void RadvdGlobalConfig(RadvdProfile *prof);
static void RadvdIsatapAdd(IsatapProfile *prof);

/********************************************************************
 * * 函数： RadvdIsatapAdd
 * * 功能： Isatap隧道接口radvd配置下发
 * * 创建： 2014-10-24
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void RadvdIsatapAdd(IsatapProfile *prof)
{
    FILE *fp;
    if((fp = fopen("etc/radvd.conf","a+")) == NULL)
    {    
        printf("open radvd.conf is error!\n");
        return;
    }   
    fprintf(fp,"interface is0\n"); 
    fprintf(fp,"{\n"); 
    fprintf(fp,"AdvSendAdvert on;\n"); 
    fprintf(fp,"UnicastOnly on;\n"); 
    fprintf(fp,"prefix %s/64\n",prof->ISATAP_IPv6Pre); 

    fprintf(fp,"{\n"); 
    fprintf(fp,"AdvOnLink on;\n"); 
    fprintf(fp,"AdvRouterAddr on;\n");
    fprintf(fp,"};\n"); 
    fprintf(fp,"};\n");
    fclose(fp); 
}
/********************************************************************
 * * 函数： RadvdPrefixAdd
 * * 功能： RADVD通告前缀单条实例添加函数
 * * 创建： 2014-09-29
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void RadvdPrefixAdd(Ipv6PrefixListProfile *prof)
{
    FILE *fp;
    char AutoEnabled[4];
    if((fp = fopen("etc/radvd.conf","a+")) == NULL)
    {    
	printf("open radvd.conf is error!\n");
        return;
    }
    if(prof->head.active == 1){
	strcpy(AutoEnabled,"on");
    }else{
	strcpy(AutoEnabled,"off");
    }
    fprintf(fp,"prefix %s/%d\n",prof->DesPrefix,prof->DesPrefixLength);
    fprintf(fp,"{\n");
    fprintf(fp,"AdvOnLink on;\n");
    fprintf(fp,"AdvAutonomous %s;\n",AutoEnabled);
    fprintf(fp,"AdvRouterAddr off;\n");
    fprintf(fp,"AdvValidLifetime %d;\n",prof->PrefixstaleTime);
    fprintf(fp,"AdvPreferredLifetime %d;\n",prof->PrefixstaleTime/2);
    fprintf(fp,"};\n");
    fclose(fp);  
}
/********************************************************************
 * * 函数： RadvdPrefixAddLan
 * * 功能： RADVD通告Lan口地址前缀添加
 * * 创建： 2014-10-11
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void RadvdPrefixAddLan(Ipv6InterfaceProfile *prof)
{
    FILE *fp;
    char enabled[4];
    if((fp = fopen("etc/radvd.conf","a+")) == NULL)
    {    
	printf("open radvd.conf is error!\n");
        return;
    }
    if(prof->LanAutoEnabled == 1){
	strcpy(enabled,"on");
    }else{
	strcpy(enabled,"off");
    }
    fprintf(fp,"prefix %s/%d\n",prof->ipv6LAN_addr,prof->LANprefixLength);
    fprintf(fp,"{\n");
    fprintf(fp,"AdvOnLink on;\n");
    fprintf(fp,"AdvAutonomous %s;\n",enabled);
    fprintf(fp,"AdvRouterAddr off;\n");
    fprintf(fp,"AdvValidLifetime 9000;\n");
    fprintf(fp,"AdvPreferredLifetime 4500;\n");
    fprintf(fp,"};\n");
    fclose(fp);  
}


/********************************************************************
 * * 函数： RadvdPrefixinit
 * * 功能： RADVD通告前缀实例添加函数
 * * 创建： 2014-09-29
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void RadvdPrefixinit()
{
    MibProfileType mibType = MIB_PROF_IPV6_NOTLISTPREFIXS;
    Ipv6PrefixListProfile  *prof = NULL;
    Ipv6InterfaceProfile *profIf = NULL;
    int min = 0, max = 0, i = 0;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++){
	prof = (Ipv6PrefixListProfile *)ProfGetInstPointByIndex(mibType, i); //从profile中读配置
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)){
//                printf("####%d------%s\n",__LINE__,__func__);
		RadvdPrefixAdd(prof);        //逐条向配置文件中写入实例
	    }

	}
   profIf = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_IPV6_INTERFACE, 0);
      if((profIf != NULL) && (ProfInstIsFree(profIf) == 0) && (strcmp(profIf->ipv6LAN_addr,"") != 0)){
	  RadvdPrefixAddLan(profIf);
      }
}

/********************************************************************
 * * 函数： Radvdinit
 * * 功能： RADVD初始化函数（重新生成配置文件）
 * * 创建： 2014-09-29
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
extern void Radvdinit()
{
    FILE *fp;
    MibProfileType mibType=MIB_PROF_RADVD;
    RadvdProfile   *prof=NULL;
    IsatapProfile *profIsatap=NULL;
    prof=(RadvdProfile *)ProfGetInstPointByIndex(mibType, 0);
    profIsatap=(IsatapProfile *)ProfGetInstPointByIndex(MIB_PROF_ISATAP, 0);
    RadvdGlobalConfig(prof);
    RadvdPrefixinit();
    if((fp = fopen("etc/radvd.conf","a+")) == NULL)
    {    
	printf("open radvd.conf is error!\n");
        return;
    }
    fprintf(fp,"};\n");
    fclose(fp);
    if(profIsatap->ISATAPEnabled == 1){
	RadvdIsatapAdd(profIsatap);
    }
    doSystem("killall radvd");
    if(prof->head.active == 1){
	doSystem("radvd -C /etc/radvd.conf -d 1&");
    }
}

/********************************************************************
 * * 函数： RadvdGlobalConfig
 * * 功能： RADVD全局参数写配置文件函数
 * * 创建： 2014-09-29
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void RadvdGlobalConfig(RadvdProfile *prof)
{
    FILE *fp;
    char enabled[4],manage[4],other[4],Type[7];
    char *port = getLanIfName();
    if(prof->head.active == 1){
        strcpy(enabled,"on");
    }else{
	strcpy(enabled,"off");
    }
    if(prof->manageEnabled == 1){
	strcpy(manage,"on");
    }else{
	strcpy(manage,"off");
    }
    if(prof->otherEnabled == 1){
	strcpy(other,"on");
    }else{
	strcpy(other,"off");
    }
    if(prof->routerPreferences == 1){
	strcpy(Type,"low");
    }else if(prof->routerPreferences == 2){
	strcpy(Type,"medium");
    }else{
	strcpy(Type,"high");
    }
    if((fp = fopen("etc/radvd.conf","w+")) == NULL)
    {    
	printf("open radvd.conf is error!\n");
        return;
    }
    fprintf(fp,"interface %s\n",port);
    fprintf(fp,"{\n");
    fprintf(fp,"AdvSendAdvert %s;\n",enabled);
    fprintf(fp,"AdvManagedFlag %s;\n",manage);
    fprintf(fp,"AdvOtherConfigFlag %s;\n",other);
    fprintf(fp,"MaxRtrAdvInterval %d;\n",prof->noticeInterval);
    fprintf(fp,"MinRtrAdvInterval %d;\n",(int)((float)(prof->noticeInterval)*0.75));
    fprintf(fp,"AdvLinkMTU %d;\n",prof->MTU_config);
    fprintf(fp,"AdvDefaultPreference %s;\n",Type);
    fprintf(fp,"AdvDefaultLifetime %d;\n",prof->routerLifetime);
    fclose(fp);
      
}

/********************************************************************
 * * 函数： Ipv6RadvdGlobalChange
 * * 功能： RADVD全局配置Change函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Ipv6RadvdGlobalChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const RadvdProfile *oldProfileDataPtr, const RadvdProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    Radvdinit();/*重新写配置文件*/
            break;

	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;
}

/********************************************************************
 * * 函数： Ipv6RadvdPrefixChange
 * * 功能： RADVD通告前缀Change函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Ipv6RadvdPrefixChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Ipv6PrefixListProfile *oldProfileDataPtr, const Ipv6PrefixListProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    Radvdinit();/*重新写配置文件*/
	    break;
        case PROF_CHANGE_EDIT:
	    Radvdinit();/*重新写配置文件*/
	    break;

	case PROF_CHANGE_DEL:
	    Radvdinit();/*重新写配置文件*/
	case PROF_CHANGE_DELALL:
	    Radvdinit();/*重新写配置文件*/
        default:/*default*/
            break;
        }
        return;
}



/********************************************************************
 * * 函数： funInitRadvd
 * * 功能： Radvd 初始化入口函数
 * * 创建： 2014-09-28
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang
 * ********************************************************************/
void funInitRadvd(void)
{
    
    registerForProfileUpdates(MIB_PROF_RADVD, (Pointer)NULL, (ProfileUpdateFnc)Ipv6RadvdGlobalChange);/*RADVD全局配置触发函数*/
    registerForProfileUpdates(MIB_PROF_IPV6_NOTLISTPREFIXS, (Pointer)NULL, (ProfileUpdateFnc)Ipv6RadvdPrefixChange);/*DADVD通告列表触发函数*/
    Radvdinit();/*初始化*/
    return;
}

#endif
