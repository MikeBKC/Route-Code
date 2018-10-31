<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("NatStatic");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

var errorstr="";
var changeFlag = "0";
var InnerPorts = "80";
/*
/////////////////////////////交互变量///////////////// 
*/
 
<%
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id"); 
	if ( edit_Name!="") aspOutNatStaticOneMapInfo(edit_Name);//需要修改
%> 
//IDs = "1";ConfigEnables = "1";Protocols = "TCP";OutPorts = "1";IPs = "192.168.1.56";PortNums = "1"; NatBinds = "3"; var errorstr="";


var edit_Names = "<% write(Form("id"));%>";
//var edit_Names = "1";
tcpBusiness=new Array(21,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389);
tcpBusinessText=new Array("21(ftp)","22(ssh)","23(telnet)","25(smtp)","66(sql*net)","79(finger)","80(web)","110(pop3)","135(epmap)","139(netbios-ssn)","443(https)","445(ms-ds)","1433(ms-sql-s)","1434(ms-sql-m)","1720(h.323)","1723(pptp)","1863(msn login)","3389(ms-ts)");
udpBusiness=new Array(53,67,68,69,123,137,138,161,162,500,1701);
udpBusinessText=new Array("53(dns)","67(bootps)","68(bootpc)","69(tftp)","123(ntp)","137(netbios-ns)","138(netbios-dgm)","161(snmp)","162(snmptrap)","500(isakmp)","1701(l2tp)");

<!--


function init(){
	var f=NatStatic_Config;
	var isSet = 0;
	if(edit_Names!=""&&edit_Names!=null){
		f.ID.value=IDs;
		f.IDold.value=IDs;
		f.ConfigEnablew.checked=(ConfigEnables=="1"?true:false);
		selVal(f.Protocol,Protocols);
		f.IP.value=IPs;
		f.OutPort.value=OutPorts;
		f.PortNum.value=PortNums;
		selVal(f.NatBind,NatBinds);
		f.Action.value="modify";
	}
	f.InnerPort.value=InnerPorts;
	protocolChange();
	for(i=1;i<=5;i++){
	    document.all["yutd"+i].innerHTML="";
	}
}
function Reset(){
	NatStatic_Config.reset();
	init();
}

function save(){
	var f=NatStatic_Config;
	if(checkData()){
		if(f.Action.value=="add") {
			f.IDold.value=f.ID.value;
		}
		f.ConfigEnable.value=(f.ConfigEnablew.checked==true?"1":"0");
		NatStatic_Config.action="/goform/formNatStaticMap";
		NatStatic_Config.submit();
	}
}
function checkData(){
	var f = NatStatic_Config;
	if(!checkName(f.ID,11,"<%write(nat_staMapName);%>",0)) return false;
	if(f.ID.value.toLowerCase()=="admin"){
		alert("<%write(nat_checkTip);%>");
		return false;
	}
	if (L2TPList == 1) {
		if(f.ID.value.toLowerCase()=="l2tp"){
			alert("<%write(nat_checkTip);%>");
			return false;
		}
	}
	if (PPTPClientList == 1) {
		if(f.ID.value.toLowerCase()=="pptp"){
			alert("<%write(nat_checkTip);%>");
			return false;
		}
	}

	if(!checkIP(f.IP,"<%write(com_IP);%>",0)) return false;
	if(!checkNumber(f.OutPort,1,65535,"<%write(nat_exterFPort);%>",0)) return false;
	if(!checkNumber(f.InnerPort,1,65535,"<%write(nat_innerFPort);%>",0)) return false;
	if(!checkNumber(f.PortNum,1,500,"<%write(nat_portNum);%>",0)) return false;
	var outnum=parseInt(f.OutPort.value)+parseInt(f.PortNum.value);
	var innum=parseInt(f.InnerPort.value)+parseInt(f.PortNum.value);
	//alert(outnum+""+innum);
	if(outnum>65536||innum>65536){
		alert("<%write(nat_ckPorTip);%>");
		return false;
	}
	return true;
}

