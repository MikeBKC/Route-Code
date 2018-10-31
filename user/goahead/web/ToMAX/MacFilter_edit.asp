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

var username="";
var macaddr="";
var edit_Name = "<% write(Form("id"));%>";
<%getSysLang();%>
<% aspoutErrorMsg();%>
<%
var edit_Name="";
edit_Name=Form("id");
if (edit_Name!="") outPutMacFilterOneInfo(edit_Name);//需要修改 
%> 
function init(){
	var f=MacFilter_Config;
	f.username.value=username;
	f.filterMac.value=macaddr;
	f.oldName.value=username;
	f.oldMac.value=macaddr;
	if(edit_Name!=""&&edit_Name!=null){
		f.Action.value="modify";
	}
}
function save(){
	var f=MacFilter_Config;
	if(!checkName(f.username,11,"<%write(com_user);%>",0)) return false;
	if(!checkMac(f.filterMac,"<% write(MACAddr); %>",0)) return ;
	MacFilter_Config.action="/goform/ConfigMacFilter";
	MacFilter_Config.submit();
}

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
  <td><script language="javascript">showSecondTitle("<% write(MacFilterEdit); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="" onSubmit="return " name="MacFilter_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td  colspan="3" height="20">&nbsp;</td></tr>
       <tr>
	   <td align="right" width="37%" height="25"><% write(com_user); %></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="25%"><input name="username" type="text" size="18" maxlength="11" onChange="showfiledtip(re_checkName(this,'<% write(com_user); %>'),yutd1)"></td>
	   <td id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" width="37%" height="25" id="MAC addr"><% write(MACAddr); %></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="25%"><input name="filterMac" type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(MACAddr); %>'),mactd)" ></td><td id="mactd"></td>
	   </tr>
	   
	 
  </table>

  
    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="50" align="center" valign="middle"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#MacFilter')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>"  onClick="GoBack_Page('MacFilter.asp')" id="goback"></td>
	
  </tr>
</table>
  
  
  </td>
  </tr>
</table><input type="hidden" name="oldName"><input type="hidden" name="Action" value="add"><input type="hidden" name="oldMac">
</form></td>
  </tr>
</table></td><script language="javascript">init();</script>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>



