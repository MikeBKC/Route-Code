<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("SysClock"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script  type="text/javascript" src="js/common.js"></script>
<script type="text/javascript">
var errorstr="";
<% getClockInfo(); %>
<!--
function init()
{ 
	var f=SysClock_Config;
	f.time_zone.value=Timezones;
	sysDate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
	Year= sysDate.getFullYear();
	if(Year < 2011)Year = 2011;
	Month= sysDate.getMonth();
	Day= sysDate.getDate();
	if (10 > sysDate.getHours())
		Hour="0"+sysDate.getHours();
	else
		Hour=""+sysDate.getHours();
	if (10 > sysDate.getMinutes())
		Minute="0"+sysDate.getMinutes();
	else
		Minute=""+sysDate.getMinutes();
	if (10 > sysDate.getSeconds())
		Second="0"+sysDate.getSeconds();
	else
		Second=""+sysDate.getSeconds();
	Month1=Month+1;//路由器内部0表示1月，1表示2月……
	f.date.value=Year.toString()+"-"+Month1.toString()+"-"+Day.toString();		
	f.time.value=Hour.toString()+":"+Minute.toString()+":"+Second.toString();
	f.year.value=Year;
	f.month.value=Month;
	changeMonth(Month);
	f.day.value=Day;
	f.hour.value=""+Hour;
	f.minute.value=""+Minute;
	f.second.value=""+Second;
	if (dst_switch == "on") 
	{
	    document.getElementById("dst").style.display = "";
	    f.dstEnable.checked = (dst_enable == "1") ? true : false;
	}
	if(sntp_enables=="on")
	{
		f.SntpEnable[1].checked=true;
		
	}
	else
	{
		f.SntpEnable[0].checked=true;
		
	}
	server1s=(server1s==""?"192.43.244.18":server1s);
	server2s=(server2s==""?"129.6.15.28":server2s);
	f.NTPServerIP.value=server1s;
	f.server2.value=server2s;;
	f.server3.value=(server3s==""?"0.0.0.0":server3s);
	setenable();
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
		f.dstEnable.disabled = true;
   }
   else{
		f.NTPServerIP.disabled =false;
		f.server2.disabled =false;
		f.server3.disabled =false;
		f.year.disabled=true;
		f.month.disabled=true;
		f.day.disabled=true;
		f.hour.disabled=true;
		f.minute.disabled=true;
		f.second.disabled=true;
		f.dstEnable.disabled = false;
   }
}
function Reset()
{
	SysClock_Config.reset();
	ip1tb.innerHTML = "";
	ip2tb.innerHTML = "";
	ip3tb.innerHTML = "";
	init();
}
function save(){
	if(checkData())
	{
	  SysClock_Config.action = "/goform/NTP";
	  SysClock_Config.submit();
	}
}
function checkData(){
	var f=SysClock_Config;
	var dstEnableObj = f.dstEnable;

	dstEnableObj.value = dstEnableObj.checked == true ? "on" : "off";
	if (f.hour.value.length==1)
	    f.hour.value="0"+f.hour.value;
	if (f.minute.value.length==1)
	    f.minute.value="0"+f.minute.value;
	if (f.second.value.length==1)
	    f.second.value="0"+f.second.value;
	if (f.month.value==1 && (f.day.value==30 || f.day.value==31))
	{
		alert("<% write(ErrorDate); %>");
		return false;
	}
	
	if (f.SntpEnable[0].checked == true) {
		if(!checkHour(f.hour))		return false;
		if(!checkMinute(f.minute))		return false;
		if(!checkSecond(f.second))		return false;
	}
	if(f.time_zone.value=="") {alert("<% write(SelTimeZone); %>");return false;}
	
	sysDate.setFullYear(eval(f.year.value),eval(f.month.value),eval(f.day.value));
	sysDate.setHours(eval(f.hour.value));
	sysDate.setMinutes(eval(f.minute.value));
	sysDate.setSeconds(eval(f.second.value));
	//alert(eval(Date.parse(sysDate.toUTCString()))/1000);
	(eval(f.month.value)+1)
	f.SysDateTime1.value ="\"" + f.year.value +"-"+ (eval(f.month.value)+1).toString() +"-"+ f.day.value + " "+f.hour.value +":"+ f.minute.value +":"+ f.second.value+"\"";
	if (f.SntpEnable[1].checked)
	{
	  if (!checkIP(f.NTPServerIP,"<% write(Server1IP); %>",0)) return false;
	  if (!checkIP(f.server2,"<% write(Server2IP); %>",0)) return false;
	  if (!checkIP(f.server3,"<% write(Server3IP); %>",1)) return false;
	}
	return true;
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
	}
	else if(monthval==1){
		if(val%400==0||((val%4==0)&&(val%100!=0)))
			creatday(tbl.day,29);
		else 
			creatday(tbl.day,28);
	}
	else
		creatday(tbl.day,31);
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
-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<% write(SysClock); %>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/" name="SysClock_Config" method="post"> 
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
        <tr>
		<td width="35%" align="right" height="35"><% write(SysNowTime); %></td>
		<td width="3%">&nbsp;</td>
		<td height="35"><% write(SCDate); %>&nbsp;&nbsp;<input name="date" type="text" class="input2" id="date" size="12" disabled><% write(SCTime); %>&nbsp;&nbsp;<input name="time" type="text" class="input2" id="time" size="12" disabled></td>
	      </tr>
		  <tr>
		<td  align="right"><% write(SelZone); %></td>
		<td >&nbsp;</td>
		<td height="35"><select name="time_zone" size="1" class="input" id="time_zone">
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
		  <tr><td colspan="4" align="left"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
		  <td height="35" align="right"width="35%" ><% write(ManuallySetTime); %><input name="SntpEnable" type="radio" value="off" onClick=setenable() checked="checked">&nbsp;</td>
		  <td width="3%">&nbsp;</td>
		<td><table width="100%" border="0" cellspacing="0" cellpadding="0" >
    <tr>
		<td width="80">&nbsp;<select name="year" class="input" onChange="changeyear(this.value)" >
			  <script language="javascript">
			  for(i=2011;i<=2020;i++){
			       document.write('<option value='+i+'>'+i+'</option>');
			  }
			  </script>
			</select> <% write(RYear); %></td>
		<td width="70"><select name="month" class="input" onChange="changeMonth(this.value)">
		<script language="javascript">
			  for(i=1;i<=12;i++){
			       document.write('<option value='+(i-1)+'>'+i+'</option>');
			  }
			  </script>
		      </select> <% write(RMou); %></td>
		<td width="70"><select name="day" class="input">
			<script language="javascript">
			  //for(i=1;i<=31;i++){
			   //    document.write('<option value='+i+'>'+i+'</option>');
			 // }
			  </script>
		      </select> <% write(RDay); %></td>
		<td><input name="hour" type="text"  onKeyPress="HourOnly(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time">:
	<input name="minute" type="text"  onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time">:
      <input name="second" type="text"  onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time"></td>
  </tr> 
