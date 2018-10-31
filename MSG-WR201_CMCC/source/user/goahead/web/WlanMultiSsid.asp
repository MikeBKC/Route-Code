
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("wireless");

function initTranslation()
{
	var e;
	e = document.getElementById("add");
	e.value= _("add");
	e = document.getElementById("delall");
	e.value= _("delall");
	e= document.getElementById("del");
	e.value= _("del");

}

var strSecondMenuFocus="多SSID";
<% aspOutputWlanMultiSsid();%>
//交互变量
/*
var WrlessEnables=new Array();
var ssids=new Array();
var wrlesModes=new Array();
var broadcastEnables=new Array();
var vlanids=new Array();
var secModes=new Array();
WrlessEnables[0]="on";
wrlesModes[0]="11B";
broadcastEnables[0]="on";
vlanids[0]="78";
secModes[0]="WEP";
var errorstr="";
////end
*/

var WrlessEnablesw=new Array();var broadcastEnablesw=new Array();var secModesw=new Array();
for(i=0;i<WrlessEnables.length;i++){
	WrlessEnablesw[i]="启用";
	broadcastEnablesw[i]="启用";
	if(WrlessEnables[i]!="on") WrlessEnablesw[i]="禁用";
	if(broadcastEnables[i]!="on") broadcastEnablesw[i]="禁用";
	switch(secModes[i]){
		case "NONE":secModesw[i]="无安全机制";break;
		case "WEP":secModesw[i]="WEP";break;
		case "WPA":secModesw[i]="WPA/WPA2";break;
		case "WPAPSK":secModesw[i]="WPA-PSK/WPA2-PSK";break;
		default: break;
	}
}
recsPerPage[0] =4;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=4;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="多SSID配置信息列表";
var titles1=new Array(4);
titles1[0]="SSID";
titles1[1]="无线功能";
titles1[2]="SSID广播";
//titles1[3]="VLAN ID";
titles1[3]="安全机制";


arrayData1=new Array(ssids,WrlessEnablesw,broadcastEnablesw,secModesw,secModes,WrlessEnables,broadcastEnables);
sortable[0]=new Array(1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_CHINESE,TYPE_CHINESE,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=7;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,4,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0) WlanMultSSID_List.id.value="";
	else WlanMultSSID_List.id.value=ssids[i];
	WlanMultSSID_List.action="WlanMultiSsid_edit.asp";
	WlanMultSSID_List.submit();
}
function DeleteConfig(index){
	if(confirm("确定删除？")){
		WlanMultSSID_List.delstr.value=ssids[index];
		WlanMultSSID_List.action="/goform/ConfigWlanMultiSsidDel";
	    WlanMultSSID_List.submit();
		
	}
}
function delAll(){
	if(confirm("确定删除所有的条目？")){
		WlanMultSSID_List.action="/goform/ConfigWlanMultiSsidDelAll";
		WlanMultSSID_List.submit();
	}
}
-->
</script>
</head>
<body onLoad="initTranslation()"onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("wireless advance"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form name="WlanMultSSID_List" method="post" action="/goform/ConfigWlanMultiSsidDel">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						new Array('高级参数','WlanAdvanced.asp',1,0),
						new Array('多SSID','WlanMultiSsid.asp',1,1)
		);
		showThreeMenu();
	</script><!--<form name="WlanMultSSID_List" method="post" action="/goform/ConfigWlanMultiSsidDel">-->
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="250">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="115" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("selecte"));</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="90" valign="top" align="right"><input type="button" value="添加新条目" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="110" valign="top" align="right"><input type="button" value="删除所有条目" class="input-button" onClick="delAll()" id="delall"></td>
			  <td width="40" valign="top" align="right"><input type="submit" value="删除" class="input-button" onClick="return(genData(WlanMultSSID_List,0,frame12,ssids,'多SSID配置信息'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr>
		  </table></td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
<!--
<body>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td height="50"></td></tr>
  <tr>
    <td><form name="WlanMultSSID_List" method="post" action="/goform">
   <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12"	   align="MIDDLE" width="600" height="115" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.chklist);>
		全选 / 全不选</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ></td> 
			  <td width="90" valign="top" align="right"><input type="button" value="添加新条目" class="input-button" onClick="EditConfig(0)"></td>
			  <td width="110" valign="top" align="right"><input type="button" value="删除所有条目" class="input-button" onClick="delAll()"></td>
			  <td width="40" valign="top" align="right"><input type="submit" value="删除" class="input-button" onClick="return(genData(WlanMacFilter_List,0,frame12,filterMacs,'MAC地址过滤信息'))"></td>
			  </tr>
		      </table></td>
		    </tr>
		  </table>
  </form></td>
  </tr>
</table>

</body>-->
</html>
