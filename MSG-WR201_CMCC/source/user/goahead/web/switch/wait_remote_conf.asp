<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>����������,���Եȡ���</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<%
var ipStr=Form("ipStr");//Զ��ip�ִ�
var macStr=Form("macStr");//Զ��mac�ִ�
var passStr=Form("passStr");//Զ�������ִ�
var isGroupOpt=Form("isGroupOpt");
var optType=Form("optType");
if (macStr!="")
{
	if (optType == "sendConf") aspRemoteConfSend();//�����·�
	else if (optType == "getConf") aspRemoteConfGet();//���û�ȡ
	else if (optType == "getEventslog") aspRemoteEventslogGet();//���û�ȡ
}
%>
var ipStr="<%write(ipStr);%>";//Զ��ip�ִ�
var macStr="<%write(macStr);%>";//Զ��mac�ִ�
var passStr="<%write(passStr);%>";//Զ�������ִ�
var isGroupOpt="<%write(isGroupOpt);%>";
var optType="<%write(optType);%>";
//end����
if(macStr=="") closeWin("�Ƿ�������");
var waitTime=5;//�ȴ�ʱ��
var waitContent;//�ȴ����ݻ�ȴ���ʾ��Ϣ
var resultPage;//��ʱ�����תҳ�� 
if(optType=="sendConf") 
{
	waitContent = "�����·���,���Եȡ���";
	resultPage ="linkageConfSend.asp";
	
}
else if(optType=="getConf")
{
	waitContent = "���û�ȡ��,���Եȡ���";
	resultPage ="linkageConf.asp";
}
else if(optType=="getEventslog")
{
	waitContent = "��־��ȡ��,���Եȡ���";
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
		document.getElementById('waitContent').innerHTML="���ӳ�ʱ�����������������ӡ���";
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
   <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
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
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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
   <!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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
