#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"free_hash.h"



#if (WIFIDOG_MAKE_MONEY == FYES)
//FreeExp freeExpList[MAX_FREE_ERP_LIST];
//int count=0;
struct hash_list_node *Hash_List[HASH_KEY];

int macAddrCmp(MacAddr *mac1,MacAddr *mac2)
{
    return memcmp(mac1,mac2,sizeof(MacAddr));
}

int hash(MacAddr mac_num)
{
    return ((mac_num.addr[4])^(mac_num.addr[5]));
}
void init_hash()
{
    int i;
    for(i=0;i<HASH_KEY;i++)
    {
	Hash_List[i] = NULL;
    }
}

unsigned int search(MacAddr mac_num,int n)//查询哈希表，有返回该mac存储时的时间，没有返回0
{
    int num = hash(mac_num);
    printf("num=%d\n",num);
    if(Hash_List[num]!=NULL)
    {
	struct hash_list_node *p = Hash_List[num];
	while(p!=NULL)
	{

	    printf("%d,%s-%s\n",__LINE__,converMac6To17Byte(p->mac_data->mac),converMac6To17Byte(mac_num));
	    if(macAddrCmp(&(p->mac_data->mac),&mac_num)==0)
	    {
	    printf("%d,%s-%s\n",__LINE__,converMac6To17Byte(p->mac_data->mac),converMac6To17Byte(mac_num));
		if(n==1)
		{
		    //
		}
		return p->mac_data->time;
	    }
	    p = p -> next;
	}
    }
    return 0;
}
void insert(FreeExp *freeExpList)
{
//    if(search(freeExpList->mac,0)==0)
//    {
	int num = hash(freeExpList->mac);
	struct hash_list_node *new_node = (struct hash_list_node*)malloc(sizeof(struct hash_list_node));
	new_node->mac_data = freeExpList;
	if(Hash_List[num]==NULL)
	{
	    Hash_List[num] = new_node;
	   // Hash_List[num] = NULL;
	}
	else
	{
//	    new_node->next = Hash_List[num];
//	    Hash_List[num] = new_node;
	    Hash_List[num]->next=new_node;
	}

//    }
    return;
}

void initFreeHash()
{
    int i=0;
    for(i=0;i<1024;i++){
	printf("nvramProfile->time[%d]=%lu\n",i,nvramProfile->freeExpList[i].time);
	if(nvramProfile->freeExpList[i].time != 0){
	    insert(&(nvramProfile->freeExpList[i]));
	}else{
	    break;
	}
    }
}

void delete(FreeExp *freeExpList)
{
    int num = hash(freeExpList->mac);
    if(Hash_List[num]!=NULL)
    {
	struct hash_list_node *p = Hash_List[num];
	if(macAddrCmp(&(p->mac_data->mac),&freeExpList->mac)==0)
	{
	    Hash_List[num]=p->next;
	    free(p);
	}
	else
	{
	    while(p->next!=NULL)
	    {
		if(macAddrCmp(&(p->next->mac_data->mac),&freeExpList->mac)==0)
		{
		    struct hash_list_node *q = p->next;
		    p->next=q->next;
		    free(q);
		    return;
		}
		p=p->next;
	    }
	}
    }
    return ;
}
int store_array_hash(MacAddr mac)
{  
    int period=0;
    time_t t,t1;
    MakeMoneyProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAKE_MONEY;

    prof=(MakeMoneyProfile *)ProfGetInstPointByIndex(mibType,0);
    printf("%s-%d\n",__func__,__LINE__);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)))
    {
	period=prof->FreeTime*60;
    }
    t1=search(mac,0);
    if(t1==0)//查询没有
    {
	if(nvramProfile->count>=MAX_FREE_EXP_LIST)//需要覆盖
	{   
	    if(nvramProfile->count>=MAX_FREE_EXP_LIST)
	    {
		nvramProfile->count = nvramProfile->count % MAX_FREE_EXP_LIST;//防止count值增大到无穷大
	    }
	    delete(&(nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST]));
	    memset(&(nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST]),0,sizeof(nvramProfile->freeExpList[0]));		    	
	    nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST].mac=mac;//将添加的mac地址添加到数组中
	    t=time(NULL);
	    nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST].time=t;//将添加的mac地址添加到数组中
	    insert(&(nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST]));
	    nvramProfile->count ++;
	}
	else//不需要覆盖
	{
	    t=time(NULL);
	    printf("%d,%s\n",__LINE__,converMac6To17Byte(mac));
	    nvramProfile->freeExpList[nvramProfile->count].mac = mac;//将添加的mac地址添加到数组中
	    nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST].time=t;//将添加的mac地址添加到数组中
	    printf("%s-%d,time[%d]=%lu\n",__func__,__LINE__,nvramProfile->count%MAX_FREE_EXP_LIST,nvramProfile->freeExpList[nvramProfile->count%MAX_FREE_EXP_LIST].time);
	    insert(&(nvramProfile->freeExpList[nvramProfile->count]));
	    nvramProfile->count++;
	}
	return period;
    }
    t=time(NULL);
    return period-(t-t1);
}

#endif
