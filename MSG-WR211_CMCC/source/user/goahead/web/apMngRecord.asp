<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>��̩�Ƽ�</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("ac_ap");

//����begin
var namearray = new Array('AP1',  'AP3');	//ap����
var types = new Array("510W", "WA1800N");	//ap�ͺ�
var serialno = new Array('0018432a', '00bc615a');	//���к�
//var iparray = new Array('192.200.2.82', '192.168.1.154');	//IP��ַ
var macarray = new Array('0022aa5e8f1a', '0022aa6581ca');	//MAC��ַ
var configuration = new Array("temp_1", "configWA1800N_00-22-AA-AF-7A-02_20111108_214400.txt");	//����ģ�����������ļ�����
var software = new Array("123.bin", "321.bin");	//�������
var zones = new Array("zone_1", "zone_3");	//���ڷ�����
var totalrecs = 2
var errorstr = "";
//����end

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
arrayCount[0] = 7;//�ܹ�������(����ָ���Ǹ�������仯����)
recsPerPage[0] = 10; //ÿҳ����������
lineButton[0] = 0;
var arrayData1 = new Array(namearray, types, serialno, macarray, configuration, software, zones);
function dsplist(index, list) {
    if (index == 0) dspList(index, list, 750, titles1, titles1.length, arrayData1, 0, 0);
}

function init() {
	initTranslation();
  	frame11.location.href = "_listhead1.htm"; //ע������ڴ˴���ʼ��������ҳ��ˢ�µ�ʱ����ܳ���bug
    frame12.location.href = "_list1.htm"; //���������ˢ�� ҳ����������Ƿ�����
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****���ϼ��****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****�м�����****-->
  <tr>
   <td width="15"></td>
  <!--////����-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("APAdmin"));//��ʾҳ������</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
   <script language="javascript">showerror(errorstr,100);//��ʾ���淵�صĴ�����Ϣ</script>
  <tr valign="top">
    <td>
	<!--//�����˵�-->
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
	<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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
<!--//ֱ�ӽ�ҳ���form���ڴ˴������磺<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
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
