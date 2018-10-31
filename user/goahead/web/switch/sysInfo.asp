 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>系统状态-系统信息</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
var passStr=Form("passStr");//远程密码字串
if (macStr=="")
{
	aspOupPutGetSysInfo();//本地
	write('var urcpError="";');
}
else 
{
	aspOupPutGetSysInfoUrcp();//联动
	write('var urcpError="";');
}
%>
var ipStr="<%write(ipStr);%>";//远程ip字串
var macStr="<%write(macStr);%>";//远程mac字串
var passStr="<%write(passStr);%>";//远程密码字串
var dhcpServer = "0.0.0.0";
var dhcpLeaseTime = 10; //dhcp租用时间剩余(单位sec)
//end

//进行时间处理
	sysDate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
	Year= sysDate.getFullYear();
	Month= sysDate.getMonth();
	Day= sysDate.getDate();

	if (10 > sysDate.getHours())
		Hour="0"+sysDate.getHours();
	else
		Hour=""+sysDate.getHours();
	if (10 > sysDate.getMinutes())
		Minute="0"+sysDate.getMinutes();
	else
		Minute=""+sysDate.getMinutes();
	if (10 > sysDate.getSeconds())
		Second="0"+sysDate.getSeconds();
	else
		Second=""+sysDate.getSeconds();
	Month1=Month+1;//路由器内部0表示1月，1表示2月……
	var systimes=Year.toString()+"-"+Month1.toString()+"-"+Day.toString()+"   "+Hour.toString()+":"+Minute.toString()+":"+Second.toString();
		
//end

var cpu1=null;
var mem=null;
var outerFlash=null;
var outerFlashStat=0;
var outerFlashUsedSpace=0;
function go_back()
{
	SYSINFO.action="remote_manage.asp";
	SYSINFO.submit();
}
function init_globalData()
{
	outerFlashUnusedSpace=convertToByte(outerFlashUnusedSpace);
	outerFlashTotalSpace=convertToByte(outerFlashTotalSpace);
	outerFlashUsedSpace=outerFlashTotalSpace-outerFlashUnusedSpace;
	if(outerFlashTotalSpace)
	{
		if(!(parseInt(outerFlashTotalSpace)))/*总空间大小为0*/
		{
			outerFlashTotalSpace =false;
		}
	}
	if(outerFlashTotalSpace)
	{
		/*使用率*/
		outerFlashStat = Math.round(outerFlashUsedSpace/outerFlashTotalSpace*100);
	}
}
init_globalData();
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#sysinfoUrcp')};
		}
	}
}
function pageInit()
{
	var obj=null;
	SYSINFO.passStr.value = passStr;
	SYSINFO.ipStr.value= ipStr;
	SYSINFO.macStr.value= macStr;
	if(macStr!='')
	{
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		window.document.title+=" / "+ipStr;
		initHelp();
	}
	cpu1=document.getElementById('cpu1');
	mem=document.getElementById('mem');
	outerFlash = document.getElementById('outerFlash');
	var flag1 = false;
	var flag2 = false;
	var flag3 = false;

	if(!outerFlashTotalSpace)
	{
		document.getElementById("trOuterFlash").style.display = "none";
	}
	else
	{
		outerFlash.parentNode.title = "已用空间："+byteFormat(outerFlashUsedSpace)+"\n总大小："+byteFormat(outerFlashTotalSpace);
	}
	if (cpuStat == 0) {
		cpu1.bgColor = "#FFFFFF";
		flag1 = true;
	}

	if (memStat == 0) {
		mem.bgColor = "#FFFFFF";
		flag2 = true;
	}
	
 	if (outerFlashStat == 0) {
		outerFlash.bgColor = "#FFFFFF";
		flag3 = true;
	}
	if (!flag1) {
		cpu1.width = cpuStat + "%";
	}

	if (!flag2) {
		mem.width = memStat + "%";
	}
	if (!flag3) {
		outerFlash.width = outerFlashStat + "%";
	}
	chgColor();
}
function chgTableColor(td) {
	if (td == null) return;
	var color = "";
	var width = eval((td.width.substring(0, td.width.length - 1)));

	if (width >= 70) {
		td.bgColor = "#FF0000";
	}
	else if (width >= 50 && width < 70) {
		td.bgColor = "#FFCC00";
	}
	else if (width >= 0 && width < 50) {
		td.bgColor = "#00FF66";
	}
}
function chgColor() {
	var natSess = 0;
	if (cpuStat != 0) {
		chgTableColor(cpu1);
	}
	if (memStat != 0) {
		chgTableColor(mem);
	}
	if (outerFlashStat != 0) {
		chgTableColor(outerFlash);
	}
}

