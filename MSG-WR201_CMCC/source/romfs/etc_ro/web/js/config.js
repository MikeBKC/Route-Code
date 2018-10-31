/* 型号 */
var platform = "nvA755W";
/* 一级菜单 */
var NetPara=1;
var WlsConf=1;
var AdvConf=1;
var wlanMng=0;	//AP集中管理
var SwFun=0;
var UserMng=1;
var PM=1;
var BM=1;
var Fw=1;
var VPN=1;
var Urcp=0;
var SysMng=1;
var SysSts=1;
var CusSrv=1;

var total=17;

/* 二级菜单 */
var FastConfig=1;	//快速配置
var RunStatus=1;	//运行状态
var InterfaceFlow=1;	//接口流量
var stat_detailed = 0;
var ReBoot=1;	//重启设备

//网络设置
var WANConfig=1;	//WAN口配置
var DoubleRoad=0;	//线路组合
var portnumber=0;	//端口数目
var LANConfig=1;	//LAN配置
var DhcpServer=1;	//DHCP服务器
var DDNS=1;	//DDNS
var UPnP=1;	//UPnP
var fitApIfConf=0;	//瘦AP LAN口配置
var DhcpServerForAc=0;	//Fat ap DHCP服务器
var WANPort=1;		//WAN口数量配置（动态端口）

//无线设置
var WlanBase=1;	//无线配置
var remove_apclient = 1;/*去除APClient*/
var WlanSecurity=1;	//无线安全
var WlanMacFilter=1;	//无线MAC地址过滤
var WlanAdvanced=1;	//高级无线设置
var WlanMultiSsid=0;	//多SSID
var WlanHostInfo=1;	//无线主机列表
var fatFitSwitch=0;
var wlanPara=0;	//无线参数设置
var apParaSwitch=0;	//选择配置

//AP集中管理
var apManage=0;   //AP管理
var serviceZone=0;   //服务区
var apTmp=0;   //AP配置模版
var apSoftMng=0;   //AP软件管理
var getApConf=0;	//AP配置文件
var wlanCltList=0;   //无线客户端

//高级配置
var NatStatic=1;	//静态NAT
var Route=1;	//路由配置
var PolicyRoute=1;
var AntiNetSnifer=0;	//网络尖兵
var Hotel=0;		//即插即用
var PortMirror8367M=0;
var PortMirror=0;
var PortMirror840E=0;
var PortMirror6530G=0;	//6530G的端口镜像功能
var port_vlan_rdd=0; /*端口Vlan*/
var PortSpeed=0;
var Syslog=1;
var Cache=0;   

//网络共享
NetShareManage=1;
FtpServer=1;
ShareAccountList=1;

//交换功能 
var ports=0;	//端口管理
var mirror=0;	//端口镜像
var port_vlan=0;	//端口VLAN
var aggregation=0; //1000m
var aggregation_100M=0;	//100M端口汇聚

//用户管理
var UserLinkRate=1;	//用户链接速率
var PPPoEGlobal=1;	//PPPoE全局设置
var IPMac=0;	//IP-Mac绑定
var WebAuth = 0; /*网页认证*/

//行为管理
var GlobalBase=0;	//上网行为管理
var PdbUpdata=0;	//策略库
var Notice=1;	//通告
var FwAddress=1;	//地址组
var UserGroup=1;	//用户组
var TimeRange=1;	//时间段
var QQExcept=0;		//QQ白名单
var MSNExcept=0;	//MSN白名单
var NetAction=1;	//上网行为审计

//带宽管理
var Group=1;	//精细化限速
var SmartQos=1;	//弹性带宽
var P2PLimit=1; //P2P限速

//防火墙
var FwBase=1;	//安全配置
var FireWall=0;	//访问控制策略
var DnsFilter=1;	//域名过滤
var ConnectionLimit=1;	//连接限制
var MacFilter=1;	//MAC地址过滤
var ParentsControl=0;	//家长控制

//VPN
var PPTPClientList=1;	//PPTP
var Encrypt = 1;     /*mppe加密*/
var L2TPList=0;	//L2TP
var IPSEC=1;	//IPSec

//联动管理
var discover=0;	//联动管理
var linkageConf=0;	//联动配置
var topoy=0;	//网络拓扑

//系统管理
var User=1;	//管理员配置
var EasyTask=0;	//易管理
var langChange=1;	//语言选择
var SysClock=1;	//系统时钟
var DeviceConfig=1;	//配置管理
var SoftwareUpdate=1;	//软件升级
var Remote=1;	//远程管理
var TaskScheduler=1;	//计划任务
var DeviceLoginControl=0;	/*设备登陆控制*/
var SSH=0;   /*设备访问控制里的SSH是否显示*/
var Https=0;	/*设备访问控制里的Https是否显示*/	
var License=0; //License
var DeviceLoginControl=0;   /*设备登陆控制*/
var SSH=0;   /*设备访问控制里的SSH*/
var Https=0;  /*设备访问控制里的Https*/

//系统状态
var RunStatus=1;	//运行状态
var FluxStat=0;	//流量统计
var SysInfo=1;	//系统信息
var SysLogInfo=1;       /*系统日志*/
var UserStats=0;	//用户状态
var fitApClts=0;//瘦AP客户端列表
var fitApSysInfo=0;	//瘦AP系统信息

//客户服务
var SaleService=1;	//客户服务

//1.1第二批页面
var WebAuth=0;	//网页认证
var AddressGroup=1; //地址组
var UserGroup=1; //用户组
var SeviceGroup=1; //服务组
var TimeRange=1; //时间段
var QQWhiteList=1; //QQ白名单
var MSNWhiteList=1; //MSN白名单
var UPnP=1; //即插即用

//AP集中管理
var apManage=0;   //AP管理
var serviceZone=0;   //服务区
var apTmp=0;   //AP配置模版
var apSoftMng=0;   //AP软件管理
var getApConf=0;	//AP配置文件
var wlanCltList=0;   //无线客户端
var wlanMng=0;	//AP集中管理
var fitApIfConf=0;	//瘦AP LAN口配置
var DhcpServerForAc=0;	//Fat ap DHCP服务器
var fatFitSwitch=0;
var wlanPara=0;
var apParaSwitch=0;
var Cache=0;
var fitApClts=0;
var fitApSysInfo=0;
var AliExcept=0;
