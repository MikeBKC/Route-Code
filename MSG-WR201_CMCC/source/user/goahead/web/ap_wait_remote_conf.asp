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
var ipStr=Form("ipStr");//Զ��ip�ִ�
var macStr=Form("macStr");//Զ��mac�ִ�
var passStr=Form("passStr");//Զ�������ִ�
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
	 aspRemoteAPConfSend();	//�·�����
	 }
	else if (optType == "sendAPSoft")
	{
	 aspRemoteApSoftSend();	//�·����
	 }
	else if (optType == "sendAPTemp")
	{
	 aspRemoteApConfTempSend();	//�·�����ģ��
	 }
	else if (optType == "getApConf")
	{
	 aspRemoteConfGet();	//��ȡ����
	 }
	else if (optType == "getApSoft")
	{
	 aspRemoteSoftGet();	//��ȡ���
	 }
	 else if (optType == "joinSZ")
	 {
	 aspRemoteApJoinSZ();	//���������
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
		aspRemoteApMacFiter();//�·�MAC��ַ����
	 }
	 else if (optType == "PapMacFilter")
	 {
		aspApBatchMacFilter();//�����·�MAC��ַ����
	 }

}
%>
var ipStr="<%write(ipStr);%>";//Զ��ip�ִ�
var macStr="<%write(macStr);%>";//Զ��mac�ִ�
var passStr="<%write(passStr);%>";//Զ�������ִ�
var isGroupOpt="<%write(isGroupOpt);%>";
var optType="<%write(optType);%>";
var serialNo="<%write(serialNo);%>";
var devType="<%write(devType);%>";
var szName="<%write(szName);%>";
var managedEn="<%write(managedEn);%>";
//end����
//alert(optType);
if(macStr=="") closeWin(_("<%write(lawless);%>"));
var waitTime=5;//�ȴ�ʱ��
var remandTime=15;
var waitContent;//�ȴ����ݻ�ȴ���ʾ��Ϣ
var resultPage;//��ʱ�����תҳ�� 
if(optType=="sendAPConf") //�·�����
{
	waitContent = _("<%write(outtingConfig);%>");
	resultPage ="pushConf.asp";
}
else if(optType=="sendAPSoft")	//�·����
{
	waitTime = 360;
	remandTime = 380;
	waitContent = _("<%write(outtingSoftware);%>");
	resultPage = "pushSoft.asp";
}
else if(optType=="sendAPTemp")	//�·�����ģ��
{
	waitContent = _("<%write(outtingConfTemp);%>");
	resultPage = "pushTmp.asp";
}
else if(optType=="getApConf")	//��ȡ����
{
	waitContent = _("<%write(gettingConfig);%>");
	resultPage ="getApConf.asp";
}
else if(optType=="getApSoft")	//��ȡ���
{
	waitTime = 40;
	remandTime = 45;
	waitContent = _("<%write(gettingSoft);%>");
	resultPage ="apSoftMng.asp";
}
else if(optType=="joinSZ")	//���������
{
	waitContent = "<%write(waitContent);%>";
	resultPage ="serviceZone.asp";
}
else if(optType=="PjoinSZ")	//���������
{
	waitTime = 20;
	remandTime = 25;
	waitContent = "<%write(waitContentP);%>";
	resultPage ="serviceZone.asp";
}
else if(optType=="apBasicType")	//���������
{
	waitTime = 5;
	remandTime = 10;
	waitContent = "<%write(waitContentBC);%>";
	resultPage ="ap_remote_manage.asp?urcpSet=1";
}
else if(optType=="apMacFilter")	//MAC��ַ����
{
	waitTime = 10;
	remandTime = 15;
	waitContent = "<%write(waitContentMacFilter);%>";
	resultPage ="pushApMacFilter.asp";
}
else if(optType=="PapMacFilter")	//MAC��ַ����
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
