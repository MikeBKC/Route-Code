#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__


#define HASH_RET_FAILED (-2)
#define HASH_RET_NEW    (-1)
#define HASH_RET_OK     0

#ifndef __URCP_SHM_H__
typedef struct _tb_entry_hdr_t{
    int16 used;
    int16 tb_next;
    uchar mac_addr[MAC_LEN];
    uint16   aged;
}tb_entry_hdr_t;
typedef struct _hash_entry_hdr_t{
    int16 idx;
}hash_entry_hdr_t;
#endif

typedef struct _hash_table_info_t{
    hash_entry_hdr_t  *pHash;/*hash header*/
    tb_entry_hdr_t    *pTb;/*data table*/
    void              *pDst;
    int32             unit; /*the size of each entry*/
    int32             hash_size;   /*the size of the hash header table*/
    int32             table_size;  /*the size of the data table*/
#if (STA_ONOFF_LOG == FYES)
    int32             first_free;  /*pTb中的第一个空闲节点*/
#endif
}hash_table_info_t;

extern int32 hash_add_entry(hash_table_info_t*tb, tb_entry_hdr_t *entry);
extern tb_entry_hdr_t * urcp_ssid_table_lookup(hash_table_info_t*tb, tb_entry_hdr_t *entry);

#endif
