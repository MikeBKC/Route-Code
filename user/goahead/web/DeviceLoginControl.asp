<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DeviceLoginControl"); %>
<% langTran("left"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%aspOutDeviceLoginControl();%>
/*var errorstr="";
var SSHEnables = "1";
var SSHPorts = "22";*/

function save()
{
    var f=DeviceLoginControl_Config;
    if(SSH==1)
    {
	if(! checkNumber(f.SSHPort,1,65535,"<% write(SSHPortRange); %>",0)) return; 
	f.SSHEnable.value=(f.SSHEnablew.checked==true?"on":"off");
    }
    if(Https==1)
    { 
	f.HttpsEnable.value=(f.HttpsEnablew.checked==true?"on":"off");
    }
    DeviceLoginControl_Config.action ="/goform/formDeviceLoginControl";
    DeviceLoginControl_Config.submit();
}
function init()
{
    var f=DeviceLoginControl_Config;
    if(SSH==1)
    {
	document.getElementById('enabledSSH').style.display="";
	f.SSHEnablew.checked=(SSHEnables.toLowerCase()=="1"?true:false);
	f.SSHPort.value=SSHPorts;
    }
    if(Https==1)
    {
	document.getElementById('enabledHttps').style.display="";
	f.HttpsEnablew.checked=(httpsEnables.toLowerCase()=="1"?true:false);
    }
}
</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("<% write(DeviceLoginControl); %>");</script>
<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr);</script>
<tr valign="top">
<td><form action=""  name="DeviceLoginControl_Config" method="post" >
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
<td align="center"  class="style7">
<table border="0" width="600" align='center'>
<tr><td colspan="3" height="30">&nbsp;</td></tr>
<tr style="display:none" id="enabledHttps">
<td width="45%" align="right" id="HttpsEn"><% write(HttpsEn); %></td>
<td width="2%"></td>
<td><input  type="checkbox" name="HttpsEnablew"><input type="hidden" name="HttpsEnable"></td>
</tr>
<tr style="display:none" id="enabledSSH">
<td width="45%" align="right" id="SSHEn"><% write(SSHEn); %></td>
<td width="2%"></td>
<td width="4%"><input  type="checkbox" name="SSHEnablew"><input type="hidden" name="SSHEnable"></td>
<td width="7%" align="right" id="SSHPort"><% write(Port); %></td>
<td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
<td width="10%"><input type="text" size="7" name="SSHPort" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,'1','65535','<% write(Port); %>'),yutd1)" maxlength="5"></td>
<td id="yutd1"></td>
</tr>
<tr><td colspan="7" height="5">&nbsp;</td></tr>
<tr><td colspan="7">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
<td height="30" align="center" valign="top">
<input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">
&nbsp;&nbsp;&nbsp;
<input type="button" class="input-button" value="<% write(com_reset); %>"  onClick="init()" id="uttreset">
&nbsp;&nbsp;&nbsp;
<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DeviceLoginControl')">
</td>
</tr>
<script language="javascript">init();</script>
</table>
</td></tr>
</table></td></tr></table>
</form></td>
</tr>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
