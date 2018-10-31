 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("apBasicConf");%>
<%langTran("commonWords");%>
<%langTran("JS");%>
<title><%write(APBaseSet);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互begin
var roaming_th = 0;
<%
var macStr = Form("macStr");//远程设备mac地址
var urcpSet = Form("urcpSet");//是否联动设置端口后跳转到此页面的
	var devType=Form("devType");
	var needReboot=Form("needReboot");
if (macStr!="" || urcpSet!="")
{//联动信息
	getApBasicConfigUrcp();
}
else
{//本地信息
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
}
%>


/*var szName = new Array();
szName[0] = "test0";
szName[1] = "test1";
szName[2] = "test2";
var userNum =new Array();
userNum[0] = 2;
userNum[1] = 3;
userNum[2] = "";
maxUserNum =32;*/


var devType="<%write(devType);%>";
var needReboot="<%write(needReboot);%>";
if (needReboot == 1)
{
	errorstr=_("<%write(rebootTips);%>");
	closeWin(errorstr);
}
//var macStr=<%write('macStr');%>
//end
/*
if(macStr=='') //不是联动管理
{
	try
	{
		parent.assist.document.getElementById('dev_name').value=deviceName;
	}
	catch(e)
	{
		//父页面不存在
	}
}
*/
var errorstr="";
function checkDbm(cntrl, minNum, maxNum, tips, allownull, onlycheck) {
	var retips = "";
	var num = trimstr(cntrl.value);
	if (num == '') {
		if (!eval(allownull)) retips = tips + "<% write(JSAlert31); %>";
	    else retips = '';
    } else if (isNaN(num)) retips = tips + "<% write(JSAlert39); %>";
    else if ((eval(num) < minNum || eval(num) > maxNum) && eval(num) != 0) retips = tips + "<% write(JSAlert23); %>" + minNum + "<% write(JSAlert24); %>" + maxNum + "<% write(JSAlert25); %>";
	    if (retips != '') {
		    if (!eval(onlycheck)) inputErro(cntrl, retips);
	        return false;
	    } else {
	        if (!eval(onlycheck)) inputRight(cntrl);
	        return true;
	    }
}

function go_back()
{
	ap_conf.action="ap_remote_manage.asp";
	ap_conf.optType.value="";//清空，防止返回的页面误操作
	ap_conf.submit();
}
function init() {
	var obj=null;
	ap_conf.passStr.value = passStr;
	ap_conf.ipStr.value= ipStr;
	ap_conf.macStr.value= macStr;
	ap_conf.szName.value= szName;
	ap_conf.managedEn.value= managedEn;
	if(szName.length == 0){
		existSZ.style.display ="none";
		existName.style.display ="none";
		}
	if(flag_5G != 1)
	{
	    flag5G_on.style.display ="none";
	}
	if (dhcp_en == 1) {
		ap_conf.startdhcp.checked = true;
	} else {
		ap_conf.startdhcp.checked = false;
	}
	if(roaming_en == 1){
		ap_conf.roaming_en.checked = true;
	}else{
		ap_conf.roaming_en.checked = false;
	}
	if(roaming_en5G == 1){
		ap_conf.roaming_en5G.checked = true;
	}else{
		ap_conf.roaming_en5G.checked = false;
	}
	startdhcpFun();
	changeRoaming();
	ap_conf.devType.value= devType;
	if(macStr!='')
	{
		window.document.title = _("<%write(APBaseSet);%>");
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		window.document.title+=" / "+ipStr;
	}
    ap_conf.ap_mode.value = (workingMode==1 ? 1:2);
    ap_conf.deviceName.value = deviceName;
    ap_conf.sysIp.value = ipAddr;
    ap_conf.gateWay.value = gateWay;
    ap_conf.subMask.value = netMask;
    ap_conf.userName.value = userName;
    //ap_conf.max_user.value = maxClient;
    ap_conf.channel.value = channel;
    ap_conf.channel5G.value = channel5G
	ap_conf.roaming_th.value = roaming_th;
	ap_conf.roaming_th5G.value = roaming_th5G;
	mode_init();
	modifyPwFun();
	initUserNum();
}
/**
重填时将所有输入框的错误提示去掉
**/
function reRightAll()
{
	allInputRight(ap_conf);//所有文本输入框
	inputRight(ap_conf.pw1);
	inputRight(ap_conf.pw2);
}

