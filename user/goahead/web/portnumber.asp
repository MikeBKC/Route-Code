<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

 
var ports = "4";
function save(){
	var f=portsno;
		portsno.submit();
 
}  

function initTranslation()
{
	var e;
 
}

function init()
{
	portsno.portnumbers.value = ports
}

</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("动态端口配置");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="portsno" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
    <td width="46%" align="right" id="portno">最大WAN口数：</td>
  	<td width="1%"></td>
	<td width="52%"><input id="portnumbers"  type="text" maxlength="1" size="3" onChange="showfiledtip(re_checkName(this,'动态端口数'),yutd1)"></td><td width="1%"  id="yutd1"></td>
  </tr>
   
	   <tr><td colspan="4">
  <table width="17%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="bottom"><input type="button" class="input-button" value="保存" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="重填" onClick="init()" id="uttreset"></td>
  </tr>
</table>

  </td></tr>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
 
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td><script>init();</script>
  </tr>
</table>
</body>
</html>
