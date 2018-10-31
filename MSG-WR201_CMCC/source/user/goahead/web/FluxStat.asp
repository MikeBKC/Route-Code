<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title> 
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("matter");

function initTranslation()
{
	var e;
/*	e = document.getElementById("internetbyte");
	e.innerHTML= _("internetbyte");
	e = document.getElementById("wirelessbyte");
	e.innerHTML= _("wirelessbyte");
*/
	e = document.getElementById("send");
	e.innerHTML= _("send");
	e = document.getElementById("get");
	e.innerHTML= _("get");
	e = document.getElementById("bytesize");
	e.innerHTML= _("bytesize");
	e = document.getElementById("numsize");
	e.innerHTML= _("numsize");

	e = document.getElementById("clear");
	e.value= _("clear");
	e = document.getElementById("update");
	e.value= _("update");
/*
	e = document.getElementById("send2");
	e.innerHTML= _("send");
	e = document.getElementById("get2");
	e.innerHTML= _("get");
	e = document.getElementById("bytesize2");
	e.innerHTML= _("bytesize");
	e = document.getElementById("numsize2");
	e.innerHTML= _("numsize");
*/
}

var sendBytes=new Array();
var sendPacks=new Array();
var recivBytes=new Array();
var recivPacks=new Array();

<% outputWANTxRXPacketByte();%>

<% outputLanTxRXPacketByte();%>

