<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="x-ua-compatible" content="IE=9" />
<meta name="format-detection" content="telephone=no" />
<meta http-equiv="pragma" content="no-cache">
<meta http-equiv="Cache-Control" content="no-cache, must-revalidate">
<meta http-equiv="expires" content="Wed, 26 Feb 1997 08:21:57 GMT">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("HomePage"); %>
<% langTran("common"); %>
<% langTran("Link");%>
<title><% aspGetTitle(); %></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link href="css/default.css" rel="stylesheet" type="text/css" />
<link href="css/style_port.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/curvycorners.js" ></script>
<script type="text/javascript" src="script/jquery-1.8.1.min.js" ></script>
<script type="text/javascript" src="script/contextMenu.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script>
<script type="text/javascript" src="script/jquery.artDialog.js" ></script>
<script type="text/javascript" src="script/iframeTools.js" ></script>
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/menu.js" ></script>
<style type="text/css">
</style>
<script type="text/javascript">
if(window.parent != window.self) window.parent.location = window.self.location;

//var shortcutMenu = new Array("�ƹ���","DHCP������","��̬DNS","ʵʱ����","ϵͳ��Ϣ");//��ݲ˵�����ʾ����Ϣ
<%aspoutDynConfig();%>
<%aspOutputShortCutMenu();%>
<% outputSystemVersion();%>	//var revisions="nv3520Gv1.0.0-110908";
<% outputProductID();%>	//var ProductID="3520G";
<%isDefaultPassword();%> //isDefaultPasswd = 1; 
//var revisions="6530G-201303323.bin";
//var hardwareID="V2";
//��ÿ�ݲ˵�������ʾ����ϸ��Ϣ
function getArray(){
    var tmpArray = new Array();
    for (var i = 0; i < shortcutMenu.length; i++) {
	for (var j = 0; j < menus.length; j++) {
	    if (shortcutMenu[i] == menus[j][6]) {
		if (menus[j][3] == 1) {
		    tmpArray.push(menus[j]);	
		}
		break;
	    }
	}
    }
    return tmpArray;
}
//��̬���ɿ�ݲ˵���
function writenavMenu(){
    var navli = "";
    var tmpArray = new Array();
    tmpArray= getArray();
    for(var j=1;j<6;j++){
	//alert("tmpArray[j]:"+tmpArray[j]);
	if(j==5){
	    navli += '<li>';
	}else{
	    navli += '<li style="background: url(images_Sui/nav_pipe.png) no-repeat center bottom;">';
	}
	if(tmpArray[j-1] != undefined){
	    if (lang_en == 1)
	    {
		navli +='<a href="#" id="'+(j-1)+'" class="contextMenu" onclick="open_windows('+(j-1)+');" style="background:url(images_Sui/'+tmpArray[j-1][2]+');background-position: 19px 0px; background-repeat:no-repeat; line-height:13px;" ><span>'+tmpArray[j-1][0]+'</span></a></li>';				
	    }
	    else
	    {
		navli +='<a href="#" id="'+(j-1)+'" class="contextMenu" onclick="open_windows('+(j-1)+');" style="background:url(images_Sui/'+tmpArray[j-1][2]+');background-position: 19px 10px; background-repeat:no-repeat;" ><span>'+tmpArray[j-1][0]+'</span></a></li>';				
	    }
	}else{
	    navli +='<a class="custom" href="appprogram.asp" id="'+j+'" style="background:url(images_Sui/custom.png);background-position: 23px 19px; background-repeat:no-repeat;"><span></span></a></li>';	
	}
    }
    return navli;

}

//����·����
function reboot_window(){
    /*
    if (confirm("�Ƿ�ȷ������·�������������Ͽ���·���������ӣ�")) {
	$.ajax({
	    url:"/goform/formRebootMachine",	
	    async:false,
	    type:"post",
	    success:function(resp){
		if (resp == "EasyTask") {
		    open_windows(null,"reload",null);
		} else {
		    open_windows(null,"reload",null);
		}
	    }
	});
    }
    */
    art.dialog({
	title:false,
	content: '<%write(restartConfirm);%>',
	icon: 'warning',
	id: "wating_box",
	ok: function(){
	    $.ajax({
		url:"/goform/formRebootMachine",	
		async:false,
		type:"post",
		success:function(resp){
		    if (resp == "EasyTask") {
			open_windows(null,"reload",null);
		    } else {
			open_windows(null,"reload",null);
		    }
		}
	    });
	    //this.close();
	    return true;
	},
	okVal: "<%write(restartNow);%>", 
	cancel: function(){
	    this.close();
	    return false;
	},
	cancelVal: "<%write(cancel);%>"
    }).lock();
}

