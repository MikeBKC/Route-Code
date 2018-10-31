<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
 <% langTran("SysInfo");%>
 <% langTran("common");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<% outputSystemNowTime();%>
<% outputSystemVersion();%>
<% outputSystemUpTime();%>
<% outputSystemCpu();%>
<% outputSystemMemory();%>
<% outputFlashStat();%>
<% outputSystemSn();%>
<% outputProductID();%>

if (cpuStat < 0) {
	cpuStat	 = 0;
}
if (memStat <0) {
	memStat = 0;
}

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
var outerFlash=null;
var outerFlashStat=0;
var outerFlashUsedSpace=0;
var http_request = false;
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

function pageInit()
{
	outerFlash = document.getElementById('outerFlash');
	var flag1 = false;
	var flag2 = false;
	var flag3 = false;
	/* 控制div高 */
	if (outerFlashTotalSpace || (hardwareID != "")) {
		HisInfo1.style.height = "122";
		HisInfo2.style.height = "122";
	} else {
		HisInfo1.style.height = "92";
		HisInfo2.style.height = "92";		
	}
	if (hardwareID != "") {
		hwmodeltr.style.display = dis;
	}

	if(outerFlashTotalSpace)
	{
		document.getElementById("trOuterFlash").style.display = dis;
	}
	else
	{
		outerFlash.parentNode.title = "<%write(UsedSpace);%>"+byteFormat(outerFlashUsedSpace)+"\n<%write(TotalSize);%>"+byteFormat(outerFlashTotalSpace);
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
<body onLoad="pageInit()" onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(SysInfo); %>");</script>
  <table width="100%" border="0"  cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form name="SysStatus_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7"><table border="0" width="620" align='center'>
  <tr><td height="15" colspan="3">&nbsp;</td></tr>
		   
  <tr><td height="214" colspan="3"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
  <td colspan="2" width="400"><table border="0" cellspacing="0" cellpadding="0" width="100%">
  <tr>
	<td width="<%write(sysinfo_width1);%>" valign="middle" height="30" id="nowtime">&nbsp;<% write(systemnow); %>:</td>
	<td width="3%"></td><td height="30" align="left">
	<script>
	document.write(systimes);
	</script>	</td>
	</tr>
	 <tr>
	<td  valign="middle" height="30" id="lasttime">&nbsp;<% write(lasttime); %>:</td>
	<td></td><td  height="30" align="left">
	<script>
	document.write(runtimes);
	</script>
	</td>
	</tr>
</table>
</td>
 
  <td valign="bottom"><table border="0" cellspacing="0" cellpadding="0" width="245"><tr><td align="right"  height="25"><input type="button" class="input-button" value="<% write(com_fresh); %>" onClick="location.reload()" id="update"></td></tr></table></td>
  </tr>
  <tr><td colspan="3" align="center" height="20"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<% write(com_imgdot); %>"><img src="<% write(com_imgspace); %>" width="8" height="1"></td>
  </tr>
</table>
</td></tr><tr><td colspan="3" height="20" id="some"><strong><% write(VerAndRes); %>:</strong></td></tr>
  <tr>
    <td height="93" align="right"><div class="styleDiv" align="left" id="HisInfo1" style=" height:122;width:245;">
	<table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr>
	<td width="27%" valign="middle" height="35" id="cpu">&nbsp;<% write(CPUusage); %>:</td>
	<td width="3%"></td><td width="58%" height="30" align="left">
	<table class="style24" width="135" align="left">
	<tr><td id="cpu1" height="15" width="1%"></td><td ></td></tr>
	</table></td>
	<td width="12%">
	<script>
	document.write(cpuStat.toString() + "%");
	</script>&nbsp;	</td>
	</tr>
	 <tr>
	<td width="27%" valign="middle" height="35" id="neicun">&nbsp;<% write(memusage); %>:</td>
	<td width="3%"></td><td width="58%" height="30" align="left">
	<table class="style24" width="135" align="left">
	<tr><td id="mem" height="15" width="1%"></td><td ></td></tr>
	</table></td>
	<td width="12%">
	<script>
	document.write(memStat.toString() + "%");
	</script>&nbsp;	</td>
	</tr>
	 <tr id="trOuterFlash"  style="display:none">
	<td width="27%" valign="middle" height="35" id="flash">&nbsp;<% write(Usbfdusage); %>:</td>
	<td width="3%"></td><td width="58%" height="30" align="left">
	<table class="style24" width="135" align="left">
	    <tr><td id="outerFlash" height="15" width="1%"></td><td ></td></tr>
	</table></td>
	<td width="12%">
	<script>
	document.write(outerFlashStat.toString() + "%");
	</script>&nbsp;	</td>
	</tr>
</table>
</div></td>
<td width="50"></td>
<td ><div class="styleDiv" align="left" id="HisInfo2" style="height:122; width:245">
	<table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr>
	<td width="25%" valign="middle" height="30" id="SN" align="right">&nbsp;<% write(SerNum); %>:</td>
	<td width="5%"></td>
	<td width="70%"  height="30" align="left">
	<script>
	document.write(productIDs);
	</script>
	</td>
	</tr>
	<tr>
	<td width="25%" valign="middle" height="30" id="product" align="right">&nbsp;<% write(ProMod); %>:</td>
	<td width="5%"></td>
	<td  height="30" align="left">
<script>
	document.write(ProductID);
	</script>
	</td>
	</tr>	
	<tr id="hwmodeltr" style="display:none">
	<td width="25%" valign="middle" height="30" id="hwmodel" align="right">&nbsp;<% write(HardVer); %>:</td>
	<td width="5%"></td>
	<td  height="30" align="left">
<script>
	document.write(hardwareID);
	</script>
	</td>
	</tr>
	 <tr>
	<td width="25%" valign="middle" height="30" id="revision" align="right">&nbsp;<% write(SofVer); %>:</td>
	<td width="5%"></td>
	<td  height="30" align="left">
<script>
	document.write(revisions);
	</script>
	</td>
	</tr>
	

</table>
  <tr><td height="15" colspan="5">&nbsp;</td></tr>
</div></td>
  </tr>
</table>
</td></tr>
        
			 
</table></td>
</tr>
</table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
	      
