<HTML><HEAD>
<%langTran("common");%>
<%langTran("SNMP");%>
<TITLE><%write(com_utt);%></TITLE>
<META http-equiv="content-type" content="text/html; charset=gb2312">
<script language="javascript"> 
var strMainMenuFocus = 3;
var strSecondMenuFocus = 6;
</script>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script  language="javascript">
<% aspOutPutGetSNMP(); %>
var lanPortCount = 5;
var Start_button = 1;
var is840 = 0;
var VlancommID = "0";
var PushEnabled = 1;
var StartMenuTotalrecs = 0;var StartMenuInst = new Array();var StartMenuUrl = new Array();var StartMenuOutput = new Array();
StartMenuTotalrecs = 0;
platformName = "4210G";
needlicense = 0;
var EnableRateLimits = "No";
var EnableQos = "No";
var systemHostname = "";
var errMsg = "";
midId = 7208959;
var ProductType = "HiPER ";
var ProductID = "4210G";
var ProductVer = "kv4210Gv12.3.bin";
var isSupportDMZ = 0;
var isSupportCBQ = 0;
var isSupportIPSSG = 1;
var isSupportVPN = 1;
var isSupportDHCP = 0;
var isSupportL3Switch = 1;
var isSupportUPnP = 1;
var isSupportIPSEC = 1;
var isSupportXuanlin = 0;
var isSupportPPPoEServer = 1;
var isSupportDOT1X = 0;
var isSupportSwitch = 1;
var isSupportPolicyRoute = 0;
var isSupportRRCP = 0;
var isSupportHotel = 0;
var DBIspTotalrecs = 1;
</script>
<style>

</style>
</HEAD>
<body onLoad="init()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(snmp_service);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td>

	
	
 <form name="SNMPConfig" method="post" action=""><br />
 
 <table width="80%" border="0" cellspacing="0" cellpadding="0" class="style7" align="center">
 <tr>
 <td width="16%">&nbsp;</td>
 <td colspan="3">&nbsp;</td>
 <td width="31%">&nbsp;</td>
 </tr>
 <tr>
 <td height="29" align="right" >&nbsp;</td>
 <td width="26%" height="29" align="right">
    <%write(snmp_enbale);%></td>
 <td width="2%">&nbsp;</td>
 <td width="22%" align="left">
 <input name="SNMPEnable" type="checkbox"  id="SNMPEnable" onClick=enableSNMP()>
 </td>
 <td height="29">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right" >&nbsp; </td>
 <td height="31" align="right">
 <%write(snmp_RoComm);%></td>
 <td height="31">&nbsp;</td>
 <td height="31" align="left">
 <input name="RoComm" type="text" id="RoComm" value="uTt22aA" size="18">
 </td>
 <td height="31">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right" >&nbsp; </td>
 <td height="31" align="right">
 <%write(snmp_dev_name);%></td>
 <td height="31">&nbsp;</td>
 <td height="31" align="left">
 <input name="RoHost" type="text"  id="RoHost" value="" size="18" maxlength="31">
 </td>
 <td height="31">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right" >&nbsp; </td>
 <td height="31" align="right">
 <%write(snmp_RoContact);%></td>
 <td height="31" >&nbsp;</td>
 <td height="31" align="left">
 <input name="RoContact" type="text"  id="RoContact" value="" size="18" maxlength="31">
 </td>
 <td height="31">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right" >&nbsp; </td>
 <td height="31" align="right">
 <%write(snmp_RoLocation);%></td>
 <td height="31">&nbsp;</td>
 <td height="31" align="left">
 <input name="RoLocation" type="text"  id="RoLocation" value="" size="18" maxlength="31">