///////////////////////////////////////交互变量///////////////
/*******有线状态******/
//var sendBytes="";var sendPacks="";var recivBytes="";var recivPacks="";
/********无线状态***************/
//var wrlesssendBytes="";var wrlesssendPacks="";var wrlessrecivBytes="";var wrlessrecivPacks="";
////////////////////////////end/////////////////////////////
function init(){
	var f=WireStatus_Config;
	if(sendBytes[0] == "")
	{
		wan1table.style.display="none";
	}
	if(sendBytes[1] == "")
	{
		wan2table.style.display="none";
	}
	if(sendBytes[2] == "")
	{
		g3table.style.display="none";
	}
	if(sendBytes[3] == "")
	{
		wlantable.style.display="none";
	}
f.wan1sendBytes.value=sendBytes[0];
	f.wan1sendPacks.value=sendPacks[0];
	f.wan1recivBytes.value=recivBytes[0];
	f.wan1recivPacks.value=recivPacks[0];
	
	f.wan2sendBytes.value=sendBytes[1];
	f.wan2sendPacks.value=sendPacks[1];
	f.wan2recivBytes.value=recivBytes[1];
	f.wan2recivPacks.value=recivPacks[1];
	
	 
	f.G3sendBytes.value=sendBytes[2];
	f.G3sendPacks.value=sendPacks[2];
	f.G3recivBytes.value=recivBytes[2];
	f.G3recivPacks.value=recivPacks[2];
		 
		
	f.WLANsendBytes.value=sendBytes[3];
	f.WLANsendPacks.value=sendPacks[3];
	f.WLANrecivBytes.value=recivBytes[3];
	f.WLANrecivPacks.value=recivPacks[3];
	
	f.LANsendBytes.value=sendBytes[4];
	f.LANsendPacks.value=sendPacks[4];
	f.LANrecivBytes.value=recivBytes[4];
	f.LANrecivPacks.value=recivPacks[4];

}
function clearStat(){
	WireStatus_Config.action="/goform/ConfigClearRxTxPacketByte";
	//WireStatus_Config.action="/goform/formRebootMachine";
	WireStatus_Config.submit();
}
function seeFlow(){
	parent.main.location='InterfaceFlow.asp';
}
/////////////////////////////////////end///////////////////////
</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("FluxStat"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form name="WireStatus_Config" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
<td class="style7">
<table border="0" width="600" align='center'>
 <tr><td colspan="5" height="26">&nbsp;</td></tr>
</table>
<table border="0" width="600" align='center' style="display:" id="wan1table">
 <tr>
		<td width="30%" align="left" height="25">&nbsp;<strong>WAN1口流量：</strong></td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" id="send">&nbsp;发送</td>
		<td  width="22%" id="get">&nbsp;接收</td>
		<td width="17%">&nbsp;</td>
	      </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="bytesize">字节数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="wan1sendBytes" class="input3" size="18" disabled></td>
		<td width="22%"><input  name="wan1recivBytes" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="numsize">数据包数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="wan1sendPacks" class="input3" size="18" disabled></td>
		<td width="22%"><input  name="wan1recivPacks" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr> <tr><td colspan="5" align="center" height="30"><table width="95%" border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
</table>
<table border="0" width="600" align='center' style="display:" id="wan2table">
   <tr>
		<td width="30%" align="left" height="25">&nbsp;<strong>WAN2口流量：</strong></td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" id="send2">&nbsp;发送</td>
		<td  width="22%" id="get2">&nbsp;接收</td>
		<td width="17%">&nbsp;</td>
	      </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="bytesize2">字节数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="wan2sendBytes" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="wan2recivBytes" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="numsize">数据包数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="wan2sendPacks" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="wan2recivPacks" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr> <tr><td colspan="5" align="center" height="30"><table width="95%" border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
</table>
<table border="0" width="600" align='center' style="display:none" id="g3table">
   <tr>
		<td width="30%" align="left" height="25">&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;  &nbsp;  &nbsp; <strong> 3G流量:</strong></td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" id="send2">&nbsp;发送</td>
		<td  width="22%" id="get2">&nbsp;接收</td>
		<td width="17%">&nbsp;</td>
	      </tr> 
		  <tr>
		<td width="30%" align="right" height="25" id="bytesize2">字节数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="G3sendBytes" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="G3recivBytes" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="numsize">数据包数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="G3sendPacks" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="G3recivPacks" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr> <tr><td colspan="5" align="center" height="30"><table width="95%" border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
</table>
<table border="0" width="600" align='center' style="display:none" id="wlantable">
   <tr>
		<td width="30%" align="left" height="25"><strong>APClient流量：</strong></td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" id="send2">&nbsp;发送</td>
		<td  width="22%" id="get2">&nbsp;接收</td>
		<td width="17%">&nbsp;</td>
	      </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="bytesize2">字节数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="WLANsendBytes" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="WLANrecivBytes" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="numsize">数据包数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="WLANsendPacks" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="WLANrecivPacks" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr> <tr><td colspan="5" align="center" height="30"><table width="95%" border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
</table>
<table border="0" width="600" align='center'>
   <tr>
		<td width="30%" align="left" height="25">&nbsp;&nbsp;&nbsp;&nbsp;<strong>LAN口流量：</strong></td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" id="send2">&nbsp;发送</td>
		<td  width="22%" id="get2">&nbsp;接收</td>
		<td width="17%">&nbsp;</td>
	      </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="bytesize2">字节数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="LANsendBytes" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="LANrecivBytes" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr>
		  <tr>
		<td width="30%" align="right" height="25" id="numsize">数据包数</td>
		<td width="5%">&nbsp;</td>
		<td  width="22%" ><input  name="LANsendPacks" class="input3" size="18" disabled></td>
		<td  width="22%"><input  name="LANrecivPacks" class="input3" size="18" disabled></td>
	      <td>&nbsp;</td>
		  </tr> 
		   <!--<tr><td colspan="5" height="40" align="center"><a onClick="seeFlow()" target="main" style="cursor :hand; color:#0000FF">查看近十分钟接口数据流量</a></td></tr>-->
		  <tr><td colspan="5" height="20">&nbsp;</td></tr>
		  <tr><td  colspan="5">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="hidden" value="1" name="hidd"><input type="button" class="input-button" value="清除" onClick="clearStat()" id="clear">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="刷新" onClick="location.reload()" id="update">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="返回" onClick="GoBack_Page('InterfaceFlow.asp')" id="returnback"></td>
  </tr>
</table>
  </td></tr> 
  </table>
</td>
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
	      
