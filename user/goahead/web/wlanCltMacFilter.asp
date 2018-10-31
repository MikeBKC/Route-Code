<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<% langTran("common"); %>
<%langTran("commonWords");%>
<%langTran("wlanCltList");%>
<% langTran("User"); %>

<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">

<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var lang="zhcn";
var strSecondMenuFocus = "无线黑名单"




//-----------交互开始------------
/*
var totalrecs = "3";
var max_totalrecs = "128";
var macarrays = new Array();
var linkedSSIDs = new Array();
macarrays[0] = "0022aa112233";
macarrays[1] = "0022aa445566";
macarrays[2] = "0022aa778899";
linkedSSIDs[0] = "test1";
linkedSSIDs[1] = "test2";
linkedSSIDs[2] = "test3";
*/
<%
aspOutShowBlackList();
%>
//-----------交互结束------------

var errorstr="";
var titles1 = new Array();
titles1[0] = "MAC地址";
titles1[1] ="SSID";

var macSSID = new Array();
for (i=0;i<totalrecs;i++){
	macSSID[i] = macarrays[i]+":"+linkedSSIDs[i];
}



currpage[0] = 1;	//当前页
recscount[0] = totalrecs;	//当前条目数
maxrecs[0] =max_totalrecs;	//最大条目数
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] ="黑名单列表";
sortable[0] = new Array(1,1);
sortType[0] = new Array(TYPE_STR, TYPE_STR);
arrayCount[0] = 2;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 0;
var arrayData1 = new Array(macarrays,linkedSSIDs);

function dsplist(index, list) {
    if (index == 0) dspList(index, list, 600, titles1, titles1.length, arrayData1, 1, 1);
}


function init() {
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
}

function EditConfig(val,i){
	
}


function DeleteConfig(index){
	if(confirm("<% write(MakeSureDel); %>")){
		WlanApClientsList.delstr.value=macSSID[index];
		//alert(WlanApClientsList.delstr.value);
		WlanApClientsList.action="/goform/formDelBlackList";
	    WlanApClientsList.submit();
	}
}
function delConfig(formName, index, list, array, tips, flagstr) //删除列表中的元素
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.delstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //显示页面的开始记录数 以0为开始下标
    recend = recbegin + recsPerPage[index]; //显示页面的结束记录数
 
    if (recend >= recscount[index]) recend = recscount[index]; //如果是最后页　把总计量赋值给recend
 
    for (var i = recbegin; i < recend; i++) { //因为recbegin比显示的开始记录数小一，所以i< 并非 <= recend
 
        if (chks.length) { //循环遍历checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.delstr.value = formName.delstr.value + array[eval(chks[t].value)] + flagstr;
                //生成隐藏参数
            } else {
                formName.delstr.value = formName.delstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.delstr.value = formName.delstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.delstr.value = formName.delstr.value + array[i] + flagstr;
            }
        }
 
        t = t + 1;
    }
 
    if (formName.delstr.value != ""){
		formName.delstr.value=formName.delstr.value.substring(0,formName.delstr.value.length-1)
		//alert(formName.delstr.value);
		formName.action = "/goform/formDelBlackList"
		formName.submit();
		}//判断是否选择checkbox
    else {
        alert("请先选择" + tips + "记录。");
    }
}




function delAll(){
	if(confirm("<% write(MakeSureDelAll); %>")){
		WlanApClientsList.action="/goform/formDelAllBlackList";
		WlanApClientsList.submit();
	}
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
						"80%",
						6,
						
						new Array("AP用户统计",'WlanApClientsStatistics.asp',1,0),
						new Array("SSID用户统计",'WlanSSIDClientsStatistics.asp',1,0),
						new Array("历史用户统计",'WlanHistoryClientsStatistics.asp',1,0),
						new Array("历史流量统计",'historyFlowStatistics.asp',1,0),
						new Array("用户列表",'wlanCltList.asp',1,0),
						new Array("黑名单",'wlanCltMacFilter.asp',1,1)
						
		);
		showThreeMenu();
	</script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><!--中间白的部分表3-->
<tr>
    <td align="center"  class="three_td" valign="top">
<table border="0"  align='center'><tr><td class="gapTd"></td><td align="center"><!--下面部分表单外面的表格5-->
<form name="WlanApClientsList" method="post" action="">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0"><!--列表表单里面的表格666666-->
		<tr><td colspan="4" height="20">&nbsp;</td></tr>   
		<tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" scrolling="no"  name="frame11"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" scrolling="no"  name="frame12"	id="framelist" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
	<input type="hidden" name="clientenableID"><input type="hidden" name="clientenable"><input type="hidden" name="macRule"><input type="hidden" name="filterSsid"></tr>
                    

			<tr><td colspan="4" height="30">
				  <table width="600" align="center"  cellpadding="0" cellspacing="0" border="0">
            <tr valign="middle">
            <td height="21" nowrap width="2%" align="left">
              <input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
              <td nowrap id="all" align="left" width="">&nbsp;&nbsp;全选 / 全不选</td>
            
             <td width="15%"  align="right"><input type="button" value="<% write(com_delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			  <td width="10%" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(delConfig(WlanApClientsList,0,frame12, macSSID,'黑名单'))" id="del"><input type="hidden" name="delstr" id="delstr"></td>
             
             
            </table>

			</td></tr>
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
</body>
</html>
