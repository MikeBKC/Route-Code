
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>

<%langTran("ap_sys_info");%>

<title><%write(title);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%
var parentPage=Form("parentPage") ;//�����ĸ�ҳ��
var ipStr=Form("ipStr");//Զ��ip�ִ�
var macStr=Form("macStr");//Զ��mac�ִ�
aspOutApSysInfo();
%>

var macStr="<%write(macStr);%>";//Զ��mac�ִ�

/*
var ips = new Array();
var macs = new Array();
var ssids = new Array();
var signals = new Array();
var rates = new Array();
ips[0]="192.168.1.1";
macs[0]="000000000000";
signals[0]="-70dbm";
rates[0]="54M";
var totalrecs = "1";
var max_totalrecs = "128";
var errorstr="";
*/

recsPerPage[0]=10;
lineButton[0]=10;
//maxrecs[0]=max_totalrecs;
currpage[0]=1;
//recscount[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="�����û��б�";
var titles1=new Array();
titles1[0]="IP��ַ";
titles1[1]="MAC��ַ";
titles1[2]="����SSID"; 
titles1[3]="�ź�";
titles1[4]="����";
//arrayData1=new Array(ips,macs,ssids,signals,rates);
sortable[0]=new Array(1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=5;


function dsplist(index,list)
{
	if(index == 0) dspList(index,list,600,titles1,titles1.length,arrayData1,0,0);
}

var arrayThreeMenu=new Array();
function menu()
{
    if (flag_5G != "0")
    {
	arrayThreeMenu=new Array(
		"90%",
		4,
		new Array("<%write(OverView);%>",'system_summary.asp',1,1),
		new Array("<%write(RF);%>",'ap_rf.asp?macStr='+macStr,1,0),
		new Array("<%write(RF5);%>",'ap_rf_5G.asp?macStr='+macStr,1,0),
		new Array("<%write(User);%>",'ap_information.asp?macStr='+macStr,1,0)
		);
    }
    else
    {
	arrayThreeMenu=new Array(
		"90%",
		3,
		new Array("<%write(OverView);%>",'system_summary.asp',1,1),
		new Array("<%write(RF);%>",'ap_rf.asp?macStr='+macStr,1,0),
		new Array("<%write(User);%>",'ap_information.asp?macStr='+macStr,1,0)
		);
    }
    showThreeMenu();
}

//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0"><!--�����1-->
  <!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****�м�����****-->
  <tr>
  <td width="15"></td>
  <!--////����-->
  <td><script language="javascript">//showSecondTitle("");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg"><!--�������2��-->
   
  <tr valign="top">
    <td>
<p>&nbsp;</p>
	<script language="javascript">
	    menu();
	</script>
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center"><!--�м�׵Ĳ��ֱ�3-->
<tr>
    <td align="center"  class="three_td" valign="top">
<!--<table border="0"  align='center' width="80%">--><!--���沿�ֱ�����ı��5-->
<form name="Drawing_List" method="post" action="/goform/formApDrawingDel">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0"><!--�б������ı��666666-->
		<tr><td colspan="4" height="40">
			<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
			<tr >
			<td width="467" align="right" valign="bottom">&nbsp;</td>
			</tr>
			<tr><td colspan="2" height="10"></td></tr>
			</TABLE>
		</td></tr>   
		<tr>
		      <td colspan="4">
			<TABLE  width="600"  border="0"  cellpadding="4" cellspacing="1" class="style22"  align="center">
<tr class="style18">
  <td class=vncell height="22" id="sysSerial"><%write(SysSerial);%></td>
  <td colspan="4" class=vtable><script language="javascript">document.write(sysSerial);</script></td>
</tr>
<tr >
  <td class=vncell height="22" width="22%" id="devType"><%write(DevType);%></td>
  <td colspan="4" class=vtable><script language="javascript">document.write(devType);</script></td>
</tr>

<tr class="style18">
  <td class=vncell height="22" id="deviceName"><%write(DeviceName);%></td>
  <td colspan="4" class=vtable><script language="javascript">document.write(deviceName);</script></td>
</tr>

<tr class="style18" style="display:" id="macStr" height="22">
<td class=vncell height="22" width="22%" id="macStr"><%write(MacStr);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(macStr);</script></td>
</tr>
<tr class="style18" style="display:" id="ipStr" height="22">
<td class=vncell height="22" width="22%" id="ipStr"><%write(IpStr);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(ipStr);</script></td>
</tr>
<tr class="style18" style="display:" id="dhcp_en" height="22">
  <td class=vncell height="22" id="gateway_td"><%write(Dhcp_en);%></td>
  <td colspan="4" class=vtable><script language="javascript">document.write(dhcp_en);</script></td>
</tr>
<tr>
  <td class=vncell height="22" width="22%" id="sysHwVersion"><%write(SysHwVersion);%></td>
  <td colspan="4" class=vtable><script language="javascript">document.write(sysHwVersion);</script></td>
</tr>
<tr class="style18">
  <td class=vncell height="22" id="sysSwVersion"><%write(SysSwVersion);%></td>
  <td colspan="4" class=vtable><script language="javascript">document.write(sysSwVersion);</script></td>
</tr>
<tr>
<td class=vncell height="22" width="22%" id="cpuRate"><%write(CpuRate);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(cpuRate);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%" id="memRate"><%write(MemRate);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(memRate);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%" id="clients"><%write(Clients);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(clients);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%" id="mfNames"><%write(MfNames);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(mfNames);</script></td>
</tr>
<tr class="style18">
<td class=vncell height="22" width="22%" id="upTime"><%write(UpTime);%></td>
<td colspan="4" class=vtable><script language="javascript">document.write(upTime);</script></td>
</tr>

</table>
			</td>
		    </tr>
		   
		 
		  </table><!--��Ϣ�б�����������6-->
  </form>
<!--</table>--><!--��Ϣ�б������ı�����5-->
</td></tr>
</table><!--3�׵Ĳ��ֽ���-->

  </td></tr><!--<script language="javascript">init();</script>-->
</table><!--�����ڶ���2--></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table><!--�����1-->
</body>
</html>
