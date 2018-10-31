<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("IPMac");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var errorstr="";
//Butterlate.setTextDomain("internet");

//function initTranslation()
//{
//	var e;
//
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e = document.getElementById("uttreset");
//	e.value= _("reset");
//	e= document.getElementById("goback");
//	e.value= _("goback");
//	e= document.getElementById("name");
//	e.innerHTML= _("name");
//	e= document.getElementById("ipaddr");
//	e.innerHTML= _("ipaddr");
//	e= document.getElementById("MACaddr");
//	e.innerHTML= _("MACaddr");
//}
var IPs = "<% getLanIp(); %>";
var Masks = "<% getLanNetmask(); %>";
<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name !="") aspOutOneArpBindConfig(edit_Name);//需要修改
%>
var edit_Names = "<% write(Form("id"));%>";
<!--


function init(){
	var f=ARPBind_Config;
	if(edit_Names!=""&&edit_Names!=null){
		f.UserName.value=UserNames;
		f.UserNameold.value=UserNames;
		f.IPold.value=arp_IPs;
		f.Macold.value=arp_Macs;
		f.Allow.value=Allows;
		f.IP.value=arp_IPs;
		f.Mac.value=arp_Macs;
		f.Action.value="modify";
	}
	for(i=1;i<=3;i++)
		document.all["yutd"+i].innerHTML="";
}

function save(){
	var f=ARPBind_Config;
	if(checkData()){
		ARPBind_Config.action="/goform/formArpBindConfig";
		ARPBind_Config.submit();
	}
}
function checkData(){
	var f = ARPBind_Config;
	if(!checkName(f.UserName,11,"<%write(com_user);%>",0)) return false;
	if(!checkIP(f.IP,"<%write(com_IP);%>",0)) return false;
	if(!checkMac(f.Mac,"<%write(com_MAC);%>",0)) return false;
	/*
	if (!checkSameSubnet(IPs,f.IP.value,Masks))
		{
			alert("IP地址与LAN口地址不在同一网段上。");
			return false;
		}*/
	return true;
}
function Reset(){
	ARPBind_Config.reset();
	init();
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_bandConf);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="ARPBind_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td><td width="41%">&nbsp;</td></tr>
  
   <tr>
	   <td width="35%" height="25" align="right" id="name"><%write(com_user);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="22%"><input name="UserName" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(com_user);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr> <input type="hidden" name="UserNameold" ><input type="hidden" name="Allow"><input type="hidden" name="IPold">  <input type="hidden" name="Macold">    
       <tr>
	   <td align="right" height="25" id="ipaddr"><%write(com_IP);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="IP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),yutd2)" ></td><td  id="yutd2"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25" id="MACaddr"><%write(com_MAC);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="Mac" type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<%write(com_MAC);%>'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('IPMac.asp')" id="goback"></td>
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
