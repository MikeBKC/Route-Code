<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<% langTran("common"); %>
<%langTran("commonWords");%>
<%langTran("wlanCltList");%>

<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var lang="zhcn";

//-----------交互开始------------
<%aspOutPutWlanCltList();%>
//var wlanFre = new Array();
if (onoff_flag == 0)
{
var macSSID = new Array();
for (i=0;i<totalrecs;i++){
	macSSID[i] = macarrays[i]+":"+linkedSSIDs[i];
	}
}
var strSecondMenuFocus = "wlanApClients";

currpage[0] = 1;	//当前页
recscount[0] = totalrecs;	//当前条目数
maxrecs[0] =max_totalrecs;	//最大条目数
listname[0] = "frame12";
listheadname[0] = "frame11";

var titles1 = new Array();
var titles1_index = 0;
if (onoff_flag == 0)
{
titles1[titles1_index++] ="MAC地址";
titles1[titles1_index++] = "接入AP";
titles1[titles1_index++] = "接入SSID";
titles1[titles1_index++] = "频段";
titles1[titles1_index++] = "信号";
titles1[titles1_index++] = "速率";
titles1[titles1_index++] = "频宽";
titles1[titles1_index++] = "下载";
titles1[titles1_index++] = "下载速率(bit/s)";
titles1[titles1_index++] = "上传";
titles1[titles1_index++] = "上传速率(bit/s)";
titles1[titles1_index++] = "在线时长";

listheadcaption[0] ="在线用户";
//sortable[0] = new Array(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1);
//sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
sortable[0] = new Array(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0] = 12;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 1020;
//var arrayData1 = new Array(users,iparrays,macarrays,linkedaps,linkedSSIDs,wlanFre,signals,rates,bindwidths,downloads,downRate,uploads,upRate,time,filterRules);
var arrayData1 = new Array( macarrays,linkedaps,linkedSSIDs,wlanFre,signals,rates,bindwidths,downloads,downRate,uploads,upRate,time,filterRules);
var oldDspList=dspList;
}
else
{
titles1[titles1_index++] ="MAC地址";
titles1[titles1_index++] = "最后一次接入日期";
titles1[titles1_index++] = "接入AP";
titles1[titles1_index++] = "在线时长";
titles1[titles1_index++] = "下载";
titles1[titles1_index++] = "上传";

listheadcaption[0] ="离线用户";
sortable[0] = new Array(1, 1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR,TYPE_STR, TYPE_STR, TYPE_STR);
arrayCount[0] = 6;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 1020;
var arrayData1 = new Array( macarrays,linkedDate, linkedaps,time,downloads,uploads);
var oldDspList=dspList;
}

dspList=function()
{
	var sum = totalrecs;
	oldDspList.apply(this,arguments);
	var tr=frame12.document.getElementById('tableList').rows;
	for (var i = 1;i < tr.length;i++)
	{
		var macStr = trimstr(tr[i].cells[1].innerHTML);
		var rel_i = (i - 1) + ((currpage[0] - 1) * recsPerPage[0]);
//		tr[i].cells[3].innerHTML = "<span title="+"主机名:"+hostName[i -1]+"&#13;&#10;"+"品牌名:"+companyName[i-1]+"><a href='javascript:parent.openStaHistoryPage(" + (i - 1) + ")'><font color='blue'>"+ macStr +"</font></a></span>";
		tr[i].cells[1].innerHTML = "<span title="+"主机名:"+hostName[rel_i]+"&#13;&#10;"+"制造商:"+companyName[rel_i]+"&#13;&#10;"+"用户名:"+users[rel_i]+"&#13;&#10;"+"IP地址:"+iparrays[rel_i]+"><a href='javascript:parent.openStaHistoryPage(" + (rel_i) + ")'><font color='blue'>"+ macStr +"</font></span>";
//		tr[i].cells[3].innerHTML = "<a href='javascript:parent.openAPInfoPage(" + (i - 1) + ")'><font color='blue'>" + macStr + "</font></a>";
	}
}


function dsplist(index, list) {
    if (index == 0) 
	{
		if (onoff_flag == 0)
		{
			dspList(index, list, 1000, titles1, titles1.length, arrayData1, 1, 0);
		}
		else
		{
			dspList(index, list, 700, titles1, titles1.length, arrayData1, 1, 0);
		}
	}
}


