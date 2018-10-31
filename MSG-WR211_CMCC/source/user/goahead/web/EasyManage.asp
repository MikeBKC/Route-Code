<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("SmartQos");%>
<%langTran("commonWords");%>
<%langTran("EasyTask");%>
<%langTran("left");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

var WrlessEnables=0;
<%aspOutputRateArr();%>
<%
getWanIfCount();
aspSmartQos();
%> 
<% aspoutEasyTask(); %>
function save()
{
    var f=EasyManage_config;
	for(var i=1;i<=wanIfCount;i++)
	{
		if(!checkNumber(eval('f.rxBand'+i),0,1000000,"<%write(com_band);%>",0)
	  		|| !checkNumber(eval('f.rxBand'+i),0,1000000,"<%write(com_band);%>",0)){
        return false;
    	} 
    
    }
    var len = f.date.length;
    for (i = 0; i < len; i++) 
    {
	f.day.value += (f.date[i].checked == true ? "1": "0");
    }    
    f.weekdaytimestart.value = f.weekdayfromhour.value + ":" +f.weekdayfromminute.value;
    f.weekdaytimestop.value = f.weekdayendhour.value + ":" + f.weekdayendminute.value;
    f.weekendtimestart.value = f.weekendfromhour.value + ":" + f.weekendfromminute.value;
    f.weekendtimestop.value = f.weekendendhour.value + ":" + f.weekendendminute.value;

    EasyManage_config.action="/goform/formEasyTask";
    EasyManage_config.submit();
}  

function init()
{
    var f=EasyManage_config;
    f.smartQosEn.checked=(smartQosEn? true:false);
	for(var i=1;i<=wanIfCount;i++)
	{
		eval( 'f.rxBand'+i).value  = rxBands[i];
		changeSelect(eval('f.rxBand'+i), eval('f.MaxDownRatew'+i));
		eval( 'f.txBand'+i).value  = txBands[i];
		changeSelect(eval('f.txBand'+i), eval('f.uMaxDownRatew'+i));
	}
    if (easyTaskEn == "on")
    {
	EasyManage_config.allowEasyManage.checked = true;
    }
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

    if (WrlessEnables == 0)
    {
	moduleDisplay("wlantip", 1);
    }
    else
    {
	moduleDisplay("wlantip", 0);
    }
}

function changeSelect(ctrl1, ctrl2)
{
	if (ctrl1.value == "")
	{
		var selectIndex = 0;
	} else {
		var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);
	}
	if (selectIndex != -1)
	{
		ctrl2.selectedIndex = selectIndex;
	} else {
		ctrl2.selectedIndex = 1;
	}
}

function changeRate(ctl,ctl2){	
	var selValue = getListValue(ctl);
	selValue=(selValue=="zidingyi"?"":selValue);
	ctl2.value = selValue;	
}
/*
打印上行带宽行
*/
function writeUpLimit(wanNo)
{
	  document.write('<tr>'
			  +'<td align="right" height="25" width="31%"><%write(sma_upBand);%></td>'
			  +'<td width="2%">&nbsp;</td>'
			  +'<td>'
			  );
			  document.write('<input type="text" name="txBand'+wanNo+'" onKeyPress="figureOnly()" onBlur="changeSelect(this,EasyManage_config.uMaxDownRatew'+wanNo+')" size="8" maxlength="6">');
			  document.write('&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;');
			  document.write('<select name="uMaxDownRatew'+wanNo+'" class="list" onChange="changeRate(this,EasyManage_config.txBand'+wanNo+')">');
			  document.write('<option value="0"><%write(com_noLimit);%></option>');
			  document.write(' <option value="zidingyi"><%write(com_defSelf);%></option>');
			  for(var i=0;i<RateArr.length;i++)
			   document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
			   document.write('</select>&nbsp;<%write(com_limitTip);%>');
			   document.write('</td></tr>');
}
/**
打印下行带宽行
**/
function writeDownLimit(wanNo)
{
		  document.write('<tr>'
			  +'<td align="right" height="25" width="31%"><%write(sma_downBand);%></td>'
			  +'<td width="2%">&nbsp;</td>'
			  +'<td>'
			  );
			  document.write('<input type="text" name="rxBand'+wanNo+'" onKeyPress="figureOnly()" onBlur="changeSelect(this,EasyManage_config.MaxDownRatew'+wanNo+')" size="8" maxlength="6">');
			  document.write('&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;');
			  document.write('<select name="MaxDownRatew'+wanNo+'" class="list" onChange="changeRate(this,EasyManage_config.rxBand'+wanNo+')">');
			   document.write('<option value="0"><%write(com_noLimit);%></option>');
			  document.write(' <option value="zidingyi"><%write(com_defSelf);%></option>');
			  for(var i=0;i<RateArr.length;i++)
			   document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
			   document.write('</select>&nbsp;<%write(com_limitTip);%>');
			   document.write('</td></tr>');
}

