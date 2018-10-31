<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("MSNFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">


<% getSysLang(); %>
var strSecondMenuFocus="<% write(MSNWhite); %>";

//交互变量
var errorstr="";var totalrecs=0;var max_totalrecs=100;
var indexID=new Array();
var msnNumber=new Array();
var remark=new Array();
<%aspExceptMSNData();%>
//end

recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(MSNWhiteList); %>";
var titles1=new Array(2);
//titles1[2]="ID";
titles1[0]="<% write(MSNID); %>";
titles1[1]="<% write(Describe); %>";


arrayData1=new Array(msnNumber,remark,indexID);
sortable[0]=new Array(1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR);
arrayCount[0]=3;


function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,2,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		MSNFilter_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(MaxConfErr); %>");
			return;
		}
	}
	else MSNFilter_List.id.value=indexID[i];
	MSNFilter_List.action="MSNFilter_edit.asp";
	MSNFilter_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(SureDel); %>")){
		MSNFilter_List.delstr.value=indexID[index];
		MSNFilter_List.action="/goform/formExceptMSNDel";
	    MSNFilter_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(SureDelAll); %>")){
		MSNFilter_List.action="/goform/formExceptMSNDelAll";
		MSNFilter_List.submit();
	}
}
function save()
{
	MSNFilter_List.action="/goform/formExceptMSNGlobalConfig";
	MSNFilter_List.submit();
}
function init()
{
	MSNFilter_List.ExceptMSNEnable.checked=(exceptMSNEnable=="on"?true:false);
}

</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(MSNWhite); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="MSNFilter_List" method="post" action="/goform/formExceptMSNDel">
		<p>&nbsp;</p>
		<script language="javascript">
		var arrayThreeMenu=new Array(
						"95%",
						3,
						new Array("QQ白名单",'QQFilter.asp',1,0),
						new Array("MSN白名单",'MSNFilter.asp',1,1),
						new Array("阿里旺旺白名单",'AlitalkFilter.asp',1,0)
					
		);
		showThreeMenu();
	</script>
	<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
    <td align="center" class="three_td">
   <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td colspan="4" height="20">&nbsp;
   <table width="95%"  border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td  colspan="4" height="20">&nbsp;</td></tr>
   	<tr>
	    <td height="20" width="40%" align="right" id="WebAuth_Switch"><% write(UseMSNWhiteList); %></td>
		<td align="left" width="5%"><input type="checkbox" name="ExceptMSNEnable" id="ExceptMSNEnable"></td>
		<td align="left">
		<input type="hidden" name="MSNEnblew"><input type="hidden" name="MSNEnbleOld"><input type="button" class="input-button" id="uttsave" value="<% write(com_save); %>"  onClick="save()">&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#MSNFilter')" id="utthelp">
		</td><td>&nbsp;</td>
	</tr>
	<tr><td  colspan="4" height="20">&nbsp;</td></tr>
   </table>
   </td></tr>
      <tr><td colspan="4" height="20">
   <table width=600 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<% write(com_imgdot); %>"><img src="<% write(com_imgspace); %>" width="8" height="1"></td>
  </tr>
</table>
   </td></tr>
		     <tr>
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11"  scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write("<% write(Selecte); %>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<% write(com_addNew); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<% write(com_delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(MSNFilter_List,0,frame12,indexID,'<% write(MSNWhiteConf); %>'))" id="del"></td>
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