</table></td>
	      </tr>
</table>
</td>
</tr>
		 
		  <tr>
		  <td height="35" align="right" ><% write(Timed); %><input name="SntpEnable" type="radio" value="on" onClick=setenable()></td>
		  <td >&nbsp;</td>
		  <td >&nbsp;</td>
		  </tr>
		<tr id="dst" style="display:none">
		     <td align="right" height="35"><%write(dst);%></td>
		     <td width="7" height="9"></td>
		     <td>
			    <input name="dstEnable" id="dstEnable" type="checkbox"/>
		    </td>
	       </tr>
	      <tr>
		  <td height="35" align="right"><% write(Server1IP); %></td>
		  <td><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		  <td><input name="NTPServerIP" type="text" disabled class="input" onKeyPress="ipOnly()" value="192.43.244.18" size="18" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(IPAddr); %>'),ip1tb)" ></td><td  id="ip1tb"></td>
		  </tr>
		  <tr>
		  <td height="35" align="right"><% write(Server2IP); %></td>
		  <td><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		  <td><input name="server2" type="text" disabled class="input" onKeyPress="ipOnly()" value="129.6.15.28" size="18" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<% write(IPAddr); %>'),ip2tb)" ></td><td  id="ip2tb"></td>
		  </tr>
		  <tr>
		  <td height="35" align="right"><% write(Server3IP); %></td>
		  <td>&nbsp;</td>
		  <td><input name="server3" type="text" disabled class="input" onKeyPress="ipOnly()" size="18" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<% write(IPAddr); %>'),ip3tb)" ><input name="NTPSync" value="1" type="hidden"></td><td  id="ip3tb"></td>
		  </tr>
		  <tr><td colspan="4"><table width="100%">
	   <tr  height="35">
	   <td align="right" width="25%" class="helpfont"><strong><% write(Attention); %>:</strong></td>
	   <td class="helpfont"><% write(AttMessage); %></td></tr>
	   </table></td></tr>
		  <tr><td colspan="3" height="20">&nbsp;</td></tr>
		    <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#SysClock')"></td>
  </tr>
</table>
  </td></tr>
  </table>
    </td></tr>
<input type="hidden" name="SysDateTime1">
</table>
<script language="javascript">init();</script>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
	      