//end交互
</script>
<style type="text/css">
.style24 {border:1px solid #408080}
</style>
</head>
<body onLoad="pageInit()" onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
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
  <td><script language="javascript">showSecondTitle('系统状态-系统信息');//显示页面名称</script>
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
	<table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form method="post" action="sysInfo.asp"  name="SYSINFO">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="sysTime">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<br>
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr><td height="5" colspan="2"></td></tr>
<tr><td width="284"   align="left"><b>系统资源状态</b></td>
  <td width="316"   align="right">
    <input type="button" id="helpButton" name="helpButton" class="input-button" value="帮助" onClick="window.open('../rhelpinfo.htm#sysinfo')">
    <input type="button" class="input-button" value="刷新" onClick="SYSINFO.submit()">
    <input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();">
  </label></td>
</tr>
<tr><td height="10" colspan="2"></td></tr>
</TABLE>
<table width="600" border="0" align="center"  cellpadding="4" cellspacing="1" class="style22" >
    <tr class="style18">
	<td width="22%" valign="middle" height="22" id="cpu" >&nbsp;CPU占用</td>
	<td width="78%" height="22">
	  <table class="style24" width="135" align="left" bgcolor="#FFFFFF">
	    <tr><td id="cpu1" height="15" width="1%"></td><td></td></tr>
	      </table>
	  <script>
	document.write(cpuStat.toString() + "%");
	  </script>	  &nbsp;	</td>
</tr>
	 <tr>
	<td width="22%" valign="middle" height="22" id="neicun">&nbsp;内存使用</td>
	<td height="22">
	  <table class="style24" width="135" align="left" bgcolor="#FFFFFF">
	    <tr><td id="mem" height="15" width="1%"></td><td ></td></tr>
	      </table>
	  <script>
	document.write(memStat.toString() + "%");
	  </script>	  &nbsp;	</td>
</tr>
	 <tr class="style18" id="trOuterFlash">
	   <td valign="middle" height="22" id="neicun2">&nbsp;U盘使用</td>
	   <td height="22">  
       <table class="style24" width="135" align="left" bgcolor="#FFFFFF">
	    <tr><td id="outerFlash" height="15" width="1%"></td><td ></td></tr>
	      </table>
	  <script>
	document.write(outerFlashStat.toString() + "%");
	  </script>	  &nbsp;	</td>
</tr>
</table>
 	<table width=600 border="0" cellspacing="0" align="center" cellpadding="0"><tr>
    <tr height="20"><td></td></tr>
   		<td background="../images/dot.gif"><img src="../images/space.gif" width="8" height="1"></td>
  	</tr><tr height="20"><td></td></tr></table>
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr><td   align="left"><b>系统基本信息</b></td></tr>
<tr height="10"><td></td></tr>
</TABLE>
	<TABLE  width="600"  border="0"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18">
<td class=vncell height="10" width="22%">系统当前时间</td>
<td class=vtable>	<script>document.write(systimes);</script></td>
</tr>
<tr>
  <td class=vncell height="5">系统运行时间</td>
  <td class=vtable><script>document.write(upTime);</script></td>
</tr>
<tr class="style18">
  <td class=vncell height="5">设备名</td>
  <td class=vtable><script>document.write(deviceName);</script></td>
</tr>
<tr  >
<td class=vncell height="22" width="22%">序列号</td>
<td class=vtable><script>document.write(sysSerial);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">软件版本</td>
<td class=vtable><script>document.write(sysVersion);</script></td>
</tr>
<tr  >
<td class=vncell height="22" width="22%">MAC地址</td>
<td class=vtable><script>document.write(sysMac);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">IP地址</td>
<td class=vtable><script>document.write(sysIp);</script></td></tr>
<tr  >
<td class=vncell height="22" width="22%">子网掩码</td>
<td class=vtable><script>document.write(sysMask);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="10" width="22%">网关</td>
<td class=vtable><script>document.write(sysGw);</script></td></tr>
<tr>
  <td class=vncell height="11">CPU VLAN ID</td>
  <td class=vtable><script>document.write(vlanId);</script></td>
</tr>
<!--
<tr class="style18">
<td class=vncell height="22" width="22%">DHCP服务器</td>
<td class=vtable><script>document.write(dhcpServer);</script></td>
</tr>
<tr>
<td class=vncell height="22" width="22%">租用时间剩余</td>
<td class=vtable><script>document.write(dhcpLeaseTime);</script> 分钟</td>
</tr>
-->
</table>
</form>
<br><br>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 </td><td class="gapTd"></td></tr> </table>
 </td>
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
