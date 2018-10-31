<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("Route");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("firewall");

//function initTranslation()
//{
//	var e;
//
//	e = document.getElementById("add");
//	e.value= _("add");
//	e = document.getElementById("delall");
//	e.value= _("delall");
//	e= document.getElementById("del");
//	e.value= _("del");
//
//}

var strSecondMenuFocus="<%write(rou_routeConf);%>";
<% aspOutStaticRouteList();%>
<%getWanIfCount();%>

//交互变量
/*
var RouteNames=new Array();
var RouteEnables=new Array();
var DesIPs=new Array();
var DesMasks=new Array();
var GateWays=new Array();
var Metrics=new Array();
var Profiles=new Array();


var errorstr="";var totalrecs=0;var max_totalrecs=100;
*/
var RouteEnablesw=new Array();
for(i=0;i<totalrecs;i++){
	if (Profiles[i] == "LAN0") {
		Profiles[i] = "LAN";
	} else {
		/* 无线接口 */
		if (Profiles[i].indexOf("PPTP") == -1) {/*先判断是否是绑定在虚接口上的*/
		var slotIndex = Profiles[i].slice(Profiles[i].length - 1);
		if (is3GPort(slotIndex,wanIfCount)) {
			Profiles[i] = "3G";
		} else if (isAPCliPort(slotIndex, wanIfCount)) {
			Profiles[i] = "APClient";
		}
		}
	}
	RouteEnablesw[i]=(RouteEnables[i]=="1"?"<%write(com_startUse);%>":"<%write(com_forbidden);%>");
}
//end

recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(rou_routeList);%>";
var titles1=new Array(7);
titles1[0]="<%write(rou_routeName);%>";
titles1[1]="<%write(com_status);%>";
titles1[2]="<%write(rou_destNet);%>";
titles1[3]="<%write(com_NetMask);%>";
titles1[4]="<%write(com_GateWayAddr);%>";
titles1[5]="<%write(com_priority);%>";
titles1[6]="<%write(com_Interface);%>";


arrayData1=new Array(RouteNames,RouteEnablesw,DesIPs,DesMasks,GateWays,Metrics,Profiles,RouteEnables);
sortable[0]=new Array(1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_CHINESE,TYPE_IP,TYPE_IP,TYPE_IP,TYPE_NUM,TYPE_STR,TYPE_STR);
arrayCount[0]=8;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,7,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		Route_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else Route_List.id.value=RouteNames[i];
	Route_List.action="Route_edit.asp";
	Route_List.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		Route_List.delstr.value=RouteNames[index];
		Route_List.action="/goform/formStaticRouteDel";
	    Route_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		Route_List.action="/goform/formStaticRouteDelAll";
		Route_List.submit();
	}
}
-->
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
  <td><script language="javascript">showSecondTitle(_("<%write(rou_routeConf);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="Route_List" method="post" action="/goform/formStaticRouteDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(Route_List,0,frame12,RouteNames,'<%write(rou_routeInfo);%>'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
		  </td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
