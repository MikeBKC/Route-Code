<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("acMode");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">


//start 交互
/*
var apMode = 1;	//1为胖AP，0为瘦AP。
var errorstr = "";
*/
//end交互
<%
	aspOutPutAcWorkMode();
%>
function selMode(val)
{
	var f=acWorkMode;
	if(val == "0")
	{
		gateWay.style.display="none";
		dns.style.display="none";
	}else if (val == "1")
	{
		gateWay.style.display=dis;
		dns.style.display=dis;
	}
	else{
		gateWay.style.display="none";
		dns.style.display="none";
	}

}

function init()
{
	var f = acWorkMode;
	if (acMode == 1)
	{
		document.getElementById('acMode').value = 1;
	}
	else
	{
		document.getElementById('acMode').value = 0;
	}
	f.acGateWay.value=ac_gateWay; 
	f.acDns.value=ac_Dns;
	selMode(acMode);
}
function save()
{
	var f = acWorkMode;
	if(f.acMode.value == "1"){
	if (!checkIP(f.acGateWay,"<%write(acGateWayAddr);%>",0)) return false;
	if (!checkMainDns(f.acDns,"<%write(acDnsSer);%>",0)) return false;
	}
	acWorkMode.submit();
}
</script>
</head>
<body onbeforeunload="logOut()"  style="background:url(images/body_main.png);  background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(workMode);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/formAcWorkMode" name="acWorkMode" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td width="102" height="20">&nbsp;</td></tr>
	   <tr>
	     <td colspan="4"><table border="0" width="100%"><tr>
	       <td width="40%" align="right" height="25" id="workMode_td"><%write(workMode);%></td>
	       <td width="3%">&nbsp;</td>
	       <td width="26%"><select name="acMode" id="acMode" class="list" onChange="selMode(this.value)" >
		     <option value="0" id="route_m"><%write(gwMode);%></option> 
	         <option value="1" id="bypass_m"><%write(bypassMode);%></option>
	         </select></td>
			 <td width="31%"></td>
	       </tr>
			<tr id="gateWay" name="gateWay" style="display:none">
				<td align="right"><%write(acGateWayAddr);%></td>
				<td></td>
				<td height="20"><input name="acGateWay" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(acGateWayAddr);%>'),yutd1)" maxlength="15"></td><td id="yutd1"></td>
			</tr>
			<tr id="dns" name="dns" style="display:none">
				<td align="right"><%write(acDnsSer);%></td>
				<td></td>
				<td height="20"><input name="acDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(acDnsSer);%>'),yutd2)" maxlength="15"></td><td id="yutd2"></td>
			</tr>
            <tr>
                <td align="center" colspan="4"><b>注意：切换模式后需重启设备才能生效！</b></td>
		   </tr>
	     </table></td>
	     </tr>
	   <tr><td height="20">&nbsp;</td></tr>
<tr><td colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<%write(refill);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<!--<input type="button"  class="input-button" value="<%write(help);%>" onClick="window.open('FitApHelpInfo.htm#fatFitSwitch')" id="utthelp">--></td>
  </tr>
</table></td></tr>
<input type="hidden"  name="wrlesMode_chanWid"><input type="hidden" name="wds_list">
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
