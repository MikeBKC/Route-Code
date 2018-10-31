<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("TagVlan");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
<% 
	aspOutPutTagVlanList();
%>
//TAG VLAN
var members = new Array();
for (var i = 0; i < totalrecs; i++) {
	members[i] = "";
	for (var j = 0; j < 4; j++) {
		if ((mbrmsk[i] & (1 << j)) != 0) {
			members[i] += "Port" + (j + 1) + " ";
		}
	}
}
//TAG VLAN

var titles1 = new Array();
titles1[0] = "<%write(vlanName);%>";
titles1[1] = "<%write(membership);%>";
titles1[2] = "<%write(vlanId);%>";
if (DhcpPoolSupport == "yes") {
titles1[3] = "<%write(gatewayIpAddr);%>";
titles1[4] = "<%write(gatewayIpMask);%>";
titles1[5] = "<%write(dhcpServer);%>";
}

currpage[0] = 1;	//当前页
recscount[0] = totalrecs;	//当前条目数
maxrecs[0] = 10;	//最大条目数
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "<%write(vlanList);%>";
arrayCount[0] = 5;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
if (DhcpPoolSupport == "yes") {
sortable[0] = new Array(1,  1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_NUM, TYPE_IP, TYPE_IP, TYPE_STR);
var arrayData1 = new Array(vlanName, members, vlanId, ipAddr, netMask, dhcp);
} else {
sortable[0] = new Array(1,  1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_NUM);
var arrayData1 = new Array(vlanName, members, vlanId);
}


function dsplist(index, list) {
	if (DhcpPoolSupport == "yes") {
    	if (index == 0) dspList(index, list, 600, titles1, titles1.length, arrayData1, 1, 1);
	} else {
		if (index == 0) dspList(index, list, 600, titles1, titles1.length, arrayData1, 1, 1);
	}
}


function init() {
	var j;
	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
	frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	for (j = 0; j < totalrecs; j++)
	{
		if (dhcp[j] == "1")
		{
			dhcp[j] = "<%write(enable);%>";
		}
		else
		{
			dhcp[j] = "<%write(disable);%>";
		}
	}
}

function EditConfig(val,i){
	if(val==0)	//添加
	{ 
		vlanList.oldName.value="";
		if(eval(maxrecs[0]-totalrecs)<1)
		{
			alert(_("<%write(warning7);%>"));
			return;
		}
	}
	else	//修改
	{
		vlanList.oldName.value=vlanName[i];
	}
	vlanList.action="TagVlan_edit.asp";
	vlanList.submit();
}

function DeleteConfig(index){
	if(confirm(_("<%write(warning3);%>"))){
		vlanList.delstr.value=vlanName[index];
		vlanList.action="/goform/formTagVlanListDel";
	    vlanList.submit();
	}
}

function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		vlanList.action="/goform/formTagVlanListDelAll";
		vlanList.submit();
	}
}

function customDel()
{
	genData(vlanList, 0, frame12, vlanName, _("<%write(tagVlan);%>"));
	vlanList.action="/goform/formTagVlanListDel";
	vlanList.submit();
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle("<%write(tagVlanList);%>");//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
	
		var arrayThreeMenu=new Array(
						"100%",
						4,
						new Array("<%write(tagVlanList);%>",'TagVlanList.asp',1,1),
						new Array("<%write(tagVlanSetting);%>",'TagVlan_edit.asp',1,0),
						new Array("<%write(pvidSetting);%>",'vid.asp',1,0),
						new Array("<%write(vlanRouting);%>",'vlan_cc.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="vlanList" method="POST">
<input type="hidden" name="delstr" value="">
<input type="hidden" value="" name="oldName">
<table  border="0" width="600" align="center" cellpadding="0" cellspacing="0">
<tr>
<td colspan="6" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="6" align="center" valign="top">
<iframe name="frame12" scrolling="no"  valign="top" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
<td width="25" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="delChk"></td>
<td align="left" width="46%" id="all"><%write(allSelect);%></td>
<td width="50%"  align="right">	   
<input type="button" class="input-button" onClick="EditConfig(0)" id="addEntry" value="<%write(addNew);%>">
<input type="button" class="input-button" onClick="delAll()" value="<%write(delAll);%>" id="delAllEntry">
<input type="button" class="input-button" onClick="customDel()" id="delEntry" value="<%write(del);%>">
</td>
</tr></table>
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
