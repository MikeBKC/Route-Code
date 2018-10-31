<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("TimeWireless");%>
<%langTran("SysInfo");%>
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
var wlanworktimeEn ="1";
<% outputSystemNowTime();%>
<% outputWlanTimeConfig(); %>
//进行时间处理
var sysDate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
var Year= sysDate.getFullYear();
var Month= sysDate.getMonth();
var Day= sysDate.getDate();
var week= sysDate.getDay();
if (10 > sysDate.getHours()) {
    Hour="0"+sysDate.getHours();
} else {
    Hour=""+sysDate.getHours();
}
if (10 > sysDate.getMinutes()) {
    Minute="0"+sysDate.getMinutes();
} else {
    Minute=""+sysDate.getMinutes();
}
if (10 > sysDate.getSeconds()) {
    Second="0"+sysDate.getSeconds();
} else {
    Second=""+sysDate.getSeconds();
}
Month1=Month+1;//路由器内部0表示1月，1表示2月……
if(week < 1) week = 6;else week-=1;
var systimes=Year.toString()+"-"+Month1.toString()+"-"+Day.toString()+"&nbsp;"+Hour.toString()+":"+Minute.toString()+":"+Second.toString()+"&nbsp;"+DateArray[week].toString();
function init(){
	var f = TimedWireless_Config;
var switch_b = $(".on-off").children("button");
	var en = "off";
	if (wlanworktimeEn == "1")
	{
	    en = "on";
	}
	initEnabledButton(switch_b,en);
	$("#WrlessEnable").val(wlanworktimeEn);	
	for (i = 0; i < days.length; i++) {
		if (days.charAt(i) == "1") f.date[i].checked = true;
	    }    
	    var index1 = weekdaytimestarts.indexOf(":");
	    var index2 = weekdaytimestops.indexOf(":");
	    f.weekdayfromhour.value = weekdaytimestarts.substring(0, index1);
	    f.weekdayfromminute.value = weekdaytimestarts.substring(index1 + 1);
	    f.weekdayendhour.value = weekdaytimestops.substring(0, index2);
	    f.weekdayendminute.value = weekdaytimestops.substring(index2 + 1);
 
	    index1 = weekendtimestarts.indexOf(":");
	    index2 = weekendtimestops.indexOf(":");
	    f.weekendfromhour.value = weekendtimestarts.substring(0, index1);
	    f.weekendfromminute.value = weekendtimestarts.substring(index1 + 1);
	    f.weekendendhour.value = weekendtimestops.substring(0, index2);
	    f.weekendendminute.value = weekendtimestops.substring(index2 + 1);
		$("#sys_time").html(systimes);
	
	
showSuccessTips(rightstr);
}
function save(){
	var f = TimedWireless_Config;
	 var len = f.date.length;
		    for (i = 0; i < len; i++) 
		    {
			f.day.value += (f.date[i].checked == true ? "1": "0");
		    }    
		    f.weekdaytimestart.value = f.weekdayfromhour.value + ":" +f.weekdayfromminute.value;
		    f.weekdaytimestop.value = f.weekdayendhour.value + ":" + f.weekdayendminute.value;
		    f.weekendtimestart.value = f.weekendfromhour.value + ":" + f.weekendfromminute.value;
		    f.weekendtimestop.value = f.weekendendhour.value + ":" + f.weekendendminute.value;
	    f.action="/goform/ConfigWirelessTime_Sui";
	    f.submit();
	
}

$(function(){
	init();	
		$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
			$("#WrlessEnable").val("0");	
		}else{
			$("#WrlessEnable").val("1");	
		}
		swicth_act(status_now);
	});
	})
</script>
<body>
<div class="warp">

<br>
	<form action="" method="post" name="TimedWireless_Config">
    
	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info">
        	<td width="180px"><%write(twl_open);%></td>
            <td width="480px" colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="WrlessEnable" id="WrlessEnable" value="" /></span>&nbsp;&nbsp;<%write(systemnow);%>&nbsp;&nbsp;<span id="sys_time"></span></td>
        </tr>
        <tr class="tab_title">
        	<td colspan="3"><%write(twl_openTime);%></td>
           
        </tr>
        <tr class="tab_info" height="70px">
        	<td><%write(twl_workDay);%></td>
            <td colspan="2">
            		<div style="padding-bottom:5px"> <input type="checkbox" name="date" value="1"><%write(com_monday);%>&nbsp;
		    <input type="checkbox" name="date" value="2"><%write(com_tuesday);%>&nbsp;
		    <input type="checkbox" name="date" value="3"><%write(com_wednesday);%>&nbsp;
		    <input type="checkbox" name="date" value="4"><%write(com_thursday);%>&nbsp;
		    <input type="checkbox" name="date" value="5"><%write(com_friday);%>
					</div>
                    <div>
                    	<select name="weekdayfromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekdayfromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<%write(com_to);%>&nbsp;<select name="weekdayendhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekdayendminute" class="list"><script language="javascript">CreateHOp(60);</script></select><input type="hidden" name="day"><input type="hidden" name="weekdaytimestart"><input type="hidden" name="weekdaytimestop">


                    </div>
            </td>
           
        </tr>
        <tr class="tab_info" height="70px">
        	<td><%write(twl_weekend2);%></td>
            <td colspan="2">
            		<div style="padding-bottom:5px">
                    	<input type="checkbox" name="date" value="6"><%write(com_saturday);%>&nbsp;
		    			<input type="checkbox" name="date" value="7"><%write(com_sunday);%>
	

                    </div>
                    <div>
                    	 <select name="weekendfromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekendfromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<%write(com_to);%>&nbsp;<select name="weekendendhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekendendminute" class="list"><script language="javascript">CreateHOp(60);</script></select><input type="hidden" name="weekendtimestart"><input type="hidden" name="weekendtimestop">

                    </div>
            </td>
        </tr>
       
       
         <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<!--<td width="70px">&nbsp;</td>-->
                        <td align="center" colspan="2" ><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right" width="200px"><input type="button" class="btn" name="back" onclick="save();"  value="<%write(com_save);%>"/></td>
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
