
#include "translate.h"
const char* transData_en[] = {
	"Unable to get the cloud setting options",//C_LANG_INDEX_CACHE_LIST_FAIL
	"Unable to get space usage",//C_LANG_INDEX_CACHE_SIZE_FAIL
	"Unable to get the traffic statistics",//C_LANG_INDEX_BANDWIDTH_FAIL
	"Request traffic",//C_LANG_INDEX_BANDWIDTH_REQUEST
	"Actual flow",//C_LANG_INDEX_BANDWIDTH_RESPONSE
	"Saving flow",//C_LANG_INDEX_BANDWIDTH_SAVE
	"Cloud Capacity",//C_LANG_INDEX_CACHE_TOTAL
	"Used",//C_LANG_INDEX_CACHE_USED
	"Unused",//C_LANG_INDEX_CACHE_FREE,
	"Failed to check the cloud service status",//C_LANG_INDEX_CACHE_CHECK_FAIL
	"Operation failed",//C_LANG_INDEX_OPERATION_FAIL
	"Device access restriction: Incorrect IP address range.",//C_LANG_INDEX_DECIVE_ACCESS_CTL_IP_ERR
	"Incorrect IP address.",//C_LANG_INDEX_IP_PLAN_ADDR_ERR
	"Duplicate rule name.",//C_LANG_INDEX_STRATEGY_NAME_REPEATED
	"Maximum number of entries has been reached. ",//C_LANG_INDEX_MAXIMUM_NUM_HAS_REACHED
	"Cannot find this rule name.",//C_LANG_INDEX_CANT_FIND_STRA_NAME
	"Rule name already exists.",//C_LANG_INDEX_STRATEGY_NAME_EXISTS
	"Failed to read the variable of edit_Name.",//C_LANG_INDEX_READ_EDIT_NAME_VAL_ERR
	"Rule name cannot be blank.",//C_LANG_INDEX_STRATEGY_NAME_CANT_NULL
	"Cannot find the entry",//C_LANG_INDEX_CANT_FIND_THE_GROP_CONF
	"All Users",//C_LANG_INDEX_ALL_USERS
	"Cannot find this rule name.\n",//C_LANG_INDEX_CANT_FIND_STRA_NAME_GN
	"Day",
	"Hour",
	"Minute",
	"Second",
	"D",
	"H",
	"M",
	"S",
	"Incorrect parameter",//C_LANG_INDEX_CINFIG_PARAMETER_WRONG
	"Invalid IP pool start IP address",
	"Invalid server IP address",
	"Number of addresses is invalid",
	"Server IP address cannot belong to IP address pool.",
	"IP address pool cannot include interface IP address",//C_LANG_INDEX_ADDR_POOL_CANT_CONTAIN_INF_IP
	"Maximum number of port forwarding entries has been reached. Cannot create a port forwarding entry for PPTP.",
	"Incorrect parameter.",
	"Incorrect IP address.",
	"The remote and local LAN subnets cannot overlap.",
	"Duplicate tunnel name.",
	"Maximum number of entries has been reached.",
	"Cannot find the tunnel name.",
	"Failed to read data",//C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR
	"Remote LAN IP address is incorrect.",
	"Remote LAN subnet mask is incorrect.",
	"The remote and local LAN subnets cannot overlap.",
	"Invalid PPP authentication mode",
	"Tunnel name already exists.",//C_LANG_INDEX_PPTP_CLIENT_TUNNEL_EXITS_ERR
	"Failed to read configuration.",
	"User name must be at most 11 characters long.",
	"Password must be at most 31 characters long.",
	"Operation failed: an account with this user name already exists.",
	"Cannot edit default administrator user name.",
	"Cannot find this user.",
	"Cannot delete the default administrator account.",//C_LANG_INDEX_CANT_DEL_DEFAULT_USER
	"Task name cannot be blank",
	"Duplicate task name.",
	"Maximum number of scheduled task entries has been reached. Cannot add new entry.",
	"Cannot find this task name.",
	"Task name must be at most 11 characters long. ",//C_LANG_INDEX_TASK_NAME_SHOULD_LESS_11
	"Invalid IP address",	    //C_LANG_INDEX_INVALID_IP
	"Cannot assign broadcast address to WAN interface.",
	"The LAN and WAN interface IP addresses must be on different subnets.",
	"Invalid connection type",
	"Failed to update policy. The existing policy is still in use.",
	"The policy is updated successfully.",
	"Invalid interface",
	"Please choose APClient Mode as the Operation Mode.",
	"Failed to read the variable of edit_Name.",	//C_LANG_INDEX_READ_EDIT_NAME_ERR
	"Source address route's start IP address is greater than end IP address.",
	"Detection target IP address cannot be on the same subnet as LAN interface",
	"Detection target IP address cannot be WAN interface IP address",
	"Failed to configure LAN settings",
	"DHCP server error",
	"Operation failed: DHCP address pool cannot include LAN interface IP address",
	"DHCP address pool cannot include LAN interface IP address",
#if (MULTI_SSID_DHCP_POOL == FYES)
	"Operation failed: Wireless SSID1 DHCP address pool cannot include LAN interface IP address",
	"Wireless SSID1 DHCP address pool cannot include LAN interface IP address",
	"Operation failed: Wireless SSID2 DHCP address pool cannot include LAN interface IP address",
	"Wireless SSID2 DHCP address pool cannot include LAN interface IP address",
#if (WIRELESS_5G == FYES)
	"Operation failed: Wireless 5G SSID1 DHCP address pool cannot include LAN interface IP address",
	"Wireless 5G SSID1 DHCP address pool cannot include LAN interface IP address",
	"Operation failed: Wireless 5G SSID2 DHCP address pool cannot include LAN interface IP address",
	"Wireless 5G SSID2 DHCP address pool cannot include LAN interface IP address",
#endif
#endif
	"虚接口包含了LAN口地址",
	"Cannot find this user name",
	"Cannot delete the port forwarding entry used for remote management.",
	"Cannot find this port forwarding name.",
	"Cannot delete default port forwarding entries.",
	"User name cannot be blank.",
	"Port forwarding name cannot be blank.",
	"Duplicate name.",
	"Cannot find this port forwarding entry.",
	"Cannot create or modify default port forwarding entries.",
	"The external port ranges cannot overlap.",
	"The internal port ranges (with the same IP address) cannot overlap.",
	"Port forwarding name already exists.",
	"Cannot delete default rules.",
	"Cannot find this rule",
	"Duplicate rule name",
	"The number of external IP addresses exceeds the limit.",
	"The external IP address ranges cannot overlap.",
	"Start internal IP address must be on the same subnet as LAN interface.",
	"End internal IP address must be on the same subnet as LAN interface.",
	"External IP address range cannot include interface IP address.",
	"External IP address cannot be interface IP address.",
	"Internal IP address range cannot include interface IP address.",
	"Internal IP address cannot be interface IP address.",
	"Cannot find route name",
	"Incorrect destination IP address",
	"Incorrect IP address/subnet mask",
	"Incorrect gateway IP address",
	"Destination network cannot include local LAN IP addresses.",
	"Incorrect priority",
	"Duplicate route name.",
	"Group name cannot be blank.",
	"Group name already exists.",
	"The number of IP addresses within the group must be at most ",
	"This entry doesn't exist.",
	"Cannot submit: this operation will make you unable to login to the Device.",
	"Cannot find this entry.",
	"Cannot submit: this operation will remove you from IP/MAC Binding List; then you cannot login to the Device because only IP/MAC binding users are allowed to access. ",
	"Cannot submit: this operation will make your computer's MAC address different from the IP/MAC binding's, so you cannot login to the Device.",
	"Cannot submit: this operation will make your computer's IP address different from the IP/MAC binding's, so you cannot login to the Device.",
	"A total of",
	"entries are bound. Failed to bind",
	"entries.",
	"Failed to read entry name.",
	"Maximum number of entries has been reached. Cannot add new entry.",
	"User name cannot start with sys.",
	"Duplicate input.",
	"Duplicate user name.",
	"Cannot bind: The IP address is already bound to another MAC address.",
	"Cannot bind: The MAC address is already bound to another IP address.",
	"Cannot bind: The IP and MAC address pair is already bound.",
	"Invalid start IP address",//C_LANG_INDEX_INVALID_START_IP_ADDR
	"Invalid primary DNS server",
	"Invalid secondary DNS server",//C_LANG_INDEX_INVALID_SEC_DNS_SERVER
	"Invalid maximum sessions",
	"There is one or more static IP addresses outside of address pool.",//C_LANG_INDEX_EXIT_FIX_IP_IN_POOL
	"PPPoE address pool cannot include LAN interface IP address",//C_LANG_INDEX_LAN_IP_CANT_IN_PPPoE_POOL
	"Incorrect static IP address",
	"Failed to get parameters",
	"Static IP address must belong to PPPoE address pool.",//C_LANG_INDEX_STATIC_IP_ADDR_CANT_IN_PPPoE_POOL
	"Cannot bind the IP address already assigned to a user.",
	"User name already exists.",
	"Static IP address already exists.",//C_LANG_INDEX_STATIC_IP_ADDR_ALREADY_EXITS
	"Entry already exists.",//C_LANG_INDEX_CASE_ALREADY_EXITS
	"Maximum number of entries has been reached. ",//C_LANG_INDEX_ALREADY_THE_MAX_TITLE
	"Entry doesn't exist.",//C_LANG_INDEX_CHANGED_CASE_NOT_EXITS
	"is referenced by user group",//C_LANG_INDEX_BE_USER_GROUP
	"and therefore cannot be modified.",//C_LANG_INDEX_USE_CANT_BE_CHANGE
	"Cannot find user name.",
	"and therefore cannot be deleted.",
	"By Date",
	"Open",
	"Will Expire Soon",
	"Expired",//C_LANG_INDEX_EXPIRE
	"Please select user(s)",
	"No entry found.",//C_LANG_INDEX_DID_NOT_FIND_CASE
	"Incorrect parameter",
	"You do not have permission to do this operation.",
	"Incorrect input. Please re-enter. ",
	"Incorrect input. New password fields don't match.",//C_LANG_INDEX_SUBMIT_WRONG_PARMETERS_PASSWD_DIFF
	"Operation succeeded",//C_LANG_INDEX_SUCCESSFUL_OPERATION
	"Old password is incorrect.",//C_LANG_INDEX_OLD_PASSWD_CIN_ERR
	"It has been changed 5 times, and cannot be changed again today.",//C_LANG_INDEX_HAVE_CHANGED_5_TIMES_CANT_TODAY

	"Cannot find group name.",
	"Schedule",
	"cannot be deleted because it is being referenced.",
	"Entry already exists.",
	"Entry doesn't exist.",
	"Cannot modify schedule name.",
	"Please select user(s)",
	"No entry found.",
	"The group name already exists in Behavior Management List or Fixed Rate Limiting Rule List.",
	"Cannot find ID.",
	"QQ number already exists.",
	"MSN account already exists.",
	"阿里旺旺账号已存在",//C_LANG_INDEX_ALI_EXIST
	"MSN account is blank.",
	"阿里旺旺帐号为空!",
	"The number of IP addresses cannot be larger than 65535.",
	"Invalid MD5. Failed to update policy. The existing policy is still in use.",
	"Unused",//C_LANG_INDEX_NOT_IN_USE
	"In Use",//C_LANG_INDEX_IN_USE
	"Authentication succeeded",
	"Authentication failed",
	"Incorrect parameters were submitted.",
	"New password fields don't match.",//C_LANG_INDEX_PASSWD_NEW_PASSWD_DIFF
	"You do not have permission.",//C_LANG_INDEX_YOU_HAVE_NO_LEGAL_POWER
	"You have changed it more than 5 times.",
	"This account is disconnected.",
	"You do not have permission to do this operation.",//C_LANG_INDEX_NO_LEGAL_POWER_FOR_OPERATION
	"Group ",
	": number of IP addresses cannot be larger than",
	"cannot be deleted because it is being referenced.",
	" is included in group ",
	" and therefore cannot be deleted.",//C_LANG_INDEX_USING_CANT_DEL
	"and group",
	": different types",//C_LANG_INDEX_TYPE_DIFF
	"Groups cannot be nested within each other: ",
	"includes",
	"Groups cannot be nested deeper than ",
	" levels. Cannot add group ",
	"doesn't exist",//C_LANG_INDEX_NEGATION
	"conflicts with system default group name",//C_LANG_INDEX_SYSTEM_GROUP_CONFLICT
	/*wusUserStats.c*/
	"IM",//C_LANG_INDEX_SW_TALK
	"P2P",//C_LANG_INDEX_SW_P2P
	"Stock Software",//C_LANG_INDEX_SW_STOCK
	"Network Video",//C_LANG_INDEX_SW_NET_VIDEO
	"Online Game",//C_LANG_INDEX_SW_NET_GAME
	"Shopping Site",//C_LANG_INDEX_SW_SHOPPING_SITE
	"SNSs",//C_LANG_INDEX_SW_SOCIAL_NET_SITE
	"Web Game",//C_LANG_INDEX_SW_WEB_GAME
	"E-mail",//C_LANG_INDEX_SW_MAIL
	"Forum",//C_LANG_INDEX_SW_FORUM
	"Others",//C_LANG_INDEX_SW_OTHERS
	/*wusDhcpServer.c*/
#if (MULTI_SSID_DHCP_POOL == FYES)
	"Invalid wired start IP address",
	"Invalid wired end IP address",
	"Invalid wireless ssid1 start IP address",
	"Invalid wireless ssid1 end IP address",
	"Invalid wireless ssid2 start IP address",
	"Invalid wireless ssid2 end IP address",
#if (WIRELESS_5G == FYES)
	"Invalid wireless 5G ssid1 start IP address",
	"Invalid wireless 5G ssid1 end IP address",
	"Invalid wireless 5G ssid2 start IP address",
	"Invalid wireless 5G ssid2 end IP address",
#endif
#else
	"Invalid start IP address",
	"Invalid end IP address",
#endif
	"Invalid subnet mask",
	"Invalid gateway IP address",
	"Start IP address and LAN IP address are not on the same subnet.",
	"Start IP address and gateway IP address are not on the same subnet.",
	"Start IP address and end IP address are not on the same subnet.",
	"End IP address is too large.",
	"IP address is out of range.",
	"SSID cannot be blank",//C_LANG_INDEX_SSID_CANT_NULL
	"Channel cannot be blank",//C_LANG_INDEX_CHANNEL_CANT_NULL
	"RADIUS server IP is invalid",//C_LANG_INDEX_RADIUS_INVAL_SER_IP
	/* wireless Wlan* */
	"Maximum number of MAC address filtering entries has been reached",
	"MAC address already exists",
	"Maximum number of MAC address filtering entries has been reached",//C_LANG_INDEX_MAC_FILTER_LIST_MAX
	"Maximum number of users in the group has been reached.",
	"Tunnel",
	" is being referenced by static route.",
	"Group already exists in the user name",
	"Cannot create entry.",

/**************** REOS SE V1.2 *******************/

	"Rename failed.",
	"VLAN ID is out of range.",
	"Name must be at most 11 bytes.",
	"Invalid gateway IP address",
	"Password must be at most 11 characters long.",
	"The port number should be between 0 and 65535.",
	"RADIUS server IP is invalid",
	"The Shared Secret must be at most 31 characters long.",
	"Password update cycle beyond the range of",
	"Operation failed: duplicate name.",
	"Operation failed: duplicate template name.",
	"Operation failed: duplicate VLAN ID.",
	"Operation failed: the DHCP address pool has been used.",
	"Cannot delete the default option.",
	"Firmware upgrading, please don't operate it repeatedly.",
	"Invalid file.",
	"Firmware model doesn't match, please choose another one and upgrade it again.",
	"Upgrade failed, CRC error.",
	"Operation failed: For the same model and hardware version, there are must be one and only one firmware selected as the default.",
	"Invalid start IP address",
	"Invalid end IP address",
	"Start IP address and gateway IP address are not on the same subnet.",
	"Start IP address and end IP address are not on the same subnet.",
	"End IP address is too large.",
	"Operation failed: an account with this user name already exists.",
	"Cannot delete use options.",
	"Delete group could not be found.",
	"Cannot find  the need to modify the permissions of the AP.",
	"The start SN cannot be larger than the end SN.",
	"The SN should not overlap.",
	"The addresses of the DHCP address pool and the IP addresses of LAN Interface are not on the same subnet.",
	"The file number allowed access has reached the maximum.",
	"Insufficient storage space remaining.",
	"Insufficient memory remaining.",
	"Operation failed",
	"导入图片太大，文件大小限制为60K。",	//C_LANG_INDEX_AP_DRAWING_FILE_LIMIT_D908W

	" is being referenced by IPSec.", //C_LANG_INDEX_USER_IS_USE_IN_IPSEC
	"It has been changed 5 times,  the need to restart the device to import.",
	"License update succeed.",
	"Invalid License, remaining 4 times.",
	"Invalid License, remaining 3 times.",
	"Invalid License, remaining 2 times.",
	"Invalid License, remaining 1 times.",
	"Invalid License, remaining 0 times.",
	"The number of WAN interfaces settings is incorrect.",	    //C_LANG_INDEX_DYN_WAN_SETNUM_ERR
	"The number of WAN interfaces isn't changed.",		    //C_LANG_INDEX_DYN_WAN_NUM_NOCHANGE
	"The change has been saved. You can now restart the Device for the change to take effect.",	//C_LANG_INDEX_DYN_WAN_SETCONF_SAVE
	"Open",
	"Will Expire Soon",
	"Expired",
	"Enabled",
	"Close",
	"Account expiration",
	"No time left",
	"Delete this WAN port configuration, policy routing may cause failure",
	"Upgrade Failed",
	"File name is too long (Less than 32 Characters)",
	"Invalid image size (Less than 10M)",
	"Import the image successfully",
	"Invalid image Format",
	"The picture doesn't exist, please import ",
	"Failed to write the file",
	"Invaled config file format",
	"Reads config file error",
    "Address pool needs to contain has static binding",      //C_LANG_INDEX_STATIC_IP_NOT_IN_DHCP_POOL
	"PPPoE account rate-limiting must be at most 10 combinations.",	//C_LANG_INDEX_PPPOE_LIMIT_CANT_MORE
	"Parents' MAC and kids' MAC must be different",
	"Failed to get authentication page, try again or reboot the device",
	"The format of the file is in error",
	"The size of the file is in error",
	"Authentication page upload successfully",
	"Wired",
	"Wireless",
	"Auto",
	"operation failed", //C_LANG_INDEX_APEFFECTCONF_FAIL
	"Maximum!",
	"The address pool which contains the address is not one of the WLANs, binding is not allowed. ",
	"No address pool contains the address, binding is not allowed.",
	"The address pool is not one of the WLANs, binding is not allowed.",
	"The address pool which contains the address is not one of the WLANs, binding is not allowed.",
	"conflicts with other DHCP address pool",
	"conflicts with other PPPOE address pool",
	"conflicts with other L2TP address pool",
	"conflicts with other PPTP address pool",
	"You can't modify the netmask of the default pool here.You can modify it in Network->LAN page.",
	"This option is not allowed to be modified",
	"There is no AC available",//C_LANG_INDEX_FATFIT_SWITCH_NOTFINADAC
	"Configuration Error",//C_LANG_INDEX_L2TP_SERVER_PARAERR
	"Maximum number of static nat has been reached.You can't create any static nat for l2tp now.",//C_LANG_INDEX_L2TP_SERVER_STATIC_ERR
	"vlan type error!",//C_LANG_INDEX_PORT_VLAN_TYPE_ERR
	"operation failed: it's not port vlan!",//C_LANG_INDEX_PORT_VLAN_FAIL_NOT_VLAN
	"Total",
	"Vlan name must be at most 11 bytes.", //C_LANG_INDEX_TV_NAME_OUT_OF_RANGE
	"Operation failed: duplicate name.",//C_LANG_INDEX_TV_NAME_CONFLICT
    "Duplicate static IP Address.",
    "Cannot modify group name.",
    "The combination of IP address and the effective time already exists.",
    "rate unit wrong!",
	"已达到账号最大会话数",  //C_LANG_INDEX_MAX_ACC_SESSION
	"已达到系统最大会话数",  //C_LANG_INDEX_MAX_SYS_SESSION
	"申请Web认证会话失败",   //C_LANG_INDEX_APPLY_SES_ERR
	"不能大于系统最大会话数", //C_LANG_INDEX_INVALID_ACC_SESSION
	"无法连接，已达到VPN最大并发数", //C_LANG_INDEX_MAX_VPN_COUNT
	"无法添加，已达到VPN最大配置条目", //C_LANG_INDEX_MAX_VPN_ITEM
	"不能删除guest用户",//C_LANG_INDEX_CANT_DEL_GUEST_USER
    "不能修改guest用户",//C_LANG_INDEX_CANT_MODIFY_GUEST_USER
    "Note: the file sharing is busy, please revise later!",//C_LANG_INDEX_SHARE_FILE_IS_BUSY
    "Invalid interface address",//C_LANG_INDEX_INVALID_INTERFACE_ADDR
    "D",//C_LANG_INDEX_SHORT_DAY
    "H",//C_LANG_INDEX_SHORT_HOUR
    "M",//C_LANG_INDEX_SHORT_MINUTE
    "S",//C_LANG_INDEX_SHORT_SECOND
	"已被引用的虚接口无法删除！",//C_LANG_INDEX_CAN_NOT_DEL_VIF
	"last hour",//C_LANG_INDEX_AP_FLOW_LAST_HOUR
	"无法定位AP",//C_LANG_INDEX_CAN_NOT_LOCATION_AP
	"图片不能命名为default!请重新导入...",
	"该AP没有5GHz射频单元",//C_LANG_INDEX_AP_WIRELESS_NOT_SUPPORT_5G
	"接口名称绑定重复", //C_LANG_INDEX_DDNS_INTERFACE_REPEAT
	"绑定接口错误",    //C_LANG_INDEX_DDNS_INTERFACE_ERROR
        "获取网关MAC地址失败，请检查您的线路连接状态是否为已连接或者静态网关ip是否填写正确！", //C_LANG_INDEX_GET_GW_MAC_FAIL
	"设置日期过大，最大支持设置到2038年1月18日",//C_LANG_INDEX_TIMESET_TOO_LARGE
	""
};
