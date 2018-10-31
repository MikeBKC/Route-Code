<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FwBase");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

//Butterlate.setTextDomain("admin"); 

//function initTranslation()
//{
// 
//}

<% aspOutSecDefense(); %>

var globalCheckTypeFlag = 0;
function enable(obj)
{
	if(obj.checked)
    {
     	obj.checked=confirm("<%write(fwb_startTip);%>");      
    }
    change_mode();

}
function change_mode()
{
	var f=FwBase_Config;
	if(f.accessEnablew.checked)
	{
		f.DesFromIP.disabled = false;
		f.DesEndIP.disabled = false;
		f.DesFromIP.style.backgroundColor = "#ffffff";
		f.DesEndIP.style. backgroundColor = "#ffffff";
	}
	else
	{
		f.DesFromIP.disabled = true;
		f.DesEndIP.disabled = true;
		f.DesFromIP.style.backgroundColor = "#cccccc";
		f.DesEndIP.style.backgroundColor = "#cccccc";
	}
}
function checkAccessCtl()
{
	var f=FwBase_Config;
	var ctl=f.accessEnablew;
	var ip=trimstr(f.DesFromIP.value);
	var ip2=trimstr(f.DesEndIP.value);

	if(!checkNumber(f.ArpBroadcastIntervalVal, 100, 5000, "<% write(fwb_arpBroadcast); %>", 0)) {
		return false;
	}
	if (ctl.checked && !checkLanIP(f.DesFromIP, "<%write(com_ipFrom);%>", false)) {
		return false;
	}
	if (ctl.checked && !checkLanIP(f.DesEndIP, "<%write(com_ipTo);%>", false)) {
		return false;
	}
	if (compareIP(ip, ip2) > 0 && ctl.checked) {
		alert("<%write(fwb_cmpTip);%>");
		return false;
	}
    return true;

}
function save(){
    var f=FwBase_Config;
    if(!checkAccessCtl()) return;//设备访问控制
    if(f.max_udp_rxpps.value > 99999 || f.max_udp_rxpps.value < 10)
    {
	alert("<%write(fwb_udp_thresh);%>");
	    return ;
    }
    if(f.max_icmp_rxpps.value > 99999 || f.max_icmp_rxpps.value < 10)
    {
	alert("<%write(fwb_icmp_thresh);%>");
	    return ;
    }
    if(f.max_syn_rxpps.value > 99999 || f.max_syn_rxpps.value < 10)
    {
	alert("<%write(fwb_syn_thresh);%>");
	    return ;
    }
    if(f.scanfCnt.value > 2000 || f.scanfCnt.value < 100)
    {
	alert("<%write(fwb_portscanf_thresh);%>");
	return;
    }
    f.DDOSEnable.value=(f.DDOSEnablew.checked==true?"on":"off"); 
    f.IPCheat.value=(f.IPCheatw.checked==true?"on":"off"); 
    f.UDPFlood.value=(f.UDPFloodw.checked==true?"on":"off"); 
    f.ICMPFlood.value=(f.ICMPFloodw.checked==true?"on":"off"); 
    f.SYNFlood.value=(f.SYNFloodw.checked==true?"on":"off"); 
    f.portScanfEnable.value=(f.portScanfEnablew.checked==true?"on":"off"); 
    f.ArpBroadcastEnable.value=(f.ArpBroadcastEnablew.checked==true?"on":"off");

    FwBase_Config.action ="/goform/formFwBase";
    FwBase_Config.submit();
}
function init()
{
    var f=FwBase_Config;
    f.DDOSEnablew.checked=(DDOSEnables.toLowerCase()=="on"?true:false);
    f.IPCheatw.checked=(IPCheatw.toLowerCase()=="on"?true:false);
    f.UDPFloodw.checked=(UDPFloodw.toLowerCase()=="on"?true:false);
    if(max_udp_rxpps == 0)
    {
	max_udp_rxpps = 500;
    }
    f.max_udp_rxpps.value = max_udp_rxpps;
    f.ICMPFloodw.checked=(ICMPFloodw.toLowerCase()=="on"?true:false);
    if(max_icmp_rxpps == 0)
    {
	max_icmp_rxpps = 500;
    }
    f.max_icmp_rxpps.value = max_icmp_rxpps;
    f.SYNFloodw.checked=(SYNFloodw.toLowerCase()=="on"?true:false);
    if(max_syn_rxpps == 0)
    {
	max_syn_rxpps = 500;
    }
    f.max_syn_rxpps.value = max_syn_rxpps;
    f.ArpBroadcastEnablew.checked=(ArpBroadcastEnables.toLowerCase()=="on"?true:false);
    f.ArpBroadcastIntervalVal.value = lanArpBroadcastInterval;
    f.DesFromIP.value = allowFromIP;
    f.DesEndIP.value = allowEndIP;
    f.accessEnablew.checked=(accessEnable.toLowerCase()=="on"?true:false);
    f.portScanfEnablew.checked=(portScanfEnablew.toLowerCase()=="on"?true:false);
    if(scanfCnt == 0)
    {
	scanfCnt = 100;
    }
    f.scanfCnt.value = scanfCnt;
    change_mode();

}



