
#include "uttMachine.h"
#include "typedef.h"
#include <string.h>
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
/**
 * ��ȡ������ Ĭ���û����������
 * prefix:Ĭ���û���ǰ׺   profType:��������
 * ����:bhou
 * ע��:Ĭ���û�������Ϊ prefix+number����ʽ
 */
int ProfGetMaxDefaultNameNo(MibProfileType profType, char* prefix)
{
    int min = 0, max = 0 ,ret = 0 ,no = 0;
    int i = 0;
    const char* profName = NULL;
    char  strNoPre[UTT_INST_NAME_LEN + 1];/*ȥ��ǰ׺���û���*/
    char  strNum[16];/*�û����еĴ�����*/
    char  regStr[32];/*����*/

    memset(regStr, 0, sizeof(regStr));
    strcpy(regStr, prefix);/*Ĭ���û���ǰ׺*/
    strcat(regStr, "%s");/*ƥ�� ǰ׺ ��ʽ���û���*/

    ProfInstLowHigh(profType, &max, &min);  
    for(i = 0; i < max; i++)
    {
	profName = ProfGetInstNameByIndex(profType, i);
	if((profName != NULL) && (*profName != '\0'))
	{
	    memset(strNoPre, 0, sizeof(strNoPre));/*ÿ��sscanfǰ����գ�����sscanf������Ϊ��ʱ�����ı�Ŀ���ִ�*/
	    sscanf(profName, regStr, strNoPre);/*ǰ׺������ŵ��ַ���*/
	    if(*strNoPre != '\0')/*ƥ�䵽*/
	    {
		memset(strNum, 0, sizeof(strNum));
		sscanf(strNoPre, "%[0-9]", strNum);/*��ȡ�����ִ�*/
		if(strlen(strNum) == strlen(strNoPre))/*ȥ��ǰ׺��ʣ�µ��ִ���Ϊ����*/
		{
		    sscanf(strNum, "%d", &no);/*ת��Ϊ����*/
		    if(no > ret)
		    {
			ret = no;
		    }
		}

	    }
	}
    }
    return ret;
}
/**
 * ����ǰ׺������Ĭ��ʵ����
 * maxNoΪ��ǰ���Ĭ��ʵ�������
 * bhou
 * 2011-11-03
 */
char* ProfBuildDefaultName(char* prefix, int maxNo)
{
    static char retName[UTT_INST_NAME_LEN + 1];
    memset(retName, 0, sizeof(retName));
    sprintf(retName,"%s%d", prefix, maxNo+1);
    return retName;
}

/**
 * �ҵ�һ���������ÿռ䲢����������
 * bhou
 * 2011-11-03
 */
int ProfGetInstIndexFree(MibProfileType profType)
{
    int min = 0, max = 0, count = 0;
    int i = 0;
    int retIndex = PROFFAIL;
    Pointer prof = NULL;

    ProfInstLowHigh(profType, &max, &min);  
    for(i = 0; i < max; i++)
    {
	prof = ProfGetInstPointByIndex(profType, i);
	if(prof != NULL && ProfInstIsFree(prof) != 0) 
	{/*���п���*/
	    retIndex = i;
	    break;
	}
    }
    return retIndex;
}
/**
 * ͳ��ĳ��ʵ�������� ��ʹ��ʵ���ĸ���
 * bhou
 * 2011-10-31
 */
int ProfCountInstNotFree(MibProfileType profType)
{
    int min = 0, max = 0, count = 0;
    int i = 0;
    Pointer prof = NULL;

    ProfInstLowHigh(profType, &max, &min);  
    for(i = 0; i < max; i++)
    {
	prof = ProfGetInstPointByIndex(profType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) 
	{/*�����Ѿ�ʹ��*/
	    count++;   
	}
    }
    return count;
}
void ProfSetNameByPoint(Pointer prof, const Byte* name) 
{
    strncpy((((mibHead*)prof)->name), name, UTT_INST_NAME_LEN); 
}

