 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>系统设置</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%
var macStr = Form("macStr");//远程设备mac地址
var urcpSet = Form("urcpSet");//是否联动设置端口后跳转到此页面的
if (macStr!="" || urcpSet!="")
{//联动信息
	aspOutPutGetSysConfUrcp();
}
else
{//本地信息
	aspOutPutGetSysConf();
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
}
%>
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
function go_back()
{
	frm_sys.action="remote_manage.asp";
	frm_sys.submit();
}
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#sysUrcp')};
		}
	}
}
function init() {
	var obj=null;
	frm_sys.passStr.value = passStr;
	frm_sys.ipStr.value= ipStr;
	frm_sys.macStr.value= macStr;
	if(macStr!='')
	{
		window.document.title = "系统设置";
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		window.document.title+=" / "+ipStr;
		initHelp();
	}
    if (onDhcpOpt82 == 1) frm_sys.dhcpOpt82.checked = true;
    if (onDhcpClient == 1) frm_sys.dhcpClient.checked = true;
    if (onArp == 1) frm_sys.arp.checked = true;
    frm_sys.sysIp.value = sysIp;
    frm_sys.sysMask.value = sysMask;
    frm_sys.sysGw.value = sysGw;
    frm_sys.vlanId.value = vlanId;
    frm_sys.macAgeTime.value = macAgeTime;
    frm_sys.deviceName.value = deviceName;
    frm_sys.pw1.value = pw;
    frm_sys.pw2.value = pw;
    dhcpcli_on(0);
}
/**
重填时将所有输入框的错误提示去掉
**/
function reRightAll()
{
	allInputRight(frm_sys);//所有文本输入框
	inputRight(frm_sys.pw1);
	inputRight(frm_sys.pw2);
}
function dhcpcli_on(ctl) {
    if (ctl != 0 && ctl.checked) {
        if (!confirm("您是否确定使用DHCP来为交换机分配IP地址？")) {
            ctl.checked = false;
            return;
        }
    }
    if (frm_sys.dhcpClient.checked) {
		frm_sys.sysIp.value = sysIp;
    	frm_sys.sysMask.value = sysMask;
    	frm_sys.sysGw.value = sysGw;
		inputRight(frm_sys.sysIp);
		inputRight(frm_sys.sysMask);
		inputRight(frm_sys.sysGw);
        frm_sys.sysIp.disabled = true;
        frm_sys.sysMask.disabled = true;
        frm_sys.sysGw.disabled = true;
    } else {
        frm_sys.sysIp.disabled = false;
        frm_sys.sysMask.disabled = false;
        frm_sys.sysGw.disabled = false;
    }
}

