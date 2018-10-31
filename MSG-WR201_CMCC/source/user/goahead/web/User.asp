<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("User"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>

<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<% aspOutUserList();%>
<%
	aspOutAssitVar();
%>
<% GetMaxAdmNum();%>



var strSecondMenuFocus = "<% write(UserMesList); %>";
var max_totalrecs=TotalAccount;
recsPerPage[0] =5;
lineButton[0] =0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(UserMesList); %>";
var titles1=new Array(1);
titles1[0]="<% write(com_user); %>";
arrayData1=new Array(UserNames);
sortable[0]=new Array(1);
sortType[0]=new Array(TYPE_STR);
arrayCount[0]=1;

<!--
function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,1,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		User_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(UserMaxMes); %>");
			return;
		}
	}
	else User_List.id.value=UserNames[i];
	User_List.action="User_edit.asp";
	User_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(MakeSureDel); %>")){
		User_List.delstr.value=UserNames[index];
		User_List.action="/goform/formUserDel";
	    User_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(MakeSureDelAll); %>")){
		User_List.action="/goform/formUserDelAll";
		User_List.submit();
	}
}
-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">
		parent.assist.document.getElementById('login_name').value=loginName;
		showSecondTitle("<% write(SecondTitle); %>");
        </script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="User_List" method="post" action="/goform/formUserDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="70%"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>
		     <tr bgcolor="c0ddff">
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11" scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
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
		<script language="javascript">document.write("<% write(Select); %>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<% write(AddNew); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<% write(DelAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(com_delete); %>" class="input-button" onClick="return(genData(User_List,0,frame12,UserNames,'<% write(UserMes); %>'))" id="del"></td>
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