Boolean ProfGetInstByIndex( MibProfileType  profileType,
		  Word		  index,
		  Pointer	  dataPtr )
{
    int len;
    Pointer p;
    SlotNumber slot;
    if ( !dataPtr ) {
       return FALSE; 
    }

    slot.slotNumber = SLOT_NUMBER_ANY;
    
    p = _getProfilePointer(slot, profileType, index, &len, NULL, NULL);
    if(p != NULL) {
     	memcpy(dataPtr, p, len);     
     	return TRUE;
    } else {
    	return FALSE;
    }
}

Pointer ProfGetInstPointByIndex(MibProfileType profileType, int index)
{
    SlotNumber slot;
    slot.slotNumber = SLOT_NUMBER_ANY;
    return _getProfilePointer(slot, profileType, index, NULL, NULL, NULL);
}

int ProfGetInstIndexByName(MibProfileType profType, const char *name)
{
    int min = 0, max = 0;
    int i = 0;
    mibHead *head;

    ProfInstLowHigh(profType, &max, &min);  
    for(i = min; i < max; i++) {
        head = (mibHead*)(ProfGetInstPointByIndex(profType, i));
        if(strncmp(head->name, name, UTT_INST_NAME_LEN) == 0) {         
            return i;
        }
    }

    return PROFFAIL;
}

Pointer ProfGetInstPointByName(MibProfileType profType, const char *name)
{
    int min = 0, max = 0;
    int i = 0;
    mibHead *head;

    ProfInstLowHigh(profType, &max, &min);  
    for(i = min; i < max; i++) {
        head = (mibHead*)(ProfGetInstPointByIndex(profType, i));
        if(strncmp(head->name, name, UTT_INST_NAME_LEN) == 0) {         
            return head;
        }
    }

    return NULL;
}

#if 0
/**
 * ���ܣ�����һ��ʵ����ĳ�� Ԫ�ص� ֵ, ���Ҹ�ʵ����Ӧ������
 * ����:
 * profType: ʵ������
 * startIndex: ÿ�β��ҿ�ʼ��������С����С�����������С������ʼ�������ڵ��������������ֻ�������������һ
 * offset:Ԫ���൱�ڸ�ʵ����ʼ��ַ��ƫ����
 * size:Ԫ�ص�byte��С
 * value:ָ��Ƚ�Ԫ��ֵ��ָ��
 *
 * return: �ҵ��򷵻� ��ʵ����Ӧ������ �����򷵻� ��С���� ��һ
 *
 * bhou
 * 2011-10-26
 */
#define ProfGetInstPointByValue(profType,startIndex,structName,eleName,value )\
    _ProfGetInstPointByValue(profType,startIndex,OFFANDSIZE(structName,eleName),value )
const int  _ProfGetInstPointByValue(MibProfileType profType, int startIndex, Uint32 offset, Uint32  size, Byte*value )
{
    int min = 0, max = 0;
    int i = 0;
    int retIndex = 0;

    Pointer prof;

    ProfInstLowHigh(profType, &max, &min);  

    retIndex = min - 1;/*��ʼ��Ϊ��С������һ*/

    if(startIndex < min)
    {
	i = min;
    }
    else if(startIndex > (max - 1))
    {
	i = max - 1;
    }
    else
    { 
	i = startIndex;
    }

    for(; i < max; i++) {
	prof = ProfGetInstPointByIndex(profType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) 
	{
	    if(memcmp(((Byte*)prof + offset), value, size) == 0)
	    {
		retIndex = i;
		break;
	    }
	}

    }

    return retIndex;
}
#endif
/*
 *  ���ܣ�����valueֵͳ��ĳ����ʵ���ĸ���
 *  ���������
 *  profType	-   ��Ӧ��mibType
 *  offset	-   profile��Ҫ���ҵĳ�Ա��ƫ����
 *  size	-   profile��Ҫ���ҵĳ�Ա�Ĵ�С
 *  cmpSize     -   �Ƚ����ݵĴ�С��ӦС��size��Ϊ0���Զ���size�Ƚ�
 *  value	-   ��ѯ��valueֵ�׵�ַ
 *  bhou
 *
 *  ����ֵ��ƥ������
 */