function gotoAdvanced()
{
    EasyManage_config.action="/WlanBase.asp";
    EasyManage_config.submit();
}

</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(leftMenud81);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/formConfigSmartQos"  name="EasyManage_config" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
	    <tr>
		<td width="31%" align="right" valign="middle" height="25"><%write(eas_wlantime);%></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="25"><div id="wlantip" name="wlantip" style="display:"><font color=#999999><% write(eas_wlanalert1); %></font><a style="cursor:pointer; color:#03F;" onClick="gotoAdvanced();"><span style="font-weight:bold"><% write(leftTag2); %></span>-><span style="font-weight:bold;"><% write(leftMenud13); %></span><a><% write(eas_wlanalert2); %></div></td>
	      </tr>
	    <tr>
		<td width="31%" align="right" valign="middle" height="25"></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="25">
		    <input type="checkbox" name="date" value="1"><% write(com_monday); %>&nbsp;
		    <input type="checkbox" name="date" value="2"><% write(com_tuesday); %>&nbsp;
		    <input type="checkbox" name="date" value="3"><% write(com_wednesday); %>&nbsp;
		    <input type="checkbox" name="date" value="4"><% write(com_thursday); %>&nbsp;
		    <input type="checkbox" name="date" value="5"><% write(com_friday); %>
		</td>
	    </tr>
	    <tr>
		<td width="31%" align="right" valign="middle" height="25"></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="25">&nbsp;
		    <select name="weekdayfromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekdayfromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<% write(com_to); %>&nbsp;<select name="weekdayendhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekdayendminute" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="day"><input type="hidden" name="weekdaytimestart"><input type="hidden" name="weekdaytimestop">
		</td>
	    </tr>
	    <tr>
		<td width="31%" align="right" valign="middle" height="25"></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="25">
		    <input type="checkbox" name="date" value="6"><% write(com_saturday); %>&nbsp;
		    <input type="checkbox" name="date" value="7"><% write(com_sunday); %>
		</td>
	    </tr>
	    <tr>
		<td width="31%" align="right" valign="middle" height="25"></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="25">&nbsp;
		    <select name="weekendfromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekendfromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<% write(com_to); %>&nbsp;<select name="weekendendhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekendendminute" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="weekendtimestart"><input type="hidden" name="weekendtimestop">
		</td>
	    </tr>
	    <tr>
		<td width="31%" align="right" valign="middle" height="25" id="daikuan"><%write(sma_startQos);%></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="25"><input name="smartQosEn" type="checkbox" ></td>
	      </tr>
          <script language="javascript">
		  for(var i=1;i<=wanIfCount;i++)
		  {
		      if (is3GPort(i,wanIfCount)) {
			  	document.write('<tr>'
			  	+'<td width="31%" align="right" height="25" id="daikuan" valign="bottom"><strong>3G:</strong></td>'
			  	+'<td width="2%">&nbsp;</td>'
			  	+'<td width="67%" height="20"> </td></tr>'
			  	);
			  } else if (isAPCliPort(i, wanIfCount)) {
			  	document.write('<tr>'
			  	+'<td width="31%" align="right" height="25" id="daikuan" valign="bottom"><strong>APClient:</strong></td>'
			  	+'<td width="2%">&nbsp;</td>'
			  	+'<td width="67%" height="20"> </td></tr>'
			  	);			  
			  } else {
			  	document.write('<tr>'
			  	+'<td width="31%" align="right" height="25" id="daikuan" valign="bottom"><strong>WAN'+i+'<%write(sma_kou);%>:</strong></td>'
			  	+'<td width="2%">&nbsp;</td>'
			  	+'<td width="67%" height="20"> </td></tr>'
			  	);
			  }
			  writeUpLimit(i);
			  writeDownLimit(i);
			
		  }
		  </script>
  <tr>
		<td width="31%" align="right" valign="middle" height="45"><%write(eas_easyTaskEn);%></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" valign="middle" height="45"><input name="allowEasyManage" type="checkbox" ></td>
  </tr>
<tr><td colspan="4">
  <table width="29%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="40" align="center" valign="middle"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#EasyTask')"></td>
  </tr> 
</table>

  </td></tr>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
 
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
