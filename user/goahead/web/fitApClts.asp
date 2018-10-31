<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
 <%langTran("common");%>
 <%langTran("fitApClts");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
Butterlate.setTextDomain("ac_ap");

//交互begin
<%
aspOutWlanStationList();
	//aspOutPutWlanCltsList();
	//write('var HostMacs=new Array("0022aa6581ca");\n');
	//write('var BWs=new Array("40M");\n');
	//write('var totalrecs=1;\n');
	//write('var errorstr = "";\n');
%>
//交互end

function initTranslation()
{
	var e;
	e = document.getElementById("refresh");
	e.value= "<% write(update); %>";
}
var ids=new Array();
for(i=0;i<totalrecs;i++){
	ids[i]=i+1;
}
recsPerPage[0] = 10;
lineButton[0] = 0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(wireHostLT); %>";
var titles1=new Array();
titles1[0]="<% write(sequenceNo); %>";
titles1[1]="<% write(macAddr); %>";
titles1[2]="<% write(channelBW); %>";


sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_STR,TYPE_STR);
arrayCount[0]=3;
arrayData1=new Array(ids,HostMacs,BWs);

function dsplist(index,list)
{
	if(index==0) dspList(index,list,-1,titles1,titles1.length,arrayData1,0,0);
}

function init() {
	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
	frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	initTranslation();
}

</script>
</head>
<body onLoad="initTranslation();init();" onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(relessHostSt); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="fitApClts" method="POST">
<table  border="0" width="600" align="center" cellpadding="0" cellspacing="0">
<tr>
<td align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td align="center" valign="top">
<iframe name="frame12" id="framelist" scrolling="no" valign="top" align="MIDDLE" width="600" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
<td align="right" valign="top">
<input type="button" value="<% write(com_fresh); %>" class="input-button" onClick="location.reload()" id="refresh"></td>
</tr>
</table>
 </form>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
   </td></tr> </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>
</body>
</html>
