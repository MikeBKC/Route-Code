<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("hotel");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

<% aspOutHotel(); %>

function save(){
    formhotelEnable.action="/goform/formHotel";	
    formhotelEnable.submit();
}  

function initTranslation()
{
    var e;
}

function init()
{
    var f=formhotelEnable;
    f.hotelEnable.checked=((hotelPnpEn=="on")?true:false);
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
  <td><script language="javascript">showSecondTitle(_("<%write(hotel);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="formhotelEnable" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center"  class="style7">
<table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
		<td width="54%" height="25" align="right" valign="bottom"><%write(hotelStart);%></td>
		<td width="1%">&nbsp;</td>
		<td width="45%" height="20" align="left" valign="bottom"><input name="hotelEnable" type="checkbox" ></td>
  </tr>
		  
 <tr><td colspan="3" height="5">&nbsp;</td></tr>
		  
 <tr>
  <td colspan="4" align="center" valign="top">
    <table width="100" border="0" cellspacing="0" cellpadding="0" align="center"> 
  	<tr>
    	<td width="50%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave"> 
		</td>
	    <td width="50%"  height="30" align="center" valign="top"><input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<%write(com_helppage);%>#Hotel')">
		</td>
  	</tr>
  </table>
  </td>
 </tr>
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
				1.&nbsp;
							
			</td>
			<td width="514" height="54" align="left" valign="top" class="helpfont">			
				<%write(hotelContent1); %>
			</td>
		</tr>
	    <tr td width="100%"  height="30" align="center"> 
			<td width="50" height="36" align="right" valign="top" class="helpfont">			
			</td>
			<td width="12" height="36" align="right" valign="top" class="helpfont">
				2.&nbsp;			
			</td>			
			<td width="514" height="36" align="left" valign="top" class="helpfont">
				<%write(hotelContent2);%>
			</td>
		</tr>
	    <tr td width="100%"  height="30" align="center"> 
			<td width="50" height="36" align="right" valign="top" class="helpfont">			
			</td>
			<td width="12" height="36" align="right" valign="top" class="helpfont">
				3.&nbsp;			
			</td>			
			<td width="514" height="36" align="left" valign="top" class="helpfont">
				<%write(hotelContent3);%>
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
