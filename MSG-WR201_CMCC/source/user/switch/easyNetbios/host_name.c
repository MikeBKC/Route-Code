#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "netbios.h"
#include "host_name.h"


#define SW_HOST_NAME_LEN 16u
#define HOST_HASH_SIZE 256u
#define SW_HOST_NAME_TABLE_SIZE 512u 


typedef struct _host_name_table
{
    uint32 used;
    uint32 index;
    uint32 next;
    uint32 ip_addr;
    unsigned char mac_addr[MAC_LEN];
    uint32 name_type;
    char host_name[SW_HOST_NAME_LEN];
    unsigned long agetime;
    unsigned long querytime;
    uint32 query_count;
}host_name_table;

host_name_table tb_hn[SW_HOST_NAME_TABLE_SIZE];


static uint32 free_index = 255u;
static void update_name_file(void);

extern void clear_host_table(void)
{
    memset(tb_hn, 0, sizeof(host_name_table) * SW_HOST_NAME_TABLE_SIZE);   
    unlink(NETBIOS_FILE);
    return;
}

/**************************************************
 * 函数: get_now_time
 * 功能：得到实时时间
 * 参数：无
 * 返回：时间
 * 作者：jfgu
 * 日期：2011-04-01
 * **************************************************/
static unsigned long get_now_time(void)
{
    struct timeval utv;
    unsigned long nowtime = 0u;

    gettimeofday(&utv, NULL);

    nowtime = utv.tv_sec;

    return nowtime;
}


/***************************************************
 *函数：get_mac_hash
 *功能：根据mac得到hash索引
 *参数：mac地址
 *返回：索引
 *作者：jfgu
 *日期：2011-03-31
 **************************************************/
static uint32 get_mac_hash(uchar mac_addr[])
{
    uchar mkey[4];
    uint32 key = 0u;
    memset(mkey, 0, 4);

    mkey[0] =  mac_addr[0] ^ mac_addr[1] ^ mac_addr[2] ^
     	   mac_addr[3] ^ mac_addr[4] ^ mac_addr[5];

    key = *(uint32 *)(mkey);
    

    return key;
  
}

/*******************************************************
 * 函数：get_free_node
 * 功能：得到空余节点
 * 参数：无
 * 返回：空余节点索引
 * 作者：jfgu
 * 日期：2011-04-01
 * ****************************************************/
static uint32 get_free_node(void)
{
    host_name_table* tmp;

    /*是否还有空节点*/
    if (free_index >= SW_HOST_NAME_TABLE_SIZE)
    {
#if 0
	return 0xFFFFFFFF;/*jfgu old code（可能导致读越界）*/
#endif
	return 0u;/*modify by bhou*/
    }

    tmp =  &(tb_hn[free_index]);
    free_index = free_index + 1;
    tmp->index = free_index;/*比实际索引大一*/
    tmp->next = 0u;/*add by bhou（不添加此项无大问题，但会减小查询效率）*/

    /*返回指针*/
    return free_index;
}

/**********************************************************
 * 函数：add_host_name
 * 功能：增加一条记录
 * 参数：mac地址，主机名，ip地址
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-01
 * ********************************************************/
extern void add_host_name(uchar mac_addr[], char *h_name, uint32 ip_addr, uint32 add_type)
{
    uint32 key = 0u;
    host_name_table *tmp = NULL;
    
    /*根据mac地址得到hash索引*/
    key = get_mac_hash(mac_addr);
    
    if (tb_hn[key].used == 0u)
    {
	/*索引处没有使用*/
	tb_hn[key].ip_addr = ip_addr;  /*ip*/
	memcpy(tb_hn[key].mac_addr, mac_addr, MAC_LEN); /*mac*/
	strncpy(tb_hn[key].host_name, h_name, SW_HOST_NAME_LEN); /*name*/
	tb_hn[key].name_type = 0x0u;  /*name type. not used, always 0*/
	tb_hn[key].agetime = get_now_time(); /*timer*/
	tb_hn[key].used = 1u; /*used*/
	tb_hn[key].index = key + 1u; /*index*/
	tb_hn[key].next = 0u; /*next*/
	tb_hn[key].query_count = 0u; /*if send query packet?*/
	update_name_file();
    }
    else
    {
	/*有冲突*/
	tmp = &(tb_hn[key]);

	/*找到mac地址相同*/
	while((mac_cmp(tmp->mac_addr, mac_addr) != 0)
	    && (tmp->next != 0u))
	{
	    tmp = &tb_hn[tmp->next-1];
	}

	/*找到，直接替换名字和ip 更新计时器*/
	if(mac_cmp(tmp->mac_addr, mac_addr)==0)/*已经保存过了*/
	{
	    if ((tmp->ip_addr != ip_addr) || (strcmp(h_name, tmp->host_name) != 0))
	    {
		tmp->ip_addr = ip_addr;
		strncpy(tmp->host_name, h_name, SW_HOST_NAME_LEN);
		update_name_file();
	    }
	   
	    if ((add_type == REGIST_ADD) || ((add_type == QUERY_ADD) && (tmp->query_count == 1u)))
	    {
		/*更新计时器*/	
		tmp->agetime = get_now_time();
		tmp->query_count = 0u;
	    }
	    return;
	}
	
	/*获取空节点*/
	tmp->next = get_free_node();

	/*取得空节点*/
#if 0
	if ((tmp->next != 0u) && (tmp->next != 0xffffffffu))/*gu.jingfei old code(modify by bhou)*/
#endif
	if (tmp->next != 0u)
	{
	    /*copy data*/
	    tb_hn[tmp->next - 1u].used = 1u;  /*used*/
	    tb_hn[tmp->next - 1u].ip_addr = ip_addr; /*ip*/
	    memcpy(tb_hn[tmp->next - 1u].mac_addr, mac_addr, MAC_LEN);  /*mac*/
	    strncpy(tb_hn[tmp->next - 1u].host_name, h_name, SW_HOST_NAME_LEN); /*name*/
	    tb_hn[tmp->next - 1u].name_type = 0u;  /*name type */
	    tb_hn[tmp->next - 1u].agetime = get_now_time(); /*timer*/
	    tb_hn[tmp->next - 1u].query_count = 0u; /*is send query packet*/
	    update_name_file();
	    return;
	}

    }

    return;

}


