
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("WlanMacFilter"); %>
<% langTran("common"); %>
<% langTran("MacFilter"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
<% 
outputMacFilterConfig();
%>

recsPerPage[0]=10;
lineButton[0]=10;
maxrecs[0]=max_totalrecs;
currpage[0]=1;
recscount[0]=totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(MACAddrFilterList); %>";
var titles1=new Array(2);
titles1[0]="<% write(com_user); %>";
titles1[1]="<% write(MACAddr); %>";

arrayData1=new Array(usernames,filterMacs);
sortable[0]=new Array(1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR);
arrayCount[0]=2;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,2,arrayData1,1,1);
}
/******************list***********************/
function init(){
	var f=MacFilterGloable_Config;
	f.MacFilterEnable.checked=(MacFilterEnables=="0"?false:true);
	if(filterRules=="0") f.filterRule[0].checked=true;
	else f.filterRule[1].checked=true;
}
function save(){
	MacFilterGloable_Config.action="/goform/ConfigMacFilterGlobalConfig";
	MacFilterGloable_Config.submit();
}
function EditConfig(val,i){
	if(val==0) MacFilter_List.id.value="";
	else
	{
	    MacFilter_List.id.value=usernames[i];
	}    
	MacFilter_List.action="MacFilter_edit.asp";
	MacFilter_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(SureDel); %>")){
	    MacFilter_List.delstr.value=usernames[index];
	    MacFilter_List.action="/goform/ConfigMacFilterDel";
	    MacFilter_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(SureDelAll); %>")){
		MacFilter_List.action="/goform/ConfigMacFilterDelAll";
		MacFilter_List.submit();
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
  <td><script language="javascript">showSecondTitle("<% write(mf_MacFilter); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						2,
						new Array("<%write(mf_List);%>",'MacFilter.asp',1,1),
						new Array("<%write(mf_Config);%>",'MacFilterConfig.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td" valign="top"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>
	<form action="" onSubmit="return " name="MacFilterGloable_Config" method="post">
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
<tr>
  <td width="28%" height="25" align="right" valign="middle" id="start MAC filter"><% write(StartMacAddrFilter); %></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle"><input type="checkbox" name="MacFilterEnable"></td>
  </tr>
  <tr>
  <td width="28%" height="25" align="right" valign="middle" id="filter rule"><% write(FilterRule); %></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle">
  <input type="radio" value="0" name="filterRule"  checked="checked">  <font style="font-size:10px; color:#666666"><script language="javascript">document.write("<% write(mf_Allow); %>");</script></font></td>
  </tr>
   <tr>
  <td width="22%" height="25" align="right" valign="middle"></td>
  <td width="2%" height="25">&nbsp;</td>
  <td align="left"  height="25" valign="middle">
<input type="radio" value="1" name="filterRule">  <font style="font-size:10px; color:#666666"><script language="javascript">document.write("<% write(mf_Forbid); %>");</script></font></td>
  </tr>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
  <tr><td  colspan="3">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#MacFilter')"></td>
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
<table border="0"  align='center' width="80%">
<form name="MacFilter_List" method="post" action="/goform/ConfigMacFilterDel">
   <table id=grouplist style="display"  border="0" align="center" cellpadding="0" cellspacing="0">
		    <tr>
		      <td colspan="4">
			<iframe src="_listhead1.htm" name="frame11"  align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4">
			<iframe src="_list1.htm" name="frame12"	id="framelist" align="MIDDLE" width="920" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
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
			  <td width="40" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(MacFilter_List,0,frame12,usernames,'<% write(MacAddrFilterMsg); %>'))" id="del"></td>
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
