<% langTran("HomePage"); %>
<% langTran("Menu"); %>
<% langTran("IPMac"); %>
<% langTran("MakeMoney");%>
<%aspoutDynConfig();%>
<%outputChangemodel();%>
// JavaScript Document
var cloud = smartCloudShare;
var dhcp = 1;
var IP_MAC=1;
var wirelessset = 1;
var InterfaceFlow =1;
var userlist = 1;
var middlesoft = 1;
if(middlesoft == 1)
{
    var modelchange = '模式切换';
    var modelasp = 'modelchang.asp';
    var array1 = '计划任务';
    var array2 = 'Timeplan.asp';
    var array4 = taskplan;
    var array5 = 270;
    var changvar = 1;
}else{
    var modelchange = '微信绑定';
    var modelasp = 'bind.asp';
    var array1 = '<%write(learnConfig);%>';
    var array2 = 'learnConfig.asp';
    var array4 = uttKnowifi;
    var array5 = 200;
    var changvar = 0;

}
if(raisecom_a30w == 1)
{
    changevar = 0;
}else{
    changevar = 1;
}
/*名称  src  图片 是否显示  页面的宽度 页面的高度 */
var menus = new Array(
			new Array('<%write(CloudShare);%>','testnew.html','cloud.png',cloud,200,300,"cloud"),
			new Array('<%write(DHCP);%>','dhcpServer.asp','dhcp.png',dhcp,700,"auto","dhcp"),
			//new Array('<%write(DDNS);%>','DDNS_Sui.asp','dns.png',changevar,700,500,"dns"),
			new Array('<%write(DDNS);%>','DDNS_Sui.asp','dns.png',0,700,500,"dns"),
			new Array('<%write(realTimeFlow);%>','InterfaceFlow_Sui.asp','interfaceflow.png',changevar,700,500,"ifFlow"),
			new Array('<%write(SysInfo);%>','sysinfo.asp','system_info.png',1,700,"auto","sysInfo"),
			new Array('<%write(internetSet);%>','ConfWizard_Sui.asp?guide=0','internet_set.png',1,700,600,"internetSet"),
			new Array('<%write(WirelessSet);%>','WirelessSeting_Sui.asp','wireless_set.png',1,700,"auto","wlanSet"),
			new Array('AP无线设置','ApWirelessSeting_Sui.asp','wireless_set.png',ApwlanSet,700,"auto","ApwlanSet"),
			new Array('AP状态','ApState.asp','ApState.png',ApSta,830,"auto","ApSta"),
			new Array('<%write(LanSet);%>','lanConfig.asp','lan.png',1,700,"auto","lan"),
			new Array('<%write(staticRoute);%>','RouteSui.asp','static_router.png',changevar,800,"auto","staticRt"),
			new Array('<%write(UPnPStatus);%>','UPnP_Sui.asp','upnp.png',changevar,700,"auto","upnp"),
			new Array('<%write(BandSet);%>','Group_Sui.asp','broadband_set.png',1,700,"auto","bb_set"),
			new Array('无线中继','testnew.html','wds.png',smartWlanRepeater,200,300,"wds"),
			//new Array('<%write(ParentControl);%>','ParentsControlSui.asp','parentcontrol.png',changevar,800,"auto","parentCtrl"),
			new Array('<%write(ParentControl);%>','ParentsControlSui.asp','parentcontrol.png',0,800,"auto","parentCtrl"),
			new Array('<%write(macFilter);%>','MacFilterSui.asp','macfilter.png',1,700,"auto","macfilter"),
			new Array('<%write(virtualServer);%>','VirtualServer.asp','virtual_server.png',1,700,'auto',"virServer"),
			new Array('<%write(DMZHost);%>','DMZConfig.asp','dmz.png',changevar,600,"auto","dmz"),
			new Array('<%write(securitySet);%>','SecuritySetting.asp','security.png',smartSecutityConf,700,500,"security"),
			new Array('<%write(UserStatistics);%>','Userstatistics.asp','userslist.png',1,800,"auto","userList"),
			new Array('<%write(runningStatus);%>','runstatus.asp','runstatus.png',1,700,"auto","runStatus"),
			new Array('<%write(WlanTimming);%>','TimedWireless.asp','wireless_timing.png',1,700,"auto","wlanTiming"),
			new Array('<%write(SysClock);%>','SysClockManage.asp','clockm.png',1,700,"auto","clockm"),
			new Array('<%write(Remote);%>','RemoteManage.asp','remote_magage.png',changevar,700,"auto","rmMagage"),
			new Array('<%write(PasswdSet);%>','User_editSui.asp','password_set.png',1,600,"auto","sysAdminSet"),
			new Array('<%write(FirmwareUpgrade);%>','FirmwareUpgrade_Sui.asp','softwareup.png',1,650,"auto","softwareup",SOFT_UDTYPE),
			new Array('<%write(BackupRecovery);%>','BackupRecovery.asp','backup.png',1,700,400,"backup"),
			new Array('<%write(DiagnoseTest);%>','diagnose.asp','diagnostic_test.png',diagTest,680,410,"diagTest"),
			//new Array('<%write(AdFilter);%>','advideo_Sui.asp','ADfilter.png',changvar,680,"auto","ADfilter",ADFILTER_UDTYPE),
			new Array('<%write(AdFilter);%>','advideo_Sui.asp','ADfilter.png',0,680,"auto","ADfilter",ADFILTER_UDTYPE),
			//new Array('<%write(gameAccl);%>','SmartQos_Sui.asp','broadband.png',changvar,680,"auto","SmartQos"),
			new Array('<%write(gameAccl);%>','SmartQos_Sui.asp','broadband.png',0,680,"auto","SmartQos"),
			//new Array('<%write(easySurfInternet);%>','hotel_Sui.asp','hotel.png',changvar,680,"auto","hotel"),
			new Array('<%write(easySurfInternet);%>','hotel_Sui.asp','hotel.png',0,680,"auto","hotel"),
			//new Array('<%write(easyManage);%>','EasyManage_Sui.asp','easymanage.png',1,680,"auto","easyManage"),
			new Array('<%write(easyManage);%>','EasyManage_Sui.asp','easymanage.png',0,680,"auto","easyManage"),
			new Array('<%write(netShare);%>','NetShare_Sui.asp','netshare.png',NetShareManage,700,"auto","netshare"),
			new Array('<%write(ipm_ipmBand);%> ','IPMac_Sui.asp','IP_MAC.png',1,700,"auto","IP_MAC"),
			//new Array('我要赚钱','makeMoney.asp','makeMoney.png',1,700,"auto","makeMoney")
			new Array('<%write(IW_MMoney);%>','makeMoney.asp','makeMoney.png',IW_MMoney,700,"auto","makeMoney"),
            new Array(modelchange,modelasp,'link.png',1,700,"auto","link"),
            //new Array('elink','elinkc.asp','elinkc.png',elinkc_flag,700,"auto","elink"),
            new Array('elink','elinkc.asp','elinkc.png',0,700,"auto","elink"),
            new Array('VPN','VPNSui.asp','vpnProxy.png',vpnProxy,700,"auto","vpnProxy"),
			//new Array('<%write(AccessCtrl);%>','AccessControl_Sui.asp','accessCtrl_Sui.png',accessCtrl,600,"auto","accessCtrl"),
			new Array('<%write(AccessCtrl);%>','AccessControl_Sui.asp','accessCtrl_Sui.png',0,600,"auto","accessCtrl"),
//			new Array('<%write(learnConfig);%>','learnConfig.asp','learnConfig.png',uttKnowifi,600,200,"learnConfi"),
            new Array(array1,array2,'learnConfig.png',array4,600,array5,"learnConfig")
			
);

