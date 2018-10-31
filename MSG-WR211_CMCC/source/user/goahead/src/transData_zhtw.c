
#include "translate.h"
const char* transData_zhtw[] = {
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
	"操作失敗",//C_LANG_INDEX_OPERATION_FAIL
	"設備存取控制，ip位址段，輸入錯誤",//C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR
	"IP 位址錯誤!",//C_LANG_INDEX_IP_PLAN_ADDR_ERR
	"策略名重複！",//C_LANG_INDEX_STRATEGY_NAME_REPEATED
	"已達最大數目！",//C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED
	"找不到該策略名！",//C_LANG_INDEX_CANT_FIND_STRA_NAME
	"策略名已經存在！",//C_LANG_INDEX_STRATEGY_NAME_EXISTS
	"讀取edit_Name變數失敗！",//C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR
	"策略名不能為空",//C_LANG_INDEX_STRATEGY_NAME_CANT_NULL
	"找不到該組配置",//C_LANG_INDEX_CANT_FIND_THE_GROP_CONF
	"所有用戶",//C_LANG_INDEX_ALL_USERS
	"找不到該策略名！\n",//C_LANG_INDEX_CANT_FIND_STRA_NAME_GN
	"天",
	"時",
	"分",
	"秒",
	"天",
	"時",
	"分",
	"秒",
	"配置參數錯誤",//C_LANG_INDEX_CINFIG_PARAMETER_WRONG
	"無效位址集區起始位址",
	"無效服務端IP位址",
	"無效位址集區地址數",
	"伺服器位址不能包含在位址集區內",
	"位址集區不能包含介面IP",//C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP
	"靜態映射已經達到最大條數，無法建立pptp需要的靜態映射",
	"參數錯誤！",
	"IP位址輸入錯誤",
	"遠端內網網段不能與本地內網網段重疊",
	"隧道名稱輸入重複！",
	"已達最大條目！",
	"找不到該隧道名稱！",
	"讀取數據失敗",//C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR
	"遠端內網網路位址輸入錯誤！",
	"遠端內網子網路遮罩輸入錯誤！",
	"遠端內網網段不能與本地內網網段重疊",
	"無效密碼驗證方式",
	"隧道名稱已存在！",//C_LANG_INDEX_PPTP_CLIENT_TUNNEL_EXITS_ERR
	"讀取配置失敗",
	"用戶名不能超過11個位元組！",
	"密碼不能超過31個位元組！",
	"操作失敗，系統中存在同名帳號！",
	"禁止修改預設使用者名",
	"找不到該用戶",
	"不能刪除默認管理員",//C_LANG_INDEX_CANT_DEL_DEFAULT_USER
	"任務名不能為空",
	"任務名輸入重複！",
	"計畫任務己經達到最大個數，不能添加新的計畫任務！",
	"找不到該任務名！",
	"任務名長度不能超過11位！",//C_LANG_INDEX_TASK_NAME_SHOULD_LESS_11
	"無效IP地址",	    //C_LANG_INDEX_INVALID_IP
	"WAN口IP位址不能配置成廣播位址",
	"LAN口IP位址不能與WAN口IP在同一網段",
	"無效接入方式",
	"更新策略失敗，仍採用舊策略!",
	"更新策略成功!",
	"無效介面",
	"請先配置AP工作模式 APClient Mode",
	"讀取edit_Name變數失敗!",	//C_LANG_INDEX_READ_EDIT_NAME_ERR
	"源位址路由的起始位址大於結束位址",
	"檢測IP不能是LAN口網段IP",
	"檢測IP不能是WAN口IP",
	"LAN配置失敗",
	"DHCP伺服器錯誤",
	"變動後的DHCP位址集區將包含LAN口位址，操作失敗",
	"DHCP位址集區包含了LAN口位址",
#if (MULTI_SSID_DHCP_POOL == FYES)
	"變動後的無線SSID1 DHCP位址集區將包含LAN口位址，操作失敗",
	"無線SSID1 DHCP位址集區將包含LAN口位址",
	"變動後的無線SSID2 DHCP位址集區將包含LAN口位址，操作失敗",
	"無線SSID2 DHCP位址集區將包含LAN口位址",
#if (WIRELESS_5G == FYES)
	"變動後的無線5G SSID1 DHCP位址集區將包含LAN口位址，操作失敗",
	"無線5G SSID1 DHCP位址集區將包含LAN口位址",
	"變動後的無線5G SSID2 DHCP位址集區將包含LAN口位址，操作失敗",
	"無線5G SSID2 DHCP位址集區將包含LAN口位址",
#endif
#endif
	"虚接口包含了LAN口地址",
	"找不到該用戶名",
	"不能刪除遠端系統管理埠映射",
	"找不到該靜態映射名",
	"不能刪除系統預設靜態映射",
	"用戶名不能為空!",
	"靜態映射名不能為空!",
	"名稱重複",
	"找不到該條映射!",
	"不能新建或者修改系統預設靜態映射",
	"外部埠重疊!",
	"相同IP內部埠重疊!",
	"靜態映射名已經存在!",
	"不能刪除預設規則",
	"找不到該規則",
	"規則名重複",
	"綁定外部ip數過多!",
	"外部IP位址段重疊",
	"內部起始IP位址不和LAN口在同一網段!",
	"內部結束IP位址不和LAN口在同一網段!",
	"外部IP位址段不能包含各介面IP位址!",
	"外部IP位址段不能等於各介面IP位址!",
	"內部IP位址段不能包含各介面IP位址!",
	"內部IP位址段不能等於各介面IP位址!",
	"找不到路由名稱!",
	"目的地址錯誤",
	"位址/遮罩錯誤",
	"閘道位址錯誤",
	"目的網路不能包含內網IP位址",
	"優先順序錯誤",
	"路由名稱輸入重複!",
	"組名不能為空!",
	"組名已經存在!",
	"組ip個數不能大於",
	"該配置不存在!",
	"無法提交：該操作將使您無法登錄設備!",
	"找不到該配置",
	"無法提交：只允許IP/MAC綁定用戶，但該操作將使您不在綁定用戶清單裡，無法登錄設備",
	"無法提交：該操作將使您登錄的MAC與綁定用戶列表裡的MAC不一致，無法登錄設備",
	"無法提交：該操作將使您登錄的IP與綁定用戶列表的IP不一致，無法登錄設備",
	"共綁定",
	"條數據，失敗",
	"條",
	"讀取配置實例名失敗!",
	"已達最大配置條目，無法添加新配置！",
	"用戶名不能以sys.開頭",
	"輸入重複!",
	"用戶名輸入重複!",
	"該IP地址已做綁定，無法重複綁定!",
	"該MAC地址已做綁定，無法重複綁定!",
	"該IP與MAC地址已做綁定，無法重複綁定!",
	"無效起始IP位址",//C_LANG_INDEX_INVALID_START_IP_ADDR
	"無效主DNS伺服器",
	"無效備DNS伺服器",//C_LANG_INDEX_INVALID_SEC_DNS_SERVER
	"無效系統最大會話數",
	"存在非位址集區內的固定IP分配",//C_LANG_INDEX_EXIT_FIX_IP_IN_POOL
	"LAN口IP位址不能包含在PPPoE位址集區內",//C_LANG_INDEX_LAN_IP_CANT_IN_PPPoE_POOL
	"固定IP位址輸入錯誤",
	"參數獲取錯誤",
	"固定IP地址不在PPPoE位址集區內！",//C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL
	"禁止綁定已分配IP",
	"用戶名已存在！",
	"固定IP地址已存在",//C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS
	"實例已存在",//C_LANG_INDEX_CASE_ALREADY_EXITS
	"已達最大條目！",//C_LANG_INDEX_ALREADY_THE_MAX_TITLE
	"修改的實例不存在！",//C_LANG_INDEX_CHANGED_CASE_NOT_EXITS
	"被用戶組",//C_LANG_INDEX_BE_USER_GROUP
	"引用，無法修改！",//C_LANG_INDEX_USE_CANT_BE_CHANGE
	"找不到用戶名！",
	"引用，無法刪除！",
	"按日期",
	"正常",
	"將過期",
	"過期",//C_LANG_INDEX_EXPIRE
	"請選擇用戶",
	"未找到實例！",//C_LANG_INDEX_DID_NOT_FIND_CASE
	"配置參數錯誤",
	"你沒有許可權進行此操作！",
	"提交參數有誤，請重新輸入！",
	"提交參數有誤，新密碼與確認密碼不同！",//C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_PASSWD_DIFF
	"操作成功",//C_LANG_INDEX_SUCCESSFUL_OPERATION
	"舊密碼填寫錯誤！",//C_LANG_INDEX_OLD_PASSWD_CIN_ERR
	"修改已超過5次，今日不能修改！",//C_LANG_INDEX_HAVE_CHANGED_5_TIMES_CANT_TODAY

	"找不到組名!",
	"時間組",
	"正被引用，無法刪除！",
	"實例已存在!",
	"修改的實例不存在！",
	"時間段名不能修改。",
	"請選擇用戶",
	"未找到實例！",
	"該組名已存在于行為管理資訊清單或精細化限速資訊清單中!",
	"找不到ID",
	"QQ號碼已存在",
	"MSN帳號已存在",
	"MSN為空。",
	"ip位址數量不能大於65535!",
	"MD5非法，更新策略失敗，仍採用舊策略!",
	"未使用",//C_LANG_INDEX_NOT_IN_USE
	"使用中",//C_LANG_INDEX_IN_USE
	"認證成功",
	"認證失敗",
	"您提交的參數有誤！",
	"新密碼與確認密碼不同",//C_LANG_INDEX_PASSWD_NEW_PASSWD_DIFF
	"您沒有許可權！",//C_LANG_INDEX_YOU_HAVE_NO_LEGAL_POWER
	"您修改超過5次",
	"此帳號未連接",
	"您沒有許可權進行此操作！",//C_LANG_INDEX_NO_LEGAL_POWER_FOR_OPERATION
	"組",
	"的ip數量不能大於",
	"被規則使用，無法刪除！",
	"被組",
	"使用，無法刪除！",//C_LANG_INDEX_USING_CANT_DEL
	"和組",
	"類型不相同",//C_LANG_INDEX_TYPE_DIFF
	"組不能相互包含，",
	"包含",
	"組的深度不能大於",
	"，無法添加組",
	"不存在",//C_LANG_INDEX_NEGATION
	"與系統組名衝突",//C_LANG_INDEX_SYSTEM_GROUP_CONFLICT
	/*wusUserStats.c*/
	"聊天軟體",//C_LANG_INDEX_SW_TALK
	"P2P軟體",//C_LANG_INDEX_SW_P2P
	"股票軟體",//C_LANG_INDEX_SW_STOCK
	"網路視頻",//C_LANG_INDEX_SW_NET_VIDEO
	"網路遊戲",//C_LANG_INDEX_SW_NET_GAME
	"購物網站",//C_LANG_INDEX_SW_SHOPPING_SITE
	"社交網站",//C_LANG_INDEX_SW_SOCIAL_NET_SITE
	"網頁遊戲",//C_LANG_INDEX_SW_WEB_GAME
	"郵件",//C_LANG_INDEX_SW_MAIL
	"論壇",//C_LANG_INDEX_SW_FORUM
	"其它",//C_LANG_INDEX_SW_OTHERS
	/*wusDhcpServer.c*/
#if (MULTI_SSID_DHCP_POOL == FYES)
	"無效有線起始位址",
	"無效有線結束地址",
	"無效無線SSID1起始位址",
	"無效無線SSID1結束地址",
	"無效無線SSID2起始位址",
	"無效無線SSID2結束地址",
#if (WIRELESS_5G == FYES)
	"無效5G無線SSID1起始地址",
	"無效5G無線SSID1结束地址",
	"無效5G無線SSID2起始地址",
	"無效5G無線SSID2结束地址",
#endif
#else
	"無效起始位址",
	"無效結束地址",
#endif
	"無效子網路遮罩",
	"無效閘道",
	"起始位址和LAN位址不在同一網路！",
	"起始位址和閘道位址不在同一網路！",
	"起始位址和結束位址不在同一網路！",
	"結束地址過大！",
	"IP地址超出範圍！",
	"SSID不能為空",//C_LANG_INDEX_SSID_CANT_NULL
	"通道不能為空",//C_LANG_INDEX_CHANNEL_CANT_NULL
	"Radius伺服器IP無效",//C_LANG_INDEX_RADIUS_INVAL_SER_IP
	/* wireless Wlan* */
	"MAC地址清單條目已經達到最大值！",
	"MAC地址清單中已有此地址",
	"無線MAC地址過濾列表已經達到最大條目！",//C_LANG_INDEX_MAC_FILTER_LIST_MAX
	"组中用户数已经达到最大值",
	"隧道",
	"正在被静态路由引用",
	"組中已存在此用戶名",
	"无法创建实例",

/**************** REOS SE V1.2 *******************/

	"重命名失敗",
	"VLAN ID超出取值範圍",
	"服務區名稱不能超過11個字節",
	"無效子網掩碼",
	"密碼長度不能超過11個字節",
	"端口應在0~65536之間",
	"無效Radius服務器IP",
	"Radius密碼長度不能超過31個字節",
	"密碼更新週期超出取值範圍",
	"操作失敗，服務區名稱衝突",
	"操作失敗，配置模板名稱衝突",
	"操作失敗，VLAN ID衝突",
	"操作失敗，地址池已被使用",
	"不能刪除默認選項",
	"軟件上傳中， 請勿重複操作！",
	"無效的文件",
	"軟件型號錯誤，請重新選擇軟件",
	"軟件校驗和錯誤",
	"無效的設置，相同型號相同硬件版本的軟件只能設置一個為默認",
	"無效起始地址",
	"無效結束地址",
	"起始地址和網関地址不在同一網絡！",
	"起始地址和結束地址不在同一網絡！",
	"結束地址過大！",
	"操作失敗，系統中存在同名帳號",
	"不能刪除使用中的選項",
	"找不到刪除的組",
	"找不到需要修改權限的AP",
	"結束序列號應大於起始序列號",
	"序列號重疊",
	"DHCP地址池地址和LAN口IP地址不在同一網絡!",

	"已達到最大允許獲取文件數",
	"剩餘存儲空間不足",
	"剩餘內存不足",
	"操作失敗",
	"导入图片太大，文件大小限制为60K。",	//C_LANG_INDEX_AP_DRAWING_FILE_LIMIT_D908W
	"開啟",
	"關閉",
	"帳號過期",
	"沒有剩餘時間",
	"删除此WAN口配置，可能会导致策略路由失效",
	"软件升级失败",
	"图片名过长（必须小于32个字符）",
	"图片大小错误（必须小于32K）",
	"已成功导入图片",
	"图片格式错误",
	"背景图片不存在，请导入",
	"写文件失败",
	"配置文件格式错误",
	"配置文件读取错误",

    "地址池范围需要包含已绑定的静态IP地址",       //C_LANG_INDEX_STATIC_IP_NOT_IN_DHCP_POOL
	"PPPOE账号限速最多可配置10种限制组合"	//C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE
	"家长电脑MAC与小孩电脑MAC不能相同",
	"读取认证页面失败，请重新尝试或重启设备尝试",
	"上传文件格式错误",
	"上传文件大小错误",
	"已成功上传认证页面",
	"共添加",
	"tw_vlan名称不能超过11个字符",//C_LANG_INDEX_TV_NAME_OUT_OF_RANGE
	"tw_操作失敗，vlan名稱衝突",//C_LANG_INDEX_TV_NAME_CONFLICT
    "tw_固定IP地址输入重复！",
    "tw_不能修改组名.",
    "tw_该配置IP地址段和生效时间组合已经存在！",
    "tw_颗粒度读取错误！",//C_LANG_INDEX_RATE_UNIT_ERR
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
    "時",//C_LANG_INDEX_SHORT_HOUR
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


