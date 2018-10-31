<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("AlitalkFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" ContentType="text/xml"> 

<% getSysLang(); %>
var strSecondMenuFocus="<% write(AliWhite); %>";
 
//交互变量
var errorstr="";var totalrecs=0;var max_totalrecs=100;
var indexID=new Array();
var AliNumber=new Array();
var remark=new Array();
var exceptAliEnable="";
var indexID=new Array();
var remark=new Array();
exceptAliEnable="off";
var totalrecs=0;var max_totalrecs=2048;
var errorstr="";
<%aspExceptAliData();%>
//end
 
recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(AliWhiteList);%>";
var titles1=new Array(2);
//titles1[2]="ID";
titles1[0]="<% write(AliID); %>";
titles1[1]="<% write(Describe); %>";
 
 
arrayData1=new Array(AliNumber,remark,indexID);
sortable[0]=new Array(1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR);
arrayCount[0]=3;
 
<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,2,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		Alitalk_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(MaxConfErr); %>");
			return;
		}
	}
	else Alitalk_List.id.value=indexID[i];
	Alitalk_List.action="AlitalkFilter_edit.asp";
	Alitalk_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(SureDel); %>")){
		Alitalk_List.delstr.value=indexID[index];
		Alitalk_List.action="/goform/formExceptAliDel";
	    Alitalk_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(SureDelAll); %>")){
		Alitalk_List.action="/goform/formExceptAliDelAll";
		Alitalk_List.submit();
	}
}
function save()
{
	Alitalk_List.action="/goform/formExceptAliGlobalConfig";
	Alitalk_List.submit();
}
function init()
{
	Alitalk_List.ExceptAliEnable.checked=(exceptAliEnable=="on"?true:false);
}
-->
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
  <td><script language="javascript">showSecondTitle("<% write(AliWhite); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="Alitalk_List" method="post" action="/goform/formExceptAliDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td colspan="4" height="20">&nbsp;
   <table width="100%"  border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td  colspan="4" height="20">&nbsp;</td></tr>
   	<tr>
	    <td height="20" width="40%" align="right" id="WebAuth_Switch"><% write(UseAliWhiteList); %></td>
		<td align="left" width="5%"><input type="checkbox" name="ExceptAliEnable" id="ExceptAliEnable"></td>
		<td align="left">
		<input type="hidden" name="AliEnblew"><input type="hidden" name="AliEnbleOld"><input type="button" class="input-button" id="uttsave" value="<% write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#AliFilter')" id="utthelp">
		</td><td>&nbsp;</td>
	</tr>
	<tr><td  colspan="4" height="20">&nbsp;</td></tr>
   </table>
   </td></tr>
      <tr><td colspan="4" height="20">
   <table width=600 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
  </tr>
</table>
   </td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11"  scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<% write(Selecte);%></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<% write(add_new); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<% write(delete_all); %>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(Alitalk_List,0,frame12,indexID,'<% write(AliWhiteConf); %>'))" id="del"></td>
			  </tr>
		      </table></td>
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
		  </td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"></td><script language="javascript">init();</script>
  </tr>
</table>

</body>
</html>
