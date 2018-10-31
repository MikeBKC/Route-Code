<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("SaleService"); %>
<% langTran("common"); %>
<% langTran("Link"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

</script>
<style type="text/css">
td{font-size:12px;color:#666666}
A:link { text-decoration: none; color: #000000;}
A:active { text-decoration: underline; color: #000000;}
A:visited { text-decoration: none; color: #000000; }
A:hover { text-decoration: underline; color: #000000;}
</style>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td colspan="3">&nbsp;</td></tr>
  <tr><td width="15"></td>
  <td><script language="javascript">showSecondTitle("<% write(SaleService); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  <tr valign="top">
    <td><form action=""  name="ReBootConfig" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
 <tr><td colspan="3" height="30" id="direction"><font style="font-size:12px; color:#666666">&nbsp; &nbsp; &nbsp; &nbsp;<% write(UttMesage); %></font></td></tr>
  <tr><td colspan="3" height="25" valign="bottom" id="phone"><font style="font-size:14px; color:#666666"><b>&nbsp; &nbsp; &nbsp; &nbsp; <% write(UttPhoneAllC); %></b></font></td></tr>
 <tr><td colspan="3" height="20">&nbsp;</td></tr>
 <tr><td colspan="3" height="20"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td width="24%" height="35"><img src="<% write(com_imgseruttcare); %>" width="120"  height="24"></td>
	<td width="8"></td>
	<td width="24%" align="center"><img src="<% write(com_imgserbbs); %>" width="120"  height="24"></td>
	<td width="8"></td>
	<td width="24%" align="center"><img src="<% write(com_imgserlib); %>" width="120"  height="24"></td>
	<td width="8"></td>
	<td width="24%" align="center"><img src="<% write(com_imgserbook); %>" width="120"  height="24"></td>
  </tr>
  <tr>
    <td valign="top" ><table width="100%" border="0" cellspacing="0" cellpadding="0" align="c">
<tr><td height="25" id="line11">&nbsp;<% write(UttcareLine1); %></td></tr>
<tr><td height="25" id="line12">&nbsp;<% write(UttcareLine2); %></td></tr>
<tr><td height="25" id="line13">&nbsp;<% write(UttcareLine3); %></td></tr>
<tr><td height="25" id="line14">&nbsp;<% write(UttcareLine4); %></td></tr>
<tr><td height="35" align="right" valign="bottom" id="more1"><a href="<% write(LinkUttcare); %>" target="_blank"><% write(TOKnowMore); %></a></td></tr>
</table>
      
	<td width="8"><img src="<% write(com_imgserline); %>" width="3" height="132"></td>
	<td valign="top" ><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td height="25" id="line21">&nbsp;<% write(ProductTopic1); %></td></tr>
<tr><td height="25" id="line22">&nbsp;<% write(ProductTopic2); %></td></tr>
<tr><td height="25" id="line23">&nbsp;<% write(ProductTopic3); %></td></tr>
<tr><td height="25" id="line24">&nbsp;<% write(ProductTopic4); %></td></tr>
<tr><td height="35" align="right" valign="bottom" id="more2"><a href="<% write(LinkForum); %>" target="_blank"><% write(TOKnowMore); %></a></td></tr>
</table>
</td>
	<td width="8"><img src="<% write(com_imgserline); %>" width="3" height="132"></td>
	<td valign="top"  ><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td height="25" id="line31">&nbsp;<% write(KnowledgeBase1); %></td></tr>
<tr><td height="25" id="line32">&nbsp;<% write(KnowledgeBase2); %></td></tr>
<tr><td height="25" id="line33">&nbsp;<% write(KnowledgeBase3); %></td></tr>
<tr><td height="25" id="line34">&nbsp;<% write(KnowledgeBase4); %></td></tr>
<tr><td height="35" align="right" valign="bottom" id="more3"><a href="<% write(LinkKnowLedge); %>" target="_blank"><% write(TOKnowMore); %></a></td></tr>
</table>
</td>
	<td width="8"><img src="com_imgserline" width="3" height="132"></td>
	<td valign="top"  ><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td height="25" id="line41">&nbsp;<% write(ReservationSer1); %></td></tr>
<tr><td height="25" id="line42">&nbsp;<% write(ReservationSer2); %></td></tr>
<tr><td height="25" id="line43">&nbsp;<% write(ReservationSer3); %></td></tr>
<tr><td height="25" id="line44">&nbsp;<% write(ReservationSer4); %></td></tr>
<tr><td height="35" align="right" valign="bottom" id="more4"><a href="<% write(LinkReservation); %>" target="_blank"><% write(TOKnowMore); %></a></td></tr>
</table>
</td>
  </tr>
</table>
</td></tr>
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
