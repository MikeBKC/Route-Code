<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("DHCP"); %>
<% langTran("common"); %>
<title><%write(DHCP);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript">
<%aspDhcpServerConfig();%> 
/*
var lanDhcpTypes="off";
var dhcpStarts="192.168.1.100";
//var dhcpMasks="255.255.255.0";
var dhcpEnds="192.168.1.200";
//var dhcpGateways="192.168.1.1";
var dnspEnbls="Enable";
var dhcpLeases="86400";
var dhcpPriDnss="192.168.1.1";
var dhcpSecDnss="0.0.0.0";
//var ISPDnss="0.0.0.0";
//var ISPDnss2="0.0.0.0";
var errorstr=""; 
*/
$(function(){
    var switch_b = $(".on-off").children("button");
    var dhcpSwitch = lanDhcpTypes=="Enable" ? "on" : "off";
    /*2.4G*/
    if (typeof(ssid1_dhcp_enables) != 'undefined') {
//	$("#wireless_dhcp").show();
	$("#ssid1").show();
	$("#ssid2").show();
	/*ssid1*/
	$("#ssid1_dhcpStart").val(ssid1_dhcpStarts);
	$("#ssid1_dhcpEnd").val(ssid1_dhcpEnds);
	if (ssid1_dhcp_enables == "Disable") {
	    $("#ssid_dhcp_enable").prop("checked", false);
	    $("#ssid1_dhcp_switch").val("OFF");
	    $("#ssid1_dhcpStart").prop("disabled",true);
	    $("#ssid1_dhcpEnd").prop("disabled",true);
	} else {
	    $("#ssid_dhcp_enable").prop("checked", true);
	    $("#ssid1_dhcp_switch").val("ON");
	    $("#ssid1_dhcpStart").prop("disabled",false);
	    $("#ssid1_dhcpEnd").prop("disabled",false);
	}

	/*ssid2*/
	$("#ssid2_dhcpStart").val(ssid2_dhcpStarts);
	$("#ssid2_dhcpEnd").val(ssid2_dhcpEnds);
	if (ssid2_dhcp_enables == "Disable") {
	    $("#ssid2_dhcp_switch").val("OFF");
	    $("#ssid2_dhcpStart").prop("disabled",true);
	    $("#ssid2_dhcpEnd").prop("disabled",true);
	} else {
	    $("#ssid2_dhcp_switch").val("ON");
	    $("#ssid2_dhcpStart").prop("disabled",false);
	    $("#ssid2_dhcpEnd").prop("disabled",false);
	}

    } else {
//	$("#wireless_dhcp").hide();
	$("#ssid1").hide();
	$("#ssid2").hide();
    }

    /*5G*/
    if (typeof(ssid1_dhcp_enables_5g) != 'undefined') {
//	$("#wireless_dhcp_5g").show();
	$("#ssid1_5g").show();
	$("#ssid2_5g").show();
	/*ssid1*/
	$("#ssid1_dhcpStart_5g").val(ssid1_dhcpStarts_5g);
	$("#ssid1_dhcpEnd_5g").val(ssid1_dhcpEnds_5g);
	/*********************************************************/
	if (ssid1_dhcp_enables_5g == "Disable") {
	    $("#ssid1_dhcp_switch_5g").val("OFF");
	    $("#ssid1_dhcpStart_5g").prop("disabled",true);
	    $("#ssid1_dhcpEnd_5g").prop("disabled",true);
	} else {
	    $("#ssid1_dhcp_switch_5g").val("ON");
	    $("#ssid1_dhcpStart_5g").prop("disabled",false);
	    $("#ssid1_dhcpEnd_5g").prop("disabled",false);
	}

	/*ssid2*/
	$("#ssid2_dhcpStart_5g").val(ssid2_dhcpStarts_5g);
	$("#ssid2_dhcpEnd_5g").val(ssid2_dhcpEnds_5g);
	/*****************************************************/
	if (ssid2_dhcp_enables_5g == "Disable") {
	    $("#ssid2_dhcp_switch_5g").val("OFF");
	    $("#ssid2_dhcpStart_5g").prop("disabled",true);
	    $("#ssid2_dhcpEnd_5g").prop("disabled",true);
	} else {
	    $("#ssid2_dhcp_switch_5g").val("ON");
	    $("#ssid2_dhcpStart_5g").prop("disabled",false);
	    $("#ssid2_dhcpEnd_5g").prop("disabled",false);
	}
    } else {
//	$("#wireless_dhcp_5g").hide();
	$("#ssid1_5g").hide();
	$("#ssid2_5g").hide();
	
    }
	
	if((typeof(ssid1_dhcp_enables_5g) == 'undefined') && (typeof(ssid1_dhcp_enables) == 'undefined'))
	{
		$("#mutil_pool").hide();
		$("#ssid_dhcp_enable").hide();	
	}
	else
	{
		$("#mutil_pool").show();
		$("#ssid_dhcp_enable").show();	
		$("#ssid_dhcp_enable").click(function(){
				$(this).prop("checked",function(i,val){
					if (val) {
					$("#ssid1_dhcp_switch").val("ON");
					$("#ssid1_dhcpStart").prop("disabled",false);
					$("#ssid1_dhcpEnd").prop("disabled",false);
					/*ssid2*/
					$("#ssid2_dhcp_switch").val("ON");
					$("#ssid2_dhcpStart").prop("disabled",false);
					$("#ssid2_dhcpEnd").prop("disabled",false);
					/*5G ssid1*/
					$("#ssid1_dhcp_switch_5g").val("ON");
					$("#ssid1_dhcpStart_5g").prop("disabled",false);
					$("#ssid1_dhcpEnd_5g").prop("disabled",false);
					/*5G ssid2*/
					$("#ssid2_dhcp_switch_5g").val("ON");
					$("#ssid2_dhcpStart_5g").prop("disabled",false);
					$("#ssid2_dhcpEnd_5g").prop("disabled",false);
					} else {
					$("#ssid1_dhcp_switch").val("OFF");
					$("#ssid1_dhcpStart").prop("disabled",true);
					$("#ssid1_dhcpEnd").prop("disabled",true);
					/*ssid2*/
					$("#ssid2_dhcp_switch").val("OFF");
					$("#ssid2_dhcpStart").prop("disabled",true);
					$("#ssid2_dhcpEnd").prop("disabled",true);
					/*5G ssid1*/
					$("#ssid1_dhcp_switch_5g").val("OFF");
					$("#ssid1_dhcpStart_5g").prop("disabled",true);
					$("#ssid1_dhcpEnd_5g").prop("disabled",true);
					/*5G ssid2*/
					$("#ssid2_dhcp_switch_5g").val("OFF");
					$("#ssid2_dhcpStart_5g").prop("disabled",true);
					$("#ssid2_dhcpEnd_5g").prop("disabled",true);
					}
				});
		});
	}

	$("#ssid_dhcp_enable").click(function(){
	    $(this).prop("checked",function(i,val){
		if (val) {
		    $("#ssid1_dhcp_switch").val("ON");
		    $("#ssid1_dhcpStart").prop("disabled",false);
		    $("#ssid1_dhcpEnd").prop("disabled",false);
			/*ssid2*/
			$("#ssid2_dhcp_switch").val("ON");
		    $("#ssid2_dhcpStart").prop("disabled",false);
		    $("#ssid2_dhcpEnd").prop("disabled",false);
			/*5G ssid1*/
			$("#ssid1_dhcp_switch_5g").val("ON");
		    $("#ssid1_dhcpStart_5g").prop("disabled",false);
		    $("#ssid1_dhcpEnd_5g").prop("disabled",false);
			/*5G ssid2*/
			$("#ssid2_dhcp_switch_5g").val("ON");
		    $("#ssid2_dhcpStart_5g").prop("disabled",false);
		    $("#ssid2_dhcpEnd_5g").prop("disabled",false);
		} else {
		    $("#ssid1_dhcp_switch").val("OFF");
		    $("#ssid1_dhcpStart").prop("disabled",true);
		    $("#ssid1_dhcpEnd").prop("disabled",true);
			/*ssid2*/
			$("#ssid2_dhcp_switch").val("OFF");
		    $("#ssid2_dhcpStart").prop("disabled",true);
		    $("#ssid2_dhcpEnd").prop("disabled",true);
			/*5G ssid1*/
			$("#ssid1_dhcp_switch_5g").val("OFF");
		    $("#ssid1_dhcpStart_5g").prop("disabled",true);
		    $("#ssid1_dhcpEnd_5g").prop("disabled",true);
			/*5G ssid2*/
			$("#ssid2_dhcp_switch_5g").val("OFF");
		    $("#ssid2_dhcpStart_5g").prop("disabled",true);
		    $("#ssid2_dhcpEnd_5g").prop("disabled",true);
			
		}
	    });
	});

    initEnabledButton(switch_b,dhcpSwitch);
    $("#ebableddhcp").val(lanDhcpTypes = (lanDhcpTypes == "Enable" ? "SERVER" : "DISABLE"));	
    $("#dhcpStart").val(dhcpStarts);
    $("#dhcpEnd").val(dhcpEnds);
    $("#dhcpMask").val(dhcpMasks);
	$("#dhcpGateway").val(dhcpGateways);
    $("#dhcpLease").val(dhcpLeases);
    $("#dhcpPriDns").val(dhcpPriDnss);
    $("#dhcpSecDns").val(dhcpSecDnss);
    $("#error_msg").html(errorstr);
	
	
    $(".on-off").click(function(){
	var switch_b = $(".on-off").children("button");
	var status_now = switch_b.attr("class");
	if(status_now=="on"){
	    $("#ebableddhcp").val("DISABLE");	
	}else{
	    $("#ebableddhcp").val("SERVER");	
	}
	swicth_act(status_now);
    });
	
    function checkData(){
	var f= DhcpServer_Config;
	//alert(f.lanDhcpType.value);
	if(!checkIP_P(f.dhcpStart,"<%write(com_startIP);%>",0)) return false;
	if(!checkIP_P(f.dhcpEnd,"<%write(com_endIP);%>",0)) return false;
	if (typeof(ssid1_dhcp_enables) != 'undefined') {
	    if (f.ssid1_dhcp_switch.value == "ON"){
		if(!checkIP_P(f.ssid1_dhcpStart,"<%write(com_startIP);%>",0)) return false;
		if(!checkIP_P(f.ssid1_dhcpEnd,"<%write(com_endIP);%>",0))  return false;
	    }
	    if (f.ssid2_dhcp_switch.value == "ON"){
		if(!checkIP_P(f.ssid2_dhcpStart,"<%write(com_startIP);%>",0)) return false;
		if(!checkIP_P(f.ssid2_dhcpEnd,"<%write(com_endIP);%>",0)) return false;
	    }
	}
	if (typeof(ssid1_dhcp_enables_5g) != 'undefined') {
	    if (f.ssid1_dhcp_switch_5g.value == "ON") {
		if(!checkIP_P(f.ssid1_dhcpStart_5g,"<%write(com_startIP);%>",0)) return false;
		if(!checkIP_P(f.ssid1_dhcpEnd_5g,"<%write(com_endIP);%>",0)) return false;
	    }
	    if (f.ssid1_dhcp_switch_5g.value == "ON") {
		if(!checkIP_P(f.ssid2_dhcpStart_5g,"<%write(com_startIP);%>",0)) return false;
		if(!checkIP_P(f.ssid2_dhcpEnd_5g,"<%write(com_endIP);%>",0)) return false;
	    }
	}
	if(!checkMask_P(f.dhcpMask,"<%write(com_NetMask);%>",0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
	    $("#error_msg").html("<%write(com_warning5);%>");
	    f.dhcpStart.focus();
	    return false;
	}
	if (f.dhcpEnd.value=="0.0.0.0")
	{
	    $("#error_msg").html("<%write(com_warning6);%>");
	    f.dhcpEnd.focus();
	    return false;
	}
	if (compareIP(f.dhcpStart.value,f.dhcpEnd.value)==1)
	{
	    $("#error_msg").html("<%write(StartIPGTEndErr);%>");
	    return false;
	}   
	if (typeof(ssid1_dhcp_enables) != 'undefined') {
	    if (f.ssid1_dhcp_switch.value == "ON"){
		if (f.ssid1_dhcpStart.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning5);%>");
		    f.ssid1_dhcpStart.focus();
		    return false;
		}
		if (f.ssid1_dhcpEnd.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning6);%>");
		    f.ssid1_dhcpEnd.focus();
		    return false;
		}
		if (compareIP(f.ssid1_dhcpStart.value,f.ssid1_dhcpEnd.value)==1)
		{
		    $("#error_msg").html("<%write(StartIPGTEndErr);%>");
		    f.ssid1_dhcpStart.focus();
		    return false;
		}   
	    }
	    if (f.ssid2_dhcp_switch.value == "ON"){
		if (f.ssid2_dhcpStart.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning5);%>");
		    f.ssid2_dhcpStart.focus();
		    return false;
		}
		if (f.ssid2_dhcpEnd.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning6);%>");
		    f.ssid2_dhcpEnd.focus();
		    return false;
		}
		if (compareIP(f.ssid2_dhcpStart.value,f.ssid2_dhcpEnd.value)==1)
		{
		    $("#error_msg").html("<%write(StartIPGTEndErr);%>");
		    f.ssid2_dhcpStart.focus();
		    return false;
		}   
	    }
	}
	if (typeof(ssid1_dhcp_enables_5g) != 'undefined') {
	    if (f.ssid1_dhcp_switch_5g.value == "ON") {
		if (f.ssid1_dhcpStart_5g.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning5);%>");
		    f.ssid1_dhcpStart_5g.focus();
		    return false;
		}
		if (f.ssid1_dhcpEnd_5g.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning6);%>");
		    f.ssid1_dhcpEnd_5g.focus();
		    return false;
		}
		if (compareIP(f.ssid1_dhcpStart_5g.value,f.ssid1_dhcpEnd_5g.value)==1)
		{
		    $("#error_msg").html("<%write(StartIPGTEndErr);%>");
		    f.ssid1_dhcpStart_5g.focus();
		    return false;
		}   
	    }
	    if (f.ssid2_dhcp_switch_5g.value == "ON") {
		if (f.ssid2_dhcpStart_5g.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning5);%>");
		    f.ssid2_dhcpStart_5g.focus();
		    return false;
		}
		if (f.ssid2_dhcpEnd_5g.value=="0.0.0.0")
		{
		    $("#error_msg").html("<%write(com_warning6);%>");
		    f.ssid2_dhcpEnd_5g.focus();
		    return false;
		}
		if (compareIP(f.ssid2_dhcpStart_5g.value,f.ssid2_dhcpEnd_5g.value)==1)
		{
		    $("#error_msg").html("<%write(StartIPGTEndErr);%>");
		    f.ssid2_dhcpStart_5g.focus();
		    return false;
		}   
	    }
	}
        if (!checkIP_P(f.dhcpGateway,"<%write(com_GateWayAddr);%>",1)) return false;
	if (!checkNumber_P(f.dhcpLease,1,86400,"<%write(ReimbursedTime);%>",0)) return false;
	if (!checkMainDns_P(f.dhcpPriDns,"<%write(com_MDNS);%>",0)) return false;
	if (!checkIP_P(f.dhcpSecDns,"<%write(com_SDNS);%>",1)) return false;

	    
	return true;
    }


    $("#savebtn").click(function(){
	if(checkData()){
	    DhcpServer_Config.action="/goform/formDhcpServerConfig";
	    DhcpServer_Config.submit();	
	}

    });
