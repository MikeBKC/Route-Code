<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("LANConfig");%>
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="script/frame.js"></script>
<script type="text/javascript" src="script/menu.js" ></script>
</head>
<script type="text/jscript">
var multiLan = 0;
<%outputLANConfig(); %>
<%aspoutDynConfig(); %>
/*
var multiLan = "1";
var lanIps = "192.168.1.1";
var lanNMs = "255.255.255.0";

var lanMacs = "0022aac798e5";

var errorstr="";
var enabledDnsPro ="on";
*/
function init(){
	var f=LAN_Config;
	f.lanIp.value = lanIps;
	f.lanNetmask.value = lanNMs;
	
	f.lanMac.value = lanMacs;
	f.enabledDns.checked = (dnspEnbls=="Enable"?true:false);
	if(middlesoft == 0)
    {
        //f.checkIP.checked = (CheckIPEnbls=="Enable"?true:false);
	}
    $("#error_msg").html(errorstr);
        showSuccessTips(rightstr);
	
	if(lang_en == 1){
		$("#winaso").attr("href","http://www.loginutt.com");
		$("#winaso").html("http://www.loginutt.com");
		if(DomainAccess == 1)
		{
			 $("#winaso").attr("href","http://www.router.level1.com");
			 $("#winaso").html("http://www.router.level1.com");
		}
	}
	
	if(galaxywind_ctrl == 1)
	{
		$("#winasotr").hide();
	}
    if(middlesoft == 1)
    {
        $("#winasotr").hide();
    }
}
function checkData(){
	var f=LAN_Config;
	if (f.lanIp.value=="0.0.0.0"){
		 $("#error_msg").html("<%write(com_checkIPAlert);%>");return false
		};
	if (!checkLanIP_P(f.lanIp,"<%write(com_IP);%>",0)) return false;
	if (!checkMask_P(f.lanNetmask,"<%write(com_NetMask);%>",0)) return false;
	if(!checkMac_P(f.lanMac,"<%write(com_MAC);%>",0)) return false;
	if (f.enabledDns.checked) {
	    f.dnsAgencyEnable.value = "on";
	} else {
	    f.dnsAgencyEnable.value = "off";
	}
	if(f.checkIP.checked){
	     f.CheckAgencyEnable.value = "on";
	}
	else{
	    f.CheckAgencyEnable.value = "off";
	} 
	return true;

}

$(function(){
    $("#savebtn").click(function(){
	var f=LAN_Config;
	if (checkData()) {
	   f.action="/goform/ConfigLANConfig" 
	   f.submit();
	}
    });	
});

$(function(){
	init();	
	$("input[name='lanIp']").get(0).focus();
})
</script>
<body>
<div class="warp">

<br>
	<form action="" method="post" name="LAN_Config">
     
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info">
        	<td width="140px"><%write(com_IP);%></td> <!-- onChange="showfiledtip(re_checkIP(this,'IP地址'),iptd)"-->
        
        <td colspan="2"><input type="text" name="lanIp" size="18" onKeyPress="ipOnly()" maxlength="15" class="txt-input"   value=""> <span width="33%"  id="iptd">
       </span>
       <!-- &nbsp;&nbsp;<%write(lan_deal);%>&nbsp;&nbsp; --><input style="display:none" type="checkbox" id="checkIP" name="checkIP" value=""/><input type="hidden" name="CheckAgencyEnable"> 
	    </td>
        </tr>
        <tr class="tab_info">
        	<td><%write(com_NetMask);%></td><!-- onChange="showfiledtip(re_checkMask(this,'子网掩码'),masktd)"-->
            <td colspan="2"><input type="text" name="lanNetmask" size="18" onKeyPress="ipOnly()" class="txt-input" maxlength="15"  value=""><span  id="masktd"></span></td>
 </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(com_MAC);%></td>
            <td colspan="2"><!--onChange="showfiledtip(re_checkMac(this,'MAC地址'),mactd)" -->
            	<input type="text" name="lanMac" size="18" onKeyPress="macOnly()" maxlength="17" class="txt-input" ><span  id="mactd"></span></td>

            
        </tr>
        <tr class="tab_info">
        	<td><%write(lan_openDNS);%></td>
            <td colspan="2"><input type="checkbox"   name="enabledDns"  id="enabledDns" value="" /><input type="hidden" name="dnsAgencyEnable"></td>
        </tr>

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
