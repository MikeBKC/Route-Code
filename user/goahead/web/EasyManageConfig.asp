<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("top"); %>
<% langTran("common");%>
<% langTran("commonWords");%>
<% langTran("Link"); %>
<% langTran("EasyTask"); %>
<% langTran("ReBoot"); %>
<% langTran("bottom"); %>
<% langTran("WlanBase"); %>
<title><%write(com_utt);%></title>

<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<style type="text/css">
body{
	margin:0px;
	padding:0px;
	text-align:center;}
a:link,a:visited{ color:#000; }
a:hover{text-decoration:none; color:#000;}

.input-text{
	
	width:150px;}
.option-link{
	background:url(images/link1.png) no-repeat;
	background-position:left center; 
	font-size:12px; cursor:pointer;
	 
}
#t	{
	position:absolute;
	float:left;
	left:40%;
	right:40%;
	top:30%;
	
}

#a	{
	float:left;
}

.al	{
	opacity: 0.80;
	filter : progid:DXImageTransform.Microsoft.Alpha(style=0,opacity=80,finishOpacity=100);
}
.al2{
	opacity: 0.00;
	filter : progid:DXImageTransform.Microsoft.Alpha(style=0,opacity=0,finishOpacity=100);
}

U {
	DISPLAY: block;
	OVERFLOW: hidden;
	HEIGHT: 1px
}
U.f1 {
	background-color:#84c6ce;
	BACKGROUND: #84c6ce;
	MARGIN: 0px 3px
}
U.f2 {
	background-color:#84c6ce;
	BORDER-RIGHT: #84c6ce 2px solid;
	MARGIN: 0px 1px;
	BORDER-LEFT: #84c6ce 2px solid
}
U.f3 {
	background-color:#84c6ce;
	BORDER-RIGHT: #84c6ce 1px solid;
	MARGIN: 0px 1px;
	BORDER-LEFT: #84c6ce 1px solid
}

.d_top{
	clear:both;
	overflow:hidden;
	background-color:#84c6ce;
	height:29px;
	vertical-align:bottom;
}
.d_top a{
	float:right;
	margin-top:5px;
	margin-right:13px;
	padding-top:3px;
	width:18px;
	color:red;
	background-color:#789;
	text-decoration:none;
	font-weight:bold;
	text-align:center;
	vertical-align:middle;
}
.d_top span{
	float:left;
	font-size:13px;
	margin-left:15px;
	margin-top:8px;
	
}
.d_body {
	BORDER-RIGHT: #84c6ce 3px solid;
	BORDER-LEFT: #84c6ce 3px solid;
	padding:10px;
	height:120px;
	background-color:#fff;
	
}
.d_foot{
	clear:both;
	overflow:hidden;
	background-color:#84c6ce;
	height:2px;
}
  