Uint32 ProfCountInstByValue(MibProfileType profType, Uint32 offset, Uint32 size, Uint32 cmpSize, Pointer value) 
{
    int i, min = 0, max = 0;
    Uint32 retCount = 0u;/*ͳ������*/
    Pointer prof = NULL;

    if((cmpSize == 0u) || (cmpSize >size))
    {
	cmpSize = size;/*Ԫ��Ҫ�ȽϵĴ�С*/
    }


    ProfInstLowHigh(profType, &max, &min);  

    /*��������*/
    for (i = min; i < max; i++) 
    {
	prof = ProfGetInstPointByIndex(profType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
	    if(memcmp(prof+offset, value, cmpSize) == 0)/*���*/
	    {
		retCount++;
	    }

	}
    }
    return retCount;
}
/*
 * ���ܣ�����valueֵ���profile��������������������
 * ���������
 *  profType	-   ��Ӧ��mibType
 *  offset	-   profile��Ҫ���ҵĳ�Ա��ƫ����
 *  size	-   profile��Ҫ���ҵĳ�Ա�Ĵ�С
 *  itype	-   ��Ҫ��ѯ��valueֵ�����ͣ�ͬrootConfig�е����ͣ�Ŀǰ��֧��ö������TENUM�����conver.h
 *  value	-   ��ѯ��valueֵ����ҳ����ֱ�ӻ�õ��ַ�����ʽ��
 *  specialIndex-   ����������������������ʵ�����ԣ�����ѯ����specialIndexΪ-1ʱ����ʾ
 *		    ����������ʵ��������ʵ��һ��ͬ�ʡ�specialIndexʹ�����ӣ�����ʵ��A��
 *		    ʵ������������ͬ������ͬ���ж�ʱ����Ҫ����A����.
 *
 * ����ֵ��
 *  δ���ҵ�����PROFFAIL,���򷵻ز��ҵ���ʵ������
 */
int ProfGetInstPointByValue(MibProfileType profType, Uint32 offset, Uint32 size, IType itype,
	char *value, int specialIndex) {
    int index, i, min = 0, max = 0;
    char buf[MAX_VALUE_LEN] = {0};

    Pointer prof = NULL;

    ProfInstLowHigh(profType, &max, &min);  

    for (i = min; i < max; i++) {
	prof = ProfGetInstPointByIndex(profType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (specialIndex != i)) 
	{
	    memset(buf, 0, MAX_VALUE_LEN);
	    parseProfileValue2(offset, size, itype, buf, prof);
#if 0
	    printf("%s:buf=%s\n", __FUNCTION__, buf);
#endif
	    if (strcmp(buf, value) == 0) {
		return i;
	    }
	}
    }
    return PROFFAIL;
}

const char*  ProfGetInstNameByIndex(MibProfileType profType, int instIndex)
{
    Pointer prof;
    mibHead *head;
    prof = ProfGetInstPointByIndex(profType, instIndex);
    if(prof != NULL) {
        head = (mibHead*)prof;
        return head->name;
    } else {
        return NULL; 
    }
}

int ProfDelInstByName(MibProfileType profType, const char* instname)
{
    int find = -1;
    
    find =  ProfGetInstIndexByName(profType, instname);
    if (find == PROFFAIL) {
        return PROFFAIL;
    } else {
        return ProfDelInstByIndex(profType, find);
    }
}

