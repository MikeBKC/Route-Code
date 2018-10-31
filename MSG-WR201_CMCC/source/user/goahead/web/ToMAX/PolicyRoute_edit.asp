<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FireWall");%>
<%langTran("PolicyRoute");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var errorstr="";
/*
/////////////////////////////交互变量/////////////////////
var PolicyNames="";//策略名
var PolicyEnables="";//启用、禁用状态
var FromIPs="";//起始地址
var EndIPs="";//结束地址
var PRIs="";//优先级
var Statuss="";//动作
var FilterTypes="";//过滤类型
var Protocols="";//协议
var DesFromPorts="";//目的起始端口
var DesEndPorts="";//目的结束端口
var DesFromIPs="";//目的起始地址
var DesEndIPs="";//目的结束地址
var SouFromPorts="";//源起始端口
var SouEndPorts="";//源结束端口
var FilterKeys="";//过滤内容

var fw_days="1111111";//生效日期
var fw_timestarts="All";//生效时间
var fw_timestops="";
*/
var addGroupNames=new Array();
<%aspOutIpGrpList();%>
<%
	getWanIfCount();
	getWanConfigCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") getOnePolicyRouteInfo(edit_Name);//需要修改
%>

var changeFlag=1;
var edit_Names = "<% write(Form("id"));%>";

tcpBusiness=new Array(21,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389);
tcpBusinessText=new Array("21(ftp)","22(ssh)","23(telnet)","25(smtp)","66(sql*net)","79(finger)","80(web)","110(pop3)","135(epmap)","139(netbios-ssn)","443(https)","445(ms-ds)","1433(ms-sql-s)","1434(ms-sql-m)","1720(h.323)","1723(pptp)","1863(msn login)","3389(ms-ts)");
udpBusiness=new Array(53,67,68,69,123,137,138,161,162,500,1701);
udpBusinessText=new Array("53(dns)","67(bootps)","68(bootpc)","69(tftp)","123(ntp)","137(netbios-ns)","138(netbios-dgm)","161(snmp)","162(snmptrap)","500(isakmp)","1701(l2tp)");
<!--
function init(){
	var f=PolicyRoute_Config;
	seldate(f,true);
	seltime(f,"All");
	if(edit_Names!=""&&edit_Names!=null){
		f.Action.value="modify";
		f.PolicyName.value=routeNames;
		f.PolicyNameold.value=routeNames;
		f.PolicyEnablew.checked=(PolicyEnables=="1"?true:false);
		f.Profile.value=Profile;
		if(srcIPs == "ipRange")
		{
		    f.FromIP.value=srcipFrom;
		    f.EndIP.value=srcipEnd;
		    f.sourceIP[0].checked = true;
		    selObject(f,"ipRange");
		}
		else if(srcIPs == "groupSel")
		{
		    f.GroupName.value = srcipGrps;
		    f.sourceIP[1].checked = true;
		    selObject(f,"groupSel");
		}else{
		
		}
		if(dstIPs == "ipRange")
		{
		    f.dstFromIP.value=dstipFrom;
		    f.dstEndIP.value=dstipEnd ;
		    f.dstIP[0].checked = true;
		    selObject1(f,"ipRange");
		}
		else if(dstIPs == "groupSel")
		{
		    f.dstGroupName.value = dstipGrps;
		    f.dstIP[1].checked = true;
		    selObject1(f,"groupSel");
		}else{
		}
			selVal(f.Protocol,protocols);
			protocolChange();
			f.DesFromPort.value=fromport;
			f.DesEndPort.value=toport;
			if (fromport == toport)
			{
			    f.CommService.value=fromport;
			}
			if ((fromport == "1") && (toport == "65535"))
			{
			    f.CommService.value=0;
			}
			if (f.CommService.value == "")
			{
			    f.CommService.value="custom";
			}
		setTimeValue(f,pr_days,pr_timestarts,pr_timestops);//common.js中的公用函数时间段赋值
	}
	else
	{
	    selObject(f,"ipRange");
	    selObject1(f,"ipRange");
	    protocolChange();
	}
	for(i=1;i<=1;i++)
		document.all["yutd"+i].innerHTML="";
}

