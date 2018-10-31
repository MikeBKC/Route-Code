 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("DoubleRoad");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

var strSecondMenuFocus="";

//function initTranslation()
//{
//	var e;
	
	/*e = document.getElementById("method");
	e.innerHTML= _("method");
	e = document.getElementById("static");
	e.innerHTML= _("static");
	e = document.getElementById("dhcp");
	e.innerHTML= _("dhcp");
	e= document.getElementById("uttsave");
	e.value= _("save");
	e = document.getElementById("uttreset");
	e.value= _("reset");
	e = document.getElementById("save2");
	e.value= _("save");
	e = document.getElementById("reset2");
	e.value= _("reset");
	e = document.getElementById("update");
	e.value= _("update");*/
//}

<%aspMultiPathGlobale(); %>

function init(){
	var f=DoubleRoadConfig;
	f.DualType[0].checked=(DualTypes=="backup"?true:false);
	f.DualType[1].checked=(DualTypes=="backup"?false:true);
	initisppref();
	adjustDualRouteType();
}
 
function save(){
		var f=DoubleRoadConfig;
		addListToText(f.main,f.MainRoad);
		addListToText(f.branch,f.BranchRoad);
		DoubleRoadConfig.action="/goform/formMultiPathGlobale";
		DoubleRoadConfig.submit();
}

function Reset(){
	DoubleRoadConfig.reset();
	init();
}
function adjustDualRouteType()
{
	if (DoubleRoadConfig.DualType[0].checked) {
	try
	{
		backtd.style.display = "table-row";
	}
	catch(e)
	{
	backtd.style.display = "block";
	}
	}
	else {
		backtd.style.display = "none";
	}
}
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
function initisppref()
{
	var i;
	var isBranchEmpty=1;
	//初始化列表前应该清空
	document.DoubleRoadConfig.main.options.length   =   0 ;
	document.DoubleRoadConfig.branch.options.length   =   0 ;

	for(i=0;i<mainRoadArr.length;i++) {
		var option = document.createElement("OPTION");
		var slotIndex = mainRoadArr[i].slice(mainRoadArr[i].length - 1);
		if (is3GPort(slotIndex,MAX_LEN)) {
			option.text = "3G";
		} else if (isAPCliPort(slotIndex, MAX_LEN)) {
			option.text = "APClient";
		} else {	
			option.text = mainRoadArr[i];
		}	
		option.value = mainRoadArr[i];
			DoubleRoadConfig.main.options.add(option);
	}
	for(i=0;i<backupRoadArr.length;i++) {
		var option = document.createElement("OPTION");	
		var slotIndex = backupRoadArr[i].slice(backupRoadArr[i].length - 1);
		if (is3GPort(slotIndex,MAX_LEN)) {
			option.text = "3G";
		} else if (isAPCliPort(slotIndex, MAX_LEN)) {
			option.text = "APClient";
		} else {		
			option.text = backupRoadArr[i];
		}
		option.value = backupRoadArr[i];
			DoubleRoadConfig.branch.options.add(option);
			isBranchEmpty=0;
	}
		
		if(isBranchEmpty == 1) DoubleRoadConfig.DualType[1].checked=true;
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
function getIndexOfList(name0)
{
	var i;
	for(i=0;i<recscount[0];i++)
	{
		if(name0 == ids[i]) return i;
	}
	return -1;
}
//-->
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
  <td><script language="javascript">showSecondTitle("<%write(com_routeCombination);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						4,
						new Array("<%write(com_globalConfig);%>",'DoubleRoad.asp',1,1),
						new Array("<%write(com_routeCombinationStatus);%>",'DoubleRoadInfo.asp',1,0),
						new Array("<% write(com_lineCombinationCheck); %>",'DoubleRoadInfo_edit.asp',1,0),
						new Array("<% write(com_IdentifyBind); %>",'IdentifyBind.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	
	<table border="0" width="95%" align='center'>
<form  name="DoubleRoadConfig" method="post">
 <tr><td colspan="4" align="center" height="25" valign="top" >&nbsp;
</td></tr>
	  <!-- <tr>
	   <td width="26%"  height="20" align="right" id="method">身份绑定</td>
	   <td width="4%">&nbsp;</td>
	   <td width="70%"><input name="IDBinding" type="checkbox" id="IDBinding" ></td>
	   </tr>-->
	    <tr><td colspan="4">
			   <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" >
	   <td width="26%"  height="50" align="right" id="method"><%write(combinationMode);%></td>
	   <td width="4%">&nbsp;</td>
	   <td width="70%"><input name="DualType" type="radio" value="backup" checked onClick=adjustDualRouteType()><%write(combinationContent1);%><br>
	   <input type="radio" name="DualType" value="balance" onClick=adjustDualRouteType()><%write(combinationContent2);%></td>
	   </table></td></tr >
	   <tr style="display:none" id="backtd" width="100%"  ><td colspan="4" valign="bottom"  align="center" >
	   <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center" >
  <tr  align="center">
  	 
    <td width="41%" align="center" height="27"><strong><%write(Mroute);%></strong></td>
    <td width="16%">&nbsp;</td>
    <td width="43%"  align="center" height="27"><strong><%write(Sroute);%></strong></td>
   
  </tr>
  <tr align="center">
    <td rowspan="2" align="center"><select class="input" size=5 name="main"	 multiple  id="main"  style="border:0px;width:200px" border=0 ondblclick="javascript:addIPToList(DoubleRoadConfig.main, DoubleRoadConfig.branch)">
    </select></td>
    <td height="30" align="center"><input type="button"  value=" ==> " onClick="addIPToList(DoubleRoadConfig.main, DoubleRoadConfig.branch)" style="COLOR: #3F3B3A; HEIGHT: 22px; FONT-SIZE: 12px; padding-top:1 "></td>
    <td rowspan="2" align="center"><select class="input" size=5	 name="branch"	multiple  id="branch" ondblclick="javascript:addIPToList(DoubleRoadConfig.branch, DoubleRoadConfig.main)" style="border:0px;width:200px" border=0 >
				</select></td>
    </tr>
  <tr align="center">
    <td height="30" align="center"><input type="button" style="COLOR: #3F3B3A; HEIGHT: 22px; FONT-SIZE: 12px; padding-top:1 " value=" <== " onClick="addIPToList(DoubleRoadConfig.branch, DoubleRoadConfig.main)"></td>
    </tr><input name="MainRoad" type="hidden"   id="MainRoad"  />
	<input name="BranchRoad" type="hidden"   id="BranchRoad" />
</table>

</td></tr>
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	    <tr><td  colspan="4">
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#DoubleLine')" id="utthelp"></td>
  </tr>
</table>
  </td></tr>
  </form>
  </table>
</td>
  </tr>
</table> 
  </td></tr><script language="javascript">init();</script>
</table></td></tr></table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table></td></tr></table>
</body>
</html>