/************************************************************
 * 函数：delete_host_name
 * 功能：根据mac地址删除记录
 * 参数：mac地址
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-01
 * **********************************************************/
void delete_host_name(uchar mac_addr[])
{
    uint32 key = 0u;
    host_name_table *tmp = NULL;
    host_name_table *pnext = NULL;
    key = get_mac_hash(mac_addr);
    uint32 uindex = 0u;
    uint32 pre_index = 0u;
    int32 changed=0;

    /*hash索引处*/
    tmp = &(tb_hn[key]);

    if ((mac_cmp(tmp->mac_addr, mac_addr) == 0) 
	    && (tmp->used == 1u))
    {
	/*索引处即mac相同*/
	/*没有冲突即下一个节点*/
	if (tmp->next == 0u)
	{
	    /*直接删除*/
	    memset(tmp, 0, sizeof(host_name_table));
	    free_index--;
	    changed = 1;
	}
	else
	{
	    /*有下一节点*/
	    /*将下一节点拷贝到索引处，然后删除下一节点*/
	    uindex = tmp->next;
	    memcpy(tmp, &(tb_hn[uindex - 1u]), sizeof(host_name_table));
	    tmp->index = key + 1u;
	    memset(&(tb_hn[uindex - 1u]), 0, sizeof(host_name_table));
	    free_index--;
	    changed = 1;
	}
    }
    else if ((mac_cmp(tmp->mac_addr, mac_addr) != 0) && (tmp->used == 1u))
    {
	/*索引处不同，遍历*/
	uindex = tmp->next;
	pre_index = tmp->index;

	while(uindex != 0u)
	{
	    tmp = &(tb_hn[uindex - 1u]);
	    pnext = &(tb_hn[pre_index - 1u]);

	    /*mac地址相同*/
	    if ((mac_cmp(mac_addr, tmp->mac_addr) == 0) && (tmp->used == 1u))
	    {
		/*将上一节点的next指向下一个， 删除此节点*/
		pnext->next = tmp->next;
		memset(tmp, 0, sizeof(host_name_table));
		free_index--;
		changed = 1;
	    }

	    uindex = tmp->next;
	    pre_index = tmp->index;

	}
    }
    else
    {
	return;
    }

    if(changed==1)
    {
	update_name_file();
    }

    return;
}

/**************************************************
 * 函数：renew_host_name_table_timeout
 * 功能：更新host表失败，即发送query包后没有回应
 * 参数：无
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-06
 ***************************************************/
extern void renew_host_name_table_timeout(void)
{
    host_name_table *tmp = NULL;
    uint32 uindex = 0u;
    uint32 i = 0u;
    uint32 t;
   
    /*遍历*/
    for (i = 0u; i < HOST_HASH_SIZE; i++)
    {
	tmp = &(tb_hn[i]);

	if (tmp->used == 1u)
	{
	    uindex = tmp->index;

	    while (uindex != 0u)
	    {
		tmp = &(tb_hn[uindex - 1u]);
		
		if ((tmp->used == 1u) && (tmp->ip_addr != 0u))
		{
		    /*时间超过NETBIOS_QUERY_TIMEOUT + HOST_NAMT_TIMEOUT*/
		    /*已经发过query包*/
		    t = get_now_time();
		    if (((t - tmp->querytime) > NETBIOS_QUERY_TIMEOUT) && (tmp->query_count == 1u))
		    {
			/*删除此节点*/
			delete_host_name(tmp->mac_addr);
			
		    }
		}
		uindex = tmp->next;
	    }

	}
    }
    return;


}


