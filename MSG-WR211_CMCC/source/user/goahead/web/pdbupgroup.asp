<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<META http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<title>艾泰科技</title>
<meta HTTP-EQUIV="refresh" CONTENT="10;url=/Group.asp">
<script type="text/javascript" src="js/common.js"></script>
</head>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("admin");
function initTranslation()
{
	var e;

	e = document.getElementById("a4");
	e.innerHTML= "&nbsp;&nbsp;&nbsp;&nbsp;"+_("a4");
	e = document.getElementById("a2");
	e.innerHTML= _("a2");
	e = document.getElementById("a3");
	e.innerHTML= _("a3");
}
	function timeLeft() {
	  var dd = document.getElementById("reloadId").value;
          document.getElementById("reloadId").value--;
	  if(dd <=0 )
	  {
	    clearInterval(timename);
            document.getElementById("reloadId").value=0;
	  }
	}
       timename=setInterval('timeLeft();',1000);
 </script>
<style>
<!--
.style3 {FONT-WEIGHT: normal; FONT-SIZE: 12px; COLOR: #000000; FONT-FAMILY: Arial, Helvetica, sans-serif}
.time2{ border:0px solid #adafae;height:16px;color:#0099FF;FONT-FAMILY: "Arial", "Helvetica", "sans-serif";FONT-SIZE: 12px;width:14px}
-->
</style>
<body onLoad=""onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("更新策略");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form  method="post"  name="form1">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
     <td height="25" align="center" class="style3" id="a4">&nbsp;&nbsp;&nbsp;&nbsp;正在更新策略，请稍候...</td>
  </tr>
  <tr>
	      <td height="28" align="center" class="style3"><table width="99" height="25"  border="0" cellpadding="0" cellspacing="0">
		    <tr>
		      <td width="64" class="style3" id="a2">剩余时间为</td>
		      <td width="20"><input name="textfield" id="reloadId" type="text" class="time2" value="10" size="2" readonly></td>
		      <td width="15" class="style3" id="a3">秒! </td>
		    </tr>
		</table></td>
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
