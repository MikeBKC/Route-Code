<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("SmartQos");%>
<%langTran("left");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

var GBoostEn="off";
if (SmartQos == 1)
{
    var SmartName = "<% write(sma_Qos); %>";
}
else
{
    var SmartName = "<% write(GB_gameBoost); %>";    
}


<%aspOutputRateArr();%>
<%
getWanIfCount();
aspSmartQos();
%>
var ratelimit=1024000;
if (RateArr.length==33)
	ratelimit=102400;
    if ((platform == "nv1220GW")||(platform == "nv750GW")||(platform == "nv521GW")||(platform == "nv521G") ||(platform == "nv521Gv2")||(platform == "nv840G")||(platform == "nv840Gv2")||(platform == "nv1220G")||(platform == "nv3320G")||(platform == "qv2610g")||(platform == "qv4240g")||(platform == "qv4220g")||(platform == "lv4330G")||(platform == "lv6530G")||(platform == "lv6530GV4"))
	ratelimit=1024000;
function save()
{
    var f=QosGlobalConfig;
	if (SmartQos == 1){
	for(var i=1;i<=wanIfCount;i++)
	{
	    if(!checkNumber(eval('f.rxBand'+i),0,ratelimit,"<%write(com_band);%>",0)
		    || !checkNumber(eval('f.txBand'+i),0,ratelimit,"<%write(com_band);%>",0)){
		return false;
	    } 
	}
	}
	f.gamePolicy.value = "";
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("gamelist"), "input");
   
	for(var i =0 ;i<chks.length && (i < 6);i++)
	{
	    if((chks[i].checked == true) && (chks[i].value != "all"))
		{
			f.gamePolicy.value = f.gamePolicy.value + chks[i].value + ";";
		}
	}
	
	f.gamePolicy.value = f.gamePolicy.value.slice(0, f.gamePolicy.value.lastIndexOf(":"));

    
    QosGlobalConfig.submit();
}  