/*wifi�Ƿ�����*/
function getWifiInfo(){
    //ȡ���һ��
    var wifi_switch = "off";
    var wifi_encryption= "off";
    var wifi_5g_switch = "off";
    var wifi_5g_encryption= "off";
    $.getJSON("/goform/formWifiInfo",function(response){
	wifi_switch = response.wifi_switch;
	wifi_encryption = response.wifi_encryption;
	wifi_5g_switch = response.wifi_5g_switch;
	wifi_5g_encryption = response.wifi_5g_encryption;
	if(wifi_switch == "on"){	
	    if(wifi_encryption == 'NONE'){
		msgs = "<%write(wirelessUnsafeMsg);%>";
	    }
	}else{
	    msgs = "<%write(wirelessOffMsg);%>";
	}
	if(wifi_5g_switch == undefined){
	
	}else if(wifi_5g_switch == "on"){	
	    if(wifi_5g_encryption == 'NONE'){
		if (msgs != "")
		{
		    msgs +="\n";
		}
		msgs += "<%write(wireless5GUnsafeMsg);%>";
	    }
	}else{
	    if (msgs != "")
	    {
		msgs +="\n";
	    }
	    msgs += "<%write(wireless5GOffMsg);%>";
	}
	$("#wifi-alert").show().attr("title",msgs);
    });
}
//���wan�ڵ���Ϣ
function getWanInfo(){
    $.getJSON("/goform/formWanInfo",function(response){
	var isWanConn = 0;
	isWanConn = response.isWanConn;
	if(!isWanConn){	
	    msgs = msgs+"<%write(showMsg);%>";
	    $("#network-alert").show().attr("title",msgs);
	    $("#set_network").removeClass("ok");
	    $(".stream").hide();
	}else{
	    $("#network-alert").hide().attr("title","");
	    $("#set_network").addClass("ok");
	    $(".stream").show();
	}
    });
    
    //check_wan_info();
}
//���wan�ڵ���Ϣ
function check_wan_info(){
    $.getJSON("/goform/wanInfo",{},function(rsp) 
    {
	if(!rsp.is_eth_link){
	    msgs = "<%write(wanConnectInfo1);%>";
	    $("#network-alert").show().attr("title",msgs);
	    $("#set_network").removeClass("ok");
	    $(".stream").hide();
	}
    });
}