function sameNetCheck(ip,gateway,mask)
{
	var ipA,maskA,ipB,maskB;
	var ip1=new Array();
	var mask1=new Array();
	var ip2=new Array();
	var mask2=new Array();
	ip1=ip.split(".");
	mask1=mask.split(".");
	ipA=((ip1[0]<<24)|(ip1[1]<<16)|(ip1[2]<<8)|(ip1[3]));
	maskA=((mask1[0]<<24))|((mask1[1]<<16))|((mask1[2]<<8))|((mask1[3]));
	
	ip2=gateway;
	mask2=mask.split(".");
	ip2=ip2.split(".");
	ipB=((ip2[0]<<24))|((ip2[1]<<16))|((ip2[2]<<8))|((ip2[3]));
	maskB=((mask2[0]<<24))|((mask2[1]<<16))|((mask2[2]<<8))|((mask2[3]));
	if (trimstr(ap_conf.gateWay.value) != "0.0.0.0") {
	if ((ipA & maskA) != (ipB & maskB))
	{
		alert(_("<%write(netTip1);%>"));
		return false;
	}
	}
	return true;
}


function save() {
	var obj = document.getElementById("ap_mode"); 
	if (obj.value == 1)
	{
		if (!checkName(ap_conf.deviceName, 11 , _("<%write(devName);%>"), 1)) {
            return;
        }
        if (!checkIP(ap_conf.sysIp, _("<%write(ipAddr);%>"))) {
            return;
        }
		if (!document.getElementById('startdhcp').checked) {
			if (trimstr(ap_conf.sysIp.value) == "0.0.0.0") {
				inputErro(ap_conf.sysIp,_("<%write(warning9);%>"));
				return;
			}
		}
		var index = trimstr(ap_conf.sysIp.value).lastIndexOf(".");
		var subString = trimstr(ap_conf.sysIp.value).slice(index + 1);
		if (!document.getElementById('startdhcp').checked) {
		if (subString == "0" || subString == "255")
		{
            inputErro(ap_conf.sysIp,_("<%write(warning11);%>"));
            return;
		}
		}
		inputRight(ap_conf.sysIp);
		
        if (!checkMask(ap_conf.subMask, _("<%write(netMask);%>"), 0)) {
            return;
        }
		if (!document.getElementById('startdhcp').checked) {
			if (trimstr(ap_conf.subMask.value) == "0.0.0.0") {
				inputErro(ap_conf.subMask,_("<%write(warning10);%>"));
				return;
			}
		}
		inputRight(ap_conf.subMask);
		
		if (!document.getElementById('startdhcp').checked) {
        if (!checkIP(ap_conf.gateWay, _("<%write(gw);%>"))) {
            return;
        }
		}
		if (!document.getElementById('startdhcp').checked) {
//			if (trimstr(ap_conf.gateWay.value) == "0.0.0.0") {
//				inputErro(ap_conf.gateWay,_("<%write(gwTips1);%>"));
//				return;
//			}
		}
		var index = trimstr(ap_conf.gateWay.value).lastIndexOf(".");
		var subString = trimstr(ap_conf.gateWay.value).slice(index + 1);
		if (!document.getElementById('startdhcp').checked) {
		if (trimstr(ap_conf.gateWay.value) != "0.0.0.0") {
		if (subString == "0" || subString == "255")
		{
            inputErro(ap_conf.gateWay,_("<%write(gwTips2);%>"));
            return;
		}
		}
		}
		inputRight(ap_conf.gateWay);
		if (!document.getElementById('startdhcp').checked) {
		if (!sameNetCheck(ap_conf.sysIp.value,ap_conf.gateWay.value,ap_conf.subMask.value))
		{
			return;
		}
		}
        if (!checkName(ap_conf.userName,11,_("<%write(username);%>"),0)) {
            return;
		}

		if (document.getElementById('modifyPw').checked)
		{
			if (!checkPassword2(ap_conf.pw1,11,_("<%write(passwd);%>"),0)) {
				return;
			}
			if (!checkPassword(ap_conf.pw2,ap_conf.pw1,11,_("<%write(passwd);%>"),0)) {
				return;
			}
			document.getElementById('modifyPw').value = 1;
		}
		else
		{
			document.getElementById('modifyPw').value = 0;
		}
/*		
		if (!checkNumber(ap_conf.max_user, 1, 32, _("<%write(mazxCLT);%>"), 0)) {
			return;
		}
*/		
		if (!checkDbm(ap_conf.roaming_th, -100, -70, _("<%write(roamTh);%>"), 0, 0)) {
			return false;
		}
		if (!checkDbm(ap_conf.roaming_th5G, -100, -70, _("<%write(roamTh);%>"), 0, 0)) {
			return false;
		}
	}
//	alert(document.getElementById('modifyPw').value);
//	return;
	var i;
	if (ap_conf.ap_mode.value != workingMode)
	{
		if (confirm("<% write(apTip);%>"))
		{
			ap_conf.reboot.value = 1;
		}
		else
		{
			ap_conf.reboot.value = 0;
			return;
		}
	}
	else
	{
		ap_conf.reboot.value = 0;
	}
//	alert(ap_conf.reboot.value);
//	return;
//	ap_conf.action=(macStr==""?"/goform/form":"/goform/formApBasicConfigUrcp");
    document.getElementById('optType').value= "apBasicType";
	ap_conf.ipStr.value = ap_conf.sysIp.value;
	
	var arr = [];
	var str = "";
	var userSum=0;
	for(i=0;i<szName.length;i++){
		var oSelect = document.getElementById("selectSZ"+i);
		var oName = document.getElementById("szName"+i);
		str =oName.value+":"+oSelect.value;
		userSum += parseInt(oSelect.value);
//		alert(userSum);
		arr.push(str);
		}
	ap_conf.szUserNums.value = arr.join(',');
	//alert("szUserNums:"+ap_conf.szUserNums.value);
	//return false;
	if(userSum>maxUserNum){
//	    alert("maxUserNum:"+maxUserNum);
//	    alert("userSum:"+userSum);
	    alert("总客户端数不能超过"+maxUserNum+"个!");
	    return false;
	}
	ap_conf.action = "ap_wait_remote_conf.asp";
    ap_conf.submit();
}