function save(){
	var f=PolicyRoute_Config;
	if(checkData()){
		getTimeValue(f);
		if(f.day.value=="0000000"&&!f.everyday.checked){
			alert("<%write(com_selectDate);%>");
			f.day.value="";
			return;
		}
		setenable(1);
		f.PolicyEnable.value=(f.PolicyEnablew.checked==true?"1":"0");
		if(f.Action.value == "add")
			f.PolicyNameold.value= f.PolicyName.value;
		
		PolicyRoute_Config.action="/goform/formPolicyRouteConf";
		PolicyRoute_Config.submit();
	}
}
function checkData(){
	var f = PolicyRoute_Config;//alert(f.PolicyName.value);
	if(f.Action.value == "modify")
	{
	    if(f.PolicyNameold.value != f.PolicyName.value)
	    {
		alert("<%write(pr_modifyalert);%>");
		f.PolicyName.value=routeNames;
		return false;
	    }
	}
	if(f.Profile.value == "") {
	    alert("<%write(pr_profileErr);%>");
	    return;
	}
	if(!checkName(f.PolicyName,11,"<%write(pr_routename);%>",0)) return false;
	if(f.sourceIP[0].checked==true)
	{
	    if(!checkIP(f.FromIP,"<%write(com_IPRange);%>",0)) return false;
	    if(!checkIP(f.EndIP,"<%write(com_IPRange);%>",0)) return false;
	    if (compareIP(f.FromIP.value,f.EndIP.value)==1)
	    {
		alert("<%write(com_IPRangeTip);%>");
		return false;
	    }
	    if((f.FromIP.value=="0.0.0.0"&&f.EndIP.value!="0.0.0.0")||(f.FromIP.value!="0.0.0.0"&&f.EndIP.value=="0.0.0.0")){
		alert("<%write(com_IPRangeTip);%>");
		return false;
	    }
	}
	if(f.dstIP[0].checked==true)
	{
	    if(!checkIP(f.dstFromIP,"<%write(fir_fromIP);%>",0)) return false;
	    if(!checkIP(f.dstEndIP,"<%write(fir_endIP);%>",0)) return false;
	    if (compareIP(f.dstFromIP.value,f.dstEndIP.value)==1)
	    {
		alert("<%write(fir_cmpTip);%>");
		return false;
	    }
	    if((f.dstFromIP.value=="0.0.0.0"&&f.dstEndIP.value!="0.0.0.0")||(f.dstFromIP.value!="0.0.0.0"&&f.dstEndIP.value=="0.0.0.0")){
		alert("<%write(fir_destIPTip);%>");
		return false;
	    }
	}
	/*
	if(f.sourceIP[1].checked==true)
	{
	    alert(f.ipGroupName.value);
	}
	if(f.sourceIP[2].checked==true)
	{
	    alert(f.userGroupName.value);
	}
	*/
		if (f.Protocol.value=="6" || f.Protocol.value=="17"){
			if(!checkNumber(f.DesFromPort,1,65535,"<%write(fir_fromPort);%>",0)) return false;
			if(!checkNumber(f.DesEndPort,1,65535,"<%write(fir_endPort);%>",0)) return false;
			if(compareNum(f.DesFromPort.value,f.DesEndPort.value)==1){
				alert("<%write(fir_destTip);%>");
				return false;
			}
		}
	return true;
}
function checkNameAllowSpace(cntrl,maxlen,tips,allownull)
{
	var name=cntrl.value;
	if(name=="")
	{
		if(!eval(allownull))
		{
		  alert(tips+"<%write(com_null);%>");
		  cntrl.focus();
		  return false;
		}
		else
			return true;
	}
	else if (name.length > maxlen)
	{
		alert(tips+"<%write(com_chkLenTip);%>"+maxlen+"<%write(com_lenAppend);%>");
		cntrl.focus();
		return false;
	}
	else if (name.indexOf(";")!=-1||name.indexOf(",")!=-1||name.indexOf("%")!=-1 || name.indexOf("\"")!=-1 || name.indexOf(":")!=-1 || name.indexOf("\'")!=-1 || name.indexOf("\\")!=-1)
	{
		alert(tips+"<%write(com_chkName);%>");
		return false;
	}
	else
		return true;
}
function Reset(){
	PolicyRoute_Config.reset();
	init();
}
function changeFilterType(val){
	var f=PolicyRoute_Config;
	if(val=="1"){
		iptbl.style.display=dis;
		urltbl.style.display="none";
	}
	else if(val=="2" || val=="4"){
		iptbl.style.display="none";
		urltbl.style.display=dis;
	}
	else{
		iptbl.style.display="none";
		urltbl.style.display=dis;
	}
	
}
function protocolChange()
{
		var f=PolicyRoute_Config;
		clearOptions(f.CommService);
		 
		if (f.Protocol.value=="6")
		{
			createOptions(f.CommService,tcpBusiness,tcpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			option=document.createElement("OPTION");
			option.text="<%write(fir_all);%>";
			option.value=0;
			f.CommService.options.add(option);
			f.CommService.selectedIndex=18;
			tbservice.style.display= dis;
			businessChange();
		}
		else if (f.Protocol.value=="17")
		{
			createOptions(f.CommService,udpBusiness,udpBusinessText);
			custom=document.createElement("OPTION");
			custom.text="<%write(com_defSelf);%>";
			custom.value="custom";
			f.CommService.options.add(custom);
			option=document.createElement("OPTION");
			option.text="<%write(fir_all);%>";
			option.value=0;
			f.CommService.options.add(option);
			f.CommService.selectedIndex=11;
			tbservice.style.display= dis;
			businessChange();
		}
		else  if (f.Protocol.value=="0")
		{
			setPorts(0,0,0,0,0);
			tbservice.style.display="none";
		}
		else
		{
			setPorts(0,0,0,0,0);
			tbservice.style.display="none";
		}
}
function businessChange()
{
	var f=PolicyRoute_Config;
	if (changeFlag)
	{
		if (f.CommService.value==0)
		{
				setPorts(1,65535,1,65535,0);
				f.DesFromPort.disabled=false;
				f.DesEndPort.disabled=false;
		}
		else if (f.CommService.value=="custom")
		{
		    setPorts("","",1,65535,1);
		}
		else
		{
			setPorts(f.CommService.value,f.CommService.value,1,65535,1);
			isSet=0;
			for (i=0;i < tcpBusiness.length;i++)
			{
				if (f.CommService.value==tcpBusiness[i])
				{
					f.Protocol.value="6";
					isSet=1;
				}
			}
			for (i=0;i < udpBusiness.length;i++)
			{
				if (f.CommService.value==udpBusiness[i])
				{
					f.Protocol.value="17";
					isSet=1;
				}
			}
			if (isSet==0)
			{
				setPorts(1,65535,1,65535,0);
			}
		}
	}
	changeFlag=1;
}
function setPorts(portVal1,portVal2,portVal3,portVal4,portEnable)
{
		var f=PolicyRoute_Config;
		portEnable=!portEnable;
		f.DesFromPort.value=portVal1;
		f.DesEndPort.value=portVal2;
		f.DesFromPort.disabled=portEnable;
		f.DesEndPort.disabled=portEnable;
}
function setenable(arg)
{
	arg=!arg;
	var f=PolicyRoute_Config;
	f.DesFromPort.disabled=arg;
	f.DesEndPort.disabled=arg;
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
function selObject(f,val)
{
	if(val=="ipRange")
	{
		f.FromIP.disabled=false;
		f.EndIP.disabled=false;
		f.GroupName.value="";
		f.GroupName.disabled=true;
	}
	if(val=="groupSel")
	{
		f.FromIP.value="0.0.0.0";
		f.FromIP.disabled=true;
		f.EndIP.value="0.0.0.0";
		f.EndIP.disabled=true;
		f.GroupName.disabled=false;
	}
}
function selObject1(f,val)
{
	if(val=="ipRange")
	{
		f.dstFromIP.disabled=false;
		f.dstEndIP.disabled=false;
		f.dstGroupName.value="";
		f.dstGroupName.disabled=true;
	}
	if(val=="groupSel")
	{
		f.dstFromIP.disabled=true;
		f.dstFromIP.value="0.0.0.0";
		f.dstEndIP.value="0.0.0.0";
		f.dstEndIP.disabled=true;
		f.dstGroupName.disabled=false;
	}
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" >
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(pr_routeconf);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="PolicyRoute_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="95%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  

	    <tr > <td colspan="4" align="center">
	   <table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	   <td align="right" height="28" id="start ttl1"><%write(com_startUse);%></td>
	   <td >&nbsp;</td>
	   <td ><input type="checkbox" name="PolicyEnablew"  checked="checked"></td>
	   </tr><input type="hidden" name="PolicyEnable">
	   <tr><td></td><td></td><td colspan="2" class="helpfont" height="25" id="dis ttl1"><%write(fir_startTip);%></td></tr>
	      <tr>
	   <td width="25%" height="28" align="right" id="ttl1"><%write(pr_routename);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="PolicyName" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(pr_routename);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr> <input type="hidden" name="PolicyNameold" >
	    <tr>
	   <td align="right" width="25%" height="28"><%write(com_Interface);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><select name="Profile">
	 	<script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
		     if(wanIfConf[i] == 1) {
				if (is3GPort(i,wanIfCount)) {
					document.write('<option value="WAN'+i+'" >3G</option>');
				} else if (isAPCliPort(i,wanIfCount)) {
					document.write('<option value="WAN'+i+'" >APClient</option>');
				} else {
					document.write('<option value="WAN'+i+'" '+ (i==1? 'selected="selected"':'') +'>WAN'+i+'</option>');
				}
		     }
		 }
		</script> 
	   
	   </select></td>
	   </tr>
       <tr>
	   <td align="right" height="28" id="IP addrs"><%write(fir_srcIP);%></td>
	   <td ><!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;</td>
	   <td ><input type="radio" name="sourceIP" value="ipRange" onClick="selObject(this.form,this.value)" checked><%write(com_segment);%>&nbsp;&nbsp;&nbsp;<input name="FromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0">&nbsp;&nbsp;<script language="javascript">document.write(_("<%write(com_toIP);%>"));</script>&nbsp;<!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;<input name="EndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont" height="25" id="ip addr in"><%write(fir_segTip);%></td></tr>
	   <tr>
	   <td>&nbsp;</td>
	   <td>&nbsp;</td>
	   <td><input type="radio" name="sourceIP" value="groupSel" onClick="selObject(this.form,this.value)"><%write(com_userGrp);%>&nbsp;<select name="GroupName" id="GroupName">
	   <option value=""><%write(com_userAll);%></option>
	   <script language="javascript">
	   for(var i=0;i<addGroupNames.length;i++)
	   {
	   		document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
	   }
	   </script>
	   </select></td>
	   </tr>
	   <tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td></tr>
       <tr>
	   <td align="right" height="28" id="IP addrs"><%write(pr_dstip);%></td>
	   <td ><!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;</td>
	   <td ><input type="radio" name="dstIP" value="ipRange" onClick="selObject1(this.form,this.value)" checked><%write(com_segment);%>&nbsp;&nbsp;&nbsp;<input name="dstFromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0">&nbsp;&nbsp;<script language="javascript">document.write(_("<%write(com_toIP);%>"));</script>&nbsp;<!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;<input name="dstEndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont" height="25" id="ip addr in"><%write(pr_segTip);%></td></tr>
	   <tr>
	   <td>&nbsp;</td>
	   <td>&nbsp;</td>
	   <td><input type="radio" name="dstIP" value="groupSel" onClick="selObject1(this.form,this.value)"><%write(com_userGrp);%>&nbsp;<select name="dstGroupName" id="dstGroupName">
	   <option value=""><%write(com_userAll);%></option>
	   <script language="javascript">
	   for(var i=0;i<addGroupNames.length;i++)
	   {
	   		document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
	   }
	   </script>
	   </select></td>
	   </tr>
	   <tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	   </table></td></tr>
	   <tr id="iptbl" style=" display:"> <td colspan="4" align="center">
	   <table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
   <tr>
	   <td  width="25%"align="right" height="28" id="ttl9"><%write(com_protocol);%></td>
	   <td width="2%">&nbsp;</td>
	   <td ><select name="Protocol" onChange="protocolChange()">
	   <option value="6" selected>6(TCP)</option>
	    <option value="17">17(UDP)</option>
	    <option value="1">1(ICMP)</option>
	    <option value="51">51(AH)</option>
		<option value="0" id="all">all(<%write(fir_all);%>)</option>
	 
	   </select></td>

	<tr id="tbservice" style="display:">
           <td align="right" height="28" id="common service"><%write(fir_service);%></td>
           <td width="2%">&nbsp;</td>
           <td ><select name="CommService" onChange="businessChange()"><script language="javascript">
          // for(i=0;i<CommServiceArrayVal.length;i++)
                        //document.write('<option value='+CommServiceArrayVal[i]+'>'+CommServiceArrayTxt[i]+'</option>');
           </script>
           </select></td>
           </tr>

	   </tr>
	   <tr>
	   <td  align="right" height="28" id="ttl10"><%write(fir_fromPort);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input type="text" name="DesFromPort" onKeyPress="figureOnly()" size="18" maxlength="5" value="21"><img src="<%write(com_imgspace);%>" width="10" height="1" id="ttl11"><%write(fir_endPort);%>&nbsp;<img src="<%write(com_imgqt);%>" width="7" height="9">&nbsp;<input type="text" name="DesEndPort" onKeyPress="figureOnly()" size="18" maxlength="5" value="21"></td>
	   </tr>
	   <tr>
	   </tr>
</table></td></tr>
	   <tr id="urltbl" style="display:none"><td colspan="4" align="center"><table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
</table>
</td></tr><tr><td colspan="4" align="center"><table width="600" border="0" cellspacing="0" cellpadding="0">
	   <tr><td width="25%" height="28" align="right" id="act time"><b><%write(com_setTime);%></b></td><td width="2%">&nbsp;</td><td>&nbsp;</td></tr>
	   <script language="javascript">CreateEftTime();</script>
</table>
</td></tr>
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#PolicyRoute')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('PolicyRoute.asp')" id="goback"></td>
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
