<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("PPTP"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">


var strSecondMenuFocus="<% write(SecondMenuFocus); %>";
var errorstr = "";
var bindIps = "";
var LanMacStr = "";
var hardFlag = 0;

<%
	OutputHardFeatureFlag();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name != "") OutputPptpSrvAccountData(edit_Name);//需要修改
%>
/*var bindipEnable = 1;
var bindIps = "192.168.16.56";*/
var edit_Names = "<% write(edit_Name);%>";//需要修改的账号名

function checkData(){
	var f=pptpSrvAccountConfig;
	if (!checkName(f.instName, 12,"<% write(TunnelName); %>",0)) return false;
	if (!checkName(f.userName,32,"<%write(com_user);%>",0)) return false;
	if(!checkPassword1(f.passwd, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	if (f.bindIp.value=="0.0.0.0"){ alert("<%write(pptp_ipNull);%>");return false};
	if (!checkIP(f.bindIp,"<%write(pptp_bindIp);%>",1)) return false;
	if(!checkMac(f.LanMac,"<%write(HardwareFeature);%>",1)) return false;
	if (f.userType.selectedIndex == 0) {
		if (!checkIP(f.remoteLanIp,"<% write(RemoteIPAddr); %>",0)) return false;
		if (!checkMask(f.remoteLanNetmask,"<% write(RemoteMaskAddr); %>",0)) return false;
	}
	if(edit_Names=="" || edit_Names == null){ 
			/*if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(PPTPUserMaxErr); %>");
			GoBack_Page("PPTPClientList.asp");
			return;
		}*/
	}
	return true;
}
function save(){
	if(checkData()){
		var f=pptpSrvAccountConfig;
		f.instName.disabled = false;
		pptpSrvAccountConfig.action="/goform/pptpSrvAccountConfig";
		pptpSrvAccountConfig.submit();
	}	
}


function init(){
	var f=pptpSrvAccountConfig;
	if((edit_Names!="") && (edit_Names!=null)){ 
		f.instName.value = instNames;
		f.userName.value = userNames;
		f.passwd.value = passwds;
		f.bindIp.value = bindIps;
		f.LanMac.value = "";
		if(LanMacStr != "000000000000")
		    f.LanMac.value = LanMacStr;
		f.remoteLanIp.value = remoteLanIps;
		f.remoteLanNetmask.value = remoteNetmasks;		
		f.Action.value="modify";
		f.userType.selectedIndex = getSelIndexByValue(f.userType, peerTypes);
		f.instName.disabled = true;
	}else{
		f.instName.value = "";
		f.userName.value = "";
		f.passwd.value = "";
		f.bindIp.value = "";
		f.LanMac.value = "";
		f.remoteLanIp.value = "";
		f.remoteLanNetmask.value = "";		
		f.Action.value="add";
		f.userType.selectedIndex = 0;
	}
	utypeChanged(f.userType)
}
function Reset(){
	pptpSrvAccountConfig.reset();
	for(i=1;i<=5;i++)
		document.all["yutd"+i].innerHTML="";
	init();	
}


function utypeChanged(arg) {

	if (getListValue(arg) == "lantolan") {
		showTable(LanMacTr, 0);
		showTable(remoteLanIpTr, 1);
		showTable(remoteLanNetmaskTr, 1);
	} else {
		if(hardFlag == 1)
		showTable(LanMacTr, 1);
		showTable(remoteLanIpTr, 0);
		showTable(remoteLanNetmaskTr, 0);	
	}
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
  <td><script language="javascript">showSecondTitle("<% write(PPTPSSecondTitle); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						2,

						new Array("<% write(GolConf); %>",'PPTPSrvGlobal.asp',1,0),
						new Array("<% write(UserConf); %>",'PPTPSrvAccount.asp',1,1)
		);
		showThreeMenu();
	</script>
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="pptpSrvAccountConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >

	
  <table border="0" width="600" align='center'>
	   <tr>
	   <td align="right"  height="25" width="40%" id="instNameTd"><input type="hidden" name="Action" value="add"><% write(TunnelName); %></td>
	   <td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="instName" size="18"  maxlength="31"   value="" onChange="showfiledtip(re_checkName(this,'<% write(TunnelName); %>'),yutd3)"></td><td width="34%"  id="yutd3"></td>
	   </tr><input type="hidden" name="oldSetName" >
  <tr>
	   <td align="right"  height="25" width="40%" id="userTypeTd"><% write(Usertype); %></td>
	   <td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><select name="userType" onChange="utypeChanged(this)">
	   <option value="lantolan"><% write(LanToLan); %></option>
	   <option value="mobile"><% write(MobileUser); %></option>
	   </select></td>
	   </tr>
  <tr>
	   <td align="right"  height="25" width="40%" id="userNameTd"><% write(com_user); %></td>
	   <td width="2%"><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="userName" size="18"  maxlength="32" onChange="showfiledtip(re_checkName(this,'<% write(com_user); %>'),yutd1)"  value=""></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="passwdTd"><% write(com_passwd); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input  type="password" name="passwd" size="18"   maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd2)" value=""></td><td  id="yutd2"></td>
	   </tr>

	<tr>
	   <td align="right" height="25" id="bindIp"><% write(pptp_bindIp); %></td>
	   <td width="7"></td>
	   <td><input  type="text" name="bindIp" size="18"   maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<%write(pptp_bindIp);%>'),yutd6)" onKeyPress="ipOnly()"></td><td  id="yutd6"></td>
       </tr>
    
	    <tr id="LanMacTr" style="display:">
	   <td align="right"  height="25" id="LanMacTd"><% write(HardwareFeature); %></td>
	   <td width="7"></td>
	   <td><input type="text" name="LanMac" size="18" onKeyPress="macOnly()" maxlength="15" onChange="showfiledtip(re_checkMac(this,'<% write(HardwareFeature); %>'),yutd7)"  value=""></td><td  id="yutd7"></td>
	   </tr>
	    <tr id="remoteLanIpTr" style="display:">
	   <td align="right"  height="25" id="remoteLanIpTd"><% write(RemoteNetAddr); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="remoteLanIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(RemoteIPAddr); %>'),yutd4)"  value=""></td><td  id="yutd4"></td>
	   </tr>
	    <tr id="remoteLanNetmaskTr" style="display:">
	   <td align="right"  height="25" id="remoteLanNetmaskTd"><% write(RemoteMaskAddr); %></td>
	   <td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
	   <td><input type="text" name="remoteLanNetmask" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<% write(RemoteMaskAddr); %>'),yutd5)"  value=""></td><td  id="yutd5"></td>
	   </tr>
	   <tr><td colspan="4" height="20">&nbsp;</td></tr>
	  <tr><td  colspan="4">
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PPTP')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('PPTPClientList.asp')" id="returnback"></td>
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