#if 0
int ProfDelInstByIndex(MibProfileType profType, int instIndex)
{   
    Pointer prof;
    mibHead *head;
    prof = ProfGetInstPointByIndex(profType, instIndex);
    if(prof != NULL) {
        head = (mibHead*)prof;
        strcpy(head->name,"");
        head->active = FALSE;
        return instIndex;
    } else {
        return PROFFAIL; 
    }
}
#else
int ProfDelInstByIndex(MibProfileType profType, int instIndex)
{
    SlotNumber slot;
    Pointer prof, p;
    Uint len = 0, num = 0;
    mibHead *head;
    bool bFlag = FALSE;
    slot.slotNumber = SLOT_NUMBER_ANY;
    if(profileBuildFactory((MibProfileType)profType, &prof)) {
    	p = _getProfilePointer(slot, (MibProfileType)profType, instIndex, &len, &num, NULL);
        if(p != NULL) {
            memcpy(p, prof, len);
        }

        bFlag = BuildDefaultFactoryValue((MibProfileType)profType, instIndex, slot, &prof);
        if((bFlag == TRUE) && (p != NULL)) {
	    memcpy(p, prof, len);
	}
        head = (mibHead*)p;
        head->active = FALSE;
        return instIndex;
    } else {
        return PROFFAIL; 
    }
}
#endif

