
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
var parentPage=Form("parentPage") ;//来自哪个页面
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
aspOutApUserInfo();
%>

var macStr="<%write(macStr);%>";//远程mac字串
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
*/
var max_totalrecs = "128";
var errorstr="";

recsPerPage[0]=10;
lineButton[0]=10;
maxrecs[0]=max_totalrecs;
currpage[0]=1;
recscount[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="无线用户列表";
var titles1=new Array();
titles1[0]="IP地址";
titles1[1]="MAC地址";
titles1[2]="接入SSID";
titles1[3]="频段";
titles1[4]="信号";
titles1[5]="速率";
arrayData1=new Array(iparrays, macarrays, linkedSSIDs, wlanFre, signals, rates);
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
						new Array("<%write(OverView);%>",'system_summary.asp?macStr='+macStr,1,0),
						new Array("<%write(RF);%>",'ap_rf.asp?macStr='+macStr,1,0),
						new Array("<%write(RF5);%>",'ap_rf_5G.asp?macStr='+macStr,1,0),
						new Array("<%write(User);%>",'ap_information.asp',1,1)
		);
    }
    else
    {
	arrayThreeMenu=new Array(
						"90%",
						3,
						new Array("<%write(OverView);%>",'system_summary.asp?macStr='+macStr,1,0),
						new Array("<%write(RF);%>",'ap_rf.asp?macStr='+macStr,1,0),
						new Array("<%write(User);%>",'ap_information.asp',1,1)
		);
    }
    showThreeMenu();
}


//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0"><!--最外层1-->
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">//showSecondTitle("");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg"><!--最外面第2层-->
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
<p>&nbsp;</p>
	<script language="javascript">
		menu();
	</script>
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center"><!--中间白的部分表3-->
<tr>
    <td align="center"  class="three_td" valign="top">
<!--<table border="0"  align='center' width="80%">--><!--下面部分表单外面的表格5-->
<form name="Drawing_List" method="post" action="/goform/formApDrawingDel">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0"><!--列表表单里面的表格666666-->
		<tr><td colspan="4" height="40">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="600" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1"><!--全选那一栏表格7-->
	
		      </table></td><!--全选那一栏表格结束7-->
		    </tr>
		  </table><!--信息列表表单里面表格结束6-->
  </form>
<!--</table>--><!--信息列表表单外面的表格结束5-->
</td></tr>
</table><!--3白的部分结束-->

  </td></tr><!--<script language="javascript">init();</script>-->
</table><!--倒数第二层2--></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table><!--最外层1-->
</body>
</html>