.linkNetwork A:link { text-decoration: none; color: #ffffff;  font-weight:bold}
 .linkNetwork A:active { text-decoration: underline; color: #ffffff;font-weight:bold}
.linkNetwork A:visited { text-decoration: none; color: #ffffff; font-weight:bold}
.linkNetwork A:hover { text-decoration: underline; color: #ffffff; font-weight:bold}

</style>
<script type="text/javascript" ContentType="text/xml">
if (typeof(tomax)=='undefined')
	var tomax = 0;


var errorstr="";
var n_totalrecs =4; //wan口的数量
var totalrecs = 4; //lan口的数量
var connectionStat = "已连接";//三种情况：已连接、未连接、断开
var lanPort = new Array();//LAN口的数量
var lanStat = new Array();//LAN口的状态
var wanPort = new Array();//WAN口的数量
var wanStat = new Array();//WAN口的状态

lanStat[0] = "DOWN";
lanPort[0] = "lan1";
lanStat[1] = "DOWN";
lanPort[1] = "lan2";
lanStat[2] = "DOWN";
lanPort[2] = "lan3";
lanStat[3] = "DOWN";
lanPort[3] = "lan4";

wanPort[0] = "wan1";
wanStat[0] = "DOWN";
wanPort[1] = "wan2";
wanStat[1] = "DOWN";
wanPort[2] = "wan3";
wanStat[2] = "DOWN";
wanPort[3] = "wan4";
wanStat[3] = "DOWN";

/*显示接口*/
function initPortDisplay() {
	for (i = 0; i < totalrecs; i++) {
		moduleDisplay('port'+(i+1)+'Td',1);
		moduleDisplay('port'+(i+1),1);
		if (lanStat[i] == "DOWN") {
			document.getElementById('port'+(i+1)+'Td').innerHTML = '<img src="images/portdown.gif" />';
		} else {
			document.getElementById('port'+(i+1)+'Td').innerHTML = '<img src="images/portup.gif" />';
		}
		document.getElementById('port'+(i+1)).innerHTML = lanPort[i].toUpperCase();
		//document.getElementById('port'+(i+1)+'TdHtml').innerHTML = lanStat[i];
	}
	for (i = 0; i < n_totalrecs; i ++) {
		moduleDisplay("port"+(i+1+totalrecs)+"Td",1);
		moduleDisplay("port"+(i+1+totalrecs),1);
		if (wanStat[i] == "DOWN") {
			document.getElementById("port"+(i+1+totalrecs)+"Td").innerHTML = '<img src="images/portdown.gif" />';
		} else {
			document.getElementById("port"+(i+1+totalrecs)+"Td").innerHTML = '<img src="images/portup.gif" />';
		}
		document.getElementById("port"+(i+1+totalrecs)).innerHTML = wanPort[i].toUpperCase();
		//document.getElementById("port"+(i+1+totalrecs)+"TdHtml").innerHTML = wanStat[i];
	}
	for (i = n_totalrecs+totalrecs; i < 8; i++)
	{
	    moduleDisplay('port'+(i+1)+'Td',0);
	    moduleDisplay('port'+(i+1),0);
	} 
}

var connStatus=1;/*连接状态 0：外网线断开；1：已连接；2：连接中； 3：检测中； 4：检测中断；5:检测完成 6：用户名或密码错误；7：连接异常*/
var connType="DHCP"; /*接入方式 STATIC(固定IP接入)  DHCP(动态IP接入)  PPPOE（PPPOE接入）*/
var revisions=""; /*软件版本*/
var ProductID="";   /*产品型号*/
var connStatusSW="<%write(eas_connected);%>";
var stopCheckConSta = 0;
var stopChanConnType = 0;
var wirelessSSID="HIPER_AF7A03";//无线SSID
var isDefaultConf = 0;
var wireLessPwd="";//无线密码
var wireDisplay=0;
var ipAddress="0.0.0.0";//固定接入 IP地址
var mask="0.0.0.0"; //子网掩码
var gateWay="0.0.0.0";//网关
var priDns="0.0.0.0";//主DNS
var secDns="0.0.0.0";//备DNS
var pppoeUsername="";//用户名
var pppoePwd="";//密码
//var nodhcp=1;//是否显示提示，dhcp不显示
var oldconnStatus=0;//原来的连接方式
var WrlessEnables=0;

<% outputSystemVersion();%>
<% outputProductID();%>
<% aspoutEasyTaskConfig();%>

var ajaxObj=ajax();

if(ajaxObj)//初始化成功
{
    ajaxObj.url="/goform/formStatForEasyTask?date="+new Date().getTime();
    ajaxObj.refreshTime=2;
    ajaxObj.processHandle=function (str)
    {
	eval(str);
	initPortDisplay();
	checkConnectStatus(connStatus);
	if ((oldconnStatus != 1) && (connStatus == 1) && (connType != EasyConfig.connectionType.value))
	{
	    if (connType == "DHCP")
	    {
		if (confirm("<%write(eas_chatocurdhcp);%>"))
		{    
		    changeConnType(connType);
		}
	    }
	    else if (connType == "PPPOE")
	    {
		if (confirm("<%write(eas_chatocurpppoe);%>"))
		{    
		    changeConnType(connType);
		}
	    }
	    else
	    {
		if (confirm("<%write(eas_chatocurstatic);%>"))
		{    
		    changeConnType(connType);
		}
	    }
	}
	else if (stopChanConnType != 1)
	{
	    changeConnType(connType);
	}
	if (connStatus == 1)
	{
	    stopChanConnType = 1;
	}
	oldconnStatus = connStatus;
    }
}


/*初始化数据*/
function initDataDisplay(){
		var weekdaycount = 0;
		var weekendcount = 0;
		var formConf =EasyConfig ;
		formConf.staticIp.value=ipAddress;
		formConf.staticNetmask.value=mask;
		formConf.staticGateway.value=gateWay;
		formConf.staticPriDns.value=priDns;
		formConf.staticSecDns.value=secDns;
		formConf.pppoeUser.value=pppoeUsername;
		formConf.pppoePass.value=pppoePwd;
		formConf.ssid.value=wirelessSSID;
		if (getQueryString("wir") == "0")
		{
		    var value = 0;
			moduleDisplay("safetonetimg",1);
			$("passReboot").style.cssText="padding-bottom:36px";
		    for (var i = 0; i < days.length; i++) {
			if (days.charAt(i) == "0")
			{
			    if (i==0){
				moduleDisplay('monday',0);
				weekdaycount++;
			    }else if(i==1){
				moduleDisplay('tuesday',0);
				weekdaycount++;
			    }else if(i==2){
				moduleDisplay('wednesday',0);
				weekdaycount++;
			    }else if(i==3){
				moduleDisplay('thursday',0);
				weekdaycount++;
			    }else if(i==4){
				moduleDisplay('friday',0);
				weekdaycount++;
			    }else if(i==5){
				moduleDisplay('saturday',0);
				weekendcount++;
			    }else if(i==6){
				moduleDisplay('sunday',0);
				weekendcount++;
			    }
			}
		    }    
		    if (weekdaycount == 5)
		    {
			moduleDisplay('weekdaytime',0);
		    }
		    if (weekendcount == 2)
		    {
			moduleDisplay('weekendtime',0);
		    }
		    if ((weekdaycount == 5) && (weekendcount == 2))
		    {
			moduleDisplay('wlantime',0);
		    }
		}
		else
		{
		    var value = 1;
			moduleDisplay("safetonettr",1);
		    formConf.safeToNet.checked = (safeToNet == 1 ? true:false);
		    formConf.day.value="1111111";
		    formConf.weekdaytimestart.value="06:00";
		    formConf.weekdaytimestop.value="23:59";
		    formConf.weekendtimestart.value="06:00";
		    formConf.weekendtimestop.value="23:59";
		}
		formConf.wir.value = getQueryString("wir");
		formConf.wireDisplay.value = value && (wireDisplay);
		moduleDisplay($("wireless"), value && (wireDisplay));
		formConf.wirelessPwd.value=wireLessPwd;
		$("modeltd").innerHTML= ProductID;
		$("versionId").innerHTML = revisions;			
		var connTypeSelect = $("connectionType");
		for(var i=0;i<connTypeSelect.options.length;i++)
		{
		    if(connTypeSelect.options[i].value==connType)
		    {
			    connTypeSelect.options[i].selected=true;
				changeConnType(connType);
				break;
		    }
		}
		formConf.allowEasyManage.checked = (easyTaskEn == "on" ? true:false);
		
	}
/*根据连接状态，显示相应的提示信息*/
function checkConnectStatus(status){
	$("status").value=status;
	switch(status){
	case 0:
		connStatusSW ="<%write(eas_disconnect);%>";
		$("connectStatus").style.color="#FF0000";
		break;
	case 1:
		connStatusSW ="<%write(eas_connected);%>";
		$("connectStatus").style.color="#00FF00";
		break;
	case 2:
		connStatusSW ="<%write(eas_connecting);%>";
		$("connectStatus").style.color="#00FF00";
		break;
	case 3:
		connStatusSW ="<%write(eas_checking);%>";
		$("connectStatus").style.color="#00FF00";
		
		break;
	case 4:
		connStatusSW ="<%write(eas_checkinter);%>";
		$("connectStatus").style.color="#FF0000";
		break;
	case 5:
		connStatusSW ="<%write(eas_checkcom);%>";
		$("connectStatus").style.color="#FF0000";
		break;
	case 6:
		connStatusSW ="<%write(eas_userpasserror);%>";
		$("connectStatus").style.color="#FF0000";
		break;
	case 7:
		connStatusSW ="<%write(eas_connecterror);%>";
		$("connectStatus").style.color="#FF0000";
		break;
	}
	$("connectStatus").innerHTML = connStatusSW;
}
//状态为检测中，触发任何文本框，检测中断，connStatus=4,通过ajax获得状态
function ajaxSubmit(url){
	var ajaxSub=ajax();
	if(ajaxSub)//初始化成功
	{
	    ajaxSub.url=url;
	    var fun=ajaxSub.post;
	    fun();
	}
}
/*状态为3（检测中），触发任何操作，连接状态为4（检测中断），使用ajax提交*/
function changeStatus()
{
    stopChanConnType = 1;
    if(connStatus==3)
    {
	ajaxSubmit("/goform/formStopWanCheck");
	connStatus=4;
	checkConnectStatus(connStatus);
	return false;
    }
    return true;
}
/*点击进入高级管理页面的*/
function gotoAdvanced(){
    if(changeStatus()){
	if(tomax == 1){
	    EasyConfig.action="/frame.asp";
	}else{
	    EasyConfig.action="/framePage.asp";
	}
	EasyConfig.submit();
    }		
}


/*改变接入方式，显示的文本框也改变*/
function changeConnType(val){
	if(val=="STATIC"){
		EasyConfig.connectionType.value="STATIC";
		moduleDisplay('britbl',1);
		moduleDisplay('pppoetbl',0);
		//document.getElementById("g3tbl").style.display="none";
 
		 // var x2 = document.getElementById("Access");
		 // x2.style.display = "block";
	}
	else if(val=="PPPOE"){
		EasyConfig.connectionType.value="PPPOE";
		moduleDisplay('britbl',0);
		moduleDisplay('pppoetbl',1);
		//document.getElementById("g3tbl").style.display="none";
	}
	else{
		EasyConfig.connectionType.value="DHCP";
		moduleDisplay('britbl',0);
		moduleDisplay('pppoetbl',0);
		//document.getElementById("g3tbl").style.display="block";
	}
 
}
/*通过id获得对象的方法*/
function $(id){return document.getElementById(id);}
/*显示修改密码和重启设备对话框框*/
function show(id,val){
		if(changeStatus()){
			var t = $(id);
			if(val=="modify"){
					moduleDisplay('modifyPWD',1);
					moduleDisplay('reset',0);
					$("tiptitle").innerHTML="<% write(eas_changePass); %>";
				}else{
					moduleDisplay('modifyPWD',0);
					moduleDisplay('reset',1);
					$("tiptitle").innerHTML="<% write(restartDevice); %>";
				}
			t.style.display="block";
			$(id).style.display="";
		}
	}
/*弹出对话框关闭的方法*/	
function cl(id){
		$(id).style.display="none";
		
		 FormModifyPWD.passwd1.value = "";
		FormModifyPWD.passwd2.value ="";
		
}

	
/*移动对话框*/
function moveEvent(e,id){
	var isIE = (document.all)?true:false;
	//navigator.userAgent.toLowerCase().indexOf("msie") != -1;
	//var event=window.event||event;
	drag = true;
	xx=isIE?event.x:e.pageX;
	yy=isIE?event.y:e.pageY;
	L = document.getElementById(id).offsetLeft;
	T = document.getElementById(id).offsetTop;

	document.onmousemove = function(e) {
		if (drag) {
			x=isIE?event.x:e.pageX;		if(x<0)x=0;
			y=isIE?event.y:e.pageY;		if(y<0)y=0;
			document.getElementById(id).style.left = L-xx+x;
			document.getElementById(id).style.top  = T-yy+y;
		}
	}
	document.onmouseup=function(){
		drag = false;
	}
}

/*IE6支持png格式图片*/
function fixPNG(myImage) { 
var arVersion = navigator.appVersion.split("MSIE"); 
var version = parseFloat(arVersion[1]); 
if ((version >= 5.5) && (version < 7) && (document.body.filters)) 
	{
		 var imgID = (myImage.id) ? "id='" + myImage.id + "' " : ""; 
		 var imgClass = (myImage.className) ? "class='" + myImage.className + "' " : ""; 
		 var imgTitle = (myImage.title) ? "title='" + myImage.title   + "' " : "title='" + myImage.alt + "' "; 
		 var imgStyle = "display:inline-block;" + myImage.style.cssText; 
		 var strNewHTML = "<span " + imgID + imgClass + imgTitle 
	 
	   + " style=\"" + "width:" + myImage.width 
	 
	   + "px; height:" + myImage.height 
	 
	   + "px;" + imgStyle + ";" 
	 
	   + "filter:progid:DXImageTransform.Microsoft.AlphaImageLoader" 
	 
	   + "(src=\'" + myImage.src + "\', sizingMethod='scale');\"></span>"; 
		 myImage.outerHTML=strNewHTML;
	} 
} 
function checkUserPwd(){
		var f=FormModifyPWD;
		var pw1 =f.passwd1.value;
		var pw2 =f.passwd2.value;
		if(pw1=="" || pw2==""){
			$("error").innerHTML = "<% write(eas_passEmpty); %>";	
			return false;
		} else if ((pw1.indexOf(",") != -1 || pw1.indexOf(";") != -1 || pw1.indexOf("；") != -1 || pw1.indexOf("%") != -1 || pw1.indexOf("\"") != -1 || pw1.indexOf("\'") != -1 || pw1.indexOf("\\") != -1 || 　pw1.indexOf(" ") != -1) || (pw2.indexOf(",") != -1 || pw2.indexOf(";") != -1 || pw2.indexOf("；") != -1 || pw2.indexOf("%") != -1 || pw2.indexOf("\"") != -1 || pw2.indexOf("\'") != -1 || pw2.indexOf("\\") != -1 || 　pw2.indexOf(" ") != -1)) {
        $("error").innerHTML="<% write(eas_specialChar); %>";
			return fasle;
   		}else if(pw1!=pw2){
			$("error").innerHTML = "<% write(eas_passDiff); %>";
			return false;		
		}
	
		return true;

	}
function saveUserPwd(){
	if(checkUserPwd()){
		FormModifyPWD.action="goform/formUserEasyTask";
		FormModifyPWD.submit();
		cl("t");
	}	
}

function checkData(){
	var f=EasyConfig;
	var indextext=getListValue(f.connectionType);
	if(indextext=="STATIC"){
		if (f.staticIp.value=="0.0.0.0"){ alert("<%write(com_checkIPAlert);%>");return false};
		if (!checkIP(f.staticIp,"<%write(com_IP);%>",0)) return false;
		if (!checkMask(f.staticNetmask,"<%write(com_NetMask);%>",0)) return false;
		if (!checkIP(f.staticGateway,"<%write(com_GateWayAddr);%>",0)) return false;
		if (!checkSameSubnet(f.staticIp.value,f.staticGateway.value,f.staticNetmask.value))
		{
			alert("<%write(com_subnetAlert);%>");
			return false;
		}
		if(platform != "nv1800N")
		{
			if (!checkMainDns(f.staticPriDns,"<%write(com_MDNS);%>",0)) return false;
			if (!checkIP(f.staticSecDns,"<%write(com_SDNS);%>",1)) return false;
		}
	}
	else if(indextext=="PPPOE"){
		if(!checkEnglishName_allowZero(f.pppoeUser,31,"<%write(com_user);%>",0)) return false;
		if(!checkPassword1(f.pppoePass, 31, 0, 0)) return false;	//检查密码：不能超过11个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	}
	 
	if ((f.wirelessPwd.value.length < 8) && (f.wirelessPwd.value.length > 0))
	{
	    alert("<%write(eas_wirePwdLen);%>");
	    return false;
	}
/*
	if (nodhcp)
	{
	    alert("<%write(eas_nodhcp);%>");
	}*/
	if(!checkEnglishName(f.ssid,32,"SSID",0)) return false;
	return true;
}

function save()
{
	if (checkData())
	{
		EasyConfig.action="goform/formEasyTaskConfig";
		EasyConfig.submit();
	}
    
}
function rebootEasyTask(){
	cl("t");
	EasyConfig.action="goform/formRebootMachine";
	EasyConfig.submit();
}

function reCheck()
{
	if (connStatus == 1)
	{
	    alert("<%write(eas_nocheck);%>");
	}
	else
	{
	    stopChanConnType = 0;
	    EasyConfig.action="goform/formEasyTaskReCheck";
	    EasyConfig.submit();
	}
}
function init(){
	fixPNG($("logoImg"));
	if(ajaxObj) 
	{
	    ajaxObj.start();
	}
	checkConnectStatus( connStatus);
	initDataDisplay();
	
}
</script>
</head>
<body onLoad="init();"onbeforeunload="" >
<table border="0" cellspacing="0" cellpadding="0" height="100%" align="center" width="90%;">
<tr><td width="100%" align="center" background="images/body_main.png">
<table cellpadding="0" cellspacing="0" width="100%" height="100%" border="0">
	<tr valign="top">
	<td>
	<table id="header" border="0" cellpadding="0" cellspacing="0" width="100%" height="10%">
    	<tr>
        	<td align="left" valign="top"><img src="images/top_l.gif" /></td>
            <td width="100%" background="images/topbg.gif" style="background-repeat:repeat-x;">
            	<table cellpadding="0" cellspacing="0" border="0" >
                	<tr>
                	<td width="90%" rowspan="2"><a href="http://www.com.cn" target="_blank"><img id="logoImg" src="images/logo.png" width="206" height="45" border="0"/></a></td>			<td width="10%" align="center">
                            <table cellpadding="0" cellspacing="0"><tr>
    
                                <td><%write(Model);%></td><td id="modeltd"></td>
                              </tr>
                              <tr id="hwmodeltr" style="display:none">
                                <td height="20" ></td><td id="hwmodeltd"></td>
                              </tr>
                              <tr>
                                <td height="20" valign="top" ><%write(SoftwareVer);%></td><td valign="top" id="versionId"></td>
                              </tr>
                        </table>
                    </td>
                	</tr>
                    <tr>
                	<td colspan="2" align="right">
                    	<table width="250" border="0" cellspacing="0" cellpadding="0" align="right"   style="padding-top:5px;" class="linkNetwork">
                          <tr bgcolor="#008893" >
                            <td width="60" height="22" align="center" valign="middle"><a title="<% write(TopUttarea); %>" target="_blank" href="<% write(LinkProduct); %>"  id="taolun"><font color=""><% write(ProductDiscu); %></font></a></td>
                            <td width="3"><img src="<% write(com_imgline); %>"  width="1" height="16"></td>
                            <td width="60" height="22" align="center" valign="middle"><a href="<% write(LinkKnowLedgeTopFo); %>"  target="_blank"  title="<% write(TopKnowledgeBase); %>" id="zhishiku"><font color=""><% write(TopKnowledge); %></font></a></td>
                            <td width="3"><img src="<% write(com_imgline); %>" width="1" height="16"></td>
                            <td width="60" align="center" valign="middle" height="22"><a href="<% write(LinkReservationTopFe); %>"  target="_blank" title="<% write(ReservationMsg); %>" id="yuyue"><font color=""><% write(Reservation); %></font></a></td>
                          </tr>
                        </table>
                    </td>
                	</tr>
                </table>
            </td>
            <td align="left" valign="top"><img src="<% write(com_imgtopr); %>" /></td>
        </tr>
    </table>
    </td></tr>
    <tr><td>
    <table id="content" border="0" cellpadding="0" cellspacing="0" width="100%" height="85%">
    	<tr><td height="20">&nbsp;</td></tr>
    	<tr valign="top">
        	
			<td width="100%"><script language="javascript">//showSecondTitle("易管理配置");</script>
            
            	<table  border="0" height="100%" width="100%" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
             	<script language="javascript">showerror(errorstr,80);</script>
                
                <tr>
                	<td>
                    	
                        <form method="post" action="" name="EasyConfig">
                        	<table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">
                            	<tr><td height="50" colspan="2"></td></tr>
                            	<tr>
                                	<td>&nbsp;</td>
                                	<td align="center" class="style7">
                                    <table  style="display" width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
                                   	
                                    <tr><td width="25%"></td><td width="50%" height="20" align="center" style="padding-right:20px">
                                    		<table style="border:solid 1px #b8daa8" border="0">
			<p>&nbsp;</p>
															<tr>
																<td id="port1" width="70" height="30" align="center" style="display:none"></td>
																<td id="port2" width="70" align="center" style="display:none"></td>
																<td id="port3" width="70" align="center" style="display:none"></td>
																<td id="port4" width="70" align="center" style="display:none"></td>
																<td id="port5" width="70" align="center" style="display:none"></td>
																<td id="port6" width="70" align="center" style="display:none"></td>
																<td id="port7" width="70" align="center" style="display:none"></td>
																<td id="port8" width="70" align="center" style="display:none"></td>
<!-- <td id="port9" width="70" align="center"></td>
 -->
															</tr>
															<tr>
																<td id="port1Td" height="30" align="center" style="display:none"></td>
																<td id="port2Td" align="center" style="display:none"></td>
																<td id="port3Td" align="center" style="display:none"></td>
																<td id="port4Td" align="center" style="display:none"></td>
																<td id="port5Td" align="center" style="display:none"></td>
																<td id="port6Td" align="center" style="display:none"></td>
																<td id="port7Td" align="center" style="display:none"></td>
																<td id="port8Td" align="center" style="display:none"></td>
<!-- <td id="port9Td" align="center"></td>
 -->
															</tr>
															<tr>
																<td  colspan="9"align="center" height="10">&nbsp;</td>
															
															</tr>
										</table><!-- 400-->
                                        
                                        
                                    </td>
                                    	<td><table border="0" cellpadding="0" cellspacing="0" width="180">
    <p>&nbsp;</p>
                                       	  <tr><td width="32"><img src="images/portup.gif" /></td>
                                        <td width="148" valign="bottom"><% write(com_Connected); %></td></tr>
                                            <tr><td colspan="2" height="7"></td></tr>
                                            <tr><td><img src="images/portdown.gif" /></td>
                                          <td  valign="bottom"><% write(com_Disconnected);%></td></tr>
                                        </table></td>
                                    </tr>
                                    <!-- 中间部分 -->
                                        <tr><td></td><td align="right">
                                            <table cellpadding="0" cellspacing="0" border="0">
                                            <tr>
                                            
                                                <td align="center">
                                                    <table cellpadding="0" border="0" cellspacing="0" >
                                                        <tr>
                                                        	<td align="center" colspan="4" >
                                                            	<table cellpadding="0" cellspacing="0" border="0"  width="600"  style="margin-top:10"><!--<tr>
                                                         			 <td align="right" width="41%" height="22"><img src="images/portup1.png"/></td>
                                                          			 <td width="2%">&nbsp;</td>
                                                          			<td>表示线路已连接</td>
                                                          			<td width="27%">&nbsp;</td>
                                                                    </tr> -->
								    <input type="hidden" value="EasyTask" name="page">
                                                                    <tr><td colspan="4" height="5"></td></tr>
                                                                     <tr>
                                                                      <td align="right" width="41%" height="22"><% write(com_ConnectionStatus);%></td>
                                                                      <td width="2%">&nbsp;</td>
                                                                      <td id="connectStatus"></td><input type="hidden" name="status" value="0" id="status"/>
                                                                      <td width="27%">&nbsp;</td>
                                                                    </tr>
                                                                    <tr id="Access">
                                                                          <td align="right" width="41%" height="22"><% write(com_accessType);%></td>
                                                                          <td width="2%">&nbsp;</td>
                                                                          <td>
                                                                       
                                                                          <select name="connectionType" id="connectionType" onFocus="changeStatus();"  class="list"  onChange="changeConnType(this.value);">
                                                                              <option value="DHCP" id="dhcp"><% write(com_DynamicIP);%></option>
                                                                              <option value="STATIC" id="static"><% write(com_StaticIP);%></option>
                                                                              <option value="PPPOE" id="pppoe"><% write(com_PPPoE);%></option>
                                                                          </select>
                                                                            <input type="button" class="input-button" name="detect" id="detect" onClick="reCheck();" value="<% write(eas_check);%>" />
                                                                          </td>
                                                                          <td width="27%">&nbsp;</td>
                                                                      </tr>
                                                                 </table></td></tr>
                                                          
                                                          <tr id="britbl" style=" display:none;padding:0; margin:0"><td align="center" colspan="4"><table cellpadding="0" cellspacing="0" border="0" width="610"><tr>
                                                                    <td height="22" width="41%" id="ipaddr" align="right"valign="top"><% write(com_IP);%></td>
                                                                    <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
                                                                    <td><input type="text" onFocus="changeStatus();" class="input" name="staticIp" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(com_IP);%>'),yutd1)"> </td>
                                                                    <td  id="yutd1" width="27%"></td>
                                                                  </tr>
                                                                  <tr>
                                                                    <td align="right" height="22" id="subnet"><% write(com_NetMask);%></td>
                                                                    <td ><img src="images/qt.gif" width="7" height="9"></td>
                                                                        <td><input type="text" class="input" onFocus="changeStatus();" name="staticNetmask" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<% write(com_NetMask);%>'),yutd2)"></td>
                                                                    <td  id="yutd2"></td>
                                                                  </tr>
                                                                  <tr>
                                                                    <td align="right"  height="22" id="gateway"><% write(com_GateWayAddr);%></td>
                                                                    <td ><img src="images/qt.gif" width="7" height="9"></td>
                                                                    <td><input type="text" class="input" onFocus="changeStatus();" name="staticGateway" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(com_GateWayAddr);%>'),yutd3)" ></td>
                                                                    <td  id="yutd3"></td>
                                                                  </tr>
                                                                  <tr id="zhuDNS" style="display:">
                                                                    <td align="right"  height="22" ><% write(com_MDNS);%></td>
                                                                    <td ><img src="images/qt.gif" width="7" height="9"></td>
                                                                    <td><input type="text" onFocus="changeStatus();" class="input" name="staticPriDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMainDns(this,'<% write(com_MDNS);%>'),yutd4)" ></td>
                                                                    <td  id="yutd4"></td>
                                                                  </tr>
                                                                  <tr id="beiDNS" style="display:">
                                                                    <td align="right"  height="22"><% write(com_SDNS);%></td>
                                                                    <td >&nbsp;</td>
                                                                    <td><input type="text" onFocus="changeStatus();" class="input" name="staticSecDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<% write(com_SDNS);%>'),yutd9)"  ></td>
                                                                    <td  id="yutd9"></td>
                                                                  </tr></table></td></tr>
                                                          		   <tr id="pppoetbl" style="display:none;"><td align="center" colspan="4"><table cellpadding="0" cellspacing="0" width="610"><tr>
                                                                        <td align="right"  height="22" width="41%" id="name"><% write(com_user);%></td>
                                                                        <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
                                                                        <td ><input type="text" class="input" onFocus="changeStatus();" name="pppoeUser" size="18" maxlength="31"></td>
                                                                        <td  id="yutd5" width="27%"></td>
                                                                      </tr>
                                                                      <tr>
                                                                        <td align="right" height="22" id="password"><% write(com_passwd);%></td>
                                                                        <td ><img src="images/qt.gif" width="7" height="9"></td>
                                                                        <td><input type="password" onFocus="changeStatus();" class="input" name="pppoePass" size="18"  maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31,0),yutd6)" ></td>
                                                                        <td  id="yutd6"></td>
                                                                      </tr></table></td></tr>
									<input type="hidden" value="0" name="wir">
									<input type="hidden" value="0" name="wireDisplay">
                                                                       <tr name="wireless" id="wireless" style="display:none"> <td align="center" colspan="4"><table cellpadding="0" cellspacing="0" width="610"><tr>
                                                                            <td align="right"  height="22" width="41%" id="wirelessSSID"><% write(eas_wireName);%></td>
                                                                            <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
                                                                            <td><input name="ssid" onFocus="changeStatus();" class="input" type="text" size="18" maxlength="32"  onChange="showfiledtip(re_checkEnglishName(this,'SSID'),ssidtd)"></td>
                                                                            <td  id="ssidtd" width="27%"></td>
                                                                          </tr>
                                                                          <tr> 
                                                                            <td align="right" height="22" width="41%" id="wirelessPwd"><%write(eas_setWlPass);%></td>
                                                                            <td width="2%"><!--<img src="images/qt.gif" width="7" height="9">--></td>
                                                                            <td><input type="password" onFocus="changeStatus();" class="input" name="wirelessPwd" size="18" onChange="showfiledtip(re_checkPassword1(this, 63,0),yutd6)" ></td>
                                                                            <td width="27%" id="yutd6"></td>
                                                                          </tr>
                                                                          <tr> 
                                                                            <td align="center" colspan="4" ><%write(eas_wlInfo);%></td>
                                                                           
                                                                          </tr>
                                                                          </table></td></tr>
                                                                             
                                                                              
                                                                          <tr><td align="center" colspan="4">
                                                                          <table width="610" border="0" cellpadding="0" cellspacing="0">
								    <tr>
                                                                      <td align="right" width="41%" height="22"><input type="checkbox" name="allowEasyManage" /></td>
                                                                      <td width="2%">&nbsp;</td>
                                                                      <td><% write(eas_easyTaskEn); %></td>
                                                                      <td width="27%">&nbsp;</td>
                                                                    </tr> 
								    <tr name="safetonettr" id="safetonettr" style="display:none">
                                                                      <td align="right" width="41%" height="22"><input type="checkbox" name="safeToNet"/></td>
                                                                      <td width="2%">&nbsp;</td>
                                                                      <td colspan="2"><% write(eas_safetonet); %><font color=#999999><% write(eas_safetonettip); %></font></td>
	</tr>
                                                                      <tr>
                                                                      <td align="right" width="35%" height="22"></td>
                                                                      <td colspan="2"><input type="button" class="input-button"name="saveConfig" onClick="save();" value="<% write(save); %>"/>&nbsp;&nbsp;<a style="cursor:pointer; color:#03F;" onClick="gotoAdvanced();"><% write(eas_gotoAdvan); %></a>
                                                                       </td>
                                                                     
                                                                      <td width="27%">&nbsp;</td>
                                                                    </tr> 
                                                                   <tr><td height="10" colspan="4">&nbsp;</td></tr>   
                                                                   							    <tr name="safetonetimg" id="safetonetimg" style="display:none">
                                                                      <td colspan="4" align="center" height="22">
	<font name="wlantime" id="wlantime" style="display:" color=#999999><%write(wlantimetip);%></font>
	<font name="monday" id="monday" style="display:" color=#999999><%write(com_monday);%>&nbsp;</font>
	<font name="tuesday" id="tuesday" style="display:" color=#999999><%write(com_tuesday);%>&nbsp;</font>
	<font name="wednesday" id="wednesday" style="display:" color=#999999><%write(com_wednesday);%>&nbsp;</font>
	<font name="thursday" id="thursday" style="display:" color=#999999><%write(com_thursday);%>&nbsp;</font>
	<font name="friday" id="friday" style="display:" color=#999999><%write(com_friday);%>&nbsp;</font>
	<font name="weekdaytime" id="weekdaytime" style="display:" color=#999999><script type="text/javascript">document.write(weekdaytimestart+'-'+weekdaytimestop);</script></font>&nbsp;&nbsp;
	<font name="saturday" id="saturday" style="display:" color=#999999><%write(com_saturday);%>&nbsp;</font>
	<font name="sunday" id="sunday" style="display:" color=#999999><%write(com_sunday);%>&nbsp;</font>&nbsp;
	<font name="weekendtime" id="weekendtime" style="display:" color=#999999><script type="text/javascript">document.write(weekendtimestart+'-'+weekendtimestop);</script></font>
	<input type="hidden" name="day"><input type="hidden" name="weekdaytimestart"><input type="hidden" name="weekdaytimestop"><input type="hidden" name="weekendtimestart"><input type="hidden" name="weekendtimestop">

	</td>
	</tr>
                                                                 </table>
                                                                     </td></tr>
                                            		</table>
                                        		</td>
                                        	</tr>
                                    
                                        <!-- 结束部分-->
                                        <tr><td height="30"></td></tr>
                                    </table><!-- 398-->
                                    		</td>
						<td valign="bottom" id="passReboot"  style="padding-bottom:50px"><div id="info" style="float:left;">
                                         <div class="option-link" style=" padding-top:10px; padding-left:20px; padding-bottom:10px; padding-right:20px;display: inline "><a onClick="show('t','modify');"><% write(eas_changePass); %></a></div>
                                         <div class="option-link" style=" padding-top:0px; padding-left:20px; padding-bottom:0px; padding-right:20px;display: inline "><a onClick="show('t','reset');"><% write(restartDevice); %></a></div>
                                     </div></td>
                                		</tr>
                                    </table><!--390 -->
                            		</td>
                                </tr>
                            </table>
                        </form>
                    </td>
                </tr>
                </table><!--382 -->

            </td>
        </tr>
       
    </table><!-- 378 -->
    </td></tr>
    <tr><td>
    <table id="footer" border="0" cellpadding="0" cellspacing="0" width="100%">
		<tr valign="bottom">
     		<td width="100%" height="29" colspan="2"  align="center" valign="middle" background="images/bottom.gif" ><% write(Copyright); %>&copy;<% write(Company); %>
            </td>
  		</tr>
     </table>    	
	</td>
    </tr>
 </table>
</td></tr></table>
<div align="center"  id="t" style="display: none;">
	
    	<div  align="center" style="width:100%;height:100%; z-index:-1; position:absolute; background-color:#555;overflow:hidden;" class="al" id="back_div">
		<iframe style="position:absolute; left:0px; top:0px; left:0px; bottom:0px; float:left; z-index:-1; margin:0px; padding:0px;" frameborder="0" scrolling="no" width="100%" height="2000px;" class="al2" id="ifr"></iframe><!--解决 IE6 中不能覆盖select标签 -->
		</div>
		<DIV style="WIDTH: 300px; position:absolute; float:left; z-index:999; clear:both; overflow:hidden; left: 1px;" id="t_div">
        	<iframe style="position:absolute; left:0px; top:0px; left:0px; bottom:0px; float:left; z-index:-1; margin:0px; padding:0px;" frameborder="0" scrolling="no" width="100%" height="2000px;" class="al2" id="ifr"></iframe><!--解决 IE6 中不能覆盖select标签 -->
			<U class="f1"></U><U class="f2"></U><U class="f3"></U>
			<div class="d_top" onMouseDown="moveEvent(event,'t_div')">
				<span id="tiptitle"></span>
				<a href="javascript:" onClick="cl('t')">×</a>
			</div>
			<DIV class="d_body">
				
                	<div id="modifyPWD" align="center" style=" padding-top:5px; padding-left:5px; display:none; " >
                    <form action="" method="post" name="FormModifyPWD">
                	<div id="error" style="color:red; padding-bottom:5px; padding-left:10px;"></div>
                    <div>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<% write(com_passwd); %><img src="images/qt.gif" width="7" height="9">&nbsp;<input name="passwd1"  type="password" style="width:138px" maxlength="31" class="input" /></div><br>
                    <div><% write(eas_confPass); %><img src="images/qt.gif" width="7" height="9"><input name="passwd2" class="input" type="password" style="width:138px" maxlength="31"></div><br/>
                     <div>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(save); %>" onClick="saveUserPwd();"/>&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(back); %>" onClick="cl('t');"/></div><br/>
                      </form>
                    </div>
                     <div id="reset" align="center" style=" padding-top:5px; padding-left:5px; display:none; ">
                     	<form method="post" action="" name="FormReset">
                        	<div><% write(reBootTip); %></div><br/><br/><div>&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(ok); %>" onClick="rebootEasyTask();"/>&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(cancel); %>" onClick="cl('t');"/></div><br/>
                            
                        </form>
                     </div>
                    <div style="clear:both;"></div>
                
			</DIV>
			<div class="d_foot" ></div>
			<U class="f3"></U><U class="f2"></U><U class="f1"></U>
		</DIV>
	</div>
</body>
</html>