int ProfInstIsFree(Pointer prof)
{
    if(prof != NULL) {
        if(strcmp(((mibHead*)prof)->name, "") != 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}

int ProfNewInst(MibProfileType proftype, const char* instname, Boolean bOverWrite)
{
    int i = 0, max = 0, min = 0;
    int find = -1;
    int size = 0;
    mibHead *head;
    Pointer prof, prof1;

    find = ProfGetInstIndexByName(proftype, instname);    
    ProfInstLowHigh(proftype, &max, &min);
    /*
     * inst name not exist,add the new inst name
     */
    if(find == -1) {
        for(i = min; i < max; i++) {
            prof1 = ProfGetInstPointByIndex(proftype, i);
	    if(prof1 == NULL) {
                continue;
            }
            head = (mibHead*)prof1;
            if(strlen(head->name) == 0) {
                size = profileBuildFactory(proftype, &prof);
                memcpy(prof1, prof, size);
                head->active = TRUE;
                strncpy(head->name, instname, UTT_INST_NAME_LEN);
                return i;
	    }   
        }
        return PROFFAIL;
    } else {
        if(bOverWrite == FALSE) {
            return PROFFAIL;
        } else {
	    prof1 = ProfGetInstPointByIndex(proftype, find);
	    if(prof1 == NULL) {
	        return PROFFAIL;
	    }
            size = profileBuildFactory(proftype, &prof);
            memcpy(prof1, prof, size);
            head = (mibHead*)prof1;
            head->active = TRUE;
            strncpy(head->name, instname, UTT_INST_NAME_LEN);
	    return find;
        }        
    } 
    return PROFFAIL;
}

static struProfAlloc * _AddNewProfAlloc(pstruProfAlloc *listhead, MibProfileType proftype,
        int instIndex, int profsize, Pointer olddata,  ProfChangeType changeType)
{
    struProfAlloc *pnewstru;
    Pointer pmem;
    unsigned memlen = sizeof(struProfAlloc) + profsize;

    if(listhead == NULL) {
        return NULL;    
    } 
  
    pmem = (Pointer)MALLOC(memlen);
    if(pmem == NULL) {
        return NULL;
    }

    pnewstru = (struProfAlloc *)(pmem);
    pnewstru->instIndex = instIndex;
    pnewstru->next = NULL;
    pnewstru->tail = pnewstru;
    pnewstru->changeType = changeType;
    pnewstru->proftype = proftype;
    pnewstru->profsize = profsize;
    pnewstru->oldProf = (Pointer)((char *)pmem + sizeof(struProfAlloc));
    
    memcpy(pnewstru->oldProf, olddata, profsize);

    if((*listhead) != NULL) {  
        if(((*listhead)->tail) != NULL) {
            (*listhead)->tail->next = pnewstru;
            (*listhead)->tail = pnewstru;
        }
    } else {
        *listhead = pnewstru;
    }
    return pnewstru;
}

#if(CWMP == FYES)
int  ProfBackupByIndexAndFlag(MibProfileType proftype, ProfChangeType changeType,
            int instIndex, pstruProfAlloc *proflist,int flag)
{
    struProfAlloc *pnewstru;
    Pointer profNvram = NULL;    
    int profsize = 0;
    int max = 0, min = 0;
    SlotNumber slot;

    if(proflist == NULL) {
        return PROFFAIL;    
    }   
    slot.slotNumber = SLOT_NUMBER_ANY;

    profNvram = _getProfilePointer(slot, proftype, instIndex, &profsize, &max, &min);
    if(((instIndex < min) || (instIndex > max)) || (proflist == NULL)
            || (profsize <= 0) || (profNvram == NULL)) {
       return PROFFAIL;    
    }   

    pnewstru = _AddNewProfAlloc(proflist, proftype, instIndex, profsize, profNvram, changeType);
    if(pnewstru != NULL) {
        pnewstru->flag = flag;
        return PROFFAIL;
    } else {
        return PROFOK;  
    }
}
#endif
int  ProfBackupByIndex(MibProfileType proftype, ProfChangeType changeType,
            int instIndex, pstruProfAlloc *proflist)
{
    Pointer profNvram = NULL;    
    int profsize = 0;
    int max = 0, min = 0;
    SlotNumber slot;

    if(proflist == NULL) {
        return PROFFAIL;    
    }   
    slot.slotNumber = SLOT_NUMBER_ANY;

    profNvram = _getProfilePointer(slot, proftype, instIndex, &profsize, &max, &min);
    if(((instIndex < min) || (instIndex > max)) || (proflist == NULL)
            || (profsize <= 0) || (profNvram == NULL)) {
       return PROFFAIL;    
    }   

    if(!_AddNewProfAlloc(proflist, proftype, instIndex, profsize, profNvram, changeType)) {
        return PROFFAIL;
    } else {
        return PROFOK;  
    }
}

int  ProfBackupByName(MibProfileType proftype, ProfChangeType changeType, char *instName, pstruProfAlloc *proflist)
{     
    int instIndex;
    int min = 0, max = 0;

    if(proflist == NULL) {
        return PROFFAIL;    
    }   
    instIndex = ProfGetInstIndexByName(proftype, instName);
    ProfInstLowHigh(proftype, &max, &min);
    if((instIndex < min) || (instIndex > max)) {
        return PROFFAIL;    
    }

    if(ProfBackupByIndex(proftype, changeType, instIndex, proflist) == PROFOK) {
        return instIndex; 
    } else {
        return PROFFAIL;
    }
}

void ProfFreeAllocList(struProfAlloc *listhead)
{
    struProfAlloc *pstru = listhead;
    struProfAlloc *pnext = NULL;  
    while(pstru != NULL) {
        pnext = pstru->next;   
        FREE((Pointer)pstru);      
        pstru = pnext;
    }
    return;
}

void  _ProfRestore(MibProfileType proftype,int instIndex, Pointer prof)
{
    Pointer profNvram = NULL;
    int profsize;
    int min = 0, max = 0;
    SlotNumber slot;
    slot.slotNumber = SLOT_NUMBER_ANY;

    profNvram = _getProfilePointer(slot, proftype, instIndex, &profsize, &max, &min);
    if((profsize <= 0) || (instIndex < min) || (instIndex > max) 
            || (prof == NULL) || (profNvram == NULL)) {
       return;
    }

    memcpy(profNvram, prof, profsize);    
    return;  
}

void  ProfRestore(struProfAlloc *profhead)
{
    struProfAlloc *profnext, *pstru;  
    pstru = profhead;

    while(pstru != NULL) {
        profnext = pstru->next;
        _ProfRestore(pstru->proftype, pstru->instIndex, pstru->oldProf);
        pstru = profnext;
    }
}

static void _ProfUpdate(MibProfileType	proftype,
		     int instIndex, Pointer	oldDataPtr, int profsize, ProfChangeType changeType)
{
    struct st_uttMsg msg;
	int msgID = 0;
    SlotNumber slot;

    slot.slotNumber = SLOT_NUMBER_ANY;

    if(oldDataPtr == NULL) {
        return;
    } 
#if 0
    newDataPtr = _getProfilePointer(slot, proftype, instIndex, &profsize, NULL, NULL);
    if((profsize < 0) || (newDataPtr == NULL)) {
        ;
    } else {
        reportProfileUpdate(slot, proftype, instIndex, oldDataPtr, newDataPtr, profsize);  
    }
#else
	msgID = UttMsgInit();
	msg.mtype = UTT_MSG_TYPE_1;
	msg.msgPara.mainType = MSG_PROF;
	msg.msgPara.secType = changeType;
	msg.msgPara.data.profChangeData.profType = proftype;
	msg.msgPara.data.profChangeData.instIndex = instIndex;
	memcpy(&(msg.msgPara.data.profChangeData.oldDataPtr), oldDataPtr, profsize);
	msgsnd(msgID, &msg, sizeof(msg.msgPara), 0);
#endif
    return;
}

void ProfUpdate(struProfAlloc * profhead)
{
    struProfAlloc *profnext, *pstru;  
    pstru = profhead;
    
    while(pstru != NULL) {
        profnext = pstru->next;
        _ProfUpdate(pstru->proftype, pstru->instIndex, pstru->oldProf, pstru->profsize, pstru->changeType);
        pstru = profnext;
    }  
}



#if 1
/* �Զ��½�ʵ���� */
Boolean
isPortMapSet(PortMap *pm, Uint16 index)
{
    return (pm->content[index >> 3] & (1 << (index & 7))) ? TRUE : FALSE;
}

void
setPortMap(PortMap *pm, Uint16 index)
{
#if 0
    printf("%s: index = %d\n", __FUNCTION__, index);
#endif
    pm->content[index >> 3] |= (1 << (index & 7));
#if 0
    printf("0x%x\n", pm->content[index >> 3]);
#endif
}

void
clearPortMap(PortMap *pm, Uint16 index)
{
    pm->content[index >> 3] &= ~(1 << (index & 7));
}

extern void clearAllPortMap(PortMap *pm) {
    memset(pm, 0, sizeof(PortMap));
    return; 
}

/*
 * �������ʵ��
 */
extern Uint16 getWebInstName(PortMap *pm, Uint16 *indexInst)
{
   int i;

   for (i = 0; i < MAX_WEB_INST; i++)
   {
	(*indexInst)++;

	if (*indexInst > MAX_WEB_INST)
	{
	    *indexInst = WEB_INST_START;
	}
     
	if (!isPortMapSet(pm, *indexInst))
        {
            setPortMap(pm, *indexInst);
	    return *indexInst;
        }
    }
    return 0;
}
#endif

#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType)
{
    struct st_uttMsg msg;

    memset(&msg,0,sizeof(msg));
    msg.mtype = UTT_MSG_TYPE_1;
    msg.msgPara.mainType = MSG_PROF;
    msg.msgPara.secType = changeType;
    msg.msgPara.data.profChangeData.profType = proftype;
    msg.msgPara.data.profChangeData.instIndex = instIndex;
    memcpy(&(msg.msgPara.data.profChangeData.oldDataPtr), oldDataPtr, profsize);

    uttNfNlSend(socket,(char*)&msg, sizeof(msg), 0U ,0U );/*������Ϣ���ں�*/
}
#endif
