#ifndef _MIBDHCP_H
#define _MIBDHCP_H

//# if ( DHCP_PLUS == FYES )

#define MAX_REMOTEID_LEN	32
#define MAX_CIRCUITID_LEN	32	
#define MAX_CLIENTID_LEN	32

#define MAX_DOMAINNAME_LEN	32
#define MAX_HOSTNAME_LEN	16
#define MAX_PARM_REQ_LIST	32
#define	DHCP_OPTIONS_MIN	312
#define MAX_RAWOPTION_LEN	32

//#define MAX_MAC_LEN		6
#define MAX_MAC_LEN		ETHER_ADDR_LEN
//#define MAX_PORTS		3
#define MAX_PORTS		NUM_ETHER_IF
//#define MAX_PROFILE_NAME_LEN	11
#define MAX_PROFILE_NAME_LEN	MAX_PROFILE_NAME_LENGTH
	
struct RawOptionClassProfile
{
	char name[MAX_PROFILE_NAME_LEN+1]; //实例的名字
	Boolean active;
	Uint8 	type; //option的类型
 	//ulong	len; //option的长度
 	char	value[MAX_RAWOPTION_LEN]; //option的内容
	Uint8	bindingport; //绑定到的端口,enum value:eth1,eth2,eth3,see cmdlist.c
};

typedef enum {
	broadcast,
	unicast,
	clientflag,
}OVERLOAD_FLAGS;

typedef enum {
	disable,
	insert
}RELAY_OPTION;

typedef enum{
	keep,
	drop,
	replace
}RELAY_POLICY;

typedef enum {
      Bnode = 0x1,// B节点将广播NetBIOS Name Queries用于名字注册解析。
      Pnode = 0x2, // P节点不使用广播;反之， 它直接查询名字服务器查询
      Mnode = 0x4, // M节点是B节点和P节点的结合，先尝试B节点做法再尝试P节点做法
      Hnode = 0x8 // H节点是P节点和B节点的结合，先尝试P节点做法再尝试B节点做法
}NBT;

struct DhcpPoolClassProfile{
     	char name[MAX_PROFILE_NAME_LEN + 1] ; //pool实例名字
	Boolean active;
	IpAddress poolstart;  //地址池的起始地址
    	Uint32  poolcount;   //地址池的地址个数
    	IpAddress netmask; //地址掩码
	IpAddress gateway; //网关
	IpAddress PriDNS; //主DNS地址
	IpAddress SecDNS; //辅助DNS地址
	IpAddress PriWINS; //主WINS地址
	IpAddress SecWINS; //辅助WINS地址
	Boolean  bAutoIPcoexist;
	NBT	NBTType; //Client所属的NETBIOS节点类型
	char domainName[MAX_DOMAINNAME_LEN];//域名
	char	circuitId[MAX_CIRCUITID_LEN];
	u_long leasetime; //地址池内分配出去的地址的租期
	IpAddress giaddr; //relay agent的ip，作为一种分配策略
	OVERLOAD_FLAGS flags;
	Uint8   bindingport; //绑定到的端口,enum value:eth1,eth2,eth3,see cmdlist.c
};

 //# endif /* DHCP_PLUS == FYES */
#endif //_MIBDHCP_H
