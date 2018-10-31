#include <stdio.h>
#include <string.h>
#include <switch.h>
#include "common.h"
#include "urcp.h"
#include "hash.h"
#include "urcp_debug.h"

#ifdef URCP_STA_MAC_TABLE

static int32 get_free_node(hash_table_info_t*tb)
{
    int32 i, free = NONE_NODE;
    tb_entry_hdr_t *tmp=NULL;

    for(i=0; i<tb->table_size; i++)
    {
	tmp = (tb_entry_hdr_t*)((uchar*)(tb->pTb) + i*(tb->unit));
	if(tmp->used==0)
	{
	    free = i + 1;
	    break;
	}
    }
    if(i>=tb->table_size)
    {
	URCP_ERROR(("urcp_dev: get free node failed\n"));
    }

    /*返回指针*/
    return free;
}

static uint32 hash_key(hash_table_info_t*tb, tb_entry_hdr_t *entry)
{
    uint32 key;
    uchar *mac;

    mac = entry ->mac_addr; 
    /*所有字节异或*/
    key = mac[0] ^ mac[1] ^ mac[2] ^ mac[3] ^ mac[4] ^ mac[5];
    /*高位和低位异或*/
    key = ((key >> 4) | key) % (tb->hash_size);

    return key;
}
/***************************************************
 * function: add a entry to the data table
 * param:    
 *      tb    --the data table information
 *      entry --the data be added
 * return:
 *      -2    --there is no more room
 *      -1    --a new entry is added
 *      0/1/2 --the entry already exist,
 *              and return the value of "aged"
 * ***********************************************/
int32 hash_add_entry(hash_table_info_t*tb, tb_entry_hdr_t *entry)
{
    uint32 key;
    uint16 index;
    int32 ret=HASH_RET_FAILED, exist=0;
    tb_entry_hdr_t* tmp=NULL;

    /*calcu hash key*/
    key = hash_key(tb, entry);
    db_info(URCP_MODULE_ID_HASH, "%s: mac: %s key: %d, type: 0x%x\n", __func__, str_macaddr(entry->mac_addr), key);
    index = (tb->pHash[key]).idx;
    tb -> pDst = NULL;
    if(index == NONE_NODE)
    {
	db_info(URCP_MODULE_ID_HASH, "%s: hash :%d not used\n", __func__, key);
	index = get_free_node(tb);
	if(index!=NONE_NODE){
            tb->pHash[key].idx = index;
	}
    }
    else
    {
	db_info(URCP_MODULE_ID_HASH, "%s: hash used\n", __func__);
	tmp = (tb_entry_hdr_t*)((uchar*)(tb->pTb) + (index-1)*(tb->unit));
	/*找到hash表末尾*/
	db_loud(URCP_MODULE_ID_HASH, "%s: lookup ...\n", __func__);
	while(
		(
#if 0//def DEV_HASH_USE_IP
                 (tmp->ip != dev_ip) || 
#endif
                 (mac_cmp(tmp->mac_addr, entry->mac_addr) != 0))/*和要插入的设备不是同一个设备*/    
		&& (tmp->tb_next != NONE_NODE)) 
	{
	    index = tmp->tb_next;
	    tmp = (tb_entry_hdr_t*)((uchar*)(tb->pTb) + (index-1)*(tb->unit));
	    db_loud(URCP_MODULE_ID_HASH, "    next idx: %d, mac: %s\n", index, str_macaddr(tmp->mac_addr));
	}

	if(
#if DEV_HASH_USE_IP  
		(tmp->ip == dev_ip) &&
#endif
		(mac_cmp(tmp->mac_addr, entry->mac_addr)==0))/*已经保存过了*/
	{
	    ret = tmp->aged;
	    tmp->aged = 0;
	    exist = 1;
	    tb -> pDst = (void*)tmp; 
	    db_info(URCP_MODULE_ID_HASH, "mac exist: idx %d\n", index);
	}
	else{
	    /*获取空节点*/
	    index = get_free_node(tb);
	    if(index != NONE_NODE){
		tmp -> tb_next = index;
	    }
	}
    }

    if((exist == 0) && (index != NONE_NODE)) 
    {
	ret = HASH_RET_NEW;
	tmp = (tb_entry_hdr_t*)((uchar*)(tb->pTb) + (index-1)*(tb->unit));
	memset(tmp, 0, tb->unit);
	/*copy*/
	tmp->used = 1;
	tmp->aged = 0;
	tmp->tb_next = NONE_NODE;
	tb -> pDst = (void*)tmp; 
	memcpy(tmp->mac_addr, entry->mac_addr, MAC_LEN);
	db_info(URCP_MODULE_ID_HASH, "dev new node: %d\n", index);
    }
    return ret;
}

tb_entry_hdr_t * hash_lookup_entry(hash_table_info_t*tb, tb_entry_hdr_t *entry)
{
    uint32  key;
    int16 index;
    tb_entry_hdr_t *pDst=NULL;

    /*calcu key*/
    key = hash_key(tb, entry);
    
    index = tb->pHash[key].idx;
    if(index != NONE_NODE)
    {
	/*不为空*/
	while(index != NONE_NODE)
	{
	    pDst = (tb_entry_hdr_t*)((uchar*)(tb->pTb) + (index-1)*(tb->unit));
	    /*compare*/
	    if ((mac_cmp(pDst->mac_addr, entry->mac_addr) == 0)) {
		break;
	    }
	    index = pDst->tb_next;
	}
	if(index == NONE_NODE)
	{
	    pDst = NULL;
	}
    }

    return pDst;
}

#endif