//�������״̬
function check_network_status(){
    $.ajax({
	url: "/goform/formNetworkStatus",
	cache: false,
	dataType: "json",
	success: function(rsp){
	    if(rsp){
		set_wan_status(rsp.isWanConn,rsp.connType,rsp.phylinkStatus);
		set_lan_status(1,rsp.isconn_lan1);
		set_lan_status(2,rsp.isconn_lan2);
		set_lan_status(3,rsp.isconn_lan3);
		set_lan_status(4,rsp.isconn_lan4);
		set_devices_cnt(rsp.devices_cnt);
		if (rsp.isCollided == 1) {
		    show_arp_collision_info(rsp.arp_collision_time,rsp.IP,rsp.MAC);
		} else {
		    $("#arp_collision-alert").hide().attr("title","");
		}
		var msgs ="";
		var flag = 0;
		//var wireless_time = "on";
		//var wireless_effect="0";//1����Ч0������Ч
		if(rsp.wifi_switch=="on"){	
		    if(rsp.wifi_encryption=="NONE"){
			msgs = "<%write(wirelessUnsafeMsg);%>";
			//$("#wifi-alert").show().attr("title",msgs);
		    } else {
			msgs ="";
			$("#wifi-alert").hide().attr("title","");
		    }
		    if(rsp.wireless_time=="on"){
			if(rsp.wireless_effect=="0"){
			    flag = 1;
			}
		    }
		}else{
		    msgs = "<%write(wirelessOffMsg);%>";
		}
		if(rsp.wifi_5g_switch == undefined){
		}else if(rsp.wifi_5g_switch=="on"){
		    if(rsp.wifi_5g_encryption=="NONE"){
			if (msgs != "")
			{
			    msgs +="\n";
			}
			msgs += "<%write(wireless5GUnsafeMsg);%>";
		    }
		    if(rsp.wireless_time=="on"){
			if(rsp.wireless_effect=="0"){
			    flag = 1;
			}
		    }
		}else{
		    if (msgs != "")
		    {
			msgs +="\n";
		    }
		    msgs += "<%write(wireless5GOffMsg);%>";
		}
		if (flag == 1)
		{
		    if (msgs != "")
		    {
			msgs +="\n";
		    }
		    msgs +="<%write(wlanTimingMsg);%>";
		}
		if(msgs == ""){
		    $("#wifi-alert").hide().attr("title",msgs);
		    $("#set_wifi").attr("title",msgs);
		}else{
		    $("#wifi-alert").show().attr("title",msgs);
		    $("#set_wifi").attr("title",msgs);
		}
	}
	$('body').stopTime('TimerA');
	    $('body').everyTime('3s','TimerA',function(){
		    check_network_status();
		    },0,true);
	},
	error :function(){
	    if (art.dialog.list['func_box'] || art.dialog.list['wating_box']){
		$('body').stopTime('TimerA');
		$('body').everyTime('1das','TimerA',function(){
		    check_network_status();
		},0,true);
	    }else{
		art.dialog({
		    id:'wating_box',
		    title:false,
		    content: '<%write(lanDisConnectInfo);%>',
		    icon: 'warning',
		    ok: function(){
			top.location.reload();
			return false;
		    },
		    okVal: "<%write(refreshPage);%> ", 
		    cancel: function(){
			this.close();
			return false;
		    },
			cancelVal: "<%write(cancel);%>"
		}).lock();
	   }
	}
    });
}
//������Ҫ���������Ŀ
function set_upgrade_count(){
	$.ajax({
	url: "/goform/formAppCheck",
	cache: false,
	dataType: "json",
	success: function(rsp){
		if(rsp){
		//rsp.countUpdate=2;
			if(rsp.countUpdate > 0){
				var str = '<span style="background:url(images_Sui/upgradeIcon1.png) no-repeat; width:24px;height:24px;display:inline-block;position:absolute; top:-12px;left:46px;color:#fff;text-align:center;font-size:12px;padding-top:2px;">'+rsp.countUpdate+'</span>';
				$("#updateCount").html(str);
			}
		}
		$('body').stopTime('TimerB');
	    $('body').everyTime('3s','TimerB',function(){
		   set_upgrade_count();
		    },0,true);
	},
	error:function(data){
	
	}
});
}
//����wan�ڵ�״̬
function set_wan_status(isconn,connType,phylinkStatus){
    var wan_now_status = $("#network-alert").attr("title")=="";
	var msg ="";
    if(wan_now_status==isconn){
	return;
    }
    if(isconn){
	$("#network-alert").hide().attr("title","");
	$("#set_network").attr("title","");
	$("#set_network").addClass("ok");
	$(".stream").show();
    }else{
	if (phylinkStatus == 0) {
		msg = "<%write(wanConnectInfo1);%>";
	   
	} else {
	    if (connType == 0) {
		msg = "<%write(wanConnectInfo2);%>";

	    } else if (connType == 1) {
		msg = "<%write(wanConnectInfo3);%>";
	
	    } else if (connType == 2) {
		msg ="<%write(wanConnectInfo4);%>";
		
	    } else if (connType ==3) {
		if (isconn == 2) {
		    msg ="<%write(wanConnectInfo5);%>";
		} else if (isconn == 6){
		    msg ="<%write(wanConnectInfo6);%>";
		} else {
		    msg ="<%write(wanConnectInfo7);%>";
		}
	    }
		
	}
	$("#network-alert").show().attr("title",msg);
	$("#set_network").attr("title",msg);
	$("#set_network").removeClass("ok");
	$(".stream").hide();
    }
}
//����lan�ڵ�״̬
var light_lan_timer = [0,0,0,0];
function set_lan_status(lan_index,isconn){
    if(isconn){
	if(light_lan_timer[lan_index] && light_lan_timer[lan_index]>0 ){
	    return;	
	}else{
	    //alert("isconn:"+isconn);
	    light_lan_timer[lan_index] = setInterval("light_flash('"+lan_index+"');",500);
	}
    }else{
	if(light_lan_timer[lan_index]>0){
	    clearInterval(light_lan_timer[lan_index]);
	    $("#lan"+lan_index).removeClass("lightning");
	    light_lan_timer[lan_index] = 0;
	}
    }
}
//����lan�����Ӻ�˿ڵı仯
function light_flash(index){
    if($("#lan"+index).hasClass("lightning")){
	$("#lan"+index).removeClass("lightning");
	$("#lan"+index).css({"background-image":"url(images_Sui/lightning.png)"});
    } else {
	$("#lan"+index).addClass("lightning");
	$("#lan"+index).css({"background-image":"url(images_Sui/disconnectlan.png)"});
    }
}
//��ʾarp��ͻ��Ϣ
function show_arp_collision_info(arp_collision_time,ip,mac)
{
    var msg;

    msg = "[" + arp_collision_time + "]";
    msg += "<%write(arpCollisionInfo);%>";
    msg += "IP " + ip;
    msg += " MAC " + mac;
    $("#arp_collision-alert").show().attr("title",msg);
}
//���������豸����Ŀ
function set_devices_cnt(devices_cnt){
    if (devices_cnt <= 1) {
	$("#userStatistics").html('<%write(Has);%><em id="device_counts"><img src="images_Sui/loading.gif" id="devices_loading" alt="<%write(ConnDeviceNum);%>" border="0" style="position:relative; top:3px;"/></em><%write(singleDevice);%>');
    } else {
	$("#userStatistics").html('<%write(Have);%><em id="device_counts"><img src="images_Sui/loading.gif" id="devices_loading" alt="<%write(ConnDeviceNum);%>" border="0" style="position:relative; top:3px;"/></em><%write(ConnectedDevice);%>');
    }
    if(devices_cnt >0 ){
	$("#devices em").html(devices_cnt);
	$("#devices_loading").hide();
    } else {
	$("#devices em").html(0);
    }
}
$(document).ready(function() {
    var str = writenavMenu();
	if(levelone_ctrl ==1 ){
		$("#LinkLogo").attr("href","http://global.level1.com/");
		$("#logo").attr("href","http://global.level1.com/");
		$("#LinkLogo").html("Official Website:http://global.level1.com/");
		$("#utt_tec").html("Copyright &copy Digital Data Communications GmbH, Germany. All Rights Reserved. ");
		$("#iphonenum").html("+49 231 9075 0");
	}
	if(galaxywind_ctrl ==1){//���ӷ���Ʒ�ƿ���
		$("#LinkLogo").attr("href","http://www.galaxywind.com/");
		$("#logo").attr("href","");
		$("#utt_tec").html("��Ȩ &copy ���ӷ��ƿƼ����޹�˾");
		$("#LinkLogo").html("");
		$("#iphonenum").html("");
		$("#hotline").html("");
		$("#logo").attr("target","");
	}
	
    $("#nav").append(str);	
    var revisions = "MSG WR201-W.00-M-v1.8.13-171228" 
    $("#software").html(revisions);
    $("#hardware").html(hardwareID);
    $(".contextMenu").hover(function(){
	    $(this).css({"background-color":"#4bc2ec"});
	    $(this).corner("10px");
	},function(){
	   $(this).css({"background-color":"#00a1e1"});
	}
    ); 
    $(".custom").hover(function(){
	    $(this).css({"background-color":"#4bc2ec"});
	    $(this).corner("10px");
	},function(){
	   $(this).css({"background-color":"#00a1e1"});
	       
	}
    ); 

    //�˳�
    $("#logout").click(function(){
	    /*
	var username = "admin";
	var password = "";
	$.ajax({ //һ��Ajax����
	    type: "post",  
	    url : "/", 
	    asnyc:false,
	    dataType:'html',
	    cache:false,
	    username:username,
	    password:password,
	    success: function(data, textStatus){
		window.location.href="login.asp";
	    }
	});
	*/
	window.location.href="logoutSuccess_Sui.asp";
	return false;
    });
    // ����״̬
    $("#runstatus").click(function(){
	open_windows(null,"runstatus");
    });
    //�û�ͳ�� 
    $("#userStatistics").click(function(){
	open_windows(null,"userStatistics");
    });
    //������	
    $("#system_guide").click(function(){
	open_windows(null,"guide");
    });
    //��������
    $("#setting_wireless").click(function(){
	open_windows(null,"wifi");
    });
    //��������
    $("#set_wifi").click(function(){
	open_windows(null,"wifi");
    });
    //��������
    $("#set_network").click(function(){
	open_windows(null,"internet_mac");
    });	
    //�豸��
    $("#device_counts").click(function(){
	open_windows(null,"device");
    });
    /* �������� */
    $("#reboot").click(function(){
	reboot_window();
    });
	/*���߶�ʱ����**/
	$("#Timed-alert").click(function(){
		open_windows(null,"wireless_timed");	
	})
    //getWifiInfo();
    //getWanInfo();
    //setTimeout("check_network_status()",1000);
});


