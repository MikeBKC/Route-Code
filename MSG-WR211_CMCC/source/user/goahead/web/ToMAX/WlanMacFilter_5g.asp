
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("WlanMacFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
<% 
outputWlanMacFilterConfig_5g();
%>
//maxrecs[0]=eval("
<% 
//write(GetInstMaxNum(0)); 
%>
//");
//start交互
//var MacFilterEnables="0";var filterRules="";var filterMacs=new Array();var totalrecs=0;var errorstr="";maxrecs[0]=0;
//end交互
filterRules=(filterRules==""?"1":filterRules);
//var max_recs=128;
var strSecondMenuFocus="<% write(WirelessMacFilter); %>";

var ids=new Array();
for(i=0;i<filterMacs.length;i++){
	ids[i]=i+1;
}
maxrecs[0]=50;
currpage[0]=1;
recsPerPage[0] = 10;
lineButton[0]=1050;
recscount[0]=totalrecs;
//var maxrecs=new Array();
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(MACAddrFilterList); %>";
var titles1=new Array(2);
titles1[0]="ID";
titles1[1]="<% write(MACAddr); %>";

arrayData1=new Array(ids,filterMacs);
sortable[0]=new Array(1,1);
sortType[0]=new Array(TYPE_NUM,TYPE_STR);
arrayCount[0]=2;


function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,2,arrayData1,1,1);
}
/******************list***********************/
function init(){
	var f=MacFilterGloable_Config;
	f.MacFilterEnable.checked=(MacFilterEnables=="0"?false:true);
	if(filterRules=="1") f.filterRule[0].checked=true;
	else f.filterRule[1].checked=true;
}
function save(){
	MacFilterGloable_Config.action="/goform/ConfigWlanMacFilterGlobalConfig_5g";
	MacFilterGloable_Config.submit();
}
function EditConfig(val,i){
	if(val==0) WlanMacFilter_List.id.value="";
	else
	{
	    filterMacs[i] = filterMacs[i].replace(/:/g,"");
	    WlanMacFilter_List.id.value=filterMacs[i];
	}    
	WlanMacFilter_List.action="WlanMacFilter_edit_5g.asp";
	WlanMacFilter_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(SureDel); %>")){
		WlanMacFilter_List.delstr.value=filterMacs[index];
		WlanMacFilter_List.action="/goform/ConfigWlanMacFilterDel_5g";
	    WlanMacFilter_List.submit();
		
	}
}
function delAll(){
	if(confirm("<% write(SureDelAll); %>")){
		WlanMacFilter_List.action="/goform/ConfigWlanMacFilterDelAll_5g";
		WlanMacFilter_List.submit();
	}
}
//-->
</script>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(WirelessMacFilter); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="80%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>
	<form action="" onSubmit="return " name="MacFilterGloable_Config" method="post">
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
<tr>
  <td width="30%" height="25" align="right" valign="middle" id="start MAC filter"><% write(StartMacAddrFilter); %></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle"><input type="checkbox" name="MacFilterEnable"></td>
  </tr>
  <tr>
  <td width="22%" height="25" align="right" valign="middle" id="filter rule"><% write(FilterRule); %></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle">
  <input type="radio" value="1" name="filterRule"  checked="checked">  <font style="font-size:10px; color:#666666"><script language="javascript">document.write("<% write(Allow); %>");</script></font></td>
  </tr>
   <tr>
  <td width="22%" height="25" align="right" valign="middle"></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle">
<input type="radio" value="2" name="filterRule">  <font style="font-size:10px; color:#666666"><script language="javascript">document.write("<% write(Forbid); %>");</script></font></td>
  </tr>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
  <tr><td  colspan="3">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#WlanMacFilter')"></td>
  </tr>
</table>
  </td></tr>
  <tr><td colspan="3" align="center" height="40"><table width=920 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
  </form></table></td>
  </tr>
  <tr><td>
<table border="0"  align='center' width="100%">
<form name="WlanMacFilter_List" method="post" action="/goform/ConfigWlanMacFilterDel_5g">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="920" height="256" marginwidth="1" marginheight="1" frameborder="0"></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write("<% write(Selecte); %>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="90" valign="top" align="right"><input type="button" value="<% write(AddrNew); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="110" valign="top" align="right"><input type="button" value="<% write(DelAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			  <td width="40" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(WlanMacFilter_List,0,frame12,filterMacs,'<% write(MacAddrFilterMsg); %>'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr>
			<tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
  </form>
</table></td></tr>
</table> 
  </td></tr><script language="javascript">init();</script>
</table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
