<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FireWall");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//add addgroupname  usergrourname 交互变量
/*
var addGroupNames=new Array();
addGroupNames[0]="1";
addGroupNames[1]="2";
var userGroupNames=new Array();
userGroupNames[0]="abc";
userGroupNames[1]="def";
*/
//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//	e = document.getElementById("uttreset"); 
//	e.value= _("reset");
//	e= document.getElementById("goback");
//	e.value= _("goback");
//
//	e = document.getElementById("start ttl1");
//	e.innerHTML= _("start ttl1");
//	e = document.getElementById("dis ttl1");
//	e.innerHTML= _("dis ttl1");
//	e = document.getElementById("IP addrs");
//	e.innerHTML= _("IP addrs");
//	e= document.getElementById("ip addr in");
//	e.innerHTML= _("ip addr in");
//	e = document.getElementById("dis first");
//	e.innerHTML= _("dis first");
//	e = document.getElementById("allow");
//	e.innerHTML= _("allow");
//
//	e= document.getElementById("avoid");
//	e.innerHTML= _("avoid");
//	e= document.getElementById("IP filter");
//	e.innerHTML= _("IP filter");
//
//	e= document.getElementById("url filter");
//	e.innerHTML= _("url filter");
//	e = document.getElementById("key filter");
//	e.innerHTML= _("key filter");
//	e = document.getElementById("all");
//	e.innerHTML= _("all");
//	e = document.getElementById("common service");
//	e.innerHTML= _("common service");
//	e = document.getElementById("act time");
//	e.innerHTML= _("act time");
//	e = document.getElementById("ttl10");
//	e.innerHTML= _("ttl10");
//	e = document.getElementById("ttl11");
//	e.innerHTML= _("ttl11");
//	e = document.getElementById("ttl12");
//	e.innerHTML= _("ttl12");
//	e = document.getElementById("ttl13");
//	e.innerHTML= _("ttl13");
//	e = document.getElementById("ttl14");
//	e.innerHTML= _("ttl14");
//	e = document.getElementById("ttl15");
//	e.innerHTML= _("ttl15");
//	e = document.getElementById("ttl1");
//	e.innerHTML= _("ttl1");
//	e = document.getElementById("ttl5");
//	e.innerHTML= _("ttl5");
//	e = document.getElementById("ttl8");
//	e.innerHTML= _("ttl8");
//
//}

var errorstr="";
var DesFromPorts="80";//目的起始端口
var DesEndPorts="80";//目的结束端口
var DesFromIPs="0.0.0.0";//目的起始地址
var DesEndIPs="0.0.0.0";//目的结束地址
var SouFromPorts="1";//源起始端口
var SouEndPorts="65535";//源结束端口
var fwGrpEn = 0;

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
var FilterKeys="";//过滤内容

var fw_days="1111111";//生效日期
var fw_timestarts="All";//生效时间
var fw_timestops="";
*/
<%aspOutIpGrpList();%>
<!-- 测试 -->
<%aspOutFwGrpList();%>
<!-- 测试 -->
<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutOneFireWallRuleInfo(edit_Name);//需要修改
%>

var changeFlag=1;
var edit_Names = "<% write(Form("id"));%>";

tcpBusiness=new Array(21,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389);
tcpBusinessText=new Array("21(ftp)","22(ssh)","23(telnet)","25(smtp)","66(sql*net)","79(finger)","80(web)","110(pop3)","135(epmap)","139(netbios-ssn)","443(https)","445(ms-ds)","1433(ms-sql-s)","1434(ms-sql-m)","1720(h.323)","1723(pptp)","1863(msn login)","3389(ms-ts)");
udpBusiness=new Array(53,67,68,69,123,137,138,161,162,500,1701);
udpBusinessText=new Array("53(dns)","67(bootps)","68(bootpc)","69(tftp)","123(ntp)","137(netbios-ns)","138(netbios-dgm)","161(snmp)","162(snmptrap)","500(isakmp)","1701(l2tp)");
//protoValArray=new Array(6,17,1,2,4,47,50,51,89,9,46,"0");
//protoTextArray=new Array("6(TCP)","17(UDP)","1(ICMP)","2(IGMP)","4(IPINIP)","47(GRE)","50(ESP)","51(AH)","89(OSPF)","9(IGRP)","46(RSVP)","All(所有)");
<!--


