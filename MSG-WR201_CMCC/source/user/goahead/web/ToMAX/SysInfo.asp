<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
 <% langTran("SysInfo");%>
 <% langTran("common");%>
<title><%write(com_utt);%></title>
<script type="text/javascript" src="js/config.js"></script>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk"><style type="text/css">
div,span{padding:0px;margin:0px;}
.style24 {border:1px solid #408080}
.container{margin:0 auto;text-align:center;width:95%;background:#ffffff;}
.btn_div{margin:0 auto;text-align:right;width:95%;margin:10px 0px;}
.title_div{padding-left:30px;line-height:26px;height:26px;background:#6bb23a;color:#ffffff;text-align:left;font-size:14px;}

.pf0{padding-left:0px;}

.list{text-align:center;width:920px;margin:0 auto;}
.mt10{margin-top:10px;}
.ml30{margin-left:30px;}
.usestype{width:120px;height:19px;border:1px solid #6bb23a;display:inline-block;}
#cpu1,#mem,#tf{margin:1px;background:#6bb23a;display:inline-block;}

</style>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

var GateWays=new Array();
var IPs=new Array();
var Masks=new Array();
var ConnStatuss=new Array();
var SpeedDowns=new Array();
var SpeedUps=new Array();
var ConnTimes=new Array();
var ConnTypes=new Array();
var PortNamesw=new Array(); 
var ConnTypesw=new Array();
var ConnStatussw=new Array();
var IPsw=new Array();
var Masksw=new Array();
var GateWaysw=new Array();
var Macs=new Array();
var MainDns=new Array();
var SenDns=new Array();
var ConnOTimes = new Array();
var ConnStatussw = new Array();
var ConnStatusw =  new Array();
var ApSZ = new Array();
var ApSSID = new Array();
var ApMacForWlan = new Array();

<% outputSystemNowTime();%>
<% outputSystemVersion();%>
<% outputSystemUpTime();%>
<% outputSystemCpu();%>
<% outputSystemMemory();%>
<% outputFlashStat();%>
<% outputSystemSn();%>
<% outputProductID();%>

<%outputLANConfig(); %>
<%aspIspRunningConfig();%>
<%aspWanConfig(); %>

<%outputWlanBaseInfo(); %>

if (platform == "nv1800N")
{
    <%aspOutPutApBasicInfoUrcp();%>
    if ( typeof SZ!="undefined" && SZ!="")
    {
	ApSZ = SZ;
    }
    if ( typeof ssid!="undefined" && ssid!="")
    {
	ApSSID = ssid;
    }
    if ( typeof macForWlan!="undefined" && macForWlan!="")
    {
	ApMacForWlan = macForWlan;
    }

}

MainDns[1] = "<%getWanDns(1, 1);%>";
SenDns[1] = "<%getWanDns(1, 2);%>";
MainDns[2] = "<%getWanDns(2, 1);%>";
SenDns[2] = "<%getWanDns(2, 2);%>";
MainDns[3] = "<%getWanDns(3, 1);%>";
SenDns[3] = "<%getWanDns(3, 2);%>";
MainDns[4] = "<%getWanDns(4, 1);%>";
SenDns[4] = "<%getWanDns(4, 2);%>";
			
			
var errorstr="";
var newtotalrecs = totalrecs+1;
var maxtotalrecs = newtotalrecs;
 var strSecondMenuFocus = "运行状态";



function init(){
	 
	var i,j;
	if(platform == "nv1800N") 
	{
	    i=0;
	    newtotalrecs = newtotalrecs - 1;
	    maxtotalrecs = newtotalrecs;
	}
	else 
	{
	    i=1;
	}
				
	for(; i< newtotalrecs; i++){
			if(platform != "nv1800N") j = i-1;
			else j = i;
			IPsw[i] = IPs[j];
			ConnTypesw[i] = ConnTypes[j];
			Masksw[i] = Masks[j];
 			GateWaysw[i] = GateWays[j];
			ConnStatussw[i]= ConnStatuss[j];
			Macs[i] = mac[j];
			ConnOTimes[i] = ConnTimes[j];

			var runday=parseInt((eval(ConnOTimes[i])/3600)/24);
			var runhour=parseInt((eval(ConnOTimes[i])/3600)%24);
			var runmiu=parseInt((eval(ConnOTimes[i])%3600)/60);
			var runsec=parseInt((eval(ConnOTimes[i])%3600)%60);
			ConnOTimes[i] = runday+"天"+runhour+"时"+runmiu+"分"+runsec+"秒";
	
	
			if(ConnTypesw[i]=="STATIC"){
				 ConnTypesw[i]="固定IP接入";
			}
			else if(ConnTypesw[i]=="PPPOE") {
				ConnTypesw[i]="PPPoE接入";
			}
			else if(ConnTypesw[i]=="DHCP"){
				ConnTypesw[i]="动态IP接入";
			} else if (ConnTypesw[i]=="3G") {
				ConnTypesw[i]="3G接入";
			}
			ConnStatusw[i] = (ConnStatussw[i]=="1"?"已连接":"未连接");
			if(platform != "nv1800N")
			{
				if (is3GPort(i)) {
					PortNamesw[i] = "3G";
					Macs[i] = "";
				} else if (isAPCliPort(i, newtotalrecs - 1)) {
					PortNamesw[i] = "APClient";
				} else {
					PortNamesw[i] = "WAN"+ i;
				}
				
				if(i != 0 && ConnTypesw[i] == ""){
					IPsw[i]=Masksw[i]=GateWaysw[i]=Macs[i]=MainDns[i]=SenDns[i]=ConnOTimes[i]=ConnStatusw[i]="";
					ConnTypesw[i]="<font color=red>未配置</font>";
				}
			}
			else {PortNamesw[i]="LAN";}
			/*if(i != 0 && ConnStatussw[i] != "1"){
				IPsw[i]=Masksw[i]=GateWaysw[i]=Macs[i]=MainDns[i]=SenDns[i]=ConnOTimes[i]="";
				
			}*/
	   }
	   if(platform != "nv1800N")
	   {
			IPsw[0]=lanIps; 
			Masksw[0]=lanNMs; 
			Macs[0]=lanMacs; 
			ConnTypesw[0] = "";
			ConnStatusw[0] = "";
			PortNamesw[0] = "LAN";
			GateWaysw[0] = "";
			MainDns[0] = "";
			SenDns[0] = "";
			ConnOTimes[0] = "";
	   }
				
}

maxrecs[0]=maxtotalrecs;   //列表最大显示量
currpage[0]=1;
recscount[0]=newtotalrecs;
recsPerPage[0] =5;
lineButton[0]=0;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="";
var titles1=new Array();
var index = 0;
titles1[index++]=_("接口");
titles1[index++]=_("连接类型");
titles1[index++]=_("连接状态");
titles1[index++]=_("IP地址");
titles1[index++]=_("子网掩码");
titles1[index++]=_("网关地址");
titles1[index++]=_("MAC地址");
if(platform != "nv1800N")
{
	titles1[index++]=_("主DNS服务器");
	titles1[index++]=_("备DNS服务器");
}
titles1[index++]=_("连接时间");
if(platform != "nv1800N")
{
	arrayData1=new Array(PortNamesw,ConnTypesw,ConnStatusw,IPsw,Masksw,GateWaysw,Macs,MainDns,SenDns,ConnOTimes);
	sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1);
	sortType[0]=new Array(TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_DATE);
	arrayCount[0]=10;
}
else
{
	maxrecs[0]=maxtotalrecs - 1;   //列表最大显示量
	recscount[0]=newtotalrecs - 1 ;
	arrayData1=new Array(PortNamesw,ConnTypesw,ConnStatusw,IPsw,Masksw,GateWaysw,Macs,ConnOTimes);
	sortable[0]=new Array(0,0,0,0,0,0,0,0);
}

function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,titles1.length,arrayData1,0,0);
}


init();

if (cpuStat < 0) {
	cpuStat	 = 0;
}
if (memStat <0) {
	memStat = 0;
}

//进行时间处理
sysDate=new Date(UTCtoLocal(eval(sysDateTimes),0) * 1000);
	Year= sysDate.getFullYear();
	Month= sysDate.getMonth();
	Day= sysDate.getDate();
	if (10 > sysDate.getHours())
		Hour="0"+sysDate.getHours();
	else
		Hour=""+sysDate.getHours();
	if (10 > sysDate.getMinutes())
		Minute="0"+sysDate.getMinutes();
	else
		Minute=""+sysDate.getMinutes();
	if (10 > sysDate.getSeconds())
		Second="0"+sysDate.getSeconds();
	else
		Second=""+sysDate.getSeconds();
	Month1=Month+1;//路由器内部0表示1月，1表示2月……
	var systimes=Year.toString()+"-"+Month1.toString()+"-"+Day.toString()+"   "+Hour.toString()+":"+Minute.toString()+":"+Second.toString();
//end
var outerFlash=null;
var outerFlashStat=0;
var outerFlashUsedSpace=0;
var http_request = false;
function init_globalData()
{
	outerFlashUnusedSpace=convertToByte(outerFlashUnusedSpace);
	outerFlashTotalSpace=convertToByte(outerFlashTotalSpace);
	outerFlashUsedSpace=outerFlashTotalSpace-outerFlashUnusedSpace;
	if(outerFlashTotalSpace)
	{
		if(!(parseInt(outerFlashTotalSpace)))/*总空间大小为0*/
		{
			outerFlashTotalSpace =false;
		}
	}
	if(outerFlashTotalSpace)
	{
		/*使用率*/
		outerFlashStat = Math.round(outerFlashUsedSpace/outerFlashTotalSpace*100);
	}
}
init_globalData();

function pageInit()
{
	parent.menu.openSysInfo1();	
	outerFlash = document.getElementById('outerFlash');
	var flag1 = false;
	var flag2 = false;
	var flag3 = false;
	/* 控制div高 */
	if (outerFlashTotalSpace || (hardwareID != "")) {
		//HisInfo1.style.height = "122";
		//HisInfo2.style.height = "122";
	} else {
		//HisInfo1.style.height = "92";
		//HisInfo2.style.height = "92";		
	}
//	if (hardwareID != "") {
//		hwmodeltr.style.display = dis;
//	}
        document.getElementById("ProductID").innerHTML = ProductID;
        document.getElementById("revisions").innerHTML = revisions;
        document.getElementById("hardwareID").innerHTML = hardwareID;
        document.getElementById("ProductIDs").innerHTML = productIDs;
        document.getElementById("runtimes").innerHTML = runtimes;
        document.getElementById("systimes").innerHTML = systimes;
	if(wireless)
        {   
	    document.getElementById("wireless").style.display = "";    
	    if(WrlessEnables == "on")
		document.getElementById("WrlessEnables").innerHTML = "启用";
	    else
		document.getElementById("WrlessEnables").innerHTML = "关闭";
	    if(channels == 0)
		document.getElementById("channels").innerHTML = "自动";
	    else
		document.getElementById("channels").innerHTML = channels;
	    if(wrlessModes == 4)
		document.getElementById("wrlessModes").innerHTML = "仅11g";
	    else if(wrlessModes == 6)
		document.getElementById("wrlessModes").innerHTML = "仅11n";
	    else if(wrlessModes == 9)
		document.getElementById("wrlessModes").innerHTML = "11b/g/n混合";
	    document.getElementById("ssids").innerHTML = ssids;
	    document.getElementById("ssid2s").innerHTML = ssid2s;
	    if(APWorkModes == 1)
		document.getElementById("wds").innerHTML = "AP Mode";
	    if(APWorkModes == 3)
		document.getElementById("wds").innerHTML = "Repeater Mode";
	    if(APWorkModes == 2)
		document.getElementById("wds").innerHTML = "Bridge Mode";
	    if(APWorkModes == 4)
	    document.getElementById("wds").innerHTML = "Lazy Mode";
            if(wireless_5g)
	    {
		document.getElementById("wireless_5g").style.display = "";
		if(WrlessEnables_5g == "on")
		    document.getElementById("WrlessEnables_5g").innerHTML = "启用";
		else
		    document.getElementById("WrlessEnables_5g").innerHTML = "关闭";
		if(channels_5g == 0)
		    document.getElementById("channels_5g").innerHTML = "自动";
		else
		    document.getElementById("channels_5g").innerHTML = channels_5g;
		if(wrlessModes_5g == 2)
		    document.getElementById("wrlessModes_5g").innerHTML = "仅11a";
		else if(wrlessModes_5g == 8)
		    document.getElementById("wrlessModes_5g").innerHTML = "11a/n混合";
		else if(wrlessModes_5g == 14)
		    document.getElementById("wrlessModes_5g").innerHTML = "11vht AC/AN/A";
		else if(wrlessModes_5g == 15)
		    document.getElementById("wrlessModes_5g").innerHTML = "11vht AC/AN";
		document.getElementById("ssids_5g").innerHTML = ssids_5g;
		document.getElementById("ssid2s_5g").innerHTML = ssid2s_5g;
		if(APWorkModes_5g == 1)
		    document.getElementById("wds_5g").innerHTML = "关闭";
		if(APWorkModes_5g == 3)
		    document.getElementById("wds_5g").innerHTML = "开启";
	    }
	    if ( platform == "nv1800N")
	    {
		document.getElementById("SSID2").style.display = "none";
		document.getElementById("WDS_24G").style.display = "none";
		if (ApSZ == "")
		{
		    document.getElementById("SZ").style.display = "none";
		}
		else
		{
		    document.getElementById("SZs").innerHTML = ApSZ;
		}
		if (ApSSID != "")
		{
		    document.getElementById("ssids").innerHTML = ApSSID;
		}
		if (ApMacForWlan != "")
		{
		    document.getElementById("ApMacForWlans").innerHTML = ApMacForWlan;
		}
		else
		{
		    document.getElementById("ApMacForWlan").style.display = "none";
		}
	    }
	    else
	    {
		document.getElementById("SZ").style.display = "none";
		document.getElementById("ApMacForWlan").style.display = "none";
	    }
	}
	if(outerFlashTotalSpace)
	{
	    document.getElementById("trOuterFlash").style.display = dis;
	}
	else
	{
	    document.getElementById("tf").title = "<%write(UsedSpace);%>"+byteFormat(outerFlashUsedSpace)+"\n<%write(TotalSize);%>"+byteFormat(outerFlashTotalSpace);
	}

	if (cpuStat == 0) {
		document.getElementById("cpu1").style.background = "#FFFFFF";
		flag1 = true;
	}

	if (memStat == 0) {
		document.getElementById("mem").style.background = "#FFFFFF";
		flag2 = true;
	}
	
 	if (outerFlashStat == 0) {
		document.getElementById("tf").style.background  = "#FFFFFF";
		flag3 = true;
	}
	if (!flag1) {
		document.getElementById("cpu1").style.width = cpuStat + "%";
	}

	if (!flag2) {
		document.getElementById("mem").style.width = memStat + "%";
	}
	if (!flag3) {
//		outerFlash.width = outerFlashStat + "%";
		document.getElementById("tf").style.width = outerFlashStat + "%";
	}
	chgColor();
}
function chgTableColor(td) {

	if (td == null) return;
	var color = "";
	var width = eval((td.style.width.substring(0, td.style.width.length - 1)));

	if (width >= 70) {
		td.style.background = "#FF0000";
	}
	else if (width >= 50 && width < 70) {
		td.style.background = "#FFCC00";
	}
	else if (width >= 0 && width < 50) {
		td.style.background = "#00FF66";
	}
}
function getElem(id){
 return document.getElementById(id);
}
function chgColor() {
	var natSess = 0;

	if (cpuStat != 0) {
		chgTableColor(getElem("cpu1"));
	}
	if (memStat != 0) {
		chgTableColor(getElem("mem"));
	}
	if (outerFlashStat != 0) {
		chgTableColor(getElem("tf"));
	}
}
 

//end交互
</script>

</head>
<body onload="pageInit()" onbeforeunload="logOut()" >
<form name="formRunStatus" method="post" action="">

<div class="btn_div"><input type="button" class="input-button" value="<% write(com_fresh); %>" onClick="location.reload()" id="update"></div>

<div class=" container style7">

	<div class="version_div">
		<div class="title_div">版本信息</div>
		<br>
		<table style="width:100%;" border="0">
			<tr>
				<td align="left" width="20%" ><div class="ml30">产品型号：<span id="ProductID"></span></div></td>
				<td width="25%">&nbsp;</td>
				<td align="left"><div>软件版本：<span id="revisions"></span></div></td>
			</tr>
			<tr>
				<td align="left" width="20%" ><div class="ml30">硬件版本：<span id="hardwareID"></span></div></td>
				<td width="25%">&nbsp;</td>
				<td align="left"><div>序列号：<span id="ProductIDs"></span></div></td>
			</tr>
		</table>
	</div>
	<br>
	<div class="time_div">
		<div class="title_div">系统时间</div>
		<br>
		<table style="width:100%;" border="0">
			<tr>
				<td align="left" width="30%" ><div class="ml30">当前系统时间：<span id="systimes"></span></div></td>
				<td width="15%">&nbsp;</td>
				<td align="left"><div>系统运行时间：<span id="runtimes"></span></div></td>
			</tr>
		</table>

	</div>
	<br>
	<div class="interstatus_div">
		<div class="title_div">接口状态</div> 
		<div class="list">

			<iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" scrolling="no" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe>
		 
			<iframe src="_list1.htm" name="frame12"	 id="framelist"  align="MIDDLE" width="920" height="160" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
		    
		</div>
          

	</div>
	<div class="wireless_div" id="wireless" style="display:none">
		<div class="title_div">无线状态</div>
		<br>
		<table width="100%" border="0">
			<tr>
			<td width="45%" align="left">
				<table style="width:100%;" border="0">
					<tr>
						<td align="right" width="28%" ><div class="ml30"><span>2.4G</span>无线功能：</div></td>
						<td align="left"><div><span id="WrlessEnables"></span></div></td>
					</tr>
					<tr>
						<td align="right" ><div class="ml30">信道：</div></td>
						<td align="left"><div><span id="channels"></span></div></td>
					</tr>
					<tr>
						<td align="right"><div class="ml30">模式：</div></td>
						<td align="left"><div><span id="wrlessModes"></span></div></td>
					</tr>
    					<tr>
						<td align="right"><div class="ml30">SSID：</div></td>
						<td  align="left"><div><span id="ssids"></span></div></td>
					</tr>
					<tr id="SSID2">
						<td align="right"><div class="ml30">SSID2：</div></td>
						<td align="left"><div><span id="ssid2s"></span></div></td>
					</tr>
					<tr id="SZ">
						<td align="right"><div class="ml30">服务区：</div></td>
						<td  align="left"><div><span id="SZs"></span></div></td>
					</tr>
					<tr id="ApMacForWlan">
						<td align="right"><div class="ml30">无线MAC地址：</div></td>
						<td  align="left"><div><span id="ApMacForWlans"></span></div></td>
					</tr>
					<tr id="WDS_24G">
						<td align="right"><div class="ml30">WDS状态：</div></td>
						<td align="left"><div><span id="wds"></span></div></td>
					</tr>
				</table>
			
			</td>
			<td width="55%" align="left">
			<table style="width:100%;display:none" border="0" id="wireless_5g">
					<tr>
						<td align="right" width="15%" ><div><span>5G</span>无线功能：</div></td>
						<td align="left"><div><span id="WrlessEnables_5g">dsd</span></div></td>
					</tr>
					<tr>
						<td align="right" width="15%" ><div>信道：</div></td>
						<td align="left"><div><span id="channels_5g"></span></div></td>
					</tr>
					<tr>
						<td align="right" width="15%" ><div>模式：</div></td>
						<td align="left"><div><span id="wrlessModes_5g"></span></div></td>
					</tr>
					<tr>
						<td align="right" width="15%" ><div class="ml30">SSID：</div></td>
						<td align="left"><div><span id="ssids_5g"></span></div></td>
					</tr>
					<tr>
						<td align="right" width="15%" ><div>SSID2：</div></td>
						<td align="left"><div><span id="ssid2s_5g"></span></div></td>
					</tr>
					<tr>
						<td align="right" width="15%" ><div>WDS状态：</div></td>
						<td align="left"><div><span id="wds_5g"></span></div></td>
					</tr>
				</table>
			
			</td>
			</tr>
		</table>
	</div>
	<br>
	<div class="sysres_div">
		<div class="title_div">系统资源状态</div>
		<br>
		<table style="width:100%;" border="0">
			<tr>
				<td align="left" width="40.33%" ><div class="ml30">CPU使用率：
					<span class="usestype"><span id="cpu1" width="17%">&nbsp;</span></span><script>
	document.write(cpuStat.toString() + "%");
	</script></div></td>
				<td width="34.33%"><div>内存使用率：
					<span class="usestype"><span id="mem" width="17%">&nbsp;</span></span><script>
	document.write(memStat.toString() + "%");
	</script></div></td>
				<td><div id="trOuterFlash" style="display:none">
				U盘使用率：
					<span class="usestype"><span id="tf" width="17%">&nbsp;</span></span><script>
	document.write(outerFlashStat.toString() + "%");
	</script>
				</div></td>
			</tr>
		</table>
		
	</div>
	<br>
</div>
  </form>
</body>
</html>
	      