</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(fwb_safeConf);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td>		
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"90%",
						2,
						new Array("<%write(fwb_inner);%>",'FwBase.asp',1,1),
						new Array("<%write(fwb_outer);%>",'FwBaseOut.asp',1,0)
		);
		showThreeMenu();
	</script>
	
<table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td" valign="top">
	<table border="0" width="600" align='center' cellspacing="0" cellpadding="0">
	 <form action=""  name="FwBase_Config" method="post" >
    <tr><td><table cellspacing="0" cellpadding="0"><tr><td colspan="3" height="30">&nbsp;</td></tr></table></td></tr>

  <tr><td colspan="4" align="center"><table width="650" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr> 
      <td colspan="7" height="25" align="left"><strong><%write(fwb_virus);%></strong></td>
  </tr> 

  <tr style="display">
    <td width="91" align="right" ><input name="DDOSEnablew" type="checkbox"><input name="DDOSEnable" type="hidden"></td>
    <td width="10"></td>
    <td width="<% write(fwb_width1);%>"><%write(fwb_ddos);%></td>
    <td colspan="4">&nbsp;</td>
  </tr>

  <tr style="display">
    <td width="91" align="right" ><input name="IPCheatw" type="checkbox"><input name="IPCheat" type="hidden"></td>
    <td width="10"></td>
    <td width="<% write(fwb_width1);%>"><%write(fwb_IPCheat);%></td>
    <td colspan="4">&nbsp;</td>
  </tr>

  <tr style="display">
    <td width="91" align="right" ><input name="UDPFloodw" type="checkbox"><input name="UDPFlood" type="hidden"></td>
    <td width="10"></td>
    <td width="<% write(fwb_width1);%>"><%write(fwb_udpFlood);%></td>
    <td  width="<% write(fwb_width2);%>" align="right"><%write(fwb_thresh);%></td>
    <td  width="73" align="center"><input type="text" value="500"  name="max_udp_rxpps" id="max_udp_rxpps"  size="5" /></td>
    <td  width="35"><%write(fwb_perSec);%>&nbsp;</td>
    <td  width="178">&nbsp; </td>
  </tr>

  <tr style="display">
    <td width="91" align="right" ><input name="ICMPFloodw" type="checkbox"><input name="ICMPFlood" type="hidden"></td>
    <td width="10"></td>
    <td width="<% write(fwb_width1);%>"><%write(fwb_icmpFlood);%></td>
    <td  width="<% write(fwb_width2);%>" align="right"><%write(fwb_thresh);%></td>
    <td  width="73" align="center"><input type="text" value="500"  name="max_icmp_rxpps" id="max_icmp_rxpps"  size="5" /></td>
    <td  width="35"><%write(fwb_perSec);%>&nbsp;</td>
    <td  width="178">&nbsp; </td>
  </tr>


  <tr style="display">
    <td width="91" align="right" ><input name="SYNFloodw" type="checkbox"><input name="SYNFlood" type="hidden"></td>
    <td width="10"></td>
    <td width="<% write(fwb_width1);%>"><%write(fwb_synFlood);%></td>
    <td  width="<% write(fwb_width2);%>" align="right"><%write(fwb_thresh);%></td>
    <td  width="73" align="center"><input type="text" value="500"  name="max_syn_rxpps" id="max_syn_rxpps"  size="5" /></td>
    <td  width="35"><%write(fwb_perSec);%>&nbsp;</td>
    <td  width="178">&nbsp; </td>
  </tr>

    <tr>
    <td width="91" align="right" ><input name="ArpBroadcastEnablew" type="checkbox"><input name="ArpBroadcastEnable" type="hidden"></td>
  	<td width="10"></td>
	<td  width="<% write(fwb_width1);%>"><% write(fwb_ArpCheat); %></td>
	<td  width="<% write(fwb_width2);%>" align="right"><% write(fwb_arpBroadcast); %></td>
	<td  width="73" align="center"><input type="text" value="100"  name="ArpBroadcastIntervalVal" id="ArpBroadcastIntervalVal"  size="5" /></td>
	<td  width="35"><% write(fwb_ms); %>&nbsp;</td>
	<td  width="178">&nbsp;</td>
  </tr>

 </table></td></tr>
 <tr>
     <td colspan="4" align="center"><table width="650" border="0" cellspacing="0" cellpadding="0" align="center">
         <tr>
             <td  colspan="7" height="25" align="left"><strong><%write(fwb_control);%></strong></td>
         </tr>
         <tr>
             <td width="91" align="right" ><input name="accessEnablew" type="checkbox" onClick="enable(this)"></td>
             <td width="10">&nbsp;</td>
             <td width="<% write(fwb_width1);%>"><%write(fwb_devCtl);%></td>
             <td width="<% write(fwb_width2);%>" align="right"><%write(com_ipFrom);%></td>
             <td width="73" align="center"><input class="input" type="text" name="DesFromIP" onKeyPress="ipOnly()" size="15" maxlength="15" value="0.0.0.0"></td>
             <td width="35" align="left"><%write(com_toIP);%></td>
             <td width="170"><input class="input" type="text" name="DesEndIP" onKeyPress="ipOnly()" size="15" maxlength="15" value="0.0.0.0"></td>
         </tr>
     </table></td>
 </tr>
 <tr>
    <td colspan="4" align="center"><table width="650" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr>
	    <td height="25" colspan="7" align="left"><strong><%write(fwb_otherCtl);%></strong></td>
	</tr>
	<tr>
	    <td width="91" align="right" ><input name="portScanfEnablew" type="checkbox"><input name="portScanfEnable"type="hidden" ></td>
	    <td width="10"></td>
	    <td width="<% write(fwb_width1);%>"><%write(fwb_portScan);%></td>
	    <td width="<% write(fwb_width2);%>" align="right"><%write(fwb_thresh);%></td>
	    <td width="73" align="center"><input type="text" value="100"  name="scanfCnt" id="scanfCnt"  size="5" /></td>
	    <td width="35"><%write(fwb_msec);%> </td>
	    <td width="220" >&nbsp;</td>
	</tr>
    </table></td>
  </tr>
  
  <tr><td colspan="3" >&nbsp;</td></tr>
	 
  <tr>
  <td colspan="3" height="10" align="center">
  <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#FwBaseHl')">
  </td> 
  </tr> 

  <tr><td colspan="3" height="10">&nbsp;</td></tr>
  <script language="javascript">init();</script></form>
</table></td></tr></table>
</td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
