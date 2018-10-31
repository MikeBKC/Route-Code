/* 基本数据类型定义
 * bhou
 * */
#ifndef _COM_SW_TYPES_H_
#define _COM_SW_TYPES_H_

#ifndef MD5_H
typedef unsigned int        uint32;
#endif
typedef int                 int32;
typedef signed short        int16;
typedef unsigned short      uint16;
typedef unsigned char       BOOL; 

/* - uchar, schar -------------------------------------------------- */
typedef unsigned char       uchar;
typedef signed char         schar;

/* - longlong, ulonglong ------------------------------------------- */

/* longlong and ulonglong: 64 bit integers */
typedef long long           longlong;
typedef unsigned long long  ulonglong;
typedef unsigned long long  uint64;

typedef unsigned int        ip_addr_t;
#endif

