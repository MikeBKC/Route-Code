<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("SysClock");%>
<%langTran("SysInfo");%>
<title><%write(SysClock);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 

<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="js/dyn_config.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
var errorstr="";
<% getClockInfo(); %>
function init(){
    var f=SysClock_Config;
    f.time_zone.value=Timezones;
    sysdate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
    year= sysdate.getFullYear();
    if(year < 2011)year = 2011;
    month= sysdate.getMonth();
    day= sysdate.getDate();
    if (10 > sysdate.getHours())
	    hour="0"+sysdate.getHours();
    else
	    hour=""+sysdate.getHours();
    if (10 > sysdate.getMinutes())
	    minute="0"+sysdate.getMinutes();
    else
	    minute=""+sysdate.getMinutes();
    if (10 > sysdate.getSeconds())
	    second="0"+sysdate.getSeconds();
    else
	    second=""+sysdate.getSeconds();
    month1=month+1;//路由器内部0表示1月，1表示2月……
    var data=year.toString()+"-"+month1.toString()+"-"+day.toString();		
    var time =hour.toString()+":"+minute.toString()+":"+second.toString();
    $("#sysTime").html(data+" "+time);
    f.year.value=year;
    f.month.value=month;
    changeMonth(month);
    f.day.value=day;
    f.hour.value=""+hour;
    f.minute.value=""+minute;
    f.second.value=""+second;
    if(sntp_enables=="on")
    {
	f.SntpEnable[1].checked=true;
    }
    else
    {
	f.SntpEnable[0].checked=true;    
    }
	if(dst ==1){
		$(".tab_dst").show();
		if(dst_enable==1){
			f.dstEnable.checked=true;
		}
		else
		{
			f.dstEnable.checked=false;	
		}
	}else{
		$(".tab_dst").hide();
	}
    server1s=(server1s==""?"192.43.244.18":server1s);
    server2s=(server2s==""?"129.6.15.28":server2s);
    f.NTPServerIP.value=server1s;
    f.server2.value=server2s;;
    f.server3.value=(server3s==""?"0.0.0.0":server3s);
    setenable();
showSuccessTips(rightstr);
}

