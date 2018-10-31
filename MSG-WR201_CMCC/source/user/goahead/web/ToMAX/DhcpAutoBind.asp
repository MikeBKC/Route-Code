<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DHCP"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<% write(DHCPAutoBind); %>";
<%getDhcpAutoBind();%>

/******************list***********************/

function init()
{
    if(DhcpNewAutobind == 1)
    {
	DHCPAutobindConfig.chkDhcpNewAutobind.checked = true;
    }
    else
    {
	DHCPAutobindConfig.chkDhcpNewAutobind.checked = false;
    }
    if(DhcpDelAutobind == 1)
    {
	DHCPAutobindConfig.chkDhcpDelAutobind.checked = true
    }
    else
    {
	DHCPAutobindConfig.chkDhcpDelAutobind.checked = false;
    }
}
function save()
{
    DHCPAutobindConfig.action = "/goform/formDhcpAutoBindConfig";
    DHCPAutobindConfig.submit();
}
function Reset()
{
    DHCPAutobindConfig.reset();
    init();
}
-->
</script>
</head>
<body onbeforeunload="logOut()" >
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("<% write(DHCP); %>");</script>
<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form name="DHCPAutobindConfig" method="post" action="/goform/formDhcpListStaticDel">
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"95%",
	4,
	new Array("<% write(DHCPset); %>",'DhcpServer.asp',1,0),
	new Array("<% write(StaticDHCP); %>",'DhcpFix.asp',1,0),
	new Array("<% write(DHCPAutoBind); %>",'DhcpAutoBind.asp',1,1),
	new Array("<% write(DHCPClient); %>",'DhcpClient.asp',1,0)
	);
showThreeMenu();
</script>
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" height="250">

<tr>

<td align="center" class="three_td" valign="top">

<table border="0" width="100%" align='center'>

<tr><td colspan="4" height="30">&nbsp;</td></tr> 

<tr>
<td width="53%" align="right" height="20" id="pppoeset"><% write(DHCPAutoBindEn); %></td>
<td width="3%">&nbsp;</td>
<td colspan="2" height="20">
<input type="checkbox" name="chkDhcpNewAutobind" id="chkDhcpNewAutobind">
</td>
</tr>

<tr>
<td width="53%" align="right" height="20" id="pppoeset"><% write(DHCPAutoDelEn); %></td>
<td width="3%">&nbsp;</td>
<td colspan="2" height="20">
<input type="checkbox" name="chkDhcpDelAutobind" id="chkDhcpDelAutobind" >
</td>
</tr>

<tr><td colspan="4">

<table width="30%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
<td height="30" align="center" valign="top">
<input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;
<input type="button" class="input-button" value="<%write(com_reset);%>"	onClick="Reset()" id="uttreset"> &nbsp;&nbsp;&nbsp;
<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DhcpAutoBind')">&nbsp;&nbsp;&nbsp;
</tr>

</table>

</td></tr>

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
