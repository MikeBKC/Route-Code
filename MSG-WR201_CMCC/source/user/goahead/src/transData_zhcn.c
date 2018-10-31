
#include "translate.h"
const char* transData_zhcn[] = {
    "无法获取云设置选项",//C_LANG_INDEX_CACHE_LIST_FAIL
	"无法获取空间使用量",//C_LANG_INDEX_CACHE_SIZE_FAIL
	"无法获取流量统计数据",//C_LANG_INDEX_BANDWIDTH_FAIL
	"请求流量",//C_LANG_INDEX_BANDWIDTH_REQUEST
	"实际流量",//C_LANG_INDEX_BANDWIDTH_RESPONSE
	"节省流量",//C_LANG_INDEX_BANDWIDTH_SAVE
	"云容量",//C_LANG_INDEX_CACHE_TOTAL
	"已使用",//C_LANG_INDEX_CACHE_USED
	"未使用",//C_LANG_INDEX_CACHE_FREE,
	"检查云服务状态失败",//C_LANG_INDEX_CACHE_CHECK_FAIL
	"操作失败",//C_LANG_INDEX_OPERATION_FAIL
	"设备访问控制，ip地址段，输入错误",//C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR
	"IP 地址错误!",//C_LANG_INDEX_IP_PLAN_ADDR_ERR
	"策略名重复！",//C_LANG_INDEX_STRATEGY_NAME_REPEATED
	"已达最大数目！",//C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED
	"找不到该策略名！",//C_LANG_INDEX_CANT_FIND_STRA_NAME
	"策略名已经存在！",//C_LANG_INDEX_STRATEGY_NAME_EXISTS
	"读取edit_Name变量失败！",//C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR
	"策略名不能为空",//C_LANG_INDEX_STRATEGY_NAME_CANT_NULL
	"找不到该组配置",//C_LANG_INDEX_CANT_FIND_THE_GROP_CONF
	"所有用户",//C_LANG_INDEX_ALL_USERS
	"找不到该策略名！\n",//C_LANG_INDEX_CANT_FIND_STRA_NAME_GN
	"天",
	"时",
	"分",
	"秒",
	"天",
	"时",
	"分",
	"秒",
	"配置参数错误",//C_LANG_INDEX_CINFIG_PARAMETER_WRONG
	"无效地址池起始地址",
	"无效服务端IP地址",
	"无效地址池地址数",
	"服务器地址不能包含在地址池内",
	"地址池不能包含接口IP",//C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP
	"静态映射已经达到最大条数，无法建立pptp需要的静态映射",
	"参数错误！",
	"IP地址输入错误",
	"远端内网网段不能与本地内网网段重叠",
	"隧道名称输入重复！",
	"已达最大条目！",
	"找不到该隧道名称！",
	"读取数据失败",//C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR
	"远端内网网络地址输入错误！",
	"远端内网子网掩码输入错误！",
	"远端内网网段不能与本地内网网段重叠",
	"无效密码验证方式",
	"隧道名称已存在！",//C_LANG_INDEX_PPTP_CLIENT_TUNNEL_EXITS_ERR
	"读取配置失败",
	"用户名不能超过11个字节！",
	"密码不能超过31个字节！",
	"操作失败，系统中存在同名账号！",
	"禁止修改默认用户名",
	"找不到该用户",// C_LANG_INDEX_CANT_FIND_USER_NAME
	"不能删除默认管理员",//C_LANG_INDEX_CANT_DEL_DEFAULT_USER
	"任务名不能为空",
	"任务名输入重复！",
	"计划任务己经达到最大个数，不能添加新的计划任务！",
	"找不到该任务名！",
	"任务名长度不能超过11位！",//C_LANG_INDEX_TASK_NAME_SHOULD_LESS_11
	"无效IP地址",	    //C_LANG_INDEX_INVALID_IP
	"WAN口IP地址不能配置成广播地址",
	"LAN口IP地址不能与WAN口IP在同一网段",
	"无效接入方式",
	"更新策略失败，仍采用旧策略!",
	"更新策略成功!",
	"无效接口",
	"请先配置AP工作模式 APClient Mode",
	"读取edit_Name变量失败!",	//C_LANG_INDEX_READ_EDIT_NAME_ERR
	"源地址路由的起始地址大于结束地址",
	"检测IP不能是LAN口网段IP",
	"检测IP不能是WAN口IP",
	"LAN配置失败",
	"DHCP服务器错误",
	"变动后的DHCP地址池将包含LAN口地址，操作失败",
	"DHCP地址池包含了LAN口地址",
#if (MULTI_SSID_DHCP_POOL == FYES)
	"变动后的无线SSID1 DHCP地址池将包含LAN口地址，操作失败",
	"无线SSID1 DHCP地址池包含了LAN口地址",
	"变动后的无线SSID2 DHCP地址池将包含LAN口地址，操作失败",
	"无线SSID2 DHCP地址池包含了LAN口地址",
#if (WIRELESS_5G == FYES)
	"变动后的无线 5G SSID1 DHCP地址池将包含LAN口地址，操作失败",
	"无线 5G SSID1 DHCP地址池包含了LAN口地址",
	"变动后的无线 5G SSID2 DHCP地址池将包含LAN口地址，操作失败",
	"无线 5G SSID2 DHCP地址池包含了LAN口地址",
#endif
#endif
	"虚接口包含了LAN口地址",
	"找不到该用户名",//C_LANG_INDEX_USER_NAME_NOT_FIND
	"不能删除远程管理端口映射",// C_LANG_INDEX_NAT_MAP_DEL
	"找不到该静态映射名",
	"不能删除系统默认静态映射",
	"用户名不能为空!",// C_LANG_INDEX_USER_NAME_NOT_NULL
	"静态映射名不能为空!",
	"名称重复",//C_LANG_INDEX_NAME_REPEAT
	"找不到该条映射!",// C_LANG_INDEX_NAT_MAP_NOT_FIND
	"不能新建或者修改系统默认静态映射",
	"外部端口重叠!",
	"相同IP内部端口重叠!",
	"静态映射名已经存在!",
	"不能删除默认规则",
	"找不到该规则",
	"规则名重复",
	"绑定外部ip数过多!",
	"外部IP地址段重叠",
	"内部起始IP地址不和LAN口在同一网段!",//C_LANG_INDEX_INNER_START_IP_LAN
	"内部结束IP地址不和LAN口在同一网段!",// C_LANG_INDEX_INNER_END_IP_LAN
	"外部IP地址段不能包含各接口IP地址!",// C_LANG_INDEX_EXTER_IP_NOT_INCLUDE
	"外部IP地址段不能等于各接口IP地址!",//C_LANG_INDEX_EXTER_IP_NOT_EQUAL
	"内部IP地址段不能包含各接口IP地址!",//C_LANG_INDEX_INNER_IP_NOT_INCLUDE
	"内部IP地址段不能等于各接口IP地址!",
	"找不到路由名称!",
	"目的地址错误",
	"地址/掩码错误",
	"网关地址错误",
	"目的网络不能包含内网IP地址",
	"优先级错误",
	"路由名称输入重复!",
	"组名不能为空!",
	"组名已经存在!",
	"组ip个数不能大于",
	"该配置不存在!",//C_LANG_INDEX_THE_CONFIG_NOT_EXIST,
	"无法提交：该操作将使您无法登录设备!",//C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN
	"找不到该配置",
	"无法提交：只允许IP/MAC绑定用户，但该操作将使您不在绑定用户列表里，无法登录设备",
	"无法提交：该操作将使您登录的MAC与绑定用户列表里的MAC不一致，无法登录设备",
	"无法提交：该操作将使您登录的IP与绑定用户列表的IP不一致，无法登录设备",
	"共绑定",
	"条数据，失败",
	"条",//C_LANG_INDEX_FAIL_BIND_NUM
	"读取配置实例名失败!",//C_LANG_INDEX_READ_INST_NAME_ERR
	"已达最大配置条目，无法添加新配置！",//C_LANG_INDEX_NOT_ADD_NEW
	"用户名不能以sys.开头",//C_LANG_INDEX_NAME_NOT_BEGINE_WITH
	"输入重复!",//C_LANG_INDEX_INPUT_REPEAT
	"用户名输入重复!",//C_LANG_INDEX_USER_INPUT_REPEAT
	"该IP地址已做绑定，无法重复绑定!",//C_LANG_INDEX_IP_HAVE_BINDED
	"该MAC地址已做绑定，无法重复绑定!",//C_LANG_INDEX_MAC_HAVE_BINDED
	"该IP与MAC地址已做绑定，无法重复绑定!",//C_LANG_INDEX_IPMAC_HAVE_BINDED
	"无效起始IP地址",//C_LANG_INDEX_INVALID_START_IP_ADDR
	"无效主DNS服务器",
	"无效备DNS服务器",//C_LANG_INDEX_INVALID_SEC_DNS_SERVER
	"无效系统最大会话数",
	"存在非地址池内的固定IP分配",//C_LANG_INDEX_EXIT_FIX_IP_IN_POOL
	"LAN口IP地址不能包含在PPPoE地址池内",//C_LANG_INDEX_LAN_IP_CANT_IN_PPPoE_POOL
	"固定IP地址输入错误",
	"参数获取错误",
	"固定IP地址不在PPPoE地址池内！",//C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL
	"禁止绑定已分配IP",//C_LANG_INDEX_BANNED_BOUND_ALREADY_ASSIGN_IP
	"用户名已存在！",//C_LANG_INDEX_USER_NAME_ALREADY_EXITS
	"固定IP地址已存在",//C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS
	"实例已存在",//C_LANG_INDEX_CASE_ALREADY_EXITS
	"已达最大条目！",//C_LANG_INDEX_ALREADY_THE_MAX_TITLE
	"修改的实例不存在！",//C_LANG_INDEX_CHANGED_CASE_NOT_EXITS
	"被用户组",//C_LANG_INDEX_BE_USER_GROUP
	"引用，无法修改！",//C_LANG_INDEX_USE_CANT_BE_CHANGE
	"找不到用户名！",
	"引用，无法删除！",
	"按日期",
	"正常",
	"将过期",
	"过期",//C_LANG_INDEX_EXPIRE
	"请选择用户",
	"未找到实例！",//C_LANG_INDEX_DID_NOT_FIND_CASE
	"配置参数错误",
	"你没有权限进行此操作！",
	"提交参数有误，请重新输入！",
	"提交参数有误，新密码与确认密码不同！",//C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_PASSWD_DIFF
	"操作成功",//C_LANG_INDEX_SUCCESSFUL_OPERATION
	"旧密码填写错误！",//C_LANG_INDEX_OLD_PASSWD_CIN_ERR
	"修改已超过5次，今日不能修改！",//C_LANG_INDEX_HAVE_CHANGED_5_TIMES_CANT_TODAY

	"找不到组名!",
	"时间组",
	"正被引用，无法删除！",
	"实例已存在!",
	"修改的实例不存在！",
	"时间段名不能修改。",
	"请选择用户",
	"未找到实例！",
	"该组名已存在于行为管理信息列表或精细化限速信息列表中!",
	"找不到ID",//C_LANG_INDEX_ID_NOT_FIND
	"QQ号码已存在",//C_LANG_INDEX_QQ_NUM_EXIST
	"MSN账号已存在",//C_LANG_INDEX_MSN_EXIST
	"阿里旺旺账号已存在",//C_LANG_INDEX_ALI_EXIST
	"MSN为空。",
	"阿里旺旺帐号为空!",
	"ip地址数量不能大于65535!",
	"MD5非法，更新策略失败，仍采用旧策略!",
	"未使用",//C_LANG_INDEX_NOT_IN_USE
	"使用中",//C_LANG_INDEX_IN_USE
	"认证成功！",
	"认证失败！",
	"您提交的参数有误！",
	"新密码与确认密码不同",//C_LANG_INDEX_PASSWD_NEW_PASSWD_DIFF
	"您没有权限！",//C_LANG_INDEX_YOU_HAVE_NO_LEGAL_POWER
	"您修改超过5次",
	"此账号未连接",
	"您没有权限进行此操作！",//C_LANG_INDEX_NO_LEGAL_POWER_FOR_OPERATION
	"组",
	"的ip数量不能大于",
	"被规则使用，无法删除！",
	"被组",
	"使用，无法删除！",//C_LANG_INDEX_USING_CANT_DEL
	"和组",
	"类型不相同",//C_LANG_INDEX_TYPE_DIFF
	"组不能相互包含，",
	"包含",
	"组的深度不能大于",
	"，无法添加组",
	"不存在",//C_LANG_INDEX_NEGATION
	"与系统组名冲突",//C_LANG_INDEX_SYSTEM_GROUP_CONFLICT
	/*wusUserStats.c*/
	"聊天软件",//C_LANG_INDEX_SW_TALK
	"P2P软件",//C_LANG_INDEX_SW_P2P
	"股票软件",//C_LANG_INDEX_SW_STOCK
	"网络视频",//C_LANG_INDEX_SW_NET_VIDEO
	"网络游戏",//C_LANG_INDEX_SW_NET_GAME
	"购物网站",//C_LANG_INDEX_SW_SHOPPING_SITE
	"社交网站",//C_LANG_INDEX_SW_SOCIAL_NET_SITE
	"网页游戏",//C_LANG_INDEX_SW_WEB_GAME
	"邮件",//C_LANG_INDEX_SW_MAIL
	"论坛",//C_LANG_INDEX_SW_FORUM
	"其它",//C_LANG_INDEX_SW_OTHERS
	/*wusDhcpServer.c*/
#if (MULTI_SSID_DHCP_POOL == FYES)
	"无效有线起始地址",
	"无效有线结束地址",
	"无效的无线SSID1起始地址",
	"无效的无线SSID1结束地址",
	"无效的无线SSID2起始地址",
	"无效的无线SSID2结束地址",
#if (WIRELESS_5G == FYES)
	"无效的5G无线SSID1起始地址",
	"无效的5G无线SSID1结束地址",
	"无效的5G无线SSID2起始地址",
	"无效的5G无线SSID2结束地址",
#endif
#else
	"无效起始地址",
	"无效结束地址",
#endif
	"无效子网掩码",
	"无效网关",
	"起始地址和LAN口地址不在同一网络！",
	"起始地址和网关地址不在同一网络！",
	"起始地址和结束地址不在同一网络！",
	"结束地址过大！",
	"IP地址超出范围！",// C_LANG_INDEX_OVERFLOW_IP_ADDR
	"SSID不能为空",//C_LANG_INDEX_SSID_CANT_NULL
	"信道不能为空",//C_LANG_INDEX_CHANNEL_CANT_NULL
	"Radius服务器IP无效",//C_LANG_INDEX_RADIUS_INVAL_SER_IP
	/* wireless Wlan* */
	"MAC地址列表条目已经达到最大值！",//C_LANG_INDEX_MAC_ADDR_LIST_MAX
	"MAC地址列表中已有此地址",//C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR
	"无线MAC地址过滤列表已经达到最大条目！",//C_LANG_INDEX_MAC_FILTER_LIST_MAX
	"组中用户数已经达到最大值",//C_LANG_INDEX_USER_GROUP_LIST_MAX
	"隧道",//C_LANG_INDEX_USER
	"正在被静态路由引用",//C_LANG_INDEX_USER_IS_USE_IN_STATIC_ROUTE
	"组中已存在此用户名",//C_LANG_INDEX_USER_NAME_EXIT_IN_GROUP
	"无法创建实例",//C_LANG_INDEX_CANNOT_CREATE_INST

	/**************** REOS SE V1.2 *******************/

	"重命名失败",
	"VLAN ID超出取值范围",
	"服务区名称不能超过11个字节",
	"无效子网掩码",
	"密码长度不能超过11个字节",
	"端口应在0~65536之间",
	"无效Radius服务器IP",
	"Radius密码长度不能超过31个字节",
	"密码更新周期超出取值范围",
	"操作失败，服务区名称冲突",
	"操作失败，配置模板名称冲突",
	"操作失败，VLAN ID冲突",
	"操作失败,地址池已被使用",
	"不能删除默认选项",
	"软件上传中，请勿重复操作！",
	"无效的文件",
	"软件型号错误，请重新选择软件",
	"软件校验和错误",
	"无效的设置，相同型号相同硬件版本的软件只能设置一个为默认",
	"无效起始地址",
	"无效结束地址",
	"起始地址和网关地址不在同一网络！",
	"起始地址和结束地址不在同一网络！",
	"结束地址过大！",
	"操作失败，系统中存在同名帐号",
	"不能删除使用中的选项",
	"找不到删除的组",
	"找不到需要修改权限的AP",
	"结束序列号应大于起始序列号",
	"序列号重叠",
	"DHCP地址池地址和LAN口IP地址不在同一网络!",
	"已达到最大允许获取文件数",
	"剩余存储空间不足",
	"剩余内存不足",
	"操作失败",
	"导入图片太大，文件大小限制为60K。",	//C_LANG_INDEX_AP_DRAWING_FILE_LIMIT_D908W

	"正在被IPSec引用！",
	"已超过五次，需要重启设备才能导入。",
	"已成功导入License！",
	"您导入的为非法License,剩余导入次数为4次.",
	"您导入的为非法License,剩余导入次数为3次.",
	"您导入的为非法License,剩余导入次数为2次.",
	"您导入的为非法License,剩余导入次数为1次.",
	"您导入的为非法License,剩余导入次数为0次.",
	"WAN口数量设置不对.",	    //C_LANG_INDEX_DYN_WAN_SETNUM_ERR
	"端口数量未改变",	    //C_LANG_INDEX_DYN_WAN_NUM_NOCHANGE
	"设置已保存，手动重启后生效",	//C_LANG_INDEX_DYN_WAN_SETCONF_SAVE
	"正常",
	"将过期",
	"过期",
	"开启",
	"关闭",
	"账号过期",
	"没有剩余时间",
	"删除此WAN口配置，可能会导致策略路由失效",  //C_LANG_INDEX_POLICY_ROUTE_ERR
	"软件升级失败",
	"文件名过长（必须小于32个字符）！",
	"图片大小错误（必须小于10M）",
	"已成功导入图片！",
	"图片格式错误",
	"背景图片不存在，请导入",
	"写文件失败",	
	"文件格式错误",
	"配置文件读取错误",
    "地址池范围需要包含已绑定的静态IP地址",       //C_LANG_INDEX_STATIC_IP_NOT_IN_DHCP_POOL
	"PPPOE账号限速最多可配置10种限制组合",	//C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE
	"家长电脑MAC与小孩电脑MAC不能相同",
	"读取认证页面失败，请重新尝试或重启设备尝试",
	"上传文件格式错误",
	"上传文件大小错误",
	"已成功上传认证页面",
	"有线",//C_LANG_INDEX_INTERNET_WIRED
	"无线",//C_LANG_INDEX_INTERNET_WIRELESS
	"自动",//C_LANG_INDEX_APBASICINFO_AUTO
	"操作失败",//C_LANG_INDEX_APEFFECTCONF_FAIL
	"已达最大条目！",//C_LANG_INDEX_APMANAGE_TIP1
	"该地址所在地址池没被用作服务区，不能被绑定！",
	"没有该地址所对应的地址池，不能被绑定！",
	"该地址池没被用作服务区，不能被静态绑定！",
	"该地址所在地址池没被用作服务区，不能被绑定！",
	"与DHCP服务器地址池冲突",
	"与PPPoE服务器地址池冲突",
	"与L2TP服务器地址池冲突",
	"与PPTP服务器地址池冲突",
	"默认地址池的子网掩码不能在这修改，需要到LAN口配置去修改！",
    "此选项不可编辑",
	"未找到可使用的AC",//C_LANG_INDEX_FATFIT_SWITCH_NOTFINADAC
	"配置参数错误",//C_LANG_INDEX_L2TP_SERVER_PARAERR
	"静态映射已达最大条数，无法建立l2tp需要的静态映射",//C_LANG_INDEX_L2TP_SERVER_STATIC_ERR
	"vlan 类型错误!",//C_LANG_INDEX_PORT_VLAN_TYPE_ERR
	"操作失败：当前设备vlan类型，非端口vlan!",//C_LANG_INDEX_PORT_VLAN_FAIL_NOT_VLAN
	"共添加",
	"vlan名称不能超过11个字符",//C_LANG_INDEX_TV_NAME_OUT_OF_RANGE
	"操作失败，服务区名称冲突",//C_LANG_INDEX_TV_NAME_CONFLICT
    "固定IP地址输入重复！",
    "不能修改组名.",
    "该配置IP地址段和生效时间组合已经存在！",
    "颗粒度读取错误！",//C_LANG_INDEX_RATE_UNIT_ERR
	"已达到账号最大会话数",  //C_LANG_INDEX_MAX_ACC_SESSION
	"已达到系统最大会话数",  //C_LANG_INDEX_MAX_SYS_SESSION
	"申请Web认证会话失败",   //C_LANG_INDEX_APPLY_SES_ERR
	"不能大于系统最大会话数", //C_LANG_INDEX_INVALID_ACC_SESSION
	"无法连接，已达到VPN最大并发数", //C_LANG_INDEX_MAX_VPN_COUNT
	"无法添加，已达到VPN最大配置条目", //C_LANG_INDEX_MAX_VPN_ITEM
	"不能删除guest用户",//C_LANG_INDEX_CANT_DEL_GUEST_USER
    "不能修改guest用户",//C_LANG_INDEX_CANT_MODIFY_GUEST_USER
    "注意：共享文件繁忙，请稍后修改！",//C_LANG_INDEX_SHARE_FILE_IS_BUSY
    "无效接口地址",//C_LANG_INDEX_INVALID_INTERFACE_ADDR
    "天",//C_LANG_INDEX_SHORT_DAY
    "时",//C_LANG_INDEX_SHORT_HOUR
    "分",//C_LANG_INDEX_SHORT_MINUTE
    "秒",//C_LANG_INDEX_SHORT_SECOND
	"已被引用的虚接口无法删除！",//C_LANG_INDEX_CAN_NOT_DEL_VIF
	"最近1小时",//C_LANG_INDEX_AP_FLOW_LAST_HOUR
	"无法定位AP",//C_LANG_INDEX_CAN_NOT_LOCATION_AP
	"图片不能命名为default!请重新导入...",
	"该AP没有5GHz射频单元",//C_LANG_INDEX_AP_WIRELESS_NOT_SUPPORT_5G
	"接口名称绑定重复", //C_LANG_INDEX_DDNS_INTERFACE_REPEAT
	"绑定接口错误",    //C_LANG_INDEX_DDNS_INTERFACE_ERROR
        "获取网关MAC地址失败，请检查您的线路连接状态是否为已连接或者静态网关ip是否填写正确！", //C_LANG_INDEX_GET_GW_MAC_FAIL
	"设置日期过大，最大支持设置到2038年1月18日",//C_LANG_INDEX_TIMESET_TOO_LARGE
	""
};