/***************************************************
 * 函数: renew_host_name_table
 * 功能：更新主机表里面的信息，如果超过30分钟的主机
 *       需要发query包确认
 * 参数：无
 * 返回: 无
 * 作者：jfgu
 * 日期：2011-04-02
 **************************************************/ 
extern void  renew_host_name_table(void)
{
    host_name_table *tmp = NULL;
    uint32 uindex = 0u;
    uint32 i = 0u;
    uint32 j = 0u;
    uint32 t = 0u;
   
    /*遍历*/
    for (i = 0u; i < HOST_HASH_SIZE; i++)
    {
	tmp = &(tb_hn[i]);

	if (tmp->used == 1u)
	{
	    uindex = tmp->index;

	    while (uindex != 0u)
	    {
		tmp = &(tb_hn[uindex - 1u]);
		
		if ((tmp->used == 1u) && (tmp->ip_addr != 0u))
		{
		    t = get_now_time();
		    if ((t - tmp->agetime > HOST_NAMT_TIMEOUT) && (tmp->query_count == 0u))
		    {
			/*时间超过HOST_NAMT_TIMEOUT 且没发过query包*/
			/*发包查找此节点的用户名是否正确*/
			/*rfc 要求每隔250ms发一个包 发3个包，这里没有时间间隔，偷工减料了*/
			for(j = 0u; j < 3u; j++)
			{
			    netbios_query(tmp->host_name);
			}
			tmp->query_count = 1u;
			tmp->querytime = get_now_time();

			/*更新agetime*/
		    }
		}
		uindex = tmp->next;
	    }

	}
    }
    return;

}


#define HOST_NAME_TITLE_FMT		"%-5s %-20s %-20s %-24s %-8s\n"
#define HOST_NAME_VALUE_FMT		"%-5d %-20s %-20s %-24s %-8d\n"
#if 0
/*******************************************************************
 * 函数：print_host_name_table
 * 功能：打印host name表
 * 参数：无
 * 返回：无
 * 作者：jfgu
 * 日期：2011-04-01
 *******************************************************************/ 
extern void print_host_name_table(void)
{
    host_name_table *tmp = NULL;
    uint32 uindex = 0u;
    uint32 i = 0u;
    char ipbuf[64];

    memset(ipbuf, 0, 64);
    uint32 ucount = 0u;
   
    /*表头*/
    printf(HOST_NAME_TITLE_FMT, 
	    "ID",
	    "IP",
	    "MAC",
	    "NAME",
	    "TYPE",
	    "AGE",
	    "INDEX",
	    "NEXT",
	    "COUNT");

    /*遍历*/
    for (i = 0u; i < HOST_HASH_SIZE; i++)
    {
	tmp = &(tb_hn[i]);

	if (tmp->used == 1u)
	{
	    uindex = tmp->index;

	    while (uindex != 0u)
	    {
		tmp = &(tb_hn[uindex - 1u]);
		if ((tmp->used == 1u) && (tmp->ip_addr != 0u))
		{
		    ucount++;
		    printf(HOST_NAME_VALUE_FMT,
			    ucount,  /*id*/
			    int2ip(tmp->ip_addr, ipbuf), /*ip*/
			    str_macaddr(tmp->mac_addr), /*mac*/
			    tmp->host_name, /*name*/
			    tmp->name_type, /*type*/
			    (get_now_time() - tmp->agetime), /*timer*/
			    tmp->index, /*index*/
			    tmp->next, /*next*/
			    tmp->query_count /*is send query_packet*/
			    );    
		}
		uindex = tmp->next;
	    }
	}
    }

    printf("\n\n");

    return;
}
#endif
static void update_name_file(void)
{
    uint32 uindex = 0u;
    uint32 i = 0u;
    char ipbuf[64];
    FILE *fp;
    host_name_table *tmp = NULL;

    memset(ipbuf, 0, 64);
    uint32 ucount = 0u;

    fp = fopen(NETBIOS_FILE, "w");
    if(fp!=NULL)
    {
	/*表头*/
        fprintf(fp, HOST_NAME_TITLE_FMT, "ID", "IP", "MAC", "NAME", 
	                        "TYPE");

	/*遍历*/
	for (i = 0u; i < HOST_HASH_SIZE; i++)
	{
	    tmp = &(tb_hn[i]);

	    if (tmp->used == 1u)
	    {
		uindex = tmp->index;

		while (uindex != 0u)
		{
		    tmp = &(tb_hn[uindex - 1u]);
		    if ((tmp->used == 1u) && (tmp->ip_addr != 0u))
		    {
			ucount++;
			fprintf(fp, HOST_NAME_VALUE_FMT,
				ucount,  /*id*/
				int2ip(tmp->ip_addr, ipbuf), /*ip*/
				str_macaddr(tmp->mac_addr), /*mac*/
				tmp->host_name, /*name*/
				tmp->name_type /*type*/
			      );    
		    }
		    uindex = tmp->next;
		}
	    }
	}
	fclose(fp);
    }
}
