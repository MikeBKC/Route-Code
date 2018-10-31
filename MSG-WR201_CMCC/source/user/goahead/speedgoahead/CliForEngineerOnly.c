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
#include "spdComFun.h"

#if (CLI_FOR_ENGINEER_ONLY == FYES)

#ifdef CLI_FOR_ENGINEER_ONLY_DEBUG
#define CLI_FOR_ENGINEER_ONLY_PRINTF(fmt, args...)          printf(fmt, ## args)
#else
#define CLI_FOR_ENGINEER_ONLY_PRINTF(fmt, args...)
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
extern void cliForEngineerOnlyInit(void);
static void cliForEngineerOnlyProfileChange(Pointer tag, MibProfileType profileType, ProfChangeType changeType, Uint32 index,
        const CliForEngineerOnlyProfile *oldProfileDataPtr, const CliForEngineerOnlyProfile *newProfileDataPtr);
static void cliForEngineerOnly_exec(Uint32 index);
static void cliForEngineerOnly_del(const CliForEngineerOnlyProfile *cliForEngineerOnlyProfile);
static void cliForEngineerOnly_delAll(void);
#endif

/******************************
 *��ʼ��
 *****************************/
extern void cliForEngineerOnlyInit(void)
{
   CliForEngineerOnlyProfile *prof = NULL;
   MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
   int min, max, index;
   ProfInstLowHigh(mibType, &max, &min);
   for(index = min; index < max; index++)
   {
       prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index);
       if((prof != NULL) && (ProfInstIsFree(prof) == 0))
       {
            doSystem(prof->command);
       }
   }
}

/********************************************************************
* ������ cliForEngineerOnlyProfileChange 
* ���ܣ� ���ñ仯��Ч����
* ������ 2015-01-7
* ���ߣ� wang.xiaohua
********************************************************************/
static void cliForEngineerOnlyProfileChange(Pointer tag, MibProfileType profileType, ProfChangeType changeType, Uint32 index, 
        const CliForEngineerOnlyProfile *oldProfileDataPtr, const CliForEngineerOnlyProfile *newProfileDataPtr)
{
    switch(changeType)
    {
        case PROF_CHANGE_ADD:
            cliForEngineerOnly_exec(index);
            break;
        case PROF_CHANGE_EDIT:
            break;
        case PROF_CHANGE_DEL:
            cliForEngineerOnly_del(oldProfileDataPtr);
            break;
        case PROF_CHANGE_DELALL:
            cliForEngineerOnly_delAll();
            break;
        default:
            break;
    }
    return ;
}
/********************************************************************
* ������ cliForEngineerOnlyProfile_exec
* ���ܣ� ִ�������
* ������ 2015-01-7
* ���ߣ� wang.xiaohua
********************************************************************/
static void cliForEngineerOnly_exec(Uint32 index)
{
    CliForEngineerOnlyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
    prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0))
    {
        if(strcmp(prof->command, "") != 0 && prof->head.active == TRUE)
        { 
            doSystem(prof->command);
        }
    }
    return;
}

/********************************************************************
* ������ cliForEngineerOnlyProfile_del
* ���ܣ� ִ�������
* ������ 2015-01-7
* ���ߣ� wang.xiaohua
********************************************************************/
static void cliForEngineerOnly_del(const CliForEngineerOnlyProfile *profToBeDeleted)
{
   CliForEngineerOnlyProfile *prof = NULL;
   MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
   prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, 0);
   if(prof != NULL && ProfInstIsFree(prof) == 0 && prof->head.active == TRUE)
   {
       if(strlen(profToBeDeleted->command) > 0)
       {
            memset(profToBeDeleted, 0, sizeof(profToBeDeleted->command));             
       }
   }
}

/********************************************************************
* ������ cliForEngineerOnlyProfile_delAll
* ���ܣ� ɾ����������
* ������ 2015-01-7
* ���ߣ� wang.xiaohua
********************************************************************/ 
static void cliForEngineerOnly_delAll(void)                                          
{                                                                                    
    int min, max, index;
    CliForEngineerOnlyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_CLI_FOR_ENGINEER_ONLY;
    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {                                                                                
        prof = (CliForEngineerOnlyProfile *)ProfGetInstPointByIndex(mibType, index); 
        if(prof != NULL && ProfInstIsFree(prof) == 0)
        {
            memset(prof->command, 0, sizeof(prof->command));
        }
    }
}

/*******************************************************
 * ע�ắ��
 ******************************************************/
extern void funInitCliForEngineerOnly(void) 
{
    registerForProfileUpdates(MIB_PROF_CLI_FOR_ENGINEER_ONLY, NULL, (ProfileUpdateFnc)cliForEngineerOnlyProfileChange);
    cliForEngineerOnlyInit();
    return ;
}

#endif