function init(){
	var f=AccessCtrlPly_Config;
	seldate(f,true);
	seltime(f,"All");
	//protocolChange();
    if(fwGrpEn == 1)
    {
        ttl16.style.display="";
    } else{
        ttl16.style.display="none";
    }
    initFwGrpSelect(1);
	if(edit_Names!=""&&edit_Names!=null){
		f.Action.value="modify";
		f.PolicyName.value=PolicyNames;
		f.PolicyNameold.value=PolicyNames;
		f.PolicyEnablew.checked=(PolicyEnables=="1"?true:false);
		if(sourceIPs == 0)
		{
		    f.FromIP.value=FromIPs;
		    f.EndIP.value=EndIPs;
		    f.sourceIP[0].checked = true;
		    selObject(f,"ipRange");
		}
		else
		{
		    f.GroupName.value = ipGrpNames;
		    f.sourceIP[1].checked = true;
		    selObject(f,"groupSel");
		}
        //显示目的地址
		if(FilterTypes=="1")
        {
		if((destIPs == 0))
        {
            f.destFromIP.value=destFromIPs;
            f.destEndIP.value=destEndIPs;
            f.destIP[0].checked = true;
            selObject(f,"destIpRange");
        }
        else
        {
            f.destGroupName.value = destIpGrpNames;
            f.destIP[1].checked = true;
            selObject(f,"destGroupSel");
        }
        }
//		f.PRI.value=PRIs;
		selVal(f.FilterType,FilterTypes);
		changeFilterType(f.FilterType.value);

        initContent();

		selVal(f.Status,Statuss);
		setTimeValue(f,fw_days,fw_timestarts,fw_timestops);//common.js中的公用函数时间段赋值
	}
	else
	{
	    selObject(f,"ipRange");
	    protocolChange();
	}
	for(i=1;i<=1;i++)
		document.all["yutd"+i].innerHTML="";
}
function initContent()
{
	var f=AccessCtrlPly_Config;
    if((f.Action.value = "modify" ) && (f.FilterType.value == FilterTypes))
    {
    if(fwTypeEn == 0)
    {
        changeFwType("fwdiy");
        f.fwType[0].checked = true;
        if(FilterTypes=="1"){
            selVal(f.Protocol,Protocols);
                protocolChange();
                f.DesFromPort.value=DesFromPorts;
                f.DesEndPort.value=DesEndPorts;
                f.DesFromIP.value=DesFromIPs;
                f.DesEndIP.value=DesEndIPs;
                f.SouFromPort.value=SouFromPorts;
                f.SouEndPort.value=SouEndPorts;
                if (DesFromPorts == DesEndPorts)
                {
                    f.CommService.value=DesFromPorts;
                }
            if ((DesFromPorts == "1") && (DesEndPorts == "65535"))
            {
                f.CommService.value=0;
            }
            if (f.CommService.value == "")
            {
                f.CommService.value="custom";
            }
        }
        else{
            f.FilterKey.value=FilterKeys;
        }
    }else{
        f.fwType[1].checked = true;
        changeFwType("fwgrp");
        f.fwGrpName.value= fwGrpName;
    }
    }
}

