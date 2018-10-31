 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("LANConfig");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("ipaddr");
//	e.innerHTML= _("ipaddr");
//	e = document.getElementById("subnet");
//	e.innerHTML= _("subnet");
//	e = document.getElementById("MACaddr");
//	e.innerHTML= _("MACaddr");
//	e= document.getElementById("zhuyi");
//	e.innerHTML= _("zhuyi");
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e = document.getElementById("uttreset");
//	e.value= _("reset");
//
//}
var multiLan = 0;
<%outputLANConfig(); %>

<!--
function checkData(){
	var f=LANConfig;
	if (f.lanIp.value=="0.0.0.0"){ alert("<%write(com_checkIPAlert);%>");return false};
	if (!checkLanIP(f.lanIp,"<%write(com_IP);%>",0)) return false;
	if(multiLan == 1)
	{
	    if(f.lanIp2.value != "0.0.0.0")
	    {
	    if (!checkLanIP(f.lanIp2,"<%write(com_IP);%>2",0)) return false;
	    }
	    if(f.lanIp3.value != "0.0.0.0")
	    {
	    if (!checkLanIP(f.lanIp3,"<%write(com_IP);%>3",0)) return false;
	    }
	    if(f.lanIp4.value != "0.0.0.0")
	    {
	    if (!checkLanIP(f.lanIp4,"<%write(com_IP);%>4",0)) return false;
	    }
	}
	if (!checkMask(f.lanNetmask,"<%write(com_NetMask);%>",0)) return false;
	if(!checkMac(f.lanMac,"<%write(com_MAC);%>",0)) return false;
	return true;
}
/*高级选项的打开与关闭控制*/
var SeniorShowEn=0;
function clickSenior(frm, adv) {
    if(SeniorShowEn == 0) {
        SeniorShowEn = 1;
    } else {
        SeniorShowEn = 0;
    }
    showTable(adv, SeniorShowEn);
}
function save(){
	if(checkData()){
		var f=LANConfig;
		LANConfig.action="/goform/ConfigLANConfig";
		LANConfig.submit();
	}	
}
var lanMode=new Array();
function createModeEnum(maxrate)
{
    
}
function init(){
	var f=LANConfig;
	f.lanIp.value = lanIps;
	f.lanNetmask.value = lanNMs;
	if(multiLan == 1)
	{
	    showTable(advanceTable, 1);
	    f.lanIp2.value = lanIp2s;
	    f.lanNetmask2.value = lanNM2s;
	    f.lanIp3.value = lanIp3s;
	    f.lanNetmask3.value = lanNM3s;
	    f.lanIp4.value = lanIp4s;
	    f.lanNetmask4.value = lanNM4s;
	}
	f.lanMac.value = lanMacs;
	if(maxSpeed!=-1)
	    f.LanMode.value=lanModes;
	if(maxSpeed==100) {
		for(var j=0;j<f.LanMode.options.length;j++)
		{
			if(f.LanMode.options[j].value=="2")
			{
				f.LanMode.remove(j);
			}
		}
	}else if(maxSpeed==10) {
		for(var k=0;k<f.LanMode.options.length;k++)
		{
			if((f.LanMode.options[k].value=="2")||(f.LanMode.options[k].value=="5")||(f.LanMode.options[k].value=="1"))
			{
				f.LanMode.remove(k);
			}
		}
	}else if(maxSpeed==-1){
	    modeID.style.display="none";
	}
}
function Reset(){
	LANConfig.reset();
	masktd.innerHTML="";
	iptd.innerHTML="";
	if(multiLan == 1)
	{
	    iptd2.innerHTML="";
	    iptd3.innerHTML="";
	    iptd4.innerHTML="";
	}
	mactd.innerHTML="";
	init();	
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_LanConfig);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form  name="LANConfig" method="post">
<p>&nbsp;</p>
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
   
  <tr>
	   <td align="right"  height="25" width="36%" id="ipaddr"><%write(com_IP);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="29%"><input type="text" name="lanIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),iptd)"  value=""></td><td width="33%"  id="iptd"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="subnet"><%write(com_NetMask);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="lanNetmask" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>'),masktd)" value=""></td><td  id="masktd"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="MACaddr"><%write(com_MAC);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="lanMac" size="18" onKeyPress="macOnly()" maxlength="17" onChange="showfiledtip(re_checkMac(this,'<%write(com_MAC);%>'),mactd)"></td><td  id="mactd"></td>
	   </tr>
	    <tr id="modeID" style="display:">
	    <td height="25" align="right"><%write(com_inter_mode);%></td>
	    <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	    <td ><select name="LanMode" id="LanMode">
		<option value="3"><%write(com_auto);%></option>
		<option value="4"><%write(com_10mhd);%></option>
		<option value="0"><%write(com_10mfd);%></option>
		<option value="5"><%write(com_100mhd);%></option>
		<option value="1"><%write(com_100mfd);%></option>
		<option value="2"><%write(com_1000mfd);%></option>
	   	</select></td><td id="mode"></td>
	    </tr>

         <tr id="advanceTable" style="display:none"><td colspan="4"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr>
         <td width="36%" height="30" align="right" ><div style="position:relative; z-index:2;"><a href="javascript:clickSenior(LANConfig, adv)" style="color:#000000;text-decoration:underline;"><strong><%write(lan_advance);%></strong></a></div></td>
         <td width="2%" >&nbsp;</td>
         <td width="47%"><%write(lan_advanceTip);%></td>
         <td width="15%"><input type="hidden" name="senior1" id="senior1" value="true" /><input type="hidden" name="senior2" id="senior2" value="true" /><input type="hidden" name="senior3" id="senior3" value="true" /></td></tr>

    <tr id="adv" style="display:none">
      <td colspan="4" >
       <table width="100%" border="0" cellpadding="0" cellspacing="0" >
	<tr>
	   <td align="right"  height="25" width="36%" id="ipaddr2"><%write(com_IP);%>2</td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="29%"><input type="text" name="lanIp2" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>2'),iptd2)"  value=""></td><td width="33%"  id="iptd2"></td>
       </tr>
       <tr>
	   <td align="right" height="25" id="subnet2"><%write(com_NetMask);%>2</td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="lanNetmask2" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>2'),masktd)" value=""></td><td  id="masktd"></td>
       </tr>
	<tr>
	   <td align="right"  height="25" width="36%" id="ipaddr3"><%write(com_IP);%>3</td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="29%"><input type="text" name="lanIp3" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>3'),iptd3)"  value=""></td><td width="33%"  id="iptd3"></td>
       </tr>
       <tr>
	   <td align="right" height="25" id="subnet3"><%write(com_NetMask);%>3</td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="lanNetmask3" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>3'),masktd)" value=""></td><td  id="masktd"></td>
       </tr>
	<tr>
	   <td align="right"  height="25" width="36%" id="ipaddr4"><%write(com_IP);%>4</td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td width="29%"><input type="text" name="lanIp4" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>4'),iptd4)"  value=""></td><td width="33%"  id="iptd4"></td>
       </tr>
       <tr>
	   <td align="right" height="25" id="subnet4"><%write(com_NetMask);%>4</td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="lanNetmask4" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>4'),masktd)" value=""></td><td  id="masktd"></td>
       </tr>

      </table>
    </td>
   </tr>		

	</table></td></tr>

	   <tr><td colspan="4"><table width="100%">
	   <tr  height="30">
	   <td class="helpfont"  align="center" id="zhuyi"><strong><%write(com_zhuyi);%></strong><%write(editIpTip);%></td></tr>
	   </table></td></tr>
	 
	    <tr><td  colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#LANConfig')"></td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
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
