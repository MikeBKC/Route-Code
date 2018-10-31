<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("DoubleRoadInfo");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

//function initTranslation()
//{
//	var e;
	/*e = document.getElementById("name");
	e.innerHTML= _("name");
	e = document.getElementById("ipaddr");
	e.innerHTML= _("ipaddr");
	e= document.getElementById("MACaddr");
	e.innerHTML= _("MACaddr");

	e = document.getElementById("uttsave");
	e.value= _("save");
	e = document.getElementById("uttreset");
	e.value= _("reset");
	e= document.getElementById("returnback");
	e.value= _("returnback");
*/
//}
var Weights="nolimit";
var errorstr = "";
<%aspOutputRateArr1();%>
<% getWanIfCount(); %>
<%aspMultiPathConfig();%>

var edit_Names = "<% write(Form("id"));%>";
var index = edit_Names;
if (edit_Names == "") index = "1";
index -= 1;

Weights=(Weights=="nolimit"?"0":Weights);

<!--
function init(){
	var f=DoubleRoadInfo_Config;
	selVal(f.PortName,PortNames[index]);
	portChange(index);
	f.Action.value="modify";
        for(i=1;i<=4;i++){
		document.all["yutd"+i].innerHTML="";
	}
}

function save(){
	var f=DoubleRoadInfo_Config;
	if(checkData()){
	
	 
		    f.Rate.value=f.MaxRatetxt.value+"k";
			
        DoubleRoadInfo_Config.PortName.disabled=false;
		DoubleRoadInfo_Config.action="/goform/formMultiPathConfig";
		DoubleRoadInfo_Config.submit();
	}
}
function checkData(){
	var f = DoubleRoadInfo_Config;
	if (!checkNumber(f.KeepLive,0,60,"<%write(detectInterval);%>",0)) return false;
	if (!checkNumber(f.RetryTimes,3,1000,"<%write(detectTimes);%>",0)) return false;
	if (f.PriAddrType.value != "gateway" && f.DestIP.value=="0.0.0.0" && f.KeepLive.value != 0){ alert("<%write(checkDestIPAlert);%>");return false};
	if(!checkIP(f.DestIP,"<%write(destIP);%>",1)) return false;
	//if (!checkNumber(f.Weight,1,100,"带宽权重",0)) return false;
 
	    if(!checkNumber(f.MaxRatetxt,1,1000000,"<%write(com_band);%>",0)) return false;

	var down_rate = parseInt(f.MaxRatetxt.value);
	
	return true;
}
function Reset(){
	DoubleRoadInfo_Config.reset();
	init();
}
function onAddrTypeChange()
{
    if (DoubleRoadInfo_Config.PriAddrType.value=="gateway") //选择网关
    {
	DoubleRoadInfo_Config.DestIP.value="0.0.0.0";
	showTable(ipTB, false);
    }
    else 
    {
	DoubleRoadInfo_Config.DestIP.value=DestIPs[index];
	showTable(ipTB, true);
    }
}
function portChange(i)
{
    var f=DoubleRoadInfo_Config;
    index = i;
    f.MaxRatetxt.value=Weights[index].replace(/k bit\/s/g,"");
    changeSelect(f.MaxRatetxt, f.MaxRatew);
    f.KeepLive.value=KeepLives[i];
    f.RetryTimes.value=RetryTimess[i];
    f.DestIP.value = DestIPs[i];
    if (f.DestIP.value == "0.0.0.0") {
	f.PriAddrType.value="gateway";
    } else {
	f.PriAddrType.value="others";
    }
    if (ConnTypes[i] == "PPPOE") {
	    f.PriAddrType.value="others";
	    f.PriAddrType.disabled=true;
    } else {
	    f.PriAddrType.disabled=false;
    }
    onAddrTypeChange();
}
function changeSelect(ctrl1, ctrl2)
{

	var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);


	if (selectIndex != -1)
	{
		ctrl2.selectedIndex = selectIndex;
	} else {
		ctrl2.selectedIndex = 0;
	}
}

