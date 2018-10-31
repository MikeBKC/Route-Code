<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("WlanMacFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

<% aspoutErrorMsg();%>
var strid = "<% write(Form("id"));%>";//alert(strid);
function init(){
	if(strid!=""&&strid!=null){
		var f=WlanMacFilter_Config;
		f.filterMac.value=strid;
		f.oldMac.value=strid;
		f.Action.value="modify";
	}
}
function save(){
	var f=WlanMacFilter_Config;
	if(!checkMac(f.filterMac,"<% write(MACAddr); %>",0)) return ;
	WlanMacFilter_Config.action="/goform/ConfigWlanMacFilter";
	WlanMacFilter_Config.submit();
	 //parent.main.location="WlanMacFilter.asp";
}

</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(MacFilterEdit); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="" onSubmit="return " name="WlanMacFilter_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td  colspan="3" height="20">&nbsp;</td></tr>
       <tr>
	   <td align="right" width="37%" height="25" id="MAC addr"><% write(MACAddr); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="60%"><input name="filterMac" type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(MACAddr); %>'),mactd)" >&nbsp;<script language="javascript">document.write("<% write(Eg); %>");</script></td><td width="1%"  id="mactd"></td>
	   </tr>
	   
	 
  </table>

  
    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="50" align="center" valign="middle"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#WlanMacFilter')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>"  onClick="GoBack_Page('WlanMacFilter.asp')" id="goback"></td>
	
  </tr>
</table>
  
  
  </td>
  </tr>
</table><input type="hidden" name="oldMac"><input type="hidden" name="Action" value="add">
</form></td>
  </tr>
</table></td><script language="javascript">init();</script>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>



