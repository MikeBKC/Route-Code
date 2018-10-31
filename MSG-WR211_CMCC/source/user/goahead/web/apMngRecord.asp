<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("ac_ap");

//交互begin
var namearray = new Array('AP1',  'AP3');	//ap名称
var types = new Array("510W", "WA1800N");	//ap型号
var serialno = new Array('0018432a', '00bc615a');	//序列号
//var iparray = new Array('192.200.2.82', '192.168.1.154');	//IP地址
var macarray = new Array('0022aa5e8f1a', '0022aa6581ca');	//MAC地址
var configuration = new Array("temp_1", "configWA1800N_00-22-AA-AF-7A-02_20111108_214400.txt");	//配置模版名或配置文件名称
var software = new Array("123.bin", "321.bin");	//软件名称
var zones = new Array("zone_1", "zone_3");	//所在服务区
var totalrecs = 2
var errorstr = "";
//交互end

<% 
	
%>
function initTranslation()
{
	var e;
}
var titles1 = new Array();
titles1[0] = _("APName");
titles1[1] = _("APType");
titles1[2] = _("serialNum");
titles1[3] = "MAC";
titles1[4] = _("config");
titles1[5] = _("soft");
titles1[6] = _("serviceZone");

currpage[0] = 1;
recscount[0] = totalrecs;
maxrecs[0] = 24;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("APMngRcd");
sortable[0] = new Array(1, 1, 1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_CHINESE, TYPE_STR, TYPE_NUM, TYPE_STR, TYPE_NUM, TYPE_STR, TYPE_STR);
arrayCount[0] = 7;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 0;
var arrayData1 = new Array(namearray, types, serialno, macarray, configuration, software, zones);
function dsplist(index, list) {
    if (index == 0) dspList(index, list, 750, titles1, titles1.length, arrayData1, 0, 0);
}

function init() {
	initTranslation();
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("APAdmin"));//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						4,
						new Array(_("APList"),'apManage.asp',1,0),
						new Array(_("APPower"),'apPermission.asp',1,0),
						new Array(_("APScan"),'apDiscover.asp',1,0),
						new Array(_("APMngRcd"),'apMngRecord.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form name="APList" method="POST" action="">
<br>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="800" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="750"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	

</td></tr>

<tr>
<td align="center" valign="top">
<iframe scrolling="auto" name="frame12" id="framelist" valign="top" align="center" width="750" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
</table>
</td>
</tr>
</table>
</form>
<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