</td>
 <td height="31">&nbsp;</td>
 </tr>
 <tr>
 <td height="29" align="right">&nbsp;</td>
 <td height="29" align="right">
   <%write(snmp_RsHost);%></td>
 <td height="29">&nbsp;</td>
 <td height="29" align="left">
 <input name="RsHost" type="checkbox"  id="RsHost"onClick=setenable()>
 </td>
 <td height="29">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right" >&nbsp;</td>
 <td height="31" align="right"> <%write(snmp_host1);%></td>
 <td height="31">&nbsp;</td>
 <td height="31" align="left">
 <input name="server1" type="text"  onKeyPress="ipOnly()" value="0.0.0.0" size="18">
 </td>
 <td height="31">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right" >&nbsp;</td>
 <td height="31" align="right"> <%write(snmp_host2);%></td>
 <td height="31">&nbsp;</td>
 <td height="31" align="left">
 <input name="server2" type="text"   onKeyPress="ipOnly()" value="0.0.0.0" size="18">
 </td>
 <td height="31">&nbsp;</td>
 </tr>
 <tr>
 <td height="31" align="right">&nbsp;</td>
 <td height="31" align="right"> <%write(snmp_host3);%></td>
 <td height="31">&nbsp;</td>
 <td height="31" align="left">
 <input name="server3" type="text"   onKeyPress="ipOnly()" value="0.0.0.0" size="18">
 </td>
 <td height="31">&nbsp;</td>
 </tr>
 
 <tr>
 <td height="31" align="right" colspan="5">
 <table width="80%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
 <tr>
 <td height="40" align="center"><input name="save1" type="button" value="<%write(com_save);%>" class="input-button" onClick="save()"/>&nbsp;
 <input name="save1" type="button" value="<%write(com_reset);%>" class="input-button" onClick="Reset()"/>
 </td>
 </tr>
 </table>
 </td>
 </tr>
 
 <tr>
 <td height="31" align="right" colspan="5">
 <table  width="100%" border="0" align="center">
 <tr>
 <td height="40" align="center"><b> <%write(snmp_tips);%></b> <%write(snmp_tip_cnt);%></td>
 </tr>
 <tr><td></td></tr>
 </table>
 </td>

 </tr>
 </table><br />

 
  <input type="hidden" name="temp1" value=1>
 </form>

    
</td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
 <script language="javascript"> 
 
var maxHostNameLen = "31";
var color = "Green";
function setenable()//"允许主机管理"对应的控件属性
{
 if (SNMPConfig.RsHost.checked && SNMPConfig.SNMPEnable.checked)
 {
 SNMPConfig.server1.readOnly =false;
 SNMPConfig.server2.readOnly =false;
 SNMPConfig.server3.readOnly =false;
  }
 else
 {
 SNMPConfig.server1.readOnly =true;
 SNMPConfig.server2.readOnly =true;
 SNMPConfig.server3.readOnly =true;
 }
}
function enableSNMP()
{
  setenable();
}
function init()
{
 if (eval("0")) alert("<%write(snmp_route_err);%>");
 SNMPConfig.SNMPEnable.checked=(snmp_enable.toLowerCase() == "1"?true:false);
 SNMPConfig.RoComm.value=community ;
 SNMPConfig.RoHost.value=dev_name;
 SNMPConfig.RoContact.value =contactor;
 SNMPConfig.RoLocation.value=position;

 SNMPConfig.RsHost.checked=(host_manage_enable.toLowerCase() == "1"?true:false);
 SNMPConfig.server1.value=host1;
 SNMPConfig.server2.value=host2;
 SNMPConfig.server3.value=host3;
 enableSNMP();
 setenable();
}
 init();
function Reset(){
	SNMPConfig.reset();
	init();
	}
 
function save()
{
 if(checkData())
 {
 SNMPConfig.action = "/goform/SNMPConfig";
 SNMPConfig.submit();
 }
}
var maxlen_name = "31";
function checkData()
{
  if(SNMPConfig.RoComm.value.length>31)
 {
	alert("<%write(snmp_RoComm_len);%>");
	return false;
 }
 if (!checkName(SNMPConfig.RoComm,maxlen_name,"<%write(snmp_RoComm);%>",0)) return false;
 
 if (SNMPConfig.RoHost.value.lstrLen() > 31) {
 alert("<%write(snmp_dev_name_len_len);%>");
 return false;
 }
 if (SNMPConfig.RoContact.value.lstrLen() > 31 ||
 SNMPConfig.RoLocation.value.lstrLen() > 31) {
 alert("<%write(snmp_RoContact_RoLocation);%>");
 return false;
 }
 if (SNMPConfig.RsHost.checked==true) {
 if (!checkIP(SNMPConfig.server1,"<%write(snmp_host1);%>",0)) return false;
 if(SNMPConfig.server1.value=="0.0.0.0")
 {
 alert("<%write(snmp_host_no_zero);%>");
 SNMPConfig.server1.focus();
 return false;
 }
 } else {
 if (!checkIP(SNMPConfig.server1,"<%write(snmp_host1);%>",1)) return false;
 }
 if (!checkIP(SNMPConfig.server2,"<%write(snmp_host2);%>",1)) return false;
 if (!checkIP(SNMPConfig.server3,"<%write(snmp_host3);%>",1)) return false;
 return true;
}
 
function Reset()
{
 SNMPConfig.reset();
 init();
}
function check()
{
 enableSNMP();
 setenable();
}
function help() {
}
</script>

