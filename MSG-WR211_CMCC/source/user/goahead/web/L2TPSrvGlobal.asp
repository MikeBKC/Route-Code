<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>��̩�Ƽ�</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("internet");

var strSecondMenuFocus="L2TP������";

function initTranslation()
{
}

<% OutputL2tpSrvGlobalData(); %>
var lanIp = "<% getLanIp(); %>";
var lanNm = "<% getLanNetmask(); %>";


/* ��ʼ�� */
function init() {
	var frm = l2tpSrvConfig;
	frm.enable.checked = enables;
	frm.authType.value = authTypes;
	frm.poolStart.value = poolIpStarts;
	frm.poolCount.value = poolIpCnts;
	frm.localIp.value = localIps;
	frm.priDns.value = priDns;
	frm.secDns.value = secDns;
}

/* ���� */
function Reset() {
	var frm = l2tpSrvConfig;
	frm.reset();
	init();
}

/* ������ݺϷ��� */
function checkData() {
	var f = l2tpSrvConfig;
	if (!checkIP(f.poolStart,"��ַ����ʼ��ַ",0)) return false;
	if (f.poolStart.value == "0.0.0.0") {
		alert("��ַ����ʼ��ַ����Ϊ0.0.0.0");
		return false;
	}
	/*
	if (f.localIp.value == lanIp) {
		alert("�����IP��ַ������LAN��IP��ַ��ͬ");
		return false;
	}
	*/
	if (!checkNumber(f.poolCount,1,maxPoolCnt,"��ַ�ص�ַ��",0)) return false;
	if (!checkMainDns(f.priDns,"��DNS������",0)) return false;     
	if (!checkIP(f.secDns,"��DNS������",1)) return false; 
	return true;
}

/* ���� */
function save() {
	var frm = l2tpSrvConfig;
	if (checkData()) {
		frm.action = "/goform/l2tpSrvGlobalConfig";
		frm.submit();
	}
}
</script>
</head>

<body background="images/body_main.png">

<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td> 
  <!--////����-->
  <td><script language="javascript">showSecondTitle("L2TP������");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,

						new Array("ȫ������",'L2TPSrvGlobal.asp',1,1),
						new Array("�˺�����",'L2TPSrvAccount.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="l2tpSrvConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >


<table border="0" width="600" align='center'>
	<tr>
	   <td align="right"  height="25" width="40%" id="enableTd"><input type="hidden" name="Action" value="add">
	   ����L2TP������</td>
	   <td width="2%">&nbsp;</td>
	   <td width="24%" align="left"><input type="checkbox" name="enable" value="ENABLE"></td>
	</tr>	  
	   <!--<tr>
	   <td align="right"  height="25" id="protocolType">VPN����������</td>
	   <td >&nbsp;</td>
	   <td ><select name="proType">
	   <option value="PPTP">PPTP</option>
	   <option value="L2TP">L2TP</option>
	   <option value="BOTH">BOTH</option>
	   </select></td>
	   <td></td>
	   </tr>-->    	   
	   <!--<tr>
	   <td align="right"  height="25" width="40%" id="setnametd">�������</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="setName" size="18"  maxlength="31"   value="" onChange="showfiledtip(re_checkName(this,'�����'),yutd6)"></td><td width="34%"  id="yutd6"></td>
	   </tr><input type="hidden" name="oldSetName" >
  <tr>
	   <td align="right"  height="25" width="40%" id="usernametd">�û���</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="userName" size="18"  maxlength="31" onChange="showfiledtip(re_checkName(this,'�û���'),yutd1)"  value=""></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="passwdtd">����</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input  type="password" name="passwd" size="19"   maxlength="11" onChange="showfiledtip(re_checkPassword1(this,'����'),yutd2)" value=""></td><td  id="yutd2"></td>
	   </tr>-->
	   <tr>
	   <td align="right"  height="25" id="authTypetd">������֤��ʽ</td>
	   <td >&nbsp;</td>
	   <td ><select name="authType">
	   <option value="NONE">NONE</option>
	   <option value="PAP">PAP</option>
	   <option value="CHAP">CHAP</option>
	   <option value="EITHER" selected="selected">EITHER</option>
	   </select></td>
	   <td></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="poolStartTd">��ַ����ʼ��ַ</td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="poolStart" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'��ַ����ʼ��ַ'),yutd1)"  value=""></td>
	   <td id="yutd1"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" id="poolCountTd">��ַ�ص�ַ��</td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="poolCount" size="18" onKeyPress="figureOnly()" maxlength="15" onChange="showfiledtip(re_checkNumber(this,1, maxPoolCnt, '��ַ�ص�ַ��'),yutd2)"  value=""></td>
	   <td id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="localIpTd">�����IP��ַ</td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="localIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'�����IP��ַ'),yutd3)"  value=""></td>
	   <td id="yutd3"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="priDnsTd">��DNS������</td>
	   <td ></td>
	   <td ><input type="text" name="priDns" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMainDns(this,'��DNS������'),yutd6)"  value=""></td>
	   <td id="yutd6"></td>
	</tr>
	   <tr>
	   <td align="right"  height="25" id="secDnsTd">��DNS������</td>
	   <td ></td>
	   <td ><input type="text" name="secDns" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'��DNS������'),yutd7)"  value=""></td>
	   <td id="yutd7"></td>
	   </tr>	  
	   <!-- <tr>
	   <td align="right"  height="25" id="serverlaniptd">Զ�����������ַ</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="serverLanIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'Զ������IP��ַ'),yutd4)"  value=""></td><td  id="yutd4"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" id="servernetmasktd">Զ��������������</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="serverLanNetMask" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'Զ��������������'),yutd5)"  value=""></td><td  id="yutd5"></td>
	   </tr>
	   <tr style="display:none">
	   <td align="right"  height="25" id="serveriptd">�����������ַ(��)</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="serverIp" size="18"  maxlength="31" value="" onChange="showfiledtip(re_checkName(this,'�����������ַ(��)'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>-->
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	  <tr><td  colspan="4">
  <table width="29%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="92%"  height="30" align="right" valign="top"><input type="button" class="input-button" value="����" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="����" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="����" onClick="GoBack_Page('L2TPList.asp')" id="returnback"></td><td width="8%">&nbsp;</td>
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
