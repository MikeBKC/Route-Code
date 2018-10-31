<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("QQFilter");%>
<%langTran("PPPoEServer");%>
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

var strSecondMenuFocus="<%write(com_filter);%>";

//交互变量
<%aspExceptQQData();%>
//end

recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(qqf_qqList);%>";
var titles1=new Array(2);
//titles1[2]="ID";
titles1[0]="<%write(qqf_qqNum);%>";
titles1[1]="<%write(com_descript);%>";


arrayData1=new Array(qqNumber,remark,indexID);
sortable[0]=new Array(1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR);
arrayCount[0]=3;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,2,arrayData1,1,1);
}
/******************list***********************/
function ExportUser(){
	QQFilter_List.action="/goform/formQQFilterExport";
	QQFilter_List.submit();
}
function ImportConfig(){
	QQFilter_List.action="QQFilterImport.asp";
	QQFilter_List.submit();
}
function EditConfig(val,i){
	if(val==0){ 
		QQFilter_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else QQFilter_List.id.value=indexID[i];
	QQFilter_List.action="QQFilter_edit.asp";
	QQFilter_List.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		QQFilter_List.delstr.value=indexID[index];
		QQFilter_List.action="/goform/formExceptQQDel";
	    QQFilter_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		QQFilter_List.action="/goform/formExceptQQDelAll";
		QQFilter_List.submit();
	}
}
function delQQ()
{
	genData(QQFilter_List, 0, frame12, indexID, '<%write(qqf_qqInfo);%>');
	QQFilter_List.action="/goform/formExceptQQDel";
	QQFilter_List.submit();
}
function save()
{
    QQFilter_List.action="/goform/formExceptQQGlobalConfig";
    QQFilter_List.submit();
}
function init()
{
    QQFilter_List.ExceptQQEnable.checked=(exceptQQEnable.toLowerCase()=="on"?true:false);
    QQFilter_List.ExceptEnterpriseQQEnable.checked=(exceptEnterpriseQQEnable.toLowerCase()=="on"?true:false);
}
-->
</script>
</head>
<body onbeforeunload="logOut()"  background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(com_filter);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="QQFilter_List" method="post" action="/goform/formExceptQQDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td colspan="4" height="20">
   <table width="95%"  border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td  colspan="4" height="20">&nbsp;</td></tr>
   	<tr>
        <td height="20" width="55%" align="right" id="EnterpriseQQ_Switch"><%write(qqf_allow);%></td>
	<td align="left" width="45%"><input type="checkbox" name="ExceptEnterpriseQQEnable" id="ExceptEnterpriseQQEnable"></td>
	</tr>
   	<tr>
        <td height="20" width="55%" align="right" id="QQ_Switch"><%write(qqf_start);%></td>
	<td align="left" width="45%"><input type="checkbox" name="ExceptQQEnable" id="ExceptQQEnable"></td>
	</tr>
	<tr> 
	<td align="center" colspan="2">
	<input type="hidden" name="QQEnblew"><input type="hidden" name="QQEnbleOld"><input type="button" class="input-button" id="uttsave" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<% write(com_helppage); %>#QQFilter')" id="utthelp">
		</td>
	</tr>
	<tr><td  colspan="4" height="20">&nbsp;</td></tr>
   </table>
   </td></tr>
   <tr><td colspan="4" height="20">
   <table width=600 border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
  </tr>
</table>
   </td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4" style="background-color:#ffffff;"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="260" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<script language="javascript">document.write(_("<%write(com_select);%>"));</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			  <td width="120" valign="top" align="right"><input type="button" value="<%write(pps_export);%>" class="input-button" onClick="ExportUser();" id="export"></td>
			  <td width="69" valign="top" align="right"><input type="button" value="<%write(pps_import);%>" class="input-button" onClick="ImportConfig()" id="export"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="delQQ()"></td>
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
