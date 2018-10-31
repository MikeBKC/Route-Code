<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("DMZ");%>
<title><%write(dmz_set);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
//var DMZIPs="0.0.0.0";var DMZEnables="1";var errorstr="";
<% aspOutDMZIP(); %>
DMZEnables = (DMZEnables=="1"?"on":"off");

function init(){
	var switch_b = $(".on-off").children("button");
	initEnabledButton(switch_b,DMZEnables);	
	var f=DMZConfig;
	//f.DMZEnable.checked=(DMZEnables=="on"?true:false);
	$("#ebableddmz").val(DMZEnables);	
	f.DMZIP.value=DMZIPs;
	showSuccessTips(rightstr);

//showSuccessTips(rightstr);
}
function checkData(){
	
	var f=DMZConfig;
	var switch_b = $(".on-off").children("button");
	var status_now = switch_b.attr("class");
	if (f.DMZIP.value=="0.0.0.0"){ $("#error_msg").html("<%write(dmz_ipTip);%>");return false};
	if (!checkIP_P(f.DMZIP,"<%write(com_IP);%>",0)) return false;
	return true;
}
function save(){
    var f=DMZConfig;
    if(checkData()){
	f.action="/goform/formDMZIP";
	f.submit();
    }
}
$(function(){
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
		    $("#ebableddmz").val("off");	
		}else{
		    $("#ebableddmz").val("on");	
		}
		swicth_act(status_now);
	});
});
</script>
<body>
<div style="width:500px; margin:0 auto; text-align:center;">

<br>
	<form action="" id="DMZConfig" method="post" name="DMZConfig">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;"class="group" width="500px">
   
    	<tr class="tab_info">
        	<td width="150px"><%write(dmz_open);%></td>
            <td colspan="2"><span class="on-off">&nbsp;<button type="button" class="on">开关</button><input type="hidden" name="DMZEnable" id="ebableddmz" value="" /></span></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(dmz_hostIP);%></td>
            <td colspan="2"><input type="text" class="txt-input" name="DMZIP" size="18" onKeyPress="ipOnly()" maxlength="15" value="0.0.0.0"> </td>
           
        </tr>
       
       
          <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="450">
                	<tr>
                    	<td width="170px">&nbsp;</td>
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
