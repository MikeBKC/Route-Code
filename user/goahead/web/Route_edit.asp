<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("Route");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

var errorstr="";
/////////////////////////////交互变量/////////////////////


var RouteNames="";
var RouteEnables="";
var DesIPs="";
var DesMasks="";
var GateWays="";
var Metrics="";
var Profiles="";
<%aspOutputPPTPAccInfo();%>
<% 
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutStaticRouteOneInfo(edit_Name);//需要修改
%> 
/*
var PptpNames = new Array();
PptpNames[0] = "pptp1"; 
PptpNames[1] = "pptp2"; 
*/
var edit_Names = "<% write(Form("id"));%>";


<!--



function init(){
	var f=Route_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.RouteName.value=RouteNames;
		f.RouteNameold.value=RouteNames;
		f.RouteEnablew.checked=(RouteEnables=="1"?true:false);
		f.DesIP.value=DesIPs;
		f.DesMask.value=DesMasks;
		f.GateWay.value=GateWays;
		f.Metric.value=Metrics;
		selVal(f.Profile,Profiles);
		f.Action.value="modify";
		f.RouteName.disabled = true;
	}
	for(i=1;i<=5;i++)
		document.all["yutd"+i].innerHTML="";
}

function save(){
	var f=Route_Config;
	if(checkData()){
		//alert(f.RouteEnablew.checked);
		f.RouteName.disabled = false;
		f.RouteEnable.value=(f.RouteEnablew.checked==true?"1":"0");
		//alert(f.RouteEnable.value);
		Route_Config.action="/goform/formStaticRoute";
		Route_Config.submit();
		f.RouteName.disabled = true;
	}
}
function checkData(){
	var f = Route_Config;
	if(!checkName(f.RouteName,11,"<%write(rou_routeName);%>",0)) return false;
	if(!checkIP(f.DesIP,"<%write(rou_destNet);%>",0)) return false;
	if (!checkMask(f.DesMask,"<%write(com_NetMask);%>",0)) return false;
	if(!checkIP(f.GateWay,"<%write(com_GateWayAddr);%>",0)) return false;
	if(! checkNumber(f.Metric,0,7,"<%write(com_priority);%>",0)) return false;
	return true;
}
function Reset(){
	Route_Config.reset();
	init();
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(rou_routeConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="Route_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
	   <td width="34%" height="25" align="right"><%write(rou_routeName);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="RouteName" type="text" maxlength="11" size="16" onChange="showfiledtip(re_checkName(this,'<%write(rou_routeName);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr> <input type="hidden" name="RouteNameold" >
	    <tr>
	   <td align="right" height="25"><%write(com_startConf);%></td>
	   <td >&nbsp;</td>
	   <td ><input type="checkbox" name="RouteEnablew"  checked="checked"><input type="hidden" name="RouteEnable"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont"><%write(rou_startTip);%></td></tr>
       <tr>
	   <td align="right" height="25"><%write(rou_destNet);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="DesIP" type="text" size="16" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(rou_destNet);%>'),yutd2)" value="0.0.0.0"></td><td  id="yutd2"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><%write(com_NetMask);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="DesMask" type="text" size="16" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>'),yutd3)" value="255.255.255.0"></td><td  id="yutd3"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><%write(com_GateWayAddr);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="GateWay" type="text" size="16" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(com_GateWayAddr);%>'),yutd4)" value="0.0.0.0"></td><td  id="yutd4"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><%write(com_priority);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td class="helpfont"><input name="Metric" type="text" size="16" maxlength="1" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,0,7,'<%write(com_priority);%>'),yutd5)" value="0">&nbsp;<%write(rou_priTip);%></td>
	   <td  id="yutd5"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><%write(com_Interface);%></td>
	   <td >&nbsp;</td>
	   <td ><select name="Profile">
	   <option value='LAN0'> LAN</option>
	 	<script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
				if (is3GPort(i,wanIfCount)) {
					document.write('<option value="WAN'+i+'" >3G</option>');
				} else if (isAPCliPort(i, wanIfCount)) {
					document.write('<option value="WAN'+i+'" >APClient</option>');
				} else {
					document.write('<option value="WAN'+i+'" '+ (i==1? 'selected="selected"':'') +'>WAN'+i+'</option>');
				}
		 }
		if(PPTPClientList == 1)
		{
		    for(var i = 0; i < PptpServerNames.length; i++)
		    {
			document.write('<option value='+PptpServerNames[i]+'>'+PptpServerNames[i]+'</option>');
		    }
		    for(var i = 0; i < PptpClientNames.length; i++)
		    {
			document.write('<option value='+PptpClientNames[i]+'>'+PptpClientNames[i]+'</option>');
		    }
		}
		</script> 
	   
	   </select></td>
	   </tr>
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Route')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('Route.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
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
