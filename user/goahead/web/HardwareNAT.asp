<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("HardwareNAT");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

<%aspOutHardwareNAT();%>

function save(){
    formHardwareNAT.submit();
}  

function init()
{
    var f=formHardwareNAT;
    f.HardwareNAT.checked=(hardwareNAT ? true:false);
}

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
  <td><script language="javascript">showSecondTitle("<%write(HardwareNAT);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/ConfigHardwareNAT"  name="formHardwareNAT" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
		<td width="46%" align="right" height="25" id="NATspeedup"><%write(NATspeedup);%></td>
		<td width="1%">&nbsp;</td>
		<td width="53%" height="20"><input name="HardwareNAT" type="checkbox" ></td>
	      </tr>
	   <tr><td colspan="4">
  <table width="21%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="bottom"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave"> &nbsp;&nbsp;&nbsp;</td>
  </tr>
</table>

  </td></tr>
 <tr><td colspan="3" height="40">&nbsp;</td></tr>
 
  <tr>
	<td colspan="4" align="center" class="helpfont" height="35" valign="bottom" id="attention5">

    <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
    	<tr td width="100%"  height="30" align="center"> 
			<td width="50" height="54" align="right" valign="top" class="helpfont">
				<strong>
				<% write(com_zhuyi); %></strong>	
			</td>
			<td width="12" height="54" align="right" valign="top" class="helpfont">
				&nbsp;
							
			</td>
			<td width="514" height="54" align="left" valign="top" class="helpfont">			
				<%write(hw_nat_tip); %>
			</td>
		</tr>
	</table>
 	</td>
 </tr>
 
 

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