function closeTimer()
{
    $('body').stopTime('TimerB');
}
$(function(){
    check_network_status();
	if(auto_update==1){
    set_upgrade_count();
	}
    /*
    $('body').everyTime('1s','TimerA',function(){
	check_network_status();
    },0,true);
    */
	if(galaxywind_ctrl ==1){//���ӷ���Ʒ�ƿ���
		$("#logo").css({"background":"url(../images_Sui/galaxywindlogo.png) no-repeat"});
	}else{
		$("#logo").css({"background":"url(../images_Sui/cmcc_logo.png) no-repeat"});
	//	$("#logo").css({"background":"url(../images_Sui/<%write(com_imglogo1);%>) no-repeat"});
	}
    if (lang_en == 1)
    {
	$("#side").css({"height":"525px"});
	$("#nav li").css({"padding-bottom":"15px", "margin-bottom":"15px"});
	$("#nav li a").css({"height":"30px", "padding-top":"52px"});
    }   
    if (isDefaultPasswd == 1)
    {
	art.dialog({
	    title:false,
	    content: '<%write(isDefaultAdminPassword);%>',
	    icon: 'warning',
	    id: "SysAdmin",
	    ok: function(){
		open_windows(null,"sysAdminSet");
		return true;
	    },
	    okVal: "<%write(setAdminPassword);%>", 
	    cancel: function(){
		this.close();
		return false;
	    },
		cancelVal: "<%write(cancel);%>"
	}).lock();
    }
    if (weChatManage == 0)
    {
	document.getElementById('weChatDimension').style.display="none";
    }
    else
    {
	document.getElementById('weChatDimension').style.display="";
    }
    var check=navigator;
    var b_version=navigator.appVersion;
    var version=b_version.split(";");
    var trim_Version=version[1].replace(/[ ]/g,"");
    if(check.appName=="Microsoft Internet Explorer"){
	if((trim_Version.indexOf('6',2)>=0)||(trim_Version.indexOf('7',2)>=0)||(trim_Version.indexOf('5.5',2)>=0))
	    document.getElementById('IEwarn').innerHTML='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="red"><%write(com_IEwarn);%></font>';
      }
});
</script>
</head>