function init()
{
    var f=QosGlobalConfig;
    if (SmartQos == 1)
    {
	smartshow.style.display="";
	f.smartQosEn.checked=(smartQosEn? true:false);
	for(var i=1;i<=wanIfCount;i++)
	{
	    eval( 'f.rxBand'+i).value  = rxBands[i];
	    changeSelect(eval('f.rxBand'+i), eval('f.MaxDownRatew'+i));
	    eval( 'f.txBand'+i).value  = txBands[i];
	    changeSelect(eval('f.txBand'+i), eval('f.uMaxDownRatew'+i));
	}
    }
    if (GameBoost == 1)
    {
	f.GBoostEn.checked=(GBoostEn.toLowerCase()=="on"? true:false);
	f.MHXY.checked =(MHXY.toLowerCase()=="on"?true:false); 
        f.QQYY.checked = (QQYY.toLowerCase()=="on"?true:false);
        f.CCTV.checked = (CCTV.toLowerCase()=="on"?true:false);
        f.SINA.checked = (SINA.toLowerCase()=="on"?true:false);
        f.QQZB.checked = (QQZB.toLowerCase()=="on"?true:false);
        f.WOW1.checked = (WOW1.toLowerCase()=="on"?true:false);
	f.CF.checked = (CF.toLowerCase()=="on"?true:false);
	f.DNF.checked = (DNF.toLowerCase()=="on"?true:false);
	f.LOL.checked = (LOL.toLowerCase()=="on"?true:false);
    }
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("gamelist"), "input");
	var checkall=true;
	for(var i =0 ;i<chks.length && (i < 6 );i++)
	{
		if((chks[i].checked == false) && (chks[i].value != "all")){
			checkall=false
		}
	}
	if(checkall==true){
		f.checkall.checked = true;
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
			  document.write('<input type="text" name="txBand'+wanNo+'" onKeyPress="figureOnly()" onBlur="changeSelect(this,QosGlobalConfig.uMaxDownRatew'+wanNo+')" size="8" maxlength="7">');
			  document.write('&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;');
			  document.write('<select name="uMaxDownRatew'+wanNo+'" class="list" onChange="changeRate(this,QosGlobalConfig.txBand'+wanNo+')">');
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
			  document.write('<input type="text" name="rxBand'+wanNo+'" onKeyPress="figureOnly()" onBlur="changeSelect(this,QosGlobalConfig.MaxDownRatew'+wanNo+')" size="8" maxlength="7">');
			  document.write('&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;');
			  document.write('<select name="MaxDownRatew'+wanNo+'" class="list" onChange="changeRate(this,QosGlobalConfig.rxBand'+wanNo+')">');
			   document.write('<option value="0"><%write(com_noLimit);%></option>');
			  document.write(' <option value="zidingyi"><%write(com_defSelf);%></option>');
			  for(var i=0;i<RateArr.length;i++)
			   document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
			   document.write('</select>&nbsp;<%write(com_limitTip);%>');
			   document.write('</td></tr>');
}
var gameflag=true;
function showgamelist(){
	if(gameflag){
		document.getElementById("pdbshow").style.display="";
		document.getElementById("GBoostEn").value="-";
	}else{
		 document.getElementById("pdbshow").style.display="none";
		document.getElementById("GBoostEn").value="+";
	}
	gameflag=!gameflag
}
function CheckAll(){
    var f=QosGlobalConfig;
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("gamelist"), "input");
	if(f.checkall.checked == true){
		for(var i =0 ;i<chks.length && (i < 6);i++)
		{
			chks[i].checked = true;
		}
	}else{
		for(var i =0 ;i<chks.length && (i < 6);i++)
		{
			chks[i].checked = false;
		}
	}
}
function changeCheck(Obj){
    var f=QosGlobalConfig;
	if(Obj.checked==false){
		f.checkall.checked = false;
		return;
	}
	var chks = document.getElementsByAttribute("type", "checkbox", document.getElementById("gamelist"), "input");
	var checkall=true;
	for(var i =0 ;i<chks.length && (i < 6 );i++)
	{
		if((chks[i].checked == false) && (chks[i].value != "all")){
			checkall=false
		}
	}
	if(checkall==true){
		f.checkall.checked = true;
	}

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
  <td><script language="javascript">showSecondTitle(SmartName);</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/formConfigSmartQos"  name="QosGlobalConfig" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
  <tr id="smartshow" name="smartshow" style="display:none">
		<td width="31%" align="right" height="25" id="daikuan"><%write(sma_startQos);%></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" height="20"><input name="smartQosEn" type="checkbox" ></td>
	      </tr>
          <script language="javascript">
		if (SmartQos == 1)
		{
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
		}
		  </script>

     <tr name="GBshow" id="GBshow" >
		<td width="31%" align="right" height="25"><%write(GB_gameBoostEn);%></td>
		<td width="2%">&nbsp;</td>
		<td width="67%" height="20"><input name="GBoostEn" id="GBoostEn" type="button" value="+"  onclick="showgamelist();"></td>
     </tr>
	 <tr name="pdbshow" id="pdbshow" style="display:none">
		<td align="center" height="25" id="daikuan" colspan="3">
		
			<table  width="450" cellpadding="0" cellspacing="0" border="0" id="gamelist" >
				<tr>

					<td width="24%" align="right" height="25"><input name="checkall" id="checkall" type="checkbox" onClick="CheckAll()" value="all"/></td>
					<td width="2%">&nbsp;</td>
				    <td width="20%" align="left"><%write(checkall);%></td>
				    <td width="5%" align="right"></td>
				    <td width="2%">&nbsp;</td>
				    <td width="20%" align="left"></td>
					<td width="5%" align="right"></td>																					<td width="2%">&nbsp;</td>
				    <td width="20%" align="left"></td>

				</tr>
				<tr>
					<td width="24%" align="right" height="25"><input name="LOL" type="checkbox" value="148" onClick="changeCheck(this)"/></td>
					<td width="2%">&nbsp;</td>
				    <td width="20%" align="left"><%write(GB_lol);%></td>
				    <td width="5%" align="right"><input name="DNF" type="checkbox" value="149" onClick="changeCheck(this)"/></td>
				    <td width="2%">&nbsp;</td>
				    <td width="20%" align="left"><%write(GB_dnf);%></td>
					<td width="5%" align="right"><input name="CF" type="checkbox" value="150" onClick="changeCheck(this)"/></td>																					<td width="2%">&nbsp;</td>
				    <td width="20%" align="left"><%write(GB_cf);%></td>
				</tr>
				<tr>
					<td align="right" height="25"><input name="WOW1" type="checkbox" value="151" onClick="changeCheck(this)"/></td>
					<td>&nbsp;</td>
				    <td align="left"><%write(GB_wow);%></td>
				    <td align="right"><input name="MHXY" type="checkbox" value="152" onClick="changeCheck(this)"/></td>
				    <td>&nbsp;</td>
				    <td align="left"><%write(GB_mhxy);%></td>
				    <td align="right"><input style="display:none" name="QQYY" type="checkbox" value="163" onClick="changeCheck(this)"/></td>
                                    <td>&nbsp;</td>
                                    <td align="left"><!--<%write(GB_qqyuyin);%>--></td>

				    <input style="display:none" name="gamePolicy" id="gamePolicy">
                                </tr>
                                <tr style="display:none">
                                    <td align="right"><input name="CCTV" type="checkbox" value="164" onClick="changeCheck(this)"/></td>
                                    <td>&nbsp;</td>
                                    <td align="left"><%write(GB_cctv);%></td>	
					<td align="right"><input name="SINA" type="checkbox" value="165" onClick="changeCheck(this)"/></td>
                                    <td>&nbsp;</td>
                                    <td align="left"><%write(GB_sina);%></td>

                                    <td align="right"><input name="QQZB" type="checkbox" value="166" onClick="changeCheck(this)"/></td>
                                    <td>&nbsp;</td>
                                    <td align="left"><%write(GB_qqzb);%></td>

					<td align="right"></td>																				            	<td>&nbsp;</td>
				    <td width="20%" align="left"></td>
				</tr>
			</table>


		</td>

	</tr>
<tr><td colspan="4">
  <table width="29%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="40" align="center" valign="middle"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#SmartQos')"></td>
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