function mode_init()
{
	var obj = document.getElementById("ap_mode"); 
	if (obj.value == 2)
	{
		ap_conf.deviceName.disabled = true;
		ap_conf.sysIp.disabled = true;
		document.getElementById('userName').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('userName').readOnly = true;
		ap_conf.startdhcp.disabled = true;
		ap_conf.pw1.disabled = true;
		ap_conf.pw2.disabled = true;
		ap_conf.subMask.disabled = true;
		ap_conf.gateWay.disabled = true;
		ap_conf.modifyPw.disabled = true;
		ap_conf.roaming_en.checked = false;
		ap_conf.roaming_en.disabled = true;
		ap_conf.roaming_th.disabled = true;
		ap_conf.roaming_en5G.checked = false;
		ap_conf.roaming_en5G.disabled = true;
		ap_conf.roaming_th5G.disabled = true;
		//ap_conf.max_user.disabled = true;
		ap_conf.channel.disabled = true;
		ap_conf.channel5G.disabled = true;
	}
	else
	{
		ap_conf.deviceName.disabled = false;
		ap_conf.sysIp.disabled = false;
		document.getElementById('userName').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('userName').readOnly = true;
		ap_conf.startdhcp.disabled = false;
		ap_conf.pw1.disabled = false;
		ap_conf.pw2.disabled = false;
		ap_conf.subMask.disabled = false;
		ap_conf.gateWay.disabled = false;
		ap_conf.modifyPw.disabled = false;
		ap_conf.roaming_en.disabled = false;
		ap_conf.roaming_th.disabled = false;
		ap_conf.roaming_en5G.disabled = false;
		ap_conf.roaming_th5G.disabled = false;
		//ap_conf.max_user.disabled = false;
		ap_conf.channel.disabled = false;
		ap_conf.channel5G.disabled = false;
	}
}

