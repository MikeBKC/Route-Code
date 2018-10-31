<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("WlanAdvanced"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
<% 
 aspOutWirelessAdvancedConfig_5g();
%>
//start交互
//var rtss="2347";var fragments="2346";var beacons="100";var dtims="1";var short_preambles="1";var wmm_capables="1";var errorstr="";
//end交互
function save(){
	var f=WlanAdv_Config;
	if(checkData()){
		f.short_preamble.value=(f.short_preamblew.checked==true?"1":"0");
		f.wmm_capable.value=(f.wmm_capablew.checked==true?"1":"0");
		WlanAdv_Config.action="/goform/wirelessAdvanced_5g";
		WlanAdv_Config.submit();
	}
}
function checkData(){
	var f=WlanAdv_Config;
	if(! checkNumber(f.rts,1,2347,"<% write(RTSThreshold); %>",0)) return false;
	if(! checkNumber(f.fragment,256,2346,"<% write(SectionHold); %>",0)) return false;
	if(! checkNumber(f.beacon,20,999,"<% write(BeaconGap); %>",0)) return false;
	if(! checkNumber(f.dtim,1,255,"<% write(DTIMGap); %>",0)) return false;
	return true;
}
function init(){
	var f=WlanAdv_Config;
	f.rts.value=rtss;
	f.fragment.value=fragments;
	f.beacon.value=beacons;
	f.dtim.value=dtims;
	//f.short_preamble.value=short_preambles;
	f.short_preamblew.checked=(short_preambles=="1"?true:false);
	f.wmm_capablew.checked=(wmm_capables=="1"?true:false);
	rtsthrdtd.innerHTML="";
	fragthrdtd.innerHTML="";
	beacontd.innerHTML="";
	dtimintvtd.innerHTML="";
}
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
  <td><script language="javascript">showSecondTitle("<% write(WirelessAdvance); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" >
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="WlanAdv_Config" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						2,
						new Array("<% write(AdvancePar); %>",'WlanAdvanced_5g.asp',1,1),
						new Array('<%write(DSSID);%>','WlanMultiSsid.asp',0,0)
		);
		showThreeMenu();
	</script>
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" height="250">
  <tr>
    <td align="center" class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr> 
   <tr>
	   <td align="right" width="25%" height="25" id="RTS threshold"><% write(RTSThreshold); %></td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="rts" size="18" value="2346" onKeyPress="figureOnly()" maxlength="4" onChange="showfiledtip(re_checkNumber(this,'1','2347','<% write(RTSThreshold); %>'),rtsthrdtd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range5); %>");</script></td><td id="rtsthrdtd"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="sub-threshold"><% write(SectionHold); %></td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="fragment" size="18" value="2346" onKeyPress="figureOnly()" maxlength="4" onChange="showfiledtip(re_checkNumber(this,256,2346,'<% write(SectionHold); %>'),fragthrdtd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range6); %>");</script></td><td id="fragthrdtd"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="beacon interval"><% write(BeaconGap); %></td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="beacon" size="18" value="100"  onKeyPress="figureOnly()" maxlength="3" onChange="showfiledtip(re_checkNumber(this,20,999,'<% write(BeaconGap); %>'),beacontd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range7); %>");</script></td><td id="beacontd"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25" id="dtim interval"><% write(DTIMGap); %></td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="dtim" size="18" value="1"  onKeyPress="figureOnly()" maxlength="3" onChange="showfiledtip(re_checkNumber(this,1,255,'<% write(DTIMGap); %>'),dtimintvtd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range8); %>");</script></td><td id="dtimintvtd"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25" id="start short preamble"><% write(UsingBel); %></td>
	   <td >&nbsp;</td>
	   <td><input type="checkbox" name="short_preamblew" >
	   </td><input type="hidden" name="short_preamble">
	   </tr>
	   <tr>
	   <td align="right" height="25" id="start WMM"><% write(UsingWMM); %></td>
	   <td >&nbsp;</td>
	   <td ><input name="wmm_capablew" type="checkbox"></td>
	   </tr><input type="hidden" name="wmm_capable">
	   <tr><td colspan="4"><table width="100%"><tr>
	  <td width="25%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription7"><% write(ShowHelp); %></td>	  
	  </tr></table></td></tr>
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="3">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#WlanAdvanced')"></td>
	
  </tr>
</table>

  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
