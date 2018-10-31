/*
 *
 * added by brwang
 */

#ifndef _TYPEDEF_SHARE_H
#define _TYPEDEF_SHARE_H
typedef unsigned int UINT32;
typedef unsigned int Uint32;
typedef unsigned long long Uint64;
typedef unsigned int Uint;
typedef unsigned int Int;
typedef  int Int32;
typedef unsigned short Uint16;
typedef unsigned char Uchar;
typedef unsigned char Uint8;
typedef short   Word;
typedef char    Byte;
typedef char    Char;
typedef UINT32  LongWord;
typedef Byte    Boolean;
typedef Byte    Bool;
typedef void*   Pointer;
typedef const void * ConstPtr;
typedef unsigned int IPADDR;

typedef struct st_MacAddr
{
   unsigned char addr[6];
}MacAddr;

#define FYES   1
#define TRUE   1
#define FALSE   0
#define NO   0
#define __PACK__       __attribute__(( aligned( 1 ), packed ))
#define PLATFORM  MPORT


#define PLATFORM_BUILDIN        (0L)
#define PROFILE_BUILDIN         (MIB_PROF_SYSTEM)

#define MALLOC         malloc
#define FREE           free 

#if 1
/* �Զ��½�ʵ�� */
# define PORTMAP_MAX	256

/* ����¼2048�� */
typedef struct _PortMap
{
    Byte        content[PORTMAP_MAX];
} PortMap;

typedef struct _webInstRecord {
    Uint16 instNameIndex; //��ǰ��������
    PortMap instRecMap;	//�ѷ���ʵ����¼
} WebInstRec;
#endif

#endif
