<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("TagVlan");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">
<% aspOutputVlanRoute(); %>
Butterlate.setTextDomain("admin");

function addIPToList(srcList, dstList) {
	var strArray = null;
	var arrayLen = 0;

	for (var i=0;i<dstList.options.length;i++) {  //计算出该列表框目前最大字节长度
		arrayLen += dstList.options[i].text.lstrLen();
	}

	for (var i=0;i<srcList.options.length;i++) {
		if (srcList.options[i].selected) {
			/*if(srcList.options[i].text == "WAN1") { //Web 10 修改，select控件中显示的是WAN1线路
				alert("WAN1线路只能属于主线路！");
				return;
			}*/
			arrayLen += srcList.options[i].text.lstrLen();
		

			var option = document.createElement("OPTION");
			option.text = srcList.options[i].text;
			option.value = srcList.options[i].value;
			dstList.options.add(option);
		}
	}
	delIPFromList(srcList, true);
}

function addListToText(srcList,roadTxt)
{
	var index;
	roadTxt.value="";
	for (var i=0;i<srcList.options.length;i++) { 
		roadTxt.value=roadTxt.value+srcList.options[i].value+",";
	}
	var len=roadTxt.value.length-1;
	roadTxt.value=roadTxt.value.substring(0,len);
}

function save(){
		var f=vlan_cc;
		addListToText(f.deny,f.hidRouteDis);
		addListToText(f.allow,f.hidRouteEn);
		vlan_cc.action="/goform/formVlanRouteConfig";
		vlan_cc.submit();
}

function init() {
	var i;
	var isBranchEmpty=1;
	//初始化列表前应该清空
	document.vlan_cc.deny.options.length   =   0 ;
	document.vlan_cc.allow.options.length   =   0 ;

	for(i=0;i<routeDisArr.length;i++) {
		var option = document.createElement("OPTION");
		var slotIndex = routeDisArr[i].slice(routeDisArr[i].length - 1);
		option.text = routeDisArr[i];
		option.value = routeDisArr[i];
		vlan_cc.deny.options.add(option);
	}
	for(i=0;i<routeEnArr.length;i++) {
		var option = document.createElement("OPTION");	
		var slotIndex = routeEnArr[i].slice(routeEnArr[i].length - 1);
		option.text = routeEnArr[i];
		option.value = routeEnArr[i];
		vlan_cc.allow.options.add(option);
	}
}

function initTranslation()
{

}

</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.gif">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(vlanRouting);%>");</script>
  <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="vlan_cc" method="post" enctype="multipart/form-data">
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						4,
						new Array("<%write(tagVlanList);%>",'TagVlanList.asp',1,0),
						new Array("<%write(tagVlanSetting);%>",'TagVlan_edit.asp',1,0),
						new Array("<%write(pvidSetting);%>",'vid.asp',1,0),
						new Array("<%write(vlanRouting);%>",'vlan_cc.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->		
	
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30"><input type="hidden" name="hidRouteEn" id="hidRouteEn" /><input type="hidden" name="hidRouteDis" id="hidRouteDis" /></td></tr>
  <tr>
    <td colspan="3" height="30">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" >
  <tr  align="center">
  	 
    <td width="41%" align="center" height="27"><strong><%write(routeVlan);%></strong></td>
    <td width="16%">&nbsp;</td>
    <td width="43%"  align="center" height="27"><strong><%write(noRouteVlan);%></strong></td>
   
  </tr>
  <tr align="center">
    <td rowspan="2" align="center"><select class="input" size=5 name="deny"	 multiple  id="deny"  style="border:0px;width:200px" border=0 ondblclick="javascript:addIPToList(vlan_cc.deny, vlan_cc.allow)">
    </select></td>
    <td height="30" align="center"><input type="button"  value=" ==> " onClick="addIPToList(vlan_cc.deny, vlan_cc.allow)" style="COLOR: #3F3B3A; HEIGHT: 22px; FONT-SIZE: 12px; padding-top:1 "></td>
    <td rowspan="2" align="center"><select class="input" size=5	 name="allow"	multiple  id="allow" ondblclick="javascript:addIPToList(vlan_cc.allow, vlan_cc.deny)" style="border:0px;width:200px" border=0 >
				</select></td>
    </tr>
  <tr align="center">
    <td height="30" align="center"><input type="button" style="COLOR: #3F3B3A; HEIGHT: 22px; FONT-SIZE: 12px; padding-top:1 " value=" <== " onClick="addIPToList(vlan_cc.branch, vlan_cc.main)"></td>
    </tr>
</table>	
	</td>
  </tr>
  <tr><td colspan="3" height="30">
<table align="center"><tr><td height="5"></td></tr><tr><td>
<input type="button" onClick="save()"class="input-button" value="<%write(com_save);%>" name="" id="uttsave">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(com_reset);%>" name="" id="uttreset" onClick="Reset();">
<input type="button" class="input-button" value="<%write(com_help);%>" name="" id="utthelp" onClick="window.open('<%write(com_helppage);%>#serviceZone')">
</td></tr></table>  
  </td></tr>
  <input type="hidden" value="1" name="re">
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
