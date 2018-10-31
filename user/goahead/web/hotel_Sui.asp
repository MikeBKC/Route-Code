<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("Menu");%>
<%langTran("hotel");%>
<title><%write(easySurfInternet);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/frame.js"></script> 
<script type="text/javascript" src="js/common.js"></script>
</head>
<style type="text/css"> 
.btn_nomal {
		background: #0099D2;	
}
.btn_loading {
		background: #aaaaaa;
}
</style>
<script type="text/javascript">
var ret = 0;
<% aspOutHotel(); %>
<% dhcpServerDetection(); %>

function init(){
	var switch_b = $(".on-off").children("button");
	initEnabledButton(switch_b,hotelPnpEn);	
	$("#hotelEnable").val(hotelPnpEn);	

showSuccessTips(rightstr);
}
function save(){
    var f=HotelConfig;
    var switch_b = $(".on-off").children("button");
    var status_now = switch_b.attr("class");
    f.action="/goform/formHotel";
    f.submit();
}
function getDetectionResult()
{
     $.ajax({
	type:"get",
	url:"/goform/formDetectionResult?date="+new Date().getTime(),
	success:function(data, textStatus){
	    eval(data);
	    if (ret == 2)
	    {
		$("#error_msg").html("<%write(hotel_warn1);%>");
	    }
	    else
	    {
		$("#error_msg").html("");
	    }
	    if (hotelPnpEn == "off")
	    {
		$("#savebtn").attr("disabled",false);
		$("#savebtn").addClass("btn_nomal").removeClass("btn_loading");
	    }
	},
	failure:function(data, textStatus){
	    $("#error_msg").html("");
	    if (hotelPnpEn == "off")
	    {
		$("#savebtn").attr("disabled",false);
		$("#savebtn").addClass("btn_nomal").removeClass("btn_loading");
	    }
	}	
    });
}
$(function(){
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
		    $("#hotelEnable").val("off");	
		}else{
		    $("#hotelEnable").val("on");	
		}
		swicth_act(status_now);
	});
	$('body').oneTime('5s','TimerC',getDetectionResult);
	$("#error_msg").html("<%write(hotel_waitCheck);%>");
	if (hotelPnpEn == "off")
	{
	    $("#savebtn").attr("disabled",true);
	    $("#savebtn").addClass("btn_loading").removeClass("btn_nomal");
	}
});
</script>
<body scroll="no">
<div style="width:500px; margin:0 auto; text-align:center;">

<br>
	<form action="" id="HotelConfig" method="post" name="HotelConfig">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;"class="group" width="500px">
   
    	<tr class="tab_info">
        	<td width="140px"><%write(easySurfInternet);%></td>
            <td colspan="2" width="360px">&nbsp;<span class="on-off"><button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="hotelEnable" id="hotelEnable" value="" /></span></td>
	</tr>
          <tr  height="60px" class="tab_savetd" valign="top"> 
        	<td colspan="3"  align="left" style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="500" >
    	<tr valign="top">
        	<td colspan="3" align="left"><span id="tips">1.<%write(hotelContent1);%>
<br>
2.<%write(hotelContent2);%><br>
3.<%write(hotelContent3);%><br><br></span></td>
	</tr>
		    <tr><td align="center" colspan="3"><span id="error_msg"  style="color:red;"></span></td></tr>
                	<tr>
                    	
                        <td align="right" colspan="3"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;</td>
                    </tr>
		<tr height="10px"><td></td><td></td></tr>
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
