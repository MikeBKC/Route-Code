<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("ap_wait_remote_auth");%>
<%langTran("commonWords");%>
<title><%write(checkingCode);%></title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
var passStr=Form("passStr");//远程密码字串
var serialNo=Form("serialNo");
var devType=Form("devType");//
var szName=Form("szName");
var managedEn=Form("managedEn");
if ( macStr != "" ) 
{
	remoteAuth();//c语言密码验证函数
}
%>
var ipStr="<%write(ipStr);%>";//远程ip字串
var macStr="<%write(macStr);%>";//远程mac字串
var passStr="<%write(passStr);%>";//远程密码字串
var serialNo="<%write(serialNo);%>";
var devType="<%write(devType);%>";
var szName="<%write(szName);%>";
var managedEn="<%write(managedEn);%>";
//end交互
if(macStr=="") closeWin(_("lawless"));
var waitTime=3;//等待时间
var waitContent=_("<%write(checkingCode);%>");//等待内容或等待提示信息
//var waitContent="操作中，请稍等......";//等待内容或等待提示信息
var resultPage="ap_remote_manage_group.asp";//到时候的跳转页面 
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
		document.getElementById('waitContent').innerHTML=_("overTMConnect");
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
	REMOTECONF.serialNo.value= serialNo;
	REMOTECONF.devType.value= devType;
	REMOTECONF.szName.value= szName;
	REMOTECONF.managedEn.value= managedEn;
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png">
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
  <table width="100%" border="0" height="200" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
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
    <input type="hidden" value="wait" name="parentPage">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="serialNo">
    <input type="hidden" value="" name="szName">
    <input type="hidden" value="" name="managedEn">
    <input type="hidden" value="" name="devType">
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
