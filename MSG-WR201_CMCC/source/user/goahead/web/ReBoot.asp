<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("ReBoot");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

//Butterlate.setTextDomain("admin");

function save(){
	var f=ReBootConfig;
	if (confirm("<%write(reBootTip);%>"))
	{
		ReBootConfig.action ="/goform/formRebootMachine";
		ReBootConfig.submit();
	}
}

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("Restart");
//	e.innerHTML= _("Restart");
//	e = document.getElementById("resetbutton");
//	e.value= _("resetbutton");
	//e = document.getElementById("ps");
	//e.innerHTML= _("reset ps");
//}

</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_reStart);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="ReBootConfig" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
    <td width="47%" align="right" id="Restart"><%write(reStart);%></td>
  	<td width="1%"></td>
	<td width="52%"><input id="resetbutton" type="button" class="input-button" value="<%write(reBoot);%>" onClick="save()"></td>
  </tr>
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <input type="hidden" value="1" name="re">
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
