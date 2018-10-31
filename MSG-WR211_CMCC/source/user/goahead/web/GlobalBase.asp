<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("GlobalBase");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript">
var strSecondMenuFocus="<%write(com_netAction);%>";


<% aspOutGroupConfigList();  %>


var ManageObj=new Array();
var EffectTimes=new Array();
var temp = new Array();
for(i=0;i<totalrecs;i++){
	
	if(sourceIPs[i] == 0)
	{
	    ManageObj[i]=FromIPs[i]+"--"+EndIPs[i];
	}
	else
	{
	    ManageObj[i] = ipGrpNames[i]; 
	}
	Allows[i]=(Allows[i]=="1"?"Yes":"No");
	/*DenyQQs[i]=(DenyQQs[i]=="1"?"Yes":"No");
	DenyMSNs[i]=(DenyMSNs[i]=="1"?"Yes":"No");
	DenyBTs[i]=(DenyBTs[i]=="1"?"Yes":"No");
	DenyThunders[i]=(DenyThunders[i]=="1"?"Yes":"No");*/
	EffectTimes[i]="";
    pdbNames[i] = pdbPolicys[i];
	

	
	if(group_days[i]==effectDay)
		EffectTimes[i]="<%write(com_every);%>";
	else{
		for(j=0;j<group_days[i].length;j++){
			if(group_days[i].charAt(j)=="1"){
				if(EffectTimes[i].length) EffectTimes[i]+="，";
					EffectTimes[i]+=DateArray[j];
			}
	 	}
	}
	
	if(group_timestarts[i]!=effectTime){
		EffectTimes[i]+="；"+group_timestarts[i]+"-"+group_timestops[i];
	}
 
		if(pdbNames[i].length > 1){
			  temp[i] = pdbNames[i].substr(0,40);
			  if( temp[i].length == 40){temp[i] = temp[i] + "...... "}
			  else{}
	} else
	{
		temp[i]= "";
		}
}


 
   
 
 


 


recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs; 
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(glo_list);%>";
var titles1=new Array(5);
titles1[0]="<%write(com_group);%>";
//titles1[1]="起始IP地址";
//titles1[2]="结束IP地址";
titles1[1]="<%write(glo_object);%>";
titles1[2]="<%write(glo_forbid);%>";
titles1[3]="<%write(glo_time);%>";
titles1[4]="<%write(com_startUse);%>";


arrayData1=new Array(GroupNames,ManageObj,temp,EffectTimes,Allows);
sortable[0]=new Array(1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=5;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,1000,titles1,5,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		GroupBaseList.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else GroupBaseList.id.value=GroupNames[i];
	GroupBaseList.action="GlobalBase_edit.asp";
	GroupBaseList.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		GroupBaseList.delstr.value=GroupNames[index];
		GroupBaseList.action="/goform/formGroupConfigDel";
	    GroupBaseList.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		GroupBaseList.action="/goform/formGroupConfigDelAll";
		GroupBaseList.submit();
	}
}

function SaveUserAllow(index,i)
{
 
	if (Allows[i].toLowerCase()=="no")　//复选框原来的值是"DENY"，即不选中，那么点击后调用本函数时状态应该是选中
	{
		str="<%write(com_startUse);%>";
		GroupBaseList.Allow.value="1";
	}
	else
	{
		str="<%write(com_forbidden);%>";
		GroupBaseList.Allow.value="0";
	}
	if (confirm("<%write(com_really);%>"+str+"<%write(glo_config);%>"))
	{
		 GroupBaseList.AllowID.value = GroupNames[i];
		 GroupBaseList.action ="/goform/formGroupAllow";
		 GroupBaseList.submit();
	}
	else
	{
		dsplisthead(index,frame11);
    	dsplist(index,frame12);
	}
}
function save()
{
	GroupBaseList.action="/goform/formUttNfGlobalConfig";
	GroupBaseList.submit();
}

function init()
{
	GroupBaseList.GlobalEnable.checked= (uttNfEnable == "on");
}

function jumpUserLinkRatePage() {
	parent.main.location = "UserLinkRate.asp";
}
-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(glo_manage);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
   <script language="javascript">showright(rightstr,80);</script>
  <tr valign="top">
    <td ><form name="GroupBaseList" method="post" action="/goform/formGroupConfigDel">
	<table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
   			<tr><td colspan="4">
				<table width="661">
				<tr>
				<td width="391" align="right">&nbsp;<%write(glo_start);%>&nbsp;</td>
				<td width="20"><input type="checkbox" name="GlobalEnable" id="GlobalEnable"></td>
				<td width="10">&nbsp;</td>
				<td width="240"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()"></td>
				</tr>
				</table>
			</td>
			</tr>
			
			<tr><td colspan="4" height="20">
			<table width="100%" border="0" cellspacing="0" cellpadding="0">
			<tr>
			<td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
			</tr>
			</table>
			</td></tr>
			
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no"  align="MIDDLE" width="100%" height="45" scrolling="no" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="100%" height="270" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<%write(com_select);%></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"><input type="hidden" name="AllowID"><input type="hidden" name="Allow"> </td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(GroupBaseList,0,frame12,GroupNames,'<%write(glo_info);%>'))"></td>
			  </tr>
		      </table></td>
		    </tr><tr><td colspan="4" height="20">&nbsp;</td></tr>
		  </table>
		  </td></tr></table>
  </form></td>
  </tr>
</table></td>
  <td width="15"><script language="javascript">init();</script></td>
  </tr>
</table>
</body>
</html>
