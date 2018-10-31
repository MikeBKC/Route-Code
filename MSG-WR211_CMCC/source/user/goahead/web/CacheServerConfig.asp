<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("CacheConfig");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

<%aspCacheServerConf();%>
function init()
{
    var f=cacheConfig;
    f.cacheServerEnable.checked = (cacheServerEnable == "on") ? true : false;
    f.cacheServerIp.value = cacheServerIp;
    if(cacheServerPort == 0)
    {
	cacheServerPort = 3128;
    }
    f.cacheServerPort.value = cacheServerPort;
    
}
function save()
{
    var f=cacheConfig;
    if(checkData())
    {
	f.action="/goform/formCacheServerConf";
	f.submit();
    }
}
function checkData()
{
    var f = cacheConfig;
    var ipstr ="0123456789.";
    var flag = 0;
    if (!f.cacheServerEnable.checked) 
    {
	return true;
    }
    if(f.cacheServerIp.value.length > 63)
    {
	alert("<%write(CacheServerDomainLen);%>")
	return false;
    }
    for (i = 0; i < f.cacheServerIp.value.length; i++) 
    {
	if(ipstr.indexOf(f.cacheServerIp.value.substring(i, i + 1)) == -1)
	{
	    flag = 1;
	}
    }
    if(flag == 0)
    {
	if (!checkIP(f.cacheServerIp, "<%write(CacheServerIp);%>", 0)) 
	{
	    return false;
	}
	if (f.cacheServerIp.value=="0.0.0.0")
	{ 
	    alert("<%write(CacheServerIpNotNone);%>");
	    return false
	}
    }
    if((f.cacheServerPort.value < 1) || (f.cacheServerPort.value > 65535))
    {
	alert("<%write(CacheServerPortNum);%>")
	return false;
    }
    return true;
}
function Reset()
{
    cacheConfig.reset();
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
  <td><script language="javascript">showSecondTitle('<%write(leftMenud82);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="cacheConfig" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	<tr>
	   <td align="right" height="25"><%write(CacheServerEn);%></td>
	   <td >&nbsp;</td>
	   <td ><input type="checkbox" name="cacheServerEnable"  id="cacheServerEnable"/></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(CacheServerIp);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="cacheServerIp" type="text" size="16" maxlength="63"></td>
	</tr>
		<tr>
			<input type="hidden" name="cacheServerPort" value="3128"/>
		<td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	    <td height="30" align="center" valign="top">
	    <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#CacheConfig')">
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
