<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<% langTran("PPTP"); %>
<% langTran("common"); %>
<% langTran("WANConfig");%>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" ContentType="text/xml">

var strSecondMenuFocus="<% write(PPTPSSecondTitle); %>";
var mtu = 1478;

<% OutputPptpSrvGlobalData(); %>

var lanIp = "<% getLanIp(); %>";
var lanNm = "<% getLanNetmask(); %>";

/* 初始化 */
function init() {
	var frm = pptpSrvConfig;
	frm.enable.checked = enables;
	frm.authType.value = authTypes;
	frm.poolStart.value = poolIpStarts;
	frm.poolCount.value = poolIpCnts;
	frm.localIp.value = localIps;
	frm.priDns.value = priDns;
	frm.secDns.value = secDns;
	frm.MTU.value=mtu;
	if(Encrypt == 1)
	{
	    frm.EncryptionMode.value = EncryptionModes;
	    showTable(EncryptMode, 1);
	}
}

/* 重填 */
function Reset() {
	var frm = pptpSrvConfig;
	frm.reset();
	init();
}

/* 检查数据合法性 */
function checkData() {
	var f = pptpSrvConfig;
	if (!checkIP(f.poolStart,"<% write(AddrPoolInitAddr); %>",0)) return false;


	if (f.poolStart.value == "0.0.0.0") {
		alert("<% write(PoolInitAddrValErr); %>");
		return false;
	}
	if(Encrypt == 1)
	{
	    if((f.EncryptionMode.value == "MPPE") && (f.authType.value != "MS-CHAPV2"))
	    {
		alert("<% write(MS_CHAPV2MustMPPE); %>");
		return false;
	    }
	}
	/*
	if (f.localIp.value == lanIp) {
		alert("<% write(PoolInitAddrLanIPErr); %>");
		return false;
	}
	*/
	if (!checkMainDns(f.priDns,"<%write(com_MDNS);%>",0)) return false;     
	if (!checkIP(f.secDns,"<% write(com_SDNS);%>",1)) return false; 
	if(! checkNumber(f.MTU,1,1492,"MTU",0)) return false;
	if (!checkNumber(f.poolCount,1,maxPoolCnt,"<% write(AddrPoolCount); %>",0)) return false;
	return true;

}

/* 保存 */
function save() {
	var frm = pptpSrvConfig;
	if (checkData()) {
		frm.action = "/goform/pptpSrvGlobalConfig";
		frm.submit();
	}
}
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
  <td><script language="javascript">showSecondTitle("<% write(PPTPSSecondTitle); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,

						new Array("<% write(GolConf); %>",'PPTPSrvGlobal.asp',1,1),
						new Array("<% write(UserConf); %>",'PPTPSrvAccount.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="pptpSrvConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >


<table border="0" width="600" align='center'>
	<tr>
	   <td align="right"  height="25" width="40%" id="enableTd"><input type="hidden" name="Action" value="add">
	   <% write(StartUsingPPTPSev); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="24%" align="left"><input type="checkbox" name="enable" value="ENABLE"></td>
	</tr>	  
	   <tr>
	   <td align="right"  height="25" id="authTypetd"><% write(com_checkPasswd); %></td>
	   <td >&nbsp;</td>
	   <td ><select name="authType">
       <option value="PAP"><% write(com_pap); %></option>
	   <option value="CHAP"><% write(com_chap); %></option>
	   <option value="MS-CHAPV2" selected="selected"><% write(com_chapv2); %></option>
	   <option value="THRIN" ><% write(com_any); %></option>
	   </select></td>
	   <td></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="poolStartTd"><% write(AddrPoolInitAddr); %></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="poolStart" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(AddrPoolInitAddr); %>'),yutd1)"  value=""></td>
	   <td id="yutd1"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" id="poolCountTd"><% write(AddrPoolCount); %></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="poolCount" size="18" onKeyPress="figureOnly()" maxlength="15" onChange="showfiledtip(re_checkNumber(this,1, maxPoolCnt, '<% write(AddrPoolCount); %>'),yutd2)"  value=""></td>
	   <td id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="localIpTd"><% write(ServerIPAddr); %></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="localIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(ServerIPAddr); %>'),yutd3)"  value=""></td>
	   <td id="yutd3"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="priDnsTd"><% write(com_MDNS); %></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="priDns" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMainDns(this,'<% write(com_MDNS);%>'),yutd4)"  value=""></td>
	   <td id="yutd4"></td>
	</tr>

	   <tr>
	   <td align="right"  height="25" id="secDnsTd"><% write(com_SDNS); %></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="secDns" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<% write(com_SDNS);%>'),yutd5)"  value=""></td>
	   <td id="yutd5"></td>
	   </tr>	  
	   <tr name="EncryptMode" id="EncryptMode" style="display:none">
	   <td align="right"  height="25"><% write(EncryptionMode); %></td>
	   <td >&nbsp;</td>
	   <td > <select name="EncryptionMode">
	   <option value='None'><% write(NoneEncrypt); %></option>
	   <option value='MPPE' selected="selected"><% write(MPPEEncrypt); %></option>
	   </select></td>
	   <td id="yutd6"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25">MTU</td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="MTU" size="18" maxlength="4" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkNumber(this,'1','1492','MTU'),yutd7)"  value="1478"></td>
	   <td id="yutd7"></td>
	   </tr>	  
	   <tr>
	   <td align="right"  height="25"></td>
	   <td >&nbsp;</td>
	   <td ><%write(wan_MTUTip);%></td>
	   </tr>	  
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	  <tr><td  colspan="4">
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="100%"  height="30" align="center" valign="top">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PPTP')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('PPTPClientList.asp')" id="returnback"></td>
</tr>
</table>
  </td>
  </tr>
  <tr>
      <td colspan="4" align="center" height="30">&nbsp;</td>
  </tr>
  </table>	
	
	
  </td>
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
