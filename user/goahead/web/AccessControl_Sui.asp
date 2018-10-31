<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("AccessControl");%>
<title><%write(title_AccessControl);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
<%aspOutAccessControl();%>

function init(){
	var f=AccessControl;
	var switch_b = $(".on-off").children("button");
	if(accessEnable == "on")
	{
	    f.accessEnablew.checked = true;
	}
	else
	{
	    f.accessEnablew.checked = false;
	    accessEnable = "off";
	}
	initEnabledButton(switch_b,accessEnable);
	$("#accessEnablew").val(accessEnable);
	$("#DesFromIP").val(allowFromIP);
	$("#DesEndIP").val(allowEndIP);
	f.ArpBroadcastIntervalVal.value = lanArpBroadcastInterval;

	showSuccessTips(rightstr);
}

function checkData(){
	
	var f=AccessControl;
	var ctl=f.accessEnablew;

	/*if(!checkNumber(f.ArpBroadcastIntervalVal, 100, 5000, "ARP广播间隔", 0)) {
	    return false;
	}*/
	if (ctl.value=="on" && !checkIP_P(f.DesFromIP, "<%write(start_IP);%>", 0)) {
	    return false;
	}
	if (ctl.value=="on" && !checkIP_P(f.DesEndIP, "<%write(end_IP);%>", 0)) {
	    return false;
	}
	if (compareIP(f.DesFromIP.value, f.DesEndIP.value) > 0 && ctl.value=="on") {
	    $("#error_msg").html("<%write(compare_IP);%>");
	    return false;
	}
	return true;
}
function save(){
    var f=AccessControl;
    if(checkData()){
	f.action="/goform/formAccessCtrl";
	f.submit();
    }
}
$(function(){
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
		    $("#accessEnablew").val("off");	
		}else{
		    $("#accessEnablew").val("on");	
		}
		swicth_act(status_now);
	});
});
</script>
<body>
<div style="width:500px; margin:0 auto; text-align:center;">

<br>
	<form action="" id="AccessControl" method="post" name="AccessControl">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;"class="group" width="500px">
   
    	<tr class="tab_info">
        	<td width="140px"><%write(AccessControl_open);%></td>
            <td colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="accessEnablew" id="accessEnablew" value="" /></span></td>
        </tr>

	<tr class="tab_info">
		<td><%write(allowIP_MASK);%></td>
            <td colspan="2"><input type="text"  onKeyPress="ipOnly()" class="txt-input" name="DesFromIP" style="width:130px;" id="DesFromIP" value="" />&nbsp;&nbsp;-&nbsp;&nbsp;<input type="text"   onKeyPress="ipOnly()" class="txt-input" name="DesEndIP" style="width:130px;" id="DesEndIP" value="" />&nbsp;&nbsp;&nbsp;<input type="hidden" name="ArpBroadcastIntervalVal" id="ArpBroadcastIntervalVal" value=0 /></td>
        </tr>

          <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="450">
		    <tr valign="top" >
			<td colspan="3" align="left"><span id="tips"><%write(accessctrl_warn);%></span></td>
		    </tr>
		    <tr>	
                    	<td width="100px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/></td>
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
