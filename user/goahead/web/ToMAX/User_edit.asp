<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>

<% langTran("User_edit"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("syscontrol");
 
var errorstr = "";
/////////////////////////////交互变量/////////////////////

<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name !="") aspOutUserOneInfo(edit_Name);//需要修改
%>

var edit_Names = "<% write(Form("id"));%>";
////////////////////////////交互变量结束/////////////////
function init(){
	var f=Admin_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.username.value=usernames;
		f.usernameold.value=edit_Names;
		f.passwd1.value=passwd1s;
		f.passwd2.value=passwd2s;
		f.Action.value="modify";
	}
	usernametd.innerHTML="";
	passwd1tb.innerHTML="";
	passwd2tb.innerHTML="";
}
function save(){
	if(re_checkSpecialName(Admin_Config.username, '<% write(com_user); %>'))
	{
		alert("<% write(BadUserName); %>");
		return;
	}
	if(checkData()){
		Admin_Config.action="/goform/formUser";
		Admin_Config.submit();	
	}
}
function checkData(){
	var f=Admin_Config;
	if(!checkName(f.username,11,"<% write(com_user); %>",0)) return false;	//检查用户名：不能为空、不能全为0、不能超11个字符、应为除:< > , % \' \\ \" & ; 和空格之外的字符。
	if(!checkPassword(f.passwd1, f.passwd2, 31, 0, 0)) return false;	//检查2次的密码输入是否一致。
	if(!checkPassword1(f.passwd1, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	if(!checkPassword1(f.passwd2, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	return true;
} 
function Reset(){
	Admin_Config.reset();
	init();
}
</script>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(Administrantor); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/" onSubmit="return " name="Admin_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
        <tr>
		<td width="36%" align="right" height="25" id="user"><% write(com_user); %></td>
		<td width="2%" align ="left"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td width="25%" height="20"><input name="username" type="text" style="width:138px" maxlength="11" onChange="showfiledtip(re_checkSpecialName(this,'<% write(com_user); %>'),usernametd)"></td>
		<td width="37%"  id="usernametd"></td>
	      </tr><input type="hidden" name="usernameold" >
		  <tr>
		<td  align="right" id="password"><% write(Passwd); %></td>
		<td><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="passwd1"  type="password" style="width:138px" maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31, 0),passwd1tb)"></td><td  id="passwd1tb"></td>
	      </tr>
		  <tr>
		<td align="right" id="confirm password"><% write(Confirmpasswd); %></td>
		<td><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="passwd2" type="password" style="width:138px" maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31, 0),passwd2tb)"></td><td  id="passwd2tb"></td>
	      </tr>
		  <tr><td colspan="4"><table width="100%">
	   <tr  height="30">
	     <td width="27%" align="right" valign="top" class="helpfont" id="attention1"><strong><% write(Attention); %>:</strong></td>
	   <td width="73%" align="left" class="helpfont" id="attention1"><% write(FAttentionMesLine1); %><br>
		<% write(FAttentionMesLine2); %></td></tr>
	   </table></td></tr>

		  <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>"  onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#User')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('User.asp')" id="goback"></td>
	
  </tr>
</table>
  </td></tr>
  </table></td></tr>
  
</table><script language="javascript">init();</script>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
	      