function save(){
	var f=AccessCtrlPly_Config;
	if(checkData()){
		getTimeValue(f);
		if(f.day.value=="0000000"&&!f.everyday.checked){
			alert("<%write(com_selectDate);%>");
			f.day.value="";
			return;
		}
		setenable(1);
		f.PolicyEnable.value=(f.PolicyEnablew.checked==true?"1":"0");
		f.Status.disabled=false;
		if(f.Action.value == "add")
			f.PolicyNameold.value= f.PolicyName.value;
		
		AccessCtrlPly_Config.action="/goform/formFireWall";
		AccessCtrlPly_Config.submit();
	}
}
function checkData(){
	var f = AccessCtrlPly_Config;//alert(f.PolicyName.value);
	if(!checkName(f.PolicyName,11,"<%write(fir_name);%>",0)) return false;
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
	if(f.destIP[0].checked==true)
	{
	    if(!checkIP(f.destFromIP,"<%write(com_IPRange);%>",0)) return false;
	    if(!checkIP(f.destEndIP,"<%write(com_IPRange);%>",0)) return false;
	    if (compareIP(f.destFromIP.value,f.destEndIP.value)==1)
	    {
		alert("<%write(com_IPRangeTip);%>");
		return false;
	    }
	    if((f.destFromIP.value=="0.0.0.0"&&f.destEndIP.value!="0.0.0.0")||(f.destFromIP.value!="0.0.0.0"&&f.destEndIP.value=="0.0.0.0")){
		alert("<%write(com_IPRangeTip);%>");
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
//	if(! checkNumber(f.PRI,0,maxtotalrecs,"优先级",0)) return false;
	var indextext=getListValue(f.FilterType);//alert(indextext);
//	if (indextext=="5")
//	{
//	    if(!checkMac(f.FilterKey,"<%write(com_MAC);%>",0,1)) return false;
//	}
	indextext = 1;/*过滤内容选项隐藏了，默认indextext值为1*/
    if (f.fwType[0].checked == true) {

	if(indextext=="1"){
		if (f.Protocol.value=="2" || f.Protocol.value=="3"){
			if(!checkNumber(f.DesFromPort,1,65535,"<%write(fir_fromPort);%>",0)) return false;
			if(!checkNumber(f.DesEndPort,1,65535,"<%write(fir_endPort);%>",0)) return false;
			if(compareNum(f.DesFromPort.value,f.DesEndPort.value)==1){
				alert("<%write(fir_destTip);%>");
				return false;
			}
		
			if(!checkNumber(f.SouFromPort,1,65535,"<%write(fir_srcfromP);%>",0)) return false;
			if(!checkNumber(f.SouEndPort,1,65535,"<%write(fir_srcendP);%>",0)) return false;
			if(compareNum(f.SouFromPort.value,f.SouEndPort.value)==1){
				alert("<%write(fir_srcTip);%>");
				return false;
			}
		}
		
		if(!checkIP(f.DesFromIP,"<%write(fir_fromIP);%>",0)) return false;
		if(!checkIP(f.DesEndIP,"<%write(fir_endIP);%>",0)) return false;
		if (compareIP(f.DesFromIP.value,f.DesEndIP.value)==1)
		{
			alert("<%write(fir_cmpTip);%>");
			return false;
		}
		if((f.DesFromIP.value=="0.0.0.0"&&f.DesEndIP.value!="0.0.0.0")||(f.DesFromIP.value!="0.0.0.0"&&f.DesEndIP.value=="0.0.0.0")){
		alert("<%write(fir_destIPTip);%>");
		return false;
		}
		/*if ((f.DesFromIP.value=="0.0.0.0" || f.DesEndIP.value=="0.0.0.0")
						&&	(f.Protocol.value == "all")
						&&	(f.FromIP.value != f.EndIP.value)
						)
			{
				alert("目的起始地址不能为0.0.0.0。");
				return false;
			}*/
	}
	else{
		if(!checkName(f.FilterKey,31,"<%write(fir_content);%>",0)) return false;
	}
    }else{

        if(indextext=="1"){
            if(!checkIP(f.DesFromIP,"<%write(fir_fromIP);%>",0)) return false;
                if(!checkIP(f.DesEndIP,"<%write(fir_endIP);%>",0)) return false;
                    if (compareIP(f.DesFromIP.value,f.DesEndIP.value)==1)
                    {
                        alert("<%write(fir_cmpTip);%>");
                            return false;
                    }
            if((f.DesFromIP.value=="0.0.0.0"&&f.DesEndIP.value!="0.0.0.0")||(f.DesFromIP.value!="0.0.0.0"&&f.DesEndIP.value=="0.0.0.0")){
                alert("<%write(fir_destIPTip);%>");
                    return false;
            }
        }

        if(f.fwGrpName.value == "")
        {
            alert("请选择服务组!");
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
	AccessCtrlPly_Config.reset();
	init();
}
function initFwGrpSelect(val)
{
	var f=AccessCtrlPly_Config;
    if(fwGrpEn == 1)
    {
    f.fwGrpName.options.length = 0; 
    option   = document.createElement("OPTION");
    option.value  = "";
    option.text = "请选择服务组";
    f.fwGrpName.options.add(option);
    var temp ;
    var option ;
    for(var i=0;i<fwGroupNames.length;i++)
    {
        temp = fwGroupTypes[i] + 1;
        if(val == temp)
        {
            option   = document.createElement("OPTION");
            option.value  = fwGroupNames[i];
            option.text = fwGroupNames[i];
            f.fwGrpName.options.add(option);
        }
    }
    }

    /*
    if(val == 1){
        for(var i=0;i<definedServ.length;i++)
        {
            option   = document.createElement("OPTION");
            option.value  = definedServ[i];
            option.text = definedServ[i];
            f.fwGrpName.options.add(option);
        }
    }
    */
}
function changeFilterType(val){
	var f=AccessCtrlPly_Config;
    f.fwType[0].checked = true;
    fwgrptbl.style.display="none";
	if(val=="1"){
		iptbl.style.display=dis;
		urltbl.style.display="none";
		f.Status.disabled=false;
		//f.Status.value=2;
		protocolChange();
	}
	else if(val=="2" || val=="4" || val=="5"){
		f.Status.disabled=false;
		//f.Status.value=1;
		
	}
	else{
		f.Status.disabled=true;
		f.Status.value=0;
	}
	if (val!="1")
	{
        //隐藏目的地址
        newdestIp.style.display="none";
        desttip.style.display="none";
        destgrp.style.display="none";
	    iptbl.style.display="none";
	    urltbl.style.display=dis;
	    f.DesFromPort.value=DesFromPorts;
	    f.DesEndPort.value=DesEndPorts;
	    f.DesFromIP.value=DesFromIPs;
	    f.DesEndIP.value=DesEndIPs;
	    f.SouFromPort.value=SouFromPorts;
	    f.SouEndPort.value=SouEndPorts;
	}else if(val == "1"){
        newdestIp.style.display="";
        desttip.style.display="";
        destgrp.style.display="";
    }
    //初始化服务组select
    initFwGrpSelect(val);
    initContent();
}
function protocolChange()
{
		var f=AccessCtrlPly_Config;
		clearOptions(f.CommService);
		 
		if (f.Protocol.value=="2")
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
		else if (f.Protocol.value=="3")
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
		else  if (f.Protocol.value=="5")
		{
			setPorts(0,0,0,0,0);
			tbservice.style.display="none";
		}
		else
		{
			setPorts(0,0,0,0,0);
			tbservice.style.display="none";
		}
		f.DesFromIP.value="0.0.0.0";
		f.DesEndIP.value="0.0.0.0";
}
function businessChange()
{
	var f=AccessCtrlPly_Config;
	if (changeFlag)
	{
		if (f.CommService.value==0)
		{
				setPorts(1,65535,1,65535,0);
				f.SouFromPort.disabled=false;
				f.SouEndPort.disabled=false;
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
					f.Protocol.value="2";
					isSet=1;
				}
			}
			for (i=0;i < udpBusiness.length;i++)
			{
				if (f.CommService.value==udpBusiness[i])
				{
					f.Protocol.value="3";
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
		var f=AccessCtrlPly_Config;
		portEnable=!portEnable;
		f.DesFromPort.value=portVal1;
		f.DesEndPort.value=portVal2;
		f.SouFromPort.value=portVal3;
		f.SouEndPort.value=portVal4;
		f.DesFromPort.disabled=portEnable;
		f.DesEndPort.disabled=portEnable;
		f.SouFromPort.disabled=portEnable;
		f.SouEndPort.disabled=portEnable;
}
function setenable(arg)
{
	arg=!arg;
	var f=AccessCtrlPly_Config;
	f.DesFromPort.disabled=arg;
	f.DesEndPort.disabled=arg;
	f.SouFromPort.disabled=arg;//源起始端口
	f.SouEndPort.disabled=arg;//源结束端口
	f.DesFromIP.disabled=arg;//目的起始地址
	f.DesEndIP.disabled=arg;//目的结束地址
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
		f.GroupName.disabled=true;
	}
	if(val=="groupSel")
	{
		f.FromIP.disabled=true;
		f.EndIP.disabled=true;
		f.GroupName.disabled=false;
	}
	if(val=="destIpRange")
	{
		f.destFromIP.disabled=false;
		f.destEndIP.disabled=false;
		f.destGroupName.disabled=true;
	}
	if(val=="destGroupSel")
	{
		f.destFromIP.disabled=true;
		f.destEndIP.disabled=true;
		f.destGroupName.disabled=false;
	}
}
function changeFwType(val)
{
	var f=AccessCtrlPly_Config;
    var hiddentab = "iptbl";
    var filterType = f.FilterType.value; 
    if(filterType != 1){
        hiddentab = "urltbl";
    }

	if(val=="fwdiy")
	{
        document.getElementById(hiddentab).style.display='';
        document.getElementById("fwgrptbl").style.display='none';
	}
    else if(val=="fwgrp")
	{
        document.getElementById(hiddentab).style.display='none';
        document.getElementById("fwgrptbl").style.display='';
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
  <td><script language="javascript">showSecondTitle(_("<%write(fir_ctlConf);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="AccessCtrlPly_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="95%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  

	    <tr > <td colspan="4" align="center">
	   <table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
	      <tr>
	   <td width="25%" height="28" align="right" id="ttl1"><%write(fir_name);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="PolicyName" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(fir_name);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr> <input type="hidden" name="PolicyNameold" >
	    <tr>
	   <td align="right" height="28" id="start ttl1"><%write(fir_start);%></td>
	   <td >&nbsp;</td>
	   <td ><input type="checkbox" name="PolicyEnablew"  checked="checked"></td>
	   </tr><input type="hidden" name="PolicyEnable">
	   <tr><td></td><td></td><td colspan="2" class="helpfont" height="25" id="dis ttl1"><%write(fir_startTip);%></td></tr>

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
/*
	   for(var j=0;j<userGroupNames.length;j++)
	   {
	   		document.write('<option value='+userGroupNames[j]+'>'+userGroupNames[j]+'</option>');
	   }*/
	   </script>
	   </select></td>
	   </tr>

	   <tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	   <!--<tr>
	   <td>&nbsp;</td>
	   <td>&nbsp;</td>
	   <td><input type="radio" name="sourceIP" value="userGroup">用户组&nbsp;<select name="userGroupName" id="userGroupName">
	   <option value="AllUser">所有用户</option>
	   <script language="javascript">
	   for(var i=0;i<userGroupNames.length;i++)
	   {
	   		document.write('<option value='+userGroupNames[i]+'>'+userGroupNames[i]+'</option>');
	   }
	   </script>
	   </select></td>
	   </tr>-->
	   <!-- <tr style="display:none">
	   <td align="right" height="28" id="ttl4">优先级</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td ><input name="PRI" type="text" size="18" maxlength="15" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,0,maxtotalrecs,'优先级'),yutd2)" value="50"></td><td  id="yutd2"></td>
	   </tr>-->

       <!--  测试 -->
       <tr id="newdestIp" name="newdestIp">
	   <td align="right" height="28" id="IP addrs">目的地址</td>
	   <td >&nbsp;</td>
	   <td ><input type="radio" name="destIP" value="destIpRange" onClick="selObject(this.form,this.value)" checked><%write(com_segment);%>&nbsp;&nbsp;&nbsp;<input name="destFromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0">&nbsp;&nbsp;<script language="javascript">document.write(_("<%write(com_toIP);%>"));</script>&nbsp;&nbsp;<input name="destEndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()" value="0.0.0.0"></td>
	   </tr>
	   <tr id="desttip" name="desttip"><td></td><td></td><td colspan="2" class="helpfont" height="25" id="ip addr in"><%write(fir_segTip);%></td></tr>
	   <tr id="destgrp" name="destgrp">
	   <td>&nbsp;</td>
	   <td>&nbsp;</td>
	   <td><input type="radio" name="destIP" value="destGroupSel" onClick="selObject(this.form,this.value)"><%write(com_userGrp);%>&nbsp;<select name="destGroupName" id="destGroupName">
	   <option value=""><%write(com_userAll);%></option>
	   <script language="javascript">
	   for(var i=0;i<addGroupNames.length;i++)
	   {
	   		document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
	   }
	   </script>
	   </select></td>
	   </tr>
       </span>
       <!--  测试 -->

	    <tr style="display:none">
	   <td align="right" height="28">&nbsp;</td>
	   <td ></td>
	   <td class="helpfont" id="dis first"><%write(com_priTip);%></td>
	   </tr>
	    <tr>
	   <td align="right" height="28" id="ttl5"><%write(fir_action);%></td>
	   <td >&nbsp;</td>
	   <td ><select name="Status">
	   <option value="1" id="allow"><%write(com_allow);%></option>
	   <option value="0" id="avoid"><%write(fir_ban);%></option>
	   </select></td>
	   </tr>
	    <tr>
	   <td align="right" height="30" id="ttl7"><%write(fir_filType);%></td>
	   <td >&nbsp;</td>
	   <td><select name="FilterType" onChange="changeFilterType(this.value)">
	   <option value="1" id="IP filter"><%write(fir_iponly);%></option>
	   <option value="2" id="url filter"><%write(fir_urlFilter);%></option>
	   <option value="3" id="key filter"><%write(fir_keyFilter);%></option>
	   <option value="4" id="dns filter"><%write(fir_dnsFilter);%></option>
    <!--   <option value="5" id="mac filter"><%write(fir_macFilter);%></option> -->
	   </select></td>
	   </tr>

<!--  测试  -->
	   <tr id="ttl16" name="ttl16" style="display:none">
	   <td align="right" height="30">服务类型</td>
	   <td >&nbsp;</td>
	   <td>
            <input type="radio" name="fwType" id="fwType" value="fwdiy" onClick="changeFwType(this.value)" checked/> 自定义
            <input type="radio" name="fwType" id="fwType" value="fwgrp" onClick="changeFwType(this.value)"/> 服务组
	   </td>
	   </tr>

	   <tr id="fwgrptbl" style=" display:none;">
	        <td align="right" height="30">服务组</td>
	        <td >&nbsp;</td>
	        <td>
                <select name="fwGrpName" width="500" id="fwGrpName">
                    <option value="">请选择服务组</option>
                </select>
            </td>
       </tr>
<!--  测试  -->

	   </table></td></tr>
	   <tr id="iptbl" style=" display:"> <td colspan="4" align="center">
       <table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
   <tr>
	   <td  width="25%"align="right" height="28" id="ttl9"><%write(com_protocol);%></td>
	   <td width="2%">&nbsp;</td>
	   <td ><select name="Protocol"  onChange="protocolChange()">
	   <option value="1">1(ICMP)</option>
	   <option value="2" selected>6(TCP)</option>
	    <option value="3">17(UDP)</option>
	    <option value="4">51(AH)</option>
		<option value="5" id="all">all(<%write(fir_all);%>)</option>
	 
	   </select></td>
	   </tr>
	    <tr id="tbservice" style="display:">
	   <td align="right" height="28" id="common service"><%write(fir_service);%></td>
	   <td width="2%">&nbsp;</td>
	   <td ><select name="CommService" onChange="businessChange()"><script language="javascript">
	  // for(i=0;i<CommServiceArrayVal.length;i++)
	   		//document.write('<option value='+CommServiceArrayVal[i]+'>'+CommServiceArrayTxt[i]+'</option>');
	   </script>
	   </select></td>
	   </tr>
	   <tr>
	   <td  align="right" height="28" id="ttl10"><%write(fir_fromPort);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input type="text" name="DesFromPort" onKeyPress="figureOnly()" size="18" maxlength="5" value="21"><img src="<%write(com_imgspace);%>" width="10" height="1" id="ttl11"><%write(fir_endPort);%>&nbsp;<img src="<%write(com_imgqt);%>" width="7" height="9">&nbsp;<input type="text" name="DesEndPort" onKeyPress="figureOnly()" size="18" maxlength="5" value="21"></td>
	   </tr>
	    <tr id="olddestIp" name="olddestIp" style="display:none">
	   <td  align="right" height="28" id="ttl12"><%write(fir_fromIP);%></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="DesFromIP" onKeyPress="ipOnly()" size="18" maxlength="15" value="0.0.0.0"><img src="<%write(com_imgspace);%>" width="10" height="1" id="ttl13"><%write(fir_endIP);%>&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="DesEndIP" onKeyPress="ipOnly()" size="18" maxlength="15" value="0.0.0.0"></td>
	   </tr>
	   <tr>
	   <td  align="right" height="28" id="ttl14"><%write(fir_srcfromP);%></td>
	   <td >&nbsp;</td>
	   <td ><input type="text" name="SouFromPort" onKeyPress="figureOnly()" size="18" maxlength="5" value="1"><img src="<%write(com_imgspace);%>" width="10" height="1" id="ttl15">&nbsp;&nbsp;&nbsp;&nbsp;<%write(fir_srcendP);%>&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="SouEndPort" onKeyPress="figureOnly()" size="18" maxlength="5" value="65535"></td>
	   </tr>
</table></td></tr>
	   <tr id="urltbl" style="display:none"><td colspan="4" align="center"><table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
   <tr>
	   <td  width="25%"align="right" height="28" id="ttl8"><%write(fir_content);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input type="text" name="FilterKey" size="18"></td>
	   </tr>
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
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#FireWallHl')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('FireWall.asp')" id="goback"></td>
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
