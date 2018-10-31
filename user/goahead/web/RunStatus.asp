<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("RunStatus");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml"> 
 
//Butterlate.setTextDomain("matter");
 var strSecondMenuFocus = "<%write(com_runStatus);%>";
//function initTranslation()
//{
/*	var e;
//	e = document.getElementById("WANsign");
//	e.innerHTML= _("WANsign");	
	e = document.getElementById("leixing");
	e.innerHTML= _("leixing");
	e = document.getElementById("zhuangtai");
	e.innerHTML= _("zhuangtai");
	e = document.getElementById("ipaddr");
	e.innerHTML= _("ip addr");
	e = document.getElementById("subnet");
	e.innerHTML= _("subnet");
	e = document.getElementById("gateway");
	e.innerHTML= _("gateway");
	e = document.getElementById("MACaddr");
	e.innerHTML= _("MAC addr");
	e = document.getElementById("zhuDNS");
	e.innerHTML= _("zhuDNS");
	e = document.getElementById("beiDNS");
	e.innerHTML= _("beiDNS");
 
//	e = document.getElementById("LANsign");
//	e.innerHTML= _("LANsign");
	e = document.getElementById("ipaddr2");
	e.innerHTML= _("ip addr");
	e = document.getElementById("subnet2");
	e.innerHTML= _("subnet");
	e = document.getElementById("MACaddr2");
	e.innerHTML= _("MAC addr");
 
	e = document.getElementById("update");
	e.value= _("update");*/
//}
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

<%outputLANConfig(); %>
<%aspIspRunningConfig();%>
<%aspWanConfig(); %>

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
if(platform == "nv1800N")
{
	newtotalrecs = totalrecs;
	maxtotalrecs = totalrecs;
}


function init(){
	 
	var i,j;
	if(platform == "nv1800N") i=0;else i=1;
				
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
			ConnOTimes[i] = runday+"<%write(run_day);%>"+runhour+"<%write(run_hour);%>"+runmiu+"<%write(run_minute);%>"+runsec+"<%write(run_second);%>";
	
	
			if(ConnTypesw[i]=="STATIC"){
				 ConnTypesw[i]="<%write(com_StaticIP);%>";
			}
			else if(ConnTypesw[i]=="PPPOE") {
				ConnTypesw[i]="<%write(com_PPPoE);%>";
			}
			else if(ConnTypesw[i]=="DHCP"){
				ConnTypesw[i]="<%write(com_DynamicIP);%>";
			} else if (ConnTypesw[i]=="3G") {
				ConnTypesw[i]="<%write(com_G3Access);%>";
			}
			ConnStatusw[i] = (ConnStatussw[i]=="1"?"<%write(com_Connected);%>":"<%write(com_Disconnected);%>");
			if(platform != "nv1800N")
			{
				if (is3GPort(i,newtotalrecs - 1)) {
					PortNamesw[i] = "3G";
					Macs[i] = "";
				} else if (isAPCliPort(i, newtotalrecs - 1)) {
					PortNamesw[i] = "APClient";
				} else {
					PortNamesw[i] = "WAN"+ i;
				}
				
				if(i != 0 && ConnTypesw[i] == ""){
					IPsw[i]=Masksw[i]=GateWaysw[i]=Macs[i]=MainDns[i]=SenDns[i]=ConnOTimes[i]=ConnStatusw[i]="";
					ConnTypesw[i]="<font color=red><%write(com_unConfigured);%></font>";
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
listheadcaption[0]=_("<%write(RunStateList);%>");
var titles1=new Array();
var index = 0;
titles1[index++]=_("<%write(com_Interface);%>");
titles1[index++]=_("<%write(com_ConnectionType);%>");
titles1[index++]=_("<%write(com_ConnectionStatus);%>");
titles1[index++]=_("<%write(com_IP);%>");
titles1[index++]=_("<%write(com_NetMask);%>");
titles1[index++]=_("<%write(com_GateWayAddr);%>");
titles1[index++]=_("<%write(com_MAC);%>");
if(platform != "nv1800N")
{
	titles1[index++]=_("<%write(com_MDNS);%>");
	titles1[index++]=_("<%write(com_SDNS);%>");
}
titles1[index++]=_("<%write(ConnectionTime);%>");
if(platform != "nv1800N")
{
	arrayData1=new Array(PortNamesw,ConnTypesw,ConnStatusw,IPsw,Masksw,GateWaysw,Macs,MainDns,SenDns,ConnOTimes);
	sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1);
	sortType[0]=new Array(TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_DATE);
	arrayCount[0]=10;
}
else
{
	arrayData1=new Array(PortNamesw,ConnTypesw,ConnStatusw,IPsw,Masksw,GateWaysw,Macs,ConnOTimes);
	sortable[0]=new Array(0,0,0,0,0,0,0,0);
}

function dsplist(index,list)
{
	if(index==0) dspList(index,list,850,titles1,titles1.length,arrayData1,0,0);
}



/////////////////////////////////////end///////////////////////
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
  <td><script language="javascript">showSecondTitle(_("<%write(com_runStatus);%>"));</script>
  <table width="100%" border="0" height="650" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td>	
	<p>&nbsp;</p>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
  <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  
  <tr><td>
<table border="0"  align='center' width="80%">
<tr><td height="20">&nbsp;</td></tr>
<tr><td>
<form name="formRunStatus" method="post" action="">

   <table id="grouplist"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" scrolling="no" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	 id="framelist"  align="MIDDLE" width="600" height="160" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6">&nbsp;</td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="40" valign="top" align="right" ><input type="button" value="<%write(com_fresh);%>" class="input-button" onClick="location.reload()" id="refresh"></td>
			
			  </tr>
		      </table></td>
		    </tr>
		  </table>
          
  </form></td></tr>
</table></td></tr>
</table> 
  </td> 
</tr><script language="javascript">
init();
</script>
</table>
</td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

