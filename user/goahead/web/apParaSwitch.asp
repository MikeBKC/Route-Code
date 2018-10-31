<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("commonWords");%>
<%langTran("apParaSwitch");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
 
//start 交互
<%
	aspOutPutApEffectConf();
//	write('var config = 1;\n');	//1为本地配置、其他为AC下发的配置
//	write('var errorstr = "";\n');
%>
//end交互


function init()
{
	if (config != 1)
	{
		document.getElementById('nonLocal').checked = true;
	}
	else
	{
		document.getElementById('local').checked = true;
	}
}

function save()
{
	confSwitch.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(chooseConfig);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td>
    <form action="/goform/formApEffectConf" name="confSwitch" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td width="102" height="20">&nbsp;</td></tr>
	   <tr>
	     <td colspan="2"><table width="100%"><tr>
	       <td width="31%" height="25" align="right" id="userfuiConfig_td"><%write(userfuiConfig);%></td>
	       <td width="6%" align="right"><input name="conf" type="radio" id="local" value="1" checked></td>
	       <td width="20%" id="localConfig_td"> <%write(localConfig);%></td>
	       <td width="6%" align="right"><input type="radio" name="conf" id="nonLocal" value="0"></td>
	       <td width="37%" id="wireOutConfig_td"><%write(wireOutConfig);%></td>
	       </tr>
	         <tr>
	           <td height="25" colspan="5" align="center" class="helpfont"><%write(rebootTips);%></td>
	           </tr>
	     </table></td>
	     </tr>
	   <tr><td height="20">&nbsp;</td></tr>
<tr><td colspan="2"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td height="30" align="center" valign="top"><input type="submit" class="input-button" value="<%write(save);%>" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<%write(refill);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<%write(help);%>" onClick="window.open('<%write(com_aphelppage);%>#apParaSwitch')" id="utthelp"></td>
  </tr>
</table></td></tr>
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
