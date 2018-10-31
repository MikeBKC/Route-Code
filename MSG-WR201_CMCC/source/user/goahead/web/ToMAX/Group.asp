<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("Group");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript">
var strSecondMenuFocus="<%write(com_limit);%>";
<% aspOutGroupConfigList();  %>
var LimitObj=new Array();
var dst_LimitObj=new Array();
var EffectTimes=new Array();
for(i=0;i<totalrecs;i++){
	if(sourceIPs[i] == 0)
	{
	    LimitObj[i]=FromIPs[i]+"--"+EndIPs[i];
	}
	else
	{
	    LimitObj[i] = ipGrpNames[i]; 
	}
    if(dst_sourceIPs[i] == 0)
    {
        dst_LimitObj[i]=dst_FromIPs[i]+"--"+dst_EndIPs[i];
    }
    else
    {
        dst_LimitObj[i]=dst_ipGrpNames[i];
    }
	policy[i]=(policy[i]=="1"?"<%write(gro_independ);%>":"<%write(gro_share);%>");
	EffectTimes[i]="";
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
}


recsPerPage[0] =10;
lineButton[0]=10;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs; 
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(gro_limitList);%>";
var titles1=new Array();
titles1[0]="<%write(com_group);%>";
//titles1[1]="<%write(gro_Limduixiang);%>";
titles1[1]="源地址组";
titles1[2]="目的地址组"
titles1[3]="<%write(gro_speed);%>";
titles1[4]="<%write(com_downloadLimit);%>";
titles1[5]="<%write(com_uploadLimit);%>";
titles1[6]="<%write(gro_time);%>";
arrayData1=new Array(GroupNames,LimitObj,dst_LimitObj,policy,dRate,uRate,EffectTimes);
arrayCount[0]=7;

function dsplist(index,list)
{
	if(index==0) dspList(index,list,920,titles1,titles1.length,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		Group_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<%write(com_editAlert);%>");
			return;
		}
	}
	else Group_List.id.value=GroupNames[i];
	Group_List.action="Group_edit.asp";
	Group_List.submit();
}
function DeleteConfig(index){
	if(confirm("<%write(com_deleteTip);%>")){
		Group_List.delstr.value=GroupNames[index];
		Group_List.action="/goform/formGroupConfigDel";
	    Group_List.submit();
	}
}
function delAll(){
	if(confirm("<%write(com_deleteAllTip);%>")){
		Group_List.action="/goform/formGroupConfigDelAll";
		Group_List.submit();
	}
}
function move()
{
	if(GroupNames.length<1)
	{
		alert("<%write(gro_policyTip);%>");
		return;
	}
	Group_List.action="/goform/formGroupConfigMoveConfig";
	Group_List.submit();
}

</script>
</head>
<body onLoad=""onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(com_limit);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
   <script language="javascript">showright(rightstr,80);</script>
  <tr valign="top">
    <td ><form name="Group_List" method="post" action="/goform/formGroupConfigDel">
	<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr>
		      <td colspan="4" align="center"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="920" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4" align="center"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="920" height="265" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="920"  height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				<input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));>
		<%write(com_select);%></div></td>
			  <td  class="style6" ><input type="hidden" name="page" value="limit"><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<%write(com_addNew);%>" class="input-button" onClick="EditConfig(0)"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<%write(com_delAll);%>" class="input-button" onClick="delAll()"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<%write(com_delete);%>" class="input-button" onClick="return(genData(Group_List,0,frame12,GroupNames,'<%write(gro_limitInfo);%>'))"></td>
			  </tr>
		      </table></td>
		    </tr>
			<tr><td colspan="4" height="10">&nbsp;&nbsp;</td></tr>
			<tr><td colspan="4" height="20">
			<table>
			<tr><td><%write(com_fromPol);%>&nbsp;<select name="sID" id="sID" class="input" style="width:120px">
				<script language="javascript">
				for(var i=0;i<GroupNames.length;i++){
					document.write('<option value="'+GroupNames[i]+'">'+GroupNames[i]+'</option>');
				}
				</script>
			</select>
			&nbsp;<input type="button" value="<%write(com_moveTo);%>" onClick="move()">&nbsp;<%write(com_policy);%>
			<select name="tID" id="tID" class="input" style="width:120px">
			<script language="javascript">
			for(var j=0;j<GroupNames.length;j++){
				document.write('<option value="'+GroupNames[j]+'">'+GroupNames[j]+'</option>');
			}
			</script>
			</select>&nbsp;&nbsp;<%write(com_before);%></td></tr>
			</table>
			</td></tr>
			<tr><td colspan="4" height="10">&nbsp;&nbsp;</td></tr>
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
