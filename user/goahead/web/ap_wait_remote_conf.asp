<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("ap_wait_remote_conf");%>
<title><%write(wait);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
<%
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
var passStr=Form("passStr");//远程密码字串
var isGroupOpt=Form("isGroupOpt");
var optType=Form("optType");
var serialNo=Form("serNo");
var devType=Form("devType");
var szName=Form("szName");
var managedEn=Form("managedEn");
var SZNameArray=Form("SZNameArray");
if (macStr!="")
{
	if (optType == "sendAPConf")
	{
	 aspRemoteAPConfSend();	//下发配置
	 }
	else if (optType == "sendAPSoft")
	{
	 aspRemoteApSoftSend();	//下发软件
	 }
	else if (optType == "sendAPTemp")
	{
	 aspRemoteApConfTempSend();	//下发配置模版
	 }
	else if (optType == "getApConf")
	{
	 aspRemoteConfGet();	//获取配置
	 }
	else if (optType == "getApSoft")
	{
	 aspRemoteSoftGet();	//获取软件
	 }
	 else if (optType == "joinSZ")
	 {
	 aspRemoteApJoinSZ();	//加入服务区
	 }
	 else if (optType == "PjoinSZ")
	 {
     aspJoinSz();
	 }
	 else if (optType == "apBasicType")
	 {
		formApBasicConfigUrcp();
	 }
	 else if (optType == "apMacFilter")
	 {
		aspRemoteApMacFiter();//下发MAC地址过滤
	 }
	 else if (optType == "PapMacFilter")
	 {
		aspApBatchMacFilter();//批量下发MAC地址过滤
	 }

}
%>
var ipStr="<%write(ipStr);%>";//远程ip字串
var macStr="<%write(macStr);%>";//远程mac字串
var passStr="<%write(passStr);%>";//远程密码字串
var isGroupOpt="<%write(isGroupOpt);%>";
var optType="<%write(optType);%>";
var serialNo="<%write(serialNo);%>";
var devType="<%write(devType);%>";
var szName="<%write(szName);%>";
var managedEn="<%write(managedEn);%>";
//end交互
//alert(optType);
if(macStr=="") closeWin(_("<%write(lawless);%>"));
var waitTime=5;//等待时间
var remandTime=15;
var waitContent;//等待内容或等待提示信息
var resultPage;//到时候的跳转页面 
if(optType=="sendAPConf") //下发配置
{
	waitContent = _("<%write(outtingConfig);%>");
	resultPage ="pushConf.asp";
}
else if(optType=="sendAPSoft")	//下发软件
{
	waitTime = 360;
	remandTime = 380;
	waitContent = _("<%write(outtingSoftware);%>");
	resultPage = "pushSoft.asp";
}
else if(optType=="sendAPTemp")	//下发配置模版
{
	waitContent = _("<%write(outtingConfTemp);%>");
	resultPage = "pushTmp.asp";
}
else if(optType=="getApConf")	//获取配置
{
	waitContent = _("<%write(gettingConfig);%>");
	resultPage ="getApConf.asp";
}
else if(optType=="getApSoft")	//获取软件
{
	waitTime = 40;
	remandTime = 45;
	waitContent = _("<%write(gettingSoft);%>");
	resultPage ="apSoftMng.asp";
}
else if(optType=="joinSZ")	//加入服务区
{
	waitContent = "<%write(waitContent);%>";
	resultPage ="serviceZone.asp";
}
else if(optType=="PjoinSZ")	//加入服务区
{
	waitTime = 20;
	remandTime = 25;
	waitContent = "<%write(waitContentP);%>";
	resultPage ="serviceZone.asp";
}
else if(optType=="apBasicType")	//加入服务区
{
	waitTime = 5;
	remandTime = 10;
	waitContent = "<%write(waitContentBC);%>";
	resultPage ="ap_remote_manage.asp?urcpSet=1";
}
else if(optType=="apMacFilter")	//MAC地址过滤
{
	waitTime = 10;
	remandTime = 15;
	waitContent = "<%write(waitContentMacFilter);%>";
	resultPage ="pushApMacFilter.asp";
}
else if(optType=="PapMacFilter")	//MAC地址过滤
{
	waitTime = 10;
	remandTime = 15;
	waitContent = "<%write(waitContentMacFilter);%>";
	resultPage ="pushApMacFilter.asp";
}

var ptr=null;
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
		APREMOTECONF.action=resultPage;
		APREMOTECONF.submit();
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
	APREMOTECONF.passStr.value = passStr;
	APREMOTECONF.ipStr.value= ipStr;
	APREMOTECONF.macStr.value= macStr;
	APREMOTECONF.szName.value= szName;
	APREMOTECONF.managedEn.value= managedEn;
	APREMOTECONF.isGroupOpt.value= isGroupOpt;
	APREMOTECONF.optType.value= optType;
	APREMOTECONF.serialNo.value= serialNo;
	APREMOTECONF.devType.value= devType;
//	alert(APREMOTECONF.serialNo.value);
//	return;
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
    <form name="APREMOTECONF" id="APREMOTECONF" method="post">
    <input type="hidden" name="isGroupOpt">
    <input type="hidden" value="wait" name="parentPage">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="szName">
    <input type="hidden" value="" name="managedEn">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="1" name="waitOver">
    <input type="hidden" value="" name="optType">
    <input type="hidden" value="" name="serialNo">
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
