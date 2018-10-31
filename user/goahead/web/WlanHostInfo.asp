
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("common"); %>
<% langTran("WlanHostInfo"); %>
<% langTran("commonWords"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
<%
aspOutWlanStationList();
%>
//start交互
//var HostMacs=new Array();var Allows=new Array();var BWs=new Array();var SGIs=new Array();var totalrecs=0;var errorstr="";
//end交互
var strSecondMenuFocus="<% write(WirelessHostStat); %>"; 
var ids=new Array();
for(i=0;i<totalrecs;i++){
	ids[i]=i+1;
}
recsPerPage[0] = 20;
lineButton[0] =20;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(WirelessHostSList); %>";
var titles1=new Array(11);
titles1[0]="<% write(ID); %>";
titles1[1]="<% write(MACAddr); %>";
titles1[2]="<% write(Filter); %>";
titles1[3]="<% write(ChannelBand); %>";


arrayData1=new Array(ids,HostMacs,Allows,BWs);
sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=4;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,4,arrayData1,0,0);
}
/******************list***********************/
function SaveUserAllow(index,i)
{
	if (Allows[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<% write(Filter); %>";
	}
	else
	{
		str="<% write(CancelFilter); %>";
	}
	WlanHostInfo_List.filterMac.value=HostMacs[i];
	WlanHostInfo_List.delstr.value=HostMacs[i];
	if (confirm("<% write(ConfirmStr1); %>"+str+"<% write(ConfirmStr2); %>"))
	{
		if(str=="<% write(Filter); %>") WlanHostInfo_List.action ="/goform/ConfigWlanMacFilter";
		else  WlanHostInfo_List.action ="/goform/ConfigWlanMacFilterDel";
		WlanHostInfo_List.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}
function FilterAll(){
	WlanHostInfo_List.action="/goform/ConfigAddAllMAC";//HostListAddAll
	WlanHostInfo_List.submit();
}
function clearInfo(){
	WlanHostInfo_List.action="/goform/ConfigHostListClear";
	WlanHostInfo_List.submit();
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(SecondTitle); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td ><form name="WlanHostInfo_List" method="post" action="/goform">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
    <tr><td colspan="4"><table width="100%"><tr>
	  <td  class="helpfont" height="30" id="discription6"><strong><% write(ShowMsg0); %></strong><% write(ShowMsg); %><br>
	  <strong><% write(ShowMsg1); %></strong><% write(ShowMsg2); %><br></td>	  
	  </tr></table></td></tr>
		    <tr>
		      <td colspan="4">
		      <iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="485" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="hidden" name="pages" value="WlanHostInfo.asp"></div></td>
			  <td  class="style6" ><input type="hidden" name="filterMac"><input type="hidden" name="delstr"></td> 
			  <td width="90" valign="top" align="right"></td>
			  <td width="100" valign="top" align="right"><input type="button" value="<% write(FilterAll); %>" class="input-button" onClick="FilterAll()" id="filter all"></td>
			  <td width="50" valign="top" align="right"><input type="button" value="<% write(com_fresh); %>" class="input-button" onClick="location.reload()" id="refresh"></td>
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
