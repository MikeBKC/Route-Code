<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��̩�Ƽ�</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("internet");

var strSecondMenuFocus="������������";

function initTranslation()
{ 
/*
	var e;
	e = document.getElementById("PPTPenabletd");
	e.innerHTML= _("PPTPenable");
	e = document.getElementById("usernametd");
	e.innerHTML= _("name");
	e = document.getElementById("passwdtd");
	e.innerHTML= _("password");
	e= document.getElementById("authtypetd");
	e.innerHTML= _("pdmethod");
	e= document.getElementById("serveriptd");
	e.innerHTML= _("serveripaddr");
	e= document.getElementById("serverlaniptd");
	e.innerHTML= _("serverlanipaddr");
	e= document.getElementById("servernetmasktd");
	e.innerHTML= _("serverlannetmask");
	e= document.getElementById("PPTPClientstatustd");
	e.innerHTML="<strong>"+ _("PPTPClentstatus")+"</strong>";
	
	e= document.getElementById("namestatustd");
	e.innerHTML="<strong>"+  _("name")+"</strong>";
	e= document.getElementById("sesstionstatustd");
	e.innerHTML= "<strong>"+ _("PPTPsessionstatus")+"</strong>";
	e= document.getElementById("gatetd");
	e.innerHTML="<strong>"+  _("PPTPgateway")+"</strong>";
	e= document.getElementById("laniptd");
	e.innerHTML= "<strong>"+ _("PPTPlanip")+"</strong>";
	e= document.getElementById("usetimetd");
	e.innerHTML= "<strong>"+ _("PPTPusetime")+"</strong>";
	e= document.getElementById("outboundtd");
	e.innerHTML= "<strong>"+ _("PPTPoutbound")+"</strong>";
	e= document.getElementById("inboundtd");
	e.innerHTML= "<strong>"+ _("PPTPinbound")+"</strong>";
	e= document.getElementById("virtualiptd");
	e.innerHTML= "<strong>"+ _("PPTPvirtualip")+"</strong>";
	
	e = document.getElementById("uttsave");
	e.value= _("save");
	e = document.getElementById("uttreset");
	e.value= _("reset");
	*/
}
var errorstr = "";
<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name != "") aspPptpClientOneConfig(edit_Name);//��Ҫ�޸�
%>

var edit_Names = "<% write(Form("id"));%>";//��Ҫ�޸ĵ��˺���
/*
var errorstr = "";
var PPTPClientEnables="on"; var setNames="1"; var userNames="1"; var passwds="1"; var AuthTypes="Either"; var serverLanIps="192.168.16.0"; var serverLanNetMasks="255.255.255.0"; var serverIps="200.200.202.58"; var totalrecs=1;var max_totalrecs=2;var errorstr="";
 
var edit_Names = "";//��Ҫ�޸ĵ��˺���
*/ 
function checkData(){
	var f=ConnectionLimitConfig;
	if (!checkName(f.setName,31,"�������",0)) return false;
	if (!checkName(f.userName,31,"�û���",0)) return false;
	if (!checkPassword1(f.passwd,"11",0)) return false;
	if (!checkIP(f.serverLanIp,"Զ������IP��ַ",0)) return false;
	if (!checkIP(f.serverLanNetMask,"Զ��������������",0)) return false;
	if (!checkName(f.serverIp,31,"�����������ַ(��)",0)) return false;
	if(edit_Names=="" || edit_Names == null){ 
			/*if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("PPTP�˺��Ѿ��ﵽ������,��������µ�PPTP�˺�"));
			GoBack_Page("PPTPClientList.asp");
			return;
		}*/
	}
	return true;
}
function save(){
	if(checkData()){
		var f=ConnectionLimitConfig;
		ConnectionLimitConfig.action="/goform/formPptpClientConfig";
		ConnectionLimitConfig.submit();
	}	
}
function init(){
	var f=ConnectionLimitConfig;
	if(edit_Names!=""&&edit_Names!=null){ 
	f.PPTPClientEnable.checked=(PPTPClientEnables=="on"?true:false);
	f.userName.value=userNames;
	f.setName.value=setNames;
	f.oldSetName.value=setNames;
	f.passwd.value=passwds;
	f.serverLanNetMask.value=serverLanNetMasks;
	f.AuthType.value=AuthTypes;
	f.serverIp.value=serverIps;
	f.serverLanIp.value=serverLanIps;
	f.Action.value="modify";
	}else{
 	f.PPTPClientEnable.checked = false;
 	f.setName.value = "";
 	f.userName.value = "";
 	f.passwd.value = "";
 	f.serverLanIp.value = ""; 
 	f.serverLanNetMask.value = "";
 	f.serverIp.value = "";
	f.Action.value="add";
	}
}
function Reset(){
	ConnectionLimitConfig.reset();
	for(i=1;i<=6;i++)
		document.all["yutd"+i].innerHTML="";
	init();	
}



//-->
</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td> 
  <!--////����-->
  <td><script language="javascript">showSecondTitle("������������");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	 
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="style7" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="PPTPClientConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >
  <table border="0" width="600" align='center'>
     
	   <tr>
	   <td width="37%" height="25" align="right">����</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td width="55%"><input name="GroupName" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'����'),yutd1)"></td><td width="6%"  id="yutd1"></td>
	   </tr>
	   <input type="hidden" name="GroupNameold" >
	   <tr>
	   <td align="right" height="25">��ʼIP��ַ</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="FromIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'��ʼIP��ַ'),yutd2)"></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25">����IP��ַ</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="EndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'����IP��ַ'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25">��������</td>
	   <td >&nbsp;</td>
	   <td><input name="EndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'����IP��ַ'),yutd3)"></td><td  id="yutd3"></td>
	   </tr><tr>
	   <td align="right" height="25">TCP������</td>
	   <td >&nbsp;</td>
	   <td><input name="EndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'����IP��ַ'),yutd3)"></td><td  id="yutd3"></td>
	   </tr><tr>
	   <td align="right" height="25">UDP������</td>
	   <td >&nbsp;</td>
	   <td><input name="EndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'����IP��ַ'),yutd3)"></td><td  id="yutd3"></td>
	   </tr><tr>
	   <td align="right" height="25">ICMP������</td>
	   <td >&nbsp;</td>
	   <td><input name="EndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'����IP��ַ'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	    <tr>
	        <td class="helpfont" height="30" align="left" colspan="3" id="attention2">һ������£����Ự����������̫С�����顰�����������͡�TCP�Ự��������С��100����UDP�Ự������С��50����ICMP�Ự������С��10��������ǵ�ֵ̫С�������¾������û������쳣��</td>
	    </tr>
	  <tr><td  colspan="4">
  <table width="24%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="right" valign="top"><input type="button" class="input-button" value="����" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="����" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="����" onClick="GoBack_Page('ConnectionLimit.asp')" id="returnback"></td>
</tr>
</table>
  </td>
	
  </tr>
  <tr><td colspan="4" align="center" height="30">&nbsp;</td></tr>

  </table></td>
  </tr>

  <script language="javascript">init();</script>
</form>

</table>
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table></td></tr></table>
</body>
</html> 
