<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("common"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("internet");

var strSecondMenuFocus="L2TP客户端配置";

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
	if ( edit_Name != "") OutputL2tpCliAccountData(edit_Name);//需要修改
%>

var edit_Names = "<% write(Form("id"));%>";//需要修改的账号名
L2TPClientEnables="on";
/*
var errorstr = "";
var PPTPClientEnables="on"; var setNames="1"; var userNames="1"; var passwds="1"; var AuthTypes="Either"; var serverLanIps="192.168.16.0"; var serverLanNetMasks="255.255.255.0"; var serverIps="200.200.202.58"; var totalrecs=1;var max_totalrecs=2;var errorstr="";
 
var edit_Names = "";//需要修改的账号名
*/ 
function checkData(){
	var f=PPTPClientConfig;
	if (!checkName(f.setName,31,"隧道名称",0)) return false;
	if (!checkName(f.userName,32,"用户名",0)) return false;
	if (!checkPassword1(f.passwd,31,0,0)) return false;
	if (!checkIP(f.serverLanIp,"远端内网IP地址",0)) return false;
	if (!checkMask(f.serverLanNetMask,"远端内网子网掩码",0)) return false;
	if (!checkName(f.serverIp,31,"隧道服务器地址(名)",0)) return false;
	if(edit_Names=="" || edit_Names == null){ 
			/*if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("PPTP账号已经达到最大个数,不能添加新的PPTP账号"));
			GoBack_Page("PPTPClientList.asp");
			return;
		}*/
	}
	return true;
}
function save(){
	if(checkData()){
		var f=PPTPClientConfig;
		PPTPClientConfig.action="/goform/l2tpCliAccountConfig";
		PPTPClientConfig.submit();
	}	
}
function init(){
	var f=PPTPClientConfig;
	if(edit_Names!=""&&edit_Names!=null){ 
	f.L2TPClientEnable.checked=(L2TPClientEnables=="on"?true:false);
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
 	f.L2TPClientEnable.checked = true;
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
	PPTPClientConfig.reset();
	for(i=1;i<=6;i++)
		document.all["yutd"+i].innerHTML="";
	init();	
}



//-->
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
  <td><script language="javascript">showSecondTitle("L2TP客户端");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<!--<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,

						new Array("PPTP客户端",'PPTPClientList.asp',1,0),
						new Array("PPTP客户端配置",'PPTPClient.asp',1,1)
		);
		showThreeMenu();
	</script>-->
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="style7" valign="top"><table width="800" border="0" cellspacing="0" cellpadding="0">

<form  name="PPTPClientConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >
  <table border="0" width="800" align='center'>
    <tr style="display:none">
	   <td align="right"  height="25" width="40%" id="PPTPenabletd"><input type="hidden" name="Action" value="add">启用该配置</td>
	   <td width="2%">&nbsp;</td>
	   <td width="24%" align="left"><input type="checkbox" name="L2TPClientEnable"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" width="40%" id="setnametd">隧道名称</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="setName" size="18"  maxlength="31"   value="" onChange="showfiledtip(re_checkName(this,'隧道名'),yutd6)"></td><td width="34%"  id="yutd6"></td>
	   </tr><input type="hidden" name="oldSetName" >
  <tr>
	   <td align="right"  height="25" width="40%" id="usernametd">用户名</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="userName" size="18"  maxlength="32" onChange="showfiledtip(re_checkName(this,'用户名'),yutd1)"  value=""></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="passwdtd">密码</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input  type="password" name="passwd" size="19"   maxlength="11" onChange="showfiledtip(re_checkPassword1(this,31,'密码'),yutd2)" value=""></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="authtypetd">密码验证方式</td>
	   <td >&nbsp;</td>
	   <td ><select name="AuthType">
	   <option value="NONE">NONE</option>
	   <option value="PAP">PAP</option>
	   <option value="CHAP">CHAP</option>
	   <option value="EITHER" selected="selected">EITHER</option>
	   </select></td>
	   <td></td>
	   </tr>
	    
	    <tr>
	   <td align="right"  height="25" id="serverlaniptd">远端内网网络地址</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="serverLanIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'远端内网IP地址'),yutd4)"  value=""></td><td  id="yutd4"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" id="servernetmasktd">远端内网子网掩码</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="serverLanNetMask" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'远端内网子网掩码'),yutd5)"  value=""></td><td  id="yutd5"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="serveriptd">隧道服务器地址(名)</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="serverIp" size="18"  maxlength="31" value="" onChange="showfiledtip(re_checkName(this,'隧道服务器地址(名)'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	  <tr><td  colspan="4">
  <table width="29%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="right" valign="top"><input type="button" class="input-button" value="保存" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="重填" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="返回" onClick="GoBack_Page('L2TPList.asp')" id="returnback"></td>
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
