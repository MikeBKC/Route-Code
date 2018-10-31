#ifndef MIBLOGHELLOMsg_H
#define MIBLOGHELLOMSG_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#pragma pack(1)
/*
 * logHelloMsg _   结构体
 * feedstart   -   标记, 固定填充为0xdeadbeef
 * version     -   版本, 目前版本为2
 * length      -   消息的总长度, 从feedstart到feedend结束
 * msgType     -   消息类型, {告警, 心跳}  提供扩展性
 * msgSn       -   消息总序号, 单向递增, 达最大值绕回
 * msgInterval -   消息发送间隔
 * mbid        -   MBID
 * swVer       -   ReOS 版本号
 * swName      -   ReOS 软件包名称
 * uptime      -   系统连续运行时间
 */

typedef unsigned long long ULLong;
typedef enum en_syslogmsgtype{
    TYPE_WARNING=0,
    TYPE_HEARTBEAT=1
}SysLogMsgType;

#define MAX_UP_DOWN_DATA_LEN 5
#define SOFTWARE_NAME_LEN    32 
#define SOFTWARE_ID_LEN      20
#define MAX_SERIAL = 65535;

typedef struct logHelloMsg_ {
    Uint32 feedstart;
    Uint16 version;
    Uint16 length;
    Uint16 msgType;
    Uint32 msgSn;
    Uint32 msgInterval;
    //心跳消息的时间间隔：即上一个心跳消息发送后过了多长时间
    Uint32 mbid;
    // MBID
    Uint8 swVer[SOFTWARE_ID_LEN];
    // REOS 版本号
    Uint8 swName[SOFTWARE_NAME_LEN];
    // REOS软件包名称
    Uint32 uptime;
    // 系统连续运行时间
    Uint8  cpu;
    // CPU利用率
    Uint8  mem;
    // 内存利用率
    Uint32 arpSessions;
    // 当前ARP会话数
    Uint32 natSessions;
    // 当前nat会话数
    Uint32 natUsers;
    // 当前nat 用户数
    Uint32 macUsers;
    // 当前MAC用户数
    Uint32 etherNums;
    // 有效的以太网接口(包括LAN口)数量, 本版本最多支持5个(1个LAN, 4个WAN)
    //  故小于6
    Uint32 rxPkt[MAX_UP_DOWN_DATA_LEN];
    Uint32 rxByte[MAX_UP_DOWN_DATA_LEN];
    Uint32 txPkt[MAX_UP_DOWN_DATA_LEN];
    Uint32 txByte[MAX_UP_DOWN_DATA_LEN];
    Uint8  optdata[0];
    //其他可能数据.
    Uint32 feedend;
    //包结束标记, 固定填充为 0xfeedface
}LogHelloMsg;
#pragma pack()
#endif
