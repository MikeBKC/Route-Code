<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DHCP"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
var errorstr="";
/////////////////////////////交互变量/////////////////////

<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutDhcpOneClientInfo(edit_Name);//需要修改
%>

var edit_Names = "<% write(Form("id"));%>";
<!--


function init(){
	var f=FixDhcp_Config;
	if(edit_Names!=""&&edit_Names!=null){
		f.UserName.value=FixUserNames;
		f.UserNameold.value=FixUserNames;
		f.IP.value=FixIPs;
		f.Mac.value=FixMacs;
		f.Action.value="modify";
	}
	fixusernametd.innerHTML="";
	fixiptd.innerHTML="";
	fixmactd.innerHTML="";
}

function save(){
	var f=FixDhcp_Config;
	if(checkData()){
                if(f.Action.value=="add") {
                     f.UserNameold.value=f.UserName.value;
                }
		FixDhcp_Config.action="/goform/formDhcpListStatic";
		FixDhcp_Config.submit();
	}
}
function checkData(){
	var f = FixDhcp_Config;
	/*
	var name=document.getElementById('UserName').value;
	if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\     '") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1)
	{
		alert("用户名应为除:< > , % \' \\ \" & ; 和空格之外的字符。");
		return false;
	}
	*/
	if(!checkName(f.UserName,13,"<% write(com_user); %>",0)) return false;
	if(!checkIP(f.IP,"<% write(com_IP); %>",0)) return false;
	if(!checkMac(f.Mac,"<% write(com_MAC); %>",0)) return false;
	return true;
}
function Reset(){
	FixDhcp_Config.reset();
	init();
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(StaticDHCP); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="FixDhcp_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
	   <td width="35%" height="25" align="right" id="name"><% write(com_user); %></td>
	   <td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input name="UserName" id="UserName" type="text" maxlength="12" size="18" onChange="showfiledtip(re_checkName(this,'<% write(com_user); %>'),fixusernametd)"></td><td  id="fixusernametd"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="ipaddr"><% write(com_IP); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td ><input name="IP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<% write(com_IP); %>'),fixiptd)"></td><td  id="fixiptd"></td>
	   </tr>
	   <input type="hidden" name="UserNameold" >
	    <tr>
	   <td align="right" height="25" id="MACaddr"><% write(com_MAC); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input name="Mac" type="text" size="18" maxlength="17" onKeyPress="macOnly()"  onChange="showfiledtip(re_checkMac(this,'<% write(com_MAC); %>'),fixmactd)"></td><td  id="fixmactd"></td>
	   </tr>
 
	   <tr><td colspan="4">
	    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="40" align="left" valign="bottom" class="helpfont"><% write(HelpFont); %></td>
  </tr>
</table>
  </td></tr>
	  

	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="middle"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#StaticDHCP')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('DhcpFix.asp')" id="returnback"></td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr><script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