function clearOptions(cntrl)
{
	while(cntrl.length)		 cntrl.options[0] = null;
}

function createOptions(cntrl,arrayVal,arrayText)
{
	for(i=0; i<arrayVal.length; i++)
	{
		option=document.createElement("OPTION");
		option.text=arrayText[i];
		option.value=arrayVal[i];
		cntrl.options.add(option);
	}
}

function protocolChange()
{
		var f=NatStatic_Config;
		clearOptions(f.CommService);
		if (f.Protocol.value=="1")
		{
			createOptions(f.CommService,tcpBusiness,tcpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			f.CommService.selectedIndex=18;
			tbservice.style.display= dis;
			businessChange();
		}
		else if (f.Protocol.value=="2")
		{
			createOptions(f.CommService,udpBusiness,udpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			f.CommService.selectedIndex=11;
			tbservice.style.display= dis;
			businessChange();
		}
		else
		{
			tbservice.style.display="none";
			f.InnerPort.value=InnerPorts;
			changeFlag = "1";
		}
}

function businessChange()
{
    var f=NatStatic_Config;
    if (changeFlag == "0")
    {
	f.CommService.value=InnerPorts;
	if (f.CommService.value=="")
	{
	    f.CommService.value="custom";
	}
	changeFlag = "1";
    }
    else
    {
	f.InnerPort.value="";
	if (f.CommService.value != "custom")
	{
	    f.InnerPort.value=f.CommService.value;
	}
    }
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
  <td><script language="javascript">showSecondTitle('<%write(nat_staConfig);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr, 80);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="NatStatic_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
	   <td width="35%" height="25" align="right"><%write(nat_staMapName);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="ID" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(nat_staMapName);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25"><%write(com_startConf);%></td>
	   <td >&nbsp;</td>
	   <td ><input name="ConfigEnablew"  type="checkbox" checked="checked"></td><td  id="fixiptd"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont"><%write(nat_startTip);%></td></tr>
	   <input type="hidden" name="ConfigEnable">
	   <input type="hidden" name="IDold" >
	    <tr>
	   <td align="right" height="25"><%write(com_protocol);%></td>
	   <td >&nbsp;</td>
	   <td><select  name="Protocol" onChange="protocolChange()">
	   <option value="1">TCP</option>
	   <option value="2">UDP</option>
	   <option value="3">TCP/UDP</option>
	   </select></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><%write(nat_exterFPort);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="OutPort" type="text" size="18" maxlength="5" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,1,65535,'<%write(nat_exterFPort);%>'),yutd2)" value="1"></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><%write(com_IP);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="IP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont"><%write(nat_natIPTip);%></td></tr>
	   <tr id="tbservice" style="display:">
	   <td align="right" height="25"><%write(nat_service);%></td>
	   <td >&nbsp;</td>
	   <td ><select name="CommService" onChange="businessChange()"></select></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><%write(nat_innerFPort);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="InnerPort" type="text" size="18" maxlength="5" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,1,65535,'<%write(nat_innerFPort);%>'),yutd4)" value="1"></td><td  id="yutd4"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><%write(nat_portNum);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="PortNum" type="text" size="18" maxlength="5" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,1,500,'<%write(nat_portNum);%>'),yutd5)" value="1"></td><td  id="yutd5"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont"><%write(nat_portTip);%></td></tr>
	     <tr>
	   <td align="right" height="25"><%write(nat_NATBind);%></td>
	   <td >&nbsp;</td>
	   <td><select name="NatBind" >
	 	<script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
				if (is3GPort(i,wanIfCount)) {
					document.write('<option value="WAN'+i+'" >3G</option>');
				} else if (isAPCliPort(i, wanIfCount)) {
					document.write('<option value="WAN'+i+'" >APClient</option>');
				} else {
					document.write('<option value="WAN'+i+'" >WAN'+i+'</option>');
				}
		 }
		</script> 
			    
			</select></td>
	   </tr>
	   
	  <tr><td colspan="3" height="20">&nbsp;</td></tr> 
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#NatStaticEdit')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('NatStatic.asp')"></td>
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
