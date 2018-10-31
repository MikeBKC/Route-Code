<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("radvd");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//---------交互开始-------------
<% outputRADVDConfig();%>
//----------交互结束---------------------

function init(){
	var f = radvdConfig;

	f.radvdEnabled.checked = (radvdEnabled == "1")?true:false;
	f.manageEnabled.checked = (manageEnabled == "1")?true:false;
	f.otherEnabled.checked = (otherEnabled == "1")?true:false;
	f.adInterval.value = adInterval;
	f.routeType.value = routeType;
	f.mtuSet.value = mtuSet;
	f.routeLifeTime.value = routeLifeTime;
	}

function checkDatas(){
	var f = radvdConfig;
        f.adInterval.value = f.adInterval.value.trim();	
        f.mtuSet.value = f.mtuSet.value.trim();	
        f.routeLifeTime.value = f.routeLifeTime.value.trim();	
	if (!checkNumber(f.adInterval,4,1800,"<%write(ad_interval);%>",0)) return false;
	if (!checkNumber(f.mtuSet,1280,1500,"<%write(mtu);%>",1)) return false;
	if (!checkNumber(f.routeLifeTime,60,3600,"<%write(route_life);%>",0)) return false;	
	if (f.adInterval.value > PrefixstaleTime){
	    alert("<%write(alert_one);%>");	
	    return false;
	}
	return true;
	
}

function save(){
	if(checkDatas()){
		radvdConfig.action = "/goform/ConfigRDVDConfig";
		radvdConfig.submit();
		}
}

function Reset(){
	radvdConfig.reset();
	init();
 
	}

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
  <td><script language="javascript">showSecondTitle('<%write(radvd);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="radvdConfig" method="post">
<script language="javascript">                                                  
                var arrayThreeMenu=new Array(                                   
                                                "80%",                          
                                                2,                              
                                new Array("<%write(radvd);%>",'radvd.asp',1,1),          
                                new Array("<%write(radvd_prefix);%>",'ipv6PrefixList.asp',1,0) 
                );                                                              
                showThreeMenu();                                                
        </script>                               
        <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="280">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
    <tr><td colspan="4" height="40">&nbsp;</td></tr>

	<tr>
	   <td align="right" width="48%" height="25"><%write(radvd_en);%></td>
	   <td  width="2%">&nbsp;</td>
	   <td width="12%" ><input type="checkbox" name="radvdEnabled"  id="radvdEnabled"></td>
           <td width="34%" ></td>
	</tr>
	<tr>
	   <td align="right" width="48%" height="25"><%write(ra_flag);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="10%" ><input type="checkbox" name="manageEnabled"  id="manageEnabled"><%write(managed);%></td>
           <td width="10%" ><input type="checkbox" name="otherEnabled"  id="otherEnabled"><%write(other);%></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(ad_interval);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="adInterval" id="adInterval" type="text" size="10" maxlength="30" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,4,1800,'<%write(ad_interval);%>'),yutd1)"></td><td  id="yutd1"></td>
	</tr>
    <tr>
	   <td align="right" height="25"><%write(route_select);%></td>
	   <td width="2%">&nbsp; </td>
	   <td><select name="routeType" class="list"  onChange="changeConnType(this.value)">
              <option value="1" id="dhcp"><%write(low);%></option>
              <option value="2" id="static"><%write(middle);%></option>
              <option value="3" id="pppoe"><%write(high);%></option>
          </select></td>

	</tr>
    <tr>
	   <td align="right" height="25"><%write(mtu);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="mtuSet" id="mtuSet" type="text" size="10" maxlength="63" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber_allowZero(this,1280,1500,'<%write(mtu);%>'),yutd2)"></td><td  id="yutd2"></td>
	</tr>
    
    <tr>
	   <td align="right" height="25"><%write(route_life);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="routeLifeTime" id="routeLifeTime" type="text" size="10" maxlength="63" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,60,3600,'<%write(route_life);%>'),yutd3)"></td><td  id="yutd3"></td>
	</tr>
   
    
    
	
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	    <td height="30" align="center" valign="top">
	    <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Syslog')">
	    </td>
	   </tr>
	</table>
	</td></tr>
    </table>
    </td></tr>
</table>
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