<body id="home">
<div id="header">
    <div class="wrap">
	<p class="top_menu" >
    
	    <a href="home.asp" id="traffic_info"><%write(homePage);%></a>
	    <span class="pipe">|</span><a href="#" id="runstatus"><%write(runningStatus);%></a>
	    <span class="pipe">|</span><a href="#" id="system_guide"><%write(setupWizard);%></a>
	    <!--<span class="pipe">|</span><a href="#" id="setting_wireless">��������</a>-->
	    <span class="pipe">|</span><a href="appprogram.asp" id="app" style="position:relative;" ><%write(App);%><span id="updateCount"></span></a>
	    <span class="pipe">|</span><a href="javascript:void();" id="logout"><%write(Logout);%></a>
	</p>
	<h1><a target="_blank" href="<%write(LinkLogo);%>" id="logo"><%write(UTT);%></a></h1>
    </div>
</div>
<div id="content" >
	<div class="wrap">
    	<div id="side">
	    <ul id="nav">               
            </ul>
        </div>
        <div id="main" >
        	<div id="map" >
            
            	<div class="stream"></div>
				<div class="alert-icon">
					<i class="network-alert func_alert" id="network-alert" title="" style="display:none;"></i>
					<i class="wifi-alert func_alert" id="wifi-alert" title="" style="display:none;"></i>
					<i class="Timed-alert func_alert" id="Timed-alert" title="" ></i>
					<i class="arp_collision-alert func_alert" id="arp_collision-alert" title="" style="display:none;"></i>
                    
				</div>
             
				<a href="#" id="set_wifi" class="guide_s"><span><%write(InternetSet);%></span></a>
				<a href="#" id="set_network" class="ok guide_s"><span><%write(WirelessSet);%></span></a>
	
            </div>
	               <div id="lans_list">
			</div>
                <p id="devices">
                <a href="#" id = "userStatistics"><span></span><%write(Have);%><em id="device_counts"><img src="images_Sui/loading.gif" id="devices_loading" alt="<%write(ConnDeviceNum);%>" border="0" style="position:relative; top:3px;"/></em><%write(ConnectedDevice);%></a>
                </p>
		 <span  id="IEwarn"><div></div> </span>
                <a href="#" id="reboot"><span><%write(restart);%></span></a>
                <!-- <div id="weixin_img"></div> -->
               
            </div>
		</div>

    </div>
  
