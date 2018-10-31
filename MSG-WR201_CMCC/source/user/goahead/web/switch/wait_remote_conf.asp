<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>联动操作中,请稍等……</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<%
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
var passStr=Form("passStr");//远程密码字串
var isGroupOpt=Form("isGroupOpt");
var optType=Form("optType");
if (macStr!="")
{
	if (optType == "sendConf") aspRemoteConfSend();//配置下发
	else if (optType == "getConf") aspRemoteConfGet();//配置获取
	else if (optType == "getEventslog") aspRemoteEventslogGet();//配置获取
}
%>
var ipStr="<%write(ipStr);%>";//远程ip字串
var macStr="<%write(macStr);%>";//远程mac字串
var passStr="<%write(passStr);%>";//远程密码字串
var isGroupOpt="<%write(isGroupOpt);%>";
var optType="<%write(optType);%>";
//end交互
if(macStr=="") closeWin("非法操作！");
var waitTime=5;//等待时间
var waitContent;//等待内容或等待提示信息
var resultPage;//到时候的跳转页面 
if(optType=="sendConf") 
{
	waitContent = "配置下发中,请稍等……";
	resultPage ="linkageConfSend.asp";
	
}
else if(optType=="getConf")
{
	waitContent = "配置获取中,请稍等……";
	resultPage ="linkageConf.asp";
}
else if(optType=="getEventslog")
{
	waitContent = "日志获取中,请稍等……";
	resultPage ="linkageEventslog.asp";
}
var ptr=null;
var remandTime=15;
var triggerTime= remandTime - waitTime;
function waitF()
{
	remandTime--;
	if(remandTime >= 0 ) 
	{
		document.getElementById('waitTime').innerHTML=remandTime;
	}
	if(remandTime == triggerTime)
	{
		REMOTECONF.action=resultPage;
		REMOTECONF.submit();
	}
	if(remandTime <= 0)
	{
		document.getElementById('waitContent').innerHTML="连接超时，请检查您的网络连接……";
		clearInterval(ptr);
	}
}
function init()
{  
	document.getElementById('waitTime').innerHTML=remandTime;
	document.getElementById('waitContent').innerHTML=waitContent;
	ptr=setInterval("waitF()",1000);
	REMOTECONF.passStr.value = passStr;
	REMOTECONF.ipStr.value= ipStr;
	REMOTECONF.macStr.value= macStr;
	REMOTECONF.isGroupOpt.value= isGroupOpt;
	REMOTECONF.optType.value= optType;
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <form name="REMOTECONF" id="REMOTECONF" method="post">
    <input type="hidden" name="isGroupOpt">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="1" name="waitOver">
    <input type="hidden" value="" name="optType">
     <table width="300" height="120" align="center">
     <tr><td height="40"></td></tr>
     <tr>
     <td id=waitContent  align="center"></td>
     </tr> 
     <tr><td id="waitTime" align="center"></td>
     </tr>
        <tr><td height="40"></td></tr>
     </table>
     </form>
   <!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
   </td></tr> </table>
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