function changeRate(ctl,ctl2){	
	var selValue = getListValue(ctl);
	selValue=(selValue=="zidingyi"?"":selValue);
	ctl2.value = selValue;	
}
//-->
</script>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(routeCombConf);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"	 name="DoubleRoadInfo_Config" method="post">
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"95%",
	4,
	new Array("<%write(com_globalConfig);%>",'DoubleRoad.asp',1,0),
	new Array("<%write(com_routeCombinationStatus);%>",'DoubleRoadInfo.asp',1,0),
	new Array("<% write(com_lineCombinationCheck); %>",'DoubleRoadInfo_edit.asp',1,1),
	new Array("<% write(com_IdentifyBind); %>",'IdentifyBind.asp',1,0)
	);
showThreeMenu();
</script>
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
<td align="center"  class="three_td" valign="top">
  <table border="0" width="920" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>

   <tr>
	   <td width="25%" height="25" align="right" id="name"><%write(com_Interface);%></td>
	   <td width="2%">&nbsp;</td>
	   <td width="65%"><select name="PortName" class="list" onChange="portChange(this.selectedIndex)">
	   <script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
		    if (is3GPort(i,wanIfCount)) {
			document.write('<option value="'+i+'" >3G</option>');
		    } else if (isAPCliPort(i, wanIfCount)) {
			document.write('<option value="'+i+'" >APClient</option>');
		    } else {
			document.write('<option value="'+i+'" >WAN'+i+'</option>');
		    }
		 }
		</script>
	   </select></td>
	   </tr>
	    <tr>
	   <td align="right" height="25" id="MACaddr"><%write(detectInterval);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="KeepLive" type="text" size="18" maxlength="2" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,0,60,'<%write(detectInterval);%>'),yutd1)" value="1">&nbsp;<%write(dInterTip);%></td><td width="18%"  id="yutd1"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25" id="MACaddr"><%write(detectTimes);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="RetryTimes" type="text" size="18" maxlength="4" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,3,1000,'<%write(detectTimes);%>'),yutd2)" value="10">&nbsp;<%write(dTimeTip);%></td><td	id="yutd2"></td>
	   </tr>

	   <tr> 
	   <td align="right" height="25" id="CheckIP"><%write(destIP);%></td>
	   <td><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><select name="PriAddrType" class="input" id="PriAddrType" onChange="onAddrTypeChange()">
	       <option value="gateway" selected><%write(gatewayIP);%></option>
	       <option value="others"><%write(otherIP);%></option></select>
	   </td><td></td>
	   </tr>

	   <tr id="ipTB" style="display:"> 
	   <td align="right" height="25"></td>
	   <td></td>
	   <td><input name="DestIP" type="text" size="18" maxlength="17" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(destIP);%>'),yutd3)"></td>
	   <td id="yutd3"></td>
	   </tr>

	    <tr  style="display:none">
	   <td align="right" height="25" id="MACaddr"><%write(bandWidthWeight);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="Weight" type="text" size="18" maxlength="3" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,1,100,'<%write(bandWidthWeight);%>'),yutd4)" value="1">&nbsp;<%write(bwidthWeightTip);%></td><td  id="yutd4"></td>
	   </tr>
	   
	    <tr>
	   <td align="right" height="25" width="27%"><%write(com_band);%><input type="hidden" name="Rate"><input type="hidden" name="Ceil" value="nolimit"></td>
		   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="MaxRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,DoubleRoadInfo_Config.MaxRatew)" size="8" maxlength="7">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="MaxRatew" class="list" onChange="changeRate(this,DoubleRoadInfo_Config.MaxRatetxt)">
	   <option value="zidingyi"><%write(com_defSelf);%></option>
		<script language="javascript">
		for(i=0;i<RateArr1.length;i++)
			document.write('<option value='+RateArrValue1[i]+'>'+RateArr1[i]+'</option>');
		</script>
</select></td>
	   </tr>
	   
	   
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="35%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td	  height="30" align="left" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('DoubleRoadInfo.asp')" id="returnback"></td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr><script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

