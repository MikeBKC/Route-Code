<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
 
<% langTran("ShareAccountList"); %>
<% langTran("common"); %>
 
<title><% write(shareATitle); %></title>
 
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml"> 
 
var UserNames=new Array();
var passwords = new Array();
var authoritys = new Array();
var ftpauths = new Array();
 
/*
UserNames[0] = "admin";
passwords[0] ="admin";
authoritys[0] ="读写";
ftpauths[0]="是";
*/

var totalrecs=1;
var errorstr="";
//var loginName = "admin";
var deviceName = "";
var deviceType = "";


var TotalAccount = "5";
 

<%aspOutputNetShareUserList();%>
 

for(var i =0;i<UserNames.length;i++){
    if( authoritys[i] == "rw" ){
        authoritys[i] = "<% write(rw); %>";
    }else{
        authoritys[i] = "<% write(onlyR); %>";
    }

    if( ftpauths[i] == "on" ){
        ftpauths[i] = "<% write(yes); %>"
    }else{
        ftpauths[i] = "<% write(no); %>"
    }
}
 
var strSecondMenuFocus = "<% write(accountList); %>";
var max_totalrecs=TotalAccount;
recsPerPage[0] =5;
lineButton[0] =0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<% write(accountList); %>";
var titles1=new Array(4);
titles1[0]="<% write(account); %>";
//titles1[1]="密码";
titles1[1]="<% write(authority); %>";
titles1[2]="<% write(ftpAuth); %>";
arrayData1=new Array(UserNames,authoritys,ftpauths);
sortable[0]=new Array(1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR);
arrayCount[0]=3;
 

function dsplist(index,list)
{
	if(index==0) dspList(index,list,600,titles1,3,arrayData1,1,1);
}
/******************list***********************/
function EditConfig(val,i){
	if(val==0){ 
		User_List.id.value="";
		if(eval(max_totalrecs-totalrecs)<1)	{
			alert("<% write(accountFull); %>");
			return;
		}
	}
	else User_List.id.value=UserNames[i];
	User_List.action="ShareAccount_edit.asp";
	User_List.submit();
}
function DeleteConfig(index){
	if(confirm("<% write(sureDel); %>")){
		User_List.delstr.value=UserNames[index];
		User_List.action="/goform/formNetShareUserDel";
	    User_List.submit();
	}
}
function delAll(){
	if(confirm("<% write(sureDelAll); %>")){
		User_List.action="/goform/formNetShareUserDelAll";
		User_List.submit();
	}
}
-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="images/body_main.png">
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
		//parent.assist.document.getElementById('login_name').value=loginName;
		showSecondTitle("<% write(shareAccount); %>");
        </script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td ><form name="User_List" method="post" action="/goform/formNetShareUserDel">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center" class="style7">
   <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0"><tr><td colspan="4" height="20">&nbsp;</td></tr>


    <tr><td colspan="4"><table width="100%"><tr>
	  <td  class="helpfont" height="30" id="discription6" align="left"><strong><% write(attention); %></strong><% write(attentionInfo); %><br> </td>	  
	  </tr></table></td></tr>

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
				<input type="checkbox" name="delchk" onClick="selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'));">
		<script language="javascript">document.write("<% write(com_select); %>");</script></div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td> 
			   <td width="90" valign="top" align="right"><input type="button" value="<% write(addNew); %>" class="input-button" onClick="EditConfig(0)" id="add"></td>
			  <td width="95" valign="top" align="right"><input type="button" value="<% write(delAll); %>" class="input-button" onClick="delAll()" id="delall"></td>
			 <td width="45" valign="top" align="right"><input type="submit" value="<% write(del); %>" class="input-button" onClick="return(genData(User_List,0,frame12,UserNames,'<% write(accountInfo); %>'))" id="del"></td>
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