function save() {
    if (!frm_sys.dhcpClient.checked) {
        if (!checkIP(frm_sys.sysIp, "IP地址")) {
            return;
        }
        if (trimstr(frm_sys.sysIp.value) == "0.0.0.0") {
            inputErro(frm_sys.sysIp,"IP地址不能为0.0.0.0!");
            return;
        }
		var index = trimstr(frm_sys.sysIp.value).lastIndexOf(".");
		var subString = trimstr(frm_sys.sysIp.value).slice(index + 1);
		if (subString == "0" || subString == "255")
		{
            inputErro(frm_sys.sysIp,"IP地址最后一个数字不能为0或255!");
            return;
		}
		inputRight(frm_sys.sysIp);
        if (!checkMask(frm_sys.sysMask, "子网掩码")) {
            return;
        }
		if (trimstr(frm_sys.sysMask.value) == "0.0.0.0") {
            inputErro(frm_sys.sysMask,"子网掩码不能为0.0.0.0!");
            return;
        }
		inputRight(frm_sys.sysMask);
        if (!checkIP(frm_sys.sysGw, "网关地址")) {
            return;
        }
		if (trimstr(frm_sys.sysGw.value) == "0.0.0.0") {
            inputErro(frm_sys.sysGw,"网关地址不能为0.0.0.0!");
            return;
        }
		inputRight(frm_sys.sysGw);
        if ((inet_addr(frm_sys.sysGw.value) & inet_addr(frm_sys.sysMask.value)) != (inet_addr(frm_sys.sysIp.value) & inet_addr(frm_sys.sysMask.value))) {
            inputErro(frm_sys.sysGw, "IP地址与静态网关不在同一网段上!");
            return;
        } else {
            inputRight(frm_sys.sysGw);
        }
        var gwgw = trimstr(frm_sys.sysGw.value);
        for (var i = 0; i < srcIpAddr.length; i++) {
            if (srcIpAddr[i] == gwgw && srcIpMask[i] == '255.255.255.255') {
                inputErro(frm_sys.sysGw, "网关地址不能是IP过滤绑定的地址!");
                return;
            }
        }
        inputRight(frm_sys.sysGw);
    }

    if (!checkNumber(frm_sys.vlanId, 1, 4094, 'CPU VLAN ID')) {
        return;
    }
    if (!checkNumber(frm_sys.macAgeTime, 30, 5000, 'MAC地址老化时间')) {
        return;
    }
    if (!checkName(frm_sys.deviceName, 11, '设备名', 1)) {
        return;
    }
    if (!checkPassword(frm_sys.pw1, frm_sys.pw2, 11, 1)) {
        return;
    }
	frm_sys.action=(macStr==""?"/goform/formSysConf":"/goform/formSysConfUrcp");
    frm_sys.submit();
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('系统设置');//显示页面名称</script>
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
	<table border="0" width="80%" align='center'>	<tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	<form method="post" name="frm_sys">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="sysTime">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<br>
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr ><td width="133"  align="left" valign="bottom" ><b>系统设置</b></td>
<td width="467" align="right" valign="bottom"><input type="button"  onClick="save()"class="input-button" value="保存" name="">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="重填" name="" onClick="javascript:document.forms[0].reset();init();reRightAll();">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="帮助" onClick="window.open('../rhelpinfo.htm#sys')">
<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();"></td>
</tr>
<tr><td colspan="2" height="10"></td></tr>
</TABLE>
<TABLE  width="600"  border="0"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18"><td class=vncell height="22" width="22%">启用DHCP中继</td>
<td class=vtable><input type="checkbox" name="dhcpOpt82" value="1"  ></td></tr>
<tr >
<td class=vncell height="22" width="22%">启用DHCP客户端</td>
<td class=vtable><input type="checkbox" name="dhcpClient" value="1"   onclick="dhcpcli_on(this)"></td>
</tr>
<tr class="style18" height="22">
<td class=vncell height="22" width="22%">IP地址</td>
<td class=vtable><input class="input" type="text" name="sysIp" onKeyPress="ipOnly()" height="22" size="16" maxlength="15" value="" onChange=checkIP(this,"IP地址")><div id="ipaddr"><div></td>
</tr>
<tr >
<td class=vncell height="22" width="22%">子网掩码</td>
<td class=vtable><input type="text" class="input" name="sysMask" size="16" onKeyPress="ipOnly()" maxlength="15" value="" onChange=checkMask(this,"子网掩码")><div id="mac"><div></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">网关</td>
<td class=vtable><input type="text" class="input"  name="sysGw" size="16" onKeyPress="ipOnly()" maxlength="15" value="" onChange=checkIP(this,"网关地址")><div id="gateway"><div></td>
</tr>
<tr>  
<td class=vncell height="22" width="22%">CPU VLAN ID</td>
<td class=vtable><input type="text" class="input" name="vlanId" size="16" maxlength="4" onKeyPress="figureOnly()" value=""  onChange="checkNumber(this, 1,4094, 'CPU VLAN ID')"><div id="vid"></div></td></tr>
<tr class="style18">
<td class=vncell height="22" width="22%">MAC地址老化时间(秒)</td>
<td class=vtable><input type="text" class="input" name="macAgeTime" size="16" maxlength="4" value="" onChange="checkNumber(this, 30, 5000, 'MAC地址老化时间')">
  <div id="mactime"></div></td>
</tr>
<tr class="">
<td class=vncell height="22" width="22%">设备名</td>
<td class=vtable><input type="text" class="input" name="deviceName" size="16" maxlength="11" value="" onChange="checkName(this,11,'设备名',1);"><div id="eqname"></div></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">密码</td>
<td class=vtable><input type="password" class="input" name="pw1" size="16" maxlength="11" value="" onChange="checkPassword1(this,11,1);" id="pw1"><div id="passwd1"></div></td>
</tr>
<tr >
<td class=vncell height="22" width="22%">密码确认</td>
<td class=vtable><input type="password" class="input" name="pw2" size="16" maxlength="11" value="" onChange="checkPassword(frm_sys.pw1,this,11,1)" id="pw2"><div id="passwd2"></div></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">启用ARP欺骗防御</td>
<td class=vtable><input type="checkbox" value="1" name="arp" size="16" maxlength="16"></td>
</tr>
</table>
<table align="center"  ><tr><td height="5"></td></tr><tr><td>
<input type="button"  onClick="save()"class="input-button" value="保存" name="">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="重填" name="" onClick="javascript:document.forms[0].reset();init();reRightAll();">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="帮助" onClick="window.open('../rhelpinfo.htm#sys')">
<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();">
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
