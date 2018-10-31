<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("PPTP"); %>
<% langTran("common"); %>
<% langTran("WANConfig");%>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<% write(SecondMenuFocus); %>";
var errorstr = "";

<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name != "") aspPptpClientOneConfig(edit_Name);//需要修改
%>

var edit_Names = "<% write(Form("id"));%>";//需要修改的账号名
/*
var errorstr = "";
var PPTPClientEnables="on"; var setNames="1"; var userNames="1"; var passwds="1"; var AuthTypes="Either"; var serverLanIps="192.168.16.0"; var serverLanNetMasks="255.255.255.0"; var serverIps="200.200.202.58"; var totalrecs=1;var max_totalrecs=2;var errorstr="";
 
var edit_Names = "";//需要修改的账号名
*/ 
function checkData(){
	var f=PPTPClientConfig;
	if (!checkName(f.setName,12,"<% write(TunnelName); %>",0)) return false;
	if (!checkName(f.userName,32,"<% write(com_user); %>",0)) return false;
	if(!checkPassword1(f.passwd, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	if (!checkIP(f.serverLanIp,"<% write(RemoteIPAddr); %>",0)) return false;
	if (!checkMask(f.serverLanNetMask,"<% write(RemoteMaskAddr); %>",0)) return false;
	if (!checkName(f.serverIp,31,"<% write(TunnelServerAddr); %>",0)) return false;
 	if(Encrypt == 1)
 	{
 	    if((f.EncryptionMode.value == "MPPE") && (f.AuthType.value != "MS-CHAPV2"))
 	    {
 		alert("<% write(MS_CHAPV2MustMPPE); %>");
 		return false;
 	    }
 	}
	if(edit_Names=="" || edit_Names == null){ 
			/*if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("<% write(PPTPUserMaxErr); %>"));
			GoBack_Page("PPTPClientList.asp");
			return;
		}*/
	}
	if(! checkNumber(f.MTU,1,1492,"MTU",0)) return false;
	return true;
}
function save(){
	if(checkData()){
		var f=PPTPClientConfig;
		PPTPClientConfig.action="/goform/formPptpClientConfig";
		PPTPClientConfig.submit();
	}	
}
function init(){
	var f=PPTPClientConfig;
	if(edit_Names!=""&&edit_Names!=null){ 
	f.PPTPClientEnable.checked=(PPTPClientEnables=="on"?true:false);
	f.PPTPClientNATEnable.checked=(PPTPClientNATEnables=="on"?true:false);
	f.userName.value=userNames;
	f.setName.value=setNames;
	f.oldSetName.value=setNames;
	f.passwd.value=passwds;
	f.serverLanNetMask.value=serverLanNetMasks;
	f.AuthType.value=AuthTypes;
	f.MTU.value=mtu;
	if(Encrypt == 1)
	{
	    f.EncryptionMode.value = EncryptionModes;
	    showTable(EncryptMode, 1);
	}
	f.serverIp.value=serverIps;
	f.serverLanIp.value=serverLanIps;
	f.Action.value="modify";
	}else{
 	f.PPTPClientEnable.checked = false;
 	f.PPTPClientNATEnable.checked = false;
 	f.setName.value = "";
 	f.userName.value = "";
 	f.passwd.value = "";
	if(Encrypt == 1)
	{
	    showTable(EncryptMode, 1);
	}
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
  <td><script language="javascript">showSecondTitle("<% write(PPTPCSecondTitle); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<!--<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,

						new Array("<% write(PPTPCSecondTitle); %>",'PPTPClientList.asp',1,0),
						new Array("<% write(SecondMenuFocus); %>",'PPTPClient.asp',1,1)
		);
		showThreeMenu();
	</script>-->
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="style7" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="PPTPClientConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >
  <table border="0" width="600" align='center'>
    <tr>
	   <td align="right"  height="25" width="40%" id="PPTPenabletd"><input type="hidden" name="Action" value="add"><% write(UseTheConf); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="24%" align="left"><input type="checkbox" name="PPTPClientEnable"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" width="40%" id="PPTPNATenabletd"><input type="hidden" name="NAT"><% write(UserNAT); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="24%" align="left"><input type="checkbox" name="PPTPClientNATEnable"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" width="40%" id="setnametd"><% write(TunnelName); %></td>
	   <td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="setName" size="18"  maxlength="31"   value="" onChange="showfiledtip(re_checkName(this,'<% write(TunnelN); %>'),yutd6)"></td><td width="34%"  id="yutd6"></td>
	   </tr><input type="hidden" name="oldSetName" >
  <tr>
	   <td align="right"  height="25" width="40%" id="usernametd"><% write(com_user); %></td>
	   <td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="userName" size="18"  maxlength="32" onChange="showfiledtip(re_checkName(this,'<%write(com_user);%>'),yutd1)"  value=""></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="passwdtd"><% write(com_passwd); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input  type="password" name="passwd" size="19"   maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd2)" value=""></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="authtypetd"><% write(com_checkPasswd); %></td>
	   <td >&nbsp;</td>
	   <td ><select name="AuthType">
	   <option value="PAP"><% write(com_pap); %></option>
	   <option value="CHAP"><% write(com_chap); %></option>
 	   <option value="MS-CHAPV2"><% write(com_chapv2); %></option>
 	   <option value="THRIN" selected="selected"><% write(com_any); %></option>
	   </select></td>
	   <td></td>
	   </tr>
	    
 	   <tr name="EncryptMode" id="EncryptMode" style="display:none">
 	   <td align="right"  height="25"><% write(EncryptionMode); %></td>
 	   <td >&nbsp;</td>
 	   <td > <select name="EncryptionMode">
 	   <option value='None' selected="selected"><% write(NoneEncrypt); %></option>
 	   <option value='MPPE'><% write(MPPEEncrypt); %></option>
 	   </select></td>
 	   <td id="yutd3"></td>
 	   </tr>

	    <tr>
	   <td align="right"  height="25" id="serverlaniptd"><% write(RemoteNetAddr); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="serverLanIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(RemoteIPAddr); %>'),yutd4)"  value=""></td><td  id="yutd4"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" id="servernetmasktd"><% write(RemoteMaskAddr); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="serverLanNetMask" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<% write(RemoteMaskAddr); %>'),yutd5)"  value=""></td><td  id="yutd5"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="serveriptd"><% write(TunnelServerAddr); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="serverIp" size="18"  maxlength="31" value="" onChange="showfiledtip(re_checkName(this,'<% write(TunnelServerAddr); %>'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" >MTU</td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td width="30%"><input type="text" name="MTU" size="18"  maxlength="4" value="1478" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,'1','1492','MTU'),yutd7)">&nbsp;<%write(wan_byte);%></td><td  id="yutd7"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" ></td>
	   <td ></td>
	   <td><%write(wan_MTUTip);%></td>
	   </tr>
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	  <tr><td  colspan="4">
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PPTP')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('PPTPClientList.asp')" id="returnback"></td>
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