</div>

</div>

<div id="footer" style="position:relative;">
	<div class="wrap">
		<p class="system_info"><%write(SoftwareVersion);%> : <span class="pipe" id="software"></span><%write(HardwareVersion);%> : <span id="hardware"></span><span class="pipe"></span></p>
		<p class="bottom_menu">
<!--			<span id="utt_tec">Copyright &copy <%write(UTT_Tec);%></span> -->
<!--			<span class="pipe">|</span><a href="<%write(LinkLogo);%>" id="LinkLogo" target="_blank"><%write(OfficialWebsite);%>:<%write(LinkLogo);%></a> -->
			<!--<span class="pipe">|</span><a href="http://bbs.hiwifi.com/forum.php" target="_blank">��������</a>
			<span class="pipe">|</span><a href="javascript:void();" id="weixin" target="_blank">�ٷ�΢��</a>
			<span class="pipe">|</span><a href="http://www.hiwifi.com/products/" target="_blank" id="buy"></a>
			<span class="pipe">|</span><a href="javascript:switch_interface_type('mobile','/cgi-bin/turbo/admin_mobile');">�ƶ������</a>-->
		</p>
	</div>
<!--	 <div style="position:absolute;bottom:0px;right:20px;color:#fff" id="weChatDimension">
 	 <div id="dimension" style="background:#f2f2f2;width:116px;height:116px;"></div><div id="tip1"  style="display:none"><div>΢�Ź���·����</div><div>��ɨ����</div></div><div id="tip2"></div> -->
	 </div>
</div>

 <div id="bmenu" style="position:absolute;display:none;top:0px;left:0px;width:100px;margin:0px;padding:2px;border:1px solid #cccccc;background-color:#fff;">
  <ul>
  <li id="open"><%write(Open);%></li>
  <li id="del"><%write(Remov);%></li>
  </ul>
 </div>
</body>
<script type="text/javascript">
var srcEle = document.createElement('script'); 
srcEle.setAttribute('src', "http://api.greenwifi.com.cn/wx/qrcode?info="+info); 
document.getElementsByTagName('head')[0].appendChild(srcEle); 
function poll_api() { 
	$("#tip1").hide(); 
	$("#tip2").html("��ά���ȡ��..."); 
    if (typeof (qrcode_url) != "undefined") { 
//	if(payok) payOkSubmit(); 
	    showDimension(qrcode_url);
	return true; 
    } 
    setTimeout(poll_api, 100); 
    //$("#tip2").html("�޷���ȡ��ά��"); 
    return false; 
} 
if (weChatManage == 1) poll_api();
//showDimension("https://mp.weixin.qq.com/cgi-bin/showqrcode?ticket=gQFg8ToAAAAAAAAAASxodHRwOi8vd2VpeGluLnFxLmNvbS9xL0IwT1AtakxrV0M0aE1TbjNYVzEyAAIEGCndVQMEgFEBAA==");
function showDimension(url){
	var s= '<img src="'+url+'" style="width:116px;height:116px;" />';
	$("#dimension").html(s);
	$("#tip1").show(); 
	$("#tip2").hide(); 
}

$("img").one("error", function(e){
     $(this).attr("src", "images/errorimg.png");
    $("#tip2").show(); 
    $("#tip2").html("�޷���ȡ��ά��.."); 
    $("#tip1").hide(); 
});
//�Զ����Ҽ�������
var cmenu = new contextMenu(
    {
	menuID : "bmenu",
	targetEle : "contextMenu"
    },
    {
	bindings:{
	    'open' : function(o){//alert("ɾ�� "+o.id);
			
				open_windows(o.id);	
		    },
	    'del' : function(o){
		var tmpArrayMenu = new Array();
		var instName = getArray()[o.id][6];
		for(var i=0;i<shortcutMenu.length;i++){
		    if(shortcutMenu[i]!=instName){
			tmpArrayMenu.push(shortcutMenu[i]);
		    } 
		}
		shortcutMenu =tmpArrayMenu;
		var str = writenavMenu();
		$("#nav").empty();
		$("#nav").append(str);

		//alert(instName);
		$.ajax({ //һ��Ajax����
		    type: "post",  
		    url : "/goform/formShortCutMenuDel", 
		    asnyc:false,
		    dataType:'html',
		    data:{instName:instName},
		    success: function(data, textStatus){
			if (data != "") {
			    alert(data);
			}
		    }
		});

	    }
	}
    }
);
    cmenu.buildContextMenu();
	
</script>
</html>