function init() {
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
        frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
		
		if (onoff_flag == 0)
		{
			WlanApClientsList.userState[0].checked  = true;
			document.getElementById('menu_1').style.visibility = "hidden";
			document.getElementById('menu_2').style.visibility = "hidden";
			document.getElementById('menu_8').style.display = "none";
			document.getElementById('framelist').height= "265";
			document.getElementById('date_select').value = "0";
		}
		else
		{
			WlanApClientsList.userState[1].checked  = true;
//			moduleDisplay('menu_3', 0);
//			moduleDisplay('menu_4', 0);
//			moduleDisplay('menu_5', 0);
//			moduleDisplay('menu_6', 0);
			document.getElementById('menu_3').style.display = "none";
			document.getElementById('menu_4').style.visibility = "hidden";
			document.getElementById('menu_5').style.visibility = "hidden";
			document.getElementById('menu_6').style.visibility = "hidden";
			document.getElementById('menu_7').style.visibility = "hidden";
			document.getElementById('menu_8').style.display = "";
			document.getElementById('framelist').height= "250";
			document.getElementById('date_select').value = date;
//			document.getElementById('mange_div2').style.display = "none";
//			WlanApClientsList.onlineMenu.style.display = "none";
		}
		//alert(macSSID);
}
for(i=0;i<totalrecs;i++){
	
	clientenables[i]=(clientenables[i]=="1"?"Yes":"No");
	
}

function openStaHistoryPage(index)
{
	staList.staMac.value= macarrays[index];
	staList.HostName.value = hostName[index];
	openForm(staList,"sta_history.asp",850,550);
}

function SaveUserAllow(index,i)
{
 
	if (clientenables[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		if(filterRules[i] == 1)
		{
		str1="将该用户从SSID：";
		str2="包含的允许地址组中移除？";
		}else if(filterRules[i] == 2)
		{
		str1="将该用户添加SSID：";
		str2="包含的禁用地址组？";
		}else{
		alert("该用户不在服务区中，无法使用此功能！");
		dsplist(index,frame12);
		return;
		}
		WlanApClientsList.clientenable.value="1";
	}
	else
	{
		if(filterRules[i] == 1)
		{
		str1="将该用户添加到SSID：";
		str2="包含的允许地址组中？";
		}else if(filterRules[i] == 2)
		{
		str1="将该用户从SSID：";
		str2="包含的禁止地址组中移除？";
		}else{
		alert("该用户不在服务区中，无法使用此功能！");
		dsplist(index,frame12);
		return;
		}
		WlanApClientsList.clientenable.value="0";
	}
//	if (confirm("<%write(com_really);%>"+str+"111111"))
	if (confirm(str1+linkedSSIDs[i]+str2))
	{
		 var tempMAC = macarrays[i];
		 tempMAC = tempMAC.replace(/:/g,'');
		 tempMAC = tempMAC.toLowerCase();
		 WlanApClientsList.clientenableID.value = tempMAC;
		 WlanApClientsList.macRule.value = filterRules[i];
		 WlanApClientsList.filterSsid.value = linkedSSIDs[i];
		 WlanApClientsList.action ="/goform/formApMacFilterChange";
		 WlanApClientsList.submit();
	}

	else
	{
		dsplisthead(index,frame11);
		dsplist(index,frame12);
	}
}


function addToFilter(formName, index, list, array, tips, flagstr) //删除列表中的元素
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.filterstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //显示页面的开始记录数 以0为开始下标
    recend = recbegin + recsPerPage[index]; //显示页面的结束记录数

    if (recend >= recscount[index]) recend = recscount[index]; //如果是最后页　把总计量赋值给recend

    for (var i = recbegin; i < recend; i++) { //因为recbegin比显示的开始记录数小一，所以i< 并非 <= recend

        if (chks.length) { //循环遍历checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks[t].value)] + flagstr;
                //生成隐藏参数
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        }

        t = t + 1;
    }

    if (formName.filterstr.value != ""){
		formName.filterstr.value=formName.filterstr.value.substring(0,formName.filterstr.value.length-1)
		formName.action = "/goform/formAddToBlackList"
		formName.submit();
		}//判断是否选择checkbox
    else {
        alert("请先选择" + tips + "记录。");
    }
}



function disconnect(formName, index, list, array, tips, flagstr) //删除列表中的元素
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.filterstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //显示页面的开始记录数 以0为开始下标
    recend = recbegin + recsPerPage[index]; //显示页面的结束记录数

    if (recend >= recscount[index]) recend = recscount[index]; //如果是最后页　把总计量赋值给recend

    for (var i = recbegin; i < recend; i++) { //因为recbegin比显示的开始记录数小一，所以i< 并非 <= recend

        if (chks.length) { //循环遍历checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks[t].value)] + flagstr;
                //生成隐藏参数
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.filterstr.value = formName.filterstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.filterstr.value = formName.filterstr.value + array[i] + flagstr;
            }
        }

        t = t + 1;
    }

    if (formName.filterstr.value != ""){
		formName.filterstr.value=formName.filterstr.value.substring(0,formName.filterstr.value.length-1)
		formName.action = "/goform/formDisconnect";
		formName.submit();
		}//判断是否选择checkbox
    else {
        alert("请先选择" + tips + "记录。");
    }
}

function menu_check()
{
	/*
	if (WlanApClientsList.userState[0].checked  == true)
	{
		alert("在线");
	}
	else
	{
		alert("离线");
	}
	*/
	
	WlanApClientsList.action = "/goform/formGetOnlineState";
	WlanApClientsList.submit();
}

