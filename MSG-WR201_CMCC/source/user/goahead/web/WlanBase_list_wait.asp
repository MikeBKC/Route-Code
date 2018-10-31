<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("WlanBase");%>
<title><%write(wait);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
<%
	ApScan5G();
%>

var waitTime=5;//等待时间
var remandTime=8;
var waitContent;//等待内容或等待提示信息
var resultPage;//到时候的跳转页面 
waitContent = _("<%write(scanning);%>");
resultPage ="WlanBase_list.asp";
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
		AP_List.action=resultPage;
		AP_List.submit();
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
	AP_List.WrlessEnable.value="<% write(Form('WrlessEnable'));%>";
	AP_List.ssid.value="<% write(Form('ssid'));%>";
	AP_List.APWorkMode.value="<% write(Form('APWorkMode'));%>";
	AP_List.APssid.value="<% write(Form('APssid'));%>";
	AP_List.APCliMacs.value="<% write(Form('APCliMacs'));%>";
	AP_List.APMac0.value="<% write(Form('APMac0'));%>";
	AP_List.APMac1.value="<% write(Form('APMac1'));%>";
	AP_List.APMac2.value="<% write(Form('APMac2'));%>";
	AP_List.APMac3.value="<% write(Form('APMac3'));%>";

	if(AP_List.APMac0.value =="") 
	AP_List.APMac0.value= "000000000000";
	if(AP_List.APMac1.value =="") 
	AP_List.APMac1.value= "000000000000";
	if(AP_List.APMac2.value =="") 
	AP_List.APMac2.value= "000000000000";
	if(AP_List.APMac3.value =="") 
	AP_List.APMac3.value= "000000000000";

	AP_List.wdsSecModes.value="<% write(Form('apclisecmode'));%>";
	AP_List.wdspskPsswds.value="<% write(Form('wdspskPsswd'));%>";

	/* */
	AP_List.wdskeynums.value="<% write(Form('wdskeynums'));%>";

	AP_List.wdswepkey1s.value="<% write(Form('apcliwepkey1'));%>";
	AP_List.wdswepkey2s.value="<% write(Form('apcliwepkey2'));%>";
	AP_List.wdswepkey3s.value="<% write(Form('apcliwepkey3'));%>";
	AP_List.wdswepkey4s.value="<% write(Form('apcliwepkey4'));%>";
	AP_List.wdskeyFormats.value="<% write(Form('apclikeyFormat'));%>";
	AP_List.wdskeyLength1s.value="<% write(Form('apclikeyLength1'));%>";
	AP_List.wdskeyLength2s.value="<% write(Form('apclikeyLength2'));%>";
	AP_List.wdskeyLength3s.value="<% write(Form('apclikeyLength3'));%>";
	AP_List.wdskeyLength4s.value="<% write(Form('apclikeyLength4'));%>";

	AP_List.wrlessMode.value="<% write(Form('wrlessMode'));%>";
	AP_List.chanWidth.value="<% write(Form('chanWidth'));%>";
	AP_List.vhtChanWidth.value="<% write(Form('vhtChanWidth'));%>";
	AP_List.channel.value="<% write(Form('channel'));%>";
	AP_List.broadcastEnable.value="<% write(Form('broadcastEnable'));%>";
	AP_List.wdsEnabled.value="1";
	AP_List.bridgessid.value="<% write(Form('bridgessid'));%>";
	AP_List.bridgebssid.value="<% write(Form('bridgebssid'));%>";
	AP_List.keytype.value="<% write(Form('keytype'));%>";
	AP_List.wepkeynum.value="<% write(Form('wepkeynum'));%>";
	AP_List.certificatetype.value="<% write(Form('certificatetype'));%>";
	AP_List.key.value="<% write(Form('key'));%>";
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
	<td ><!--<form name="AP_List" method="post" action="">-->
    <td>
	<table border="0" width="80%" align='center'><tr><td align="center">
	<form  name="AP_List" method="post"><tr><td></tr></td>
    <input type="hidden" name="AP_LIST" value="aplist"/>
    <input type="hidden" name="WrlessEnable" value=""/>
    <input type="hidden" name="ssid" value=""/>
    <input type="hidden" name="APWorkMode" value=""/>
    <input type="hidden" name="wrlessMode" value=""/>
    <input type="hidden" name="channel" value=""/>
    <input type="hidden" name="chanWidth" value=""/>
    <input type="hidden" name="vhtChanWidth" value=""/>
    <input type="hidden" name="broadcastEnable" value=""/>
    <input type="hidden" name="wdsEnabled" value=""/>
    <input type="hidden" name="APssid" value=""/>
    <input type="hidden" name="APCliMacs" value=""/>
    <input type="hidden" name="APMac0" value=""/>
    <input type="hidden" name="APMac1" value=""/>
    <input type="hidden" name="APMac2" value=""/>
    <input type="hidden" name="APMac3" value=""/>
    <input type="hidden" name="wdsSecModes" value=""/>
    <input type="hidden" name="wdspskPsswds" value=""/>
    <input type="hidden" name="wdskeynums" value=""/>
    <input type="hidden" name="wdswepkey1s" value=""/>
    <input type="hidden" name="wdswepkey2s" value=""/>
    <input type="hidden" name="wdswepkey3s" value=""/>
    <input type="hidden" name="wdswepkey4s" value=""/>
    <input type="hidden" name="wdskeyFormats" value=""/>
    <input type="hidden" name="wdskeyLength1s" value=""/>
    <input type="hidden" name="wdskeyLength2s" value=""/>
    <input type="hidden" name="wdskeyLength3s" value=""/>
    <input type="hidden" name="wdskeyLength4s" value=""/>

    <input type="hidden" name="bridgessid" value=""/>
     <input type="hidden" name="bridgebssid" value=""/>
    <input type="hidden" name="keytype" value=""/>
    <input type="hidden" name="wepkeynum" value=""/>
    <input type="hidden" name="certificatetype" value=""/>
    <input type="hidden" name="key" value=""/>
    
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
