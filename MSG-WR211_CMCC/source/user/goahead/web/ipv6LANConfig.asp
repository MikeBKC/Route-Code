<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("ipv6LANConfig");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//---------交互开始-------------
//var ipv6LAN_addr = "";
//var LANprefixLength = 64;

//var errorstr="";

<%aspOutputIpv6Lan(); %>

//----------交互结束---------------------

function init(){
	var f = ipv6LANConfig;
        f.LanAutoEnabled.checked = (LanAutoEnabled == "1")?true:false;
	f.ipv6LAN_addr.value = ipv6LAN_addr;
	f.LANprefixLength.value = LANprefixLength;

	}
	


function checkDatas(){
	var f = ipv6LANConfig;
        f.ipv6LAN_addr.value = f.ipv6LAN_addr.value.trim();	       //去掉地址前后空格
	if (!checkNumber(f.LANprefixLength,1,127,"<%write(ipv6lan_prefix);%>",0)) return false;
	    if (!checkIpv6(f.ipv6LAN_addr,"<%write(ipv6_addr);%>",0)) return false;
	    if (!checkIpv6Sp(f.ipv6LAN_addr,"<%write(ipv6_addr);%>")) return false;
	if (ipv6WAN_addr != ""){
	    if(WANprefixLength != f.LANprefixLength.value){
		if(ipv6WAN_addr == f.ipv6LAN_addr.value){
		    alert("<%write(alert_one);%>");
                    return false;
		}
	    }else{
		if (checkIpv6Subnet2(ipv6WAN_addr,f.ipv6LAN_addr.value,f.LANprefixLength.value)){
		    alert("<%write(alert_two);%>");
		    return false;
		}

	    }      
	}
	return true;
	
}
function save(){
	if(checkDatas()){
		ipv6LANConfig.action = "/goform/formIpv6Lan";
		ipv6LANConfig.submit();
		}
}

function Reset(){
	ipv6LANConfig.reset();
	init();
 
	}
function ConfigDel(){
    if(confirm("<%write(com_deleteTip);%>")){
	ipv6LANConfig.action="/goform/Ipv6LanDel";
	ipv6LANConfig.submit();
    }
}
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(ipv6lan_config);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form name="ipv6LANConfig" method="post">
<table width="80%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" bordercolor="green" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	
     <tr>
       <td align="right" width="44%" height="25"><%write(LanAdvAuto);%></td>
       <td  width="2%">&nbsp;</td>
       <td width="12%" ><input type="checkbox" name="LanAutoEnabled"  id=LanAutoEnabled"></td>
    </tr>

	<tr>
	   <td align="right" width="35%" height="25"><%write(ipv6_addr);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6LAN_addr" id="ipv6LAN_addr" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(ipv6_addr);%>',0),iptd)"></td><td id="iptd"></td>
	</tr>
   
    <tr>
	   <td align="right" height="25"><%write(ipv6_prefix);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="LANprefixLength" id="LANprefixLength" type="text" size="10" maxlength="63" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,1,127,'<%write(ipv6lan_prefix);%>'),yutd)" > </td><td  id="yutd"></td>
	</tr>
    
    
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	    <td height="30" align="center" valign="top">
	    <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Syslog')">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_delete);%>" class="input-button" onClick="ConfigDel()">
	    </td>
	   </tr>
	</table>
	</td></tr>
    </table>
    </td></tr>
</table>
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