function delAllLog()
{
	WlanApClientsList.action = "/goform/formDelAllLog";
	WlanApClientsList.submit();
}

function exportData(){
	WlanApClientsList.exportstr.value = "onlineUsers.tar";
	WlanApClientsList.action = "/goform/formExportCltInfo";
	WlanApClientsList.submit();
	
	}

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
  <td><script language="javascript">showSecondTitle("无线统计");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg"><!--最外面第2层-->
   <script language="javascript">showerror(errorstr,80);</script>
  <tr>
    <td>

	<script language="javascript">
		var arrayThreeMenu=new Array(
						"90%",
						6,
						
						new Array("AP用户统计",'WlanApClientsStatistics.asp',1,0),
						new Array("SSID用户统计",'WlanSSIDClientsStatistics.asp',1,0),
						new Array("历史用户统计",'WlanHistoryClientsStatistics.asp',1,0),
						new Array("历史流量统计",'historyFlowStatistics.asp',1,0),
						new Array("用户列表",'wlanCltList.asp',1,1),
						new Array("黑名单",'wlanCltMacFilter.asp',1,0)
						
		);
		showThreeMenu();
	</script>
  <table width="90%" border="0" cellspacing="0" cellpadding="0" align="center"><!--中间白的部分表3-->
<tr>
    <td align="center"  class="three_td" valign="top">
<table border="0" width="100%"  align='center'><tr><td class="gapTd"></td><td align="center"><!--下面部分表单外面的表格5-->

<form name="WlanApClientsList" method="post" action="">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0"><!--列表表单里面的表格666666-->
		<tr><td colspan="4" height="20">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" scrolling="no"  name="frame11"  align="MIDDLE" width="700" height="45" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" scrolling="auto"  name="frame12"	id="framelist" align="MIDDLE" width="700" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
	<input type="hidden" name="clientenableID"><input type="hidden" name="clientenable"><input type="hidden" name="macRule"><input type="hidden" name="filterSsid"></tr>
                    

			<tr><td colspan="4" height="30">
				 <table width="100%" align="center"  cellpadding="0" cellspacing="0" border="0">
            <tr valign="middle" >
            <td height="30" width="2%" align="left">
              <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
              <td id="all" width="17%" align="left">&nbsp;&nbsp;全选 / 全不选</td>
			    <td width="20%" align="left" ><input type="radio" checked="checked" onClick="menu_check()" id="userState1" value="ONLINE" name="userState"/>在线&nbsp;<input type="radio" checked="checked" onClick="menu_check()" value="OFFLINE" id="userState2" name="userState"/>离线&nbsp;</td>
	
			<td width="13%" align="left" id="menu_1">显示区间&nbsp;</td>	    
	       <td id="menu_2" width="20%" align="left" ><select name="date_select" id="date_select" class="list" onChange="menu_check()" >
		    <option value="0" >1天之内</option> 
	        <option value="1" >3天之内</option>
			 <option value="2" >7天之内</option>
			 <option value="3" >1月之内</option>
			 <option value="4" >所有</option>
	         </select></td>
			
			<td id="menu_8" align="left" width="8%" style="display:"><input type="button" name="DelAllLog" id="DelAllLog" value="全部删除" class="input-button" onClick="delAllLog()"></td>
	   
			  <td id="menu_3" align="left" width="7%"><input type="button" name="refresh" id="refresh" value="刷新" class="input-button" onClick="location.reload()"></td>
              <td  id="menu_4" width="13%"> <input type="button" class="input-button" onClick="addToFilter(WlanApClientsList,0,frame12, macSSID,'在线用户')" value="加入黑名单" id="mange_div2" name="mange_div2"><input type="hidden" id="filterstr" name="filterstr"></td>
              <td  id="menu_5" width="11%">
			   <input type="button" class="input-button" onClick="disconnect(WlanApClientsList,0,frame12, macarrays,'在线用户');" value="断开连接" id="mange_div2" name="mange_div2">
			  </td>
              <td  id="menu_6" width="8%">
                <input type="button" class="input-button" onClick="exportData();" value="导出数据" id="mange_div2" name="mange_div2"><input type="hidden" id="exportstr" name="export"> </td>
	   
            </tr>
			
            </table>

			</td></tr>
            <tr id="menu_7" height="30" valign="middle"><td><b>注意：黑名单功能只有在软件版本是2.4及以上的AP设备生效！</b></td></tr>
		  </table><!--信息列表表单里面表格结束6-->
  </form>

</td><td class="gapTd"></td></tr> </table><!--信息列表表单外面的表格结束5-->
</td></tr>
</table><!--3白的部分结束-->

  </td></tr><script language="javascript">init();</script>
</table><!--倒数第二层2--></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table><!--最外层1-->
<form method="POST" target="" name="staList">
<input type="hidden" value="stalsit" name="parentPage">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="staMac">
<input type="hidden" value="" name="HostName">
<input type="hidden" value="" name="optType">
</form>
</body>
</html>
