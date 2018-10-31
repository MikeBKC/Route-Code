<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("firewall");

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

var strSecondMenuFocus="地址组配置";
//<% aspOutStaticRouteList();%>

//交互变量
/*
var RouteNames=new Array();
var RouteEnables=new Array();
var DesIPs=new Array();
var DesMasks=new Array();
var GateWays=new Array();
var Metrics=new Array();
var Profiles=new Array();

*/
var errorstr="";var totalrecs=0;var max_totalrecs=100;
var RouteEnablesw=new Array();

var GroupNames=new Array();
var GroupType=new Array();
var GroupContent=new Array();
//end

recsPerPage[0] =5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="地址组配置列表";
var titles1=new Array(3);
titles1[0]="组名";
titles1[1]="类型";
titles1[2]="成员";


arrayData1=new Array(GroupNames,GroupType,GroupContent);
sortable[0]=new Array(0,0,0);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=2;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,3,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		AddGroup_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("配置已经达到最大条数,不能添加新配置");
			return;
		}
	}
	else AddGroup_List.id.value=AddGroupNames[i];
	AddGroup_List.action="FwAddress_edit.asp";
	AddGroup_List.submit();
}
function DeleteConfig(index){
	if(confirm("确定删除？")){
		AddGroup_List.delstr.value=AddGroupNames[index];
		AddGroup_List.action="/goform/";
	    AddGroup_List.submit();
	}
}
function delAll(){
	if(confirm("确定删除所有的条目？")){
		AddGroup_List.action="/goform/";
		AddGroup_List.submit();
	}
}
-->
</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("地址组配置信息列表");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="AddGroup_List" method="post" action="/goform/formStaticRouteDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="140" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
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
			  <td width="95" valign="top" align="right"><input type="button" value="删除所有条目" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="删除" class="input-button" onClick="return(genData(AddGroup_List,0,frame12,AddGroupNames,'地址组配置信息'))" id="del"></td>
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