function startdhcpFun()
{
	if (document.getElementById('startdhcp').checked)
	{
//		ap_conf.sysIp.readOnly = true;
//		ap_conf.subMask.readOnly= true;
//		ap_conf.gateWay.readOnly= true;
		document.getElementById("ip_add").style.display = "none";
		document.getElementById("mask_add").style.display = "none";
		document.getElementById("gw_add").style.display = "none";
		ap_conf.dhcp_en.value = 1;
//		ap_conf.sysIp.onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
//		ap_conf.subMask.onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
//		ap_conf.gateWay.onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
//		ap_conf.sysIp.value = ipStr;
//		ap_conf.gateWay.value = gateWay;
//		ap_conf.subMask.value = netMask;
	}
	else
	{
//		ap_conf.sysIp.readOnly = false;
//		ap_conf.subMask.readOnly = false;
//		ap_conf.gateWay.readOnly = false;
		document.getElementById("ip_add").style.display = "";
		document.getElementById("mask_add").style.display = "";
		document.getElementById("gw_add").style.display = "";
		ap_conf.dhcp_en.value = 0;
	}
}

function modifyPwFun()
{
	if (document.getElementById('modifyPw').checked)
	{
		document.getElementById('pw1').onkeydown= "";
		document.getElementById('pw2').onkeydown= "";
		document.getElementById('pw1').readOnly = false;
		document.getElementById('pw2').readOnly = false;
	}
	else
	{
		document.getElementById('pw1').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('pw2').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('pw1').readOnly = true;
		document.getElementById('pw2').readOnly = true;
		document.getElementById('pw1').value = "";
		document.getElementById('pw2').value = "";
		inputRight(ap_conf.pw1);
		inputRight(ap_conf.pw2);
	}
}
function changeRoaming(){
	  
if(document.getElementById("roaming_en").checked){
	document.getElementById("roaming_en").value = 1;
	document.getElementById("roaming_th").disabled = false;
  }else{
	document.getElementById("roaming_en").value = 0;
	document.getElementById("roaming_th").disabled = true;
  }
if(document.getElementById("roaming_en5G").checked){
	document.getElementById("roaming_en5G").value = 1;
	document.getElementById("roaming_th5G").disabled = false;
  }else{
	document.getElementById("roaming_en5G").value = 0;
	document.getElementById("roaming_th5G").disabled = true;
  }
}

function initUserNum(){
	var oTable= document.getElementById("szTable");
	//alert("oTable :"+document.getElementById("szTable"));
	for(var i=0;i<szName.length;i++){
		var TR = document.createElement("tr");
		var TD1 = document.createElement("td");
		var TD2 = document.createElement("td");
		TD1.innerHTML = "<input type='text' id='szName"+i+"' name='szName"+i+"' disabled='disabled' value='"+szName[i]+"' size='10'></input>";
		TD2.innerHTML = "<select id='selectSZ"+i+"' name='selectSZ"+i+"'><option value=0 selected='selected'>0</option></select>";
		TR.appendChild(TD1);
		TR.appendChild(TD2);
		oTable.tBodies[0].appendChild(TR);
		var oSelect = document.getElementById("selectSZ"+i);
		for(var j=1;j<=maxUserNum;j++){
			var opp = new Option(j,j);
			if(j==userNum[i]){
				opp.selected="selected"
			}
			oSelect.add(opp);
		}//for(var j=0;j<maxUserNum;j++)
			
	 }//for(var i=0;i<szName.length;i++){

	}//initUserNum
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">
	urcpErrorPost("ap_remote_manage.asp");
</script>
<table width="100%" border="0"  cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(APBaseSet);%>"));//显示页面名称</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" bordercolor="red" width="80%" align='center'>	<tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	<form method="post" name="ap_conf">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="devType">
<input type="hidden" value="" name="reboot">
<input type="hidden" value="" name="dhcp_en">
<input type="hidden" value="" name="optType" id="optType">
<input type="hidden" value="" name="szUserNums" id="szUserNums">
<br>
<TABLE  width="600"  border="0" bordercolor="yellow"  cellpadding="0" cellspacing="0"   align="center">
<tr >
<td width="133"  align="left" valign="bottom" id="APBaseSet_td"><b><%write(APBaseSet);%></b></td>
<td width="467" align="right" valign="bottom">&nbsp;</td>
</tr>
<tr><td colspan="2" height="10"></td></tr>
</TABLE>
<TABLE id="mainTable"  width="600"  border="0" bordercolor="blue"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18">
  <td class=vncell height="22" id="workMode_td"><%write(workMode);%></td>
  <td colspan="4" class=vtable><select name="ap_mode" id="ap_mode" class="input" style="width:60px" onChange="mode_init()">
    <option value="1" id="thinAP"><%write(thinAP);%></option>
    <option value="2" id="fatAP"><%write(fatAP);%></option>
  </select></td>
