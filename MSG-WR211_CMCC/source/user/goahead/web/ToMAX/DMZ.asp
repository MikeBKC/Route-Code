 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("DMZ");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e = document.getElementById("uttreset");
//	e.value= _("reset");
//
//	e = document.getElementById("start DMZ");
//	e.innerHTML= _("start DMZ");
//	e= document.getElementById("discription");
//	e.innerHTML= _("discription");
//	e = document.getElementById("DMZ ip");
//	e.innerHTML= _("DMZ ip");
//
//}

<% aspOutDMZIP(); %>
/*var DMZIPs="202.101.10.10";
var DMZEnables="on";
var errorstr="";*/


DMZEnables = (DMZEnables=="1"?"on":"off");

function checkData(){
	var f=DMZConfig;
	if(f.DMZEnable.checked){
		if (f.DMZIP.value=="0.0.0.0"){ alert("<%write(dmz_ipTip);%>");return false};
		if (!checkIP(f.DMZIP,"<%write(com_IP);%>",0)) return false;
	}
	return true;
}
function save(){
	if(checkData()){
		var f=DMZConfig;
		DMZConfig.action="/goform/formDMZIP";
		DMZConfig.submit();
	}	
}
function init(){
	var f=DMZConfig;
	f.DMZEnable.checked=(DMZEnables=="on"?true:false);
	f.DMZIP.value=DMZIPs;
}
function Reset(){
	DMZConfig.reset();
	iptd.innerHTML="";
	init();	
}
//-->
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
  <td><script language="javascript">showSecondTitle("DMZ");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="DMZConfig" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						3,
						new Array(_("<%write(com_staticMap);%>"),'NatStatic.asp',1,0),
						new Array(_("<%write(com_NATRule);%>"),'NatRule.asp',1,0),
						new Array("DMZ",'DMZ.asp',1,1)
		);
		showThreeMenu();
	</script>
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" height="250">
  <tr>
    <td align="center" class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr> 
   <tr>
	   <td align="right"  height="25" width="35%" id="start DMZ"><%write(dmz_start);%></td>
	   <td width="2%">&nbsp;</td>
	   <td><input type="checkbox" name="DMZEnable" ></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont" id="discription"><%write(dmz_startTip);%></td></tr>
       <tr>
	   <td align="right" height="25" id="DMZ ip"><%write(dmz_hostIP);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="DMZIP" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(dmz_hostIP);%>'),iptd)" value="0.0.0.0"></td><td  id="iptd"></td>
	   </tr>
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DMZ')"></td>
	
  </tr>
</table>

  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