function setenable(){
    var f=SysClock_Config;
    if (!f.SntpEnable[1].checked) {
	f.NTPServerIP.disabled =true;
	f.server2.disabled =true;
	f.server3.disabled =true;
	f.year.disabled=false;
	f.month.disabled=false;
	f.day.disabled=false;
	f.hour.disabled=false;
	f.minute.disabled=false;
	f.second.disabled=false;
    } else{
	f.NTPServerIP.disabled =false;
	f.server2.disabled =false;
	f.server3.disabled =false;
	f.year.disabled=true;
	f.month.disabled=true;
	f.day.disabled=true;
	f.hour.disabled=true;
	f.minute.disabled=true;
	f.second.disabled=true;
    }
}
function changeMonth(val){
	var tbl=SysClock_Config;
	if(val==3||val==5||val==8||val==10){
		creatday(tbl.day,30);
	}
	else if(val==1){
		var yearval=tbl.year.value;
		if(yearval%400==0||((yearval%4==0)&&(yearval%100!=0)))
			creatday(tbl.day,29);
		else 
			creatday(tbl.day,28);
	}
	else
	creatday(tbl.day,31);
}
function changeyear(val){
    var tbl=SysClock_Config;
    var monthval=tbl.month.value;

    if(monthval==3||monthval==5||monthval==8||monthval==10){
	creatday(tbl.day,30);
    } else if(monthval==1){
	if(val%400==0||((val%4==0)&&(val%100!=0))) {
	    creatday(tbl.day,29);
	} else {
	    creatday(tbl.day,28);
	}
    } else {
	creatday(tbl.day,31);
    }
}
function creatday(TAB,num)
{ 
    if(num>0){
	delSelectList(TAB,false); 
     	for (i=1;i<=num;i++)
	{
	    var option1=document.createElement("OPTION");
	    option1.text=i;
	    option1.value=i;
	    TAB.options.add(option1);
	}
    }
}
function checkData(){
    var f=SysClock_Config;
    var sysDate = new Date();
    if (f.hour.value.length==1)
	f.hour.value="0"+f.hour.value;
    if (f.minute.value.length==1)
	f.minute.value="0"+f.minute.value;
    if (f.second.value.length==1)
	f.second.value="0"+f.second.value;
    if (f.month.value==1 && (f.day.value==30 || f.day.value==31))
    {
	$("#error_msg").html("<%write(ErrorDate);%>");
	return false;
    }
    
    if (f.SntpEnable[0].checked == true) {
	if(!checkHour_P(f.hour))		return false;
	if(!checkMinute_P(f.minute))		return false;
	if(!checkSecond_P(f.second))		return false;
    }
    if(f.time_zone.value=="") {$("#error_msg").html("请选择时区");return false;}
    
    sysDate.setFullYear(eval(f.year.value),eval(f.month.value),eval(f.day.value));
    sysDate.setHours(eval(f.hour.value));
    sysDate.setMinutes(eval(f.minute.value));
    sysDate.setSeconds(eval(f.second.value));
    //alert(eval(Date.parse(sysDate.toUTCString()))/1000);
    (eval(f.month.value)+1)
    f.SysDateTime1.value ="\"" + f.year.value +"-"+ (eval(f.month.value)+1).toString() +"-"+ f.day.value + " "+f.hour.value +":"+ f.minute.value +":"+ f.second.value+"\"";
    if (f.SntpEnable[1].checked)/*开启网路时钟同步*/
    {
      if (!checkIP_P(f.NTPServerIP,"<%write(Server1IP);%>",0)) return false;
      if (!checkIP_P(f.server2,"<%write(Server2IP);%>",0)) return false;
      if (!checkIP_P(f.server3,"<%write(Server3IP);%>",1)) return false;
    }
    return true;
}
function save(){
    var f = SysClock_Config;
    if(checkData()){
	f.action = "/goform/NTP";
	f.submit();
    }	
}
$(function(){
    init();
    $("#error_msg").html(errorstr);
});
</script>
<body>
<div class="warp" style="width:650px">

