<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("Syslog");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

<%aspSyslogConf();%>
function init()
{
    var f=Syslog_Config;
    f.SyslogEnable.checked = (SyslogEn == "on") ? true : false;
    f.ServerIp.value = ServerIp;
    if(SyslogPort == 0)
    {
	SyslogPort = 514;
    }
    f.ServerPort.value = SyslogPort;
    if(SyslogType == "")
    {
	SyslogType = "Local0";
    }
    f.SyslogType.value = SyslogType;

    if(SyslogInterval == "")                  
    {                                            
        SyslogInterval = "0";               
    } else {
        HeartBeat.style.display = "";
    }                              
    f.SyslogInterval.value = SyslogInterval;
}
function save()
{
    var f=Syslog_Config;
    if(checkData())
    {
	f.action="/goform/formSyslogConf";
	f.submit();
    }
}
function checkData()
{
    var f = Syslog_Config;
    var ipstr ="0123456789.";
    var flag = 0;
    if (!f.SyslogEnable.checked) 
    {
	return true;
    }
    if(f.ServerIp.value.length > 63)
    {
	alert("<%write(Syslog_Domain_Len);%>")
	return false;
    }
    for (i = 0; i < f.ServerIp.value.length; i++) 
    {
	if(ipstr.indexOf(f.ServerIp.value.substring(i, i + 1)) == -1)
	{
	    flag = 1;
	}
    }
    if(flag == 0)
    {
	if (!checkIP(f.ServerIp, "<%write(Syslog_ServerIp);%>", 0)) 
	{
	    return false;
	}
	if (f.ServerIp.value=="0.0.0.0")
	{ 
	    alert("<%write(ServerIpNotNone);%>");
	    return false
	}
    }
    if((f.ServerPort.value < 1) || (f.ServerPort.value > 65535))
    {
	alert("<%write(Syslog_Port_Num);%>")
	return false;
    }
    return true;
}
function Reset()
{
    Syslog_Config.reset();
    init();
}
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(leftMenud68);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="Syslog_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	<tr>
	   <td align="right" width="50%" height="25"><%write(Syslog_en);%></td>
	   <td  width="2%">&nbsp;</td>
	   <td width="48%" ><input type="checkbox" name="SyslogEnable"  id="SyslogEnable"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(Syslog_ServerIp);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ServerIp" type="text" size="16" maxlength="63"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(Syslog_ServerPort);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td class="helpfont"><input name="ServerPort" type="text" size="16" maxlength="5" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,0,65535,'<%write(Syslog_ServerPort);%>'),yutd5)" value="0"></td>
	   <td  id="yutd5"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(Syslog_MesType);%></td>
	   <td >&nbsp;</td>
	   <td ><select name="SyslogType">
	   <option selected value="Local0">Local0</option>
	   <option value="Local1">Local1</option>
	   <option value="Local2">Local2</option>
	   <option value="Local3">Local3</option>
	   <option value="Local4">Local4</option>
	   <option value="Local5">Local5</option>
	   <option value="Local6">Local6</option>
	   <option value="Local7">Local7</option>
	   </select></td>
	</tr>

    <tr id="HeartBeat" name="HeartBeat" style="display:none">
	   <td align="right" height="25"><%write(Syslog_InterVal);%></td>
	   <td >&nbsp;</td>
	   <td ><select name="SyslogInterval" id="SyslogInterval">
       <option selected value="0">0</option>
       <script language="javascript">
       var sta = 10;
       var obj = document.getElementById("SyslogInterval");
       for(;sta <= 300 ; sta= sta + 10){
           obj.options[obj.length] = new Option( sta,sta);
       }
       </script>
	   </select>&nbsp;<%write(com_second);%></td>
	</tr>

	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	    <td height="30" align="center" valign="top">
	    <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Syslog')">
	    </td>
	   </tr>
	</table>
	</td></tr>
    </table>
    </td></tr>
</table>
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
