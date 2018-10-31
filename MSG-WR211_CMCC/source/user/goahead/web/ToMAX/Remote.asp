<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("Remote"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<% aspOutRemoteControlInfo();%>

function save(){
	var f=Remote_Config;
	if(! checkNumber(f.OutPort,1,65535,"<% write(OutPort); %>",0)) return; 
	f.HttpEnable.value=(f.HttpEnablew.checked==true?"1":"0"); 
	//alert(f.HttpEnable.value);
	Remote_Config.action ="/goform/formRemoteControl";
	Remote_Config.submit();
}
function init(){
	var f=Remote_Config;
	f.HttpEnablew.checked=(HttpEnables.toLowerCase()=="1"?true:false);
	f.OutPort.value=OutPorts;
	selVal(f.Profile,Profiles);
	yutd1.innerHTML="";
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
  <td><script language="javascript">showSecondTitle("<% write(Remote); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="Remote_Config" method="post" >
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr>
    <td width="33%" align="right" id="start HTTP"><% write(SUHTTP); %></td>
  	<td width="2%"></td>
	<td><input  type="checkbox" name="HttpEnablew"><input type="hidden" name="HttpEnable"></td>
  </tr>
    <tr><td></td><td></td><td colspan="2" class="helpfont" id="discription">
	<% write(SUHTTPMessage); %>
    </td></tr>
  <tr> 
    <td  align="right" id="external port"><% write(OutPort); %></td>
  	<td><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	<td><input type="text" size="12" name="OutPort" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,'1','65535','<% write(OutPort); %>'),yutd1)" maxlength="5"></td><td id="yutd1"></td>
  </tr>
   <tr  style="display:none">
	   <td align="right" height="25"  ><% write(Connector); %></td>
	   <td >&nbsp;</td>
	   <td ><select name="Profile">
	  <option value='WAN1'  selected="selected"> WAN1</option>
	  <option value='WAN2'> WAN2</option>
	  
	   
	   </select></td>
	   </tr>
  <tr><td colspan="4" align="center" class="helpfont" height="35" valign="bottom" id="attention5"><strong><% write(Attention); %>：</strong><% write(AttMessage); %></td></tr>
  <tr><td colspan="3" height="5">&nbsp;</td></tr>
  <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>"  onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Remote')"></td>
  </tr>
  <script language="javascript">init();</script>
</table>
  </td></tr>
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
