 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>ϵͳ״̬-ϵͳ��Ϣ</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//����
<%
var ipStr=Form("ipStr");//Զ��ip�ִ�
var macStr=Form("macStr");//Զ��mac�ִ�
var passStr=Form("passStr");//Զ�������ִ�
if (macStr=="")
{
	aspOupPutGetSysInfo();//����
	write('var urcpError="";');
}
else 
{
	aspOupPutGetSysInfoUrcp();//����
	write('var urcpError="";');
}
%>
var ipStr="<%write(ipStr);%>";//Զ��ip�ִ�
var macStr="<%write(macStr);%>";//Զ��mac�ִ�
var passStr="<%write(passStr);%>";//Զ�������ִ�
var dhcpServer = "0.0.0.0";
var dhcpLeaseTime = 10; //dhcp����ʱ��ʣ��(��λsec)
//end

//����ʱ�䴦��
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
	Month1=Month+1;//·�����ڲ�0��ʾ1�£�1��ʾ2�¡���
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
		if(!(parseInt(outerFlashTotalSpace)))/*�ܿռ��СΪ0*/
		{
			outerFlashTotalSpace =false;
		}
	}
	if(outerFlashTotalSpace)
	{
		/*ʹ����*/
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
		outerFlash.parentNode.title = "���ÿռ䣺"+byteFormat(outerFlashUsedSpace)+"\n�ܴ�С��"+byteFormat(outerFlashTotalSpace);
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

//end����
</script>
<style type="text/css">
.style24 {border:1px solid #408080}
</style>
</head>
<body onLoad="pageInit()" onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">showSecondTitle('ϵͳ״̬-ϵͳ��Ϣ');//��ʾҳ������</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form method="post" action="sysInfo.asp"  name="SYSINFO">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="sysTime">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<br>
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr><td height="5" colspan="2"></td></tr>
<tr><td width="284"   align="left"><b>ϵͳ��Դ״̬</b></td>
  <td width="316"   align="right">
    <input type="button" id="helpButton" name="helpButton" class="input-button" value="����" onClick="window.open('../rhelpinfo.htm#sysinfo')">
    <input type="button" class="input-button" value="ˢ��" onClick="SYSINFO.submit()">
    <input type="button" class="input-button" style="display:none" name="goBack" value="����" onClick="go_back();">
  </label></td>
</tr>
<tr><td height="10" colspan="2"></td></tr>
</TABLE>
<table width="600" border="0" align="center"  cellpadding="4" cellspacing="1" class="style22" >
    <tr class="style18">
	<td width="22%" valign="middle" height="22" id="cpu" >&nbsp;CPUռ��</td>
	<td width="78%" height="22">
	  <table class="style24" width="135" align="left" bgcolor="#FFFFFF">
	    <tr><td id="cpu1" height="15" width="1%"></td><td></td></tr>
	      </table>
	  <script>
	document.write(cpuStat.toString() + "%");
	  </script>	  &nbsp;	</td>
</tr>
	 <tr>
	<td width="22%" valign="middle" height="22" id="neicun">&nbsp;�ڴ�ʹ��</td>
	<td height="22">
	  <table class="style24" width="135" align="left" bgcolor="#FFFFFF">
	    <tr><td id="mem" height="15" width="1%"></td><td ></td></tr>
	      </table>
	  <script>
	document.write(memStat.toString() + "%");
	  </script>	  &nbsp;	</td>
</tr>
	 <tr class="style18" id="trOuterFlash">
	   <td valign="middle" height="22" id="neicun2">&nbsp;U��ʹ��</td>
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
<tr><td   align="left"><b>ϵͳ������Ϣ</b></td></tr>
<tr height="10"><td></td></tr>
</TABLE>
	<TABLE  width="600"  border="0"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18">
<td class=vncell height="10" width="22%">ϵͳ��ǰʱ��</td>
<td class=vtable>	<script>document.write(systimes);</script></td>
</tr>
<tr>
  <td class=vncell height="5">ϵͳ����ʱ��</td>
  <td class=vtable><script>document.write(upTime);</script></td>
</tr>
<tr class="style18">
  <td class=vncell height="5">�豸��</td>
  <td class=vtable><script>document.write(deviceName);</script></td>
</tr>
<tr  >
<td class=vncell height="22" width="22%">���к�</td>
<td class=vtable><script>document.write(sysSerial);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">����汾</td>
<td class=vtable><script>document.write(sysVersion);</script></td>
</tr>
<tr  >
<td class=vncell height="22" width="22%">MAC��ַ</td>
<td class=vtable><script>document.write(sysMac);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%">IP��ַ</td>
<td class=vtable><script>document.write(sysIp);</script></td></tr>
<tr  >
<td class=vncell height="22" width="22%">��������</td>
<td class=vtable><script>document.write(sysMask);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="10" width="22%">����</td>
<td class=vtable><script>document.write(sysGw);</script></td></tr>
<tr>
  <td class=vncell height="11">CPU VLAN ID</td>
  <td class=vtable><script>document.write(vlanId);</script></td>
</tr>
<!--
<tr class="style18">
<td class=vncell height="22" width="22%">DHCP������</td>
<td class=vtable><script>document.write(dhcpServer);</script></td>
</tr>
<tr>
<td class=vncell height="22" width="22%">����ʱ��ʣ��</td>
<td class=vtable><script>document.write(dhcpLeaseTime);</script> ����</td>
</tr>
-->
</table>
</form>
<br><br>
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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
