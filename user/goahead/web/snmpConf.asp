 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("snmpConf");%>
<%langTran("commonWords");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%aspOutPutGetSysSnmpConf();%>
//end交互
function init() {
    if (onSnmp == 1) frm_sys.snmp.checked = true;
    frm_sys.snmpTrap.value = snmpTrap;
    frm_sys.readCommunity.value = readCommunity;
    frm_sys.writeCommunity.value = writeCommunity;
    frm_sys.trapCommunity.value = trapCommunity;
//    snmp_on();
}
/**
Cancel时将所有输入框的错误提示去掉
**/
function reRightAll()
{
	allInputRight(frm_sys);//所有文本输入框
}
function snmp_on() {
    if (!frm_sys.snmp.checked) {
		frm_sys.snmpTrap.value = snmpTrap;
		frm_sys.readCommunity.value = readCommunity;
    	frm_sys.writeCommunity.value = writeCommunity;
    	frm_sys.trapCommunity.value = trapCommunity;
		allInputRight(frm_sys);
        frm_sys.readCommunity.disabled = true;
        frm_sys.writeCommunity.disabled = true;
        frm_sys.trapCommunity.disabled = true;
        frm_sys.snmpTrap.disabled = true;
    } else {
        frm_sys.readCommunity.disabled = false;
        frm_sys.writeCommunity.disabled = false;
        frm_sys.trapCommunity.disabled = false;
        frm_sys.snmpTrap.disabled = false;
    }
}
function save() {
   if (frm_sys.snmp.checked) {
        if (!checkIP(frm_sys.snmpTrap, '<%write(snmp_trap_host);%>')) {
            return;
        }
        if (!checkName(frm_sys.readCommunity, 15, '<%write(snmp_read_name);%>', 1)) {
            return;
        }
        if (!checkName(frm_sys.writeCommunity, 15, '<%write(snmp_write_name);%>', 1)) {
            return;
        }
        if (!checkName(frm_sys.trapCommunity, 15, '<%write(snmp_trap_name);%>', 1)) {
            return;
        }
    }
	frm_sys.action="/goform/formSysSnmp";
    frm_sys.submit();
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="<%write(com_bodyMain);%>" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
    <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(snmp_config);%>");//显示页面名称</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//显示Save返回的错误信息</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	<tr><td class="gapTd"></td><td align="center">
	<form method="post" name="frm_sys">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="sysTime">
<br>
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr><td width="133"  align="left" valign="bottom" ><b><%write(snmp);%></b></td>
</tr>
<tr><td colspan="2" height="10"></td></tr>
</TABLE>
<TABLE  width="600"  border="0"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr >
<td class=vncell height="22" width="26%"><%write(snmp_enable);%></td>
<td width="74%" class=vtable><input type="checkbox" name="snmp" value="1" ></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="26%"><%write(snmp_trap_host);%></td> 
<td class=vtable><input type="text" class="input" name="snmpTrap" onKeyPress="ipOnly()" maxlength="15" size="16" value="" onChange="checkIP(this,'<%write(snmp_trap_host);%>')"><div id="snmptrap"></div></td>
</tr>
<tr ><td class=vncell height="22" width="26%"><%write(snmp_read_name);%></td>
<td class=vtable>
<input type="text" class="input" name="readCommunity" size="16" maxlength="155555" value="" onChange="checkName(this, 15, '<%write(snmp_read_name);%>',1)"><div id="snmpRname"></div></td></tr>
<tr class="style18"><td class=vncell height="22" width="26%"><%write(snmp_write_name);%></td>
<td class=vtable><input type="text" class="input" name="writeCommunity" size="16" maxlength="15" value=""onChange="checkName(this, 15, '<%write(snmp_write_name);%>',1)"><div id="snmpWname"></div></td></tr>
<tr ><td class=vncell height="22" width="26%"><%write(snmp_trap_name);%></td>
<td class=vtable>
<input type="text" class="input" name="trapCommunity" size="16" maxlength="15" value="" onChange="checkName(this, 15, '<%write(snmp_trap_name);%>',1)"><div id="snmpname"></div></td></tr>
</table>
<table align="center"  ><tr><td height="10"></td></tr><tr><td>
<input type="button"  onClick="save()"class="input-button" value="<%write(com_save);%>" name="">&nbsp;
<!--<input type="button" class="input-button" value="Refresh" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(com_reset);%>" name="" onClick="init();reRightAll();">
<input type="button" class="input-button" value="<%write(com_help);%>" name="" onClick="window.open('<%write(com_helppage);%>#snmpConf')">
</td></tr></table>
</form>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 </td><td class="gapTd"></td></tr> </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>
</body>
</html>