showSuccessTips(rightstr);
});
</script>
</head>

<body>
<div class="warp">

<br>
	<form action="" method="post" name="DhcpServer_Config">
	<!--
     <div id="error_msg" style="color:red;"></div>
     -->
	<table border="0" cellpadding="0" cellspacing="1" width="500px" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info">
        	<td width="120px"><%write(dhcp_open);%></td>
            <td colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="lanDhcpType" id="ebableddhcp" value="" /></span></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(dhcp_pool);%></td>
            <td colspan="2"><input type="text"  onKeyPress="ipOnly()" class="txt-input" name="dhcpStart" style="width:130px;" id="dhcpStart" value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text"   onKeyPress="ipOnly()" class="txt-input" name="dhcpEnd" style="width:130px;" id="dhcpEnd" value="" />&nbsp;&nbsp;&nbsp;</td>
        </tr>
		<tr class="tab_info" id="mutil_pool">
			<td><%write(multiple_dhcp_pool);%></td>
			<td colspan="2"><input type="checkbox" name="ssid_dhcp_switch" id="ssid_dhcp_enable"/></td>
		</tr>
	<div id="wireless_dhcp">
        <tr class="tab_info" id="ssid1">
        	<td><%write(ssid1_dhcp_pool);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="ssid1_dhcpStart" style="width:130px;" id="ssid1_dhcpStart" value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text"  class="txt-input" name="ssid1_dhcpEnd" style="width:130px;" id="ssid1_dhcpEnd" value="" />&nbsp;&nbsp;&nbsp;<input type="hidden"  name="ssid1_dhcp_switch" id="ssid1_dhcp_switch" value="OFF"/><!--&nbsp;<%write(enable);%>--></td>
        </tr>
        <tr class="tab_info" id="ssid2">
        	<td><%write(ssid2_dhcp_pool);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="ssid2_dhcpStart" style="width:130px;" id="ssid2_dhcpStart" value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text"  class="txt-input" name="ssid2_dhcpEnd" style="width:130px;" id="ssid2_dhcpEnd" value="" />&nbsp;&nbsp;&nbsp;<input type="hidden"  name="ssid2_dhcp_switch" id="ssid2_dhcp_switch" value="OFF"/><!--&nbsp;<%write(enable);%>--></td>
        </tr>
	</div>
	<div id="wireless_dhcp_5g">
        <tr class="tab_info" id="ssid1_5g">
        	<td><%write(ssid1_dhcp_pool_5g);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="ssid1_dhcpStart_5g" style="width:130px;" id="ssid1_dhcpStart_5g" value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text"  class="txt-input" name="ssid1_dhcpEnd_5g" style="width:130px;" id="ssid1_dhcpEnd_5g" value="" />&nbsp;&nbsp;&nbsp;<input type="hidden" name="ssid1_dhcp_switch_5g" id="ssid1_dhcp_switch_5g" value="OFF"/><!--&nbsp;<%write(enable);%>--></td>
        </tr>
        <tr class="tab_info" id="ssid2_5g">
        	<td><%write(ssid2_dhcp_pool_5g);%></td>
            <td colspan="2"><input type="text"  class="txt-input" name="ssid2_dhcpStart_5g" style="width:130px;" id="ssid2_dhcpStart_5g" value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text"  class="txt-input" name="ssid2_dhcpEnd_5g" style="width:130px;" id="ssid2_dhcpEnd_5g" value="" />&nbsp;&nbsp;&nbsp;<input type="hidden" name="ssid2_dhcp_switch_5g" id="ssid2_dhcp_switch_5g" value="OFF"/><!--&nbsp;<%write(enable);%>--></td>
        </tr>
	</div>
        <tr class="tab_info">
        	<td><%write(com_NetMask);%></td>
            <td colspan="2"><input type="text" name="dhcpMask" id="dhcpMask"  onKeyPress="ipOnly()" class="txt-input" maxlength="15"  value=""><span  id="masktd"></span></td>
 </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(com_GateWayAddr);%></td>
            <td colspan="2"><input type="text"  onKeyPress="ipOnly()"  class="txt-input" name="dhcpGateway" id="dhcpGateway" style="width:130px;"  value="" /></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(ReimbursedTime);%></td>
            <td colspan="2">
            	<input type="text"  class="txt-input" name="dhcpLease"  onKeyPress="figureOnly()" style="width:80px;" id="dhcpLease" maxlength="6" value="" />&nbsp;&nbsp;s
            </td>
        </tr>
        <tr class="tab_info">
        	<td><%write(dhcp_MDNSServ);%></td>
            <td colspan="2"><input type="text"  onKeyPress="ipOnly()" class="txt-input" name="dhcpPriDns" style="width:130px;" id="dhcpPriDns" value="" /></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(dhcp_SDNSServ);%></td>
            <td colspan="2"><input type="text"  onKeyPress="ipOnly()" class="txt-input" name="dhcpSecDns" style="width:130px;" id="dhcpSecDns" value="" /></td>
        </tr>
       
	<!--
        <tr  height="60px" class="tab_savetd">
        	<td colspan="3" style="padding-left:400px;" ><input type="button" class="btn" name="back" id="savebtn" value="保存"/></td>
            
        </tr>
	-->
        <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<td width="170px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="save" id="savebtn" value="<%write(com_save);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
    </table>
    </form>
<br>
<br>
</div>
</body>
</html>
