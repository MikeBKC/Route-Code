<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>���˷�����,���Եȡ���</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//����
var waitTime=3;//�ȴ�ʱ��
var waitContent="���˷�����,���Եȡ���";//�ȴ����ݻ�ȴ���ʾ��Ϣ
var resultPage="topoy.asp";//��ʱ�����תҳ�� 
//end����
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
		form1.action=resultPage;
		form1.submit();
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
    <form name="form1" id="form1" method="post">
<input type=hidden name=show id=show value=yes>
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