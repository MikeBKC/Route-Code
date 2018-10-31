#ifndef MIBLOGHELLOMsg_H
#define MIBLOGHELLOMSG_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#pragma pack(1)
/*
 * logHelloMsg _   �ṹ��
 * feedstart   -   ���, �̶����Ϊ0xdeadbeef
 * version     -   �汾, Ŀǰ�汾Ϊ2
 * length      -   ��Ϣ���ܳ���, ��feedstart��feedend����
 * msgType     -   ��Ϣ����, {�澯, ����}  �ṩ��չ��
 * msgSn       -   ��Ϣ�����, �������, �����ֵ�ƻ�
 * msgInterval -   ��Ϣ���ͼ��
 * mbid        -   MBID
 * swVer       -   ReOS �汾��
 * swName      -   ReOS ���������
 * uptime      -   ϵͳ��������ʱ��
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
    //������Ϣ��ʱ����������һ��������Ϣ���ͺ���˶೤ʱ��
    Uint32 mbid;
    // MBID
    Uint8 swVer[SOFTWARE_ID_LEN];
    // REOS �汾��
    Uint8 swName[SOFTWARE_NAME_LEN];
    // REOS���������
    Uint32 uptime;
    // ϵͳ��������ʱ��
    Uint8  cpu;
    // CPU������
    Uint8  mem;
    // �ڴ�������
    Uint32 arpSessions;
    // ��ǰARP�Ự��
    Uint32 natSessions;
    // ��ǰnat�Ự��
    Uint32 natUsers;
    // ��ǰnat �û���
    Uint32 macUsers;
    // ��ǰMAC�û���
    Uint32 etherNums;
    // ��Ч����̫���ӿ�(����LAN��)����, ���汾���֧��5��(1��LAN, 4��WAN)
    //  ��С��6
    Uint32 rxPkt[MAX_UP_DOWN_DATA_LEN];
    Uint32 rxByte[MAX_UP_DOWN_DATA_LEN];
    Uint32 txPkt[MAX_UP_DOWN_DATA_LEN];
    Uint32 txByte[MAX_UP_DOWN_DATA_LEN];
    Uint8  optdata[0];
    //������������.
    Uint32 feedend;
    //���������, �̶����Ϊ 0xfeedface
}LogHelloMsg;
#pragma pack()
#endif