//弹出窗口
function open_windows(index,param,arrayMenu){
		if(art.dialog.list['func_box']){
			art.dialog.list['func_box'].close();
		}
		var autotype=0;
		if(index!=null){
			if(arrayMenu==null || arrayMenu =='undefined')
			{ 
				arrayMenu = getArray();
			}
			art.dialog.open(arrayMenu[index][1], {id:"func_box", title:arrayMenu[index][0] ,width:arrayMenu[index][4], height:arrayMenu[index][5], lock:true,drag: false,follow:document.getElementById("header")});
		}else{
			autotype =arrayMenu;
		}
		if(param=="runstatus"){
			art.dialog.open("runstatus.asp", {id:"func_box", title:"<%write(runningStatus);%>" ,width:700, height:"auto", lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="userStatistics"){
			art.dialog.open("Userstatistics.asp", {id:"func_box", title:"<%write(UserStatistics);%>" ,width:800, height:"auto", lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="guide_wifi"){
			art.dialog.open("WireWizard_Sui.asp", {id:"func_box", title:"<%write(wirelessSet);%>" ,width:700, height:400, lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="guide_wifi_5g"){
			art.dialog.open("WireWizard_Sui_5g.asp", {id:"func_box", title:"<%write(WirelessSet5G);%>" ,width:700, height:400, lock:true,drag: false,follow:document.getElementById("header")});
		}
        if(param=="guide"){
            if(ChangeModel == 0)
            {
                art.dialog.open("AutoCheck_Sui.asp", {id:"func_box", title:"<%write(internetSet);%>" ,width:600, height:400, lock:true,drag: false,follow:document.getElementById("header")});
            }else{
                art.dialog.open("ConfWizard_Sui.asp?autotype=2", {id:"func_box", title:"<%write(internetSet);%>" ,width:650, height:550, lock:true,drag: false,follow:document.getElementById("header")});
            }
        }
		if(param=="wifi"){
			art.dialog.open("WirelessSeting_Sui.asp", {id:"func_box", title:"<%write(WirelessSet);%>" ,width:700, height:"auto", lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="wifi_5g"){
			art.dialog.open("WirelessSeting5g_Sui.asp", {id:"func_box", title:"<%write(WirelessSet);%>" ,width:700, height:"auto", lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="internet"){
			art.dialog.open("ConfWizard_Sui.asp?autotype="+autotype, {id:"func_box", title:"<%write(InternetSet);%>" ,width:700, height:550, lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="internet_mac"){
			art.dialog.open("ConfWizard_Sui.asp?guide=0", {id:"func_box", title:"<%write(internetSet);%>" ,width:700, height:550, lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="device"){
			art.dialog.open("testnew.html", {id:"func_box", title:"已连接设备列表" ,width:400, height:600, lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="reload"){
			art.dialog.open("reload_Sui.asp", {id:"func_box", title:"<%write(restarting);%>" ,width:600, height:200, lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="EasyTask"){
			art.dialog.open("reload_EasyTask_Sui.asp", {id:"func_box", title:"<%write(restarting);%>" ,width:600, height:200, lock:true,drag: false,follow:document.getElementById("header")});
		}
		if(param=="wireless_timed"){
			art.dialog.open("TimedWireless.asp", {id:"func_box", title:"<%write(WlanTimming);%>" ,width:700, height:400, lock:true,drag: false,follow:document.getElementById("header")});}
	    if(param=="arpList"){
		art.dialog.open("IPMac_Sui.asp", {id:"func_box", title:"<%write(ipm_ipmBand);%>" ,width:700, height:"auto", lock:true,drag: false,follow:document.getElementById("header")});		
		}
	    if(param=="arpConf"){
		art.dialog.open("IPMacConfigSui.asp", {id:"func_box", title:"<%write(ipm_ipmBand);%>" ,width:700, height:546, lock:true,drag: false,follow:document.getElementById("header")});		
		}
	    if(param=="makeMoney"){
		art.dialog.open("makeMoney.asp", {id:"func_box", title:"畐ifi" ,width:700, height:400, lock:true,drag: false,follow:document.getElementById("header")});		
		}
	    if(param=="income"){
		art.dialog.open("income.asp", {id:"func_box", title:"赚钱wifi" ,width:700, height:400, lock:true,drag: false,follow:document.getElementById("header")});		
		}
		if(param=="sysAdminSet"){
			art.dialog.open("User_editSui.asp", {id:"func_box", title:"<%write(PasswdSet);%>" ,width:600, height:"auto", lock:true,drag: false,follow:document.getElementById("header")});
		}else{//永远在最后一个			
			art.dialog.open(param, {id:"func_box", title:"学习设置" ,width:1200, height:650, lock:true,drag: false,follow:document.getElementById("header")});
		}


}
/*
function swicth_act(action){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		//alert("status_now:"+status_now);
		if(status_now == "on"){
			//swicth_act("off");
			switch_b.removeClass("on");
			switch_b.addClass("off");
			//$("#ebabledwifi").attr("value","off");
		} else if (status_now == "off"){
			//swicth_act("on");
			switch_b.addClass("on");
			switch_b.removeClass("off");
		//	$("#ebabledwifi").attr("value","on");
		}
	}
function initEnabledButton(switch_b,status){
	//var switch_b = $(".on-off").children("button");
	if(status=="on"){
		switch_b.addClass("on");
		switch_b.removeClass("off");	
	}else{
		switch_b.removeClass("on");
		switch_b.addClass("off");	
	}
}
*/