<br>
	<form action="" method="post" name="SysClock_Config">

	<table border="0" cellpadding="0" cellspacing="1" id="runstatusInfo" style="background:#5e5e5e;width:650px;" class="group" >
   
    	<tr class="tab_info">
        	<td width="210px"><%write(systemnow);%></td>
            <td colspan="2" id="sysTime"></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(SelZone);%></td>
            <td colspan="2"><select name="time_zone" size="1" class="select" style="width:300px;" id="time_zone">
				  <option value="-39600">UTC-1100(<% write(Place_11); %>)</option>
				  <option value="-36000">UTC-1000(<% write(Place_10); %>)</option>
				  <option value="-32400">UTC-0900(<% write(Place_9); %>)</option>
				  <option value="-28800">UTC-0800(<% write(Place_8); %>)</option>
				  <option value="-25200">UTC-0700(<% write(Place_7); %>)</option>
				  <option value="-21600">UTC-0600(<% write(Place_6); %>)</option>
				  <option value="-18000">UTC-0500(<% write(Place_5); %>)</option>
				  <option value="-14400">UTC-0400(<% write(Place_4); %>)</option>
				  <option value="-12600">UTC-0330(<% write(Place_35); %>)</option>
				  <option value="-10800">UTC-0300(<% write(Place_3); %>)</option>
				  <option value="-7200">UTC-0200(<% write(Place_2); %>)</option>
				  <option value="-3600">UTC-0100(<% write(Place_1); %>)</option>
				  <option value="0">UTC+0000(<% write(Place0); %>)</option>
				  <option value="3600">UTC+0100(<% write(Place1); %>)</option>
				  <option value="7200">UTC+0200(<% write(Place2); %>)</option>
				  <option value="10800">UTC+0300(<% write(Place3); %>)</option>
				  <option value="12600">UTC+0330(<% write(Place35); %>)</option>
				  <option value="14400">UTC+0400(<% write(Place4); %>)</option>
				  <option value="16200">UTC+0430(<% write(Place45); %>)</option>
				  <option value="18000">UTC+0500(<% write(Place5); %>)</option>
				  <option value="19800">UTC+0530(<% write(Place55); %>)</option>
				  <option value="21600">UTC+0600(<% write(Place6); %>)</option>
				  <option value="23400">UTC+0630(<% write(Place65); %>)</option>
				  <option value="25200">UTC+0700(<% write(Place7); %>)</option>
				  <option value="28800">UTC+0800(<% write(Place8); %>)</option>
				  <option value="32400">UTC+0900(<% write(Place9); %>)</option>
				  <option value="34200">UTC+0930(<% write(Place95); %>)</option>
				  <option value="36000">UTC+1000(<% write(Place10); %>)</option>
				  <option value="39600">UTC+1100(<% write(Place11); %>)</option>
				  <option value="43200">UTC+1200(<% write(Place12); %>)</option>
				   </select></td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(ManuallySetTime);%></td>
            <td colspan="2" align="left" style="padding-left:0px;" >
            	<table width="100%" border="0" cellspacing="0" cellpadding="0" >
                    <tr>
                        <td align="left">
						<input name="SntpEnable" type="radio" value="off" onClick="setenable()" checked="checked">
						<select name="year" class="select" style="width:60px;" onChange="changeyear(this.value)" >
                              <script language="javascript">
                              for(i=2011;i<=2020;i++){
                                   document.write('<option value='+i+'>'+i+'</option>');
                              }
                              </script>
                            </select> <%write(RYearShort);%></td>
                        <td><select name="month" class="select" style="width:45px;" onChange="changeMonth(this.value)">
                        <script language="javascript">
                              for(i=1;i<=12;i++){
                                   document.write('<option value='+(i-1)+'>'+i+'</option>');
                              }
                              </script>
                              </select> <%write(RMouShort);%></td>
                        <td><select name="day" class="select" style="width:45px;">
                            <script language="javascript">
                              for(i=1;i<=31;i++){
                                   document.write('<option value='+i+'>'+i+'</option>');
                              }
                              </script>
                              </select> <%write(RDayShort);%></td>
                        <td><input name="hour" type="text" class="txt-input" onKeyPress="HourOnly(this)" style="width:30px;" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time" />:
                    <input name="minute" type="text" class="txt-input" style="width:30px;" onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time" />:
                      <input name="second" type="text" class="txt-input" style="width:30px;" onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time" /></td>
                  </tr> 
                </table>
            </td>
        </tr>
        <tr class="tab_info">
        	<td><%write(Timed);%></td>
            <td colspan="2"><input name="SntpEnable" type="radio" value="on" onClick="setenable();"></td>
        </tr>
		<tr class="tab_info tab_dst">
			<td><%write(EnableDaylightSavingTime);%></td>
			<td><input type="checkbox" name="dstEnable" id="dstEnable" /></td>
		</tr>
        <tr class="tab_info">
        	<td><%write(Server1IP);%></td>
            <td colspan="2"><input name="NTPServerIP" type="text" disabled class="txt-input" style="width:130px;" onKeyPress="ipOnly()" value="192.43.244.18" size="18" maxlength="15" /></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(Server2IP);%></td>
            <td colspan="2"><input name="server2" type="text" disabled class="txt-input" style="width:130px;" onKeyPress="ipOnly()" value="129.6.15.28" size="18" maxlength="15" /></td>
        </tr>
        <tr class="tab_info">
        	<td><%write(Server3IP);%></td>
            <td colspan="2"><input name="server3" type="text"  class="txt-input" style="width:130px;" disabled class="input" onKeyPress="ipOnly()" size="18" maxlength="15"  /><input name="NTPSync" value="1" type="hidden"></td>
        </tr>
        <tr  class="tab_savetd_tip"  valign="top"> 
        	<td colspan="3" align="left"  style="padding-left:10px; padding-top:4px;">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr valign="top">
                    	<td colspan="3" align="left"><span id="tips"><%write(warning);%></span></td>
                    </tr>
                	<tr>
                   	<td width="170px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
    </table>
    <input type="hidden" name="SysDateTime1">

    </form>
<br>
<br>
</div>
</body>
</html>
