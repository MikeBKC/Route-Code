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
 * ����: get_now_time
 * ���ܣ��õ�ʵʱʱ��
 * ��������
 * ���أ�ʱ��
 * ���ߣ�jfgu
 * ���ڣ�2011-04-01
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
 *������get_mac_hash
 *���ܣ�����mac�õ�hash����
 *������mac��ַ
 *���أ�����
 *���ߣ�jfgu
 *���ڣ�2011-03-31
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
 * ������get_free_node
 * ���ܣ��õ�����ڵ�
 * ��������
 * ���أ�����ڵ�����
 * ���ߣ�jfgu
 * ���ڣ�2011-04-01
 * ****************************************************/
static uint32 get_free_node(void)
{
    host_name_table* tmp;

    /*�Ƿ��пսڵ�*/
    if (free_index >= SW_HOST_NAME_TABLE_SIZE)
    {
#if 0
	return 0xFFFFFFFF;/*jfgu old code�����ܵ��¶�Խ�磩*/
#endif
	return 0u;/*modify by bhou*/
    }

    tmp =  &(tb_hn[free_index]);
    free_index = free_index + 1;
    tmp->index = free_index;/*��ʵ��������һ*/
    tmp->next = 0u;/*add by bhou������Ӵ����޴����⣬�����С��ѯЧ�ʣ�*/

    /*����ָ��*/
    return free_index;
}

/**********************************************************
 * ������add_host_name
 * ���ܣ�����һ����¼
 * ������mac��ַ����������ip��ַ
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-04-01
 * ********************************************************/
extern void add_host_name(uchar mac_addr[], char *h_name, uint32 ip_addr, uint32 add_type)
{
    uint32 key = 0u;
    host_name_table *tmp = NULL;
    
    /*����mac��ַ�õ�hash����*/
    key = get_mac_hash(mac_addr);
    
    if (tb_hn[key].used == 0u)
    {
	/*������û��ʹ��*/
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
	/*�г�ͻ*/
	tmp = &(tb_hn[key]);

	/*�ҵ�mac��ַ��ͬ*/
	while((mac_cmp(tmp->mac_addr, mac_addr) != 0)
	    && (tmp->next != 0u))
	{
	    tmp = &tb_hn[tmp->next-1];
	}

	/*�ҵ���ֱ���滻���ֺ�ip ���¼�ʱ��*/
	if(mac_cmp(tmp->mac_addr, mac_addr)==0)/*�Ѿ��������*/
	{
	    if ((tmp->ip_addr != ip_addr) || (strcmp(h_name, tmp->host_name) != 0))
	    {
		tmp->ip_addr = ip_addr;
		strncpy(tmp->host_name, h_name, SW_HOST_NAME_LEN);
		update_name_file();
	    }
	   
	    if ((add_type == REGIST_ADD) || ((add_type == QUERY_ADD) && (tmp->query_count == 1u)))
	    {
		/*���¼�ʱ��*/	
		tmp->agetime = get_now_time();
		tmp->query_count = 0u;
	    }
	    return;
	}
	
	/*��ȡ�սڵ�*/
	tmp->next = get_free_node();

	/*ȡ�ÿսڵ�*/
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
 * ������delete_host_name
 * ���ܣ�����mac��ַɾ����¼
 * ������mac��ַ
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-04-01
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

    /*hash������*/
    tmp = &(tb_hn[key]);

    if ((mac_cmp(tmp->mac_addr, mac_addr) == 0) 
	    && (tmp->used == 1u))
    {
	/*��������mac��ͬ*/
	/*û�г�ͻ����һ���ڵ�*/
	if (tmp->next == 0u)
	{
	    /*ֱ��ɾ��*/
	    memset(tmp, 0, sizeof(host_name_table));
	    free_index--;
	    changed = 1;
	}
	else
	{
	    /*����һ�ڵ�*/
	    /*����һ�ڵ㿽������������Ȼ��ɾ����һ�ڵ�*/
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
	/*��������ͬ������*/
	uindex = tmp->next;
	pre_index = tmp->index;

	while(uindex != 0u)
	{
	    tmp = &(tb_hn[uindex - 1u]);
	    pnext = &(tb_hn[pre_index - 1u]);

	    /*mac��ַ��ͬ*/
	    if ((mac_cmp(mac_addr, tmp->mac_addr) == 0) && (tmp->used == 1u))
	    {
		/*����һ�ڵ��nextָ����һ���� ɾ���˽ڵ�*/
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
 * ������renew_host_name_table_timeout
 * ���ܣ�����host��ʧ�ܣ�������query����û�л�Ӧ
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-04-06
 ***************************************************/
extern void renew_host_name_table_timeout(void)
{
    host_name_table *tmp = NULL;
    uint32 uindex = 0u;
    uint32 i = 0u;
    uint32 t;
   
    /*����*/
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
		    /*ʱ�䳬��NETBIOS_QUERY_TIMEOUT + HOST_NAMT_TIMEOUT*/
		    /*�Ѿ�����query��*/
		    t = get_now_time();
		    if (((t - tmp->querytime) > NETBIOS_QUERY_TIMEOUT) && (tmp->query_count == 1u))
		    {
			/*ɾ���˽ڵ�*/
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
 * ����: renew_host_name_table
 * ���ܣ������������������Ϣ���������30���ӵ�����
 *       ��Ҫ��query��ȷ��
 * ��������
 * ����: ��
 * ���ߣ�jfgu
 * ���ڣ�2011-04-02
 **************************************************/ 
extern void  renew_host_name_table(void)
{
    host_name_table *tmp = NULL;
    uint32 uindex = 0u;
    uint32 i = 0u;
    uint32 j = 0u;
    uint32 t = 0u;
   
    /*����*/
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
			/*ʱ�䳬��HOST_NAMT_TIMEOUT ��û����query��*/
			/*�������Ҵ˽ڵ���û����Ƿ���ȷ*/
			/*rfc Ҫ��ÿ��250ms��һ���� ��3����������û��ʱ������͵��������*/
			for(j = 0u; j < 3u; j++)
			{
			    netbios_query(tmp->host_name);
			}
			tmp->query_count = 1u;
			tmp->querytime = get_now_time();

			/*����agetime*/
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
 * ������print_host_name_table
 * ���ܣ���ӡhost name��
 * ��������
 * ���أ���
 * ���ߣ�jfgu
 * ���ڣ�2011-04-01
 *******************************************************************/ 
extern void print_host_name_table(void)
{
    host_name_table *tmp = NULL;
    uint32 uindex = 0u;
    uint32 i = 0u;
    char ipbuf[64];

    memset(ipbuf, 0, 64);
    uint32 ucount = 0u;
   
    /*��ͷ*/
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

    /*����*/
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
	/*��ͷ*/
        fprintf(fp, HOST_NAME_TITLE_FMT, "ID", "IP", "MAC", "NAME", 
	                        "TYPE");

	/*����*/
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