</tr>
<tr >
  <td class=vncell height="22" width="<% write(apwidth1);%>" id="devName_td"><%write(devName);%></td>
  <td colspan="4" class=vtable><input type="text" class="input" name="deviceName" size="16" maxlength="11" value="" onChange='checkName(this,11,_("<%write(devName);%>"),1);'></td>
</tr>

<tr class="style18">
  <td class=vncell height="22" id="passwd_td2"><%write(startdhcp);%></td>
  <td colspan="4" class=vtable><input type="checkbox" name="startdhcp" id="startdhcp" onClick="startdhcpFun();"></td>
</tr>

<tr class="style18" style="display:" id="ip_add" height="22">
<td class=vncell height="22" width="<% write(apwidth1);%>" id="ipAddr_td"><%write(ipAddr);%></td>
<td colspan="4" class=vtable><input class="input" type="text" name="sysIp" onKeyPress="ipOnly()" height="22" size="16" maxlength="15" value="" onChange='checkIP(this,_("<%write(ipAddr);%>"));'><div id="ipaddr"><div></td>
</tr>
<tr class="style18" style="display:" id="mask_add" height="22">
<td class=vncell height="22" width="<% write(apwidth1);%>" id="mask_td"><%write(netMask);%></td>
<td colspan="4" class=vtable><input class="input" type="text" name="subMask" onKeyPress="ipOnly()" height="22" size="16" maxlength="15" value="" onChange='checkMask(this,_("<%write(netMask);%>"),0);'><div id="mask"><div></td>
</tr>
<tr class="style18" style="display:" id="gw_add" height="22">
  <td class=vncell height="22" id="gateway_td"><%write(gw);%></td>
  <td colspan="4" class=vtable><input class="input" type="text" name="gateWay" onKeyPress="ipOnly()" height="22" size="16" maxlength="15" value="" onChange='checkIP(this,_("<%write(gw);%>"));'></td>
</tr>
<tr>
  <td class=vncell height="22" width="<% write(apwidth1);%>" id="userName_td"><%write(username);%></td>
  <td colspan="4" class=vtable><input type="text" class="input" name="userName" id="userName" size="16" maxlength="11" onChange='checkName(this,11,_("<%write(username);%>"),0);' value="admin">  <div id="userName"></div></td>
</tr>
<tr class="style18">
  <td class=vncell height="22" id="passwd_td2"><%write(modifyPw);%></td>
  <td colspan="4" class=vtable><input type="checkbox" name="modifyPw" id="modifyPw" onClick="modifyPwFun();"></td>
</tr>
<tr>
<td class=vncell height="22" width="<% write(apwidth1);%>" id="passwd_td"><%write(newPasswd);%></td>
<td colspan="4" class=vtable><input type="password" class="input" name="pw1" size="16" maxlength="11" value="" id="pw1" readonly><div id="passwd1"></div></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="<% write(apwidth1);%>" id="surepasswd_td"><%write(sureNewPasswd);%></td>
<td colspan="4" class=vtable><input type="password" class="input" name="pw2" size="16" maxlength="11" value="" id="pw2" readonly><div id="passwd2"></div></td>
</tr>
<tr id="existSZ" style="display:">
<td>最大客户端数量限制</td>
<td>加入服务区后可使用该功能(0表示不限制)</td>
</tr>

<tr id="existName" style="display:">
<td></td>
<td><table width="100%" border="0" id="szTable"><tr><td width="30%">服务区</td><td>客户端数</td></tr></table></td>
</tr>

</table>
<br />
<table  width="600"  border="0" bordercolor="red" cellpadding="0" cellspacing="0"   align="center">
<tr >
<td width="133"  align="left" valign="bottom" id="APBaseSet_td"><b>2.4GHz射频</b></td>
<td width="467" align="right" valign="bottom">&nbsp;</td>
</tr>
<tr><td colspan="2" height="10"></td></tr>
</table>

<table  width="600"  border="0" bordercolor="green"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18">
  <td class=vncell height="22" ><%write(roamEn);%></td>
  <td colspan="4" class=vtable><input type="checkbox" name="roaming_en" id="roaming_en" onClick="changeRoaming();"></td>
</tr>

<tr>
  <td class=vncell height="22" id="roamTh_td"><%write(roamTh);%></td>
  <td colspan="4" class=vtable><input type="text" class="input" name="roaming_th" id="roaming_th" size="3" maxlength="4" value="-88" onChange='checkDbm(this, -100, -70, _("<%write(roamTh);%>"), 0, 0);'><%write(roamingThresValue);%>
</td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="<% write(apwidth1);%>" id="chnanel_td"><%write(channel);%></td>
<td colspan="4" class=vtable><select name="channel" id="channel" class="input" style="width:60px">
  <option value="0" id="auto"><%write(auto);%></option>
  <option value="1">1</option>
  <option value="2">2</option>
  <option value="3">3</option>
  <option value="4">4</option>
  <option value="5">5</option>
  <option value="6">6</option>
  <option value="7">7</option>
  <option value="8">8</option>
  <option value="9">9</option>
  <option value="10">10</option>
  <option value="11">11</option>
</select></td>
</tr>
</table>
<br />
<div id="flag5G_on" style="display:">
<table width="600"  border="0" bordercolor="red" cellpadding="0" cellspacing="0"   align="center">
<tr>
<td width="133"  align="left" valign="bottom" id="APBaseSet_td"><b>5GHz射频</b></td>
<td width="467" align="right" valign="bottom">&nbsp;</td>
</tr>
<tr><td colspan="2" height="10"></td></tr>
</table>
<table  width="600"  border="0" bordercolor="green"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18">
  <td class=vncell height="22" ><%write(roamEn);%></td>
  <td colspan="4" class=vtable><input type="checkbox" name="roaming_en5G" id="roaming_en5G" onClick="changeRoaming();"></td>
</tr>

<tr>
  <td class=vncell height="22" id="roamTh_td"><%write(roamTh);%></td>
  <td colspan="4" class=vtable><input type="text" class="input" name="roaming_th5G" id="roaming_th5G" size="3" maxlength="4" value="-88" onChange='checkDbm(this, -100, -70, _("<%write(roamTh);%>"), 0, 0);'><%write(roamingThresValue);%>
</td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="<% write(apwidth1);%>" id="chnanel_td"><%write(channel);%></td>
<td colspan="4" class=vtable><select name="channel5G" id="channel5G" class="input" style="width:60px">
  <option value="0" id="auto"><%write(auto);%></option>
<!--  <option value="1">1</option> -->
  <option value="36">36</option>
  <option value="40">40</option>
  <option value="44">44</option>
  <option value="48">48</option>
  <option value="149">149</option>
  <option value="153">153</option>
  <option value="157">157</option>
  <option value="161">161</option>
  <option value="165">165</option>
  
</select></td>
</tr>
</table>
</div>
<div style="width:600px;height:20px; text-align:left; line-height:20px; margin:5px auto 0 auto; color:#3f3b3a;">
<b>注意：最大客户端数量限制，仅对已加入服务区的AP启用！</b>
</div>
<table align="center" border="0"><tr><td height="5"></td></tr><tr><td>
<input type="button"  onClick="save()"class="input-button" value="<%write(save);%>" name="" id="saveButton">&nbsp;
<input type="button" class="input-button" value="<%write(refill);%>" name="" onClick="javascript:document.forms[0].reset();init();reRightAll();mode_init();" id="resetButton">
<input type="button" class="input-button" name="helpButton" value="<%write(help);%>" onClick="window.open('<% write(com_helppage);%>#apBasicConf')" id="helpButton">
<input type="button" class="input-button" name="goBack" value="<%write(back);%>" onClick="go_back();" id="backButton">
</td></tr></table>
</form>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 </td><td class="gapTd"></td></tr> </table> </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>
</body>
</html>
